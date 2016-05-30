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

void NSDestroyThreadHandle(NSThread *);

NSThread * NSThreadInit(NSThreadFunc func, void * data)
{
    if (!func)
    {
        NS_LOG(ERROR, "thread function is null");
        return NULL;
    }

    NSThread * handle = (NSThread *)OICMalloc(sizeof(NSThread));
    if (!handle)
    {
        NS_LOG(ERROR, "thread allocation fail");
        return NULL;
    }

    memset(handle, 0, sizeof(NSThread));

    pthread_mutexattr_init(&(handle->mutex_attr));
    if (pthread_mutexattr_settype(&(handle->mutex_attr), PTHREAD_MUTEX_RECURSIVE))
    {
        NS_LOG(ERROR, "thread mutex_attr init fail");
        NSDestroyThreadHandle(handle);
        return NULL;
    }

    if (pthread_mutex_init(&(handle->mutex), &(handle->mutex_attr)))
    {
        NS_LOG(ERROR, "thread mutex init fail");
        NSDestroyThreadHandle(handle);
        return NULL;
    }

    if (pthread_mutex_lock(&(handle->mutex)))
    {
        NS_LOG(ERROR, "thread mutex lock fail");
        NSDestroyThreadHandle(handle);
        return NULL;
    }

    handle->isStarted = true;

    if (pthread_create(&(handle->thread_id), NULL, func,
            (data == NULL) ? (void *) handle : (void *)data))
    {
        NS_LOG(ERROR, "thread create fail");
        NSDestroyThreadHandle(handle);
        return NULL;
    }

    pthread_mutex_unlock(&(handle->mutex));

    return handle;
}

void NSThreadLock(NSThread * handle)
{
    pthread_mutex_lock(&(handle->mutex));
}

void NSThreadUnlock(NSThread * handle)
{
    pthread_mutex_unlock(&(handle->mutex));
}

void NSThreadStop(NSThread * handle)
{
    NSDestroyThreadHandle(handle);
}

void NSThreadJoin(NSThread * handle)
{
	if (handle->thread_id)
	{
		pthread_join(handle->thread_id, NULL);
	}
}

void NSDestroyThreadHandle(NSThread * handle)
{
    handle->isStarted = false;

    NSThreadJoin(handle);

    pthread_mutex_destroy(&(handle->mutex));
    pthread_mutexattr_destroy(&(handle->mutex_attr));
}

