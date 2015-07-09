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

#include "simulator_attribute_automation.h"
#include "simulator_resource.h"
#include <thread>

#define SLEEP_FOR(X) if (X > 0) std::this_thread::sleep_for(std::chrono::milliseconds(X));

AttributeUpdateAutomation::AttributeUpdateAutomation(
    SimulatorResource *resource, const std::string &attrName, AutomationType type, int interval)
    : m_resource(resource),
      m_attrName(attrName),
      m_type(type),
      m_status(false),
      m_stopRequested(false),
      m_updateInterval(interval) {}

SimulatorResult AttributeUpdateAutomation::start()
{
    if (true == m_status)
        return SIMULATOR_AUTOMATION_ALREADY_STARTED;

    // Check the validity of attribute
    SimulatorResourceModel resModel = m_resource->getModel();
    if (false == resModel.getAttribute(m_attrName, m_attribute))
        return SIMULATOR_ERROR;

    if (m_updateInterval < 0)
    {
        m_updateInterval = m_attribute.getUpdateFrequencyTime();
        if (0 > m_updateInterval)
            m_updateInterval = 0;
    }

    m_thread = new std::thread(&AttributeUpdateAutomation::updateAttribute, this);
    m_status = true;

    return SIMULATOR_SUCCESS;
}

void AttributeUpdateAutomation::stop()
{
    m_stopRequested = true;
    m_thread->join();
    m_status = false;
}

void AttributeUpdateAutomation::updateAttribute()
{
    do
    {
        setAttributeValue();
        if (m_stopRequested)
            break;
    }
    while (AutomationType::RECURRENT == m_type);

    m_status = false;
}

void AttributeUpdateAutomation::setAttributeValue()
{
    if (0 == m_attribute.getValueType()) // For integer type values
    {
        int min;
        int max;
        m_attribute.getRange(min, max);
        for (int value = min; value <= max; value++)
        {
            m_resource->updateAttribute(m_attribute.getName(), value);
            SLEEP_FOR(m_updateInterval);
        }
    }
    else
    {
        for (int index = 0; index < m_attribute.getAllowedValuesSize(); index++)
        {
            m_resource->updateAttributeFromAllowedValues(m_attribute.getName(), index);
            SLEEP_FOR(m_updateInterval);
        }
    }
}


ResourceUpdateAutomation::ResourceUpdateAutomation(
    SimulatorResource *resource, AutomationType type, int interval)
    : m_resource(resource),
      m_type(type),
      m_status(false),
      m_updateInterval(interval) {}

SimulatorResult ResourceUpdateAutomation::start()
{
    if (true == m_status)
        return SIMULATOR_AUTOMATION_ALREADY_STARTED;

    m_resModel = m_resource->getModel();
    std::map<std::string, SimulatorResourceModel::Attribute> attributes = m_resModel.getAttributes();
    if (0 == attributes.size())
    {
        m_status = false;
        return SIMULATOR_ERROR;
    }

    for (auto & attribute : attributes)
    {
        AttributeUpdateAutomationPtr attributeAutomation = std::make_shared<AttributeUpdateAutomation>
                (m_resource, attribute.first, m_type, m_updateInterval);
        m_attrUpdationList.push_back(attributeAutomation);
        if (SIMULATOR_SUCCESS != attributeAutomation->start())
        {
            m_status = false;
            stop();
            return SIMULATOR_ERROR;
        }
    }

    m_status = true;
    return SIMULATOR_SUCCESS;
}

void ResourceUpdateAutomation::stop()
{
    // Stop all the attributes updation
    for (auto & attrAutomation : m_attrUpdationList)
    {
        attrAutomation->stop();
    }

    m_attrUpdationList.clear();
    m_status = false;
}

UpdateAutomationManager::UpdateAutomationManager()
    : m_automationId(0) {}

SimulatorResult UpdateAutomationManager::startResourceAutomation(SimulatorResource *resource,
        int &id, AutomationType type, int interval)
{
    ResourceUpdateAutomationPtr resoureceAutomation(new ResourceUpdateAutomation(resource, type,
            interval));
    SimulatorResult result = resoureceAutomation->start();
    if (SIMULATOR_SUCCESS != result)
    {
        id = -1;
        return result;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_resourceUpdationList[m_automationId++] = resoureceAutomation;
    id = m_automationId - 1;
    return result;
}

SimulatorResult UpdateAutomationManager::startAttributeAutomation(SimulatorResource *resource,
        const std::string &attrName, int &id, AutomationType type, int interval)
{
    AttributeUpdateAutomationPtr attributeAutomation(new AttributeUpdateAutomation(resource, attrName,
            type, interval));
    SimulatorResult result = attributeAutomation->start();
    if (SIMULATOR_SUCCESS != result)
    {
        id = -1;
        return result;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_attrUpdationList[m_automationId++] = attributeAutomation;
    id = m_automationId - 1;
    return result;
}

void UpdateAutomationManager::stop(int automationId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_resourceUpdationList.end() != m_resourceUpdationList.find(automationId))
    {
        m_resourceUpdationList[automationId]->stop();
        m_resourceUpdationList.erase(m_resourceUpdationList.find(automationId));
    }
    else if (m_attrUpdationList.end() != m_attrUpdationList.find(automationId))
    {
        m_attrUpdationList[automationId]->stop();
        m_attrUpdationList.erase(m_attrUpdationList.find(automationId));
    }
}

void UpdateAutomationManager::stopAll()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::for_each(m_resourceUpdationList.begin(),
                  m_resourceUpdationList.end(), [] (std::pair<int, ResourceUpdateAutomationPtr> element)
    {
        element.second->stop();
    });
    m_resourceUpdationList.clear();

    std::for_each(m_attrUpdationList.begin(),
                  m_attrUpdationList.end(), [] (std::pair<int, AttributeUpdateAutomationPtr> element)
    {
        element.second->stop();
    });
    m_attrUpdationList.clear();
}
