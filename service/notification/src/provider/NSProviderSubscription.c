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

#include "NSProviderSubscription.h"

NSResult NSInitSubscriptionList()
{
    NS_LOG(DEBUG, "NSInitSubscriptionList - IN");

    consumerSubList = NSCacheCreate();
    consumerSubList->cacheType = NS_PROVIDER_CACHE_SUBSCRIBER;

    NS_LOG(DEBUG, "NSInitSubscriptionList - OUT");
    return NS_OK;
}

NSResult NSSetSubscriptionAcceptPolicy(NSAccessPolicy policy)
{
    NS_LOG(DEBUG, "NSSetSubscriptionAcceptPolicy - IN");

    if (policy == NS_ACCEPTER_PROVIDER)
    {
        NS_LOG(DEBUG, "Place Provider as a subscription accepter");
    }
    else if (policy == NS_ACCEPTER_CONSUMER)
    {
        NS_LOG(DEBUG, "Place Consumer as a subscription accepter");
    }

    NSSubscriptionAccepter = policy;

    NS_LOG(DEBUG, "NSSetSubscriptionAcceptPolicy - OUT");
    return NS_OK;
}

int NSGetSubscriptionAccepter()
{
    return NSSubscriptionAccepter;
}

NSResult NSSendAccessPolicyResponse(OCEntityHandlerRequest *entityHandlerRequest)
{
    NS_LOG(DEBUG, "NSSendAccessPolicyResponse - IN");

    // put notification resource
    OCResourceHandle notificationResourceHandle;
    if (NSPutNotificationResource(NSGetSubscriptionAccepter(), &notificationResourceHandle)
            != NS_OK)
    {
        NS_LOG(ERROR, "Fail to put notification resource");
        return NS_ERROR;
    }

    // make response for the Get Request
    OCEntityHandlerResponse response;
    response.numSendVendorSpecificHeaderOptions = 0;
    memset(response.sendVendorSpecificHeaderOptions, 0,
            sizeof response.sendVendorSpecificHeaderOptions);
    memset(response.resourceUri, 0, sizeof response.resourceUri);

    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        NS_LOG(ERROR, "payload is NULL");
        return NS_ERROR;
    }

    OCRepPayloadSetUri(payload, NSGetNotificationUri());
    OCRepPayloadSetPropInt(payload, NS_ATTRIBUTE_POLICY, NSGetSubscriptionAccepter());
    OCRepPayloadSetPropString(payload, NS_ATTRIBUTE_MESSAGE, NSGetNotificationMessageUri());
    OCRepPayloadSetPropString(payload, NS_ATTRIBUTE_SYNC, NSGetNotificationSyncUri());

    response.requestHandle = entityHandlerRequest->requestHandle;
    response.resourceHandle = entityHandlerRequest->resource;
    response.persistentBufferFlag = 0;
    response.ehResult = OC_EH_OK;
    response.payload = (OCPayload *) payload;

    // Send Response
    if (OCDoResponse(&response) != OC_STACK_OK)
    {
        NS_LOG(ERROR, "Fail to AccessPolicy send response");
        return NS_ERROR;
    }
    OCRepPayloadDestroy(payload);
    NSFreeOCEntityHandlerRequest(entityHandlerRequest);

    NS_LOG(DEBUG, "NSSendAccessPolicyResponse - OUT");
    return NS_OK;
}

void NSHandleSubscription(OCEntityHandlerRequest *entityHandlerRequest, NSResourceType resourceType)
{
    NS_LOG(DEBUG, "NSHandleSubscription - IN");

    if (resourceType == NS_RESOURCE_MESSAGE)
    {
        NS_LOG(DEBUG, "resourceType == NS_RESOURCE_MESSAGE");
        NSCacheElement * element = (NSCacheElement *) OICMalloc(sizeof(NSCacheElement));
        NSCacheSubData * subData = (NSCacheSubData *) OICMalloc(sizeof(NSCacheSubData));

        subData->id = OICStrdup(entityHandlerRequest->devAddr.addr);
        subData->isWhite = false;
        subData->messageObId = entityHandlerRequest->obsInfo.obsId;
        subData->syncObId = 0;

        element->data = (void*) subData;
        element->next = NULL;

        NS_LOG_V(DEBUG, "SubList IP[ID] = [%s]", subData->id);
        NS_LOG_V(DEBUG, "SubList message observation ID = [%d]", subData->messageObId);

        if (NSCacheWrite(consumerSubList, element) != NS_OK)
        {
            NS_LOG(DEBUG, "fail to write cache");
        }

        if (NSGetSubscriptionAccepter() == NS_ACCEPTER_PROVIDER)
        {
            NS_LOG(DEBUG, "NSGetSubscriptionAccepter == NS_ACCEPTER_PROVIDER");
            NSAskAcceptanceToUser(entityHandlerRequest);
        }
        else if (NSGetSubscriptionAccepter() == NS_ACCEPTER_CONSUMER)
        {
            NS_LOG(DEBUG, "NSGetSubscriptionAccepter == NS_ACCEPTER_CONSUMER");
            NSSendSubscriptionResponse(entityHandlerRequest, true);
        }
    }
    else if (resourceType == NS_RESOURCE_SYNC)
    {
        NS_LOG(DEBUG, "resourceType == NS_RESOURCE_SYNC");
        NSCacheElement * element = (NSCacheElement *) OICMalloc(sizeof(NSCacheElement));

        NSCacheSubData * subData = (NSCacheSubData *) OICMalloc(sizeof(NSCacheSubData));

        subData->id = OICStrdup(entityHandlerRequest->devAddr.addr);
        subData->isWhite = false;
        subData->syncObId = entityHandlerRequest->obsInfo.obsId;
        subData->messageObId = 0;

        element->data = (void*) subData;
        element->next = NULL;

        NS_LOG_V(DEBUG, "SubList IP[ID] = [%s]", subData->id);
        NS_LOG_V(DEBUG, "SubList sync observation ID = [%d]", subData->syncObId);

        if (NSCacheWrite(consumerSubList, element) != NS_OK)
        {
            NS_LOG(ERROR, "Fail to write cache");
        }
    }

    NS_LOG(DEBUG, "NSHandleSubscription - OUT");
}

void NSHandleUnsubscription(OCEntityHandlerRequest *entityHandlerRequest)
{
    NS_LOG(DEBUG, "NSHandleUnsubscription - IN");

    NSCacheElement * element = (NSCacheElement *) OICMalloc(sizeof(NSCacheElement));
    NSCacheSubData * subData = (NSCacheSubData *) OICMalloc(sizeof(NSCacheSubData));
    subData->id = OICStrdup(entityHandlerRequest->devAddr.addr);
    subData->isWhite = false;
    subData->messageObId = entityHandlerRequest->obsInfo.obsId;

    element->data = (void*) subData;
    element->next = NULL;

    NS_LOG_V(DEBUG, "SubList IP[ID] = [%s]", subData->id);
    NS_LOG_V(DEBUG, "SubList observation ID = [%d]", subData->syncObId);

    if (NSCacheWrite(consumerSubList, element) != NS_OK)
    {
        NS_LOG(ERROR, "fail to write consumer white list");
    }

    NS_LOG(DEBUG, "NSHandleUnsubscription - IN");
    NSFreeOCEntityHandlerRequest(entityHandlerRequest);
}

void NSAskAcceptanceToUser(OCEntityHandlerRequest *entityHandlerRequest)
{
    NS_LOG(DEBUG, "NSAskAcceptanceToUser - IN");

    NSPushQueue(RESPONSE_SCHEDULER, TASK_CB_SUBSCRIPTION, entityHandlerRequest);

    NS_LOG(DEBUG, "NSAskAcceptanceToUser - OUT");
}

NSResult NSSendResponse(const char * id, bool accepted)
{
    NS_LOG(DEBUG, "NSSendResponse - IN");

    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        NS_LOG(ERROR, "fail to create playload");
        return NS_ERROR;
    }

    OCResourceHandle rHandle;
    if (NSPutMessageResource(NULL, &rHandle) != NS_OK)
    {
        NS_LOG(ERROR, "Fail to put notification resource");
        return NS_ERROR;
    }

    OCRepPayloadSetUri(payload, NSGetNotificationMessageUri());
    OCRepPayloadSetPropString(payload, NS_ATTRIBUTE_ID, "0000-0000-0000-0000");
    OCRepPayloadSetPropBool(payload, NS_ATTRIBUTE_ACCPETANCE, accepted);

    NSCacheElement * element = NSCacheRead(consumerSubList, id);

    if(element == NULL)
    {
        NS_LOG(ERROR, "element is NULL");
        return NS_ERROR;
    }
    NSCacheSubData * subData = (NSCacheSubData*) element->data;

    if (OCNotifyListOfObservers(rHandle, (OCObservationId*)&subData->messageObId, 1, payload, OC_HIGH_QOS)
            != OC_STACK_OK)
    {
        NS_LOG(ERROR, "fail to send Acceptance");
        OCRepPayloadDestroy(payload);
        return NS_ERROR;

    }
    OCRepPayloadDestroy(payload);

    NS_LOG(DEBUG, "NSSendResponse - OUT");
    return NS_OK;
}

NSResult NSSendSubscriptionResponse(OCEntityHandlerRequest *entityHandlerRequest, bool accepted)
{
    NS_LOG(DEBUG, "NSSendSubscriptionResponse - IN");

    if (!entityHandlerRequest)
    {
        NS_LOG(ERROR, "Invalid request pointer");
        return OC_EH_ERROR;
    }

    if (accepted)
    {
        NS_LOG(DEBUG, "accepted is true");
        NSCacheElement * element = (NSCacheElement *) OICMalloc(sizeof(NSCacheElement));

        NSCacheSubData * subData = (NSCacheSubData *) OICMalloc(sizeof(NSCacheSubData));
        subData->id = OICStrdup(entityHandlerRequest->devAddr.addr);
        subData->isWhite = true;
        subData->messageObId = entityHandlerRequest->obsInfo.obsId;

        element->data = (void*) subData;
        element->next = NULL;

        if (NSCacheWrite(consumerSubList, element) != NS_OK)
        {
            NS_LOG(ERROR, "fail to write consumer white list");
        }
    }

    NSSendResponse(entityHandlerRequest->devAddr.addr, accepted);
    NSFreeOCEntityHandlerRequest(entityHandlerRequest);

    NS_LOG(DEBUG, "NSSendSubscriptionResponse - OUT");
    return NS_OK;
}

void * NSSubScriptionSchedule(void *ptr)
{
    if (ptr == NULL)
    {
        NS_LOG(DEBUG, "Create NSSubScriptionSchedule");
    }

    while (NSIsRunning[SUBSCRIPTION_SCHEDULER])
    {
        sem_wait(&NSSemaphore[SUBSCRIPTION_SCHEDULER]);
        pthread_mutex_lock(&NSMutex[SUBSCRIPTION_SCHEDULER]);

        if (NSHeadMsg[SUBSCRIPTION_SCHEDULER] != NULL)
        {
            NSTask *node = NSHeadMsg[SUBSCRIPTION_SCHEDULER];
            NSHeadMsg[SUBSCRIPTION_SCHEDULER] = node->nextTask;

            switch (node->taskType)
            {
                case TASK_SEND_POLICY:
                    NS_LOG(DEBUG, "CASE TASK_SEND_POLICY : ");
                    NSSendAccessPolicyResponse((OCEntityHandlerRequest*) node->taskData);
                    break;

                case TASK_RECV_SUBSCRIPTION:
                    NS_LOG(DEBUG, "CASE TASK_RECV_SUBSCRIPTION : ");
                    NSHandleSubscription((OCEntityHandlerRequest*) node->taskData,
                            NS_RESOURCE_MESSAGE);
                    break;

                case TASK_RECV_UNSUBSCRIPTION:
                    NS_LOG(DEBUG, "CASE TASK_RECV_UNSUBSCRIPTION : ");
                    NSHandleUnsubscription((OCEntityHandlerRequest*) node->taskData);
                    break;

                case TASK_SEND_ALLOW:
                {
                    NS_LOG(DEBUG, "CASE TASK_SEND_ALLOW : ");
                    NSConsumer * consumer = (NSConsumer *) node->taskData;

                    char * id = OICStrdup(consumer->mDeviceId);

                    NSCacheUpdateSubScriptionState(consumerSubList, id, true);
                    NSSendResponse(id, true);
                    OICFree(id);
                    NSFreeConsumer(consumer);
                    break;
                }
                case TASK_SEND_DENY:
                {
                    NS_LOG(DEBUG, "CASE TASK_SEND_DENY : ");
                    NSConsumer * consumer = (NSConsumer *) node->taskData;

                    char * id = OICStrdup(consumer->mDeviceId);

                    NSCacheUpdateSubScriptionState(consumerSubList, id, false);
                    NSSendResponse(id, false);
                    OICFree(id);
                    NSFreeConsumer(consumer);

                    break;
                }
                case TASK_SYNC_SUBSCRIPTION:
                    NS_LOG(DEBUG, "CASE TASK_SYNC_SUBSCRIPTION : ");
                    NSHandleSubscription((OCEntityHandlerRequest*) node->taskData,
                            NS_RESOURCE_SYNC);
                    break;
                default:
                    break;

            }
            OICFree(node);
        }

        pthread_mutex_unlock(&NSMutex[SUBSCRIPTION_SCHEDULER]);

    }
    NS_LOG(INFO, "Destroy NSSubScriptionSchedule");
    return NULL;
}
