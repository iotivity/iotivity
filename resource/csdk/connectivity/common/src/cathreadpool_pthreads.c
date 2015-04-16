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
 *
 * This file provides APIs related to thread pool.
 */

#include <pthread.h>
#include "cathreadpool.h"
#include "logger.h"
#include "oic_malloc.h"

#define TAG PCF("UTHREADPOOL")

/**
 * empty struct to represent the details.  This implementation has no data
 * that it needs to keep track of, so it only uses NULL for the internal value.
 */
typedef struct ca_thread_pool_details_t
{
} ca_thread_pool_details_t;

/**
 * struct to wrap the pthreads callback properly.  The function pointer for
 * pthreads requires a void* return value, however u_thread_func is a void.
 */
typedef struct ca_thread_pool_callback_info_t
{
    ca_thread_func func;
    void* data;
} ca_thread_pool_callback_info_t;

// passthrough function to convert the pthreads call to a u_thread_func call
void* ca_thread_pool_pthreads_delegate(void* data)
{
    ca_thread_pool_callback_info_t* info = (ca_thread_pool_callback_info_t*)data;
    info->func(info->data);
    OICFree(info);
    return NULL;
}

// this implementation doesn't do a thread pool, so this function is essentially
// a no-op besides creating a valid ca_thread_pool_t object.  It was determined after
// reading through the existing implementation that the thread-pooling was unnecessary
// for the posix platforms.  Behavior shouldn't be changed since previously num_of_threads
// was greater than the number of requested threads.
CAResult_t ca_thread_pool_init(int32_t num_of_threads, ca_thread_pool_t *thread_pool)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if(!thread_pool)
    {
        OIC_LOG(ERROR, TAG, "Parameter thraed_pool was null!");
        return CA_STATUS_INVALID_PARAM;
    }

    if(num_of_threads <= 0)
    {
        OIC_LOG(ERROR, TAG, "num_of_threads must be positive and non-zero");
        return CA_STATUS_INVALID_PARAM;
    }

    *thread_pool = OICMalloc(sizeof(struct ca_thread_pool));

    if(!*thread_pool)
    {
        OIC_LOG(ERROR, TAG, "Failed to allocate for thread-pool");
        return CA_MEMORY_ALLOC_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t ca_thread_pool_add_task(ca_thread_pool_t thread_pool, ca_thread_func method,
                                    void *data)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if(NULL == thread_pool || NULL == method)
    {
        OIC_LOG(ERROR, TAG, "thread_pool or method was NULL");
        return CA_STATUS_INVALID_PARAM;
    }

    ca_thread_pool_callback_info_t* info = OICMalloc(sizeof(ca_thread_pool_callback_info_t));
    if(!info)
    {
        OIC_LOG(ERROR, TAG, "Failed to allocate for memory wrapper");
        return CA_MEMORY_ALLOC_FAILED;
    }

    info->func = method;
    info->data = data;

    pthread_t threadHandle;

    int result = pthread_create(&threadHandle, NULL, ca_thread_pool_pthreads_delegate, info);

    if(result != 0)
    {
        OIC_LOG_V(ERROR, TAG, "Thread start failed with error %d", result);
        return CA_STATUS_FAILED;
    }

    // detach will cause the thread to either terminate normally and clean up after
    // itself, which prevents us from having to do any manual join/cleanup later, or
    // it will be terminated upon application exit.
    result = pthread_detach(threadHandle);

    if(result != 0)
    {
        OIC_LOG_V(ERROR, TAG, "Thread detach failed with error %d", result);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void ca_thread_pool_free(ca_thread_pool_t thread_pool)
{
    OIC_LOG(DEBUG, TAG, "IN");

    OICFree(thread_pool);
    OIC_LOG(DEBUG, TAG, "OUT");
}
