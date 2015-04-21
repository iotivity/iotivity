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
#include "SSMInterface/SoftSensorManager.h"
#include "QueryProcessor/QueryEngine.h"
#include "QueryProcessor/ContextModel.h"
#include "QueryProcessor/ConditionedModel.h"
#include "QueryProcessor/EvaluationEngine.h"
#include "QueryProcessor/PropagationEngine.h"
#include "SensorProcessor/SensingEngine.h"
#include "SensorProcessor/ContextExecutor.h"
#include "SensorProcessor/ContextDataReader.h"
#include "SensorProcessor/ResourceFinder.h"
#include "rapidxml/rapidxml.hpp"

inline bool operator<( const OID &lhs, const OID &rhs )
{
    int ret = memcmp( &lhs, &rhs, sizeof(OID));
    return (ret < 0 ? true : false );
}

SSMRESULT CSoftSensorManager::finalConstruct()
{
    return SSM_S_OK;
}

void CSoftSensorManager::finalRelease()
{
}

SSMRESULT CSoftSensorManager::initializeCore(std::string xmlDescription)
{
    SSMRESULT                   res = SSM_E_FAIL;
    rapidxml::xml_document<>    xmlDoc;
    std::string                 strKey;
    std::string                 strValue;
    rapidxml::xml_node<>        *root = NULL;
    rapidxml::xml_node<>        *itemSSMCore = NULL;
    rapidxml::xml_node<>        *itemDevice = NULL;

    std::string                 name;
    std::string                 type;
    std::string                 pathSoftSensors;
    std::string                 pathDescription;

    std::string                 copyDescription = xmlDescription.c_str();

    try
    {
        xmlDoc.parse<0>((char *)copyDescription.c_str());

        root = xmlDoc.first_node();

        if (!root)
        {
            throw rapidxml::parse_error("No Root Element", 0);
        }

        strKey = root->name();

        if (strKey != "SSMCore")
        {
            throw rapidxml::parse_error("Invalid root tag name", 0);
        }

        for (itemSSMCore = root->first_node(); itemSSMCore; itemSSMCore = itemSSMCore->next_sibling())
        {
            strKey = itemSSMCore->name();

            if (strKey == "Device")
            {
                for (itemDevice = itemSSMCore->first_node(); itemDevice; itemDevice = itemDevice->next_sibling())
                {
                    strKey = itemDevice->name();

                    if (strKey == "Name")
                    {
                        name = itemDevice->value();
                    }
                    else if (strKey == "Type")
                    {
                        type = itemDevice->value();
                    }
                    else
                    {
                        ;/*NULL*/
                    }
                }
            }
            else if (strKey == "Config")
            {
                for (itemDevice = itemSSMCore->first_node(); itemDevice; itemDevice = itemDevice->next_sibling())
                {
                    strKey = itemDevice->name();

                    if (strKey == "SoftSensorRepository")
                    {
                        pathSoftSensors = itemDevice->value();
                    }
                    else if (strKey == "SoftSensorDescription")
                    {
                        pathDescription = itemDevice->value();
                    }
                    else
                    {
                        ;/*NULL*/
                    }
                }
            }
            else
            {
                ;/*NULL*/
            }
        }
    }
    catch (rapidxml::parse_error &e)
    {
        SSM_CLEANUP_ASSERT(SSM_E_INVALIDXML);
    }

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_ISensingEngine, (IBase **)&m_pSensingEngine));
    SSM_CLEANUP_ASSERT(m_pSensingEngine->queryInterface(OID_IContextRepository,
                       (IBase **)&m_pContextRepository));
    SSM_CLEANUP_ASSERT(m_pContextRepository->initRepository(name, type, pathSoftSensors,
                       pathDescription));

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IPropagationEngine, (IBase **)&m_pPropagationEngine));

CLEANUP:
    if (res != SSM_S_OK)
    {
        terminateCore(false);
    }

    return res;
}

SSMRESULT CSoftSensorManager::startCore()
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(m_pContextRepository->startResourceFinder());

CLEANUP:
    return res;
}

SSMRESULT CSoftSensorManager::stopCore()
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(m_pContextRepository->stopResourceFinder());

CLEANUP:
    return res;
}

SSMRESULT CSoftSensorManager::terminateCore(bool factoryResetFlag)
{
    return SSM_S_OK;
}

SSMRESULT CSoftSensorManager::createQueryEngine(IQueryEngine **ppQueryEngine)
{
    SSMRESULT res = SSM_E_FAIL;
    IQueryEngineInternal    *pQueryEngineInternal = NULL;
    SSM_CLEANUP_ASSERT(CreateInstance(OID_IQueryEngineInternal, (IBase **)&pQueryEngineInternal));
    *ppQueryEngine = (IQueryEngine *)pQueryEngineInternal;

CLEANUP:
    return res;
}

unsigned long CSoftSensorManager::releaseQueryEngine(IQueryEngine *pQueryEngine)
{
    IQueryEngineInternal    *pQueryEngineInternal = NULL;
    pQueryEngineInternal = (IQueryEngineInternal *)(CQueryEngine *)pQueryEngine;

    return pQueryEngineInternal->release();
}

SSMRESULT CSoftSensorManager::getInstalledModelList(std::vector<ISSMResource *> *pList)
{
    m_pSensingEngine->getList(pList);

    return SSM_S_OK;
}

CSimpleMutex                *g_mtxGlobalInstance = NULL;
std::map<OID, IBase *>       *g_globalInstance = NULL;
IThreadPool                 *g_pThreadPool = NULL;

SSMRESULT CreateGlobalInstance(const OID &objectID, IBase **ppvObject)
{
    SSMRESULT res = SSM_E_NOINTERFACE;

    if (ppvObject == NULL)
    {
        return SSM_E_POINTER;
    }

    *ppvObject = NULL;

    g_mtxGlobalInstance->lock();
    res = SSM_S_FALSE;

    if (IsEqualOID(OID_ITasker, objectID))
    {
        if (g_globalInstance->find(OID_ITasker) == g_globalInstance->end())
        {
            SSM_CLEANUP_ASSERT(CreateInstance(OID_ITasker, ppvObject));
        }
    }
    else if (IsEqualOID(OID_IThreadPool, objectID))
    {
        if (g_globalInstance->find(OID_IThreadPool) == g_globalInstance->end())
        {
            SSM_CLEANUP_ASSERT(CreateInstance(OID_IThreadPool, ppvObject));
        }
    }
    else if (IsEqualOID(OID_IEvaluationEngine, objectID))
    {
        if (g_globalInstance->find(OID_IEvaluationEngine) == g_globalInstance->end())
        {
            SSM_CLEANUP_ASSERT(CreateInstance(OID_IEvaluationEngine, ppvObject));
        }
    }
    else if (IsEqualOID(OID_IPropagationEngine, objectID))
    {
        if (g_globalInstance->find(OID_IPropagationEngine) == g_globalInstance->end())
        {
            SSM_CLEANUP_ASSERT(CreateInstance(OID_IPropagationEngine, ppvObject));
        }
    }
    else if (IsEqualOID(OID_IContextRepository, objectID))
    {
        if (g_globalInstance->find(OID_IContextRepository) == g_globalInstance->end())
        {
            SSM_CLEANUP_ASSERT(CreateInstance(OID_IContextRepository, ppvObject));
        }
    }
    else if (IsEqualOID(OID_IContextDataReader, objectID))
    {
        if (g_globalInstance->find(OID_IContextDataReader) == g_globalInstance->end())
        {
            SSM_CLEANUP_ASSERT(CreateInstance(OID_IContextDataReader, ppvObject));
        }
    }
    else if (IsEqualOID(OID_ISensingEngine, objectID))
    {
        if (g_globalInstance->find(OID_ISensingEngine) == g_globalInstance->end())
        {
            SSM_CLEANUP_ASSERT(CreateInstance(OID_ISensingEngine, ppvObject));
        }
    }
    else
    {
        res = SSM_E_NOINTERFACE;
    }

    switch (res)
    {
        case SSM_S_OK:
            (*g_globalInstance)[objectID] = *ppvObject;
            break;

        case SSM_S_FALSE:
            (*g_globalInstance)[objectID]->addRef();
            *ppvObject = (*g_globalInstance)[objectID];
            res = SSM_S_OK;
            break;

        default:
            goto CLEANUP;
    }

CLEANUP:
    g_mtxGlobalInstance->unlock();
    return res;
}

SSMRESULT CreateInstance(const OID &objectID, IBase **ppObject)
{
    SSMRESULT res = SSM_E_NOINTERFACE;

    if (ppObject == NULL)
    {
        return SSM_E_POINTER;
    }

    *ppObject = NULL;

    if (IsEqualOID(OID_ITasker, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CTasker>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IWorkerThread, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CWorkerThread>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IThreadPool, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CThreadPool>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IEvaluationEngine, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CEvaluationEngine>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IPropagationEngine, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CPropagationEngine>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IContextRepository, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CContextRepository>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_ISensingEngine, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CSensingEngine>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IContextExecutor, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CContextExecutor>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IContextModel, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CContextModel>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IConditionedModel, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CConditionedModel>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IConditionedQueryResult, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CConditionedQueryResult>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IConditionedQuery, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CConditionedQuery>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IResourceFinder, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CResourceFinder>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IQueryEngineInternal, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CQueryEngine>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_ISoftSensorManager, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CSoftSensorManager>(objectID, ppObject));
    }
    else if (IsEqualOID(OID_IContextDataReader, objectID))
    {
        SSM_CLEANUP_ASSERT(CreateNewObject<CContextDataReader>(objectID, ppObject));
    }

CLEANUP:
    return res;
}

SSMRESULT CreateGlobalInstanceRepo()
{
    SSMRESULT res = SSM_E_FAIL;

    if (g_mtxGlobalInstance != NULL)
        SSM_CLEANUP_ASSERT(SSM_E_OUTOFMEMORY);

    if (g_globalInstance != NULL)
        SSM_CLEANUP_ASSERT(SSM_E_OUTOFMEMORY);

    g_mtxGlobalInstance = new CSimpleMutex();
    SSM_CLEANUP_NULL_ASSERT(g_mtxGlobalInstance);
    g_globalInstance = new std::map<OID, IBase *>();
    SSM_CLEANUP_NULL_ASSERT(g_globalInstance);

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IThreadPool, (IBase **)&g_pThreadPool));

CLEANUP:
    return res;
}

SSMRESULT DestroyGlobalInstanceRepo()
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(g_pThreadPool->destroyThreadPool());

    SAFE_RELEASE(g_pThreadPool);
    SAFE_DELETE(g_mtxGlobalInstance);
    SAFE_DELETE(g_globalInstance);

CLEANUP:
    return res;
}
