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

#ifndef SIMULATOR_SINGLE_RESOURCE_IMPL_H_
#define SIMULATOR_SINGLE_RESOURCE_IMPL_H_

#include "simulator_single_resource.h"
#include "resource_update_automation_mngr.h"
#include "RamlParser.h"

class SimulatorResourceFactory;
class SimulatorSingleResourceImpl : public SimulatorSingleResource
{
    public:
        friend class SimulatorResourceFactory;

        std::string getName() const;
        SimulatorResource::Type getType() const;
        std::string getURI() const;
        std::string getResourceType() const;
        std::vector<std::string> getInterface() const;
        void setInterface(const std::vector<std::string> &interfaces);
        void setName(const std::string &name);
        void setURI(const std::string &uri);
        void setResourceType(const std::string &resourceType);
        void addInterface(const std::string &interfaceType);
        void removeInterface(const std::string &interfaceType);
        void addInterface(const std::vector<std::string> &interfaceList);
        void removeInterface(const std::vector<std::string> &interfaceList);
        void setObservable(bool state);
        void setObserverCallback(ObserverCallback callback);
        bool isObservable();
        bool isStarted();
        void start();
        void stop();
        std::vector<ObserverInfo> getObserversList();
        void notify(int id);
        void notifyAll();
        void notify(int id, SimulatorResourceModel &resModel);
        void notifyAll(SimulatorResourceModel &resModel);

        bool getAttribute(const std::string &attrName,
                          SimulatorResourceModel::Attribute &attribute);
        void addAttribute(const SimulatorResourceModel::Attribute &attribute, bool notify = true);
        bool getAttributeProperty(const std::string &attrName,
                                  SimulatorResourceModel::AttributeProperty &property);
        bool setAttributeProperty(const std::string &attrName,
                                  const SimulatorResourceModel::AttributeProperty &property);
        bool updateAttributeValue(const SimulatorResourceModel::Attribute &attribute,
                                  bool notify = true);
        bool removeAttribute(const std::string &attrName, bool notify = true);
        SimulatorResourceModel getResourceModel();
        void setModelChangeCallback(ResourceModelChangedCallback callback);
        int startResourceUpdation(AutomationType type, int updateInterval,
                                  updateCompleteCallback callback);
        int startAttributeUpdation(const std::string &attrName, AutomationType type,
                                   int updateInterval, updateCompleteCallback callback);
        std::vector<int> getResourceUpdationIds();
        std::vector<int> getAttributeUpdationIds();
        void stopUpdation(const int id);
        void setResourceModel(const SimulatorResourceModel &resModel);
        void setPutErrorResponseModel(const SimulatorResourceModel &resModel);
        void setPostErrorResponseModel(const SimulatorResourceModel &resModel);
        void setActionType(std::map<RAML::ActionType, RAML::ActionPtr> &actionType);
        RAML::ActionType getActionType(std::string requestType);
        void notifyApp();
        void notifyApp(SimulatorResourceModel &resModel);

    private:
        SimulatorSingleResourceImpl();
        OCEntityHandlerResult handleRequests(std::shared_ptr<OC::OCResourceRequest> request);
        std::shared_ptr<OC::OCResourceResponse> handleReadRequest(
            std::shared_ptr<OC::OCResourceRequest> request, const std::string &interfaceType);
        std::shared_ptr<OC::OCResourceResponse> handleWriteRequest(
            std::shared_ptr<OC::OCResourceRequest> request, const std::string &interfaceType);
        OCEntityHandlerResult sendErrorResponse(std::shared_ptr<OC::OCResourceRequest> request,
            const int errCode, OCEntityHandlerResult responseCode);
        void setCommonProperties(SimulatorResourceModel &resModel);
        bool updateResourceModel(OC::OCRepresentation &ocRep, SimulatorResourceModel &resModel);
        void addObserver(OC::ObservationInfo ocObserverInfo);
        void removeObserver(OC::ObservationInfo ocObserverInfo);
        void removeAllObservers();

        SimulatorResource::Type m_type;
        std::string m_name;
        std::string m_uri;
        std::string m_resourceType;
        std::vector<std::string> m_interfaces;
        std::map<std::string, std::vector<std::string>> m_requestTypes;

        std::recursive_mutex m_objectLock;
        std::mutex m_modelLock;
        SimulatorResourceModel m_resModel;
        SimulatorResourceModel m_putErrorResModel;
        SimulatorResourceModel m_postErrorResModel;
        ResourceModelChangedCallback m_modelCallback;
        ObserverCallback m_observeCallback;
        UpdateAutomationMngr m_updateAutomationMgr;
        std::vector<ObserverInfo> m_observersList;
        std::map<RAML::ActionType , RAML::ActionPtr> m_actionTypes;

        OCResourceProperty m_property;
        OCResourceHandle m_resourceHandle;
};

typedef std::shared_ptr<SimulatorSingleResourceImpl> SimulatorSingleResourceImplSP;

#endif
