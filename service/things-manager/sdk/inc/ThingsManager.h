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

/// @file   ThingsManager.h
///
/// @brief  This file contains the declaration of  ThingsManager class
///         and its members related to ThingsManager.

#ifndef __OC_THINGSMANAGER__
#define __OC_THINGSMANAGER__

#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include "OCPlatform.h"
#include "OCApi.h"
#include "GroupManager.h"

using namespace OC;

namespace OIC
{
    /**
     * @class	ThingsManager
     * @brief	This class provides a set of functions regarding group management,
     *          synchronization of group, configuration of things, and diagnostics about things.
     *
     */
    class ThingsManager
    {
    public:
        /**
         * Constructor for ThingsManager
         */
        ThingsManager(void);

        /**
         * Virtual destructor for ThingsManager
         */
        ~ThingsManager(void);

        /**
         * API for discoverying candidate resources.
         * Callback is called  when all resource types are found.
         *
         * @param resourceTypes - required resource types(called "candidate")
         * @param candidateCallback - callback. Returns OCResource vector.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
                std::function< void(std::vector< std::shared_ptr< OCResource > >) > callback,
                int waitsec = -1);

        /**
         * API for subscribing child's state.
         *
         * @param resource - collection resource for subscribing presence of all child resources.
         * @param callback - callback funcion for result of child's presence.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult subscribeCollectionPresence(std::shared_ptr< OCResource > resource,
                std::function< void(std::string, OCStackResult) > callback);

        /**
         * API for register and bind resource to group.
         *
         * @param childHandle - child resource handle. It will be filled from resource param.
         * @param resource - resource for register and bind to group. It has all data.
         * @param collectionHandle - collection resource handle. It will be added child resource.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult bindResourceToGroup(OCResourceHandle& childHandle,
                std::shared_ptr< OCResource > resource, OCResourceHandle& collectionHandle);

        // Group Synchronization

        /**
         * API for finding a specific remote group when a resource tries to join a group.
         * Callback is called when a group is found or not.
         *
         * @param collectionResourceTypes - resource types of a group to find and join
         * @param callback - callback. It has OCResource param.
         *                    If a group is found, OCResource has the group resource.
         *                    Otherwise, OCResource is NULL.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult findGroup(std::vector< std::string > collectionResourceTypes,
                FindCallback callback);

        /**
         * API for creating a new group.
         *
         * @param collectionResourceType - resource type of a group to create
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult createGroup(std::string collectionResourceType);

        /**
         * API for joining a group. This API is used when a resource that has a group tries
         * to find a specific remote resource and makes it join a group
         *
         * @param collectionResourceType - resource type of a group to join.
         * @param resourceHandle - resource handle to join a group.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult joinGroup(std::string collectionResourceType,
                OCResourceHandle resourceHandle);

        /**
         * API for joining a group. This API is used when a resource that
         * doesn't have a group tries to find and join a specific remote group.
         *
         * @param resource - group resource pointer to join.
         *                   It can be the callback result of findGroup().
         * @param resourceHandle - resource handle to join a group.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult joinGroup(const std::shared_ptr< OCResource > resource,
                OCResourceHandle resourceHandle);

        /**
         * API for leaving a joined group.
         *
         * @param collectionResourceType - resource type of a group to leave.
         * @param resourceHandle - resource handle to leave a group.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult leaveGroup(std::string collectionResourceType,
                OCResourceHandle resourceHandle);

        /**
         * API for deleting a group.
         *
         * @param collectionResourceType - resource type of a group to delete.
         *
         * @return void
         */
        void deleteGroup(std::string collectionResourceType);

        /**
         * API for getting a list of joined groups.
         *
         * @param void
         *
         * @return std::map - return value of this API.
         *                  It returns group resource type and group resource handle as a map type.
         */
        std::map< std::string, OCResourceHandle > getGroupList(void);

        // Things Configuration

        /**
         * API for updating configuration value of multiple things of a target group
         * or a single thing.
         * Callback is called when a response arrives.
         * Before using the below function, a developer should acquire a resource pointer of
         * (collection) resource that he wants to send a request by calling findResource() function
         * provided in OCPlatform. And he should also notice a "Configuration Name" term which
         * represents a nickname of a target attribute of a resource that he wants to update.
         * The base motivation to introduce the term is to avoid a usage of URI to access a resource
         * from a developer. Thus, a developer should know which configuration names are supported
         * by Things Configuration class and what the configuration name means.
         * To get a list of supported configuration names,
         * use getListOfSupportedConfigurationUnits()
         * function, which provides the list in JSON format.
         *
         * @param resource - resource pointer representing the target group or the single thing.
         * @param configurations - ConfigurationUnit: a nickname of attribute of target resource
         *                         (e.g., installedlocation, currency, (IP)address)
         *                         Value : a value to be updated
         * @param callback - callback for updateConfigurations.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult updateConfigurations(std::shared_ptr< OCResource > resource,
                std::map< std::string, std::string > configurations,
                std::function<
                        void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                const int eCode) > callback);

        /**
         * API for getting configuration value of multiple things of a target group
         * or a single thing.
         * Callback is called when a response arrives.
         *
         * @param resource - resource pointer representing the target group or the single thing.
         * @param configurations - ConfigurationUnit: a nickname of attribute of target resource.
         * @param callback - callback for getConfigurations.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult getConfigurations(std::shared_ptr< OCResource > resource,
                std::vector< std::string > configurations,
                std::function<
                        void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                const int eCode) > callback);

        /**
         * API for showing the list of supported configuration units (configurable parameters)
         * Callback is called when a response arrives.
         *
         * @param void
         * @return std::string - return value of this API.
         *                       It returns the list in JSON format
         */
        std::string getListOfSupportedConfigurationUnits();

        /**
         * API for boostrapping system configuration parameters from a bootstrap server.
         * Callback call when a response from the bootstrap server arrives.
         *
         * @param callback - callback for doBootstrap.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult doBootstrap(
                std::function<
                        void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                const int eCode) > callback);

        // Things Diagnostics

        /**
         * API to let thing(device) reboot.
         * The target thing could be a group of multiple things or a single thing.
         * Callback is called when a response arrives.
         *
         * @param resource - resource pointer representing the target group
         * @param callback - callback for reboot.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult reboot(std::shared_ptr< OCResource > resource,
                std::function<
                        void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                const int eCode) > callback);

        /**
         * API for factory reset on thing(device).
         * The target thing could be a group of multiple things or a single thing.
         * Callback is called when a response arrives.
         *
         * @param resource - resource pointer representing the target group
         * @param callback - callback for factoryReset.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult factoryReset(std::shared_ptr< OCResource > resource,
                std::function<
                        void(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                                const int eCode) > callback);

        // Group Action.

        /**
         * API for extracting Action Set string from the Action Set class instance
         *
         * @param newActionSet - pointer of Action Set
         *
         * @return std::string - return value of this API.
         *					     It returns Action Set String.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        std::string getStringFromActionSet(const ActionSet *newActionSet);

        /**
         * API for extrracting Action Set class instance from Action Set String.
         *
         * @param desc - description of Action set
         *
         * @return ActionSet* - return value of this API.
         *                      It returns pointer of ActionSet.
         */
        ActionSet* getActionSetfromString(std::string desc);

        /**
         * API for adding an Action Set.
         * Callback is called when the response of PUT operation arrives.
         *
         * @param resource - resource pointer of the group resource
         * @param newActionSet - pointer of Action Set
         * @param callback - callback for PUT operation.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult addActionSet(std::shared_ptr< OCResource > resource,
                const ActionSet* newActionSet, PutCallback cb);

        /**
         * API for executing the Action Set.
         * Callback is called when the response of  POST operation arrives.
         *
         * @param resource - resource pointer of the group resource
         * @param actionsetName - Action Set name for executing the Action set
         * @param callback - callback for POST operation.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult executeActionSet(std::shared_ptr< OCResource > resource,
                std::string actionsetName, PostCallback cb);

        /**
         * API for reading the Action Set.
         * Callback is called when the response of  GET operation arrives.
         *
         * @param resource - resource pointer of the group resource
         * @param actionsetName - Action Set name for reading the Action set
         * @param callback - callback for GET operation.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult getActionSet(std::shared_ptr< OCResource > resource,
                std::string actionsetName, GetCallback cb);

        /**
         * API for removing the Action Set.
         * Callback is called when the response of  POST operation arrives.
         *
         * @param resource - resource pointer of the group resource
         * @param actionsetName - Action Set name for removing the Action set
         * @param callback - callback for POST operation.
         *
         * @return OCStackResult - return value of this API.
         *                         It returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult deleteActionSet(std::shared_ptr< OCResource > resource,
                std::string actionsetName, PostCallback);

    };
}
#endif  /* __OC_THINGSMANAGER__*/
