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
#ifndef ITEMS_H_
#define ITEMS_H_

#include <string>
#include <vector>
#include <map>
#include "Properties.h"
#include "Helpers.h"
#include "AllowedValues.h"

namespace RAML
{
    class Properties;
    class AllowedValues;
    class Items
    {
        public:
            Items() {}
            void addProperty(const std::string &propName, Properties *property)
            {
                if (m_properties.end() == m_properties.find(propName))
                {
                    m_properties[propName] =  property;
                }
            }
            bool getproperty(const std::string &propName, Properties *value)
            {
                if (m_properties.end() != m_properties.find(propName))
                {
                    value = m_properties[propName];
                    return true;
                }
                return false;
            }
            std::map<std::string, Properties *> getProperties()
            {
                return m_properties;
            }
            void setType(const std::string &type)
            {
                m_type = type;
            }
            std::string getType()
            {
                return m_type;
            }
            void setRequiredValue(const std::string &reqValue)
            {
                if (m_required.end() == std::find(m_required.begin(), m_required.end(), reqValue))
                {
                    m_required.push_back(reqValue);
                }
            }
            std::vector<std::string> getRequiredValues()
            {
                return m_required;
            }
            template <typename T>
            bool setAllowedValues(const std::vector<T> &values)
            {
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
        private:
            std::map<std::string, Properties *> m_properties;
            std::string m_type;
            std::vector<std::string>  m_required;
            AllowedValues m_allowedValues;
    };
}
#endif

