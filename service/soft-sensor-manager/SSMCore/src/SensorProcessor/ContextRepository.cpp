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

static inline std::string trim_both(const std::string& str)
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

	SSM_CLEANUP_ASSERT(CreateInstance(OID_IResourceFinder, (IBase**)&m_resourceFinder));
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

SSMRESULT CContextRepository::registerResourceFinderEvent(IN IResourceEvent *pResourceEvent)
{
	m_resourceEvents.push_back(pResourceEvent);
	return SSM_S_OK;
}

void CContextRepository::finalRelease()
{
}

void CContextRepository::setCurrentDeviceInfo(IN std::string name, IN std::string type)
{
	//TODO: Someone need to provides a way to generate permanent uuid function
	/*
	m_currentDeviceInfo.friendlyName = name;
	//m_currentDeviceInfo.ID="2fac1234-31f8-11b4-a222-08002b34c003";
	m_currentDeviceInfo.ID = udn;
	m_currentDeviceInfo.IPAddress = "127.0.0.1";
	m_currentDeviceInfo.tp = tp;
	//m_currentDeviceInfo.type = SSM_DEVICE_MOBILE;
	m_currentDeviceInfo.type = td;
	*/
	m_name = name;
	m_type = type;
}

std::vector<DictionaryData> CContextRepository::loadXMLFromString(char *xmlData)
{
	// use  rapidxml-----------------------
	rapidxml::xml_document< char > xmlDoc;
	//xmlDoc.parse< 0 >( &xmlData.front() );
	xmlDoc.parse< 0 >(xmlData);

	std::string keyStr;
	std::string valueStr;

	rapidxml::xml_node< char > *item;
	rapidxml::xml_node< char > *subItem;
	rapidxml::xml_node< char > *subItem2;
	rapidxml::xml_node< char > *subItem3;

	std::vector<DictionaryData> dictionaryDataList;
	// get value
	rapidxml::xml_node< char > *root = xmlDoc.first_node();

	//SSM_RESULT_ASSERT(root,"XML file is empty",dictionaryDataList);
	if(!root)
	{
		return dictionaryDataList;
	}

	for( item = root->first_node(); item; item = item->next_sibling() )
	{
		DictionaryData dictionaryData;
		for( subItem = item->first_node(); subItem ; subItem = subItem->next_sibling() )
		{
			//root name
			keyStr = subItem->name();  // key
			valueStr = subItem->value();   // value

			if(!keyStr.compare("root_name"))
			{
				dictionaryData.rootName = trim_both(valueStr);
			}
			////std::cout<<keyStr << " : " << subItem->value() <<std::endl<<std::endl; //root_name
			for(subItem2 = subItem->first_node(); subItem2 ; subItem2 = subItem2->next_sibling())
			{
				std::map<std::string,std::string> propertyMap;
				std::vector<std::string> enterconditionVector;

				keyStr = subItem2->name();  // key
				valueStr = subItem2->value();   // value

				if(!keyStr.compare("attribute_property_count"))
				{
					dictionaryData.attributePropertyCount = trim_both(valueStr);
				}
				else if(!keyStr.compare("output_property_count"))
				{
					dictionaryData.outputPropertyCount  = trim_both(valueStr);
				}
				if(!keyStr.compare("inputs_count"))
				{
					dictionaryData.inputCount = trim_both(valueStr);
				}
				else if(!keyStr.compare("app_inputs_count"))
				{
					dictionaryData.appInputCount  = trim_both(valueStr);
				}
				else if(!keyStr.compare("app_input"))
				{
					dictionaryData.appInputs.push_back(trim_both(valueStr));
				}
				else if(!keyStr.compare("input"))
				{
					dictionaryData.inputs.push_back(trim_both(valueStr));
				}
				////std::cout<<name << " :: " << subItem2->value() <<std::endl<<std::endl;
				for(subItem3 = subItem2->first_node(); subItem3 ; subItem3 = subItem3->next_sibling())
				{ 

					std::string newKeyStr = subItem3->name();  // key
					valueStr = subItem3->value();   // value

					if(!keyStr.compare("attribute_property") || !keyStr.compare("output_property") )
					{
						propertyMap.insert(std::make_pair(trim_both(newKeyStr),trim_both(valueStr)))	;
					}
					else if(!keyStr.compare("enter_condition"))
					{
						enterconditionVector.push_back(trim_both(valueStr));
					}
				}
				if(!keyStr.compare("attribute_property"))
				{
					dictionaryData.attributeProperty.push_back(propertyMap);
				}
				else if(!keyStr.compare("output_property"))
				{
					dictionaryData.outputProperty.push_back(propertyMap);
				}
				else if(!keyStr.compare("enter_condition"))
				{
					dictionaryData.enterConditions.push_back(enterconditionVector);
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

		dictionaryDataList.push_back(dictionaryData);
	}

	return dictionaryDataList;
}

std::vector<DictionaryData> CContextRepository::loadXMLFromFile(const char *strFile )
{
	std::basic_ifstream< char > xmlFile( strFile );
	std::vector<DictionaryData> returnData;

	//SSM_RESULT_ASSERT(!xmlFile.fail(),"File open failed.",returnData);
	
	if(!xmlFile.fail())
	{
		xmlFile.seekg(0, std::ios::end);
		unsigned int size = (unsigned int)xmlFile.tellg();
		xmlFile.seekg(0);

		std::vector< char > xmlData(size+1);
		xmlData[size] = 0;

		xmlFile.read( &xmlData.front(), (std::streamsize)size );
		xmlFile.close();
		returnData = loadXMLFromString(&xmlData.front());
	}

	return returnData;
}

SSMRESULT CContextRepository::getSoftSensorList(OUT std::vector<ISSMResource*> *pHighLevelList)
{
	SSMRESULT res = SSM_E_FAIL;
	std::vector<DictionaryData> dict = loadXMLFromFile(HIGH_LOCATION);
	res = makeSSMResourceListForDictionaryData("HIGH", dict, pHighLevelList);

	return res;
}

SSMRESULT CContextRepository::getPrimitiveSensorList(OUT std::vector<ISSMResource*> *pLowLevelList)
{
	for (size_t i = 0; i < m_lstSensor.size(); i++)
	{
		pLowLevelList->push_back(m_lstSensor.at(i));
	}

	return SSM_S_OK;
}

SSMRESULT CContextRepository::onResourceFound(IN ISSMResource *pSensor)
{
	m_lstSensor.push_back(pSensor);

	for (size_t i = 0; i < m_resourceEvents.size(); i++)
	{
		m_resourceEvents.at(i)->onResourceEvent(SSM_EVENT_ADDED, pSensor, "");
	}

	return SSM_S_OK;
}

SSMRESULT CContextRepository::onResourceLost(IN ISSMResource *pSensor)
{
	std::vector<ISSMResource*>::iterator	itor;
	itor = std::find(m_lstSensor.begin(), m_lstSensor.end(), pSensor);

	if (itor != m_lstSensor.end())
	{
		m_lstSensor.erase(itor);
		return SSM_S_OK;
	}
	
	return SSM_E_FAIL;
}

SSMRESULT CContextRepository::startObserveResource(IN ISSMResource *pSensor, IN IEvent *pEvent)
{
	return m_resourceFinder->startObserveResource(pSensor, pEvent);
}

SSMRESULT CContextRepository::stopObserveResource(IN ISSMResource *pSensor)
{
	return m_resourceFinder->stopObserveResource(pSensor);
}

//TODO: Need to fix
SSMRESULT CContextRepository::makeSSMResourceListForDictionaryData(const std::string typeString, std::vector<DictionaryData> dataList, std::vector<ISSMResource*> *pList)
{
	SSMRESULT res = SSM_E_FAIL;

	for(unsigned int i = 0 ; i < dataList.size() ; ++i)
	{
		ISSMResource *pResource = new ISSMResource();
		pResource->location = SENSOR_LOCATION_LOCAL;
		pResource->type = dataList.at(i).rootName;
		pResource->name = std::string("coap://127.0.0.1/") + dataList.at(i).rootName;
		pResource->ip = "coap://127.0.0.1/";

		pResource->inputList = dataList.at(i).inputs;
		
		for(unsigned int j = 0 ; j <  dataList.at(i).outputProperty.size() ;++j )
		{
			pResource->outputProperty.push_back(dataList.at(i).outputProperty.at(j));
		}
		for(unsigned int j = 0 ; j <  dataList.at(i).attributeProperty.size() ;++j )
		{
			pResource->outputProperty.push_back(dataList.at(i).attributeProperty.at(j));
		}
		pList->push_back(pResource);
	}

	res = SSM_S_OK;

	return res;
}
