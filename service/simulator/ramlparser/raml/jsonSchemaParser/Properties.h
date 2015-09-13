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

#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include <string>
#include <vector>
#include <map>
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>
#include <limits>
#include "Items.h"
#include "AllowedValues.h"
#include "cJSON.h"

namespace RAML
{
    class Properties
    {
        public:
            Properties(): m_min(INT_MAX), m_max(INT_MAX), m_multipleOf(INT_MAX) {}
            Properties(const std::string &name) : m_name(name), m_min(INT_MAX), m_max(INT_MAX),
                m_multipleOf(INT_MAX) {}

            inline std::string getName(void) const
            {
                return m_name;
            }
            inline void setName(const std::string &name)
            {
                m_name = name;
            }
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
            inline void getRange(int &min, int &max, int &multipleOf) const
            {
                min = m_min;
                max = m_max;
                multipleOf = m_multipleOf;
            }
            inline void setMin(const int &min)
            {
                m_min = min;
            }
            inline void setMax(const int &max)
            {
                m_max = max;
            }
            inline void setMultipleOf(const int &multipleOf)
            {
                m_multipleOf = multipleOf;
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
            inline std::vector<int> getAllowedValuesInt()
            {
                return m_allowedValues.getValuesInt();
            }
            inline std::vector<std::string> getAllowedValuesString()
            {
                return m_allowedValues.getValuesString();
            }
            inline void setDescription(const std::string &description)
            {
                m_description = description;
            }
            inline std::string getDescription()
            {
                return m_description;
            }
            inline int getUpdateFrequencyTime()
            {
                return m_updateInterval;
            }
            inline void setUpdateFrequencyTime(int interval)
            {
                m_updateInterval = interval;
            }
            void setType(const std::string &type)
            {
                m_type = type;
            }
            std::string getType()
            {
                return m_type;
            }
            void setPattern(const std::string &pattern)
            {
                m_pattern = pattern;
            }
            std::string getPattern()
            {
                return m_pattern;
            }
            void setFormat(const std::string &format)
            {
                m_format = format;
            }
            std::string getFormat()
            {
                return m_format;
            }
            void setItem(Items *item)
            {
                m_items.push_back(item);
            }
            std::vector<Items *> getItems()
            {
                return m_items;
            }
            void setUnique( int value)
            {
                if (value == cJSON_True) m_unique = true;
                else m_unique = false;
            }

            bool getUnique()
            {
                return m_unique;
            }

            void setAdditionalItems(int value)
            {
                if (value == cJSON_True) m_additionalItems = true;
                else m_additionalItems = false;
            }

            bool getAdditionalItems()
            {
                return m_additionalItems;
            }
        private:
            std::string m_name;
            ValueVariant m_value;
            int m_max;
            int m_min;
            int m_multipleOf;
            AllowedValues m_allowedValues;
            int m_updateInterval;
            std::string m_type;
            std::string m_pattern;
            std::string m_format;
            std::string m_description;
            bool m_unique;
            bool m_additionalItems;
            std::vector<Items *> m_items;
    };
    typedef std::shared_ptr<Properties> PropertiesPtr;

}
#endif
