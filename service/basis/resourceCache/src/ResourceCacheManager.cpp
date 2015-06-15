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
            s_instance = new ResourceCacheManager();;
        }
        s_mutexForCreation.unlock();
    }
    return s_instance;
}

CacheID ResourceCacheManager::requestResourceCache(
        PrimitiveResourcePtr pResource, CacheCB func,
        REPORT_FREQUENCY rf, long reportTime)
{
    CacheID ret = 0;

    if(rf != REPORT_FREQUENCY::NONE)
    {
        if(func == NULL)
        {
            return ret;
        }
        if(!reportTime)
        {
            // default setting
            reportTime = 30;
        }
    }

    DataCachePtr newHandler = findDataCache(pResource);
    if(newHandler == nullptr)
    {
        newHandler = std::make_shared<DataCache>(pResource, func, rf, reportTime);
        s_cacheDataList->push_back(newHandler);
    }
    ret = newHandler->addSubscriber(func, rf, reportTime);

    return ret;
}

OCStackResult ResourceCacheManager::cancelResourceCache(PrimitiveResourcePtr pResource, CacheID id)
{
    OCStackResult ret = OC_STACK_ERROR;

    if(id == 0)
    {
        return ret;
    }

    // TODO cancel cache
    CacheID retID = 0;
    DataCachePtr deleteCacheHandler = findDataCache(pResource);
    if(deleteCacheHandler == nullptr)
    {
        return ret;
    }
    else
    {
        retID = deleteCacheHandler->deleteSubscriber(id);
    }

    if(retID == id)
    {
        ret = OC_STACK_OK;
    }

    return ret;
}

DataCachePtr ResourceCacheManager::findDataCache(PrimitiveResourcePtr pResource)
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

DataCachePtr ResourceCacheManager::findDataCache(CacheID id)
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

CachedDataPtr ResourceCacheManager::getCachedData(PrimitiveResourcePtr pResource)
{
    DataCachePtr handler = findDataCache(pResource);
    if(handler == nullptr)
    {
        return nullptr;
    }
    return handler->getCachedData();
}

CachedDataPtr ResourceCacheManager::getCachedData(CacheID id)
{
    DataCachePtr handler = findDataCache(id);
    if(handler == nullptr)
    {
        return nullptr;
    }
    return handler->getCachedData();
}

CACHE_STATE ResourceCacheManager::getResourceCacheState(PrimitiveResourcePtr pResource)
{
    DataCachePtr handler = findDataCache(pResource);
    if(handler == nullptr)
    {
        return CACHE_STATE::NONE;
    }
    return handler->getCacheState();
}
CACHE_STATE ResourceCacheManager::getResourceCacheState(CacheID id)
{
    DataCachePtr handler = findDataCache(id);
    if(handler == nullptr)
    {
        return CACHE_STATE::NONE;
    }
    return handler->getCacheState();
}
