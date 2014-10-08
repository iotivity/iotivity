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
*			 This class represents thread client that called from worker thread.\n
*			 The application who wants to use worker thread, inherit this interface
*
* @see
*/
class IThreadClient : public IBase
{
public:
	/**
	* @fn	  onExecute
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
	* @fn	  onTerminate
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
	virtual ~IThreadClient(){};
};

/**
* @class    CObjectMultiThreadModel
* @brief    CObjectMultiThreadModel Interface
*			 This class represents declared CObject's threading model
*
* @see
*/
class CObjectMultiThreadModel
{
public:
	/**
	* @fn	  Increment
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
	* @fn	  decrement
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
*			 This class represents mutex strategy
*
* @see
*/
class CNullMutex
{
public:
	/**
	* @fn	  lock
	* @brief Called when current thread need to locked
	*
	* @param NONE
	* 
	* @return NONE
	* @warning      
	* @exception    
	* @see
	*/
	void lock(){}

	/**
	* @fn	  unlock
	* @brief Called when current thread need to unlocked
	*
	* @param NONE
	* 
	* @return NONE
	* @warning      
	* @exception    
	* @see
	*/
	void unlock(){}
};

/**
* @class    CAutoMutex
* @brief    CAutoMutex Interface
*			 This class represents automated mutex class
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
	T& t_;
};

#if defined(WIN32)
/**
* @class    CSimpleMutex
* @brief    CSimpleMutex Interface
*			 This class represents basic mutex class.\n
*			 This mutex must support recursive locking mechanism
*
* @see
*/
class CSimpleMutex
{
private:
	CRITICAL_SECTION m_criticalSection;

public:
	CSimpleMutex()
	{
		InitializeCriticalSection(&m_criticalSection);
	}

	~CSimpleMutex()
	{
		DeleteCriticalSection(&m_criticalSection);
	}

	/**
	* @fn	  lock
	* @brief Lock current thread
	*
	* @param NONE
	* 
	* @return NONE
	* @warning      
	* @exception    
	* @see
	*/
	void lock()
	{
		EnterCriticalSection(&m_criticalSection);		
	}

	/**
	* @fn	  unlock
	* @brief Unlock current thread
	*
	* @param NONE
	* 
	* @return NONE
	* @warning      
	* @exception    
	* @see
	*/
	void unlock()
	{
		LeaveCriticalSection(&m_criticalSection);
	}
};

/**
* @class    CTaskWorker
* @brief    CTaskWorker Interface
*			 This class represents worker thread utility that holds one thread
*
* @see
*/
class CTaskWorker
{
private:
	HANDLE m_hThread;

	CSimpleMutex  m_mutex;

	HANDLE m_hInitializeEvent;
	HANDLE m_hAddTaskEvent;
	HANDLE m_hShutdownEvent;
	HANDLE m_hShutdownCompleteEvent;

	struct ClientEntry
	{
		IThreadClient   *pClient;
		void			*pArg;
	};

	std::list<ClientEntry>	m_clientEntry;

	static void _worker(void *pArg)
	{
		CTaskWorker *pThread = (CTaskWorker *)pArg;
		return pThread->worker();
	}

	bool getTask(ClientEntry *clientEntry)
	{		
		HANDLE handles[] = {m_hShutdownEvent, m_hAddTaskEvent};

		bool bRet = false;

		switch(WaitForMultipleObjects(2, handles, FALSE, INFINITE))
		{
			//requested for shutdown
		case WAIT_OBJECT_0:
			bRet = false;
			break;

			//requested for new task added
		case WAIT_OBJECT_0 + 1:
			m_mutex.lock();
			*clientEntry = m_clientEntry.front();
			m_clientEntry.pop_front();
			if(m_clientEntry.empty())
			{
				ResetEvent(m_hAddTaskEvent);
			}
			m_mutex.unlock();
			bRet = true;
			break;

			//invalid event raised
		default:
			bRet = false;
			break;
		}

		return bRet;
	}

	void worker()
	{
		ClientEntry clientEntry;

		//Thread Creation completed
		SetEvent(m_hInitializeEvent);

		//Wait for any tasks
		while(getTask(&clientEntry))
		{
			clientEntry.pClient->onExecute(clientEntry.pArg);
			clientEntry.pClient->onTerminate(clientEntry.pArg);
			SAFE_RELEASE(clientEntry.pClient);
		}

		//Thread is almost terminated
		SetEvent(m_hShutdownCompleteEvent);
	}

public:
	CTaskWorker()
	{
		m_hThread = NULL;
		m_hInitializeEvent = NULL;
		m_hAddTaskEvent = NULL;
		m_hShutdownEvent = NULL;
		m_hShutdownCompleteEvent = NULL;
	}

	~CTaskWorker()
	{
	}

	/**
	* @fn	  initialize
	* @brief Initialize current worker thread
	*
	* @param NONE
	* 
	* @return SSMRESULT
	* @warning      
	* @exception    
	* @see
	*/
	SSMRESULT initialize()
	{
		m_hInitializeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		m_hAddTaskEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		m_hShutdownEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		m_hShutdownCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		//Create thread and wait for jobs
		m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_worker,
			this, 0, NULL);

		if(m_hThread == NULL)
		{
			return SSM_E_FAIL;
		}

		//Wait till thread creation is done
		if(WaitForSingleObject(m_hInitializeEvent, INFINITE) != WAIT_OBJECT_0)
		{
			return SSM_E_FAIL;
		}

		return SSM_S_OK;
	}

	/**
	* @fn	  addTask
	* @brief Add task to current worker thread.\n
	*		  Each task called only once
	*
	* @param [in] IThreadClient *pThreadClient - Implemented thread client class instance that called from worker thread.
	* @param [in] void *param - Optional parameter that IThreadClient::OnExecute/OnTerminate can receive
	* 
	* @return SSMRESULT
	* @warning      
	* @exception    
	* @see
	*/
	SSMRESULT addTask(IThreadClient *pThreadClient, void *param)
	{
		ClientEntry clientEntry;

		pThreadClient->addRef();
		clientEntry.pClient = pThreadClient;
		clientEntry.pArg = param;

		m_mutex.lock();
		m_clientEntry.push_back(clientEntry);
		//Let the task worker know, we just added task
		SetEvent(m_hAddTaskEvent);
		m_mutex.unlock();

		return SSM_S_OK;
	}

	/**
	* @fn	  terminate
	* @brief Terminate current worker thread.\n
	*		  All remained tasks are destroyed after IThreadClient::OnTerminate called
	*
	* @param NONE
	* 
	* @return SSMRESULT
	* @warning      
	* @exception    
	* @see
	*/
	SSMRESULT terminate()
	{
		SSMRESULT res = SSM_S_OK;
		//Wait Created Thread's termination
		SetEvent(m_hShutdownEvent);

		//Wait till thread creation is done
		if(WaitForSingleObject(m_hShutdownCompleteEvent, 10000) != WAIT_OBJECT_0)
			res = SSM_E_FAIL;

		m_mutex.lock();
		//Remove all tasks from queue
		for(std::list<ClientEntry>::iterator itor = m_clientEntry.begin();
			itor != m_clientEntry.end(); ++itor)
		{
			ClientEntry clientEntry = *itor;
			clientEntry.pClient->onTerminate(clientEntry.pArg);
			SAFE_RELEASE(clientEntry.pClient);
		}
		m_clientEntry.clear();
		m_mutex.unlock();

		if(m_hThread != NULL)
		{
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}

		if(m_hInitializeEvent != NULL)
		{
			CloseHandle(m_hInitializeEvent);
			m_hInitializeEvent = NULL;
		}

		if(m_hAddTaskEvent != NULL)
		{
			CloseHandle(m_hAddTaskEvent);
			m_hAddTaskEvent = NULL;
		}

		if(m_hShutdownEvent != NULL)
		{
			CloseHandle(m_hShutdownEvent);
			m_hShutdownEvent = NULL;
		}

		if(m_hShutdownCompleteEvent != NULL)
		{
			CloseHandle(m_hShutdownCompleteEvent);
			m_hShutdownCompleteEvent = NULL;
		}

		return res;
	}
};

#else

#define WFMO
namespace neosmart
{
#ifdef WFMO
	struct neosmart_wfmo_t_
	{
		pthread_mutex_t Mutex;
		pthread_cond_t CVariable;
		union
		{
			int FiredEvent; //WFSO
			int EventsLeft; //WFMO
		} Status;
		bool StillWaiting;
		int RefCount;
		bool WaitAll;

		void Destroy()
		{
			pthread_mutex_destroy(&Mutex);
			pthread_cond_destroy(&CVariable);
		}
	};
	typedef neosmart_wfmo_t_ *neosmart_wfmo_t;

	struct neosmart_wfmo_info_t_
	{
		neosmart_wfmo_t Waiter;
		int WaitIndex;
	};
	typedef neosmart_wfmo_info_t_ *neosmart_wfmo_info_t;
#endif

	struct neosmart_event_t_
	{
		bool AutoReset;
		pthread_cond_t CVariable;
		pthread_mutex_t Mutex;
		bool State;
#ifdef WFMO
		std::deque<neosmart_wfmo_info_t_> RegisteredWaits;
#endif
	};
	typedef neosmart_event_t_ * neosmart_event_t;

#ifdef WFMO
	static bool RemoveExpiredWaitHelper(neosmart_wfmo_info_t_ wait)
	{
		int result = pthread_mutex_trylock(&wait.Waiter->Mutex);

		if(result == EBUSY)
		{
			return false;
		}

		assert(result == 0);

		if(wait.Waiter->StillWaiting == false)
		{
			--wait.Waiter->RefCount;
			assert(wait.Waiter->RefCount >= 0);

			result = pthread_mutex_unlock(&wait.Waiter->Mutex);
			assert(result == 0);

			if(wait.Waiter->RefCount == 0)
			{
				wait.Waiter->Destroy();
				delete wait.Waiter;
			}
			else
			{
				;/*NULL*/
			}

			return true;
		}

		result = pthread_mutex_unlock(&wait.Waiter->Mutex);
		assert(result == 0);

		return false;
	}
#endif

	static int UnlockedWaitForEvent(neosmart_event_t event, uint64_t milliseconds)
	{
		int result = 0;
		if(!event->State)
		{
			//Zero-timeout event state check optimization
			if(milliseconds == 0)
			{
				return ETIMEDOUT;
			}

			timespec ts;
			if(milliseconds != (uint64_t) -1)
			{
				timeval tv;
				gettimeofday(&tv, NULL);

				uint64_t nanoseconds = ((uint64_t) tv.tv_sec) * 1000 * 1000 * 1000 + milliseconds * 1000 * 1000 + ((uint64_t) tv.tv_usec) * 1000;

				ts.tv_sec = nanoseconds / 1000 / 1000 / 1000;
				ts.tv_nsec = (nanoseconds - ((uint64_t) ts.tv_sec) * 1000 * 1000 * 1000);
			}

			do
			{
				//Regardless of whether it's an auto-reset or manual-reset event:
				//wait to obtain the event, then lock anyone else out
				if(milliseconds != (uint64_t) -1)
				{
					result = pthread_cond_timedwait(&event->CVariable, &event->Mutex, &ts);
				}
				else
				{
					result = pthread_cond_wait(&event->CVariable, &event->Mutex);
				}
			} while(result == 0 && !event->State);

			if(result == 0 && event->AutoReset)
			{
				//We've only accquired the event if the wait succeeded
				event->State = false;
			}
		}
		else if(event->AutoReset)
		{
			//It's an auto-reset event that's currently available;
			//we need to stop anyone else from using it
			result = 0;
			event->State = false;
		}
		//Else we're trying to obtain a manual reset event with a signalled state;
		//don't do anything
		return result;
	}

	static int WaitForEvent(neosmart_event_t event, uint64_t milliseconds)
	{
		int tempResult;
		if(milliseconds == 0)
		{
			tempResult = pthread_mutex_trylock(&event->Mutex);
			if(tempResult == EBUSY)
			{
				return ETIMEDOUT;
			}
		}
		else
		{
			tempResult = pthread_mutex_lock(&event->Mutex);
		}

		assert(tempResult == 0);

		int result = UnlockedWaitForEvent(event, milliseconds);

		tempResult = pthread_mutex_unlock(&event->Mutex);
		assert(tempResult == 0);

		return result;
	}

#ifdef WFMO
	static int WaitForMultipleEvents(neosmart_event_t *events, int count, bool waitAll, uint64_t milliseconds, int &waitIndex)
	{
		neosmart_wfmo_t wfmo = new neosmart_wfmo_t_;

		int result = 0;
		int tempResult = pthread_mutex_init(&wfmo->Mutex, 0);
		assert(tempResult == 0);

		tempResult = pthread_cond_init(&wfmo->CVariable, 0);
		assert(tempResult == 0);

		neosmart_wfmo_info_t_ waitInfo;
		waitInfo.Waiter = wfmo;
		waitInfo.WaitIndex = -1;

		wfmo->WaitAll = waitAll;
		wfmo->StillWaiting = true;
		wfmo->RefCount = 1;

		if(waitAll)
		{
			wfmo->Status.EventsLeft = count;
		}
		else
		{
			wfmo->Status.FiredEvent = -1;
		}

		tempResult = pthread_mutex_lock(&wfmo->Mutex);
		assert(tempResult == 0);

		bool done = false;
		waitIndex = -1;

		for(int i = 0; i < count; ++i)
		{
			waitInfo.WaitIndex = i;

			//Must not release lock until RegisteredWait is potentially added
			tempResult = pthread_mutex_lock(&events[i]->Mutex);
			assert(tempResult == 0);

			//Before adding this wait to the list of registered waits, let's clean up old, expired waits while we have the event lock anyway
			events[i]->RegisteredWaits.erase(std::remove_if(events[i]->RegisteredWaits.begin(), events[i]->RegisteredWaits.end(), RemoveExpiredWaitHelper), events[i]->RegisteredWaits.end());

			if(UnlockedWaitForEvent(events[i], 0) == 0)
			{
				tempResult = pthread_mutex_unlock(&events[i]->Mutex);
				assert(tempResult == 0);

				if(waitAll)
				{
					--wfmo->Status.EventsLeft;
					assert(wfmo->Status.EventsLeft >= 0);
				}
				else
				{
					wfmo->Status.FiredEvent = i;
					waitIndex = i;
					done = true;
					break;
				}
			}
			else
			{
				events[i]->RegisteredWaits.push_back(waitInfo);
				++wfmo->RefCount;

				tempResult = pthread_mutex_unlock(&events[i]->Mutex);
				assert(tempResult == 0);
			}
		}

		timespec ts;
		if(!done)
		{
			if(milliseconds == 0)
			{
				result = ETIMEDOUT;
				done = true;
			}
			else if(milliseconds != (uint64_t) -1)
			{
				timeval tv;
				gettimeofday(&tv, NULL);

				uint64_t nanoseconds = ((uint64_t) tv.tv_sec) * 1000 * 1000 * 1000 + milliseconds * 1000 * 1000 + ((uint64_t) tv.tv_usec) * 1000;

				ts.tv_sec = nanoseconds / 1000 / 1000 / 1000;
				ts.tv_nsec = (nanoseconds - ((uint64_t) ts.tv_sec) * 1000 * 1000 * 1000);
			}
		}

		while(!done)
		{
			//One (or more) of the events we're monitoring has been triggered?

			//If we're waiting for all events, assume we're done and check if there's an event that hasn't fired
			//But if we're waiting for just one event, assume we're not done until we find a fired event
			done = (waitAll && wfmo->Status.EventsLeft == 0) || (!waitAll && wfmo->Status.FiredEvent != -1);

			if(!done)
			{
				if(milliseconds != (uint64_t) -1)
				{
					result = pthread_cond_timedwait(&wfmo->CVariable, &wfmo->Mutex, &ts);
				}
				else
				{
					result = pthread_cond_wait(&wfmo->CVariable, &wfmo->Mutex);
				}

				if(result != 0)
				{
					break;
				}
			}
		}

		waitIndex = wfmo->Status.FiredEvent;
		wfmo->StillWaiting = false;

		--wfmo->RefCount;
		assert(wfmo->RefCount >= 0);

		tempResult = pthread_mutex_unlock(&wfmo->Mutex);
		assert(tempResult == 0);

		if(wfmo->RefCount == 0)
		{
			wfmo->Destroy();
			delete wfmo;
		}
		else
		{
			;/*NULL*/
		}

		return result;
	}
	/* //Unused
	static int WaitForMultipleEvents(neosmart_event_t *events, int count, bool waitAll, uint64_t milliseconds)
	{
	int unused;
	return WaitForMultipleEvents(events, count, waitAll, milliseconds, unused);
	}
	*/
#endif

	static int DestroyEvent(neosmart_event_t event)
	{
		int result = pthread_cond_destroy(&event->CVariable);
		assert(result == 0);

		result = pthread_mutex_destroy(&event->Mutex);
		assert(result == 0);

		delete event;

		return 0;
	}

	static int SetEvent(neosmart_event_t event)
	{
		int result = pthread_mutex_lock(&event->Mutex);
		assert(result == 0);

		event->State = true;

		//Depending on the event type, we either trigger everyone or only one
		if(event->AutoReset)
		{
#ifdef WFMO
			while(!event->RegisteredWaits.empty())
			{
				neosmart_wfmo_info_t i = &event->RegisteredWaits.front();

				result = pthread_mutex_lock(&i->Waiter->Mutex);
				assert(result == 0);

				--i->Waiter->RefCount;
				assert(i->Waiter->RefCount >= 0);
				if(!i->Waiter->StillWaiting)
				{
					result = pthread_mutex_unlock(&i->Waiter->Mutex);
					assert(result == 0);

					if(i->Waiter->RefCount == 0)
					{
						i->Waiter->Destroy();
						delete i->Waiter;
					}
					else
					{
						;/*NULL*/
					}
					event->RegisteredWaits.pop_front();
					continue;
				}

				event->State = false;

				if(i->Waiter->WaitAll)
				{
					--i->Waiter->Status.EventsLeft;
					assert(i->Waiter->Status.EventsLeft >= 0);
					//We technically should do i->Waiter->StillWaiting = Waiter->Status.EventsLeft != 0
					//but the only time it'll be equal to zero is if we're the last event, so no one
					//else will be checking the StillWaiting flag. We're good to go without it.
				}
				else
				{
					i->Waiter->Status.FiredEvent = i->WaitIndex;
					i->Waiter->StillWaiting = false;
				}

				result = pthread_mutex_unlock(&i->Waiter->Mutex);
				assert(result == 0);

				result = pthread_cond_signal(&i->Waiter->CVariable);
				assert(result == 0);

				event->RegisteredWaits.pop_front();

				result = pthread_mutex_unlock(&event->Mutex);
				assert(result == 0);

				return 0;
			}
#endif
			//event->State can be false if compiled with WFMO support
			if(event->State)
			{
				result = pthread_mutex_unlock(&event->Mutex);
				assert(result == 0);

				result = pthread_cond_signal(&event->CVariable);
				assert(result == 0);

				return 0;
			}
		}
		else
		{
#ifdef WFMO
			for(size_t i = 0; i < event->RegisteredWaits.size(); ++i)
			{
				neosmart_wfmo_info_t info = &event->RegisteredWaits[i];

				result = pthread_mutex_lock(&info->Waiter->Mutex);
				assert(result == 0);

				--info->Waiter->RefCount;
				assert(info->Waiter->RefCount >= 0);

				if(!info->Waiter->StillWaiting)
				{
					result = pthread_mutex_unlock(&info->Waiter->Mutex);
					assert(result == 0);

					if(info->Waiter->RefCount == 0)
					{
						info->Waiter->Destroy();
						delete info->Waiter;
					}
					else
					{
						;/*NULL*/						
					}
					continue;
				}

				if(info->Waiter->WaitAll)
				{
					--info->Waiter->Status.EventsLeft;
					assert(info->Waiter->Status.EventsLeft >= 0);
					//We technically should do i->Waiter->StillWaiting = Waiter->Status.EventsLeft != 0
					//but the only time it'll be equal to zero is if we're the last event, so no one
					//else will be checking the StillWaiting flag. We're good to go without it.
				}
				else
				{
					info->Waiter->Status.FiredEvent = info->WaitIndex;
					info->Waiter->StillWaiting = false;
				}

				result = pthread_mutex_unlock(&info->Waiter->Mutex);
				assert(result == 0);

				result = pthread_cond_signal(&info->Waiter->CVariable);
				assert(result == 0);
			}
			event->RegisteredWaits.clear();
#endif
			result = pthread_mutex_unlock(&event->Mutex);
			assert(result == 0);

			result = pthread_cond_broadcast(&event->CVariable);
			assert(result == 0);
		}

		return 0;
	}

	static neosmart_event_t CreateEvent(bool manualReset, bool initialState)
	{
		neosmart_event_t event = new neosmart_event_t_;

		int result = pthread_cond_init(&event->CVariable, 0);
		assert(result == 0);

		result = pthread_mutex_init(&event->Mutex, 0);
		assert(result == 0);

		event->State = false;
		event->AutoReset = !manualReset;

		if(initialState)
		{
			result = SetEvent(event);
			assert(result == 0);
		}

		return event;
	}
	
	static int ResetEvent(neosmart_event_t event)
	{
		int result = pthread_mutex_lock(&event->Mutex);
		assert(result == 0);

		event->State = false;

		result = pthread_mutex_unlock(&event->Mutex);
		assert(result == 0);

		return 0;
	}
}

/**
* @class    CSimpleMutex
* @brief    CSimpleMutex Interface
*			 This class represents basic mutex class.\n
*			 This mutex must support recursive locking mechanism
*
* @see
*/
class CSimpleMutex
{
private:
	pthread_mutex_t			m_mutex;
	pthread_mutexattr_t		m_mutexAttribute;

public:
	CSimpleMutex()
	{
		pthread_mutexattr_init(&m_mutexAttribute);
		pthread_mutexattr_settype(&m_mutexAttribute, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&m_mutex, &m_mutexAttribute);
	}

	~CSimpleMutex()
	{
		pthread_mutex_destroy(&m_mutex);
		pthread_mutexattr_destroy(&m_mutexAttribute);
	}

	/**
	* @fn	  lock
	* @brief Lock current thread
	*
	* @param NONE
	* 
	* @return NONE
	* @warning      
	* @exception    
	* @see
	*/
	void lock()
	{
		pthread_mutex_lock( &m_mutex );
	}

	/**
	* @fn	  unlock
	* @brief Unlock current thread
	*
	* @param NONE
	* 
	* @return NONE
	* @warning      
	* @exception    
	* @see
	*/
	void unlock()
	{
		pthread_mutex_unlock( &m_mutex );
	}
};

/**
* @class    CTaskWorker
* @brief    CTaskWorker Interface
*			 This class represents worker thread utility that holds one thread
*
* @see
*/
class CTaskWorker
{
private:
	pthread_t m_hThread;

	CSimpleMutex  m_mutex;

	neosmart::neosmart_event_t m_hInitializeEvent;
	neosmart::neosmart_event_t m_hAddTaskEvent;
	neosmart::neosmart_event_t m_hShutdownEvent;
	neosmart::neosmart_event_t m_hShutdownCompleteEvent;

	struct ClientEntry
	{
		IThreadClient   *pClient;
		void			*pArg;
	};

	std::list<ClientEntry>	m_clientEntry;

	static void _worker(void *pArg)
	{
		CTaskWorker *pThread = (CTaskWorker *)pArg;
		return pThread->worker();
	}

	bool getTask(ClientEntry *clientEntry)
	{
		neosmart::neosmart_event_t handles[] = {m_hShutdownEvent, m_hAddTaskEvent};

		bool bRet = false;
		int index = 0;

		if(neosmart::WaitForMultipleEvents(handles, 2, false, -1, index) != 0)
		{
			return false;
		}

		switch(index)
		{
			//requested for shutdown
		case 0:
			bRet = false;
			break;

			//requested for new task added
		case 1:
			m_mutex.lock();
			*clientEntry = m_clientEntry.front();
			m_clientEntry.pop_front();
			if(m_clientEntry.empty())
			{
				neosmart::ResetEvent(m_hAddTaskEvent);
			}
			m_mutex.unlock();
			bRet = true;
			break;

			//invalid event raised
		default:
			bRet = false;
			break;
		}

		return bRet;
	}

	void worker()
	{
		ClientEntry clientEntry;

		//Thread Creation completed
		neosmart::SetEvent(m_hInitializeEvent);

		//Wait for any tasks
		while(getTask(&clientEntry))
		{
			clientEntry.pClient->onExecute(clientEntry.pArg);
			clientEntry.pClient->onTerminate(clientEntry.pArg);
			SAFE_RELEASE(clientEntry.pClient);
		}

		//Thread is almost terminated
		neosmart::SetEvent(m_hShutdownCompleteEvent);
	}

public:
	CTaskWorker()
	{
		m_hThread = 0;
		m_hInitializeEvent = NULL;
		m_hAddTaskEvent = NULL;
		m_hShutdownEvent = NULL;
		m_hShutdownCompleteEvent = NULL;
	}

	~CTaskWorker()
	{
	}

	/**
	* @fn	  initialize
	* @brief Initialize current worker thread
	*
	* @param NONE
	* 
	* @return SSMRESULT
	* @warning      
	* @exception    
	* @see
	*/
	SSMRESULT initialize()
	{
		m_hInitializeEvent = neosmart::CreateEvent(false, false);

		m_hAddTaskEvent = neosmart::CreateEvent(true, false);

		m_hShutdownEvent = neosmart::CreateEvent(false, false);

		m_hShutdownCompleteEvent = neosmart::CreateEvent(false, false);

		//Create thread and wait for jobs
		if(pthread_create(&m_hThread, NULL, (void*(*)(void*))_worker, (void*)this) != 0)
		{
			return SSM_E_FAIL;
		}

		//Wait till thread creation is done
		if(neosmart::WaitForEvent(m_hInitializeEvent, -1) != 0)
		{
			return SSM_E_FAIL;
		}

		return SSM_S_OK;
	}

	/**
	* @fn	  addTask
	* @brief Add task to current worker thread.\n
	*		  Each task called only once
	*
	* @param [in] IThreadClient *pThreadClient - Implemented thread client class instance that called from worker thread.
	* @param [in] void *param - Optional parameter that IThreadClient::OnExecute/OnTerminate can receive
	* 
	* @return SSMRESULT
	* @warning      
	* @exception    
	* @see
	*/
	SSMRESULT addTask(IThreadClient *pThreadClient, void *param)
	{
		ClientEntry clientEntry;

		pThreadClient->addRef();
		clientEntry.pClient = pThreadClient;
		clientEntry.pArg = param;

		m_mutex.lock();
		m_clientEntry.push_back(clientEntry);
		//Let the task worker know, we just added task
		neosmart::SetEvent(m_hAddTaskEvent);
		m_mutex.unlock();

		return SSM_S_OK;
	}

	/**
	* @fn	  terminate
	* @brief Terminate current worker thread.\n
	*		  All remained tasks are destroyed after IThreadClient::OnTerminate called
	*
	* @param NONE
	* 
	* @return SSMRESULT
	* @warning      
	* @exception    
	* @see
	*/
	SSMRESULT terminate()
	{
		SSMRESULT res = SSM_S_OK;
		//Wait Created Thread's termination
		neosmart::SetEvent(m_hShutdownEvent);

		//Wait till thread creation is done
		if(neosmart::WaitForEvent(m_hShutdownCompleteEvent, 10000) != 0)
		{
			res = SSM_E_FAIL;
		}

		m_mutex.lock();
		//Remove all tasks from queue
		for(std::list<ClientEntry>::iterator itor = m_clientEntry.begin();
			itor != m_clientEntry.end(); ++itor)
		{
			ClientEntry clientEntry = *itor;
			clientEntry.pClient->onTerminate(clientEntry.pArg);
			SAFE_RELEASE(clientEntry.pClient);
		}
		m_clientEntry.clear();
		m_mutex.unlock();

		if(m_hThread != 0)
		{
			pthread_detach(m_hThread);
			m_hThread = 0;
		}

		if(m_hInitializeEvent != NULL)
		{
			neosmart::DestroyEvent(m_hInitializeEvent);
			m_hInitializeEvent = NULL;
		}

		if(m_hAddTaskEvent != NULL)
		{
			neosmart::DestroyEvent(m_hAddTaskEvent);
			m_hAddTaskEvent = NULL;
		}

		if(m_hShutdownEvent != NULL)
		{
			neosmart::DestroyEvent(m_hShutdownEvent);
			m_hShutdownEvent = NULL;
		}

		if(m_hShutdownCompleteEvent != NULL)
		{
			neosmart::DestroyEvent(m_hShutdownCompleteEvent);
			m_hShutdownCompleteEvent = NULL;
		}

		return res;
	}
};

#endif

static const OID OID_IThreadPool = { 0x53855605, 0xb2a0, 0x4a31, { 0xa2, 0x60, 0x36, 0x4d, 0x43, 0xc1, 0x2f, 0x96 } };
/**
* @class    IThreadPool
* @brief    IThreadPool Interface
*			 This class represents worker thread pool utility
*
* @see
*/
class IThreadPool : public IBase
{
public:
	/**
	* @fn	  addTask
	* @brief Add task to current worker thread pool.\n
	*		  Each task called only once
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
* @class    CThreadPool
* @brief    CThreadPool Interface
*			 This class represents worker thread pool utility that can holds multiple thread
*
* @see
*/
class CThreadPool :
	public CObjectRoot<CObjectMultiThreadModel>
	, public IThreadPool
{
private:
	CTaskWorker			m_taskWorker;

public:
	SSMRESULT finalConstruct()
	{
		return m_taskWorker.initialize();
	}

	void finalRelease()
	{
		m_taskWorker.terminate();
	}

	SSMRESULT queryInterface(const OID& objectID, IBase** ppObject)
	{
		if(ppObject == NULL)
			return SSM_E_POINTER;

		if(IsEqualOID(objectID, OID_IThreadPool))
		{
			IBase *pBase = this;
			pBase->addRef();
			*ppObject = pBase;
			return SSM_S_OK;
		}

		return SSM_E_NOINTERFACE;
	}

	/**
	* @fn	  addTask
	* @brief Add task to current worker thread pool.\n
	*		  Each task called only once
	*
	* @param [in] IThreadClient *pThreadClient - Implemented thread client class instance that called from worker thread.
	* @param [in] void *param - Optional parameter that IThreadClient::OnExecute/OnTerminate can receive
	* 
	* @return SSMRESULT
	* @warning      
	* @exception    
	* @see
	*/
	SSMRESULT addTask(IThreadClient *pThreadClient, void *param)
	{
		return m_taskWorker.addTask(pThreadClient, param);
	}
};

#endif
