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

#include "attribute_generator.h"

AttributeGenerator::AttributeGenerator(const SimulatorResourceModel::Attribute &attribute)
    :   m_name(attribute.getName()),
        m_min(INT_MIN),
        m_max(INT_MAX),
        m_rangeIndex(-1),
        m_allowedValueIndex(0),
        m_hasRange(false),
        m_hasAllowedValue(false)
{
    if (attribute.getValueType() ==
        SimulatorResourceModel::Attribute::ValueType::INTEGER)
    {
        attribute.getRange(m_min, m_max);
        if (INT_MIN != m_min && INT_MAX != m_max)
        {
            m_hasRange = true;
            m_rangeIndex = m_min;
        }
    }
    else
    {
        m_allowedValues = attribute.getAllowedValues();
        if (0 != m_allowedValues.size())
        {
            m_hasAllowedValue = true;
        }
    }
}

bool AttributeGenerator::hasNext()
{
    if (m_hasRange && m_rangeIndex <= m_max)
    {
        return true;
    }

    if (m_hasAllowedValue && m_allowedValueIndex < m_allowedValues.size())
    {
        return true;
    }

    return false;
}

bool AttributeGenerator::next(SimulatorResourceModel::Attribute &attribute)
{
    attribute.setName(m_name);

    if (m_hasRange)
    {
        attribute.setValue(m_rangeIndex++);
        return true;
    }
    else if (m_hasAllowedValue)
    {
        attribute.setValue(m_allowedValues[m_allowedValueIndex++]);
        return true;
    }

    return false;
}

SimulatorResourceModel::Attribute AttributeGenerator::current()
{
    SimulatorResourceModel::Attribute attribute;

    attribute.setName(m_name);
    if (m_hasRange)
    {
        attribute.setValue(m_rangeIndex);
    }
    else if (m_hasAllowedValue)
    {
        attribute.setValue(m_allowedValues[m_allowedValueIndex]);
    }

    return attribute;
}

void AttributeGenerator::reset()
{
    if (m_hasRange)
    {
        m_rangeIndex = m_min;
    }
    else if (m_hasAllowedValue)
    {
        m_allowedValueIndex = 0;
    }
}

AttributeCombinationGen::AttributeCombinationGen(
        const std::vector<SimulatorResourceModel::Attribute> &attributes)
{
    for (auto &attr : attributes)
    {
        AttributeGenerator attrGen(attr);
        m_attrGenList.push_back(attr);
    }

    m_index = -1;
}

bool AttributeCombinationGen::next(SimulatorResourceModel &resModel)
{
    if (!m_attrGenList.size())
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_lock);

    // This block will execute for only first time
    if (-1 == m_index)
    {
        for (int index = 0; index < m_attrGenList.size(); index++)
        {
            // Add the attribute on resource model
            addAttributeToModel(index);
        }

        m_index = m_attrGenList.size() - 1;
        resModel = m_resModel;
        return true;
    }

    // Get the next attribute from statck top element
    if (m_attrGenList[m_index].hasNext())
    {
        addAttributeToModel(m_index);
        resModel = m_resModel;
        return true;
    }
    else
    {
        for (int index = m_index; index >= 0; index--)
        {
            if (!m_attrGenList[index].hasNext())
            {
                if (!index)
                    return false;

                m_attrGenList[index].reset();
                addAttributeToModel(index);
            }
            else
            {
                addAttributeToModel(index);
                break;
            }
        }

        resModel = m_resModel;
        return true;
    }

    return false;
}

void AttributeCombinationGen::addAttributeToModel(int index)
{
    SimulatorResourceModel::Attribute attribute;
    m_attrGenList[index].next(attribute);
    m_resModel.addAttribute(attribute, true);
}
