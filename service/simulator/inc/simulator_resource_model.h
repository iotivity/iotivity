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

/**
 * @file   simulator_resource_model.h
 *
 * @brief   This file contains a class which represents the resource model for simulator
 *             resources and provides a set of functions for updating the model.
 */

#ifndef SIMULATOR_RESOURCE_MODEL_H_
#define SIMULATOR_RESOURCE_MODEL_H_

#include <map>
#include <set>

#include "OCPlatform.h"

class SimulatorResourceModel;
typedef boost::variant <
int,
double,
bool,
std::string,
SimulatorResourceModel,

std::vector<int>,
std::vector<double>,
std::vector<bool>,
std::vector<std::string>,
std::vector<SimulatorResourceModel>,

std::vector<std::vector<int>>,
std::vector<std::vector<double>>,
std::vector<std::vector<bool>>,
std::vector<std::vector<std::string>>,
std::vector<std::vector<SimulatorResourceModel>>,

std::vector<std::vector<std::vector<int>>>,
std::vector<std::vector<std::vector<double>>>,
std::vector<std::vector<std::vector<bool>>>,
std::vector<std::vector<std::vector<std::string>>>,
std::vector<std::vector<std::vector<SimulatorResourceModel>>>
> AttributeValueVariant;

enum class AttributeValueType
{
    UNKNOWN,         /**< unknown value type */
    INTEGER,         /**< integer value type */
    DOUBLE,          /**< double value type */
    BOOLEAN,         /**< boolean value type */
    STRING,          /**< string value type */
    RESOURCE_MODEL,  /**< resource model type */
    VECTOR           /**< vector type */
};

class OCRepresentationBuilder;
class ToStringConverter;
class AttributeProperty;

/**
 * @class   SimulatorResourceModel
 * @brief   This class provides a set of functions for accessing and manipulating the resource model.
 */
class SimulatorResourceModel
{
    public:
        friend class OCRepresentationBuilder;
        friend class ToStringConverter;

        class TypeInfo
        {
            public:
                TypeInfo(AttributeValueType type = AttributeValueType::UNKNOWN,
                         AttributeValueType baseType = AttributeValueType::UNKNOWN, int depth = 0);
                AttributeValueType type() const;
                AttributeValueType baseType() const;
                int depth() const;
                bool operator ==(const TypeInfo &rhs) const;

            private:
                AttributeValueType m_type;
                AttributeValueType m_baseType;
                int m_depth;
        };

        /**
         * add the value
         * @param[in] name   find the name
         * @param[in] value  add the attribute value for the given name
         * @return boolean value
         */
        template <typename T>
        bool add(const std::string &name, T value)
        {
            if (!name.empty() && m_attributes.end() == m_attributes.find(name))
            {
                AttributeValueVariant newValue = value;
                m_attributes[name] = value;
                return true;
            }
            return false;
        }

        /**
         * get the value
         * @param[in] name get the value for given name
         * @return value
         */
        template <typename T>
        T get(const std::string &name) const
        {
            T val = T();
            auto x = m_attributes.find(name);
            if (x != m_attributes.end())
            {
                try
                {
                    val = boost::get<T>(x->second);
                }
                catch (boost::bad_get &e)
                {
                    return val;
                }
            }
            return val;
        }

        /**
         * update the value
         * @param[in] name   name to find
         * @param[in] value  update the value
         * @return boolean value
         */
        template <typename T>
        bool update(const std::string &name, T value)
        {
            AttributeValueVariant newValue = value;
            return updateValue(name, newValue);
        }

        /**
         * remove the value
         * @param[in] name    name to remove
         * @return boolean value
         */
        bool remove(const std::string &name);

        bool contains(const std::string &name) const;

        size_t size() const;

        /**
         * get the type
         * @param[in] name   get the type of given name
         * @return type of name
         */
        TypeInfo getType(const std::string &name) const;

        /**
         * get attribute values
         * @return attribute value
         */
        std::map<std::string, AttributeValueVariant> getAttributeValues() const;

        /**
         * get attribute values using name value
         * @param name   attribute name
         * @return attribute value
         */
        AttributeValueVariant getAttributeValue(const std::string &name) const;

        std::set<std::string> getAttributeNameSet() const;

        OC::OCRepresentation asOCRepresentation() const;

        std::string asString() const;

        static SimulatorResourceModel build(const OC::OCRepresentation &ocRep);

    private:
        TypeInfo getTypeInfo(const AttributeValueVariant &value) const;
        bool updateValue(const std::string &name, const AttributeValueVariant &value);

        std::map<std::string, AttributeValueVariant> m_attributes;
};

/**
 * @class SimulatorResourceAttribute
 */
class SimulatorResourceAttribute
{
    public:
        SimulatorResourceAttribute() = default;
        SimulatorResourceAttribute(const std::string &name);
        SimulatorResourceAttribute(const std::string &name, const AttributeValueVariant &value);

        /**
         * get the resource name
         * @return resource name
         */
        std::string getName() const;
        /**
         * get the type
         * @return resource type
         */
        const SimulatorResourceModel::TypeInfo getType() const;
        /**
         * get the attribute property
         * @return attribute property
         */
        std::shared_ptr<AttributeProperty> getProperty() const;
        /**
         * get the attribute value
         * @return attribute value
         */
        AttributeValueVariant getValue() const;

        /**
         * set the atribute name
         * @param[in] name   set the name
         */
        void setName(const std::string &name);
        /**
         * set the attribute property
         * @param[in] property   property to set
         */
        void setProperty(const std::shared_ptr<AttributeProperty> &property);

        /**
         * set attribute value
         * @param[in] value   value to set
         */
        template <typename T>
        void setValue(const T &value)
        {
            m_value = std::make_shared<AttributeValueVariant>(value);
        }

        std::string asString() const;

    private:
        std::string m_name;
        std::shared_ptr<AttributeValueVariant> m_value;
        std::shared_ptr<AttributeProperty> m_property;
};

typedef std::shared_ptr<SimulatorResourceModel> SimulatorResourceModelSP;

#endif
