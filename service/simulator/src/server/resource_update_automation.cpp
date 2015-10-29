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

#include "resource_update_automation.h"
#include "simulator_single_resource_impl.h"
#include "attribute_generator.h"
#include "simulator_exceptions.h"
#include "simulator_logger.h"
#include "logger.h"

#define ATAG "ATTRIBUTE_AUTOMATION"
#define RTAG "RESOURCE_AUTOMATION"

#define SLEEP_FOR(X) if (X > 0) std::this_thread::sleep_for(std::chrono::milliseconds(X));

AttributeUpdateAutomation::AttributeUpdateAutomation(int id, SimulatorSingleResource *resource,
        const SimulatorResourceModel::Attribute &attribute, AutomationType type, int interval,
        updateCompleteCallback callback, std::function<void (const int)> finishedCallback)
    :   m_resource(resource),
        m_type(type),
        m_id(id),
        m_stopRequested(false),
        m_updateInterval(interval),
        m_attributeGen(attribute),
        m_callback(callback),
        m_finishedCallback(finishedCallback),
        m_thread(nullptr)
{
    if (m_updateInterval < 0)
    {
        m_updateInterval = 0;
    }
}

void AttributeUpdateAutomation::start()
{
    m_thread = new std::thread(&AttributeUpdateAutomation::updateAttribute, this);
}

void AttributeUpdateAutomation::stop()
{
    m_stopRequested = true;
    if (m_thread)
        m_thread->join();
}

void AttributeUpdateAutomation::updateAttribute()
{
    SimulatorSingleResourceImpl *resourceImpl =
        dynamic_cast<SimulatorSingleResourceImpl *>(m_resource);

    if (!resourceImpl)
        return;

    do
    {
        try
        {
            SimulatorResourceModel::Attribute attribute;
            while (!m_stopRequested && true == m_attributeGen.next(attribute))
            {
                if (false == m_resource->updateAttributeValue(attribute))
                {
                    OC_LOG_V(ERROR, TAG, "Failed to update the attribute![%s]", attribute.getName());
                    continue;
                }
                resourceImpl->notifyApp();

                SLEEP_FOR(m_updateInterval);
            }

            m_attributeGen.reset();
        }
        catch (SimulatorException &e)
        {
            break;
        }
    }
    while (!m_stopRequested && AutomationType::RECURRENT == m_type);

    if (!m_stopRequested)
    {
        OC_LOG_V(DEBUG, ATAG, "Attribute:%s automation is completed!", m_attrName.c_str());
        SIM_LOG(ILogger::INFO, "Automation of " << m_attrName << " attribute is completed.");
    }

    // Notify application through callback
    if (m_callback)
        m_callback(m_resource->getURI(), m_id);

    if (m_finishedCallback && !m_stopRequested)
        m_finishedCallback(m_id);
}

ResourceUpdateAutomation::ResourceUpdateAutomation(int id, SimulatorSingleResource *resource,
        AutomationType type, int interval, updateCompleteCallback callback,
        std::function<void (const int)> finishedCallback)
    :   m_resource(resource),
        m_type(type),
        m_id(id),
        m_stopRequested(false),
        m_updateInterval(interval),
        m_callback(callback),
        m_finishedCallback(finishedCallback),
        m_thread(nullptr) {}

void ResourceUpdateAutomation::start()
{
    std::vector<SimulatorResourceModel::Attribute> attributes;
    for (auto &attributeEntry : m_resource->getResourceModel().getAttributes())
    {
        attributes.push_back(attributeEntry.second);
    }

    if (0 == attributes.size())
    {
        OC_LOG(ERROR, RTAG, "Resource has zero attributes!");
        throw SimulatorException(SIMULATOR_ERROR, "Resource has zero attributes!");
    }

    m_thread = new std::thread(&ResourceUpdateAutomation::updateAttributes, this, attributes);
}

void ResourceUpdateAutomation::stop()
{
    m_stopRequested = true;
    if (m_thread)
        m_thread->join();
}

void ResourceUpdateAutomation::updateAttributes(
    std::vector<SimulatorResourceModel::Attribute> attributes)
{
    SimulatorSingleResourceImpl *resourceImpl =
        dynamic_cast<SimulatorSingleResourceImpl *>(m_resource);

    if (!resourceImpl)
        return;

    do
    {
        AttributeCombinationGen attrCombGen(attributes);
        SimulatorResourceModel resModel;
        while (!m_stopRequested && attrCombGen.next(resModel))
        {
            for (auto &attributeEntry : resModel.getAttributes())
            {
                resourceImpl->updateAttributeValue(attributeEntry.second);
            }

            resourceImpl->notifyApp();
            SLEEP_FOR(m_updateInterval);
        }
    }
    while (!m_stopRequested && AutomationType::RECURRENT == m_type);

    // Notify application
    if (m_callback)
        m_callback(m_resource->getURI(), m_id);

    if (m_finishedCallback && !m_stopRequested)
        m_finishedCallback(m_id);
}

