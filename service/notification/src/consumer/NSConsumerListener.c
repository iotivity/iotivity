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

#include "NSConsumerListener.h"

#include <memory.h>
#include <string.h>

#include "NSConstants.h"
#include "NSConsumerCommon.h"
#include "payload_logging.h"
#include "cautilinterface.h"

#include "NSConsumerDiscovery.h"

#define NS_PRESENCE_SUBSCRIBE_QUERY "coap://224.0.1.187:5683/oic/ad?rt=oic.r.notification"

typedef OCStackApplicationResult(*onRIResponse)(OCDoHandle, OCClientResponse *);

OCStackApplicationResult NSRIResponseListener(
        void * ctx, OCDoHandle handle, OCClientResponse * clientResponse);

void NSConnectionStateListener(CATransportAdapter_t adapter,
        const char *remote_address, bool connected);

void NSAdapterStateListener(CATransportAdapter_t adapter, bool enabled);

NSResult NSConsumerListenerInit()
{
    NSSetResponseListener(NSRIResponseListener);

    // TODO replace with comment lines when enable network monitor of IP Adapter
    CARegisterNetworkMonitorHandler(NSAdapterStateListener, NSConnectionStateListener);
//    if (CARegisterNetworkMonitorHandler(NSAdapterStateListener, NSConnectionStateListener)
//            != CA_STATUS_OK)
//    {
//        return NS_ERROR;
//    }

    if (OC_STACK_OK != NSRequestToResourceIntrospection(NULL, OC_REST_PRESENCE, NULL,
            NS_PRESENCE_SUBSCRIBE_QUERY, NULL, NSConsumerPresenceListener))
    {
        NS_LOG(ERROR, "Presence request fail");
        return NS_ERROR;
    }

    if (OC_STACK_OK != NSRequestToResourceIntrospection(NULL, OC_REST_DISCOVER, NULL,
            NS_DISCOVER_QUERY, NULL, NSProviderDiscoverListener))
    {
        NS_LOG(ERROR, "Discover request fail");
        return NS_ERROR;
    }

    return NS_OK;
}

OCStackApplicationResult NSRIResponseListener(
        void * ctx, OCDoHandle handle, OCClientResponse * clientResponse)
{
    if (ctx == NULL)
    {
        NS_LOG(ERROR, "Callback is null");
        return OC_STACK_DELETE_TRANSACTION;
    }

    if (clientResponse->result == OC_STACK_OK && clientResponse != NULL)
    {
        OIC_LOG_PAYLOAD(INFO, clientResponse->payload);
        ((onRIResponse)ctx)(handle, clientResponse);
    }
    else
    {
        NS_LOG_V(ERROR, "result is not ok : %d", clientResponse->result);
    }

    return OC_STACK_KEEP_TRANSACTION;
}

void NSConnectionStateListener(CATransportAdapter_t adapter,
        const char *remote_address, bool connected)
{
    NS_LOG_V(DEBUG, "adapter : %d", adapter);
    NS_LOG_V(DEBUG, "remote_address : %s", remote_address);
    NS_LOG_V(DEBUG, "isConnect : %d", connected);

    (void) adapter;
    (void) remote_address;

    if (connected)
    {
        NS_LOG(DEBUG, "try to discover notification provider.");

        NSTask * task = NSMakeTask(TASK_EVENT_CONNECTED, NULL);
        if (!task)
        {
            NS_LOG(ERROR, "NSTask allocation fail.");
            return;
        }
        NSConsumerPushEvent(task);
    }
}

void NSAdapterStateListener(CATransportAdapter_t adapter, bool enabled)
{
    NS_LOG_V(DEBUG, "adapter : %d", adapter);
    NS_LOG_V(DEBUG, "isEnabled : %d", enabled);

    (void) adapter;

    if (enabled)
    {
        NS_LOG(DEBUG, "try to discover notification provider.");

        NSTask * task = NSMakeTask(TASK_EVENT_CONNECTED, NULL);
        if (!task)
        {
            NS_LOG(ERROR, "NSTask allocation fail.");
            return;
        }
        NSConsumerPushEvent(task);
    }
}
