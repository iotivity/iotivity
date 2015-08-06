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

#include "simulator_manager.h"
#include "resource_manager.h"
#include "simulator_client.h"

using namespace OC;

SimulatorManager *SimulatorManager::getInstance()
{
    static SimulatorManager s_instance;
    return &s_instance;
}

SimulatorManager::SimulatorManager()
{
    OC::PlatformConfig conf
    {
        OC::ServiceType::InProc,
        OC::ModeType::Both,
        "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
        0,         // Uses randomly available port
        OC::QualityOfService::LowQos
    };

    OC::OCPlatform::Configure(conf);
}

SimulatorResourcePtr SimulatorManager::createResource(const std::string &configPath,
        SimulatorResource::ResourceModelChangedCB callback)
{
    return ResourceManager::getInstance()->createResource(configPath, callback);
}

std::vector<SimulatorResourcePtr> SimulatorManager::createResource(const std::string &configPath,
        const int count, SimulatorResource::ResourceModelChangedCB callback)
{
    return ResourceManager::getInstance()->createResource(configPath, count, callback);
}

std::vector<SimulatorResourcePtr> SimulatorManager::getResources(void)
{
    return ResourceManager::getInstance()->getResources();
}

SimulatorResult SimulatorManager::deleteResource(SimulatorResourcePtr &resource)
{
    return ResourceManager::getInstance()->deleteResource(resource);
}

SimulatorResult SimulatorManager::deleteResources(const std::string &resourceType)
{
    return ResourceManager::getInstance()->deleteResources(resourceType);
}

SimulatorResult SimulatorManager::findResource(const std::string &resourceType,
        ResourceFindCallback callback)
{
    return SimulatorClient::getInstance()->findResource(resourceType, callback);
}

std::vector<SimulatorRemoteResourcePtr> SimulatorManager::getFoundResources(
    const std::string resourceType)
{
    return SimulatorClient::getInstance()->getFoundResources(resourceType);
}

void SimulatorManager::setLogger(std::shared_ptr<ILogger> logger)
{
    simLogger().setCustomTarget(logger);
}

bool SimulatorManager::setDefaultConsoleLogger()
{
    return simLogger().setDefaultConsoleTarget();
}

bool SimulatorManager::setDefaultFileLogger(std::string &path)
{
    return simLogger().setDefaultFileTarget(path);
}