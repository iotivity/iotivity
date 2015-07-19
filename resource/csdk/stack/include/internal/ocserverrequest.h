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

#ifndef OC_SERVER_REQUEST_H
#define OC_SERVER_REQUEST_H

#include "cacommon.h"
#include "cainterface.h"
/**
 * The signature of the internal call back functions to handle responses from entity handler
 */
typedef OCStackResult (* OCEHResponseHandler)(OCEntityHandlerResponse * ehResponse);

// following structure will be created in occoap and passed up the stack on the server side
typedef struct OCServerRequest
{
    // the REST method retrieved from received request PDU
    OCMethod method;
    // resourceUrl will be filled in occoap using the path options in received request PDU
    char resourceUrl[MAX_URI_LENGTH];
    // resource query send by client
    char query[MAX_QUERY_LENGTH];

    // qos is indicating if the request is CON or NON
    OCQualityOfService qos;
    // Observe option field
    uint32_t observationOption;
    OCStackResult observeResult;
    uint8_t numResponses;
    OCEHResponseHandler ehResponseHandler;
    /** Remote endpoint address **/
    OCDevAddr devAddr;
    // token for the request
    CAToken_t requestToken;
    // token length the request
    uint8_t tokenLength;
    // The ID of CoAP pdu                                   //Kept in
    uint16_t coapID;                                        //CoAP
    uint8_t delayedResNeeded;
    //////////////////////////////////////////////////////////
    // An array of the received vendor specific header options
    uint8_t numRcvdVendorSpecificHeaderOptions;
    OCHeaderOption rcvdVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];
    uint8_t requestComplete;
    struct OCServerRequest * next;
    // Flag indicating slow response
    uint8_t slowFlag;
    uint8_t notificationFlag;
    size_t payloadSize;
    // payload is retrieved from the payload of the received request PDU
    uint8_t payload[1];
} OCServerRequest;

// following structure will be created in ocstack to aggregate responses (in future: for block transfer)
typedef struct OCServerResponse {
    struct OCServerResponse * next;
    // this is the pointer to server payload data to be transferred
    OCPayload* payload;
    OCRequestHandle requestHandle;
} OCServerResponse;

/**
 * Handler function for sending a response from a single resource
 *
 * @param ehResponse - pointer to the response from the resource
 *
 * @return
 *     OCStackResult
 */
OCStackResult HandleSingleResponse(OCEntityHandlerResponse * ehResponse);

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
OCStackResult HandleAggregateResponse(OCEntityHandlerResponse * ehResponse);

/**
 * Get a server request from the server request list using the specified token.
 *
 * @param token - token of server request
 * @param tokenLength - length of token
 *
 * @return
 *     OCServerRequest*
 */
OCServerRequest * GetServerRequestUsingToken (const CAToken_t token, uint8_t tokenLength);

/**
 * Get a server request from the server request list using the specified handle
 *
 * @param handle - handle of server request
 * @return
 *     OCServerRequest*
 */
OCServerRequest * GetServerRequestUsingHandle (const OCServerRequest * handle);

/**
 * Get a server response from the server response list using the specified handle
 *
 * @param handle - handle of server response
 *
 * @return
 *     OCServerResponse*
 */
OCServerResponse * GetServerResponseUsingHandle (const OCServerRequest * handle);

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
 * @param devAddr - OCDevAddr
 *
 * @return
 *     OCStackResult
 */
OCStackResult AddServerRequest (OCServerRequest ** request, uint16_t coapID,
        uint8_t delayedResNeeded, uint8_t notificationFlag, OCMethod method,
        uint8_t numRcvdVendorSpecificHeaderOptions, uint32_t observationOption,
        OCQualityOfService qos, char * query,
        OCHeaderOption * rcvdVendorSpecificHeaderOptions,
        uint8_t * payload, CAToken_t requestToken,
        uint8_t tokenLength,
        char * resourceUrl, size_t reqTotalSize,
        const OCDevAddr *devAddr);

/**
 * Form the OCEntityHandlerRequest struct that is passed to a resource's entity handler
 *
 * @param entityHandlerRequest - pointer to the OCEntityHandlerRequest struct that is created
 * @param request          - request handle
 * @param method           - RESTful method
 * @param endpoint         - requesting endpoint
 * @param resource         - resource handle
 * @param queryBuf         - resource query of request
 * @param payload          - payload of request
 * @param payloadSize      - size of the payload request
 * @param numVendorOptions - number of vendor options
 * @param vendorOptions    - vendor options
 * @param observeAction    - observe action flag
 * @param observeID        - observe ID
 *
 * @return
 *     OCStackResult
 */
OCStackResult FormOCEntityHandlerRequest(
                                OCEntityHandlerRequest *entityHandlerRequest,
                                OCRequestHandle request,
                                OCMethod method,
                                OCDevAddr *endpoint,
                                OCResourceHandle resource,
                                char *queryBuf,
                                uint8_t *payload,
                                size_t payloadSize,
                                uint8_t numVendorOptions,
                                OCHeaderOption *vendorOptions,
                                OCObserveAction observeAction,
                                OCObservationId observeID);

/**
 * Find a server request in the server request list and delete
 *
 * @param serverRequest - server request to find and delete
 */
void FindAndDeleteServerRequest(OCServerRequest * serverRequest);

#endif //OC_SERVER_REQUEST_H

