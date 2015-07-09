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

#include "resource_manager.h"

int ResourceManager::id;
ResourceManager *ResourceManager::getInstance()
{
    static ResourceManager s_instance;
    return &s_instance;
}

ResourceManager::ResourceManager()
    : m_resourceCreator(new SimulatorResourceCreator()) {}

ResourceManager::~ResourceManager()
{
    delete m_resourceCreator;
}

SimulatorResourcePtr ResourceManager::createResource(const std::string &configPath,
        SimulatorResource::ResourceModelChangedCB callback)
{
    /**
     * TODO: Temporarily creating the light resource for testing the basic flow
     * Once the config parser is included this method will simulate the resource based on the config file
     */
    SimulatorResourcePtr simulatorResource = m_resourceCreator->createLightResoure();
    simulatorResource->setModelChangeCallback(callback);

    simulatorResource->setURI(getURI(simulatorResource->getURI()));
    OC::EntityHandler entityHandler = std::bind(&SimulatorResource::entityHandler,
                                      simulatorResource.get(), std::placeholders::_1);
    std::string uri = simulatorResource->getURI();
    OCStackResult result = OC::OCPlatform::registerResource(simulatorResource->m_resourceHandle,
                           uri,
                           simulatorResource->getResourceType(),
                           simulatorResource->getInterfaceType(),
                           entityHandler,
                           OC_DISCOVERABLE | OC_OBSERVABLE);
    if (OC_STACK_OK != result)
    {
        return NULL;
    }

    // Add the resource to resource list table
    m_resourceList[simulatorResource->getResourceType()].insert(
        std::pair<std::string, SimulatorResourcePtr>(simulatorResource->getURI(), simulatorResource));
    return simulatorResource;
}

std::vector<SimulatorResourcePtr> ResourceManager::createResource(const std::string &configPath,
        const int count, SimulatorResource::ResourceModelChangedCB callback)
{
    std::vector<SimulatorResourcePtr> list;
    return list;
}

std::vector<SimulatorResourcePtr> ResourceManager::getResources(void) const
{
    std::vector<SimulatorResourcePtr> list;
    return list;
}

SimulatorResult ResourceManager::deleteResource(SimulatorResourcePtr &resource)
{
    if (!resource.get())
        return SIMULATOR_RESOURCE_NOT_FOUND;

    auto resourceTableEntry = m_resourceList.find(resource->getResourceType());
    if (m_resourceList.end() != resourceTableEntry)
    {
        auto resourceEntry = resourceTableEntry->second.find(resource->getURI());
        if (resourceTableEntry->second.end() != resourceEntry)
        {
            if (OC_STACK_OK == OC::OCPlatform::unregisterResource(resource->getHandle()))
                resourceTableEntry->second.erase(resourceEntry);
        }
    }

    return SIMULATOR_SUCCESS;
}

SimulatorResult ResourceManager::deleteResources(const std::string &resourceType)
{
    auto resourceTableEntry = m_resourceList.find(resourceType);
    if (m_resourceList.end() != resourceTableEntry)
    {
        return SIMULATOR_RESOURCE_NOT_FOUND;
    }

    for (auto resourceEntry : resourceTableEntry->second)
    {
        SimulatorResourcePtr resource = resourceEntry.second;
        if (OC_STACK_OK == OC::OCPlatform::unregisterResource(resource->getHandle()))
            resourceTableEntry->second.erase(resourceTableEntry->second.find(resource->getURI()));
    }

    return SIMULATOR_SUCCESS;
}

std::string ResourceManager::getURI(std::string uri)
{
    std::ostringstream os;
    os << uri;
    if (!uri.empty() && '/' != uri[uri.length() - 1])
        os << '/';
    os << "simulator/" << id++;
    return os.str();
}

