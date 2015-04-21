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
#include "ConditionedModel.h"

SSMRESULT CConditionedModel::finalConstruct()
{
    SSMRESULT res = SSM_E_FAIL;

    m_triggerId = -1;

    m_pConditionedModelEvent = NULL;

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IEvaluationEngine, (IBase **)&m_pEvaluationEngine));

CLEANUP:
    return res;
}

void CConditionedModel::finalRelease()
{
    m_pConditionedModelEvent = NULL;

    if (m_triggerId > -1)
    {
        deactivateTrigger();
    }

    m_triggerId = -1;
}

SSMRESULT CConditionedModel::create(IContextModel *pBaseModel,
                                    ModelConditionVec *pModelConditions)
{
    m_watchCondition = *pModelConditions;
    return pBaseModel->queryInterface(OID_IContextModel, (IBase **)&m_pBaseModel);
}

SSMRESULT CConditionedModel::getBaseContextModel(IContextModel **ppBaseContextModel)
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(m_pBaseModel->queryInterface(OID_IContextModel, (IBase **)ppBaseContextModel));

CLEANUP:
    return res;
}

SSMRESULT CConditionedModel::registerConditionedModelEvent(IConditionedModelEvent
        *pConditionedModelEvent)
{
    m_pConditionedModelEvent = pConditionedModelEvent;
    return SSM_S_OK;
}

SSMRESULT CConditionedModel::onWatchModelData(int triggerId, int dataId)
{
    SSMRESULT res = SSM_E_FAIL;

    m_mtxConditionedData.lock();
    m_triggeredDataIds.push_back(dataId);
    m_mtxConditionedData.unlock();

    if (m_pConditionedModelEvent)
    {
        res = m_pConditionedModelEvent->onConditionedModelTriggered(triggerId);
    }

    return res;
}

SSMRESULT CConditionedModel::activateTrigger(int *pTriggerId)
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(m_pEvaluationEngine->watchModelData(m_pBaseModel->getModelId(),
                       &m_watchCondition, this, &m_triggerId));

    *pTriggerId = m_triggerId;

CLEANUP:
    return res;
}

SSMRESULT CConditionedModel::deactivateTrigger()
{
    SSMRESULT res = SSM_S_OK;

    if (m_triggerId > -1)
    {
        SSM_CLEANUP_ASSERT(m_pEvaluationEngine->dropWatchModelData(m_triggerId));
    }

    m_triggerId = -1;

CLEANUP:
    return res;
}

bool CConditionedModel::hasAffectedData()
{
    bool ret = false;

    m_mtxConditionedData.lock();

    if (m_triggeredDataIds.size() > 0 || m_affectedDataIds.size() > 0)
    {
        ret = true;
        goto CLEANUP;
    }

    if (m_pEvaluationEngine->getConditionedModelData(m_pBaseModel->getModelId(),
            &m_watchCondition, &m_affectedDataIds) != SSM_S_OK)
    {
        ret = false;
        goto CLEANUP;
    }

    if (m_affectedDataIds.size() > 0)
    {
        ret = true;
        goto CLEANUP;
    }

CLEANUP:
    m_mtxConditionedData.unlock();
    return ret;
}

SSMRESULT CConditionedModel::getAffectedData(IntVec *pDataIds)
{
    SSMRESULT res = SSM_E_FAIL;

    m_mtxConditionedData.lock();

    if (m_triggeredDataIds.size() > 0)
    {
        pDataIds->push_back(m_triggeredDataIds.front());
        m_triggeredDataIds.pop_front();
        SSM_CLEANUP_ASSERT(SSM_S_OK);
    }
    else
    {
        m_affectedDataIds.clear();
        SSM_CLEANUP_ASSERT(m_pEvaluationEngine->getConditionedModelData(m_pBaseModel->getModelId(),
                           &m_watchCondition, &m_affectedDataIds));
        *pDataIds = m_affectedDataIds;
    }

CLEANUP:
    m_mtxConditionedData.unlock();
    return res;
}

SSMRESULT CConditionedModel::getWatchCondition(ModelConditionVec *pModelConditions)
{
    *pModelConditions = m_watchCondition;
    return SSM_S_OK;
}
