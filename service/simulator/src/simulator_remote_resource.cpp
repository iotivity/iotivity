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

#include "simulator_remote_resource.h"
#include "OCApi.h"

SimulatorRemoteResource::SimulatorRemoteResource(std::shared_ptr<OC::OCResource> resource)
    : m_ocResource(resource) {}

std::string SimulatorRemoteResource::getURI() const
{
    return m_ocResource->uri();
}

std::string SimulatorRemoteResource::getHost() const
{
    return m_ocResource->host();
}

std::string SimulatorRemoteResource::getSID() const
{
    return m_ocResource->sid();
}

OCConnectivityType SimulatorRemoteResource::getConnectivityType() const
{
    return m_ocResource->connectivityType();
}

std::vector < std::string > SimulatorRemoteResource::getResourceTypes() const
{
    return m_ocResource->getResourceTypes();
}

std::vector < std::string > SimulatorRemoteResource::getResourceInterfaces() const
{
    return m_ocResource->getResourceInterfaces();
}

bool SimulatorRemoteResource::isObservable() const
{
    return m_ocResource->isObservable();
}

SimulatorResult SimulatorRemoteResource::observe(SimulatorRemoteResource::ObserveType type,
        const QueryParamsMap &queryParams, RepresentationChangeCallback callback)
{
    std::lock_guard<std::mutex> lock(m_observeMutex);

    static int observeState = false;
    if (!observeState)
    {
        OC::ObserveCallback observeCallback = [callback](const OC::HeaderOptions & headerOptions,
                                              const OC::OCRepresentation & rep, const int errorCode,
                                              const int sequenceNum)
        {
            // Convert OCRepresentation to SimulatorResourceModel
            SimulatorResourceModel resModel = SimulatorResourceModel::create(rep);
            callback(errorCode, resModel, sequenceNum);
        };

        OC::ObserveType observeType =
            (type == SimulatorRemoteResource::OBSERVE_ALL) ? OC::ObserveType::ObserveAll :
            OC::ObserveType::Observe;
        OCStackResult error = m_ocResource->observe(observeType, queryParams, observeCallback);
        if (OC_STACK_OK == error)
        {
            observeState = true;
            return SIMULATOR_SUCCESS;
        }
        else
        {
            return SIMULATOR_ERROR;
        }
    }

    return SIMULATOR_RESOURCE_ALREADY_OBSERVING;
}

SimulatorResult SimulatorRemoteResource::cancelObserve()
{
    OCStackResult error = m_ocResource->cancelObserve();
    if (OC_STACK_OK != error)
        return SIMULATOR_ERROR;
    return SIMULATOR_SUCCESS;
}

SimulatorResult SimulatorRemoteResource::get(const std::string &resourceType,
        const std::string &interfaceType,
        const QueryParamsMap &queryParams, ResponseCallback callback)
{
    OC::GetCallback getCallback = [callback](const OC::HeaderOptions & headerOptions,
                                  const OC::OCRepresentation & rep, const int errorCode)
    {
        // Convert OCRepresentation to SimulatorResourceModel
        SimulatorResourceModel resModel = SimulatorResourceModel::create(rep);
        callback(errorCode, resModel);
    };

    // Convert SimulatorResourceModel to OcRepresentation
    OCStackResult result = m_ocResource->get(resourceType, interfaceType, queryParams,
                           getCallback);
    if (OC_STACK_OK != result)
    {
        return SIMULATOR_ERROR;
    }

    return SIMULATOR_SUCCESS;
}

SimulatorResult SimulatorRemoteResource::get(const QueryParamsMap &queryParams,
        ResponseCallback callback)
{
    std::string resourceType;
    std::string interfaceType;
    return get(resourceType, interfaceType, queryParams, callback);
}

SimulatorResult SimulatorRemoteResource::put(const std::string &resourceType,
        const std::string &interfaceType,
        const SimulatorResourceModel &representation,
        const QueryParamsMap &queryParams, ResponseCallback callback)
{
    OC::PutCallback putCallback = [callback](const OC::HeaderOptions & headerOptions,
                                  const OC::OCRepresentation & rep, const int errorCode)
    {
        // Convert OCRepresentation to SimulatorResourceModel
        SimulatorResourceModel resModel = SimulatorResourceModel::create(rep);
        callback(errorCode, resModel);
    };

    // Convert SimulatorResourceModel to OcRepresentation
    OC::OCRepresentation ocRep = representation.getOCRepresentation();
    OCStackResult result = m_ocResource->put(resourceType, interfaceType, ocRep, queryParams,
                           putCallback);
    if (OC_STACK_OK != result)
    {
        return SIMULATOR_ERROR;
    }

    return SIMULATOR_SUCCESS;
}

SimulatorResult SimulatorRemoteResource::put(const SimulatorResourceModel &representation,
        const QueryParamsMap &queryParams, ResponseCallback callback)
{
    std::string resourceType;
    std::string interfaceType;
    return put(resourceType, interfaceType, representation, queryParams, callback);
}

SimulatorResult SimulatorRemoteResource::post(const std::string &resourceType,
        const std::string &interfaceType,
        const SimulatorResourceModel &representation,
        const QueryParamsMap &queryParams, ResponseCallback callback)
{
    OC::PostCallback postCallback = [callback](const OC::HeaderOptions & headerOptions,
                                    const OC::OCRepresentation & rep, const int errorCode)
    {
        // Convert OCRepresentation to SimulatorResourceModel
        SimulatorResourceModel resModel = SimulatorResourceModel::create(rep);
        callback(errorCode, resModel);
    };

    // Convert SimulatorResourceModel to OcRepresentation
    OC::OCRepresentation ocRep = representation.getOCRepresentation();
    OCStackResult result = m_ocResource->post(resourceType, interfaceType, ocRep, queryParams,
                           postCallback);
    if (OC_STACK_OK != result)
    {
        return SIMULATOR_ERROR;
    }

    return SIMULATOR_SUCCESS;
}

SimulatorResult SimulatorRemoteResource::post(const SimulatorResourceModel &representation,
        const QueryParamsMap &queryParams, ResponseCallback callback)
{
    std::string resourceType;
    std::string interfaceType;
    return post(resourceType, interfaceType, representation, queryParams, callback);
}

