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

DevicePresence::DevicePresence(PrimitiveResourcePtr pResource)
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
    } catch(PrimitiveException & e)
    {
        OC_LOG_V(DEBUG, BROKER_TAG, "exception in subscribe Presence %s",e.what());
    }

    state = DEVICE_STATE::REQUESTED;
    pTimeoutCB = std::bind(&DevicePresence::timeOutCB, this, std::placeholders::_1);
    isWithinTime = true;
    //TODO generate Timer(if(!isTimer))
}

DevicePresence::~DevicePresence()
{
    resourcePresenceList.clear();
}

const std::string DevicePresence::getAddress() const
{
    return address;
}

void DevicePresence::addPresenceResource(ResourcePresence * rPresence)
{
    resourcePresenceList.push_back(rPresence);
}

void DevicePresence::removePresenceResource(ResourcePresence * rPresence)
{
    resourcePresenceList.remove(rPresence);
}

void DevicePresence::requestAllResourcePresence()
{
    for(auto it : resourcePresenceList)
    {
        it->requestResourceState();
    }
}

bool DevicePresence::isEmptyResourcePresence() const
{
    return resourcePresenceList.empty();
}

void DevicePresence::subscribeCB(OCStackResult ret,
        const unsigned int seq, const std::string& hostAddress)
{
    const char* log = hostAddress.c_str();
    OC_LOG_V(DEBUG, BROKER_TAG, "Received presence CB from: %s",log);
    OC_LOG_V(DEBUG, BROKER_TAG, "In subscribeCB: %d",ret);

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
                if(!resourcePresenceList.empty())
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
                if(!resourcePresenceList.empty())
                {
                    OC_LOG_V(DEBUG, BROKER_TAG, "ready to execute requestAllResourcePresence()");
                    requestAllResourcePresence();
                }
                break;

            case OC_STACK_PRESENCE_STOPPED:
            case OC_STACK_PRESENCE_TIMEOUT:
                OC_LOG_V(DEBUG, BROKER_TAG, "Server Presence Stop!!");
                state = DEVICE_STATE::LOST_SIGNAL;
                requestAllResourcePresence();
                break;

            case OC_STACK_PRESENCE_DO_NOT_HANDLE:
                OC_LOG_V(DEBUG, BROKER_TAG, "Presence Lost Signal because do not handled");
                state = DEVICE_STATE::LOST_SIGNAL;
                requestAllResourcePresence();
                break;

            default:
                OC_LOG_V(DEBUG, BROKER_TAG, "Presence Lost Signal because unknown type");
                state = DEVICE_STATE::LOST_SIGNAL;
                requestAllResourcePresence();
                break;
        }
    }
    else
    {
        OC_LOG_V(DEBUG, BROKER_TAG, "This response is Timeout but device steel alive");
        this->state = DEVICE_STATE::ALIVE;
        isWithinTime = true;
    }
}

void * DevicePresence::timeOutCB(unsigned int msg)
{
    this->isWithinTime = false;
    OC_LOG_V(DEBUG, BROKER_TAG, "Timeout execution. will be discard after receiving cb message");
    state = DEVICE_STATE::LOST_SIGNAL;

    return NULL;
}
