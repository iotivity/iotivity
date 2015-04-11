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
#include "QueryEngine.h"
#include "DataReader.h"

SSMRESULT CQueryEngine::finalConstruct()
{
    SSMRESULT res = SSM_E_FAIL;

    m_cqid = 0;

    m_pQueryEngineEvent = NULL;

    SSM_CLEANUP_ASSERT(CreateInstance(OID_ITasker, (IBase **)&m_pTasker));

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IPropagationEngine, (IBase **)&m_pPropagationEngine));

CLEANUP:
    return res;
}

void CQueryEngine::finalRelease()
{
    m_pQueryEngineEvent = NULL;

    m_mtxQueries.lock();

    for (std::map<int, IConditionedQuery *>::iterator itor = m_conditionedQueries.begin();
         itor != m_conditionedQueries.end(); ++itor)
    {
        itor->second->deactivateTriggers();
        SAFE_RELEASE(itor->second);
    }

    for (std::map<int, CContextQuery *>::iterator itor = m_contextQueries.begin();
         itor != m_contextQueries.end(); ++itor)
    {
        //Token *root = itor->second->getRoot();
        //SAFE_DELETE(root);
        SAFE_DELETE(itor->second);
    }

    m_mtxQueries.unlock();
}

SSMRESULT CQueryEngine::processQueryResult(int userTriggerId,
        std::vector<result_model> *result)
{
    SSMRESULT           res = SSM_E_FAIL;
    ModelPropertyVec    modelData;
    std::vector<result_model> result_model_data_id;
    IntVec                  modelID;
    std::vector<std::string> contextName;
    IContextModel           *temp_contextmodel = NULL;
    IContextModel           *temp_contextmodel2 = NULL;

    intptr_t             *pData = NULL;

    CDataReader *pDataReader = NULL;

    m_mtxQueries.lock();

    if (m_contextQueries.find(userTriggerId) == m_contextQueries.end())
    {
        SSM_CLEANUP_ASSERT(SSM_E_FAIL);
    }

    m_contextQueries[userTriggerId]->check_result_model();
    m_contextQueries[userTriggerId]->return_contextName(&contextName);
    m_contextQueries[userTriggerId]->return_modelID(&modelID);

    for (unsigned int i = 0; i < modelID.size(); i++)
    {
        SSM_CLEANUP_ASSERT(m_pPropagationEngine->getContextModel(contextName.at(i), &temp_contextmodel2));

        for (unsigned int j = 0 ; j < result->size() ; j++)
        {
            int data;
            if (result->at(j).dataId.size() <= 0)
            {
                continue;
            }

            int modelid = modelID.at(i);
            std::vector<int> dataid;
            for (unsigned int k = 0 ; k < result->at(j).dataId.size() ; k++)
            {
                SSM_CLEANUP_ASSERT(m_pPropagationEngine->getContextModel(result->at(j).modelName,
                                   &temp_contextmodel));
                data = result->at(j).dataId.at(k);

                if (modelID.at(i) < result->at(j).modelID )
                {
                    SSM_CLEANUP_ASSERT(temp_contextmodel->getParentDataId(data, temp_contextmodel2, &data));
                    dataid.push_back(data);
                }
                else if (modelID.at(i) > result->at(j).modelID )
                {
                    SSM_CLEANUP_ASSERT(temp_contextmodel->getChildDataId(data, temp_contextmodel2, &dataid));
                }
                else
                {
                    dataid.push_back(data);
                }
                SAFE_RELEASE(temp_contextmodel);
            }

            m_contextQueries[userTriggerId]->integrate_result(&result_model_data_id, modelid, &dataid,
                    temp_contextmodel2->getModelName());
        }

        SAFE_RELEASE(temp_contextmodel2);
    }

    pDataReader = new CDataReader();

    for (unsigned int i = 0; i < result_model_data_id.size(); i++)
    {
        std::vector<CModelData *>    modelDataSet;

        for (unsigned int j = 0; j < (result_model_data_id)[i].dataId.size(); j++)
        {
            CModelData *pModelData = new CModelData();
            IContextModel       *pCM = NULL;
            ModelPropertyVec    modelPropertyVec;

            SSM_CLEANUP_ASSERT(m_pPropagationEngine->getContextModel((result_model_data_id)[i].modelName,
                               &pCM));
            SSM_CLEANUP_ASSERT(pCM->getModelData((result_model_data_id)[i].dataId[j], &modelPropertyVec));
            pModelData->setDataId((result_model_data_id)[i].dataId[j]);
            for (ModelPropertyVec::iterator itor = modelPropertyVec.begin();
                 itor != modelPropertyVec.end(); ++itor)
            {
                pModelData->addModelData(itor->propertyName, itor->propertyValue);
            }

            modelDataSet.push_back(pModelData);

            SAFE_RELEASE(pCM);
        }

        SSM_CLEANUP_ASSERT(pDataReader->addModelData((result_model_data_id)[i].modelName, &modelDataSet));
    }
    pData = new intptr_t [3];
    pData[0] = EVENT_TYPE_OUTER;
    pData[1] = userTriggerId;
    pData[2] = reinterpret_cast<intptr_t>(pDataReader);

    m_pTasker->addTask(this, (void *)pData);

    res = SSM_S_OK;

CLEANUP:
    m_mtxQueries.unlock();
    SAFE_RELEASE(temp_contextmodel);
    SAFE_RELEASE(temp_contextmodel2);
    return res;
}

SSMRESULT CQueryEngine::validateQueryResult(IConditionedQueryResult *pConditionedQueryResult,
        std::vector<result_model> *resultData)
{
    SSMRESULT               res = SSM_E_FAIL;
    IContextModel           *pContextModel = NULL;
    IConditionedModel       *pConditionedModel = NULL;

    for (unsigned int i = 0 ; i < pConditionedQueryResult->getConditionedModelCount() ; i++)
    {
        std::vector<int>    temp_dataID;
        result_model        temp_result;
        SSM_CLEANUP_ASSERT(pConditionedQueryResult->getConditionedContextModel(i, &pConditionedModel));
        SSM_CLEANUP_ASSERT(pConditionedModel->getAffectedData(&temp_dataID));

        if (temp_dataID.size() == 0)
        {
            break;
        }

        SSM_CLEANUP_ASSERT(pConditionedModel->getBaseContextModel(&pContextModel));
        temp_result.modelID = pContextModel->getModelId();
        temp_result.dataId = temp_dataID;
        temp_result.modelName = pContextModel->getModelName();
        resultData->push_back(temp_result);
        SAFE_RELEASE(pConditionedModel);
        SAFE_RELEASE(pContextModel);
    }

    if (resultData->size() == pConditionedQueryResult->getConditionedModelCount())
    {
        res = SSM_S_OK;
    }
    else
    {
        res = SSM_S_FALSE;
    }

CLEANUP:
    SAFE_RELEASE(pConditionedModel);
    SAFE_RELEASE(pContextModel);
    return res;
}

SSMRESULT CQueryEngine::onConditionedQueryEvent(int userTriggerId,
        IConditionedQueryResult *pConditionedQueryResult)
{
    SSMRESULT                   res = SSM_E_FAIL;
    std::vector<result_model>   result;

    SSM_CLEANUP_ASSERT(validateQueryResult(pConditionedQueryResult, &result));
    SSM_CLEANUP_ASSERT(processQueryResult(userTriggerId, &result));

CLEANUP:
    return res;
}

void CQueryEngine::onExecute(void *pArg)
{
    intptr_t *pData = (intptr_t *)pArg;

    switch (pData[0])
    {
        case EVENT_TYPE_INNER:
            processQueryResult(pData[1], (std::vector<result_model> *)pData[2]);
            break;

        case EVENT_TYPE_OUTER:
            if (m_pQueryEngineEvent != NULL)
            {
                m_pQueryEngineEvent->onQueryEngineEvent(pData[1], (IDataReader *)pData[2]);
            }
            break;

        default:
            break;
    }
}

void CQueryEngine::onTerminate(void *pArg)
{
    intptr_t *pData = (intptr_t *)pArg;
    std::vector<result_model>   *pResult = NULL;
    CDataReader                 *pDataReader = NULL;

    switch (pData[0])
    {
        case EVENT_TYPE_INNER:
            pResult = (std::vector<result_model> *)pData[2];
            SAFE_DELETE(pResult);
            break;

        case EVENT_TYPE_OUTER:
            pDataReader = (CDataReader *)pData[2];
            SAFE_DELETE(pDataReader);
            break;

        default:
            break;
    }
    SAFE_ARRAY_DELETE(pData);
}

SSMRESULT CQueryEngine::executeContextQuery(std::string contextQuery, int *cqid)
{
    SSMRESULT               res = SSM_E_FAIL;
    IConditionedQuery       *pConditionedQuery = NULL;
    IConditionedQueryResult *pConditionedQueryResult = NULL;
    QueryCondition          queryConditions;
    CContextQuery           *clsContextQuery = NULL;
    Token                   token;
    CCQLParser              cqlParser;
    IContextModel::ActivationType   queryCommandType;

    if (!cqlParser.parse(contextQuery, &token))
    {
        SSM_CLEANUP_ASSERT(SSM_E_FAIL);
    }

    if (!cqlParser.check_grammer(&token))
    {
        SSM_CLEANUP_ASSERT(SSM_E_FAIL);
    }

    clsContextQuery = new CContextQuery();
    SSM_CLEANUP_NULL_ASSERT(clsContextQuery);
    SSM_CLEANUP_ASSERT(clsContextQuery->initialize(token));
    clsContextQuery->make_QueryCondition(&queryConditions);

    if (CCQLParser::tolower(token.child_token[0].name) == "subscribe")
    {
        queryCommandType = IContextModel::ACTIVATION_TYPE_SUBSCRIBE;
    }
    else
    {
        queryCommandType = IContextModel::ACTIVATION_TYPE_GET;
    }

    SSM_CLEANUP_ASSERT(m_pPropagationEngine->createConditionedQuery(queryCommandType,
                       &queryConditions, this, &pConditionedQuery));

    m_mtxQueries.lock();
    pConditionedQuery->addRef();
    m_conditionedQueries[m_cqid] = pConditionedQuery;
    m_contextQueries[m_cqid] = clsContextQuery;
    m_mtxQueries.unlock();

    if (pConditionedQuery->hasAllConditionedModels() == true)
    {
        std::vector<result_model>   *pResult = NULL;

        SSM_CLEANUP_ASSERT(pConditionedQuery->getConditionedQueryResult(&pConditionedQueryResult));
        pResult = new std::vector<result_model>();
        if (validateQueryResult(pConditionedQueryResult, pResult) == SSM_S_OK)
        {
            //We have valid data, let's deliver to application.
            intptr_t  *pData = new intptr_t [3];
            pData[0] = EVENT_TYPE_INNER;
            pData[1] = m_cqid;
            pData[2] = reinterpret_cast<intptr_t>(pResult);

            m_pTasker->addTask(this, (void *)pData);
        }
        else
        {
            SAFE_DELETE(pResult);
            if (queryCommandType == IContextModel::ACTIVATION_TYPE_GET)
            {
                //There is no valid data. let's request new one
                SSM_CLEANUP_ASSERT(pConditionedQuery->activateTriggers(m_cqid));
            }
        }
    }
    else
    {
        if (queryCommandType == IContextModel::ACTIVATION_TYPE_GET)
        {
            //There is no models such name
            SSM_CLEANUP_ASSERT(SSM_E_FAIL);
        }
    }

    //Every subscribe command must request new data to models
    if (queryCommandType == IContextModel::ACTIVATION_TYPE_SUBSCRIBE)
    {
        SSM_CLEANUP_ASSERT(pConditionedQuery->activateTriggers(m_cqid));
    }

    *cqid = m_cqid++;

CLEANUP:
    SAFE_RELEASE(pConditionedQuery);
    SAFE_RELEASE(pConditionedQueryResult);
    return res;
}

//TODO: Registration with multiple instance support
SSMRESULT CQueryEngine::registerQueryEvent(IQueryEngineEvent *pQueryEngineEvent)
{
    m_pQueryEngineEvent = pQueryEngineEvent;
    return SSM_S_OK;
}

SSMRESULT CQueryEngine::unregisterQueryEvent(IQueryEngineEvent *pQueryEngineEvent)
{
    if (m_pQueryEngineEvent == pQueryEngineEvent)
    {
        m_pQueryEngineEvent = NULL;
        return SSM_S_OK;
    }

    return SSM_E_FAIL;
}

SSMRESULT CQueryEngine::killContextQuery(int cqid)
{
    SSMRESULT res = SSM_E_FAIL;

    std::map<int, IConditionedQuery *>::iterator itorConditionedQueries;
    std::map<int, CContextQuery *>::iterator itorContextQuries;

    m_mtxQueries.lock();
    itorConditionedQueries = m_conditionedQueries.find(cqid);
    itorContextQuries = m_contextQueries.find(cqid);

    if (itorConditionedQueries != m_conditionedQueries.end())
    {
        SSM_CLEANUP_ASSERT(itorConditionedQueries->second->deactivateTriggers());
        itorConditionedQueries->second->release();
        m_conditionedQueries.erase(itorConditionedQueries);
    }

    if (itorContextQuries != m_contextQueries.end())
    {
        SAFE_DELETE(itorContextQuries->second);
        m_contextQueries.erase(itorContextQuries);
    }

CLEANUP:
    m_mtxQueries.unlock();
    return res;
}
