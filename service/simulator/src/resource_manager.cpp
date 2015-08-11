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
#include "simulator_logger.h"

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

SimulatorResourceServerPtr ResourceManager::createResource(const std::string &configPath,
        SimulatorResourceServer::ResourceModelChangedCB callback)
{
    /**
     * TODO: Temporarily creating the light resource for testing the basic flow
     * Once the config parser is included this method will simulate the resource based on the config file
     */
    SimulatorResourceServerPtr simulatorResource = m_resourceCreator->createLightResoure();
    simulatorResource->setModelChangeCallback(callback);
    std::string uri = getURI(simulatorResource->getURI());
    if(uri.empty())
    {
        SIM_LOG(ILogger::ERROR, "Cannot register resource. Resource URI is empty");
        return NULL;
    }
    simulatorResource->setURI(uri);
    SimulatorResult result = simulatorResource->start();
    if (SIMULATOR_SUCCESS != result)
    {
        SIM_LOG(ILogger::ERROR, "Failed to register resource [" << simulatorResource->getURI() <<
                "] with platform");
        return NULL;
    }

    // Add the resource to resource list table
    std::lock_guard<std::recursive_mutex> lock(m_listMutex);
    m_resourceList[simulatorResource->getResourceType()].insert(
        std::pair<std::string, SimulatorResourceServerPtr>(simulatorResource->getURI(), simulatorResource));
    return simulatorResource;
}

std::vector<SimulatorResourceServerPtr> ResourceManager::createResource(
    const std::string &configPath,
    const int count, SimulatorResourceServer::ResourceModelChangedCB callback)
{
    std::vector<SimulatorResourceServerPtr> resourceList;
    for (int i = 0; i < count; i++)
    {
        /**
         * TODO: Temporarily creating the light resource for testing the basic flow
         * Once the config parser is included this method will simulate the resource based on the config file
         */
        SimulatorResourceServerPtr simulatorResource = m_resourceCreator->createLightResoure();
        simulatorResource->setModelChangeCallback(callback);
        std::string uri = getURI(simulatorResource->getURI());
        if(uri.empty())
        {
            SIM_LOG(ILogger::ERROR, "Cannot register resource. Resource URI is empty");
            break;
        }
        simulatorResource->setURI(uri);
        SimulatorResult result = simulatorResource->start();
        if (SIMULATOR_SUCCESS != result)
        {
            resourceList.clear();
            SIM_LOG(ILogger::ERROR, "Failed to register resources!");
            break;
        }
        else
        {
            resourceList.push_back(simulatorResource);
        }
    }

    // Add the resource to resource list table
    std::lock_guard<std::recursive_mutex> lock(m_listMutex);
    for (auto & resource : resourceList)
    {
        m_resourceList[resource->getResourceType()].insert(
            std::pair<std::string, SimulatorResourceServerPtr>(resource->getURI(), resource));
    }

    return resourceList;
}

std::vector<SimulatorResourceServerPtr> ResourceManager::getResources(
    const std::string &resourceType)
{
    std::lock_guard<std::recursive_mutex> lock(m_listMutex);

    std::vector<SimulatorResourceServerPtr> resourceList;
    for (auto resourceTableEntry : m_resourceList)
    {
        if (!resourceType.empty() && resourceType.compare(resourceTableEntry.first))
            continue;

        for (auto resourceEntry : resourceTableEntry.second)
        {
            resourceList.push_back(resourceEntry.second);
        }
    }

    return resourceList;
}

SimulatorResult ResourceManager::deleteResource(SimulatorResourceServerPtr &resource)
{
    std::lock_guard<std::recursive_mutex> lock(m_listMutex);

    if (nullptr == resource)
        return SIMULATOR_BAD_INPUT;

    SimulatorResult result = SIMULATOR_RESOURCE_NOT_FOUND;

    try
    {
        auto resourceTableEntry = m_resourceList.find(resource->getResourceType());
        if (m_resourceList.end() != resourceTableEntry)
        {
            auto resourceEntry = resourceTableEntry->second.find(resource->getURI());
            if (resourceTableEntry->second.end() != resourceEntry)
            {
                if (SIMULATOR_SUCCESS == resource->stop())
                {
                    resourceTableEntry->second.erase(resourceEntry);
                    result = SIMULATOR_SUCCESS;
                }
                else
                {
                    result = SIMULATOR_ERROR;
                }
            }
        }
    }
    catch (OC::OCException &except)
    {
        SIM_LOG(ILogger::ERROR, except.reason() << except.code())
        result = SIMULATOR_ERROR;
    }

    return result;
}

SimulatorResult ResourceManager::deleteResources(const std::string &resourceType)
{
    std::lock_guard<std::recursive_mutex> lock(m_listMutex);

    SimulatorResult result = SIMULATOR_RESOURCE_NOT_FOUND;
    try
    {
        for (auto & resourceTableEntry : m_resourceList)
        {
            if (!resourceType.empty() && resourceType.compare(resourceTableEntry.first))
                continue;

            for (auto & resourceEntry : resourceTableEntry.second)
            {
                SimulatorResourceServerPtr resource = resourceEntry.second;
                if (SIMULATOR_SUCCESS == resource->stop())
                {
                    resourceTableEntry.second.erase(resourceTableEntry.second.find(resource->getURI()));
                    result = SIMULATOR_SUCCESS;
                }
                else
                {
                    return SIMULATOR_ERROR;
                }
            }
        }
    }
    catch (OC::OCException &except)
    {
        SIM_LOG(ILogger::ERROR, except.reason() << except.code())
        result = SIMULATOR_ERROR;
    }

    return result;
}

std::string ResourceManager::getURI(std::string uri)
{
    if(uri.empty())
    {
        return uri;
    }
    std::ostringstream os;
    os << uri;
    if ('/' != uri[uri.length() - 1])
        os << '/';
    os << "simulator/" << id++;
    return os.str();
}

