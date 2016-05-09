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
#include "NSConsumerMessageHandler.h"
#include "oic_malloc.h"

// Public APIs
NSResult NSStartConsumer(
        NSProviderDiscoveredCallback discoverCb,
        NSNotificationReceivedCallback  postCb,
        NSSyncCallback syncCb)
{
    if (NSIsStartedConsumer())
    {
        printf("is already started consumer service?\n");
        return NS_OK;
    }

    NSSetDiscoverProviderCb(discoverCb);
    NSSetNotificationPostedCb(postCb);
    NSSetNotificationSyncCb(syncCb);
    NSSetIsStartedConsumer(true);

    if (NS_OK != NSConsumerMessageHandlerInit())
    {
        return NS_ERROR;
    }

    return NS_OK;
}

NSResult NSStopConsumer()
{
    NSSetDiscoverProviderCb(NULL);
    NSSetNotificationPostedCb(NULL);
    NSSetNotificationSyncCb(NULL);
    NSSetIsStartedConsumer(false);

    return NS_OK;
}

NSResult NSSubscribeProvider(NSProvider * provider)
{
    NSTask * subscribeTask = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE, (void *) provider);
    if (!subscribeTask)
    {
        NS_CONSUMER_LOG(ERROR, "NSTask allocation fail");
        return NS_ERROR;
    }

    return NSConsumerPushEvent(subscribeTask);
}

NSResult NSUnsubscribeProvider(NSProvider * provider)
{
    NSTask * unsubscribeTask = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL, (void *) provider);
    if (!unsubscribeTask)
    {
        NS_CONSUMER_LOG(ERROR, "NSTask allocation fail");
        return NS_ERROR;
    }

    return NSConsumerPushEvent(unsubscribeTask);
}

NSResult NSRescanProvider()
{
    NSTask * discoverTask = NSMakeTask(TASK_CONSUMER_REQ_DISCOVER, NULL);
    if (!discoverTask)
    {
        NS_CONSUMER_LOG(ERROR, "NSTask allocation fail");
        return NS_ERROR;
    }

    return NSConsumerPushEvent(discoverTask);
}

NSResult NSRead(NSMessage * obj)
{
    NSTask * readTask = NSMakeTask(TASK_SEND_READ, (void *) obj);
    if (!readTask)
    {
        NS_CONSUMER_LOG(ERROR, "NSTask allocation fail");
        return NS_ERROR;
    }

    return NSConsumerPushEvent(readTask);
}

NSResult NSDismiss(NSMessage * obj)
{
    NSTask * dismissTask = NSMakeTask(TASK_SEND_DISMISS, (void *) obj);
    if (!dismissTask)
    {
        NS_CONSUMER_LOG(ERROR, "NSTask allocation fail");
        return NS_ERROR;
    }

    return NSConsumerPushEvent(dismissTask);
}

NSResult NSDropNSObject(NSMessage * obj)
{
    if (!obj)
    {
        return NS_ERROR;
    }

    if (obj->mId)
    {
        OICFree(obj->mId);
        obj->mId = NULL;
    }

    if (obj->mTitle)
    {
        OICFree(obj->mTitle);
        obj->mTitle = NULL;
    }

    if (obj->mContentText)
    {
        OICFree(obj->mContentText);
        obj->mContentText = NULL;
    }

    OICFree(obj);

    return NS_OK;
}
