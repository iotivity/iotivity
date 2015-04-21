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
#include "ThreadManager.h"

CSimpleMutex::CSimpleMutex()
{
#if defined(WIN32)
    InitializeCriticalSection(&m_criticalSection);
#elif defined(LINUX)
    pthread_mutexattr_init(&m_mutexAttribute);
    pthread_mutexattr_settype(&m_mutexAttribute, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_mutex, &m_mutexAttribute);
#else
#error WIN32 or LINUX tag must be defined
#endif
}

CSimpleMutex::~CSimpleMutex()
{
#if defined(WIN32)
    DeleteCriticalSection(&m_criticalSection);
#elif defined(LINUX)
    pthread_mutex_destroy(&m_mutex);
    pthread_mutexattr_destroy(&m_mutexAttribute);
#else
#error WIN32 or LINUX tag must be defined
#endif
}

void CSimpleMutex::lock()
{
#if defined(WIN32)
    EnterCriticalSection(&m_criticalSection);
#elif defined(LINUX)
    pthread_mutex_lock(&m_mutex);
#else
#error WIN32 or LINUX tag must be defined
#endif
}

void CSimpleMutex::unlock()
{
#if defined(WIN32)
    LeaveCriticalSection(&m_criticalSection);
#elif defined(LINUX)
    pthread_mutex_unlock(&m_mutex);
#else
#error WIN32 or LINUX tag must be defined
#endif
}


CSemaphore::CSemaphore()
{
}

CSemaphore::~CSemaphore()
{
}

SSMRESULT CSemaphore::initialize()
{
#if defined(WIN32)
    hSemaphore = CreateSemaphore(NULL, 1, 1, NULL);

    if (hSemaphore == NULL)
        return SSM_E_FAIL;
#elif defined(LINUX)
    if (sem_init(&hSemaphore, 0, 0) == -1)
        return SSM_E_FAIL;
#else
#error WIN32 or LINUX tag must be defined
#endif
    return SSM_S_OK;
}

SSMRESULT CSemaphore::destroy()
{
#if defined(WIN32)
    if (CloseHandle(hSemaphore) == FALSE)
        return SSM_E_FAIL;
#elif defined(LINUX)
    if (sem_destroy(&hSemaphore) == -1)
        return SSM_E_FAIL;
#else
#error WIN32 or LINUX tag must be defined
#endif
    return SSM_S_OK;
}

SSMRESULT CSemaphore::take()
{
#if defined(WIN32)
    if (WaitForSingleObject(hSemaphore, INFINITE) == WAIT_OBJECT_0)
        return SSM_S_OK;

    return SSM_E_FAIL;
#elif defined(LINUX)
    if (sem_wait(&hSemaphore) == -1)
        return SSM_E_FAIL;

    return SSM_S_OK;
#else
#error WIN32 or LINUX tag must be defined
#endif
}

SSMRESULT CSemaphore::give()
{
#if defined(WIN32)
    if (ReleaseSemaphore(hSemaphore, 1, NULL) != 0)
        return SSM_S_OK;

    return SSM_E_FAIL;
#elif defined(LINUX)
    if (sem_post(&hSemaphore) == -1)
        return SSM_E_FAIL;

    return SSM_S_OK;
#else
#error WIN32 or LINUX tag must be defined
#endif
}


bool CWorkerThread::getTask(ClientEntry *clientEntry)
{
    m_mtxClientEntry.lock();
    //Check empty
    if (m_ClientEntry.empty())
    {
        m_mtxClientEntry.unlock();
        //Sleep if there are no more tasks
        m_semTask.take();
    }
    else
    {
        m_mtxClientEntry.unlock();
    }

    //Check destroy
    m_mtxThreadTerm.lock();
    if (m_bThreadTerm == true)
    {
        m_mtxThreadTerm.unlock();
        return false;
    }
    m_mtxThreadTerm.unlock();

    m_mtxClientEntry.lock();
    if (!m_ClientEntry.empty())
    {
        *clientEntry = m_ClientEntry.front();
        m_ClientEntry.pop_front();
    }
    m_mtxClientEntry.unlock();

    return true;
}

void CWorkerThread::worker()
{
    ClientEntry clientEntry;

    m_semTask.initialize();

    //Thread Creation completed
    m_semInit.give();

    //Wait for any tasks
    while (getTask(&clientEntry))
    {
        if (clientEntry.pClient)
        {
            clientEntry.pClient->onExecute(clientEntry.pArg);
            clientEntry.pClient->onTerminate(clientEntry.pArg);
        }
        SAFE_RELEASE(clientEntry.pClient);
    }

    //Clean all remaining tasks
    m_mtxClientEntry.lock();
    //Remove all tasks from queue
    for (std::list<ClientEntry>::iterator itor = m_ClientEntry.begin();
         itor != m_ClientEntry.end(); ++itor)
    {
        ClientEntry clientEntry = *itor;
        clientEntry.pClient->onTerminate(clientEntry.pArg);
        SAFE_RELEASE(clientEntry.pClient);
    }
    m_ClientEntry.clear();
    m_mtxClientEntry.unlock();

    m_semTask.destroy();

    m_semTerm.give();
}

SSMRESULT CWorkerThread::initialize()
{
    SSMRESULT res = SSM_E_FAIL;

    //Create thread and wait for jobs
#if defined(WIN32)
    m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_worker,
                             this, 0, NULL);

    if (m_hThread == NULL)
    {
        return SSM_E_FAIL;
    }
#elif defined(LINUX)
    if (pthread_create(&m_hThread, NULL, (void *(*)(void *))_worker, (void *)this) != 0)
    {
        return SSM_E_FAIL;
    }
#else
#error WIN32 or LINUX tag must be defined
#endif
    //Wait till thread created
    SSM_CLEANUP_ASSERT(m_semInit.take());
    SSM_CLEANUP_ASSERT(m_semInit.destroy());

CLEANUP:
    return res;
}

SSMRESULT CWorkerThread::terminate()
{
    SSMRESULT res = SSM_E_FAIL;

    //Let worker destroyed
    m_mtxThreadTerm.lock();
    m_bThreadTerm = true;
    m_mtxThreadTerm.unlock();

    SSM_CLEANUP_ASSERT(m_semTask.give());

    SSM_CLEANUP_ASSERT(m_semTerm.take());
    SSM_CLEANUP_ASSERT(m_semTerm.destroy());

#if defined(WIN32)
    if (m_hThread != NULL)
    {
        CloseHandle(m_hThread);
    }
#elif defined(LINUX)
    pthread_detach(m_hThread);
#else
#error WIN32 or LINUX tag must be defined
#endif
    res = SSM_S_OK;

CLEANUP:
    return res;
}

SSMRESULT CWorkerThread::finalConstruct()
{
    SSMRESULT   res = SSM_E_FAIL;

    m_bThreadTerm = false;

    SSM_CLEANUP_ASSERT(m_semInit.initialize());
    SSM_CLEANUP_ASSERT(m_semTerm.initialize());

CLEANUP:
    return res;
}

void CWorkerThread::finalRelease()
{
}

SSMRESULT CWorkerThread::addTask(IThreadClient *pThreadClient, void *param)
{
    ClientEntry clientEntry;

    pThreadClient->addRef();
    clientEntry.pClient = pThreadClient;
    clientEntry.pArg = param;

    m_mtxClientEntry.lock();
    if (m_ClientEntry.empty())
    {
        m_semTask.give();
    }
    m_ClientEntry.push_back(clientEntry);
    //Let the task worker know, we just added task
    m_mtxClientEntry.unlock();

    return SSM_S_OK;
}


SSMRESULT CTasker::finalConstruct()
{
    SSMRESULT   res = SSM_E_FAIL;

    SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IThreadPool, (IBase **)&m_pThreadPool));

    SSM_CLEANUP_ASSERT(m_pThreadPool->createWorkerThread(&m_pWorkerThread));

CLEANUP:
    return res;
}

void CTasker::finalRelease()
{
}

SSMRESULT CTasker::addTask(IThreadClient *pThreadClient, void *param)
{
    return m_pWorkerThread->addTask(pThreadClient, param);
}


SSMRESULT CThreadPool::finalConstruct()
{
    return SSM_S_OK;
}

void CThreadPool::finalRelease()
{
    for (std::vector<IWorkerThread *>::iterator itor = m_lstWorkerThread.begin();
         itor != m_lstWorkerThread.end(); ++itor)
    {
        (*itor)->release();
    }
}

SSMRESULT CThreadPool::createWorkerThread(IWorkerThread **ppWorkerThread)
{
    SSMRESULT res = SSM_E_FAIL;

    IWorkerThread       *pWorkerThread = NULL;

    SSM_CLEANUP_NULL_ASSERT(ppWorkerThread);

    SSM_CLEANUP_ASSERT(CreateInstance(OID_IWorkerThread, (IBase **)&pWorkerThread));
    SSM_CLEANUP_ASSERT(pWorkerThread->initialize());
    SSM_CLEANUP_ASSERT(pWorkerThread->queryInterface(OID_IWorkerThread, (IBase **)ppWorkerThread));
    m_lstWorkerThread.push_back(pWorkerThread);

CLEANUP:
    return res;
}

SSMRESULT CThreadPool::destroyThreadPool()
{
    SSMRESULT   res = SSM_E_FAIL;

    for (std::vector<IWorkerThread *>::iterator itor = m_lstWorkerThread.begin();
         itor != m_lstWorkerThread.end(); ++itor)
    {
        SSM_CLEANUP_ASSERT((*itor)->terminate());
    }

    res = SSM_S_OK;

CLEANUP:
    return res;
}