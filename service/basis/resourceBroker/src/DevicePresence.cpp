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

#include "DevicePresence.h"

DevicePresence::DevicePresence(PrimitiveResourcePtr pResource, BrokerCB _cb)
{
    pSubscribeRequestCB = std::bind(&DevicePresence::subscribeCB, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    presenceResourceList
    = std::unique_ptr<std::list<ResourcePresencePtr>>(new std::list<ResourcePresencePtr>);
    addPresenceResource(pResource, _cb);

    OC::OCPlatform::subscribePresence(presenceHandle, pResource->getHost(),
            BROKER_TRANSPORT, pSubscribeRequestCB);
}

DevicePresence::~DevicePresence()
{
    // TODO Auto-generated destructor stub
    OC::OCPlatform::unsubscribePresence(presenceHandle);
    presenceResourceList->clear();
}

void DevicePresence::addPresenceResource(PrimitiveResourcePtr pResource, BrokerCB _cb)
{
    ResourcePresencePtr newPresenceResource = ResourcePresencePtr(new ResourcePresence(pResource, _cb));
    presenceResourceList->push_back(newPresenceResource);
}

ResourcePresencePtr DevicePresence::findResourcePresence(PrimitiveResourcePtr pResource, BrokerCB _cb)
{
    ResourcePresencePtr retResource(nullptr);
    if (presenceResourceList->empty())// || pResource.isEmpty())
    {
       return retResource;
    }
    else
    {
       std::list<ResourcePresencePtr>::iterator it;

       ResourcePresencePtr temp = ResourcePresencePtr(new ResourcePresence(pResource, _cb));
       it = std::find(presenceResourceList->begin(), presenceResourceList->end(), temp);

       if(it == presenceResourceList->end())
       {
           return retResource;
       }
       else
       {
           retResource = *it;
       }
    }

    return retResource;
}

void DevicePresence::subscribeCB(OCStackResult ret,
        const unsigned int seq, const std::string& msg)
{

}
