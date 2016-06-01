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
    if (NSIsStartedConsumer())
    {
        NS_LOG(DEBUG, "is already started consumer service?");
        return NS_OK;
    }

    if (!discoverCb)
    {
        NS_LOG(ERROR, "discoverCb is NULL");
        return NS_ERROR;
    }

    if (!postCb)
    {
        NS_LOG(ERROR, "postCb is NULL");
        return NS_ERROR;
    }

    if (!syncCb)
    {
        NS_LOG(ERROR, "syncCb is NULL");
        return NS_ERROR;
    }

    NSSetDiscoverProviderCb(discoverCb);
    NSSetNotificationPostedCb(postCb);
    NSSetNotificationSyncCb(syncCb);
    NSSetIsStartedConsumer(true);

    if (NS_OK != NSConsumerMessageHandlerInit())
    {
        NSStopConsumer();
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

    NSConsumerMessageHandlerExit();

    return NS_OK;
}

NSResult NSSubscribe(NSProvider * provider)
{
    NSTask * subscribeTask = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE, (void *) provider);
    if (!subscribeTask)
    {
        NS_LOG(ERROR, "NSTask allocation fail");
        return NS_ERROR;
    }

    return NSConsumerPushEvent(subscribeTask);
}

NSResult NSUnsubscribe(NSProvider * provider)
{
    NSTask * unsubscribeTask = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL, (void *) provider);
    if (!unsubscribeTask)
    {
        NS_LOG(ERROR, "NSTask allocation fail");
        return NS_ERROR;
    }

    return NSConsumerPushEvent(unsubscribeTask);
}

NSResult NSRescanProvider()
{
    NSTask * discoverTask = NSMakeTask(TASK_CONSUMER_REQ_DISCOVER, NULL);
    if (!discoverTask)
    {
        NS_LOG(ERROR, "NSTask allocation fail");
        return NS_ERROR;
    }

    return NSConsumerPushEvent(discoverTask);
}

NSResult NSConsumerReadCheck(NSMessage * obj)
{
    NSTask * readTask = NSMakeTask(TASK_SEND_READ, (void *) obj);
    if (!readTask)
    {
        NS_LOG(ERROR, "NSTask allocation fail");
        return NS_ERROR;
    }

    return NSConsumerPushEvent(readTask);
}

NSResult NSConsumerDismissCheck(NSMessage * obj)
{
    NSTask * dismissTask = NSMakeTask(TASK_SEND_DISMISS, (void *) obj);
    if (!dismissTask)
    {
        NS_LOG(ERROR, "NSTask allocation fail");
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

    if (obj->mSource)
    {
        OICFree(obj->mSource);
        obj->mSource = NULL;
    }

    OICFree(obj);

    return NS_OK;
}
