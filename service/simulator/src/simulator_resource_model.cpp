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

#include "simulator_resource_model.h"
#include <sstream>
#include <boost/lexical_cast.hpp>

class to_string_visitor : public boost::static_visitor<std::string>
{
    public:
        template <typename T>
        result_type operator ()(const T &value)
        {
            return boost::lexical_cast<std::string>(value);
        }
};

class add_to_representation : public boost::static_visitor<>
{
    public:
        add_to_representation(OC::OCRepresentation &rep, const std::string &key)
            : m_rep(rep), m_key(key) {}

        template <typename T>
        void operator ()(const T &value)
        {
            m_rep.setValue(m_key, value);
        }

        OC::OCRepresentation &&getRep()
        {
            return std::move(m_rep);
        }

    private:
        OC::OCRepresentation m_rep;
        std::string m_key;
};

class range_validation : public boost::static_visitor<bool>
{
    public:
        range_validation (SimulatorResourceModel::Attribute &attrItem)
            : m_attrItem(attrItem) {}

        bool operator ()(int &value)
        {
            int min, max;
            m_attrItem.getRange(min, max);
            if (value >= min && value <= max)
                return true;
            return false;
        }

        bool operator ()(double &value)
        {
            std::vector<SimulatorResourceModel::Attribute::ValueVariant> values
                = m_attrItem.getAllowedValues();
            for (SimulatorResourceModel::Attribute::ValueVariant & val : values)
            {
                SimulatorResourceModel::Attribute::ValueVariant vVal = value;
                if (val == vVal)
                    return true;
            }
            return false;
        }

        bool operator ()(std::string &value)
        {
            std::vector<SimulatorResourceModel::Attribute::ValueVariant> values
                = m_attrItem.getAllowedValues();
            for (SimulatorResourceModel::Attribute::ValueVariant & vVal : values)
            {
                std::string val = boost::get<std::string>(vVal);
                if (val == value)
                    return true;
            }

            return false;
        }

    private:
        SimulatorResourceModel::Attribute &m_attrItem;
};

SimulatorResourceModel::Attribute::ValueVariant
&SimulatorResourceModel::Attribute::AllowedValues::at(int index)
{
    return m_values.at(index);
}

int SimulatorResourceModel::Attribute::AllowedValues::size() const
{
    return m_values.size();
}

std::string SimulatorResourceModel::Attribute::AllowedValues::toString() const
{
    std::ostringstream stream;
    stream << "[ ";
    for (auto & value : m_values)
    {
        to_string_visitor visitor;
        stream << boost::apply_visitor(visitor, value) << " ";
    }
    stream << "]";
    return stream.str();
}

std::vector<std::string> SimulatorResourceModel::Attribute::AllowedValues::toVectorString() const
{
    std::vector<std::string> values;

    for (auto & value : m_values)
    {
        to_string_visitor visitor;
        values.push_back(boost::apply_visitor(visitor, value));
    }
    return values;
}

std::vector<SimulatorResourceModel::Attribute::ValueVariant>
SimulatorResourceModel::Attribute::AllowedValues::getValues()
{
    return m_values;
}

std::string SimulatorResourceModel::Attribute::getName(void) const
{
    return m_name;
}

void SimulatorResourceModel::Attribute::setName(const std::string &name)
{
    m_name = name;
}

void SimulatorResourceModel::Attribute::getRange(int &min, int &max) const
{
    min = m_min;
    max = m_max;
}

void SimulatorResourceModel::Attribute::setRange(const int &min, const int &max)
{
    m_min = min;
    m_max = max;
}

int SimulatorResourceModel::Attribute::getAllowedValuesSize() const
{
    return m_allowedValues.size();
}

void SimulatorResourceModel::Attribute::setFromAllowedValue(const int allowedValueIndex)
{
    m_value = m_allowedValues.at(allowedValueIndex);
}

std::string SimulatorResourceModel::Attribute::valueToString() const
{
    to_string_visitor visitor;
    return boost::apply_visitor(visitor, m_value);
}

std::string SimulatorResourceModel::Attribute::allowedValuesToString() const
{
    return m_allowedValues.toString();
}

std::vector<std::string> SimulatorResourceModel::Attribute::allowedValuesToVectorString() const
{
    return m_allowedValues.toVectorString();
}

void SimulatorResourceModel::Attribute::addValuetoRepresentation(OC::OCRepresentation &rep,
        const std::string &key) const
{
    add_to_representation visitor(rep, key);
    boost::apply_visitor(visitor, m_value);
    rep = visitor.getRep();
}

bool SimulatorResourceModel::Attribute::compare(SimulatorResourceModel::Attribute &attribute)
{
    // Check the value types
    if (m_value.which() != attribute.getValue().which())
    {
        return false;
    }

    // Check the value in allowed range
    range_validation visitor(*this);
    return boost::apply_visitor(visitor, attribute.getValue());
}

std::vector<SimulatorResourceModel::Attribute::ValueVariant>
SimulatorResourceModel::Attribute::getAllowedValues()
{
    return m_allowedValues.getValues();
}

bool SimulatorResourceModel::getAttribute(const std::string &attrName, Attribute &value)
{
    if (m_attributes.end() != m_attributes.find(attrName))
    {
        value = m_attributes[attrName];
        return true;
    }

    return false;
}

std::map<std::string, SimulatorResourceModel::Attribute> SimulatorResourceModel::getAttributes()
const
{
    return m_attributes;
}

void SimulatorResourceModel::setRange(const std::string &attrName, const int min, const int max)
{
    if (m_attributes.end() != m_attributes.find(attrName))
        m_attributes[attrName].setRange(min, max);
}

void SimulatorResourceModel::setUpdateInterval(const std::string &attrName, int interval)
{
    if (m_attributes.end() != m_attributes.find(attrName))
        m_attributes[attrName].setUpdateFrequencyTime(interval);
}

void SimulatorResourceModel::updateAttributeFromAllowedValues(const std::string &attrName,
        const int allowedValueIndex)
{
    if (m_attributes.end() != m_attributes.find(attrName))
        m_attributes[attrName].setFromAllowedValue(allowedValueIndex);
}

void SimulatorResourceModel::removeAttribute(const std::string &attrName)
{
    m_attributes.erase(attrName);
    return;
}

OC::OCRepresentation SimulatorResourceModel::getOCRepresentation() const
{
    OC::OCRepresentation rep;
    for (auto & attribute : m_attributes)
    {
        (attribute.second).addValuetoRepresentation(rep, attribute.first);
    }

    return rep;
}

bool SimulatorResourceModel::update(OC::OCRepresentation &ocRep)
{
    if (0 == ocRep.size())
        return true;

    // Convert OCRepresentation to SimulatorResourceModel
    SimulatorResourceModel resModel = create(ocRep);

    return update(resModel);
}

bool SimulatorResourceModel::update(SimulatorResourceModel &repModel)
{
    std::map<std::string, SimulatorResourceModel::Attribute> attributes = repModel.getAttributes();
    for (auto & attributeItem : attributes)
    {
        // Check the attribute presence
        SimulatorResourceModel::Attribute attribute;
        if (false == getAttribute((attributeItem.second).getName(), attribute))
        {
            return false;
        }

        // Check the validity of the value to be set
        if (false == attribute.compare(attributeItem.second))
        {
            return false;
        }
        m_attributes[(attributeItem.second).getName()].setValue((attributeItem.second).getValue());
    }

    return true;
}

SimulatorResourceModel SimulatorResourceModel::create(const OC::OCRepresentation &ocRep)
{
    SimulatorResourceModel resModel;
    for (auto & attributeItem : ocRep)
    {
        SimulatorResourceModel::Attribute attribute;
        if (attributeItem.type() == OC::AttributeType::Integer)
            attribute.setValue(attributeItem.getValue<int>());
        if (attributeItem.type() == OC::AttributeType::Double)
            attribute.setValue(attributeItem.getValue<double>());
        if (attributeItem.type() == OC::AttributeType::String)
            attribute.setValue(attributeItem.getValue<std::string>());

        attribute.setName(attributeItem.attrname());
        resModel.m_attributes[attributeItem.attrname()] = attribute;
    }

    return resModel;
}

