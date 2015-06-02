//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef RESOURCECACHE_H_
#define RESOURCECACHE_H_

#include <list>
#include <string>

#include "../../../../resource/csdk/stack/include/octypes.h"
#include "CacheHandler.h"
#include "CacheTypes.h"

#define CACHE_TAG PCF("CACHE")

class ResourceCache
{
public:
    ResourceCache * getInstance();

    CacheID requestResourceCache(
            ServiceResource & pResource,
                CacheCB func = NULL, REPORT_FREQUENCY rf = REPORT_FREQUENCY::NONE, long time = 0l);

//    OCStackResult cancelResourceCache(std::string address, std::string uri);
    OCStackResult cancelResourceCache(ServiceResource & pResource, CacheID id);

//    OCStackResult updateResourceCache(std::string address, std::string uri);
    OCStackResult updateResourceCache(ServiceResource & pResource);

//    OCStackResult getResourceCache(std::string address, std::string uri);
    OCStackResult getResourceCache(ServiceResource & pResource);

private:
    ResourceCache();
    ~ResourceCache();

    static ResourceCache * s_instance;
    static std::mutex s_mutexForCreation;
    static std::list< CacheHandler * > * s_cacheDataList;

    CacheHandler * findCacheHandler(ServiceResource & pResource);
};

#endif /* RESOURCECACHE_H_ */
