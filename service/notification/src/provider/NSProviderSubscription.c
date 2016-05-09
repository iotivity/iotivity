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

NSResult NSSetSubscriptionAcceptPolicy(NSAccessPolicy policy)
{
    if (policy == NS_ACCEPTER_PROVIDER)
    {
        OIC_LOG(INFO, SUBSCRIPTION_TAG, "Place Provider as a subscription accepter");
    }
    else if (policy == NS_ACCEPTER_CONSUMER)
    {
        OIC_LOG(INFO, SUBSCRIPTION_TAG, "Place Consumer as a subscription accepter");
    }

    NSSubscriptionAccepter = policy;

    return NS_OK;
}

int NSGetSubscriptionAccepter()
{
    return NSSubscriptionAccepter;
}

NSResult NSSendAccessPolicyResponse(OCEntityHandlerRequest *entityHandlerRequest)
{
    OIC_LOG(DEBUG, NOTIFICATION_TAG, "Send Notification Policy to consumer");

    // put notification resource
    OCResourceHandle notificationResourceHandle;
    if(NSPutNotificationResource(NSGetSubscriptionAccepter(), &notificationResourceHandle) != NS_OK)
    {
        OIC_LOG(ERROR, SUBSCRIPTION_TAG, PCF("Failed to put notification resource"));
        return NS_ERROR;
    }

    // make response for the Get Request
    OCEntityHandlerResponse response;
    response.numSendVendorSpecificHeaderOptions = 0;
    memset(response.sendVendorSpecificHeaderOptions, 0,
            sizeof response.sendVendorSpecificHeaderOptions);
    memset(response.resourceUri, 0, sizeof response.resourceUri);

    OCRepPayload* payload = OCRepPayloadCreate();
    //response.payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, SUBSCRIPTION_TAG, PCF("Failed to allocate Payload"));
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
    response.payload = (OCPayload *)payload;


    // Send Response
    if (OCDoResponse(&response) != OC_STACK_OK)
    {
        OIC_LOG(ERROR, SUBSCRIPTION_TAG, PCF("Fail to send response"));
        return NS_ERROR;
    }
    OCRepPayloadDestroy(payload);

    return NS_OK;
}

void NSHandleSubscription(OCEntityHandlerRequest *entityHandlerRequest, int state)
{

    OIC_LOG(INFO, SUBSCRIPTION_TAG, "Start to subscription process");

    printf("state = 0 \n");

    // write consumer info to cache
    if(state == 0)
    {

        printf("state = 0 - 1\n");

        if (NSProviderWriteToCache(NS_CONSUMER_BLACKLIST, entityHandlerRequest->obsInfo.obsId, state) != NS_OK)
        {
            OIC_LOG(ERROR, SUBSCRIPTION_TAG, PCF("fail to write consumer black list"));
        }
        printf("state = 0 - 2\n");
        // check access policy
        if (NSGetSubscriptionAccepter() == NS_ACCEPTER_PROVIDER)
        {
            // OCDevAddr --> NSConsumer
            printf("before NSGetSubscriptionAccepter() == NS_ACCEPTER_PROVIDER)\n");
            NSAskAcceptanceToUser(entityHandlerRequest);
            printf("After NSGetSubscriptionAccepter() == NS_ACCEPTER_PROVIDER)\n");
        }
        else if (NSGetSubscriptionAccepter() == NS_ACCEPTER_CONSUMER)
        {
            printf("state = 0 - 3\n");
            NSSendSubscriptionResponse(entityHandlerRequest, NS_ACCEPTER_CONSUMER, true);
        }
    }
    else if(state == 1)
    {
        printf("state = 1 - 0\n");
        if (NSProviderWriteToCache(NS_CONSUMER_WHITELIST, entityHandlerRequest->obsInfo.obsId, state) != NS_OK)
        {
            OIC_LOG(ERROR, SUBSCRIPTION_TAG, PCF("fail to write consumer white list"));
        }
        printf("state = 1 - 1\n");
    }
}

void NSHandleUnsubscription(OCEntityHandlerRequest *entityHandlerRequest)
{
    OIC_LOG(INFO, SUBSCRIPTION_TAG, "Start to unsubscription process");

    // ProcessObserveUnregister (entityHandlerRequest);

    // write consumer info to cache
    if (NSProviderWriteToCache(NS_CONSUMER_BLACKLIST, entityHandlerRequest->devAddr) != NS_OK)
    {
        OIC_LOG(ERROR, SUBSCRIPTION_TAG, "fail to write consumer black list");
    }
}

void NSAskAcceptanceToUser(OCEntityHandlerRequest *entityHandlerRequest)
{
    OIC_LOG(DEBUG, SUBSCRIPTION_TAG, "Ask for user to Allow or Deny");

    NSPushQueue(RESPONSE_SCHEDULER, TASK_CB_SUBSCRIPTION, entityHandlerRequest);
    // one queue is required for response callback to ask subscription allowance
}

NSResult NSSendResponse(int observeId, bool accepted)
{

    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, SUBSCRIPTION_TAG, PCF("Failed to allocate Payload"));
        return NS_ERROR;
    }

    OCResourceHandle rHandle;
    if (NSPutMessageResource(NULL, &rHandle) != NS_OK)
    {
        OIC_LOG(ERROR, NOTIFICATION_TAG, PCF("Fail to put notification resource"));
        return NS_ERROR;
    }

    OCRepPayloadSetUri(payload, NSGetNotificationMessageUri());
    OCRepPayloadSetPropString(payload, NS_ATTRIBUTE_ID, "0000-0000-0000-0000");
    OCRepPayloadSetPropBool(payload, NS_ATTRIBUTE_ACCPETANCE, accepted);

    if (OCNotifyListOfObservers(rHandle, &observeId, 1, payload, OC_HIGH_QOS) != OC_STACK_OK)
    {
        OIC_LOG(ERROR, SUBSCRIPTION_TAG, "fail to send Acceptance");
        OCRepPayloadDestroy(payload);
        return NS_ERROR;

    }
    OCRepPayloadDestroy(payload);

    return NS_OK;
}

NSResult NSSendSubscriptionResponse(OCEntityHandlerRequest *entityHandlerRequest,
        NSAccessPolicy accepter, bool accepted)
{
    OIC_LOG(INFO, SUBSCRIPTION_TAG, "Send ACCEPT message to consumer");

    if (!entityHandlerRequest)
    {
        OIC_LOG (ERROR, LISTENER_TAG, "Invalid request pointer");
        return OC_EH_ERROR;
    }

    // write consumer info to cache

    printf("accepted 1 \n");
    if (accepted)
    {
        if (NSProviderWriteToCache(NS_CONSUMER_WHITELIST, entityHandlerRequest->obsInfo.obsId) != NS_OK)
        {
            OIC_LOG(ERROR, SUBSCRIPTION_TAG, "fail to write consumer white list");
        }
    }

    NSSendResponse(&entityHandlerRequest->obsInfo.obsId, accepted);

    return NS_OK;

}

void * NSSubScriptionSchedule(void *ptr)
{

    if (ptr == NULL)
    {
        OIC_LOG(INFO, SUBSCRIPTION_TAG, "Create NSSubscriptionSchedule");
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

                    NSSendAccessPolicyResponse((OCEntityHandlerRequest*) node->taskData);
                    break;

                case TASK_RECV_SUBSCRIPTION:
                    printf("before TASK_RECV_SUBSCRIPTION:\n");
                    NSHandleSubscription((OCEntityHandlerRequest*) node->taskData, 0);
                    printf("after TASK_RECV_SUBSCRIPTION:\n");
                    break;

                case TASK_RECV_UNSUBSCRIPTION:

                    NSHandleUnsubscription((OCEntityHandlerRequest*) node->taskData);
                    break;

                case TASK_SEND_ALLOW:
                {
                    NSConsumer * consumer = (NSConsumer *)node->taskData;
                    int * pObId = (int *)consumer->mUserData;

                    printf("observer ID = %d\n", *pObId);

                    if (NSProviderWriteToCache(NS_CONSUMER_WHITELIST, *pObId, 0) != NS_OK)
                    {
                        OIC_LOG(ERROR, SUBSCRIPTION_TAG, "fail to write consumer white list");
                    }
                    NSSendResponse(*pObId, true);

                    break;
                }
                case TASK_SEND_DENY:

//                    NSSendSubscriptionResponse((OCEntityHandlerRequest*) node->taskData,
//                            NS_ACCEPTER_PROVIDER, false);
                    break;

                case TASK_SYNC_SUBSCRIPTION:
                    NSHandleSubscription((OCEntityHandlerRequest*) node->taskData, 1);
                    break;

            }
            OICFree(node);
        }

        pthread_mutex_unlock(&NSMutex[SUBSCRIPTION_SCHEDULER]);

    }
    return NULL;
}
