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
#include "ConditionedQueryResult.h"

SSMRESULT CConditionedQueryResult::finalConstruct()
{
    SSMRESULT res = SSM_E_FAIL;

    res = SSM_S_OK;
    //CLEANUP:
    return res;
}

void CConditionedQueryResult::finalRelease()
{
    for (std::vector<IConditionedModel *>::iterator itor = m_conditionedModels.begin();
         itor != m_conditionedModels.end(); ++itor)
    {
        SAFE_RELEASE((*itor));
    }
}

SSMRESULT CConditionedQueryResult::addConditionedModel(IN IConditionedModel *pConditionedModel)
{
    pConditionedModel->addRef();
    m_conditionedModels.push_back(pConditionedModel);

    return SSM_S_OK;
}

unsigned int CConditionedQueryResult::getConditionedModelCount()
{
    return m_conditionedModels.size();
}

SSMRESULT CConditionedQueryResult::getConditionedContextModel(IN unsigned int conditionedModelIndex,
        OUT IConditionedModel **ppConditionedModel)
{
    if (conditionedModelIndex > m_conditionedModels.size() - 1)
    {
        return SSM_E_FAIL;
    }

    return m_conditionedModels[conditionedModelIndex]->queryInterface(OID_IConditionedModel,
            (IBase **)ppConditionedModel);
}
