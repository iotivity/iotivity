//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "REHelper.h"

REHelper* REHelper::s_reHelperInstance = NULL;
std::mutex REHelper::s_mutex;

REHelper::REHelper()
{
    PlatformConfig config
    { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos };
    OCPlatform::Configure(config);
    IOTIVITYTEST_LOG(INFO, "Platform Configuration Done!!");
    m_pDiscoveryManager = RCSDiscoveryManager::getInstance();
    discoveryTask = nullptr;

}

REHelper::~REHelper()
{
}

REHelper* REHelper::getInstance(void)
{
    if (s_reHelperInstance == NULL)
    {
        s_mutex.lock();

        if (s_reHelperInstance == NULL)
        {
            s_reHelperInstance = new REHelper();
        }

        s_mutex.unlock();
    }

    return s_reHelperInstance;
}

std::string REHelper::getFailureMessage()
{
    return m_errorMessage;
}

bool REHelper::findPrimitiveResources(string resourceType)
{
    bool isSuccess = false;
    m_primitiveResourceList.clear();

    RCSAddress host = RCSAddress::multicast();
    string uri = "";

    try
    {
    	discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(RCSAddress::multicast(),
        		OC_RSRVD_WELL_KNOWN_URI, resourceType, std::bind(&REHelper::OnPrimitiveResourceDiscovered, this, PH::_1));
        isSuccess = waitForResourceFound(m_primitiveResourceList, CALLBACK_WAIT_MAX);
    }
    catch (exception& e)
    {
        IOTIVITYTEST_LOG(ERROR, "Exception occurred while finding resource, exception is: %s",
                e.what());
        return false;
    }

    return isSuccess;

}

void REHelper::OnPrimitiveResourceDiscovered(
        shared_ptr< OIC::Service::RCSRemoteResourceObject > pResource)
{
    s_mutex.lock();

    IOTIVITYTEST_LOG(DEBUG, "Inside PrimitiveResourceFound!!");

    if (pResource && ((pResource->getUri().compare(LIGHT_URI) == 0) || (pResource->getUri().compare(FAN_URI) == 0)))
    {
        m_primitiveResourceList.push_back(pResource);
        IOTIVITYTEST_LOG(DEBUG, "Resource added to found resource list, host = %s , uri = %s",
                pResource->getAddress().c_str(), pResource->getUri().c_str());
    }
    else
    {
        IOTIVITYTEST_LOG(WARNING, "Found resource is invalid");
    }

    s_mutex.unlock();
}

vector< shared_ptr< RCSRemoteResourceObject > > REHelper::getFoundResourceList()
{
    return m_primitiveResourceList;
}

bool REHelper::waitForResourceFound(
        std::vector< shared_ptr< OIC::Service::RCSRemoteResourceObject > > &foundResourceList,
        int timeOut)
{
    bool isTimeOut = false;
    int passedTime = 0;
    while (foundResourceList.size() == 0)
    {
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        passedTime++;
        IOTIVITYTEST_LOG(INFO, "Waited %d seconds for finding resources", passedTime);

        if (passedTime > timeOut)
        {
            isTimeOut = true;
            break;
        }
    }
    discoveryTask->cancel();

    return !isTimeOut;
}

std::string REHelper::getResourceState(ResourceState resourceState)
{
    switch (resourceState) {
        case OIC::Service::ResourceState::NONE:
            return "NONE";

        case OIC::Service::ResourceState::REQUESTED:
            return "REQUESTED";

        case OIC::Service::ResourceState::ALIVE:
            return "ALIVE";

        case OIC::Service::ResourceState::LOST_SIGNAL:
            return "LOST_SIGNAL";

        case OIC::Service::ResourceState::DESTROYED:
            return "DESTROYED";

        default:
            return "NOT_DEFINED";
    }
}

std::string REHelper::getCacheState(CacheState cacheState)
{
    switch (cacheState) {
        case OIC::Service::CacheState::NONE:
            return "NONE";

        case OIC::Service::CacheState::UNREADY:
            return "UNREADY";

        case OIC::Service::CacheState::READY:
            return "READY";

        case OIC::Service::CacheState::LOST_SIGNAL:
            return "LOST_SIGNAL";

        default:
            return "NOT_DEFINED";
    }
}
