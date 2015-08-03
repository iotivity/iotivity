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

#include "ResourceHosting.h"

#include "PresenceSubscriber.h"
#include "OCPlatform.h"
#include "RCSDiscoveryManager.h"

namespace OIC
{
namespace Service
{

namespace
{
    std::string HOSTING_TAG = "/hosting";
    size_t HOSTING_TAG_SIZE = (size_t)HOSTING_TAG.size();
    std::string MULTICAST_PRESENCE_ADDRESS = std::string("coap://") + OC_MULTICAST_PREFIX;
    std::string HOSTING_RESOURSE_TYPE = "Resource.Hosting";
}

ResourceHosting * ResourceHosting::s_instance(nullptr);
std::mutex ResourceHosting::s_mutexForCreation;

ResourceHosting::ResourceHosting()
: hostingObjectList(),
  discoveryManager(nullptr),
  presenceHandle(),
  pPresenceCB(nullptr), pDiscoveryCB(nullptr)
{
}

ResourceHosting * ResourceHosting::getInstance()
{
    if (!s_instance)
    {
        s_mutexForCreation.lock();
        if (!s_instance)
        {
            s_instance = new ResourceHosting();
            s_instance->initializeResourceHosting();
        }
        s_mutexForCreation.unlock();
    }
    return s_instance;
}

void ResourceHosting::startHosting()
{
    try
    {
        requestMulticastPresence();
        requestMulticastDiscovery();
    }catch(PlatformException &e)
    {
        OIC_HOSTING_LOG(DEBUG,
                "[ResourceHosting::startHosting]PlatformException:%s", e.what());
        throw;
    }catch(InvalidParameterException &e)
    {
        OIC_HOSTING_LOG(DEBUG,
                "[ResourceHosting::startHosting]InvalidParameterException:%s", e.what());
        throw;
    }catch(std::exception &e)
    {
        OIC_HOSTING_LOG(DEBUG,
                "[ResourceHosting::startHosting]std::exception:%s", e.what());
        throw;
    }
}

void ResourceHosting::stopHosting()
{
    // clear list hostingObjectList
    if(presenceHandle.isSubscribing())
    {
        presenceHandle.unsubscribe();
    }
    for(auto it : hostingObjectList)
    {
        it.reset();
    }
}

void ResourceHosting::initializeResourceHosting()
{
    pPresenceCB = std::bind(&ResourceHosting::presenceHandler, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    pDiscoveryCB = std::bind(&ResourceHosting::discoverHandler, this,
            std::placeholders::_1);

    discoveryManager = RCSDiscoveryManager::getInstance();
}

void ResourceHosting::requestMulticastPresence()
{
    try
    {
        presenceHandle = PresenceSubscriber(MULTICAST_PRESENCE_ADDRESS,
                OCConnectivityType::CT_DEFAULT, pPresenceCB);
    }catch(...)
    {
        throw;
    }
}

void ResourceHosting::presenceHandler(OCStackResult ret, const unsigned int /*seq*/,
        const std::string & address)
{
    switch(ret)
    {
    case OC_STACK_OK:
    case OC_STACK_CONTINUE:
    case OC_STACK_RESOURCE_CREATED:
    {
        // TODO start discovery
        requestDiscovery(address);
        break;
    }

    case OC_STACK_RESOURCE_DELETED:
    case OC_STACK_COMM_ERROR:
    case OC_STACK_TIMEOUT:
    case OC_STACK_PRESENCE_STOPPED:
    case OC_STACK_PRESENCE_TIMEOUT:
    case OC_STACK_PRESENCE_DO_NOT_HANDLE:
    case OC_STACK_ERROR:
        // TODO presence error
        break;

    case OC_STACK_INVALID_URI:
    case OC_STACK_INVALID_QUERY:
    case OC_STACK_INVALID_IP:
    case OC_STACK_INVALID_PORT:
    case OC_STACK_INVALID_CALLBACK:
    case OC_STACK_INVALID_METHOD:
    case OC_STACK_INVALID_PARAM:
    case OC_STACK_INVALID_OBSERVE_PARAM:
    case OC_STACK_NO_MEMORY:
    case OC_STACK_ADAPTER_NOT_ENABLED:
    case OC_STACK_NOTIMPL:
    case OC_STACK_NO_RESOURCE:
    case OC_STACK_RESOURCE_ERROR:
    case OC_STACK_SLOW_RESOURCE:
    case OC_STACK_DUPLICATE_REQUEST:
    case OC_STACK_NO_OBSERVERS:
    case OC_STACK_OBSERVER_NOT_FOUND:
    case OC_STACK_INVALID_OPTION:
    case OC_STACK_VIRTUAL_DO_NOT_HANDLE:
    case OC_STACK_MALFORMED_RESPONSE:
    case OC_STACK_PERSISTENT_BUFFER_REQUIRED:
    case OC_STACK_INVALID_REQUEST_HANDLE:
    case OC_STACK_INVALID_DEVICE_INFO:
    case OC_STACK_INVALID_JSON:
        break;
    default:
        // TODO unknown presence result
        break;
    }
}

void ResourceHosting::requestMulticastDiscovery()
{
    requestDiscovery();
}
void ResourceHosting::requestDiscovery(std::string address)
{
    std::string host = address;
    std::string uri = OC_RSRVD_WELL_KNOWN_URI + std::string("?rt=") + HOSTING_RESOURSE_TYPE;
    RCSAddress rcsAddress = RCSAddress::unicast(host);
    discoveryManager->discoverResource(rcsAddress, uri, pDiscoveryCB);
}

void ResourceHosting::discoverHandler(RemoteObjectPtr remoteResource)
{
    std::string discoverdUri = remoteResource->getUri();
    if(discoverdUri.compare(
            discoverdUri.size()-HOSTING_TAG_SIZE, HOSTING_TAG_SIZE, HOSTING_TAG) != 0)
    {
        return;
    }

    HostingObjectPtr foundHostingObject = findRemoteResource(remoteResource);
    if(foundHostingObject == nullptr)
    {
        try
        {
            foundHostingObject.reset(new HostingObject());
            foundHostingObject->initializeHostingObject(remoteResource,
                    std::bind(&ResourceHosting::destroyedHostingObject, this, foundHostingObject));
            hostingObjectList.push_back(foundHostingObject);
        }catch(InvalidParameterException &e)
        {
            OIC_HOSTING_LOG(DEBUG,
                    "[ResourceHosting::discoverHandler]InvalidParameterException:%s", e.what());
        }
    }
}

ResourceHosting::HostingObjectPtr ResourceHosting::findRemoteResource(
        RemoteObjectPtr remoteResource)
{
    HostingObjectPtr retObject = nullptr;

    for(auto it : hostingObjectList)
    {
        RemoteObjectPtr inListPtr = it->getRemoteResource();
        if(inListPtr != nullptr && isSameRemoteResource(inListPtr, remoteResource))
        {
            retObject = it;
        }
    }

    return retObject;
}

bool ResourceHosting::isSameRemoteResource(
        RemoteObjectPtr remoteResource_1, RemoteObjectPtr remoteResource_2)
{
    bool ret = false;
    if(remoteResource_1->getAddress() == remoteResource_2->getAddress() &&
//       remoteResource_1->getID() == remoteResource_2->getID() &&
       remoteResource_1->getUri() == remoteResource_2->getUri())
    {
        ret = true;
    }
    return ret;
}

void ResourceHosting::destroyedHostingObject(HostingObjectPtr destroyedPtr)
{
    hostingObjectList.remove(destroyedPtr);
}

} /* namespace Service */
} /* namespace OIC */
