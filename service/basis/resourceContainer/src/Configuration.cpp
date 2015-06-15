//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "Configuration.h"

static inline std::string trim_both(const std::string &str)
{
    int npos = str.find_first_not_of(" \t\v\n\r");

    if (npos == -1)
    {
        return "";
    }

    unsigned int n = (unsigned int)npos;
    std::string tempString = n == std::string::npos ? str : str.substr(n, str.length());

    n = tempString.find_last_not_of(" \t\v\n\r");

    return n == std::string::npos ? tempString : tempString.substr(0, n + 1);
}

Configuration::Configuration()
{
    // TODO: temporary path and config file name
    getCurrentPath(&m_pathConfigFile);
    m_pathConfigFile.append("/");
    m_pathConfigFile.append("ResourceContainerConfig.xml");
}

Configuration::~Configuration()
{}

Configuration::Configuration(string configFile)
{
    m_configFile = configFile;
    getCurrentPath(&m_pathConfigFile);
    m_pathConfigFile.append("/");
    m_pathConfigFile.append(m_configFile);
}

void Configuration::getCommonConfiguration(configInfo *configOutput)
{
    string strConfigData;

    rapidxml::xml_document< char > xmlDoc;

    rapidxml::xml_node< char > *root;
    rapidxml::xml_node< char > *item;
    rapidxml::xml_node< char > *subItem;

    string strKey, strValue;

    getConfigDocument(m_pathConfigFile, &strConfigData);

    try
    {
        xmlDoc.parse< 0 >((char *)strConfigData.c_str());

        // <container>
        root = xmlDoc.first_node();

        if (!root)
        {
            throw rapidxml::parse_error("No Root Element", 0);
        }
        std::map< std::string, std::string > bundleMap;
        for (item = root->first_node(); item; item = item->next_sibling())
        {
            strKey = item->name();
            strValue = item->value();

            // <config>
            if (!strKey.compare("config"))
            {
                for (subItem = item->first_node(); subItem;
                     subItem = subItem->next_sibling())
                {
                    strKey = subItem->name();
                    strValue = subItem->value();

                    bundleMap.insert(std::make_pair(trim_both(strKey), trim_both(strValue)));
                }
                break;
            }
        }
        configOutput->push_back(bundleMap);
    }
    catch (rapidxml::parse_error &e)
    {
        cout << "xml parsing failed !!" << endl;
        cout << e.what() << endl;
    }
}

void Configuration::getConfiguredBundles(configInfo *configOutput)
{
    string strConfigData;

    rapidxml::xml_document< char > xmlDoc;

    rapidxml::xml_node< char > *bundle;
    rapidxml::xml_node< char > *subItem;

    string strKey, strValue;

    cout << "Opening: " << m_pathConfigFile << endl;

    getConfigDocument(m_pathConfigFile, &strConfigData);

    //cout << strConfigData.c_str() << endl;
    try
    {

        xmlDoc.parse< 0 >((char *)strConfigData.c_str());
        //cout << "Name of first node is: " << xmlDoc.first_node()->name() << endl;

        for (bundle = xmlDoc.first_node()->first_node("bundle"); bundle; bundle = bundle->next_sibling())
        {
            std::map< std::string, std::string > bundleMap;
            //cout << "Bundle: " << bundle->name() << endl;
            for (subItem = bundle->first_node(); subItem; subItem = subItem->next_sibling())
            {
                strKey = subItem->name();
                strValue = subItem->value();
                if (strlen(subItem->value()) > 0)
                {
                    bundleMap.insert(std::make_pair(trim_both(strKey), trim_both(strValue)));
                    //cout << strKey << " " << strValue << endl;

                }
            }
            configOutput->push_back(bundleMap);
        }

    }
    catch (rapidxml::parse_error &e)
    {
        cout << "xml parsing failed !!" << endl;
        cout << e.what() << endl;
    }
}

void Configuration::getBundleConfiguration(string bundleId, configInfo *configOutput)
{
    string strConfigData;

    rapidxml::xml_document< char > xmlDoc;

    rapidxml::xml_node< char > *root;
    rapidxml::xml_node< char > *item;
    rapidxml::xml_node< char > *subItem;

    string strKey, strValue;

    getConfigDocument(m_pathConfigFile, &strConfigData);

    try
    {
        xmlDoc.parse< 0 >((char *)strConfigData.c_str());

        // <container>
        root = xmlDoc.first_node();

        if (!root)
        {
            throw rapidxml::parse_error("No Root Element", 0);
        }
        std::map< std::string, std::string > bundleMap;
        for (item = root->first_node(); item; item = item->next_sibling())
        {
            strKey = item->name();
            strValue = item->value();

            // <bundle>
            if (!strKey.compare("bundle"))
            {
                for (subItem = item->first_node(); subItem; subItem = subItem->next_sibling())
                {
                    strKey = subItem->name();
                    strValue = subItem->value();

                    if (!strKey.compare("bundleID") && strValue.compare(bundleId))
                        break;

                    // bundle info (except resource data)
                    if (strKey.compare("resources"))
                    {
                        bundleMap.insert(std::make_pair(trim_both(strKey), trim_both(strValue)));
                    }
                }
            }
        }
        configOutput->push_back(bundleMap);

    }
    catch (rapidxml::parse_error &e)
    {
        cout << "xml parsing failed !!" << endl;
        cout << e.what() << endl;
    }
}

void Configuration::getResourceConfiguration(string bundleId, vector<resourceInfo> *configOutput)
{
    string strConfigData;

    rapidxml::xml_document< char > xmlDoc;

    rapidxml::xml_node< char > *root;
    rapidxml::xml_node< char > *item;
    rapidxml::xml_node< char > *subItem, *subItem2, *subItem3, *subItem4, *subItem5;

    string strKey, strValue;

    getConfigDocument(m_pathConfigFile, &strConfigData);

    try
    {
        xmlDoc.parse< 0 >((char *)strConfigData.c_str());

        // <container>
        root = xmlDoc.first_node();

        if (!root)
        {
            throw rapidxml::parse_error("No Root Element", 0);
        }

        for (item = root->first_node(); item; item = item->next_sibling())
        {
            strKey = item->name();
            strValue = item->value();

            // <bundle>
            if (!strKey.compare("bundle"))
            {
                for (subItem = item->first_node(); subItem; subItem = subItem->next_sibling())
                {
                    strKey = subItem->name();
                    strValue = subItem->value();

                    if (!strKey.compare("bundleID") && strValue.compare(bundleId))
                        break;

                    // <resources>
                    if (!strKey.compare("resources"))
                    {
                        for (subItem2 = subItem->first_node(); subItem2;
                             subItem2 = subItem2->next_sibling())
                        {
                            strKey = subItem2->name();
                            strValue = subItem2->value();

                            // <resourceInfo> : for 1 resource
                            if (!strKey.compare("resourceInfo"))
                            {
                                resourceInfo tempResourceInfo;

                                for (subItem3 = subItem2->first_node(); subItem3; subItem3 = subItem3->next_sibling())
                                {

                                    strKey = subItem3->name();
                                    strValue = subItem3->value();

                                    if (!strKey.compare("name"))
                                        tempResourceInfo.name = trim_both(strValue);

                                    else if (!strKey.compare("uri"))
                                        tempResourceInfo.uri = trim_both(strValue);

                                    else if (!strKey.compare("resourceType"))
                                        tempResourceInfo.resourceType = trim_both(strValue);

                                    else
                                    {
                                        for (subItem4 = subItem3->first_node(); subItem4; subItem4 = subItem4->next_sibling())
                                        {
                                            map <string, string> propertyMap;

                                            strKey = subItem4->name();

                                            for (subItem5 = subItem4->first_node(); subItem5; subItem5 = subItem5->next_sibling())
                                            {
                                                string newStrKey = subItem5->name();
                                                string newStrValue = subItem5->value();

                                                propertyMap[trim_both(newStrKey)] = trim_both(newStrValue);
                                            }

                                            tempResourceInfo.resourceProperty[trim_both(strKey)].push_back(propertyMap);
                                        }
                                    }
                                }
                                configOutput->push_back(tempResourceInfo);
                            }
                        }
                    }
                }
            }
        }
    }
    catch (rapidxml::parse_error &e)
    {
        cout << "xml parsing failed !!" << endl;
        cout << e.what() << endl;
    }
}

void Configuration::getConfigDocument(std::string pathConfigFile, std::string *pConfigData)
{
    std::basic_ifstream< char > xmlFile(pathConfigFile.c_str());

    if (!xmlFile.fail())
    {
        xmlFile.seekg(0, std::ios::end);
        unsigned int size = (unsigned int)xmlFile.tellg();
        xmlFile.seekg(0);

        std::vector< char > xmlData(size + 1);
        xmlData[size] = 0;

        xmlFile.read(&xmlData.front(), (std::streamsize) size);
        xmlFile.close();
        *pConfigData = std::string(xmlData.data());
    }
    else
    {
        std::cout << "Configuration File load failed !!" << std::endl;
    }
}

void Configuration::getCurrentPath(std::string *path)
{
    char buffer[2048];
    char *strPath = NULL;

    int length = readlink("/proc/self/exe", buffer, 2047);

    buffer[length] = '\0';

    strPath = strrchr(buffer, '/');

    *strPath = '\0';

    path->append(buffer);
}
