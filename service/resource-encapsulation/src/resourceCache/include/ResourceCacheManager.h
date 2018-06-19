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

/**
 * @file
 *
 * This file contains the declaration of classes and its members related to Resource Cache.
 */

#ifndef RCM_RESOURCECACHEMANAGER_H_
#define RCM_RESOURCECACHEMANAGER_H_

#include <list>
#include <string>
#include <mutex>
#include <map>

#include "CacheTypes.h"
#include "DataCache.h"
#include "ObserveCache.h"

namespace OIC
{
    namespace Service
    {

      /**
       * This is internal class for RCSRemoteResourceObject, represents resource
       * cache and provides simple ways to interact with it. Resource Cache
       * represents, one of the feature of  resource-encapsulation layer:
       * "Caching data [attributes] of remote resource".
       *
       * @see RCSRemoteResourceObject
       */
        class ResourceCacheManager
        {
            public:

                /**
                 * No cached data exception class for resource cache.
                 *
                 * @see RCSException
                 */
                class HasNoCachedDataException: public RCSException
                {
                    public:
                        HasNoCachedDataException(std::string &&what)
                            : RCSException { std::move(what) } {}
                };

                /**
                 * Gets instance of Resource cache manager.
                 *
                 * @return ResourceCacheManager instance.
                 */
                static ResourceCacheManager *getInstance();

                /**
                 * Start Caching data [attributes] for the given resource.
                 * This method will be called internally by RCSRemoteResourceObject.
                 *
                 * @param pResource Primitive resource.
                 * @param func Cache callback function.
                 * @param cm Caching method.
                 * @param rf Report frequency.
                 * @param time Caching interval.
                 *
                 * @return Cache Id.
                 *
                 * @throw InvalidParameterException In case of invalid parameters.
                 *
                 * @see PrimitiveResourcePtr
                 * @see CacheCB
                 * @see RCSRemoteResourceObject
                 */
                CacheID requestResourceCache(
                    PrimitiveResourcePtr pResource, CacheCB func = NULL,
                    CACHE_METHOD cm = CACHE_METHOD::ITERATED_GET,
                    REPORT_FREQUENCY rf = REPORT_FREQUENCY::NONE, long time = 0l);

                /**
                 * Stops caching resource data [attributes] for the given cache id.
                 * This method will be called internally by RCSRemoteResourceObject.
                 *
                 * @param id Cache Id.
                 *
                 * @throw InvalidParameterException In case of invalid Cache id.
                 *
                 * @see RCSRemoteResourceObject
                 * @see requestResourceCache
                 * @see CacheID
                 */
                void cancelResourceCache(CacheID id);

                /**
                 * Updates resource cache for the given cache id.
                 *
                 * @param id Cache Id.
                 *
                 * @throw InvalidParameterException In case of invalid Cache id.
                 *
                 * @see requestResourceCache
                 * @see CacheID
                 */
                void updateResourceCache(CacheID id) const;

                /**
                 * Gets cached resource data [attributes] for the given cache id.
                 * This method will be called internally by RCSRemoteResourceObject.
                 *
                 * @param id Cache Id.
                 *
                 * @throw InvalidParameterException In case of invalid Cache id.
                 * @throw HasNoCachedDataException In case of no cached data.
                 *
                 * @see RCSRemoteResourceObject
                 * @see RCSResourceAttributes
                 * @see requestResourceCache
                 * @see CacheID
                 */
                const RCSResourceAttributes getCachedData(CacheID id) const;

                /**
                 * Gets cache state for the given cache id.
                 * This method will be called internally by RCSRemoteResourceObject.
                 *
                 * @param id Cache Id.
                 *
                 * @throw InvalidParameterException In case of invalid Cache id.
                 *
                 * @see RCSRemoteResourceObject
                 * @see requestResourceCache
                 * @see CacheID
                 */
                CACHE_STATE getResourceCacheState(CacheID id) const;

                /**
                 * Checks whether cache data is available or not.
                 *
                 * @param id Cache Id.
                 *
                 * @return True if cached data available, otherwise false.
                 *
                 * @throw InvalidParameterException In case of invalid Cache id.
                 *
                 * @see RCSRemoteResourceObject
                 * @see requestResourceCache
                 * @see CacheID
                 */
                bool isCachedData(CacheID id) const;

                /**
                 * Stops resource cache manager.
                 *
                 */
                static void stopResourceCacheManager();

            private:
                static ResourceCacheManager *s_instance;
                static std::mutex s_mutex;
                static std::mutex s_mutexForCreation;
                static std::unique_ptr<std::list<DataCachePtr>> s_cacheDataList;
                std::map<CacheID, DataCachePtr> cacheIDmap;

                std::list<ObserveCache::Ptr> m_observeCacheList;
                std::map<CacheID, ObserveCache::Ptr> observeCacheIDmap;

                ResourceCacheManager() = default;
                ~ResourceCacheManager();
                ResourceCacheManager(const ResourceCacheManager &) = delete;
                ResourceCacheManager(ResourceCacheManager &&) = delete;

                ResourceCacheManager &operator=(const ResourceCacheManager &) const = delete;
                ResourceCacheManager &operator=(ResourceCacheManager && ) const = delete;

                static void initializeResourceCacheManager();
                DataCachePtr findDataCache(PrimitiveResourcePtr pResource) const;
                DataCachePtr findDataCache(CacheID id) const;
        };
    } // namespace Service
} // namespace OIC

#endif /* RCM_RESOURCECACHEMANAGER_H_ */
