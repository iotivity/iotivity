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
 * @file   resource_manager.h
 *
 * @brief   This file provides a class for a set of APIs relating to resource management
 *               in the Service Provider side.
 */

#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <string>
#include <vector>
#include <map>
#include "simulator_resource.h"
#include "simulator_resource_creator.h"
#include "simulator_error_codes.h"

/**
 * @class   ResourceManager
 * @brief   This class provides a set of functions for managing the resource(s) in the Service Provider Module.
 */
class ResourceManager
{
    public:
        /**
         * This method is used to create/obtain the singleton instance of ResourceManager.
         *
         * @return ResourceManager - Singleton instance of ResourceManager.
         */
        static ResourceManager *getInstance(void);

        /**
         * This method is called for creating a single resource from the configuration file.
         *
         * @param configPath - RAML configuration file path.
         * @param callback - Callback method for receive notifications when resource model changes.
         *
         * @return SimulatorResourcePtr - Shared pointer of SimulatorResource on success, otherwise NULL.
         */
        SimulatorResourcePtr createResource(const std::string &configPath,
                                            SimulatorResource::ResourceModelChangedCB callback);

        /**
         * This method is called for creating a collection of resources from the configuration file.
         *
         * @param configPath - RAML configuration file path.
         * @param count - Number of resource to be created.
         * @param callback - Callback method for receive notifications when resource model changes.
         *
         * @return SimulatorResourcePtr - A vector of Shared pointers of SimulatorResource Objects.
         */
        std::vector<SimulatorResourcePtr> createResource(const std::string &configPath, const int count,
                SimulatorResource::ResourceModelChangedCB callback);

        /**
         * This method is called for obtaining a list of created resources.
         *
         * @return SimulatorResourcePtr - A vector of Shared pointers of SimulatorResource Objects.
         */
        std::vector<SimulatorResourcePtr> getResources(void) const;

        /**
         * This method is called for deleting a single resource.
         *
         * @param resource - Shared pointer of the SimulatorResource to be deleted.
         *
         * @return SimulatorResult
         */
        SimulatorResult deleteResource(SimulatorResourcePtr &resource);

        /**
         * This method is called for deleting multiple resources.
         * If this method is called without any parameter, then all resources will be deleted.
         * If thie method is called with a specific resourcetype as a parameter, then all the resources
         * of that particular type will be deleted.
         *
         * @param resourceType - Resource type of the resource
         *
         * @return SimulatorResult
         */
        SimulatorResult deleteResources(const std::string &resourceType = "");

    private:
        /**
         * This method is called for obtaining a unique URI when creating multiple resources using createResource API.
         * It appends a unique key to the given URI.
         * Example: If input is "/a/light", then the output will be "/a/light/simulator/0" for the first resource
         * and "/a/light/simulator/1" for the second resource and so on.
         *
         * @param uri - URI of the resource
         *
         * @return Unique URI for the resource
         */
        std::string getURI(std::string uri);

        ResourceManager();
        ~ResourceManager();

        SimulatorResourceCreator *m_resourceCreator;
        static int id;

        /**
         * This multi-level map organizes the resources in the form of ResourceType as the key
         * and a set of resources of that resourceType as the value.
         * The value is another map which has the ResourceURI as the key and the shared pointer
         * of the SimulatorResource object as the value.
         */
        std::map<std::string, std::map<std::string, SimulatorResourcePtr>> m_resourceList;
};

#endif

