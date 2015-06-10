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

#include "BrokerTypes.h"
#include "ResourceBroker.h"

#define OIC_COAP "coap://"
#define DEFAULT_CONTEXT_VALUE 0x99

ResourceBroker * ResourceBroker::s_instance = NULL;
std::unique_ptr<std::list< DevicePresencePtr >>  ResourceBroker::s_presenceList(nullptr);

ResourceBroker::ResourceBroker()
{
    // TODO Auto-generated constructor stub
    if(s_presenceList == nullptr)
    {
        s_presenceList = std::unique_ptr<std::list< DevicePresencePtr >>(new std::list<DevicePresencePtr>);
    }
}

ResourceBroker::~ResourceBroker()
{
    // TODO Auto-generated destructor stub
    if(s_presenceList != nullptr)
    {
        s_presenceList->clear();
    }
}

ResourceBroker * ResourceBroker::getInstance()
{
    if (!s_instance)
    {
        s_mutexForCreation.lock();
        if (!s_instance)
        {
            s_instance = new ResourceBroker();
        }
        s_mutexForCreation.unlock();
    }
    return s_instance;
}

OCStackResult ResourceBroker::hostResource(PrimitiveResourcePtr pResource, BrokerCB cb)
{
    OCStackResult ret = OC_STACK_INVALID_PARAM;
    if (pResource->getUri().empty() || pResource->getHost().empty())
    {
        return ret;
    }
    if (cb == NULL)
    {
        ret = OC_STACK_INVALID_CALLBACK;
        return ret;
    }

    DevicePresencePtr presenceItem = findDevicePresence(pResource, cb);
    if(presenceItem == NULL)
    {
        DevicePresencePtr newItem = DevicePresencePtr(new DevicePresence(pResource, cb));
        s_presenceList->push_back(newItem);
    }
    else
    {
        presenceItem->addPresenceResource(pResource, cb);
    }

    return ret;
}

DevicePresencePtr ResourceBroker::findDevicePresence(PrimitiveResourcePtr pResource, BrokerCB cb)
{
    DevicePresencePtr retDevice(nullptr);
    if (s_presenceList->empty())// || pResource->isEmpty())
    {
        return retDevice;
    }
    else
    {
        std::list<DevicePresencePtr>::iterator it;

        DevicePresencePtr temp = DevicePresencePtr(new DevicePresence(pResource, cb));
        it = std::find(s_presenceList->begin(), s_presenceList->end(), temp);

        if(it == s_presenceList->end())
        {
            return retDevice;
        }
        else
        {
            retDevice = *it;
        }
    }

    return retDevice;
}

ResourcePresencePtr ResourceBroker::findResourcePresence(PrimitiveResourcePtr pResource, BrokerCB cb)
{
    ResourcePresencePtr retResource(nullptr);
    if (s_presenceList->empty())// || pResource.isEmpty())
    {
        return retResource;
    }
    else
    {
        DevicePresencePtr foundDevice = findDevicePresence(pResource, cb);
        retResource = foundDevice->findResourcePresence(pResource, cb);
    }

    return retResource;
}
