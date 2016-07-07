/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

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
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return NULL;
        }
    }

    return NULL;
}

void subscribeRequestCallback(NSConsumer *consumer)
{
    OIC_LOG(INFO, TAG, "consumer requested to subscribe");

    printf("NS_APP Consumer Device ID: %s\n", consumer->consumerId);

    NSAccept(consumer, true);
}

void syncCallback(NSSyncInfo *sync)
{
    OIC_LOG(INFO, TAG, "sync requested");

    printf("NS_APP Sync State: %d\n", sync->state);
}

OCStackApplicationResult CloudLoginoutCallback(void *ctx,
        OCDoHandle handle, OCClientResponse *clientResponse)
{
    int CtxValue = 0x99;
    if (ctx != (void *)CtxValue)
    {
        OIC_LOG(INFO, TAG, "Invalid Cloud Login/out callback received");
    }

    OIC_LOG(INFO, TAG, "Login/out response received");

    if (clientResponse->payload != NULL &&
            clientResponse->payload->type == PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(INFO, TAG, "PAYLOAD_TYPE_REPRESENTATION received");

        OCRepPayloadValue *val = ((OCRepPayload *)clientResponse->payload)->values;

        OIC_LOG(INFO, TAG, "Get payload values");
        while (val)
        {
            OIC_LOG_V(INFO, TAG, "key: %s / Value: %s", val->name, val->str);
            val = val->next;
        }

        char *serverAddress = "coap+tcp://52.69.149.85:5683";
        NSProviderEnableRemoteService(serverAddress);
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

    // cloud host address
    const char *host = "coap+tcp://52.69.149.85:5683";
    // cloud auth session
    const char *auth_session = "gZDRuDyYapZXIcrs";


    OIC_LOG(INFO, TAG, "NSStartProvider()");

    // open oic_db
    static OCPersistentStorage ps = {server_fopen, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        OIC_LOG(INFO, TAG, "OCStack init error");
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
        printf("6. NSGetConsumerList \n");
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
                OIC_LOG(INFO, TAG, "NSStartProvider(Accepter: Provider)");
                NSStartProvider(NS_ACCESS_ALLOW, subscribeRequestCallback, syncCallback);
                break;
            case 2:
                OIC_LOG(INFO, TAG, "NSStartProvider(Accepter: Consumer)");
                NSStartProvider(NS_ACCESS_DENY, subscribeRequestCallback, syncCallback);
                break;
            case 3:
                OIC_LOG(INFO, TAG, "NSSendNotification()");

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
                OIC_LOG(INFO, TAG, "NSRead");
                NSSyncInfo * sync = (NSSyncInfo*) OICMalloc(sizeof(NSSyncInfo));

                sync->messageId = OICStrdup("dev_001");
                sync->state = 1;

                break;

            case 5:
                NSStopProvider();
                break;
            case 6:
                OIC_LOG(INFO, TAG, "NSGetConsumerList");
                break;
            case 11:
                OIC_LOG(INFO, TAG, "NSCloudLogin");

                NSCloudLogin(host, auth_session, CloudLoginoutCallback);
                OIC_LOG(INFO, TAG, "OCCloudLogin requested");
                break;
            case 12:
                OIC_LOG(INFO, TAG, "NSCloudLogout");

                NSCloudLogout(host, auth_session, CloudLoginoutCallback);
                OIC_LOG(INFO, TAG, "OCCloudLogout requested");
                break;
            case 0:
                NSStopProvider();
                isExit = true;
                break;
            default:
                OIC_LOG(INFO, TAG, "Under Construction");
                break;
        }

        printf("\n");
    }

    return 0;
}
