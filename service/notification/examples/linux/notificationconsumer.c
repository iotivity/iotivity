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
#include <string.h>
#include "pthread.h"

#include "ocstack.h"
#include "NSCommon.h"
#include "NSConsumerInterface.h"

#ifdef WITH_CLOUD
#include "NSConstants.h"
#include "oic_malloc.h"
#include "cloud_connector.h"

#define CLOUD_CONTEXT_VALUE 0x99

char CLOUD_ADDRESS[50];
char CLOUD_AUTH_PROVIDER[50];
char CLOUD_AUTH_CODE[50];
char CLOUD_UID[50];
char CLOUD_ACCESS_TOKEN[50];
#endif


NSProvider * g_provider = NULL;

FILE* server_fopen(const char *path, const char *mode)
{
    (void)path;
    return fopen("oic_ns_provider_db.dat", mode);
}

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

void onProviderChanged(NSProvider * provider, NSProviderState response)
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

    // open oic_db
    static OCPersistentStorage ps = {server_fopen, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit1(OC_CLIENT_SERVER, OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS) != OC_STACK_OK)
    {
        printf("OCInit fail\n");
        return 0;
    }

    NSConsumerConfig cfg;
    cfg.changedCb = onProviderChanged;
    cfg.messageCb = onNotificationPosted;
    cfg.syncInfoCb = onNotificationSync;

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
#ifdef WITH_CLOUD
        printf("21. Enable Remote Service (after login)\n");
        printf("31. Cloud Signup\n");
        printf("32. Cloud Login\n");
        printf("33. Cloud Logout\n");
#endif

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
#ifdef WITH_CLOUD
            case 21:
                printf("Enable Remote Service");
                if(!IsCloudLoggedin())
                {
                    printf("Cloud Login required");
                    break;
                }
                NSConsumerEnableRemoteService(CLOUD_ADDRESS);
                break;
            case 31:
                printf("Remote Server Address: ");
                gets(CLOUD_ADDRESS);

                printf("Auth Provider(eg. github): ");
                gets(CLOUD_AUTH_PROVIDER);

                printf("Auth Code: ");
                gets(CLOUD_AUTH_CODE);

                OCCloudSignup(CLOUD_ADDRESS, OCGetServerInstanceIDString(),
                    CLOUD_AUTH_PROVIDER, CLOUD_AUTH_CODE, CloudSignupCallback);
                printf("OCCloudSignup requested");
                break;
            case 32:
                printf("Remote Server Address: ");
                gets(CLOUD_ADDRESS);

                printf("UID: ");
                gets(CLOUD_UID);

                printf("ACCESS_TOKEN: ");
                gets(CLOUD_ACCESS_TOKEN);

                OCCloudLogin(CLOUD_ADDRESS, CLOUD_UID, OCGetServerInstanceIDString(),
                    CLOUD_ACCESS_TOKEN, CloudLoginoutCallback);
                printf("OCCloudLogin requested");
                break;
            case 33:
                OCCloudLogout(CLOUD_ADDRESS, CloudLoginoutCallback);
                printf("OCCloudLogin requested");
                break;
#endif
            default:
                break;
        }
    }
    return 0;
}
