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
#ifndef _ThreadManager_H_
#define _ThreadManager_H_

#include "PlatformLayer.h"

static const OID OID_IThreadClient = { 0x6f379191, 0xd9f1, 0x480c, { 0xac, 0x23, 0xb2, 0x88, 0x9e, 0x69, 0x28, 0x7b } };
/**
* @class    IThreadClient
* @brief    IThreadClient Interface
*            This class represents thread client that called from worker thread.\n
*            The application who wants to use worker thread, inherit this interface
*
* @see
*/
class IThreadClient : public IBase
{
    public:
        /**
        * @fn     onExecute
        * @brief Called when worker thread is ready execute message
        *
        * @param [in] void *pArg - Optional argument
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual void onExecute(void *pArg) = 0;

        /**
        * @fn     onTerminate
        * @brief Called when worker thread has finished last executed message
        *
        * @param [in] void *pArg - Optional argument
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual void onTerminate(void *pArg) = 0;
        virtual ~IThreadClient() {};
};

/**
* @class    CObjectMultiThreadModel
* @brief    CObjectMultiThreadModel Interface
*            This class represents declared CObject's threading model
*
* @see
*/
class CObjectMultiThreadModel
{
    public:
        /**
        * @fn     Increment
        * @brief Called when reference counter need to increase
        *
        * @param [in] volatile unsigned long *p - Reference counter's pointer
        *
        * @return unsigned long
        * @warning
        * @exception
        * @see
        */
        static unsigned long increment(volatile unsigned long *p)
        {
#if defined(WIN32)
            return InterlockedIncrement(p);
#elif defined(LINUX)
            return __sync_add_and_fetch(p, 1);
#else
#error WIN32 or LINUX tag must be defined
#endif
        }

        /**
        * @fn     decrement
        * @brief Called when reference counter need to decrease
        *
        * @param [in] volatile unsigned long *p - Reference counter's pointer
        *
        * @return unsigned long
        * @warning
        * @exception
        * @see
        */
        static unsigned long decrement(volatile unsigned long *p)
        {
#if defined(WIN32)
            return InterlockedDecrement(p);
#elif defined(LINUX)
            return __sync_add_and_fetch(p, -1);
#else
#error WIN32 or LINUX tag must be defined
#endif
        }
};

/**
* @class    CNullMutex
* @brief    CNullMutex Interface
*            This class represents mutex strategy
*
* @see
*/
class CNullMutex
{
    public:
        /**
        * @fn     lock
        * @brief Called when current thread need to locked
        *
        * @param NONE
        *
        * @return NONE
        * @warning
        * @exception
        * @see
        */
        void lock() {}

        /**
        * @fn     unlock
        * @brief Called when current thread need to unlocked
        *
        * @param NONE
        *
        * @return NONE
        * @warning
        * @exception
        * @see
        */
        void unlock() {}
};

/**
* @class    CAutoMutex
* @brief    CAutoMutex Interface
*            This class represents automated mutex class
*
* @see
*/
template<typename T>
class CAutoMutex
{
    public:
        CAutoMutex(T &t) :
            t_(t)
        {
            t_.lock();
        }
        ~CAutoMutex()
        {
            t_.unlock();
        }

    private:
        T &t_;
};

/**
* @class    CSimpleMutex
* @brief    CSimpleMutex Interface
*            This class represents basic mutex class.\n
*            This mutex must support recursive locking mechanism
*
* @see
*/
class CSimpleMutex
{
    private:
#if defined(WIN32)
        CRITICAL_SECTION m_criticalSection;
#elif defined(LINUX)
        pthread_mutex_t         m_mutex;
        pthread_mutexattr_t     m_mutexAttribute;
#else
#error WIN32 or LINUX tag must be defined
#endif

    public:
        CSimpleMutex();

        ~CSimpleMutex();

        /**
        * @fn     lock
        * @brief Lock current thread
        *
        * @param NONE
        *
        * @return NONE
        * @warning
        * @exception
        * @see
        */
        void lock();

        /**
        * @fn     unlock
        * @brief Unlock current thread
        *
        * @param NONE
        *
        * @return NONE
        * @warning
        * @exception
        * @see
        */
        void unlock();
};

class CSemaphore
{
    private:
#if defined(WIN32)
        HANDLE hSemaphore;
#elif defined(LINUX)
        sem_t hSemaphore;
#else
#error WIN32 or LINUX tag must be defined
#endif

    public:
        CSemaphore();

        ~CSemaphore();

        SSMRESULT initialize();

        SSMRESULT destroy();

        SSMRESULT take();

        SSMRESULT give();
};


static const OID OID_IWorkerThread = { 0x12a67457, 0xce97, 0x41c4, { 0x90, 0xb8, 0xcc, 0x72, 0x5f, 0xc4, 0x40, 0x27 } };
/**
* @class    IWorkerThread
* @brief    IWorkerThread Interface
*            This class represents worker thread utility
*
* @see
*/
class IWorkerThread : public IBase
{
    public:
        /**
        * @fn     initialize
        * @brief Initialize current worker thread
        *
        * @param NONE
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT initialize() = 0;

        /**
        * @fn     terminate
        * @brief Terminate current worker thread.\n
        *         All remained tasks are destroyed after IThreadClient::OnTerminate called
        *
        * @param NONE
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT terminate() = 0;

        /**
        * @fn     addTask
        * @brief Add task to current worker thread.\n
        *         Each task called only once
        *
        * @param [in] IThreadClient *pThreadClient - Implemented thread client class instance that called from worker thread.
        * @param [in] void *param - Optional parameter that IThreadClient::OnExecute/OnTerminate can receive
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        virtual SSMRESULT addTask(IThreadClient *pThreadClient, void *param) = 0;
};

/**
* @class    CWorkerThread
* @brief    CWorkerThread Interface
*            This class represents worker thread utility that holds one thread
*
* @see
*/
class CWorkerThread :
    public CObjectRoot<CObjectMultiThreadModel>
    , public IWorkerThread
{
    private:
        struct ClientEntry
        {
            IThreadClient   *pClient;
            void            *pArg;
        };
#if defined(WIN32)
        HANDLE m_hThread;
#elif defined(LINUX)
        pthread_t m_hThread;
#else
#error WIN32 or LINUX tag must be defined
#endif
        CSemaphore              m_semInit;
        CSemaphore              m_semTerm;

        CSimpleMutex            m_mtxClientEntry;
        CSimpleMutex            m_mtxThreadTerm;
        CSemaphore              m_semTask;

        bool                    m_bThreadTerm;

        std::list<ClientEntry>  m_ClientEntry;

        bool getTask(ClientEntry *clientEntry);

        void worker();

        static void _worker(void *pArg)
        {
            CWorkerThread *pThread = (CWorkerThread *)pArg;
            return pThread->worker();
        }

    public:
        SSMRESULT finalConstruct();
        void finalRelease();

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IWorkerThread))
            {
                IBase *pBase = (IWorkerThread *)this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        /**
        * @fn     initialize
        * @brief Initialize current worker thread
        *
        * @param NONE
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT initialize();

        /**
        * @fn     terminate
        * @brief Terminate current worker thread.\n
        *         All remained tasks are destroyed after IThreadClient::OnTerminate called
        *
        * @param NONE
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT terminate();

        /**
        * @fn     addTask
        * @brief Add task to current worker thread.\n
        *         Each task called only once
        *
        * @param [in] IThreadClient *pThreadClient - Implemented thread client class instance that called from worker thread.
        * @param [in] void *param - Optional parameter that IThreadClient::OnExecute/OnTerminate can receive
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT addTask(IThreadClient *pThreadClient, void *param);
};


static const OID OID_IThreadPool = { 0x53855605, 0xb2a0, 0x4a31, { 0xa2, 0x60, 0x36, 0x4d, 0x43, 0xc1, 0x2f, 0x96 } };
/**
* @class    IThreadPool
* @brief    IThreadPool Interface
*            This class represents worker thread utility
*
* @see
*/
class IThreadPool : public IBase
{
    public:
        virtual SSMRESULT createWorkerThread(OUT IWorkerThread **ppWorkerThread) = 0;
        virtual SSMRESULT destroyThreadPool() = 0;
};

/**
* @class    CThreadPool
* @brief    CThreadPool Interface
*            This class represents worker thread pool utility that can holds multiple thread
*
* @see
*/
class CThreadPool :
    public CObjectRoot<CObjectMultiThreadModel>
    , public IThreadPool
{
    private:
        std::vector<IWorkerThread *> m_lstWorkerThread;

    public:
        SSMRESULT finalConstruct();
        void finalRelease();

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IThreadPool))
            {
                IBase *pBase = this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        SSMRESULT createWorkerThread(OUT IWorkerThread **ppWorkerThread);

        SSMRESULT destroyThreadPool();
};

static const OID OID_ITasker = { 0x8f2b0b4f, 0xaab8, 0x4cbc, { 0x99, 0x1, 0x65, 0xde, 0x89, 0x82, 0x9f, 0x7a } };
/**
* @class    ITasker
* @brief    ITasker Interface
*            This class represents worker thread utility
*
* @see
*/
class ITasker : public IBase
{
    public:
        virtual SSMRESULT addTask(IThreadClient *pThreadClient, void *param) = 0;
};

class CTasker :
    public CObjectRoot<CObjectMultiThreadModel>
    , public ITasker
{
    private:
        CObjectPtr<IThreadPool>     m_pThreadPool;
        CObjectPtr<IWorkerThread>   m_pWorkerThread;

    public:
        SSMRESULT finalConstruct();
        void finalRelease();

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_ITasker))
            {
                IBase *pBase = this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        SSMRESULT addTask(IThreadClient *pThreadClient, void *param);
};
#endif
