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
#include "ContextExecutor.h"

SSMRESULT CContextExecutor::finalConstruct()
{
    SSMRESULT res = SSM_E_FAIL;

    ctxEvent = NULL;

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_ITasker, (IBase **)&m_pTasker));
    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IContextRepository, (IBase **)&m_pContextRepository));
    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IContextDataReader, (IBase **)&m_pContextDataReader));

    SSM_CLEANUP_ASSERT(m_pContextRepository->registerResourceFinderEvent(this));

CLEANUP:
    return res;
}

void CContextExecutor::finalRelease()
{
}

void CContextExecutor::registerCallback(ICtxEvent *pEvent)
{
    ctxEvent = pEvent;
}

//Data from soft sensors
void CContextExecutor::addOutput(std::vector<ContextData> contextData)
{
    //LOGE("ADDOUTPUT");
    std::map<std::string, CallbackData>::iterator   itor;
    std::string type = contextData[0].rootName;

    //TODO: name must be a this soft sensors identifier
    //m_mtxRequestedContextCallback.Lock();
    itor = m_requestedContextCallback.find(type);
    if (itor != m_requestedContextCallback.end())
    {
        TypeofEvent callType = itor->second.m_callType;
        itor->second.m_pCallbackEvent->onEvent(type, callType, contextData);
        if (callType == SSM_ONCE)
        {
            unregisterContext(callType, m_registeredResources[type], this);
        }

    }
    //m_mtxRequestedContextCallback.Unlock();
}

void CContextExecutor::getDataFromDatabase(std::string modelName, int startIndex, int count,
        std::vector<ContextData> *data, int *pLastIndex)
{
    m_pContextDataReader->getContextData(modelName, startIndex, count, data, pLastIndex);
}

void CContextExecutor::registerContext(TypeofEvent callType, ISSMResource *pSSMResource,
                                       IEvent *pEvent)
{
    //will make check routine for One resource has callbacks.
    std::vector<ISSMResource *>  baseList;

    CallbackData callbackData(callType, pSSMResource->name, pEvent);

    //m_mtxRequestedContextCallback.Lock();
    m_registeredResources[pSSMResource->type] = pSSMResource;
    m_requestedContextCallback[pSSMResource->type] = callbackData;
    //m_mtxRequestedContextCallback.Unlock();

    //This is stand-alone sensor
    if (pSSMResource->inputList.size() == 0)
    {
        //Let sensors work
        std::vector<ContextData> inputData;
        switch (pSSMResource->location)
        {
            case SENSOR_LOCATION_LOCAL:
                runLogic(inputData, pSSMResource->type);
                break;

            case SENSOR_LOCATION_REMOTE:
                //Let observer work!
                m_pContextRepository->startObserveResource(pSSMResource, this);
                break;
        }
    }

    m_pContextRepository->getPrimitiveSensorList(&baseList);
    m_pContextRepository->getSoftSensorList(&baseList);

    for (unsigned int i = 0; i < pSSMResource->inputList.size(); ++i)
    {
        ISSMResource *pKeyResource = NULL;
        if (findString(&baseList, pSSMResource->inputList[i],
                       &pKeyResource) != SSM_E_FAIL) //if element of inputList is in the primitive resources.
        {
            if (m_relatedSoftSensor.find(pKeyResource->type) != m_relatedSoftSensor.end()) //already exists
            {
                //EXIST!!!!
                //check related Context needs insert or not. if softSensor resource is not a member of related context then insert.
                std::vector<std::string> softSensorList = m_relatedSoftSensor[pKeyResource->type];

                for (unsigned int j = 0; j < softSensorList.size(); ++j)
                {
                    if (softSensorList[j].compare(pSSMResource->type) != 0)
                    {
                        m_relatedSoftSensor[pKeyResource->type].push_back(pSSMResource->type);
                        break;
                    }
                }
            }
            else // If first time
            {
                //NO EXIST!!!!
                //insert resource in the all map and vector
                m_relatedSoftSensor[pKeyResource->type].push_back(pSSMResource->type);

                //Recursive call
                registerContext(callType, pKeyResource, this);
            }
        }
        else //unable to find installed sensors. take it to keep list
        {
            m_mapResourceLookup[pSSMResource->inputList[i]].push_back(CallbackData(callType, pSSMResource->type,
                    this));
        }
    }
}

void CContextExecutor::onExecute(void *pArg)
{
    intptr_t         *pMessage = (intptr_t *)pArg;

    RESOURCE_EVENT_TYPE     eventType = (RESOURCE_EVENT_TYPE)pMessage[0];
    ISSMResource            *pResource = (ISSMResource *)pMessage[1];

    switch (eventType)
    {
        case SSM_EVENT_NORMAL:
            break;

        case SSM_EVENT_ADDED:
            if (m_mapResourceLookup.find(pResource->type) != m_mapResourceLookup.end())
            {
                CallbackData        *callBack = NULL;

                for (size_t i = 0; i < m_mapResourceLookup[pResource->type].size(); i++)
                {
                    callBack = &m_mapResourceLookup[pResource->type][i];

                    m_relatedSoftSensor[pResource->type].push_back(callBack->m_name);

                    registerContext(callBack->m_callType, pResource, callBack->m_pCallbackEvent);
                }

                m_mapResourceLookup.erase(m_mapResourceLookup.find(pResource->type));
            }
            break;

        case SSM_REMOVED:
            break;

        case SSM_UPDATED:
            break;

        default:
            break;
    }
}

void CContextExecutor::onTerminate(void *pArg)
{
    intptr_t         *pMessage = (intptr_t *)pArg;

    delete[] pMessage;
}

int CContextExecutor::onResourceEvent(RESOURCE_EVENT_TYPE eventType, ISSMResource *pSSMResource,
                                      std::string info)
{
    intptr_t          *pMessage = new intptr_t [2];

    pMessage[0] = eventType;
    pMessage[1] = reinterpret_cast<intptr_t>(pSSMResource);
    return (int)m_pTasker->addTask(this, (void *)pMessage);
}

SSMRESULT CContextExecutor::findString(std::vector<ISSMResource *> *sList, const std::string str,
                                       ISSMResource **ppResource)
{
    SSMRESULT ret = SSM_E_FAIL;
    for (unsigned int i = 0 ; i < sList->size() ; ++i)
    {
        if ((*sList)[i]->type == str)
        {
            *ppResource = (*sList)[i];
            ret = SSM_S_OK;
            break;
        }
    }
    return ret;
}

std::map<std::string, std::vector<ContextData> >  CContextExecutor::getPreparedContextList(
    std::string primitiveSensor)
{
    //check m_relatedSoftSensor / apply timestamp

    SSMRESULT ret = SSM_E_FAIL;

    std::map<std::string, std::vector<ContextData> >    returnData;
    std::vector<ContextData>                            contextDataList;

    for (unsigned int i = 0; i < m_relatedSoftSensor[primitiveSensor].size(); ++i)
    {
        std::string softSensorName = m_relatedSoftSensor[primitiveSensor][i];
        if (m_registeredResources.find(softSensorName) != m_registeredResources.end())
        {
            ret = SSM_S_OK;
            std::vector<std::string> inputList = m_registeredResources[softSensorName]->inputList;
            for (unsigned int j = 0; j < inputList.size(); j++) //check all "inputlist" arrived or not
            {
                if (m_storedPrimitiveSensorData.find(inputList[j]) == m_storedPrimitiveSensorData.end())
                {
                    //Still we have partial data
                    ret = SSM_E_FAIL;
                    break;
                }
                else
                {
                    //Copy all properties of current primitiveSensor data to outputs
                    for (std::vector<ContextData>::iterator itor = m_storedPrimitiveSensorData[inputList[j]].begin();
                         itor != m_storedPrimitiveSensorData[inputList[j]].end(); ++itor)
                    {
                        contextDataList.push_back(*itor);
                    }
                }
            }

            //We have all inputs
            if (ret == SSM_S_OK)
            {
                returnData.insert(std::make_pair(softSensorName, contextDataList));
            }
            contextDataList.clear();
        }
    }

    return returnData;
}

//Data from every primitive sensors
int CContextExecutor::onEvent(std::string type, TypeofEvent callType,
                              std::vector<ContextData> ctxData)
{
    //std::string root_name = ctxData.at(0).root_name; //-> deviceId+ctxData.root_name

    if (m_relatedSoftSensor.find(type) != m_relatedSoftSensor.end()) //already registered?
    {
        //update recent values(overwrite)
        m_storedPrimitiveSensorData[type] = ctxData;

        //find soft sensor
        std::map<std::string, std::vector<ContextData> > readyContextList = getPreparedContextList(type);

        //Run SoftSensor!  readyContextList has all data for run
        std::map<std::string, std::vector<ContextData> >::iterator iter;
        for (iter = readyContextList.begin(); iter != readyContextList.end(); ++iter)
        {
            std::string softSensorName = iter->first;
            std::vector<ContextData> inputData = iter->second;

            runLogic(inputData, softSensorName);
        }
    }
    else if (m_registeredResources.find(type) != m_registeredResources.end()) //This data is primitive
    {
        addOutput(ctxData);
    }

    return SSM_S_OK;
}

void  CContextExecutor::unregisterContext(TypeofEvent callType, ISSMResource *pSSMResource,
        IEvent *pEvent)
{
    std::vector<ISSMResource *> baseList;

    //////////
    ///TODO: Need to clean up m_mapResourceLookup list
    //////////

    //This is primitive sensor
    if (pSSMResource->inputList.size() == 0)
    {
        //Let sensors work
        std::vector<ContextData> inputData;
        switch (pSSMResource->location)
        {
            case SENSOR_LOCATION_LOCAL:
                //TODO: Must free soft sensor
                if (m_libraryList.find(pSSMResource->type) != m_libraryList.end() && callType != SSM_ONCE)
                {
                    m_pContextRepository->unloadSoftSensor(m_libraryList[pSSMResource->type]);
                    m_libraryList.erase(m_libraryList.find(pSSMResource->type));
                }

                if (m_ctxEventList.find(pSSMResource->type) != m_ctxEventList.end())
                {
                    m_ctxEventList.erase(m_ctxEventList.find(pSSMResource->type));
                }
                break;

            case SENSOR_LOCATION_REMOTE:
                //Let observer stop work!
                m_pContextRepository->stopObserveResource(pSSMResource);
                break;
        }
    }

    //m_mtxRequestedContextCallback.Lock();
    if (m_requestedContextCallback.find(pSSMResource->type) != m_requestedContextCallback.end())
    {
        m_requestedContextCallback.erase(m_requestedContextCallback.find(pSSMResource->type));
    }
    if (m_registeredResources.find(pSSMResource->type) != m_registeredResources.end())
    {
        m_registeredResources.erase(m_registeredResources.find(pSSMResource->type));
    }
    //m_mtxRequestedContextCallback.Unlock();

    //TODO: support multiple concurrent query reference
    m_pContextRepository->getPrimitiveSensorList(&baseList);
    m_pContextRepository->getSoftSensorList(&baseList);

    for (unsigned int i = 0 ; i < pSSMResource->inputList.size() ; ++i)
    {
        ISSMResource *pPrimitiveSensor = NULL;
        if (findString(&baseList, pSSMResource->inputList[i], &pPrimitiveSensor) != SSM_E_FAIL)
        {
            std::vector<std::string> *softSensorList = &m_relatedSoftSensor[pPrimitiveSensor->type];
            for (unsigned int j = 0; j < softSensorList->size(); ++j)
            {
                if (!(*softSensorList)[j].compare(pSSMResource->type))
                {
                    softSensorList->erase(softSensorList->begin() + j);

                    if (softSensorList->size() == 0) //no more related context.
                    {
                        m_relatedSoftSensor.erase(m_relatedSoftSensor.find(pPrimitiveSensor->type));

                        //Recursive call
                        unregisterContext(callType, pPrimitiveSensor, this);
                        break;
                    }
                }
            }
        }
    }
}

//Called when soft sensor try to work
void CContextExecutor::runLogic(std::vector<ContextData> inputData, std::string softSensor)
{
    m_mtxLibraryIO.lock();
    if (m_ctxEventList.find(softSensor) == m_ctxEventList.end())
    {
        void *hSoftSensor = NULL;
        if (m_pContextRepository->loadSoftSensor(softSensor, this, &hSoftSensor) == SSM_S_OK)
        {
            m_libraryList[softSensor] = hSoftSensor;
            m_ctxEventList[softSensor] = ctxEvent;
            m_ctxEventList[softSensor]->onCtxEvent(SPF_START, inputData);
        }
    }
    else
    {
        m_ctxEventList[softSensor]->onCtxEvent(SPF_START, inputData);
    }
    m_mtxLibraryIO.unlock();
}
