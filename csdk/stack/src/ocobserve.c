//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "ocstack.h"
#include "ocstackconfig.h"
#include "ocstackinternal.h"
#include "ocobserve.h"
#include "ocresource.h"
#include "occoap.h"
#include "utlist.h"
#include "debug.h"
#include <string.h>

// Module Name
#define MOD_NAME PCF("ocobserve")

#define TAG  PCF("OCStackObserve")

#define VERIFY_NON_NULL(arg) { if (!arg) {OC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} }

static struct ResourceObserver * serverObsList = NULL;
extern uint32_t SERVER_DISCOVERABLE;

OCStackResult OCObserverStatus(OCCoAPToken * token, uint8_t status)
{
    OCStackResult result = OC_STACK_ERROR;
    ResourceObserver * observer = NULL;

    switch(status)
    {
    case OC_OBSERVER_NOT_INTERESTED:
        OC_LOG(DEBUG, TAG, PCF("observer is not interested in our notifications anymore"));
        //observer is dead, or it is not observing anymore
        result = DeleteObserver (token);
        if(result != OC_STACK_OK)
        {
            result = OC_STACK_OBSERVER_NOT_REMOVED;
        }
        else
        {
            OC_LOG(DEBUG, TAG, PCF("removing an observer"));
        }
        break;
    case OC_OBSERVER_STILL_INTERESTED:
        //observer is still interested
        OC_LOG(DEBUG, TAG, PCF("observer is interested in our \
                notifications, reset the failedCount"));
        observer = GetObserver(token);
        if(observer)
        {
            observer->forceCON = 0;
            observer->failedCommCount = 0;
            result = OC_STACK_OK;
        }
        else
        {
            result = OC_STACK_OBSERVER_NOT_FOUND;
        }
        break;
    case OC_OBSERVER_FAILED_COMM:
        //observer is not reachable
        OC_LOG(DEBUG, TAG, PCF("observer is not reachable"));
        observer = GetObserver(token);
        if(observer)
        {
            if(observer->failedCommCount >= MAX_OBSERVER_FAILED_COMM)
            {
                result = DeleteObserver (token);
                if(result != OC_STACK_OK)
                {
                    result = OC_STACK_OBSERVER_NOT_REMOVED;
                }
                else
                {
                    OC_LOG(DEBUG, TAG, PCF("removing an observer"));
                }
            }
            else
            {
                observer->failedCommCount++;
                result = OC_STACK_OBSERVER_NOT_REMOVED;
            }
            observer->forceCON = 1;
            OC_LOG_V(DEBUG, TAG, "Failed count for this observer is %d",observer->failedCommCount);
        }
        break;
    default:
        break;
    }

    return result;
}

OCStackResult ProcessObserveRequest (OCResource *resource, OCRequest *request)
{
    OCStackResult stackRet = OC_STACK_ERROR;
    OCEntityHandlerResult ehRet = OC_EH_ERROR;
    OCEntityHandlerRequest *ehReq = request->entityHandlerRequest;
    OCObserveReq *obs = request->observe;

    OC_LOG(INFO, TAG, PCF("Entering ProcessObserveRequest"));

    // Register new observation
    request->entityHandlerRequest->resource = (OCResourceHandle)resource;
    ehRet = resource->entityHandler(OC_OBSERVE_FLAG, request->entityHandlerRequest);
    if(ehRet == OC_EH_OK)
    {
        if (obs->option == OC_RESOURCE_OBSERVE_REGISTER)
        {
            // Add subscriber to the server observation list
            // TODO: we need to check if the obsrever is already there using its OCDevAdd....
            stackRet = AddObserver ((const char*)(request->resourceUrl), (const char *)(ehReq->query),
                    obs->token, obs->subAddr, resource, request->qos);
            if(stackRet != OC_STACK_OK)
            {
                obs->result = OC_STACK_OBSERVER_NOT_ADDED;
            }
            else
            {
                OC_LOG(DEBUG, TAG, PCF("adding an observer"));
            }
        }
        else if (obs->option == OC_RESOURCE_OBSERVE_DEREGISTER)
        {
            // Deregister observation
            stackRet = DeleteObserver (obs->token);
            if(stackRet != OC_STACK_OK)
            {
                obs->result = OC_STACK_OBSERVER_NOT_REMOVED;
            }
            else
            {
                OC_LOG(DEBUG, TAG, PCF("removing an observer"));
            }
        }
        else
        {
            // Invalid option
            OC_LOG(ERROR, TAG, PCF("Invalid CoAP observe option"));
            obs->result = OC_STACK_INVALID_OBSERVE_PARAM;
        }
        stackRet = OC_STACK_OK;
    }
    else
    {
        stackRet = OC_STACK_ERROR;
    }
    return stackRet;
}

OCStackResult SendObserverNotification (OCMethod method, OCResource *resPtr, uint32_t maxAge)
{
    uint8_t numObs = 0;
    OCStackResult stackRet = OC_STACK_ERROR;
    OCEntityHandlerResult ehRet = OC_EH_ERROR;
    ResourceObserver *resourceObserver = serverObsList;
    OCEntityHandlerRequest * entityHandlerReq = NULL;
    unsigned char* jsonPayload = NULL;
    unsigned char bufRes[MAX_RESPONSE_LENGTH] = {0};
    // TODO: we should allow the server application to define qos for each notification
    OCQualityOfService qos = OC_NON_CONFIRMABLE;

    // Find clients that are observing this resource
    while (resourceObserver)
    {
        if (resourceObserver->resource == resPtr)
        {
            numObs++;
            #ifdef WITH_PRESENCE
            if(method != OC_REST_PRESENCE)
            {
            #endif
                // Invoke the entity handler for the client to process
                // the query according to the new representation
                FormOCEntityHandlerRequest(&entityHandlerReq, OC_REST_GET, bufRes,
                        NULL, resourceObserver->query);
                entityHandlerReq->resource = (OCResourceHandle)resPtr;

                // Even if entity handler for a resource is not successful
                // we continue calling entity handler for other resources
                ehRet = BuildObsJSONResponse((OCResource *) resPtr, entityHandlerReq);
                jsonPayload = (unsigned char *)entityHandlerReq->resJSONPayload;
            #ifdef WITH_PRESENCE
            }
            else
            {
                //we know it is the default entity handler
                OC_LOG(DEBUG, TAG, "This notification is for Presence");
                // we create the payload here
                sprintf((char *)bufRes, "%u:%u", resPtr->sequenceNum, maxAge);
                jsonPayload = bufRes;
                ehRet = OC_EH_OK;
            }
            #endif
            if (OC_EH_OK == ehRet)
            {
                stackRet = OC_STACK_OK;
                OC_LOG_V(INFO, TAG, "OCStack payload: %s",
                        jsonPayload);

                // send notifications based on the qos of the request
                qos = resourceObserver->qos;
                if(qos == OC_NON_CONFIRMABLE)
                {
                    OC_LOG_V(INFO, TAG, "Current NON count for this observer is %d",
                            resourceObserver->NONCount);
                    #ifdef WITH_PRESENCE
                    if((resourceObserver->forceCON \
                            || resourceObserver->NONCount >= MAX_OBSERVER_NON_COUNT) \
                            && method != OC_REST_PRESENCE)
                    #else
                    if(resourceObserver->forceCON \
                            || resourceObserver->NONCount >= MAX_OBSERVER_NON_COUNT)
                    #endif
                    {
                        resourceObserver->NONCount = 0;
                        // at some point we have to to send CON to check on the
                        // availability of observer
                        OC_LOG(INFO, TAG, PCF("This time we are sending the \
                                notification as CON"));
                        qos = OC_CONFIRMABLE;
                    }
                    else
                    {
                        resourceObserver->NONCount++;
                    }
                }
                OCSendCoAPNotification(resourceObserver->resUri, resourceObserver->addr,
                        stackRet, qos,
                        resourceObserver->token,
                        jsonPayload, resPtr->sequenceNum, maxAge);
            }
            else
            {
                stackRet = OC_STACK_ERROR;
            }

            OCFree(entityHandlerReq);
        }
        resourceObserver = resourceObserver->next;
    }
    if (numObs == 0)
    {
        OC_LOG(INFO, TAG, PCF("Resource has no observers"));
        stackRet = OC_STACK_NO_OBSERVERS;
    }
    return stackRet;
}

OCStackResult AddObserver (const char   *resUri,
                           const char   *query,
                           OCCoAPToken * token,
                           OCDevAddr    *addr,
                           OCResource   *resHandle,
                           OCQualityOfService qos)
{
    ResourceObserver *obsNode = NULL;
    OCCoAPToken *tokPtr = NULL;

    obsNode = (ResourceObserver *) OCMalloc(sizeof(ResourceObserver));
    if (obsNode)
    {
        obsNode->resUri = (unsigned char *)OCMalloc(strlen(resUri)+1);
        VERIFY_NON_NULL (obsNode->resUri);
        memcpy (obsNode->resUri, resUri, strlen(resUri)+1);
        obsNode->qos = qos;
        if(query)
        {
            obsNode->query = (unsigned char *)OCMalloc(strlen(query)+1);
            VERIFY_NON_NULL (obsNode->query);
            memcpy (obsNode->query, query, strlen(query)+1);
        }
        obsNode->token = (OCCoAPToken *)OCMalloc(sizeof(OCCoAPToken));
        VERIFY_NON_NULL (obsNode->token);
        tokPtr = obsNode->token;
        tokPtr->tokenLength = token->tokenLength;
        memcpy (tokPtr->token, token->token, tokPtr->tokenLength);
        obsNode->addr = (OCDevAddr *)OCMalloc(sizeof(OCDevAddr));
        VERIFY_NON_NULL (obsNode->addr);
        memcpy (obsNode->addr, addr, sizeof(OCDevAddr));
        obsNode->resource = resHandle;
        obsNode->failedCommCount = 0;
        obsNode->NONCount = 0;
        obsNode->forceCON = 0;

        LL_APPEND (serverObsList, obsNode);
        return OC_STACK_OK;
    }

exit:
    OCFree(obsNode->resUri);
    OCFree(obsNode->query);
    OCFree(obsNode->token);
    OCFree(obsNode->addr);
    OCFree(obsNode);
    return OC_STACK_NO_MEMORY;
}

ResourceObserver* GetObserver (const OCCoAPToken * token)
{
    ResourceObserver *out = NULL;

    if(token) 
    {
        LL_FOREACH (serverObsList, out) 
        {
            if((out->token->tokenLength == token->tokenLength) &&
               (memcmp(out->token->token, token->token, token->tokenLength) == 0))
            {
                return out;
            }
        }
    }
    OC_LOG(INFO, TAG, PCF("Observer node not found!!"));
    return NULL;
}

OCStackResult DeleteObserver (OCCoAPToken * token)
{
    ResourceObserver *obsNode = NULL;

    obsNode = GetObserver (token);
    if (obsNode)
    {
        LL_DELETE (serverObsList, obsNode);
        OCFree(obsNode->resUri);
        OCFree(obsNode->query);
        OCFree(obsNode->token);
        OCFree(obsNode->addr);
        OCFree(obsNode);
    }
    // it is ok if we did not find the observer...
    return OC_STACK_OK;
}

void DeleteObserverList()
{
    ResourceObserver *out = NULL;
    ResourceObserver *tmp = NULL;
    LL_FOREACH_SAFE (serverObsList, out, tmp) 
    {
        DeleteObserver (out->token);
    }
    serverObsList = NULL;
}
