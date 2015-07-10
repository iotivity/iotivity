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

#ifndef SIMULATOR_RESOURCE_UPDATE_AUTOMATION_H_
#define SIMULATOR_RESOURCE_UPDATE_AUTOMATION_H_

#include "simulator_resource_model.h"
#include "simulator_error_codes.h"

class SimulatorResource;

enum class AutomationType
{
    NORMAL,
    RECURRENT
};

class AttributeUpdateAutomation
{
    public:
        AttributeUpdateAutomation(SimulatorResource *resource,
                                  const std::string &attrName, AutomationType type = AutomationType::NORMAL, int interval = -1);
        SimulatorResult start();
        void stop();

    private:
        void updateAttribute();
        void setAttributeValue();

        SimulatorResource *m_resource;
        std::string m_attrName;
        AutomationType m_type;
        bool m_status;
        std::thread *m_thread;
        bool m_stopRequested;
        int m_updateInterval;
        SimulatorResourceModel::Attribute m_attribute;
};

typedef std::shared_ptr<AttributeUpdateAutomation> AttributeUpdateAutomationPtr;

class ResourceUpdateAutomation
{
    public:
        ResourceUpdateAutomation(SimulatorResource *resource,
                                 AutomationType type = AutomationType::NORMAL, int interval = -1);
        SimulatorResult start();
        void stop();

    private:
        void updateAttribute();
        void setAttributeValue();

        SimulatorResource *m_resource;
        AutomationType m_type;
        bool m_status;
        std::thread *m_thread;
        int m_updateInterval;
        SimulatorResourceModel m_resModel;
        std::vector<AttributeUpdateAutomationPtr> m_attrUpdationList;
};

typedef std::shared_ptr<ResourceUpdateAutomation> ResourceUpdateAutomationPtr;

class UpdateAutomationManager
{
    public:
        UpdateAutomationManager();
        SimulatorResult startResourceAutomation(SimulatorResource *resource,
                                                int &id, AutomationType type = AutomationType::NORMAL, int interval = -1);
        SimulatorResult startAttributeAutomation(SimulatorResource *resource,
                const std::string &attrName, int &id, AutomationType type = AutomationType::NORMAL,
                int interval = -1);
        void stop(int automationId);
        void stopAll();

    private:
        std::map<int, ResourceUpdateAutomationPtr> m_resourceUpdationList;
        std::map<int, AttributeUpdateAutomationPtr> m_attrUpdationList;
        int m_automationId;
        std::mutex m_mutex;
};

typedef std::shared_ptr<UpdateAutomationManager> UpdateAutomationManagerPtr;

#endif //SIMULATOR_RESOURCE_UPDATE_AUTOMATION_H_
