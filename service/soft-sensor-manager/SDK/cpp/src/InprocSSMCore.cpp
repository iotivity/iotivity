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
#include "SSMInterface.h"
#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"

class SSMCoreEventReceiver : public IQueryEngineEvent
{
    public:
        SSMCoreEventReceiver()
        {
        }

        SSMRESULT onQueryEngineEvent(int cqid, IDataReader *pResult)
        {
            SSMRESULT res = SSM_E_FAIL;

            m_mtxListener.lock();

            if (m_mapListener.find(cqid) == m_mapListener.end())
            {
                SSM_CLEANUP_ASSERT(res);
            }

            m_mapListener[cqid]->onQueryEngineEvent(cqid, pResult);

            res = SSM_S_OK;

CLEANUP:
            m_mtxListener.unlock();
            return res;
        }

        void lockListener()
        {
            m_mtxListener.lock();
        }

        void unlockListener()
        {
            m_mtxListener.unlock();
        }

        void addListener(int cqid, IQueryEngineEvent *pEngineEvent)
        {
            m_mapListener[cqid] = pEngineEvent;
        }

        void removeListener(int cqid)
        {
            m_mapListener.erase(cqid);
        }

    private:
        CSimpleMutex                        m_mtxListener;
        std::map<int, IQueryEngineEvent *>   m_mapListener;
};

IQueryEngine                        *g_pQueryEngineInstance = NULL;
SSMCoreEventReceiver                *g_pEventReceiver = NULL;

SSMRESULT OIC::InitializeSSM(std::string xmlDescription)
{
    SSMRESULT res = SSM_E_FAIL;

    if (g_pQueryEngineInstance != NULL)
        SSM_CLEANUP_ASSERT(SSM_E_INITIALIZED);

    g_pEventReceiver = new SSMCoreEventReceiver();
    SSM_CLEANUP_NULL_ASSERT(g_pEventReceiver);
    SSM_CLEANUP_ASSERT(InitializeSSMCore(xmlDescription));
    SSM_CLEANUP_ASSERT(StartSSMCore());
    SSM_CLEANUP_ASSERT(CreateQueryEngine(&g_pQueryEngineInstance));
    SSM_CLEANUP_ASSERT(g_pQueryEngineInstance->registerQueryEvent(g_pEventReceiver));

CLEANUP:
    if (res != SSM_S_OK &&
        res != SSM_E_INITIALIZED)
    {
        SAFE_DELETE(g_pEventReceiver);
    }

    return res;
}

SSMRESULT OIC::TerminateSSM()
{
    SSMRESULT res = SSM_E_FAIL;

    if (g_pQueryEngineInstance == NULL)
        SSM_CLEANUP_ASSERT(SSM_E_NOTINIT);

    SSM_CLEANUP_ASSERT(g_pQueryEngineInstance->unregisterQueryEvent(g_pEventReceiver));
    ReleaseQueryEngine(g_pQueryEngineInstance);
    g_pQueryEngineInstance = NULL;
    SSM_CLEANUP_ASSERT(StopSSMCore());
    SSM_CLEANUP_ASSERT(TerminateSSMCore());

CLEANUP:
    SAFE_DELETE(g_pEventReceiver);
    return res;
}

SSMRESULT OIC::RegisterQuery(std::string queryString, IQueryEngineEvent *listener,
                             int &cqid)
{
    SSMRESULT res = SSM_E_FAIL;

    if (g_pQueryEngineInstance == NULL)
        SSM_CLEANUP_ASSERT(SSM_E_NOTINIT);

    g_pEventReceiver->lockListener();
    SSM_CLEANUP_ASSERT(g_pQueryEngineInstance->executeContextQuery(queryString, &cqid));
    g_pEventReceiver->addListener(cqid, listener);

CLEANUP:
    if (g_pEventReceiver != NULL)
        g_pEventReceiver->unlockListener();
    return res;
}

SSMRESULT OIC::UnregisterQuery(int cqid)
{
    SSMRESULT res = SSM_E_FAIL;

    if (g_pQueryEngineInstance == NULL)
        SSM_CLEANUP_ASSERT(SSM_E_NOTINIT);

    g_pEventReceiver->lockListener();
    SSM_CLEANUP_ASSERT(g_pQueryEngineInstance->killContextQuery(cqid));
    g_pEventReceiver->removeListener(cqid);

CLEANUP:
    if (g_pEventReceiver != NULL)
        g_pEventReceiver->unlockListener();
    return res;
}