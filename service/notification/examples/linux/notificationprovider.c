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
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "NSProviderInterface.h"
#include "NSCommon.h"
#include "logger.h"
#include "octypes.h"
#include "pthread.h"
#include "oic_string.h"
#include "oic_malloc.h"

#define TAG "NSProviderExample"

// Input the following values to publish resource to cloud
char REMOTE_SERVER_ADDRESS[50];
char REMOTE_SERVER_SESSION[50];
char mainConsumer[37] = {'\0',};

extern char *strdup(const char *s);

bool isExit = false;

int id;

void* OCProcessThread(void * ptr)
{
    (void) ptr;
    while (!isExit)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            printf("OCStack process error");
            return NULL;
        }
    }

    return NULL;
}

void subscribeRequestCallback(NSConsumer *consumer)
{
    printf("consumer requested to subscribe");

    printf("NS_APP Consumer Device ID: %s\n", consumer->consumerId);

    if(mainConsumer[0] == '\0')
    {
        OICStrcpy(mainConsumer, 37, consumer->consumerId);
    }

    NSAcceptSubscription(consumer, true);
}

void syncCallback(NSSyncInfo *sync)
{
    printf("sync requested");

    printf("NS_APP Sync State: %d\n", sync->state);
}

#ifdef WITH_CLOUD
OCStackApplicationResult CloudLoginoutCallback(void *ctx,
        OCDoHandle handle, OCClientResponse *clientResponse)
{
    int CtxValue = 0x99;
    if (ctx != (void *)CtxValue)
    {
        printf("Invalid Cloud Login/out callback received");
    }

    printf("Login/out response received");

    if (clientResponse->payload != NULL &&
            clientResponse->payload->type == PAYLOAD_TYPE_REPRESENTATION)
    {
        printf("PAYLOAD_TYPE_REPRESENTATION received");

        OCRepPayloadValue *val = ((OCRepPayload *)clientResponse->payload)->values;

        printf("Get payload values");
        while (val)
        {
            printf("key: %s / Value: %s", val->name, val->str);
            val = val->next;
        }

        NSProviderEnableRemoteService(REMOTE_SERVER_ADDRESS);
    }

    return OC_STACK_KEEP_TRANSACTION;
}
#endif

FILE* server_fopen(const char *path, const char *mode)
{
    (void)path;
    return fopen("oic_ns_provider_db.dat", mode);
}

void printTopics(NSTopicLL * topics)
{
    if(!topics)
    {
        printf("topics is null\n");
        return;
    }

    NSTopicLL * iter = topics;

    while(iter)
    {
        printf("tName = %s, tState = %d\n", iter->topicName, (int)iter->state);
        iter = iter->next;
    }
}

void removeTopics(NSTopicLL * topics)
{
    if(!topics)
    {
        printf("topics is null\n");
        return;
    }

    NSTopicLL * iter = topics;

    while(iter)
    {
        NSTopicLL * del = iter;
        if(del->topicName)
        {
            OICFree(del->topicName);
        }
        iter = iter->next;

        OICFree(del);
    }
}

int main()
{
    int num;
    pthread_t processThread;

    printf("NSStartProvider()\n\n");

    // open oic_db
    static OCPersistentStorage ps = {server_fopen, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        printf("OCStack init error");
        return 0;
    }

    pthread_create(&processThread, NULL, OCProcessThread, unlink);

    while (!isExit)
    {
        char dummy;

        printf("==============================================\n");
        printf("1.  NSStartProvider(Accepter: Provider) \n");
        printf("2.  NSStartProvider(Accepter: Consumer) \n");
        printf("3.  NSSendNotification() \n");
        printf("4.  NSRead() \n");
        printf("5.  NSProviderAddTopic(); \n");
        printf("6.  NSProviderDeleteTopic(); \n");
        printf("7.  NSProviderSelectTopic(); \n");
        printf("8.  NSProviderUnselectTopic(); \n");
        printf("9.  NSProviderGetConsumerTopics(); \n");
        printf("10. NSProviderGetTopics(); \n");
        printf("11. NSStopProvider() \n");
        printf("12. NSCloudLogin \n");
        printf("13. NSCloudLogout \n");
        printf("0. Exit() \n");
        printf("==============================================\n");

        printf("input : ");

        scanf("%d", &num);
        fflush(stdin);
        scanf("%c", &dummy);
        fflush(stdin);

        switch (num)
        {
            case 1:
            {
                printf("NSStartProvider(Accepter: Provider)");
                NSProviderConfig config;
                config.policy = true;
                config.subRequestCallback = subscribeRequestCallback;
                config.syncInfoCallback = syncCallback;
                config.userInfo = OICStrdup("OCF_NOTIFICATION");
                NSStartProvider(config);
            }
                break;

            case 2:
            {
                printf("NSStartProvider(Accepter: Consumer)");
                NSProviderConfig config;
                config.policy = false;
                config.subRequestCallback = subscribeRequestCallback;
                config.syncInfoCallback = syncCallback;
                config.userInfo = OICStrdup("OCF_NOTIFICATION");
                NSStartProvider(config);
            }
                break;

            case 3:
            {
                printf("NSSendNotification()");
                char title[100];
                char body[100];

                printf("id : %d\n", ++id);
                printf("title : ");

                gets(title);

                printf("body : ");
                gets(body);

                printf("app - mTitle : %s \n", title);
                printf("app - mContentText : %s \n", body);

                NSMessage * msg = NSCreateMessage();

                msg->title = OICStrdup(title);
                msg->contentText = OICStrdup(body);
                msg->sourceName = OICStrdup("OCF");

                NSSendMessage(msg);
            }
                break;

            case 4:
                printf("NSRead\n");
                break;

            case 5:
                printf("NSProviderAddTopic\n");
                NSProviderAddTopic("OCF_TOPIC1");
                NSProviderAddTopic("OCF_TOPIC2");
                NSProviderAddTopic("OCF_TOPIC3");
                NSProviderAddTopic("OCF_TOPIC4");
                break;

            case 6:
                printf("NSProviderDeleteTopic\n");
                NSProviderDeleteTopic("OCF_TOPIC2");
                break;

            case 7:
                printf("NSProviderSelectTopic\n");
                NSProviderSelectTopic(mainConsumer, "OCF_TOPIC1");
                NSProviderSelectTopic(mainConsumer, "OCF_TOPIC2");
                NSProviderSelectTopic(mainConsumer, "OCF_TOPIC3");
                NSProviderSelectTopic(mainConsumer, "OCF_TOPIC4");
                break;

            case 8:
                printf("NSProviderUnSelectTopic\n");
                NSProviderUnselectTopic(mainConsumer, "OCF_TOPIC1");
                break;

            case 9:
                printf("NSProviderGetConsumerTopics\n");
                {
                    NSTopicLL * topics = NSProviderGetConsumerTopics(mainConsumer);
                    printTopics(topics);
                    removeTopics(topics);
                }
                break;

            case 10:
                printf("NSProviderGetConsumerTopics\n");
                {
                    NSTopicLL * topics = NSProviderGetTopics();
                    printTopics(topics);
                    removeTopics(topics);
                }
                break;

            case 11:
                NSStopProvider();
                break;

            case 12:
                printf("NSCloudLogin");

                printf("Cloud Address: ");
                gets(REMOTE_SERVER_ADDRESS);

                printf("Session Code: ");
                gets(REMOTE_SERVER_SESSION);

#ifdef WITH_CLOUD
                NSCloudLogin(REMOTE_SERVER_ADDRESS, REMOTE_SERVER_SESSION, CloudLoginoutCallback);
#endif
                printf("OCCloudLogin requested");
                break;
            case 13:
                printf("NSCloudLogout");
#ifdef WITH_CLOUD
                NSCloudLogout(REMOTE_SERVER_ADDRESS, REMOTE_SERVER_SESSION, CloudLoginoutCallback);
#endif
                printf("OCCloudLogout requested");
                break;
            case 0:
                NSStopProvider();
                isExit = true;
                break;
            default:
                printf("Under Construction");
                break;
        }

        printf("\n");
    }

    return 0;
}
