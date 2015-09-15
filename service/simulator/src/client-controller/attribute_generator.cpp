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

AttributeGenerator::AttributeGenerator(SimulatorResourceModel::Attribute &attribute)
{
    m_name.assign(attribute.getName());
    m_type = attribute.getValueType();

    if (!attribute.getValueType())
        attribute.getRange(m_min, m_max);
    if (m_max > 0)
    {
        m_hasRange = true;
    }

    if (!m_type)
        m_rangeIndex = m_min;

    m_allowedValues = attribute.getAllowedValues();
    if (0 != m_allowedValues.size())
    {
        m_hasAllowedValue = true;
    }
    m_nextAllowedValueIndex = 0;
    m_prevAllowedValueIndex = m_allowedValues.size();
}

AttributeGenerator::~AttributeGenerator()
{
    m_rangeIndex = 0;
    m_min = 0;
    m_max = 0;
    m_nextAllowedValueIndex = 0;
    m_prevAllowedValueIndex = 0;
    m_type = 0;
}

bool AttributeGenerator::hasNext()
{
    if (m_hasRange && m_rangeIndex <= m_max && !m_type)
    {
        return true;
    }

    if (m_hasAllowedValue && m_nextAllowedValueIndex < m_allowedValues.size())
    {
        return true;
    }

    return false;
}

AttributeSP AttributeGenerator::next()
{
    AttributeSP attr = std::make_shared<SimulatorResourceModel::Attribute>(m_name);

    if (!attr)
        return nullptr;

    if (m_hasRange && !m_type)
    {
        attr->setName(m_name);
        attr->setValue(m_rangeIndex++);
        return attr;
    }

    if (m_hasAllowedValue)
    {
        switch (m_type)
        {
            case 1:
                {
                    attr->setValue(m_allowedValues[m_nextAllowedValueIndex++]);
                }
                break;

            case 3:
                {
                    attr->setValue(m_allowedValues[m_nextAllowedValueIndex++]);
                }
                break;
        }

        return attr;
    }
}

AttributeSP AttributeGenerator::previous()
{
    AttributeSP attr = std::make_shared<SimulatorResourceModel::Attribute>(m_name);

    if (!attr)
        return nullptr;

    if (m_hasRange && !m_type)
    {
        attr->setValue(m_rangeIndex - 1);
        return attr;
    }

    if (m_hasAllowedValue)
    {
        switch (m_type)
        {
            case 1:
                {
                    attr->setValue(m_allowedValues[m_prevAllowedValueIndex - 1]);
                }
                break;

            case 3:
                {
                    attr->setValue(m_allowedValues[m_prevAllowedValueIndex - 1]);
                }
                break;
        }

        return attr;
    }
}

