/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef _NS_THREAD_H_
#define _NS_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdbool.h>

typedef enum
{
    pthread
} NS_THREAD_MODEL;

#define NS_THREAD pthread

#if (NS_THREAD == pthread)
#include <pthread.h>

typedef struct
{
    bool isStarted;                 /**< check started or not */
    pthread_t thread_id;            /**< thread Id */
    pthread_mutex_t mutex;          /**< mutex variable */
    pthread_mutexattr_t mutex_attr; /**< mutex attribute variable */
} NSConsumerThread;

#endif

/** callback function for notification thread */
typedef void *(*NSThreadFunc)(void *);

/**
 * Initialize thread
 * @param[in] func   callback function
 * @param[in] data   data
 * @return handle of notification consumer thread
 */
NSConsumerThread * NSThreadInit(NSThreadFunc func, void * data);

/**
 * lock thread
 * @param[in] handle   consumer thread handle
 */
void NSThreadLock(NSConsumerThread * handle);

/**
 * unlock thread
 * @param[in] handle   consumer thread handle
 */
void NSThreadUnlock(NSConsumerThread * handle);

/**
 * join thread
 * @param[in] handle   consumer thread handle
 */
void NSThreadJoin(NSConsumerThread * handle);

/**
 * stop thread
 * @param[in] handle   consumer thread handle
 */
void NSThreadStop(NSConsumerThread * handle);

/**
 * destroy thread handle
 * @param[in] handle   consumer thread handle
 */
void NSDestroyThreadHandle(NSConsumerThread * handle);

/**
 * thread detach
 */
void NSThreadDetach(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NS_THREAD_H_
