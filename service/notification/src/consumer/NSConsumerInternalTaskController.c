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

    NSSetMessageCacheList(NULL);
}

void NSDestroyProviderCacheList()
{
    NSCacheList * cache = *(NSGetProviderCacheList());
    if (cache)
    {
        NSStorageDestroy(cache);
    }

    NSSetProviderCacheList(NULL);
}

NSMessage * NSMessageCacheFind(const char * messageId)
{
    NS_VERIFY_NOT_NULL(messageId, NULL);

    NSCacheList * MessageCache = *(NSGetMessageCacheList());
    if (!MessageCache)
    {
        NS_LOG(DEBUG, "Message Cache Init");
        MessageCache = NSStorageCreate();
        NS_VERIFY_NOT_NULL(MessageCache, NULL);

        MessageCache->cacheType = NS_CONSUMER_CACHE_MESSAGE;
        NSSetMessageCacheList(MessageCache);
    }

    NSCacheElement * cacheElement = NSStorageRead(MessageCache, messageId);
    NS_VERIFY_NOT_NULL(cacheElement, NULL);

    return NSCopyMessage(((NSStoreMessage *) cacheElement->data)->msg);
}

NSProvider_internal * NSProviderCacheFind(const char * providerId)
{
    NS_VERIFY_NOT_NULL(providerId, NULL);

    NSCacheList * ProviderCache = *(NSGetProviderCacheList());
    if (!ProviderCache)
    {
        NS_LOG(DEBUG, "Provider Cache Init");
        ProviderCache = NSStorageCreate();
        NS_VERIFY_NOT_NULL(ProviderCache, NULL);

        ProviderCache->cacheType = NS_CONSUMER_CACHE_PROVIDER;
        NSSetProviderCacheList(ProviderCache);
    }

    NSCacheElement * cacheElement = NSStorageRead(ProviderCache, providerId);
    NS_VERIFY_NOT_NULL(cacheElement, NULL);

    return NSCopyProvider_internal((NSProvider_internal *) cacheElement->data);
}

NSProvider_internal * NSFindProviderFromAddr(OCDevAddr * addr)
{
    NS_VERIFY_NOT_NULL(addr, NULL);

    NSCacheList * ProviderCache = *(NSGetProviderCacheList());
    if (!ProviderCache)
    {
        NS_LOG(DEBUG, "Provider Cache does not intialized.");
        return NULL;
    }

    NSCacheElement * cacheElement =
            NSGetProviderFromAddr(ProviderCache, addr->addr, addr->port);

    NS_VERIFY_NOT_NULL(cacheElement, NULL);

    return NSCopyProvider_internal((NSProvider_internal *) cacheElement->data);
}

void NSRemoveCacheElementMessage(NSCacheElement * obj)
{
    NSRemoveMessage(((NSStoreMessage *)obj->data)->msg);
    NSOICFree(obj->data);
    NSOICFree(obj);
}

NSResult NSMessageCacheUpdate(NSMessage * msg, NSSyncType type)
{
    NS_VERIFY_NOT_NULL(msg, NS_ERROR);

    NSCacheList * MessageCache = *(NSGetMessageCacheList());
    if (!MessageCache)
    {
        NS_LOG(DEBUG, "Message Cache Init");
        MessageCache = NSStorageCreate();
        NS_VERIFY_NOT_NULL(MessageCache, NS_ERROR);

        MessageCache->cacheType = NS_CONSUMER_CACHE_MESSAGE;
        NSSetMessageCacheList(MessageCache);
    }

    NSStoreMessage * sMsg = (NSStoreMessage *)OICMalloc(sizeof(NSStoreMessage));
    NS_VERIFY_NOT_NULL(sMsg, NS_ERROR);

    NSCacheElement * obj = (NSCacheElement *)OICMalloc(sizeof(NSCacheElement));
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(obj, NS_ERROR, NSOICFree(sMsg));

    sMsg->status = type;
    sMsg->msg = NSCopyMessage(msg);

    obj->data = (NSCacheData *) sMsg;
    obj->next = NULL;

    NS_LOG(DEBUG, "try to write to storage");
    NSResult ret = NSStorageWrite(MessageCache, obj);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(ret == NS_OK ? (void *) 1 : NULL,
            NS_ERROR, NSRemoveCacheElementMessage(obj));

    NSRemoveCacheElementMessage(obj);

    NS_LOG(DEBUG, "Update message done");
    return NS_OK;
}

NSResult NSProviderCacheUpdate(NSProvider_internal * provider)
{
    NSCacheList * ProviderCache = *(NSGetProviderCacheList());
    if (!ProviderCache)
    {
        NS_LOG(DEBUG, "Provider Cache Init");
        ProviderCache = NSStorageCreate();
        NS_VERIFY_NOT_NULL(ProviderCache, NS_ERROR);

        ProviderCache->cacheType = NS_CONSUMER_CACHE_PROVIDER;
        NSSetProviderCacheList(ProviderCache);
    }

    NS_VERIFY_NOT_NULL(provider, NS_ERROR);

    NSCacheElement * obj = (NSCacheElement *)OICMalloc(sizeof(NSCacheElement));
    NS_VERIFY_NOT_NULL(obj, NS_ERROR);

    obj->data = (NSCacheData *) provider;
    obj->next = NULL;

    NS_LOG(DEBUG, "try to write to storage");
    NSResult ret = NSStorageWrite(ProviderCache, obj);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(ret == NS_OK ? (void *) 1 : NULL,
            NS_ERROR, NSOICFree(obj));

    return NS_OK;
}

void NSCancelAllSubscription()
{
    NSCacheList * ProviderCache = *(NSGetProviderCacheList());
    if (!ProviderCache)
    {
        NS_LOG(DEBUG, "Provider Cache Init");
        ProviderCache = NSStorageCreate();
        NS_VERIFY_NOT_NULL_V(ProviderCache);

        ProviderCache->cacheType = NS_CONSUMER_CACHE_PROVIDER;
        NSSetProviderCacheList(ProviderCache);
    }

    NSCacheElement * obj = NULL;
    while ((obj = NSPopProviderCacheList(ProviderCache)))
    {
        NS_LOG(DEBUG, "build NSTask");
        NSTask * task = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL, (void *) obj->data);
        NS_VERIFY_NOT_NULL_V(task);

        NSConsumerPushEvent(task);
    }
}

void NSConsumerHandleProviderDiscovered(NSProvider_internal * provider)
{
    NS_VERIFY_NOT_NULL_V(provider);

    bool isAdded = true;
    bool isSubscribing = false;

    NSProvider_internal * providerCacheDataFromAddr
        = NSFindProviderFromAddr(provider->connection->addr);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING_V(
        (providerCacheDataFromAddr == NULL) ? (void *)1 : NULL,
        NSRemoveProvider_internal(providerCacheDataFromAddr));

    NSProvider_internal * providerCacheData = NSProviderCacheFind(provider->providerId);

    if (providerCacheData == NULL)
    {
        isAdded = false;
    }
    else
    {
        providerCacheData->accessPolicy = provider->accessPolicy;
        NSProviderConnectionInfo * infos = providerCacheData->connection;
        OCTransportAdapter newAdapter = provider->connection->addr->adapter;
        while (infos)
        {
            isSubscribing |= infos->isSubscribing;
            if (infos->addr->adapter == newAdapter && infos->isSubscribing == true)
            {
                NS_LOG_V(DEBUG, "This provider already discovered : %s:%d",
                         infos->addr->addr, infos->addr->port);
                NS_LOG_V(DEBUG, "Subscription : %d", infos->isSubscribing);
                return;
            }
            infos = infos->next;
        }
    }

    NSResult ret = NSProviderCacheUpdate(provider);
    NS_VERIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);

    if (isAdded == false)
    {
        NS_LOG(DEBUG, "New provider is discovered");
    }
    else
    {
        provider = providerCacheData;
        NS_LOG(DEBUG, "provider's connection is updated.");
    }

    if (provider->accessPolicy == NS_SELECTION_CONSUMER && isSubscribing == false)
    {
        NS_LOG(DEBUG, "accepter is NS_ACCEPTER_CONSUMER, Callback to user");
        NSProvider * providerForCb = NSCopyProvider(provider);
        NSDiscoveredProvider(providerForCb);
    }
    else
    {
        NS_LOG(DEBUG, "accepter is NS_ACCEPTER_PROVIDER, request subscribe");
        NSProvider_internal * subProvider = NSCopyProvider_internal(provider);
        NSTask * task = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE, (void *) subProvider);
        NS_VERIFY_NOT_NULL_V(task);

        NSConsumerPushEvent(task);
    }

    NSRemoveProvider_internal(providerCacheData);
}

void NSConsumerHandleProviderDeleted(NSProvider_internal * provider)
{
    NS_VERIFY_NOT_NULL_V(provider);

    NSCacheList * providerCache = *(NSGetProviderCacheList());
    NS_VERIFY_NOT_NULL_V(providerCache);

    NSResult ret = NSStorageDelete(providerCache, provider->providerId);
    NS_VERIFY_NOT_NULL_V(ret == NS_OK ? (void *)1 : NULL);
}

void NSConsumerHandleSubscribeSucceed(NSProvider_internal * provider)
{
    NS_VERIFY_NOT_NULL_V(provider);

    NSCacheList * ProviderCache = *(NSGetProviderCacheList());

    NSCacheElement * cacheElement = NSStorageRead(ProviderCache, provider->providerId);
    NS_VERIFY_NOT_NULL_V(cacheElement);

    pthread_mutex_t * mutex = NSGetCacheMutex();
    pthread_mutex_lock(mutex);

    NS_VERIFY_NOT_NULL_V(cacheElement);
    NSProvider_internal * prov = (NSProvider_internal *)cacheElement->data;
    NSProviderConnectionInfo *infos = prov->connection;
    while(infos)
    {
        infos->isSubscribing = true;
        infos = infos->next;
    }

    pthread_mutex_unlock(mutex);
}

void NSConsumerHandleRecvProviderChanged(NSMessage * msg)
{
    NS_VERIFY_NOT_NULL_V(msg);

    NS_LOG_V(DEBUG, "confirmed by : %s", msg->providerId);
    NSProvider_internal * provider = NSProviderCacheFind(msg->providerId);
    NS_VERIFY_NOT_NULL_V(provider);

    NS_LOG(DEBUG, "call back to user");
    NSProvider * prov = NSCopyProvider(provider);
    NSProviderChanged(prov, (NSResponse) msg->messageId);
    NSRemoveProvider_internal(provider);
}

void NSConsumerHandleRecvMessage(NSMessage * msg)
{
    NS_VERIFY_NOT_NULL_V(msg);

    NSResult ret = NSMessageCacheUpdate(msg, NS_SYNC_UNREAD);
    NS_VERIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);

    NSMessagePost((NSMessage *) msg);
}

void NSConsumerHandleRecvSyncInfo(NSSyncInfo * sync)
{
    NS_VERIFY_NOT_NULL_V(sync);

    char msgId[NS_DEVICE_ID_LENGTH] = { 0, };
    snprintf(msgId, NS_DEVICE_ID_LENGTH, "%lld", (long long int)sync->messageId);

    NSMessage * msg = NSMessageCacheFind(msgId);
    NS_VERIFY_NOT_NULL_V(msg);

    NSResult ret = NSMessageCacheUpdate(msg, sync->state);
    NS_VERIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);
    NSRemoveMessage(msg);

    NSNotificationSync(sync);
}

void NSConsumerHandleMakeSyncInfo(NSSyncInfo * sync)
{
    NS_VERIFY_NOT_NULL_V(sync);

    NSProvider_internal * provider = NSProviderCacheFind(sync->providerId);
    NS_VERIFY_NOT_NULL_V (provider);

    NSProviderConnectionInfo * connections = NSCopyProviderConnections(provider->connection);
    NS_VERIFY_NOT_NULL_V (connections);

    NSSyncInfo_internal * syncInfo = (NSSyncInfo_internal *)OICMalloc(sizeof(NSSyncInfo_internal));
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING_V(syncInfo, NSRemoveConnections(connections));

    OICStrcpy(syncInfo->providerId, sizeof(char) * NS_DEVICE_ID_LENGTH, sync->providerId);
    syncInfo->messageId = sync->messageId;
    syncInfo->state = sync->state;
    syncInfo->connection = connections;

    NSTask * syncTask = NSMakeTask(TASK_SEND_SYNCINFO, (void *) syncInfo);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING_V(syncTask, NSOICFree(syncInfo));

    NSConsumerPushEvent(syncTask);
}

void NSConsumerHandleGetTopicUri(NSMessage * msg)
{
    NS_VERIFY_NOT_NULL_V(msg);

    NSProvider_internal * provider = NSProviderCacheFind(msg->providerId);
    NS_VERIFY_NOT_NULL_V(provider);

    NSTask * topicTask = NSMakeTask(TASK_CONSUMER_REQ_TOPIC_LIST, (void *) provider);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING_V(topicTask, NSRemoveProvider_internal(provider));

    NSConsumerPushEvent(topicTask);
}

void NSConsumerHandleRecvTopicLL(NSProvider_internal * provider)
{
    NS_VERIFY_NOT_NULL_V(provider);

    NSRemoveConnections(provider->connection);
    provider->connection = NULL;

    NSResult ret = NSProviderCacheUpdate(provider);
    NS_VERIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);

    NS_LOG(DEBUG, "call back to user");
    NSProvider * prov = NSCopyProvider(provider);
    NSProviderChanged((NSProvider *) prov, (NSResponse) NS_TOPIC);
}

void NSConsumerInternalTaskProcessing(NSTask * task)
{
    NS_VERIFY_NOT_NULL_V(task);

    NS_LOG_V(DEBUG, "Receive Event : %d", (int)task->taskType);
    switch (task->taskType)
    {
        case TASK_CONSUMER_SENT_REQ_OBSERVE:
        {
            NS_LOG(DEBUG, "Receive Subscribe succeed from provider.");
            NSConsumerHandleSubscribeSucceed((NSProvider_internal *)task->taskData);
            NSRemoveProvider_internal((NSProvider_internal *)task->taskData);
            break;
        }
        case TASK_CONSUMER_RECV_PROVIDER_CHANGED:
        {
            NS_LOG(DEBUG, "Receive Provider Changed");
            NSConsumerHandleRecvProviderChanged((NSMessage *)task->taskData);
            NSRemoveMessage((NSMessage *)task->taskData);
            break;
        }
        case TASK_CONSUMER_RECV_MESSAGE:
        {
            NS_LOG(DEBUG, "Receive New Notification");
            NSConsumerHandleRecvMessage((NSMessage *)task->taskData);
            NSRemoveMessage((NSMessage *)task->taskData);
            break;
        }
        case TASK_CONSUMER_PROVIDER_DISCOVERED:
        {
            NS_LOG(DEBUG, "Receive New Provider is discovered.");
            NSConsumerHandleProviderDiscovered((NSProvider_internal *)task->taskData);
            NSRemoveProvider_internal((NSProvider_internal *)task->taskData);
            break;
        }
        case TASK_RECV_SYNCINFO:
        {
            NS_LOG(DEBUG, "Receive SyncInfo.");
            NSConsumerHandleRecvSyncInfo((NSSyncInfo *)task->taskData);
            NSOICFree(task->taskData);
            break;
        }
        case TASK_MAKE_SYNCINFO:
        {
            NS_LOG(DEBUG, "Make SyncInfo, get Provider's Addr");
            NSConsumerHandleMakeSyncInfo((NSSyncInfo *)task->taskData);
            NSOICFree(task->taskData);
            break;
        }
        case TASK_CONSUMER_REQ_TOPIC_URI:
        {
            NS_LOG(DEBUG, "Request Topic Uri");
            NSConsumerHandleGetTopicUri((NSMessage *)task->taskData);
            NSRemoveMessage((NSMessage *)task->taskData);
            break;
        }
        case TASK_CONSUMER_RECV_TOPIC_LIST:
        {
            NS_LOG(DEBUG, "Receive Topic List");
            NSConsumerHandleRecvTopicLL((NSProvider_internal *)task->taskData);
            NSRemoveProvider_internal((NSProvider_internal *)task->taskData);
            break;
        }
        case TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL:
        {
            NS_LOG(DEBUG, "Make Subscribe cancel from provider.");
            NSConsumerHandleProviderDeleted((NSProvider_internal *)task->taskData);
            NSRemoveProvider_internal((NSProvider_internal *)task->taskData);
            break;
        }
        default :
        {
            NS_LOG(ERROR, "Unknown TASK Type");
            return ;
        }
    }
    NSOICFree(task);
}
