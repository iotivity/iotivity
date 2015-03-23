//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/**
 * @file
 *
 */

#include "ThingsManager.h"
#include "GroupManager.h"
#include "GroupSynchronization.h"
#include "ThingsConfiguration.h"
#include "ThingsDiagnostics.h"
#include <algorithm>
#include <thread>

using namespace OC;
namespace OIC
{

    GroupManager *g_groupManager;
    GroupSynchronization *g_groupSync = NULL;
    ThingsConfiguration *g_thingsConf = NULL;
    ThingsDiagnostics *g_thingsDiag = NULL;

    ThingsManager::ThingsManager(void)
    {
        g_groupManager = new GroupManager();
        g_groupSync = GroupSynchronization::getInstance();
        g_thingsConf = ThingsConfiguration::getInstance();
        g_thingsDiag = ThingsDiagnostics::getInstance();
        g_thingsConf->setGroupManager(g_groupManager);
        g_thingsDiag->setGroupManager(g_groupManager);
    }

    /**
     * Virtual destructor
     */
    ThingsManager::~ThingsManager(void)
    {
        delete g_groupManager;
        g_groupSync->deleteInstance();
        g_thingsConf->deleteInstance();
        g_thingsDiag->deleteInstance();
    }

    OCStackResult ThingsManager::findCandidateResources(std::vector< std::string > resourceTypes,
            std::function< void(std::vector< std::shared_ptr< OCResource > >) > callback,
            int waitsec)
    {
        OCStackResult result = g_groupManager->findCandidateResources(resourceTypes, callback,
                waitsec);

        return result;
    }

    OCStackResult ThingsManager::subscribeCollectionPresence(std::shared_ptr< OCResource > resource,
            std::function< void(std::string, OCStackResult) > callback)
    {
        OCStackResult result = g_groupManager->subscribeCollectionPresence(resource, callback);

        return result;
    }

    OCStackResult ThingsManager::bindResourceToGroup(OCResourceHandle& childHandle, std::shared_ptr< OCResource > resource, OCResourceHandle& collectionHandle)
    {
        OCStackResult result = g_groupManager->bindResourceToGroup(childHandle,resource,collectionHandle);

        return result;
    }

    OCStackResult ThingsManager::findGroup(std::vector< std::string > collectionResourceTypes,
            FindCallback callback)
    {
        OCStackResult result = g_groupSync->findGroup(collectionResourceTypes, callback);

        return result;
    }

    OCStackResult ThingsManager::createGroup(std::string collectionResourceType)
    {
        OCStackResult result = g_groupSync->createGroup(collectionResourceType);

        return result;
    }

    OCStackResult ThingsManager::joinGroup(std::string collectionResourceType,
            OCResourceHandle resourceHandle)
    {
        OCStackResult result = g_groupSync->joinGroup(collectionResourceType, resourceHandle);

        return result;
    }

    OCStackResult ThingsManager::joinGroup(const std::shared_ptr< OCResource > resource,
            OCResourceHandle resourceHandle)
    {
        OCStackResult result = g_groupSync->joinGroup(resource, resourceHandle);

        return result;
    }

    OCStackResult ThingsManager::leaveGroup(std::string collectionResourceType,
            OCResourceHandle resourceHandle)
    {
        OCStackResult result = g_groupSync->leaveGroup(collectionResourceType, resourceHandle);

        return result;
    }

    OCStackResult ThingsManager::leaveGroup(const std::shared_ptr< OCResource > resource,
                            std::string collectionResourceType,
                            OCResourceHandle resourceHandle)
    {
        return g_groupSync->leaveGroup(resource, collectionResourceType, resourceHandle);
    }

    void ThingsManager::deleteGroup(std::string collectionResourceType)
    {
        g_groupSync->deleteGroup(collectionResourceType);
    }

    std::map< std::string, OCResourceHandle > ThingsManager::getGroupList()
    {
        return g_groupSync->getGroupList();
    }

    OCStackResult ThingsManager::updateConfigurations(std::shared_ptr< OCResource > resource,
            std::map< ConfigurationName, ConfigurationValue > configurations,
            ConfigurationCallback callback)
    {
        return g_thingsConf->updateConfigurations(resource, configurations, callback);
    }
    OCStackResult ThingsManager::getConfigurations(std::shared_ptr< OCResource > resource,
            std::vector< ConfigurationName > configurations, ConfigurationCallback callback)
    {
        return g_thingsConf->getConfigurations(resource, configurations, callback);
    }
    std::string ThingsManager::getListOfSupportedConfigurationUnits()
    {
        return g_thingsConf->getListOfSupportedConfigurationUnits();
    }

    OCStackResult ThingsManager::doBootstrap(ConfigurationCallback callback)
    {
        return g_thingsConf->doBootstrap(callback);
    }

    OCStackResult ThingsManager::reboot(std::shared_ptr< OCResource > resource,
            ConfigurationCallback callback)
    {
        return g_thingsDiag->reboot(resource, callback);
    }
    OCStackResult ThingsManager::factoryReset(std::shared_ptr< OCResource > resource,
            ConfigurationCallback callback)
    {
        return g_thingsDiag->factoryReset(resource, callback);
    }

    std::string ThingsManager::getStringFromActionSet(const ActionSet *newActionSet)
    {
        return g_groupManager->getStringFromActionSet(newActionSet);
    }
    ActionSet* ThingsManager::getActionSetfromString(std::string desc)
    {
        return g_groupManager->getActionSetfromString(desc);
    }
    OCStackResult ThingsManager::addActionSet(std::shared_ptr< OCResource > resource,
            const ActionSet* newActionSet, PutCallback cb)
    {
        return g_groupManager->addActionSet(resource, newActionSet, cb);
    }
    OCStackResult ThingsManager::executeActionSet(std::shared_ptr< OCResource > resource,
            std::string actionsetName, PostCallback cb)
    {
        return g_groupManager->executeActionSet(resource, actionsetName, cb);
    }
    OCStackResult ThingsManager::executeActionSet(std::shared_ptr< OCResource > resource,
                    std::string actionsetName, long int delay, PostCallback cb)
    {
        return g_groupManager->executeActionSet(resource, actionsetName, delay, cb);
    }
    OCStackResult ThingsManager::cancelActionSet(std::shared_ptr< OCResource > resource,
                    std::string actionsetName, PostCallback cb)
    {
        return g_groupManager->cancelActionSet(resource, actionsetName, cb);
    }
    OCStackResult ThingsManager::getActionSet(std::shared_ptr< OCResource > resource,
            std::string actionsetName, GetCallback cb)
    {
        return g_groupManager->getActionSet(resource, actionsetName, cb);
    }
    OCStackResult ThingsManager::deleteActionSet(std::shared_ptr< OCResource > resource,
            std::string actionsetName, PostCallback cb)
    {
        return g_groupManager->deleteActionSet(resource, actionsetName, cb);
    }
}
