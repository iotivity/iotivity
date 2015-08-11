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
#include "simulator_resource_server.h"
#include <thread>

#define SLEEP_FOR(X) if (X > 0) std::this_thread::sleep_for(std::chrono::milliseconds(X));

AttributeUpdateAutomation::AttributeUpdateAutomation(
    SimulatorResourceServer *resource, const std::string &attrName, updateCompleteCallback callback,
    int automationId, std::function<void (const int)> finishedCallback, AutomationType type,
    int interval)
    : m_resource(resource),
      m_attrName(attrName),
      m_type(type),
      m_id(automationId),
      m_status(false),
      m_stopRequested(false),
      m_updateInterval(interval),
      m_callback(callback),
      m_finishedCallback(finishedCallback) {}

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

    // Notify application through callback
    if (m_callback)
        m_callback(m_resource->getURI(), m_id);
    if (m_finishedCallback && !m_stopRequested)
        m_finishedCallback(m_id);
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
            if (m_stopRequested)
                break;
            m_resource->updateAttribute(m_attribute.getName(), value);
            SLEEP_FOR(m_updateInterval);
        }
    }
    else
    {
        for (int index = 0; index < m_attribute.getAllowedValuesSize(); index++)
        {
            if (m_stopRequested)
                break;
            m_resource->updateAttributeFromAllowedValues(m_attribute.getName(), index);
            SLEEP_FOR(m_updateInterval);
        }
    }
}


ResourceUpdateAutomation::ResourceUpdateAutomation(
    SimulatorResourceServer *resource, updateCompleteCallback callback,
    int automationId, std::function<void (const int)> finishedCallback, AutomationType type,
    int interval)
    : m_resource(resource),
      m_type(type),
      m_id(automationId),
      m_status(false),
      m_updateInterval(interval),
      m_callback(callback),
      m_finishedCallback(finishedCallback) {}

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

    int id = 0;
    for (auto & attribute : attributes)
    {
        AttributeUpdateAutomationPtr attributeAutomation = std::make_shared<AttributeUpdateAutomation>
                (m_resource, attribute.first, nullptr, id,
                 std::bind(&ResourceUpdateAutomation::finished, this, std::placeholders::_1),
                 m_type, m_updateInterval);
        m_attrUpdationList[id++] = attributeAutomation;
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

void ResourceUpdateAutomation::finished(int id)
{
    if (m_attrUpdationList.end() != m_attrUpdationList.find(id))
    {
        m_attrUpdationList.erase(m_attrUpdationList.find(id));
    }

    if (!m_attrUpdationList.size())
    {
        // Notify application through callback
        if (m_callback)
            m_callback(m_resource->getURI(), m_id);
        if (m_finishedCallback)
            m_finishedCallback(m_id);
    }
}
void ResourceUpdateAutomation::stop()
{
    // Stop all the attributes updation
    for (auto & attrAutomation : m_attrUpdationList)
    {
        (attrAutomation.second)->stop();
    }

    m_attrUpdationList.clear();
    m_status = false;
}

UpdateAutomationManager::UpdateAutomationManager()
    : m_automationId(0) {}

SimulatorResult UpdateAutomationManager::startResourceAutomation(SimulatorResourceServer *resource,
        int &id, updateCompleteCallback callback, AutomationType type, int interval)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    ResourceUpdateAutomationPtr resourceAutomation(new ResourceUpdateAutomation(
                resource, callback, m_automationId,
                std::bind(&UpdateAutomationManager::automationFinished, this, std::placeholders::_1),
                type, interval));
    SimulatorResult result = resourceAutomation->start();
    if (SIMULATOR_SUCCESS != result)
    {
        id = -1;
        return result;
    }

    m_resourceUpdationList[m_automationId] = resourceAutomation;
    id = m_automationId++;
    return result;
}

SimulatorResult UpdateAutomationManager::startAttributeAutomation(SimulatorResourceServer *resource,
        const std::string &attrName, int &id, updateCompleteCallback callback, AutomationType type,
        int interval)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    AttributeUpdateAutomationPtr attributeAutomation(new AttributeUpdateAutomation(
                resource, attrName, callback, m_automationId,
                std::bind(&UpdateAutomationManager::automationFinished, this, std::placeholders::_1),
                type, interval));
    SimulatorResult result = attributeAutomation->start();
    if (SIMULATOR_SUCCESS != result)
    {
        id = -1;
        return result;
    }

    m_attrUpdationList[m_automationId] = attributeAutomation;
    id = m_automationId++;
    return result;
}

std::vector<int> UpdateAutomationManager::getResourceAutomationIds()
{
    std::vector<int> ids;
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto & automation : m_resourceUpdationList)
        ids.push_back(automation.first);

    return ids;
}

std::vector<int> UpdateAutomationManager::getAttributeAutomationIds()
{
    std::vector<int> ids;
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto & automation : m_attrUpdationList)
        ids.push_back(automation.first);

    return ids;
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

void UpdateAutomationManager::automationFinished(int id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_resourceUpdationList.end() != m_resourceUpdationList.find(id))
    {
        m_resourceUpdationList.erase(m_resourceUpdationList.find(id));
    }
    else if (m_attrUpdationList.end() != m_attrUpdationList.find(id))
    {
        m_attrUpdationList.erase(m_attrUpdationList.find(id));
    }
}