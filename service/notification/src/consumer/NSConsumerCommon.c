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

#include "NSConsumerCommon.h"
#include "NSConstants.h"
#include "NSThread.h"
#include "oic_malloc.h"
#include "oic_string.h"

bool * NSGetBoneIsStartedConsumer()
{
    static bool g_isStartedConsumer = false;

    return & g_isStartedConsumer;
}

void NSSetIsStartedConsumer(bool setValue)
{
    * NSGetBoneIsStartedConsumer() = setValue;
}

bool NSIsStartedConsumer()
{
    return * NSGetBoneIsStartedConsumer();
}

NSProviderDiscoveredCallback * NSGetBoneDiscoverCb()
{
    static NSProviderDiscoveredCallback g_discoverCb = NULL;

    return & g_discoverCb;
}

void NSSetDiscoverProviderCb(NSProviderDiscoveredCallback cb)
{
    * NSGetBoneDiscoverCb() = cb;
}

NSProviderDiscoveredCallback NSGetDiscoverCb()
{
    return * NSGetBoneDiscoverCb();
}

void * NSDiscoverdProviderFunc(void * provider)
{
    NSGetDiscoverCb()((NSProvider *) provider);

    return NULL;
}

void NSDiscoveredProvider(NSProvider * provider)
{
    if (!NSThreadInit(NSDiscoverdProviderFunc, (void *) provider))
    {
        NS_LOG(ERROR, "execute discovered provider callback fail");
        return;
    }
}

NSSyncCallback * NSGetBoneNotificationSyncCb()
{
    static NSSyncCallback g_syncCb = NULL;

    return & g_syncCb;
}

void NSSetNotificationSyncCb(NSSyncCallback cb)
{
    * NSGetBoneNotificationSyncCb() = cb;
}

typedef struct
{
    NSProvider * provider;
    NSSync * sync;
} NSSyncData;

void * NSNotificationSyncFunc(void * obj)
{
    NSProvider * provider = ((NSSyncData *) obj)->provider;
    NSSync * syncData = ((NSSyncData *) obj)->sync;
    (* NSGetBoneNotificationSyncCb())(provider, syncData);
    return NULL;
}

void NSNotificationSync(NSProvider * provider, NSSync * sync)
{
    NSSyncData * obj = (NSSyncData *)OICMalloc(sizeof(NSSyncData));
    obj->provider = provider;
    obj->sync = sync;

    if (!NSThreadInit(NSNotificationSyncFunc, (void *) obj))
    {
        NS_LOG(ERROR, "execute noti post callback fail");
        return;
    }
}

NSNotificationReceivedCallback  * NSGetBoneNotificationPostedCb()
{
    static NSNotificationReceivedCallback  g_postCb = NULL;

    return & g_postCb;
}

void NSSetNotificationPostedCb(NSNotificationReceivedCallback  cb)
{
    * NSGetBoneNotificationPostedCb() = cb;
}

NSNotificationReceivedCallback  NSGetNotificationPostedCb()
{
    return * NSGetBoneNotificationPostedCb();
}

typedef struct
{
    NSProvider * provider;
    NSMessage * msg;
} NSMessageData;

void * NSNotificationPostFunc(void * obj)
{
    NSMessageData * msgData = (NSMessageData *) obj;

    NSGetNotificationPostedCb()((NSProvider *) msgData->provider,
            (NSMessage *) msgData->msg);
    return NULL;
}

void NSNotificationPost(NSProvider * provider, NSMessage * msg)
{
    NSMessageData * obj = (NSMessageData *)OICMalloc(sizeof(NSMessageData));
    if (!obj)
    {
        NS_LOG(ERROR, "NSMessageData allocation is failed");
        return;
    }
    obj->provider = provider;
    obj->msg = msg;

    if (!NSThreadInit(NSNotificationPostFunc, (void *) obj))
    {
        NS_LOG(ERROR, "execute noti post callback fail");
        return;
    }
}

onRIResponseListener * NSGetBoneResponseListener()
{
    static onRIResponseListener g_responseCb = NULL;

    return & g_responseCb;
}

void NSSetResponseListener(onRIResponseListener cb)
{
    * NSGetBoneResponseListener() = cb;
}

onRIResponseListener NSGetResponseListener()
{
    return * NSGetBoneResponseListener();
}

NSTask * NSMakeTask(NSTaskType type, void * data)
{
    NSTask * retTask = OICMalloc(sizeof(NSTask));
    if (!retTask)
    {
        return NULL;
    }

    retTask->taskType = type;
    retTask->taskData = data;
    retTask->nextTask = NULL;

    return retTask;
}

NSMessage_consumer * NSCopyMessage(NSMessage_consumer * msg)
{
    if (!msg)
    {
        return NULL;
    }

    NSMessage_consumer * newMsg = (NSMessage_consumer *)OICMalloc(sizeof(NSMessage_consumer));
    if (!newMsg)
    {
        return NULL;
    }

    newMsg->mId = OICStrdup(msg->mId);
    newMsg->mTitle = OICStrdup(msg->mTitle);
    newMsg->mContentText = OICStrdup(msg->mContentText);
    newMsg->addr = (OCDevAddr *)OICMalloc(sizeof(OCDevAddr));
    if (!newMsg->addr)
    {
        NS_LOG(ERROR, "OCDevAddr allocation is failed");
    }
    memcpy(newMsg->addr, msg->addr, sizeof(OCDevAddr));

    return newMsg;
}
void NSRemoveMessage(NSMessage_consumer * msg)
{
    if (msg->mId)
    {
        OICFree(msg->mId);
        msg->mId = NULL;
    }
    if (msg->mTitle)
    {
        OICFree(msg->mTitle);
        msg->mTitle = NULL;
    }
    if (msg->mContentText)
    {
        OICFree(msg->mContentText);
        msg->mContentText = NULL;
    }
    if (msg->addr)
    {
        OICFree(msg->addr);
        msg->addr = NULL;
    }

    OICFree(msg);
    msg = NULL;
}

OCStackResult NSRequestToResourceIntrospection(OCDoHandle * handle,
        OCMethod method, const OCDevAddr * addr,
        const char * queryUrl, OCPayload * payload, void * callback)
{
    OCCallbackData cbdata;

    cbdata.cb = NSGetResponseListener();
    cbdata.context = callback;
    cbdata.cd = NULL;

    return OCDoResource(handle, method, queryUrl, addr,
            payload, CT_DEFAULT, NS_QOS, &cbdata, NULL, 0);
}

