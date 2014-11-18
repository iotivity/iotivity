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
#ifndef _ConditionedModel_H_
#define _ConditionedModel_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"

/**
* @class    CConditionedModel
* @brief    CConditionedModel Interface
*            This class represents Context Model's data search from given conditions
*
* @see
*/
class CConditionedModel :
    public CObjectRoot<CObjectMultiThreadModel>
    , public IConditionedModel
    , public IEvaluationEngineEvent
{
    private:
        CObjectPtr<IEvaluationEngine>           m_pEvaluationEngine;
        CObjectPtr<IContextModel>               m_pBaseModel;
        ModelConditionVec                       m_watchCondition;
        IConditionedModelEvent                  *m_pConditionedModelEvent;
        int                                     m_triggerId;
        CSimpleMutex                            m_mtxConditionedData;
        std::list<int>                          m_triggeredDataIds;
        IntVec                                  m_affectedDataIds;

    public:
        SSMRESULT finalConstruct();

        void finalRelease();

        /**
        * @fn     create
        * @brief Create Conditioned Model class using conditions.
        *
        * @param [in] IContextModel *pBaseModel - Base Context Model interface for Conditioned data reading
        * @param [in] ModelConditionVec *pModelConditions - Conditions for data retrieving
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT create(IN IContextModel *pBaseModel, IN ModelConditionVec *pModelConditions);

        /**
        * @fn     onWatchModelData
        * @brief Called when Model data meets given conditions.
        *
        * @param [in] int triggerId - Trigger id.
        * @param [in] int dataId - Affected data's Id
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT onWatchModelData(IN int triggerId, IN int dataId);

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IConditionedModel))
            {
                IBase *pBase = (IConditionedModel *)this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        /**
        * @fn     getBaseContextModel
        * @brief Get base Context Model interface
        *
        * @param [out] IContextModel **ppBaseContextModel - Interface pointer for retrieving base Context Model interface.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getBaseContextModel(OUT IContextModel **ppBaseContextModel);

        /**
        * @fn     registerConditionedModelEvent
        * @brief Register for receiving triggered event from given conditions
        *
        * @param [in] IConditionedModelEvent *pConditionedModelEvent - Interface for listening events.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT registerConditionedModelEvent(IN IConditionedModelEvent *pConditionedModelEvent);

        /**
        * @fn     activateTrigger
        * @brief Activate trigger using given conditions
        *
        * @param [out] int *pTriggerId - Activated trigger id.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT activateTrigger(OUT int *pTriggerId);

        /**
        * @fn     deactivateTrigger
        * @brief Deactivate trigger that activated
        *
        * @param NONE
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT deactivateTrigger();

        /**
        * @fn     hasAffectedData
        * @brief Quries if current Conditioned Model has affected data
        *
        * @param NONE
        *
        * @return bool
        * @warning
        * @exception
        * @see
        */
        bool hasAffectedData();

        /**
        * @fn     getAffectedData
        * @brief Get affected data from given conditions includes triggered data
        *
        * @param [out] IntVec   *pDataIds - Affected dataId sets.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getAffectedData(OUT IntVec *pDataIds);

        /**
        * @fn     getWatchCondition
        * @brief Get watching conditions created from Context Model
        *
        * @param [out] ModelConditionVec *pModelConditions - Affected dataId sets.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getWatchCondition(OUT ModelConditionVec *pModelConditions);
};

#endif
