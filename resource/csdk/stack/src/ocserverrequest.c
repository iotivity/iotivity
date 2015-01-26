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
#include "ocserverrequest.h"
#include "ocresourcehandler.h"


#ifdef CA_INT
#include "cacommon.h"
#include "cainterface.h"
#endif

// Module Name
#define VERIFY_NON_NULL(arg) { if (!arg) {OC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} }

#define TAG  PCF("ocserverrequest")

static struct OCServerRequest * serverRequestList = NULL;
static struct OCServerResponse * serverResponseList = NULL;

#ifdef CA_INT
OCServerRequest * GetServerRequestUsingToken (const CAToken_t token)
#else // CA_INT
OCServerRequest * GetServerRequestUsingToken (const OCCoAPToken token)
#endif // CA_INT
{
    OCServerRequest * out = NULL;
    LL_FOREACH (serverRequestList, out)
    {
#ifdef CA_INT
        OC_LOG(INFO, TAG,PCF("comparing tokens"));
        OC_LOG_BUFFER(INFO, TAG, (const uint8_t *)token, CA_MAX_TOKEN_LEN);
        OC_LOG_BUFFER(INFO, TAG, (const uint8_t *)out->requestToken, CA_MAX_TOKEN_LEN);
        if(memcmp(out->requestToken, token, CA_MAX_TOKEN_LEN) == 0)
        {
            return out;
        }
#else // CA_INT
        OC_LOG(INFO, TAG,PCF("comparing tokens"));
        OC_LOG_BUFFER(INFO, TAG, token.token, token.tokenLength);
        OC_LOG_BUFFER(INFO, TAG, out->requestToken.token, out->requestToken.tokenLength);
        if((out->requestToken.tokenLength == token.tokenLength) &&
                (memcmp(out->requestToken.token, token.token, token.tokenLength) == 0))
        {
            return out;
        }
#endif // CA_INT
    }
    OC_LOG(INFO, TAG, PCF("Server Request not found!!"));
    return NULL;
}

OCServerRequest * GetServerRequestUsingHandle (const OCServerRequest * handle)
{
    OCServerRequest * out = NULL;
    LL_FOREACH (serverRequestList, out)
    {
        if(out == handle)
        {
            return out;
        }
    }
    OC_LOG(INFO, TAG, PCF("Server Request not found!!"));
    return NULL;
}

OCServerResponse * GetServerResponseUsingHandle (const OCServerRequest * handle)
{
    OCServerResponse * out = NULL;
    LL_FOREACH (serverResponseList, out)
    {
        if(out->requestHandle == handle)
        {
            return out;
        }
    }
    OC_LOG(INFO, TAG, PCF("Server Response not found!!"));
    return NULL;
}

#ifdef CA_INT
OCStackResult AddServerRequest (OCServerRequest ** request, uint16_t coapID,
        uint8_t delayedResNeeded, uint8_t secured, uint8_t notificationFlag, OCMethod method,
        uint8_t numRcvdVendorSpecificHeaderOptions, uint32_t observationOption,
        OCQualityOfService qos, unsigned char * query,
        OCHeaderOption * rcvdVendorSpecificHeaderOptions,
        unsigned char * reqJSONPayload, CAToken_t * requestToken,
        OCDevAddr * requesterAddr, unsigned char * resourceUrl, size_t reqTotalSize,
        CAAddress_t *addressInfo, CAConnectivityType_t connectivityType)
#else // CA_INT
OCStackResult AddServerRequest (OCServerRequest ** request, uint16_t coapID,
        uint8_t delayedResNeeded, uint8_t secured, uint8_t notificationFlag, OCMethod method,
        uint8_t numRcvdVendorSpecificHeaderOptions, uint32_t observationOption,
        OCQualityOfService qos, unsigned char * query,
        OCHeaderOption * rcvdVendorSpecificHeaderOptions,
        unsigned char * reqJSONPayload, OCCoAPToken * requestToken,
        OCDevAddr * requesterAddr, unsigned char * resourceUrl, size_t reqTotalSize)
#endif // CA_INT
{
    OCServerRequest * serverRequest = NULL;

    //Note: OCServerRequest includes 1 byte for the JSON Payload.  payloadSize is calculated
    //as the required length of the string, so this will result in enough room for the
    //null terminator as well.
    serverRequest = (OCServerRequest *) OCCalloc(1, sizeof(OCServerRequest) + reqTotalSize - 1);
    VERIFY_NON_NULL(serverRequest);

    serverRequest->coapID = coapID;
    serverRequest->delayedResNeeded = delayedResNeeded;
    serverRequest->secured = secured;
    serverRequest->notificationFlag = notificationFlag;

    serverRequest->method = method;
    serverRequest->numRcvdVendorSpecificHeaderOptions = numRcvdVendorSpecificHeaderOptions;
    serverRequest->observationOption = observationOption;
    serverRequest->observeResult = OC_STACK_ERROR;
    serverRequest->qos = qos;
    serverRequest->ehResponseHandler = HandleSingleResponse;
    serverRequest->numResponses = 1;
    if(query)
    {
        memcpy(serverRequest->query, query, strlen((const char *)query) + 1);
    }
    if(rcvdVendorSpecificHeaderOptions)
    {
        memcpy(serverRequest->rcvdVendorSpecificHeaderOptions, rcvdVendorSpecificHeaderOptions,
            MAX_HEADER_OPTIONS * sizeof(OCHeaderOption));
    }
    if(reqJSONPayload)
    {
        // destination is at least 1 greater than the source, so a NULL always exists in the
        // last character
        strncpy((char*)serverRequest->reqJSONPayload,
                (const char*)reqJSONPayload, reqTotalSize - 1);
    }
    serverRequest->requestComplete = 0;
    if(requestToken)
    {
#ifdef CA_INT
        serverRequest->requestToken = (CAToken_t)OCMalloc(CA_MAX_TOKEN_LEN+1);
        VERIFY_NON_NULL (serverRequest->requestToken);
        memset(serverRequest->requestToken, 0, CA_MAX_TOKEN_LEN + 1);
        memcpy(serverRequest->requestToken, *requestToken, CA_MAX_TOKEN_LEN);
#else // CA_INT
        memcpy(&serverRequest->requestToken, requestToken, sizeof(OCCoAPToken));
#endif // CA_INT
    }
    if(requesterAddr)
    {
        memcpy(&serverRequest->requesterAddr, requesterAddr, sizeof(OCDevAddr));
    }
    if(resourceUrl)
    {
        memcpy(serverRequest->resourceUrl, resourceUrl, strlen((const char *)resourceUrl) + 1);
    }
#ifdef CA_INT
    if (addressInfo)
    {
        serverRequest->addressInfo = *addressInfo;
    }
    serverRequest->connectivityType = connectivityType;
#endif

    *request = serverRequest;
    OC_LOG(INFO, TAG, PCF("Server Request Added!!"));
    LL_APPEND (serverRequestList, serverRequest);
    return OC_STACK_OK;

exit:
    if (serverRequest)
    {
        OCFree(serverRequest);
        serverRequest = NULL;
    }
    *request = NULL;
    return OC_STACK_NO_MEMORY;
}

OCStackResult AddServerResponse (OCServerResponse ** response, OCRequestHandle requestHandle)
{
    OCServerResponse * serverResponse = NULL;

    serverResponse = (OCServerResponse *) OCCalloc(1, sizeof(OCServerResponse));
    VERIFY_NON_NULL(serverResponse);

    serverResponse->payload = (unsigned char *) OCMalloc(MAX_RESPONSE_LENGTH);
    VERIFY_NON_NULL(serverResponse->payload);
    memset(serverResponse->payload, 0, MAX_RESPONSE_LENGTH);

    serverResponse->remainingPayloadSize = MAX_RESPONSE_LENGTH;
    serverResponse->requestHandle = requestHandle;

    *response = serverResponse;
    OC_LOG(INFO, TAG, PCF("Server Response Added!!"));
    LL_APPEND (serverResponseList, serverResponse);
    return OC_STACK_OK;

exit:
    if (serverResponse)
    {
        OCFree(serverResponse);
        serverResponse = NULL;
    }
    *response = NULL;
    return OC_STACK_NO_MEMORY;
}

// Form the OCEntityHandlerRequest struct
OCStackResult FormOCEntityHandlerRequest(OCEntityHandlerRequest * entityHandlerRequest, OCRequestHandle request,
        OCMethod method, OCResourceHandle resource, unsigned char * queryBuf, unsigned char * bufReqPayload,
        uint8_t numVendorOptions, OCHeaderOption * vendorOptions, OCObserveAction observeAction,
        OCObservationId observeID)
{
    if (entityHandlerRequest)
    {
        memset(entityHandlerRequest, 0, sizeof(OCEntityHandlerRequest));
        entityHandlerRequest->requestHandle = request;
        entityHandlerRequest->method = method;
        entityHandlerRequest->resource = (OCResourceHandle) resource;
        entityHandlerRequest->query = queryBuf;
        entityHandlerRequest->reqJSONPayload = bufReqPayload;
        entityHandlerRequest->numRcvdVendorSpecificHeaderOptions = numVendorOptions;
        entityHandlerRequest->rcvdVendorSpecificHeaderOptions = vendorOptions;

        entityHandlerRequest->obsInfo.action = observeAction;
        entityHandlerRequest->obsInfo.obsId = observeID;
        return OC_STACK_OK;
    }

    return OC_STACK_INVALID_PARAM;
}

void FindAndDeleteServerResponse(OCServerResponse * serverResponse)
{
    OCServerResponse* tmp;
    if(serverResponse)
    {
        LL_FOREACH(serverResponseList, tmp)
        {
            if (serverResponse == tmp)
            {
                DeleteServerResponse(tmp);
                return;
            }
        }
    }
}

void DeleteServerResponse(OCServerResponse * serverResponse)
{
    if(serverResponse) {
        LL_DELETE(serverResponseList, serverResponse);
        OCFree(serverResponse->payload);
        OCFree(serverResponse);
        OC_LOG(INFO, TAG, PCF("Server Response Removed!!"));
    }
}

void FindAndDeleteServerRequest(OCServerRequest * serverRequest)
{
    OCServerRequest* tmp;
    if(serverRequest)
    {
        LL_FOREACH(serverRequestList, tmp)
        {
            if (serverRequest == tmp)
            {
                DeleteServerRequest(tmp);
                return;
            }
        }
    }
}

void DeleteServerRequest(OCServerRequest * serverRequest)
{
    if(serverRequest) {
        LL_DELETE(serverRequestList, serverRequest);
        OCFree(serverRequest);
        serverRequest = NULL;
        OC_LOG(INFO, TAG, PCF("Server Request Removed!!"));
    }
}

OCStackResult HandleSingleResponse(OCEntityHandlerResponse * ehResponse)
{
#ifdef CA_INT
    OCStackResult result = OC_STACK_ERROR;
    CARemoteEndpoint_t responseEndpoint;
    CAResponseInfo_t responseInfo;
    CAHeaderOption_t* optionsPointer;

    OC_LOG_V(INFO, TAG, "Inside HandleSingleResponse: %s", ehResponse->payload);

    OCServerRequest *serverRequest = (OCServerRequest *)ehResponse->requestHandle;

    // Copy the address
    responseEndpoint.resourceUri      = (CAURI_t)serverRequest->resourceUrl;
    responseEndpoint.addressInfo      = serverRequest->addressInfo;
    responseEndpoint.connectivityType = serverRequest->connectivityType;
    responseEndpoint.isSecured        = serverRequest->secured;

    // Copy the info
    switch (ehResponse->ehResult)
    {
        case OC_EH_OK:
            responseInfo.result = CA_SUCCESS;
            break;
        case OC_EH_ERROR:
            responseInfo.result = CA_BAD_REQ;
            break;
        case OC_EH_RESOURCE_CREATED:
            responseInfo.result = CA_CREATED;
            break;
        case OC_EH_RESOURCE_DELETED:
            responseInfo.result = CA_DELETED;
            break;
        case OC_EH_SLOW:
            responseInfo.result = CA_SUCCESS;
            break;
        case OC_EH_FORBIDDEN:
            responseInfo.result = CA_BAD_REQ;
            break;
        default:
            responseInfo.result = CA_BAD_REQ;
            break;
    }

    // TODO-CA: Need to do something with a slow response if a confirmed request was sent
    // from client

    // TODO-CA:  Need to handle CA_MSG_RESET and CA_MSG_ACKNOWLEDGE
    switch (serverRequest->qos)
    {
        case OC_LOW_QOS:
            responseInfo.info.type = CA_MSG_NONCONFIRM;
            break;
        case OC_MEDIUM_QOS:
            responseInfo.info.type = CA_MSG_NONCONFIRM;
            break;
        case OC_HIGH_QOS:
            responseInfo.info.type = CA_MSG_CONFIRM;
            break;
        case OC_NA_QOS:
            responseInfo.info.type = CA_MSG_NONCONFIRM;
            break;
        default:
            responseInfo.info.type = CA_MSG_NONCONFIRM;
            break;
    }

    responseInfo.info.token = (CAToken_t)OCMalloc(CA_MAX_TOKEN_LEN+1);
    if (!responseInfo.info.token)
    {
        OC_LOG(FATAL, TAG, "Response Info Token is NULL");
        return result;
    }
    memset(responseInfo.info.token, 0, CA_MAX_TOKEN_LEN + 1);
    memcpy(responseInfo.info.token, serverRequest->requestToken, CA_MAX_TOKEN_LEN);

    if(serverRequest->observeResult == OC_STACK_OK)
    {
        responseInfo.info.numOptions = ehResponse->numSendVendorSpecificHeaderOptions + 1;
    }
    else
    {
        responseInfo.info.numOptions = ehResponse->numSendVendorSpecificHeaderOptions;
    }

    responseInfo.info.options = (CAHeaderOption_t *)
                                    malloc(sizeof(CAHeaderOption_t) * responseInfo.info.numOptions);

    optionsPointer = responseInfo.info.options;

    if(serverRequest->observeResult == OC_STACK_OK)
    {
        responseInfo.info.numOptions = ehResponse->numSendVendorSpecificHeaderOptions + 1;
    }

    // TODO-CA Revisit this logic
    if(serverRequest->observeResult == OC_STACK_OK)
    {
        responseInfo.info.options[0].protocolID = CA_COAP_ID;
        responseInfo.info.options[0].optionID = COAP_OPTION_OBSERVE;
        responseInfo.info.options[0].optionLength = sizeof(uint32_t);
        memcpy(responseInfo.info.options[0].optionData,
                &(serverRequest->observationOption), sizeof(uint32_t));

        // Point to the next header option before copying vender specific header options
        optionsPointer += 1;
    }

    if (ehResponse->numSendVendorSpecificHeaderOptions)
    {
        memcpy(optionsPointer, ehResponse->sendVendorSpecificHeaderOptions,
                        sizeof(OCHeaderOption) * ehResponse->numSendVendorSpecificHeaderOptions);
    }

    // Allocate memory for the payload.
    char *payload = (char *)OCMalloc(MAX_RESPONSE_LENGTH);
    if(!payload)
    {
        return OC_STACK_NO_MEMORY;
    }
    memset(payload, 0, MAX_RESPONSE_LENGTH);
    // Put the JSON prefix and suffix around the payload
    strcpy(payload, (const char *)OC_JSON_PREFIX);
    strcat(payload, (const char *)ehResponse->payload);
    strcat(payload, (const char *)OC_JSON_SUFFIX);
    responseInfo.info.payload = (CAPayload_t)payload;

    CAResult_t caResult = CASendResponse(&responseEndpoint, &responseInfo);
    if(caResult != CA_STATUS_OK)
    {
        OC_LOG(ERROR, TAG, PCF("CASendResponse error"));
    }
    else
    {
        result = OC_STACK_OK;
    }

    OCFree(payload);
    //Delete the request
    FindAndDeleteServerRequest(serverRequest);
    return result;
#else
    OCStackResult result = OC_STACK_ERROR;
    OCServerProtocolResponse protocolResponse;
    memset(&protocolResponse, 0, sizeof(OCServerProtocolResponse));

    OC_LOG_V(INFO, TAG, "Inside HandleSingleResponse: %s", ehResponse->payload);

    OCServerRequest *serverRequest = (OCServerRequest *)ehResponse->requestHandle;
    // Format protocol response structure with data needed for
    // sending the response
    protocolResponse.qos = serverRequest->qos;

    if((OCResource *)ehResponse->resourceHandle &&
            ((OCResource *)ehResponse->resourceHandle)->resourceProperties == (OCResourceProperty) 0)
    {
        ehResponse->ehResult = OC_EH_RESOURCE_DELETED;
    }
    protocolResponse.result = EntityHandlerCodeToOCStackCode(ehResponse->ehResult);
    protocolResponse.requesterAddr = &serverRequest->requesterAddr;
    protocolResponse.requestToken = &serverRequest->requestToken;
    protocolResponse.numSendVendorSpecificHeaderOptions = ehResponse->numSendVendorSpecificHeaderOptions;
    protocolResponse.sendVendorSpecificHeaderOptions = ehResponse->sendVendorSpecificHeaderOptions;
    protocolResponse.resourceUri = ehResponse->resourceUri;
    protocolResponse.delayedResNeeded = serverRequest->delayedResNeeded;
    protocolResponse.secured = serverRequest->secured;
    protocolResponse.slowFlag = serverRequest->slowFlag;
    protocolResponse.notificationFlag = serverRequest->notificationFlag;

    //should we put the prefix and suffix here?
    protocolResponse.payload = (unsigned char *) OCMalloc(MAX_RESPONSE_LENGTH);
    if(!protocolResponse.payload)
    {
        return OC_STACK_NO_MEMORY;
    }
    strcpy((char *)protocolResponse.payload, (const char *)OC_JSON_PREFIX);
    strcat((char *)protocolResponse.payload, (const char *)ehResponse->payload);
    strcat((char *)protocolResponse.payload, (const char *)OC_JSON_SUFFIX);
    protocolResponse.payloadSize = strlen((const char *)protocolResponse.payload) + 1;
    protocolResponse.resourceUri = ehResponse->resourceUri;

    //revise the following
    protocolResponse.coapID = serverRequest->coapID;
    if(serverRequest->observeResult == OC_STACK_OK)
    {
        protocolResponse.observationOption = serverRequest->observationOption;
    }
    else
    {
        protocolResponse.observationOption = OC_OBSERVE_NO_OPTION;
    }
    // Make call to OCCoAP layer
    result = OCDoCoAPResponse(&protocolResponse);

    OCFree(protocolResponse.payload);
    //Delete the request
    FindAndDeleteServerRequest(serverRequest);
    return result;
#endif
}

OCStackResult HandleAggregateResponse(OCEntityHandlerResponse * ehResponse)
{
    OCStackResult stackRet = OC_STACK_ERROR;
    OCServerRequest * serverRequest = NULL;
    OCServerResponse * serverResponse = NULL;

    OC_LOG_V(INFO, TAG, "Inside HandleAggregateResponse: %s", ehResponse->payload);

    serverRequest = GetServerRequestUsingHandle((OCServerRequest *)ehResponse->requestHandle);
    serverResponse = GetServerResponseUsingHandle((OCServerRequest *)ehResponse->requestHandle);

    if(serverRequest)
    {
        if(!serverResponse)
        {
            OC_LOG(INFO, TAG, PCF("This is the first response fragment"));
            stackRet = AddServerResponse(&serverResponse, ehResponse->requestHandle);
            if (OC_STACK_OK != stackRet)
            {
                OC_LOG(ERROR, TAG, PCF("Error adding server response"));
                return stackRet;
            }
            VERIFY_NON_NULL(serverResponse);
            VERIFY_NON_NULL(serverResponse->payload);
        }

        if((serverResponse->remainingPayloadSize >= ehResponse->payloadSize + 1 &&
                serverRequest->numResponses == 1) ||
                (serverResponse->remainingPayloadSize >= ehResponse->payloadSize + 2 &&
                        serverRequest->numResponses > 1))
        {
            OC_LOG(INFO, TAG, PCF("There is room in response buffer"));
            // append
            snprintf((char *)serverResponse->payload, serverResponse->remainingPayloadSize, "%s%s", (char *)serverResponse->payload, (char *)ehResponse->payload);
            OC_LOG_V(INFO, TAG, "Current aggregated response  ...%s", serverResponse->payload);
            serverResponse->remainingPayloadSize -= ehResponse->payloadSize;
            (serverRequest->numResponses)--;
            if(serverRequest->numResponses == 0)
            {
                OC_LOG(INFO, TAG, PCF("This is the last response fragment"));
                ehResponse->payload = serverResponse->payload;
                ehResponse->payloadSize = strlen((char *) serverResponse->payload) + 1;
                stackRet = HandleSingleResponse(ehResponse);
                //Delete the request and response
                FindAndDeleteServerRequest(serverRequest);
                FindAndDeleteServerResponse(serverResponse);
            }
            else
            {
                OC_LOG(INFO, TAG, PCF("More response fragment to come"));
                // TODO: we should consider using strcat rather than setting a char by char here!
                snprintf((char *)serverResponse->payload, serverResponse->remainingPayloadSize, "%s%c", (char *)serverResponse->payload,OC_JSON_SEPARATOR);
                OC_LOG_V(INFO, TAG, "Current aggregated response  ...%s", serverResponse->payload);
                (serverResponse->remainingPayloadSize)--;
                stackRet = OC_STACK_OK;
            }
        }
        else
        {
            OC_LOG(INFO, TAG, PCF("No room in response buffer"));
            //Delete the request and response
            FindAndDeleteServerRequest(serverRequest);
            FindAndDeleteServerResponse(serverResponse);
            stackRet = OC_STACK_NO_MEMORY;
        }
    }
exit:
    return stackRet;
}
