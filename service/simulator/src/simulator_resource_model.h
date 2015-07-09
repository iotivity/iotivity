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

#ifndef SIMULATOR_RESOURCE_ATTRIBUTE_H_
#define SIMULATOR_RESOURCE_ATTRIBUTE_H_

#include <string>
#include <vector>
#include "OCPlatform.h"
#include "OCApi.h"

class SimulatorResource;
/**
 * @class   SimulatorResourceModel
 * @brief   This class provides a set of functions for accessing and manipulating the resource model.
 */
class SimulatorResourceModel
{
        friend class SimulatorResource;

    public:
        /**
          * @class   Attribute
          * @brief   This class represents a resource attribute whose values can be generic.
          */
        class Attribute
        {
            public:
                typedef boost::variant <
                int,
                double,
                std::string
                > ValueVariant;

                Attribute() = default;
                Attribute(const std::string &attrName) : m_name(attrName) {}

                std::string getName(void) const;
                void setName(const std::string &name);

                template <typename T>
                T getValue() const
                {
                    T val = T();
                    return boost::get<T>(m_value);
                }

                ValueVariant &getValue()
                {
                    return m_value;
                }

                int getValueType() const
                {
                    return m_value.which();
                }

                template <typename T>
                void setValue(const T &value)
                {
                    m_value = value;
                }

                void setFromAllowedValue(const int allowedValueIndex);

                void getRange(int &min, int &max) const;

                void setRange(const int &min, const int &max);

                template <typename T>
                bool setAllowedValues(const std::vector<T> &values)
                {
                    ValueVariant temp = values.at(0);
                    if (temp.which() != m_value.which())
                    {
                        return false;
                    }

                    m_allowedValues.addValues(values);
                    return true;
                }

                /**
                  * This method is used to get the size of the allowed values.
                  *
                  * @return Size of the allowed values
                  */
                int getAllowedValuesSize() const;

                /**
                  * This method is used to get the string representation of the value.
                  *
                  * @return Attribute's value as a string
                  */
                std::string valueToString() const;

                /**
                  * This method is used to get the string representation of all the allowed values.
                  *
                  * @return All allowed values as a string
                  */
                std::string allowedValuesToString() const;
                std::vector<std::string> allowedValuesToVectorString() const;

                void addValuetoRepresentation(OC::OCRepresentation &rep, const std::string &key);

                bool compare(Attribute &attribute);

                std::vector<ValueVariant> getAllowedValues();

                int getUpdateFrequencyTime() {return m_updateInterval;}
                void setUpdateFrequencyTime(int interval) {m_updateInterval = interval;}

            private:
                class AllowedValues
                {
                    public:
                        template <typename T>
                        void addValue(const T &value)
                        {
                            ValueVariant temp = value;
                            m_values.push_back(temp);
                        }

                        template <typename T>
                        void addValues(const std::vector<T> &values)
                        {
                            for (auto value : values)
                            {
                                ValueVariant vValue = value;
                                m_values.push_back(vValue);
                            }
                        }

                        ValueVariant &at(int index);
                        int size() const;
                        std::string toString() const;
                        std::vector<std::string> toVectorString() const;
                        std::vector<ValueVariant> getValues();
                    private:
                        std::vector<ValueVariant> m_values;
                };

                std::string m_name;
                ValueVariant m_value;
                int m_max;
                int m_min;
                AllowedValues m_allowedValues;
                int m_updateInterval;
        };

        typedef enum { POST, PUT, DELETE } UpdateType;

        /**
         * This method is used to get the number of attributes in the resource.
         *
         * @return Count of attributes
         */
        int size() const { return m_attributes.size(); }

        /**
          * This method is used to get the value of an attribute.
          *
          * @param attrName - Attribute name
          * @param value - Attribute value
          *
          * @return Boolean, true if attribute exists, otherwise false.
          */
        bool getAttribute(const std::string &attrName, Attribute &value);

        /**
          * This method is used to get the entire list of attributes in the form of key-value pair.
          * Attribute name is the key and an instance of Attribute is the value.
          *
          * @return A map of all the attributes
          */
        std::map<std::string, Attribute> getAttributes() const;

    private:
        template <typename T>
        void addAttribute(const std::string &attrName, const T &attrValue)
        {
            if (m_attributes.end() == m_attributes.find(attrName))
            {
                m_attributes[attrName] = Attribute(attrName);
                m_attributes[attrName].setValue(attrValue);
            }
        }

        void setRange(const std::string &attrName, const int min, const int max);

        template <typename T>
        void setAllowedValues(const std::string &attrName, const std::vector<T> &values)
        {
            if (m_attributes.end() != m_attributes.find(attrName))
                m_attributes[attrName].setAllowedValues(values);
        }

        void setUpdateInterval(const std::string &attrName, int interval);

        template <typename T>
        void updateAttribute(const std::string &attrName, const T &value)
        {
            if (m_attributes.end() != m_attributes.find(attrName))
                m_attributes[attrName].setValue(value);
        }

        void updateAttributeFromAllowedValues(const std::string &attrName, const int allowedValueIndex);

        void removeAttribute(const std::string &attrName);

        OC::OCRepresentation getOCRepresentation();

        bool update(OC::OCRepresentation &ocRep, UpdateType type);

        bool update(SimulatorResourceModel &repModel, UpdateType type);

        std::map<std::string, Attribute> m_attributes;
};

#endif
