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


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#define _POSIX_C_SOURCE 200112L
#include <string.h>
#include <ctype.h>

#include "ocstack.h"
#include "ocstackinternal.h"
#include "ocresourcehandler.h"
#include "occlientcb.h"
#include "ocobserve.h"
#include "ocrandom.h"
#include "ocmalloc.h"
#include "ocserverrequest.h"
#include "ocsecurityinternal.h"

#include "cacommon.h"
#include "cainterface.h"

#ifdef WITH_ARDUINO
#include "Time.h"
#else
#include <sys/time.h>
#endif
#include "coap_time.h"
#include "utlist.h"
#include "pdu.h"

#ifndef ARDUINO
#include <arpa/inet.h>
#endif

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef enum {
    OC_STACK_UNINITIALIZED = 0, OC_STACK_INITIALIZED, OC_STACK_UNINIT_IN_PROGRESS
} OCStackState;
#ifdef WITH_PRESENCE
typedef enum {
    OC_PRESENCE_UNINITIALIZED = 0, OC_PRESENCE_INITIALIZED
} OCPresenceState;
#endif

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------
static OCStackState stackState = OC_STACK_UNINITIALIZED;

OCResource *headResource = NULL;
static OCResource *tailResource = NULL;
#ifdef WITH_PRESENCE
static OCPresenceState presenceState = OC_PRESENCE_UNINITIALIZED;
static PresenceResource presenceResource;
static uint8_t PresenceTimeOutSize = 0;
static uint32_t PresenceTimeOut[] = {50, 75, 85, 95, 100};
#endif

OCMode myStackMode;
OCDeviceEntityHandler defaultDeviceHandler;
OCStackResult getQueryFromUri(const char * uri, unsigned char** resourceType, char ** newURI);

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define TAG  PCF("OCStack")
#define VERIFY_SUCCESS(op, successCode) { if (op != successCode) \
            {OC_LOG_V(FATAL, TAG, "%s failed!!", #op); goto exit;} }
#define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); return (retVal); } }
#define VERIFY_NON_NULL_V(arg) { if (!arg) {OC_LOG_V(FATAL, TAG, "%s is NULL", #arg);\
    goto exit;} }

//TODO: we should allow the server to define this
#define MAX_OBSERVE_AGE (0x2FFFFUL)

//=============================================================================
// Helper Functions
//=============================================================================
static uint32_t GetTime(float afterSeconds)
{
    coap_tick_t now;
    coap_ticks(&now);
    return now + (uint32_t)(afterSeconds * COAP_TICKS_PER_SECOND);
}

static OCStackResult FormOCResponse(OCResponse * * responseLoc,
                                    ClientCB * cbNode,
                                    uint32_t maxAge,
                                    unsigned char * fullUri,
                                    unsigned char * rcvdUri,
                                    CAToken_t * rcvdToken,
                                    OCClientResponse * clientResponse,
                                    unsigned char * bufRes)
{
    OCResponse * response = (OCResponse *) OCMalloc(sizeof(OCResponse));
    if (!response)
    {
        return OC_STACK_NO_MEMORY;
    }
    response->cbNode = cbNode;
    response->maxAge = maxAge;
    response->fullUri = fullUri;
    response->rcvdUri = rcvdUri;
    response->rcvdToken = rcvdToken;
    response->clientResponse = clientResponse;
    response->bufRes = bufRes;

    *responseLoc = response;
    return OC_STACK_OK;
}

/// This method is used to create the IPv4 dev_addr structure.
/// TODO: Remove in future. Temporary helper function.
/// Builds a socket interface address using IP address and port number
static int32_t OCBuildIPv4Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d,
        uint16_t port, OCDevAddr *ipAddr)
{
    if ( !ipAddr ) {
        OC_LOG(FATAL, TAG, "Invalid argument");
        return 1;
    }

    ipAddr->addr[0] = a;
    ipAddr->addr[1] = b;
    ipAddr->addr[2] = c;
    ipAddr->addr[3] = d;
    *((uint16_t*)&(ipAddr->addr[4])) = port;

    return 0;
}

//-----------------------------------------------------------------------------
// Internal API function
//-----------------------------------------------------------------------------

// This internal function is called to update the stack with the status of
// observers and communication failures
OCStackResult OCStackFeedBack(CAToken_t * token, uint8_t status)
{
    OCStackResult result = OC_STACK_ERROR;
    ResourceObserver * observer = NULL;
    OCEntityHandlerRequest ehRequest = {0};

    switch(status)
    {
    case OC_OBSERVER_NOT_INTERESTED:
        OC_LOG(DEBUG, TAG, PCF("observer is not interested in our notifications anymore"));
        observer = GetObserverUsingToken (token);
        if(observer)
        {
            result = FormOCEntityHandlerRequest(&ehRequest, (OCRequestHandle) NULL,
                    OC_REST_NOMETHOD, (OCResourceHandle) NULL, NULL, NULL, 0,
                    NULL, OC_OBSERVE_DEREGISTER, observer->observeId);
            if(result != OC_STACK_OK)
            {
                return result;
            }
            observer->resource->entityHandler(OC_OBSERVE_FLAG, &ehRequest);
        }
        //observer is not observing anymore
        result = DeleteObserverUsingToken (token);
        if(result == OC_STACK_OK)
        {
            OC_LOG(DEBUG, TAG, PCF("Removed observer successfully"));
        }
        else
        {
            result = OC_STACK_OK;
            OC_LOG(DEBUG, TAG, PCF("Observer Removal failed"));
        }
        break;
    case OC_OBSERVER_STILL_INTERESTED:
        //observer is still interested
        OC_LOG(DEBUG, TAG, PCF("observer is interested in our \
                notifications, reset the failedCount"));
        observer = GetObserverUsingToken (token);
        if(observer)
        {
            observer->forceHighQos = 0;
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
        OC_LOG(DEBUG, TAG, PCF("observer is unreachable"));
        observer = GetObserverUsingToken (token);
        if(observer)
        {
            if(observer->failedCommCount >= MAX_OBSERVER_FAILED_COMM)
            {
                result = FormOCEntityHandlerRequest(&ehRequest, (OCRequestHandle) NULL,
                        OC_REST_NOMETHOD, (OCResourceHandle) NULL, NULL, NULL, 0,
                        NULL, OC_OBSERVE_DEREGISTER, observer->observeId);
                if(result != OC_STACK_OK)
                {
                    return OC_STACK_ERROR;
                }
                observer->resource->entityHandler(OC_OBSERVE_FLAG, &ehRequest);
                //observer is unreachable
                result = DeleteObserverUsingToken (token);
                if(result == OC_STACK_OK)
                {
                    OC_LOG(DEBUG, TAG, PCF("Removed observer successfully"));
                }
                else
                {
                    result = OC_STACK_OK;
                    OC_LOG(DEBUG, TAG, PCF("Observer Removal failed"));
                }
            }
            else
            {
                observer->failedCommCount++;
                result = OC_STACK_CONTINUE;
            }
            observer->forceHighQos = 1;
            OC_LOG_V(DEBUG, TAG, "Failed count for this observer is %d",observer->failedCommCount);
        }
        break;
    default:
        OC_LOG(ERROR, TAG, PCF("Unknown status"));
        result = OC_STACK_ERROR;
        break;
        }
    return result;
}

OCStackResult CAToOCStackResult(CAResponseResult_t caCode)
{
    OCStackResult ret = OC_STACK_ERROR;

    switch(caCode)
    {
        case CA_SUCCESS:
            ret = OC_STACK_OK;
            break;
        case CA_CREATED:
            ret = OC_STACK_RESOURCE_CREATED;
            break;
        case CA_DELETED:
            ret = OC_STACK_RESOURCE_DELETED;
            break;
        case CA_BAD_REQ:
            ret = OC_STACK_INVALID_QUERY;
            break;
        case CA_BAD_OPT:
            ret = OC_STACK_INVALID_OPTION;
            break;
        case CA_NOT_FOUND:
            ret = OC_STACK_NO_RESOURCE;
            break;
        default:
            break;
    }
    return ret;
}

OCStackResult OCToCAConnectivityType(OCConnectivityType ocConType, CAConnectivityType_t* caConType)
{
    OCStackResult ret = OC_STACK_OK;

    switch(ocConType)
    {
        case OC_ETHERNET:
            *caConType = CA_ETHERNET;
            break;
        case OC_WIFI:
            *caConType = CA_WIFI;
            break;
        case OC_EDR:
            *caConType = CA_EDR;
            break;
        case OC_LE:
            *caConType = CA_LE;
            break;
        case OC_ALL:
            // Currently OC_ALL represents WIFI and ETHERNET
            // Add other connectivity types as they are enabled in future
            *caConType = (CAConnectivityType_t) (CA_WIFI|CA_ETHERNET);
            break;
        default:
            ret = OC_STACK_INVALID_PARAM;
            break;
    }
    return ret;
}

OCStackResult CAToOCConnectivityType(CAConnectivityType_t caConType, OCConnectivityType *ocConType)
{
    OCStackResult ret = OC_STACK_OK;

    switch(caConType)
    {
        case CA_ETHERNET:
            *ocConType = OC_ETHERNET;
            break;
        case CA_WIFI:
            *ocConType = OC_WIFI;
            break;
        case CA_EDR:
            *ocConType = OC_EDR;
            break;
        case CA_LE:
            *ocConType = OC_LE;
            break;
        default:
            ret = OC_STACK_INVALID_PARAM;
            break;
    }
    return ret;
}

// update response.addr appropriately from endPoint.addressInfo
OCStackResult UpdateResponseAddr(OCClientResponse *response, const CARemoteEndpoint_t* endPoint)
{
    OCStackResult ret = OC_STACK_ERROR;
    static OCDevAddr address = {0};
    char * tok = NULL;
    char * savePtr = NULL;
    char * cpAddress = (char *) OCMalloc(strlen(endPoint->addressInfo.IP.ipAddress) + 1);
    if(!cpAddress)
    {
        ret = OC_STACK_NO_MEMORY;
        goto exit;
    }
    memcpy(cpAddress, endPoint->addressInfo.IP.ipAddress,
            strlen(endPoint->addressInfo.IP.ipAddress) + 1);

    // Grabs the first three numbers from the IPv4 address and replaces dots
    for(int i=0; i<4; i++)
    {
        tok = strtok_r(i==0 ? cpAddress : NULL, ".", &savePtr);

        if(!tok)
        {
            ret = OC_STACK_ERROR;
            goto exit;
        }
        address.addr[i] = atoi(tok);
    }

    memcpy(&address.addr[4], &endPoint->addressInfo.IP.port, sizeof(uint32_t));

    if(response)
    {
        response->addr = &address;
        ret = CAToOCConnectivityType(endPoint->connectivityType, &(response->connType));
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("OCClientResponse is NULL!"));
    }
exit:
    OCFree(cpAddress);
    return ret;
}

void parsePresencePayload(char* payload, uint32_t* seqNum, uint32_t* maxAge, char** resType)
{
    char * tok = NULL;
    char * savePtr;
    // The format of the payload is {"oc":[%u:%u:%s]}
    // %u : sequence number,
    // %u : max age
    // %s : Resource Type (Optional)
    tok = strtok_r(payload, "[:]}", &savePtr);
    payload[strlen(payload)] = ':';
    tok = strtok_r(NULL, "[:]}", &savePtr);
    payload[strlen((char *)payload)] = ':';
    *seqNum = (uint32_t) atoi(tok);

    tok = strtok_r(NULL, "[:]}", &savePtr);
    *maxAge = (uint32_t) atoi(tok);
    tok = strtok_r(NULL, "[:]}",&savePtr);

    if(tok)
    {
        *resType = (char *)OCMalloc(strlen(tok));
        if(!*resType)
        {
            return;
        }
        payload[strlen((char *)payload)] = ':';
        strcpy(*resType, tok);
        OC_LOG_V(DEBUG, TAG, "----------------resourceTypeName %s", *resType);
    }
    payload[strlen((char *)payload)] = ']';
}

OCStackResult HandlePresenceResponse(const CARemoteEndpoint_t* endPoint,
                            const CAResponseInfo_t* responseInfo)
{
    OCStackApplicationResult cbResult = OC_STACK_DELETE_TRANSACTION;
    ClientCB * cbNode = NULL;
    char *resourceTypeName = NULL;
    OCClientResponse response;
    OCStackResult result = OC_STACK_ERROR;
    uint32_t lowerBound = 0;
    uint32_t higherBound = 0;
    uint32_t maxAge = 0;

    char *fullUri = NULL;
    char *ipAddress = NULL;
    int presenceSubscribe = 0;
    int multicastPresenceSubscribe = 0;

    if (responseInfo->result != CA_SUCCESS)
    {
        OC_LOG_V(ERROR, TAG, "HandlePresenceResponse failed %d", responseInfo->result);
        return OC_STACK_ERROR;
    }

    fullUri = (char *) OCMalloc(MAX_URI_LENGTH );

    if(NULL == fullUri)
    {
        OC_LOG(ERROR, TAG, PCF("Memory could not be allocated for fullUri"));
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    ipAddress = (char *) OCMalloc(strlen(endPoint->addressInfo.IP.ipAddress) + 1);

    if(NULL == ipAddress)
    {
        OC_LOG(ERROR, TAG, PCF("Memory could not be allocated for ipAddress"));
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    strncpy(ipAddress, endPoint->addressInfo.IP.ipAddress,
                            strlen(endPoint->addressInfo.IP.ipAddress));
    ipAddress[strlen(endPoint->addressInfo.IP.ipAddress)] = '\0';

    snprintf(fullUri, MAX_URI_LENGTH, "coap://%s:%u%s", ipAddress, endPoint->addressInfo.IP.port,
                OC_PRESENCE_URI);

    cbNode = GetClientCB(NULL, NULL, (unsigned char *) fullUri);

    if(cbNode)
    {
        presenceSubscribe = 1;
    }
    else
    {
        snprintf(fullUri, MAX_URI_LENGTH, "%s%s", OC_MULTICAST_IP, endPoint->resourceUri);
        cbNode = GetClientCB(NULL, NULL, (unsigned char *) fullUri);
        if(cbNode)
        {
            multicastPresenceSubscribe = 1;
        }
    }

    if(!presenceSubscribe && !multicastPresenceSubscribe)
    {
        OC_LOG(INFO, TAG, PCF("Received a presence notification, but I do not have callback \
                                                ------------ ignoring"));
        goto exit;
    }

    // No payload to the application in case of presence
    response.resJSONPayload = NULL;
    response.result = OC_STACK_OK;

    result = UpdateResponseAddr(&response, endPoint);
    if(result != OC_STACK_OK)
    {
        goto exit;
    }

    if(responseInfo->info.payload)
    {
        parsePresencePayload(responseInfo->info.payload,
                                &(response.sequenceNumber),
                                &maxAge,
                                &resourceTypeName);
    }

    if(presenceSubscribe)
    {
        if(cbNode->sequenceNumber == response.sequenceNumber)
        {
            OC_LOG(INFO, TAG, PCF("===============No presence change"));
            goto exit;
        }

        if(maxAge == 0)
        {
            OC_LOG(INFO, TAG, PCF("===============Stopping presence"));
            response.result = OC_STACK_PRESENCE_STOPPED;
            if(cbNode->presence)
            {
                OCFree(cbNode->presence->timeOut);
                OCFree(cbNode->presence);
                cbNode->presence = NULL;
            }
        }
        else
        {
            if(!cbNode->presence)
            {
                cbNode->presence = (OCPresence *) OCMalloc(sizeof(OCPresence));

                if(!(cbNode->presence))
                {
                    OC_LOG(ERROR, TAG, PCF("Could not allocate memory for cbNode->presence"));
                    result = OC_STACK_NO_MEMORY;
                    goto exit;
                }

                VERIFY_NON_NULL_V(cbNode->presence);
                cbNode->presence->timeOut = NULL;
                cbNode->presence->timeOut = (uint32_t *)
                        OCMalloc(PresenceTimeOutSize * sizeof(uint32_t));
                if(!(cbNode->presence->timeOut)){
                    OC_LOG(ERROR, TAG,
                                  PCF("Could not allocate memory for cbNode->presence->timeOut"));
                    OCFree(cbNode->presence);
                    result = OC_STACK_NO_MEMORY;
                    goto exit;
                }
            }

            OC_LOG_V(INFO, TAG, "===============Update presence TTL, now time is %u", GetTime(0));
            cbNode->presence->TTL = maxAge;
            for(int index = 0; index < PresenceTimeOutSize; index++)
            {
                lowerBound = GetTime(PresenceTimeOut[index]/ 100.0f*cbNode->presence->TTL);
                higherBound = GetTime(PresenceTimeOut[index + 1]/100.0f*cbNode->presence->TTL);
                cbNode->presence->timeOut[index] = OCGetRandomRange(lowerBound, higherBound);
                OC_LOG_V(DEBUG, TAG, "----------------lowerBound timeout  %d", lowerBound);
                OC_LOG_V(DEBUG, TAG, "----------------higherBound timeout %d", higherBound);
                OC_LOG_V(DEBUG, TAG, "----------------timeOut entry  %d",
                        cbNode->presence->timeOut[index]);
            }
            cbNode->presence->TTLlevel = 0;
            OC_LOG_V(DEBUG, TAG, "----------------this TTL level %d", cbNode->presence->TTLlevel);


            OC_LOG(INFO, TAG, PCF("===============Presence changed, calling up the stack"));
            cbNode->sequenceNumber = response.sequenceNumber;

            // Ensure that a filter is actually applied.
            if(resourceTypeName && cbNode->filterResourceType)
            {
                if(!findResourceType(cbNode->filterResourceType, resourceTypeName))
                {
                    goto exit;
                }
            }
        }
    }
    else
    {
        // This is the multicast case

        OCMulticastNode* mcNode = NULL;
        mcNode = GetMCPresenceNode((const unsigned char *)fullUri);

        if(mcNode != NULL)
        {
            if(mcNode->nonce == response.sequenceNumber)
            {
                OC_LOG(INFO, TAG, PCF("===============No presence change (Multicast)"));
                goto exit;
            }
            mcNode->nonce = response.sequenceNumber;

            if(maxAge == 0)
            {
                OC_LOG(INFO, TAG, PCF("===============Stopping presence"));
                response.result = OC_STACK_PRESENCE_STOPPED;
            }
        }
        else
        {
            uint32_t uriLen = strlen((char*)fullUri);
            unsigned char* uri = (unsigned char *) OCMalloc(uriLen + 1);
            if(uri)
            {
                memcpy(uri, fullUri, (uriLen + 1));
            }
            else
            {
                OC_LOG(INFO, TAG,
                    PCF("===============No Memory for URI to store in the presence node"));
                result = OC_STACK_NO_MEMORY;
                goto exit;
            }
            result = AddMCPresenceNode(&mcNode, (unsigned char*) uri, response.sequenceNumber);
            if(result == OC_STACK_NO_MEMORY)
            {
                OC_LOG(INFO, TAG,
                    PCF("===============No Memory for Multicast Presence Node"));
                result = OC_STACK_NO_MEMORY;
                OCFree(uri);
                goto exit;
            }
        }

        // Ensure that a filter is actually applied.
        if(resourceTypeName && cbNode->filterResourceType)
        {
            if(!findResourceType(cbNode->filterResourceType, resourceTypeName))
            {
                goto exit;
            }
        }
    }

    cbResult = cbNode->callBack(cbNode->context, cbNode->handle, &response);

    if (cbResult == OC_STACK_DELETE_TRANSACTION)
    {
        FindAndDeleteClientCB(cbNode);
    }

exit:
OCFree(fullUri);
OCFree(ipAddress);
OCFree(resourceTypeName);
return result;
}


//This function will be called back by CA layer when a response is received
void HandleCAResponses(const CARemoteEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo)
{
    OC_LOG(INFO, TAG, PCF("Enter HandleCAResponses"));

    OCStackApplicationResult appResult = OC_STACK_DELETE_TRANSACTION;

    if(NULL == endPoint)
    {
        OC_LOG(ERROR, TAG, PCF("endPoint is NULL"));
        return;
    }

    if(NULL == responseInfo)
    {
        OC_LOG(ERROR, TAG, PCF("responseInfo is NULL"));
        return;
    }

    if(strcmp(endPoint->resourceUri, OC_PRESENCE_URI) == 0)
    {
        HandlePresenceResponse(endPoint, responseInfo);
        return;
    }

    ClientCB *cbNode = GetClientCB((CAToken_t *)&(responseInfo->info.token), NULL, NULL);

    if (cbNode)
    {
        OC_LOG(INFO, TAG, PCF("Calling into application address space"));
        OCClientResponse response;

        OCStackResult result = UpdateResponseAddr(&response, endPoint);
        if(result != OC_STACK_OK)
        {
            OC_LOG(ERROR, TAG, PCF("Invalid connectivity type in endpoint"));
            return;
        }

        response.result = CAToOCStackResult(responseInfo->result);
        response.resJSONPayload = (unsigned char*)responseInfo->info.payload;
        response.numRcvdVendorSpecificHeaderOptions = 0;
        if(responseInfo->info.options && responseInfo->info.numOptions > 0)
        {
            int start = 0;
            //First option always with option ID is OC_COAP_OPTION_OBSERVE if it is available.
            if(responseInfo->info.options[0].optionID == COAP_OPTION_OBSERVE)
            {
                memcpy (&(response.sequenceNumber),
                            &(responseInfo->info.options[0].optionData), sizeof(uint32_t));
                response.numRcvdVendorSpecificHeaderOptions = responseInfo->info.numOptions - 1;
                start = 1;
            }
            else
            {
               response.numRcvdVendorSpecificHeaderOptions = responseInfo->info.numOptions;
            }

            if(response.numRcvdVendorSpecificHeaderOptions > MAX_HEADER_OPTIONS)
            {
                OC_LOG(ERROR, TAG, PCF("#header options are more than MAX_HEADER_OPTIONS"));
                return;
            }

            for (uint8_t i = start; i < responseInfo->info.numOptions; i++)
            {
                memcpy (&(response.rcvdVendorSpecificHeaderOptions[i-start]),
                 &(responseInfo->info.options[i]), sizeof(OCHeaderOption));
            }
        }
        appResult = cbNode->callBack(cbNode->context,
                cbNode->handle, &response);
        if (appResult == OC_STACK_DELETE_TRANSACTION)
        {
            FindAndDeleteClientCB(cbNode);
        }
    }
    OC_LOG_V(INFO, TAG, PCF("Received payload: %s\n"), (char*)responseInfo->info.payload);
    OC_LOG(INFO, TAG, PCF("Exit HandleCAResponses"));
}

//This function will be called back by CA layer when a request is received
void HandleCARequests(const CARemoteEndpoint_t* endPoint, const CARequestInfo_t* requestInfo)
{
    OC_LOG(INFO, TAG, PCF("Enter HandleCARequests"));
    if(!endPoint)
    {
        OC_LOG(ERROR, TAG, PCF("endPoint is NULL"));
        return;
    }

    if(!requestInfo)
    {
        OC_LOG(ERROR, TAG, PCF("requestInfo is NULL"));
        return;
    }

    OCStackResult requestResult = OC_STACK_ERROR;

    if(myStackMode == OC_CLIENT)
    {
        //TODO: should the client be responding to requests?
        return;
    }

    OCServerProtocolRequest serverRequest = {};

    OC_LOG_V(INFO, TAG, PCF("***** Endpoint URI ***** : %s\n"), (char*)endPoint->resourceUri);

    char * newUri = (char *)endPoint->resourceUri;
    unsigned char * query = NULL;
    getQueryFromUri(endPoint->resourceUri, &query, &newUri);
    OC_LOG_V(INFO, TAG, PCF("**********URI without query ****: %s\n"), newUri);
    OC_LOG_V(INFO, TAG, PCF("**********Query ****: %s\n"), query);
    if(strlen(newUri) < MAX_URI_LENGTH)
    {
        //copy URI
        memcpy (&(serverRequest.resourceUrl), newUri, strlen(newUri));
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("URI length exceeds MAX_URI_LENGTH."));
        return;
    }
    //copy query
    if(query)
    {
        if(strlen((char*)query) < MAX_QUERY_LENGTH)
        {
            memcpy (&(serverRequest.query), query, strlen((char*)query));
        }
        else
        {
            OC_LOG(ERROR, TAG, PCF("Query length exceeds MAX_QUERY_LENGTH."));
            return;
        }
    }
    //copy request payload
    if (requestInfo->info.payload)
    {
        serverRequest.reqTotalSize = strlen(requestInfo->info.payload) + 1;
        memcpy (&(serverRequest.reqJSONPayload), requestInfo->info.payload,
                strlen(requestInfo->info.payload));
        serverRequest.reqTotalSize = strlen((const char *)requestInfo->info.payload) + 1;
    }
    else
    {
        serverRequest.reqTotalSize = 1;
    }

    switch (requestInfo->method)
    {
        case CA_GET:
            {
                serverRequest.method = OC_REST_GET;
                break;
            }
        case CA_PUT:
            {
                serverRequest.method = OC_REST_PUT;
                break;
            }
        case CA_POST:
            {
                serverRequest.method = OC_REST_POST;
                break;
            }
        case CA_DELETE:
            {
                serverRequest.method = OC_REST_DELETE;
                break;
            }
        default:
            {
                OC_LOG(ERROR, TAG, PCF("Received CA method %d not supported"));
                return;
            }
    }

    OC_LOG_V(INFO, TAG, "HandleCARequests: CA token length = %d", CA_MAX_TOKEN_LEN);
    OC_LOG_BUFFER(INFO, TAG, (const uint8_t *)requestInfo->info.token, CA_MAX_TOKEN_LEN);

    serverRequest.requestToken = (CAToken_t)OCCalloc(1, CA_MAX_TOKEN_LEN+1);
    // Module Name
    if (!serverRequest.requestToken)
    {
        OC_LOG(FATAL, TAG, "Server Request Token is NULL");
        return;
    }
    memcpy(serverRequest.requestToken, requestInfo->info.token, CA_MAX_TOKEN_LEN);

    if (requestInfo->info.type == CA_MSG_CONFIRM)
    {
        serverRequest.qos = OC_HIGH_QOS;
    }
    else if (requestInfo->info.type == CA_MSG_NONCONFIRM)
    {
        serverRequest.qos = OC_LOW_QOS;
    }
    else if (requestInfo->info.type == CA_MSG_ACKNOWLEDGE)
    {
        // TODO-CA: Need to handle this
    }
    else if (requestInfo->info.type == CA_MSG_RESET)
    {
        // TODO-CA: Need to handle this
    }
    // CA does not need the following 3 fields
    serverRequest.coapID = 0;
    serverRequest.delayedResNeeded = 0;
    serverRequest.secured = endPoint->isSecured;

    // copy the address
    serverRequest.addressInfo      = endPoint->addressInfo;
    serverRequest.connectivityType = endPoint->connectivityType;

    // copy vendor specific header options
    // TODO-CA: CA is including non-vendor header options as well, like observe.
    // Need to filter those out
    uint8_t tempNum = (requestInfo->info.numOptions);
    GetObserveHeaderOption(&serverRequest.observationOption, requestInfo->info.options, &tempNum);
    if (requestInfo->info.numOptions > MAX_HEADER_OPTIONS)
    {
        OC_LOG(ERROR, TAG,
                PCF("The request info numOptions is greater than MAX_HEADER_OPTIONS"));
        OCFree(serverRequest.requestToken);
        return;
    }
    serverRequest.numRcvdVendorSpecificHeaderOptions = tempNum;
    if (serverRequest.numRcvdVendorSpecificHeaderOptions)
    {
        memcpy (&(serverRequest.rcvdVendorSpecificHeaderOptions), requestInfo->info.options,
            sizeof(CAHeaderOption_t)*tempNum);
    }

    requestResult = HandleStackRequests (&serverRequest);
    if(requestResult != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("HandleStackRequests failed"));
    }
    OC_LOG(INFO, TAG, PCF("Exit HandleCARequests"));
}

//This function will be called back by occoap layer when a request is received
OCStackResult HandleStackRequests(OCServerProtocolRequest * protocolRequest)
{
    OC_LOG(INFO, TAG, PCF("Entering HandleStackRequests (OCStack Layer)"));
    OCStackResult result = OC_STACK_ERROR;
    ResourceHandling resHandling;
    OCResource *resource;
    if(!protocolRequest)
    {
        OC_LOG(ERROR, TAG, PCF("protocolRequest is NULL"));
        return OC_STACK_INVALID_PARAM;
    }

    OCServerRequest * request = GetServerRequestUsingToken(protocolRequest->requestToken);
    if(!request)
    {
        OC_LOG(INFO, TAG, PCF("This is a new Server Request"));
        result = AddServerRequest(&request, protocolRequest->coapID,
                protocolRequest->delayedResNeeded, protocolRequest->secured, 0,
                protocolRequest->method, protocolRequest->numRcvdVendorSpecificHeaderOptions,
                protocolRequest->observationOption, protocolRequest->qos,
                protocolRequest->query, protocolRequest->rcvdVendorSpecificHeaderOptions,
                protocolRequest->reqJSONPayload, &protocolRequest->requestToken,
                protocolRequest->resourceUrl,protocolRequest->reqTotalSize,
                &protocolRequest->addressInfo, protocolRequest->connectivityType);
        if (OC_STACK_OK != result)
        {
            OC_LOG(ERROR, TAG, PCF("Error adding server request"));
            return result;
        }
        VERIFY_NON_NULL(request, ERROR, OC_STACK_NO_MEMORY);

        if(!protocolRequest->reqMorePacket)
        {
            request->requestComplete = 1;
        }
    }
    else
    {
        OC_LOG(INFO, TAG, PCF("This is either a repeated Server Request or blocked Server Request"));
    }

    if(request->requestComplete)
    {
        OC_LOG(INFO, TAG, PCF("This Server Request is complete"));
        result = DetermineResourceHandling (request, &resHandling, &resource);
        if (result == OC_STACK_OK)
        {
            result = ProcessRequest(resHandling, resource, request);
        }
        else
        {
            result = OC_STACK_ERROR;
        }
    }
    else
    {
        OC_LOG(INFO, TAG, PCF("This Server Request is incomplete"));
        result = OC_STACK_CONTINUE;
    }
    return result;
}

int ParseIPv4Address(unsigned char * ipAddrStr, uint8_t * ipAddr, uint16_t * port)
{
    size_t index = 0;
    unsigned char *itr, *coap;
    uint8_t dotCount = 0;

    ipAddr[index] = 0;
    *port = 0;
    /* search for scheme */
    itr = ipAddrStr;
    if (!isdigit((unsigned char) *ipAddrStr))
    {
        coap = (unsigned char *) OC_COAP_SCHEME;
        while (*coap && tolower(*itr) == *coap)
        {
            coap++;
            itr++;
        }
    }
    ipAddrStr = itr;

    while (*ipAddrStr) {
        if (isdigit((unsigned char) *ipAddrStr))
        {
            ipAddr[index] *= 10;
            ipAddr[index] += *ipAddrStr - '0';
        }
        else if ((unsigned char) *ipAddrStr == '.')
        {
            index++;
            dotCount++;
            ipAddr[index] = 0;
        }
        else
        {
            break;
        }
        ipAddrStr++;
    }
    if(*ipAddrStr == ':')
    {
        ipAddrStr++;
        while (*ipAddrStr){
            if (isdigit((unsigned char) *ipAddrStr))
            {
                *port *= 10;
                *port += *ipAddrStr - '0';
            }
            else
            {
                break;
            }
            ipAddrStr++;
        }
    }


    if (ipAddr[0] < 255 && ipAddr[1] < 255 && ipAddr[2] < 255 && ipAddr[3] < 255
            && dotCount == 3)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Private internal function prototypes
//-----------------------------------------------------------------------------

static OCDoHandle GenerateInvocationHandle();
static OCStackResult initResources();
static void insertResource(OCResource *resource);
static OCResource *findResource(OCResource *resource);
static void insertResourceType(OCResource *resource,
        OCResourceType *resourceType);
static OCResourceType *findResourceTypeAtIndex(OCResourceHandle handle,
        uint8_t index);
static void insertResourceInterface(OCResource *resource,
        OCResourceInterface *resourceInterface);
static OCResourceInterface *findResourceInterfaceAtIndex(
        OCResourceHandle handle, uint8_t index);
static void deleteResourceType(OCResourceType *resourceType);
static void deleteResourceInterface(OCResourceInterface *resourceInterface);
static void deleteResourceElements(OCResource *resource);
static int deleteResource(OCResource *resource);
static void deleteAllResources();
static void incrementSequenceNumber(OCResource * resPtr);
static OCStackResult verifyUriQueryLength(const char * inputUri,
        uint16_t uriLen);
static uint8_t OCIsPacketTransferRequired(const char *request, const char *response, uint16_t size);
OCStackResult getResourceType(const char * query, unsigned char** resourceType);
static CAResult_t OCSelectNetwork();

//-----------------------------------------------------------------------------
// Public APIs
//-----------------------------------------------------------------------------

/**
 * Initialize the OC Stack.  Must be called prior to starting the stack.
 *
 * @param ipAddr
 *     IP Address of host device
 * @param port
 *     Port of host device
 * @param mode
 *     Host device is client, server, or client-server
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack init error
 */
OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
{
    OCStackResult result = OC_STACK_ERROR;
    CAResult_t caResult = CA_STATUS_OK;
    OC_LOG(INFO, TAG, PCF("Entering OCInit"));

    // Validate mode
    if (!((mode == OC_CLIENT) || (mode == OC_SERVER) || (mode == OC_CLIENT_SERVER)))
    {
        OC_LOG(ERROR, TAG, PCF("Invalid mode"));
        return OC_STACK_ERROR;
    }

    if (ipAddr)
    {
        OC_LOG_V(INFO, TAG, "IP Address = %s", ipAddr);
    }

    OCSeedRandom();
    caResult = CAInitialize();
    if(caResult == CA_STATUS_OK)
    {
        caResult = OCSelectNetwork();
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("Failed to initialize the CA Layer."));
        return CAResultToOCResult(caResult);
    }

    if(caResult == CA_STATUS_OK)
    {
        CARegisterHandler(HandleCARequests, HandleCAResponses);
        {
            OC_LOG(INFO, TAG, PCF("CARegisterHandler..."));
            stackState = OC_STACK_INITIALIZED;
            result = OC_STACK_OK;
            switch (mode)
            {
                case OC_CLIENT:
                    caResult = CAStartDiscoveryServer();
                    OC_LOG(INFO, TAG, PCF("Client mode: CAStartDiscoveryServer"));
                    break;
                case OC_SERVER:
                    caResult = CAStartListeningServer();
                    OC_LOG(INFO, TAG, PCF("Server mode: CAStartListeningServer"));
                    break;
                case OC_CLIENT_SERVER:
                    caResult = CAStartListeningServer();
                    if(caResult == CA_STATUS_OK)
                    {
                        caResult = CAStartDiscoveryServer();
                    }
                    OC_LOG(INFO, TAG, PCF("Client-server mode"));
                    break;
                default:
                    OC_LOG(ERROR, TAG, PCF("Invalid mode"));
                    return OC_STACK_ERROR;
                    break;
            }

        }

        result = CAResultToOCResult(caResult);
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("Failed to initialize any transports."));
        return CAResultToOCResult(caResult);
    }

    myStackMode = mode;
    defaultDeviceHandler = NULL;

#if defined(__WITH_DTLS__)
    caResult = CARegisterDTLSCredentialsHandler(GetDtlsPskCredentials);
    result = (caResult == CA_STATUS_OK) ? OC_STACK_OK : OC_STACK_ERROR;
#endif // (__WITH_DTLS__)

#ifdef WITH_PRESENCE
    PresenceTimeOutSize = sizeof(PresenceTimeOut)/sizeof(PresenceTimeOut[0]) - 1;
#endif // WITH_PRESENCE

    if (result == OC_STACK_OK)
    {
        stackState = OC_STACK_INITIALIZED;
    }
    // Initialize resource
    if(result == OC_STACK_OK && myStackMode != OC_CLIENT)
    {
        result = initResources();
    }
    if(result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("Stack initialization error"));
    }
    return result;
}

/**
 * Stop the OC stack.  Use for a controlled shutdown.
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack not initialized
 */
OCStackResult OCStop()
{
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering OCStop"));

    if (stackState == OC_STACK_UNINIT_IN_PROGRESS)
    {
        OC_LOG(DEBUG, TAG, PCF("Stack already stopping, exiting"));
        return OC_STACK_OK;
    }
    else if (stackState != OC_STACK_INITIALIZED)
    {
        OC_LOG(ERROR, TAG, PCF("Stack not initialized"));
        return OC_STACK_ERROR;
    }

    stackState = OC_STACK_UNINIT_IN_PROGRESS;

    #ifdef WITH_PRESENCE
    // Ensure that the TTL associated with ANY and ALL presence notifications originating from
    // here send with the code "OC_STACK_PRESENCE_STOPPED" result.
    presenceResource.presenceTTL = 0;
    #endif // WITH_PRESENCE

    // Free memory dynamically allocated for resources
    deleteAllResources();
    DeleteDeviceInfo();
    CATerminate();
    //CATerminate does not return any error code. It is OK to assign result to OC_STACK_OK.
    result = OC_STACK_OK;

    if (result == OC_STACK_OK)
    {
        // Remove all observers
        DeleteObserverList();
        // Remove all the client callbacks
        DeleteClientCBList();
        stackState = OC_STACK_UNINITIALIZED;
        result = OC_STACK_OK;
    } else {
        stackState = OC_STACK_INITIALIZED;
        result = OC_STACK_ERROR;
    }

    // Deinit security blob
    DeinitOCSecurityInfo();

    if (result != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, PCF("Stack stop error"));
    }

    return result;
}

/**
 * Map OCQualityOfService to CAMessageType
 *
 * @param OCQualityOfService - Input qos.
 *
 * Returns CA message type for a given qos.
 */
CAMessageType_t qualityOfServiceToMessageType(OCQualityOfService qos)
{
    switch (qos)
    {
        case OC_HIGH_QOS:
            return CA_MSG_CONFIRM;
        case OC_LOW_QOS:
        case OC_MEDIUM_QOS:
        case OC_NA_QOS:
        default:
            return CA_MSG_NONCONFIRM;
    }
}

/**
 * Verify the lengths of the URI and the query separately
 *
 * @param inputUri       - Input URI and query.
 * @param uriLen         - The length of the initial URI with query.
 *
 * Note: The '?' that appears after the URI is not considered as
 * a part of the query.
 */
OCStackResult verifyUriQueryLength(const char *inputUri, uint16_t uriLen)
{
    char *query;

    query = strchr (inputUri, '?');

    if (query != NULL)
    {
        if((query - inputUri) > MAX_URI_LENGTH)
        {
            return OC_STACK_INVALID_URI;
        }

        if((inputUri + uriLen - 1 - query) > MAX_QUERY_LENGTH)
        {
            return OC_STACK_INVALID_QUERY;
        }
    }
    else if(uriLen > MAX_URI_LENGTH)
    {
        return OC_STACK_INVALID_URI;
    }
    return OC_STACK_OK;
}

/**
 * Discover or Perform requests on a specified resource
 * (specified by that Resource's respective URI).
 *
 * @param handle             - @ref OCDoHandle to refer to the request sent out on behalf of
 *                             calling this API.
 * @param method             - @ref OCMethod to perform on the resource
 * @param requiredUri        - URI of the resource to interact with
 * @param referenceUri       - URI of the reference resource
 * @param request            - JSON encoded request
 * @param qos                - quality of service
 * @param cbData             - struct that contains asynchronous callback function that is invoked
 *                             by the stack when discovery or resource interaction is complete
 * @param options            - The address of an array containing the vendor specific header
 *                             header options to be sent with the request
 * @param numOptions         - Number of vendor specific header options to be included
 *
 * @return
 *     OC_STACK_OK               - no errors
 *     OC_STACK_INVALID_CALLBACK - invalid callback function pointer
 *     OC_STACK_INVALID_METHOD   - invalid resource method
 *     OC_STACK_INVALID_URI      - invalid required or reference URI
 *
 * Note: IN case of CA, when using multicast, the required URI should not contain IP address.
 *       Instead, it just contains the URI to the resource such as "/oc/core".
 */
OCStackResult OCDoResource(OCDoHandle *handle, OCMethod method, const char *requiredUri,
            const char *referenceUri, const char *request, OCConnectivityType conType,
            OCQualityOfService qos, OCCallbackData *cbData,
            OCHeaderOption * options, uint8_t numOptions)
{
    OCStackResult result = OC_STACK_ERROR;
    ClientCB *clientCB = NULL;
    unsigned char * requestUri = NULL;
    unsigned char * resourceType = NULL;
    unsigned char * query = NULL;
    char * newUri = (char *)requiredUri;
    (void) referenceUri;
    CARemoteEndpoint_t* endpoint = NULL;
    CAResult_t caResult;
    CAToken_t token = NULL;
    CAInfo_t requestData;
    CARequestInfo_t requestInfo;
    CAGroupEndpoint_t grpEnd = {0};
    OCDoHandle resHandle = NULL;

    // To track if memory is allocated for additional header options
    uint8_t hdrOptionMemAlloc = 0;

    OC_LOG(INFO, TAG, PCF("Entering OCDoResource"));

    // Validate input parameters
    VERIFY_NON_NULL(cbData, FATAL, OC_STACK_INVALID_CALLBACK);
    VERIFY_NON_NULL(cbData->cb, FATAL, OC_STACK_INVALID_CALLBACK);

    //TODO ("Need to form the final query by concatenating require and reference URI's");
    VERIFY_NON_NULL(requiredUri, FATAL, OC_STACK_INVALID_URI);

    uint16_t uriLen = strlen(requiredUri);

    // ToDo: We should also check if the requiredUri has a mutlicast address,
    // then qos has to be OC_Low_QOS
    switch (method)
    {
        case OC_REST_GET:
        case OC_REST_PUT:
        case OC_REST_POST:
        case OC_REST_DELETE:
        case OC_REST_OBSERVE:
        case OC_REST_OBSERVE_ALL:
        case OC_REST_CANCEL_OBSERVE:
            break;
        #ifdef WITH_PRESENCE
        case OC_REST_PRESENCE:
            break;
        #endif
        default:
            result = OC_STACK_INVALID_METHOD;
            goto exit;
    }

    if((result = verifyUriQueryLength(requiredUri, uriLen)) != OC_STACK_OK)
    {
        goto exit;
    }

    if((request) && (strlen(request) > MAX_REQUEST_LENGTH))
    {
        result = OC_STACK_INVALID_PARAM;
        goto exit;
    }

#ifdef WITH_PRESENCE
    if(method == OC_REST_PRESENCE)
    {
        result = getQueryFromUri(requiredUri, &query, &newUri);
        if(query)
        {
            result = getResourceType((char *) query, &resourceType);
            if(resourceType)
            {
                OC_LOG_V(DEBUG, TAG, "Got Resource Type: %s", resourceType);
            }
            else
            {
                OC_LOG(DEBUG, TAG, PCF("Resource type is NULL."));
            }
        }
        else
        {
            OC_LOG(DEBUG, TAG, PCF("Query string is NULL."));
        }
        if(result != OC_STACK_OK)
        {
            goto exit;
        }
    }
#endif // WITH_PRESENCE

    requestUri = (unsigned char *) OCMalloc(uriLen + 1);
    if(requestUri)
    {
        memcpy(requestUri, newUri, (uriLen + 1));
    }
    else
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    resHandle = GenerateInvocationHandle();
    if(!resHandle)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    memset(&requestData, 0, sizeof(CAInfo_t));
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    switch (method)
    {
        case OC_REST_GET:
        case OC_REST_OBSERVE:
        case OC_REST_OBSERVE_ALL:
        case OC_REST_CANCEL_OBSERVE:
            {
                requestInfo.method = CA_GET;
                break;
            }
        case OC_REST_PUT:
            {
                requestInfo.method = CA_PUT;
                break;
            }
        case OC_REST_POST:
            {
                requestInfo.method = CA_POST;
                break;
            }
        case OC_REST_DELETE:
            {
                requestInfo.method = CA_DELETE;
                break;
            }
        #ifdef WITH_PRESENCE
        case OC_REST_PRESENCE:
            {
                // Replacing method type with GET because "presence"
                // is a stack layer only implementation.
                requestInfo.method = CA_GET;
                break;
            }
        #endif
        default:
            result = OC_STACK_INVALID_METHOD;
            goto exit;
    }

    //High QoS is not supported
    if(qos == OC_HIGH_QOS)
    {
        result = OC_STACK_INVALID_PARAM;
        goto exit;
    }

    // create token
    caResult = CAGenerateToken(&token);
    if (caResult != CA_STATUS_OK)
    {
        OC_LOG(ERROR, TAG, PCF("CAGenerateToken error"));
        CADestroyToken(token);
        goto exit;
    }

    requestData.type = qualityOfServiceToMessageType(qos);
    requestData.token = token;
    if ((method == OC_REST_OBSERVE) || (method == OC_REST_OBSERVE_ALL))
    {
        result = CreateObserveHeaderOption (&(requestData.options), options,
                                    numOptions, OC_OBSERVE_REGISTER);
        if (result != OC_STACK_OK)
        {
            goto exit;
        }
        hdrOptionMemAlloc = 1;
        requestData.numOptions = numOptions + 1;
    }
    else
    {
        requestData.options = (CAHeaderOption_t*)options;
        requestData.numOptions = numOptions;
    }
    requestData.payload = (char *)request;

    requestInfo.info = requestData;

    CAConnectivityType_t caConType;

    result = OCToCAConnectivityType((OCConnectivityType) conType, &caConType);
    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("Invalid Connectivity Type"));
        goto exit;
    }

    // send request
    if(conType == OC_ALL)
    {
        grpEnd.connectivityType = caConType;

        grpEnd.resourceUri = (CAURI_t) OCMalloc(uriLen + 1);
        if(!grpEnd.resourceUri)
        {
            result = OC_STACK_NO_MEMORY;
            goto exit;
        }
        strncpy(grpEnd.resourceUri, requiredUri, (uriLen + 1));

        caResult = CASendRequestToAll(&grpEnd, &requestInfo);
    }
    else
    {
        caResult = CACreateRemoteEndpoint(newUri, caConType, &endpoint);

        if (caResult != CA_STATUS_OK)
        {
            OC_LOG(ERROR, TAG, PCF("CACreateRemoteEndpoint error"));
            goto exit;
        }

        caResult = CASendRequest(endpoint, &requestInfo);
    }

    if (caResult != CA_STATUS_OK)
    {
        OC_LOG(ERROR, TAG, PCF("CASendRequest"));
        goto exit;
    }

    if((result = AddClientCB(&clientCB, cbData, &token, &resHandle, method,
                             requestUri, resourceType)) != OC_STACK_OK)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    if(handle)
    {
        *handle = resHandle;
    }

exit:
    if(newUri != requiredUri)
    {
        OCFree(newUri);
    }
    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCDoResource error"));
        FindAndDeleteClientCB(clientCB);
        OCFree(resHandle);
    }
    CADestroyRemoteEndpoint(endpoint);
    OCFree(grpEnd.resourceUri);

    if (hdrOptionMemAlloc)
    {
        OCFree(requestData.options);
    }
    return result;
}

/**
 * Cancel a request associated with a specific @ref OCDoResource invocation.
 *
 * @param handle - Used to identify a specific OCDoResource invocation.
 * @param qos    - used to specify Quality of Service (read below for more info)
 * @param options- used to specify vendor specific header options when sending
 *                 explicit observe cancellation
 * @param numOptions- Number of header options to be included
 *
 * @return
 *     OC_STACK_OK               - No errors; Success
 *     OC_STACK_INVALID_PARAM    - The handle provided is invalid.
 */
OCStackResult OCCancel(OCDoHandle handle, OCQualityOfService qos, OCHeaderOption * options,
        uint8_t numOptions)
{
    /*
     * This ftn is implemented one of two ways in the case of observation:
     *
     * 1. qos == OC_NON_CONFIRMABLE. When observe is unobserved..
     *      Remove the callback associated on client side.
     *      When the next notification comes in from server,
     *      reply with RESET message to server.
     *      Keep in mind that the server will react to RESET only
     *      if the last notification was sent ans CON
     *
     * 2. qos == OC_CONFIRMABLE. When OCCancel is called,
     *      and it is associated with an observe request
     *      (i.e. ClientCB->method == OC_REST_OBSERVE || OC_REST_OBSERVE_ALL),
     *      Send CON Observe request to server with
     *      observe flag = OC_RESOURCE_OBSERVE_DEREGISTER.
     *      Remove the callback associated on client side.
     */
    OCStackResult ret = OC_STACK_OK;
    CARemoteEndpoint_t* endpoint = NULL;
    CAResult_t caResult;
    CAInfo_t requestData;
    CARequestInfo_t requestInfo;
    // Track if memory is allocated for additional header options
    uint8_t hdrOptionMemAlloc = 0;

    if(!handle) {
        return OC_STACK_INVALID_PARAM;
    }

    OC_LOG(INFO, TAG, PCF("Entering OCCancel"));

    ClientCB *clientCB = GetClientCB(NULL, handle, NULL);

    if(clientCB) {
        switch (clientCB->method)
        {
            case OC_REST_OBSERVE:
            case OC_REST_OBSERVE_ALL:
                //TODO-CA : Why CA_WIFI alone?
                caResult = CACreateRemoteEndpoint((char *)clientCB->requestUri, CA_WIFI,
                                                  &endpoint);
                if (caResult != CA_STATUS_OK)
                {
                    OC_LOG(ERROR, TAG, PCF("CACreateRemoteEndpoint error"));
                    return OC_STACK_ERROR;
                }

                memset(&requestData, 0, sizeof(CAInfo_t));
                requestData.type =  qualityOfServiceToMessageType(qos);
                requestData.token = clientCB->token;
                if (CreateObserveHeaderOption (&(requestData.options),
                            options, numOptions, OC_OBSERVE_DEREGISTER) != OC_STACK_OK)
                {
                    return OC_STACK_ERROR;
                }
                hdrOptionMemAlloc = 1;
                requestData.numOptions = numOptions + 1;
                memset(&requestInfo, 0, sizeof(CARequestInfo_t));
                requestInfo.method = CA_GET;
                requestInfo.info = requestData;
                // send request
                caResult = CASendRequest(endpoint, &requestInfo);
                if (caResult != CA_STATUS_OK)
                {
                    OC_LOG(ERROR, TAG, PCF("CASendRequest error"));
                }
                if(caResult == CA_STATUS_OK)
                {
                    ret = OC_STACK_OK;
                }
            #ifdef WITH_PRESENCE
            case OC_REST_PRESENCE:
                FindAndDeleteClientCB(clientCB);
                break;
            #endif
            default:
                return OC_STACK_INVALID_METHOD;
        }
    }
    CADestroyRemoteEndpoint(endpoint);
    if (hdrOptionMemAlloc)
    {
        OCFree(requestData.options);
    }

    return ret;
}

#ifdef WITH_PRESENCE
OCStackResult OCProcessPresence()
{
    OCStackResult result = OC_STACK_OK;
    uint8_t ipAddr[4] = { 0 };
    uint16_t port = 0;

    OC_LOG(INFO, TAG, PCF("Entering RequestPresence"));
    ClientCB* cbNode = NULL;
    OCDevAddr dst;
    OCClientResponse clientResponse;
    OCResponse * response = NULL;
    OCStackApplicationResult cbResult = OC_STACK_DELETE_TRANSACTION;

    LL_FOREACH(cbList, cbNode) {
        if(OC_REST_PRESENCE == cbNode->method)
        {
            if(cbNode->presence)
            {
                uint32_t now = GetTime(0);
                OC_LOG_V(DEBUG, TAG, "----------------this TTL level %d",
                                                        cbNode->presence->TTLlevel);
                OC_LOG_V(DEBUG, TAG, "----------------current ticks %d", now);


                if(cbNode->presence->TTLlevel >= (PresenceTimeOutSize + 1))
                {
                    goto exit;
                }

                if(cbNode->presence->TTLlevel < PresenceTimeOutSize){
                    OC_LOG_V(DEBUG, TAG, "----------------timeout ticks %d",
                            cbNode->presence->timeOut[cbNode->presence->TTLlevel]);
                }

                if(cbNode->presence->TTLlevel >= PresenceTimeOutSize)
                {
                    OC_LOG(DEBUG, TAG, PCF("----------------No more timeout ticks"));
                    if (ParseIPv4Address( cbNode->requestUri, ipAddr, &port))
                    {
                        OCBuildIPv4Address(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3], port,
                                &dst);

                        clientResponse.sequenceNumber = 0;
                        clientResponse.result = OC_STACK_PRESENCE_TIMEOUT;
                        clientResponse.addr = (OCDevAddr *) &dst;
                        clientResponse.resJSONPayload = NULL;

                        result = FormOCResponse(&response, cbNode, 0, NULL, NULL,
                                &cbNode->token, &clientResponse, NULL);
                        if(result != OC_STACK_OK)
                        {
                            goto exit;
                        }

                        // Increment the TTLLevel (going to a next state), so we don't keep
                        // sending presence notification to client.
                        cbNode->presence->TTLlevel++;
                        OC_LOG_V(DEBUG, TAG, "----------------moving to TTL level %d",
                                                cbNode->presence->TTLlevel);
                    }
                    else
                    {
                        result = OC_STACK_INVALID_IP;
                        goto exit;
                    }

                    cbResult = cbNode->callBack(cbNode->context, cbNode->handle, &clientResponse);
                    if (cbResult == OC_STACK_DELETE_TRANSACTION)
                    {
                        FindAndDeleteClientCB(cbNode);
                    }
                }

                if(now >= cbNode->presence->timeOut[cbNode->presence->TTLlevel])
                {
                    CAResult_t caResult;
                    CARemoteEndpoint_t* endpoint = NULL;
                    CAInfo_t requestData;
                    CARequestInfo_t requestInfo;

                    OC_LOG(DEBUG, TAG, PCF("time to test server presence =========="));

                    //TODO-CA : Why CA_WIFI alone?
                    caResult = CACreateRemoteEndpoint((char *)cbNode->requestUri, CA_WIFI,
                                                  &endpoint);

                    if (caResult != CA_STATUS_OK)
                    {
                        OC_LOG(ERROR, TAG, PCF("CACreateRemoteEndpoint error"));
                        goto exit;
                    }

                    memset(&requestData, 0, sizeof(CAInfo_t));
                    requestData.type = CA_MSG_NONCONFIRM;
                    requestData.token = cbNode->token;

                    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
                    requestInfo.method = CA_GET;
                    requestInfo.info = requestData;

                    caResult = CASendRequest(endpoint, &requestInfo);

                    if (caResult != CA_STATUS_OK)
                    {
                        OC_LOG(ERROR, TAG, PCF("CASendRequest error"));
                        goto exit;
                    }

                    cbNode->presence->TTLlevel++;
                    OC_LOG_V(DEBUG, TAG, "----------------moving to TTL level %d",
                                                            cbNode->presence->TTLlevel);
                }
            }
        }
    }
exit:
    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCProcessPresence error"));
    }
    return result;
}
#endif // WITH_PRESENCE

/**
 * Called in main loop of OC client or server.  Allows low-level processing of
 * stack services.
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCProcess()
{
    #ifdef WITH_PRESENCE
    OCProcessPresence();
    #endif
    CAHandleRequestResponse();

    return OC_STACK_OK;
}

#ifdef WITH_PRESENCE
/**
 * When operating in @ref OCServer or @ref OCClientServer mode, this API will start sending out
 * presence notifications to clients via multicast. Once this API has been called with a success,
 * clients may query for this server's presence and this server's stack will respond via multicast.
 *
 * Server can call this function when it comes online for the first time, or when it comes back
 * online from offline mode, or when it re enters network.
 *
 * @param ttl - Time To Live in seconds
 * Note: If ttl is '0', then the default stack value will be used (60 Seconds).
 *
 * @return
 *     OC_STACK_OK      - No errors; Success
 */
OCStackResult OCStartPresence(const uint32_t ttl)
{
    OCChangeResourceProperty(
            &(((OCResource *)presenceResource.handle)->resourceProperties),
            OC_ACTIVE, 1);

    if(ttl > 0)
    {
        presenceResource.presenceTTL = ttl;
    }

    if(OC_PRESENCE_UNINITIALIZED == presenceState)
    {
        presenceState = OC_PRESENCE_INITIALIZED;

        CAAddress_t addressInfo;
        strncpy(addressInfo.IP.ipAddress, "224.0.1.187", CA_IPADDR_SIZE);
        addressInfo.IP.port = 5683;

        //TODO make sure there is no memory leak here since another copy
        //of token is being created inside AddObserver
        CAToken_t caToken = NULL;
        CAResult_t caResult = CAGenerateToken(&caToken);
        if (caResult != CA_STATUS_OK)
        {
            OC_LOG(ERROR, TAG, PCF("CAGenerateToken error"));
            CADestroyToken(caToken);
            return OC_STACK_ERROR;
        }

        CAConnectivityType_t connType;
        OCToCAConnectivityType(OC_ALL, &connType );
        AddObserver(OC_PRESENCE_URI, NULL, 0, &caToken,
                (OCResource *)presenceResource.handle, OC_LOW_QOS,
                &addressInfo, connType);
    }

    // Each time OCStartPresence is called
    // a different random 32-bit integer number is used
    ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();

    return SendPresenceNotification(NULL);
}

/**
 * When operating in @ref OCServer or @ref OCClientServer mode, this API will stop sending out
 * presence notifications to clients via multicast. Once this API has been called with a success,
 * this server's stack will not respond to clients querying for this server's presence.
 *
 * Server can call this function when it is terminating, going offline, or when going
 * away from network.
 *
 * @return
 *     OC_STACK_OK      - No errors; Success
 */
OCStackResult OCStopPresence()
{
    OCStackResult result = OC_STACK_ERROR;

    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
    }

    // make resource inactive
    result = OCChangeResourceProperty(
            &(((OCResource *) presenceResource.handle)->resourceProperties),
            OC_ACTIVE, 0);

    if(result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG,
                      PCF("Changing the presence resource properties to ACTIVE not successful"));
        return result;
    }

    return SendStopNotification();
}
#endif


OCStackResult OCSetDefaultDeviceEntityHandler(OCDeviceEntityHandler entityHandler)
{
    defaultDeviceHandler = entityHandler;

    return OC_STACK_OK;
}

OCStackResult OCSetDeviceInfo(OCDeviceInfo deviceInfo)
{
    OC_LOG(INFO, TAG, PCF("Entering OCSetDeviceInfo"));

    if(myStackMode == OC_CLIENT)
    {
        return OC_STACK_ERROR;
    }

    return SaveDeviceInfo(deviceInfo);
}

/**
 * Create a resource
 *
 * @param handle - pointer to handle to newly created resource.  Set by ocstack.
 *                 Used to refer to resource
 * @param resourceTypeName - name of resource type.  Example: "core.led"
 * @param resourceInterfaceName - name of resource interface.  Example: "core.rw"
 * @param uri - URI of the resource.  Example:  "/a/led"
 * @param entityHandler - entity handler function that is called by ocstack to handle requests, etc
 *                        NULL for default entity handler
 * @param resourceProperties - properties supported by resource.
 *                             Example: OC_DISCOVERABLE|OC_OBSERVABLE
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCCreateResource(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceInterfaceName,
        const char *uri, OCEntityHandler entityHandler,
        uint8_t resourceProperties)
{

    OCResource *pointer = NULL;
    char *str = NULL;
    size_t size;
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering OCCreateResource"));

    if(myStackMode == OC_CLIENT)
    {
        return result;
    }
    // Validate parameters
    if(!uri || (strlen(uri) == 0))
    {
        OC_LOG(ERROR, TAG, PCF("URI is invalid"));
        return OC_STACK_INVALID_URI;
    }
    // Is it presented during resource discovery?
    if (!handle || !resourceTypeName)
    {
        OC_LOG(ERROR, TAG, PCF("Input parameter is NULL"));
        return OC_STACK_INVALID_PARAM;
    }

    if(!resourceInterfaceName || strlen(resourceInterfaceName) == 0) {
        resourceInterfaceName = OC_RSRVD_INTERFACE_DEFAULT;
    }

    // Make sure resourceProperties bitmask has allowed properties specified
    if (resourceProperties
            > (OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | OC_SLOW | OC_SECURE)) {
        OC_LOG(ERROR, TAG, PCF("Invalid property"));
        return OC_STACK_INVALID_PARAM;
    }

    // If the headResource is NULL, then no resources have been created...
    pointer = headResource;
    if (pointer)
    {
        // At least one resources is in the resource list, so we need to search for
        // repeated URLs, which are not allowed.  If a repeat is found, exit with an error
        while (pointer)
        {
            if (strcmp(uri, pointer->uri) == 0)
            {
                OC_LOG(ERROR, TAG, PCF("URI already in use"));
                return OC_STACK_INVALID_PARAM;
            }
            pointer = pointer->next;
        }
    }
    // Create the pointer and insert it into the resource list
    pointer = (OCResource *) OCCalloc(1, sizeof(OCResource));
    if (!pointer)
    {
        goto exit;
    }
    pointer->sequenceNum = OC_OFFSET_SEQUENCE_NUMBER;

    insertResource(pointer);

    // Set the uri
    size = strlen(uri) + 1;
    str = (char *) OCMalloc(size);
    if (!str)
    {
        goto exit;
    }
    strncpy(str, uri, size);
    pointer->uri = str;

    // Set properties.  Set OC_ACTIVE
    pointer->resourceProperties = (OCResourceProperty) (resourceProperties
            | OC_ACTIVE);

    // Add the resourcetype to the resource
    result = BindResourceTypeToResource(pointer, resourceTypeName);
    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("Error adding resourcetype"));
        goto exit;
    }

    // Add the resourceinterface to the resource
    result = BindResourceInterfaceToResource(pointer, resourceInterfaceName);
    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("Error adding resourceinterface"));
        goto exit;
    }

    // If an entity handler has been passed, attach it to the newly created
    // resource.  Otherwise, set the default entity handler.
    if (entityHandler)
    {
        pointer->entityHandler = entityHandler;
    }
    else
    {
        pointer->entityHandler = defaultResourceEHandler;
    }

    *handle = pointer;
    result = OC_STACK_OK;

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(pointer->rsrcType);
    }
    #endif
exit:
    if (result != OC_STACK_OK)
    {
        // Deep delete of resource and other dynamic elements that it contains
        deleteResource(pointer);
        OCFree(str);
    }
    return result;
}



/**
 * Create a resource. with host ip address for remote resource
 *
 * @param handle - pointer to handle to newly created resource.  Set by ocstack.
 *                 Used to refer to resource
 * @param resourceTypeName - name of resource type.  Example: "core.led"
 * @param resourceInterfaceName - name of resource interface.  Example: "core.rw"
 * @param host - HOST address of the remote resource.  Example:  "coap://xxx.xxx.xxx.xxx:xxxxx"
 * @param uri - URI of the resource.  Example:  "/a/led"
 * @param entityHandler - entity handler function that is called by ocstack to handle requests, etc
 *                        NULL for default entity handler
 * @param resourceProperties - properties supported by resource.
 *                             Example: OC_DISCOVERABLE|OC_OBSERVABLE
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */

OCStackResult OCCreateResourceWithHost(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceInterfaceName,
        const char *host,
        const char *uri,
        OCEntityHandler entityHandler,
        uint8_t resourceProperties)
{
    char *str = NULL;
    size_t size;
    OCStackResult result = OC_STACK_ERROR;

    result = OCCreateResource(handle, resourceTypeName, resourceInterfaceName,
                                uri, entityHandler, resourceProperties);

    if (result != OC_STACK_ERROR)
    {
        // Set the uri
        size = strlen(host) + 1;
        str = (char *) OCMalloc(size);
        if (!str)
        {
            return OC_STACK_ERROR;
        }
        strncpy(str, host, size);
        ((OCResource *) *handle)->host = str;
    }

    return result;
}

/**
 * Add a resource to a collection resource.
 *
 * @param collectionHandle - handle to the collection resource
 * @param resourceHandle - handle to resource to be added to the collection resource
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 *     OC_STACK_INVALID_PARAM - invalid collectionhandle
 */
OCStackResult OCBindResource(
        OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
{
    OCResource *resource;
    uint8_t i;

    OC_LOG(INFO, TAG, PCF("Entering OCBindResource"));

    // Validate parameters
    VERIFY_NON_NULL(collectionHandle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(resourceHandle, ERROR, OC_STACK_ERROR);
    // Container cannot contain itself
    if (collectionHandle == resourceHandle)
    {
        OC_LOG(ERROR, TAG, PCF("Added handle equals collection handle"));
        return OC_STACK_INVALID_PARAM;
    }

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *) collectionHandle);
    if (!resource)
    {
        OC_LOG(ERROR, TAG, PCF("Collection handle not found"));
        return OC_STACK_INVALID_PARAM;
    }

    // Look for an open slot to add add the child resource.
    // If found, add it and return success
    for (i = 0; i < MAX_CONTAINED_RESOURCES; i++)
    {
        if (!resource->rsrcResources[i])
        {
            resource->rsrcResources[i] = (OCResource *) resourceHandle;
            OC_LOG(INFO, TAG, PCF("resource bound"));
            return OC_STACK_OK;
        }
    }

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(((OCResource *) resourceHandle)->rsrcType);
    }
    #endif

    // Unable to add resourceHandle, so return error
    return OC_STACK_ERROR;
}

/**
 * Remove a resource from a collection resource.
 *
 * @param collectionHandle - handle to the collection resource
 * @param resourceHandle - handle to resource to be added to the collection resource
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 *     OC_STACK_INVALID_PARAM - invalid collectionHandle
 */
OCStackResult OCUnBindResource(
        OCResourceHandle collectionHandle, OCResourceHandle resourceHandle) {
    OCResource *resource;
    uint8_t i;

    OC_LOG(INFO, TAG, PCF("Entering OCUnBindResource"));

    // Validate parameters
    VERIFY_NON_NULL(collectionHandle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(resourceHandle, ERROR, OC_STACK_ERROR);
    // Container cannot contain itself
    if (collectionHandle == resourceHandle) {
        OC_LOG(ERROR, TAG, PCF("removing handle equals collection handle"));
        return OC_STACK_INVALID_PARAM;
    }

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *) collectionHandle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Collection handle not found"));
        return OC_STACK_INVALID_PARAM;
    }

    // Look for an open slot to add add the child resource.
    // If found, add it and return success
    for (i = 0; i < MAX_CONTAINED_RESOURCES; i++) {
        if (resourceHandle == resource->rsrcResources[i]) {
            resource->rsrcResources[i] = (OCResource *) NULL;
            OC_LOG(INFO, TAG, PCF("resource unbound"));

            // Send notification when resource is unbounded successfully.
            #ifdef WITH_PRESENCE
            if(presenceResource.handle)
            {
                ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
                SendPresenceNotification(((OCResource *) resourceHandle)->rsrcType);
            }
            #endif

            return OC_STACK_OK;
        }
    }

    OC_LOG(INFO, TAG, PCF("resource not found in collection"));

    // Unable to add resourceHandle, so return error
    return OC_STACK_ERROR;
}

OCStackResult BindResourceTypeToResource(OCResource* resource,
                                            const char *resourceTypeName)
{
    OCResourceType *pointer = NULL;
    char *str = NULL;
    size_t size;
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering BindResourceTypeToResource"));

    // Validate parameters
    VERIFY_NON_NULL(resourceTypeName, ERROR, OC_STACK_INVALID_PARAM);
    // TODO:  Does resource attribute resentation really have to be maintained in stack?
    // Is it presented during resource discovery?

    //TODO ("Make sure that the resourcetypename doesn't already exist in the resource");

    // Create the resourcetype and insert it into the resource list
    pointer = (OCResourceType *) OCCalloc(1, sizeof(OCResourceType));
    if (!pointer) {
        goto exit;
    }

    // Set the resourceTypeName
    size = strlen(resourceTypeName) + 1;
    str = (char *) OCMalloc(size);
    if (!str) {
        goto exit;
    }
    strncpy(str, resourceTypeName, size);
    pointer->resourcetypename = str;

    insertResourceType(resource, pointer);
    result = OC_STACK_OK;

    exit: if (result != OC_STACK_OK) {
        OCFree(pointer);
        OCFree(str);
    }

    return result;
}

OCStackResult BindResourceInterfaceToResource(OCResource* resource,
        const char *resourceInterfaceName)
{
    OCResourceInterface *pointer = NULL;
    char *str = NULL;
    size_t size;
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering BindResourceInterfaceToResource"));

    // Validate parameters
    VERIFY_NON_NULL(resourceInterfaceName, ERROR, OC_STACK_INVALID_PARAM);

    //TODO ("Make sure that the resourceinterface name doesn't already exist in the resource");

    // Create the resourceinterface and insert it into the resource list
    pointer = (OCResourceInterface *) OCCalloc(1, sizeof(OCResourceInterface));
    if (!pointer) {
        goto exit;
    }

    // Set the resourceinterface name
    size = strlen(resourceInterfaceName) + 1;
    str = (char *) OCMalloc(size);
    if (!str) {
        goto exit;
    }
    strncpy(str, resourceInterfaceName, size);
    pointer->name = str;

    // Bind the resourceinterface to the resource
    insertResourceInterface(resource, pointer);

    result = OC_STACK_OK;

    exit: if (result != OC_STACK_OK) {
        OCFree(pointer);
        OCFree(str);
    }

    return result;
}

/**
 * Bind a resourcetype to a resource.
 *
 * @param handle - handle to the resource
 * @param resourceTypeName - name of resource type.  Example: "core.led"
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCBindResourceTypeToResource(OCResourceHandle handle,
        const char *resourceTypeName) {

    OCStackResult result = OC_STACK_ERROR;
    OCResource *resource;

    // Make sure resource exists
    resource = findResource((OCResource *) handle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_ERROR;
    }

    // call internal function
    result = BindResourceTypeToResource(resource, resourceTypeName);

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(resource->rsrcType);
    }
    #endif

    return result;
}

/**
 * Bind a resourceinterface to a resource.
 *
 * @param handle - handle to the resource
 * @param resourceInterfaceName - name of resource interface.  Example: "oc.mi.b"
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */

OCStackResult OCBindResourceInterfaceToResource(OCResourceHandle handle,
        const char *resourceInterfaceName) {

    OCStackResult result = OC_STACK_ERROR;
    OCResource *resource;

    // Make sure resource exists
    resource = findResource((OCResource *) handle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_ERROR;
    }

    // call internal function
    result = BindResourceInterfaceToResource(resource, resourceInterfaceName);

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(resource->rsrcType);
    }
    #endif

    return result;
}

/**
 * Get the number of resources that have been created in the stack.
 *
 * @param numResources - pointer to count variable
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error

 */
OCStackResult OCGetNumberOfResources(uint8_t *numResources) {
    OCResource *pointer = headResource;

    OC_LOG(INFO, TAG, PCF("Entering OCGetNumberOfResources"));
    VERIFY_NON_NULL(numResources, ERROR, OC_STACK_INVALID_PARAM);
    *numResources = 0;
    while (pointer) {
        *numResources = *numResources + 1;
        pointer = pointer->next;
    }
    return OC_STACK_OK;
}

/**
 * Get a resource handle by index.
 *
 * @param index - index of resource, 0 to Count - 1
 *
 * @return
 *    Resource handle - if found
 *    NULL - if not found
 */
OCResourceHandle OCGetResourceHandle(uint8_t index) {
    OCResource *pointer = headResource;
    uint8_t i = 0;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceHandle"));

    // Iterate through the list
    while ((i < index) && pointer) {
        i++;
        pointer = pointer->next;
    }
    return (OCResourceHandle) pointer;
}

/**
 * Delete resource specified by handle.  Deletes resource and all resourcetype and resourceinterface
 * linked lists.
 *
 * @param handle - handle of resource to be deleted
 *
 * @return
 *     OC_STACK_OK              - no errors
 *     OC_STACK_ERROR           - stack process error
 *     OC_STACK_NO_RESOURCE     - resource not found
 *     OC_STACK_INVALID_PARAM   - invalid param
 */
OCStackResult OCDeleteResource(OCResourceHandle handle) {
    OC_LOG(INFO, TAG, PCF("Entering OCDeleteResource"));

    if (!handle) {
        OC_LOG(ERROR, TAG, PCF("Invalid param"));
        return OC_STACK_INVALID_PARAM;
    }

    OCResource *resource = findResource((OCResource *) handle);
    if (resource == NULL) {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_NO_RESOURCE;
    }

    if (deleteResource((OCResource *) handle) == 0) {
        OC_LOG(ERROR, TAG, PCF("Error deleting resource"));
        return OC_STACK_ERROR;
    }

    return OC_STACK_OK;
}

/**
 * Get the URI of the resource specified by handle.
 *
 * @param handle - handle of resource
 * @return
 *    URI string - if resource found
 *    NULL - resource not found
 */
const char *OCGetResourceUri(OCResourceHandle handle) {
    OCResource *resource;
    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceUri"));

    resource = findResource((OCResource *) handle);
    if (resource) {
        return resource->uri;
    }
    return (const char *) NULL;
}

/**
 * Get the properties of the resource specified by handle.
 * NOTE: that after a resource is created, the OC_ACTIVE property is set
 * for the resource by the stack.
 *
 * @param handle - handle of resource
 * @return
 *    property bitmap - if resource found
 *    NULL - resource not found
 */
uint8_t OCGetResourceProperties(OCResourceHandle handle) {
    OCResource *resource;
    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceProperties"));

    resource = findResource((OCResource *) handle);
    if (resource) {
        return resource->resourceProperties;
    }
    return 0;
}

/**
 * Get the number of resource types of the resource.
 *
 * @param handle - handle of resource
 * @param numResourceTypes - pointer to count variable
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCGetNumberOfResourceTypes(OCResourceHandle handle,
        uint8_t *numResourceTypes) {
    OCResource *resource;
    OCResourceType *pointer;

    OC_LOG(INFO, TAG, PCF("Entering OCGetNumberOfResourceTypes"));
    VERIFY_NON_NULL(numResourceTypes, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);

    *numResourceTypes = 0;

    resource = findResource((OCResource *) handle);
    if (resource) {
        pointer = resource->rsrcType;
        while (pointer) {
            *numResourceTypes = *numResourceTypes + 1;
            pointer = pointer->next;
        }
    }
    return OC_STACK_OK;
}

/**
 * Get name of resource type of the resource.
 *
 * @param handle - handle of resource
 * @param index - index of resource, 0 to Count - 1
 *
 * @return
 *    resource type name - if resource found
 *    NULL - resource not found
 */
const char *OCGetResourceTypeName(OCResourceHandle handle, uint8_t index) {
    OCResourceType *resourceType;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceTypeName"));

    resourceType = findResourceTypeAtIndex(handle, index);
    if (resourceType) {
        return resourceType->resourcetypename;
    }
    return (const char *) NULL;
}



/**
 * Get the number of resource interfaces of the resource.
 *
 * @param handle - handle of resource
 * @param numResources - pointer to count variable
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCGetNumberOfResourceInterfaces(OCResourceHandle handle,
        uint8_t *numResourceInterfaces) {
    OCResourceInterface *pointer;
    OCResource *resource;

    OC_LOG(INFO, TAG, PCF("Entering OCGetNumberOfResourceInterfaces"));

    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(numResourceInterfaces, ERROR, OC_STACK_INVALID_PARAM);

    *numResourceInterfaces = 0;
    resource = findResource((OCResource *) handle);
    if (resource) {
        pointer = resource->rsrcInterface;
        while (pointer) {
            *numResourceInterfaces = *numResourceInterfaces + 1;
            pointer = pointer->next;
        }
    }
    return OC_STACK_OK;
}

/**
 * Get name of resource interface of the resource.
 *
 * @param handle - handle of resource
 * @param index - index of resource, 0 to Count - 1
 *
 * @return
 *    resource interface name - if resource found
 *    NULL - resource not found
 */
const char *OCGetResourceInterfaceName(OCResourceHandle handle, uint8_t index) {
    OCResourceInterface *resourceInterface;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceInterfaceName"));

    resourceInterface = findResourceInterfaceAtIndex(handle, index);
    if (resourceInterface) {
        return resourceInterface->name;
    }
    return (const char *) NULL;
}

/**
 * Get resource handle from the collection resource by index.
 *
 * @param collectionHandle - handle of collection resource
 * @param index - index of contained resource, 0 to Count - 1
 *
 * @return
 *    handle to resource - if resource found
 *    NULL - resource not found
 */
OCResourceHandle OCGetResourceHandleFromCollection(OCResourceHandle collectionHandle,
        uint8_t index) {
    OCResource *resource;

    OC_LOG(INFO, TAG, PCF("Entering OCGetContainedResource"));

    if (index >= MAX_CONTAINED_RESOURCES) {
        return NULL;
    }

    resource = findResource((OCResource *) collectionHandle);
    if (!resource) {
        return NULL;
    }

    return resource->rsrcResources[index];
}

/**
 * Bind an entity handler to the resource.
 *
 * @param handle - handle to the resource that the contained resource is to be bound
 * @param entityHandler - entity handler function that is called by ocstack to handle requests, etc
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCBindResourceHandler(OCResourceHandle handle,
        OCEntityHandler entityHandler) {
    OCResource *resource;

    OC_LOG(INFO, TAG, PCF("Entering OCBindResourceHandler"));

    // Validate parameters
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);
    //VERIFY_NON_NULL(entityHandler, ERROR, OC_STACK_INVALID_PARAM);

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *)handle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_ERROR;
    }

    // Bind the handler
    resource->entityHandler = entityHandler;

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(resource->rsrcType);
    }
    #endif

    return OC_STACK_OK;
}

/**
 * Get the entity handler for a resource.
 *
 * @param handle - handle of resource
 *
 * @return
 *    entity handler - if resource found
 *    NULL - resource not found
 */
OCEntityHandler OCGetResourceHandler(OCResourceHandle handle) {
    OCResource *resource;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceHandler"));

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *)handle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return NULL;
    }

    // Bind the handler
    return resource->entityHandler;
}

void incrementSequenceNumber(OCResource * resPtr)
{
    // Increment the sequence number
    resPtr->sequenceNum += 1;
    if (resPtr->sequenceNum == MAX_SEQUENCE_NUMBER)
    {
        resPtr->sequenceNum = OC_OFFSET_SEQUENCE_NUMBER+1;
    }
    return;
}

/**
 * Notify Presence subscribers that a resource has been modified
 *
 * @param resourceType - Handle to the resourceType linked list of resource
 *                       that was modified.
 * @param qos          - Quality Of Service
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
#ifdef WITH_PRESENCE


OCStackResult SendPresenceNotification(OCResourceType *resourceType)
{
    OCResource *resPtr = NULL;
    OCStackResult result = OC_STACK_ERROR;
    OCMethod method = OC_REST_PRESENCE;
    uint32_t maxAge = 0;
    resPtr = findResource((OCResource *) presenceResource.handle);
    if(NULL == resPtr)
    {
        return OC_STACK_NO_RESOURCE;
    }

    if((((OCResource *) presenceResource.handle)->resourceProperties) & OC_ACTIVE)
    {
        maxAge = presenceResource.presenceTTL;

        result = SendAllObserverNotification(method, resPtr, maxAge, resourceType, OC_LOW_QOS);
    }

    return result;
}

/**
 * Send Stop Notification to Presence subscribers
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 *
 */

OCStackResult SendStopNotification()
{
    OCResource *resPtr = NULL;
    OCStackResult result = OC_STACK_ERROR;
    OCMethod method = OC_REST_PRESENCE;
    resPtr = findResource((OCResource *) presenceResource.handle);
    if(NULL == resPtr)
    {
        return OC_STACK_NO_RESOURCE;
    }

    // maxAge is 0. ResourceType is NULL.
    result = SendAllObserverNotification(method, resPtr, 0, NULL, OC_LOW_QOS);

    return result;
}

#endif // WITH_PRESENCE
/**
 * Notify observers that an observed value has changed.
 *
 * @param handle - handle of resource
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_NO_RESOURCE - invalid resource handle
 *     OC_STACK_NO_OBSERVERS - no more observers intrested in resource
 */
OCStackResult OCNotifyAllObservers(OCResourceHandle handle, OCQualityOfService qos) {

    OC_LOG(INFO, TAG, PCF("Entering OCNotifyAllObservers"));

    OCResource *resPtr = NULL;
    OCStackResult result;
    OCMethod method = OC_REST_NOMETHOD;
    uint32_t maxAge = 0;

    OC_LOG(INFO, TAG, PCF("Entering OCNotifyAllObservers"));
    #ifdef WITH_PRESENCE
    if(handle == presenceResource.handle)
    {
        return OC_STACK_OK;
    }
    #endif // WITH_PRESENCE
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_ERROR);

    // Verify that the resource exists
    resPtr = findResource ((OCResource *) handle);
    if (NULL == resPtr)
    {
        return OC_STACK_NO_RESOURCE;
    }
    else
    {
        //only increment in the case of regular observing (not presence)
        incrementSequenceNumber(resPtr);
        method = OC_REST_OBSERVE;
        maxAge = MAX_OBSERVE_AGE;
        #ifdef WITH_PRESENCE
        result = SendAllObserverNotification (method, resPtr, maxAge, NULL, qos);
        #else
        result = SendAllObserverNotification (method, resPtr, maxAge, qos);
        #endif
        return result;
    }
}

OCStackResult
OCNotifyListOfObservers (OCResourceHandle handle,
                         OCObservationId  *obsIdList,
                         uint8_t          numberOfIds,
                         unsigned char    *notificationJSONPayload,
                         OCQualityOfService qos)
{
    OC_LOG(INFO, TAG, PCF("Entering OCNotifyListOfObservers"));

    OCResource *resPtr = NULL;
    //TODO: we should allow the server to define this
    uint32_t maxAge = MAX_OBSERVE_AGE;

    VERIFY_NON_NULL(handle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(obsIdList, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(notificationJSONPayload, ERROR, OC_STACK_ERROR);

    // Verify that the resource exists
    resPtr = findResource ((OCResource *) handle);
    if (NULL == resPtr || myStackMode == OC_CLIENT)
    {
        return OC_STACK_NO_RESOURCE;
    }
    else
    {
        incrementSequenceNumber(resPtr);
    }
    return (SendListObserverNotification(resPtr, obsIdList, numberOfIds,
            notificationJSONPayload, maxAge, qos));
}

/**
 * Send a response to a request.
 * The response can be a regular, slow, or block (i.e. a response that
 * is too large to be sent in a single PDU and must span multiple transmissions)
 *
 * @param response - pointer to structure that contains response parameters
 *
 * @return
 *     OC_STACK_OK                         - No errors; Success
 *     OC_STACK_INVALID_PARAM              - Invalid pointer to OCServerResponse
 *     OC_STACK_INVALID_REQUEST_HANDLE     - Request handle not found
 *     OC_STACK_PERSISTENT_BUFFER_REQUIRED - Block transfer needed for response, so a
 *                                           persistent response buffer is necessary
 */
OCStackResult OCDoResponse(OCEntityHandlerResponse *ehResponse)
{
    OCStackResult result = OC_STACK_ERROR;
    OCServerRequest *serverRequest = NULL;

    OC_LOG(INFO, TAG, PCF("Entering OCDoResponse"));

    // Validate input parameters
    VERIFY_NON_NULL(ehResponse, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(ehResponse->requestHandle, ERROR, OC_STACK_INVALID_PARAM);

    // TODO: Placeholder for creating a response entry when implementing
    // block transfer feature

    // If a response payload is present, check if block transfer is required
    if (ehResponse->payload && OCIsPacketTransferRequired(NULL,
            (const char *)ehResponse->payload, ehResponse->payloadSize))
    {
        OC_LOG(INFO, TAG, PCF("Block transfer required"));

        // Persistent response buffer is needed for block transfer
        if (!ehResponse->persistentBufferFlag)
        {
            OC_LOG(WARNING, TAG, PCF("Persistent response buffer required"));
            return OC_STACK_PERSISTENT_BUFFER_REQUIRED;
        }
        // TODO: Placeholder for block transfer handling
        // TODO: Placeholder for setting the the response handle in the OCServerResponse struct
            // when implementing the block transfer feature
    }
    else
    {
        // Normal response
        // Get pointer to request info
        serverRequest = GetServerRequestUsingHandle((OCServerRequest *)ehResponse->requestHandle);
        if(serverRequest)
        {
            result = serverRequest->ehResponseHandler(ehResponse);
        }
    }
    return result;
}

/**
 * Cancel a response.  Applies to a block response
 *
 * @param responseHandle - response handle set by stack in OCServerResponse after
 *                         OCDoResponse is called
 *
 * @return
 *     OC_STACK_OK               - No errors; Success
 *     OC_STACK_INVALID_PARAM    - The handle provided is invalid.
 */
OCStackResult OCCancelResponse(OCResponseHandle responseHandle)
{
    OCStackResult result = OC_STACK_NOTIMPL;

    OC_LOG(INFO, TAG, PCF("Entering OCCancelResponse"));

    // TODO: validate response handle

    return result;
}

//-----------------------------------------------------------------------------
// Private internal function definitions
//-----------------------------------------------------------------------------
/**
 * Generate handle of OCDoResource invocation for callback management.
 */
static OCDoHandle GenerateInvocationHandle()
{
    OCDoHandle handle = NULL;
    // Generate token here, it will be deleted when the transaction is deleted
    handle = (OCDoHandle) OCMalloc(sizeof(uint8_t[CA_MAX_TOKEN_LEN]));
    if (handle)
    {
        OCFillRandomMem((uint8_t*)handle, sizeof(uint8_t[CA_MAX_TOKEN_LEN]));
    }

    return handle;
}
#ifdef WITH_PRESENCE
OCStackResult OCChangeResourceProperty(OCResourceProperty * inputProperty,
        OCResourceProperty resourceProperties, uint8_t enable)
{
    if (resourceProperties
            > (OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | OC_SLOW)) {
        OC_LOG(ERROR, TAG, PCF("Invalid property"));
        return OC_STACK_INVALID_PARAM;
    }
    if(!enable)
    {
        *inputProperty = (OCResourceProperty) (*inputProperty & ~(resourceProperties));
    }
    else
    {
        *inputProperty = (OCResourceProperty) (*inputProperty | resourceProperties);
    }
    return OC_STACK_OK;
}
#endif

/**
 * Initialize resource data structures, variables, etc.
 */
OCStackResult initResources() {
    OCStackResult result = OC_STACK_OK;
    // Init application resource vars
    headResource = NULL;
    tailResource = NULL;
    // Init Virtual Resources
    #ifdef WITH_PRESENCE
    presenceResource.presenceTTL = OC_DEFAULT_PRESENCE_TTL;
    //presenceResource.token = OCGenerateCoAPToken();
    result = OCCreateResource(&presenceResource.handle,
            OC_RSRVD_RESOURCE_TYPE_PRESENCE,
            "core.r",
            OC_PRESENCE_URI,
            NULL,
            OC_OBSERVABLE);
    //make resource inactive
    result = OCChangeResourceProperty(
            &(((OCResource *) presenceResource.handle)->resourceProperties),
            OC_ACTIVE, 0);
    #endif
    return result;
}

/**
 * Add a resource to the end of the linked list of resources.
 *
 * @param resource - resource to be added
 */
void insertResource(OCResource *resource)
{
    if (!headResource)
    {
        // First resource. Head and tail are the same.
        headResource = resource;
        tailResource = resource;
    }
    else
    {
        tailResource->next = resource;
        tailResource = resource;
    }
    resource->next = NULL;
}

/**
 * Find a resource in the linked list of resources.
 *
 * @param resource - resource to be found
 * @return
 *     NULL                - resource not found
 *     pointer to resource - pointer to resource that was found in the linked list
 */
OCResource *findResource(OCResource *resource)
{
    OCResource *pointer = headResource;

    while (pointer)
    {
        if (pointer == resource)
        {
            return resource;
        }
        pointer = pointer->next;
    }
    return NULL;
}

void deleteAllResources()
{
    OCResource *pointer = headResource;
    OCResource *temp;

    while (pointer)
    {
        temp = pointer->next;
        #ifdef WITH_PRESENCE
        if(pointer != (OCResource *) presenceResource.handle)
        {
        #endif // WITH_PRESENCE
            deleteResource(pointer);
        #ifdef WITH_PRESENCE
        }
        #endif // WITH_PRESENCE
        pointer = temp;
    }

    #ifdef WITH_PRESENCE
    // Ensure that the last resource to be deleted is the presence resource. This allows for all
    // presence notification attributed to their deletion to be processed.
    deleteResource((OCResource *) presenceResource.handle);
    #endif // WITH_PRESENCE
}

/**
 * Delete the resource from the linked list.
 *
 * @param resource - resource to be deleted
 * @return
 *    0 - error
 *    1 - success
 */
int deleteResource(OCResource *resource)
{
    OCResource *prev = NULL;
    OCResource *temp;

    temp = headResource;
    while (temp)
    {
        if (temp == resource)
        {
            // Invalidate all Resource Properties.
            resource->resourceProperties = (OCResourceProperty) 0;
            #ifdef WITH_PRESENCE
            if(resource != (OCResource *) presenceResource.handle)
            {
            #endif // WITH_PRESENCE
                OCNotifyAllObservers((OCResourceHandle)resource, OC_HIGH_QOS);
            #ifdef WITH_PRESENCE
            }

            if(presenceResource.handle)
            {
                ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
                if(resource != (OCResource *) presenceResource.handle)
                {
                    SendPresenceNotification(resource->rsrcType);
                }
                else
                {
                    SendPresenceNotification(NULL);
                }
            }
            #endif

            // Only resource in list.
            if (temp == headResource && temp == tailResource)
            {
                headResource = NULL;
                tailResource = NULL;
            }
            // Deleting head.
            else if (temp == headResource)
            {
                headResource = temp->next;
            }
            // Deleting tail.
            else if (temp == tailResource)
            {
                tailResource = prev;
                tailResource->next = NULL;
            }
            else
            {
                prev->next = temp->next;
            }

            deleteResourceElements(temp);
            OCFree(temp);
            return 1;
        }
        else
        {
            prev = temp;
            temp = temp->next;
        }
    }

    return 0;
}

/**
 * Delete all of the dynamically allocated elements that were created for the resource.
 *
 * @param resource - specified resource
 */
void deleteResourceElements(OCResource *resource)
{
    if (!resource) {
        return;
    }

    // remove URI
    OCFree(resource->uri);

    // Delete resourcetype linked list
    deleteResourceType(resource->rsrcType);

    // Delete resourceinterface linked list
    deleteResourceInterface(resource->rsrcInterface);
}

/**
 * Delete all of the dynamically allocated elements that were created for the resource type.
 *
 * @param resourceType - specified resource type
 */
void deleteResourceType(OCResourceType *resourceType)
{
    OCResourceType *pointer = resourceType;
    OCResourceType *next;

    while (pointer)
    {
        next = pointer->next;
        OCFree(pointer->resourcetypename);
        OCFree(pointer);
        pointer = next;
    }
}

/**
 * Delete all of the dynamically allocated elements that were created for the resource interface.
 *
 * @param resourceInterface - specified resource interface
 */
void deleteResourceInterface(OCResourceInterface *resourceInterface) {
    OCResourceInterface *pointer = resourceInterface;
    OCResourceInterface *next;

    while (pointer)
    {
        next = pointer->next;
        OCFree(pointer->name);
        OCFree(pointer);
        pointer = next;
    }
}

/**
 * Insert a resource type into a resource's resource type linked list.
 *
 * @param resource - resource where resource type is to be inserted
 * @param resourceType - resource type to be inserted
 */
void insertResourceType(OCResource *resource, OCResourceType *resourceType) {
    OCResourceType *pointer;

    if (resource && !resource->rsrcType)
    {
        resource->rsrcType = resourceType;
    }
    else
    {
        if(resource)
        {
            pointer = resource->rsrcType;
        }
        else
        {
            pointer = resourceType;
        }
        while (pointer->next) {
            pointer = pointer->next;
        }
        pointer->next = resourceType;
    }
    resourceType->next = NULL;
}

/**
 * Get a resource type at the specified index within a resource.
 *
 * @param handle - handle of resource
 * @param index - index of resource type
 *
 * @return
 *    resourcetype - if found
 *    NULL - not found
 */
OCResourceType *findResourceTypeAtIndex(OCResourceHandle handle, uint8_t index) {
    OCResource *resource;
    OCResourceType *pointer;
    uint8_t i;

    // Find the specified resource
    resource = findResource((OCResource *) handle);
    if (!resource)
    {
        return NULL;
    }

    // Make sure a resource has a resourcetype
    if (!resource->rsrcType)
    {
        return NULL;
    }

    // Iterate through the list
    pointer = resource->rsrcType;
    i = 0;
    while ((i < index) && pointer)
    {
        i++;
        pointer = pointer->next;
    }
    return pointer;
}

/**
 * Finds a resource type in an OCResourceType link-list.
 *
 * @param resourceTypeList - the link-list to be searched through
 * @param resourceTypeName - the key to search for
 *
 * @return
 *      resourceType that matches the key (ie. resourceTypeName)
 *      NULL - either an invalid parameter or this function was unable to find the key.
 */
OCResourceType *findResourceType(OCResourceType * resourceTypeList, const char * resourceTypeName)
{
    if(resourceTypeList && resourceTypeName)
    {
        OCResourceType * rtPointer = resourceTypeList;
        while(resourceTypeName && rtPointer)
        {
            if(rtPointer->resourcetypename &&
                    strcmp(resourceTypeName, (const char *)
                    (rtPointer->resourcetypename)) == 0)
            {
                break;
            }
            rtPointer = rtPointer->next;
        }
        return rtPointer;
    }
    return NULL;
}
/**
 * Insert a resource interface into a resource's resource interface linked list.
 *
 * @param resource - resource where resource interface is to be inserted
 * @param resourceInterface - resource interface to be inserted
 */
void insertResourceInterface(OCResource *resource,
        OCResourceInterface *resourceInterface)
{
    OCResourceInterface *pointer;

    if (!resource->rsrcInterface)
    {
        resource->rsrcInterface = resourceInterface;
    }
    else
    {
        pointer = resource->rsrcInterface;
        while (pointer->next)
        {
            pointer = pointer->next;
        }
        pointer->next = resourceInterface;
    }
    resourceInterface->next = NULL;
}

/**
 * Get a resource interface at the specified index within a resource.
 *
 * @param handle - handle of resource
 * @param index - index of resource interface
 *
 * @return
 *    resourceinterface - if found
 *    NULL - not found
 */
OCResourceInterface *findResourceInterfaceAtIndex(OCResourceHandle handle,
        uint8_t index) {
    OCResource *resource;
    OCResourceInterface *pointer;
    uint8_t i = 0;

    // Find the specified resource
    resource = findResource((OCResource *) handle);
    if (!resource) {
        return NULL;
    }

    // Make sure a resource has a resourceinterface
    if (!resource->rsrcInterface) {
        return NULL;
    }

    // Iterate through the list
    pointer = resource->rsrcInterface;

    while ((i < index) && pointer) {
        i++;
        pointer = pointer->next;
    }
    return pointer;
}

/**
 * Determine if a request/response must be sent in a block transfer because it is too large to be
 * sent in a single PDU.  This function can be used for either a request or a response
 *
 * @param request  - NULL or pointer to request
 * @param response - NULL or pointer to response
 * @param size     - 0 or size of the request/response.  If 0, strlen is used for determining
 *                   the length of the request/response
 *
 * @return
 *    0 - packet transfer NOT required (i.e. normal request/response)
 *    1 - packet transfer required (i.e. block transfer needed)
 */
uint8_t OCIsPacketTransferRequired(const char *request, const char *response, uint16_t size)
{
    uint8_t result = 0;

    // Determine if we are checking a request or a response
    if (request)
    {
        // If size is greater than 0, use it for the request size value, otherwise
        // assume request is null terminated and use strlen for size value
        if ((size > MAX_REQUEST_LENGTH) || (strlen(request) > MAX_REQUEST_LENGTH))
        {
            result = 1;
        }
    }
    else if (response)
    {
        // If size is greater than 0, use it for the response size value, otherwise
        // assume response is null terminated and use strlen for size value
        if ((size > MAX_RESPONSE_LENGTH) || (strlen(response) > MAX_RESPONSE_LENGTH))
        {
            result = 1;
        }
    }
    return result;
}

/**
 * Retrieves a resource type based upon a query ontains only just one
 * resource attribute (and that has to be of type "rt").
 *
 * @remark This API malloc's memory for the resource type. Do not malloc resourceType
 * before passing in.
 *
 * @param query - The quert part of the URI
 * @param resourceType - The resource type to be populated; pass by reference.
 *
 * @return
 *  OC_STACK_INVALID_PARAM - Returns this if the resourceType parameter is invalid/NULL.
 *  OC_STACK_OK            - Success
 */
OCStackResult getResourceType(const char * query, unsigned char** resourceType)
{
    if(!query)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult result = OC_STACK_ERROR;

    if(strncmp(query, "rt=", 3) == 0)
    {
        *resourceType = (unsigned char *) OCMalloc(strlen(query)-3);
        if(!*resourceType)
        {
            result = OC_STACK_NO_MEMORY;
        }

        strcpy((char *)*resourceType, ((const char *)&query[3]));
        result = OC_STACK_OK;
    }

    return result;
}

OCStackResult getQueryFromUri(const char * uri, unsigned char** query, char ** newURI)
{
    if(!uri)
    {
        return OC_STACK_INVALID_URI;
    }
    if(!query || !newURI)
    {
        return OC_STACK_INVALID_PARAM;
    }
    char * leftToken = NULL;
    char * tempURI = (char *) OCMalloc(strlen(uri) + 1);
    if(!tempURI)
    {
        goto exit;
    }
    strcpy(tempURI, uri);
    char* strTokPtr;
    leftToken = strtok_r((char *)tempURI, "?", &strTokPtr);

    //TODO-CA: This could be simplified. Clean up required.
    while(leftToken != NULL)
    {
        if(strncmp(leftToken, "rt=", 3) == 0 || strncmp(leftToken, "if=", 3) == 0)
        {
            *query = (unsigned char *) OCMalloc(strlen(leftToken) + 1);
            if(!*query)
            {
                OCFree(tempURI);
                goto exit;
            }
            strcpy((char *)*query, ((const char *)&leftToken[0]));
            break;
        }
        leftToken = strtok_r(NULL, "?", &strTokPtr);
    }

    *newURI = tempURI;

    return OC_STACK_OK;

    exit:
        return OC_STACK_NO_MEMORY;
}

const ServerID OCGetServerInstanceID(void)
{
    static bool generated = false;
    static ServerID sid;

    if(generated)
    {
        return sid;
    }

    sid = OCGetRandom();
    generated = true;
    return sid;
}

const char* OCGetServerInstanceIDString(void)
{
    // max printed length of a base 10
    // uint32 is 10 characters, so 11 includes null.
    // This will change as the representation gets switched
    // to another value
    static char buffer[11];
    int n = sprintf(buffer, "%u", OCGetServerInstanceID());
    if (n < 0)
    {
        buffer[0]='\0';
    }

    return buffer;
}

/// Retrieve the IPv4 address embedded inside OCDev address data structure
int32_t OCDevAddrToIPv4Addr(OCDevAddr *ipAddr, uint8_t *a, uint8_t *b,
        uint8_t *c, uint8_t *d )
{
    if ( !ipAddr || !a || !b || !c || !d ) {
        OC_LOG(FATAL, TAG, "Invalid argument");
        return OC_STACK_INVALID_PARAM;
    }

    *a = ipAddr->addr[0];
    *b = ipAddr->addr[1];
    *c = ipAddr->addr[2];
    *d = ipAddr->addr[3];

    return OC_STACK_OK;
}


/// Retrieve the IPv4 address embedded inside OCDev address data structure
int32_t OCDevAddrToPort(OCDevAddr *ipAddr, uint16_t *port)
{
    if ( !ipAddr || !port ) {
        OC_LOG(FATAL, TAG, "Invalid argument");
        return OC_STACK_INVALID_PARAM;
    }

    *port = *((uint16_t*)&ipAddr->addr[4]);

    return OC_STACK_OK;
}

/*
 * Attempts to initialize every network interface that the CA Layer might have compiled in.
 *
 * Note: At least one interface must succeed to initialize. If all calls to @ref CASelectNetwork
 * return something other than @ref CA_STATUS_OK, then this function fails.
 *
 * @return
 * CA_STATUS_OK - Success
 * CA_NOT_SUPPORTED or CA_STATUS_FAILED - None of the transports successfully initialized.
 */
CAResult_t OCSelectNetwork()
{
    CAResult_t retResult = CA_STATUS_FAILED;
    CAResult_t caResult = CA_STATUS_OK;

    CAConnectivityType_t connTypes[] = {
            CA_ETHERNET,
            CA_WIFI,
            CA_EDR,
            CA_LE};
    int numConnTypes = sizeof(connTypes)/sizeof(connTypes[0]);

    for(int i = 0; i<numConnTypes; i++)
    {
        // Ignore CA_NOT_SUPPORTED error. The CA Layer may have not compiled in the interface.
        if(caResult == CA_STATUS_OK || caResult == CA_NOT_SUPPORTED)
        {
           caResult = CASelectNetwork(connTypes[i]);
           if(caResult == CA_STATUS_OK)
           {
               retResult = CA_STATUS_OK;
           }
        }
    }

    if(retResult != CA_STATUS_OK)
    {
        return caResult; // Returns error of appropriate transport that failed fatally.
    }

    return retResult;
}

/**
 * Takes a @ref CAResult_t and converts it to a similar or equivalent @ref OCStackResult value.
 *
 * @return @ref OCStackResult - The equivalent or similar result code to the in-param caResult.
 */
OCStackResult CAResultToOCResult(CAResult_t caResult)
{
    switch (caResult)
    {
        case CA_STATUS_OK:
            return OC_STACK_OK;
        case CA_STATUS_INVALID_PARAM:
            return OC_STACK_INVALID_PARAM;
        case CA_ADAPTER_NOT_ENABLED:
            return OC_STACK_ADAPTER_NOT_ENABLED;
        case CA_SERVER_STARTED_ALREADY:
            return OC_STACK_OK;
        case CA_SERVER_NOT_STARTED:
            return OC_STACK_ERROR;
        case CA_DESTINATION_NOT_REACHABLE:
            return OC_STACK_COMM_ERROR;
        case CA_SOCKET_OPERATION_FAILED:
            return OC_STACK_COMM_ERROR;
        case CA_SEND_FAILED:
            return OC_STACK_COMM_ERROR;
        case CA_RECEVIE_FAILED:
            return OC_STACK_COMM_ERROR;
        case CA_MEMORY_ALLOC_FAILED:
            return OC_STACK_NO_MEMORY;
        case CA_REQUEST_TIMEOUT:
            return OC_STACK_TIMEOUT;
        case CA_DESTINATION_DISCONNECTED:
            return OC_STACK_COMM_ERROR;
        case CA_STATUS_FAILED:
            return OC_STACK_ERROR;
        case CA_NOT_SUPPORTED:
            return OC_STACK_NOTIMPL;
        default:
            return OC_STACK_ERROR;
    }
}
