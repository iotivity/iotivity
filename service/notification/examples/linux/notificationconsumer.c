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

#include <stdio.h>
#include <unistd.h>
#include "pthread.h"

#include "ocstack.h"
#include "NSCommon.h"
#include "NSConsumerInterface.h"

#ifdef WITH_CLOUD
#include "NSConstants.h"
#include "NSConsumerCommon.h"
#include "oic_malloc.h"

#define CLOUD_CONTEXT_VALUE 0x99
#define CLOUD_PRESENCE_SUBSCRIBE_QUERY ""          // refer to IoTivity Cloud Module Sample

#define CLOUD_HOST_ADDRESS ""                      // refer to IoTivity Cloud Module Sample
#define CLOUD_IOTIVITYNS_SESSION ""                // refer to IoTivity Cloud Module Sample
#endif

NSProvider * g_provider = NULL;

void onDiscoverNotification(NSProvider * provider)
{
    printf("notification resource discovered\n");
    printf("subscribe result %d\n", NSSubscribe(provider));
    printf("startSubscribing\n");
}

void printProviderTopicList(NSProvider *provider)
{
    printf("printProviderTopicList\n");
    if (provider->topicLL)
    {
        NSTopicLL * iter = provider->topicLL;
        while (iter)
        {
            printf("Topic Name: %s\t Topic State: %d\n", iter->topicName, iter->state);
            iter = iter->next;
        }
    }
}

void onProviderChanged(NSProvider * provider, NSResponse response)
{
    printf("Provider changed: %d\n", response);
    printf("subscribed provider Id : %s\n", provider->providerId);

    if (response == NS_TOPIC)
    {
        printf ("Provider Topic Updated\n");

        printProviderTopicList(provider);
        g_provider = provider;
    }
}

void onNotificationPosted(NSMessage * notification)
{
    printf("id : %lld\n", (long long int)notification->messageId);
    printf("title : %s\n", notification->title);
    printf("content : %s\n", notification->contentText);
    printf("source : %s\n", notification->sourceName);
    if (notification->topic && strlen(notification->topic) > 0)
    {
        printf("topic : %s\n", notification->topic);
    }
    NSConsumerSendSyncInfo(notification->providerId, notification->messageId, NS_SYNC_READ);
}

void onNotificationSync(NSSyncInfo * sync)
{
    printf("Sync ID : %lld\n", (long long int)sync->messageId);
    printf("Sync STATE : %d\n", sync->state);
}

#ifdef WITH_CLOUD
OCStackApplicationResult handleLoginoutCB(void *ctx,
        OCDoHandle handle,
        OCClientResponse *clientResponse)
{
    (void)handle;
    if (ctx != (void *)CLOUD_CONTEXT_VALUE)
    {
        NS_LOG(DEBUG, "Invalid Login/out callback received");
    }

    NS_LOG(DEBUG, "Login/out response received");

    if (clientResponse->payload != NULL &&
        clientResponse->payload->type == PAYLOAD_TYPE_REPRESENTATION)
    {
        NS_LOG(DEBUG, "PAYLOAD_TYPE_REPRESENTATION received");

        OCRepPayloadValue *val = ((OCRepPayload *)clientResponse->payload)->values;

        while (val)
        {
            val = val->next;
        }
        NS_LOG(DEBUG, "Get payload values");

        OCDevAddr * addr = NULL;
        addr = (OCDevAddr *) OICMalloc(sizeof(OCDevAddr));
        memcpy(addr, clientResponse->addr, sizeof(OCDevAddr));

        NSTask * task = NSMakeTask(TASK_EVENT_CONNECTED_TCP, addr);

        NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(task, OC_STACK_KEEP_TRANSACTION, NSOICFree(addr));
        NSConsumerPushEvent(task);
    }

    return OC_STACK_KEEP_TRANSACTION;
}
#endif

void* OCProcessThread(void * ptr)
{
    (void) ptr;

    while (true)
    {
        usleep(2000);
        if(OCProcess() != OC_STACK_OK)
        {
            OCStop();
            break;
        }
    }

    return NULL;
}

int main(void)
{
    bool isExit = false;
    pthread_t OCThread = NULL;

    printf("start Iotivity\n");
    if (OCInit1(OC_CLIENT, OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS) != OC_STACK_OK)
    {
        printf("OCInit fail\n");
        return 0;
    }

    NSConsumerConfig cfg;
    cfg.discoverCb = onDiscoverNotification;
    cfg.changedCb = onProviderChanged;
    cfg.messageCb = onNotificationPosted;
    cfg.syncInfoCb = onNotificationSync;

#ifdef WITH_CLOUD
    NS_LOG(DEBUG, "process OCCloudLogin...");
    NS_LOG(DEBUG, "OCCloudLogin return");
#endif

    pthread_create(&OCThread, NULL, OCProcessThread, NULL);

    printf("start notification consumer service\n");
    while (!isExit)
    {
        int num = 0;
        char dummy = '\0';

        printf("1. Start Consumer\n");
        printf("2. Stop Consumer\n");
        printf("3. Get Topics\n");
        printf("4. Select Topics\n");
        printf("5. Exit\n");

        printf("Input: ");

        scanf("%d", &num);
        fflush(stdin);
        scanf("%c", &dummy);
        fflush(stdin);

        switch (num)
        {
            case 1:
                printf("1. Start Consumer\n");
                NSStartConsumer(cfg);
                break;
            case 2:
                printf("2. Stop Consumer");
                NSStopConsumer();
                break;
            case 3:
                printf("3. Get Topics\n");
                if(g_provider)
                {
                    NSConsumerGetInterestTopics(g_provider);
                }
                break;
            case 4:
                printf("4. Select Topics\n");

                if (g_provider && g_provider->topicLL)
                {
                    NSTopicLL * iter = g_provider->topicLL;
                    int i = 0;
                    while (iter)
                    {
                        iter->state = (i++)%2;
                        printf("Topic Name: %s\t Topic State: %d\n", iter->topicName, iter->state);
                        iter = iter->next;
                    }
                    NSConsumerSelectInterestTopics(g_provider);
                }
                break;
            case 5:
                printf("5. Exit");
                isExit = true;
                break;
            default:
                break;
        }
    }
    return 0;
}
