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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "caqueueingthread.h"
#include "oic_malloc.h"
#include "logger.h"

#define TAG PCF("CA")

static void CAQueueingThreadBaseRoutine(void *threadValue)
{
    OIC_LOG_V(DEBUG, TAG, "message handler main thread start..");

    CAQueueingThread_t *thread = (CAQueueingThread_t *) threadValue;

    if (thread == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "thread data passing error!!");

        return;
    }

    while (!thread->isStop)
    {
        // mutex lock
        u_mutex_lock(thread->threadMutex);

        // if queue is empty, thread will wait
        if (u_queue_get_size(thread->dataQueue) <= 0)
        {
            OIC_LOG_V(DEBUG, TAG, "wait..");

            // wait
            u_cond_wait(thread->threadCond, thread->threadMutex);

            OIC_LOG_V(DEBUG, TAG, "wake up..");
        }

        // mutex unlock
        u_mutex_unlock(thread->threadMutex);

        // check stop flag
        if (thread->isStop)
            continue;

        // get data
        u_queue_message_t *message = u_queue_get_element(thread->dataQueue);
        if (message == NULL)
        {
            continue;
        }

        void *data = message->msg;

        // process data
        thread->threadTask(data);

        // free
        OICFree(message);
    }

    u_cond_signal(thread->threadCond);

    OIC_LOG_V(DEBUG, TAG, "message handler main thread end..");
}

CAResult_t CAQueueingThreadInitialize(CAQueueingThread_t *thread, u_thread_pool_t handle,
                                      CAThreadTask task)
{
    if (thread == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "thread instance is empty..");
        return CA_STATUS_FAILED;
    }

    if (handle == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "thread pool handle is empty..");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "thread initialize..");

    memset(thread, 0, sizeof(CAQueueingThread_t));

    // mutex init
    u_mutex_init();

    // set send thread data
    thread->threadPool = handle;
    thread->dataQueue = u_queue_create();
    thread->threadMutex = u_mutex_new();
    thread->threadCond = u_cond_new();
    thread->isStop = CA_TRUE;
    thread->threadTask = task;

    return CA_STATUS_OK;
}

CAResult_t CAQueueingThreadStart(CAQueueingThread_t *thread)
{
    if (thread == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "thread instance is empty..");
        return CA_STATUS_FAILED;
    }

    if (thread->threadPool == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "thread pool handle is empty..");
        return CA_STATUS_FAILED;
    }

    if (CA_FALSE == thread->isStop) //Queueing thread already running
    {
        OIC_LOG_V(DEBUG, TAG, "queueing thread already running..");
        return CA_STATUS_OK;
    }

    thread->isStop = CA_FALSE;
    CAResult_t res = u_thread_pool_add_task(thread->threadPool, CAQueueingThreadBaseRoutine,
                                            thread);
    if (res != CA_STATUS_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "thread pool add task error(send thread).");
        thread->isStop = CA_TRUE;
        return res;
    }

    return res;
}

CAResult_t CAQueueingThreadAddData(CAQueueingThread_t *thread, void *data, uint32_t size)
{
    if (thread == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "thread instance is empty..");
        return CA_STATUS_FAILED;
    }

    if (data == NULL || size == 0)
    {
        OIC_LOG_V(DEBUG, TAG, "data is empty..");

        return CA_STATUS_FAILED;
    }

    // create thread data
    u_queue_message_t *message = (u_queue_message_t *) OICMalloc(sizeof(u_queue_message_t));

    if (message == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "memory error!!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memset(message, 0, sizeof(u_queue_message_t));

    message->msg = data;
    message->size = sizeof(size);

    // mutex lock
    u_mutex_lock(thread->threadMutex);

    // add thread data into list
    u_queue_add_element(thread->dataQueue, message);

    // notity the thread
    u_cond_signal(thread->threadCond);

    // mutex unlock
    u_mutex_unlock(thread->threadMutex);

    return CA_STATUS_OK;
}

CAResult_t CAQueueingThreadDestroy(CAQueueingThread_t *thread)
{
    if (thread == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "thread instance is empty..");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "thread destroy..");

    u_mutex_free(thread->threadMutex);
    thread->threadMutex = NULL;
    u_cond_free(thread->threadCond);
    u_queue_delete(thread->dataQueue);

    return CA_STATUS_OK;
}

CAResult_t CAQueueingThreadStop(CAQueueingThread_t *thread)
{
    if (thread == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "thread instance is empty..");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "thread stop request!!");

    if (!thread->isStop)
    {
        // mutex lock
        u_mutex_lock(thread->threadMutex);

        // set stop flag
        thread->isStop = CA_TRUE;

        // notity the thread
        u_cond_signal(thread->threadCond);

        u_cond_wait(thread->threadCond, thread->threadMutex);

        // mutex unlock
        u_mutex_unlock(thread->threadMutex);
    }

    return CA_STATUS_OK;
}

