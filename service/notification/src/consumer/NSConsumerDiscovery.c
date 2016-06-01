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

#include "NSConsumerDiscovery.h"

#include <string.h>
#include "NSCommon.h"
#include "NSConsumerCommon.h"
#include "NSConstants.h"
#include "ocpayload.h"
#include "oic_malloc.h"

#define NS_PAYLOAD_KEY_ACCEPTER "ACCEPTER"
#define NS_DISCOVER_QUERY "/oic/res?rt=oic.r.notification"
#define NS_PRESENCE_SUBSCRIBE_QUERY "coap://224.0.1.187:5683/oic/ad?rt=oic.r.notification"
#define NS_GET_INFORMATION_QUERY "/notification?if=oic.if.notification"

OCDoHandle * getPresenceHandle()
{
    static OCDoHandle * g_PresenceHandle = NULL;

    return g_PresenceHandle;
}

OCStackApplicationResult NSConsumerPresenceListener(
        void * ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    (void) ctx;
    (void) handle;

    NS_VERTIFY_NOT_NULL(clientResponse, OC_STACK_KEEP_TRANSACTION);
    NS_VERTIFY_STACK_OK(clientResponse->result, OC_STACK_KEEP_TRANSACTION);

    NS_LOG_V(DEBUG, "Presence income : %s:%d",
            clientResponse->devAddr.addr, clientResponse->devAddr.port);
    NS_LOG_V(DEBUG, "Presence result : %d",
            clientResponse->result);
    NS_LOG_V(DEBUG, "Presence sequenceNum : %d",
            clientResponse->sequenceNumber);

    if (!NSIsStartedConsumer())
    {
        return OC_STACK_DELETE_TRANSACTION;
    }

    OCPresencePayload * payload = (OCPresencePayload *)clientResponse->payload;
    if (payload->trigger == OC_PRESENCE_TRIGGER_DELETE ||
            clientResponse->result == OC_STACK_PRESENCE_STOPPED)
    {
        // TODO find request and cancel
        NS_LOG(DEBUG, "stopped presence or resource is deleted.");
        //OCCancel(handle, NS_QOS, NULL, 0);
    }

    else if (payload->trigger == OC_PRESENCE_TRIGGER_CREATE)
    {
        NSInvokeRequest(NULL, OC_REST_DISCOVER, clientResponse->addr,
            NS_DISCOVER_QUERY, NULL, NSProviderDiscoverListener, NULL);
    }

    return OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult NSProviderDiscoverListener(
        void * ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    (void) ctx;
    (void) handle;

    NS_VERTIFY_NOT_NULL(clientResponse, OC_STACK_KEEP_TRANSACTION);
    NS_VERTIFY_STACK_OK(clientResponse->result, OC_STACK_KEEP_TRANSACTION);

    NS_LOG_V(DEBUG, "Discover income : %s:%d",
            clientResponse->devAddr.addr, clientResponse->devAddr.port);
    NS_LOG_V(DEBUG, "Discover result : %d",
            clientResponse->result);
    NS_LOG_V(DEBUG, "Discover sequenceNum : %d",
            clientResponse->sequenceNumber);

    if (!NSIsStartedConsumer())
    {
        return OC_STACK_DELETE_TRANSACTION;
    }

    if (!clientResponse->payload)
    {
        return OC_STACK_KEEP_TRANSACTION;
    }

    OCResourcePayload * resource = ((OCDiscoveryPayload *)clientResponse->payload)->resources;
    while (resource)
    {
        if (!strcmp(resource->uri, NS_RESOURCE_URI))
        {
            NSInvokeRequest(NULL, OC_REST_GET, clientResponse->addr,
                    NS_RESOURCE_URI, NULL, NSIntrospectProvider, NULL);
        }
        resource = resource->next;
    }

    return OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult NSIntrospectProvider(
        void * ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    (void) ctx;
    (void) handle;

    NS_VERTIFY_NOT_NULL(clientResponse, OC_STACK_KEEP_TRANSACTION);
    NS_VERTIFY_STACK_OK(clientResponse->result, OC_STACK_KEEP_TRANSACTION);

    int64_t accepter = 0;

    NS_LOG_V(DEBUG, "GET response income : %s:%d",
            clientResponse->devAddr.addr, clientResponse->devAddr.port);
    NS_LOG_V(DEBUG, "GET response result : %d",
            clientResponse->result);
    NS_LOG_V(DEBUG, "GET response sequenceNum : %d",
            clientResponse->sequenceNumber);
    NS_LOG_V(DEBUG, "GET response resource uri : %s",
            clientResponse->resourceUri);

    if (!NSIsStartedConsumer())
    {
        return OC_STACK_DELETE_TRANSACTION;
    }

    if (!clientResponse->payload)
    {
        NS_LOG(ERROR, "payload is null");
        return OC_STACK_KEEP_TRANSACTION;
    }

    if (!OCRepPayloadGetPropInt((OCRepPayload *)clientResponse->payload,
            NS_PAYLOAD_KEY_ACCEPTER, & accepter))
    {
        NS_LOG(ERROR, "can not seach for accepter");
        return OC_STACK_KEEP_TRANSACTION;
    }

    NSProvider * newProvider = (NSProvider *)OICMalloc(sizeof(NSProvider));
    if (!newProvider)
    {
        NS_LOG(DEBUG, "NSProvider allocation fail");
        return OC_STACK_KEEP_TRANSACTION;
    }

    // TODO set id
    newProvider->mId = NULL;
    newProvider->mUserData = (void *)OICMalloc(sizeof(OCDevAddr));
    if (!newProvider)
    {
        NS_LOG(DEBUG, "OCDevAddr allocation fail");
        OICFree(newProvider);
        return OC_STACK_KEEP_TRANSACTION;
    }
    memcpy(newProvider->mUserData, clientResponse->addr, sizeof(OCDevAddr));

    {
        OCRepPayload * payload = (OCRepPayload *)clientResponse->payload;
        while (payload)
        {
            NS_LOG_V(DEBUG, "Payload Key : %s", payload->values->name);
            payload = payload->next;
        }
    }

    if (!OCRepPayloadGetPropString((OCRepPayload *)clientResponse->payload,
            "MESSAGE_URI", & newProvider->messageUri))
    {
        OICFree(newProvider->mUserData);
        OICFree(newProvider);
        NS_LOG(ERROR, "can not seach for message uri");
        return OC_STACK_KEEP_TRANSACTION;
    }

    if (!OCRepPayloadGetPropString((OCRepPayload *)clientResponse->payload,
            NS_ATTRIBUTE_SYNC, & newProvider->syncUri))
    {
        OICFree(newProvider->messageUri);
        OICFree(newProvider->mUserData);
        OICFree(newProvider);
        NS_LOG(ERROR, "can not seach for sync uri");
        return OC_STACK_KEEP_TRANSACTION;
    }

    if (accepter == NS_ACCEPTER_CONSUMER)
    {
        NS_LOG(DEBUG, "accepter is NS_ACCEPTER_CONSUMER, Callback to user");

        NSDiscoveredProvider(newProvider);
    }
    else
    {
        NS_LOG(DEBUG, "accepter is NS_ACCEPTER_PROVIDER, request subscribe");

        NSTask * task = NSMakeTask(TASK_CONSUMER_REQ_SUBSCRIBE, (void *) newProvider);
        if (!task)
        {
            NS_LOG(DEBUG, "NSTask allocation fail");
            return OC_STACK_KEEP_TRANSACTION;
        }

        NSConsumerPushEvent(task);
    }

    return OC_STACK_KEEP_TRANSACTION;
}

void NSConsumerDiscoveryTaskProcessing(NSTask * task)
{
    if (!task)
    {
        NS_LOG(ERROR, "task is null");
        return;
    }

    NS_LOG_V(DEBUG, "Receive Event : %d", (int)task->taskType);
    if (task->taskType == TASK_EVENT_CONNECTED || task->taskType == TASK_CONSUMER_REQ_DISCOVER)
    {
        NSInvokeRequest(NULL, OC_REST_DISCOVER, NULL, NS_DISCOVER_QUERY,
                NULL, NSProviderDiscoverListener, NULL);
    }
    else
    {
        NS_LOG(ERROR, "Unknown type message");
    }
}

