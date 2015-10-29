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

#include "OCPlatform.h"
#include <map>

class OCRepresentationBuilder;
class ToStringConverter;

/**
 * @class   SimulatorResourceModel
 * @brief   This class provides a set of functions for accessing and manipulating the resource model.
 */
class SimulatorResourceModel
{
    public:
        friend class OCRepresentationBuilder;
        friend class ToStringConverter;

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
        > ValueVariant;

        enum class ValueType
        {
            UNKNOWN,
            INTEGER,
            DOUBLE,
            BOOLEAN,
            STRING,
            RESOURCE_MODEL,
            VECTOR
        };

        class TypeInfo
        {
            public:
                TypeInfo(ValueType, ValueType, int);
                TypeInfo(const TypeInfo &) = default;
                TypeInfo &operator=(const TypeInfo &) = default;
                TypeInfo(TypeInfo &&) = default;
                TypeInfo &operator=(TypeInfo &&) = default;

                ValueType type() const;
                ValueType baseType() const;
                int depth() const;
                bool operator ==(const TypeInfo &) const;
                bool operator !=(const TypeInfo &) const;

            private:
                ValueType m_type;
                ValueType m_baseType;
                int m_depth;
        };

        class AttributeProperty
        {
            public:
                enum class Type
                {
                    UNKNOWN,
                    RANGE,
                    VALUE_SET
                };

                AttributeProperty();
                AttributeProperty(const AttributeProperty &) = default;
                AttributeProperty &operator=(const AttributeProperty &) = default;
                AttributeProperty(AttributeProperty &&) = default;
                AttributeProperty &operator=(AttributeProperty &&) = default;

                explicit AttributeProperty(int min, int max);
                explicit AttributeProperty(const std::vector<int> &valueSet);
                explicit AttributeProperty(const std::vector<double> &valueSet);
                explicit AttributeProperty(const std::vector<bool> &valueSet);
                explicit AttributeProperty(const std::vector<std::string> &valueSet);
                explicit AttributeProperty(const std::vector<ValueVariant> &valueSet);

                Type type() const;
                int min() const;
                int max() const;
                int valueSetSize() const;
                std::vector<ValueVariant> valueSet() const;
                std::string valueSetToString() const;

            private:
                Type m_type;
                int m_min;
                int m_max;
                std::vector<ValueVariant> m_valueSet;
        };

        class Attribute
        {
            public:
                Attribute(const std::string &name) : m_name(name) {}
                Attribute() = default;
                Attribute(const Attribute &) = default;
                Attribute &operator=(const Attribute &) = default;
                Attribute(Attribute &&) = default;
                Attribute &operator=(Attribute &&) = default;

                std::string getName() const;
                TypeInfo getType() const;
                const AttributeProperty &getProperty() const;
                AttributeProperty &getProperty();

                void setName(const std::string &);
                void setProperty(const AttributeProperty &);

                template <typename T>
                void setValue(const T &value)
                {
                    m_value = std::make_shared<ValueVariant>(value);
                }

                ValueVariant getValue() const { return *m_value; }

                std::string toString() const;

            private:
                std::string m_name;
                std::shared_ptr<ValueVariant> m_value;
                AttributeProperty m_property;
        };

        SimulatorResourceModel() = default;
        SimulatorResourceModel(const SimulatorResourceModel &) = default;
        SimulatorResourceModel &operator=(const SimulatorResourceModel &) = default;
        SimulatorResourceModel(SimulatorResourceModel &&) = default;
        SimulatorResourceModel &operator=(SimulatorResourceModel && ) = default;

        template <typename T>
        bool add(const std::string &key, T value)
        {
            ValueVariant newValue = value;
            return setAttributeValue(key, newValue, true, false);
        }

        bool add(const Attribute &attribute);

        template <typename T>
        T get(const std::string &key) const
        {
            T val = T();
            auto x = m_attributes.find(key);
            if (x != m_attributes.end())
            {
                val = boost::get<T>(x->second);
            }
            return val;
        }

        template <typename T>
        bool updateValue(const std::string &key, T value, bool forcewrite = false)
        {
            ValueVariant newValue = value;
            return setAttributeValue(key, newValue, false, forcewrite);
        }

        bool updateValue(const Attribute &attribute, bool forcewrite = false);

        bool containsAttribute(const std::string &key);

        bool setAttributeProperty(const std::string &key, const AttributeProperty &property);

        bool getAttributeProperty(const std::string &key, AttributeProperty &property);

        int size() const;

        TypeInfo getType(const std::string &key);

        std::map<std::string, Attribute> getAttributes();

        bool getAttribute(const std::string &key, Attribute &attribute);

        bool removeAttribute(const std::string &key);

        bool update(OC::OCRepresentation &ocRep);

        OC::OCRepresentation getOCRepresentation();

        bool match(const SimulatorResourceModel &resModel, bool strict = false);

        std::string toString() const;

        static SimulatorResourceModel build(const OC::OCRepresentation &ocRep);

    private:
        TypeInfo getTypeInfo(const ValueVariant &value) const;
        bool setAttributeValue(const std::string &key, const ValueVariant &newValue,
                               bool create, bool overwrite);
        bool match(const std::string &key, const ValueVariant &newValue);
        bool update(SimulatorResourceModel &resModel);
        std::map<std::string, ValueVariant> getValues() const;

        std::map<std::string, ValueVariant> m_attributes;
        std::map<std::string, AttributeProperty> m_attrProperties;
};

typedef std::shared_ptr<SimulatorResourceModel> SimulatorResourceModelSP;

#endif
