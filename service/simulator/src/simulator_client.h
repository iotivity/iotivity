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

/**
 * @file   simulator_client.h
 *
 * @brief   This file provides a class for realizing simulator client functionality.
 *
 */

#ifndef SIMULATOR_CLIENT_H_
#define SIMULATOR_CLIENT_H_

#include "simulator_remote_resource.h"
#include "simulator_error_codes.h"

/**
 * @class   SimulatorClient
 * @brief   This class provides a set of functions for discovering the resources currently running on the servers.
 */
class SimulatorClient
{
    public:

        /**
         * API for getting singleton instance of SimulatorClient class.
         *
         * @return Instance of SimulatorClient class.
         *
         */
        static SimulatorClient *getInstance(void);

        /**
         * API for discovering resources of a particular resource type.
         * Callback is called when a resource is found.
         *
         * @param resourceType - required resource type
         * @param callback - Returns SimulatorRemoteResource.
         *
         * @return SimulatorResult - return value of this API.
         *                         It returns SIMULATOR_SUCCESS if success.
         *
         * NOTE: SimulatorResult is defined in simulator_error_codes.h.
         */
        SimulatorResult findResource(const std::string &resourceType, ResourceFindCallback callback);

        /**
         * API for getting list of already found resources.
         *
         * @param resourceType - resource type
         *
         * @return List of SimulatorRemoteResource
         *
         */
        std::vector<SimulatorRemoteResourcePtr> getFoundResources(
            const std::string &resourceType = "");

    private:

        SimulatorClient() = default;
        ~SimulatorClient() = default;

        void onResourceFound(std::shared_ptr<OC::OCResource> resource, ResourceFindCallback callback);
        void addResourceToList(const std::string &resourceType, const std::string &sid,
                               SimulatorRemoteResourcePtr &resource);

        std::recursive_mutex m_resourceListMutex;
        std::map<std::string, std::map<std::string, SimulatorRemoteResourcePtr>> m_resourceList;
};

#endif //SIMULATOR_CLIENT_H_

