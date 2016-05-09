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

#include "NSProviderNotification.h"

NSResult NSGetObsIdList(OCObservationId *list)
{
    // // TODO: return white list

    list = NULL;

    return NS_OK;
}

uint32_t NSGetNumberOfObsList()
{
    // TODO: return # of list

    return 0;
}

NSResult NSGetMessagePayload(NSMessage *msg, OCRepPayload** msgPayload)
{

    *msgPayload = OCRepPayloadCreate();

    if (!*msgPayload)
    {
        OIC_LOG(ERROR, NOTIFICATION_TAG, PCF("Failed to allocate payload"));
        return NS_ERROR;
    }

    OCRepPayloadSetUri(*msgPayload, NSGetNotificationMessageUri());
    OCRepPayloadSetPropString(*msgPayload, NS_ATTRIBUTE_ID, msg->mId);
    OCRepPayloadSetPropString(*msgPayload, NS_ATTRIBUTE_TITLE, msg->mTitle);
    OCRepPayloadSetPropString(*msgPayload, NS_ATTRIBUTE_TEXT, msg->mContentText);

    return NS_OK;
}

NSResult NSGetSyncPayload(NSSync *sync, OCRepPayload** syncPayload)
{

    *syncPayload = OCRepPayloadCreate();

    if (!*syncPayload)
    {
        OIC_LOG(ERROR, NOTIFICATION_TAG, PCF("Failed to allocate payload"));
        return NS_ERROR;
    }

    OCRepPayloadSetUri(*syncPayload, NSGetNotificationSyncUri());
    OCRepPayloadSetPropString(*syncPayload, NS_ATTRIBUTE_ID, sync->mMessageId);
    OCRepPayloadSetPropInt(*syncPayload, NS_ATTRIBUTE_STATE, sync->mState);

    return NS_OK;
}

NSResult NSSendMessage(NSMessage *msg)
{
    OIC_LOG(DEBUG, NOTIFICATION_TAG, "Send Notification Message to consumer");
    int i;
    // Set Resource and get resource handle
    OCResourceHandle rHandle;
    if (NSPutMessageResource(msg, &rHandle) != NS_OK)
    {
        OIC_LOG(ERROR, NOTIFICATION_TAG, PCF("Fail to put notification resource"));
        return NS_ERROR;
    }

    // Send Notification to subscribers

    NSWhiteList * whiteList = NSProviderReadCache(NS_CONSUMER_WHITELIST, 0);

    if(whiteList->size == 0)
    {
        printf("printf - no observers\n");
        OIC_LOG(ERROR, NOTIFICATION_TAG, PCF("no observers"));
        return NS_ERROR;
    }

    OCRepPayload* payload;
    printf("printf - no observers - 1\n");
    if (NSGetMessagePayload(msg, &payload) != NS_OK)
    {
        printf("printf - no observers - 2\n");
        OIC_LOG(ERROR, NOTIFICATION_TAG, PCF("Failed to allocate payload"));
        return NS_ERROR;
    }

    for(i = 0; i < whiteList->size; ++i)
    {
        printf("NS_ -------------------------------------------------------message\n");
        printf("NS_ whiteList->idList[%d] = %d\n", i, whiteList->idList[i]);
        printf("NS_ -------------------------------------------------------message\n");
    }

    // Notify message to subscribers

    OCStackResult ocstackResult = OCNotifyListOfObservers(rHandle, whiteList->idList, whiteList->size, payload, OC_LOW_QOS);
    printf("NS_ message ocstackResult = %d\n", ocstackResult);


    if (ocstackResult != OC_STACK_OK)
    {
        printf("printf - no observers - 3\n");
        OIC_LOG(ERROR, NOTIFICATION_TAG, "fail to send message");
        OCRepPayloadDestroy(payload);
        return NS_ERROR;

    }
    printf("printf - no observers - 4\n");
    OCRepPayloadDestroy(payload);

    return NS_OK;
}

NSResult NSSendSync(NSSync *sync)
{
    OIC_LOG(DEBUG, NOTIFICATION_TAG, "Send Notification Sync to consumer");
    int i;
    // Set Resource and get resource handle

    OCResourceHandle rHandle;
    if (NSPutSyncResource(sync, &rHandle) != NS_OK)
    {
        OIC_LOG(ERROR, NOTIFICATION_TAG, PCF("Fail to put sync resource"));
        return NS_ERROR;
    }

    NSWhiteList * whiteList = NSProviderReadCache(NS_CONSUMER_WHITELIST, 1);

    if(whiteList->size == 0)
    {
        OIC_LOG(ERROR, NOTIFICATION_TAG, PCF("no observers"));
        return NS_ERROR;
    }

    // Send sync to subscribers

    OCRepPayload* payload;
    if (NSGetSyncPayload(sync, &payload) != NS_OK)
    {
        OIC_LOG(ERROR, NOTIFICATION_TAG, PCF("Failed to allocate payload"));
        return NS_ERROR;
    }

    // Notify sync to subscribers

    for(i = 0; i < whiteList->size; ++i)
    {
        printf("NS_ -------------------------------------------------------sync\n");
        printf("NS_ whiteList->idList[%d] = %d\n", i, whiteList->idList[i]);
        printf("NS_ -------------------------------------------------------sync\n");
    }

    OCStackResult ocstackResult = OCNotifyListOfObservers(rHandle, whiteList->idList, whiteList->size, payload, OC_LOW_QOS);

    printf("NS_ sync ocstackResult = %d\n", ocstackResult);
    if (ocstackResult != OC_STACK_OK)
    {
        OIC_LOG(ERROR, NOTIFICATION_TAG, "fail to send Sync");
        OCRepPayloadDestroy(payload);
        return NS_ERROR;

    }
    OCRepPayloadDestroy(payload);

    return NS_OK;
}

void * NSNotificationSchedule(void *ptr)
{
    if (ptr == NULL)
    {
        OIC_LOG(DEBUG, NOTIFICATION_TAG, "Create NSNotifiactionSchedule");
    }

    while (NSIsRunning[NOTIFICATION_SCHEDULER])
    {
        sem_wait(&NSSemaphore[NOTIFICATION_SCHEDULER]);
        pthread_mutex_lock(&NSMutex[NOTIFICATION_SCHEDULER]);

        if (NSHeadMsg[NOTIFICATION_SCHEDULER] != NULL)
        {
            NSTask *node = NSHeadMsg[NOTIFICATION_SCHEDULER];
            NSHeadMsg[NOTIFICATION_SCHEDULER] = node->nextTask;

            switch (node->taskType)
            {
                case TASK_SEND_NOTIFICATION:
                {
                    NSMessage * nsMsg = node->taskData;
                    NSSendMessage(nsMsg);
                    break;
                }
                case TASK_SEND_READ:
                    NSSendSync((NSSync*) node->taskData);
                    break;
                case TASK_RECV_READ:
                    NSSendSync((NSSync*) node->taskData);
                    NSPushQueue(RESPONSE_SCHEDULER, TASK_CB_SYNC, node->taskData);
                    break;

                default:
                    OIC_LOG(ERROR, NOTIFICATION_TAG, "Unknown type message");
                    break;

            }
            OICFree(node);
        }

        pthread_mutex_unlock(&NSMutex[NOTIFICATION_SCHEDULER]);

    }
    return NULL;
}
