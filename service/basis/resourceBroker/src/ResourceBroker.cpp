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
std::list< DevicePresence * >  ResourceBroker::s_presenceList = NULL;

ResourceBroker::ResourceBroker()
{
    // TODO Auto-generated constructor stub
    if(s_presenceList.empty())
    {
        s_presenceList = new std::list< DevicePresence * >();
    }
}

ResourceBroker::~ResourceBroker()
{
    // TODO Auto-generated destructor stub
    if(!s_presenceList.empty())
    {
        s_presenceList.clear();
    }
    delete s_presenceList;
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

OCStackResult ResourceBroker::hostResource(PrimitiveResource & pResource, BrokerCB cb)
{
    OCStackResult ret = OC_STACK_INVALID_PARAM;
    if (pResource.getUri().empty() || pResource.getHost().empty())
    {
        return ret;
    }
    if (cb == NULL)
    {
        ret = OC_STACK_INVALID_CALLBACK;
        return ret;
    }

    DevicePresence * presenceItem = findDevicePresence(pResource, cb);
    if(presenceItem == NULL)
    {
        DevicePresence *newItem = new DevicePresence(pResource, cb);
        s_presenceList.push_back(newItem);
    }
    else
    {
        presenceItem->addPresenceResource(pResource, cb);
    }

    return ret;
}

DevicePresence * ResourceBroker::findDevicePresence(PrimitiveResource& pResource, BrokerCB cb)
{
    DevicePresence * retDevice = NULL;
    if (s_presenceList.empty() || pResource.isEmpty())
    {
        return NULL;
    }
    else
    {
        std::list<DevicePresence *>::iterator it;

        DevicePresence * temp = new DevicePresence(pResource, cb);
        it = std::find(s_presenceList.begin(), s_presenceList.end(), temp);
        delete temp;

        if(it == s_presenceList.end())
        {
            return NULL;
        }
        else
        {
            retDevice = it;
        }
    }

    return retDevice;
}

ResourcePresence * ResourceBroker::findResourcePresence(PrimitiveResource & pResource, BrokerCB cb)
{
    ResourcePresence * retResource = NULL;
    if (s_presenceList.empty() || pResource.isEmpty())
    {
        return NULL;
    }
    else
    {
        DevicePresence * foundDevice = findDevicePresence(pResource, cb);
        retResource = foundDevice->findResourcePresence(pResource, cb);
    }

    return retResource;
}
