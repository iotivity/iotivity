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

#ifndef _NS_CONSUMER_CONSTANTS_H_
#define _NS_CONSUMER_CONSTANTS_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdbool.h>

#include "NSConsumerInterface.h"
#include "NSStructs.h"
#include "ocstack.h"

#define NS_QOS OC_LOW_QOS
#define NS_RESOURCE_TYPE "oic.r.notification"
#define NS_RESOURCE_URI "/notification"
#define NS_INTERFACE_BASELINE "oic.if.baseline"
#define NS_INTERFACE_NOTIFICATION "oic.if.notification"
#define NS_RESOURCE_QUERY "/oic/res"

#define NS_DISCOVER_QUERY "/oic/res?rt=oic.r.notification"

#define NS_VERTIFY_NOT_NULL_V(obj) { if ((obj) == NULL) \
    { \
        NS_LOG_V(ERROR, "%s : obj is null", __func__); \
        return; \
    }}

#define NS_VERTIFY_NOT_NULL(obj, retVal) { if ((obj) == NULL) \
    { \
        NS_LOG_V(ERROR, "%s : obj is null", __func__); \
        return (retVal); \
    }}

#define NS_VERTIFY_NOT_NULL_WITH_POST_CLEANING(obj, retVal, func) { \
    if ((obj) == NULL) \
    { \
        NS_LOG_V(ERROR, "%s : obj is null", __func__); \
        NS_LOG(ERROR, "execute deletion"); \
        (func); \
        return (retVal); \
    }}

#define NS_VERTIFY_STACK_OK_V(obj) { OCStackResult _ret = (obj); \
    if ( _ret != OC_STACK_OK) \
    { \
        NS_LOG_V(ERROR, "%s : obj is not OC_STACK_OK : %d", __func__, _ret); \
        return; \
    }}

#define NS_VERTIFY_STACK_OK(obj, retVal) { OCStackResult _ret = (obj); \
    if ( _ret != OC_STACK_OK) \
    { \
        NS_LOG_V(ERROR, "%s : obj is not OC_STACK_OK : %d", __func__, _ret); \
        return (retVal); \
    }}

#define NS_VERTIFY_STACK_OK_WITH_POST_CLEANING(obj, retVal, func) { \
    OCStackResult _ret = (obj); \
    if ( _ret != OC_STACK_OK) \
    { \
        NS_LOG_V(ERROR, "%s : obj is not OC_STACK_OK : %d", __func__, _ret); \
        (func); \
        return (retVal); \
    }}

typedef enum
{
    Read,
    Dismiss,
    Notification,
} NSConsumerMessageTypes;

typedef struct
{
    // Mandatory
    char * mId;
    char * mTitle;

    //Optional
    char * mContentText;
    char * mSource;

    OCDevAddr * addr;
    char * syncUri;
    NSConsumerMessageTypes type;
} NSMessage_consumer;

bool NSIsStartedConsumer();
void NSSetIsStartedConsumer(bool setValue);

void NSSetDiscoverProviderCb(NSProviderDiscoveredCallback cb);
void NSDiscoveredProvider(NSProvider * handle);

void NSSetNotificationPostedCb(NSNotificationReceivedCallback  cb);
void NSNotificationPost(NSProvider * provider, NSMessage * obj);

void NSSetNotificationSyncCb(NSSyncCallback cb);
void NSNotificationSync(NSProvider * provider, NSSync * sync);

NSTask * NSMakeTask(NSTaskType, void *);

NSResult NSConsumerPushEvent(NSTask *);

NSMessage_consumer * NSCopyMessage(NSMessage_consumer *);
void NSRemoveMessage(NSMessage_consumer *);

OCStackResult NSInvokeRequest(OCDoHandle * handle,
        OCMethod method, const OCDevAddr * addr,
        const char * queryUrl, OCPayload * payload,
        void * callbackFunc, void * callbackData);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NS_CONSUMER_CONSTANTS_H_
