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
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef RESOURCE_PROPERTIES_H_
#define RESOURCE_PROPERTIES_H_

#include <string>
#include <vector>
#include <map>
#include "cJSON.h"
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>

namespace RAML
{

    class ResourceProperties
    {
        public:
            ResourceProperties() {}
            ResourceProperties(cJSON *cjson) : m_cjson(cjson) { readJson();}

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

                    inline std::string getName(void) const { return m_name; }
                    inline void setName(const std::string &name) { m_name = name;}

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
                    int getValueInt()
                    {
                        return boost::lexical_cast<int> (m_value);
                    }
                    std::string getValueString()
                    {
                        return boost::lexical_cast<std::string> (m_value);
                    }

                    template <typename T>
                    void setValue(const T &value)
                    {
                        m_value = value;
                    }

                    inline void getRange(int &min, int &max) const
                    {
                        min = m_min;
                        max = m_max;
                    }

                    inline void setRange(const int &min, const int &max)
                    {
                        m_min = min;
                        m_max = max;
                    }

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
                    inline int getAllowedValuesSize() const
                    {
                        return m_allowedValues.size();
                    }

                    inline std::vector<ValueVariant> getAllowedValues()
                    {
                        return m_allowedValues.getValues();
                    }

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

                            inline ValueVariant &at(int index)
                            {
                                return m_values.at(index);
                            }
                            inline int size() const
                            {
                                return m_values.size();
                            }

                            inline std::vector<ValueVariant> getValues()
                            {
                                return m_values;
                            }

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
            int size() const { return m_attributes.size(); }
            inline bool getAttribute(const std::string &attrName, Attribute &value)
            {
                if (m_attributes.end() != m_attributes.find(attrName))
                {
                    value = m_attributes[attrName];
                    return true;
                }

                return false;
            }

            inline std::map<std::string, Attribute> getAttributes() const
            {
                return m_attributes;
            }


            template <typename T>
            void addAttribute(const std::string &attrName, const T &attrValue)
            {
                if (m_attributes.end() == m_attributes.find(attrName))
                {
                    m_attributes[attrName] = Attribute(attrName);
                    m_attributes[attrName].setValue(attrValue);
                }
            }

        private:
            inline void setRange(const std::string &attrName, const int min, const int max)
            {
                if (m_attributes.end() != m_attributes.find(attrName))
                    m_attributes[attrName].setRange(min, max);
            }


            template <typename T>
            void setAllowedValues(const std::string &attrName, const std::vector<T> &values)
            {
                if (m_attributes.end() != m_attributes.find(attrName))
                    m_attributes[attrName].setAllowedValues(values);
            }

            inline void setUpdateInterval(const std::string &attrName, int interval)
            {
                if (m_attributes.end() != m_attributes.find(attrName))
                    m_attributes[attrName].setUpdateFrequencyTime(interval);
            }


            inline void removeAttribute(const std::string &attrName)
            {
                m_attributes.erase(attrName);
                return;
            }
            void readJson();

        public:
            std::string getResoureType() const {return m_rt; }
            std::string getInterface() const {return m_if; }

        private:
            std::map<std::string, Attribute> m_attributes;
            std::string m_rt;
            std::string m_if;
            cJSON *m_cjson;

    };

}
#endif
