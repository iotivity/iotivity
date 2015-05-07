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
//

#ifndef OC_CLIENT_CB
#define OC_CLIENT_CB

#include "ocstack.h"

#include "ocresource.h"
#include "cacommon.h"

typedef struct OCPresence
{
    // This is the TTL associated with presence
    uint32_t TTL;
    uint32_t * timeOut;
    uint32_t TTLlevel;
} OCPresence;

typedef struct OCMulticastNode
{
    char * uri;
    uint32_t nonce;
    struct OCMulticastNode * next;
} OCMulticastNode;

typedef struct resourcetype_t OCResourceType;

typedef struct ClientCB {
    // callback method defined in application address space
    OCClientResponseHandler callBack;
    // callback context data
    void * context;
    // callback method to delete context data
    OCClientContextDeleter deleteCallback;
    //  when a response is recvd with this token, above callback will be invoked
    CAToken_t token;
    // a response is recvd with this token length
    uint8_t tokenLength;
    // Invocation handle tied to original call to OCDoResource()
    OCDoHandle handle;
    // This is used to determine if all responses should be consumed or not.
    // (For now, only pertains to OC_REST_OBSERVE_ALL Vs. OC_REST_OBSERVE functionality)
    OCMethod method;
    // This is the sequence identifier the server applies to the invocation tied to 'handle'.
    uint32_t sequenceNumber;
    // This is the request uri associated with the call back
    char * requestUri;
    // Struct to hold TTL info for presence
    #ifdef WITH_PRESENCE
    OCPresence * presence;
    OCResourceType * filterResourceType;
    #endif
    // The connectivity type on which the request was sent on.
    OCConnectivityType conType;
    // The TTL for this callback. Holds the time till when this callback can
    // still be used. TTL is set to 0 when the callback is for presence and observe.
    // Presence has ttl mechanism in the "presence" member of this struct and observes
    // can be explicitly cancelled.
    uint32_t TTL;
    // next node in this list
    struct ClientCB    *next;
} ClientCB;

extern struct ClientCB *cbList;

/** @ingroup ocstack
 *
 * This method is used to add a client callback method in cbList.
 *
 * @param[out] clientCB
 *              The resulting node from making this call. Null if out of memory.
 * @param[in] cbData
 *              address to client callback function.
 * @param[in] token
 *              identifier for OTA CoAP comms.
 * @param[in] handle
 *              Masked in the public API as an 'invocation handle' - Used for callback management.
 * @param[in] method
 *              OCMethod via which this client callback is expected to operate
 * @param[in] requestUri
 *              the resource uri of the request.
 * @param[in] resourceType
 *              the resourceType associated with a presence request.
 * @param[in] conType
 *              the connectivity type on which the associated request for this clientCB was sent on.
 * @param[in] ttl
 *              time to live in coap_ticks for the callback.
 * @brief If the handle you're looking for does not exist, the stack will reply with a RST message.
 *
 * @return OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult
AddClientCB (ClientCB** clientCB, OCCallbackData* cbData,
             CAToken_t token, uint8_t tokenLength,
             OCDoHandle *handle, OCMethod method,
             char * requestUri, char * resourceTypeName, OCConnectivityType conType, uint32_t ttl);

/** @ingroup ocstack
 *
 * This method is used to remove a callback node from cbList.
 *
 * @param[in] cbNode
 *              address to client callback node.
 */
void DeleteClientCB(ClientCB *cbNode);


/** @ingroup ocstack
 *
 * This method is used to search and retrieve a cb node in cbList.
 *
 * @param[in] token
 *              token to search for.
 * @param[in] handle
 *              handle to search for.
 * @param[in] requestUri
 *              Uri to search for.
 *
 * @brief You can search by token OR by handle, but not both.
 *
 * @return address of the node if found, otherwise NULL
 */
ClientCB* GetClientCB(const CAToken_t token, uint8_t tokenLength,
        OCDoHandle handle, const char * requestUri);

/**
 * Inserts a new resource type filter into this cb node.
 *
 * @param[in] cbNode
 *              the node to add the new resourceType filter to
 * @param[in] resourceTypeName
 *              the value to create the new resourceType filter from
 *
 * @return
 *      OC_STACK_OK on success
 *      OC_STACK_ERROR with invalid parameters
 *      OC_STACK_NO_MEMORY when out of memory
 */
#ifdef WITH_PRESENCE
OCStackResult InsertResourceTypeFilter(ClientCB * cbNode, char * resourceTypeName);
#endif // WITH_PRESENCE

/** @ingroup ocstack
 *
 * This method is used to clear the cbList.
 *
 */
void DeleteClientCBList();

/** @ingroup ocstack
 *
 * This method is used to verify the presence of a cb node in cbList
 * and then delete it.
 *
 * @param[in] cbNode
 *              address to client callback node.
 */
void FindAndDeleteClientCB(ClientCB * cbNode);

/** @ingroup ocstack
 *
 * This method is used to search a multicast presence node from list.
 *
 * @param[in]  uri
 *              the uri of the request.
 *
 * @return OCMulticastNode
 *              The resulting node from making this call. Null if doesn't exist.
 */
//------------------------------------------------------------------------
OCMulticastNode* GetMCPresenceNode(const char * uri);

/** @ingroup ocstack
 *
 * This method is used to add a multicast presence node to the list.
 *
 * @param[out] outnode
 *              The resulting node from making this call. Null if out of memory.
 * @param[in] uri
 *              the uri of the server.
 * @param[in] nonce
 *              current nonce for the server
 *
 * @return OC_STACK_OK for Success, otherwise some error value
 */
//------------------------------------------------------------------------
OCStackResult AddMCPresenceNode(OCMulticastNode** outnode, char* uri, uint32_t nonce);

#endif //OC_CLIENT_CB

