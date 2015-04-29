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
#include "SSMInterface/SSMCore.h"
#include "SSMInterface/SoftSensorManager.h"
#include "Common/InternalInterface.h"

static ISoftSensorManager       *g_pSoftSensorManager = NULL;

SSMRESULT CreateQueryEngine(IQueryEngine **ppQueryEngine)
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_NULL_ASSERT(g_pSoftSensorManager);
    SSM_CLEANUP_ASSERT(g_pSoftSensorManager->createQueryEngine(ppQueryEngine));

CLEANUP:
    return res;
}

unsigned long ReleaseQueryEngine(IQueryEngine *pQueryEngine)
{
    if (pQueryEngine == NULL)
    {
        return -1;
    }

    if (g_pSoftSensorManager == NULL)
    {
        return -1;
    }

    return g_pSoftSensorManager->releaseQueryEngine(pQueryEngine);
}

SSMRESULT InitializeSSMCore(std::string xmlDescription)
{
    SSMRESULT res = SSM_E_FAIL;

    if (g_pSoftSensorManager != NULL)
        SSM_CLEANUP_ASSERT(SSM_E_INITIALIZED);

    SSM_CLEANUP_ASSERT(CreateGlobalInstanceRepo());
    SSM_CLEANUP_ASSERT(CreateInstance(OID_ISoftSensorManager, (IBase **)&g_pSoftSensorManager));
    SSM_CLEANUP_ASSERT(g_pSoftSensorManager->initializeCore(xmlDescription));

CLEANUP:
    if (res != SSM_S_OK &&
        res != SSM_E_INITIALIZED)
    {
        SAFE_RELEASE(g_pSoftSensorManager);
        DestroyGlobalInstanceRepo();
    }

    return res;
}

SSMRESULT StartSSMCore()
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_NULL_ASSERT(g_pSoftSensorManager);
    SSM_CLEANUP_ASSERT(g_pSoftSensorManager->startCore());

CLEANUP:
    return res;
}

SSMRESULT StopSSMCore()
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_NULL_ASSERT(g_pSoftSensorManager);
    SSM_CLEANUP_ASSERT(g_pSoftSensorManager->stopCore());

CLEANUP:
    return res;
}

SSMRESULT TerminateSSMCore(bool factoryResetFlag)
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_NULL_ASSERT(g_pSoftSensorManager);
    SSM_CLEANUP_ASSERT(g_pSoftSensorManager->terminateCore(factoryResetFlag));

CLEANUP:
    SAFE_RELEASE(g_pSoftSensorManager);
    DestroyGlobalInstanceRepo();
    return res;
}

const char *GetSSMError(SSMRESULT res)
{
    const char *msg = NULL;

    switch (res)
    {
        case SSM_S_OK:
            msg = "Success";
            break;

        case SSM_E_POINTER:
            msg = "SSM_E_POINTER";
            break;

        case SSM_E_OUTOFMEMORY:
            msg = "SSM_E_OUTOFMEMORY";
            break;

        case SSM_E_FAIL:
            msg = "SSM_E_FAIL";
            break;

        case SSM_E_NOTINIT:
            msg = "SSM_E_NOTINIT";
            break;

        case SSM_E_INITIALIZED:
            msg = "SSM_E_INITIALIZED";
            break;

        case SSM_E_INVALIDXML:
            msg = "SSM_E_INVALIDXML";
            break;

        case SSM_E_NOINTERFACE:
            msg = "SSM_E_NOINTERFACE";
            break;

        case SSM_E_NOTIMPL:
            msg = "SSM_E_NOTIMPL";
            break;

        default:
            msg = "Not defined";
            break;
    }

    return msg;
}

SSMRESULT GetInstalledModelList(std::vector<ISSMResource *> *pList)
{
    SSMRESULT res = SSM_E_FAIL;

    SSM_CLEANUP_NULL_ASSERT(g_pSoftSensorManager);
    g_pSoftSensorManager->getInstalledModelList(pList);

CLEANUP:
    return res;
}
