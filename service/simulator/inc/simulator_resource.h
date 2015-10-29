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

#ifndef SIMULATOR_RESOURCE_H_
#define SIMULATOR_RESOURCE_H_

#include "simulator_server_types.h"
#include "simulator_resource_model.h"
#include "simulator_uncopyable.h"
#include "simulator_exceptions.h"

class SimulatorResource : public UnCopyable
{
    public:
        enum class Type
        {
            SINGLE_RESOURCE,
            COLLECTION_RESOURCE
        };

        /**
         * Callback method for receiving notifications when resource representation model changes.
         *
         * @param uri - URI of resource whose representation model got changed.
         * @param resModel - Resource model.
         */
        typedef std::function<void (const std::string &uri, SimulatorResourceModel &resModel)>
        ResourceModelChangedCallback;

        /**
         * Callback method for receiving notifications when observer is registered/unregistered
         * with resource.
         *
         * @param uri - Resource URI
         * @param state - OBSERVE_REGISTER if observer is registered, otherwise OBSERVE_UNREGISTER.
         * @param observerInfo - Information about observer.
         */
        typedef std::function<void (const std::string &uri, ObservationStatus state, const ObserverInfo &observerInfo)>
        ObserverCallback;

        /**
         * API to get the name of the resource.
         *
         * @return Resource name.
         */
        virtual std::string getName() const = 0;

        /**
         * API to get the type which indicates whether resource is single or collection resource.
         *
         * @return Type of resource.
         */
        virtual SimulatorResource::Type getType() const = 0;

        /**
         * API to get the resource URI.
         *
         * @return Resource URI.
         */
        virtual std::string getURI() const = 0;

        /**
         * API to get the resource type.
         *
         * @return Resource type.
         */
        virtual std::string getResourceType() const = 0;

        /**
         * API to get the interfaces resource is bound with.
         *
         * @return Interface type.
         */
        virtual std::vector<std::string> getInterface() const = 0;

        /**
         * API to set the name of the resource.
         *
         * @param name - Name to be set.
         *
         * NOTE: API throws @InvalidArgsException, @SimulatorException exceptions.
         */
        virtual void setName(const std::string &name) = 0;

        /**
         * API to set the resource URI.
         *
         * @param uri - URI to be set.
         *
         * NOTE: API throws @InvalidArgsException, @SimulatorException exceptions.
         */
        virtual void setURI(const std::string &uri) = 0;

        /**
         * API to set the resource type.
         *
         * @param resourceType - resource type string.
         *
         * NOTE: API throws @InvalidArgsException, @SimulatorException exceptions.
         */
        virtual void setResourceType(const std::string &resourceType) = 0;

        /**
         * API to add interface type for resource.
         *
         * @param interfaceType - interface to be added for resource.
         *
         * NOTE: API throws @InvalidArgsException, @SimulatorException exceptions.
         */
        virtual void addInterface(std::string interfaceType) = 0;

        /**
         * API to make the resource observable or not.
         *
         * @param state - true make the resource observable, otherwise non-observable.
         *
         * NOTE: API throws @SimulatorException exceptions.
         */
        virtual void setObservable(bool state) = 0;

        /**
         * API to set the callback for receiving the notifications when
         * observer is registered or unregistered with resource.
         *
         * @param callback - Callback to be set for receiving the notifications.
         */
        virtual void setObserverCallback(ObserverCallback callback) = 0;

        /**
         * API to get the observable state of resource.
         *
         * @return bool - true if resource is observable, otherwise false.
         */
        virtual bool isObservable() = 0;

        /**
         * API to get the start state of resource.
         *
         * @return bool - true if resource is started, otherwise false.
         */
        virtual bool isStarted() = 0;

        /**
         * API to start the resource.
         *
         * NOTE: API throws @SimulatorException exception.
         */
        virtual void start() = 0;

        /**
         * API to stop the resource.
         *
         * NOTE: API throws @SimulatorException exception.
         */
        virtual void stop() = 0;

        /**
         * API to get observers which are registered with resource.
         *
         * @return vector of ObserverInfo.
         */
        virtual std::vector<ObserverInfo> getObserversList() = 0;

        /**
         * API to notify current resource model to specific observer.
         *
         * @param id - Observer ID to notify.
         *
         * NOTE: API throws @SimulatorException exception.
         */
        virtual void notify(int id) = 0;

        /**
         * API to notify all registered observers.
         *
         * NOTE: API throws @SimulatorException exception.
         */
        virtual void notifyAll() = 0;
};

typedef std::shared_ptr<SimulatorResource> SimulatorResourceSP;

#endif
