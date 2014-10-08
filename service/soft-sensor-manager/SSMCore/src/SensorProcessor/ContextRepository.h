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
#ifndef _ContextRepository_H_
#define _ContextRepository_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"
#include "Common/rapidxml.hpp"
#include "Common/rapidxml_print.hpp"
#include "ResourceFinder.h"

/**
 * @class    CContextRepository
 * @brief    Class for context manifest
 *           parse, get, set context manifest. provide device information
 *
 * @see      
 */

class CContextRepository :
	public CObjectRoot<CObjectMultiThreadModel>
	, public IContextRepository
	, public IResourceFinderEvent
{
private:
   CSimpleMutex					m_mtxFileIO;
   std::vector<ISSMResource*>	m_lstSensor;
   CObjectPtr<IResourceFinder>	m_resourceFinder;
   std::string					m_name;
   std::string					m_type;
   std::vector<IResourceEvent*>	m_resourceEvents;

public:
	SSMRESULT finalConstruct();
	void finalRelease();

	SSMRESULT queryInterface(const OID& objectID, IBase** ppObject)
	{
		if(ppObject == NULL)
			return SSM_E_POINTER;

		if(IsEqualOID(objectID, OID_IContextRepository))
		{
			IBase *pBase = this;
			pBase->addRef();
			*ppObject = pBase;
			return SSM_S_OK;
		}

		return SSM_E_NOINTERFACE;
	}


	/**
	* @fn           setCurrentDeviceInfo
	* @brief        set device information
	*
	* @param        [in] std::string name - Device name
	* @param        [in] std::string type - Device Type
	* @return       void
	*
	* @warning      
	* @exception    
	* @see          
	*/
	void setCurrentDeviceInfo(IN std::string name, IN std::string type);
	
	/**
	* @fn           getSoftSensorList
	* @brief        Get high level context resource list
	*
	* @param		[out] std::vector<ISSMResource*> *pSoftSensorList - High level context list
	* @return       SSMRESULT
	*					SSM_S_OK
	*					, SSM_S_FALSE
	*					, SSM_E_POINTER
	*					, SSM_E_OUTOFMEMORY
	*					, SSM_E_FAIL
	*					, SSM_E_NOINTERFACE
	*					, SSM_E_NOTIMPL
	*
	* @warning
	* @exception
	* @see
	*/
	SSMRESULT getSoftSensorList(OUT std::vector<ISSMResource*> *pSoftSensorList);

	/**
	* @fn           getPrimitiveSensorList
	* @brief        Get low level context resource list
	*
	* @param		[out] std::vector<ISSMResource*> *pPrimitiveSensorList - Low level context list
	* @return       SSMRESULT
	*					SSM_S_OK
	*					, SSM_S_FALSE
	*					, SSM_E_POINTER
	*					, SSM_E_OUTOFMEMORY
	*					, SSM_E_FAIL
	*					, SSM_E_NOINTERFACE
	*					, SSM_E_NOTIMPL
	*
	* @warning
	* @exception
	* @see
	*/
	SSMRESULT getPrimitiveSensorList(OUT std::vector<ISSMResource*> *pPrimitiveSensorList);
	
	SSMRESULT registerResourceFinderEvent(IN IResourceEvent *pResourceEvent);
	SSMRESULT startResourceFinder();
	SSMRESULT onResourceFound(IN ISSMResource *pSensor);
	SSMRESULT onResourceLost(IN ISSMResource *pSensor);

	SSMRESULT startObserveResource(IN ISSMResource *pSensor, IN IEvent *pEvent);
	SSMRESULT stopObserveResource(IN ISSMResource *pSensor);

private:
	SSMRESULT makeSSMResourceListForDictionaryData(IN const std::string typeString, IN std::vector<DictionaryData> dataList, OUT std::vector<ISSMResource*> *pList) ;
	std::vector<DictionaryData> loadXMLFromString(IN char *xmlData);
	std::vector<DictionaryData> loadXMLFromFile(IN const char *strFile );
};
#endif