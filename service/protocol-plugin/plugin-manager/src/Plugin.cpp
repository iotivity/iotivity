//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/// @file Plugin.cpp

/// @brief


#include "Plugin.h"

namespace OIC
{

    Plugin::Plugin(void)
    {

    }

    Plugin::~Plugin(void)
    {

    }

    std::string Plugin::getID(void)
    {
        std::map<std::string, AttributeValue>::iterator m_iterator;

        m_iterator = m_attributeMap.find("Id");

        if (m_iterator != m_attributeMap.end())
        {
            return m_iterator->second;
        }
        else
        {
            return "";
        }
    }

    std::string Plugin::getVersion(void)
    {
        std::map<std::string, AttributeValue>::iterator m_iterator;

        m_iterator = m_attributeMap.find("Version");

        if (m_iterator != m_attributeMap.end())
        {
            return m_iterator->second;
        }
        else
        {
            return "";
        }
    }

    std::string Plugin::getName(void)
    {
        std::map<std::string, AttributeValue>::iterator m_iterator;

        m_iterator = m_attributeMap.find("Name");

        if (m_iterator != m_attributeMap.end())
        {
            return m_iterator->second;
        }
        else
        {
            return "";
        }
    }


    std::string Plugin::getProviderName(void)
    {
        std::map<std::string, AttributeValue>::iterator m_iterator;

        m_iterator = m_attributeMap.find("Provider");

        if (m_iterator != m_attributeMap.end())
        {
            return m_iterator->second;
        }
        else
        {
            return "";
        }
    }


    std::string Plugin::getValueByAttribute(const std::string attribute)
    {
        std::map<std::string, AttributeValue>::iterator m_iterator;

        m_iterator = m_attributeMap.find(attribute);

        if (m_iterator != m_attributeMap.end())
        {
            return m_iterator->second;
        }
        else
        {
            return "";
        }
    }

    void Plugin::setValue(const std::string key, const std::string value)
    {
        m_attributeMap.insert(std::pair<std::string, AttributeValue>(key, value));
    }

    bool Plugin:: operator==(Plugin &plugin)
    {
        if ((!getID().compare(plugin.getID())))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}



