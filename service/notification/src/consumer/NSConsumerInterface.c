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

#include "NSConsumerInterface.h"

#include <stdlib.h>
#include <stdbool.h>

#include "NSCommon.h"
#include "NSConsumerCommon.h"
#include "NSConstants.h"
#include "NSConsumerScheduler.h"
#include "oic_malloc.h"
#include "oic_string.h"

// Public APIs
NSResult NSStartConsumer(NSConsumerConfig config)
{
    bool isStartedConsumer = NSIsStartedConsumer();
    NS_VERIFY_NOT_NULL(isStartedConsumer == false ? (void *) 1 : NULL, NS_OK);

    NS_VERIFY_NOT_NULL(config.messageCb, NS_ERROR);
    NS_VERIFY_NOT_NULL(config.syncInfoCb, NS_ERROR);
    NS_VERIFY_NOT_NULL(config.changedCb, NS_ERROR);

    NSSetMessagePostedCb(config.messageCb);
    NSSetNotificationSyncCb(config.syncInfoCb);
    NSSetProviderChangedCb(config.changedCb);
    NSSetIsStartedConsumer(true);

    NSResult ret = NSConsumerMessageHandlerInit();
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(ret == NS_OK ? (void *) 1 : NULL,
            NS_ERROR, NSStopConsumer());

    return NS_OK;
}

NSResult NSStopConsumer()
{
    bool isStartedConsumer = NSIsStartedConsumer();
    NS_VERIFY_NOT_NULL(isStartedConsumer == true ? (void *) 1 : NULL, NS_ERROR);

    NSSetMessagePostedCb(NULL);
    NSSetNotificationSyncCb(NULL);
    NSSetProviderChangedCb(NULL);
    NSSetIsStartedConsumer(false);

    NSConsumerMessageHandlerExit();

    return NS_OK;
}

NSResult NSConsumerEnableRemoteService(char *serverAddress)
{
    bool isStartedConsumer = NSIsStartedConsumer();
    NS_VERIFY_NOT_NULL(isStartedConsumer == true ? (void *) 1 : NULL, NS_ERROR);

    char * queryAddr = OICStrdup(serverAddress);
    NS_VERIFY_NOT_NULL(queryAddr, NS_ERROR);

    NSTask * discoverTask = NSMakeTask(TASK_CONSUMER_REQ_DISCOVER, (void *)queryAddr);
    NS_VERIFY_NOT_NULL(discoverTask, NS_ERROR);

    return NSConsumerPushEvent(discoverTask);
}

NSResult NSSubscribe(NSProvider * provider)
{
    bool isStartedConsumer = NSIsStartedConsumer();
    NS_VERIFY_NOT_NULL(isStartedConsumer == true ? (void *) 1 : NULL, NS_ERROR);

    NSProvider * prov = (NSProvider *)NSCopyProvider((NSProvider_internal *) provider);
    NS_VERIFY_NOT_NULL(prov, NS_ERROR);

    NSTask * subscribeTask = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE, (void *) prov);
    NS_VERIFY_NOT_NULL(subscribeTask, NS_ERROR);

    return NSConsumerPushEvent(subscribeTask);
}

NSResult NSUnsubscribe(NSProvider * provider)
{
    bool isStartedConsumer = NSIsStartedConsumer();
    NS_VERIFY_NOT_NULL(isStartedConsumer == true ? (void *) 1 : NULL, NS_ERROR);

    NSProvider * prov = (NSProvider *)NSCopyProvider((NSProvider_internal *) provider);
    NS_VERIFY_NOT_NULL(prov, NS_ERROR);

    NSTask * unsubscribeTask = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL, (void *) prov);
    NS_VERIFY_NOT_NULL(unsubscribeTask, NS_ERROR);

    return NSConsumerPushEvent(unsubscribeTask);
}

NSResult NSConsumerSendSyncInfo(const char * providerId, uint64_t messageId, NSSyncType type)
{
    bool isStartedConsumer = NSIsStartedConsumer();
    NS_VERIFY_NOT_NULL(isStartedConsumer == true ? (void *) 1 : NULL, NS_ERROR);

    NSSyncInfo * syncInfo = (NSSyncInfo *)OICMalloc(sizeof(NSSyncInfo));
    NS_VERIFY_NOT_NULL(syncInfo, NS_ERROR);

    OICStrcpy(syncInfo->providerId, sizeof(char) * NS_DEVICE_ID_LENGTH, providerId);
    syncInfo->messageId = messageId;
    syncInfo->state = type;

    NSTask * syncTask = NSMakeTask(TASK_MAKE_SYNCINFO, (void *) syncInfo);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(syncTask, NS_ERROR, NSOICFree(syncInfo));

    return NSConsumerPushEvent(syncTask);
}

NSResult NSRescanProvider()
{
    bool isStartedConsumer = NSIsStartedConsumer();
    NS_VERIFY_NOT_NULL(isStartedConsumer == true ? (void *) 1 : NULL, NS_ERROR);

    NSTask * discoverTask = NSMakeTask(TASK_CONSUMER_REQ_DISCOVER, NULL);
    NS_VERIFY_NOT_NULL(discoverTask, NS_ERROR);

    return NSConsumerPushEvent(discoverTask);
}

NSProvider * NSConsumerGetProvider(const char * providerId)
{
    bool isStartedConsumer = NSIsStartedConsumer();
    NS_VERIFY_NOT_NULL(isStartedConsumer == true ? (void *) 1 : NULL, NULL);

    NS_VERIFY_NOT_NULL(providerId, NULL);

    NSProvider_internal * prov = NSConsumerFindNSProvider(providerId);
    NS_VERIFY_NOT_NULL(prov, NULL);

    NSProvider * retProv = NSCopyProvider(prov);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(retProv, NULL, NSRemoveProvider_internal(prov));
    NSRemoveProvider_internal(prov);

    return retProv;
}

NSMessage * NSConsumerGetMessage(uint64_t messageId)
{
    bool isStartedConsumer = NSIsStartedConsumer();
    NS_VERIFY_NOT_NULL(isStartedConsumer == true ? (void *) 1 : NULL, NULL);

    char msgId[NS_DEVICE_ID_LENGTH] = { 0, };
    snprintf(msgId, NS_DEVICE_ID_LENGTH, "%lld", (long long int)messageId);

    return (NSMessage *) NSConsumerFindNSMessage(msgId);
}

NSResult NSConsumerGetInterestTopics(NSProvider * provider)
{
    bool isStartedConsumer = NSIsStartedConsumer();
    NS_VERIFY_NOT_NULL(isStartedConsumer == true ? (void *) 1 : NULL, NS_ERROR);

    NS_VERIFY_NOT_NULL(provider, NS_ERROR);

    NS_LOG_V(DEBUG, "NSProvider ID: %s", provider->providerId);
    NSProvider_internal * prov_internal = NSConsumerFindNSProvider(provider->providerId);
    NS_VERIFY_NOT_NULL(prov_internal, NS_ERROR);

    NSSelector selector = prov_internal->accessPolicy;
    NSRemoveProvider_internal(prov_internal);
    NS_VERIFY_NOT_NULL(selector == NS_SELECTION_CONSUMER ? (void *) 1 : NULL, NS_ERROR);

    NSProvider * prov = (NSProvider *)NSCopyProvider((NSProvider_internal *) provider);
    NS_VERIFY_NOT_NULL(prov, NS_ERROR);

    NSTask * topicTask = NSMakeTask(TASK_CONSUMER_GET_TOPIC_LIST, (void *) prov);
    NS_VERIFY_NOT_NULL(topicTask, NS_ERROR);

    return NSConsumerPushEvent(topicTask);
}

NSResult NSConsumerSelectInterestTopics(NSProvider * provider)
{
    bool isStartedConsumer = NSIsStartedConsumer();
    NS_VERIFY_NOT_NULL(isStartedConsumer == true ? (void *) 1 : NULL, NS_ERROR);

    NS_VERIFY_NOT_NULL(provider, NS_ERROR);
    NS_VERIFY_NOT_NULL(provider->topicLL, NS_ERROR);

    NSProvider_internal * prov_internal = NSConsumerFindNSProvider(provider->providerId);
    NS_VERIFY_NOT_NULL(prov_internal, NS_ERROR);

    NSSelector selector = prov_internal->accessPolicy;
    NSRemoveProvider_internal(prov_internal);
    NS_VERIFY_NOT_NULL(selector == NS_SELECTION_CONSUMER ? (void *) 1 : NULL, NS_ERROR);

    NSProvider * prov = (NSProvider *)NSCopyProvider((NSProvider_internal *) provider);
    NS_VERIFY_NOT_NULL(prov, NS_ERROR);

    NSTask * topicTask = NSMakeTask(TASK_CONSUMER_SELECT_TOPIC_LIST, (void *) prov);
    NS_VERIFY_NOT_NULL(topicTask, NS_ERROR);

    return NSConsumerPushEvent(topicTask);
}

NSResult NSDropNSMessage(NSMessage * obj)
{
    NS_VERIFY_NOT_NULL(obj, NS_ERROR);

    obj->messageId = 0;
    NSOICFree(obj->title);
    NSOICFree(obj->contentText);
    NSOICFree(obj->sourceName);
    NSOICFree(obj);

    return NS_OK;
}
