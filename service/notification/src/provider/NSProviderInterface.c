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

#include "NSProviderInterface.h"
#include "NSProviderScheduler.h"
#include "NSProviderListener.h"
#include "NSProviderSubscription.h"
#include "NSProviderNotification.h"
#include "NSProviderCallbackResponse.h"
#include "NSProviderMemoryCache.h"
#include "NSProviderTopic.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "cautilinterface.h"
#include "NSProviderSystem.h"
#include "oic_time.h"
#include "octhread.h"

#ifndef SYSTEM_WINDOWS
#include <pthread.h>

typedef struct
{
    pthread_mutex_t mutex;
} oc_mutex_ns_internal;
#endif

bool initProvider = false;

oc_mutex nsInitMutex;
oc_cond nstopicCond;

void initializeMutex()
{
    static oc_mutex initMutex = NULL;
    if (initMutex == NULL)
    {
        initMutex = oc_mutex_new();
    }
    nsInitMutex = initMutex;
}

void NSInitialize()
{
    NS_LOG(DEBUG, "NSSetList - IN");

#ifdef SYSTEM_WINDOWS
    NSCacheMutex = oc_mutex_new();
#else
    oc_mutex_ns_internal * mutexInfo
        = (oc_mutex_ns_internal *) OICMalloc(sizeof(oc_mutex_ns_internal));
    if (!mutexInfo)
    {
        NS_LOG(ERROR, "Cache mutex Create fail");
    }
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&(mutexInfo->mutex), &attr);
    pthread_mutexattr_destroy(&attr);

    NSCacheMutex = (oc_mutex) mutexInfo;
#endif

    nstopicCond = oc_cond_new();

    NSInitSubscriptionList();
    NSInitTopicList();
    NS_LOG(DEBUG, "NSSetList - OUT");
}

void NSDeinitailize()
{
    NSProviderStorageDestroy(consumerSubList);
    NSProviderStorageDestroy(consumerTopicList);
    NSProviderStorageDestroy(registeredTopicList);

    oc_mutex_free(NSCacheMutex);
    oc_cond_free(nstopicCond);
}

NSResult NSStartProvider(NSProviderConfig config)
{
    NS_LOG(DEBUG, "NSStartProvider - IN");

    initializeMutex();

    oc_mutex_lock(nsInitMutex);

    if (!initProvider)
    {
        NS_LOG(DEBUG, "Init Provider");
        initProvider = true;
        NSInitProviderInfo(config.userInfo);
        NSSetSubscriptionAccessPolicy(config.subControllability);
        NSRegisterSubscribeRequestCb(config.subRequestCallback);
        NSRegisterSyncCb(config.syncInfoCallback);
        CARegisterNetworkMonitorHandler((CAAdapterStateChangedCB)NSProviderAdapterStateListener,
                (CAConnectionStateChangedCB)NSProviderConnectionStateListener);

        NS_LOG(DEBUG, "Check secured build option..");
#ifdef SECURED
        NS_LOG(DEBUG, "Built with SECURED");

        if (!config.resourceSecurity)
        {
            NS_LOG(DEBUG, "Resource Security Off");
        }

        NSSetResourceSecurity(config.resourceSecurity);
#endif

        NSInitialize();
        NSInitScheduler();
        NSStartScheduler();

        NSPushQueue(DISCOVERY_SCHEDULER, TASK_START_PRESENCE, NULL);
        NSPushQueue(DISCOVERY_SCHEDULER, TASK_REGISTER_RESOURCE, NULL);
    }
    else
    {
        NS_LOG(DEBUG, "Already started Notification Provider");
    }
    oc_mutex_unlock(nsInitMutex);

    NS_LOG(DEBUG, "NSStartProvider - OUT");
    return NS_OK;
}

NSResult NSStopProvider()
{
    NS_LOG(DEBUG, "NSStopProvider - IN");
    oc_mutex_lock(nsInitMutex);

    if (initProvider)
    {
        NSPushQueue(DISCOVERY_SCHEDULER, TASK_STOP_PRESENCE, NULL);
        NSDeinitProviderInfo();
        NSUnRegisterResource();
        NSRegisterSubscribeRequestCb((NSSubscribeRequestCallback)NULL);
        NSRegisterSyncCb((NSProviderSyncInfoCallback)NULL);
        NSStopScheduler();
        NSDeinitailize();

        initProvider = false;
    }

    oc_mutex_unlock(nsInitMutex);
    NS_LOG(DEBUG, "NSStopProvider - OUT");
    return NS_OK;
}

NSResult NSProviderEnableRemoteService(char *serverAddress)
{
#if (defined WITH_CLOUD && defined RD_CLIENT)
    NS_LOG(DEBUG, "NSProviderEnableRemoteService - IN");
    oc_mutex_lock(nsInitMutex);

    if (!initProvider || !serverAddress)
    {
        NS_LOG(DEBUG, "Provider service has not been started yet");
        oc_mutex_unlock(nsInitMutex);
        return NS_FAIL;
    }

    NS_LOG_V(DEBUG, "Remote server address: %s", serverAddress);
    NS_LOG(DEBUG, "Request to publish resource");
    NSPushQueue(DISCOVERY_SCHEDULER, TASK_PUBLISH_RESOURCE, serverAddress);

    oc_mutex_unlock(nsInitMutex);
    NS_LOG(DEBUG, "NSProviderEnableRemoteService - OUT");
    return NS_OK;
#else
    (void) serverAddress;
#endif
    NS_LOG_V(DEBUG, "Not logged in remote server: %s", serverAddress);
    return NS_FAIL;
}

NSResult NSProviderDisableRemoteService(char *serverAddress)
{
#if (defined WITH_CLOUD && defined RD_CLIENT)
    NS_LOG(DEBUG, "NSProviderDisableRemoteService - IN");
    oc_mutex_lock(nsInitMutex);

    if (!initProvider || !serverAddress)
    {
        NS_LOG(DEBUG, "Provider service has not been started yet");
        return NS_FAIL;
    }

    NS_LOG_V(DEBUG, "Remote server address: %s", serverAddress);

    NS_LOG(DEBUG, "Delete remote server info");
    NSDeleteRemoteServerAddress(serverAddress);

    oc_mutex_unlock(nsInitMutex);
    NS_LOG(DEBUG, "NSProviderDisableRemoteService - OUT");
    return NS_OK;
#else
    (void) serverAddress;
#endif
    NS_LOG_V(DEBUG, "Not logged in remote server : %s", serverAddress);
    return NS_FAIL;
}

#ifdef WITH_MQ
NSResult NSProviderSubscribeMQService(const char * serverAddress, const char * topicName)
{
    NS_LOG(DEBUG, "NSProviderSubscribeMQService - IN");
    pthread_mutex_lock(&nsInitMutex);

    if (!initProvider || !serverAddress || !topicName)
    {
        NS_LOG(DEBUG, "Provider service has not been started yet or set the server "
                "address and topicName");
        pthread_mutex_unlock(&nsInitMutex);
        return NS_FAIL;
    }

    NSMQTopicAddress * topicAddr = (NSMQTopicAddress *)OICMalloc(sizeof(NSMQTopicAddress));

    if (!topicAddr)
    {
        NS_LOG(DEBUG, "fail to memory allocate");
        pthread_mutex_unlock(&nsInitMutex);
        return NS_FAIL;
    }

    topicAddr->serverAddr = NSGetQueryAddress(serverAddress);
    topicAddr->topicName = OICStrdup(topicName);

    NS_LOG_V(DEBUG, "input Topic Name : %s", topicAddr->topicName);

    NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_MQ_REQ_SUBSCRIBE, (void *) topicAddr);

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSProviderSubscribeMQService - OUT");
    return NS_OK;
}
#endif

NSResult NSSendMessage(NSMessage * msg)
{
    NS_LOG(DEBUG, "NSSendNotification - IN");

    oc_mutex_lock(nsInitMutex);

    if (!initProvider || msg == NULL)
    {
        NS_LOG(ERROR, "Msg is NULL");
        oc_mutex_unlock(nsInitMutex);
        return NS_FAIL;
    }

    NSMessage * newMsg = NSDuplicateMessage(msg);
    NSPushQueue(NOTIFICATION_SCHEDULER, TASK_SEND_NOTIFICATION, newMsg);

    oc_mutex_unlock(nsInitMutex);

    NS_LOG(DEBUG, "NSSendNotification - OUT");
    return NS_OK;
}

NSResult NSProviderSendSyncInfo(uint64_t messageId, NSSyncType type)
{
    NS_LOG(DEBUG, "NSProviderReadCheck - IN");
    oc_mutex_lock(nsInitMutex);

    NSSyncInfo * syncInfo = (NSSyncInfo *)OICMalloc(sizeof(NSSyncInfo));

    if (!initProvider || !syncInfo)
    {
        NS_LOG(ERROR, "Provider is not started");
        oc_mutex_unlock(nsInitMutex);
        return NS_FAIL;
    }

    OICStrcpy(syncInfo->providerId, UUID_STRING_SIZE, NSGetProviderInfo()->providerId);
    syncInfo->messageId = messageId;
    syncInfo->state = type;
    NSPushQueue(NOTIFICATION_SCHEDULER, TASK_SEND_READ, syncInfo);

    oc_mutex_unlock(nsInitMutex);
    NS_LOG(DEBUG, "NSProviderReadCheck - OUT");
    return NS_OK;
}

NSResult NSAcceptSubscription(const char * consumerId, bool accepted)
{
    NS_LOG(DEBUG, "NSAccept - IN");
    oc_mutex_lock(nsInitMutex);

    if(!initProvider || !consumerId || consumerId[0] == '\0' || NSGetPolicy() == NS_POLICY_CONSUMER)
    {
        NS_LOG(ERROR, "Provider is not started or consumerId is NULL or NS Policy is Consumer");
        oc_mutex_unlock(nsInitMutex);
        return NS_FAIL;
    }

    char * newConsumerId = OICStrdup(consumerId);
    if (accepted)
    {
        NS_LOG(DEBUG, "accepted is true - ALLOW");
        NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_SEND_ALLOW, newConsumerId);
    }
    else
    {
        NS_LOG(DEBUG, "accepted is false - DENY");
        NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_SEND_DENY, newConsumerId);
    }

    oc_mutex_unlock(nsInitMutex);
    NS_LOG(DEBUG, "NSAccept - OUT");
    return NS_OK;
}

NSMessage * NSCreateMessage()
{
    NS_LOG(DEBUG, "NSCreateMessage - IN");
    oc_mutex_lock(nsInitMutex);

    NSMessage * msg = NSInitializeMessage();

    OICStrcpy(msg->providerId, UUID_STRING_SIZE, NSGetProviderInfo()->providerId);

    oc_mutex_unlock(nsInitMutex);
    NS_LOG(DEBUG, "NSCreateMessage - OUT");
    return msg;
}

NSTopicLL * NSProviderGetConsumerTopics(const char * consumerId)
{
    NS_LOG(DEBUG, "NSProviderGetConsumerTopics - IN");
    oc_mutex_lock(nsInitMutex);

    if(!initProvider || !consumerId || consumerId[0] == '\0')
    {
        NS_LOG(DEBUG, "Provider is not started or consumer id should be set");
        oc_mutex_unlock(nsInitMutex);
        return NULL;
    }

    NSTopicSync topicSync;
    topicSync.consumerId = OICStrdup(consumerId);
    topicSync.topics = NULL;
    topicSync.condition = nstopicCond;
    topicSync.mutex = nsInitMutex;

    NSPushQueue(TOPIC_SCHEDULER, TAST_GET_CONSUMER_TOPICS, &topicSync);
    oc_cond_wait(topicSync.condition, nsInitMutex);
    OICFree(topicSync.consumerId);

    oc_mutex_unlock(nsInitMutex);
    NS_LOG(DEBUG, "NSProviderGetConsumerTopics - OUT");
    return topicSync.topics;
}

NSTopicLL * NSProviderGetTopics()
{
    NS_LOG(DEBUG, "NSProviderGetTopics - IN");
    oc_mutex_lock(nsInitMutex);

    if (!initProvider)
    {
        NS_LOG(ERROR, "Provider is not started");
        oc_mutex_unlock(nsInitMutex);
        return NULL;
    }

    NSTopicSync topicSync;
    topicSync.consumerId = NULL;
    topicSync.topics = NULL;
    topicSync.condition = nstopicCond;
    topicSync.mutex = nsInitMutex;

    NSPushQueue(TOPIC_SCHEDULER, TASK_GET_TOPICS, &topicSync);
    oc_cond_wait(topicSync.condition, nsInitMutex);

    oc_mutex_unlock(nsInitMutex);
    NS_LOG(DEBUG, "NSProviderGetTopics - OUT");
    return topicSync.topics;
}

NSResult NSProviderRegisterTopic(const char * topicName)
{
    NS_LOG(DEBUG, "NSProviderAddTopics - IN");
    oc_mutex_lock(nsInitMutex);

    if(!initProvider || !topicName || topicName[0] == '\0')
    {
        oc_mutex_unlock(nsInitMutex);
        NS_LOG(DEBUG, "Provider is not started or topic Name should be set");
        return NS_FAIL;
    }

    NSTopicSyncResult topicSyncResult;
    topicSyncResult.topicData = (void *) OICStrdup(topicName);
    topicSyncResult.condition = nstopicCond;
    topicSyncResult.result = NS_OK;
    topicSyncResult.mutex = nsInitMutex;

    NSPushQueue(TOPIC_SCHEDULER, TASK_REGISTER_TOPIC, &topicSyncResult);
    oc_cond_wait(topicSyncResult.condition, nsInitMutex);
    if(topicSyncResult.result != NS_OK)
    {
        oc_mutex_unlock(nsInitMutex);
        return NS_FAIL;
    }

    oc_mutex_unlock(nsInitMutex);
    NS_LOG(DEBUG, "NSProviderAddTopics - OUT");
    return NS_OK;
}

NSResult NSProviderUnregisterTopic(const char * topicName)
{
    NS_LOG(DEBUG, "NSProviderDeleteTopics - IN");
    oc_mutex_lock(nsInitMutex);

    if(!initProvider || !topicName || topicName[0] == '\0')
    {
        oc_mutex_unlock(nsInitMutex);
        NS_LOG(DEBUG, "Provider is not started or topic Name should be set");
        return NS_FAIL;
    }

    NSTopicSyncResult topicSyncResult;
    topicSyncResult.topicData = (void *) OICStrdup(topicName);
    topicSyncResult.condition = nstopicCond;
    topicSyncResult.result = NS_OK;
    topicSyncResult.mutex = nsInitMutex;

    NSPushQueue(TOPIC_SCHEDULER, TASK_UNREGISTER_TOPIC, &topicSyncResult);
    oc_cond_wait(topicSyncResult.condition, nsInitMutex);
    if (topicSyncResult.result != NS_OK)
    {
        oc_mutex_unlock(nsInitMutex);
        return NS_FAIL;
    }
    OICFree(topicSyncResult.topicData);

    oc_mutex_unlock(nsInitMutex);
    NS_LOG(DEBUG, "NSProviderDeleteTopics - OUT");
    return NS_OK;
}

NSResult NSProviderSetConsumerTopic(const char * consumerId, const char * topicName)
{
    NS_LOG(DEBUG, "NSProviderSelectTopics - IN");
    oc_mutex_lock(nsInitMutex);

    NSCacheTopicSubData * topicSubData =
            (NSCacheTopicSubData *) OICMalloc(sizeof(NSCacheTopicSubData));

    if (!initProvider || !consumerId || consumerId[0] == '\0' || !topicName || topicName[0] == '\0'
            || !NSGetPolicy() || !topicSubData)
    {
        NS_LOG(DEBUG, "provider is not started or "
                "consumer id should be set for topic subscription or "
                "Configuration must set to true.");
        oc_mutex_unlock(nsInitMutex);
        return NS_FAIL;
    }

    OICStrcpy(topicSubData->id, NS_UUID_STRING_SIZE, consumerId);
    topicSubData->topicName = OICStrdup(topicName);

    NSTopicSyncResult topicSyncResult;
    topicSyncResult.topicData = (void *) topicSubData;
    topicSyncResult.condition = nstopicCond;
    topicSyncResult.result = NS_FAIL;
    topicSyncResult.mutex = nsInitMutex;

    NSPushQueue(TOPIC_SCHEDULER, TASK_SUBSCRIBE_TOPIC, (void *)&topicSyncResult);
    oc_cond_wait(topicSyncResult.condition, nsInitMutex);

    oc_mutex_unlock(nsInitMutex);
    NS_LOG(DEBUG, "NSProviderSelectTopics - OUT");
    return topicSyncResult.result;
}

NSResult NSProviderUnsetConsumerTopic(const char * consumerId, const char * topicName)
{
    NS_LOG(DEBUG, "NSProviderUnselectTopics - IN");
    oc_mutex_lock(nsInitMutex);

    NSCacheTopicSubData * topicSubData =
            (NSCacheTopicSubData *) OICMalloc(sizeof(NSCacheTopicSubData));

    if (!initProvider || !consumerId || consumerId[0] == '\0' || !topicName || topicName[0] == '\0'
            || !NSGetPolicy() || !topicSubData)
    {
        NS_LOG(DEBUG, "provider is not started or "
                "consumer id should be set for topic subscription or "
                "Configuration must set to true.");
        oc_mutex_unlock(nsInitMutex);
        return NS_FAIL;
    }

    OICStrcpy(topicSubData->id, NS_UUID_STRING_SIZE, consumerId);
    topicSubData->topicName = OICStrdup(topicName);

    NSTopicSyncResult topicSyncResult;
    topicSyncResult.topicData = (void *) topicSubData;
    topicSyncResult.condition = nstopicCond;
    topicSyncResult.result = NS_FAIL;
    topicSyncResult.mutex = nsInitMutex;

    NSPushQueue(TOPIC_SCHEDULER, TASK_UNSUBSCRIBE_TOPIC, (void *)&topicSyncResult);
    oc_cond_wait(topicSyncResult.condition, nsInitMutex);

    oc_mutex_unlock(nsInitMutex);
    NS_LOG(DEBUG, "NSProviderUnselectTopics - OUT");
    return topicSyncResult.result;
}
