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

#ifndef RESOURCECACHEMANAGER_H_
#define RESOURCECACHEMANAGER_H_

#include <list>
#include <string>
#include <mutex>

#include "OCPlatform.h"
#include "CacheTypes.h"
#include "DataCache.h"

#define CACHE_TAG PCF("CACHE")

class ResourceCacheManager
{
public:
    static ResourceCacheManager * getInstance();

    CacheID requestResourceCache(
            PrimitiveResourcePtr pResource,
                CacheCB func = NULL, REPORT_FREQUENCY rf = REPORT_FREQUENCY::NONE, long time = 0l);
    OCStackResult cancelResourceCache(PrimitiveResourcePtr pResource, CacheID id);
    OCStackResult updateResourceCache(PrimitiveResourcePtr pResource);

    const ResourceAttributes getCachedData(PrimitiveResourcePtr pResource) const;
    const ResourceAttributes getCachedData(CacheID id) const;

    CACHE_STATE getResourceCacheState(PrimitiveResourcePtr pResource) const;
    CACHE_STATE getResourceCacheState(CacheID id) const;

    ~ResourceCacheManager();
private:
    ResourceCacheManager();

    static ResourceCacheManager * s_instance;
    static std::mutex s_mutexForCreation;
    static std::unique_ptr<std::list<DataCachePtr>> s_cacheDataList;

    DataCachePtr findDataCache(PrimitiveResourcePtr pResource) const;
    DataCachePtr findDataCache(CacheID id) const;
};

#endif /* RESOURCECACHEMANAGER_H_ */
