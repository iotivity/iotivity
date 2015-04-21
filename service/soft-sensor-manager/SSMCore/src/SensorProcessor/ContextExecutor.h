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
#ifndef _ContextExecutor_H_
#define _ContextExecutor_H_

#include "SSMInterface/SSMCore.h"
#include "SSMInterface/SSMModelDefinition.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"

#include "ContextRepository.h"

/**
 * @class    CContextExecutor
 * @brief    Class for execute context model.
 *           Delegate requested context to lower layer.
 *           Execute context model.
 *
 * @see
 */
class CContextExecutor :
    public CObjectRoot<CObjectMultiThreadModel>
    , public IContextExecutor
    , public IResourceEvent
    , public IThreadClient
{
    private:
        CObjectPtr<ITasker>             m_pTasker;

        CObjectPtr<IContextRepository>  m_pContextRepository;

        CObjectPtr<IContextDataReader>  m_pContextDataReader;

        /**
        * @brief Sensor data from primitive sensors
        */
        std::map<std::string, std::vector<ContextData> >        m_storedPrimitiveSensorData;

        /**
        * @brief key = primitive, soft sensor / values = soft sensors that has key in the input list
        */
        std::map<std::string, std::vector<std::string> >        m_relatedSoftSensor;

        /**
        * @brief requested high layer's callback data.(IEvent instance, deviceId, call type)
        */
        std::map<std::string, CallbackData>                 m_requestedContextCallback;

        /**
        * @brief registered high layer's context resource data.
        */
        std::map<std::string, ISSMResource *>               m_registeredResources;

        /**
        * @brief context model event of context model library
        */
        std::map<std::string, ICtxEvent *>                   m_ctxEventList;

        /**
        * @brief context model library list
        */
        std::map<std::string, void *>                        m_libraryList;

        ICtxEvent                                           *ctxEvent;
        //CSimpleMutex                                      m_mtxRequestedContextCallback;

        CSimpleMutex                                        m_mtxLibraryIO, m_mtxUserDataIO;

        std::map<std::string, std::vector<CallbackData> >   m_mapResourceLookup;

    public:

        SSMRESULT finalConstruct();
        void finalRelease();

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IContextExecutor))
            {
                IBase *pBase = (IContextExecutor *)this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        /**
        * @fn           registerCallback
        * @brief        ICtxDelegate Interface.
        *               If context model call this function, then get model's instance
        *
        * @param        [in]  ICtxEvent *pEvent -  context model's ICtxEvent instance.
        *
        * @return       void
        *
        * @warning
        * @exception
        * @see
        */
        void registerCallback(ICtxEvent *pEvent);

        /**
        * @fn           addOutput
        * @brief        ICtxDelegate Interface.
        *               Get context model's result and delegate to upper layer.
        *
        * @param        [in]  std::vector<ContextData> contextData -  context model's result
        * @return       void
        *
        * @warning
        * @exception
        * @see
        */
        void addOutput(std::vector<ContextData> contextData);

        /**
        * @fn           getDataFromDatabase
        * @brief        ICtxDelegate Interface.
        *               Get context model's historical data
        *
        * @param        [in] std::string modelName - Model name that looking for
        * @param        [in] int startIndex - Starting index of model data
        * @param        [in] int count - Number of data to retrieve
        * @param        [out] std::vector<ContextData> *data   -  data from database
        * @param        [out] int *pLastIndex - Index of last data
        * @return       void
        *
        * @warning
        * @exception
        * @see
        */
        void getDataFromDatabase(std::string modelName, int startIndex, int count,
                                 std::vector<ContextData> *data, int *pLastIndex);

        /**
        * @fn           onEvent
        * @brief        IEvent Interface.
        *               Call from lower layer when lower context data generated.
        *
        * @param        [in]  std::string deviceID -  caller device UUID.
        * @param        [in]  TypeofEvent callType -  context event type.
        * @param        [in]  std::vector<ContextData> ctxData -  context data
        *
        * @return       int - function result status
        *
        * @warning
        * @exception
        * @see
        */
        int onEvent(std::string deviceID, TypeofEvent callType, std::vector<ContextData> ctxData);

        /**
        * @fn           registerContext
        * @brief        Register context model request.
        *
        * @param        [in]  TypeofEvent callType -  Type of event. SSM_ONCE or SSM_REPEAT
        * @param        [in]  ISSMResource *pSSMResource -  Requested context model resource.
        * @param        [in]  IEvent *pEvent -  IEvent class for callback.
        *
        * @return       void
        *
        * @warning
        * @exception
        * @see
        */
        void registerContext(TypeofEvent callType, ISSMResource *pSSMResouce, IEvent *pEvent);

        /**
        * @fn           unregisterContext
        * @brief        Unregister context model request.
        *
        * @param        [in]  TypeofEvent callType -  Type of event. SSM_ONCE or SSM_REPEAT
        * @param        [in]  ISSMResource *pSSMResource -  Requested context model resource.
        * @param        [in]  IEvent *pEvent -  IEvent class for callback.
        *
        * @return       void
        *
        * @warning
        * @exception
        * @see
        */
        void  unregisterContext(TypeofEvent callType, ISSMResource *pSSMResource, IEvent *pEvent);

        void onExecute(void *pArg);
        void onTerminate(void *pArg);
        int onResourceEvent(RESOURCE_EVENT_TYPE eventType, ISSMResource *pSSMResource,
                            std::string info);

    private:
        SSMRESULT findString(std::vector<ISSMResource *> *sList, const std::string str,
                             ISSMResource **ppResource);
        std::map<std::string, std::vector<ContextData> >  getPreparedContextList(
            std::string primitiveSensor);
        void runLogic(std::vector<ContextData> inputData, std::string softSensor);
};

#endif
