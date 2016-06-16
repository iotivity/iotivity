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
#include "NSConsumerQueueScheduler.h"
#include "oic_malloc.h"

// Public APIs
NSResult NSStartConsumer(
        NSProviderDiscoveredCallback discoverCb,
        NSNotificationReceivedCallback  postCb,
        NSSyncCallback syncCb)
{
    bool isStartedConsumer = NSIsStartedConsumer();
    NS_VERTIFY_NOT_NULL(isStartedConsumer == false ? (void *) 1 : NULL, NS_OK);

    NS_VERTIFY_NOT_NULL(discoverCb, NS_ERROR);
    NS_VERTIFY_NOT_NULL(postCb, NS_ERROR);
    NS_VERTIFY_NOT_NULL(syncCb, NS_ERROR);

    NSSetDiscoverProviderCb(discoverCb);
    NSSetMessagePostedCb(postCb);
    NSSetNotificationSyncCb(syncCb);
    NSSetIsStartedConsumer(true);

    NSResult ret = NSConsumerMessageHandlerInit();
    NS_VERTIFY_NOT_NULL_WITH_POST_CLEANING(ret == NS_OK ? (void *) 1 : NULL,
            NS_ERROR, NSStopConsumer());

    return NS_OK;
}

NSResult NSStopConsumer()
{
    NSSetDiscoverProviderCb(NULL);
    NSSetMessagePostedCb(NULL);
    NSSetNotificationSyncCb(NULL);
    NSSetIsStartedConsumer(false);

    NSConsumerMessageHandlerExit();

    return NS_OK;
}

NSResult NSSubscribe(NSProvider * provider)
{
    NSTask * subscribeTask = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE, (void *) provider);
    NS_VERTIFY_NOT_NULL(subscribeTask, NS_ERROR);

    return NSConsumerPushEvent(subscribeTask);
}

NSResult NSUnsubscribe(NSProvider * provider)
{
    NSTask * unsubscribeTask = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL, (void *) provider);
    NS_VERTIFY_NOT_NULL(unsubscribeTask, NS_ERROR);

    return NSConsumerPushEvent(unsubscribeTask);
}

NSResult NSRescanProvider()
{
    NSTask * discoverTask = NSMakeTask(TASK_CONSUMER_REQ_DISCOVER, NULL);
    NS_VERTIFY_NOT_NULL(discoverTask, NS_ERROR);

    return NSConsumerPushEvent(discoverTask);
}

NSResult NSConsumerReadCheck(NSMessage * obj)
{
    NSTask * readTask = NSMakeTask(TASK_SEND_READ, (void *) obj);
    NS_VERTIFY_NOT_NULL(readTask, NS_ERROR);

    return NSConsumerPushEvent(readTask);
}

NSResult NSConsumerDismissCheck(NSMessage * obj)
{
    NSTask * dismissTask = NSMakeTask(TASK_SEND_DISMISS, (void *) obj);
    NS_VERTIFY_NOT_NULL(dismissTask, NS_ERROR);

    return NSConsumerPushEvent(dismissTask);
}

NSResult NSDropNSObject(NSMessage * obj)
{
    NS_VERTIFY_NOT_NULL(obj, NS_ERROR);

    obj->messageId = 0;
    NSOICFree(obj->title);
    NSOICFree(obj->contentText);
    NSOICFree(obj->sourceName);
    NSOICFree(obj);

    return NS_OK;
}
