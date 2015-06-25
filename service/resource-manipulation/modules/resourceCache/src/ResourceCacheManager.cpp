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

#include "ResourceCacheManager.h"

ResourceCacheManager * ResourceCacheManager::s_instance = NULL;
std::mutex ResourceCacheManager::s_mutexForCreation;
std::unique_ptr<std::list<DataCachePtr>> ResourceCacheManager::s_cacheDataList(nullptr);

ResourceCacheManager::ResourceCacheManager()
{
    // TODO Auto-generated constructor stub
    if(!s_cacheDataList)
    {
        s_cacheDataList = std::unique_ptr<std::list<DataCachePtr>>(new std::list<DataCachePtr>);
    }
}

ResourceCacheManager::~ResourceCacheManager()
{
    // TODO Auto-generated destructor stub
    if(s_cacheDataList)
    {
        s_cacheDataList->clear();
    }
}

ResourceCacheManager * ResourceCacheManager::getInstance()
{
    if(s_instance == nullptr)
    {
        s_mutexForCreation.lock();
        if(s_instance == nullptr)
        {
            s_instance = new ResourceCacheManager();
        }
        s_mutexForCreation.unlock();
    }
    return s_instance;
}

CacheID ResourceCacheManager::requestResourceCache(
        PrimitiveResourcePtr pResource, CacheCB func,
        REPORT_FREQUENCY rf, long reportTime)
{
    CacheID retID = 0;

    if(rf != REPORT_FREQUENCY::NONE)
    {
        if(func == NULL)
        {
            return retID;
        }
        if(!reportTime)
        {
            // default setting
            reportTime = DEFAULT_REPORT_TIME;
        }
    }

    DataCachePtr newHandler = findDataCache(pResource);
    if(newHandler == nullptr)
    {
        newHandler = std::make_shared<DataCache>(pResource, func, rf, reportTime);
        s_cacheDataList->push_back(newHandler);
    }
    retID = newHandler->addSubscriber(func, rf, reportTime);

    return retID;
}

CacheID ResourceCacheManager::cancelResourceCache(CacheID id)
{
    CacheID retID = 0;
    if(id == 0)
    {
        return retID;
    }

    DataCachePtr foundCacheHandler = findDataCache(id);
    if(foundCacheHandler == nullptr)
    {
        return retID;
    }
    else
    {
        retID = foundCacheHandler->deleteSubscriber(id);
    }

    return retID;
}

DataCachePtr ResourceCacheManager::findDataCache(PrimitiveResourcePtr pResource) const
{
    DataCachePtr retHandler = nullptr;
    for (auto & i : * s_cacheDataList)
    {
        if(i->getPrimitiveResource()->getUri() == pResource->getUri() &&
                i->getPrimitiveResource()->getHost() == pResource->getHost())
        {
            retHandler = i;
            break;
        }
    }
    return retHandler;
}

DataCachePtr ResourceCacheManager::findDataCache(CacheID id) const
{
    DataCachePtr retHandler = nullptr;
    for (auto & i : * s_cacheDataList)
    {
        SubscriberInfoPair pair = i->findSubscriber(id);
        if(pair.first != 0)
        {
            retHandler = i;
            break;
        }
    }
    return retHandler;
}

OCStackResult ResourceCacheManager::updateResourceCache(PrimitiveResourcePtr pResource)
{
    OCStackResult ret = OC_STACK_ERROR;

    // TODO update now (request get)

    return ret;
}

const ResourceAttributes ResourceCacheManager::getCachedData(PrimitiveResourcePtr pResource) const
{
    DataCachePtr handler = findDataCache(pResource);
    if(handler == nullptr)
    {
        return ResourceAttributes();
    }
    return handler->getCachedData();
}

const ResourceAttributes ResourceCacheManager::getCachedData(CacheID id) const
{
    DataCachePtr handler = findDataCache(id);
    if(handler == nullptr)
    {
        return ResourceAttributes();
    }
    return handler->getCachedData();
}

CACHE_STATE ResourceCacheManager::getResourceCacheState(PrimitiveResourcePtr pResource) const
{
    DataCachePtr handler = findDataCache(pResource);
    if(handler == nullptr)
    {
        return CACHE_STATE::NONE;
    }
    return handler->getCacheState();
}
CACHE_STATE ResourceCacheManager::getResourceCacheState(CacheID id) const
{
    DataCachePtr handler = findDataCache(id);
    if(handler == nullptr)
    {
        return CACHE_STATE::NONE;
    }
    return handler->getCacheState();
}
