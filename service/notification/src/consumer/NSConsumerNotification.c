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

#include "NSConsumerNotification.h"

#include "NSConstants.h"
#include "NSConsumerCommon.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocpayload.h"

const char NS_MESSAGE_ACCEPTANCE[] = "0000-0000-0000-0000";

NSMessage_consumer * NSBuildOICNotification(OCClientResponse * clientResponse);
NSSync * NSBuildOICNotificationSync(OCClientResponse * clientResponse);

NSProvider * NSGetProvider(OCClientResponse * clientResponse);
NSResult NSPushToCache(OCClientResponse * clientResponse, NSTaskType type);

NSResult NSConsumerSubscribeProvider(NSProvider * provider)
{
    if (OC_STACK_OK != NSSendRequest(&(provider->messageHandle),
            OC_REST_OBSERVE, (OCDevAddr *) provider->mUserData,
            provider->messageUri, NULL, NSConsumerNotificationListener))
    {
        return NS_ERROR;
    }

    if (OC_STACK_OK != NSSendRequest(&(provider->syncHandle),
            OC_REST_OBSERVE, (OCDevAddr *) provider->mUserData,
            provider->syncUri, NULL, NSConsumerSyncListener))
    {
        return NS_ERROR;
    }

    return NS_OK;
}

NSResult NSConsumerPostProvider(OCDevAddr * addr, OCPayload * payload, const char * uri)
{

    if (OC_STACK_OK != NSSendRequest(NULL, OC_REST_POST, addr,
            uri, payload, NULL))
    {
        return NS_ERROR;
    }

    return NS_OK;
}

OCStackApplicationResult NSConsumerSyncListener(
        OCDoHandle handle, OCClientResponse * clientResponse)
{
    (void) handle;

    NSSync * newNoti = NULL;
    NSProvider * provider = NSGetProvider(clientResponse);
    if (!provider)
    {
        NS_LOG(ERROR, "getting provider is failed");
        return OC_STACK_KEEP_TRANSACTION;
    }

    newNoti = NSBuildOICNotificationSync(clientResponse);
    if (!newNoti)
    {
        return OC_STACK_KEEP_TRANSACTION;
    }

    NSTaskType taskType = TASK_RECV_READ;

    if (newNoti->mState != Notification_Read)
    {
        NS_LOG(DEBUG, "newNoti->type : Dismiss");
        taskType = TASK_RECV_DISMISS;
    }
    else
    {
        NS_LOG(DEBUG, "newNoti->type : Read");
    }

    NSNotificationSync(provider, newNoti);

    if (NS_OK != NSPushToCache(clientResponse, taskType))
    {
        return OC_STACK_KEEP_TRANSACTION;
    }

    return OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult NSConsumerNotificationListener(
        OCDoHandle handle, OCClientResponse * clientResponse)
{
    (void) handle;

    NSProvider * provider = NSGetProvider(clientResponse);
    if (!provider)
    {
        NS_LOG(ERROR, "getting provider is failed");
        return OC_STACK_KEEP_TRANSACTION;
    }

    NSMessage_consumer * newNoti = NSBuildOICNotification(clientResponse);
    if (!newNoti)
    {
        return OC_STACK_KEEP_TRANSACTION;
    }

    if (!strcmp(newNoti->mId, NS_MESSAGE_ACCEPTANCE))
    {
        NS_LOG(DEBUG, "Receive Subscribe confirm");
        OICFree(provider->mUserData);
        OICFree(provider);
        NSRemoveMessage(newNoti);
        return OC_STACK_KEEP_TRANSACTION;
    }

    NSTaskType taskType = TASK_CONSUMER_RECV_NOTIFICATION;

    NS_LOG(DEBUG, "newNoti->type == Notification");
    NSNotificationPost(provider, (NSMessage *) newNoti);

    if (NS_OK != NSPushToCache(clientResponse, taskType))
    {
        NSRemoveMessage(newNoti);
        return OC_STACK_KEEP_TRANSACTION;
    }

    return OC_STACK_KEEP_TRANSACTION;
}

NSResult NSPushToCache(OCClientResponse * clientResponse, NSTaskType type)
{
    NSMessage_consumer * cachedNoti = NSBuildOICNotification(clientResponse);
    if (!cachedNoti)
    {
        return NS_ERROR;
    }
    NSTask * task = NSMakeTask(type, (void *) cachedNoti);
    if (!task)
    {
        NS_LOG(ERROR, "NSTask allocation fail");
        NSRemoveMessage(cachedNoti);
        return NS_ERROR;
    }
    NSConsumerPushEvent(task);

    return NS_OK;
}

NSMessage_consumer * NSBuildOICNotification(OCClientResponse * clientResponse)
{
    if(!clientResponse->payload)
    {
        return NULL;
    }

    NSMessage_consumer * retNoti = (NSMessage_consumer *)OICMalloc(sizeof(NSMessage_consumer));
    if (!retNoti)
    {
        return NULL;
    }

    retNoti->mId = NULL;
    retNoti->mTitle = NULL;
    retNoti->mContentText = NULL;
    retNoti->mSource = NULL;

    OCRepPayload * payload = (OCRepPayload *)clientResponse->payload;
    if (!OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_ID, &retNoti->mId))
    {
        NS_LOG(ERROR, "id of received notification is null");
        OICFree(retNoti);
        return NULL;
    }

    OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_TITLE, &retNoti->mTitle);
    OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_TEXT, &retNoti->mContentText);
    OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_SOURCE, &retNoti->mSource);

    NS_LOG_V(DEBUG, "Msg Address : %s", clientResponse->addr->addr);
    NS_LOG_V(DEBUG, "Msg ID : %s", retNoti->mId);
    NS_LOG_V(DEBUG, "Msg Title : %s", retNoti->mTitle);
    NS_LOG_V(DEBUG, "Msg Content : %s", retNoti->mContentText);
    NS_LOG_V(DEBUG, "Msg Source : %s", retNoti->mSource);

    retNoti->addr = (OCDevAddr *)OICMalloc(sizeof(OCDevAddr));
    memcpy(retNoti->addr, clientResponse->addr, sizeof(OCDevAddr));

    retNoti->type = Notification;

    return retNoti;
}

NSSync * NSBuildOICNotificationSync(OCClientResponse * clientResponse)
{
    if(!clientResponse->payload)
    {
        return NULL;
    }
    NSSync * retSync = (NSSync *)OICMalloc(sizeof(NSSync));
    if (!retSync)
    {
        return NULL;
    }

    retSync->mMessageId = NULL;
    retSync->mState = Notification_Read;

    OCRepPayload * payload = (OCRepPayload *)clientResponse->payload;
    if (!OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_ID, &retSync->mMessageId))
    {
        NS_LOG(ERROR, "id of received sync is null");
        OICFree(retSync);
        return NULL;
    }
    int64_t state;
    if (!OCRepPayloadGetPropInt(payload, NS_ATTRIBUTE_STATE, & state))
    {
        NS_LOG(ERROR, "state of received sync is null");
        OICFree(retSync->mMessageId);
        OICFree(retSync);
        return NULL;
    }

    retSync->mState = (NSSyncTypes) state;

    NS_LOG_V(DEBUG, "Sync ID : %s", retSync->mMessageId);
    NS_LOG_V(DEBUG, "Sync State : %d", (int) retSync->mState);

    return retSync;
}

NSProvider * NSGetProvider(OCClientResponse * clientResponse)
{
    NSProvider * newProvider = (NSProvider *)OICMalloc(sizeof(NSProvider));
    if (!newProvider)
    {
        NS_LOG(DEBUG, "NSProvider allocation fail");
        return NULL;
    }

    // TODO set id
    newProvider->mId = NULL;
    newProvider->mUserData = (void *)OICMalloc(sizeof(OCDevAddr));
    if (!newProvider->mUserData)
    {
        NS_LOG(DEBUG, "OCDevAddr allocation fail");
        OICFree(newProvider);
        return NULL;
    }
    memcpy(newProvider->mUserData, clientResponse->addr, sizeof(OCDevAddr));

    return newProvider;
}

void NSConsumerNotificationTaskProcessing(NSTask * task)
{
    if (!task)
    {
        NS_LOG(ERROR, "task is null");
        return;
    }

    NS_LOG_V(DEBUG, "Receive Event : %d", (int)task->taskType);
    if (task->taskType == TASK_CONSUMER_REQ_SUBSCRIBE)
    {
        if (NS_OK != NSConsumerSubscribeProvider((NSProvider *)task->taskData))
        {
            NS_LOG(ERROR, "Subscribe fail");
            return;
        }
    }
    else if (task->taskType == TASK_SEND_READ || task->taskType == TASK_SEND_DISMISS)
    {
        NSMessage_consumer * nsConsumer = (NSMessage_consumer *) task->taskData;
        if (!nsConsumer)
        {
            NS_LOG(ERROR, "taskData is NULL");
            return;
        }

        OCRepPayload * payload = OCRepPayloadCreate ();
        if (!payload)
        {
            NS_LOG(ERROR, "Failed to create POST payload object");
            return;
        }

        int type = (task->taskType == TASK_SEND_READ) ? 0 : 1;
        OCRepPayloadSetPropString(payload, "ID", (char *) nsConsumer->mId);
        OCRepPayloadSetPropString(payload, "SOURCE", (char *) nsConsumer->mSource);
        OCRepPayloadSetPropInt(payload, "STATE", type);

        // TODO fix param for uri
        if (NS_OK != NSConsumerPostProvider(
                (OCDevAddr *) nsConsumer->addr, (OCPayload *) payload, "/notification/sync"))
        {
            NS_LOG(ERROR, "Subscribe fail");
            return;
        }
    }
    else if (task->taskType == TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL)
    {
        NSProvider * provider = (NSProvider *)task->taskData;

        OCCancel(provider->messageHandle, NS_QOS, NULL, 0);
        OCCancel(provider->syncHandle, NS_QOS, NULL, 0);
    }
    else
    {
        NS_LOG(ERROR, "Unknown type message");
    }
}
