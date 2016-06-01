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

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "oic_malloc.h"

#include "NSStructs.h"
#include "NSConstants.h"
#include "NSConsumerCommon.h"
#include "NSConsumerCommunication.h"

#include "NSThread.h"
#include "NSConsumerQueue.h"

#include "NSConsumerDiscovery.h"
#include "NSConsumerInternalTaskController.h"
#include "NSConsumerNetworkEventListener.h"
#include "NSConsumerQueueScheduler.h"
#include "NSConsumerSystem.h"

void * NSConsumerMsgHandleThreadFunc(void * handle);

void * NSConsumerMsgPushThreadFunc(void * data);

void NSConsumerTaskProcessing(NSTask * task);

NSConsumerThread ** NSGetMsgHandleThreadHandle()
{
    static NSConsumerThread * handle = NULL;
    return & handle;
}

void NSSetMsgHandleThreadHandle(NSConsumerThread * handle)
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
    NSConsumerThread * handle = NULL;
    NSConsumerQueue * queue = NULL;

    if (NS_OK != NSConsumerListenerInit())
    {
        NS_LOG(ERROR, "listener init fail");
        return NS_ERROR;
    }

    if (NS_OK != NSConsumerSystemInit())
    {
        NS_LOG(ERROR, "system init fail");
        return NS_ERROR;
    }

    handle = NSThreadInit(NSConsumerMsgHandleThreadFunc, NULL);
    if (!handle)
    {
        NS_LOG(ERROR, "msg handle thread init fail");
        return NS_ERROR;
    }
    NSSetMsgHandleThreadHandle(handle);

    queue = NSCreateQueue();
    if (!queue)
    {
        NS_LOG(ERROR, "msg handle queue create fail");
        return NS_ERROR;
    }
    NSSetMsgHandleQueue(queue);

    return NS_OK;
}

NSResult NSConsumerPushEvent(NSTask * task)
{
    if (!NSThreadInit(NSConsumerMsgPushThreadFunc, (void *) task))
    {
        NS_LOG(ERROR, "insert to queue thread init fail");
        return NS_ERROR;
    }

    return NS_OK;
}

void NSConsumerMessageHandlerExit()
{
    NSDestroyCacheList();
    NSThreadStop(*(NSGetMsgHandleThreadHandle()));
}

void * NSConsumerMsgHandleThreadFunc(void * threadHandle)
{
    NSConsumerQueue * queue = NULL;
    NSConsumerQueueObject * obj = NULL;
    NSConsumerThread * queueHandleThread = (NSConsumerThread *) threadHandle;
    NS_LOG(DEBUG, "created thread for consumer message handle");
    if (!queueHandleThread)
    {
        NS_LOG(ERROR, "thread handle is null");
        return NULL;
    }

    while (true)
    {
        if (!queueHandleThread->isStarted)
        {
            NS_LOG(ERROR, "msg handler thread will be terminated");
            break;
        }

        queue = *(NSGetMsgHandleQueue());
        if (!queue)
        {
            continue;
        }

        if (NSIsQueueEmpty(queue))
        {
            usleep(2000);
            continue;
        }

        NSThreadLock(queueHandleThread);
        NS_LOG(DEBUG, "msg handler working");
        obj = NSPopQueue(queue);

        if (obj)
        {
            NSConsumerTaskProcessing((NSTask *)(obj->data));
        }

        NSThreadUnlock(queueHandleThread);

    }

    return NULL;
}

void * NSConsumerMsgPushThreadFunc(void * data)
{
    NSConsumerQueueObject * obj = NULL;
    NSConsumerQueue * queue = NULL;
    NSConsumerThread * msgHandleThread = *(NSGetMsgHandleThreadHandle());
    if (!msgHandleThread)
    {
        NS_LOG(ERROR, "NSThreadHandle is null. can not insert to queue");
        return NULL;
    }

    obj = (NSConsumerQueueObject *)OICMalloc(sizeof(NSConsumerQueueObject));
    if (!obj)
    {
        NS_LOG(ERROR, "NSConsumerQueueObject allocation fail");
        return NULL;
    }

    obj->data = data;
    obj->next = NULL;

    NSThreadLock(msgHandleThread);

    queue = *(NSGetMsgHandleQueue());
    if (!queue)
    {
        NS_LOG(ERROR, "NSQueue is null. can not insert to queue");
        OICFree(data);
        OICFree(obj);
    }
    else
    {
        NSPushQueue(queue, obj);
    }

    NSThreadUnlock(msgHandleThread);

    return NULL;
}

void NSConsumerTaskProcessing(NSTask * task)
{
    switch (task->taskType)
    {
    case TASK_EVENT_CONNECTED:
    case TASK_CONSUMER_REQ_DISCOVER:
    {
        NSConsumerDiscoveryTaskProcessing(task);
        break;
    }
    case TASK_CONSUMER_REQ_SUBSCRIBE:
    case TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL:
    case TASK_SEND_READ:
    case TASK_SEND_DISMISS:
    {
        NSConsumerNotificationTaskProcessing(task);
        break;
    }
    case TASK_RECV_READ:
    case TASK_RECV_DISMISS:
    case TASK_CONSUMER_RECV_NOTIFICATION:
    {
        NSConsumerSubscriptionTaskProcessing(task);
        break;
    }
    default:
        NS_LOG(ERROR, "Unknown type of task");
        break;
    }
}
