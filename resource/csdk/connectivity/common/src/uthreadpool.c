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
 * @file    uthreadpool.c
 * @brief   This file provides APIs related to thread pool
 */

#include "uthreadpool.h"
#include "logger.h"
#include "oic_malloc.h"

#define TAG PCF("UTHREADPOOL")

/**
 * @var gThreadpool
 * @brief Glib thread pool.
 */
static GThreadPool *gThreadpool = NULL;

/**
 * @fn run
 * @brief function which is registed to glib thread pool.
 */
static void run(void *thread_data, void *user_data);

CAResult_t u_thread_pool_init(uint32_t num_of_threads, u_thread_pool_t *thread_pool)
{
    OIC_LOG(DEBUG, TAG, "IN");

    GError *error = NULL;

#ifdef __ANDROID__
    //If not intialized, gthreadpool intialize check fails
    g_thread_init(NULL);
#endif //__ANDROID__

    gThreadpool = g_thread_pool_new(run, NULL, num_of_threads, TRUE, &error);
    if (NULL == gThreadpool)
    {
        OIC_LOG(ERROR, TAG, "g_thread_pool_new failed!");
        if (NULL != error)
        {
            OIC_LOG_V(ERROR, TAG, "Error is: %s", error->message);
            g_error_free(error);
        }
        return CA_STATUS_FAILED;
    }

    *thread_pool = (u_thread_pool_t) gThreadpool;

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t u_thread_pool_add_task(u_thread_pool_t thread_pool, void (*routine)(void *), 
                                    void *data)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == routine)
    {
        OIC_LOG(ERROR, TAG, "routine is NULL!");
        return CA_STATUS_FAILED;
    }

    u_thread_msg_t *message = (u_thread_msg_t *) OICMalloc(sizeof(u_thread_msg_t));
    if (NULL == message)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    message->data = data;
    message->func = routine;

    g_thread_pool_push((GThreadPool *) thread_pool, (void *) message, NULL);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void u_thread_pool_free(u_thread_pool_t thread_pool)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == thread_pool)
    {
        OIC_LOG(DEBUG, TAG, "thread_pool is NULL. Its already freed.");
        return;
    }

    GThreadPool *threadpool = (GThreadPool *) thread_pool;
    g_thread_pool_free(threadpool, TRUE, TRUE);

    OIC_LOG(DEBUG, TAG, "OUT");
}

void run(void *thread_data, void *user_data)
{
    u_thread_msg_t *message = (u_thread_msg_t *) thread_data;

    if (NULL == message)
    {
        OIC_LOG(ERROR, TAG, "Invalid task data");
        return;
    }

    if(message->func)
    {
        OIC_LOG(DEBUG, TAG, "Calling routine with data as parameter");
        message->func(message->data);
    }

    // Free message
    OICFree(message);
}
