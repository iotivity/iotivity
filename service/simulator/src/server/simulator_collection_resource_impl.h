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

class SimulatorResourceFactory;
class SimulatorCollectionResourceImpl : public SimulatorCollectionResource
{
    public:
        friend class SimulatorResourceFactory;

        std::string getName() const;
        SimulatorResource::Type getType() const;
        std::string getURI() const;
        std::string getResourceType() const;
        std::vector<std::string> getInterface() const;
        bool isObservable();
        bool isStarted();
        SimulatorResourceModel getResourceModel();
        void setInterface(const std::vector<std::string> &interfaces);
        void setName(const std::string &name);
        void setURI(const std::string &uri);
        void setResourceType(const std::string &resourceType);
        void addInterface(std::string interfaceType);
        void setObservable(bool state);
        void setObserverCallback(ObserverCallback callback);
        void setModelChangeCallback(ResourceModelChangedCallback callback);
        void start();
        void stop();
        std::vector<ObserverInfo> getObserversList();
        void notify(int id);
        void notifyAll();

        std::vector<std::string> getSupportedResources();
        void addChildResource(SimulatorResourceSP &resource);
        void removeChildResource(SimulatorResourceSP &resource);
        void removeChildResource(const std::string &uri);
        std::vector<SimulatorResourceSP> getChildResources();

        void setResourceModel(const SimulatorResourceModel &resModel);

    private:
        SimulatorCollectionResourceImpl();

        OCEntityHandlerResult handleRequests(std::shared_ptr<OC::OCResourceRequest> request);
        std::shared_ptr<OC::OCResourceResponse> requestOnBaseLineInterface(
            std::shared_ptr<OC::OCResourceRequest> request);
        std::shared_ptr<OC::OCResourceResponse> requestOnLinkListInterface(
            std::shared_ptr<OC::OCResourceRequest> request);
        std::shared_ptr<OC::OCResourceResponse> requestOnBatchInterface(
            std::shared_ptr<OC::OCResourceRequest> request);
        void sendNotification(OC::ObservationIds &observers);
        void addLink(SimulatorResourceSP &resource);
        void removeLink(std::string uri);

        SimulatorResource::Type m_type;
        std::string m_name;
        std::string m_uri;
        std::string m_resourceType;
        std::vector<std::string> m_interfaces;

        std::mutex m_modelLock;
        SimulatorResourceModel m_resModel;
        std::recursive_mutex m_objectLock;
        std::mutex m_childResourcesLock;
        std::map<std::string, SimulatorResourceSP> m_childResources;
        std::vector<std::string> m_supportedTypes;
        std::vector<ObserverInfo> m_observersList;
        ObserverCallback m_observeCallback;
        ResourceModelChangedCallback m_modelCallback;

        OCResourceProperty m_property;
        OCResourceHandle m_resourceHandle;
};

#endif
