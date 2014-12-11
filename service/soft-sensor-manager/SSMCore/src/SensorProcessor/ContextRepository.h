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
        CSimpleMutex                 m_mtxFileIO;
        std::vector<ISSMResource *>   m_lstPrimitiveSensor;
        std::vector<ISSMResource *>   m_lstSoftSensor;
        CObjectPtr<IResourceFinder>  m_resourceFinder;
        std::string                  m_name;
        std::string                  m_type;
        std::vector<IResourceEvent *> m_resourceEvents;
        std::string                  m_pathSoftSensors;
        std::string                  m_pathSoftSensorsDescription;

    public:
        SSMRESULT finalConstruct();
        void finalRelease();

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IContextRepository))
            {
                IBase *pBase = this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }


        /**
        * @fn           initRepository
        * @brief        initialize repository using given info
        *
        * @param        [in] std::string name - Device name
        * @param        [in] std::string type - Device Type
        * @param        [in] std::string pathSoftSensors - SoftSensors Repository path
        * @param        [in] std::string pathDescription - SoftSensors Description path
        * @return       SSMRESULT
        *
        * @warning
        * @exception
        * @see
        */
        SSMRESULT initRepository(IN std::string name, IN std::string type, IN std::string pathSoftSensors,
                                 IN std::string pathDescription);

        /**
        * @fn           getSoftSensorList
        * @brief        Get soft sensor list
        *
        * @param        [out] std::vector<ISSMResource*> *pSoftSensorList - List of soft sensors
        * @return       SSMRESULT
        *                   SSM_S_OK
        *                   , SSM_S_FALSE
        *                   , SSM_E_POINTER
        *                   , SSM_E_OUTOFMEMORY
        *                   , SSM_E_FAIL
        *                   , SSM_E_NOINTERFACE
        *                   , SSM_E_NOTIMPL
        *
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getSoftSensorList(OUT std::vector<ISSMResource *> *pSoftSensorList);

        /**
        * @fn           getPrimitiveSensorList
        * @brief        Get primitive sensor list
        *
        * @param        [out] std::vector<ISSMResource*> *pPrimitiveSensorList - List of primitive sensors
        * @return       SSMRESULT
        *                   SSM_S_OK
        *                   , SSM_S_FALSE
        *                   , SSM_E_POINTER
        *                   , SSM_E_OUTOFMEMORY
        *                   , SSM_E_FAIL
        *                   , SSM_E_NOINTERFACE
        *                   , SSM_E_NOTIMPL
        *
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getPrimitiveSensorList(OUT std::vector<ISSMResource *> *pPrimitiveSensorList);

        SSMRESULT registerResourceFinderEvent(IN IResourceEvent *pResourceEvent);
        SSMRESULT startResourceFinder();
        SSMRESULT onResourceFound(IN ISSMResource *pSensor);
        SSMRESULT onResourceLost(IN ISSMResource *pSensor);

        SSMRESULT startObserveResource(IN ISSMResource *pSensor, IN IEvent *pEvent);
        SSMRESULT stopObserveResource(IN ISSMResource *pSensor);

        SSMRESULT loadSoftSensor(IN std::string softSensorName, IN ICtxDelegate *pDelegate,
                                 OUT void **hSoftSensor);
        SSMRESULT unloadSoftSensor(IN void *hSoftSensor);

    private:
        SSMRESULT makeSSMResourceListForDictionaryData(IN std::vector<DictionaryData> dataList,
                OUT std::vector<ISSMResource *> *pList) ;
        SSMRESULT loadXMLFromFile(IN std::string descriptionFilePath,
                                  IN std::vector<DictionaryData> *dataList);
        SSMRESULT loadXMLFromString(IN char *xmlData, IN std::vector<DictionaryData> *dataList);
        SSMRESULT GetCurrentPath(OUT std::string *path);
};
#endif