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
#include "NSCacheAdapter.h"
#include "cautilinterface.h"
#include "NSProviderSubscription.h"
#include "NSProviderNotification.h"

bool initProvider = false;
static NSSubscribeRequestCallback g_subscribeRequestCb = NULL;
static NSSyncCallback g_syncCb = NULL;
pthread_mutex_t NSCacheMutex;

void NSRegisterSubscribeRequestCb(NSSubscribeRequestCallback subscribeRequestCb)
{
    g_subscribeRequestCb = subscribeRequestCb;
}

void  NSRegisterSyncCb(NSSyncCallback syncCb)
{
    g_syncCb = syncCb;
}

void NSSubscribeRequestCb(NSConsumer *consumer)
{
    g_subscribeRequestCb(consumer);
}

void NSSyncCb(NSSync *sync)
{
    g_syncCb(sync);
}

NSResult NSStartProvider(NSAccessPolicy policy, NSSubscribeRequestCallback subscribeRequestCb,
        NSSyncCallback syncCb)
{
    OIC_LOG(INFO, INTERFACE_TAG, "Notification Service Start Provider..");

    NSSetSubscriptionAcceptPolicy(policy);

    if (!initProvider)
    {
        initProvider = true;

        NSSetList();
        NSInitScheduler();
        NSStartScheduler();
    }

    NSRegisterSubscribeRequestCb(subscribeRequestCb);
    NSRegisterSyncCb(syncCb);

    CARegisterNetworkMonitorHandler(NSProviderAdapterStateListener,
            NSProviderConnectionStateListener);

    NSPushQueue(DISCOVERY_SCHEDULER, TASK_START_PRESENCE, NULL);
    NSPushQueue(DISCOVERY_SCHEDULER, TASK_REGISTER_RESOURCE, NULL);

    return NS_OK;
}

void NSSetList()
{
    pthread_mutex_init(&NSCacheMutex, NULL);
    NSInitSubscriptionList();
    NSInitMessageList();
}

NSResult NSStopProvider()
{
    NSRegisterSubscribeRequestCb((NSSubscribeRequestCallback)NULL);
    NSRegisterSyncCb((NSSyncCallback)NULL);
    initProvider = false;
    return NS_OK;
}

NSResult NSSendNotification(NSMessage *msg)
{
    OIC_LOG(INFO, INTERFACE_TAG, "Send Notification");
    NSPushQueue(NOTIFICATION_SCHEDULER, TASK_SEND_NOTIFICATION, msg);
    return NS_OK;
}

NSResult NSProviderReadCheck(NSMessage *msg)
{
    OIC_LOG(INFO, INTERFACE_TAG, "Read Sync");
    NSPushQueue(NOTIFICATION_SCHEDULER, TASK_SEND_READ, msg);
    return NS_OK;
}

NSResult NSAccept(NSConsumer *consumer, bool accepted)
{
    OIC_LOG(INFO, INTERFACE_TAG, "Response Acceptance");

    if(accepted)
    {
        NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_SEND_ALLOW, consumer);
    }
    else
    {
        NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_SEND_DENY, consumer);
    }

    return NS_OK;
}

void * NSResponseSchedule(void * ptr)
{
    if (ptr == NULL)
    {
        OIC_LOG(INFO, INTERFACE_TAG, "Init NSResponseSchedule");
    }

    while (NSIsRunning[RESPONSE_SCHEDULER])
    {
        sem_wait(&NSSemaphore[RESPONSE_SCHEDULER]);
        pthread_mutex_lock(&NSMutex[RESPONSE_SCHEDULER]);

        if (NSHeadMsg[RESPONSE_SCHEDULER] != NULL)
        {
            NSTask *node = NSHeadMsg[RESPONSE_SCHEDULER];
            NSHeadMsg[RESPONSE_SCHEDULER] = node->nextTask;

            switch (node->taskType)
            {
                case TASK_CB_SUBSCRIPTION:
                {
                    printf("before - TASK_CB_SUBSCRIPTION :\n");
                    OCEntityHandlerRequest * request = (OCEntityHandlerRequest*)node->taskData;
                    NSConsumer consumer;

                    consumer.mId = strdup(request->devAddr.addr);
                    int * obId = (int *) OICMalloc(sizeof(int));
                    *obId = request->obsInfo.obsId;
                    consumer.mUserData = obId;

                    NSSubscribeRequestCb(&consumer);
                    printf("after - TASK_CB_SUBSCRIPTION :\n");
                    break;
                }
                case TASK_CB_SYNC:
                {
                    NSSync * sync = (NSSync*)node->taskData;
                    NSSyncCb(sync);
                    break;
                }
                default:
                    OIC_LOG(INFO, INTERFACE_TAG, "Response to User");

                    // TODO: NSSubscribeRequestCb

                    // TODO: NSSyncCb

                    break;

            }
            OICFree(node);
        }

        pthread_mutex_unlock(&NSMutex[RESPONSE_SCHEDULER]);

    }
    return NULL;
}

NSResult NSTestStartPresence()
{
    NSPushQueue(DISCOVERY_SCHEDULER, TASK_START_PRESENCE, NULL);
    return NS_OK;
}

NSResult NSTestStopPresence()
{
    NSPushQueue(DISCOVERY_SCHEDULER, TASK_STOP_PRESENCE, NULL);
    return NS_OK;
}


