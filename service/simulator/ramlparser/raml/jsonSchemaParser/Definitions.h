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
#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <string>
#include <vector>
#include <map>
#include "Properties.h"

namespace RAML
{
    class Definitions
    {
        public:

            Definitions() = default;
            Definitions(const std::string &name) : m_defName(name) {}

            inline std::string getName(void) const
            {
                return m_defName;
            }
            inline void setName(const std::string &name)
            {
                m_defName = name;
            }
            inline std::string getType(void) const
            {
                return m_type;
            }
            inline void setType(const std::string &type)
            {
                m_type = type;
            }
            void addProperty(const std::string &propName, Properties *property)
            {
                if (m_properties.end() == m_properties.find(propName))
                {
                    m_properties[propName] =  property;
                }
            }
            void setRequiredValue(const std::string &reqValue)
            {
                if (m_required.end() == std::find(m_required.begin(), m_required.end(), reqValue))
                {
                    m_required.push_back(reqValue);
                }
            }
            std::vector<std::string> getRequiredValues() const
            {
                return m_required;
            }
            int propertiesSize() const { return m_properties.size(); }

            inline bool getproperty(const std::string &propName, Properties *value)
            {
                if (m_properties.end() != m_properties.find(propName))
                {
                    value = m_properties[propName];
                    return true;
                }
                return false;
            }
            inline std::map<std::string, Properties *>  getProperties()
            {
                return m_properties;
            }
        private:
            std::map<std::string, Properties *> m_properties;
            std::string m_defName;
            std::string m_type;
            std::vector<std::string> m_required;

    };
    typedef std::shared_ptr<Definitions> DefinitionsPtr;

}
#endif
