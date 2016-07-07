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

#define NS_SYNC_URI "/notification/sync"

unsigned long NS_MESSAGE_ACCEPTANCE = 1;

NSMessage_consumer * NSCreateMessage_internal(uint64_t msgId, const char * providerId);
NSSyncInfo * NSCreateSyncInfo_consumer(uint64_t msgId, const char * providerId, NSSyncType state);

NSMessage_consumer * NSGetMessage(OCClientResponse * clientResponse);
NSSyncInfo * NSGetSyncInfoc(OCClientResponse * clientResponse);

OCRepPayload * NSGetofSyncInfoPayload(NSMessage_consumer * message, int type);
OCStackResult NSSendSyncInfoUsingMessage(NSMessage_consumer * message, int type);

// TODO it seem to not to be this file
NSResult NSPushToCache(OCClientResponse * clientResponse, NSTaskType type);

NSResult NSConsumerSubscribeProvider(NSProvider * provider)
{
    NSProvider_internal * provider_internal = (NSProvider_internal *) provider;
    NS_VERIFY_NOT_NULL(provider_internal, NS_ERROR);

    NS_LOG(DEBUG, "get subscribe message query");
    char * query = NSMakeRequestUriWithConsumerId(provider_internal->messageUri);
    NS_VERIFY_NOT_NULL(query, NS_ERROR);

    NS_LOG(DEBUG, "subscribe message");
    NS_LOG_V(DEBUG, "subscribe query : %s", query);
    OCStackResult ret = NSInvokeRequest(&(provider_internal->i_messageHandle),
                          OC_REST_OBSERVE, provider_internal->i_addr,
                          query, NULL, NSConsumerMessageListener, NULL);
    NS_VERIFY_STACK_OK(ret, NS_ERROR);
    NSOICFree(query);

    NS_LOG(DEBUG, "get subscribe sync query");
    query = NSMakeRequestUriWithConsumerId(provider_internal->syncUri);
    NS_VERIFY_NOT_NULL(query, NS_ERROR);

    NS_LOG(DEBUG, "subscribe sync");
    NS_LOG_V(DEBUG, "subscribe query : %s", query);
    ret = NSInvokeRequest(&(provider_internal->i_syncHandle),
                          OC_REST_OBSERVE, provider_internal->i_addr,
                          query, NULL, NSConsumerSyncInfoListener, NULL);
    NS_VERIFY_STACK_OK(ret, NS_ERROR);
    NSOICFree(query);

    return NS_OK;
}

OCStackApplicationResult NSConsumerCheckPostResult(
        void * ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    (void) ctx;
    (void) handle;

    NS_VERIFY_NOT_NULL(clientResponse, OC_STACK_KEEP_TRANSACTION);
    NS_VERIFY_STACK_OK(clientResponse->result, OC_STACK_KEEP_TRANSACTION);

    return OC_STACK_KEEP_TRANSACTION;
}

void NSRemoveSyncInfoObj(NSSyncInfo * sync)
{
    NSOICFree(sync);
}

OCStackApplicationResult NSConsumerSyncInfoListener(
        void * ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    (void) ctx;
    (void) handle;

    NS_VERIFY_NOT_NULL(clientResponse, OC_STACK_KEEP_TRANSACTION);
    NS_VERIFY_STACK_OK(clientResponse->result, OC_STACK_KEEP_TRANSACTION);

    NS_LOG(DEBUG, "get NSSyncInfo");
    NSSyncInfo * newSync = NSGetSyncInfoc(clientResponse);
    NS_VERIFY_NOT_NULL(newSync, OC_STACK_KEEP_TRANSACTION);

    NSTaskType taskType = TASK_RECV_SYNCINFO;

    NS_LOG(DEBUG, "build NSTask");
    NSTask * task = NSMakeTask(taskType, (void *) newSync);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(task,
               OC_STACK_KEEP_TRANSACTION, NSRemoveSyncInfoObj(newSync));

    NSConsumerPushEvent(task);

    return OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult NSConsumerMessageListener(
        void * ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    (void) ctx;
    (void) handle;

    NS_VERIFY_NOT_NULL(clientResponse, OC_STACK_KEEP_TRANSACTION);
    NS_VERIFY_STACK_OK(clientResponse->result, OC_STACK_KEEP_TRANSACTION);

    NS_LOG(DEBUG, "build NSMessage");
    NSMessage_consumer * newNoti = NSGetMessage(clientResponse);
    NS_VERIFY_NOT_NULL(newNoti, OC_STACK_KEEP_TRANSACTION);

    NSTaskType type = TASK_CONSUMER_RECV_MESSAGE;

    if (newNoti->messageId == NS_MESSAGE_ACCEPTANCE)
    {
        NS_LOG(DEBUG, "Receive Subscribe confirm");
        type = TASK_CONSUMER_RECV_SUBSCRIBE_CONFIRMED;
    }
    else
    {
        NS_LOG(DEBUG, "Receive new message");
    }

    NS_LOG(DEBUG, "build NSTask");
    NSTask * task = NSMakeTask(type, (void *) newNoti);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(task, NS_ERROR, NSRemoveMessage(newNoti));

    NSConsumerPushEvent(task);

    return OC_STACK_KEEP_TRANSACTION;
}

NSResult NSPushToCache(OCClientResponse * clientResponse, NSTaskType type)
{
    NSMessage_consumer * cachedNoti = NSGetMessage(clientResponse);
    NS_LOG(DEBUG, "build NSMessage");
    NS_VERIFY_NOT_NULL(cachedNoti, NS_ERROR);

    NS_LOG(DEBUG, "build NSTask");
    NSTask * task = NSMakeTask(type, (void *) cachedNoti);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(task, NS_ERROR, NSRemoveMessage(cachedNoti));

    NSConsumerPushEvent(task);

    return NS_OK;
}

void NSGetMessagePostClean(char * pId, OCDevAddr * addr)
{
    NSOICFree(pId);
    NSOICFree(addr);
}

NSMessage_consumer * NSGetMessage(OCClientResponse * clientResponse)
{
    NS_VERIFY_NOT_NULL(clientResponse->payload, NULL);
    OCRepPayload * payload = (OCRepPayload *)clientResponse->payload;

    NS_LOG(DEBUG, "get msg id");
    uint64_t id = NULL;
    bool getResult = OCRepPayloadGetPropInt(payload, NS_ATTRIBUTE_MESSAGE_ID, (int64_t *)&id);
    NS_VERIFY_NOT_NULL(getResult == true ? (void *) 1 : NULL, NULL);

    NS_LOG(DEBUG, "get provider id");
    char * pId = NULL;
    getResult = OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_PROVIDER_ID, &pId);
    NS_LOG_V (DEBUG, "provider id: %s", pId);
    NS_VERIFY_NOT_NULL(getResult == true ? (void *) 1 : NULL, NULL);

    NS_LOG(DEBUG, "get provider address");
    OCDevAddr * addr = (OCDevAddr *)OICMalloc(sizeof(OCDevAddr));
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(addr, NULL, NSGetMessagePostClean(pId, addr));
    memcpy(addr, clientResponse->addr, sizeof(OCDevAddr));

    NS_LOG(DEBUG, "create NSMessage");
    NSMessage_consumer * retMsg = NSCreateMessage_internal(id, pId);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(retMsg, NULL, NSGetMessagePostClean(pId, addr));
    NSOICFree(pId);

    retMsg->i_addr = addr;
    retMsg->i_messageTypes = NS_SYNC_UNREAD;

    NS_LOG(DEBUG, "get msg optional field");
    OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_TITLE, &retMsg->title);
    OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_TEXT, &retMsg->contentText);
    OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_SOURCE, &retMsg->sourceName);

    OCRepPayloadGetPropInt(payload, NS_ATTRIBUTE_TYPE, (int64_t *)&retMsg->type);
    OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_DATETIME, &retMsg->dateTime);
    OCRepPayloadGetPropInt(payload, NS_ATTRIBUTE_TTL, (int64_t *)&retMsg->ttl);

    NS_LOG_V(DEBUG, "Msg Address : %s", retMsg->i_addr->addr);
    NS_LOG_V(DEBUG, "Msg ID      : %lld", retMsg->messageId);
    NS_LOG_V(DEBUG, "Msg Title   : %s", retMsg->title);
    NS_LOG_V(DEBUG, "Msg Content : %s", retMsg->contentText);
    NS_LOG_V(DEBUG, "Msg Source  : %s", retMsg->sourceName);
    NS_LOG_V(DEBUG, "Msg Type    : %d", retMsg->type);
    NS_LOG_V(DEBUG, "Msg Date    : %s", retMsg->dateTime);
    NS_LOG_V(DEBUG, "Msg ttl     : %lld", retMsg->ttl);

    return retMsg;
}

NSSyncInfo * NSGetSyncInfoc(OCClientResponse * clientResponse)
{
    NS_VERIFY_NOT_NULL(clientResponse->payload, NULL);

    OCRepPayload * payload = (OCRepPayload *)clientResponse->payload;

    NS_LOG(DEBUG, "get msg id");
    uint64_t id = NULL;
    bool getResult = OCRepPayloadGetPropInt(payload, NS_ATTRIBUTE_MESSAGE_ID, (int64_t *)&id);
    NS_VERIFY_NOT_NULL(getResult == true ? (void *) 1 : NULL, NULL);

    NS_LOG(DEBUG, "get provider id");
    char * pId = NULL;
    getResult = OCRepPayloadGetPropString(payload, NS_ATTRIBUTE_PROVIDER_ID, &pId);
    NS_VERIFY_NOT_NULL(getResult == true ? (void *) 1 : NULL, NULL);

    NS_LOG(DEBUG, "get state");
    int64_t state = 0;
    getResult = OCRepPayloadGetPropInt(payload, NS_ATTRIBUTE_STATE, & state);
    NS_VERIFY_NOT_NULL(getResult == true ? (void *) 1 : NULL, NULL);

    NS_LOG(DEBUG, "create NSSyncInfo");
    NSSyncInfo * retSync = NSCreateSyncInfo_consumer(id, pId, (NSSyncType)state);
    NS_VERIFY_NOT_NULL(retSync, NULL);

    NS_LOG_V(DEBUG, "Sync ID : %lld", retSync->messageId);
    NS_LOG_V(DEBUG, "Sync State : %d", (int) retSync->state);
    NS_LOG_V(DEBUG, "Sync Provider ID : %s", retSync->providerId);

    return retSync;
}

NSMessage_consumer * NSCreateMessage_internal(uint64_t id, const char * providerId)
{
    NSMessage_consumer * retMsg = (NSMessage_consumer *)OICMalloc(sizeof(NSMessage_consumer));
    NS_VERIFY_NOT_NULL(retMsg, NULL);

    retMsg->messageId = id;
    OICStrcpy(retMsg->providerId, sizeof(char) * NS_DEVICE_ID_LENGTH, providerId);
    retMsg->title = NULL;
    retMsg->contentText = NULL;
    retMsg->sourceName = NULL;
    retMsg->type = NS_MESSAGE_INFO;
    retMsg->dateTime = NULL;
    retMsg->ttl = 0;
    retMsg->i_addr = NULL;

    return retMsg;
}

NSSyncInfo * NSCreateSyncInfo_consumer(uint64_t msgId, const char * providerId, NSSyncType state)
{
    NSSyncInfo * retSync = (NSSyncInfo *)OICMalloc(sizeof(NSSyncInfo));
    NS_VERIFY_NOT_NULL(retSync, NULL);

    retSync->messageId = msgId;
    retSync->state = state;
    OICStrcpy(retSync->providerId, sizeof(char) * NS_DEVICE_ID_LENGTH, providerId);

    return retSync;
}

OCStackResult NSSendSyncInfo(NSSyncInfo * syncInfo, OCDevAddr * addr)
{
    OCRepPayload * payload = OCRepPayloadCreate();
    NS_VERIFY_NOT_NULL(payload, OC_STACK_ERROR);

    OCRepPayloadSetPropInt(payload, NS_ATTRIBUTE_MESSAGE_ID, (int64_t)syncInfo->messageId);
    OCRepPayloadSetPropInt(payload, NS_ATTRIBUTE_STATE, syncInfo->state);
    OCRepPayloadSetPropString(payload, NS_ATTRIBUTE_PROVIDER_ID, syncInfo->providerId);

    return NSInvokeRequest(NULL, OC_REST_POST, addr,
                           NS_SYNC_URI, (OCPayload*)payload,
                           NSConsumerCheckPostResult, NULL);
}

void NSConsumerCommunicationTaskProcessing(NSTask * task)
{
    NS_VERIFY_NOT_NULL_V(task);

    NS_LOG_V(DEBUG, "Receive Event : %d", (int)task->taskType);
    if (task->taskType == TASK_CONSUMER_REQ_SUBSCRIBE)
    {
        NS_LOG(DEBUG, "Request Subscribe");
        NSResult ret = NSConsumerSubscribeProvider((NSProvider *)task->taskData);
        NS_VERIFY_NOT_NULL_V(ret == NS_OK ? (void *)1 : NULL);
    }
    else if (task->taskType == TASK_SEND_SYNCINFO)
    {
        // TODO find target OCDevAddr using provider Id.
        NSSyncInfo_internal * syncInfo = (NSSyncInfo_internal *)task->taskData;
        OCDevAddr * addr = syncInfo->i_addr;
        OCStackResult ret = NSSendSyncInfo((NSSyncInfo *)(task->taskData), addr);
        NS_VERIFY_STACK_OK_V(ret);

        NSOICFree(syncInfo->i_addr);
        NSOICFree(syncInfo);
    }
    else if (task->taskType == TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL)
    {
        NSProvider_internal * provider = (NSProvider_internal *)task->taskData;

        OCCancel(provider->i_messageHandle, NS_QOS, NULL, 0);
        OCCancel(provider->i_syncHandle, NS_QOS, NULL, 0);
    }
    else
    {
        NS_LOG(ERROR, "Unknown type message");
    }
}
