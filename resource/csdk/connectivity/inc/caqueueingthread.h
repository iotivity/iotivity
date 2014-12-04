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

/**
 * @file caqueueingthread.h
 */
#ifndef __CA_THREAD_H_
#define __CA_THREAD_H_

#include <stdint.h>

#include "uthreadpool.h"
#include "umutex.h"
#include "uqueue.h"
#include "cacommon.h"
#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*CAThreadTask)(void* threadData);

typedef struct
{
    u_thread_pool_t threadPool;
    u_mutex threadMutex;
    u_cond threadCond;
    CAThreadTask threadTask;
    CABool_t isStop;
    u_queue_t* dataQueue;
} CAQueueingThread_t;

CAResult_t CAQueueingThreadInitialize(CAQueueingThread_t* thread, u_thread_pool_t handle,
        CAThreadTask task);

CAResult_t CAQueueingThreadStart(CAQueueingThread_t* thread);

// shallow copy
CAResult_t CAQueueingThreadAddData(CAQueueingThread_t* thread, void* data, uint32_t size);

CAResult_t CAQueueingThreadStop(CAQueueingThread_t* thread);

CAResult_t CAQueueingThreadDestroy(CAQueueingThread_t* thread);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // __CA_THREAD_H_
