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
    OIC_LOG(DEBUG, TAG, "message handler main thread start..");

    CAQueueingThread_t *thread = (CAQueueingThread_t *) threadValue;

    if (thread == NULL)
    {
        OIC_LOG(ERROR, TAG, "thread data passing error!!");

        return;
    }

    while (!thread->isStop)
    {
        // mutex lock
        u_mutex_lock(thread->threadMutex);

        // if queue is empty, thread will wait
        if (u_queue_get_size(thread->dataQueue) <= 0)
        {
            OIC_LOG(DEBUG, TAG, "wait..");

            // wait
            u_cond_wait(thread->threadCond, thread->threadMutex);

            OIC_LOG(DEBUG, TAG, "wake up..");
        }

        // mutex unlock
        u_mutex_unlock(thread->threadMutex);

        // check stop flag
        if (thread->isStop)
        {
            continue;
        }

        // get data
        u_queue_message_t *message = u_queue_get_element(thread->dataQueue);
        if (message == NULL)
        {
            continue;
        }

        // process data
        thread->threadTask(message->msg);

        // free
        if (thread->destroy != NULL)
        {
            thread->destroy(message->msg, message->size);
        }
        else
        {
            OICFree(message->msg);
        }

        OICFree(message);
    }

    // remove all remained list data.
    while (u_queue_get_size(thread->dataQueue) > 0)
    {
        // get data
        u_queue_message_t *message = u_queue_get_element(thread->dataQueue);

        // free
        if(message != NULL) {
            if (thread->destroy != NULL)
            {
                thread->destroy(message->msg, message->size);
            }
            else
            {
                OICFree(message->msg);
            }

            OICFree(message);
        }
    }

    u_cond_signal(thread->threadCond);

    OIC_LOG(DEBUG, TAG, "message handler main thread end..");
}

CAResult_t CAQueueingThreadInitialize(CAQueueingThread_t *thread, u_thread_pool_t handle,
                                      CAThreadTask task, CADataDestroyFunction destroy)
{
    if (thread == NULL)
    {
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_FAILED;
    }

    if (handle == NULL)
    {
        OIC_LOG(ERROR, TAG, "thread pool handle is empty..");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "thread initialize..");

    // set send thread data
    thread->threadPool = handle;
    thread->dataQueue = u_queue_create();
    thread->threadMutex = u_mutex_new();
    thread->threadCond = u_cond_new();
    thread->isStop = true;
    thread->threadTask = task;
    thread->destroy = destroy;

    return CA_STATUS_OK;
}

CAResult_t CAQueueingThreadStart(CAQueueingThread_t *thread)
{
    if (thread == NULL)
    {
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_FAILED;
    }

    if (thread->threadPool == NULL)
    {
        OIC_LOG(ERROR, TAG, "thread pool handle is empty..");
        return CA_STATUS_FAILED;
    }

    if (false == thread->isStop) //Queueing thread already running
    {
        OIC_LOG(DEBUG, TAG, "queueing thread already running..");
        return CA_STATUS_OK;
    }

    // mutex lock
    u_mutex_lock(thread->threadMutex);
    thread->isStop = false;
    // mutex unlock
    u_mutex_unlock(thread->threadMutex);

    CAResult_t res = u_thread_pool_add_task(thread->threadPool, CAQueueingThreadBaseRoutine,
                                            thread);
    if (res != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, TAG, "thread pool add task error(send thread).");
    }

    return res;
}

CAResult_t CAQueueingThreadAddData(CAQueueingThread_t *thread, void *data, uint32_t size)
{
    if (thread == NULL)
    {
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_FAILED;
    }

    if (data == NULL || size == 0)
    {
        OIC_LOG(ERROR, TAG, "data is empty..");

        return CA_STATUS_FAILED;
    }

    // create thread data
    u_queue_message_t *message = (u_queue_message_t *) OICMalloc(sizeof(u_queue_message_t));

    if (message == NULL)
    {
        OIC_LOG(ERROR, TAG, "memory error!!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    message->msg = data;
    message->size = size;

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
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "thread destroy..");

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
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "thread stop request!!");

    if (!thread->isStop)
    {
        // mutex lock
        u_mutex_lock(thread->threadMutex);

        // set stop flag
        thread->isStop = true;

        // notify the thread
        u_cond_signal(thread->threadCond);

        u_cond_wait(thread->threadCond, thread->threadMutex);

        // mutex unlock
        u_mutex_unlock(thread->threadMutex);
    }

    return CA_STATUS_OK;
}
