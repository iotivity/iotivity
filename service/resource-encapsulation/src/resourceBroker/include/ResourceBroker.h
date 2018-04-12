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

/**
 * @file
 *
 * This file contains the declaration of classes and its members related to Resource Broker.
 */

#ifndef RB_RESOURCEBROKER_H_
#define RB_RESOURCEBROKER_H_

#include <functional>
#include <list>
#include <string>
#include <algorithm>
#include <mutex>
#include <condition_variable>

#include "BrokerTypes.h"
#include "ResourcePresence.h"

namespace OIC
{
    namespace Service
    {
       /**
        * This is internal class for RCSRemoteResourceObject, represents resource
        * broker and provides simple ways to interact with it.
        * Resource broker represents, one of the feature of resource-encapsulation layer:
        * "Monitor the state of remote resource".
        *
        * @see RCSRemoteResourceObject
        */
        class ResourceBroker
        {
        public:

            /**
             * The Invalid parameter exception class for resource broker.
             *
             * @see RCSException
             */
            class InvalidParameterException: public RCSException
            {
            public:
                InvalidParameterException(std::string&& what)
                : RCSException{ std::move(what) } {}
            };

            /**
             * The failed to subscribe presence exception class for resource broker.
             *
             * @see RCSPlatformException
             */
            class FailedSubscribePresenceException: public RCSPlatformException
            {
            public:
                FailedSubscribePresenceException(OCStackResult reason)
                : RCSPlatformException{reason} {}
            };

            /**
             * Gets instance of ResourceBroker.
             *
             * @return ResourceBroker instance.
             */
            static ResourceBroker * getInstance();

            /**
             * Start Monitoring state for the given resource.
             * This method will be called internally by RCSRemoteResourceObject.
             *
             * @param pResource Primitive resource.
             * @param cb Broker callback function.
             *
             * @return Broker Id.
             *
             * @see BrokerCB
             * @see PrimitiveResourcePtr
             * @see RCSRemoteResourceObject
             */
            BrokerID hostResource(PrimitiveResourcePtr pResource, BrokerCB cb);

            /**
             * Stop Monitoring state for the given broker Id.
             * This method  will be called internally by RCSRemoteResourceObject.
             *
             * @param brokerId Broker Id.
             *
             * @see RCSRemoteResourceObject
             * @see BrokerID
             */
            void cancelHostResource(BrokerID brokerId);

           /**
            * Gets the current state for the given broker Id.
            *
            * @param brokerId Broker Id.
            *
            * @return Current state of resource.
            *
            * @see BROKER_STATE
            */
            BROKER_STATE getResourceState(BrokerID brokerId);

           /**
            * Gets the current state for the given primitive resource.
            *
            * @param pResource Primitive resource.
            *
            * @return Current state of resource.
            *
            * @see PrimitiveResourcePtr
            * @see BROKER_STATE
            */
            BROKER_STATE getResourceState(PrimitiveResourcePtr pResource);

        private:
            static ResourceBroker * s_instance;
            static std::mutex s_mutexForCreation;
            static std::unique_ptr<PresenceList>  s_presenceList;
            static std::unique_ptr<BrokerIDMap> s_brokerIDMap;

            ResourceBroker() = default;
            ~ResourceBroker();
            ResourceBroker(const ResourceBroker&) = delete;
            ResourceBroker(ResourceBroker&&) = delete;

            ResourceBroker& operator=(const ResourceBroker&) const = delete;
            ResourceBroker& operator=(ResourceBroker&&) const = delete;

            void initializeResourceBroker();
            BrokerID generateBrokerID();
            ResourcePresencePtr findResourcePresence(PrimitiveResourcePtr pResource);
        };
    } // namespace Service
} // namespace OIC

#endif /* RB_RESOURCEBROKER_H_ */
