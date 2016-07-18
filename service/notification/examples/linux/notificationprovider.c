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
#include "NSCommon.h"
#include "NSProviderInterface.h"
#include "logger.h"
#include "octypes.h"
#include "pthread.h"
#include "oic_string.h"
#include "oic_malloc.h"

#define TAG "NSProviderExample"

// Input the following values to publish resource to cloud
char REMOTE_SERVER_ADDRESS[50];
char REMOTE_SERVER_SESSION[50];

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

    NSAccept(consumer, true);
}

void syncCallback(NSSyncInfo *sync)
{
    printf("sync requested");

    printf("NS_APP Sync State: %d\n", sync->state);
}

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

FILE* server_fopen(const char *path, const char *mode)
{
    (void)path;
    return fopen("oic_ns_provider_db.dat", mode);
}

int main()
{
    int num;
    pthread_t processThread;

    printf("NSStartProvider()");

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

        printf("1. NSStartProvider(Accepter: Provider) \n");
        printf("2. NSStartProvider(Accepter: Consumer) \n");
        printf("3. NSSendNotification() \n");
        printf("4. NSRead \n");
        printf("5. NSStopProvider() \n");

        printf("11. NSCloudLogin \n");
        printf("12. NSCloudLogout \n");
        printf("0. Exit() \n");

        printf("input : ");

        scanf("%d", &num);
        fflush(stdin);
        scanf("%c", &dummy);
        fflush(stdin);

        switch (num)
        {
            case 1:
                printf("NSStartProvider(Accepter: Provider)");
                NSStartProvider(NS_ACCESS_ALLOW, subscribeRequestCallback, syncCallback);
                break;
            case 2:
                printf("NSStartProvider(Accepter: Consumer)");
                NSStartProvider(NS_ACCESS_DENY, subscribeRequestCallback, syncCallback);
                break;
            case 3:
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

                break;

            case 4:
                printf("NSRead");
                NSSyncInfo * sync = (NSSyncInfo*) OICMalloc(sizeof(NSSyncInfo));

                sync->messageId = OICStrdup("dev_001");
                sync->state = 1;

                break;

            case 5:
                NSStopProvider();
                break;

            case 11:
                printf("NSCloudLogin");

                printf("Cloud Address: ");
                gets(REMOTE_SERVER_ADDRESS);

                printf("Session Code: ");
                gets(REMOTE_SERVER_SESSION);


                NSCloudLogin(REMOTE_SERVER_ADDRESS, REMOTE_SERVER_SESSION, CloudLoginoutCallback);
                printf("OCCloudLogin requested");
                break;
            case 12:
                printf("NSCloudLogout");

                NSCloudLogout(REMOTE_SERVER_ADDRESS, REMOTE_SERVER_SESSION, CloudLoginoutCallback);
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
