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
std::list< DataCache * > * ResourceCacheManager::s_cacheDataList = NULL;

ResourceCacheManager::ResourceCacheManager()
{
    // TODO Auto-generated constructor stub
    if(!s_cacheDataList)
    {
        s_cacheDataList = new std::list< DataCache * >();
    }
}

ResourceCacheManager::~ResourceCacheManager()
{
    // TODO Auto-generated destructor stub
    if(s_cacheDataList)
    {
        s_cacheDataList->clear();
        delete s_cacheDataList;
    }
}


ResourceCacheManager * ResourceCacheManager::getInstance()
{
    if(!s_instance)
    {
        s_mutexForCreation.lock();
        if(!s_instance)
        {
            s_instance = new ResourceCacheManager();
        }
        s_mutexForCreation.unlock();
    }
    return s_instance;
}

CacheID ResourceCacheManager::requestResourceCache(
        PrimitiveResource & pResource, CacheCB func, REPORT_FREQUENCY rf, long reportTime)
{
    CacheID ret = 0;

    if(rt == NULL || pResource == NULL)
    {
        return ret;
    }

    if(rt != REPORT_FREQUENCY::NONE)
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

    DataCache * newHandler = findCacheHandler(pResource);
    if(newHandler == nullptr)
    {
        DataCache * newHandler = new DataCache(pResource, func, rf, reportTime);
        s_cacheDataList->push_back(newHandler);
    }
    ret = newHandler->addSubscriber(func, rf, reportTime);

    return ret;
}

OCStackResult ResourceCacheManager::cancelResourceCache(PrimitiveResource & pResource, CacheID id)
{
    OCStackResult ret = OC_STACK_ERROR;

    if(pResource == NULL || id == 0)
    {
        return ret;
    }

    // TODO cancel cache
    CacheID retID = 0;
    DataCache * deleteCacheHandler = findCacheHandler(pResource);
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

DataCache * ResourceCacheManager::findCacheHandler(PrimitiveResource & pResource)
{
    DataCache * retHandler = nullptr;
    for (auto i : s_cacheDataList)
    {
        if(i->getPrimitiveResource() == pResource)
        {
            retHandler = i;
            break;
        }
    }
    return retHandler;
}

OCStackResult ResourceCacheManager::updateResourceCache(PrimitiveResource & pResource)
{
    OCStackResult ret = OC_STACK_ERROR;

    // TODO update now (request get)

    return ret;
}
