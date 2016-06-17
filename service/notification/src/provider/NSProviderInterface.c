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
#include "NSStorageAdapter.h"
#include "NSProviderMemoryCache.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "cautilinterface.h"
#include "NSProviderSystem.h"
#include "oic_time.h"

bool initProvider = false;
static NSSubscribeRequestCallback g_subscribeRequestCb = NULL;
static NSProviderSyncInfoCallback g_syncCb = NULL;

pthread_mutex_t nsInitMutex;

void initializeMutex()
{
    static pthread_mutex_t initMutex = PTHREAD_MUTEX_INITIALIZER;
    nsInitMutex = initMutex;
}

void NSRegisterSubscribeRequestCb(NSSubscribeRequestCallback subscribeRequestCb)
{
    NS_LOG(DEBUG, "NSRegisterSubscribeRequestCb - IN");
    g_subscribeRequestCb = subscribeRequestCb;
    NS_LOG(DEBUG, "NSRegisterSubscribeRequestCb - OUT");
}

void  NSRegisterSyncCb(NSProviderSyncInfoCallback syncCb)
{
    NS_LOG(DEBUG, "NSRegisterSyncCb - IN");
    g_syncCb = syncCb;
    NS_LOG(DEBUG, "NSRegisterSyncCb - OUT");
}

void NSSubscribeRequestCb(NSConsumer *consumer)
{
    NS_LOG(DEBUG, "NSSubscribeRequestCb - IN");
    g_subscribeRequestCb(consumer);
    NS_LOG(DEBUG, "NSSubscribeRequestCb - OUT");
}

void NSSyncCb(NSSyncInfo *sync)
{
    NS_LOG(DEBUG, "NSSyncCb - IN");
    g_syncCb(sync);
    NS_LOG(DEBUG, "NSSyncCb - OUT");
}

NSResult NSStartProvider(NSAccessPolicy policy, NSSubscribeRequestCallback subscribeRequestCb,
        NSProviderSyncInfoCallback syncCb)
{
    NS_LOG(DEBUG, "NSStartProvider - IN");

    initializeMutex();

    pthread_mutex_lock(&nsInitMutex);

    if (!initProvider)
    {
        NS_LOG(DEBUG, "Init Provider");
        initProvider = true;
        NSInitProviderInfo();
        NSSetSubscriptionAccessPolicy(policy);
        NSRegisterSubscribeRequestCb(subscribeRequestCb);
        NSRegisterSyncCb(syncCb);
        CARegisterNetworkMonitorHandler(NSProviderAdapterStateListener,
                NSProviderConnectionStateListener);

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
    pthread_mutex_init(&NSCacheMutex, NULL);
    NSInitSubscriptionList();
    NSInitMessageList();
    NS_LOG(DEBUG, "NSSetList - OUT");
}

NSResult NSStopProvider()
{
    NS_LOG(DEBUG, "NSStopProvider - IN");
    pthread_mutex_lock(&nsInitMutex);

    if(initProvider)
    {
        NSDeinitProviderInfo();
        NSUnRegisterResource();
        NSRegisterSubscribeRequestCb((NSSubscribeRequestCallback)NULL);
        NSRegisterSyncCb((NSProviderSyncInfoCallback)NULL);
        NSStopScheduler();
        NSStorageDestroy(consumerSubList);
        NSStorageDestroy(messageList);

        initProvider = false;
    }

    pthread_mutex_unlock(&nsInitMutex);
    NS_LOG(DEBUG, "NSStopProvider - OUT");
    return NS_OK;
}

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

NSResult NSAccept(NSConsumer *consumer, bool accepted)
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

void * NSInterfaceSchedule(void * ptr)
{
    if (ptr == NULL)
    {
        NS_LOG(DEBUG, "Create NSReponseSchedule");
    }

    while (NSIsRunning[INTERFACE_SCHEDULER])
    {
        sem_wait(&NSSemaphore[INTERFACE_SCHEDULER]);
        pthread_mutex_lock(&NSMutex[INTERFACE_SCHEDULER]);

        if (NSHeadMsg[INTERFACE_SCHEDULER] != NULL)
        {
            NSTask *node = NSHeadMsg[INTERFACE_SCHEDULER];
            NSHeadMsg[INTERFACE_SCHEDULER] = node->nextTask;

            switch (node->taskType)
            {
                case TASK_CB_SUBSCRIPTION:
                {
                    NS_LOG(DEBUG, "CASE TASK_CB_SUBSCRIPTION : ");

                    OCEntityHandlerRequest * request = (OCEntityHandlerRequest*)node->taskData;
                    NSConsumer * consumer = (NSConsumer *)OICMalloc(sizeof(NSConsumer));

                    char * consumerId = NSGetValueFromQuery(OICStrdup(request->query),
                            NS_QUERY_CONSUMER_ID);

                    if(consumerId)
                    {
                        OICStrcpy(consumer->consumerId, UUID_STRING_SIZE, consumerId);
                        NSSubscribeRequestCb(consumer);
                    }

                    NSFreeConsumer(consumer);
                    NSFreeOCEntityHandlerRequest(request);

                    break;
                }
                case TASK_CB_SYNC:
                {
                    NS_LOG(DEBUG, "CASE TASK_CB_SYNC : ");
                    NSSyncInfo * sync = (NSSyncInfo*)node->taskData;
                    NSSyncCb(NSDuplicateSync(sync));
                    NSFreeSync(sync);
                    break;
                }
                default:
                    NS_LOG(DEBUG, "No Task Type");
                    break;
            }
            OICFree(node);
        }

        pthread_mutex_unlock(&NSMutex[INTERFACE_SCHEDULER]);
    }

    NS_LOG(DEBUG, "Destroy NSResponseSchedule");
    return NULL;
}

