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

#ifndef DATACACHE_H_
#define DATACACHE_H_

#include <list>
#include <memory>
#include <string>
#include <boost/progress.hpp>

#include "OCResource.h"
#include "logger.h"

#include "CacheTypes.h"

class DataCache
{
public:
    DataCache(
            PrimitiveResource & pResource,
            CacheCB func,
            REPORT_FREQUENCY rf,
            long repeatTime);
    ~DataCache();

    CacheID addSubscriber(CacheCB func, REPORT_FREQUENCY rf, long repeatTime);
    CacheID deleteSubscriber(CacheID id);

    std::shared_ptr<CacheData> getCachedData();
    PrimitiveResource * getPrimitiveResource();

private:
    // origin resource info
    std::string uri;
    std::string address;

    // resource instance
    PrimitiveResource *sResource;
    std::shared_ptr<BaseResource> baseHandler;

    std::shared_ptr<ResourceAttributes> attributes;

    // cached data info
    std::shared_ptr<CacheData> data;
    long updateTime;
    CACHE_STATE state;

    // subscriber info
    SubscriberInfo *subscriberList;
    SubscriberInfoPair findSubscriber(CacheID id);

    // for requestCB from base
    void onObserve(const HeaderOptions& _hos,
            const ResponseStatement& _rep, int _result, int _seq);
    void onGet(const HeaderOptions& _hos, const ResponseStatement& _rep, int _result);

    ObserveCB pObserveCB;
    GetCB pGetCB;

    //
    OCStackResult updateCacheData();

    // for timer
    void setUpdateTime();
    void onTimer();
    bool isAvailable();

};

#endif /* DATACACHE_H_ */
