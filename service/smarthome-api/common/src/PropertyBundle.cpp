/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
#include <iostream>
#include <PropertyBundle.h>
#include <CommonException.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            bool PropertyBundle::contains(const std::string& key) const
            {
                std::map<std::string, PropertyValue*>::const_iterator it = m_values.find(key);
                if (it != m_values.end())
                {
                    return true;
                }
                return false;
            }

            size_t PropertyBundle::size() const
            {
                return m_values.size();
            }

            PropertyType PropertyBundle::getType(const std::string& key)
            {
                std::map<std::string, PropertyValue*>::const_iterator iter = m_values.find(key);
                if (iter != m_values.end())
                {
                    return iter->second->getType();
                }
                return Unknown;
            }
        }
    }
}
