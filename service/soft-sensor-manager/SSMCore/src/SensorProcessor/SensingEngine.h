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
#ifndef _ResponseReactor_H_
#define _ResponseReactor_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"

/**
 * @class    CSensingEngine
 * @brief    Class for implementing main abstration of SensorProcessor
 *
 *
 * @see
 */
class CSensingEngine :
    public CObjectRoot<CObjectMultiThreadModel>
    , public ISensingEngine
{
    private:
        CObjectPtr<IContextRepository>  m_pContextRepository;
        CObjectPtr<IContextExecutor>    m_pContextExecutor;

        /**
        * @brief requested high layer's callback data.(IEvent instance, deviceId, call type)
        */
        std::map<std::string, CallbackData > m_requestedCallbackData;

        /**
        * @brief Context model data from lower layer(OnEvent Callback)
        */
        std::map<std::string, std::vector<ContextData> > m_storedLowerContextData;

        CSimpleMutex                        m_mtxRequestedContextData;
        CSimpleMutex                        m_mtxUnregisterContext;

    public:
        SSMRESULT finalConstruct();
        void finalRelease();

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_ISensingEngine))
            {
                IBase *pBase = this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        /**
        * @fn           registerContext
        * @brief        Register context model request.
        *
        * @param        [in]  TypeofEvent callType -  Type of event. SSM_ONCE or SSM_REPEAT
        * @param        [in]  ISSMResource *pSSMResource -  Requested context model resource.
        * @param        [in]  IEvent *pEvent -  IEvent class for callback.
        *
        * @return       SSMRESULT
        *
        * @warning
        * @exception
        * @see
        */
        SSMRESULT registerContext(IN TypeofEvent callType, IN ISSMResource *pSSMResource,
                                  IN IEvent *pEvent);

        /**
        * @fn           unregisterContext
        * @brief        Unregister context model request.
        *
        * @param        [in]  TypeofEvent callType -  Type of event. SSM_ONCE or SSM_REPEAT
        * @param        [in]  ISSMResource *pSSMResource -  Requested context model resource.
        * @param        [in]  IEvent *pEvent -  IEvent class for callback.
        *
        * @return       SSMRESULT
        *
        * @warning
        * @exception
        * @see
        */
        SSMRESULT  unregisterContext(IN TypeofEvent callType, IN ISSMResource *pSSMResource,
                                     IN IEvent *pEvent);

        /**
        * @fn           getList
        * @brief        Get context model list
        *
        * @param        [out]  std::vector<ISSMResource> *pList - ISSMResource vector of low level context models or high level context models.
        *
        * @return       SSMRESULT
        *
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getList(OUT std::vector<ISSMResource *> *pList);

        /**
        * @fn           onEvent
        * @brief        IEvent Interface.
        *               Call from executor when context model data generated.
        *
        * @param        [in]  std::string name - caller Resource name.
        * @param        [in]  TypeofEvent callType -  context event type.
        * @param        [in]  std::vector<ContextData> ctxData -  context data
        *
        * @return       int - function result status
        *
        * @warning
        * @exception
        * @see
        */
        int onEvent(IN std::string name, IN TypeofEvent callType, IN std::vector<ContextData> ctxData);
};
#endif