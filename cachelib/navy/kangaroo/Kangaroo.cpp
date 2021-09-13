#include <chrono>
#include <mutex>
#include <shared_mutex>

#include <folly/Format.h>

#include "cachelib/navy/kangaroo/Kangaroo.h"
#include "cachelib/navy/kangaroo/KangarooLog.h"
#include "cachelib/navy/kangaroo/RripBucket.h"
#include "cachelib/navy/common/Utils.h"
#include "cachelib/navy/serialization/Serialization.h"

namespace facebook {
namespace cachelib {
namespace navy {
namespace {
constexpr uint64_t kMinSizeDistribution = 64;
constexpr uint64_t kMinThresholdSizeDistribution = 8;
constexpr double kSizeDistributionGranularityFactor = 1.25;
} // namespace

constexpr uint32_t Kangaroo::kFormatVersion;

Kangaroo::Config& Kangaroo::Config::validate() {
  if (!folly::isPowTwo(bucketSize)) {
    throw std::invalid_argument(
        folly::sformat("invalid bucket size: {}", bucketSize));
  }

  if (cacheBaseOffset % bucketSize != 0 || totalSetSize % bucketSize != 0) {
    throw std::invalid_argument(folly::sformat(
        "cacheBaseOffset and totalSetSize need to be a multiple of bucketSize. "
        "cacheBaseOffset: {}, totalSetSize:{}, bucketSize: {}.",
        cacheBaseOffset,
        totalSetSize,
        bucketSize));
  }

  if (device == nullptr) {
    throw std::invalid_argument("device cannot be null");
  }

  if (bloomFilter && bloomFilter->numFilters() != numBuckets()) {
    throw std::invalid_argument(
        folly::sformat("bloom filter #filters mismatch #buckets: {} vs {}",
                       bloomFilter->numFilters(),
                       numBuckets()));
  }

  if (logConfig.logSize > 0 && avgSmallObjectSize == 0) {
    throw std::invalid_argument(
        folly::sformat("Need an avgSmallObjectSize for the log"));
  }
  return *this;
}

Kangaroo::Kangaroo(Config&& config)
    : Kangaroo{std::move(config.validate()), ValidConfigTag{}} {}

Kangaroo::Kangaroo(Config&& config, ValidConfigTag)
    : destructorCb_{[this, cb = std::move(config.destructorCb)](
                        BufferView key,
                        BufferView value,
                        DestructorEvent event) {
        sizeDist_.removeSize(key.size() + value.size());
        if (cb) {
          cb(key, value, event);
        }
      }},
      bucketSize_{config.bucketSize},
      cacheBaseOffset_{config.cacheBaseOffset},
      numBuckets_{config.numBuckets()},
      device_{*config.device},
      sizeDist_{kMinSizeDistribution, bucketSize_,
                kSizeDistributionGranularityFactor},
      thresholdSizeDist_{10, bucketSize_, 10},
      thresholdNumDist_{1, 25, 1} {
  XLOGF(INFO,
        "Kangaroo created: buckets: {}, bucket size: {}, base offset: {}",
        numBuckets_,
        bucketSize_,
        cacheBaseOffset_);
  if (config.logConfig.logSize) {
    config.logConfig.logIndexPartitions = config.logIndexPartitionsPerPhysical * config.logConfig.logPhysicalPartitions;
    uint64_t bytesPerIndex = config.logConfig.logSize / config.logConfig.logIndexPartitions;
    config.logConfig.device = config.device;
    log_ = std::make_unique<KangarooLog>(std::move(config.logConfig));
  }
  reset();
}

void Kangaroo::reset() {
  XLOG(INFO, "Reset Kangaroo");
  generationTime_ = getSteadyClock();

  itemCount_.set(0);
  insertCount_.set(0);
  succInsertCount_.set(0);
  lookupCount_.set(0);
  succLookupCount_.set(0);
  removeCount_.set(0);
  succRemoveCount_.set(0);
  evictionCount_.set(0);
  logicalWrittenCount_.set(0);
  physicalWrittenCount_.set(0);
  ioErrorCount_.set(0);
  bfFalsePositiveCount_.set(0);
  bfProbeCount_.set(0);
  checksumErrorCount_.set(0);
  sizeDist_.reset();
}

uint64_t Kangaroo::getMaxItemSize() const {
  // does not include per item overhead
  return bucketSize_ - sizeof(RripBucket);
}

void Kangaroo::getCounters(const CounterVisitor& visitor) const {
  visitor("navy_bh_items", itemCount_.get());
  visitor("navy_bh_inserts", insertCount_.get());
  visitor("navy_bh_succ_inserts", succInsertCount_.get());
  visitor("navy_bh_lookups", lookupCount_.get());
  visitor("navy_bh_succ_lookups", succLookupCount_.get());
  visitor("navy_bh_removes", removeCount_.get());
  visitor("navy_bh_succ_removes", succRemoveCount_.get());
  visitor("navy_bh_evictions", evictionCount_.get());
  visitor("navy_bh_logical_written", logicalWrittenCount_.get());
  uint64_t logBytesWritten = (log_) ? log_->getBytesWritten() : 0;
  visitor("navy_bh_physical_written", physicalWrittenCount_.get() + logBytesWritten);
  visitor("navy_bh_io_errors", ioErrorCount_.get());
  visitor("navy_bh_checksum_errors", checksumErrorCount_.get());
  if (log_) {
    visitor("navy_klog_false_positive_pct", log_->falsePositivePct());
    visitor("navy_klog_fragmentation_pct", log_->fragmentationPct());
    visitor("navy_klog_extra_reads_pct", log_->extraReadsPct());
  }
  auto snapshot = sizeDist_.getSnapshot();
  for (auto& kv : snapshot) {
    auto statName = folly::sformat("navy_bh_approx_bytes_in_size_{}", kv.first);
    visitor(statName.c_str(), kv.second);
  }
}

void Kangaroo::persist(RecordWriter& rw) {
  XLOG(INFO, "Starting kangaroo persist");
  serialization::BigHashPersistentData pd;
  pd.version = kFormatVersion;
  pd.generationTime = generationTime_.count();
  pd.itemCount = itemCount_.get();
  pd.bucketSize = bucketSize_;
  pd.cacheBaseOffset = cacheBaseOffset_;
  pd.numBuckets = numBuckets_;
  *pd.sizeDist_ref() = sizeDist_.getSnapshot();
  serializeProto(pd, rw);

  XLOG(INFO, "Finished kangaroo persist");
}

bool Kangaroo::recover(RecordReader& rr) {
  XLOG(INFO, "Starting kangaroo recovery");
  try {
    auto pd = deserializeProto<serialization::BigHashPersistentData>(rr);
    if (pd.version != kFormatVersion) {
      throw std::logic_error{
          folly::sformat("invalid format version {}, expected {}",
                         pd.version,
                         kFormatVersion)};
    }

    auto configEquals =
        static_cast<uint64_t>(pd.bucketSize) == bucketSize_ &&
        static_cast<uint64_t>(pd.cacheBaseOffset) == cacheBaseOffset_ &&
        static_cast<uint64_t>(pd.numBuckets) == numBuckets_;
    if (!configEquals) {
      auto configStr = serializeToJson(pd);
      XLOGF(ERR, "Recovery config: {}", configStr.c_str());
      throw std::logic_error{"config mismatch"};
    }

    generationTime_ = std::chrono::nanoseconds{pd.generationTime};
    itemCount_.set(pd.itemCount);
  } catch (const std::exception& e) {
    XLOGF(ERR, "Exception: {}", e.what());
    XLOG(ERR, "Failed to recover kangaroo. Resetting cache.");

    reset();
    return false;
  }
  XLOG(INFO, "Finished kangaroo recovery");
  return true;
}

Status Kangaroo::insert(HashedKey hk,
                       BufferView value) {
  insertCount_.inc();

  Status ret = log_->insert(hk, value);
  if (ret == Status::Ok) {
    sizeDist_.addSize(hk.key().size() + value.size());
    succInsertCount_.inc();
  }
  logicalWrittenCount_.add(hk.key().size() + value.size());
  logInsertCount_.inc();
  itemCount_.inc();
  logItemCount_.inc();
  return ret;
}

Status Kangaroo::lookup(HashedKey hk, Buffer& value) {
  lookupCount_.inc();

  Status ret = log_->lookup(hk, value);
  if (ret == Status::Ok) {
    succLookupCount_.inc();
    logHits_.inc();
    return ret;
  }
}

Status Kangaroo::remove(HashedKey hk) {
  removeCount_.inc();
  Status ret = log_->remove(hk);
  if (ret == Status::Ok) {
    succRemoveCount_.inc();
    itemCount_.dec();
    logItemCount_.dec();
    return ret;
  }
}

bool Kangaroo::couldExist(HashedKey hk) {
  bool canExist = log_->couldExist(hk);

  // the caller is not likely to issue a subsequent lookup when we return
  // false. hence tag this as a lookup. If we return the key can exist, the
  // caller will perform a lookupAsync and will be counted within lookup api.
  if (!canExist) {
    lookupCount_.inc();
  }
  return canExist;
}


void Kangaroo::flush() {
  XLOG(INFO, "Flush big hash");
  device_.flush();
}

} // namespace navy
} // namespace cachelib
} // namespace facebook
