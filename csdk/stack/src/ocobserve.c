//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
#include "ocstack.h"
#include "ocstackinternal.h"
#include "ocobserve.h"
#include "occoap.h"
#include "utlist.h"
#include "debug.h"

// Module Name
#define MOD_NAME PCF("ocobserve")

#define TAG  PCF("OCStackObserve")

#define VERIFY_NON_NULL(arg) { if (!arg) {OC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} }

static struct ObserveResourceServer *serverObsList = NULL;

OCStackResult AddObserver (const char   *resUri,
                           const char   *query,
                           uint8_t      *token,
                           size_t       tokenLength,
                           OCDevAddr    *addr,
                           OCResource   *resHandle);

OCStackResult ProcessObserveRequest (OCResource *resource, OCRequest *request)
{
    OCStackResult result;
    OCEntityHandlerRequest *ehReq = request->entityHandlerRequest;
    OCObserveReq *obs = request->observe;

    OC_LOG(INFO, TAG, "Entering ProcessObserveRequest");
    printf ("Observer option: %s\n", obs->option);
    if (strcmp ((char *)obs->option, OC_RESOURCE_OBSERVE_REGISTER) == 0) {
        // Register new observation
        printf ("Register new observation\n\n");
        result = resource->entityHandler(OC_OBSERVE_FLAG, request->entityHandlerRequest);
        if (OC_STACK_OK == result)
        {
            // Add subscriber to the server observation list
            result = AddObserver ((const char*)(request->resourceUrl), (const char *)(ehReq->query), 
                                   obs->coapToken, obs->coapTokenLen, obs->subAddr, resource);
        } 
        return result;
    } else if (strcmp ((char *)request->observe, OC_RESOURCE_OBSERVE_DEREGISTER) == 0) {
        // Deregister observation
        return OC_STACK_NOTIMPL;
    } else {
        // Invalid option
        return OC_STACK_INVALID_OBSERVE_PARAM;
    }
}

OCStackResult SendObserverNotification (OCResourceHandle handle, OCResource *resPtr) 
{
    uint8_t numObs = 0;
    OCStackResult result;
    ObserveResourceServer *obsRes = serverObsList;
    OCEntityHandlerRequest entityHandlerReq;
    unsigned char bufRes[MAX_RESPONSE_LENGTH] = {0};
    /* 
     * TODO: In the current releast all observes are set as non-confirmable since the
     * entity handler does not have a way to specify the message QoS - add a parameter. 
     * Sending all observes NON does not confirm with the observe draft (ver14).
     */
    OCQualityOfService qos = OC_NON_CONFIRMABLE;

    // Increment the sequence number
    resPtr->sequenceNum += 1;
    if (resPtr->sequenceNum == MAX_SEQUENCE_NUMBER)
        resPtr->sequenceNum = 1;

    // Find clients that are observing this resource
    while (obsRes) {
        if (obsRes->resource == handle) {
            // Invoke the entity handler for the client to process the query according to
            // the new representation
            numObs++;
            entityHandlerReq.resource = handle;
            entityHandlerReq.query = obsRes->query;
            entityHandlerReq.method = OC_REST_GET;
            entityHandlerReq.reqJSONPayload = NULL;
            entityHandlerReq.resJSONPayload = bufRes;
            entityHandlerReq.resJSONPayloadLen = MAX_RESPONSE_LENGTH;
            // Even if entity handler for a resource is not successful we continue calling
            // entity handler for other resources 
            result = resPtr->entityHandler (OC_REQUEST_FLAG, &entityHandlerReq);
            if (OC_STACK_OK == result)
            {
                printf ("Calling OCCoAPSendMessage: Payload: %s\n", entityHandlerReq.resJSONPayload);
                OCCoAPSendMessage (obsRes->addr, result, qos, obsRes->coapTok,
                                   (const char *)entityHandlerReq.resJSONPayload,
                                    resPtr->sequenceNum);
            }
        }
        obsRes = obsRes->next;
    }
    if (numObs == 0)
    {
        OC_LOG(INFO, TAG, "Resource has no observers");
        return OC_STACK_NO_OBSERVERS;
    }
    return OC_STACK_OK;
}

OCStackResult AddObserver (const char   *resUri,
                           const char   *query,
                           uint8_t      *token,
                           size_t       tokenLength,
                           OCDevAddr    *addr,
                           OCResource   *resHandle)
{
    ObserveResourceServer *obsNode;
    OCCoAPToken *tokPtr;

    obsNode = (ObserveResourceServer *) OCMalloc(sizeof(ObserveResourceServer));
    if (obsNode) {
        obsNode->resUri = (unsigned char *)OCMalloc(sizeof(strlen(resUri)+1));
        VERIFY_NON_NULL (obsNode->resUri);
        memcpy (obsNode->resUri, resUri, sizeof(strlen(resUri)+1));
        obsNode->query = (unsigned char *)OCMalloc(sizeof(strlen(query)+1));
        VERIFY_NON_NULL (obsNode->query);
        memcpy (obsNode->query, query, sizeof(strlen(query)+1));
        obsNode->coapTok = (OCCoAPToken *)OCMalloc(sizeof(OCCoAPToken));
        VERIFY_NON_NULL (obsNode->coapTok);
        tokPtr = obsNode->coapTok;
        memcpy (tokPtr->token, token, sizeof(OCCoAPToken));
        tokPtr->tokenLength = tokenLength;
        obsNode->addr = (OCDevAddr *)OCMalloc(sizeof(OCDevAddr));
        VERIFY_NON_NULL (obsNode->addr);
        memcpy (obsNode->addr, addr, sizeof(OCDevAddr));
        obsNode->resource = resHandle;

        LL_APPEND (serverObsList, obsNode);
        return OC_STACK_OK;
    }

exit:
    OCFree(obsNode->resUri);
    OCFree(obsNode->query);
    OCFree(obsNode->coapTok);
    OCFree(obsNode->addr);
    OCFree(obsNode);
    return OC_STACK_NO_MEMORY;
}
