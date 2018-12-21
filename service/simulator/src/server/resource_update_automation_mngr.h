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

#ifndef SIMULATOR_RESOURCE_UPDATE_AUTOMATION_MNGR_H_
#define SIMULATOR_RESOURCE_UPDATE_AUTOMATION_MNGR_H_

#include "resource_update_automation.h"
#include <unordered_map>

/**
 * @class SimulatorSingleResourceImpl
 */
class SimulatorSingleResourceImpl;
/**
 * @class UpdateAutomationMngr
 */
class UpdateAutomationMngr
{
    public:
        UpdateAutomationMngr() : m_id(0) {}

        /**
         * start resource automation
         * @param[in] resource    automation resource
         * @param[in] type        resource type
         * @param[in] interval    interval
         * @param[in] callback    callback function
         * @return resource automation id value
         */
        int startResourceAutomation(std::shared_ptr<SimulatorSingleResourceImpl> resource,
                                    AutoUpdateType type, int interval,
                                    const SimulatorSingleResource::AutoUpdateCompleteCallback &callback);

        /**
         * start attribute automation
         * @param[in] resource     simulator resource
         * @param[in] attrName     attribute name
         * @param[in] type         attribute type
         * @param[in] interval     interval
         * @param[in] callback     callback function
         * @return resource automation id value
         */
        int startAttributeAutomation(std::shared_ptr<SimulatorSingleResourceImpl> resource,
                                     const std::string &attrName, AutoUpdateType type, int interval,
                                     const SimulatorSingleResource::AutoUpdateCompleteCallback &callback);

        /**
         * get resource automation Ids
         * @return resource automation id
         */
        std::vector<int> getResourceAutomationIds();

        /**
         * get attribute automation Ids
         * @return attribute automation id
         */
        std::vector<int> getAttributeAutomationIds();

        /**
         * stop updation for given Id
         * @param[in] updationID   id to update
         */
        void stop(int updationID);

        /** stop all updation */
        void stopAll();

    private:
        void automationCompleted(int id);

        int m_id;
        std::mutex m_lock;
        std::unordered_map<int, ResourceUpdateAutomationSP> m_resourceUpdationList;
        std::unordered_map<int, AttributeUpdateAutomationSP> m_attrUpdationList;
};

#endif
