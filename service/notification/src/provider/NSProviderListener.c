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
    OIC_LOG_V (INFO, LISTENER_TAG, "Inside entity handler - flags: 0x%x", flag);

    OCEntityHandlerResult ehResult = OC_EH_OK;
    OCEntityHandlerResponse response =
    { 0, 0, OC_EH_ERROR, 0, 0,
    { },
    { 0 }, false };

    (void)callback;

    // Validate pointer
    if (!entityHandlerRequest)
    {
        OIC_LOG (ERROR, LISTENER_TAG, "Invalid request pointer");
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
        OIC_LOG (INFO, LISTENER_TAG, "Flag includes OC_REQUEST_FLAG");

        if (OC_REST_GET == entityHandlerRequest->method)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_REST_GET from client");

            NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_SEND_POLICY, (void *)entityHandlerRequest);
            ehResult = OC_EH_OK;

        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_REST_PUT from client");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_REST_POST from client");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_DELETE == entityHandlerRequest->method)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_REST_DELETE from client");
            ehResult = OC_EH_OK;
        }
        else
        {
            OIC_LOG_V (INFO, LISTENER_TAG, "Received unsupported method %d from client",
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
                OIC_LOG (INFO, LISTENER_TAG, "Received vendor specific options");
                uint8_t i = 0;
                OCHeaderOption * rcvdOptions = entityHandlerRequest->rcvdVendorSpecificHeaderOptions;
                for (i = 0; i < entityHandlerRequest->numRcvdVendorSpecificHeaderOptions; i++)
                {
                    if (((OCHeaderOption) rcvdOptions[i]).protocolID == OC_COAP_ID)
                    {
                        OIC_LOG_V(INFO, LISTENER_TAG, "Received option with OC_COAP_ID and ID %u with",
                                ((OCHeaderOption)rcvdOptions[i]).optionID );

                        OIC_LOG_BUFFER(INFO, LISTENER_TAG, ((OCHeaderOption)rcvdOptions[i]).optionData,
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
    return ehResult;
}

OCEntityHandlerResult NSEntityHandlerMessageCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback)
{
    OIC_LOG_V (INFO, LISTENER_TAG, "Inside entity handler - flags: 0x%x", flag);

    OCEntityHandlerResult ehResult = OC_EH_OK;
    OCEntityHandlerResponse response =
    { 0, 0, OC_EH_ERROR, 0, 0,
    { },
    { 0 }, false };

    (void)callback;

    // Validate pointer
    if (!entityHandlerRequest)
    {
        OIC_LOG (ERROR, LISTENER_TAG, "Invalid request pointer");
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
        OIC_LOG (INFO, LISTENER_TAG, "Flag includes OC_REQUEST_FLAG");

        if (OC_REST_GET == entityHandlerRequest->method)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_REST_GET from client");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_REST_PUT from client");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_REST_POST from client");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_DELETE == entityHandlerRequest->method)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_REST_DELETE from client");
            ehResult = OC_EH_OK;
        }
        else
        {
            OIC_LOG_V (INFO, LISTENER_TAG, "Received unsupported method %d from client",
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
                OIC_LOG (INFO, LISTENER_TAG, "Received vendor specific options");
                uint8_t i = 0;
                OCHeaderOption * rcvdOptions = entityHandlerRequest->rcvdVendorSpecificHeaderOptions;
                for (i = 0; i < entityHandlerRequest->numRcvdVendorSpecificHeaderOptions; i++)
                {
                    if (((OCHeaderOption) rcvdOptions[i]).protocolID == OC_COAP_ID)
                    {
                        OIC_LOG_V(INFO, LISTENER_TAG, "Received option with OC_COAP_ID and ID %u with",
                                ((OCHeaderOption)rcvdOptions[i]).optionID );

                        OIC_LOG_BUFFER(INFO, LISTENER_TAG, ((OCHeaderOption)rcvdOptions[i]).optionData,
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
        OIC_LOG(INFO, LISTENER_TAG, "Flag includes OC_OBSERVE_FLAG");

        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_OBSERVE_REGISTER from client");

            printf("NS_ register message observerID : %d\n", entityHandlerRequest->obsInfo.obsId);
            NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_RECV_SUBSCRIPTION, entityHandlerRequest);

        }
    }

    OCPayloadDestroy(response.payload);
    return ehResult;
}

OCEntityHandlerResult NSEntityHandlerSyncCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback)
{
    OIC_LOG_V (INFO, LISTENER_TAG, "Inside entity handler - flags: 0x%x", flag);

    OCEntityHandlerResult ehResult = OC_EH_OK;
    OCEntityHandlerResponse response =
    { 0, 0, OC_EH_ERROR, 0, 0,
    { },
    { 0 }, false };

    (void)callback;

    // Validate pointer
    if (!entityHandlerRequest)
    {
        OIC_LOG (ERROR, LISTENER_TAG, "Invalid request pointer");
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
        OIC_LOG (INFO, LISTENER_TAG, "Flag includes OC_REQUEST_FLAG");

        if (OC_REST_GET == entityHandlerRequest->method)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_REST_GET from client");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_REST_PUT from client");
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_REST_POST from client");

            NSPushQueue(NOTIFICATION_SCHEDULER, TASK_SEND_READ, NSBuildOICNotificationSync(entityHandlerRequest->payload));
            ehResult = OC_EH_OK;
        }
        else if (OC_REST_DELETE == entityHandlerRequest->method)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_REST_DELETE from client");
            ehResult = OC_EH_OK;
        }
        else
        {
            OIC_LOG_V (INFO, LISTENER_TAG, "Received unsupported method %d from client",
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
                OIC_LOG (INFO, LISTENER_TAG, "Received vendor specific options");
                uint8_t i = 0;
                OCHeaderOption * rcvdOptions = entityHandlerRequest->rcvdVendorSpecificHeaderOptions;
                for (i = 0; i < entityHandlerRequest->numRcvdVendorSpecificHeaderOptions; i++)
                {
                    if (((OCHeaderOption) rcvdOptions[i]).protocolID == OC_COAP_ID)
                    {
                        OIC_LOG_V(INFO, LISTENER_TAG, "Received option with OC_COAP_ID and ID %u with",
                                ((OCHeaderOption)rcvdOptions[i]).optionID );

                        OIC_LOG_BUFFER(INFO, LISTENER_TAG, ((OCHeaderOption)rcvdOptions[i]).optionData,
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
        OIC_LOG(INFO, LISTENER_TAG, "Flag includes OC_OBSERVE_FLAG");

        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
            OIC_LOG (INFO, LISTENER_TAG, "Received OC_OBSERVE_REGISTER from client");
            printf("NS_ register sync observerID : %d\n", entityHandlerRequest->obsInfo.obsId);
            NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_SYNC_SUBSCRIPTION, entityHandlerRequest);
        }
    }

    OCPayloadDestroy(response.payload);
    return ehResult;
}

void NSProviderConnectionStateListener(CATransportAdapter_t adapter, const char *remote_address,
        bool connected)
{

    // should be implementation
    (void)adapter;
    (void)remote_address;

    OIC_LOG (INFO, LISTENER_TAG, "Connection State Changed");

    if (connected)
    {
        OIC_LOG (INFO, LISTENER_TAG, "CONNECTED");

        // Set Connection State
        NSSetProviderConnectionState(CONNECTED);

        // Start Presence
        NSPushQueue(DISCOVERY_SCHEDULER, TASK_START_PRESENCE, NULL);
    }
}

void NSProviderAdapterStateListener(CATransportAdapter_t adapter, bool enabled)
{
    // should be implementation
    (void)adapter;

    OIC_LOG (INFO, LISTENER_TAG, "Adapter State Changed");

    if (enabled)
    {
        OIC_LOG (INFO, LISTENER_TAG, "CONNECTED");

        // Set Connection State
        NSSetProviderConnectionState(CONNECTED);

        // Start Presence
        NSPushQueue(DISCOVERY_SCHEDULER, TASK_START_PRESENCE, NULL);
    }
}

NSSync * NSBuildOICNotificationSync(OCPayload * payload)
{
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
        OIC_LOG(DEBUG, LISTENER_TAG, "id of received sync is null");
        OICFree(retSync);
        return NULL;
    }
    int64_t state;
    if (!OCRepPayloadGetPropInt(repPayload, NS_ATTRIBUTE_STATE, & state))
    {

        OIC_LOG(DEBUG, LISTENER_TAG, "id of received sync is null");

        OICFree(retSync->mMessageId);
        OICFree(retSync);
        return NULL;
    }

    retSync->mState = (NSSyncTypes) state;

    OIC_LOG_V(DEBUG, LISTENER_TAG, "Sync ID : %s", retSync->mMessageId);
    OIC_LOG_V(DEBUG, LISTENER_TAG, "Sync State : %d", (int) retSync->mState);

    return retSync;
}

NSResult NSMakeTask(NSTaskType type, OCEntityHandlerRequest *request, NSTask * task)
{
    task = (NSTask*) OICMalloc(sizeof(NSTask));
    if (!task)
    {
        OIC_LOG(ERROR, LISTENER_TAG, PCF("Fail to allocate memory"));
        return NS_ERROR;
    }

    task->taskType = type;
    task->taskData = request;
    task->nextTask = NULL;

    return NS_OK;
}

