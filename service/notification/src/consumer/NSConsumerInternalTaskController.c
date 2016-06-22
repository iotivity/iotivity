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

NSCacheList ** NSGetMessageCacheList()
{
    static NSCacheList * messageCache = NULL;
    return & messageCache;
}

void NSSetMessageCacheList(NSCacheList * cache)
{
    *(NSGetMessageCacheList()) = cache;
}

NSCacheList ** NSGetProviderCacheList()
{
    static NSCacheList * providerCache = NULL;
    return & providerCache;
}

void NSSetProviderCacheList(NSCacheList * cache)
{
    *(NSGetProviderCacheList()) = cache;
}

void NSDestroyMessageCacheList()
{
    NSCacheList * cache = *(NSGetMessageCacheList());
    if (cache)
    {
        NSStorageDestroy(cache);
    }
}

void NSDestroyProviderCacheList()
{
    NSCacheList * cache = *(NSGetProviderCacheList());
    if (cache)
    {
        NSStorageDestroy(cache);
    }
}

NSResult NSMessageCacheUpdate(NSCacheList * cache, NSMessage_consumer * msg, NSSyncType type)
{
    NS_VERIFY_NOT_NULL(msg, NS_ERROR);

    msg->type = type;

    NSCacheElement * obj = (NSCacheElement *)OICMalloc(sizeof(NSCacheElement));
    NS_VERIFY_NOT_NULL(obj, NS_ERROR);

    obj->data = (NSCacheData *) msg;
    obj->next = NULL;

    NS_LOG(DEBUG, "try to write to storage");
    NSResult ret = NSStorageWrite(cache, obj);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(ret == NS_OK ? (void *) 1 : NULL,
            NS_ERROR, NSRemoveMessage(msg));

    //NSRemoveMessage(msg);
    NSOICFree(obj);

    return NS_OK;
}

NSResult NSProviderCacheUpdate(NSCacheList * cache, NSProvider_internal * provider)
{
    NS_VERIFY_NOT_NULL(provider, NS_ERROR);

    NSCacheElement * obj = (NSCacheElement *)OICMalloc(sizeof(NSCacheElement));
    NS_VERIFY_NOT_NULL(obj, NS_ERROR);

    obj->data = (NSCacheData *) provider;
    obj->next = NULL;

    NS_LOG(DEBUG, "try to write to storage");
    NSResult ret = NSStorageWrite(cache, obj);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(ret == NS_OK ? (void *) 1 : NULL,
            NS_ERROR, NSRemoveProvider(provider));

    //NSRemoveProvider(provider);
    NSOICFree(obj);

    return NS_OK;
}

void NSConsumerHandleProviderDiscovered(NSProvider_internal * provider)
{
    // TODO need to check for discovered provider is new and store provider.[DONE]
    NS_VERIFY_NOT_NULL_V(provider);

    NSCacheList * cache = *(NSGetProviderCacheList());
    NSCacheElement * cacheElement = NSStorageRead(cache, provider->providerId);
    if (cacheElement)
    {
        NS_LOG_V (ERROR, "Provider is already discovered - ProviderID[%s]", provider->providerId);
        return ;
    }
    else
    {
        NS_LOG (ERROR, "New provider is discovered");
        NSResult ret = NSProviderCacheUpdate(cache, provider);
        NS_VERIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);
    }

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

    // TODO change to find provider using pId. [DONE]
    NSCacheList * cache = *(NSGetProviderCacheList());
    NSCacheElement * cacheElement = NSStorageRead(cache, msg->providerId);
    NS_VERIFY_NOT_NULL_V (cacheElement);

    NSProvider * provider = (NSProvider *) cacheElement->data;

    NSSubscriptionAccepted(provider);
}

void NSConsumerHandleRecvMessage(NSMessage_consumer * msg)
{
    NS_VERIFY_NOT_NULL_V(msg);
    // TODO store message to cache [DONE]
    NSCacheList * cache = *(NSGetMessageCacheList());
    NSResult ret = NSMessageCacheUpdate(cache, msg, NS_SYNC_UNREAD);
    NS_VERIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);

    NSMessagePost((NSMessage *) msg);
}

void NSConsumerHandleRecvSyncInfo(NSSyncInfo * sync)
{
    NS_VERIFY_NOT_NULL_V(sync);

    // TODO need to check for provider is available. [DONE]
    NSCacheList * providerCache = *(NSGetProviderCacheList());
    NSCacheElement * providerCacheElement = NSStorageRead(providerCache, sync->providerId);
    NS_VERIFY_NOT_NULL_V (providerCacheElement);

    // TODO need to update msg list. [DONE]
    char msgId[NS_DEVICE_ID_LENGTH] = { 0, };
    snprintf(msgId, NS_DEVICE_ID_LENGTH, "%ld", sync->messageId);

    NSCacheList * messageCache = *(NSGetMessageCacheList());
    NSCacheElement * messageCacheElement = NSStorageRead(messageCache, msgId);
    NS_VERIFY_NOT_NULL_V (messageCacheElement);

    NSMessage_consumer * msg = (NSMessage_consumer *) messageCacheElement->data;
    NSResult ret = NSMessageCacheUpdate(messageCache, msg, sync->state);
    NS_VERIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);

    NSNotificationSync(sync);
}

void NSConsumerHandleMakeSyncInfo(NSSyncInfo * sync)
{
    NS_VERIFY_NOT_NULL_V(sync);

    // TODO need to check for provider is available. [DONE]
    NSCacheList * providerCache = *(NSGetProviderCacheList());
    NSCacheElement * providerCacheElement = NSStorageRead(providerCache, sync->providerId);
    NS_VERIFY_NOT_NULL_V (providerCacheElement);
    NSProvider_internal * provider = (NSProvider_internal *) providerCacheElement->data;
    NS_VERIFY_NOT_NULL_V (provider);

    NSSyncInfo_internal * syncInfo = (NSSyncInfo_internal *)OICMalloc(sizeof(NSSyncInfo_internal));
    NS_VERIFY_NOT_NULL_V(syncInfo);

    OICStrcpy(syncInfo->providerId, sizeof(char) * NS_DEVICE_ID_LENGTH, sync->providerId);
    syncInfo->messageId = sync->messageId;
    syncInfo->state = sync->state;
    syncInfo->_addr = (OCDevAddr *)OICMalloc(sizeof(OCDevAddr));
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING_V(syncInfo->_addr, NSOICFree(syncInfo));
    memcpy(syncInfo->_addr, provider->_addr, sizeof(OCDevAddr));

    NSTask * syncTask = NSMakeTask(TASK_SEND_SYNCINFO, (void *) syncInfo);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING_V(syncTask, NSOICFree(syncInfo));

    NSConsumerPushEvent(syncTask);

    NSOICFree(sync);
}

void NSConsumerInternalTaskProcessing(NSTask * task)
{
    NS_VERIFY_NOT_NULL_V(task);

    NSCacheList * MessageCache = *(NSGetMessageCacheList());
    if (!MessageCache)
    {
        NS_LOG(DEBUG, "Cache Init");
        MessageCache = NSStorageCreate();
        NS_VERIFY_NOT_NULL_V(MessageCache);

        MessageCache->cacheType = NS_CONSUMER_CACHE_MESSAGE;
        NSSetMessageCacheList(MessageCache);
    }

    NSCacheList * ProviderCache = *(NSGetProviderCacheList());
    if (!ProviderCache)
    {
        NS_LOG(DEBUG, "Cache Init");
        ProviderCache = NSStorageCreate();
        NS_VERIFY_NOT_NULL_V(ProviderCache);

        ProviderCache->cacheType = NS_CONSUMER_CACHE_PROVIDER;
        NSSetProviderCacheList(ProviderCache);
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
        case TASK_MAKE_SYNCINFO:
        {
            NS_LOG(DEBUG, "Make SyncInfo, get Provider's Addr");
            NSConsumerHandleMakeSyncInfo((NSSyncInfo *)task->taskData);
            break;
        }
       case TASK_RECV_READ:
       {
           NS_LOG(DEBUG, "Receive Read Notification");

           ret = NSMessageCacheUpdate(MessageCache, task, NS_SYNC_READ);
           NS_VERIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);
           break;
       }
       case TASK_RECV_DISMISS:
       {
           NS_LOG(DEBUG, "Receive Dismiss Notification");

           ret = NSMessageCacheUpdate(MessageCache, task, NS_SYNC_DELETED);
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
