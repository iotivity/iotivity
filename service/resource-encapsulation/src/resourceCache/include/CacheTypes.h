/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef RCM_CACHETYPES_H
#define RCM_CACHETYPES_H

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "experimental/logger.h"

#include "PrimitiveResource.h"
#include "RCSResourceAttributes.h"

/** OIC namespace */
namespace OIC
{
    /** service namespace */
    namespace Service
    {

        class DataCache;

#define CACHE_TAG  "CACHE"   /*!< cache tag information */
#define CACHE_DEFAULT_REPORT_MILLITIME 10000 /*!< default report time */
#define CACHE_DEFAULT_EXPIRED_MILLITIME 15000 /*!< default expired time */

        /** enum for report frequency value */
        enum class REPORT_FREQUENCY
        {
            NONE = 0,        /**< Initialize frequency */
            UPTODATE,        /**< up to date frequency */
            PERIODICTY       /**< periodicty frequency */
        };

        struct Report_Info
        {
            REPORT_FREQUENCY rf;
            int reportID;               /**< report id */
            long repeatTime;            /**< repeat time */
            unsigned int timerID;       /**< timer id */
        };

        /** enum for cache state */
        enum class CACHE_STATE
        {
            READY = 0,          /**< ready state */
            READY_YET,          /**< ready yet state */
            LOST_SIGNAL,        /**< lost signal state */
            DESTROYED,          /**< destroyed state */
            UPDATING,           /**< updating state */
            NONE                /**< none state */
        };

        /** enum for cache mode */
        enum class CACHE_MODE
        {
            OBSERVE = 0,         /**< observe mode */
            FREQUENCY            /**< frequency of mode */
        };

        /** enum for cache method */
        enum class CACHE_METHOD
        {
            OBSERVE_ONLY,           /**< observe only method */
            ITERATED_GET            /**< iterated get method */
        };

        typedef int CacheID;

        typedef std::function<OCStackResult(std::shared_ptr<PrimitiveResource>,
                        const RCSResourceAttributes &, int eCode)> CacheCB;
        typedef std::map<int, std::pair<Report_Info, CacheCB>> SubscriberInfo;
        typedef std::pair<int, std::pair<Report_Info, CacheCB>> SubscriberInfoPair;

        typedef OC::OCResource BaseResource;
        typedef PrimitiveResource::GetCallback GetCB;
        typedef PrimitiveResource::ObserveCallback ObserveCB;

        typedef std::shared_ptr<DataCache> DataCachePtr;
        typedef std::shared_ptr<PrimitiveResource> PrimitiveResourcePtr;
    } /* namespace Service */
} /* namespace OIC */

#endif /* RCM_CACHETYPES_H */
