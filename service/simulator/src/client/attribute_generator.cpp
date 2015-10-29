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
    :   m_attribute(attribute),
        m_curIntValue(INT_MIN),
        m_valueSetIndex(0)
{
    if (m_attribute.getProperty().type() == SimulatorResourceModel::AttributeProperty::Type::RANGE)
    {
        m_curIntValue = m_attribute.getProperty().min();
    }
    else if (m_attribute.getProperty().type() ==
             SimulatorResourceModel::AttributeProperty::Type::VALUE_SET)
    {
        m_supportedValues = m_attribute.getProperty().valueSet();
    }
}

bool AttributeGenerator::hasNext()
{
    if (m_attribute.getProperty().type() == SimulatorResourceModel::AttributeProperty::Type::RANGE
        && m_curIntValue <= m_attribute.getProperty().max())
    {
        return true;
    }
    else if (m_attribute.getProperty().type() ==
             SimulatorResourceModel::AttributeProperty::Type::VALUE_SET
             && m_valueSetIndex <= m_supportedValues.size() - 1)
    {
        return true;
    }

    return false;
}

bool AttributeGenerator::next(SimulatorResourceModel::Attribute &attribute)
{
    if (!hasNext())
        return false;

    attribute.setName(m_attribute.getName());
    if (m_attribute.getProperty().type() == SimulatorResourceModel::AttributeProperty::Type::RANGE
        && m_curIntValue <= m_attribute.getProperty().max())
    {
        attribute.setValue(m_curIntValue++);
    }
    else if (m_attribute.getProperty().type() ==
             SimulatorResourceModel::AttributeProperty::Type::VALUE_SET
             && m_valueSetIndex <= m_supportedValues.size() - 1)
    {
        attribute.setValue(m_supportedValues[m_valueSetIndex++]);
    }

    return true;
}

SimulatorResourceModel::Attribute AttributeGenerator::current()
{
    SimulatorResourceModel::Attribute attribute;
    attribute.setName(m_attribute.getName());

    if (m_attribute.getProperty().type() == SimulatorResourceModel::AttributeProperty::Type::RANGE
        && m_curIntValue <= m_attribute.getProperty().max())
    {
        attribute.setValue(m_curIntValue);
    }
    else if (m_attribute.getProperty().type() ==
             SimulatorResourceModel::AttributeProperty::Type::VALUE_SET
             && m_valueSetIndex <= m_supportedValues.size() - 1)
    {
        attribute.setValue(m_supportedValues[m_valueSetIndex]);
    }

    return attribute;
}

void AttributeGenerator::reset()
{
    m_curIntValue = m_attribute.getProperty().min();
    m_valueSetIndex = 0;
}

AttributeCombinationGen::AttributeCombinationGen(
    const std::vector<SimulatorResourceModel::Attribute> &attributes)
{
    for (auto &attr : attributes)
    {
        AttributeGenerator attrGen(attr);
        m_attrGenList.push_back(attr);
        m_resModel.add(attr);
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
        for (size_t index = 0; index < m_attrGenList.size(); index++)
        {
            // Add the attribute on resource model
            updateAttributeInModel(index);
        }

        m_index = m_attrGenList.size() - 1;
        resModel = m_resModel;
        return true;
    }

    // Get the next attribute from statck top element
    if (m_attrGenList[m_index].hasNext())
    {
        updateAttributeInModel(m_index);
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
                updateAttributeInModel(index);
            }
            else
            {
                updateAttributeInModel(index);
                break;
            }
        }

        resModel = m_resModel;
        return true;
    }

    return false;
}

void AttributeCombinationGen::updateAttributeInModel(int index)
{
    SimulatorResourceModel::Attribute attribute;
    if (m_attrGenList[index].next(attribute))
        m_resModel.updateValue(attribute);
}
