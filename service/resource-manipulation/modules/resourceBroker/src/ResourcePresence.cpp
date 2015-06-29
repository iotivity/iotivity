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

#include "../include/ResourcePresence.h"

#include <bits/atomic_base.h>
#include <bits/shared_ptr_base.h>
#include <time.h>
#include <unistd.h>
#include <cstdbool>
#include <exception>
#include <iostream>
#include <memory>
#include <strstream>

#include "PrimitiveResource.h"
#include "DeviceAssociation.h"
#include "DevicePresence.h"

ResourcePresence::ResourcePresence(PrimitiveResourcePtr pResource)
{
    primitiveResource = pResource;
    isTimeoutCB = false;
    receivedTime = 0L;
    pGetCB = std::bind(&ResourcePresence::getCB, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    pTimeoutCB = std::bind(&ResourcePresence::timeOutCB, this,std::placeholders::_1);
    pPollingCB = std::bind(&ResourcePresence::pollingCB, this,std::placeholders::_1);

    requesterList
    = std::unique_ptr<std::list<BrokerRequesterInfoPtr>>(new std::list<BrokerRequesterInfoPtr>);

    timeoutHandle = primitiveTimer.requestTimer(SAFE_TIME, pTimeoutCB);

    state = BROKER_STATE::REQUESTED;
    isWithinTime = true;

    mode = BROKER_MODE::NON_PRESENCE_MODE;
    primitiveResource->requestGet(pGetCB);

    registerDevicePresence();
}

void ResourcePresence::registerDevicePresence()
{
    std::string deviceAddress = primitiveResource->getHost();

    DevicePresencePtr foundDevice
    = DeviceAssociation::getInstance()->findDevice(deviceAddress);

    if(foundDevice == nullptr)
    {
        foundDevice = DevicePresencePtr(new DevicePresence(primitiveResource));
        DeviceAssociation::getInstance()->addDevice(foundDevice);
    }
    foundDevice->addPresenceResource(this);
}

ResourcePresence::~ResourcePresence()
{
    std::string deviceAddress = primitiveResource->getHost();

    DevicePresencePtr foundDevice
    = DeviceAssociation::getInstance()->findDevice(deviceAddress);

    if(foundDevice != nullptr)
    {
        foundDevice->removePresenceResource(this);

        if(foundDevice->isEmptyResourcePresence())
        {
            DeviceAssociation::getInstance()->removeDevice(foundDevice);
        }
    }

    requesterList->clear();

    state = BROKER_STATE::DESTROYED;
}

void ResourcePresence::addBrokerRequester(BrokerID _id, BrokerCB _cb)
{
    BrokerRequesterInfoPtr newRequester = BrokerRequesterInfoPtr(new BrokerRequesterInfo());
    newRequester->brockerId = _id;
    newRequester->brockerCB = _cb;
    requesterList->push_back(newRequester);
}

void ResourcePresence::removeAllBrokerRequester()
{
    requesterList->erase(requesterList->begin(), requesterList->end());
}

void ResourcePresence::removeBrokerRequester(BrokerID _id)
{
    std::list<BrokerRequesterInfoPtr>::iterator iter = requesterList->begin();
    for(; iter != requesterList->end(); ++iter)
    {
        if(iter->get()->brockerId == _id)
        {
            requesterList->erase(iter);
            break;
        }
    }
}

bool ResourcePresence::isEmptyRequester() const
{
    return requesterList->empty();
}

void ResourcePresence::requestResourceState()
{
    OC_LOG_V(DEBUG, BROKER_TAG, "Request Get\n");
    primitiveResource->requestGet(pGetCB);
}

void ResourcePresence::executeAllBrokerCB(BROKER_STATE changedState)
{
    OC_LOG_V(DEBUG, BROKER_TAG, "executeAllBrokerCB()");
    if(state != changedState)
    {
        state = changedState;
        for(BrokerRequesterInfoPtr & item : * requesterList)
        {
            item->brockerCB(state);
        }
    }
}

void ResourcePresence::setResourcestate(BROKER_STATE _state)
{
    this->state = _state;

}

void * ResourcePresence::timeOutCB(unsigned int msg)
{
    isTimeoutCB = true;

    time_t currentTime;
    time(&currentTime);
    currentTime+=0L;

    if((receivedTime == 0L) || ((receivedTime+SAFE_TIME) > currentTime))
    {
        return NULL;
    }
    this->isWithinTime = false;
    OC_LOG_V(DEBUG, BROKER_TAG, "Timeout execution. will be discard after receiving cb message");

    executeAllBrokerCB(BROKER_STATE::LOST_SIGNAL);
    pollingCB(0);

    isTimeoutCB = false;

    return NULL;
}

void * ResourcePresence::pollingCB(unsigned int msg)
{
    this->requestResourceState();
    timeoutHandle = primitiveTimer.requestTimer(SAFE_TIME,pTimeoutCB);

    return NULL;
}

void ResourcePresence::getCB(const HeaderOptions &hos, const ResponseStatement& rep, int eCode)
{
    OC_LOG_V(DEBUG, BROKER_TAG, "response getCB\n");
    while(isTimeoutCB)
    {
        OC_LOG_V(DEBUG, BROKER_TAG, "wait\n");
        sleep(2);
    }
    time_t currentTime;
    time(&currentTime);
    receivedTime = currentTime;
    try
    {
        state = BROKER_STATE::ALIVE;
        verifiedGetResponse(eCode);

        if(isWithinTime)
        {
            primitiveTimer.cancelTimer(timeoutHandle);
            isWithinTime = true;
        }

    }
    catch(std::exception& e)
    {

    }

    if(mode == BROKER_MODE::NON_PRESENCE_MODE)
    {
        // TODO set timer & request get
        primitiveTimer.requestTimer(SAFE_TIME,pPollingCB);
    }

}

void ResourcePresence::verifiedGetResponse(int eCode)
{
    switch(eCode)
    {
        case OC_STACK_OK:
        case OC_STACK_CONTINUE:
            state = BROKER_STATE::ALIVE;
            OC_LOG_V(DEBUG, BROKER_TAG, "resource state : %d",(int)state);
            break;

        case OC_STACK_INVALID_REQUEST_HANDLE:
        case OC_STACK_RESOURCE_DELETED:
        case OC_STACK_TIMEOUT:
        case OC_STACK_COMM_ERROR:
        case OC_STACK_PRESENCE_STOPPED:
        case OC_STACK_PRESENCE_TIMEOUT:
            if(!requesterList->empty())
            {
                executeAllBrokerCB(BROKER_STATE::LOST_SIGNAL);
            }
            else
            {
                setResourcestate(BROKER_STATE::LOST_SIGNAL);
            }
            break;

        default:
            if(!requesterList->empty())
            {
                executeAllBrokerCB(BROKER_STATE::LOST_SIGNAL);
            }
            else
            {
                setResourcestate(BROKER_STATE::LOST_SIGNAL);
            }
            break;
    }
}

PrimitiveResourcePtr ResourcePresence::getPrimitiveResource()
{
    return primitiveResource;
}

BROKER_STATE ResourcePresence::getResourceState()
{
    return state;
}

void ResourcePresence::changePresenceMode(BROKER_MODE newMode)
{
    if(newMode != mode)
    {
        if(newMode == BROKER_MODE::NON_PRESENCE_MODE)
        {
            requestResourceState();
        }
        mode = newMode;
    }
}
