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
#include <string.h>

#include "ocstack.h"
#include "ocserverrequest.h"
#include "ocresourcehandler.h"
#include "ocmalloc.h"

#include "cacommon.h"
#include "cainterface.h"

#include "utlist.h"
#include "pdu.h"

// Module Name
#define VERIFY_NON_NULL(arg) { if (!arg) {OC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} }

#define TAG  PCF("ocserverrequest")

static struct OCServerRequest * serverRequestList = NULL;
static struct OCServerResponse * serverResponseList = NULL;

OCServerRequest * GetServerRequestUsingToken (const CAToken_t token)
{
    OCServerRequest * out = NULL;
    LL_FOREACH (serverRequestList, out)
    {
        OC_LOG(INFO, TAG,PCF("comparing tokens"));
        OC_LOG_BUFFER(INFO, TAG, (const uint8_t *)token, CA_MAX_TOKEN_LEN);
        OC_LOG_BUFFER(INFO, TAG, (const uint8_t *)out->requestToken, CA_MAX_TOKEN_LEN);
        if(memcmp(out->requestToken, token, CA_MAX_TOKEN_LEN) == 0)
        {
            return out;
        }
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

OCStackResult AddServerRequest (OCServerRequest ** request, uint16_t coapID,
        uint8_t delayedResNeeded, uint8_t secured, uint8_t notificationFlag, OCMethod method,
        uint8_t numRcvdVendorSpecificHeaderOptions, uint32_t observationOption,
        OCQualityOfService qos, unsigned char * query,
        OCHeaderOption * rcvdVendorSpecificHeaderOptions,
        unsigned char * reqJSONPayload, CAToken_t * requestToken,
        unsigned char * resourceUrl, size_t reqTotalSize,
        CAAddress_t *addressInfo, CAConnectivityType_t connectivityType)
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
        strncpy((char*)serverRequest->query,
                (const char*)query, sizeof(serverRequest->query) - 1);
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
        serverRequest->requestToken = (CAToken_t)OCMalloc(CA_MAX_TOKEN_LEN+1);
        VERIFY_NON_NULL (serverRequest->requestToken);
        memset(serverRequest->requestToken, 0, CA_MAX_TOKEN_LEN + 1);
        memcpy(serverRequest->requestToken, *requestToken, CA_MAX_TOKEN_LEN);
    }

    if(resourceUrl)
    {
        strncpy((char*)serverRequest->resourceUrl,
                (const char*)resourceUrl, sizeof(serverRequest->resourceUrl) - 1);
    }

    if (addressInfo)
    {
        serverRequest->addressInfo = *addressInfo;
    }
    serverRequest->connectivityType = connectivityType;

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
OCStackResult FormOCEntityHandlerRequest(
        OCEntityHandlerRequest * entityHandlerRequest,
        OCRequestHandle request,
        OCMethod method,
        OCResourceHandle resource,
        unsigned char * queryBuf,
        unsigned char * bufReqPayload,
        uint8_t numVendorOptions,
        OCHeaderOption * vendorOptions,
        OCObserveAction observeAction,
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
    OCStackResult result = OC_STACK_ERROR;
    CARemoteEndpoint_t responseEndpoint = {};
    CAResponseInfo_t responseInfo = {};
    CAHeaderOption_t* optionsPointer;

    OC_LOG_V(INFO, TAG, "Inside HandleSingleResponse: %s", ehResponse->payload);

    OCServerRequest *serverRequest = (OCServerRequest *)ehResponse->requestHandle;

    // Copy the address
    responseEndpoint.resourceUri      = (CAURI_t) serverRequest->resourceUrl;
    responseEndpoint.addressInfo      = serverRequest->addressInfo;
    responseEndpoint.connectivityType = serverRequest->connectivityType;
    responseEndpoint.isSecured        = (CABool_t) serverRequest->secured;

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
            responseInfo.result = CA_UNAUTHORIZED_REQ;
            break;
        default:
            responseInfo.result = CA_BAD_REQ;
            break;
    }

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

    // TODO: This exposes CoAP specific details.  At some point, this should be
    // re-factored and handled in the CA layer.
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
    // Check the payload size with OC_JSON_PREFIX and OC_JSON_SUFFIX including NULL terminated
    // string is less than MAX_RESPONSE_LENGTH.
    int len = ehResponse->payloadSize + strlen(OC_JSON_PREFIX) + strlen(OC_JSON_SUFFIX) + 1;
    if(len > MAX_RESPONSE_LENGTH)
    {
        return OC_STACK_ERROR;
    }
    // Allocate memory for the payload.
    char *payload = (char *)OCCalloc(1, MAX_RESPONSE_LENGTH);
    if(!payload)
    {
        return OC_STACK_NO_MEMORY;
    }
    // Put the JSON prefix and suffix around the payload
    strcpy(payload, (const char *)OC_JSON_PREFIX);
    if(ehResponse->payloadSize)
    {
        strncat(payload, (const char *)ehResponse->payload, ehResponse->payloadSize);
    }
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
}

OCStackResult HandleAggregateResponse(OCEntityHandlerResponse * ehResponse)
{
    OCStackResult stackRet = OC_STACK_ERROR;
    OCServerRequest * serverRequest = NULL;
    OCServerResponse * serverResponse = NULL;
    uint16_t bufferNeeded = 0;

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

        // If there is more than 1 response, then we need to allow for a null-termination
        // in the server response payload buffer AND the JSON response separator
        bufferNeeded = ehResponse->payloadSize + 1;
        if (serverRequest->numResponses > 1)
        {
            bufferNeeded += strlen(OC_JSON_SEPARATOR_STR);
        }
        if(serverResponse->remainingPayloadSize >= bufferNeeded)
        {
            OC_LOG(INFO, TAG, PCF("There is room in response buffer"));
            // append
            strncat((char *)serverResponse->payload,
                    (char *)ehResponse->payload,
                    serverResponse->remainingPayloadSize);
            OC_LOG_V(INFO, TAG, "Current aggregated response  ...%s", serverResponse->payload);
            serverResponse->remainingPayloadSize -= strlen((char *)ehResponse->payload);
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
                OC_LOG(INFO, TAG, PCF("More response fragments to come"));
                strncat((char *)serverResponse->payload,
                        OC_JSON_SEPARATOR_STR,
                        serverResponse->remainingPayloadSize);
                OC_LOG_V(INFO, TAG, "Current aggregated response  ...%s", serverResponse->payload);
                serverResponse->remainingPayloadSize -= strlen(OC_JSON_SEPARATOR_STR);
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
