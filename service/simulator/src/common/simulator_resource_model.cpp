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
#include "simulator_exceptions.h"
#include "logger.h"

#include <boost/lexical_cast.hpp>
#include <cfloat>

#define TAG "SIM_RESOURCE_MODEL"

template <typename T>
struct TypeConverter {};

template <>
struct TypeConverter<int>
{
    constexpr static SimulatorResourceModel::ValueType type =
        SimulatorResourceModel::ValueType::INTEGER;
};

template <>
struct TypeConverter<double>
{
    constexpr static SimulatorResourceModel::ValueType type =
        SimulatorResourceModel::ValueType::DOUBLE;
};

template <>
struct TypeConverter<bool>
{
    constexpr static SimulatorResourceModel::ValueType type =
        SimulatorResourceModel::ValueType::BOOLEAN;
};

template <>
struct TypeConverter<std::string>
{
    constexpr static SimulatorResourceModel::ValueType type =
        SimulatorResourceModel::ValueType::STRING;
};

template <>
struct TypeConverter<SimulatorResourceModel>
{
    constexpr static SimulatorResourceModel::ValueType type =
        SimulatorResourceModel::ValueType::RESOURCE_MODEL;
};

template <typename T>
struct TypeDetails
{
    constexpr static SimulatorResourceModel::ValueType type =
        TypeConverter<T>::type;
    constexpr static SimulatorResourceModel::ValueType baseType =
        TypeConverter<T>::type;
    constexpr static int depth = 0;
};

template <typename T>
struct TypeDetails<std::vector<T>>
{
    constexpr static SimulatorResourceModel::ValueType type =
        SimulatorResourceModel::ValueType::VECTOR;
    constexpr static SimulatorResourceModel::ValueType baseType =
        TypeDetails<T>::baseType;
    constexpr static int depth = 1 + TypeDetails<T>::depth;
};

class AttributeTypeVisitor : public boost::static_visitor<>
{
    public:
        AttributeTypeVisitor() : m_type(SimulatorResourceModel::ValueType::UNKNOWN),
            m_baseType(SimulatorResourceModel::ValueType::UNKNOWN), m_depth(0) {}

        template <typename T>
        void operator ()(const T &)
        {
            m_type = TypeDetails<T>::type;
            m_baseType = TypeDetails<T>::baseType;
            m_depth = TypeDetails<T>::depth;
        }

        SimulatorResourceModel::ValueType m_type;
        SimulatorResourceModel::ValueType m_baseType;
        int m_depth;
};

class OCRepresentationBuilder
{
    public:
        class ValueConverter : public boost::static_visitor<>
        {
            public:
                ValueConverter(OC::OCRepresentation &rep, const std::string &key)
                    : m_rep(rep), m_key(key) {}

                template <typename T>
                void operator ()(const T &value)
                {
                    m_rep.setValue(m_key, value);
                }

                void operator ()(const SimulatorResourceModel &value)
                {
                    OC::OCRepresentation ocRep;
                    for (auto &element : value.getValues())
                    {
                        ValueConverter visitor(ocRep, element.first);
                        boost::apply_visitor(visitor, element.second);
                    }

                    m_rep.setValue(m_key, ocRep);
                }

                template <typename T>
                void operator ()(const std::vector<T> &values)
                {
                    m_rep.setValue(m_key, values);
                }

                void operator ()(const std::vector<SimulatorResourceModel> &values)
                {
                    std::vector<OC::OCRepresentation> ocRepArray(values.size());
                    for (size_t i = 0; i < values.size(); i++)
                    {
                        for (auto &element : values[i].getValues())
                        {
                            ValueConverter visitor(ocRepArray[i], element.first);
                            boost::apply_visitor(visitor, element.second);
                        }
                    }

                    m_rep.setValue(m_key, ocRepArray);
                }

                template <typename T>
                void operator ()(const std::vector<std::vector<T>> &values)
                {
                    m_rep.setValue(m_key, values);
                }

                void operator ()(const std::vector<std::vector<SimulatorResourceModel>> &values)
                {
                    std::vector<std::vector<OC::OCRepresentation>> ocRepArray;
                    for (size_t i = 0; i < values.size(); i++)
                    {
                        for (size_t j = 0; j < values[i].size(); j++)
                        {
                            for (auto &element : values[i][j].getValues())
                            {
                                ValueConverter visitor(ocRepArray[i][j], element.first);
                                boost::apply_visitor(visitor, element.second);
                            }
                        }
                    }

                    m_rep.setValue(m_key, ocRepArray);
                }

                template <typename T>
                void operator ()(const std::vector<std::vector<std::vector<T>>> &values)
                {
                    m_rep.setValue(m_key, values);
                }

                void operator ()(const std::vector<std::vector<std::vector<SimulatorResourceModel>>> &values)
                {
                    std::vector<std::vector<std::vector<OC::OCRepresentation>>> ocRepArray;
                    for (size_t i = 0; i < values.size(); i++)
                    {
                        for (size_t j = 0; j < values[i].size(); j++)
                        {
                            for (size_t k = 0; j < values[i][j].size(); k++)
                            {
                                for (auto &element : values[i][j][k].getValues())
                                {
                                    ValueConverter visitor(ocRepArray[i][j][k], element.first);
                                    boost::apply_visitor(visitor, element.second);
                                }
                            }
                        }
                    }

                    m_rep.setValue(m_key, ocRepArray);
                }

            private:
                OC::OCRepresentation &m_rep;
                std::string m_key;
        };

        OC::OCRepresentation build(SimulatorResourceModel &model)
        {
            OC::OCRepresentation ocRep;
            for (auto &element : model.getValues())
            {
                ValueConverter visitor(ocRep, element.first);
                boost::apply_visitor(visitor, element.second);
            }

            return std::move(ocRep);
        }
};

// TODO: Class is very heavy, revisit again to clean
class SimulatorResourceModelBuilder
{
    public:
        SimulatorResourceModel build(const OC::OCRepresentation &ocRep)
        {
            SimulatorResourceModel resModel;
            handleRepresentationType(resModel, ocRep);
            return std::move(resModel);
        }

    private:
        void handleRepresentationType(SimulatorResourceModel &resModel,
                                      const OC::OCRepresentation &ocRep)
        {
            for (auto &ocAttribute : ocRep)
            {
                if (OC::AttributeType::Integer == ocAttribute.type())
                {
                    resModel.add<int>(ocAttribute.attrname(), ocAttribute.getValue<int>());
                }
                else if (OC::AttributeType::Double == ocAttribute.type())
                {
                    resModel.add<double>(ocAttribute.attrname(), ocAttribute.getValue<double>());
                }
                else if (OC::AttributeType::Boolean == ocAttribute.type())
                {
                    resModel.add<bool>(ocAttribute.attrname(), ocAttribute.getValue<bool>());
                }
                else if (OC::AttributeType::String == ocAttribute.type())
                {
                    resModel.add<std::string>(ocAttribute.attrname(), ocAttribute.getValue<std::string>());
                }
                else if (OC::AttributeType::OCRepresentation == ocAttribute.type())
                {
                    SimulatorResourceModel subResModel;
                    OC::OCRepresentation ocSubRep = ocAttribute.getValue<OC::OCRepresentation>();
                    handleRepresentationType(subResModel, ocSubRep);
                    resModel.add<SimulatorResourceModel>(ocAttribute.attrname(), subResModel);
                }
                else if (OC::AttributeType::Vector == ocAttribute.type())
                {
                    handleVectorType(resModel, ocAttribute);
                }
            }
        }

        void handleVectorType(SimulatorResourceModel &resModel,
                              const OC::OCRepresentation::AttributeItem &ocAttribute)
        {
            if (1 == ocAttribute.depth())
            {
                handleVectorTypeDepth1(resModel, ocAttribute);
            }
            else if (2 == ocAttribute.depth())
            {
                handleVectorTypeDepth2(resModel, ocAttribute);
            }
            else if (3 == ocAttribute.depth())
            {
                handleVectorTypeDepth3(resModel, ocAttribute);
            }
        }

        void handleVectorTypeDepth1(SimulatorResourceModel &resModel,
                                    const OC::OCRepresentation::AttributeItem &ocAttribute)
        {
            if (OC::AttributeType::Integer == ocAttribute.base_type())
            {
                resModel.add(ocAttribute.attrname(), ocAttribute.getValue<std::vector<int>>());
            }
            else if (OC::AttributeType::Double == ocAttribute.base_type())
            {
                resModel.add(ocAttribute.attrname(), ocAttribute.getValue<std::vector<double>>());
            }
            else if (OC::AttributeType::Boolean == ocAttribute.base_type())
            {
                resModel.add(ocAttribute.attrname(), ocAttribute.getValue<std::vector<bool>>());
            }
            else if (OC::AttributeType::String == ocAttribute.base_type())
            {
                resModel.add(ocAttribute.attrname(), ocAttribute.getValue<std::vector<std::string>>());
            }
            else if (OC::AttributeType::OCRepresentation == ocAttribute.base_type())
            {
                std::vector<OC::OCRepresentation> ocSubRepArray =
                    ocAttribute.getValue<std::vector<OC::OCRepresentation>>();

                std::vector<SimulatorResourceModel> subResModelArray(ocSubRepArray.size());
                for  (size_t i = 0; i < ocSubRepArray.size(); i++)
                {
                    handleRepresentationType(subResModelArray[i], ocSubRepArray[i]);
                }

                resModel.add<std::vector<SimulatorResourceModel>>(ocAttribute.attrname(), subResModelArray);
            }
        }

        void handleVectorTypeDepth2(SimulatorResourceModel &resModel,
                                    const OC::OCRepresentation::AttributeItem &ocAttribute)
        {
            if (OC::AttributeType::Integer == ocAttribute.base_type())
            {
                resModel.add(ocAttribute.attrname(), ocAttribute.getValue<std::vector<std::vector<int>>>());
            }
            else if (OC::AttributeType::Double == ocAttribute.base_type())
            {
                resModel.add(ocAttribute.attrname(), ocAttribute.getValue<std::vector<std::vector<double>>>());
            }
            else if (OC::AttributeType::Boolean == ocAttribute.base_type())
            {
                resModel.add(ocAttribute.attrname(), ocAttribute.getValue<std::vector<std::vector<bool>>>());
            }
            else if (OC::AttributeType::String == ocAttribute.base_type())
            {
                resModel.add(ocAttribute.attrname(), ocAttribute.getValue<std::vector<std::vector<std::string>>>());
            }
            else if (OC::AttributeType::OCRepresentation == ocAttribute.base_type())
            {
                std::vector<std::vector<OC::OCRepresentation>> ocSubRepArray =
                            ocAttribute.getValue<std::vector<std::vector<OC::OCRepresentation>>>();

                std::vector<std::vector<SimulatorResourceModel>> subResModelArray(ocSubRepArray.size());
                for  (size_t i = 0; i < ocSubRepArray.size(); i++)
                {
                    std::vector<SimulatorResourceModel> innerArray1(ocSubRepArray[i].size());
                    for  (size_t j = 0; j < ocSubRepArray[i].size(); j++)
                        handleRepresentationType(innerArray1[j], ocSubRepArray[i][j]);
                    subResModelArray[i] = innerArray1;
                }

                resModel.add<std::vector<std::vector<SimulatorResourceModel>>>(
                    ocAttribute.attrname(), subResModelArray);
            }
        }

        void handleVectorTypeDepth3(SimulatorResourceModel &resModel,
                                    const OC::OCRepresentation::AttributeItem &ocAttribute)
        {
            if (OC::AttributeType::Integer == ocAttribute.base_type())
            {
                resModel.add(ocAttribute.attrname(),
                             ocAttribute.getValue<std::vector<std::vector<std::vector<int>>>>());
            }
            else if (OC::AttributeType::Double == ocAttribute.base_type())
            {
                resModel.add(ocAttribute.attrname(),
                             ocAttribute.getValue<std::vector<std::vector<std::vector<double>>>>());
            }
            else if (OC::AttributeType::Boolean == ocAttribute.base_type())
            {
                resModel.add(ocAttribute.attrname(),
                             ocAttribute.getValue<std::vector<std::vector<std::vector<bool>>>>());
            }
            else if (OC::AttributeType::String == ocAttribute.base_type())
            {
                resModel.add(ocAttribute.attrname(),
                             ocAttribute.getValue<std::vector<std::vector<std::vector<std::string>>>>());
            }
            else if (OC::AttributeType::OCRepresentation == ocAttribute.base_type())
            {
                std::vector<std::vector<std::vector<OC::OCRepresentation>>> ocSubRepArray =
                    ocAttribute.getValue<std::vector<std::vector<std::vector<OC::OCRepresentation>>>>();

                std::vector<std::vector<std::vector<SimulatorResourceModel>>> subResModelArray(
                    ocSubRepArray.size());
                for  (size_t i = 0; i < ocSubRepArray.size(); i++)
                {
                    std::vector<std::vector<SimulatorResourceModel>> innerArray1(ocSubRepArray[i].size());
                    for  (size_t j = 0; j < ocSubRepArray[i].size(); j++)
                    {
                        std::vector<SimulatorResourceModel> innerArray2(ocSubRepArray[i][j].size());
                        for  (size_t k = 0; k < ocSubRepArray[i][j].size(); k++)
                        {
                            handleRepresentationType(innerArray2[k], ocSubRepArray[i][j][k]);
                        }
                        innerArray1[j] = innerArray2;
                    }
                    subResModelArray[i] = innerArray1;
                }

                resModel.add<std::vector<std::vector<std::vector<SimulatorResourceModel>>>>(
                    ocAttribute.attrname(), subResModelArray);
            }
        }
};

class RangeValidator : public boost::static_visitor<bool>
{
    public:
        RangeValidator(SimulatorResourceModel::AttributeProperty &property) :
            m_property(property) {}

        bool operator ()(const int &value)
        {
            if (checkRange(value) || checkSupportedValueSet(value))
                return true;
            return false;
        }

        bool operator ()(const double &value)
        {
            if (checkRange(value) || checkSupportedValueSet(value))
                return true;
            return false;
        }

        bool operator ()(const bool &value)
        {
            if (checkSupportedValueSet(value))
                return true;
            return false;
        }

        bool operator ()(const std::string &value)
        {
            int len = value.length();
            if (checkRange(len) || checkSupportedValueSet(value))
                return true;
            return false;
        }

        bool operator ()(const SimulatorResourceModel &)
        {
            return true;
        }

        template <typename T>
        bool operator ()(const std::vector<T> &values)
        {
            // Verify array property
            int length = values.size();
            if (!checkRange(length))
                return false;

            // Verify array element property
            if (!m_property.getChildProperty())
                return true;

            m_property = *(m_property.getChildProperty());
            for (int index = 0; index < length; index++)
            {
                if (!operator ()(values[index]))
                    return false;
            }

            return true;
        }

        template <typename T>
        bool operator ()(const std::vector<std::vector<T>> &values)
        {
            // Verify array property
            int length = values.size();
            if (!checkRange(length))
                return false;

            // Verify array element property
            if (!m_property.getChildProperty())
                return true;

            m_property = *(m_property.getChildProperty());
            for (int index = 0; index < length; index++)
            {
                if (!operator ()(values[index]))
                    return false;
            }

            return true;
        }

        template <typename T>
        bool operator ()(const std::vector<std::vector<std::vector<T>>> &values)
        {
            // Verify array property
            int length = values.size();
            if (!checkRange(length))
                return false;

            // Verify array element property
            if (!m_property.getChildProperty())
                return true;

            m_property = *(m_property.getChildProperty());
            for (int index = 0; index < length; index++)
            {
                if (!operator ()(values[index]))
                    return false;
            }

            return true;
        }

    private:
        inline bool isValidProperty()
        {
            if (SimulatorResourceModel::AttributeProperty::Type::UNKNOWN !=
                m_property.type())
                return true;
            return false;
        }

        bool checkRange(const double &value)
        {
            if (SimulatorResourceModel::AttributeProperty::Type::RANGE ==
                m_property.type())
            {
                if (value >= m_property.min() && value <= m_property.max())
                    return true;
            }

            return false;
        }

        template <typename T>
        bool checkSupportedValueSet(const T &value)
        {
            if (SimulatorResourceModel::AttributeProperty::Type::VALUE_SET ==
                m_property.type())
            {
                for (auto &element : m_property.valueSet())
                {
                    T allowedValue = boost::get<T>(element);
                    if (allowedValue == value)
                        return true;
                }
            }

            return false;
        }

        SimulatorResourceModel::AttributeProperty m_property;
};

class ToStringConverter
{
    public:
        class ValueVisitor : public boost::static_visitor<std::string>
        {
            public:

                // Single basic type value conversion to string
                template <typename T>
                std::string operator ()(const T &value)
                {
                    try
                    {
                        return boost::lexical_cast<std::string>(value);
                    }
                    catch (const boost::bad_lexical_cast &e)
                    {
                        return "CONVERSION_FAILED!";
                    }
                }

                // SimulatorResourceModel conversion to string
                std::string operator ()(const SimulatorResourceModel &value)
                {
                    std::ostringstream out;
                    out << "{ ";
                    for (auto &element : value.getValues())
                    {
                        out << "\"" << element.first << "\" : ";

                        ValueVisitor visitor;
                        out << boost::apply_visitor(visitor, element.second);

                        out << ", ";
                    }
                    out << "}";
                    return out.str();
                }

                template <typename T>
                std::string operator ()(const std::vector<T> &values)
                {
                    std::ostringstream out;
                    out << "[ ";

                    for (size_t i = 0; i < values.size(); i++)
                    {
                        out << operator ()(values[i]);
                        out << " ";
                    }

                    out << "]";
                    return out.str();
                }

                template <typename T>
                std::string operator ()(const std::vector<std::vector<T>> &values)
                {
                    std::ostringstream out;
                    out << "[ ";

                    for (size_t i = 0; i < values.size(); i++)
                    {
                        out << operator ()(values[i]);
                        out << " ";
                    }

                    out << "]";
                    return out.str();
                }

                template <typename T>
                std::string operator ()(const std::vector<std::vector<std::vector<T>>> &values)
                {
                    std::ostringstream out;
                    out << "[ ";

                    for (size_t i = 0; i < values.size(); i++)
                    {
                        out << operator ()(values[i]);
                        out << " ";
                    }

                    out << "]";
                    return out.str();
                }
        };

        std::string getStringRepresentation(const SimulatorResourceModel &resModel)
        {
            ValueVisitor visitor;
            SimulatorResourceModel::ValueVariant value = resModel;
            return boost::apply_visitor(visitor, value);
        }

        std::string getStringRepresentation(const SimulatorResourceModel::ValueVariant &value)
        {
            ValueVisitor visitor;
            return boost::apply_visitor(visitor, value);
        }
};

SimulatorResourceModel::TypeInfo::TypeInfo(
    SimulatorResourceModel::ValueType type = SimulatorResourceModel::ValueType::UNKNOWN,
    SimulatorResourceModel::ValueType baseType = SimulatorResourceModel::ValueType::UNKNOWN,
    int depth = 0)
    :   m_type (type), m_baseType(baseType), m_depth(depth) {}

SimulatorResourceModel::ValueType SimulatorResourceModel::TypeInfo::type() const
{
    return m_type;
}

SimulatorResourceModel::ValueType SimulatorResourceModel::TypeInfo::baseType() const
{
    return m_baseType;
}

int SimulatorResourceModel::TypeInfo::depth() const
{
    return m_depth;
}

bool SimulatorResourceModel::TypeInfo::operator==(
    const SimulatorResourceModel::TypeInfo &rhs ) const
{
    if (m_type == rhs.type() && m_baseType == rhs.baseType()
        && m_depth == rhs.depth())
        return true;
    return false;
}

bool SimulatorResourceModel::TypeInfo::operator!=(
    const SimulatorResourceModel::TypeInfo &rhs ) const
{
    if (m_type != rhs.type() || m_baseType != rhs.baseType()
        || m_depth != rhs.depth())
        return true;
    return false;
}

SimulatorResourceModel::AttributeProperty::AttributeProperty()
    :   m_type(SimulatorResourceModel::AttributeProperty::Type::UNKNOWN),
        m_min(INT_MIN),
        m_max(INT_MAX) {}

SimulatorResourceModel::AttributeProperty::AttributeProperty(double min, double max)
    :   m_type(SimulatorResourceModel::AttributeProperty::Type::RANGE),
        m_min(min),
        m_max(max) {}

SimulatorResourceModel::AttributeProperty::AttributeProperty(
    const std::vector<int> &valueSet)
    :   m_type(SimulatorResourceModel::AttributeProperty::Type::VALUE_SET),
        m_min(INT_MIN),
        m_max(INT_MAX),
        m_valueSet(valueSet.begin(), valueSet.end()) {}

SimulatorResourceModel::AttributeProperty::AttributeProperty(
    const std::vector<double> &valueSet)
    :   m_type(SimulatorResourceModel::AttributeProperty::Type::VALUE_SET),
        m_min(INT_MIN),
        m_max(INT_MAX),
        m_valueSet(valueSet.begin(), valueSet.end()) {}

SimulatorResourceModel::AttributeProperty::AttributeProperty(
    const std::vector<bool> &valueSet)
    :   m_type(SimulatorResourceModel::AttributeProperty::Type::VALUE_SET),
        m_min(INT_MIN),
        m_max(INT_MAX),
        m_valueSet(valueSet.begin(), valueSet.end()) {}

SimulatorResourceModel::AttributeProperty::AttributeProperty(
    const std::vector<std::string> &valueSet)
    :   m_type(SimulatorResourceModel::AttributeProperty::Type::VALUE_SET),
        m_min(INT_MIN),
        m_max(INT_MAX),
        m_valueSet(valueSet.begin(), valueSet.end()) {}

SimulatorResourceModel::AttributeProperty::AttributeProperty(
    const std::vector<ValueVariant> &valueSet)
    :   m_type(SimulatorResourceModel::AttributeProperty::Type::VALUE_SET),
        m_min(INT_MIN),
        m_max(INT_MAX),
        m_valueSet(valueSet.begin(), valueSet.end()) {}

SimulatorResourceModel::AttributeProperty::Type
SimulatorResourceModel::AttributeProperty::type() const
{
    return m_type;
}

double SimulatorResourceModel::AttributeProperty::min() const
{
    return m_min;
}

double SimulatorResourceModel::AttributeProperty::max() const
{
    return m_max;
}

int SimulatorResourceModel::AttributeProperty::valueSetSize() const
{
    return m_valueSet.size();
}

std::vector<SimulatorResourceModel::ValueVariant>
SimulatorResourceModel::AttributeProperty::valueSet() const
{
    return m_valueSet;
}

std::string SimulatorResourceModel::AttributeProperty::valueSetToString() const
{
    std::ostringstream out;
    out << "[ ";
    for (auto &value : m_valueSet)
    {
        out << ToStringConverter().getStringRepresentation(value);
        out << ", ";
    }
    out << "]";
    return out.str();
}

void SimulatorResourceModel::AttributeProperty::setChildProperty(AttributeProperty &childProperty)
{
    m_childProperty.reset(new SimulatorResourceModel::AttributeProperty(childProperty));
}

std::shared_ptr<SimulatorResourceModel::AttributeProperty>
SimulatorResourceModel::AttributeProperty::getChildProperty()
{
    return m_childProperty;
}

std::string SimulatorResourceModel::Attribute::getName() const
{
    return m_name;
}

SimulatorResourceModel::TypeInfo SimulatorResourceModel::Attribute::getType() const
{
    if (m_value)
    {
        AttributeTypeVisitor typeVisitor;
        boost::apply_visitor(typeVisitor, *(m_value.get()));
        SimulatorResourceModel::TypeInfo typeInfo(typeVisitor.m_type, typeVisitor.m_baseType,
                typeVisitor.m_depth);
        return typeInfo;
    }

    return SimulatorResourceModel::TypeInfo();
}

const SimulatorResourceModel::AttributeProperty &
SimulatorResourceModel::Attribute::getProperty() const
{
    return m_property;
}

SimulatorResourceModel::AttributeProperty &SimulatorResourceModel::Attribute::getProperty()
{
    return m_property;
}

void SimulatorResourceModel::Attribute::setName(const std::string &name)
{
    m_name = name;
}

void SimulatorResourceModel::Attribute::setProperty(
    const SimulatorResourceModel::AttributeProperty &property)
{
    m_property = property;
}

std::string SimulatorResourceModel::Attribute::toString() const
{
    return ToStringConverter().getStringRepresentation(*m_value);
}

bool SimulatorResourceModel::add(const SimulatorResourceModel::Attribute &attribute)
{
    if (attribute.getName().empty())
        return false;

    if (m_attributes.end() == m_attributes.find(attribute.getName()))
    {
        m_attributes[attribute.getName()] = attribute.getValue();
        if (SimulatorResourceModel::AttributeProperty::Type::UNKNOWN !=
            attribute.getProperty().type())
        {
            m_attrProperties[attribute.getName()] = attribute.getProperty();
        }
        return true;
    }

    return false;
}

bool SimulatorResourceModel::updateValue(const SimulatorResourceModel::Attribute &attribute,
        bool overwrite)
{
    return setAttributeValue(attribute.getName(), attribute.getValue(), false, overwrite);
}

bool SimulatorResourceModel::containsAttribute(const std::string &key)
{
    if (m_attributes.end() != m_attributes.find(key))
        return true;
    return false;
}

bool SimulatorResourceModel::setAttributeProperty(const std::string &key,
        const SimulatorResourceModel::AttributeProperty &property)
{
    if (!key.empty() && m_attributes.end() != m_attributes.find(key))
    {
        m_attrProperties[key] = property;
        return true;
    }

    return false;
}

bool SimulatorResourceModel::getAttributeProperty(const std::string &key,
        SimulatorResourceModel::AttributeProperty &property)
{
    if (m_attrProperties.end() != m_attrProperties.find(key))
    {
        property = m_attrProperties[key];
        return true;
    }

    return false;
}

int SimulatorResourceModel::size() const
{
    return m_attributes.size();
}

SimulatorResourceModel::TypeInfo SimulatorResourceModel::getType(const std::string &key)
{
    if (m_attributes.end() != m_attributes.find(key))
    {
        return getTypeInfo(m_attributes.find(key)->second);
    }

    return SimulatorResourceModel::TypeInfo();
}

SimulatorResourceModel::TypeInfo SimulatorResourceModel::getTypeInfo(const ValueVariant &value)
const
{
    AttributeTypeVisitor typeVisitor;
    boost::apply_visitor(typeVisitor, value);
    SimulatorResourceModel::TypeInfo typeInfo(typeVisitor.m_type, typeVisitor.m_baseType,
            typeVisitor.m_depth);
    return typeInfo;
}

std::map<std::string, SimulatorResourceModel::ValueVariant> SimulatorResourceModel::getValues()
const
{
    return m_attributes;
}

std::map<std::string, SimulatorResourceModel::Attribute> SimulatorResourceModel::getAttributes()
{
    std::map<std::string, SimulatorResourceModel::Attribute> attributes;
    for (auto &element : m_attributes)
    {
        SimulatorResourceModel::Attribute attribute(element.first);
        attribute.setValue(element.second);

        if (m_attrProperties.end() != m_attrProperties.find(element.first))
            attribute.setProperty(m_attrProperties[element.first]);

        attributes[element.first] = attribute;
    }

    return attributes;
}

bool SimulatorResourceModel::getAttribute(const std::string &key,
        SimulatorResourceModel::Attribute &attribute)
{
    if (m_attributes.end() != m_attributes.find(key))
    {
        attribute.setName(m_attributes.find(key)->first);
        attribute.setValue(m_attributes.find(key)->second);

        if (m_attrProperties.end() != m_attrProperties.find(key))
            attribute.setProperty(m_attrProperties[key]);

        return true;
    }

    return false;
}

bool SimulatorResourceModel::removeAttribute(const std::string &key)
{
    if (m_attributes.end() == m_attributes.find(key))
    {
        return false;
    }

    m_attributes.erase(m_attributes.find(key));
    return true;
}

bool SimulatorResourceModel::update(OC::OCRepresentation &ocRep)
{
    SimulatorResourceModel resModel = SimulatorResourceModel::build(ocRep);
    return update(resModel);
}

OC::OCRepresentation SimulatorResourceModel::getOCRepresentation()
{
    OCRepresentationBuilder ocRepBuilder;
    return ocRepBuilder.build(*this);
}

SimulatorResourceModel SimulatorResourceModel::build(const OC::OCRepresentation &ocRep)
{
    SimulatorResourceModelBuilder resModelBuilder;
    return resModelBuilder.build(ocRep);
}

bool SimulatorResourceModel::update(SimulatorResourceModel &resModel)
{
    if (false == match(resModel))
    {
        return false;
    }

    for (auto &element : resModel.getValues())
    {
        if (m_attributes.end() != m_attributes.find(element.first))
        {
            m_attributes[element.first] = element.second;
        }
    }

    return true;
}

bool SimulatorResourceModel::setAttributeValue(const std::string &key,
        const ValueVariant &newValue, bool create, bool overwrite)
{
    if (key.empty())
    {
        OIC_LOG(ERROR, TAG, "key is empty!");
        return false;
    }

    /*
     *   Add new entry to attributes map table if key doest not exist,
     *   othewise check the value update policy before setting value.
     */
    if (m_attributes.end() == m_attributes.find(key) && create)
    {
        m_attributes[key] = newValue;
    }
    else
    {
        if (true == match(key, newValue))
        {
            m_attributes[key] = newValue;
        }
        else if (overwrite)
        {
            m_attributes[key] = newValue;
            m_attrProperties.erase(m_attrProperties.find(key));
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool SimulatorResourceModel::match(const std::string &key,
                                   const SimulatorResourceModel::ValueVariant &newValue)
{
    if (m_attributes.end() != m_attributes.find(key))
    {
        // Check the type of values
        if (getTypeInfo(m_attributes[key]) != getTypeInfo(newValue))
        {
            return false;
        }

        // Check the value based on attribute property
        SimulatorResourceModel::AttributeProperty prop;
        if (getAttributeProperty(key, prop)
            && SimulatorResourceModel::AttributeProperty::Type::UNKNOWN != prop.type())
        {
            RangeValidator rangeValidator(prop);
            return boost::apply_visitor(rangeValidator, newValue);
        }

        return true;
    }

    return false;
}

bool SimulatorResourceModel::match(const SimulatorResourceModel &resModel, bool strict)
{
    for (auto &element : resModel.getValues())
    {
        // Attribute presence check
        if (m_attributes.end() == m_attributes.find(element.first) && !strict)
        {
            continue;
        }
        else if (strict)
        {
            return false;
        }

        // Attribute value type check
        if (getTypeInfo(m_attributes[element.first]) != getTypeInfo(element.second))
        {
            return false;
        }

        SimulatorResourceModel::AttributeProperty prop;
        if (getAttributeProperty(element.first, prop)
            && SimulatorResourceModel::AttributeProperty::Type::UNKNOWN != prop.type())
        {
            RangeValidator rangeValidator(prop);
            if (false == boost::apply_visitor(rangeValidator, element.second))
                return false;
        }
    }

    return true;
}

std::string SimulatorResourceModel::toString() const
{
    return ToStringConverter().getStringRepresentation(*this);
}
