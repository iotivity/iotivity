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

static pthread_mutex_t g_create_mutex;

NSConsumerThread * NSThreadInit(NSThreadFunc func, void * data)
{
    NS_VERIFY_NOT_NULL(func, NULL);

    pthread_mutex_init(&g_create_mutex, NULL);

    NSConsumerThread * handle = (NSConsumerThread *)OICMalloc(sizeof(NSConsumerThread));
    NS_VERIFY_NOT_NULL(handle, NULL);

    memset(handle, 0, sizeof(NSConsumerThread));

    pthread_mutexattr_init(&(handle->mutex_attr));

    int pthreadResult = pthread_mutexattr_settype(&(handle->mutex_attr), PTHREAD_MUTEX_RECURSIVE);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(pthreadResult == 0 ? (void *)1 : NULL,
            NULL, NSDestroyThreadHandle(handle));

    pthreadResult = pthread_mutex_init(&(handle->mutex), &(handle->mutex_attr));
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(pthreadResult == 0 ? (void *)1 : NULL,
            NULL, NSDestroyThreadHandle(handle));

    pthread_mutex_lock(&g_create_mutex);

    handle->isStarted = true;

    pthread_attr_t attrDetached = {};
    pthread_attr_init(& attrDetached);
    pthread_attr_setdetachstate(& attrDetached, PTHREAD_CREATE_DETACHED);

    pthreadResult = pthread_create(&(handle->thread_id), & attrDetached, func,
                           (data == NULL) ? (void *) handle : (void *)data);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(pthreadResult == 0 ? (void *)1 : NULL,
            NULL, NSDestroyThreadHandle(handle));

    pthread_attr_destroy(& attrDetached);

    pthread_mutex_unlock(&g_create_mutex);

    return handle;
}

void NSThreadLock(NSConsumerThread * handle)
{
    NS_VERIFY_NOT_NULL_V(handle);

    pthread_mutex_lock(&(handle->mutex));
}

void NSThreadUnlock(NSConsumerThread * handle)
{
    NS_VERIFY_NOT_NULL_V(handle);

    pthread_mutex_unlock(&(handle->mutex));
}

void NSThreadStop(NSConsumerThread * handle)
{
    NS_VERIFY_NOT_NULL_V(handle);

    handle->isStarted = false;
    NSThreadJoin(handle);

    NSDestroyThreadHandle(handle);
}

void NSThreadJoin(NSConsumerThread * handle)
{
    NS_VERIFY_NOT_NULL_V(handle);

    if (handle->thread_id)
    {
        void * retData = NULL;
        pthread_join(handle->thread_id, & retData);
        NSOICFree(retData);
    }
}

void NSDestroyThreadHandle(NSConsumerThread * handle)
{
    NS_VERIFY_NOT_NULL_V(handle);

    pthread_mutex_destroy(&(handle->mutex));
    pthread_mutexattr_destroy(&(handle->mutex_attr));

    pthread_mutex_unlock(&g_create_mutex);
}

void NSThreadDetach()
{
    pthread_detach(pthread_self());
}
