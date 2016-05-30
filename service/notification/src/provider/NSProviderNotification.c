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

NSResult NSInitMessageList()
{
    NS_LOG(DEBUG, "NSInitMessageList - IN");

    messageList = NSCacheCreate();
    messageList->cacheType = NS_PROVIDER_CACHE_MESSAGE;

    NS_LOG(DEBUG, "NSInitMessageList - OUT");
    return NS_OK;
}

NSResult NSGetMessagePayload(NSMessage *msg, OCRepPayload** msgPayload)
{
    NS_LOG(DEBUG, "NSGetMessagePayload - IN");

    *msgPayload = OCRepPayloadCreate();

    if (!*msgPayload)
    {
        NS_LOG(ERROR, "Failed to allocate payload");
        return NS_ERROR;
    }

    OCRepPayloadSetUri(*msgPayload, NSGetNotificationMessageUri());
    if(msg->mId)
        OCRepPayloadSetPropString(*msgPayload, NS_ATTRIBUTE_ID, msg->mId);
    if(msg->mTitle)
        OCRepPayloadSetPropString(*msgPayload, NS_ATTRIBUTE_TITLE, msg->mTitle);
    if(msg->mContentText)
        OCRepPayloadSetPropString(*msgPayload, NS_ATTRIBUTE_TEXT, msg->mContentText);
    if(msg->mSource)
        OCRepPayloadSetPropString(*msgPayload, NS_ATTRIBUTE_SOURCE, msg->mSource);

    NS_LOG(DEBUG, "NSGetMessagePayload - OUT");
    return NS_OK;
}

NSResult NSGetSyncPayload(NSSync *sync, OCRepPayload** syncPayload)
{
    NS_LOG(DEBUG, "NSGetSyncPayload - IN");

    *syncPayload = OCRepPayloadCreate();

    if (!*syncPayload)
    {
        NS_LOG(ERROR, "Failed to allocate payload");
        return NS_ERROR;
    }

    OCRepPayloadSetUri(*syncPayload, NSGetNotificationSyncUri());

    if(sync->mMessageId)
    {
        OCRepPayloadSetPropString(*syncPayload, NS_ATTRIBUTE_ID, sync->mMessageId);
        OCRepPayloadSetPropInt(*syncPayload, NS_ATTRIBUTE_STATE, sync->mState);
    }

    NS_LOG(DEBUG, "NSGetSyncPayload - OUT");
    return NS_OK;
}

NSResult NSSendMessage(NSMessage *msg)
{
    NS_LOG(DEBUG, "NSSendMessage - IN");

    OCResourceHandle rHandle;
    OCObservationId obArray[255] = { 0, };
    int obCount = 0, i;

    if (NSPutMessageResource(msg, &rHandle) != NS_OK)
    {
        NS_LOG(ERROR, "fail to Put notification resource");
        return NS_ERROR;
    }

    if (consumerSubList->head == NULL)
    {
        NS_LOG(ERROR, "SubList->head is NULL, empty SubList");
        return NS_ERROR;
    }

    OCRepPayload* payload;

    if (NSGetMessagePayload(msg, &payload) != NS_OK)
    {
        NS_LOG(ERROR, "fail to Get message payload");
        return NS_ERROR;
    }

    NSCacheElement * it = consumerSubList->head;

    while (it)
    {
        NSCacheSubData * subData = (NSCacheSubData *) it->data;
        NS_LOG_V(DEBUG, "subData->id = %s", subData->id);
        NS_LOG_V(DEBUG, "subData->messageId = %d", subData->messageObId);
        NS_LOG_V(DEBUG, "subData->obID = %d", subData->syncObId);
        NS_LOG_V(DEBUG, "subData->isWhite = %d", subData->isWhite);

        if (subData->isWhite)
        {
            obArray[obCount++] = subData->messageObId;
        }

        it = it->next;
    }

    NS_LOG_V(DEBUG, "observer Count = %d", obCount);

    for (i = 0; i < obCount; ++i)
    {
        NS_LOG(DEBUG, "-------------------------------------------------------message\n");
        NS_LOG_V(DEBUG, "SubScription WhiteList[%d] = %d", i, obArray[i]);
        NS_LOG(DEBUG, "-------------------------------------------------------message\n");
    }

    OCStackResult ocstackResult = OCNotifyListOfObservers(rHandle, obArray, obCount, payload,
            OC_LOW_QOS);

    NS_LOG_V(DEBUG, "Message ocstackResult = %d", ocstackResult);

    if (ocstackResult != OC_STACK_OK)
    {
        NS_LOG(ERROR, "fail to send message");
        OCRepPayloadDestroy(payload);
        return NS_ERROR;
    }
    OCRepPayloadDestroy(payload);
    NSFreeMessage(msg);

    NS_LOG(DEBUG, "NSSendMessage - OUT");

    return NS_OK;
}

NSResult NSSendSync(NSSync *sync)
{
    NS_LOG(DEBUG, "NSSendSync - IN");

    OCObservationId obArray[255] = { 0, };
    int obCount = 0;
    int i;

    OCResourceHandle rHandle;
    if (NSPutSyncResource(sync, &rHandle) != NS_OK)
    {
        NS_LOG(ERROR, PCF("Fail to put sync resource"));
        return NS_ERROR;
    }

    NSCacheElement * it = consumerSubList->head;

    while (it)
    {
        NSCacheSubData * subData = (NSCacheSubData *) it->data;
        if (subData->isWhite)
        {
            obArray[obCount++] = subData->syncObId;
        }

        it = it->next;

    }

    OCRepPayload* payload;
    if (NSGetSyncPayload(sync, &payload) != NS_OK)
    {
        NS_LOG(ERROR, "Failed to allocate payload");
        return NS_ERROR;
    }

    for (i = 0; i < obCount; ++i)
    {
        NS_LOG(DEBUG, "-------------------------------------------------------message\n");
        NS_LOG_V(DEBUG, "Sync WhiteList[%d] = %d", i, obArray[i]);
        NS_LOG(DEBUG, "-------------------------------------------------------message\n");
    }

    OCStackResult ocstackResult = OCNotifyListOfObservers(rHandle, obArray,
            obCount, payload, OC_LOW_QOS);

    NS_LOG_V(DEBUG, "Sync ocstackResult = %d", ocstackResult);

    if (ocstackResult != OC_STACK_OK)
    {
        NS_LOG(ERROR, "fail to send Sync");
        OCRepPayloadDestroy(payload);
        return NS_ERROR;

    }

    OCRepPayloadDestroy(payload);

    NS_LOG(DEBUG, "NSSendSync - OUT");
    return NS_OK;
}

void * NSNotificationSchedule(void *ptr)
{
    if (ptr == NULL)
    {
        NS_LOG(DEBUG, "Create NSNotifiactionSchedule");
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
                    NS_LOG(DEBUG, "CASE TASK_SEND_NOTIFICATION : ");
                    NSSendMessage((NSMessage *)node->taskData);
                    break;
                }
                case TASK_SEND_READ:
                    NS_LOG(DEBUG, "CASE TASK_SEND_READ : ");
                    NSSendSync((NSSync*) node->taskData);
                    break;
                case TASK_RECV_READ:
                    NS_LOG(DEBUG, "CASE TASK_RECV_READ : ");
                    NSSendSync((NSSync*) node->taskData);
                    NSPushQueue(RESPONSE_SCHEDULER, TASK_CB_SYNC, node->taskData);
                    break;

                default:
                    NS_LOG(ERROR, "Unknown type message");
                    break;

            }
            OICFree(node);
        }

        pthread_mutex_unlock(&NSMutex[NOTIFICATION_SCHEDULER]);

    }

    NS_LOG(INFO, "Destroy NSNotificationSchedule");
    return NULL;
}
