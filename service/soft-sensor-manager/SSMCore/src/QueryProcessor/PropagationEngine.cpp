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
#include "PropagationEngine.h"
#include "ContextModel.h"

SSMRESULT CPropagationEngine::finalConstruct()
{
    SSMRESULT               res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_ITasker, (IBase **)&m_pTasker));

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IContextDataReader, (IBase **)&m_pContextDataReader));

    SSM_CLEANUP_ASSERT(m_pContextDataReader->registerContextModelAccessor(this));

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IEvaluationEngine, (IBase **)&m_pEvaluationEngine));

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IContextRepository, (IBase **)&m_pContextRepository));

    SSM_CLEANUP_ASSERT(m_pContextRepository->registerResourceFinderEvent(this));

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_ISensingEngine, (IBase **)&m_pSensingEngine));

    SSM_CLEANUP_ASSERT(initializeEngine());

CLEANUP:
    return res;
}

void CPropagationEngine::finalRelease()
{
    terminateEngine();

    for (std::map<std::string, IContextModel *>::iterator itor = m_installedContextModel.begin();
         itor != m_installedContextModel.end(); ++itor)
    {
        ((CContextModel *)itor->second)->registerContextModelEvent(NULL);
        SAFE_RELEASE(itor->second);
    }

    for (VirtualModelMap::iterator itor = m_lookUpContextModel.begin();
         itor != m_lookUpContextModel.end(); ++itor)
    {
        for (std::vector< std::pair<ModelConditionVec, IConditionedQuery *> >::iterator inneritor =
                 itor->second.begin(); inneritor != itor->second.end(); ++inneritor)
        {
            SAFE_RELEASE(inneritor->second);
        }
    }
}

SSMRESULT CPropagationEngine::findContextModel(IN std::string modelName,
        OUT IContextModel  **ppContextModel)
{
    SSMRESULT res = SSM_S_FALSE;
    std::map<std::string, IContextModel *>::iterator idxContextModel;

    m_mtxContextModelList.lock();
    idxContextModel = m_installedContextModel.find(modelName);

    if (idxContextModel != m_installedContextModel.end())
    {
        SSM_CLEANUP_ASSERT(idxContextModel->second->queryInterface(OID_IContextModel,
                           (IBase **)ppContextModel));
    }

CLEANUP:
    m_mtxContextModelList.unlock();
    return res;
}

SSMRESULT CPropagationEngine::registerCondition(IN IContextModel *pContextModel,
        IN ModelConditionVec *pConditions, IN IConditionedQuery *pConditionedQuery)
{
    SSMRESULT res = SSM_E_FAIL;
    IConditionedModel           *pConditionedModel = NULL;

    SSM_CLEANUP_ASSERT(pContextModel->createConditionedModel(pConditions, &pConditionedModel));
    SSM_CLEANUP_ASSERT(pConditionedQuery->registerConditionedModel(pConditionedModel));

    res = SSM_S_OK;

CLEANUP:
    SAFE_RELEASE(pConditionedModel);
    return res;
}

SSMRESULT CPropagationEngine::registerVirtualCondition(IN std::string virtualContextModelName,
        IN ModelConditionVec *pConditions, IN IConditionedQuery *pConditionedQuery)
{
    pConditionedQuery->addRef();
    m_mtxLookUpList.lock();
    m_lookUpContextModel[virtualContextModelName].push_back(std::make_pair(*pConditions,
            pConditionedQuery));
    m_mtxLookUpList.unlock();
    return SSM_S_OK;
}

SSMRESULT CPropagationEngine::propagateNewModel(IN std::string newModelName,
        IN IContextModel *pContextModel)
{
    SSMRESULT res = SSM_E_FAIL;
    VirtualModelMap::iterator   itorVModels;

    m_mtxLookUpList.lock();
    itorVModels = m_lookUpContextModel.find(newModelName);

    if (itorVModels != m_lookUpContextModel.end())
    {
        for (std::vector< std::pair<ModelConditionVec, IConditionedQuery *> >::iterator itor =
                 itorVModels->second.begin(); itor != itorVModels->second.end(); ++itor)
        {
            SSM_CLEANUP_ASSERT(registerCondition(pContextModel, &itor->first, itor->second));

            SAFE_RELEASE(itor->second);
        }

        m_lookUpContextModel.erase(itorVModels);
    }

    res = SSM_S_OK;

CLEANUP:
    m_mtxLookUpList.unlock();
    return res;
}

SSMRESULT CPropagationEngine::addResourceFromGetList()
{
    SSMRESULT res = SSM_E_FAIL;
    std::vector<ISSMResource *>      contextModelList;
    std::stringstream               sstream;

    m_pSensingEngine->getList(&contextModelList);
    for (std::vector<ISSMResource *>::iterator itor = contextModelList.begin();
         itor != contextModelList.end(); ++itor)
    {
        SSM_CLEANUP_ASSERT(installContextModelFromISSMResource(*itor));
    }

    res = SSM_S_OK;

CLEANUP:
    return res;
}

//Install new resource and update Device table
SSMRESULT CPropagationEngine::installContextModelFromISSMResource(IN ISSMResource *pSSMResource)
{
    SSMRESULT res = SSM_E_FAIL;

    ModelProperty               modelProperty;
    ModelPropertyVec            modelProperties;
    CObject<CContextModel>      *pContextModel;
    int                         deviceId = 0;
    std::string                 lifeTime;

    switch ((res = findContextModel(pSSMResource->type.c_str(), (IContextModel **)&pContextModel)))
    {
        //we found model
        case SSM_S_OK:
            break;

        //no models using that name, create new one
        case SSM_S_FALSE:
            for (std::vector<std::map<std::string, std::string> >::iterator itor =
                     pSSMResource->outputProperty.begin(); itor != pSSMResource->outputProperty.end();
                 ++itor)
            {
                if ((*itor)["name"] == "lifetime")
                {
                    lifeTime = (*itor)["value"];
                    continue;
                }

                modelProperty.propertyName = (*itor)["name"];
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

            //Install new resource
            SSM_CLEANUP_ASSERT(installContextModel(m_pDeviceModel, IContextModel::CONSTRUCTION_TYPE_EXTERNAL,
                                                   pSSMResource->type.c_str(), &modelProperties, (IContextModel **)&pContextModel));

            if (lifeTime.length() > 0)
            {
                pContextModel->setLifeTime(lifeTime);
            }

            modelProperties.clear();
            break;

        default:
            goto CLEANUP;
    }

    //Update Device data to DB
    SSM_CLEANUP_ASSERT(updateDeviceInfo(pSSMResource, &deviceId));

    //TODO: Must be modified for restructuring
    pContextModel->addSSMResourceAndDeviceDataId(pSSMResource->type, deviceId, pSSMResource);

    res = SSM_S_OK;

CLEANUP:
    SAFE_RELEASE(pContextModel);
    return res;
}

SSMRESULT CPropagationEngine::updateDeviceInfo(IN ISSMResource *pSSMResource, OUT int *deviceId)
{
    SSMRESULT res = SSM_E_FAIL;

    ModelProperty               modelProperty;
    ModelPropertyVec            modelProperties;
    std::map<std::string, int>::iterator    itorSearchedDeviceId;
    int                         deviceDataId;

    modelProperty.propertyName = "fname";
    modelProperty.propertyType = ModelProperty::TYPE_TEXT;
    modelProperty.propertyValue = pSSMResource->friendlyName;
    modelProperties.push_back(modelProperty);

    modelProperty.propertyName = "ip";
    modelProperty.propertyType = ModelProperty::TYPE_TEXT;
    modelProperty.propertyValue = pSSMResource->ip;
    modelProperties.push_back(modelProperty);

    modelProperty.propertyName = "version";
    modelProperty.propertyType = ModelProperty::TYPE_TEXT;
    modelProperty.propertyValue = "1.0";
    modelProperties.push_back(modelProperty);

    itorSearchedDeviceId = m_searchedSensorDataId.find(pSSMResource->ip);

    if (itorSearchedDeviceId == m_searchedSensorDataId.end())
    {
        SSM_CLEANUP_ASSERT(m_pDeviceModel->addModelData(1, &modelProperties, &deviceDataId));
        m_searchedSensorDataId[pSSMResource->ip] = deviceDataId;
    }
    else
    {
        deviceDataId = m_searchedSensorDataId[pSSMResource->ip];
    }

    if (deviceId != NULL)
        *deviceId = deviceDataId;

    res = SSM_S_OK;

CLEANUP:
    return res;
}

SSMRESULT CPropagationEngine::installCurrentSensors()
{
    SSMRESULT res = SSM_E_FAIL;
    std::vector<ISSMResource *>      contextModelList;

    m_pSensingEngine->getList(&contextModelList);

    for (std::vector<ISSMResource *>::iterator itor = contextModelList.begin();
         itor != contextModelList.end(); ++itor)
    {
        SSM_CLEANUP_ASSERT(installContextModelFromISSMResource(*itor));
    }

    res = SSM_S_OK;

CLEANUP:
    return res;
}

SSMRESULT CPropagationEngine::installContextModel(IN IContextModel *pParentModel,
        IN IContextModel::ConstructionType constructionType, IN const char *modelName,
        IN ModelPropertyVec *pModelDescs, OUT IContextModel **ppContextModel)
{
    SSMRESULT                   res = SSM_E_FAIL;
    CObject<CContextModel>      *pNewModel;

    SSM_CLEANUP_ASSERT(CreateInstance(OID_IContextModel, (IBase **)&pNewModel));
    SSM_CLEANUP_ASSERT(pNewModel->create(constructionType, pParentModel, modelName, pModelDescs));
    SSM_CLEANUP_ASSERT(pNewModel->registerContextModelEvent(this));

    pNewModel->addRef();
    m_mtxContextModelList.lock();
    m_installedContextModel[modelName] = pNewModel;
    m_mtxContextModelList.unlock();

    SSM_CLEANUP_ASSERT(propagateNewModel(modelName, pNewModel));

    if (ppContextModel)
    {
        SSM_CLEANUP_ASSERT(pNewModel->queryInterface(OID_IContextModel, (IBase **)ppContextModel));
    }

CLEANUP:
    SAFE_RELEASE(pNewModel);
    return res;
}

SSMRESULT CPropagationEngine::cleanUpRemoteDeviceInfo()
{
    SSMRESULT               res = SSM_E_FAIL;
    ModelConditionVec       deviceCondition(1);
    IConditionedModel       *pRemoteDeviceInformation = NULL;
    IntVec                  deviceDataIds;

    deviceCondition[0].modelProperty.propertyName = "dataId";
    deviceCondition[0].modelProperty.propertyValue = "1";
    deviceCondition[0].modelProperty.propertyType = ModelProperty::TYPE_NUMERIC;
    deviceCondition[0].predicate = ModelCondition::PREDICATE_GT;
    SSM_CLEANUP_ASSERT(m_pDeviceModel->createConditionedModel(&deviceCondition,
                       &pRemoteDeviceInformation));
    SSM_CLEANUP_ASSERT(pRemoteDeviceInformation->getAffectedData(&deviceDataIds));

    for (unsigned int i = 0; i < deviceDataIds.size(); i++)
    {
        SSM_CLEANUP_ASSERT(m_pDeviceModel->deleteModelData(deviceDataIds[i]));
    }

CLEANUP:
    SAFE_RELEASE(pRemoteDeviceInformation);
    return res;
}

void CPropagationEngine::onExecute(IN void *pArg)
{
    SSMRESULT res = SSM_E_FAIL;

    intptr_t         *pMessage = (intptr_t *)pArg;

    RESOURCE_EVENT_TYPE eventType = (RESOURCE_EVENT_TYPE)pMessage[0];
    ISSMResource            *pResource = (ISSMResource *)pMessage[1];

    switch (eventType)
    {
        case SSM_EVENT_NORMAL:
            SSM_CLEANUP_ASSERT(addResourceFromGetList());
            break;

        case SSM_EVENT_ADDED:
            SSM_CLEANUP_ASSERT(installContextModelFromISSMResource(pResource));
            break;

        case SSM_REMOVED:
            break;

        case SSM_UPDATED:
            break;

        default:
            break;
    }

CLEANUP:
    ;
}

void CPropagationEngine::onTerminate(IN void *pArg)
{
    intptr_t         *pMessage = (intptr_t *)pArg;

    delete[] pMessage;
}

int CPropagationEngine::onResourceEvent(IN RESOURCE_EVENT_TYPE eventType,
                                        IN ISSMResource *pSSMResource, IN std::string info)
{
    intptr_t          *pMessage = new intptr_t [2];

    pMessage[0] = eventType;
    pMessage[1] = reinterpret_cast<intptr_t>(pSSMResource);
    return (int)m_pTasker->addTask(this, (void *)pMessage);
}

SSMRESULT CPropagationEngine::initializeEngine()
{
    SSMRESULT               res = SSM_E_FAIL;
    CObject<CContextModel>  *pRootModel = NULL;
    ModelPropertyVec        rootModelProperties(1);

    ModelPropertyVec        deviceModelProperties(3);

    ISSMResource            ssmResource;

    SSM_CLEANUP_ASSERT(m_pEvaluationEngine->initializeEngine());

    //Create root model (Root)
    rootModelProperties[0].propertyName = "name";
    rootModelProperties[0].propertyType = ModelProperty::TYPE_TEXT;

    SSM_CLEANUP_ASSERT(CreateInstance(OID_IContextModel, (IBase **)&pRootModel));

    SSM_CLEANUP_ASSERT(pRootModel->create(IContextModel::CONSTRUCTION_TYPE_INTERNAL, NULL,
                                          "root", &rootModelProperties));
    SSM_CLEANUP_ASSERT(pRootModel->queryInterface(OID_IContextModel, (IBase **)&m_pRootModel));

    //Create basic model (Device)
    deviceModelProperties[0].propertyName = "fname";
    deviceModelProperties[0].propertyType = ModelProperty::TYPE_TEXT;
    deviceModelProperties[1].propertyName = "ip";
    deviceModelProperties[1].propertyType = ModelProperty::TYPE_TEXT;
    deviceModelProperties[2].propertyName = "version";
    deviceModelProperties[2].propertyType = ModelProperty::TYPE_TEXT;

    SSM_CLEANUP_ASSERT(installContextModel(m_pRootModel, IContextModel::CONSTRUCTION_TYPE_INTERNAL,
                                           "Device",
                                           &deviceModelProperties, &m_pDeviceModel));

    ssmResource.ip = "coap://127.0.0.1/";
    ssmResource.friendlyName = "MyDevice";

    SSM_CLEANUP_ASSERT(updateDeviceInfo(&ssmResource, NULL));

    SSM_CLEANUP_ASSERT(installCurrentSensors());

CLEANUP:
    SAFE_RELEASE(pRootModel);
    return res;
}

void CPropagationEngine::terminateEngine()
{
    SSMRESULT               res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(cleanUpRemoteDeviceInfo());

CLEANUP:
    return;
}

SSMRESULT CPropagationEngine::getContextModel(IN std:: string modelName,
        OUT IContextModel **ppContextModel)
{
    SSMRESULT res = SSM_E_FAIL;

    res = findContextModel(modelName, ppContextModel);

    return res == SSM_S_OK ? SSM_S_OK : SSM_E_FAIL;
}

SSMRESULT CPropagationEngine::createConditionedQuery(IN IContextModel::ActivationType
        activationType, IN QueryCondition *pQueryCondition, IN IConditionedQueryEvent *pEvent,
        OUT IConditionedQuery **ppConditionedQuery)
{
    SSMRESULT res = SSM_E_FAIL;
    CObject<CConditionedQuery>  *pConditionedQuery;
    IContextModel               *pContextModel = NULL;

    SSM_CLEANUP_ASSERT(CreateInstance(OID_IConditionedQuery, (IBase **)&pConditionedQuery));

    SSM_CLEANUP_ASSERT(pConditionedQuery->create(activationType, pQueryCondition->size()));
    for (QueryCondition::iterator itor = pQueryCondition->begin();
         itor != pQueryCondition->end(); ++itor)
    {
        switch (findContextModel(itor->first, &pContextModel))
        {
            case SSM_S_OK:
                SSM_CLEANUP_ASSERT(registerCondition(pContextModel, &itor->second, pConditionedQuery));
                break;

            case SSM_S_FALSE:
                SSM_CLEANUP_ASSERT(registerVirtualCondition(itor->first, &itor->second, pConditionedQuery));
                break;

            default:
                res = SSM_E_FAIL;
                goto CLEANUP;
        }
        SAFE_RELEASE(pContextModel);
    }

    SSM_CLEANUP_ASSERT(pConditionedQuery->registerQueryConditionEvent(pEvent));

    SSM_CLEANUP_ASSERT(pConditionedQuery->queryInterface(OID_IConditionedQuery,
                       (IBase **)ppConditionedQuery));

CLEANUP:
    SAFE_RELEASE(pConditionedQuery);
    SAFE_RELEASE(pContextModel);
    return res;
}

SSMRESULT CPropagationEngine::onModelStatusChange(IN IContextModel::Status newStatus,
        IN ISSMResource *pSSMResource, IN IContextModel *pModel)
{
    switch (newStatus)
    {
        case IContextModel::STATUS_ACTIVATE:
            pModel->addRef();
            m_pSensingEngine->registerContext(SSM_REPEAT, pSSMResource, (CContextModel *)pModel);
            break;

        case IContextModel::STATUS_DEACTIVATE:
            m_pSensingEngine->unregisterContext(SSM_REPEAT, pSSMResource, (CContextModel *)pModel);
            //pModel->CleanUpModelData();
            pModel->release();
            break;

        case IContextModel::STATUS_START_READ_VALUE:
            //Model must be released from OnEvent callType
            pModel->addRef();
            m_pSensingEngine->registerContext(SSM_ONCE, pSSMResource, (CContextModel *)pModel);
            break;

        case IContextModel::STATUS_STOP_READ_VALUE:
            pModel->release();
            break;

        default:
            break;
    }

    return SSM_S_OK;
}

SSMRESULT CPropagationEngine::onQueryContextModel(IN std::string modelName,
        OUT IContextModel **ppContextModel)
{
    std::map<std::string, IContextModel *>::iterator     itorContextModel;

    itorContextModel = m_installedContextModel.find(modelName);

    if (itorContextModel == m_installedContextModel.end())
    {
        return SSM_E_FAIL;
    }

    return itorContextModel->second->queryInterface(OID_IContextModel, (IBase **)ppContextModel);
}
