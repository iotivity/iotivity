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
#ifndef _SoftSensorManager_H_
#define _SoftSensorManager_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"

/**
* @class    CSoftSensorManager
* @brief    CSoftSensorManager Interface
*            This class represents main class of SSM.
*
* @see
*           static ISoftSensorManager       *g_pSoftSensorManager = NULL;
*           CreateInstance(OID_ISoftSensorManager, (IBase**)&g_pSoftSensorManager);
*           g_pSoftSensorManager->TerminateCore();
*           g_pSoftSensorManager->Release();
*/

class CSoftSensorManager :
    public CObjectRoot<CObjectMultiThreadModel>
    , public ISoftSensorManager
{
    private:
        CObjectPtr<IContextRepository>      m_pContextRepository;
        CObjectPtr<IPropagationEngine>      m_pPropagationEngine;
        CObjectPtr<ISensingEngine>          m_pSensingEngine;

    public:
        SSMRESULT finalConstruct();

        void finalRelease();

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_ISoftSensorManager))
            {
                IBase *pBase = this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        /**
        * @fn initializeCore
        * @brief Initialize core using given configuration information.
        *           <SSMCore>
        *               <Device>
        *                   <UDN>windows7-31f8-11b4-a222-08002b34c003</UDN>
        *                   <Name>MyPC</Name>
        *                   <Type>PC</Type>
        *               </Device>
        *           </SSMCore>
        *
        * @param [in] std::string xmlDescription - specification described in XML
        * @param [out] IBase** ppvObject - reference pointer to get instance pointer
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT initializeCore(std::string xmlDescription);

        /**
        * @fn startCore
        * @brief Start Core.
        *         This enables network capability that can discover and discovered.
        *
        * @param None
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT startCore();

        /**
        * @fn stopCore
        * @brief Stop Core.
        *         This disables network capability that can discover and discovered.
        *
        * @param None
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT stopCore();

        /**
        * @fn TerminateCore
        * @brief Terminates Core.
        *         Free all allocated resources
        *
        * @param None
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT terminateCore(bool factoryResetFlag);

        /**
        * @fn createQueryEngine
        * @brief Create new Query Engine instance.
        *         Each engine instance has it's own event thread
        *         we recommend each application should creates only one instance.
        *
        * @param [out] IQueryEngine **ppQueryEngine - instance pointer of new Query Engine
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT createQueryEngine(IQueryEngine **ppQueryEngine);

        /**
        * @fn releaseQueryEngine
        * @brief Release exists Query Engine instance.
        *
        * @param [in] IQueryEngine *pQueryEngine - instance pointer of exists Query Engine
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        unsigned long releaseQueryEngine(IQueryEngine *pQueryEngine);

        /**
        * @fn getInstalledModelList
        * @brief Gets all installed models from local and remote
        *
        * @param [out] std::vector<ISSMResource*> *pList - List of installed context model
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getInstalledModelList(std::vector<ISSMResource *> *pList);
};

#endif
