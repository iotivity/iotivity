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

//-------------------------------------------------------------------------------------------------
// Local functions
//-------------------------------------------------------------------------------------------------

/**
 * Add a server response to the server response list
 *
 * @param response initialized server response that is created by this function
 * @param requestHandle - handle of the response
 *
 * @return
 *     OCStackResult
 */
static OCStackResult AddServerResponse (OCServerResponse ** response, OCRequestHandle requestHandle)
{
    OCServerResponse * serverResponse = NULL;

    serverResponse = (OCServerResponse *) OCCalloc(1, sizeof(OCServerResponse));
    VERIFY_NON_NULL(serverResponse);

    serverResponse->payload = (char *) OCCalloc(1, MAX_RESPONSE_LENGTH);
    VERIFY_NON_NULL(serverResponse->payload);

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

/**
 * Delete a server request from the server request list
 *
 * @param serverRequest - server request to delete
 */
static void DeleteServerRequest(OCServerRequest * serverRequest)
{
    if(serverRequest)
    {
        LL_DELETE(serverRequestList, serverRequest);
        OCFree(serverRequest->requestToken);
        OCFree(serverRequest);
        serverRequest = NULL;
        OC_LOG(INFO, TAG, PCF("Server Request Removed!!"));
    }
}

/**
 * Delete a server response from the server response list
 *
 * @param serverResponse - server response to delete
 */
static void DeleteServerResponse(OCServerResponse * serverResponse)
{
    if(serverResponse)
    {
        LL_DELETE(serverResponseList, serverResponse);
        OCFree(serverResponse->payload);
        OCFree(serverResponse);
        OC_LOG(INFO, TAG, PCF("Server Response Removed!!"));
    }
}

/**
 * Find a server response and delete it from the server response list
 *
 * @param serverResponse - server response to find and delete
 */
static void FindAndDeleteServerResponse(OCServerResponse * serverResponse)
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

//-------------------------------------------------------------------------------------------------
// Internal APIs
//-------------------------------------------------------------------------------------------------

/**
 * Get a server request from the server request list using the specified token.
 *
 * @param token - token of server request
 * @param tokenLength - length of token
 *
 * @return
 *     OCServerRequest*
 */
OCServerRequest * GetServerRequestUsingToken (const CAToken_t token, uint8_t tokenLength)
{
    if(!token)
    {
        OC_LOG(ERROR, TAG, PCF("Invalid Parameter Token"));
        return NULL;
    }

    OCServerRequest * out = NULL;
    LL_FOREACH (serverRequestList, out)
    {
        OC_LOG(INFO, TAG,PCF("comparing tokens"));
        OC_LOG_BUFFER(INFO, TAG, (const uint8_t *)token, tokenLength);
        OC_LOG_BUFFER(INFO, TAG, (const uint8_t *)out->requestToken, tokenLength);
        if(memcmp(out->requestToken, token, tokenLength) == 0)
        {
            return out;
        }
    }
    OC_LOG(ERROR, TAG, PCF("Server Request not found!!"));
    return NULL;
}

/**
 * Get a server request from the server request list using the specified handle
 *
 * @param handle - handle of server request
 * @return
 *     OCServerRequest*
 */
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
    OC_LOG(ERROR, TAG, PCF("Server Request not found!!"));
    return NULL;
}

/**
 * Get a server response from the server response list using the specified handle
 *
 * @param handle - handle of server response
 *
 * @return
 *     OCServerResponse*
 */
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
    OC_LOG(ERROR, TAG, PCF("Server Response not found!!"));
    return NULL;
}

/**
 * Add a server request to the server request list
 *
 * @param request - initialized server request that is created by this function
 * @param coapID - ID of CoAP pdu
 * @param delayedResNeeded - delayed response required 0=no 1=yes
 * @param secured - secure endpoint 0=no 1=yes
 * @param notificationFlag - //TODO: remove - does not appear to be used any longer
 * @param method - RESTful method
 * @param numRcvdVendorSpecificHeaderOptions - number of received vendor specific header options
 * @param observationOption - value of observation option
 * @param qos - request QOS
 * @param query - request query
 * @param rcvdVendorSpecificHeaderOptions - received vendor specific header options
 * @param reqJSONPayload - request JSON payload
 * @param requestToken - request token
 * @param tokenLength - request token length
 * @param resourceUrl - URL of resource
 * @param reqTotalSize - total size of the request
 * @param addressInfo - CA Address
 * @param connectivityType - connection type
 *
 * @return
 *     OCStackResult
 */
OCStackResult AddServerRequest (OCServerRequest ** request, uint16_t coapID,
        uint8_t delayedResNeeded, uint8_t secured, uint8_t notificationFlag, OCMethod method,
        uint8_t numRcvdVendorSpecificHeaderOptions, uint32_t observationOption,
        OCQualityOfService qos, char * query,
        OCHeaderOption * rcvdVendorSpecificHeaderOptions,
        char * reqJSONPayload, CAToken_t requestToken,
        uint8_t tokenLength,
        char * resourceUrl, size_t reqTotalSize,
        CAAddress_t *addressInfo, CATransportType_t connectivityType)
{
    OCServerRequest * serverRequest = NULL;

    //Note: OCServerRequest includes 1 byte for the JSON Payload.  payloadSize is calculated
    //as the required length of the string, so this will result in enough room for the
    //null terminator as well.
    serverRequest = (OCServerRequest *) OCCalloc(1, sizeof(OCServerRequest) +
        (reqTotalSize ? reqTotalSize : 1) - 1);
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
    if(reqJSONPayload && reqTotalSize)
    {
        // destination is at least 1 greater than the source, so a NULL always exists in the
        // last character
        strncpy((char*)serverRequest->reqJSONPayload,
                (const char*)reqJSONPayload, reqTotalSize - 1);
    }
    serverRequest->requestComplete = 0;
    if(requestToken)
    {
        // If tokenLength is zero, the return value depends on the
        // particular library implementation (it may or may not be a null pointer).
        if (tokenLength)
        {
            serverRequest->requestToken = (CAToken_t) OCMalloc(tokenLength);
            VERIFY_NON_NULL(serverRequest->requestToken);
            memcpy(serverRequest->requestToken, requestToken, tokenLength);
        }

    }
    serverRequest->tokenLength = tokenLength;

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

/**
 * Form the OCEntityHandlerRequest struct that is passed to a resource's entity handler
 *
 * @param entityHandlerRequest - pointer to the OCEntityHandlerRequest struct that is created
 * @param request          - request handle
 * @param method           - RESTful method
 * @param resource         - resource handle
 * @param queryBuf         - resource query of request
 * @param bufReqPayload    - JSON payload of request
 * @param numVendorOptions - number of vendor options
 * @param vendorOptions    - vendor options
 * @param observeAction    - observe action flag
 * @param observeID        - observe ID
 *
 * @return
 *     OCStackResult
 */
OCStackResult FormOCEntityHandlerRequest(
        OCEntityHandlerRequest * entityHandlerRequest,
        OCRequestHandle request,
        OCMethod method,
        OCResourceHandle resource,
        char * queryBuf,
        char * bufReqPayload,
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

/**
 * Find a server request in the server request list and delete
 *
 * @param serverRequest - server request to find and delete
 */
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

CAResponseResult_t ConvertEHResultToCAResult (OCEntityHandlerResult result)
{
    CAResponseResult_t caResult = CA_BAD_REQ;

    switch (result)
    {
        case OC_EH_OK:
            caResult = CA_SUCCESS;
            break;
        case OC_EH_ERROR:
            caResult = CA_BAD_REQ;
            break;
        case OC_EH_RESOURCE_CREATED:
            caResult = CA_CREATED;
            break;
        case OC_EH_RESOURCE_DELETED:
            caResult = CA_DELETED;
            break;
        case OC_EH_SLOW:
            caResult = CA_SUCCESS;
            break;
        case OC_EH_FORBIDDEN:
            caResult = CA_BAD_REQ;
            break;
        case OC_EH_RESOURCE_NOT_FOUND:
            caResult = CA_NOT_FOUND;
            break;
        default:
            caResult = CA_BAD_REQ;
            break;
    }
    return caResult;
}


/**
 * Handler function for sending a response from a single resource
 *
 * @param ehResponse - pointer to the response from the resource
 *
 * @return
 *     OCStackResult
 */

OCStackResult HandleSingleResponse(OCEntityHandlerResponse * ehResponse)
{
    OCStackResult result = OC_STACK_ERROR;
    CARemoteEndpoint_t responseEndpoint = {};
    CAResponseInfo_t responseInfo = {};
    CAHeaderOption_t* optionsPointer = NULL;

    OC_LOG_V(INFO, TAG, "Inside HandleSingleResponse: %s", ehResponse->payload);

    if(!ehResponse || !ehResponse->requestHandle)
    {
        return OC_STACK_ERROR;
    }

    if(ehResponse->payloadSize >= (MAX_RESPONSE_LENGTH))// - OC_JSON_PREFIX_LEN - OC_JSON_SUFFIX_LEN))
    {
        OC_LOG_V(ERROR, TAG, "Response payload size was too large.  Max is %hu, payload size was %hu",
                (MAX_RESPONSE_LENGTH - OC_JSON_PREFIX_LEN - OC_JSON_SUFFIX_LEN), ehResponse->payloadSize);
        return OC_STACK_INVALID_PARAM;
    }

    OCServerRequest *serverRequest = (OCServerRequest *)ehResponse->requestHandle;

    // Copy the address
    responseEndpoint.resourceUri      = (CAURI_t) serverRequest->resourceUrl;
    responseEndpoint.addressInfo      = serverRequest->addressInfo;
    responseEndpoint.transportType    = serverRequest->connectivityType;
    responseEndpoint.isSecured        = serverRequest->secured;

    responseInfo.result = ConvertEHResultToCAResult(ehResponse->ehResult);

    if(serverRequest->notificationFlag && serverRequest->qos == OC_HIGH_QOS)
    {
        responseInfo.info.type = CA_MSG_CONFIRM;
    }
    else if(serverRequest->notificationFlag && serverRequest->qos != OC_HIGH_QOS)
    {
        responseInfo.info.type = CA_MSG_NONCONFIRM;
    }
    else if(!serverRequest->notificationFlag && !serverRequest->slowFlag &&
            serverRequest->qos == OC_HIGH_QOS)
    {
        responseInfo.info.type = CA_MSG_ACKNOWLEDGE;
    }
    else if(!serverRequest->notificationFlag && serverRequest->slowFlag &&
            serverRequest->qos == OC_HIGH_QOS)
    {
        responseInfo.info.type = CA_MSG_CONFIRM;
    }
    else if(!serverRequest->notificationFlag)
    {
        responseInfo.info.type = CA_MSG_NONCONFIRM;
    }

    responseInfo.info.messageId = serverRequest->coapID;
    responseInfo.info.token = (CAToken_t)OCMalloc(CA_MAX_TOKEN_LEN+1);
    if (!responseInfo.info.token)
    {
        OC_LOG(FATAL, TAG, "Response Info Token is NULL");
        return result;
    }

    memcpy(responseInfo.info.token, serverRequest->requestToken, serverRequest->tokenLength);
    responseInfo.info.tokenLength = serverRequest->tokenLength;

    if(serverRequest->observeResult == OC_STACK_OK)
    {
        responseInfo.info.numOptions = ehResponse->numSendVendorSpecificHeaderOptions + 1;
    }
    else
    {
        responseInfo.info.numOptions = ehResponse->numSendVendorSpecificHeaderOptions;
    }

    if(responseInfo.info.numOptions > 0)
    {
        responseInfo.info.options = (CAHeaderOption_t *)
                                      OCCalloc(responseInfo.info.numOptions,
                                              sizeof(CAHeaderOption_t));

        if(!responseInfo.info.options)
        {
            OC_LOG(FATAL, TAG, PCF("options is NULL"));
            OCFree(responseInfo.info.token);
            return OC_STACK_NO_MEMORY;
        }

        optionsPointer = responseInfo.info.options;

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
                            sizeof(OCHeaderOption) *
                            ehResponse->numSendVendorSpecificHeaderOptions);
        }
    }
    else
    {
        responseInfo.info.options = NULL;
    }

    char payload[MAX_RESPONSE_LENGTH + OC_JSON_PREFIX_LEN + OC_JSON_SUFFIX_LEN] = {};

    // Put the JSON prefix and suffix around the payload
    strcpy(payload, (const char *)OC_JSON_PREFIX);
    strncat(payload, (const char *)ehResponse->payload, ehResponse->payloadSize);
    strcat(payload, (const char *)OC_JSON_SUFFIX);
    responseInfo.info.payload = (CAPayload_t)payload;

    #ifdef WITH_PRESENCE
    //TODO: Add other connectivity types to CAConnTypes[] when enabled
    CATransportType_t CAConnTypes[] = {CA_IPV4};
    const char * connTypes[] = {"ip transport"};
    int size = sizeof(CAConnTypes)/ sizeof(CATransportType_t);
    CATransportType_t connType = responseEndpoint.transportType;
    CAResult_t caResult = CA_STATUS_FAILED;
    result = OC_STACK_OK;

    //Sending response on all n/w interfaces
    for(int i = 0; i < size; i++ )
    {
        responseEndpoint.transportType = (CATransportType_t)(connType & CAConnTypes[i]);
        if(responseEndpoint.transportType)
        {
            //The result is set to OC_STACK_OK only if CASendResponse succeeds in sending the
            //response on all the n/w interfaces else it is set to OC_STACK_ERROR
            caResult = CASendResponse(&responseEndpoint, &responseInfo);
            if(caResult != CA_STATUS_OK)
            {
                OC_LOG_V(ERROR, TAG, "CASendResponse failed on %s", connTypes[i]);
                result = CAResultToOCResult(caResult);
            }
            else
            {
                OC_LOG_V(INFO, TAG, "CASendResponse succeeded on %s", connTypes[i]);
            }
        }
    }
    #else
    CAResult_t caResult = CASendResponse(&responseEndpoint, &responseInfo);
    if(caResult != CA_STATUS_OK)
    {
        OC_LOG(ERROR, TAG, PCF("CASendResponse failed"));
        result = CAResultToOCResult(caResult);
    }
    else
    {
        result = OC_STACK_OK;
    }
    #endif

    OCFree(responseInfo.info.token);
    OCFree(responseInfo.info.options);
    //Delete the request
    FindAndDeleteServerRequest(serverRequest);
    return result;
}

/**
 * Handler function for sending a response from multiple resources, such as a collection.
 * Aggregates responses from multiple resource until all responses are received then sends the
 * concatenated response
 *
 * TODO: Need to add a timeout in case a (remote?) resource does not respond
 *
 * @param ehResponse - pointer to the response from the resource
 *
 * @return
 *     OCStackResult
 */
OCStackResult HandleAggregateResponse(OCEntityHandlerResponse * ehResponse)
{
    OCStackResult stackRet = OC_STACK_ERROR;
    OCServerRequest * serverRequest = NULL;
    OCServerResponse * serverResponse = NULL;
    uint16_t bufferNeeded = 0;

    if(!ehResponse || !ehResponse->payload)
    {
        OC_LOG(ERROR, TAG, PCF("HandleAggregateResponse invalid parameters"));
        return OC_STACK_INVALID_PARAM;
    }

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
            OC_LOG(ERROR, TAG, PCF("There is room in response buffer"));
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
            OC_LOG(ERROR, TAG, PCF("No room in response buffer"));
            //Delete the request and response
            FindAndDeleteServerRequest(serverRequest);
            FindAndDeleteServerResponse(serverResponse);
            stackRet = OC_STACK_NO_MEMORY;
        }
    }
exit:
    return stackRet;
}

