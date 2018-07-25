/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/**
 * @file
 *
 * This file contains the declaration of classes and its members related to Resource Presence.
 */

#ifndef RB_RESOURCEPRESENCE_H_
#define RB_RESOURCEPRESENCE_H_

#include <functional>
#include <list>
#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "BrokerTypes.h"
#include "ExpiryTimer.h"

namespace OIC
{
    namespace Service
    {
       /**
        * This is internal class used by ResourceBroker, provides simple APIs to
        * monitor resource.
        *
        * @see ResourceBroker
        */
        class ResourcePresence : public std::enable_shared_from_this<ResourcePresence>
        {
        public:
            ResourcePresence();
            ~ResourcePresence();

            /**
             * Start Monitoring state for the given resource.
             * This method will be called internally by ResourceBroker.
             *
             * @param pResource Primitive resource.
             *
             * @see ResourceBroker
             * @see PrimitiveResourcePtr
             */
            void initializeResourcePresence(PrimitiveResourcePtr pResource);

            /**
             * Add given brokerId and callback in BrokerIDMap.
             *
             * @param _id Broker id to be added.
             * @param _cb Broker callback to be added.
             *
             * @see BrokerCB
             * @see BrokerIDMap
             */
            void addBrokerRequester(BrokerID _id, BrokerCB _cb);

           /**
            * Remove given brokerId from BrokerIDMap.
            *
            * @param _id Broker id to be removed.
            *
            * @see BrokerIDMap
            */
            void removeBrokerRequester(BrokerID _id);

           /**
            * Remove all the brokerId [Requester entries] from BrokerIDMap.
            *
            * @see BrokerIDMap
            */
            void removeAllBrokerRequester();

           /**
            * Requests the current state of resource.
            *
            */
            void requestResourceState() const;

           /**
            * Change the broker mode.
            *
            * @see BROKER_MODE
            */
            void changePresenceMode(BROKER_MODE newMode);

           /**
            * Check whether BrokerIDMap is empty or not.
            *
            * @return True if BrokerIDMap is empty, otherwise false.
            *
            * @see BrokerIDMap
            */
            bool isEmptyRequester() const;

           /**
            * Gets BrokerIDMap size.
            *
            * @return Size of BrokerIDMap.
            *
            * @see BrokerIDMap
            */
            int  requesterListSize() const;

           /**
            * Gets primitive resource.
            *
            * @return Primitive resource.
            *
            * @see PrimitiveResourcePtr
            */
            const PrimitiveResourcePtr getPrimitiveResource() const;

             /**
             * Gets current state of resource.
             *
             * @return Current resource state.
             *
             * @see BROKER_STATE
             */
            BROKER_STATE getResourceState() const;

        private:
            std::unique_ptr<std::list<BrokerRequesterInfoPtr>> requesterList;
            PrimitiveResourcePtr primitiveResource;
            ExpiryTimer expiryTimer;

            BROKER_STATE state;
            BROKER_MODE mode;

            bool isWithinTime;
            std::atomic_long receivedTime;
            std::mutex cbMutex;
            unsigned int timeoutHandle;

            RequestGetCB pGetCB;
            TimerCB pTimeoutCB;
            TimerCB pPollingCB;

            void registerDevicePresence();
        public:
            /** This method is for get callback */
            void getCB(const HeaderOptions &hos, const ResponseStatement& rep, int eCode);
            /** This method is for timeout callback */
            void timeOutCB(unsigned int msg);
        private:
            void verifiedGetResponse(int eCode);

            void pollingCB(unsigned int msg = 0);

            void executeAllBrokerCB(BROKER_STATE changedState);
            void setResourcestate(BROKER_STATE _state);
        };
    } // namespace Service
} // namespace OIC

#endif /* RB_RESOURCEPRESENCE_H_ */

