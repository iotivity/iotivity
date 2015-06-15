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
#include "PrimitiveException.h"

DevicePresence::DevicePresence(PrimitiveResourcePtr pResource, BrokerCB _cb)
{
    createDevicePresence(pResource, _cb);
}

DevicePresence::DevicePresence()
{
}

DevicePresence::~DevicePresence()
{
    // TODO Auto-generated destructor stub

    resourcePresenceList->clear();
}

void DevicePresence::createDevicePresence(PrimitiveResourcePtr pResource, BrokerCB _cb)
{
    pSubscribeRequestCB = std::bind(&DevicePresence::subscribeCB, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    const char* log = pResource->getHost().c_str();
    OC_LOG_V(DEBUG, BROKER_TAG, "%s",log);
    try
    {
        presenceSubscriber
        = PresenceSubscriber(pResource->getHost(), BROKER_TRANSPORT, pSubscribeRequestCB);

        OC_LOG_V(DEBUG, BROKER_TAG, "subscribe Presence");
    } catch(PrimitiveException e)
    {
        OC_LOG_V(DEBUG, BROKER_TAG, "exception in subscribe Presence %s",e.what());
    }
    resourcePresenceList
    = std::unique_ptr<std::list<ResourcePresencePtr>>(new std::list<ResourcePresencePtr>);

    addPresenceResource(pResource, _cb);
    state = DEVICE_STATE::REQUESTED;
    pTimeoutCB = std::bind(&DevicePresence::TimeOutCB, this,
                std::placeholders::_1);
    //TODO generate Timer(if(!isTimer))
    //register pTimeroutCB
    this->isWithinTime = true;
}
void DevicePresence::TimeOutCB(int msg)
{
    this->isWithinTime = false;
    OC_LOG_V(DEBUG, BROKER_TAG, "Timeout execution. will be discard after receiving cb message");
    state = DEVICE_STATE::LOST_SIGNAL;
}
void DevicePresence::addPresenceResource(PrimitiveResourcePtr pResource, BrokerCB _cb)
{
    ResourcePresencePtr newPresenceResource = ResourcePresencePtr(new ResourcePresence(pResource, _cb));
    resourcePresenceList->push_back(newPresenceResource);

}

ResourcePresencePtr DevicePresence::findResourcePresence(PrimitiveResourcePtr pResource, BrokerCB _cb)
{
    ResourcePresencePtr retResource(nullptr);

    if (resourcePresenceList->empty())// || pResource.isEmpty())
    {
       return retResource;
    }
    else
    {
       std::list<ResourcePresencePtr>::iterator it;

       ResourcePresencePtr temp = ResourcePresencePtr(new ResourcePresence(pResource, _cb));
       it = std::find(resourcePresenceList->begin(), resourcePresenceList->end(), temp);
       if(it == resourcePresenceList->end())
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

ResourcePresencePtr DevicePresence::findResourcePresence(PrimitiveResourcePtr pResource)
{
    ResourcePresencePtr retResource(nullptr);
    if (resourcePresenceList->empty())// || pResource.isEmpty())
    {
       return retResource;
    }
    else
    {
        for(auto & it : * resourcePresenceList)
        {
            if(it->getPrimitiveResource() == pResource)
            {
                retResource = it;
            }
        }
    }
    return retResource;
}

void DevicePresence::requestAllResourcePresence()
{
    std::list<ResourcePresencePtr>::iterator it;
    for( auto & it : * resourcePresenceList)
    {
        it->requestResourceState();
    }
}

void DevicePresence::subscribeCB(OCStackResult ret,
        const unsigned int seq, const std::string& hostAddress)
{
    const char* log = hostAddress.c_str();
    OC_LOG_V(DEBUG, BROKER_TAG, "Received presence CB from: %s",log);
    OC_LOG_V(DEBUG, BROKER_TAG, "In subscribeCB: %d",ret);
    //TODO : cancel timer if(isTimer)
    if(isWithinTime)
    {
        switch(ret)
        {
            case OC_STACK_OK:
            case OC_STACK_RESOURCE_CREATED:
            case OC_STACK_CONTINUE:

                OC_LOG_V(DEBUG, BROKER_TAG, "SEQ# %d",seq);
                state = DEVICE_STATE::ALIVE;

                OC_LOG_V(DEBUG, BROKER_TAG, "device state : %d",(int)state);
                if(!resourcePresenceList->empty())
                {
                    requestAllResourcePresence();
                }
                break;
            case OC_STACK_INVALID_REQUEST_HANDLE:
            case OC_STACK_RESOURCE_DELETED:
            case OC_STACK_TIMEOUT:
            case OC_STACK_COMM_ERROR:
                //TODO get request
                state = DEVICE_STATE::REQUESTED;
                if(!resourcePresenceList->empty())
                {
                    OC_LOG_V(DEBUG, BROKER_TAG, "ready to execute requestAllResourcePresence()");
                    requestAllResourcePresence();
                }
                break;
            case OC_STACK_PRESENCE_STOPPED:
            case OC_STACK_PRESENCE_TIMEOUT:

                OC_LOG_V(DEBUG, BROKER_TAG, "Server Presence Stop!!");
                state = DEVICE_STATE::LOST_SIGNAL;
                for(auto & it : * resourcePresenceList)
                {
                    it->setResourcestate(BROKER_STATE::LOST_SIGNAL);
                    it->executeAllBrokerCB();
                }

                break;
            case OC_STACK_PRESENCE_DO_NOT_HANDLE:
                OC_LOG_V(DEBUG, BROKER_TAG, "Presence Lost Signal because do not handled");
                state = DEVICE_STATE::LOST_SIGNAL;
                break;
            default:
                state = DEVICE_STATE::LOST_SIGNAL;
                break;
        }
    }
    else
    {
        OC_LOG_V(DEBUG, BROKER_TAG, "its message is not available because of Timeout msg");
        this->state = DEVICE_STATE::ALIVE;
        //notify cb message to user.
        isWithinTime = true;
    }
}
