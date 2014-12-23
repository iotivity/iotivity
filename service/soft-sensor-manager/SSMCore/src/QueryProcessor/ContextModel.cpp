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
#include "ContextModel.h"
#include "ConditionedModel.h"

SSMRESULT CContextModel::finalConstruct()
{
    SSMRESULT res = SSM_E_FAIL;
    ModelProperty       defaultModelProperty;

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_ITasker, (IBase **)&m_pTasker));

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IEvaluationEngine, (IBase **)&m_pEvaluationEngine));

    m_modelId = 0;

    m_pContextModelEvent = NULL;

    m_activationCount = 0;

    m_constructionType = CONSTRUCTION_TYPE_INTERNAL;

    m_secLifeTime = "2147483647";

    defaultModelProperty.propertyName = "dataId";
    defaultModelProperty.propertyType = ModelProperty::TYPE_INTEGER;
    m_modelProperties.push_back(defaultModelProperty);

    defaultModelProperty.propertyName = "lastTime";
    defaultModelProperty.propertyType = ModelProperty::TYPE_INTEGER;
    m_modelProperties.push_back(defaultModelProperty);

    defaultModelProperty.propertyName = "lifetime";
    defaultModelProperty.propertyType = ModelProperty::TYPE_INTEGER;
    m_modelProperties.push_back(defaultModelProperty);

    defaultModelProperty.propertyName = "available";
    defaultModelProperty.propertyType = ModelProperty::TYPE_TEXT;
    m_modelProperties.push_back(defaultModelProperty);

CLEANUP:
    return res;
}

void CContextModel::finalRelease()
{
    SSMRESULT       res = SSM_E_FAIL;
    std::map<int, IntVec>::iterator     itorDeviceId = m_mapDeviceDataIds.begin();
    m_pContextModelEvent = NULL;

    for (unsigned int i = 0; i < m_mapDeviceDataIds.size(); i++)
    {
        //Other Device
        if (itorDeviceId->first > 1)
        {
            for (unsigned int j = 0; j < itorDeviceId->second.size(); j++)
            {
                SSM_CLEANUP_ASSERT(deleteModelData(itorDeviceId->second[j]));
            }
        }

        itorDeviceId++;
    }

    //for(std::vector<IConditionedModel*>::iterator itor = m_ConditionedModels.begin();
    //  itor != m_ConditionedModels.end(); ++itor)
    //{
    //  (*itor)->Release();
    //}

CLEANUP:
    return;
}

void CContextModel::registerSSMResource(IN ActivationType activationType, IN int targetDeviceDataId,
                                        IN ISSMResource *pSSMResource)
{
    intptr_t     *pData = NULL;

    m_mtxActivationCount.lock();
    switch (activationType)
    {
        case ACTIVATION_TYPE_SUBSCRIBE:
            if (m_mapSubscribedDevice.find(targetDeviceDataId) == m_mapSubscribedDevice.end())
            {
                pData = new intptr_t  [2];
                pData[0] = STATUS_ACTIVATE;
                pData[1] = reinterpret_cast<intptr_t>(pSSMResource);
                m_pTasker->addTask(this, (void *)pData);
                m_mapSubscribedDevice[targetDeviceDataId] = 1;
            }
            else
            {
                m_mapSubscribedDevice[targetDeviceDataId]++;
            }
            break;

        case ACTIVATION_TYPE_GET:
            if (m_mapSubscribedDevice.find(targetDeviceDataId) == m_mapSubscribedDevice.end())
            {
                if (m_mapGetDevice.find(targetDeviceDataId) == m_mapGetDevice.end())
                {
                    pData = new intptr_t[2];
                    pData[0] = STATUS_START_READ_VALUE;
                    pData[1] = reinterpret_cast<intptr_t>(pSSMResource);
                    m_pTasker->addTask(this, (void *)pData);
                    m_mapGetDevice[targetDeviceDataId] = 1;
                }
                else
                {
                    m_mapGetDevice[targetDeviceDataId]++;
                }
            }
            break;

        default:
            break;
    }
    m_mtxActivationCount.unlock();
}

void CContextModel::unregisterSSMResource(IN ActivationType activationType,
        IN int targetDeviceDataId, IN ISSMResource *pSSMResource)
{
    intptr_t     *pData = NULL;

    m_mtxActivationCount.lock();
    switch (activationType)
    {
        case ACTIVATION_TYPE_SUBSCRIBE:
            if (m_mapSubscribedDevice.find(targetDeviceDataId) != m_mapSubscribedDevice.end())
            {
                if (--m_mapSubscribedDevice[targetDeviceDataId] == 0)
                {
                    pData = new intptr_t [2];
                    pData[0] = STATUS_DEACTIVATE;
                    pData[1] = reinterpret_cast<intptr_t>(pSSMResource);
                    m_pTasker->addTask(this, (void *)pData);
                    m_mapSubscribedDevice.erase(targetDeviceDataId);
                }
            }
            break;

        case ACTIVATION_TYPE_GET:
            if (m_mapGetDevice.find(targetDeviceDataId) != m_mapGetDevice.end())
            {
                if (--m_mapGetDevice[targetDeviceDataId] == 0)
                {
                    pData = new intptr_t [2];
                    pData[0] = STATUS_STOP_READ_VALUE;
                    pData[1] = reinterpret_cast<intptr_t>(pSSMResource);
                    m_pTasker->addTask(this, (void *)pData);
                }
            }
            break;

        default:
            break;
    }
    m_mtxActivationCount.unlock();
}

SSMRESULT CContextModel::create(IN ConstructionType constructionType,
                                IN IContextModel *pParentModel, IN std::string modelName, IN ModelPropertyVec *pModelProperties)
{
    SSMRESULT res = SSM_E_FAIL;
    int         modelId = 1;

    if (pParentModel)
    {
        SSM_CLEANUP_ASSERT(pParentModel->queryInterface(OID_IContextModel, (IBase **)&m_pParentModel));
        modelId = pParentModel->getModelId();
    }

    m_constructionType = constructionType;
    m_modelName = modelName;

    SSM_CLEANUP_ASSERT(m_pEvaluationEngine->createModel(modelId, modelName.c_str(), pModelProperties,
                       &m_modelId));

    for (ModelPropertyVec::iterator itor = pModelProperties->begin();
         itor != pModelProperties->end(); ++itor)
    {
        m_modelProperties.push_back(*itor);
    }

CLEANUP:
    return res;
}

SSMRESULT CContextModel::registerContextModelEvent(IN IContextModelEvent *pContextModelEvent)
{
    m_pContextModelEvent = pContextModelEvent;
    return SSM_S_OK;
}

void CContextModel::onExecute(IN void *pArg)
{
    intptr_t *pData = (intptr_t *)pArg;

    if (m_pContextModelEvent)
    {
        m_pContextModelEvent->onModelStatusChange((Status)pData[0], (ISSMResource *)pData[1], this);
    }
}

void CContextModel::onTerminate(IN void *pArg)
{
    intptr_t *pData = (intptr_t *)pArg;
    SAFE_ARRAY_DELETE(pData);
}

//TODO: called when new data arrived
int CContextModel::onEvent(IN std::string deviceID, IN TypeofEvent callType,
                           IN std::vector<ContextData> ctxData)
{
    SSMRESULT           res = SSM_E_FAIL;
    int                 dataId = 0;
    ModelProperty       modelProperty;
    ModelPropertyVec    modelProperties;
    IntVec              *deviceDataIds = NULL;
    std::map<int, IntVec>::iterator     itorDeviceDataIds;
    int                 idxUpdate = 0;

    if (m_modelName != ctxData[0].rootName)
    {
        SSM_CLEANUP_ASSERT(SSM_E_POINTER);
    }

    if (m_mapSSM.find(deviceID) == m_mapSSM.end())
    {
        SSM_CLEANUP_ASSERT(SSM_E_POINTER);
    }

    itorDeviceDataIds = m_mapDeviceDataIds.find(m_mapSSM[deviceID].first);

    if (itorDeviceDataIds != m_mapDeviceDataIds.end())
    {
        deviceDataIds = &(itorDeviceDataIds->second);
    }

    for (std::vector<ContextData>::iterator itorContextData =
             ctxData.begin(); itorContextData != ctxData.end(); ++itorContextData)
    {
        for (std::vector<std::map<std::string, std::string> >::iterator itor =
                 (*itorContextData).outputProperty.begin(); itor != (*itorContextData).outputProperty.end();
             ++itor)
        {
            if (itor->find("error") != itor->end())
            {
                modelProperty.propertyName = "available";
                modelProperty.propertyValue = "false";
                modelProperty.propertyType = ModelProperty::TYPE_TEXT;
                modelProperties.push_back(modelProperty);
                break;
            }

            modelProperty.propertyName = (*itor)["name"];
            modelProperty.propertyValue = (*itor)["value"];
            if ((*itor)["type"] == "string")
            {
                modelProperty.propertyType = ModelProperty::TYPE_TEXT;
            }
            else if ((*itor)["type"] == "int")
            {
                modelProperty.propertyType = ModelProperty::TYPE_INTEGER;
            }
            else if ((*itor)["type"] == "double")
            {
                modelProperty.propertyType = ModelProperty::TYPE_REAL;
            }
            else if ((*itor)["type"] == "float")
            {
                modelProperty.propertyType = ModelProperty::TYPE_REAL;
            }
            else
            {
                modelProperty.propertyType = ModelProperty::TYPE_TEXT;
            }
            modelProperties.push_back(modelProperty);
        }

        //Add lifetime to field
        modelProperty.propertyName = "lifetime";
        modelProperty.propertyValue = m_secLifeTime;
        modelProperty.propertyType = ModelProperty::TYPE_INTEGER;

        modelProperties.push_back(modelProperty);

        if (deviceDataIds == NULL)
        {
            SSM_CLEANUP_ASSERT(addModelData(m_mapSSM[deviceID].first, &modelProperties, &dataId));
            m_mapDeviceDataIds[m_mapSSM[deviceID].first].push_back(dataId);
        }
        else
        {
            SSM_CLEANUP_ASSERT(updateModelData(m_mapDeviceDataIds[m_mapSSM[deviceID].first][idxUpdate++],
                                               &modelProperties));
        }

        modelProperties.clear();
    }

CLEANUP:
    return 0;
}

int CContextModel::getModelId()
{
    return m_modelId;
}

std::string CContextModel::getModelName()
{
    return m_modelName;
}

IContextModel::ConstructionType CContextModel::getConstructionType()
{
    return m_constructionType;
}

SSMRESULT CContextModel::getParentDataId(IN int dataId, IN IContextModel *pParentModel,
        OUT int *pParentDataId)
{
    return m_pEvaluationEngine->getParentDataId(m_modelId, dataId, pParentModel->getModelId(),
            pParentDataId);
}

SSMRESULT CContextModel::getChildDataId(IN int dataId, IN IContextModel *pChildModel,
                                        OUT IntVec *pChildDataIds)
{
    return m_pEvaluationEngine->getChildDataId(m_modelId, dataId, pChildModel->getModelId(),
            pChildDataIds);
}

SSMRESULT CContextModel::activate(IN ActivationType activationType, IN int targetDeviceDataId)
{
    if (targetDeviceDataId < 0 && activationType == ACTIVATION_TYPE_SUBSCRIBE)
    {
        m_mtxActivationCount.lock();
        m_activationCount++;
        m_mtxActivationCount.unlock();
    }

    for (std::map<std::string, std::pair<int, ISSMResource> >::iterator itor = m_mapSSM.begin();
         itor != m_mapSSM.end(); ++itor)
    {
        if (targetDeviceDataId < 0)
        {
            registerSSMResource(activationType, itor->second.first, &itor->second.second);
        }
        else if (itor->second.first == targetDeviceDataId)
        {
            registerSSMResource(activationType, itor->second.first, &itor->second.second);
            break;
        }
    }

    return SSM_S_OK;
}

SSMRESULT CContextModel::deactivate(IN ActivationType activationType, IN int targetDeviceDataId)
{
    if (targetDeviceDataId < 0 && activationType == ACTIVATION_TYPE_SUBSCRIBE)
    {
        m_mtxActivationCount.lock();
        m_activationCount--;
        m_mtxActivationCount.unlock();
    }

    for (std::map<std::string, std::pair<int, ISSMResource> >::iterator itor = m_mapSSM.begin();
         itor != m_mapSSM.end(); ++itor)
    {
        if (targetDeviceDataId < 0)
        {
            unregisterSSMResource(activationType, itor->second.first, &itor->second.second);
        }
        else if (itor->second.first == targetDeviceDataId)
        {
            unregisterSSMResource(activationType, itor->second.first, &itor->second.second);
            break;
        }
    }

    return SSM_S_OK;
}
/*
SSMRESULT CContextModel::GetModelSchema(OUT ModelPropertyVec *pModelProperties)
{
    return m_pEvaluationEngine->GetModelSchema(m_ModelId, pModelProperties);
}
*/
SSMRESULT CContextModel::addModelData(IN int parentDataId, IN ModelPropertyVec *pData,
                                      OUT int *pDataId)
{
    return m_pEvaluationEngine->addModelData(m_modelId, m_pParentModel->getModelId(), parentDataId,
            pData, pDataId);
}

SSMRESULT CContextModel::updateModelData(IN int dataId, IN ModelPropertyVec *pData)
{
    return m_pEvaluationEngine->updateModelData(m_modelId, dataId, pData);
}

SSMRESULT CContextModel::deleteModelData(IN int dataId)
{
    return m_pEvaluationEngine->deleteModelData(m_modelId, dataId);
}

SSMRESULT CContextModel::getModelData(IN int dataId, OUT ModelPropertyVec *pData)
{
    SSMRESULT res = SSM_E_FAIL;
    int     i = 0;

    SSM_CLEANUP_ASSERT(m_pEvaluationEngine->getModelData(m_modelId, dataId, pData));
    for (ModelPropertyVec::iterator itor = pData->begin(); itor != pData->end(); ++itor)
    {
        itor->propertyName = m_modelProperties[i++].propertyName;
    }

CLEANUP:
    return res;
}

SSMRESULT CContextModel::getModelDataSet(IN int startIndex, IN int count,
        OUT std::vector<ModelPropertyVec> *pDataSet, OUT int *pLastIndex)
{
    SSMRESULT res = SSM_E_FAIL;
    int     i;

    SSM_CLEANUP_ASSERT(m_pEvaluationEngine->getModelDataSet(m_modelId, startIndex, count, pDataSet,
                       pLastIndex));

    for (std::vector<ModelPropertyVec>::iterator itorModelProperty = pDataSet->begin();
         itorModelProperty != pDataSet->end(); ++itorModelProperty)
    {
        i = 0;
        for (ModelPropertyVec::iterator itor = itorModelProperty->begin(); itor != itorModelProperty->end();
             ++itor)
        {
            itor->propertyName = m_modelProperties[i++].propertyName;
        }
    }

CLEANUP:
    return res;
}

SSMRESULT CContextModel::createConditionedModel(IN ModelConditionVec *pModelConditionVec,
        OUT IConditionedModel **ppConditionedModel)
{
    SSMRESULT res = SSM_E_FAIL;
    CObject<CConditionedModel>                  *pConditionedModel;

    SSM_CLEANUP_ASSERT(CreateInstance(OID_IConditionedModel, (IBase **)&pConditionedModel));
    SSM_CLEANUP_ASSERT(pConditionedModel->create(this, pModelConditionVec));
    SSM_CLEANUP_ASSERT(pConditionedModel->queryInterface(OID_IConditionedModel,
                       (IBase **)ppConditionedModel));

    //(*ppConditionedModel)->AddRef();
    //m_ConditionedModels.push_back(*ppConditionedModel);

CLEANUP:
    SAFE_RELEASE(pConditionedModel);
    return res;
}
/*
SSMRESULT CContextModel::CleanUpModelData()
{
    SSMRESULT res = SSM_E_FAIL;

    for(std::map<int,int>::iterator itor = m_mapDataId.begin();
        itor != m_mapDataId.end(); ++itor)
    {
        CHK_SSMRESULT(DeleteModelData(itor->second));
    }

    res = SSM_S_OK;

CLEANUP:
    return res;
}
*/

//Called new install or re install
void CContextModel::addSSMResourceAndDeviceDataId(IN std::string deviceId, IN int deviceDataId,
        IN ISSMResource *pSSMResource)
{
    if (deviceDataId == 1)
    {
        IntVec dataId;
        m_pParentModel->getChildDataId(deviceDataId, this, &dataId);
        if (dataId.size() > 0)
        {
            m_mapDeviceDataIds[deviceDataId] = dataId;
        }
    }

    if (m_mapSSM.find(deviceId) != m_mapSSM.end())
    {
        m_mtxActivationCount.lock();
        if (m_activationCount > 0)
        {
            unregisterSSMResource(ACTIVATION_TYPE_SUBSCRIBE, deviceDataId, &m_mapSSM[deviceId].second);
        }
        m_mtxActivationCount.unlock();
    }
    else
    {
        m_mapSSM[deviceId] = std::pair<int, ISSMResource>(deviceDataId, *pSSMResource);
    }

    m_mtxActivationCount.lock();
    if (m_activationCount > 0)
    {
        registerSSMResource(ACTIVATION_TYPE_SUBSCRIBE, deviceDataId, &m_mapSSM[deviceId].second);
    }
    m_mtxActivationCount.unlock();
}

void CContextModel::setLifeTime(std::string seconds)
{
    if (atoi(seconds.c_str()) > 0)
    {
        m_secLifeTime = seconds;
    }
}
