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
 * This file provides APIs related to mutex and semaphores.
 */

#ifndef OC_THREAD_H_
#define OC_THREAD_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct ca_mutex_internal *ca_mutex;
typedef struct ca_cond_internal *ca_cond;
typedef struct ca_thread_internal *ca_thread;

/**
 * Enums for ca_cond_wait_for return values.
 */
typedef enum
{
   CA_WAIT_SUCCESS = 0,    /**< Condition Signal. */
   CA_WAIT_INVAL = -1,     /**< Invalid Condition. */
   CA_WAIT_TIMEDOUT = -2   /**< Condition Timed Out. */
} CAWaitResult_t;

typedef enum
{
    CA_THREAD_SUCCESS = 0,
    CA_THREAD_ALLOCATION_FAILURE = 1,
    CA_THREAD_CREATE_FAILURE=2,
    CA_THREAD_INVALID=3,
    CA_THREAD_WAIT_FAILURE=4,
    CA_THREAD_INVALID_PARAMETER=5
} CAThreadResult_t;

/**
 * Allocates, and starts a new thread
 *
 * @param[out] t  The thread that will refer to a newly allocated, and started thread
 * @param[in] start_routine The function that will execute in a new thread
 * @param[in] arg The information passed to the start_routine
 * @return CAThreadResult_t An enumeration of possible outcomes
 * @retval CA_THREAD_SUCCESS If a thread was successfully allocated and started.
 * @retval CA_THREAD_ALLOCATION_FAILURE If a thread was unable to be allocated
 * @retval CA_THREAD_CREATE_FAILURE If a thread was unable to be started
 *
 */
CAThreadResult_t ca_thread_new(ca_thread *t, void *(*start_routine)(void *), void *arg);

/**
 * Frees a thread previously allocated with ca_thread_new()
 *
 * @param[in] t The thread to be unallocated
 * @return CAThreadResult_t An enumeration of possible outcomes
 * @retval CA_THREAD_SUCCESS If a thread was successfully unallocated
 * @retval CA_THREAD_INVALID_PARAMETER If param t is NULL
 *
 */
CAThreadResult_t ca_thread_free(ca_thread t);

/**
 * Block until a thread's execution has been completed
 *
 * @param[in] t The thread to be waited on
 * @return CAThreadResult_t An enumeration of possible outcomes
 * @retval CA_THREAD_SUCCESS If the thread successfully completed execution
 * @retval CA_THREAD_WAIT_FAILURE If a problem occured while waiting for execution of the thread to complete
 *
 */
CAThreadResult_t ca_thread_wait(ca_thread t);

/**
 * Creates new mutex.
 *
 * @return  Reference to newly created mutex, otherwise NULL.
 *
 */
ca_mutex ca_mutex_new(void);

/**
 * Lock the mutex.
 *
 * @param  mutex  The mutex to be locked.
 *
 */
void ca_mutex_lock(ca_mutex mutex);

/**
 * Unlock the mutex.
 *
 * @param  mutex  The mutex to be unlocked.
 *
 */
void ca_mutex_unlock(ca_mutex mutex);

/**
 * Free the mutex.
 *
 * @param  mutex  The mutex to be freed.
 * @return bool to indicate success or failure
 * @retval true if mutex was freed successfully
 * @retval false if mutex parameter is invalid
 *
 */
bool ca_mutex_free(ca_mutex mutex);

/**
 * Creates new condition.
 *
 * @return  Reference to newly created ca_cond, otherwise NULL.
 *
 */
ca_cond ca_cond_new(void);

/**
 * One of threads is woken up if multiple threads are waiting for cond.
 *
 * @param  cond  The condtion to be signaled.
 *
 */
void ca_cond_signal(ca_cond cond);

/**
 * All of threads are woken up if multiple threads are waiting for cond.
 *
 * @param  cond  The condtion to be signaled.
 *
 */
void ca_cond_broadcast(ca_cond cond);

/**
 * Waits until this thread woken up on cond.
 *
 * @param  cond  The condtion to be wait for to signal.
 * @param  mutex  The mutex which is currently locked from calling thread.
 *
 */
void ca_cond_wait(ca_cond cond, ca_mutex mutex);

/**
 * Waits until this thread woken up on cond,
 * but not longer than the interval specified by microseconds.
 * The mutex is unlocked before falling asleep and locked again before resuming.
 * If microseconds is 0, ca_cond_wait_for() acts like ca_cond_wait().
 *
 * @param  cond  The condtion to be wait for to signal.
 * @param  mutex  The mutex which is currently locked from calling thread.
 * @param  microseconds  relative time for waiting, microseconds.
 *
 * @return ::CA_WAIT_SUCCESS if the condition was signaled,
 *         ::CA_WAIT_TIMEDOUT if wait period exceeded,
 *         ::CA_WAIT_INVAL for invalid parameters.
 *
 */
CAWaitResult_t ca_cond_wait_for(ca_cond cond, ca_mutex mutex, uint64_t microseconds);

/**
 * Free the condition.
 *
 * @param  cond  The condition to be freed.
 *
 */
void ca_cond_free(ca_cond cond);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* CA_MUTEX_H_ */
