//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
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

#include <string.h>
#include "ocadaptation.h"
#include "oic_string.h"

#include "ocstack.h"
#include "ocresource.h"
#include "ocserverrequest.h"
#include "ocstackinternal.h"
#include "ocresourcehandler.h"
#include "ocrandom.h"
#include "ocpayload.h"
#include "ocobserve.h"
#include "ocpayloadint.h"

#include "jutility.h"


// Module Name
#define MOD_NAME PCF("ocobserve")

#define TAG  PCF("OCStackObserve")

#define VERIFY_NON_NULLA(arg) { if (!arg) {OC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} }

#ifdef WITH_RESOURCE_API

static struct ResourceObserver * serverObsList = NULL;
/**
 * Determine observe QOS based on the QOS of the request.
 * The qos passed as a parameter overrides what the client requested.
 * If we want the client preference taking high priority make:
 *     qos = resourceObserver->qos;
 *
 * @param method RESTful method.
 * @param resourceObserver Observer.
 * @param appQoS Quality of service.
 * @return The quality of service of the observer.
 */
static OCQualityOfService DetermineObserverQoS(
                ResourceObserver *resourceObserver, OCQualityOfService appQoS)
{
    OCQualityOfService decidedQoS = appQoS == OC_NA_QOS ? resourceObserver->qos : appQoS;

    if (appQoS != OC_HIGH_QOS)
    {
        OC_LOG_V(INFO, TAG, "Current NON count for this observer is %d",
                resourceObserver->lowQosCount);
        if (resourceObserver->forceHighQos
                || resourceObserver->lowQosCount >= MAX_OBSERVER_NON_COUNT)
        {
            resourceObserver->lowQosCount = 0;
            // at some point we have to send CON to check on the
            // availability of observer
            OC_LOG(INFO, TAG, PCF("This time we are sending the notification as High qos"));
            decidedQoS = OC_HIGH_QOS;
        }
        else
        {
            (resourceObserver->lowQosCount)++;
        }
    }
    return decidedQoS;
}

OCStackResult SendAllObserverNotification(OCResource *resource, OCQualityOfService qos)
{
    OCStackResult result = OC_STACK_OK;
    ResourceObserver *observer;
    uint8_t numObs = 0;

    OCRequestInfo_t request;
    memset(&request, 0, sizeof (request));
    OCEntityHandlerRequest ehRequest = {0};

    // Find clients that are observing this resource
    for (observer = serverObsList; observer; observer = observer->next)
    {
        if (observer->resource != resource)
        {
            continue;
        }

        FillServerRequest(&request,
                          OC_REST_GET,
                          resource->sequenceNum,
                          qos,
                          observer->query,
                          observer->token,
                          observer->resUri,
                          &observer->devAddr);
        numObs++;
        qos = DetermineObserverQoS(observer, qos);

        request.observeResult = OC_STACK_OK;
        result = FormOCEntityHandlerRequest(&ehRequest, &request, resource);
        ehRequest.obsInfo.action = OC_OBSERVE_NO_OPTION;

        resource->entityHandler(OC_REQUEST_FLAG, &ehRequest, resource->ehCallbackParam);
    }

    if (numObs == 0)
    {
        OC_LOG(INFO, TAG, PCF("Resource has no observers"));
        result = OC_STACK_NO_OBSERVERS;
    }
    return result;
}

OCStackResult SendListObserverNotification(OCResource *resource,
                                           OCObservationId *obsIdList,
                                           uint8_t numberOfIds,
                                           const OCRepPayload *payload,
                                           uint32_t maxAge,
                                           OCQualityOfService qos)
{
    OC_LOG(INFO, TAG, PCF("Entering SendListObserverNotification"));
    (void)maxAge;

    uint8_t numIds = numberOfIds;
    ResourceObserver *observer = NULL;
    uint8_t numSentNotification = 0;
    OCStackResult result = OC_STACK_ERROR;
    OCRequestInfo_t request;
    memset(&request, 0, sizeof (request));
    bool observeErrorFlag = false;

    for (; numIds; obsIdList++, numIds--)
    {
        observer = GetObserverUsingId(*obsIdList);
        if (!observer || resource != observer->resource)
        {
            continue;
        }

        qos = DetermineObserverQoS(observer, qos);

        FillServerRequest(&request,
                          OC_REST_GET,
                          resource->sequenceNum,
                          qos,
                          observer->query,
                          observer->token,
                          observer->resUri,
                          &observer->devAddr);

        OCEntityHandlerResponse ehResponse = {0};
        ehResponse.ehResult = OC_EH_OK;
        ehResponse.payload = (OCPayload *)payload;
        ehResponse.persistentBufferFlag = 0;
        ehResponse.requestHandle = (OCRequestHandle)&request;
        ehResponse.resourceHandle = (OCResourceHandle)resource;

        result = OCDoResponse(&ehResponse);

        if (result == OC_STACK_OK)
        {
            OC_LOG_V(INFO, TAG, "Observer id %d notified.", *obsIdList);
            numSentNotification++;
        }
        else
        {
            OC_LOG_V(INFO, TAG, "Error notifying observer id %d.", *obsIdList);
            observeErrorFlag = true;
        }
    }

    if (numSentNotification == numberOfIds && !observeErrorFlag)
    {
        return OC_STACK_OK;
    }

    if (numSentNotification == 0)
    {
        return OC_STACK_NO_OBSERVERS;
    }

    OC_LOG(ERROR, TAG, PCF("Observer notification error"));
    return OC_STACK_ERROR;
}

OCStackResult GenerateObserverId(OCObservationId *observationId)
{
    ResourceObserver *resObs = NULL;

    do
    {
        *observationId = OCGetRandomByte();
        // Check if observation Id already exists
        resObs = GetObserverUsingId(*observationId);
    } while (NULL != resObs);

    OC_LOG_V(INFO, TAG, "Generated bservation ID is %u", *observationId);

    return OC_STACK_OK;
}

OCStackResult AddObserver(const char         *resUri,
                          const char         *query,
                          OCObservationId    obsId,
                          const OCToken_t    *token,
                          OCResource         *resHandle,
                          OCQualityOfService qos,
                          const OCDevAddr    *devAddr)
{
    if (!(resHandle->resourceProperties & OC_OBSERVABLE))
    {
        return OC_STACK_RESOURCE_ERROR;
    }
    ResourceObserver *obsNode = NULL;

    obsNode = (ResourceObserver *)OCCalloc(MAT_observer);

    obsNode->observeId = obsId;
    OICStrcpy(obsNode->resUri, sizeof (obsNode->resUri), resUri);
    obsNode->qos = qos;
    OICStrcpy(obsNode->query, sizeof (obsNode->query), query);
    obsNode->token = *token;
    obsNode->devAddr = *devAddr;
    obsNode->resource = resHandle;

    LL_APPEND(serverObsList, obsNode);

    return OC_STACK_OK;
}

ResourceObserver* GetObserverUsingId(const OCObservationId observeId)
{
    ResourceObserver *out = NULL;

    if (observeId)
    {
        LL_FOREACH (serverObsList, out)
        {
            if (out->observeId == observeId)
            {
                return out;
            }
        }
    }

    return NULL;
}

ResourceObserver* GetObserverUsingToken(const OCToken_t *token)
{
    ResourceObserver *out = NULL;

    OC_LOG(INFO, TAG,PCF("Looking for token"));
    OC_LOG_BUFFER(INFO, TAG, (const uint8_t *)&(token->t.chars), token->size);
    OC_LOG(INFO, TAG,PCF("\tFound token:"));

    LL_FOREACH (serverObsList, out)
    {
        OC_LOG_BUFFER(INFO, TAG, (const uint8_t *)&(out->token.t.chars), token->size);
        if (out->token.t.value == token->t.value)
        {
            return out;
        }
    }

    OC_LOG(INFO, TAG, PCF("Observer node not found"));
    return NULL;
}

OCStackResult DeleteObserverUsingToken(const OCToken_t *token)
{
    ResourceObserver *obsNode = NULL;

    obsNode = GetObserverUsingToken(token);
    if (obsNode)
    {
        OC_LOG_V(INFO, TAG, "deleting observer id  %u with token", obsNode->observeId);
        LL_DELETE (serverObsList, obsNode);
        OCFree(obsNode, MAT_observer);
    }

    return OC_STACK_OK;
}

void DeleteObserverList()
{
    ResourceObserver *out = NULL;
    LL_FOREACH_SAFE(serverObsList, out)
    {
        if (out)
        {
            DeleteObserverUsingToken(&(out->token));
        }
    }
    serverObsList = NULL;
}
#endif // WITH_RESOURCE_API
