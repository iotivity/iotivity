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
 * Client callback interface
 */

#ifndef OC_CLIENTCALLBACKS_H_
#define OC_CLIENTCALLBACKS_H_

/**
 * Response from queries to remote servers. Queries are made by calling the OCDoResource API.
 */
typedef struct
{
    /** Address of remote server.*/
    OCDevAddr devAddr;

    /** backward compatibility (points to devAddr).*/
    OCDevAddr *addr;

    /** backward compatibility.*/
    OCConnectivityType connType;

    /** the security identity of the remote server.*/
    OCIdentity identity;

    /** the is the result of our stack, OCStackResult should contain coap/other error codes.*/
    OCStackResult result;

    /** If associated with observe, this will represent the sequence of notifications from server.*/
    uint32_t sequenceNumber;

    /** resourceURI.*/
    const char *resourceUri;

    /** the payload for the response PDU.*/
    OCPayload *payload;

    /** Number of the received vendor specific header options.*/
    uint8_t numRcvdVendorSpecificHeaderOptions;

    /** An array of the received vendor specific header options.*/
    OCHeaderOption rcvdVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];
} OCClientResponse;

/**
 * Client applications implement this callback to consume responses received from Servers.
 */
typedef OCStackApplicationResult (* OCClientResponseHandler)(void *context, OCDoHandle handle,
    OCClientResponse * clientResponse);

/**
 * Client applications using a context pointer implement this callback to delete the
 * context upon removal of the callback/context pointer from the internal callback-list.
 */
typedef void (* OCClientContextDeleter)(void *context);

/**
 * This info is passed from application to OC Stack when initiating a request to Server.
 */
typedef struct OCCallbackData
{
    /** Pointer to the context.*/
    void *context;

    /** The pointer to a function the stack will call to handle the requests.*/
    OCClientResponseHandler cb;

    /** A pointer to a function to delete the context when this callback is removed.*/
    OCClientContextDeleter cd;

#ifdef SUPPORTS_DEFAULT_CTOR
    OCCallbackData() = default;
    OCCallbackData(void* ctx, OCClientResponseHandler callback, OCClientContextDeleter deleter)
        :context(ctx), cb(callback), cd(deleter){}
#endif
} OCCallbackData;

#endif //  OC_CLIENTCALLBACKS_H_
