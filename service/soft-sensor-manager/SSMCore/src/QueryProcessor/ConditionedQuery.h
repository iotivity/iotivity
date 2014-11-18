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
#ifndef _ConditionedQuery_H_
#define _ConditionedQuery_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"
#include "ConditionedQueryResult.h"
/**
* @class    CConditionedQuery
* @brief    CConditionedQuery Interface
*            This class represents parsed CQL for data retrieving
*
* @see
*/
class CConditionedQuery :
    public CObjectRoot<CObjectMultiThreadModel>
    , public IConditionedQuery
    , public IThreadClient
    , public IConditionedModelEvent
{
    private:
        CObjectPtr<ITasker>                     m_pTasker;
        //CObjectPtr<IPropagationEngine>            m_PropagationEngine;
        CObject<CConditionedQueryResult>        *m_pConditionedQueryResult;
        std::vector<IConditionedModel *>         m_conditionedModels;
        CSimpleMutex                            m_mtxConditionCount;
        IConditionedQueryEvent                  *m_conditionedQueryEvent;
        int                                     m_untrackedConditionsCount;
        bool                                    m_reservedForActivate;
        int                                     m_userTriggerId;
        IContextModel::ActivationType           m_activationType;
        int                                     m_targetDeviceDataId;

    public:
        SSMRESULT finalConstruct();

        void finalRelease();

        /**
        * @fn     create
        * @brief Create Conditioned Query for multiple data searching or triggering
        *
        * @param [in] IContextModel::ActivationType activationType - Specify ContextModel's working type
        * @param [in] int totalConditionsCount - Specify total condition's count each condition is related to each ContextModel
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT create(IN IContextModel::ActivationType activationType, IN int totalConditionsCount);

        /**
        * @fn     registerQueryConditionEvent
        * @brief Register IConditionedQueryEvent interface for listening events
        *
        * @param [in] IConditionedQueryEvent *pConditionedQueryEvent - Interface pointer for listening
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT registerQueryConditionEvent(IN IConditionedQueryEvent *pConditionedQueryEvent);

        /**
        * @fn     onConditionedModelTriggered
        * @brief Called when each Conditioned Model meets the conditions
        *
        * @param [in] int triggerId - trigger id that which Conditioned Model triggered
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT onConditionedModelTriggered(IN int triggerId);

        void onExecute(IN void *pArg);

        void onTerminate(IN void *pArg);

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IConditionedQuery))
            {
                IBase *pBase = (IConditionedQuery *)this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        /**
        * @fn     registerConditionedModel
        * @brief Register Conditioned Model for tracking triggers
        *
        * @param [in] IConditionedModel *pConditionedModel - Interface of Conditioned Model
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT registerConditionedModel(IN IConditionedModel *pConditionedModel);

        /**
        * @fn     activateTriggers
        * @brief Activate triggers for data evaluating. This changed related ContextModel's status to running
        *
        * @param [in] int userTriggerId - Specify TriggerId for event listening
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT activateTriggers(IN int userTriggerId);

        /**
        * @fn     deactivateTriggers
        * @brief Deactivate triggers for data evaluating.
        *
        * @param [in] int userTriggerId - Specify TriggerId for event listening
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT deactivateTriggers();

        /**
        * @fn     getConditionedQueryResult
        * @brief Retrieves conditioned data from current cached data
        *
        * @param [out] IConditionedQueryResult **ppConditionedQueryResult - Interface of result reader
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getConditionedQueryResult(OUT IConditionedQueryResult **ppConditionedQueryResult);

        /**
        * @fn     hasAllConditionedModels
        * @brief Retrieves current conditioned query has all Conditioned ContextModels for evaluating
        *
        * @param NONE
        *
        * @return bool
        * @warning
        * @exception
        * @see
        */
        bool hasAllConditionedModels();
};

#endif
