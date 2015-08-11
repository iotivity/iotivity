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

#include "simulator_resource_server.h"
#include "simulator_attribute_automation.h"
#include "simulator_logger.h"

SimulatorResourceServer::SimulatorResourceServer()
    : m_resourceHandle(NULL),
      m_property((OCResourceProperty) (OC_DISCOVERABLE | OC_OBSERVABLE)) {}

SimulatorResult SimulatorResourceServer::setURI(const std::string &uri)
{
    if (m_resourceHandle)
        return SIMULATOR_OPERATION_NOT_ALLOWED;
    m_uri = uri;
    return SIMULATOR_SUCCESS;
}

std::string SimulatorResourceServer::getURI() const
{
    return m_uri;
}

SimulatorResult SimulatorResourceServer::setResourceType(const std::string &resourceType)
{
    if (m_resourceHandle)
        return SIMULATOR_OPERATION_NOT_ALLOWED;
    m_resourceType = resourceType;
    return SIMULATOR_SUCCESS;
}

std::string SimulatorResourceServer::getResourceType() const
{
    return m_resourceType;
}

SimulatorResult SimulatorResourceServer::setInterfaceType(const std::string &interfaceType)
{
    if (m_resourceHandle)
        return SIMULATOR_OPERATION_NOT_ALLOWED;
    m_interfaceType = interfaceType;
    return SIMULATOR_SUCCESS;
}

std::string SimulatorResourceServer::getInterfaceType() const
{
    return m_interfaceType;
}

void SimulatorResourceServer::setName(const std::string &name)
{
    m_name = name;
}

std::string SimulatorResourceServer::getName() const
{
    return m_name;
}

SimulatorResult SimulatorResourceServer::setObservable(bool state)
{
    if (m_resourceHandle)
        return SIMULATOR_OPERATION_NOT_ALLOWED;

    if (true == state)
        m_property = static_cast<OCResourceProperty>(m_property | OC_OBSERVABLE);
    else
        m_property = static_cast<OCResourceProperty>(m_property ^ OC_OBSERVABLE);

    return SIMULATOR_SUCCESS;
}

bool SimulatorResourceServer::isObservable() const
{
    return (m_property & OC_OBSERVABLE);
}

SimulatorResult SimulatorResourceServer::start()
{
    if (m_resourceHandle)
        return SIMULATOR_RESOURCE_ALREADY_REGISTERED;

    if (m_uri.empty() || m_resourceType.empty() || m_interfaceType.empty()
        || !m_callback)
    {
        return SIMULATOR_BAD_INPUT;
    }

    OCStackResult result = OC::OCPlatform::registerResource(m_resourceHandle,
                           m_uri,
                           m_resourceType,
                           m_interfaceType,
                           std::bind(&SimulatorResourceServer::entityHandler,
                                     this, std::placeholders::_1), m_property);

    if (OC_STACK_OK != result)
        return SIMULATOR_ERROR;
    return SIMULATOR_SUCCESS;
}

SimulatorResult SimulatorResourceServer::stop()
{
    if (nullptr == m_resourceHandle)
    {
        SIM_LOG(ILogger::ERROR, "Resourece is not registered!");
        return SIMULATOR_RESOURCE_NOT_REGISTERED;
    }

    OCStackResult result = OC::OCPlatform::unregisterResource(m_resourceHandle);
    if (OC_STACK_OK != result)
        return SIMULATOR_ERROR;

    m_resourceHandle = nullptr;
    return SIMULATOR_SUCCESS;
}

void SimulatorResourceServer::setRange(const std::string &attrName, const int min, const int max)
{
    m_resModel.setRange(attrName, min, max);
}

void SimulatorResourceServer::setUpdateInterval(const std::string &attrName, int interval)
{
    m_resModel.setUpdateInterval(attrName, interval);
}

void SimulatorResourceServer::removeAttribute(const std::string &attrName)
{
    m_resModel.removeAttribute(attrName);
    notifyListOfObservers();
}

void SimulatorResourceServer::updateAttributeFromAllowedValues(const std::string &attrName,
        const int allowedValueIndex)
{
    m_resModel.updateAttributeFromAllowedValues(attrName, allowedValueIndex);
    notifyListOfObservers();
}

SimulatorResourceModel SimulatorResourceServer::getModel() const
{
    return m_resModel;
}

void SimulatorResourceServer::setModelChangeCallback(ResourceModelChangedCB callback)
{
    m_callback = callback;
}

SimulatorResult SimulatorResourceServer::startUpdateAutomation(AutomationType type,
        updateCompleteCallback callback, int &id)
{
    return m_updateAutomationMgr.startResourceAutomation(this, id, callback, type);
}

SimulatorResult SimulatorResourceServer::startUpdateAutomation(const std::string &attrName,
        AutomationType type, updateCompleteCallback callback, int &id)
{
    return m_updateAutomationMgr.startAttributeAutomation(this, attrName, id, callback, type);
}

std::vector<int> SimulatorResourceServer::getResourceAutomationIds()
{
    return m_updateAutomationMgr.getResourceAutomationIds();
}

std::vector<int> SimulatorResourceServer::getAttributeAutomationIds()
{
    return m_updateAutomationMgr.getAttributeAutomationIds();
}

void SimulatorResourceServer::stopUpdateAutomation(const int id)
{
    m_updateAutomationMgr.stop(id);
}

OC::OCRepresentation SimulatorResourceServer::getOCRepresentation()
{
    return m_resModel.getOCRepresentation();
}

bool SimulatorResourceServer::modifyResourceModel(OC::OCRepresentation &ocRep)
{
    bool status = m_resModel.update(ocRep);
    if (true == status)
    {
        notifyListOfObservers();
    }
    return status;
}

OCEntityHandlerResult SimulatorResourceServer::entityHandler(std::shared_ptr<OC::OCResourceRequest>
        request)
{
    OCEntityHandlerResult errCode = OC_EH_ERROR;
    if (!request)
    {
        return OC_EH_ERROR;
    }

    if (OC::RequestHandlerFlag::RequestFlag & request->getRequestHandlerFlag())
    {
        auto response = std::make_shared<OC::OCResourceResponse>();
        response->setRequestHandle(request->getRequestHandle());
        response->setResourceHandle(request->getResourceHandle());

        if ("GET" == request->getRequestType())
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] GET request received");
            response->setErrorCode(200);
            response->setResponseResult(OC_EH_OK);
            response->setResourceRepresentation(getOCRepresentation());

            if (OC_STACK_OK == OC::OCPlatform::sendResponse(response))
            {
                errCode = OC_EH_OK;
            }
        }
        else if ("PUT" == request->getRequestType())
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] PUT request received");
            OC::OCRepresentation rep = request->getResourceRepresentation();
            if (true == modifyResourceModel(rep))
            {
                response->setErrorCode(200);
                response->setResponseResult(OC_EH_OK);
                response->setResourceRepresentation(getOCRepresentation());
            }
            else
            {
                response->setErrorCode(400);
                response->setResponseResult(OC_EH_ERROR);
            }

            if (OC_STACK_OK == OC::OCPlatform::sendResponse(response))
            {
                errCode = OC_EH_OK;
            }
        }
        else if ("POST" == request->getRequestType())
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] POST request received");
            OC::OCRepresentation rep = request->getResourceRepresentation();
            if (true == modifyResourceModel(rep))
            {
                response->setErrorCode(200);
                response->setResponseResult(OC_EH_OK);
                response->setResourceRepresentation(getOCRepresentation());
            }
            else
            {
                response->setErrorCode(400);
                response->setResponseResult(OC_EH_ERROR);
            }

            if (OC_STACK_OK == OC::OCPlatform::sendResponse(response))
            {
                errCode = OC_EH_OK;
            }
        }
        else if ("DELETE" == request->getRequestType())
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] DELETE request received!");
            OC::OCRepresentation rep = request->getResourceRepresentation();

            // DELETE request handling not supported right now
            response->setErrorCode(400);
            response->setResponseResult(OC_EH_ERROR);
            if (OC_STACK_OK == OC::OCPlatform::sendResponse(response))
            {
                errCode = OC_EH_OK;
            }
        }
        else
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] UNKNOWN type request received");
            response->setResponseResult(OC_EH_ERROR);
            OC::OCPlatform::sendResponse(response);
            errCode = OC_EH_ERROR;
        }
    }

    if (OC::RequestHandlerFlag::ObserverFlag & request->getRequestHandlerFlag())
    {
        if (false == isObservable())
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] OBSERVE request received");
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] Sending error as resource is in unobservable state");
            return OC_EH_ERROR;
        }

        OC::ObservationInfo observationInfo = request->getObservationInfo();
        if (OC::ObserveAction::ObserveRegister == observationInfo.action)
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] OBSERVE REGISTER request received");
            m_interestedObservers.push_back(observationInfo.obsId);
        }
        else if (OC::ObserveAction::ObserveUnregister == observationInfo.action)
        {
            SIM_LOG(ILogger::INFO, "[" << m_uri << "] OBSERVE UNREGISTER request received");
            m_interestedObservers.erase(std::remove(m_interestedObservers.begin(),
                                                    m_interestedObservers.end(),
                                                    observationInfo.obsId),
                                        m_interestedObservers.end());
        }
        errCode = OC_EH_OK;
    }

    return errCode;
}

void SimulatorResourceServer::notifyListOfObservers ()
{
    if (!m_resourceHandle)
    {
        return;
    }

    if (m_interestedObservers.size())
    {
        std::shared_ptr<OC::OCResourceResponse> resourceResponse =
        {std::make_shared<OC::OCResourceResponse>()};

        resourceResponse->setErrorCode(200);
        resourceResponse->setResponseResult(OC_EH_OK);
        resourceResponse->setResourceRepresentation(getOCRepresentation(), OC::DEFAULT_INTERFACE);

        SIM_LOG(ILogger::INFO, "[" << m_uri << "] Sending notification to all observers" << m_uri);
        OC::OCPlatform::notifyListOfObservers(m_resourceHandle, m_interestedObservers, resourceResponse);
    }

    if (m_callback)
    {
        m_callback(m_uri, m_resModel);
    }
}
