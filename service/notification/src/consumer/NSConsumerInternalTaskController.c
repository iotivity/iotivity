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

#include "NSConstants.h"
#include "NSConsumerCommon.h"
#include "NSConsumerInternalTaskController.h"
#include "NSStructs.h"

#include "oic_malloc.h"
#include "oic_string.h"

NSCacheList ** NSGetCacheList()
{
    static NSCacheList * cache = NULL;
    return & cache;
}

void NSSetCacheList(NSCacheList * cache)
{
    *(NSGetCacheList()) = cache;
}

void NSDestroyCacheList()
{
    NSCacheList * cache = *(NSGetCacheList());
    if (cache)
    {
        NSStorageDestroy(cache);
    }
}

NSResult NSCacheUpdate(NSCacheList * cache, NSTask * task, NSConsumerMessageTypes type)
{
    NSMessage_consumer * noti = (NSMessage_consumer *) task->taskData;
    NS_VERIFY_NOT_NULL(noti, NS_ERROR);

    noti->type = type;

    NSCacheElement * obj = (NSCacheElement *)OICMalloc(sizeof(NSCacheElement));
    NS_VERIFY_NOT_NULL(obj, NS_ERROR);

    obj->data = (NSCacheData *) noti;
    obj->next = NULL;

    NS_LOG(DEBUG, "try to write to storage");
    NSResult ret = NSStorageWrite(cache, obj);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(ret == NS_OK ? (void *) 1 : NULL,
            NS_ERROR, NSRemoveMessage(noti));

    NSRemoveMessage(noti);
    NSOICFree(obj);

    return NS_OK;
}

void NSConsumerHandleProviderDiscovered(NSProvider_internal * provider)
{
    // TODO need to check for discovered provider is new and store provider.

    if (provider->accessPolicy == NS_ACCESS_DENY)
    {
        NS_LOG(DEBUG, "accepter is NS_ACCEPTER_CONSUMER, Callback to user");
        NSDiscoveredProvider((NSProvider *) provider);
    }
    else
    {
        NS_LOG(DEBUG, "accepter is NS_ACCEPTER_PROVIDER, request subscribe");
        NSTask * task = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE, (void *) provider);
        NS_VERIFY_NOT_NULL_V(task);

        NSConsumerPushEvent(task);
    }
}

void NSConsumerHandleRecvSubscriptionConfirmed(NSMessage_consumer * msg)
{
    NS_VERIFY_NOT_NULL_V(msg);

    // TODO change to find provider using pId.
    NSProvider * provider = NULL;
    {
        provider = (NSProvider *)OICMalloc(sizeof(NSProvider));
        NS_VERIFY_NOT_NULL_V(provider);
        OICStrcpy(provider->providerId, sizeof(char) * NS_DEVICE_ID_LENGTH, msg->providerId);
    }

    NSSubscriptionAccepted(provider);
}

void NSConsumerHandleRecvMessage(NSMessage_consumer * msg)
{
    NS_VERIFY_NOT_NULL_V(msg);
    // TODO store message to cache
//    NSResult ret = NSCacheUpdate(cache, task, Notification);
//    NS_VERTIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);

    NSMessagePost((NSMessage *) msg);
}

void NSConsumerHandleRecvSyncInfo(NSSyncInfo * sync)
{
    NS_VERIFY_NOT_NULL_V(sync);

    // TODO need to check for provider is available.
    // TODO need to update msg list.
//        NSResult ret = NSPushToCache(clientResponse, taskType);
//        NS_VERTIFY_NOT_NULL(ret == NS_OK ? (void *)1 : NULL, OC_STACK_KEEP_TRANSACTION);

    NSNotificationSync(sync);
}

void NSConsumerInternalTaskProcessing(NSTask * task)
{
    NS_VERIFY_NOT_NULL_V(task);

    NSCacheList * cache = *(NSGetCacheList());
    if (!cache)
    {
        NS_LOG(DEBUG, "Cache Init");
        cache = NSStorageCreate();
        NS_VERIFY_NOT_NULL_V(cache);

        cache->cacheType = NS_CONSUMER_CACHE_MESSAGE;
        NSSetCacheList(cache);
    }

    NSResult ret = NS_ERROR;
    NS_LOG_V(DEBUG, "Receive Event : %d", (int)task->taskType);
    switch (task->taskType)
    {
        case TASK_CONSUMER_RECV_SUBSCRIBE_CONFIRMED:
        {
            NS_LOG(DEBUG, "Receive Subscribe confirm from provider.");
            NSConsumerHandleRecvSubscriptionConfirmed((NSMessage_consumer *)task->taskData);
            break;
        }
        case TASK_CONSUMER_RECV_MESSAGE:
        {
            NS_LOG(DEBUG, "Receive New Notification");
            NSConsumerHandleRecvMessage((NSMessage_consumer *)task->taskData);

            break;
        }
        case TASK_CONSUMER_PROVIDER_DISCOVERED:
        {
            NS_LOG(DEBUG, "Receive New Provider is discovdered.");
            NSConsumerHandleProviderDiscovered((NSProvider_internal *)task->taskData);
            break;
        }
        case TASK_RECV_SYNCINFO:
        {
            NS_LOG(DEBUG, "Receive SyncInfo.");
            NSConsumerHandleRecvSyncInfo((NSSyncInfo *)task->taskData);
            break;
        }
        case TASK_RECV_READ:
        {
            NS_LOG(DEBUG, "Receive Read Notification");

            ret = NSCacheUpdate(cache, task, Read);
            NS_VERIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);
            break;
        }
        case TASK_RECV_DISMISS:
        {
            NS_LOG(DEBUG, "Receive Dismiss Notification");

            ret = NSCacheUpdate(cache, task, Dismiss);
            NS_VERIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);
            break;
        }
        default :
        {
            NS_LOG(ERROR, "Unknown TASK Type");
            return ;
        }
    }
}
