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

#include "simulator_single_resource_impl.h"
#include "simulator_utils.h"
#include "simulator_logger.h"
#include "logger.h"

#define TAG "SIM_SINGLE_RESOURCE"

SimulatorSingleResourceImpl::SimulatorSingleResourceImpl()
    :   m_type(SimulatorResource::Type::SINGLE_RESOURCE),
        m_interfaces {OC::DEFAULT_INTERFACE},
        m_resourceHandle(NULL)
{
    m_property = static_cast<OCResourceProperty>(OC_DISCOVERABLE | OC_OBSERVABLE);
}

std::string SimulatorSingleResourceImpl::getName() const
{
    return m_name;
}

SimulatorResource::Type SimulatorSingleResourceImpl::getType() const
{
    return m_type;
}

std::string SimulatorSingleResourceImpl::getURI() const
{
    return m_uri;
}

std::string SimulatorSingleResourceImpl::getResourceType() const
{
    return m_resourceType;
}

std::vector<std::string> SimulatorSingleResourceImpl::getInterface() const
{
    return m_interfaces;
}

void SimulatorSingleResourceImpl::setName(const std::string &name)
{
    VALIDATE_INPUT(name.empty(), "Name is empty!")

    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (m_resourceHandle)
    {
        throw SimulatorException(SIMULATOR_OPERATION_NOT_ALLOWED,
                                 "Name can not be set when resource is started!");
    }

    m_name = name;
}

void SimulatorSingleResourceImpl::setURI(const std::string &uri)
{
    VALIDATE_INPUT(uri.empty(), "Uri is empty!")

    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (m_resourceHandle)
    {
        throw SimulatorException(SIMULATOR_OPERATION_NOT_ALLOWED,
                                 "URI can not be set when resource is started!");
    }

    m_uri = uri;
}

void SimulatorSingleResourceImpl::setResourceType(const std::string &resourceType)
{
    VALIDATE_INPUT(resourceType.empty(), "Resource type is empty!")

    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (m_resourceHandle)
    {
        throw SimulatorException(SIMULATOR_OPERATION_NOT_ALLOWED,
                                 "Resource type can not be set when resource is started!");
    }

    m_resourceType = resourceType;
}

void SimulatorSingleResourceImpl::addInterface(std::string interfaceType)
{
    VALIDATE_INPUT(interfaceType.empty(), "Interface type is empty!")

    if (interfaceType == OC::LINK_INTERFACE
        || interfaceType == OC::BATCH_INTERFACE
        || interfaceType == OC::GROUP_INTERFACE)
    {
        throw NoSupportException("Single type resource does not support this interface!");
    }

    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (m_resourceHandle)
    {
        throw SimulatorException(SIMULATOR_OPERATION_NOT_ALLOWED,
                                 "Interface type can not be set when resource is started!");
    }

    auto found = std::find(m_interfaces.begin(), m_interfaces.end(), interfaceType);
    if (found != m_interfaces.end())
        m_interfaces.push_back(interfaceType);
}

void SimulatorSingleResourceImpl::setObservable(bool state)
{
    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (m_resourceHandle)
    {
        throw SimulatorException(SIMULATOR_OPERATION_NOT_ALLOWED,
                                 "Observation state can not be changed when resource is started!");
    }

    if (true == state)
        m_property = static_cast<OCResourceProperty>(m_property | OC_OBSERVABLE);
    else
        m_property = static_cast<OCResourceProperty>(m_property ^ OC_OBSERVABLE);
}

void SimulatorSingleResourceImpl::setObserverCallback(ObserverCallback callback)
{
    VALIDATE_CALLBACK(callback)
    m_observeCallback = callback;
}

bool SimulatorSingleResourceImpl::isObservable()
{
    return (m_property & OC_OBSERVABLE);
}

bool SimulatorSingleResourceImpl::isStarted()
{
    return (nullptr != m_resourceHandle);
}

void SimulatorSingleResourceImpl::start()
{
    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (m_resourceHandle)
    {
        throw SimulatorException(SIMULATOR_ERROR, "Resource already registered!");
    }

    if (m_uri.empty() || m_resourceType.empty())
    {
        throw SimulatorException(SIMULATOR_ERROR, "Found incomplete data to start resource!");
    }

    typedef OCStackResult (*RegisterResource)(OCResourceHandle &, std::string &, const std::string &,
            const std::string &, OC::EntityHandler, uint8_t);

    invokeocplatform(static_cast<RegisterResource>(OC::OCPlatform::registerResource),
                     m_resourceHandle, m_uri, m_resourceType, m_interfaces[0],
                     std::bind(&SimulatorSingleResourceImpl::handleRequests,
                               this, std::placeholders::_1), m_property);

    for (size_t index = 1; m_interfaces.size() > 1 && index < m_interfaces.size(); index++)
    {
        typedef OCStackResult (*bindInterfaceToResource)(const OCResourceHandle &,
                const std::string &);

        try
        {
            invokeocplatform(static_cast<bindInterfaceToResource>(
                                 OC::OCPlatform::bindInterfaceToResource), m_resourceHandle,
                             m_interfaces[index]);
        }
        catch (SimulatorException &e)
        {
            stop();
            throw;
        }
    }
}

void SimulatorSingleResourceImpl::stop()
{
    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (!m_resourceHandle)
        return;

    typedef OCStackResult (*UnregisterResource)(const OCResourceHandle &);

    invokeocplatform(static_cast<UnregisterResource>(OC::OCPlatform::unregisterResource),
                     m_resourceHandle);

    m_resourceHandle = nullptr;
}

std::vector<ObserverInfo> SimulatorSingleResourceImpl::getObserversList()
{
    return m_observersList;
}

void SimulatorSingleResourceImpl::notify(int id)
{
    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (!m_resourceHandle)
        return;

    std::shared_ptr<OC::OCResourceResponse> resourceResponse =
    {std::make_shared<OC::OCResourceResponse>()};

    resourceResponse->setErrorCode(200);
    resourceResponse->setResponseResult(OC_EH_OK);
    resourceResponse->setResourceRepresentation(m_resModel.getOCRepresentation(),
            OC::DEFAULT_INTERFACE);

    OC::ObservationIds observers;
    observers.push_back(id);

    typedef OCStackResult (*NotifyListOfObservers)(OCResourceHandle, OC::ObservationIds &,
            const std::shared_ptr<OC::OCResourceResponse>);

    invokeocplatform(static_cast<NotifyListOfObservers>(OC::OCPlatform::notifyListOfObservers),
                     m_resourceHandle, observers, resourceResponse);
}

void SimulatorSingleResourceImpl::notifyAll()
{
    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (!m_resourceHandle)
        return;

    if (!m_observersList.size())
        return;

    std::shared_ptr<OC::OCResourceResponse> resourceResponse =
    {std::make_shared<OC::OCResourceResponse>()};

    resourceResponse->setErrorCode(200);
    resourceResponse->setResponseResult(OC_EH_OK);
    resourceResponse->setResourceRepresentation(m_resModel.getOCRepresentation(),
            OC::DEFAULT_INTERFACE);

    OC::ObservationIds observers;
    for (auto &observer : m_observersList)
        observers.push_back(observer.id);

    typedef OCStackResult (*NotifyListOfObservers)(OCResourceHandle, OC::ObservationIds &,
            const std::shared_ptr<OC::OCResourceResponse>);

    invokeocplatform(static_cast<NotifyListOfObservers>(OC::OCPlatform::notifyListOfObservers),
                     m_resourceHandle, observers, resourceResponse);
}

bool SimulatorSingleResourceImpl::getAttribute(const std::string &attrName,
        SimulatorResourceModel::Attribute &attribute)
{
    VALIDATE_INPUT(attrName.empty(), "Attribute name is empty!")

    std::lock_guard<std::mutex> lock(m_modelLock);
    return m_resModel.getAttribute(attrName, attribute);
}

void SimulatorSingleResourceImpl::addAttribute(const SimulatorResourceModel::Attribute &attribute,
        bool notify)
{
    std::lock_guard<std::mutex> lock(m_modelLock);
    if (m_resModel.containsAttribute(attribute.getName()))
        throw SimulatorException(SIMULATOR_ERROR, "Attribute exist with same name!");

    if (!m_resModel.add(attribute))
        throw SimulatorException(SIMULATOR_ERROR, "Failed to add attribute!");

    if (notify && isStarted())
        notifyAll();
}

bool SimulatorSingleResourceImpl::getAttributeProperty(const std::string &attrName,
        SimulatorResourceModel::AttributeProperty &property)
{
    VALIDATE_INPUT(attrName.empty(), "Attribute name is empty!")

    std::lock_guard<std::mutex> lock(m_modelLock);
    return m_resModel.getAttributeProperty(attrName, property);
}

bool SimulatorSingleResourceImpl::setAttributeProperty(const std::string &attrName,
        const SimulatorResourceModel::AttributeProperty &property)
{
    VALIDATE_INPUT(attrName.empty(), "Attribute name is empty!")

    std::lock_guard<std::mutex> lock(m_modelLock);
    return m_resModel.setAttributeProperty(attrName, property);
}

bool SimulatorSingleResourceImpl::updateAttributeValue(
    const SimulatorResourceModel::Attribute &attribute,
    bool notify)
{
    std::lock_guard<std::mutex> lock(m_modelLock);
    if (m_resModel.updateValue(attribute))
    {
        if (notify && isStarted())
            notifyAll();
        return true;
    }

    return false;
}

bool SimulatorSingleResourceImpl::removeAttribute(const std::string &attrName, bool notify)
{
    VALIDATE_INPUT(attrName.empty(), "Attribute name is empty!")

    std::lock_guard<std::mutex> lock(m_modelLock);
    if (m_resModel.removeAttribute(attrName))
    {
        if (notify && isStarted())
            notifyAll();
        return true;
    }

    return false;
}

SimulatorResourceModel SimulatorSingleResourceImpl::getResourceModel()
{
    std::lock_guard<std::mutex> lock(m_modelLock);
    return m_resModel;
}

void SimulatorSingleResourceImpl::setModelChangeCallback(ResourceModelChangedCallback callback)
{
    VALIDATE_CALLBACK(callback)
    m_modelCallback = callback;
}

int SimulatorSingleResourceImpl::startResourceUpdation(AutomationType type,
        int updateInterval, updateCompleteCallback callback)
{
    VALIDATE_CALLBACK(callback)

    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (!m_resourceHandle)
        throw SimulatorException(SIMULATOR_NO_RESOURCE, "Resource is not registered!");

    return m_updateAutomationMgr.startResourceAutomation(this, type, updateInterval, callback);
}

int SimulatorSingleResourceImpl::startAttributeUpdation(const std::string &attrName,
        AutomationType type, int updateInterval, updateCompleteCallback callback)
{
    VALIDATE_CALLBACK(callback)

    std::lock_guard<std::recursive_mutex> lock(m_objectLock);
    if (!m_resourceHandle)
        throw SimulatorException(SIMULATOR_NO_RESOURCE, "Resource is not registered!");

    return m_updateAutomationMgr.startAttributeAutomation(this, attrName, type,
            updateInterval, callback);
}

std::vector<int> SimulatorSingleResourceImpl::getResourceUpdationIds()
{
    return m_updateAutomationMgr.getResourceAutomationIds();
}

std::vector<int> SimulatorSingleResourceImpl::getAttributeUpdationIds()
{
    return m_updateAutomationMgr.getAttributeAutomationIds();
}

void SimulatorSingleResourceImpl::stopUpdation(const int id)
{
    m_updateAutomationMgr.stop(id);
}

void SimulatorSingleResourceImpl::setResourceModel(const SimulatorResourceModel &resModel)
{
    std::lock_guard<std::mutex> lock(m_modelLock);
    m_resModel = resModel;
}

void SimulatorSingleResourceImpl::notifyApp()
{
    if (m_modelCallback)
    {
        SimulatorResourceModel resModel = m_resModel;
        m_modelCallback(m_uri, resModel);
    }
}

OCEntityHandlerResult SimulatorSingleResourceImpl::handleRequests(
    std::shared_ptr<OC::OCResourceRequest> request)
{
    OCEntityHandlerResult errCode = OC_EH_ERROR;
    if (!request)
        return OC_EH_ERROR;

    if (OC::RequestHandlerFlag::RequestFlag & request->getRequestHandlerFlag())
    {
        {
            OC::OCRepresentation rep = request->getResourceRepresentation();
            std::string payload = getPayloadString(rep);
            SIM_LOG(ILogger::INFO, "[" << m_name << "] " << request->getRequestType()
                    << " request received. \n**Payload details**\n" << payload)
        }

        // Handover the request to appropriate interface handler
        std::string interfaceType(OC::DEFAULT_INTERFACE);
        OC::QueryParamsMap queryParams = request->getQueryParameters();
        if (queryParams.end() != queryParams.find("if"))
            interfaceType = queryParams["if"];

        std::shared_ptr<OC::OCResourceResponse> response;
        if (interfaceType == OC::DEFAULT_INTERFACE)
        {
            response = requestOnBaseLineInterface(request);
        }

        // Send response if the request handled by resource
        if (response)
        {
            if (OC_STACK_OK != OC::OCPlatform::sendResponse(response))
                return OC_EH_ERROR;
        }
        else
        {
            SIM_LOG(ILogger::ERROR, "[" << m_name << "] " << "Unsupported request received!")
            return OC_EH_ERROR;
        }
    }

    if (OC::RequestHandlerFlag::ObserverFlag & request->getRequestHandlerFlag())
    {
        if (false == isObservable())
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] OBSERVE request received")
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] Sending error as resource is in unobservable state")
            return OC_EH_ERROR;
        }

        OC::ObservationInfo observationInfo = request->getObservationInfo();
        if (OC::ObserveAction::ObserveRegister == observationInfo.action)
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] OBSERVE REGISTER request received");

            ObserverInfo info {observationInfo.obsId, observationInfo.address, observationInfo.port};
            m_observersList.push_back(info);

            if (m_observeCallback)
                m_observeCallback(m_uri, ObservationStatus::REGISTER, info);
        }
        else if (OC::ObserveAction::ObserveUnregister == observationInfo.action)
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] OBSERVE UNREGISTER request received");

            ObserverInfo info;
            for (auto iter = m_observersList.begin(); iter != m_observersList.end(); iter++)
            {
                if ((info = *iter), info.id == observationInfo.obsId)
                {
                    m_observersList.erase(iter);
                    break;
                }
            }

            if (m_observeCallback)
                m_observeCallback(m_uri, ObservationStatus::UNREGISTER, info);
        }
        errCode = OC_EH_OK;
    }

    return errCode;
}

std::shared_ptr<OC::OCResourceResponse> SimulatorSingleResourceImpl::requestOnBaseLineInterface(
    std::shared_ptr<OC::OCResourceRequest> request)
{
    std::shared_ptr<OC::OCResourceResponse> response;
    if ("GET" == request->getRequestType())
    {
        response = std::make_shared<OC::OCResourceResponse>();
        response->setErrorCode(200);
        response->setResponseResult(OC_EH_OK);
        response->setResourceRepresentation(m_resModel.getOCRepresentation());
        std::string resPayload = getPayloadString(m_resModel.getOCRepresentation());
        SIM_LOG(ILogger::INFO, "[" << m_uri <<
                "] Sending response for GET request. \n**Payload details**" << resPayload)
    }
    else if ("PUT" == request->getRequestType()
             || "POST" == request->getRequestType())
    {
        OC::OCRepresentation requestRep = request->getResourceRepresentation();
        if (true == m_resModel.update(requestRep))
        {
            notifyAll();
            notifyApp();

            response = std::make_shared<OC::OCResourceResponse>();
            response->setErrorCode(200);
            response->setResponseResult(OC_EH_OK);
            response->setResourceRepresentation(m_resModel.getOCRepresentation());
            std::string resPayload = getPayloadString(m_resModel.getOCRepresentation());
            SIM_LOG(ILogger::INFO, "[" << m_uri <<
                    "] Sending response for " << request->getRequestType() << " request. \n**Payload details**" <<
                    resPayload)
        }
        else
        {
            response = std::make_shared<OC::OCResourceResponse>();
            response->setErrorCode(400);
            response->setResponseResult(OC_EH_ERROR);
        }
    }
    else if ("DELETE" == request->getRequestType())
    {
        // TODO: Handle this request
    }

    if (response)
    {
        response->setRequestHandle(request->getRequestHandle());
        response->setResourceHandle(request->getResourceHandle());
    }

    return response;
}
