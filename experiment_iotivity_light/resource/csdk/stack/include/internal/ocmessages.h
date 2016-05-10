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
 * Message structure up and down in the stack.
 *
 */

#ifndef OC_MESSAGES_H_
#define OC_MESSAGES_H_

#include "ocenums.h"
#include "ocpayloads.h"
#include "ocservercallbacks.h"

/**
 * Base Information received
 *
 * This structure is used to hold request & response base information
 */
typedef struct
{
    OCMessageType_t coapType;   /**< coap message type (CON, NON, ACK, RST) */
    uint16_t messageId;         /**< Message id */
    OCToken_t token;            /**< Token for CA */
    OCHeaderOption options[MAX_HEADER_OPTIONS]; /** header options for app */
    uint8_t numOptions;         /**< Number of Header options */
    OCPayloadType payloadType;
    void *parsedPayload;        /**< parsed incoming payload */
    char resourcePath[URI_RESOURCE_LENGTH]; /**< Resource URI information **/
    char resourceQuery[URI_QUERY_LENGTH]; /**< Resource URI information **/
    char observationOption;     /**< observation type **/
    OCRemoteId_t identity;      /**< endpoint identity */
    int coapCode;               /**< method (request) or result (response) */
} CAInfo_t;

typedef struct
{
    OCDataType_t type;          /**< Request, Response, Error */
    OCDevAddr devAddr;          /**< Where the message came from */
    CAInfo_t info;              /**< The real information (all types) */
    void *tempPDU;              /**< short term pointer to pdu */
    uint8_t *encodedPayload;    /**< encoded payload */
    size_t encodedSize;         /**< size in bytes of the payload */
    int messageLength;          /**< identify message for debug only */
} CAMessageInfo_t;

/**
 * CA Remote Access information for XMPP Client
 *
 */
typedef struct
{
    char *hostname;     /**< XMPP server hostname */
    uint16_t port;      /**< XMPP server serivce port */
    char *xmpp_domain;  /**< XMPP login domain */
    char *username;     /**< login username */
    char *password;     /**< login password */
    char *resource;     /**< specific resource for login */
    char *user_jid;     /**< specific JID for login */
} CARAInfo_t;

/**
 * Following structure will be created in ocstack to aggregate responses
 * (in future: for block transfer)
 */
typedef struct OCServerResponse {
    struct OCServerResponse *next;  // Linked list
    OCPayload *payload;             // server payload data to be transferred
    uint16_t remainingPayloadSize;  // Remaining size to be transferred
    OCRequestHandle requestHandle;  // Requests to handle
} OCServerResponse;

#define REQUEST_SIGNATURE (0x12345678)

typedef struct OCRequestInfo
{
    CAMessageInfo_t messageInfo;    // must be first

    struct OCRequestInfo *next;     // for the linked list of active requests

    int signature;                  // should have value REQUEST_SIGNATURE
    OCPayload *parsedPayload;
    OCQualityOfService qos;
    char observationOption;
    OCEHResponseHandler ehResponseHandler;
    OCVirtualResources virtualResourceType;
    uint8_t slowFlag;
    uint8_t notificationFlag;
    OCStackResult observeResult;
    int numResponses;
} OCRequestInfo_t;

#endif // OC_MESSAGES_H_
