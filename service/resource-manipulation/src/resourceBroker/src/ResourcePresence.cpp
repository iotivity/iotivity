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

#include "ResourcePresence.h"

#include <bits/atomic_base.h>
#include <bits/shared_ptr_base.h>
#include <time.h>
#include <unistd.h>
#include <cstdbool>
#include <exception>
#include <iostream>
#include <memory>

#include "PrimitiveResource.h"
#include "DeviceAssociation.h"
#include "DevicePresence.h"

namespace OIC
{
    namespace Service
    {
        ResourcePresence::ResourcePresence()
        {
            primitiveResource = nullptr;
            isTimeoutCB = false;
            receivedTime = 0L;
            state = BROKER_STATE::REQUESTED;
            isWithinTime = true;
            mode = BROKER_MODE::NON_PRESENCE_MODE;
            timeoutHandle = 0;

            requesterList = nullptr;

            pGetCB = std::bind(&ResourcePresence::getCB, this,
                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            pTimeoutCB = std::bind(&ResourcePresence::timeOutCB, this,std::placeholders::_1);
            pPollingCB = std::bind(&ResourcePresence::pollingCB, this,std::placeholders::_1);
        }

        void ResourcePresence::initializeResourcePresence(PrimitiveResourcePtr pResource)
        {
            primitiveResource = pResource;
            requesterList
            = std::unique_ptr<std::list<BrokerRequesterInfoPtr>>
            (new std::list<BrokerRequesterInfoPtr>);

            timeoutHandle = expiryTimer.postTimer(BROKER_SAFE_MILLISECOND, pTimeoutCB);

            primitiveResource->requestGet(pGetCB);

            registerDevicePresence();
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
            BrokerRequesterInfoPtr newRequester;
            newRequester.reset(new BrokerRequesterInfo());

            newRequester->brokerId = _id;
            newRequester->brokerCB = _cb;
            requesterList->push_back(newRequester);
        }

        void ResourcePresence::removeAllBrokerRequester()
        {
            if(requesterList != nullptr)
            {
                requesterList->erase(requesterList->begin(), requesterList->end());
            }
        }

        void ResourcePresence::removeBrokerRequester(BrokerID _id)
        {
            std::list<BrokerRequesterInfoPtr>::iterator iter = requesterList->begin();
            for(; iter != requesterList->end(); ++iter)
            {
                if(iter->get()->brokerId == _id)
                {
                    requesterList->erase(iter);
                    break;
                }
            }
        }

        bool ResourcePresence::isEmptyRequester() const
        {
            return (requesterList!=nullptr)?requesterList->empty():true;
        }

        int ResourcePresence::requesterListSize() const {
            return (requesterList!=nullptr)?requesterList->size():0;
        }

        void ResourcePresence::requestResourceState() const
        {
            OC_LOG_V(DEBUG, BROKER_TAG, "Request Get\n");
            primitiveResource->requestGet(pGetCB);
        }

        void ResourcePresence::registerDevicePresence()
        {
            std::string deviceAddress = primitiveResource->getHost();

            DevicePresencePtr foundDevice
            = DeviceAssociation::getInstance()->findDevice(deviceAddress);

            if(foundDevice == nullptr)
            {
                try
                {
                    foundDevice.reset(new DevicePresence());
                    foundDevice->initializeDevicePresence(primitiveResource);
                }catch(...)
                {
                    throw;
                }
                DeviceAssociation::getInstance()->addDevice(foundDevice);
            }
            foundDevice->addPresenceResource(this);
        }

        void ResourcePresence::executeAllBrokerCB(BROKER_STATE changedState)
        {
            OC_LOG_V(DEBUG, BROKER_TAG, "executeAllBrokerCB()");
            if(state != changedState)
            {
                setResourcestate(changedState);
                if(requesterList->empty() != true)
                {
                    for(BrokerRequesterInfoPtr & item : * requesterList)
                    {
                        item->brokerCB(state);
                    }
                }
            }
        }

        void ResourcePresence::setResourcestate(BROKER_STATE _state)
        {
            this->state = _state;
        }

        void ResourcePresence::timeOutCB(unsigned int msg)
        {
            std::unique_lock<std::mutex> lock(cbMutex);
            isTimeoutCB = true;

            time_t currentTime;
            time(&currentTime);
            currentTime += 0L;

            if((receivedTime.load(boost::memory_order_consume) == 0) ||
              ((receivedTime + BROKER_SAFE_SECOND) > currentTime ))
            {
                this->isWithinTime = false;
                isTimeoutCB = false;
                cbCondition.notify_all();

                return;
            }
            this->isWithinTime = false;
            OC_LOG_V(DEBUG, BROKER_TAG,
                    "Timeout execution. will be discard after receiving cb message");

            executeAllBrokerCB(BROKER_STATE::LOST_SIGNAL);
            pollingCB();

            isTimeoutCB = false;
            cbCondition.notify_all();
        }

        void ResourcePresence::pollingCB(unsigned int msg)
        {
            OC_LOG_V(DEBUG,BROKER_TAG,"IN PollingCB\n");
            this->requestResourceState();
            timeoutHandle = expiryTimer.postTimer(BROKER_SAFE_MILLISECOND,pTimeoutCB);
        }

        void ResourcePresence::getCB(const HeaderOptions &hos,
                const ResponseStatement& rep, int eCode)
        {
            OC_LOG_V(DEBUG, BROKER_TAG, "response getCB\n");
            while(isTimeoutCB)
            {
                OC_LOG_V(DEBUG, BROKER_TAG, "waiting for terminate TimeoutCB\n");
                std::unique_lock<std::mutex> lock(cbMutex);
                cbCondition.wait(lock);
            }

            time_t currentTime;
            time(&currentTime);
            receivedTime = currentTime;

            verifiedGetResponse(eCode);

            if(isWithinTime)
            {
                expiryTimer.cancelTimer(timeoutHandle);
                isWithinTime = true;
            }

            if(mode == BROKER_MODE::NON_PRESENCE_MODE)
            {
                // TODO set timer & request get
                expiryTimer.postTimer(BROKER_SAFE_MILLISECOND,pPollingCB);
            }

        }

        void ResourcePresence::verifiedGetResponse(int eCode)
        {
            BROKER_STATE verifiedState = BROKER_STATE::NONE;
            switch(eCode)
            {
            case OC_STACK_OK:
            case OC_STACK_CONTINUE:
                verifiedState = BROKER_STATE::ALIVE;
                break;

            case OC_STACK_RESOURCE_DELETED:
                verifiedState = BROKER_STATE::DESTROYED;
                break;

            case OC_STACK_INVALID_REQUEST_HANDLE:
            case OC_STACK_TIMEOUT:
            case OC_STACK_COMM_ERROR:
            case OC_STACK_PRESENCE_STOPPED:
            case OC_STACK_PRESENCE_TIMEOUT:
            default:
                verifiedState = BROKER_STATE::LOST_SIGNAL;
                break;
            }

            executeAllBrokerCB(verifiedState);
            OC_LOG_V(DEBUG, BROKER_TAG, "resource state : %d",(int)state);
        }

        const PrimitiveResourcePtr ResourcePresence::getPrimitiveResource() const
        {
            return primitiveResource;
        }

        BROKER_STATE ResourcePresence::getResourceState() const
        {
            return state;
        }

        void ResourcePresence::changePresenceMode(BROKER_MODE newMode)
        {
            if(newMode != mode)
            {
                expiryTimer.cancelTimer(timeoutHandle);
                if(newMode == BROKER_MODE::NON_PRESENCE_MODE)
                {
                    timeoutHandle = expiryTimer.postTimer(BROKER_SAFE_MILLISECOND,pTimeoutCB);
                    requestResourceState();
                }
                mode = newMode;
            }
        }
    } // namespace Service
} // namespace OIC
