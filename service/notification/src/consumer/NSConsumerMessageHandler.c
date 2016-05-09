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

#include "NSConsumerMessageHandler.h"

#include <stdlib.h>
#include <stdbool.h>

#include "oic_malloc.h"

#include "NSStructs.h"
#include "NSConsumerCommon.h"

#include "NSThread.h"
#include "NSConsumerQueue.h"

#include "NSConsumerListener.h"
#include "NSConsumerDiscovery.h"
#include "NSConsumerSystem.h"
#include "NSConsumerNotification.h"
#include "NSConsumerSubsription.h"

void * NSConsumerMsgHandleThreadFunc(void * handle);

void * NSConsumerMsgPushThreadFunc(void * data);

void NSConsumerHandleMsg(NSTask * task);

NSThreadHandle ** NSGetMsgHandleThreadHandle()
{
    static NSThreadHandle * handle = NULL;
    return & handle;
}

void NSSetMsgHandleThreadHandle(NSThreadHandle * handle)
{
   *(NSGetMsgHandleThreadHandle()) = handle;
}

NSConsumerQueue ** NSGetMsgHandleQueue()
{
    static NSConsumerQueue * queue = NULL;
    return & queue;
}

void NSSetMsgHandleQueue(NSConsumerQueue * queue)
{
   *(NSGetMsgHandleQueue()) = queue;
}

NSResult NSConsumerMessageHandlerInit()
{
    NSThreadHandle * handle = NULL;
    NSConsumerQueue * queue = NULL;

    if (NS_OK != NSConsumerListenerInit())
    {
        NS_CONSUMER_LOG(ERROR, "listener init fail");
        return NS_ERROR;
    }

    if (NS_OK != NSConsumerSystemInit())
    {
        NS_CONSUMER_LOG(ERROR, "system init fail");
        return NS_ERROR;
    }

    handle = NSThreadInit(NSConsumerMsgHandleThreadFunc, NULL);
    if (!handle)
    {
        NS_CONSUMER_LOG(ERROR, "msg handle thread init fail");
        return NS_ERROR;
    }
    NSSetMsgHandleThreadHandle(handle);

    queue = NSCreateQueue();
    if (!queue)
    {
        NS_CONSUMER_LOG(ERROR, "msg handle queue create fail");
        return NS_ERROR;
    }
    NSSetMsgHandleQueue(queue);

    return NS_OK;
}

NSResult NSConsumerPushEvent(NSTask * task)
{
    if (!NSThreadInit(NSConsumerMsgPushThreadFunc, (void *) task))
    {
        NS_CONSUMER_LOG(ERROR, "insert to queue thread init fail");
        return NS_ERROR;
    }

    return NS_OK;
}

void NSConsumerMessageHandlerExit()
{
    NSThreadStop(*(NSGetMsgHandleThreadHandle()));
}

void * NSConsumerMsgHandleThreadFunc(void * threadHandle)
{
    NSConsumerQueue * queue = NULL;
    NSConsumerQueueObject * obj = NULL;
    NSThreadHandle * handle = (NSThreadHandle *) threadHandle;
    NS_CONSUMER_LOG(DEBUG, "created thread for consumer message handle");
    if (!handle)
    {
        NS_CONSUMER_LOG(ERROR, "thread handle is null");
        return NULL;
    }

    while (true)
    {
        if (!handle->isStarted)
        {
            NS_CONSUMER_LOG(ERROR, "msg handler thread will be terminated");
            break;
        }

        queue = *(NSGetMsgHandleQueue());
        if (!queue)
        {
            continue;
        }

        if (NSIsEmptyQueue(queue))
        {
            continue;
        }

        NSThreadLock(handle);
        NS_CONSUMER_LOG(DEBUG, "msg handler working");
        obj = NSPopQueue(queue);
        // TODO processing obj

        NSConsumerHandleMsg((NSTask *)(obj->data));

        NSThreadUnlock(handle);

    }

    return NULL;
}

void * NSConsumerMsgPushThreadFunc(void * data)
{
    NSConsumerQueueObject * obj = NULL;
    NSConsumerQueue * queue = NULL;
    NSThreadHandle * handle = *(NSGetMsgHandleThreadHandle());
    if (!handle)
    {
        NS_CONSUMER_LOG(ERROR, "NSThreadHandle is null. can not insert to queue");
        return NULL;
    }

    obj = (NSConsumerQueueObject *)OICMalloc(sizeof(NSConsumerQueueObject));
    if (!obj)
    {
        NS_CONSUMER_LOG(ERROR, "NSConsumerQueueObject allocation fail");
        return NULL;
    }

    obj->data = data;
    obj->next = NULL;

    NSThreadLock(handle);

    queue = *(NSGetMsgHandleQueue());
    if (!queue)
    {
        NS_CONSUMER_LOG(ERROR, "NSQueue is null. can not insert to queue");
        OICFree(data);
        OICFree(obj);
    }
    else
    {
        NSPushQueue(queue, obj);
    }

    NSThreadUnlock(handle);

    return NULL;
}

void NSConsumerHandleMsg(NSTask * task)
{
    switch (task->taskType)
    {
    case TASK_EVENT_CONNECTED:
    case TASK_CONSUMER_REQ_DISCOVER:
    {
        NSConsumerDiscoveryHandleMsg(task);
        break;
    }
    case TASK_CONSUMER_REQ_SUBSCRIBE:
    case TASK_SEND_READ:
    case TASK_SEND_DISMISS:
    {
        NSConsumerNotificationHandleMsg(task);
        break;
    }
    case TASK_RECV_READ:
    case TASK_RECV_DISMISS:
    case TASK_CONSUMER_RECV_NOTIFICATION:
    {
        NSConsumerSubscriptionHandleMsg(task);
        break;
    }
    default:
        NS_CONSUMER_LOG(ERROR, "Unknown type of task");
        break;
    }
}
