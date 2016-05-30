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

#include "NSProviderListener.h"

OCEntityHandlerResult NSEntityHandlerNotificationCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback)
{
    NS_LOG(DEBUG, "NSEntityHandlerNotificationCb - IN");

    OCEntityHandlerResult ehResult = OC_EH_OK;
    OCEntityHandlerResponse response =
    { 0, 0, OC_EH_ERROR, 0, 0,
    { },
    { 0 }, false };

    (void)callback;

    // Validate pointer
    if (!entityHandlerRequest)
    {
        NS_LOG(ERROR, "Invalid request pointer");
        return OC_EH_ERROR;
    }

    // Initialize certain response fields
    response.numSendVendorSpecificHeaderOptions = 0;
    memset(response.sendVendorSpecificHeaderOptions, 0,
            sizeof response.sendVendorSpecificHeaderOptions);
    memset(response.resourceUri, 0, sizeof response.resourceUri);
    OCRepPayload* payload = NULL;

    if (flag & OC_REQUEST_FLAG)
    {
        NS_LOG(DEBUG, "Flag includes OC_REQUEST_FLAG");

        if (OC_REST_GET == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerNotificationCb - OC_REST_GET");

            NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_SEND_POLICY,
                    NSCopyOCEntityHandlerRequest(entityHandlerRequest));
            ehResult = OC_EH_OK;

        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerNotificationCb - OC_REST_PUT");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerNotificationCb - OC_REST_POST");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_DELETE == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerNotificationCb - OC_REST_DELETE");
            ehResult = OC_EH_OK;
        }
        else
        {
            NS_LOG_V (DEBUG, "Received unsupported method %d from client",
                    entityHandlerRequest->method);
            ehResult = OC_EH_OK;
        }

        // If the result isn't an error or forbidden, send response
        if (!((ehResult == OC_EH_ERROR) || (ehResult == OC_EH_FORBIDDEN)))
        {
            // Format the response.  Note this requires some info about the request
            response.requestHandle = entityHandlerRequest->requestHandle;
            response.resourceHandle = entityHandlerRequest->resource;
            response.ehResult = ehResult;
            //response.payload = reinterpret_cast<OCPayload*>(payload);
            response.payload = (OCPayload*) payload;
            // Indicate that response is NOT in a persistent buffer
            response.persistentBufferFlag = 0;

            // Handle vendor specific options
            if (entityHandlerRequest->rcvdVendorSpecificHeaderOptions
                    && entityHandlerRequest->numRcvdVendorSpecificHeaderOptions)
            {
                NS_LOG (DEBUG, "Received vendor specific options");
                uint8_t i = 0;
                OCHeaderOption * rcvdOptions = entityHandlerRequest->rcvdVendorSpecificHeaderOptions;
                for (i = 0; i < entityHandlerRequest->numRcvdVendorSpecificHeaderOptions; i++)
                {
                    if (((OCHeaderOption) rcvdOptions[i]).protocolID == OC_COAP_ID)
                    {
                        OIC_LOG_V(DEBUG, LISTENER_TAG, "Received option with OC_COAP_ID and ID %u with",
                                ((OCHeaderOption)rcvdOptions[i]).optionID );

                        OIC_LOG_BUFFER(DEBUG, LISTENER_TAG, ((OCHeaderOption)rcvdOptions[i]).optionData,
                                MAX_HEADER_OPTION_DATA_LENGTH);
                    }
                }
                OCHeaderOption * sendOptions = response.sendVendorSpecificHeaderOptions;
                uint8_t option2[] =
                { 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
                uint8_t option3[] =
                { 31, 32, 33, 34, 35, 36, 37, 38, 39, 40 };
                sendOptions[0].protocolID = OC_COAP_ID;
                sendOptions[0].optionID = 2248;
                memcpy(sendOptions[0].optionData, option2, sizeof(option2));
                sendOptions[0].optionLength = 10;
                sendOptions[1].protocolID = OC_COAP_ID;
                sendOptions[1].optionID = 2600;
                memcpy(sendOptions[1].optionData, option3, sizeof(option3));
                sendOptions[1].optionLength = 10;
                response.numSendVendorSpecificHeaderOptions = 2;
            }
        }
    }

    OCPayloadDestroy(response.payload);
    NS_LOG(DEBUG, "NSEntityHandlerNotificationCb - OUT");
    return ehResult;
}

OCEntityHandlerResult NSEntityHandlerMessageCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback)
{
    NS_LOG(DEBUG, "NSEntityHandlerMessageCb - IN");

    OCEntityHandlerResult ehResult = OC_EH_OK;
    OCEntityHandlerResponse response =
    { 0, 0, OC_EH_ERROR, 0, 0,
    { },
    { 0 }, false };

    (void)callback;

    // Validate pointer
    if (!entityHandlerRequest)
    {
        NS_LOG (ERROR,"Invalid request pointer");
        return OC_EH_ERROR;
    }

    // Initialize certain response fields
    response.numSendVendorSpecificHeaderOptions = 0;
    memset(response.sendVendorSpecificHeaderOptions, 0,
            sizeof response.sendVendorSpecificHeaderOptions);
    memset(response.resourceUri, 0, sizeof response.resourceUri);
    OCRepPayload* payload = NULL;

    if (flag & OC_REQUEST_FLAG)
    {
        NS_LOG(DEBUG, "Flag includes OC_REQUEST_FLAG");

        if (OC_REST_GET == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerMessageCb - OC_REST_GET");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerMessageCb - OC_REST_PUT");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerMessageCb - OC_REST_POST");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_DELETE == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerMessageCb - OC_REST_DELETE");
            ehResult = OC_EH_OK;
        }
        else
        {
            NS_LOG_V(DEBUG, "Received unsupported method %d from client",
                    entityHandlerRequest->method);
            ehResult = OC_EH_OK;
        }

        // If the result isn't an error or forbidden, send response
        if (!((ehResult == OC_EH_ERROR) || (ehResult == OC_EH_FORBIDDEN)))
        {
            // Format the response.  Note this requires some info about the request
            response.requestHandle = entityHandlerRequest->requestHandle;
            response.resourceHandle = entityHandlerRequest->resource;
            response.ehResult = ehResult;
            //response.payload = reinterpret_cast<OCPayload*>(payload);
            response.payload = (OCPayload*) payload;
            // Indicate that response is NOT in a persistent buffer
            response.persistentBufferFlag = 0;

            // Handle vendor specific options
            if (entityHandlerRequest->rcvdVendorSpecificHeaderOptions
                    && entityHandlerRequest->numRcvdVendorSpecificHeaderOptions)
            {
                NS_LOG(DEBUG, "Received vendor specific options");
                uint8_t i = 0;
                OCHeaderOption * rcvdOptions = entityHandlerRequest->rcvdVendorSpecificHeaderOptions;
                for (i = 0; i < entityHandlerRequest->numRcvdVendorSpecificHeaderOptions; i++)
                {
                    if (((OCHeaderOption) rcvdOptions[i]).protocolID == OC_COAP_ID)
                    {
                        OIC_LOG_V(DEBUG, LISTENER_TAG, "Received option with OC_COAP_ID and ID %u with",
                                ((OCHeaderOption)rcvdOptions[i]).optionID );

                        OIC_LOG_BUFFER(DEBUG, LISTENER_TAG, ((OCHeaderOption)rcvdOptions[i]).optionData,
                                MAX_HEADER_OPTION_DATA_LENGTH);
                    }
                }
                OCHeaderOption * sendOptions = response.sendVendorSpecificHeaderOptions;
                uint8_t option2[] =
                { 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
                uint8_t option3[] =
                { 31, 32, 33, 34, 35, 36, 37, 38, 39, 40 };
                sendOptions[0].protocolID = OC_COAP_ID;
                sendOptions[0].optionID = 2248;
                memcpy(sendOptions[0].optionData, option2, sizeof(option2));
                sendOptions[0].optionLength = 10;
                sendOptions[1].protocolID = OC_COAP_ID;
                sendOptions[1].optionID = 2600;
                memcpy(sendOptions[1].optionData, option3, sizeof(option3));
                sendOptions[1].optionLength = 10;
                response.numSendVendorSpecificHeaderOptions = 2;
            }
        }
    }

    if (flag & OC_OBSERVE_FLAG)
    {
        NS_LOG(DEBUG, "NSEntityHandlerMessageCb - OC_OBSERVE_FLAG");

        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
            NS_LOG(DEBUG, "NSEntityHandlerMessageCb - OC_OBSERVE_REGISTER");
            NS_LOG_V(DEBUG, "NSEntityHandlerMessageCb\n"
                    "Register message observerID : %d\n", entityHandlerRequest->obsInfo.obsId);
            NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_RECV_SUBSCRIPTION,
                    NSCopyOCEntityHandlerRequest(entityHandlerRequest));
        }
    }

    OCPayloadDestroy(response.payload);
    NS_LOG(DEBUG, "NSEntityHandlerMessageCb - OUT");
    return ehResult;
}

OCEntityHandlerResult NSEntityHandlerSyncCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback)
{
    NS_LOG(DEBUG, "NSEntityHandlerSyncCb - IN");
    OCEntityHandlerResult ehResult = OC_EH_OK;
    OCEntityHandlerResponse response =
    { 0, 0, OC_EH_ERROR, 0, 0,
    { },
    { 0 }, false };

    (void)callback;

    // Validate pointer
    if (!entityHandlerRequest)
    {
        NS_LOG(ERROR, "Invalid request pointer");
        return OC_EH_ERROR;
    }

    // Initialize certain response fields
    response.numSendVendorSpecificHeaderOptions = 0;
    memset(response.sendVendorSpecificHeaderOptions, 0,
            sizeof response.sendVendorSpecificHeaderOptions);
    memset(response.resourceUri, 0, sizeof response.resourceUri);
    OCRepPayload* payload = NULL;

    if (flag & OC_REQUEST_FLAG)
    {
        NS_LOG(DEBUG, "Flag includes OC_REQUEST_FLAG");

        if (OC_REST_GET == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerSyncCb - OC_REST_GET");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerSyncCb - OC_REST_PUT");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            /** Receive sync data from consumer which read or dismiss notification message.
                           And broadcast the sync data to all subscribers including provider app
                           to synchronize the notification message status. */

            NS_LOG(DEBUG, "NSEntityHandlerSyncCb - OC_REST_POST");

            NSPushQueue(NOTIFICATION_SCHEDULER, TASK_RECV_READ,
                    NSBuildOICNotificationSync(entityHandlerRequest->payload));
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_DELETE == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "Received OC_REST_DELETE from client");
            ehResult = OC_EH_OK;
        }
        else
        {
            NS_LOG_V(DEBUG, "Received unsupported method %d from client",
                    entityHandlerRequest->method);
            ehResult = OC_EH_OK;
        }

        // If the result isn't an error or forbidden, send response
        if (!((ehResult == OC_EH_ERROR) || (ehResult == OC_EH_FORBIDDEN)))
        {
            // Format the response.  Note this requires some info about the request
            response.requestHandle = entityHandlerRequest->requestHandle;
            response.resourceHandle = entityHandlerRequest->resource;
            response.ehResult = ehResult;
            //response.payload = reinterpret_cast<OCPayload*>(payload);
            response.payload = (OCPayload*) payload;
            // Indicate that response is NOT in a persistent buffer
            response.persistentBufferFlag = 0;

            // Handle vendor specific options
            if (entityHandlerRequest->rcvdVendorSpecificHeaderOptions
                    && entityHandlerRequest->numRcvdVendorSpecificHeaderOptions)
            {
                NS_LOG(DEBUG, "Received vendor specific options");
                uint8_t i = 0;
                OCHeaderOption * rcvdOptions = entityHandlerRequest->rcvdVendorSpecificHeaderOptions;
                for (i = 0; i < entityHandlerRequest->numRcvdVendorSpecificHeaderOptions; i++)
                {
                    if (((OCHeaderOption) rcvdOptions[i]).protocolID == OC_COAP_ID)
                    {
                        OIC_LOG_V(DEBUG, LISTENER_TAG, "Received option with OC_COAP_ID and ID %u with",
                                ((OCHeaderOption)rcvdOptions[i]).optionID );

                        OIC_LOG_BUFFER(DEBUG, LISTENER_TAG, ((OCHeaderOption)rcvdOptions[i]).optionData,
                                MAX_HEADER_OPTION_DATA_LENGTH);
                    }
                }
                OCHeaderOption * sendOptions = response.sendVendorSpecificHeaderOptions;
                uint8_t option2[] =
                { 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
                uint8_t option3[] =
                { 31, 32, 33, 34, 35, 36, 37, 38, 39, 40 };
                sendOptions[0].protocolID = OC_COAP_ID;
                sendOptions[0].optionID = 2248;
                memcpy(sendOptions[0].optionData, option2, sizeof(option2));
                sendOptions[0].optionLength = 10;
                sendOptions[1].protocolID = OC_COAP_ID;
                sendOptions[1].optionID = 2600;
                memcpy(sendOptions[1].optionData, option3, sizeof(option3));
                sendOptions[1].optionLength = 10;
                response.numSendVendorSpecificHeaderOptions = 2;
            }
        }
    }

    if (flag & OC_OBSERVE_FLAG)
    {
        /** Requested by consumers to synchronize notification message status.
            Store the observer IDs to storage or cache */

        NS_LOG(DEBUG, "NSEntityHandlerSyncCb - OC_OBSERVE_FLAG");

        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
            NS_LOG(DEBUG, "NSEntityHandlerSyncCb - OC_OBSERVE_REGISTER");
            NS_LOG_V(DEBUG, "NSEntityHandlerSyncCb\n - "
                    "Register Sync observerID : %d\n", entityHandlerRequest->obsInfo.obsId);
            NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_SYNC_SUBSCRIPTION,
                    NSCopyOCEntityHandlerRequest(entityHandlerRequest));
        }
    }

    OCPayloadDestroy(response.payload);
    NS_LOG(DEBUG, "NSEntityHandlerSyncCb - OUT");
    return ehResult;
}

void NSProviderConnectionStateListener(CATransportAdapter_t adapter, const char *remote_address,
        bool connected)
{

    // should be implementation
    (void)adapter;
    (void)remote_address;

    NS_LOG(DEBUG, "NSProviderConnectionStateListener - IN");

    if (connected)
    {
        NS_LOG(DEBUG, "CONNECTED");

        // Set Connection State
        NSSetProviderConnectionState(CONNECTED);

        // Start Presence
        NSPushQueue(DISCOVERY_SCHEDULER, TASK_START_PRESENCE, NULL);
    }

    NS_LOG(DEBUG, "NSProviderConnectionStateListener - OUT");
}

void NSProviderAdapterStateListener(CATransportAdapter_t adapter, bool enabled)
{
    // should be implementation
    (void)adapter;

    NS_LOG(DEBUG, "NSProviderAdapterStateListener - IN");

    if (enabled)
    {
        NS_LOG(DEBUG, "CONNECTED");

        // Set Connection State
        NSSetProviderConnectionState(CONNECTED);

        // Start Presence
        NSPushQueue(DISCOVERY_SCHEDULER, TASK_START_PRESENCE, NULL);
    }

    NS_LOG(DEBUG, "NSProviderAdapterStateListener - OUT");
}

NSSync * NSBuildOICNotificationSync(OCPayload * payload)
{
    NS_LOG(DEBUG, "NSBuildOICNotificationSync - IN");

    if(!payload)
    {
        return NULL;
    }
    NSSync * retSync = (NSSync *)OICMalloc(sizeof(NSSync));
    if (!retSync)
    {
        return NULL;
    }

    retSync->mMessageId = NULL;
    retSync->mState = Notification_Read;

    OCRepPayload * repPayload = (OCRepPayload *)payload;
    if (!OCRepPayloadGetPropString(repPayload, NS_ATTRIBUTE_ID, &retSync->mMessageId))
    {
        OICFree(retSync);
        return NULL;
    }
    int64_t state;
    if (!OCRepPayloadGetPropInt(repPayload, NS_ATTRIBUTE_STATE, & state))
    {
        OICFree(retSync->mMessageId);
        OICFree(retSync);
        return NULL;
    }

    retSync->mState = (NSSyncTypes) state;

    NS_LOG_V(DEBUG, "Sync ID : %s", retSync->mMessageId);
    NS_LOG_V(DEBUG, "Sync State : %d", (int) retSync->mState);

    NS_LOG(DEBUG, "NSBuildOICNotificationSync - OUT");

    return retSync;
}

NSResult NSMakeTask(NSTaskType type, OCEntityHandlerRequest *request, NSTask * task)
{
    NS_LOG(DEBUG, "NSMakeTask - IN");

    task = (NSTask*) OICMalloc(sizeof(NSTask));
    if (!task)
    {
        NS_LOG(ERROR, "Fail to allocate memory");
        return NS_ERROR;
    }

    task->taskType = type;
    task->taskData = request;
    task->nextTask = NULL;

    NS_LOG(DEBUG, "NSMakeTask - OUT");
    return NS_OK;
}

