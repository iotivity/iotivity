/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
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
#include "ContextRepository.h"
#include "rapidxml/rapidxml.hpp"

static inline std::string trim_both(const std::string &str)
{

    int npos = str.find_first_not_of(" \t\v\n");
    if (npos == -1)
    {
        return "";
    }
    unsigned int n = (unsigned int)npos;
    std::string tempString = n == std::string::npos ? str : str.substr(n, str.length());

    n = tempString.find_last_not_of(" \t\v\n");
    return n == std::string::npos ? tempString : tempString.substr(0, n + 1);
}

SSMRESULT CContextRepository::finalConstruct()
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(CreateInstance(OID_IResourceFinder, (IBase **)&m_resourceFinder));
    SSM_CLEANUP_ASSERT(m_resourceFinder->registerResourceFinderEvent(this));

CLEANUP:
    return res;
}

SSMRESULT CContextRepository::startResourceFinder()
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(m_resourceFinder->startResourceFinder());

CLEANUP:
    return res;
}

SSMRESULT CContextRepository::stopResourceFinder()
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(m_resourceFinder->stopResourceFinder());

CLEANUP:
    return res;
}

SSMRESULT CContextRepository::registerResourceFinderEvent(IResourceEvent *pResourceEvent)
{
    m_resourceEvents.push_back(pResourceEvent);
    return SSM_S_OK;
}

void CContextRepository::finalRelease()
{
}

SSMRESULT CContextRepository::initRepository(std::string name, std::string type,
        std::string pathSoftSensors, std::string pathDescription)
{
    SSMRESULT res = SSM_E_FAIL;

    std::vector<DictionaryData> dict;

    m_name = name;
    m_type = type;

    if (pathSoftSensors.length() == 0)
    {
        SSM_CLEANUP_ASSERT(getCurrentPath(&m_pathSoftSensors));
        m_pathSoftSensors.append("/");
    }
    else
    {
        m_pathSoftSensors = pathSoftSensors;
    }

    if (pathDescription.length() == 0)
    {
        SSM_CLEANUP_ASSERT(getCurrentPath(&m_pathSoftSensorsDescription));
        m_pathSoftSensorsDescription.append("/");
        m_pathSoftSensorsDescription.append(DEFAULT_PATH_SOFT_SENSORS);
    }
    else
    {
        m_pathSoftSensorsDescription = pathDescription;
    }


    SSM_CLEANUP_ASSERT(loadXMLFromFile(m_pathSoftSensorsDescription.c_str(), &dict));
    SSM_CLEANUP_ASSERT(makeSSMResourceListForDictionaryData(dict, &m_lstSoftSensor));

CLEANUP:
    return res;
}

SSMRESULT CContextRepository::loadXMLFromString(char *xmlData,
        std::vector<DictionaryData> *dataList)
{
    // use  rapidxml-----------------------
    SSMRESULT res = SSM_E_INVALIDXML;
    rapidxml::xml_document< char > xmlDoc;
    //xmlDoc.parse< 0 >( &xmlData.front() );

    std::string keyStr;
    std::string valueStr;

    rapidxml::xml_node< char > *item;
    rapidxml::xml_node< char > *subItem;
    rapidxml::xml_node< char > *subItem2;
    rapidxml::xml_node< char > *subItem3;

    rapidxml::xml_node< char > *root;

    try
    {
        xmlDoc.parse< 0 >(xmlData);

        // get value
        root = xmlDoc.first_node();

        if (!root)
        {
            throw rapidxml::parse_error("No Root Element", 0);
        }

        for (item = root->first_node(); item; item = item->next_sibling())
        {
            DictionaryData dictionaryData;
            for (subItem = item->first_node(); subItem; subItem = subItem->next_sibling())
            {
                //root name
                keyStr = subItem->name();  // key
                valueStr = subItem->value();   // value

                if (!keyStr.compare("name"))
                {
                    dictionaryData.rootName = trim_both(valueStr);
                }
                ////std::cout<<keyStr << " : " << subItem->value() <<std::endl<<std::endl; //root_name
                for (subItem2 = subItem->first_node(); subItem2; subItem2 = subItem2->next_sibling())
                {
                    std::map<std::string, std::string> propertyMap;
                    std::vector<std::string> enterconditionVector;

                    keyStr = subItem2->name();  // key
                    valueStr = subItem2->value();   // value

                    if (!keyStr.compare("input"))
                    {
                        dictionaryData.inputs.push_back(trim_both(valueStr));
                    }
                    ////std::cout<<name << " :: " << subItem2->value() <<std::endl<<std::endl;
                    for (subItem3 = subItem2->first_node(); subItem3; subItem3 = subItem3->next_sibling())
                    {
                        std::string newKeyStr = subItem3->name();  // key
                        valueStr = subItem3->value();   // value

                        if (!keyStr.compare("attribute") || !keyStr.compare("output"))
                        {
                            propertyMap.insert(std::make_pair(trim_both(newKeyStr), trim_both(valueStr)));
                        }
                    }
                    if (!keyStr.compare("attribute"))
                    {
                        dictionaryData.attributeProperty.push_back(propertyMap);
                    }
                    else if (!keyStr.compare("output"))
                    {
                        dictionaryData.outputProperty.push_back(propertyMap);
                    }
                }
            }
            //for accurate data.
            /*
            dictionaryData.app_input_count = std::to_string((long long)dictionaryData.app_inputs.size());
            dictionaryData.input_count = std::to_string((long long)dictionaryData.inputs.size());
            dictionaryData.attribute_property_count = std::to_string((long long)dictionaryData.attribute_property.size());
            dictionaryData.output_property_count = std::to_string((long long)dictionaryData.output_property.size());
            */

            dataList->push_back(dictionaryData);
        }

        res = SSM_S_OK;
    }
    catch (rapidxml::parse_error &e)
    {
        SSM_CLEANUP_ASSERT(SSM_E_INVALIDXML);
    }

CLEANUP:
    return res;
}

SSMRESULT CContextRepository::loadXMLFromFile(std::string descriptionFilePath,
        std::vector<DictionaryData> *dataList)
{
    SSMRESULT res = SSM_E_FAIL;
    std::basic_ifstream< char > xmlFile(descriptionFilePath.c_str());

    //path loaded
    if (!xmlFile.fail())
    {
        xmlFile.seekg(0, std::ios::end);
        unsigned int size = (unsigned int)xmlFile.tellg();
        xmlFile.seekg(0);

        std::vector< char > xmlData(size + 1);
        xmlData[size] = 0;

        xmlFile.read(&xmlData.front(), (std::streamsize)size);
        xmlFile.close();
        SSM_CLEANUP_ASSERT(loadXMLFromString(&xmlData.front(), dataList));
    }
    else
    {
        //let work with no soft sensor manager
        res = SSM_S_OK;
    }

CLEANUP:
    return res;
}

SSMRESULT CContextRepository::getSoftSensorList(std::vector<ISSMResource *> *pSoftSensorList)
{
    for (size_t i = 0; i < m_lstSoftSensor.size(); i++)
    {
        pSoftSensorList->push_back(m_lstSoftSensor.at(i));
    }

    return SSM_S_OK;
}

SSMRESULT CContextRepository::getPrimitiveSensorList(std::vector<ISSMResource *>
        *pPrimitiveSensorList)
{
    for (size_t i = 0; i < m_lstPrimitiveSensor.size(); i++)
    {
        pPrimitiveSensorList->push_back(m_lstPrimitiveSensor.at(i));
    }

    return SSM_S_OK;
}

SSMRESULT CContextRepository::onResourceFound(ISSMResource *pSensor)
{
    m_lstPrimitiveSensor.push_back(pSensor);

    for (size_t i = 0; i < m_resourceEvents.size(); i++)
    {
        m_resourceEvents.at(i)->onResourceEvent(SSM_EVENT_ADDED, pSensor, "");
    }

    return SSM_S_OK;
}

SSMRESULT CContextRepository::onResourceLost(ISSMResource *pSensor)
{
    std::vector<ISSMResource *>::iterator    itor;
    itor = std::find(m_lstPrimitiveSensor.begin(), m_lstPrimitiveSensor.end(), pSensor);

    if (itor != m_lstPrimitiveSensor.end())
    {
        m_lstPrimitiveSensor.erase(itor);
        return SSM_S_OK;
    }

    return SSM_E_FAIL;
}

SSMRESULT CContextRepository::startObserveResource(ISSMResource *pSensor, IEvent *pEvent)
{
    return m_resourceFinder->startObserveResource(pSensor, pEvent);
}

SSMRESULT CContextRepository::stopObserveResource(ISSMResource *pSensor)
{
    return m_resourceFinder->stopObserveResource(pSensor);
}

//TODO: Need to fix
SSMRESULT CContextRepository::makeSSMResourceListForDictionaryData(std::vector<DictionaryData>
        dataList, std::vector<ISSMResource *> *pList)
{
    SSMRESULT res = SSM_E_FAIL;

    for (unsigned int i = 0 ; i < dataList.size() ; ++i)
    {
        ISSMResource *pResource = new ISSMResource();
        pResource->location = SENSOR_LOCATION_LOCAL;
        pResource->type = dataList[i].rootName;
        pResource->name = std::string("coap://127.0.0.1/") + dataList[i].rootName;
        pResource->ip = "coap://127.0.0.1/";

        pResource->inputList = dataList[i].inputs;

        for (unsigned int j = 0; j < dataList[i].outputProperty.size(); ++j)
        {
            pResource->outputProperty.push_back(dataList[i].outputProperty[j]);
        }
        for (unsigned int j = 0; j < dataList[i].attributeProperty.size(); ++j)
        {
            pResource->outputProperty.push_back(dataList[i].attributeProperty[j]);
        }
        pList->push_back(pResource);
    }

    res = SSM_S_OK;

    return res;
}

SSMRESULT CContextRepository::loadSoftSensor(std::string softSensorName, ICtxDelegate *pDelegate,
        void **hSoftSensor)
{
    std::stringstream   sstream;
    SSMRESULT           res = SSM_E_FAIL;

    typedef void(*InitContext)(ICtxDelegate *);
    InitContext InitializeContextFunction = NULL;

    // load dll(so)

#ifdef WIN32
    sstream << m_pathSoftSensors << softSensorName.c_str() << ".dll" << std::ends;

    HINSTANCE hModule = NULL;
    hModule = LoadLibraryA(sstream.str().c_str());

    if (hModule != NULL)
    {
        InitializeContextFunction = (InitContext)GetProcAddress(hModule, "InitializeContext");
    }
#else
    //sstream << "/data/data/com.example.javaproject/lib/lib" << modelName <<".so" << std::ends;
    sstream << m_pathSoftSensors << "lib" << softSensorName.c_str() << ".so" << std::ends;

    void *hModule = NULL;
    hModule = dlopen(sstream.str().c_str(), RTLD_LOCAL | RTLD_LAZY);

    if (hModule != NULL)
    {
        InitializeContextFunction = (InitContext)dlsym(hModule, "InitializeContext");
    }
#endif
    if (hModule == NULL)
    {
        //load library failed. raise error
        SSM_CLEANUP_ASSERT(SSM_E_FAIL);
    }

    if (InitializeContextFunction != NULL)
    {
        InitializeContextFunction(pDelegate);
        *hSoftSensor = hModule;
        res = SSM_S_OK;
    }
    else
    {
        //Unload module and return error
        SSM_CLEANUP_ASSERT(unloadSoftSensor(hModule));
        SSM_CLEANUP_ASSERT(SSM_E_FAIL);
    }

CLEANUP:
    return res;
}

SSMRESULT CContextRepository::unloadSoftSensor(void *hSoftSensor)
{
    SSMRESULT           res = SSM_E_FAIL;

#ifdef WIN32
    SSM_CLEANUP_COND_ASSERT(FreeLibrary((HINSTANCE)hSoftSensor), TRUE, "FreeLibrary failed");
#else
    SSM_CLEANUP_COND_ASSERT(dlclose(hSoftSensor), 0, "dlclose failed");
#endif

    res = SSM_S_OK;

CLEANUP:
    return res;
}

SSMRESULT CContextRepository::getCurrentPath(std::string *path)
{
    char        buffer[2048];
    SSMRESULT   res = SSM_E_FAIL;
#if defined(WIN32)
    DWORD length = GetModuleFileNameA(NULL, buffer, 2047);

    if (length == 0)
    {
        SSM_CLEANUP_ASSERT(SSM_E_FAIL);
    }

    buffer[length] = '\0';
    if (PathRemoveFileSpecA(buffer) == 0)
    {
        SSM_CLEANUP_ASSERT(SSM_E_FAIL);
    }

#elif defined(LINUX)
    char    *strPath = NULL;
    int length = ::readlink("/proc/self/exe", buffer, 2047);

    if (length == -1)
    {
        SSM_CLEANUP_ASSERT(SSM_E_FAIL);
    }

    buffer[length] = '\0';

    strPath = strrchr(buffer, '/');

    if (strPath == NULL)
    {
        SSM_CLEANUP_ASSERT(SSM_E_FAIL);
    }

    *strPath = '\0';
#endif

    path->append(buffer);
    res = SSM_S_OK;

CLEANUP:
    return res;
}
