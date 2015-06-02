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

#include "ResourceCache.h"

ResourceCache * ResourceCache::s_instance = NULL;
std::mutex ResourceCache::s_mutexForCreation;
std::list< CacheHandler * > * ResourceCache::s_cacheDataList = NULL;

ResourceCache::ResourceCache()
{
    // TODO Auto-generated constructor stub
    if(!s_cacheDataList)
    {
        s_cacheDataList = new std::list< CacheHandler * >();
    }
}

ResourceCache::~ResourceCache()
{
    // TODO Auto-generated destructor stub
    if(s_cacheDataList)
    {
        s_cacheDataList->clear();
        delete s_cacheDataList;
    }
}


ResourceCache * ResourceCache::getInstance()
{
    if(!s_instance)
    {
        s_mutexForCreation.lock();
        if(!s_instance)
        {
            s_instance = new ResourceCache();
        }
        s_mutexForCreation.unlock();
    }
    return s_instance;
}

CacheID ResourceCache::requestResourceCache(
        ServiceResource & pResource, CacheCB func, REPORT_FREQUENCY rf, long reportTime)
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

    CacheHandler * newHandler = findCacheHandler(pResource);
    if(newHandler == nullptr)
    {
        CacheHandler * newHandler = new CacheHandler(pResource, func, rf, reportTime);
        s_cacheDataList->push_back(newHandler);
    }
    ret = newHandler->addSubscriber(func, rf, reportTime);

    return ret;
}

OCStackResult ResourceCache::cancelResourceCache(ServiceResource & pResource, CacheID id)
{
    OCStackResult ret = OC_STACK_ERROR;

    if(pResource == NULL || id == 0)
    {
        return ret;
    }

    // TODO cancel cache
    CacheID retID = 0;
    CacheHandler * deleteCacheHandler = findCacheHandler(pResource);
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

CacheHandler * ResourceCache::findCacheHandler(ServiceResource & pResource)
{
    CacheHandler * retHandler = nullptr;
    for (auto i : s_cacheDataList)
    {
        if(i->getServiceResource() == pResource)
        {
            retHandler = i;
            break;
        }
    }
    return retHandler;
}

OCStackResult ResourceCache::updateResourceCache(ServiceResource & pResource)
{
    OCStackResult ret = OC_STACK_ERROR;

    // TODO update now (request get)

    return ret;
}
