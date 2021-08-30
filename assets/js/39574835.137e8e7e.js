"use strict";(self.webpackChunkmy_website=self.webpackChunkmy_website||[]).push([[2361],{3905:function(e,t,r){r.d(t,{Zo:function(){return u},kt:function(){return p}});var a=r(7294);function n(e,t,r){return t in e?Object.defineProperty(e,t,{value:r,enumerable:!0,configurable:!0,writable:!0}):e[t]=r,e}function i(e,t){var r=Object.keys(e);if(Object.getOwnPropertySymbols){var a=Object.getOwnPropertySymbols(e);t&&(a=a.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),r.push.apply(r,a)}return r}function c(e){for(var t=1;t<arguments.length;t++){var r=null!=arguments[t]?arguments[t]:{};t%2?i(Object(r),!0).forEach((function(t){n(e,t,r[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(r)):i(Object(r)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(r,t))}))}return e}function o(e,t){if(null==e)return{};var r,a,n=function(e,t){if(null==e)return{};var r,a,n={},i=Object.keys(e);for(a=0;a<i.length;a++)r=i[a],t.indexOf(r)>=0||(n[r]=e[r]);return n}(e,t);if(Object.getOwnPropertySymbols){var i=Object.getOwnPropertySymbols(e);for(a=0;a<i.length;a++)r=i[a],t.indexOf(r)>=0||Object.prototype.propertyIsEnumerable.call(e,r)&&(n[r]=e[r])}return n}var l=a.createContext({}),s=function(e){var t=a.useContext(l),r=t;return e&&(r="function"==typeof e?e(t):c(c({},t),e)),r},u=function(e){var t=s(e.components);return a.createElement(l.Provider,{value:t},e.children)},h={inlineCode:"code",wrapper:function(e){var t=e.children;return a.createElement(a.Fragment,{},t)}},d=a.forwardRef((function(e,t){var r=e.components,n=e.mdxType,i=e.originalType,l=e.parentName,u=o(e,["components","mdxType","originalType","parentName"]),d=s(r),p=n,f=d["".concat(l,".").concat(p)]||d[p]||h[p]||i;return r?a.createElement(f,c(c({ref:t},u),{},{components:r})):a.createElement(f,c({ref:t},u))}));function p(e,t){var r=arguments,n=t&&t.mdxType;if("string"==typeof e||n){var i=r.length,c=new Array(i);c[0]=d;var o={};for(var l in t)hasOwnProperty.call(t,l)&&(o[l]=t[l]);o.originalType=e,o.mdxType="string"==typeof e?e:n,c[1]=o;for(var s=2;s<i;s++)c[s]=r[s];return a.createElement.apply(null,c)}return a.createElement.apply(null,r)}d.displayName="MDXCreateElement"},606:function(e,t,r){r.r(t),r.d(t,{frontMatter:function(){return o},contentTitle:function(){return l},metadata:function(){return s},toc:function(){return u},default:function(){return d}});var a=r(7462),n=r(3366),i=(r(7294),r(3905)),c=["components"],o={id:"cache_library_intro",title:"Cache Library User Guides",slug:"/"},l=void 0,s={unversionedId:"cache_library_intro",id:"cache_library_intro",isDocsHomePage:!1,title:"Cache Library User Guides",description:"Cache Library User Guides",source:"@site/docs/cache_library_intro.md",sourceDirName:".",slug:"/",permalink:"/docs/",editUrl:"https://github.com/facebook/docusaurus/edit/master/website/docs/cache_library_intro.md",version:"current",frontMatter:{id:"cache_library_intro",title:"Cache Library User Guides",slug:"/"},sidebar:"someSidebar",next:{title:"Github Repository Synchronization (after cleanup)",permalink:"/docs/installation/github-squash-sync"}},u=[{value:"Cache Library User Guides",id:"cache-library-user-guides",children:[]},{value:"Who should be using cachelib?",id:"who-should-be-using-cachelib",children:[]},{value:"Getting started with CacheLib",id:"getting-started-with-cachelib",children:[]}],h={toc:u};function d(e){var t=e.components,r=(0,n.Z)(e,c);return(0,i.kt)("wrapper",(0,a.Z)({},h,r,{components:t,mdxType:"MDXLayout"}),(0,i.kt)("h2",{id:"cache-library-user-guides"},"Cache Library User Guides"),(0,i.kt)("p",null,"Cache Library (cachelib) is a C++ library for accessing and managing cache data. It is a\nthread-safe API that enables developers to build and customize scalable, concurrent caches."),(0,i.kt)("h2",{id:"who-should-be-using-cachelib"},"Who should be using cachelib?"),(0,i.kt)("p",null,"CacheLib is targeted at services that use gigabytes of memory to cache information. It provides an API similar to ",(0,i.kt)("inlineCode",{parentName:"p"},"malloc")," and a concurrent unordered hash table for caching. Consider using cache and cachelib for your service if any of the following applies to the service:"),(0,i.kt)("ul",null,(0,i.kt)("li",{parentName:"ul"},"Caching variable sized objects."),(0,i.kt)("li",{parentName:"ul"},"Concurrent access with zero copy semantics."),(0,i.kt)("li",{parentName:"ul"},"Hard RSS memory constraints for cache to avoid OOMs."),(0,i.kt)("li",{parentName:"ul"},"Variety of caching algorithms like LRU, Segmented LRU, FIFO, 2Q, and TTL."),(0,i.kt)("li",{parentName:"ul"},"Intelligent and automatic tuning of cache for dynamic changes to workload."),(0,i.kt)("li",{parentName:"ul"},"Shared-memory based persistence of cache across process restarts."),(0,i.kt)("li",{parentName:"ul"},"Transparent usage of DRAM and SSD for caching.")),(0,i.kt)("h2",{id:"getting-started-with-cachelib"},"Getting started with CacheLib"),(0,i.kt)("p",null,"To get started with cachelib, read these guides:"),(0,i.kt)("ul",null,(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("a",{parentName:"li",href:"Cache_Library_User_Guides/Set_up_a_simple_cache/"},"Set up a simple cache")),(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("a",{parentName:"li",href:"Cache_Library_User_Guides/Write_data_to_cache/"},"Write data to the cache"),"."),(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("a",{parentName:"li",href:"Cache_Library_User_Guides/Read_data_from_cache/"},"Read data from the cache"),"."),(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("a",{parentName:"li",href:"Cache_Library_User_Guides/Remove_data_from_cache/"},"Remove data from the cache"),"."),(0,i.kt)("li",{parentName:"ul"},(0,i.kt)("a",{parentName:"li",href:"Cache_Library_User_Guides/Visit_data_in_cache/"},"Visit data in the cache"),".")),(0,i.kt)("p",null,"Are you looking for a guide on how CacheLib is designed and how to add new features to it? Please refer to our architecture guide."))}d.isMDXComponent=!0}}]);