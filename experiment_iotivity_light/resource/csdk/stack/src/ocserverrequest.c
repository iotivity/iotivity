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

#include "ocstack.h"
#include "ocresource.h"
#include "ocserverrequest.h"
#include "ocresourcehandler.h"
#include "ocadaptation.h"
#include "oic_string.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"

#include "jutility.h"

// Module Name
#define VERIFY_NON_NULLA(arg) { if (!arg) {OC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} }

#define TAG  PCF("ocserverrequest")

//-------------------------------------------------------------------------------------------------
// Local functions
//-------------------------------------------------------------------------------------------------

#ifdef WITH_COLLECTIONS

static struct OCServerResponse *serverResponseList = NULL;

/**
 * Add a server response to the server response list
 *
 * @param response initialized server response that is created by this function
 * @param requestHandle - handle of the response
 *
 * @return
 *     OCStackResult
 */
static OCServerResponse *AddServerResponse(OCRequestHandle requestHandle)
{
    OCServerResponse *response = (OCServerResponse *)OCCalloc(MAT_collection);

    response->requestHandle = requestHandle;

    LL_APPEND(serverResponseList, response);
    return response;
}

/**
 * Delete a server response from the server response list
 *
 * @param serverResponse - server response to delete
 */
static void DeleteServerResponse(OCServerResponse * serverResponse)
{
    if (serverResponse)
    {
        LL_DELETE(serverResponseList, serverResponse);
        OCFree(serverResponse->payload, MAT_payload);
        OCFree(serverResponse, MAT_collection);
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
 * Get a server response from the server response list using the specified handle
 *
 * @param handle - handle of server response
 *
 * @return
 *     OCServerResponse*
 */
OCServerResponse *GetServerResponseUsingHandle (const OCRequestInfo_t *handle)
{
    OCServerResponse *out = NULL;
    LL_FOREACH(serverResponseList, out)
    {
        if (out->requestHandle == handle)
        {
            return out;
        }
    }
    return NULL;
}
#endif // WITH_COLLECTIONS

void FillServerRequest(OCRequestInfo_t *request,
                                OCMethod method,
                                uint32_t observationOption,
                                OCQualityOfService qos,
                                char *query,
                                OCToken_t requestToken,
                                char *resourceUrl,
                                const OCDevAddr *devAddr)
{
    request->messageInfo.info.coapCode = method;
    request->observationOption = observationOption;
    request->qos = qos;
    OICStrcpy(request->messageInfo.info.resourceQuery,
                       sizeof (request->messageInfo.info.resourceQuery), query);
    OICStrcpy(request->messageInfo.info.resourcePath,
                  sizeof (request->messageInfo.info.resourcePath), resourceUrl);
    request->messageInfo.info.token = requestToken;
    request->messageInfo.devAddr = *devAddr;
    request->notificationFlag = 1; // notifications arrive this way
    request->ehResponseHandler = HandleSingleResponse;
}

OCStackResult FormOCEntityHandlerRequest(OCEntityHandlerRequest *ehr,
                                         OCRequestInfo_t *req,
                                         OCResource *resource)
{
    ehr->resource = (OCResourceHandle)resource;
    ehr->requestHandle = (OCRequestHandle)req;
    ehr->method = req->messageInfo.info.coapCode;
    ehr->query = req->messageInfo.info.resourceQuery;
    ehr->obsInfo.action = req->observationOption;
    ehr->payload = req->parsedPayload;
    ehr->numRcvdVendorSpecificHeaderOptions = req->messageInfo.info.numOptions;
    ehr->rcvdVendorSpecificHeaderOptions = (OCHeaderOption *)req->messageInfo.info.options;
    return OC_STACK_OK;
}

static CAResponseResult_t ConvertEHResultToCAResult(OCEntityHandlerResult result, OCMethod method)
{
    CAResponseResult_t caResult;

    switch (result)
    {
    case OC_EH_OK:
       switch (method)
       {
       case OC_REST_PUT:
       case OC_REST_POST:
           // This Response Code is like HTTP 204 "No Content" but only used
           // in response to POST and PUT requests.
           caResult = CA_CHANGED;
           break;
       case OC_REST_GET:
           // This Response Code is like HTTP 200 "OK" but only used
           // in response to GET requests.
           caResult = CA_CONTENT;
           break;
       default:
           // This should not happen, but give it a value just in case
           caResult = CA_CONTENT;
           OC_LOG_V(ERROR, TAG,
                   "Unexpected OC_EH_OK return code for method [d].", method);
        }
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
        caResult = CA_CONTENT;
        break;
    case OC_EH_FORBIDDEN:
        caResult = CA_UNAUTHORIZED_REQ;
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

static OCMessageType_t ComputeCoapType(OCRequestInfo_t *request)
{
    if (request->notificationFlag)
    {
        return request->qos == OC_HIGH_QOS ? OC_MSG_CONFIRM : OC_MSG_NONCONFIRM;
    }

    if (request->qos == OC_HIGH_QOS)
    {
        return request->slowFlag ? OC_MSG_CONFIRM : OC_MSG_ACKNOWLEDGE;
    }

    return OC_MSG_NONCONFIRM;
}

/**
 * Handler function for sending a response from a single resource
 * @param ehResponse - pointer to the response from the resource
 */
OCStackResult HandleSingleResponse(OCEntityHandlerResponse *ehResponse)
{
    if (ehResponse->numSendVendorSpecificHeaderOptions > MAX_HEADER_OPTIONS)
    {
        return OC_STACK_INVALID_PARAM;
    }
    OCStackResult result = OC_STACK_OK;

    // Copy from request to response
    OCRequestInfo_t *request = (OCRequestInfo_t *)ehResponse->requestHandle;
    CAMessageInfo_t *response = (CAMessageInfo_t *)request;
    CAInfo_t *info = &(response->info);

    OICStrcpy(info->resourcePath, sizeof (info->resourcePath),
                                       request->messageInfo.info.resourcePath);
    OICStrcpy(info->resourceQuery, sizeof (info->resourceQuery),
                                       request->messageInfo.info.resourceQuery);

    // Get from entity handler and put in response
    info->coapCode = ConvertEHResultToCAResult(ehResponse->ehResult,
                                            request->messageInfo.info.coapCode);
    info->coapType = ComputeCoapType(request);
    info->numOptions = ehResponse->numSendVendorSpecificHeaderOptions;
    memcpy(info->options, ehResponse->sendVendorSpecificHeaderOptions,
                                    sizeof (OCHeaderOption) * info->numOptions);
    if (ehResponse->payload)
    {
        response->encodedSize = MAX_ENCODED_PAYLOAD_LENGTH;
        result = OCConvertPayload(ehResponse->payload,
                              response->encodedPayload, &response->encodedSize);
        if (result != OC_STACK_OK)
        {
            return result;
        }
    }

    OC_LOG(INFO, TAG, PCF("Calling CASendResponse with:"));
    OC_LOG_V(INFO, TAG, "\tEndpoint address: %s", response->devAddr.addr);
    OC_LOG_V(INFO, TAG, "\tEndpoint adapter: %d", response->devAddr.adapter);
    OC_LOG_V(INFO, TAG, "\tResponse result : %d", info->coapCode);
    OC_LOG_V(INFO, TAG, "\tResponse for uri: %s?%s", info->resourcePath, info->resourceQuery);

    OCSendMessage(response);
    //Delete the request (which is also the response)
    OCFree(request, MAT_message);
    return OC_STACK_OK;
}

#ifdef WITH_COLLECTIONS

/**
 * Handler function for sending a response from multiple resources,
 * such as a collection. Aggregates responses from multiple resource until
 * all responses are received then sends the concatenated response
 *
 * TODO: Need to add a timeout in case a (remote?) resource does not respond
 */
OCStackResult HandleAggregateResponse(OCEntityHandlerResponse *ehResponse)
{
    if (!ehResponse || !ehResponse->payload)
    {
        OC_LOG(ERROR, TAG, PCF("HandleAggregateResponse invalid parameters"));
        return OC_STACK_INVALID_PARAM;
    }
    if (ehResponse->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        return OC_STACK_ERROR;
    }

    OCRequestInfo_t *request = (OCRequestInfo_t *)ehResponse->requestHandle;
    if (!request || request->signature != REQUEST_SIGNATURE)
    {
        return OC_STACK_ERROR;
    }

    OCServerResponse *response = GetServerResponseUsingHandle(request);
    if (!response)
    {
        OC_LOG(INFO, TAG, PCF("This is the first response fragment"));
        response = AddServerResponse(ehResponse->requestHandle);
    }

    if (response->payload)
    {
        OCRepPayloadAppend((OCRepPayload*)response->payload,
                                            (OCRepPayload*)ehResponse->payload);
    }
    else
    {
        response->payload = ehResponse->payload;
    }

    if (--request->numResponses)
    {
        OC_LOG(INFO, TAG, PCF("More response fragments to come"));
        return OC_STACK_OK;
    }

    OC_LOG(INFO, TAG, PCF("This is the last response fragment"));
    ehResponse->payload = response->payload;
    OCStackResult stackRet = HandleSingleResponse(ehResponse);
    //Delete the response
    FindAndDeleteServerResponse(response);

    return stackRet;
}

#endif // WITH_COLLECTIONS
