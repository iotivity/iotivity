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

#include "simulator_client.h"
#include "simulator_logger.h"

SimulatorClient *SimulatorClient::getInstance()
{
    static SimulatorClient s_instance;
    return &s_instance;
}

SimulatorResult SimulatorClient::findResource(const std::string &resourceType,
        ResourceFindCallback callback)
{
    // Construct the request query for discovery
    std::string query = OC_MULTICAST_DISCOVERY_URI;
    query.append("?rt=");
    query.append(resourceType);

    OCStackResult result = OC::OCPlatform::findResource("", query,
                           CT_DEFAULT,
                           std::bind(&SimulatorClient::onResourceFound, this,
                                     std::placeholders::_1, callback));

    if (OC_STACK_OK != result)
    {
        SIM_LOG(ILogger::ERROR, "Failed to find the resource!");
        return SIMULATOR_ERROR;
    }

    return SIMULATOR_SUCCESS;
}

std::vector<SimulatorRemoteResourcePtr> SimulatorClient::getFoundResources(
    const std::string &resourceType)
{
    std::lock_guard<std::recursive_mutex> lock(m_resourceListMutex);

    std::map<std::string, SimulatorRemoteResourcePtr> tempList;
    for (auto resourceTableEntry : m_resourceList)
    {
        if (resourceType.compare(resourceTableEntry.first))
        {
            tempList = resourceTableEntry.second;
            break;
        }
        else
        {
            for (auto resourceEntry : resourceTableEntry.second)
            {
                tempList[resourceEntry.first] = resourceEntry.second;
            }
        }
    }

    // Convert map to vector
    std::vector<SimulatorRemoteResourcePtr> resourceList;
    for (auto & resource : tempList)
        resourceList.push_back(resource.second);

    return resourceList;
}

void SimulatorClient::onResourceFound(std::shared_ptr<OC::OCResource> resource,
                                      ResourceFindCallback callback)
{
    if (nullptr == resource)
    {
        return;
    }

    std::string serverId = resource->sid();

    // Construct SimulatorRemoteResource
    SimulatorRemoteResourcePtr simulatorResource(new SimulatorRemoteResource(resource));

    // Add the resource to list
    for (auto & resourceType : resource->getResourceTypes())
        addResourceToList(resourceType, serverId, simulatorResource);

    callback(simulatorResource);
}

void SimulatorClient::addResourceToList(const std::string &resourceType, const std::string &sid,
                                        SimulatorRemoteResourcePtr &resource)
{
    std::lock_guard<std::recursive_mutex> lock(m_resourceListMutex);

    if (!resourceType.empty() && !sid.empty() && nullptr != resource)
    {
        auto resourceTableEntry = m_resourceList.find(resourceType);
        if (m_resourceList.end() != resourceTableEntry)
        {
            auto resourceEntry = resourceTableEntry->second.find(sid);
            if (resourceTableEntry->second.end() != resourceEntry)
            {
                return;
            }
        }

        m_resourceList[resourceType].insert(std::pair<std::string, SimulatorRemoteResourcePtr>(sid,
                                            resource));
    }
}

