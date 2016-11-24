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

bool NSProviderIsSyncAttributes(OCRepPayload * payload);
bool NSProviderIsTopicAttributes(OCRepPayload * payload);
OCStackResult NSProviderSendResponse(OCEntityHandlerRequest * entityHandlerRequest,
        OCRepPayload * payload, char * reqInterface, OCEntityHandlerResult ehResult,
        NSInterfaceType interfaceType, NSResourceType resourceType);

OCEntityHandlerResult NSEntityHandlerNotificationCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback)
{
    NS_LOG(DEBUG, "NSEntityHandlerNotificationCb - IN");

    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    (void)callback;

    if (!entityHandlerRequest)
    {
        NS_LOG(ERROR, "Invalid request pointer");
        return ehResult;
    }

    if (flag & OC_REQUEST_FLAG)
    {
        NS_LOG(DEBUG, "Flag includes OC_REQUEST_FLAG");

        if (OC_REST_GET == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerNotificationCb - OC_REST_GET");

            char * copyQuery = OICStrdup(entityHandlerRequest->query);
            char * reqInterface = NSGetValueFromQuery(copyQuery, NS_QUERY_INTERFACE);

            if (reqInterface && strcmp(reqInterface, NS_INTERFACE_BASELINE) != 0
                    && strcmp(reqInterface, NS_INTERFACE_READ) != 0)
            {
                NS_LOG(ERROR, "Invalid interface");
                OICFree(copyQuery);
                return ehResult;
            }

            OICFree(copyQuery);
            NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_SEND_POLICY,
                    NSCopyOCEntityHandlerRequest(entityHandlerRequest));

            ehResult = OC_EH_OK;
        }
        else
        {
            NS_LOG_V (DEBUG, "Received method %d from client", entityHandlerRequest->method);
        }
    }

    NS_LOG(DEBUG, "NSEntityHandlerNotificationCb - OUT");
    return ehResult;
}

OCEntityHandlerResult NSEntityHandlerMessageCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback)
{
    NS_LOG(DEBUG, "NSEntityHandlerMessageCb - IN");

    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    char * reqInterface = NULL;
    OCRepPayload * payload = NULL;

    (void)callback;

    if (!entityHandlerRequest)
    {
        NS_LOG (ERROR,"Invalid request pointer");
        return ehResult;
    }

    if (flag & OC_REQUEST_FLAG)
    {
        NS_LOG(DEBUG, "Flag includes OC_REQUEST_FLAG");
        NS_LOG_V(DEBUG, "Received method %d from client", entityHandlerRequest->method);

        if (OC_REST_GET == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerMessageCb - OC_REST_GET");

            char * copyQuery = OICStrdup(entityHandlerRequest->query);
            reqInterface = OICStrdup(NSGetValueFromQuery(copyQuery, NS_QUERY_INTERFACE));
            OICFree(copyQuery);

            if (reqInterface && strcmp(reqInterface, NS_INTERFACE_BASELINE) != 0
                    && strcmp(reqInterface, NS_INTERFACE_READ) != 0)
            {
                NS_LOG(ERROR, "Invalid interface");
                return ehResult;
            }
            ehResult = OC_EH_OK;
        }
        else
        {
            NS_LOG_V (DEBUG, "Received method %d from client", entityHandlerRequest->method);
        }
    }

    if (flag & OC_OBSERVE_FLAG)
    {
        NS_LOG(DEBUG, "NSEntityHandlerMessageCb - OC_OBSERVE_FLAG");

        OCObserveAction ocObAction = entityHandlerRequest->obsInfo.action;

        if (ocObAction == OC_OBSERVE_REGISTER)
        {
            NS_LOG(DEBUG, "NSEntityHandlerMessageCb - OC_OBSERVE_REGISTER");
            NS_LOG_V(DEBUG, "NSEntityHandlerMessageCb\n"
                    "Register message observerID : %d\n", entityHandlerRequest->obsInfo.obsId);

            NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_RECV_SUBSCRIPTION,
                    NSCopyOCEntityHandlerRequest(entityHandlerRequest));
            ehResult = OC_EH_OK;
        }
        else if(ocObAction == OC_OBSERVE_DEREGISTER)
        {
            NS_LOG(DEBUG, "NSEntityHandlerMessageCb - OC_OBSERVE_DEREGISTER");
            NS_LOG_V(DEBUG, "NSEntityHandlerMessageCb\n - "
                    "Deregister Message observerID : %d\n", entityHandlerRequest->obsInfo.obsId);
            NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_RECV_UNSUBSCRIPTION,
                    NSCopyOCEntityHandlerRequest(entityHandlerRequest));
            ehResult = OC_EH_OK;
        }
    }

    ehResult = NSProviderSendResponse(entityHandlerRequest, payload, reqInterface, ehResult,
            NS_INTERFACE_TYPE_READ, NS_RESOURCE_MESSAGE);
    OICFree(reqInterface);
    NS_LOG(DEBUG, "NSEntityHandlerMessageCb - OUT");
    return ehResult;
}

OCEntityHandlerResult NSEntityHandlerSyncCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback)
{
    NS_LOG(DEBUG, "NSEntityHandlerSyncCb - IN");
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    char * reqInterface = NULL;
    OCRepPayload * payload = NULL;

    (void)callback;

    if (!entityHandlerRequest)
    {
        NS_LOG(ERROR, "Invalid request pointer");
        return ehResult;
    }

    if (flag & OC_REQUEST_FLAG)
    {
        NS_LOG(DEBUG, "Flag includes OC_REQUEST_FLAG");

        if (OC_REST_GET == entityHandlerRequest->method)
        {

            char * copyQuery = OICStrdup(entityHandlerRequest->query);
            reqInterface = OICStrdup(NSGetValueFromQuery(copyQuery, NS_QUERY_INTERFACE));
            OICFree(copyQuery);

            if (reqInterface && strcmp(reqInterface, NS_INTERFACE_BASELINE) != 0
                    && strcmp(reqInterface, NS_INTERFACE_READWRITE) != 0)
            {
                NS_LOG(ERROR, "Invalid interface");
                return ehResult;
            }

            ehResult = OC_EH_OK;
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            /** Receive sync data from consumer which read or dismiss notification message.
                And broadcast the sync data to all subscribers including provider app
                to synchronize the notification message status. */

            NS_LOG(DEBUG, "NSEntityHandlerSyncCb - OC_REST_POST");

            if(NSProviderIsSyncAttributes((OCRepPayload *)entityHandlerRequest->payload))
            {
                NSPushQueue(NOTIFICATION_SCHEDULER, TASK_RECV_READ,
                                    NSGetSyncInfo(entityHandlerRequest->payload));
                ehResult = OC_EH_OK;
            }
        }
        else
        {
            NS_LOG_V(DEBUG, "Received method %d from client", entityHandlerRequest->method);
        }
    }

    if (flag & OC_OBSERVE_FLAG)
    {
        /** Requested by consumers to synchronize notification message status.
            Store the observer IDs to storage or cache */

        NS_LOG(DEBUG, "NSEntityHandlerSyncCb - OC_OBSERVE_FLAG");

        OCObserveAction ocObAction = entityHandlerRequest->obsInfo.action;

        if (ocObAction == OC_OBSERVE_REGISTER)
        {
            NS_LOG(DEBUG, "NSEntityHandlerSyncCb - OC_OBSERVE_REGISTER");
            NS_LOG_V(DEBUG, "NSEntityHandlerSyncCb\n - "
                    "Register Sync observerID : %d\n", entityHandlerRequest->obsInfo.obsId);
            NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_SYNC_SUBSCRIPTION,
                    NSCopyOCEntityHandlerRequest(entityHandlerRequest));
        }
        else if(ocObAction == OC_OBSERVE_DEREGISTER)
        {
            NS_LOG(DEBUG, "NSEntityHandlerSyncCb - OC_OBSERVE_DEREGISTER");
            NS_LOG_V(DEBUG, "NSEntityHandlerSyncCb\n - "
                    "Deregister Sync observerID : %d\n", entityHandlerRequest->obsInfo.obsId);
            NSPushQueue(SUBSCRIPTION_SCHEDULER, TASK_RECV_UNSUBSCRIPTION,
                    NSCopyOCEntityHandlerRequest(entityHandlerRequest));
        }
    }

    NS_LOG(DEBUG, "NSEntityHandlerSyncCb - OUT");

    ehResult = NSProviderSendResponse(entityHandlerRequest, payload, reqInterface, ehResult,
            NS_INTERFACE_TYPE_READ, NS_RESOURCE_MESSAGE);
    OICFree(reqInterface);

    return ehResult;
}

OCEntityHandlerResult NSEntityHandlerTopicCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback)
{
    NS_LOG(DEBUG, "NSEntityHandlerTopicCb - IN");
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    char * reqInterface = NULL;
    OCRepPayload * payload = NULL;

    (void)callback;

    if (!entityHandlerRequest)
    {
        NS_LOG(ERROR, "Invalid request pointer");
        return ehResult;
    }

    if (flag & OC_REQUEST_FLAG)
    {
        NS_LOG(DEBUG, "Flag includes OC_REQUEST_FLAG");

        if (OC_REST_GET == entityHandlerRequest->method)
        {
            NS_LOG(DEBUG, "NSEntityHandlerTopicCb - OC_REST_GET");

            char * copyReq = OICStrdup(entityHandlerRequest->query);
            reqInterface = OICStrdup(NSGetValueFromQuery(copyReq, NS_QUERY_INTERFACE));
            OICFree(copyReq);

            if (reqInterface && strcmp(reqInterface, NS_INTERFACE_BASELINE) != 0
                    && strcmp(reqInterface, NS_INTERFACE_READWRITE) != 0)
            {
                NS_LOG(ERROR, "Invalid interface");
                return ehResult;
            }
            // send consumer's interesting topic list if consumer id exists
            // otherwise send  registered topic list
            NSPushQueue(TOPIC_SCHEDULER, TASK_SEND_TOPICS,
                    NSCopyOCEntityHandlerRequest(entityHandlerRequest));

            ehResult = OC_EH_OK;
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            // Receive interesting topic list from consumers
            // Send topic notice message(id = TOPIC) to the consumer 
            // which requests to post.
            NS_LOG(DEBUG, "NSEntityHandlerTopicCb - OC_REST_POST");
            // Accepter is provider. our service is not support sendtopiclist from OC_REST_POST
            // Accepter is consumer. our service is support sendtopiclist from OC_REST_POST
            if(NSGetPolicy() == false &&
                    NSProviderIsTopicAttributes(OCRepPayloadClone((OCRepPayload *)
                            entityHandlerRequest->payload)))
            {
                NSPushQueue(TOPIC_SCHEDULER, TASK_POST_TOPIC,
                        NSCopyOCEntityHandlerRequest(entityHandlerRequest));
                ehResult = OC_EH_OK;
            }
        }
        else
        {
            NS_LOG_V(DEBUG, "Received unsupported method %d from client",
                    entityHandlerRequest->method);
            ehResult = OC_EH_ERROR;
        }
    }

    NS_LOG(DEBUG, "NSEntityHandlerTopicCb - OUT");
    ehResult = NSProviderSendResponse(entityHandlerRequest, payload, reqInterface, ehResult,
            NS_INTERFACE_TYPE_READWRITE, NS_RESOURCE_TOPIC);
    OICFree(reqInterface);
    return ehResult;
}

void NSProviderConnectionStateListener(const CAEndpoint_t * info, bool connected)
{
    NS_LOG(DEBUG, "NSProviderConnectionStateListener - IN");

    if (connected)
    {
        NS_LOG(DEBUG, "CONNECTED");

        // Set Connection State
        NSSetProviderConnectionState(CONNECTED);

        // Start Presence
        NSPushQueue(DISCOVERY_SCHEDULER, TASK_START_PRESENCE, NULL);

        if(info->adapter == CA_ADAPTER_TCP)
        {
            NS_LOG_V(DEBUG, "TCP Connected remote address: %s:%d", info->addr, info->port);
        }
    }
    else
    {
        NS_LOG(DEBUG, "DISCONNECTED");

        // Set Connection State
        NSSetProviderConnectionState(DISCONNECTED);

        if(info->adapter == CA_ADAPTER_TCP)
        {
            NS_LOG_V(DEBUG, "TCP Disconnected remote address: %s:%d", info->addr, info->port);
        }
    }

    NS_LOG(DEBUG, "NSProviderConnectionStateListener - OUT");
}

void NSProviderAdapterStateListener(CATransportAdapter_t adapter, bool enabled)
{
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
    else
    {

        NS_LOG(DEBUG, "DISCONNECTED");

        // Set Connection State
        NSSetProviderConnectionState(DISCONNECTED);
    }

    NS_LOG(DEBUG, "NSProviderAdapterStateListener - OUT");
}

bool NSProviderCompareSyncAttributes(const char * name)
{
    if (!strcmp(name, NS_ATTRIBUTE_MESSAGE_ID) ||
        !strcmp(name, NS_ATTRIBUTE_PROVIDER_ID) ||
        !strcmp(name, NS_ATTRIBUTE_STATE))
    {
        return true;
    }

    return false;
}

bool NSProviderIsSyncAttributes(OCRepPayload * payload)
{
    NS_LOG(DEBUG, "get extra info");

    OCRepPayloadValue * curr = payload->values;
    while(curr)
    {
        if (!NSProviderCompareSyncAttributes(curr->name))
        {
            return false;
        }
        curr = curr->next;
    }

    return true;
}

bool NSProviderCompareTopicAttributes(const char * name)
{
    if (!strcmp(name, NS_ATTRIBUTE_TOPIC_LIST) ||
        !strcmp(name, NS_ATTRIBUTE_CONSUMER_ID))
    {
        return true;
    }

    return false;
}

bool NSProviderCompareSubTopicAttributes(const char * name)
{
    if (!strcmp(name, NS_ATTRIBUTE_TOPIC_NAME) ||
        !strcmp(name, NS_ATTRIBUTE_TOPIC_SELECTION))
    {
        return true;
    }

    return false;
}

bool NSProviderIsTopicAttributes(OCRepPayload * payload)
{
    NS_LOG(DEBUG, "get extra info");

    OCRepPayloadValue * curr = payload->values;
    while(curr)
    {
        if (!NSProviderCompareTopicAttributes(curr->name))
        {
            return false;
        }

        if (!strcmp(curr->name, NS_ATTRIBUTE_TOPIC_LIST))
        {
            OCRepPayload ** topicListPayload = NULL;
            OCRepPayloadValue * payloadValue = NULL;
            payloadValue = NSPayloadFindValue(payload, NS_ATTRIBUTE_TOPIC_LIST);
            size_t dimensionSize = calcDimTotal(payloadValue->arr.dimensions);
            size_t dimensions[3] = { dimensionSize, 0, 0 };

            if (!dimensionSize)
            {
                return false;
            }

            OCRepPayloadGetPropObjectArray(payload, NS_ATTRIBUTE_TOPIC_LIST, &topicListPayload,
                    dimensions);

            for (int i = 0; i < (int) dimensionSize; i++)
            {
                OCRepPayloadValue * subCurr = topicListPayload[i]->values;
                while(subCurr)
                {
                    if (!NSProviderCompareSubTopicAttributes(subCurr->name))
                    {
                        for(int j = i; j < (int) dimensionSize; ++j)
                        {
                            OCRepPayloadDestroy(topicListPayload[i]);
                        }

                        OCRepPayloadDestroy(payload);
                        return false;
                    }
                    subCurr = subCurr->next;
                }
                OCRepPayloadDestroy(topicListPayload[i]);
            }
        }
        curr = curr->next;
    }

    OCRepPayloadDestroy(payload);
    return true;
}

OCStackResult NSProviderSendResponse(OCEntityHandlerRequest * entityHandlerRequest,
        OCRepPayload * payload, char * reqInterface, OCEntityHandlerResult ehResult,
        NSInterfaceType interfaceType, NSResourceType resourceType)
{
    if (reqInterface && strcmp(reqInterface, NS_INTERFACE_BASELINE) == 0)
    {
        payload = OCRepPayloadCreate();

        if (!payload)
        {
            NS_LOG(ERROR, "payload is NULL");
            return ehResult;
        }

        OCResourcePayloadAddStringLL(&payload->interfaces, NS_INTERFACE_BASELINE);
        interfaceType == NS_INTERFACE_TYPE_READ ?
                OCResourcePayloadAddStringLL(&payload->interfaces, NS_INTERFACE_READ)
                : OCResourcePayloadAddStringLL(&payload->interfaces, NS_INTERFACE_READWRITE);

        char * rtStr = NULL;
        switch (resourceType)
        {
            case NS_RESOURCE_MESSAGE:
                rtStr = NS_COLLECTION_MESSAGE_TYPE;
                break;
            case NS_RESOURCE_SYNC:
                rtStr = NS_COLLECTION_SYNC_TYPE;
                break;
            case NS_RESOURCE_TOPIC:
                rtStr = NS_COLLECTION_TOPIC_TYPE;
                break;
            default:
                NS_LOG(ERROR, "sendResponseError");
                return ehResult;
        }

        OCResourcePayloadAddStringLL(&payload->types, rtStr);
    }

    if (resourceType == NS_RESOURCE_TOPIC && entityHandlerRequest->method == OC_REST_GET)
    {
        OCRepPayloadDestroy(payload);
        return ehResult;
    }

    OCEntityHandlerResponse response;
    response.numSendVendorSpecificHeaderOptions = 0;
    memset(response.sendVendorSpecificHeaderOptions, 0,
            sizeof response.sendVendorSpecificHeaderOptions);
    memset(response.resourceUri, 0, sizeof response.resourceUri);

    response.requestHandle = entityHandlerRequest->requestHandle;
    response.resourceHandle = entityHandlerRequest->resource;
    response.persistentBufferFlag = 0;
    response.ehResult = ehResult;
    response.payload = (OCPayload *) payload;

    if (OCDoResponse(&response) != OC_STACK_OK)
    {
        NS_LOG(ERROR, "Fail to AccessPolicy send response");
        ehResult = OC_STACK_ERROR;
    }

    OCRepPayloadDestroy(payload);
    return ehResult;
}
