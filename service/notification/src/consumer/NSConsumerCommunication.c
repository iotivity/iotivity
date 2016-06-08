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

#include "NSConstants.h"
#include "NSConsumerCommon.h"
#include "NSConsumerCommunication.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocpayload.h"

const char NS_MESSAGE_ACCEPTANCE[] = "0000-0000-0000-0000";

NSMessage_consumer * NSGetNSMessage(OCClientResponse * clientResponse);
NSSync * NSGetNSSyncInfo(OCClientResponse * clientResponse);
NSProvider * NSGetNSProvider(OCClientResponse * clientResponse);
OCRepPayload * NSGetPayloadofSyncInfo(NSMessage_consumer * message, int type);


NSResult NSPushToCache(OCClientResponse * clientResponse, NSTaskType type);

OCStackResult NSSendSyncInfo(NSMessage_consumer * message, int type);

NSResult NSConsumerSubscribeProvider(NSProvider * provider)
{
    OCStackResult ret = NSInvokeRequest(&(provider->messageHandle),
                          OC_REST_OBSERVE, (OCDevAddr *) provider->mUserData,
                          provider->messageUri, NULL, NSConsumerMessageListener, NULL);
    NS_VERTIFY_STACK_OK(ret, NS_ERROR);

    ret = NSInvokeRequest(&(provider->syncHandle),
                          OC_REST_OBSERVE, (OCDevAddr *) provider->mUserData,
                          provider->syncUri, NULL, NSConsumerSyncInfoListener, NULL);
    NS_VERTIFY_STACK_OK(ret, NS_ERROR);

    return NS_OK;
}

OCStackApplicationResult NSConsumerPostResultCheck(
        void * ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    (void) ctx;
    (void) handle;

    NS_VERTIFY_NOT_NULL(clientResponse, OC_STACK_KEEP_TRANSACTION);
    NS_VERTIFY_STACK_OK(clientResponse->result, OC_STACK_KEEP_TRANSACTION);

    return OC_STACK_KEEP_TRANSACTION;
}

NSResult NSConsumerPostProvider(OCDevAddr * addr, OCPayload * payload, const char * uri)
{
    OCStackResult ret = NSInvokeRequest(NULL, OC_REST_POST, addr, uri, payload,
                                        NSConsumerPostResultCheck, NULL);
    NS_VERTIFY_STACK_OK(ret, NS_ERROR);

    return NS_OK;
}

OCStackApplicationResult NSConsumerSyncInfoListener(
        void * ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    (void) ctx;
    (void) handle;

    NS_VERTIFY_NOT_NULL(clientResponse, OC_STACK_KEEP_TRANSACTION);
    NS_VERTIFY_STACK_OK(clientResponse->result, OC_STACK_KEEP_TRANSACTION);

    NSSync * newNoti = NULL;

    NS_LOG(ERROR, "get provider");
    NSProvider * provider = NSGetNSProvider(clientResponse);
    NS_VERTIFY_NOT_NULL(provider, OC_STACK_KEEP_TRANSACTION);

    newNoti = NSGetNSSyncInfo(clientResponse);
    NS_VERTIFY_NOT_NULL(newNoti, OC_STACK_KEEP_TRANSACTION);

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

    NSResult ret = NSPushToCache(clientResponse, taskType);
    NS_VERTIFY_NOT_NULL(ret == NS_OK ? (void *)1 : NULL, OC_STACK_KEEP_TRANSACTION);

    return OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult NSConsumerMessageListener(
        void * ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    (void) ctx;
    (void) handle;

    NS_VERTIFY_NOT_NULL(clientResponse, OC_STACK_KEEP_TRANSACTION);
    NS_VERTIFY_STACK_OK(clientResponse->result, OC_STACK_KEEP_TRANSACTION);

    NS_LOG(DEBUG, "build NSProvider");
    NSProvider * provider = NSGetNSProvider(clientResponse);
    NS_VERTIFY_NOT_NULL(provider, OC_STACK_KEEP_TRANSACTION);

    NS_LOG(DEBUG, "build NSMessage");
    NSMessage_consumer * newNoti = NSGetNSMessage(clientResponse);
    NS_VERTIFY_NOT_NULL_WITH_POST_CLEANING(
            newNoti, OC_STACK_KEEP_TRANSACTION, OICFree(provider));

    if (!strcmp(newNoti->mId, NS_MESSAGE_ACCEPTANCE))
    {
        // TODO update provider list.
        NS_LOG(DEBUG, "Receive Subscribe confirm");
        OICFree(provider->mUserData);
        OICFree(provider);
        NSRemoveMessage(newNoti);
        return OC_STACK_KEEP_TRANSACTION;
    }

    NS_LOG(DEBUG, "newNoti->type == Notification");
    NSNotificationPost(provider, (NSMessage *) newNoti);

    NSResult ret = NSPushToCache(clientResponse, TASK_CONSUMER_RECV_MESSAGE);
    NS_VERTIFY_NOT_NULL(ret == NS_OK ? (void *)1 : NULL, OC_STACK_KEEP_TRANSACTION);

    return OC_STACK_KEEP_TRANSACTION;
}

NSResult NSPushToCache(OCClientResponse * clientResponse, NSTaskType type)
{
    NSMessage_consumer * cachedNoti = NSGetNSMessage(clientResponse);
    NS_LOG(DEBUG, "build NSMessage");
    NS_VERTIFY_NOT_NULL(cachedNoti, NS_ERROR);

    NS_LOG(DEBUG, "build NSTask");
    NSTask * task = NSMakeTask(type, (void *) cachedNoti);
    NS_VERTIFY_NOT_NULL_WITH_POST_CLEANING(task, NS_ERROR, NSRemoveMessage(cachedNoti));

    NSConsumerPushEvent(task);

    return NS_OK;
}

NSMessage_consumer * NSGetNSMessage(OCClientResponse * clientResponse)
{
    NS_VERTIFY_NOT_NULL(clientResponse->payload, NULL);

    NS_LOG(DEBUG, "get id");
    OCRepPayload * payload = (OCRepPayload *)clientResponse->payload;
    char * id = NULL;
    bool getResult = OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_ID, &id);
    NS_VERTIFY_NOT_NULL(getResult == true ? (void *) 1 : NULL, NULL);

    NS_LOG(DEBUG, "create NSMessage");
    NSMessage_consumer * retNoti = (NSMessage_consumer *)OICMalloc(sizeof(NSMessage_consumer));
    NS_VERTIFY_NOT_NULL(retNoti, NULL);

    retNoti->mId = id;
    retNoti->mTitle = NULL;
    retNoti->mContentText = NULL;
    retNoti->mSource = NULL;

    OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_TITLE, &retNoti->mTitle);
    OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_TEXT, &retNoti->mContentText);
    OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_SOURCE, &retNoti->mSource);

    NS_LOG_V(DEBUG, "Msg Address : %s", clientResponse->addr->addr);
    NS_LOG_V(DEBUG, "Msg ID : %s", retNoti->mId);
    NS_LOG_V(DEBUG, "Msg Title : %s", retNoti->mTitle);
    NS_LOG_V(DEBUG, "Msg Content : %s", retNoti->mContentText);
    NS_LOG_V(DEBUG, "Msg Source : %s", retNoti->mSource);

    NS_LOG(DEBUG, "copy target address");
    retNoti->addr = (OCDevAddr *)OICMalloc(sizeof(OCDevAddr));
    NS_VERTIFY_NOT_NULL(retNoti->addr, NULL);
    memcpy(retNoti->addr, clientResponse->addr, sizeof(OCDevAddr));

    retNoti->type = Notification;

    return retNoti;
}

NSSync * NSGetNSSyncInfo(OCClientResponse * clientResponse)
{
    NS_VERTIFY_NOT_NULL(clientResponse->payload, NULL);

    OCRepPayload * payload = (OCRepPayload *)clientResponse->payload;

    NS_LOG(DEBUG, "get state");
    int64_t state = 0;
    bool getResult = OCRepPayloadGetPropInt(payload, NS_ATTRIBUTE_STATE, & state);
    NS_VERTIFY_NOT_NULL(getResult == true ? (void *) 1 : NULL, NULL);

    NS_LOG(DEBUG, "get id");
    char * id = NULL;
    getResult = OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_ID, &id);
    NS_VERTIFY_NOT_NULL(getResult == true ? (void *) 1 : NULL, NULL);

    NS_LOG(DEBUG, "create NSSyncInfo");
    NSSync * retSync = (NSSync *)OICMalloc(sizeof(NSSync));
    NS_VERTIFY_NOT_NULL(retSync, NULL);

    retSync->mMessageId = id;
    retSync->mState = (NSSyncTypes) state;

    NS_LOG_V(DEBUG, "Sync ID : %s", retSync->mMessageId);
    NS_LOG_V(DEBUG, "Sync State : %d", (int) retSync->mState);

    return retSync;
}

NSProvider * NSGetNSProvider(OCClientResponse * clientResponse)
{
    NS_LOG(DEBUG, "create NSProvider");
    NSProvider * newProvider = (NSProvider *)OICMalloc(sizeof(NSProvider));
    NS_VERTIFY_NOT_NULL(newProvider, NULL);

    // TODO set id
    newProvider->mId = NULL;
    newProvider->mUserData = (void *)OICMalloc(sizeof(OCDevAddr));
    NS_VERTIFY_NOT_NULL_WITH_POST_CLEANING(newProvider, NULL, OICFree(newProvider));

    memcpy(newProvider->mUserData, clientResponse->addr, sizeof(OCDevAddr));

    return newProvider;
}

void NSConsumerNotificationTaskProcessing(NSTask * task)
{
    NS_VERTIFY_NOT_NULL_V(task);

    NS_LOG_V(DEBUG, "Receive Event : %d", (int)task->taskType);
    if (task->taskType == TASK_CONSUMER_REQ_SUBSCRIBE)
    {
        NS_LOG(DEBUG, "Request Subscribe");
        NSResult ret = NSConsumerSubscribeProvider((NSProvider *)task->taskData);
        NS_VERTIFY_NOT_NULL_V(ret == NS_OK ? (void *)1 : NULL);
    }
    else if (task->taskType == TASK_SEND_READ || task->taskType == TASK_SEND_DISMISS)
    {
        NSMessage_consumer * message = (NSMessage_consumer *) task->taskData;
        NS_VERTIFY_NOT_NULL_V(message);

        OCStackResult ret = NSSendSyncInfo(message, (task->taskType == TASK_SEND_READ) ? 0 : 1);
        NS_VERTIFY_STACK_OK_V(ret);

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

OCRepPayload * NSGetPayloadofSyncInfo(NSMessage_consumer * message, int type)
{
    OCRepPayload * payload = OCRepPayloadCreate();
    NS_VERTIFY_NOT_NULL(payload, NULL);

    OCRepPayloadSetPropString(payload, "ID", (char *) message->mId);
    OCRepPayloadSetPropInt(payload, "STATE", type);
    if (message->mSource)
    {
        OCRepPayloadSetPropString(payload, "SOURCE", (char *) message->mSource);
    }

    return payload;
}

OCStackResult NSSendSyncInfo(NSMessage_consumer * message, int type)
{
    OCRepPayload * payload = NSGetPayloadofSyncInfo(message, type);
    NS_VERTIFY_NOT_NULL(payload, OC_STACK_ERROR);

    return NSInvokeRequest(NULL, OC_REST_POST, message->addr,
                           "/notification/sync", (OCPayload*)payload,
                           NSConsumerPostResultCheck, NULL);
}
