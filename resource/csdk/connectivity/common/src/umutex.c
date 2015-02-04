/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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
 * @file    umutex.c
 * @brief   This file provides APIs related to mutex and semaphores
 */

#include "umutex.h"
#include <glib.h>
#include <string.h>
#include "logger.h"

/**
 * @def TAG
 * @brief Logging tag for module name
 */
#define TAG PCF("UMUTEX")

u_mutex u_mutex_new(void)
{
    GMutex *mutexLock = g_new(GMutex, 1);
    g_mutex_init(mutexLock);
    return (u_mutex) mutexLock;
}

void u_mutex_lock(u_mutex mutex)
{
    if (NULL == mutex)
    {
        OIC_LOG_V(ERROR, TAG , "u_mutex_lock, Invalid mutex !");
        return;
    }

    GMutex *mutexLock = (GMutex *) mutex;
    g_mutex_lock(mutexLock);
}

bool u_mutex_trylock(u_mutex mutex)
{
    if (NULL == mutex)
    {
        OIC_LOG_V(ERROR, TAG, "u_mutex_trylock, Invalid mutex !");
        return false;
    }

    GMutex *mutexLock = (GMutex *) mutex;

    return(g_mutex_trylock(mutexLock));
}

void u_mutex_unlock(u_mutex mutex)
{
    if (NULL == mutex)
    {
        OIC_LOG_V(ERROR, TAG, "u_mutex_unlock, Invalid mutex !");
        return;
    }

    GMutex *mutexLock = (GMutex *) mutex;
    g_mutex_unlock(mutexLock);
}

void u_mutex_free(u_mutex mutex)
{
    if (NULL == mutex)
    {
        OIC_LOG_V(ERROR, TAG, "u_mutex_free, Invalid mutex !");
        return;
    }

    GMutex *mutexLock = (GMutex *) mutex;
    g_mutex_clear(mutexLock);
    g_free(mutexLock);
}

u_cond u_cond_new(void)
{
    GCond *condition = g_new(GCond, 1);
    g_cond_init(condition);
    return (u_cond) condition;
}

void u_cond_signal(u_cond cond)
{
    if (NULL == cond)
    {
        OIC_LOG_V(ERROR, TAG, "u_cond_signal, Invalid condition !");
        return;
    }

    GCond *condition = (GCond *) cond;
    g_cond_signal(condition);
}

void u_cond_broadcast(u_cond cond)
{
    if (NULL == cond)
    {
        OIC_LOG_V(ERROR, TAG, "u_cond_broadcast, Invalid condition !");
        return;
    }

    GCond *condition = (GCond *) cond;
    g_cond_broadcast(condition);
}

void u_cond_wait(u_cond cond, u_mutex mutex)
{
    if (NULL == mutex)
    {
        OIC_LOG_V(ERROR, TAG, "u_cond_wait, Invalid mutex !");
        return;
    }

    if (NULL == cond)
    {
        OIC_LOG_V(ERROR, TAG, "u_cond_wait, Invalid condition !");
        return;
    }

    GMutex *mutexLock = (GMutex *) mutex;
    GCond *condition = (GCond *) cond;
    g_cond_wait(condition, mutexLock);
}

void u_cond_wait_until(u_cond cond, u_mutex mutex, int32_t microseconds)
{
    if (NULL == mutex)
    {
        OIC_LOG_V(ERROR, TAG, "u_cond_wait, Invalid mutex !");
        return;
    }

    if (NULL == cond)
    {
        OIC_LOG_V(ERROR, TAG, "u_cond_wait, Invalid condition !");
        return;
    }

    GMutex *mutexLock = (GMutex *) mutex;
    GCond *condition = (GCond *) cond;

    if (microseconds <= 0)
    {
        g_cond_wait(condition, mutexLock);
        return;
    }

    gint64 end_time;
    end_time = g_get_monotonic_time() + microseconds;

    g_cond_wait_until(condition, mutexLock, end_time);
}

void u_cond_free(u_cond cond)
{
    if (NULL == cond)
    {
        OIC_LOG_V(ERROR, TAG, "u_cond_free, Invalid condition !");
        return;
    }

    GCond *condition = (GCond *) cond;
    g_cond_clear(condition);
    g_free(condition);
}


