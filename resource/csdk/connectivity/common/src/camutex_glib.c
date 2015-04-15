/* ****************************************************************
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

/**
 * @file
 * This file provides APIs related to mutex and semaphores.
 */

#include "camutex.h"
#include <glib.h>
#include <string.h>
#include "logger.h"

/**
 * @def TAG
 * @brief Logging tag for module name
 */
#define TAG PCF("UMUTEX")

ca_mutex ca_mutex_new(void)
{
    GMutex *mutexLock = g_new(GMutex, 1);
    g_mutex_init(mutexLock);
    return (ca_mutex) mutexLock;
}

void ca_mutex_lock(ca_mutex mutex)
{
    if (NULL == mutex)
    {
        OIC_LOG(ERROR, TAG , "ca_mutex_lock, Invalid mutex !");
        return;
    }

    GMutex *mutexLock = (GMutex *) mutex;
    g_mutex_lock(mutexLock);
}

bool ca_mutex_trylock(ca_mutex mutex)
{
    if (NULL == mutex)
    {
        OIC_LOG(ERROR, TAG, "ca_mutex_trylock, Invalid mutex !");
        return false;
    }

    GMutex *mutexLock = (GMutex *) mutex;

    return(g_mutex_trylock(mutexLock));
}

void ca_mutex_unlock(ca_mutex mutex)
{
    if (NULL == mutex)
    {
        OIC_LOG(ERROR, TAG, "ca_mutex_unlock, Invalid mutex !");
        return;
    }

    GMutex *mutexLock = (GMutex *) mutex;
    g_mutex_unlock(mutexLock);
}

bool ca_mutex_free(ca_mutex mutex)
{
    if (NULL == mutex)
    {
        OIC_LOG(ERROR, TAG, "ca_mutex_free, Invalid mutex !");
        return false;
    }

    GMutex *mutexLock = (GMutex *) mutex;
    g_mutex_clear(mutexLock);
    g_free(mutexLock);
    return true;
}

ca_cond ca_cond_new(void)
{
    GCond *condition = g_new(GCond, 1);
    g_cond_init(condition);
    return (ca_cond) condition;
}

void ca_cond_signal(ca_cond cond)
{
    if (NULL == cond)
    {
        OIC_LOG(ERROR, TAG, "ca_cond_signal, Invalid condition !");
        return;
    }

    GCond *condition = (GCond *) cond;
    g_cond_signal(condition);
}

void ca_cond_broadcast(ca_cond cond)
{
    if (NULL == cond)
    {
        OIC_LOG(ERROR, TAG, "ca_cond_broadcast, Invalid condition !");
        return;
    }

    GCond *condition = (GCond *) cond;
    g_cond_broadcast(condition);
}

void ca_cond_wait(ca_cond cond, ca_mutex mutex)
{
    if (NULL == mutex)
    {
        OIC_LOG(ERROR, TAG, "ca_cond_wait, Invalid mutex !");
        return;
    }

    if (NULL == cond)
    {
        OIC_LOG(ERROR, TAG, "ca_cond_wait, Invalid condition !");
        return;
    }

    GMutex *mutexLock = (GMutex *) mutex;
    GCond *condition = (GCond *) cond;
    g_cond_wait(condition, mutexLock);
}

CAWaitResult_t ca_cond_wait_until(ca_cond cond, ca_mutex mutex, uint64_t microseconds)
{
    if (NULL == mutex)
    {
        OIC_LOG(ERROR, TAG, "ca_cond_wait, Invalid mutex !");
        return CA_WAIT_INVAL;
    }

    if (NULL == cond)
    {
        OIC_LOG(ERROR, TAG, "ca_cond_wait, Invalid condition !");
        return CA_WAIT_INVAL;
    }

    GMutex *mutexLock = (GMutex *) mutex;
    GCond *condition = (GCond *) cond;

    if (microseconds == 0)
    {
        g_cond_wait(condition, mutexLock);
        return CA_WAIT_SUCCESS;
    }

    gboolean bRet = g_cond_wait_until(condition, mutexLock, microseconds);
    return bRet ? CA_WAIT_SUCCESS : CA_WAIT_TIMEDOUT;
}

void ca_cond_free(ca_cond cond)
{
    if (NULL == cond)
    {
        OIC_LOG(ERROR, TAG, "ca_cond_free, Invalid condition !");
        return;
    }

    GCond *condition = (GCond *) cond;
    g_cond_clear(condition);
    g_free(condition);
}


