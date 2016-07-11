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
#define NS_DEVICE_ID_LENGTH 37

#define NS_VERIFY_NOT_NULL_V(obj) \
    { \
        if ((obj) == NULL) \
        { \
            NS_LOG_V(ERROR, "%s : %s is null", __func__, #obj); \
            return; \
        } \
    }

#define NS_VERIFY_NOT_NULL(obj, retVal) \
    { \
        if ((obj) == NULL) \
        { \
            NS_LOG_V(ERROR, "%s : %s is null", __func__, #obj); \
            return (retVal); \
        } \
    }

#define NS_VERIFY_NOT_NULL_WITH_POST_CLEANING_V(obj, func) \
    { \
        if ((obj) == NULL) \
        { \
            NS_LOG_V(ERROR, "%s : %s is null", __func__, #obj); \
            NS_LOG(ERROR, "execute deletion"); \
            (func); \
            return; \
        } \
    }

#define NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(obj, retVal, func) \
    { \
        if ((obj) == NULL) \
        { \
            NS_LOG_V(ERROR, "%s : %s is null", __func__, #obj); \
            NS_LOG(ERROR, "execute deletion"); \
            (func); \
            return (retVal); \
        } \
    }

#define NS_VERIFY_STACK_OK_V(obj) \
    { \
        OCStackResult _ret = (obj); \
        if ( _ret != OC_STACK_OK) \
        { \
            NS_LOG_V(ERROR, "%s : %s is not OC_STACK_OK : %d", __func__, #obj, _ret); \
            return; \
        } \
    }

#define NS_VERIFY_STACK_OK(obj, retVal) \
    { \
        OCStackResult _ret = (obj); \
        if ( _ret != OC_STACK_OK) \
        { \
            NS_LOG_V(ERROR, "%s : %s is not OC_STACK_OK : %d", __func__, #obj, _ret); \
            return (retVal); \
        } \
    }

#define NS_VERIFY_STACK_OK_WITH_POST_CLEANING(obj, retVal, func) \
    { \
        OCStackResult _ret = (obj); \
        if ( _ret != OC_STACK_OK) \
        { \
            NS_LOG_V(ERROR, "%s : %s is not OC_STACK_OK : %d", __func__, #obj, _ret); \
            (func); \
            return (retVal); \
        } \
    }

#define NSOICFree(obj) \
    { \
        if ((obj)) \
        { \
            OICFree((obj)); \
            (obj) = NULL; \
            NS_LOG_V(DEBUG, "%s : %s Removed", __func__, #obj); \
        } \
    }

typedef struct NSProviderConnectionInfo
{
    OCDevAddr * addr;

    OCDoHandle messageHandle;
    OCDoHandle syncHandle;

    struct NSProviderConnectionInfo * next;

} NSProviderConnectionInfo;

typedef struct
{
    char providerId[NS_DEVICE_ID_LENGTH];

    char * messageUri;
    char * syncUri;

    NSAccessPolicy accessPolicy;

    NSProviderConnectionInfo * connection;

} NSProvider_internal;

typedef struct
{
    uint64_t messageId;
    char providerId[NS_DEVICE_ID_LENGTH];
    NSSyncType state;

    NSProviderConnectionInfo * connection;

} NSSyncInfo_internal;

typedef struct
{
    // Mandatory
    uint64_t messageId;
    char providerId[NS_DEVICE_ID_LENGTH];

    //optional
    NSMessageType type;
    char * dateTime;
    uint64_t ttl;
    char * title;
    char * contentText;
    char * sourceName;
    NSMediaContents mediaContents;

    OCDevAddr * i_addr;
    NSSyncType i_messageTypes;
} NSMessage_consumer;

bool NSIsStartedConsumer();
void NSSetIsStartedConsumer(bool setValue);

void NSSetDiscoverProviderCb(NSProviderDiscoveredCallback cb);
void NSDiscoveredProvider(NSProvider * provider);

void NSSetSubscriptionAcceptedCb(NSSubscriptionAcceptedCallback cb);
void NSSubscriptionAccepted(NSProvider * provider);

void NSSetMessagePostedCb(NSMessageReceivedCallback  cb);
void NSMessagePost(NSMessage * obj);

void NSSetNotificationSyncCb(NSSyncInfoReceivedCallback cb);
void NSNotificationSync(NSSyncInfo * sync);

char ** NSGetConsumerId();
void NSSetConsumerId(char * cId);

char * NSMakeRequestUriWithConsumerId(const char * uri);

NSTask * NSMakeTask(NSTaskType, void *);

NSResult NSConsumerPushEvent(NSTask *);

NSMessage_consumer * NSCopyMessage(NSMessage_consumer *);
void NSRemoveMessage(NSMessage_consumer *);

NSProviderConnectionInfo * NSCreateProviderConnections(OCDevAddr * inAddr);
NSProviderConnectionInfo * NSCopyProviderConnections(NSProviderConnectionInfo * conn);
void NSRemoveConnections(NSProviderConnectionInfo * connections);

NSProvider_internal * NSCopyProvider(NSProvider_internal *);
void NSRemoveProvider(NSProvider_internal *);

OCStackResult NSInvokeRequest(OCDoHandle * handle,
        OCMethod method, const OCDevAddr * addr,
        const char * queryUrl, OCPayload * payload,
        void * callbackFunc, void * callbackData, OCConnectivityType type);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NS_CONSUMER_CONSTANTS_H_
