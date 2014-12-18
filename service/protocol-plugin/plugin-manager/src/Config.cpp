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

/// @file Config.cpp

/// @brief


#include "Config.h"


using namespace OIC;
using namespace rapidxml;
using namespace std;

Config *Config::s_configinstance = NULL;

Config::Config()
{
    if (loadConfigFile("./pluginmanager.xml") != PM_S_OK)
    {
        fprintf(stderr, "PM Configuration file is not exist current Folder.\n" );
        exit(EXIT_FAILURE);
    }
}

Config::~Config(void)
{
    s_configinstance->deleteinstance();
    s_configinstance = NULL;
}

PMRESULT Config::loadConfigFile(const std::string configfilepath)
{
    // Read the xml file
    std::ifstream xmlFile(configfilepath.c_str());
    if(!xmlFile.good())
    {
        return PM_S_FALSE;
    }
    xml_document<> doc;
    //into a vector
    std::vector<char> buffer((istreambuf_iterator<char>(xmlFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');

    // Parse the buffer using the xml file parsing library into doc
    parsing(buffer, &doc);

    // Find our root node
    xml_node<> *root_node = doc.first_node("pluginManager");
    xml_node<> *pluginInfo = root_node->first_node("pluginInfo");

    getXmlData(pluginInfo, "pluginPath");
    getXmlData(pluginInfo, "maxMEM");
    getXmlData(pluginInfo, "version");
    getXmlData(pluginInfo, "name");

    return PM_S_OK;
}

PMRESULT Config::parsing(std::vector<char> buffer, xml_document<> *doc)
{
    // Parse the buffer using the xml file parsing library into doc
    try
    {
        doc->parse<0>(&buffer[0]);
    }
    catch (rapidxml::parse_error err)
    {
        //print errors to screen
        fprintf(stderr, "PM Configuration file parsing error \n");
        exit(EXIT_FAILURE); //then exit
    }
    return PM_S_OK;
}

PMRESULT Config::getXmlData(xml_node<> *pluginInfo, std::string key)
{
    xml_attribute<> *iAttr = NULL;
    std::string value  = "";
    if (iAttr = pluginInfo->first_attribute(key.c_str()))
    {
        value = iAttr->value();
        setValue(key, value);
        return PM_S_OK;
    }
    else
    {
        return PM_S_FALSE;
    }
}

void Config::setValue(const std::string key, const std::string value)
{
    m_configurationMap.insert( std::pair<std::string, std::string>(key, value));
}

std::string  Config::getValue(const std::string key)
{
    std::map<std::string, std::string>::iterator m_iterator;

    m_iterator = m_configurationMap.find(key.c_str());

    if (m_iterator != m_configurationMap.end())
    {
        return m_iterator->second;
    }
    else
    {
        return "";
    }
}

std::string  Config::getVersion()
{
    std::map<std::string, std::string>::iterator m_iterator;

    m_iterator = m_configurationMap.find("version");

    if (m_iterator != m_configurationMap.end())
    {
        return m_iterator->second;
    }
    else
    {
        return "";
    }
}

std::string  Config::getPluginPath()
{
    std::map<std::string, std::string>::iterator m_iterator;

    m_iterator = m_configurationMap.find("pluginPath");

    if (m_iterator != m_configurationMap.end())
    {
        return m_iterator->second;
    }
    else
    {
        return "";
    }
}