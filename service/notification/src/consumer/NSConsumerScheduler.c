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

#include "NSConsumerScheduler.h"

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "oic_malloc.h"
#include "oic_string.h"
#include "ocrandom.h"

#include "NSStructs.h"
#include "NSConstants.h"
#include "NSConsumerCommon.h"
#include "NSConsumerCommunication.h"

#include "NSThread.h"
#include "NSConsumerQueue.h"

#include "NSConsumerDiscovery.h"
#include "NSConsumerInternalTaskController.h"
#include "NSConsumerNetworkEventListener.h"
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

    uint8_t uuid[UUID_SIZE];
    char uuidString[UUID_STRING_SIZE];
    OCGenerateUuid(uuid);
    OCConvertUuidToString(uuid, uuidString);
    NSSetConsumerId(uuidString);
    NS_LOG_V(DEBUG, "Consumer ID : %s", *NSGetConsumerId());

    NS_LOG(DEBUG, "listener init");
    NSResult ret = NSConsumerListenerInit();
    NS_VERIFY_NOT_NULL(ret == NS_OK ? (void *) 1 : NULL, NS_ERROR);

    NS_LOG(DEBUG, "system init");
    ret = NSConsumerSystemInit();
    NS_VERIFY_NOT_NULL(ret == NS_OK ? (void *) 1 : NULL, NS_ERROR);

    NS_LOG(DEBUG, "queue thread init");
    handle = NSThreadInit(NSConsumerMsgHandleThreadFunc, NULL);
    NS_VERIFY_NOT_NULL(handle, NS_ERROR);
    NSSetMsgHandleThreadHandle(handle);

    NS_LOG(DEBUG, "create queue");
    queue = NSCreateQueue();
    NS_VERIFY_NOT_NULL(queue, NS_ERROR);
    NSSetMsgHandleQueue(queue);

    return NS_OK;
}

NSResult NSConsumerPushEvent(NSTask * task)
{
    NSConsumerThread * thread = NSThreadInit(NSConsumerMsgPushThreadFunc, (void *) task);
    NS_VERIFY_NOT_NULL(thread, NS_ERROR);

    return NS_OK;
}

void NSConsumerMessageHandlerExit()
{

    NSConsumerListenerTermiate();
    NSCancelAllSubscription();
    NSThreadStop(*(NSGetMsgHandleThreadHandle()));
    NSDestroyQueue(*(NSGetMsgHandleQueue()));
    NSSetMsgHandleQueue(NULL);

    NSDestroyMessageCacheList();
    NSDestroyProviderCacheList();
}

void * NSConsumerMsgHandleThreadFunc(void * threadHandle)
{
    NSConsumerQueue * queue = *(NSGetMsgHandleQueue());;
    NSConsumerQueueObject * obj = NULL;

    NS_LOG(DEBUG, "create thread for consumer message handle");
    NSConsumerThread * queueHandleThread = (NSConsumerThread *) threadHandle;
    NS_VERIFY_NOT_NULL(queueHandleThread, NULL);

    while (true)
    {
        if (!queue)
        {
            queue = *(NSGetMsgHandleQueue());
            usleep(2000);
            continue;
        }

        if (!queueHandleThread->isStarted && NSIsQueueEmpty(queue))
        {
            NS_LOG(ERROR, "msg handler thread will be terminated");
            break;
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

    NS_LOG(DEBUG, "get queueThread handle");
    NSConsumerThread * msgHandleThread = *(NSGetMsgHandleThreadHandle());
    NS_VERIFY_NOT_NULL(msgHandleThread, NULL);

    NS_LOG(DEBUG, "create queue object");
    obj = (NSConsumerQueueObject *)OICMalloc(sizeof(NSConsumerQueueObject));
    NS_VERIFY_NOT_NULL(obj, NULL);

    obj->data = data;
    obj->next = NULL;

    NSThreadLock(msgHandleThread);

    queue = *(NSGetMsgHandleQueue());
    if (!queue)
    {
        NS_LOG(ERROR, "NSQueue is null. can not insert to queue");
        NSOICFree(data);
        NSOICFree(obj);
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
    case TASK_EVENT_CONNECTED_TCP:
    case TASK_CONSUMER_REQ_DISCOVER:
    {
        NSConsumerDiscoveryTaskProcessing(task);
        break;
    }
    case TASK_CONSUMER_REQ_SUBSCRIBE:
    case TASK_SEND_SYNCINFO:
    {
        NSConsumerCommunicationTaskProcessing(task);
        break;
    }
    case TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL:
    {
        NSProvider_internal * data = NSCopyProvider((NSProvider_internal *)task->taskData);
        NS_VERIFY_NOT_NULL_V(data);
        NSTask * conTask = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL, data);
        NS_VERIFY_NOT_NULL_V(conTask);
        NSConsumerCommunicationTaskProcessing(task);
        NSConsumerInternalTaskProcessing(conTask);
        break;
    }
    case TASK_RECV_SYNCINFO:
    case TASK_CONSUMER_RECV_MESSAGE:
    case TASK_CONSUMER_PROVIDER_DISCOVERED:
    case TASK_CONSUMER_RECV_SUBSCRIBE_CONFIRMED:
    case TASK_MAKE_SYNCINFO:
    {
        NSConsumerInternalTaskProcessing(task);
        break;
    }
    default:
        NS_LOG(ERROR, "Unknown type of task");
        break;
    }
}

NSMessage * NSConsumerFindNSMessage(const char* messageId)
{
    NS_VERIFY_NOT_NULL(messageId, NULL);

    return NSMessageCacheFind(messageId);
}

NSProvider_internal * NSConsumerFindNSProvider(const char * providerId)
{
    NS_VERIFY_NOT_NULL(providerId, NULL);

    return NSProviderCacheFind(providerId);
}
