//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "NSThread.h"

#include "NSConstants.h"
#include "NSConsumerCommon.h"

#include <memory.h>
#include "oic_malloc.h"

#ifndef SYSTEM_WINDOWS
#include <pthread.h>

typedef struct
{
    pthread_mutex_t mutex;
} oc_mutex_ns_internal;
#endif

NSConsumerThread * NSThreadInit(NSThreadFunc func, void * data)
{
    NS_VERIFY_NOT_NULL(func, NULL);

    NSConsumerThread * handle = (NSConsumerThread *)OICMalloc(sizeof(NSConsumerThread));
    NS_VERIFY_NOT_NULL(handle, NULL);

    memset(handle, 0, sizeof(NSConsumerThread));

#ifndef SYSTEM_WINDOWS
    oc_mutex_ns_internal * mutexInfo
        = (oc_mutex_ns_internal *) OICMalloc(sizeof(oc_mutex_ns_internal));
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(mutexInfo, NULL, NSDestroyThreadHandle(handle));

    pthread_mutexattr_t * mutex_attr
        = (pthread_mutexattr_t *) OICMalloc(sizeof(pthread_mutexattr_t));
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(mutex_attr, NULL,
              {
                  NSOICFree(mutexInfo);
                  NSDestroyThreadHandle(handle));
              }
    int pthreadResult = pthread_mutexattr_settype(mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(pthreadResult == 0 ? (void *)1 : NULL, NULL,
            {
                    NSOICFree(mutexInfo);
                    NSOICFree(mutex_attr);
                    NSDestroyThreadHandle(handle);
            });

    pthreadResult = pthread_mutex_init(& (mutexInfo->mutex), mutex_attr);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(pthreadResult == 0 ? (void *)1 : NULL, NULL,
            {
                    NSOICFree(mutexInfo);
                    pthread_mutexattr_destroy(mutex_attr);
                    NSOICFree(mutex_attr);
                    NSDestroyThreadHandle(handle);
            });
    pthread_mutexattr_destroy(mutex_attr);
    NSOICFree(mutex_attr);
    handle->mutex = (oc_mutex) mutexInfo;
#else
    handle->mutex = oc_mutex_new();
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(handle->mutex, NULL, NSDestroyThreadHandle(handle));
#endif

    handle->isStarted = true;

    OCThreadResult_t ret = oc_thread_new(& handle->thread_id, func,
                             (data == NULL) ? (void *) handle : (void *)data);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(ret == OC_THREAD_SUCCESS ? (void*)1 : NULL, NULL,
            NSDestroyThreadHandle(handle));

    return handle;
}

void NSThreadLock(NSConsumerThread * handle)
{
    NS_VERIFY_NOT_NULL_V(handle);

    oc_mutex_lock(handle->mutex);
}

void NSThreadUnlock(NSConsumerThread * handle)
{
    NS_VERIFY_NOT_NULL_V(handle);

    oc_mutex_unlock(handle->mutex);
}

void NSThreadJoin(NSConsumerThread * handle)
{
    NS_VERIFY_NOT_NULL_V(handle);

    if (handle->isStarted)
    {
        oc_thread_wait(handle->thread_id);
    }
}

void NSThreadStop(NSConsumerThread * handle)
{
    NS_VERIFY_NOT_NULL_V(handle);

    handle->isStarted = false;
    NSThreadJoin(handle);

    NSDestroyThreadHandle(handle);
}

void NSDestroyThreadHandle(NSConsumerThread * handle)
{
    NS_VERIFY_NOT_NULL_V(handle);

    oc_mutex_free(handle->mutex);
}
