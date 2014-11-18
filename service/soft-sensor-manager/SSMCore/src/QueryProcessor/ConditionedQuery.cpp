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
#include "ConditionedQuery.h"

SSMRESULT CConditionedQuery::finalConstruct()
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_ITasker, (IBase **)&m_pTasker));
    SSM_CLEANUP_ASSERT(CreateInstance(OID_IConditionedQueryResult,
                                      (IBase **)&m_pConditionedQueryResult));

    m_conditionedQueryEvent = NULL;

    m_untrackedConditionsCount = 0;

    m_reservedForActivate = false;

    m_userTriggerId = 0;

    m_targetDeviceDataId = -1;

CLEANUP:
    return res;
}

void CConditionedQuery::finalRelease()
{
    m_conditionedQueryEvent = NULL;

    SAFE_RELEASE(m_pConditionedQueryResult);

    for (std::vector<IConditionedModel *>::iterator itor = m_conditionedModels.begin();
         itor != m_conditionedModels.end(); ++itor)
    {
        (*itor)->registerConditionedModelEvent(NULL);
        SAFE_RELEASE((*itor));
    }
}

SSMRESULT CConditionedQuery::create(IN IContextModel::ActivationType activationType,
                                    IN int totalConditionsCount)
{
    m_activationType = activationType;
    m_untrackedConditionsCount = totalConditionsCount;
    return SSM_S_OK;
}

SSMRESULT CConditionedQuery::registerQueryConditionEvent(IN IConditionedQueryEvent
        *pConditionedQueryEvent)
{
    m_conditionedQueryEvent = pConditionedQueryEvent;
    return SSM_S_OK;
}

SSMRESULT CConditionedQuery::onConditionedModelTriggered(IN int triggerId)
{
    SSMRESULT res = SSM_E_FAIL;
    int         evaluatedConditions = m_conditionedModels.size();

    for (unsigned int i = 0; i < m_conditionedModels.size(); i++)
    {
        if (m_conditionedModels[i]->hasAffectedData())
        {
            evaluatedConditions--;
        }
    }

    if (evaluatedConditions == 0)
    {
        m_pConditionedQueryResult->addRef();
        SSM_CLEANUP_ASSERT(m_pTasker->addTask(this, (IConditionedQueryResult *)m_pConditionedQueryResult));
    }

    res = SSM_S_OK;

CLEANUP:
    return res;
}

void CConditionedQuery::onExecute(IN void *pArg)
{
    if (m_conditionedQueryEvent)
    {
        m_conditionedQueryEvent->onConditionedQueryEvent(m_userTriggerId, (IConditionedQueryResult *)pArg);
    }
}

void CConditionedQuery::onTerminate(IN void *pArg)
{
    IConditionedQueryResult *pResult = (IConditionedQueryResult *)pArg;
    SAFE_RELEASE(pResult);
}

SSMRESULT CConditionedQuery::registerConditionedModel(IN IConditionedModel *pConditionedModel)
{
    SSMRESULT           res = SSM_E_FAIL;
    IContextModel       *pContextModel = NULL;

    SSM_CLEANUP_ASSERT(pConditionedModel->registerConditionedModelEvent(this));
    SSM_CLEANUP_ASSERT(m_pConditionedQueryResult->addConditionedModel(pConditionedModel));
    pConditionedModel->addRef();
    m_conditionedModels.push_back(pConditionedModel);

    m_untrackedConditionsCount--;

    SSM_CLEANUP_ASSERT(pConditionedModel->getBaseContextModel(&pContextModel));

    //Some special cases, we use device::dataId property to efficient request
    if (pContextModel->getModelName() == "Device")
    {
        ModelConditionVec   modelConditionVec;
        SSM_CLEANUP_ASSERT(pConditionedModel->getWatchCondition(&modelConditionVec));
        for (ModelConditionVec::iterator itor = modelConditionVec.begin();
             itor != modelConditionVec.end(); ++itor)
        {
            if (itor->modelProperty.propertyName == "dataId" &&
                itor->predicate == ModelCondition::PREDICATE_EQ)
            {
                m_targetDeviceDataId = atoi(itor->modelProperty.propertyValue.c_str());
                break;
            }
        }
    }

    if (hasAllConditionedModels() == true && m_reservedForActivate)
    {
        SSM_CLEANUP_ASSERT(activateTriggers(m_userTriggerId));
    }

CLEANUP:
    SAFE_RELEASE(pContextModel);
    return res;
}

SSMRESULT CConditionedQuery::activateTriggers(IN int userTriggerId)
{
    SSMRESULT           res = SSM_E_FAIL;
    int                 triggerId = 0;
    IContextModel       *pBaseContextModel = NULL;

    m_userTriggerId = userTriggerId;

    if (hasAllConditionedModels() == false)
    {
        m_reservedForActivate = true;
        return SSM_S_OK;
    }

    for (std::vector<IConditionedModel *>::iterator itor = m_conditionedModels.begin();
         itor != m_conditionedModels.end(); ++itor)
    {
        SSM_CLEANUP_ASSERT((*itor)->activateTrigger(&triggerId));
        SSM_CLEANUP_ASSERT((*itor)->getBaseContextModel(&pBaseContextModel));

        switch (pBaseContextModel->getConstructionType())
        {
            case IContextModel::CONSTRUCTION_TYPE_EXTERNAL:
            case IContextModel::CONSTRUCTION_TYPE_REMOTE:
                SSM_CLEANUP_ASSERT(pBaseContextModel->activate(m_activationType, m_targetDeviceDataId));
                break;

            default:
                break;
        }
        SAFE_RELEASE(pBaseContextModel);
    }

CLEANUP:
    SAFE_RELEASE(pBaseContextModel);
    return res;
}

SSMRESULT CConditionedQuery::deactivateTriggers()
{
    SSMRESULT res = SSM_E_FAIL;
    IContextModel       *pBaseContextModel = NULL;

    for (std::vector<IConditionedModel *>::iterator itor = m_conditionedModels.begin();
         itor != m_conditionedModels.end(); ++itor)
    {
        SSM_CLEANUP_ASSERT((*itor)->deactivateTrigger());
        SSM_CLEANUP_ASSERT((*itor)->getBaseContextModel(&pBaseContextModel));

        switch (pBaseContextModel->getConstructionType())
        {
            case IContextModel::CONSTRUCTION_TYPE_EXTERNAL:
            case IContextModel::CONSTRUCTION_TYPE_REMOTE:
                SSM_CLEANUP_ASSERT(pBaseContextModel->deactivate(m_activationType, m_targetDeviceDataId));
                break;

            default:
                break;
        }
        SAFE_RELEASE(pBaseContextModel);
    }

CLEANUP:
    SAFE_RELEASE(pBaseContextModel);
    return res;
}

SSMRESULT CConditionedQuery::getConditionedQueryResult(OUT IConditionedQueryResult
        **ppConditionedQueryResult)
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(m_pConditionedQueryResult->queryInterface(OID_IConditionedQueryResult,
                       (IBase **)ppConditionedQueryResult));

CLEANUP:
    return res;
}

bool CConditionedQuery::hasAllConditionedModels()
{
    return m_untrackedConditionsCount == 0 ? true : false;
}
