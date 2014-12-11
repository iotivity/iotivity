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
#ifndef _PropagationEngine_H_
#define _PropagationEngine_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"
#include "ConditionedQuery.h"

typedef std::map< std::string, std::vector< std::pair<ModelConditionVec, IConditionedQuery *> > >
VirtualModelMap;

/**
* @class    CPropagationEngine
* @brief    CPropagationEngine Interface
*            This class represents propagation engine that manages ContextModel's automation
*
* @see
*/
class CPropagationEngine :
    public CObjectRoot<CObjectMultiThreadModel>
    , public IPropagationEngine
    , public IContextModelAccessor
    , public IThreadClient
    , public IContextModelEvent
    , public IResourceEvent
{
    private:
        CObjectPtr<ITasker>                         m_pTasker;
        CObjectPtr<IEvaluationEngine>               m_pEvaluationEngine;
        CObjectPtr<IContextModel>                   m_pRootModel;

        CObjectPtr<IContextDataReader>              m_pContextDataReader;
        CObjectPtr<IContextRepository>              m_pContextRepository;

        std::map<std::string, IContextModel *>       m_installedContextModel;
        VirtualModelMap                             m_lookUpContextModel;
        CSimpleMutex                                m_mtxContextModelList;
        CSimpleMutex                                m_mtxLookUpList;

        CObjectPtr<ISensingEngine>                  m_pSensingEngine;

        CObjectPtr<IContextModel>                   m_pDeviceModel;

        std::map<std::string, int>                  m_searchedSensorDataId;

        SSMRESULT findContextModel(IN std::string modelName, OUT IContextModel  **ppContextModel);

        SSMRESULT registerCondition(IN IContextModel *pContextModel, IN ModelConditionVec *pConditions,
                                    IN IConditionedQuery *pConditionedQuery);

        SSMRESULT registerVirtualCondition(IN std::string virtualContextModelName,
                                           IN ModelConditionVec *pConditions, IN IConditionedQuery *pConditionedQuery);

        SSMRESULT propagateNewModel(IN std::string newModelName, IN IContextModel *pContextModel);

        SSMRESULT addResourceFromGetList();

        //TODO: Install Context model from SSMResource
        SSMRESULT installCurrentSensors();

        SSMRESULT installContextModelFromISSMResource(IN ISSMResource *pSSMResource);

        SSMRESULT installContextModel(IN IContextModel *pParentModel,
                                      IN IContextModel::ConstructionType constructionType,
                                      IN const char *modelName, IN ModelPropertyVec *pModelDescs, OUT IContextModel **ppContextModel);

        SSMRESULT recoverEngineFromDatabase();

        SSMRESULT cleanUpRemoteDeviceInfo();

        SSMRESULT initializeEngine();

        void terminateEngine();

        SSMRESULT updateDeviceInfo(IN ISSMResource *pSSMResource, OUT int *deviceId);

    public:
        SSMRESULT finalConstruct();

        void finalRelease();

        void onExecute(IN void *pArg);

        void onTerminate(IN void *pArg);

        /**
        * @fn     onResourceEvent
        * @brief Called when new device has discovered
        *
        * @param [in] RESOURCE_EVENT_TYPE eventType - Enumerator that represents Device's status
        * @param [in] ISSMResource *pSSMResouce - NULL
        * @param [in] std::string info - NULL
        *
        * @return int
        * @warning
        * @exception
        * @see
        */
        int onResourceEvent(IN RESOURCE_EVENT_TYPE eventType, IN ISSMResource *pSSMResource,
                            IN std::string info);

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IPropagationEngine))
            {
                IBase *pBase = (IPropagationEngine *)this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        /**
        * @fn     getContextModel
        * @brief Retrieve ContextModel interface specified name
        *
        * @param [in] std::string modelName - ContextModel's name that looking for
        * @param [out] IContextModel **ppContextModel - Interface pointer for retrieving
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getContextModel(IN std::string modelName, OUT IContextModel **ppContextModel);

        /**
        * @fn     createConditionedQuery
        * @brief Create Conditioned Query using conditions
        *
        * @param [in] IContextModel::ActivationType activationType - ActivationType for ContextModel's working strategy
        * @param [in] QueryCondition *pQueryCondition - Conditions for data retrieving
        * @param [in] IConditionedQueryEvent *pEvent - Event interface for Conditioned Result listening
        * @param [out] IConditionedQuery **ppConditionedQuery - Interface for retrieving Conditioned Query
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT createConditionedQuery(IN IContextModel::ActivationType activationType,
                                         IN QueryCondition *pQueryCondition,
                                         IN IConditionedQueryEvent *pEvent, OUT IConditionedQuery **ppConditionedQuery);

        /**
        * @fn     onModelStatusChange
        * @brief Called when ContextModel wants to activate or deactivate
        *
        * @param [in] IContextModel::Status newStatus - Context Model's new status
        * @param [in] ISSMResource *pSSMResource - Instance of actual resource
        * @param [in] IContextModel *pModel - Affected Context Model
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT onModelStatusChange(IN IContextModel::Status newStatus, IN ISSMResource *pSSMResource,
                                      IN IContextModel *pModel);

        /**
        * @fn     onQueryContextModel
        * @brief Called when ContextModelAccessor wants to query installed ContextModel
        *
        * @param [in] std::string modelName - ContextModel name to find out
        * @param [out] IContextModel **ppContextModel - Interface of quried ContextModel
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT onQueryContextModel(IN std::string modelName, OUT IContextModel **ppContextModel);
};
#endif
