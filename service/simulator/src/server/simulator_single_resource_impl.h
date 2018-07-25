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
#include "simulator_resource_model_schema.h"
#include "request_model.h"
#include "resource_update_automation_mngr.h"
#include "RamlParser.h"

/**
 * @class SimulatorResourceFactory
 */
class SimulatorResourceFactory;
class SimulatorSingleResourceImpl : public SimulatorSingleResource,
    public std::enable_shared_from_this<SimulatorSingleResourceImpl>
{
    public:
        friend class SimulatorResourceFactory;
        /**
         * set resource name
         * @param[in] name  resource name
         */
        void setName(const std::string &name);
        /**
         * set the given URI information
         * @param[in] uri   resource uri
         */
        void setURI(const std::string &uri);
        /**
         * set the resource type
         * @param[in] resourceType  resource type
         */
        void setResourceType(const std::string &resourceType);
        /**
         * set the interface detail
         * @param[in] interfaceType  interface type
         */
        void setInterface(const std::string &interfaceType);
        /**
         * set interface
         * @param[in] interfaceType  interface types
         */
        void setInterface(const std::vector<std::string> &interfaceTypes);
        /**
         * add new interface
         * @param[in] interfaceType  interface type
         */
        void addInterface(const std::string &interfaceType);
        /**
         * set observable state information
         * @param[in] state   state to set
         */
        void setObservable(bool state);
        /**
         * set discoverable state information
         * @param[in] state   state to set
         */
        void setDiscoverable(bool state);
        /**
         * set observer callback
         * @param[in] callback   callback function
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
         * check resource is started or not
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
         * @return observers object
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
         * get attribute value
         * @param[in] attrName   attribute name
         * @param[out] attribute attribute
         * @return boolean value
         */
        bool getAttribute(const std::string &attrName, SimulatorResourceAttribute &attribute);
        /**
         * get attribute
         * @return resource object
         */
        std::map<std::string, SimulatorResourceAttribute> getAttributes();
        /**
         * add new attribute
         * @param[in] attribute   resource attribute
         * @param[in] notify      notify resource value
         * @return boolean value
         */
        bool addAttribute(const SimulatorResourceAttribute &attribute, bool notify = true);
        /**
         * update attribute value
         * @param[in] attribute   resource attribute
         * @param[in] notify      notify resource value
         * @return boolean value
         */
        bool updateAttributeValue(const SimulatorResourceAttribute &attribute,
                                  bool notify = true);
        /**
         * remove attribute with given information
         * @param[in] attrName     attribute name
         * @param[in] notify       notify resource value
         * @return boolean value
         */
        bool removeAttribute(const std::string &attrName, bool notify = true);
        /**
         * start resource updation
         * @param[in] type            update type
         * @param[in] updateInterval  update interval
         * @param[in] callback        callback function
         * return resource object
         */
        int startResourceUpdation(AutoUpdateType type, int updateInterval,
                                  AutoUpdateCompleteCallback callback);
        /**
         * start attribute updation
         * @param[in] attrName         attribute name
         * @param[in] type             update type
         * @param[in] updateInterval   update interval
         * @param[in] callback         callback function
         * @return attribute object
         */
        int startAttributeUpdation(const std::string &attrName, AutoUpdateType type,
                                   int updateInterval, AutoUpdateCompleteCallback callback);
        /**
         * get resource updations
         * @return resource object
         */
        std::vector<int> getResourceUpdations();
        /**
         * get attribute updations
         * @return attribute object
         */
        std::vector<int> getAttributeUpdations();
        /**
         * set updation
         * @param[in] id   id to update
         */
        void stopUpdation(const int id);
        /**
         * update resource model
         * @param[in] reqResModel      resource model
         * @param[in] updatedResModel  resource model to update
         * @param[in] overwrite        boolean value for overwrite
         * @param[in] notify           boolean value for notify
         * @return boolean value
         */
        bool updateResourceModel(const SimulatorResourceModel &reqResModel,
                                 SimulatorResourceModel &updatedResModel, bool overwrite = false,
                                 bool notify = true);
        /**
         * notify app
         */
        void notifyApp();
        /**
         * notify app
         * @param[in] resModel   resource model
         */
        void notifyApp(const SimulatorResourceModel &resModel);

    private:
        SimulatorSingleResourceImpl();
        void setResourceModel(const SimulatorResourceModel &resModel);
        void setResourceModelSchema(
            const std::shared_ptr<SimulatorResourceModelSchema> &resModelSchema);
        void setRequestModel(
            const std::unordered_map<std::string, std::shared_ptr<RequestModel>> &requestModels);
        void notify(int observerID, const SimulatorResourceModel &resModel);
        void notifyAll(const SimulatorResourceModel &resModel);
        void addObserver(const OC::ObservationInfo &ocObserverInfo);
        void removeObserver(const OC::ObservationInfo &ocObserverInfo);
        void removeAllObservers();
        void setCommonProperties(OC::OCRepresentation &ocResRep);
        OCEntityHandlerResult handleRequests(std::shared_ptr<OC::OCResourceRequest> request);
        OCEntityHandlerResult handleGET(const std::shared_ptr<OC::OCResourceRequest> &request);
        OCEntityHandlerResult handlePOST(const std::shared_ptr<OC::OCResourceRequest> &request);
        OCEntityHandlerResult handlePUT(const std::shared_ptr<OC::OCResourceRequest> &request);
        OCEntityHandlerResult handleDELETE(const std::shared_ptr<OC::OCResourceRequest> &request);
        bool isValidInterface(const std::string &interfaceType, const std::string &requestType);
        OCEntityHandlerResult sendResponse(const std::shared_ptr<OC::OCResourceRequest> &request,
                                           const int errorCode, OCEntityHandlerResult responseResult);
        OCEntityHandlerResult sendResponse(const std::shared_ptr<OC::OCResourceRequest> &request,
                                           const int errorCode, OCEntityHandlerResult responseResult, OC::OCRepresentation &Payload,
                                           const std::string &interfaceType);

        std::recursive_mutex m_objectLock;
        std::recursive_mutex m_modelLock;
        std::mutex m_modelSchemaLock;
        SimulatorResourceModel m_resModel;
        std::shared_ptr<SimulatorResourceModelSchema> m_resModelSchema;
        std::unordered_map<std::string, std::shared_ptr<RequestModel>> m_requestModels;
        UpdateAutomationMngr m_updateAutomationMgr;
        std::vector<ObserverInfo> m_observersList;

        ResourceModelUpdateCallback m_modelCallback;
        ObserverCallback m_observeCallback;

        OCResourceProperty m_property;
        OCResourceHandle m_resourceHandle;
};

typedef std::shared_ptr<SimulatorSingleResourceImpl> SimulatorSingleResourceImplSP;

#endif
