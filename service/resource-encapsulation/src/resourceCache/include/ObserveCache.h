/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/**
 * @file
 *
 * This file contains the declaration of classes and its members related to Cache.
 */

#ifndef RCM_OBSERVECACHE_H_
#define RCM_OBSERVECACHE_H_

#include <list>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>

#include "CacheTypes.h"
#include "PrimitiveResource.h"
#include "ExpiryTimer.h"

namespace OIC
{
    namespace Service
    {

      /**
       * This is internal class for ResourceCacheManager, represents resource
       * cache and provides simple ways to interact with it.
       *
       * @see ResourceCacheManager
       */
        class ObserveCache : public std::enable_shared_from_this<ObserveCache>
        {
            public:
                typedef std::function<OCStackResult(std::shared_ptr<PrimitiveResource>,
                            const RCSResourceAttributes &, int)> DataCacheCB;
                typedef std::shared_ptr<ObserveCache> Ptr;

            public:
                ObserveCache(std::weak_ptr<PrimitiveResource> pResource);
                ~ObserveCache() = default;

                ObserveCache(const ObserveCache &) = delete;
                ObserveCache(ObserveCache &&) = delete;
                ObserveCache & operator = (const ObserveCache &) = delete;
                ObserveCache & operator = (ObserveCache &&) = delete;

                /**
                 * Starts Caching of data [attributes].
                 * This method will be called internally by ResourceCacheManager.
                 *
                 * @param func Data cache callback.
                 *
                 * @see ResourceCacheManager
                 * @see DataCacheCB
                 */
                void startCache(DataCacheCB func);

                /**
                 * Stops Caching of data [attributes].
                 * This method will be called internally by ResourceCacheManager.
                 *
                 * @see ResourceCacheManager
                 */
                void stopCache();

                /**
                 * Gets cache state.
                 *
                 * @return cache state.
                 *
                 * @see CACHE_STATE
                 */
                CACHE_STATE getCacheState() const;

                /**
                 * Gets cached data.
                 *
                 * @return cached resource attributes.
                 *
                 * @see CACHE_STATE
                 */
                RCSResourceAttributes getCachedData() const;

                /**
                 * Checks whether cached data is available.
                 *
                 * @return True if cached data is available, otherwise false.
                 */
                bool isCachedData() const;

                /**
                 * Checks whether caching started or not.
                 *
                 * @return True if caching started, otherwise false.
                 */
                bool isStartCache() const;

            private:
                typedef unsigned int TimerID;
                typedef std::weak_ptr<PrimitiveResource> weakPrimitiveResource;
                typedef std::weak_ptr<ObserveCache> weakDataCache;

                // resource instance
                weakPrimitiveResource m_wpResource;

                // cached data info
                RCSResourceAttributes m_attributes;
                CACHE_STATE m_state;

                DataCacheCB m_reportCB;

                std::atomic<bool> m_isStart;

                CacheID m_id;

            private:
                static void verifyObserveCB(const HeaderOptions &_hos,
                                            const ResponseStatement &_rep, int _result,
                                            unsigned int _seq, weakDataCache ptr);
                void onObserve(const HeaderOptions &_hos,
                               const ResponseStatement &_rep, int _result, unsigned int _seq);
                bool convertOCResultToSuccess(OCStackResult ret);
        };
    } // namespace Service
} // namespace OIC

#endif /* RCM_DATACACHE_H_ */
