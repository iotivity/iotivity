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
#ifndef _EvaluationEngine_H_
#define _EvaluationEngine_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/sqlite3.h"
#include "Common/InternalInterface.h"

/**
* @class    CEvaluationEngine
* @brief    CEvaluationEngine Interface
*            This class represents Evaluation Engine
*
* @see
*/
class CEvaluationEngine :
    public CObjectRoot<CObjectMultiThreadModel>
    , public IEvaluationEngine
    , public IThreadClient
{
    private:
        sqlite3             *m_pSQLite3;
        int                 m_iTriggerId;
        CSimpleMutex        m_mtxTriggerId;
        std::map<int, IEvaluationEngineEvent *>  m_mapTriggers;
        CSimpleMutex        m_mtxDataRelation;
        CObjectPtr<ITasker> m_pTasker;

        SSMRESULT executeSQL_NoReturn(std::string strSQL);

        SSMRESULT executeSQL_IntReturn(std::string strSQL, int *pResult);

        static void onSQLTrigger(sqlite3_context *context, int argc, sqlite3_value **argv);

        SSMRESULT initializeEngine();

        void terminateEngine();

    public:
        SSMRESULT finalConstruct();

        void finalRelease();

        void onExecute(void *pArg);

        void onTerminate(void *pArg);

        /**
        * @fn     onWatcherTriggered
        * @brief Called when newly added data meets the conditions
        *
        * @param [in] int triggerId - Trigger id, which condition is triggered
        * @param [in] int dataId - Data id, which data meets the conditions
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT onWatcherTriggered(int triggerId, int dataId);

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IEvaluationEngine))
            {
                IBase *pBase = (IEvaluationEngine *)this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        /**
        * @fn     createModel
        * @brief Create new context model
        *
        * @param [in] int parentModelId - Parent Context Model id that related to Model hierarchy.
        * @param [in] const char *newModelName - Represent model's name
        * @param [in] ModelPropertyVec *pModelProperties - Represent model's properties that exposed to CQL
        * @param [out] int *pModelId - Created model's id
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT createModel(int parentModelId, const char *newModelName,
                              ModelPropertyVec *pModelDescs, int *pModelId);

        /**
        * @fn     addModelData
        * @brief Add model data to Context Model
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int parentModelId - parent model id that created CreateModel's execution.
        * @param [in] int parentDataId - Specify current model's parent Context Model's dataId
        * @param [in] ModelPropertyVec *pData - Packed data for adding
        * @param [out] int *pDataId - Returned dataId that newly added
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT addModelData(int modelId, int parentModelId, int parentDataId,
                               ModelPropertyVec *pModelValues, int *pDataId);

        /**
        * @fn     updateModelData
        * @brief Update model data to Context Model
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int dataId - DataId that provided AddModelData's execution
        * @param [in] ModelPropertyVec *pData - Packed data for updating
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT updateModelData(int modelId, int dataId, ModelPropertyVec *pModelValues);

        //SSMRESULT DeleteModel(int modelId);

        /**
        * @fn     deleteModelData
        * @brief Delete model data using given index
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int dataId - DataId that provided AddModelData's execution
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT deleteModelData(int modelId, int dataId);

        /**
        * @fn     getModelData
        * @brief Get model data
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int dataId - DataId that provided AddModelData's execution
        * @param [out] ModelPropertyVec *pData - Packed data
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getModelData(int modelId, int dataId, ModelPropertyVec *pModelProperties);

        /**
        * @fn     getModelDataSet
        * @brief Get model data set
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int startIndex - Starting index of model data
        * @param [in] int count - Number of data to retrieve
        * @param [out] vector<ModelPropertyVec> *pDataSet - Packed data
        * @param [out] int *pLastIndex - Index of last data
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getModelDataSet(int modelId, int startIndex, int count,
                                  std::vector<ModelPropertyVec> *pDataSet, int *pLastIndex);

        //SSMRESULT GetModelSchema(int modelId, ModelPropertyVec *pModelProperties);

        /**
        * @fn     getConditionedModelData
        * @brief Get affected data from given conditions
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] ModelConditionVec *pModelConditions - Conditions for data retrieving
        * @param [out] IntVec   *pDataIds - Searched dataId sets.
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getConditionedModelData(int modelId, ModelConditionVec *pModelConditions,
                                          IntVec *pDataIds);

        /**
        * @fn     watchModelData
        * @brief Watching data from given conditions
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] ModelConditionVec *pModelConditions - Conditions for data triggering
        * @param [in] IEvaluationEngineEvent *pEvaluationEngineEvent - Event interface where triggered event listening
        * @param [out] int *pTriggerId - Identifier which trigger is activated
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT watchModelData(int modelId, ModelConditionVec *pModelConditions,
                                 IEvaluationEngineEvent *pEvaluationEngineEvent, int *pTriggerId);

        /**
        * @fn     dropWatchModelData
        * @brief Stop watching data
        *
        * @param [in] int triggerId - Identifier which trigger to drop
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT dropWatchModelData(int triggerId);

        /**
        * @fn     getParentDataId
        * @brief Get parent dataId related to model hierarchy
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int dataId - DataId that current model's
        * @param [in] int parentModelId - Designated parent Context Model's id
        * @param [out] int *pParentDataId - Designated parent Context Model's data's Id
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getParentDataId(int modelId, int dataId, int parentModelId,
                                  int *pParentDataId);

        /**
        * @fn     getChildDataId
        * @brief Get child dataId related to model hierarchy
        *
        * @param [in] int modelId - model id that created CreateModel's execution.
        * @param [in] int dataId - DataId that current model's
        * @param [in] int childModelId - Designated child Context Model's id
        * @param [out] IntVec *pChildDataIds - Designated child Context Model's data's Id
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getChildDataId(int modelId, int dataId, int childModelId,
                                 IntVec *pChildDataIds);

        //SSMRESULT GetPathToRoot(int currentModelId, IntVec *pPath);

        //SSMRESULT GetModelId(const char *modelName, int *pModelId);

        //SSMRESULT GetModelHierarchy(int rootModelId, StringVec *pPath);

        //SSMRESULT GetDataHierarchy(int rootModelId, int dataId, StringVec *pPath);
};

#endif
