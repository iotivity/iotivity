//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
//
//*********************************************************************

/**
 * @file
 * This file provides APIs related to mutex and semaphores.
 */

#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <oic_malloc.h>

#include "camutex.h"
#include "logger.h"

/**
 * TAG
 * Logging tag for module name
 */
#define TAG PCF("UMUTEX")

static const uint64_t USECS_PER_SEC = 1000000;
static const uint64_t NANOSECS_PER_USECS = 1000;

typedef struct _tagMutexInfo_t
{
    pthread_mutex_t mutex;
} ca_mutex_internal;

typedef struct _tagEventInfo_t
{
    pthread_cond_t cond;
} ca_cond_internal;

ca_mutex ca_mutex_new(void)
{
    ca_mutex retVal = NULL;
    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) OICMalloc(sizeof(ca_mutex_internal));
    if (NULL != mutexInfo)
    {
        // create the mutex with the attributes set
        int ret=pthread_mutex_init(&(mutexInfo->mutex), PTHREAD_MUTEX_DEFAULT);
        if (0 == ret)
        {
            retVal = (ca_mutex) mutexInfo;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s Failed to initialize mutex !", __func__);
            OICFree(mutexInfo);
        }
    }

    return retVal;
}

bool ca_mutex_free(ca_mutex mutex)
{
    bool bRet=false;

    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) mutex;
    if (mutexInfo)
    {
        int ret = pthread_mutex_destroy(&mutexInfo->mutex);
        if (0 == ret)
        {
            OICFree(mutexInfo);
            bRet=true;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s Failed to free mutex !", __func__);
        }
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
        int ret = pthread_mutex_lock(&mutexInfo->mutex);
        assert(0 == ret);
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s Invalid mutex !", __func__);
        return;
    }
}

bool ca_mutex_trylock(ca_mutex mutex)
{
    if (NULL == mutex)
    {
        OIC_LOG_V(ERROR, TAG, "%s Invalid mutex !", __func__);
        return false;
    }

    bool bRet = false;

    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) mutex;

    int result = pthread_mutex_trylock(&mutexInfo->mutex);

    switch (result)
    {
        case 0:
            // Success
            bRet = true;
            break;
        case EINVAL:
            OIC_LOG_V(ERROR, TAG, "%s: Invalid mutex !", __func__);
            break;
        case EBUSY:
        default:
            break;
    }

    return bRet;
}

void ca_mutex_unlock(ca_mutex mutex)
{
    ca_mutex_internal *mutexInfo = (ca_mutex_internal*) mutex;
    if (mutexInfo)
    {
        int ret = pthread_mutex_unlock(&mutexInfo->mutex);
        assert(ret == 0);
    }
    else
    {
          OIC_LOG_V(ERROR, TAG, "%s: Invalid mutex !", __func__);
          return;
    }
}

ca_cond ca_cond_new(void)
{
    ca_cond retVal = NULL;
    ca_cond_internal *eventInfo = (ca_cond_internal*) OICMalloc(sizeof(ca_cond_internal));
    if (NULL != eventInfo)
    {
        int ret = pthread_cond_init(&(eventInfo->cond), NULL);
        if (0 == ret)
        {
            retVal = (ca_cond) eventInfo;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s: Failed to initialize condition variable %d!", __func__, ret);
            OICFree(eventInfo);
        }
    }

    return retVal;
}

void ca_cond_free(ca_cond cond)
{
    ca_cond_internal *eventInfo = (ca_cond_internal*) cond;
    if (eventInfo != NULL)
    {
        int ret = pthread_cond_destroy(&(eventInfo->cond));
        if (0 == ret)
        {
            OICFree(cond);
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "%s: Failed to destroy condition variable %d", __func__, ret);
        }
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
        int ret = pthread_cond_signal(&(eventInfo->cond));
        if (0 != ret)
        {
            OIC_LOG_V(ERROR, TAG, "%s: Failed to signal condition variable", __func__);
        }
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
        int ret = pthread_cond_broadcast(&(eventInfo->cond));
        if (0 != ret)
        {
            OIC_LOG_V(ERROR, TAG, "%s: failed to signal condition variable", __func__);
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "%s: Invalid parameter", __func__);
    }
}

void ca_cond_wait(ca_cond cond, ca_mutex mutex)
{
    ca_cond_wait_until(cond, mutex, 0L);
}

CAWaitResult_t ca_cond_wait_until(ca_cond cond, ca_mutex mutex, uint64_t microseconds)
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

    if (microseconds > 0)
    {
        struct timespec abstime;
        abstime.tv_sec = (microseconds / USECS_PER_SEC);
        abstime.tv_nsec = ((microseconds % USECS_PER_SEC) * NANOSECS_PER_USECS);

        //Wait for the given time
        int ret = pthread_cond_timedwait(&(eventInfo->cond), &(mutexInfo->mutex), &abstime);
        switch (ret)
        {
            case 0:
                // Success
                retVal = CA_WAIT_SUCCESS;
                break;
            case ETIMEDOUT:
                retVal = CA_WAIT_TIMEDOUT;
                break;
            case EINVAL:
                OIC_LOG_V(ERROR, TAG, "%s: condition, mutex, or abstime is Invalid", __func__);
                retVal = CA_WAIT_INVAL;
                break;
            default:
                OIC_LOG_V(ERROR, TAG, "%s: pthread_cond_timedwait returned %d", __func__, retVal);
                retVal = CA_WAIT_INVAL;
                break;
        }
    }
    else
    {
        // Wait forever
        int ret = pthread_cond_wait(&eventInfo->cond, &mutexInfo->mutex);
        retVal = ret == 0 ? CA_WAIT_SUCCESS : CA_WAIT_INVAL;
    }

    return retVal;
}

