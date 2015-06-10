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

#include <memory>
#include <cstdlib>
#include <functional>
#include <map>
#include <utility>
#include <ctime>

#include "OCApi.h"

#include "DataCache.h"

#include "ResponseStatement.h"
#include "ResourceAttributes.h"

DataCache::DataCache(
            PrimitiveResourcePtr pResource,
            CacheCB func,
            REPORT_FREQUENCY rf,
            long repeatTime
            ):sResource(pResource)
{
    subscriberList = std::unique_ptr<SubscriberInfo>(new SubscriberInfo());
    data = std::make_shared<CachedData>();

    state = CACHE_STATE::READY_YET;
    updateTime = 0l;

    pObserveCB = (ObserveCB)(std::bind(&DataCache::onObserve, this,
            std::placeholders::_1, std::placeholders::_2,
            std::placeholders::_3, std::placeholders::_4));
    pGetCB = (GetCB)(std::bind(&DataCache::onGet, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    if(pResource->isObservable())
    {
        pResource->requestObserve(pObserveCB);
    }
    else
    {
        // TODO set timer
    }
}

DataCache::~DataCache()
{
    // TODO Auto-generated destructor stub
    data.reset();

    // TODO delete all node!!
    subscriberList->clear();
    subscriberList.release();
}

CacheID DataCache::addSubscriber(CacheCB func, REPORT_FREQUENCY rf, long repeatTime)
{
    Report_Info newItem;
    newItem.rf = rf;
    newItem.latestReportTime = 0l;
    newItem.repeatTime = repeatTime;

    srand(time(NULL));
    newItem.reportID = rand();

    while(1)
    {
        if(findSubscriber(newItem.reportID).first == 0 || newItem.reportID == 0)
        {
            newItem.reportID = rand();
        }
        else
        {
            break;
        }
    }

    subscriberList->insert(std::make_pair(newItem.reportID, std::make_pair(newItem, func)));

    return newItem.reportID;
}

CacheID DataCache::deleteSubscriber(CacheID id)
{
    CacheID ret = 0;

    SubscriberInfoPair pair = findSubscriber(id);
    if(pair.first != 0)
    {
        ret = pair.first;
        subscriberList->erase(pair.first);
    }

    return ret;
}

SubscriberInfoPair DataCache::findSubscriber(CacheID id)
{
    SubscriberInfoPair ret;

    for(auto & i : *subscriberList)
    {
        if(i.first == id)
        {
            ret = std::make_pair(i.first, std::make_pair((Report_Info)i.second.first, (CacheCB)i.second.second));
        }
    }

    return ret;
}

CachedDataPtr DataCache::getCachedData()
{
    if(state != CACHE_STATE::READY)
    {
        return nullptr;
    }
    return data;
}

std::shared_ptr<PrimitiveResource> DataCache::getPrimitiveResource()
{
    return sResource;
}

void DataCache::onObserve(
        const HeaderOptions& _hos, const ResponseStatement& _rep, int _result, int _seq)
{

    if(_result != OC_STACK_OK)
    {
        // TODO handle error
        return;
    }

    if(_rep.getAttributes().empty())
    {
        return;
    }

    ResourceAttributes att = _rep.getAttributes();

    // set data
    data->clear();


    for(auto & i : att)
    {
        const std::string &key = i.key();
//        std::string value = i.value();
        std::string val;
        data->insert(CachedData::value_type(key, val));
    }

    // notify!!

    for(auto & i : * subscriberList)
    {
        if(i.second.first.rf == REPORT_FREQUENCY::UPTODATE)
        {
            i.second.second(this->sResource, data);
        }
    }
}

void DataCache::onGet(const HeaderOptions& _hos,
        const ResponseStatement& _rep, int _result)
{

}

CACHE_STATE DataCache::getCacheState()
{
    return state;
}
