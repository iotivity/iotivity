/* *****************************************************************
*
* Copyright 2016 Intel Corporation
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


/**
 * @file
 * This file provides APIs related to mutex and semaphores.
 */
#include "iotivity_config.h"
#include "octhread.h"
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <stdio.h>
#include <errno.h>
#include <oic_malloc.h>

#include "logger.h"

static const uint64_t USECS_PER_MSEC = 1000;

typedef struct _tagMutexInfo_t
{
    CRITICAL_SECTION mutex;
} ca_mutex_internal;

typedef struct _tagEventInfo_t
{
    CONDITION_VARIABLE cond;
} ca_cond_internal;

typedef struct _tagThreadInfo_t
{
    HANDLE handle;
} ca_thread_internal;

CAThreadResult_t ca_thread_new(ca_thread *t, void *(*start_routine)(void *), void *arg)
{
    CAThreadResult_t res = CA_THREAD_SUCCESS;
    ca_thread_internal *threadInfo = (ca_thread_internal*)OICMalloc(sizeof(ca_thread_internal));
    if (NULL != threadInfo)
    {
        threadInfo->handle = CreateThread(NULL, 0, (PTHREAD_START_ROUTINE)start_routine, arg, 0, NULL);
        if (threadInfo->handle == NULL)
        {
            res = CA_THREAD_CREATE_FAILURE;
            *t = NULL;
            OICFree(threadInfo);
            OIC_LOG_V(ERROR, TAG, "%s: CreateThread failed: %i", __func__, GetLastError());
        }
        else
        {
            *t = (ca_thread)threadInfo;
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Failed to allocate thread!", __func__);
        *t = NULL;
        res = CA_THREAD_ALLOCATION_FAILURE;
    }

   return res;
}

CAThreadResult_t ca_thread_free(ca_thread t)
{
    CAThreadResult_t res = CA_THREAD_INVALID_PARAMETER;
    ca_thread_internal *threadInfo = (ca_thread_internal*) t;
    if (threadInfo)
    {
        CloseHandle(threadInfo->handle);
        OICFree(threadInfo);
        res = CA_THREAD_SUCCESS;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Invalid thread !", __func__);
    }
    return res;
}

CAThreadResult_t ca_thread_wait(ca_thread t)
{
    CAThreadResult_t res = CA_THREAD_SUCCESS;
    ca_thread_internal *threadInfo = (ca_thread_internal*) t;
    DWORD joinres = WaitForSingleObject(threadInfo->handle, INFINITE);
    if (WAIT_OBJECT_0 != joinres)
    {
        OIC_LOG(ERROR, TAG, "Failed to join thread");
        res = CA_THREAD_WAIT_FAILURE;
    }
    else
    {
        CloseHandle(threadInfo->handle);
    }
    return res;
}

ca_mutex ca_mutex_new(void)
{
    ca_mutex retVal = NULL;
    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) OICMalloc(sizeof(ca_mutex_internal));
    if (NULL != mutexInfo)
    {
        InitializeCriticalSection(&mutexInfo->mutex);
        retVal = (ca_mutex)mutexInfo;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Failed to allocate mutex!", __func__);
    }

    return retVal;
}

bool ca_mutex_free(ca_mutex mutex)
{
    bool bRet = false;
    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) mutex;
    if (mutexInfo)
    {
        DeleteCriticalSection(&mutexInfo->mutex);
        OICFree(mutexInfo);
        bRet=true;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Invalid mutex !", __func__);
    }

    return bRet;
}

void ca_mutex_lock(ca_mutex mutex)
{
    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) mutex;
    if (mutexInfo)
    {
        EnterCriticalSection(&mutexInfo->mutex);
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Invalid mutex !", __func__);
    }
}

void ca_mutex_unlock(ca_mutex mutex)
{
    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) mutex;
    if (mutexInfo)
    {
        LeaveCriticalSection(&mutexInfo->mutex);
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid mutex !", __func__);
    }
}

ca_cond ca_cond_new(void)
{
    ca_cond retVal = NULL;
    ca_cond_internal *eventInfo = (ca_cond_internal*) OICMalloc(sizeof(ca_cond_internal));
    if (NULL != eventInfo)
    {
        InitializeConditionVariable(&eventInfo->cond);
        retVal = (ca_cond) eventInfo;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Failed to allocate condition variable!", __func__);
    }

    return retVal;
}

void ca_cond_free(ca_cond cond)
{
    ca_cond_internal *eventInfo = (ca_cond_internal*) cond;
    if (eventInfo != NULL)
    {
        OICFree(cond);
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid parameter", __func__);
    }
}

void ca_cond_signal(ca_cond cond)
{
    ca_cond_internal *eventInfo = (ca_cond_internal*) cond;
    if (eventInfo != NULL)
    {
        WakeConditionVariable(&eventInfo->cond);
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid parameter", __func__);
    }
}

void ca_cond_broadcast(ca_cond cond)
{
    ca_cond_internal* eventInfo = (ca_cond_internal*) cond;
    if (eventInfo != NULL)
    {
        WakeAllConditionVariable(&eventInfo->cond);
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid parameter", __func__);
    }
}

void ca_cond_wait(ca_cond cond, ca_mutex mutex)
{
    ca_cond_wait_for(cond, mutex, 0L);
}

CAWaitResult_t ca_cond_wait_for(ca_cond cond, ca_mutex mutex, uint64_t microseconds)
{
    CAWaitResult_t retVal = CA_WAIT_INVAL;

    ca_cond_internal *eventInfo = (ca_cond_internal*) cond;
    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) mutex;

    if (NULL == mutexInfo)
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid mutex", __func__);
        return CA_WAIT_INVAL;
    }

    if (NULL == eventInfo)
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid condition", __func__);
        return CA_WAIT_INVAL;
    }

    DWORD milli = 0;
    if (microseconds > 0)
    {
        milli = (DWORD)(microseconds / USECS_PER_MSEC);
    }
    else
    {
        milli = INFINITE;
    }

    // Wait for the given time        
    if (!SleepConditionVariableCS(&eventInfo->cond, &mutexInfo->mutex, milli))
    {
        if (GetLastError() == ERROR_TIMEOUT)
        {
            retVal = CA_WAIT_TIMEDOUT;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "SleepConditionVariableCS() failed %i", GetLastError());
            retVal = CA_WAIT_INVAL;
        }
    }
    else
    {
        retVal = CA_WAIT_SUCCESS;
    }

    return retVal;
}

