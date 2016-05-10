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

/*
 * Server callback structures and functions
 */

#ifndef OC_SERVERCALLBACKS_H_
#define OC_SERVERCALLBACKS_H_

/**
 * Incoming requests handled by the server. Requests are passed in as a parameter to the
 * OCEntityHandler callback API.
 * The OCEntityHandler callback API must be implemented in the application in order
 * to receive these requests.
 */
typedef struct
{
    /** Associated resource.*/
    OCResourceHandle resource;

    /** Associated request handle.*/
    OCRequestHandle requestHandle;

    /** the REST method retrieved from received request PDU.*/
    OCMethod method;

    /** description of endpoint that sent the request.*/
    OCDevAddr devAddr;

    /** resource query send by client.*/
    char *query;

    /** Information associated with observation - valid only when OCEntityHandler flag includes
     * ::OC_OBSERVE_FLAG.*/
    OCObservationInfo obsInfo;

    /** Number of the received vendor specific header options.*/
    uint8_t numRcvdVendorSpecificHeaderOptions;

    /** Pointer to the array of the received vendor specific header options.*/
    OCHeaderOption *rcvdVendorSpecificHeaderOptions;

    /** the payload from the request PDU.*/
    OCPayload *payload;
} OCEntityHandlerRequest;

/**
 * Request handle is passed to server via the entity handler for each incoming request.
 * Stack assigns when request is received, server sets to indicate what request response is for.
 */
typedef struct
{
    /** Allow the entity handler to pass a result with the response.*/
    OCEntityHandlerResult  ehResult;

    /** Request handle.*/
    OCRequestHandle requestHandle;

    /** Resource handle.*/
    OCResourceHandle resourceHandle;

    /** This is the pointer to server payload data to be transferred.*/
    OCPayload* payload;

    /** number of the vendor specific header options .*/
    uint8_t numSendVendorSpecificHeaderOptions;

    /** An array of the vendor specific header options the entity handler wishes to use in response.*/
    OCHeaderOption sendVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];

    /** URI of new resource that entity handler might create.*/
    char resourceUri[URI_RESOURCE_LENGTH];

    /** Server sets to true for persistent response buffer,false for non-persistent response buffer*/
    uint8_t persistentBufferFlag;
} OCEntityHandlerResponse;

/*
 * -------------------------------------------------------------------------------------------
 * Callback function definitions
 * -------------------------------------------------------------------------------------------
 */

/**
 * Application server implementations must implement this callback to consume requests OTA.
 * Entity handler callback needs to fill the resPayload of the entityHandlerRequest.
 */
typedef OCEntityHandlerResult (*OCEntityHandler)
(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest, void* callbackParam);

/**
 * Device Entity handler need to use this call back instead of OCEntityHandler.
 */
typedef OCEntityHandlerResult (*OCDeviceEntityHandler)
(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest, char* uri, void* callbackParam);

/**
 * The signature of the internal call back functions to handle responses from entity handler
 */
typedef OCStackResult (* OCEHResponseHandler)(OCEntityHandlerResponse * ehResponse);

#endif // OC_SERVERCALLBACKS_H_
