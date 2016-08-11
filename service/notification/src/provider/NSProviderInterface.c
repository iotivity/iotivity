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
#include "NSStorageAdapter.h"
#include "NSProviderMemoryCache.h"
#include "NSProviderTopic.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "cautilinterface.h"
#include "NSProviderSystem.h"
#include "oic_time.h"

bool initProvider = false;

pthread_mutex_t nsInitMutex;

void initializeMutex()
{
    static pthread_mutex_t initMutex = PTHREAD_MUTEX_INITIALIZER;
    nsInitMutex = initMutex;
}

NSResult NSStartProvider(NSProviderConfig config)
{
    NS_LOG(DEBUG, "NSStartProvider - IN");

    initializeMutex();

    pthread_mutex_lock(&nsInitMutex);

    if (!initProvider)
    {
        NS_LOG(DEBUG, "Init Provider");
        initProvider = true;
        NSInitProviderInfo(config.userInfo);
        NSSetSubscriptionAccessPolicy(config.policy);
        NSRegisterSubscribeRequestCb(config.subRequestCallback);
        NSRegisterSyncCb(config.syncInfoCallback);
        CARegisterNetworkMonitorHandler((CAAdapterStateChangedCB)NSProviderAdapterStateListener,
                (CAConnectionStateChangedCB)NSProviderConnectionStateListener);

        NSSetList();
        NSInitScheduler();
        NSStartScheduler();

        NSPushQueue(DISCOVERY_SCHEDULER, TASK_START_PRESENCE, NULL);
        NSPushQueue(DISCOVERY_SCHEDULER, TASK_REGISTER_RESOURCE, NULL);
    }
    else
    {
        NS_LOG(DEBUG, "Already started Notification Provider");
    }
    pthread_mutex_unlock(&nsInitMutex);

    NS_LOG(DEBUG, "NSStartProvider - OUT");
    return NS_OK;
}

void NSSetList()
{
    NS_LOG(DEBUG, "NSSetList - IN");

    pthread_mutexattr_init(&NSCacheMutexAttr);
    int pthreadResult = pthread_mutexattr_settype(&NSCacheMutexAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&NSCacheMutex, &NSCacheMutexAttr);

    NSInitSubscriptionList();
    NSInitMessageList();
    NSInitTopicList();
    NS_LOG(DEBUG, "NSSetList - OUT");
}

void NSDestroyList()
{
    NSStorageDestroy(consumerSubList);
    NSStorageDestroy(messageList);
    NSStorageDestroy(consumerTopicList);
    NSStorageDestroy(registeredTopicList);

    pthread_mutex_destroy(&NSCacheMutex);
    pthread_mutexattr_destroy(&NSCacheMutexAttr);
}

NSResult NSStopProvider()
{
    NS_LOG(DEBUG, "NSStopProvider - IN");
    pthread_mutex_lock(&nsInitMutex);

    if(initProvider)
    {
        NSPushQueue(DISCOVERY_SCHEDULER, TASK_STOP_PRESENCE, NULL);
        NSDeinitProviderInfo();
        NSUnRegisterResource();
        NSRegisterSubscribeRequestCb((NSSubscribeRequestCallback)NULL);
        NSRegisterSyncCb((NSProviderSyncInfoCallback)NULL);
        NSStopScheduler();
        NSDestroyList();

        initProvider = false;
    }

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSStopProvider - OUT");
    return NS_OK;
}

#ifdef WITH_CLOUD
NSResult NSProviderEnableRemoteService(char *serverAddress)
{
    NS_LOG(DEBUG, "NSProviderEnableRemoteService - IN");
    pthread_mutex_lock(&nsInitMutex);
 
    if(!initProvider)
    {
        NS_LOG(DEBUG, "Provider service has not been started yet");
        pthread_mutex_unlock(&nsInitMutex);
        return NS_FAIL;
    }

    NS_LOG_V(DEBUG, "Remote server address: %s", serverAddress);
    NSPushQueue(DISCOVERY_SCHEDULER, TASK_PUBLISH_RESOURCE, serverAddress);

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSProviderEnableRemoteService - OUT");
    return NS_OK;
}

NSResult NSProviderDisableRemoteService(char *serverAddress)
{
    NS_LOG(DEBUG, "NSProviderDisableRemoteService - IN");
    pthread_mutex_lock(&nsInitMutex);

    if(!initProvider)
    {
        NS_LOG(DEBUG, "Provider service has not been started yet");
        return NS_FAIL;
    }
    NS_LOG_V(DEBUG, "Remote server address: %s", serverAddress);

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSProviderDisableRemoteService - OUT");
    return NS_OK;
}
#endif

NSResult NSSendMessage(NSMessage *msg)
{
    NS_LOG(DEBUG, "NSSendNotification - IN");

    pthread_mutex_lock(&nsInitMutex);

    if(msg == NULL)
    {
        NS_LOG(ERROR, "Msg is NULL");
        pthread_mutex_unlock(&nsInitMutex);
        return NS_ERROR;
    }

    NSMessage * newMsg = NSDuplicateMessage(msg);
    NSPushQueue(NOTIFICATION_SCHEDULER, TASK_SEND_NOTIFICATION, newMsg);

    pthread_mutex_unlock(&nsInitMutex);

    NS_LOG(DEBUG, "NSSendNotification - OUT");
    return NS_OK;
}

NSResult NSProviderSendSyncInfo(uint64_t messageId, NSSyncType type)
{
    NS_LOG(DEBUG, "NSProviderReadCheck - IN");
    pthread_mutex_lock(&nsInitMutex);

    NSSyncInfo * syncInfo = (NSSyncInfo *)OICMalloc(sizeof(NSSyncInfo));
    OICStrcpy(syncInfo->providerId, UUID_STRING_SIZE, NSGetProviderInfo()->providerId);
    syncInfo->messageId = messageId;
    syncInfo->state = type;
    NSPushQueue(NOTIFICATION_SCHEDULER, TASK_SEND_READ, syncInfo);

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSProviderReadCheck - OUT");
    return NS_OK;
}

NSResult NSAcceptSubscription(NSConsumer *consumer, bool accepted)
{
    NS_LOG(DEBUG, "NSAccept - IN");

    pthread_mutex_lock(&nsInitMutex);

    NSConsumer * newConsumer = NSDuplicateConsumer(consumer);
    if(accepted)
    {
        NS_LOG(DEBUG, "accepted is true - ALLOW");
        NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_SEND_ALLOW, newConsumer);
    }
    else
    {
        NS_LOG(DEBUG, "accepted is false - DENY");
        NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_SEND_DENY, newConsumer);
    }

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSAccept - OUT");
    return NS_OK;
}

NSMessage * NSCreateMessage()
{
    NS_LOG(DEBUG, "NSCreateMessage - IN");
    pthread_mutex_lock(&nsInitMutex);

    NSMessage * msg = NSInitializeMessage();
    OICStrcpy(msg->providerId, UUID_STRING_SIZE, NSGetProviderInfo()->providerId);

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSCreateMessage - OUT");
    return msg;
}

NSTopics * NSProviderGetConsumerTopics(char *consumerId)
{
    NS_LOG(DEBUG, "NSProviderGetConsumerTopics - IN");
    pthread_mutex_lock(&nsInitMutex);

    //TODO implement get subscribed topics for consumer

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSProviderGetConsumerTopics - OUT");

    return NULL;
}

NSTopics * NSProviderGetTopics()
{
    NS_LOG(DEBUG, "NSProviderGetTopics - IN");
    pthread_mutex_lock(&nsInitMutex);

    //TODO implement get registered topics for consumer

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSProviderGetTopics - OUT");

    return NULL;
}

NSResult NSProviderAddTopic(char *topicName)
{
    NS_LOG(DEBUG, "NSProviderAddTopics - IN");
    pthread_mutex_lock(&nsInitMutex);

    NSPushQueue(TOPIC_SCHEDULER, TASK_ADD_TOPIC, topicName);

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSProviderAddTopics - OUT");
    return NS_OK;
}

NSResult NSProviderDeleteTopic(char *topicName)
{
    NS_LOG(DEBUG, "NSProviderDeleteTopics - IN");
    pthread_mutex_lock(&nsInitMutex);

    NSPushQueue(TOPIC_SCHEDULER, TASK_DELETE_TOPIC, topicName);

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSProviderDeleteTopics - OUT");
    return NS_OK;
}

NSResult NSProviderSelectTopic(char *consumerId, char *topicName)
{
    NS_LOG(DEBUG, "NSProviderSelectTopics - IN");
    pthread_mutex_lock(&nsInitMutex);

    if(!consumerId)
    {
        NS_LOG(DEBUG, "consumer id should be set for topic subscription");
        return NS_FAIL;
    }

    //TODO: add consumerId to task struct
    NSPushQueue(TOPIC_SCHEDULER, TASK_SUBSCRIBE_TOPIC, topicName);

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSProviderSelectTopics - OUT");
    return NS_OK;
}

NSResult NSProviderUnselectTopic(char *consumerId, char *topicName)
{
    NS_LOG(DEBUG, "NSProviderUnselectTopics - IN");
    pthread_mutex_lock(&nsInitMutex);

    if(!consumerId)
    {
        NS_LOG(DEBUG, "consumer id should be set for topic subscription");
        return NS_FAIL;
    }
    
    //TODO: add consumerId to task struct
    NSPushQueue(TOPIC_SCHEDULER, TASK_UNSUBSCRIBE_TOPIC, topicName);

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSProviderUnselectTopics - OUT");
    return NS_OK;
}

