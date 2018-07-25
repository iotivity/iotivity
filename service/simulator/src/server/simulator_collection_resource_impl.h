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

#ifndef SIMULATOR_COLLECTION_RESOURCE_IMPL_H_
#define SIMULATOR_COLLECTION_RESOURCE_IMPL_H_

#include "simulator_collection_resource.h"
#include "simulator_resource_model_schema.h"
#include "request_model.h"
#include "RamlParser.h"

/**
 * @class SimulatorResourceFactory
 */
class SimulatorResourceFactory;
class SimulatorCollectionResourceImpl : public SimulatorCollectionResource
{
    public:
        friend class SimulatorResourceFactory;

        /**
         * check collection resource
         * @return boolean value
         */
        bool isCollection() const;
        /**
         * set the resource name
         * @param[in] name   resource name
         */
        void setName(const std::string &name);
        /**
         * set uri information
         * @param[in] uri   resource uri
         */
        void setURI(const std::string &uri);
        /**
         * set resource type
         * @param[in] resourceType   resource type
         */
        void setResourceType(const std::string &resourceType);
        /**
         * set interface type
         * @param[in] interfaceType  interface type
         */
        void setInterface(const std::string &interfaceType);
        /**
         * set interface type
         * @param[in] interfaceTypes  interface types
         */
        void setInterface(const std::vector<std::string> &interfaceTypes);
        /**
         * add the new interface
         * @param[in] interfaceType  interface type
         */
        void addInterface(const std::string &interfaceType);
        /**
         * set the observable state information
         * @param[in] state   value to set
         */
        void setObservable(bool state);
        /**
         * set discoverable state information
         * @param[in] state   value to set
         */
        void setDiscoverable(bool state);
        /**
         * set observer callback
         * @param[in] callback   observer callback function
         */
        void setObserverCallback(ObserverCallback callback);
        /**
         * set model change callback
         * @param[in] callback   callback function
         */
        void setModelChangeCallback(ResourceModelUpdateCallback callback);
        /**
         * check observable value
         * @return boolean value
         */
        bool isObservable() const;
        /**
         * check discoverable value
         * @return boolean value
         */
        bool isDiscoverable() const;
        /**
         * check resource observation started or not
         * @return boolean value
         */
        bool isStarted() const;
        /**
         * start resource
         */
        void start();
        /**
         * stop resource
         */
        void stop();
        /**
         * get resource model information
         * @return resource model object
         */
        SimulatorResourceModel getResourceModel();
        /**
         * get observers details
         * @return observer object
         */
        std::vector<ObserverInfo> getObservers() const;
        /**
         * notify to given Id
         * @param[in] observerID   observer id
         */
        void notify(int observerID);
        /**
         * notify to all
         */
        void notifyAll();
        /**
         * get supported resources information
         * @return supported resource object
         */
        std::vector<std::string> getSupportedResources();
        /**
         * add child resource information
         * @param[in] resource    simulator resource
         */
        void addChildResource(const std::shared_ptr<SimulatorResource> &resource);
        /**
         * remove child resource information
         * @param[in] resource    simulator resource
         */
        void removeChildResource(const std::shared_ptr<SimulatorResource> &resource);
        /**
         * remove child resource
         * @param[in] uri   resource URI
         */
        void removeChildResource(const std::string &uri);
        /**
         * get child resources information
         * @return resource object
         */
        std::vector<SimulatorResourceSP> getChildResources();

    private:
        SimulatorCollectionResourceImpl();
        void setResourceModel(const SimulatorResourceModel &resModel);
        void setResourceModelSchema(
            const std::shared_ptr<SimulatorResourceModelSchema> &resModelSchema);
        void setRequestModel(
            const std::unordered_map<std::string, std::shared_ptr<RequestModel>> &requestModels);

        OCEntityHandlerResult handleRequests(std::shared_ptr<OC::OCResourceRequest> request);
        std::shared_ptr<OC::OCResourceResponse> requestOnBaseLineInterface(
            std::shared_ptr<OC::OCResourceRequest> request);
        std::shared_ptr<OC::OCResourceResponse> requestOnLinkListInterface(
            std::shared_ptr<OC::OCResourceRequest> request);
        std::shared_ptr<OC::OCResourceResponse> requestOnBatchInterface(
            std::shared_ptr<OC::OCResourceRequest> request);
        OCEntityHandlerResult sendResponse(const std::shared_ptr<OC::OCResourceRequest> &request,
                                           const int errorCode, OCEntityHandlerResult responseResult);
        void sendNotification(OC::ObservationIds &observers);
        void addLink(const std::shared_ptr<SimulatorResource> &resource);
        void removeLink(std::string uri);
        bool isValidInterface(const std::string &interfaceType, const std::string &requestType);

        std::recursive_mutex m_objectLock;
        std::mutex m_modelLock;
        std::mutex m_childResourcesLock;
        std::mutex m_modelSchemaLock;
        SimulatorResourceModel m_resModel;
        std::shared_ptr<SimulatorResourceModelSchema> m_resModelSchema;
        std::unordered_map<std::string, std::shared_ptr<RequestModel>> m_requestModels;
        std::map<std::string, SimulatorResourceSP> m_childResources;
        std::vector<std::string> m_supportedTypes;
        std::vector<ObserverInfo> m_observersList;

        ObserverCallback m_observeCallback;
        ResourceModelUpdateCallback m_modelCallback;

        OCResourceProperty m_property;
        OCResourceHandle m_resourceHandle;
};

#endif
