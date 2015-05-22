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

/// @brief Config class reads pluginmanager.xml file and configures PPM


#include "Config.h"


using namespace OIC;
using namespace rapidxml;
using namespace std;

Config *Config::s_configinstance = NULL;

Config::Config(void *args)
{
    std::string path = ".";
/**
 * For Tizen Platform, specifiy the absolute location of config file. It is required for
 * Tizen 2.3 EFL App to work.
 */
#ifdef __TIZEN__
    if (loadConfigFile("/opt/usr/apps/org.iotivity.service.ppm.ppmsampleapp/lib/pluginmanager.xml")
                                                                                        != PM_S_OK)
#else
#ifdef ANDROID
    JavaVM *jvm = (JavaVM *)args;
    JNIEnv *env;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    jclass cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
    jmethodID mid = env->GetStaticMethodID(cls, "getPackageName", "()Ljava/lang/String;");
    jstring jpath = (jstring)env->CallStaticObjectMethod(cls, mid);
    path = env->GetStringUTFChars(jpath, 0);

    if(path != ".")
        path = "/data/data/" + path + "/files";
#endif

    if (loadConfigFile(path + "/pluginmanager.xml") != PM_S_OK)
#endif //#ifdef __TIZEN__
    {
        fprintf(stderr, "PM Configuration file is not exist current Folder.\n" );
        exit(EXIT_FAILURE);
    }
}

Config::~Config(void)
{
    if (s_configinstance) 
    {
        s_configinstance->deleteinstance();
        s_configinstance = NULL;
    }
}

PMRESULT Config::loadConfigFile(const std::string configfilepath)
{
    // Read the xml file
   xml_document< char > doc;
    std::basic_ifstream< char > xmlFile(configfilepath.c_str());
    if (!xmlFile.good())
    {
        return PM_S_FALSE;
    }

    xmlFile.seekg(0, std::ios::end);
    unsigned int size = (unsigned int)xmlFile.tellg();
    xmlFile.seekg(0);

    std::vector< char > xmlData(size + 1);
    xmlData[size] = 0;

    xmlFile.read(&xmlData.front(), (std::streamsize)size);
    xmlFile.close();

    // Parse the xmlData using the xml file parsing library into doc
    parsing(&xmlData.front(), &doc);

    // Find our root node
    xml_node< char > *root_node = doc.first_node("pluginManager");
    if(!root_node)
    {
        throw parse_error("No Root Element", 0);
    }

    xml_node< char > *pluginInfo = root_node->first_node("pluginInfo");

    getXmlData(pluginInfo, "pluginPath");

    return PM_S_OK;
}

PMRESULT Config::parsing(char *xmlData, xml_document<> *doc)
{
    // Parse the xmlData using the xml file parsing library into doc
    try
    {
        doc->parse< 0 >(xmlData);
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

std::string Config::getValue(const std::string key)
{
    std::map<std::string, std::string>::iterator m_iterator;

    // Find the value for key
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

std::string Config::getVersion()
{
    std::map<std::string, std::string>::iterator m_iterator;

    // Find the value for key - version
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

std::string Config::getPluginPath()
{
    std::map<std::string, std::string>::iterator m_iterator;

    // Find the value for key - pluginPath
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