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

