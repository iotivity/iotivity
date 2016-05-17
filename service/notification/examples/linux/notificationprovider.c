#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "NSCommon.h"
#include "NSProviderInterface.h"
#include "logger.h"
#include "octypes.h"
#include "pthread.h"


#define TAG "notiProviderExample"

extern char *strdup(const char *s);

bool isExit = false;

pthread_t processThread;

int id;

void OCProcessThread(void * ptr)
{
    (void) ptr;
    while (!isExit)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return;
        }
    }
}

void subscribeRequestCallback(NSConsumer *consumer)
{
    OIC_LOG(INFO, TAG, "consumer requested to subscribe");
    char *cid = consumer->mId;

    printf("NS_ Consumer ID: %s\n", cid);

    NSAccept(consumer, true);
}

void syncCallback(NSSync *sync)
{
    OIC_LOG(INFO, TAG, "sync requested");

    printf("NS_ Sync State: %d\n", sync->mState);
}

int main()
{
    int num;

    OIC_LOG(INFO, TAG, "NSStartProvider()");

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        OIC_LOG(INFO, TAG, "OCStack init error");
        return 0;
    }

    pthread_create(&processThread, NULL, OCProcessThread, NULL);

    NSMessage * message = (NSMessage*) OICMalloc(sizeof(NSMessage));
    if (!message)
    {
        OIC_LOG(ERROR, TAG, PCF("Fail to allocate memory"));
        return NS_ERROR;
    }

    while (true)
    {
        char dummy;

        printf("1. NSStartProvider(Accepter: Provider) \n");
        printf("2. NSStartProvider(Accepter: Consumer) \n");
        printf("3. NSSendNotification() \n");
        printf("4. NSRead \n");
        //printf("5. NSAccept \n");
        printf("6. NSGetConsumerList \n");
        //printf("7. startPresence \n");
        //printf("8. stopPresence \n");
        printf("0. Exit() \n");

        printf("input : ");

        scanf("%d", &num);
        scanf("%c", &dummy);
        fflush(stdin);

        switch (num)
        {
            case 1:
                OIC_LOG(INFO, TAG, "NSStartProvider(Accepter: Provider)");
                NSStartProvider(NS_ACCEPTER_PROVIDER, subscribeRequestCallback, syncCallback);
                break;
            case 2:
                OIC_LOG(INFO, TAG, "NSStartProvider(Accepter: Consumer)");
                NSStartProvider(NS_ACCEPTER_CONSUMER, subscribeRequestCallback, syncCallback);
                break;
            case 3:
                OIC_LOG(INFO, TAG, "NSSendNotification()");

                char title[100];
                char body[100];
                char charID[100];

                printf("id : %d\n", ++id);

                printf("title : ");

                gets(title);

                printf("body : ");
                gets(body);

                printf("app - mId : %s \n", charID);
                printf("app - mTitle : %s \n", title);
                printf("app - mContentText : %s \n", body);


                NSMessage * msg = (NSMessage *)malloc(sizeof(NSMessage));

                sprintf(charID, "%d", id);

                msg->mId = strdup(charID);
                msg->mTitle = strdup(title);
                msg->mContentText = strdup(body);

                NSSendNotification(msg);

                break;

            case 4:
                OIC_LOG(INFO, TAG, "NSRead");
                NSSync * sync = (NSSync*) malloc(sizeof(NSSync));

                sync->mMessageId = strdup("dev_001");
                sync->mState = 1;

                NSSendSync(sync);

                break;
/*
            case 5:
                OIC_LOG(INFO, TAG, "NSAccept");

                NSConsumer * consumer = (NSConsumer *)malloc(sizeof(NSConsumer));
                consumer->mId = strdup("dev_001");
                consumer->mUserData = NULL;
                bool accepted = true;

                NSAccept(consumer, accepted);

                break;*/

            case 6:
                OIC_LOG(INFO, TAG, "NSGetConsumerList");

                /*
                NSConsumer *list = NULL;
                int numberOfList;

                NSGetConsumerList(list, numberOfList);

                if(list != NULL)
                {
                    OIC_LOG(INFO, TAG, "Consumer list");
                }
                else
                {
                    OIC_LOG(INFO, TAG, "No Consumer list");
                }
                    */
                break;
            case 7:
                OIC_LOG(INFO, TAG, "NSStartPresence - not working");
                //NSTestStartPresence();
                break;
            case 8:
                OIC_LOG(INFO, TAG, "NSStopPresence- not working");
                //NSTestStopPresence();
                break;
            default:
                OIC_LOG(INFO, TAG, "Under Construction");
                break;
        }

        printf("\n");
    }

    return 0;
}
