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

#ifdef WITH_CLIENT

#include "oic_malloc.h"
#include "oic_string.h"
#include <string.h>

#ifdef WITH_ARDUINO
#include "Time.h"
#else
#include <sys/time.h>
#endif
//#include "coap_time.h"

#include "jutility.h"
#include "oclogger.h"
#include "occlientcb.h"
#include "ocadaptation.h"

/// Module Name
#define TAG PCF("occlientcb")

struct ClientCB *cbList = NULL;
static OCMulticastNode * mcPresenceNodes = NULL;

OCStackResult
AddClientCB (ClientCB **clientCB, OCCallbackData *cbData,
             const OCToken_t *token,
             OCDoHandle *handle, OCMethod method,
             OCDevAddr *devAddr, char *requestUri,
             char *resourceTypeName, uint32_t ttl)
{
    (void)resourceTypeName;
    ClientCB *cbNode = NULL;

    #ifdef WITH_PRESENCE
    if (method == OC_REST_PRESENCE)
    {   // Retrieve the presence callback structure for this specific requestUri.
        cbNode = GetClientCB(NULL, 0, requestUri);
    }
    #endif // WITH_PRESENCE

    if (!cbNode)// If it does not already exist, create new node.
    {
        cbNode = (ClientCB*) OICMalloc(sizeof(ClientCB));
        OC_LOG(INFO, TAG, PCF("Adding client callback with token"));
        OC_LOG_BUFFER(INFO, TAG, (const uint8_t *)&(token->t.chars), token->size);
        cbNode->callBack = cbData->cb;
        cbNode->context = cbData->context;
        cbNode->deleteCallback = cbData->cd;
        //Note: token memory is allocated in the caller OCDoResource
        //but freed in DeleteClientCB
        cbNode->token = *token;
        cbNode->handle = *handle;
        cbNode->method = method;
        cbNode->sequenceNumber = 0;
        #ifdef WITH_PRESENCE
        cbNode->presence = NULL;
        cbNode->filterResourceType = NULL;
        #endif // WITH_PRESENCE

        if (
        #ifdef WITH_PRESENCE
            method == OC_REST_PRESENCE ||
        #endif // WITH_PRESENCE
            method == OC_REST_OBSERVE  ||
            method == OC_REST_OBSERVE_ALL)
        {
            cbNode->TTL = 0;
        }
        else
        {
            cbNode->TTL = ttl;
        }
        OICStrcpy(cbNode->requestUri, sizeof (cbNode->requestUri), requestUri);
        cbNode->devAddr = *devAddr;
        OC_LOG_V(INFO, TAG, "Added Callback for uri : %s", requestUri);
        LL_APPEND(cbList, cbNode);
        *clientCB = cbNode;
    }
    else
    {
        // Ensure that the handle the SDK hands back up to the application layer for the
        // OCDoResource call matches the found ClientCB Node.
        *clientCB = cbNode;

        if (cbData->cd)
        {
            cbData->cd(cbData->context);
        }

        *handle = cbNode->handle;
    }
    OICFree(requestUri);

    #ifdef WITH_PRESENCE
    if (method == OC_REST_PRESENCE && resourceTypeName)
    {
        // Amend the found or created node by adding a new resourceType to it.
        return InsertResourceTypeFilter(cbNode, (char *)resourceTypeName);
    }
    #endif

    return OC_STACK_OK;
}

void DeleteClientCB(ClientCB *cbNode)
{
    LL_DELETE(cbList, cbNode);
    OC_LOG (INFO, TAG, PCF("Deleting token"));
    OC_LOG_BUFFER(INFO, TAG, cbNode->token.t.chars, cbNode->token.size);
    if (cbNode->deleteCallback)
    {
        cbNode->deleteCallback(cbNode->context);
    }

    #ifdef WITH_PRESENCE
    if (cbNode->presence)
    {
        OICFree(cbNode->presence->timeOut);
        OICFree(cbNode->presence);
    }
    if (cbNode->method == OC_REST_PRESENCE)
    {
        OCResourceTypeCB *pointer = cbNode->filterResourceType;
        OCResourceTypeCB *next = NULL;
        while (pointer)
        {
            next = pointer->next;
            OICFree(pointer->name);
            OICFree(pointer);
            pointer = next;
        }
    }
    #endif // WITH_PRESENCE
    OICFree(cbNode);
}

/*
 * This function checks if the node is past its time to live and
 * deletes it if timed-out. Calling this function with a  presence or observe
 * callback with ttl set to 0 will not delete anything as presence nodes have
 * their own mechanisms for timeouts. A null argument will cause the function to
 * silently return.
 */
static void CheckAndDeleteTimedOutCB(ClientCB* cbNode)
{
    if (!cbNode)
    {
        return;
    }
    if (cbNode->TTL == 0)
    {
        return;
    }
    if (cbNode->TTL < GetTicks(0))
    {
        OC_LOG(INFO, TAG, PCF("Deleting timed-out callback"));
        DeleteClientCB(cbNode);
    }
}

ClientCB* GetClientCB(const OCToken_t *token, OCDoHandle handle, const char *requestUri)
{

    ClientCB* out = NULL;

    if (token)
    {
        OC_LOG (INFO, TAG, PCF ("Looking for token"));
        OC_LOG_BUFFER(INFO, TAG, token->t.chars, token->size);
        OC_LOG(INFO, TAG, PCF("\tFound in callback list"));
        LL_FOREACH(cbList, out)
        {
            OC_LOG_BUFFER(INFO, TAG, out->token.t.chars, out->token.size);

            if (out->token.t.value == token->t.value)
            {
                return out;
            }
            CheckAndDeleteTimedOutCB(out);
        }
    }
    else if (handle)
    {
        LL_FOREACH(cbList, out)
        {
            if (out->handle == handle)
            {
                return out;
            }
            CheckAndDeleteTimedOutCB(out);
        }
    }
    else if (requestUri)
    {
        OC_LOG_V(INFO, TAG, "Looking for uri %s", requestUri);
        LL_FOREACH(cbList, out)
        {
            OC_LOG_V(INFO, TAG, "\tFound %s", out->requestUri);
            if (out->requestUri && strcmp(out->requestUri, requestUri ) == 0)
            {
                return out;
            }
            CheckAndDeleteTimedOutCB(out);
        }
    }
    OC_LOG(INFO, TAG, PCF("Callback Not found !!"));
    return NULL;
}

#ifdef WITH_PRESENCE
OCStackResult InsertResourceTypeFilter(ClientCB *cbNode, char *resourceTypeName)
{
    OCResourceTypeCB *newResourceType = NULL;
    if (cbNode && resourceTypeName[0])
    {
        // Form a new resourceType member.
        newResourceType = (OCResourceTypeCB *)OICMalloc(sizeof (OCResourceText_t));
        if (!newResourceType)
        {
            return OC_STACK_NO_MEMORY;
        }

        newResourceType->next = NULL;
        OICStrcpy(newResourceType->name, URI_RESOURCE_LENGTH, resourceTypeName);

        LL_APPEND(cbNode->filterResourceType, newResourceType);
        return OC_STACK_OK;
    }
    return OC_STACK_ERROR;
}
#endif // WITH_PRESENCE

void DeleteClientCBList()
{
    ClientCB* out;
    LL_FOREACH_SAFE(cbList, out)
    {
        DeleteClientCB(out);
    }
    cbList = NULL;
}

void FindAndDeleteClientCB(ClientCB * cbNode)
{
    ClientCB* tmp;
    if (cbNode)
    {
        LL_FOREACH(cbList, tmp)
        {
            if (cbNode == tmp)
            {
                DeleteClientCB(tmp);
                break;
            }
        }
    }
}

OCStackResult AddMCPresenceNode(OCMulticastNode** outnode, const char* uri, uint32_t nonce)
{
    if(!outnode)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCMulticastNode *node;

    node = (OCMulticastNode*) OICMalloc(sizeof(OCMulticastNode));

    if (node)
    {
        node->nonce = nonce;
        OICStrcpy(node->uri, sizeof (node->uri), uri);
        LL_APPEND(mcPresenceNodes, node);
        *outnode = node;
        return OC_STACK_OK;
    }
    *outnode = NULL;
    return OC_STACK_NO_MEMORY;
}

OCMulticastNode* GetMCPresenceNode(const char * uri)
{
    OCMulticastNode* out = NULL;

    if(uri)
    {
        LL_FOREACH(mcPresenceNodes, out)
        {
            if(out->uri && strcmp(out->uri, uri) == 0)
            {
                return out;
            }
        }
    }
    OC_LOG(INFO, TAG, PCF("MulticastNode Not found !!"));
    return NULL;
}

#endif // WITH_CLIENT
