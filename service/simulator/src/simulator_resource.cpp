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

#include "simulator_resource.h"
#include "simulator_attribute_automation.h"

void SimulatorResource::setRange(const std::string &attrName, const int min, const int max)
{
    m_resModel.setRange(attrName, min, max);
}

void SimulatorResource::setUpdateInterval(const std::string &attrName, int interval)
{
    m_resModel.setUpdateInterval(attrName, interval);
}

void SimulatorResource::removeAttribute(const std::string &attrName)
{
    m_resModel.removeAttribute(attrName);
    notifyListOfObservers();
}

void SimulatorResource::setURI(const std::string &uri)
{
    m_uri = uri;
}

std::string SimulatorResource::getURI() const
{
    return m_uri;
}

void SimulatorResource::setResourceType(const std::string &resourceType)
{
    m_resourceType = resourceType;
}

std::string SimulatorResource::getResourceType() const
{
    return m_resourceType;
}

void SimulatorResource::setInterfaceType(const std::string &interfaceType)
{
    m_interfaceType = interfaceType;
}

std::string SimulatorResource::getInterfaceType() const
{
    return m_interfaceType;
}

OCResourceHandle SimulatorResource::getHandle() const
{
    return m_resourceHandle;
}

void SimulatorResource::setName(const std::string &name)
{
    m_name = name;
}

std::string SimulatorResource::getName() const
{
    return m_name;
}

void SimulatorResource::updateAttributeFromAllowedValues(const std::string &attrName,
        const int allowedValueIndex)
{
    m_resModel.updateAttributeFromAllowedValues(attrName, allowedValueIndex);
}

SimulatorResourceModel SimulatorResource::getModel() const
{
    return m_resModel;
}

void SimulatorResource::setModelChangeCallback(ResourceModelChangedCB callback)
{
    m_callback = callback;
}

SimulatorResult SimulatorResource::startUpdateAutomation(AutomationType type, int &id)
{
    return m_updateAutomationMgr.startResourceAutomation(this, id, type);
}

SimulatorResult SimulatorResource::startUpdateAutomation(const std::string &attrName,
        AutomationType type, int &id)
{
    return m_updateAutomationMgr.startAttributeAutomation(this, attrName, id, type);
}

void SimulatorResource::stopUpdateAutomation(const int id)
{
    m_updateAutomationMgr.stop(id);
}

OC::OCRepresentation SimulatorResource::getOCRepresentation()
{
    return m_resModel.getOCRepresentation();
}

bool SimulatorResource::modifyResourceModel(OC::OCRepresentation &ocRep,
        SimulatorResourceModel::UpdateType type)
{
    return m_resModel.update(ocRep, type);
}

OCEntityHandlerResult SimulatorResource::entityHandler(std::shared_ptr<OC::OCResourceRequest>
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
            OC::OCRepresentation rep = request->getResourceRepresentation();
            if (true == modifyResourceModel(rep, SimulatorResourceModel::UpdateType::PUT))
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
            OC::OCRepresentation rep = request->getResourceRepresentation();
            if (true == modifyResourceModel(rep, SimulatorResourceModel::UpdateType::POST))
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
            OC::OCRepresentation rep = request->getResourceRepresentation();
            if (true == modifyResourceModel(rep, SimulatorResourceModel::UpdateType::DELETE))
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
        else
        {
            response->setResponseResult(OC_EH_ERROR);
            OC::OCPlatform::sendResponse(response);
            errCode = OC_EH_ERROR;
        }
    }
    else if (OC::RequestHandlerFlag::ObserverFlag & request->getRequestHandlerFlag())
    {
        OC::ObservationInfo observationInfo = request->getObservationInfo();
        if (OC::ObserveAction::ObserveRegister == observationInfo.action)
        {
            m_interestedObservers.push_back(observationInfo.obsId);
        }
        else if (OC::ObserveAction::ObserveUnregister == observationInfo.action)
        {
            m_interestedObservers.erase(std::remove(m_interestedObservers.begin(),
                                                    m_interestedObservers.end(),
                                                    observationInfo.obsId),
                                        m_interestedObservers.end());
        }
        errCode = OC_EH_OK;
    }

    return errCode;
}

void SimulatorResource::notifyListOfObservers ()
{
    if (0 == m_interestedObservers.size())
        return;

    std::shared_ptr<OC::OCResourceResponse> resourceResponse =
    {std::make_shared<OC::OCResourceResponse>()};

    resourceResponse->setErrorCode(200);
    resourceResponse->setResponseResult(OC_EH_OK);
    resourceResponse->setResourceRepresentation(getOCRepresentation(), OC::DEFAULT_INTERFACE);

    OC::OCPlatform::notifyListOfObservers(getHandle(), m_interestedObservers, resourceResponse);
}
