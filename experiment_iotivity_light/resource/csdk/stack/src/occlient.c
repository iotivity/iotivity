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


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

// Defining _POSIX_C_SOURCE macro with 200112L (or greater) as value
// causes header files to expose definitions
// corresponding to the POSIX.1-2001 base
// specification (excluding the XSI extension).
// For POSIX.1-2001 base specification,
// Refer http://pubs.opengroup.org/onlinepubs/009695399/
#define _POSIX_C_SOURCE 200112L
#include <string.h>
#include <ctype.h>

#include "ocenums.h"
#include "ocadaptation.h"
#include "occlientcb.h"
#include "ocserverrequest.h"
#include "ocstackinternal.h"
#include "ocresourcehandler.h"
#include "ocobserve.h"
#include "ocrandom.h"
#include "oic_malloc.h"
#include "oic_string.h"
//#include "secureresourcemanager.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "jutility.h"
#include "ocglobals.h"
#include "ocstack.h"

#ifdef WITH_ARDUINO
#include "Time.h"
#else
#include <sys/time.h>
#endif

#ifndef ARDUINO
#include <arpa/inet.h>
#endif

#ifndef UINT32_MAX
#define UINT32_MAX   (0xFFFFFFFFUL)
#endif

#ifdef WITH_CLIENT
//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------

#ifdef WITH_PRESENCE
static PresenceResource presenceResource;
static uint8_t PresenceTimeOutSize = 0;
static uint32_t PresenceTimeOut[] = {50, 75, 85, 95, 100};
#endif

#ifdef RA_ADAPTER
//TODO: revisit this design
static bool gRASetInfo = false;
#endif

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define TAG  PCF("OCClient")

OCStackResult OCInitClient()
{
#ifdef WITH_PRESENCE
    PresenceTimeOutSize = sizeof (PresenceTimeOut) / sizeof (PresenceTimeOut[0]) - 1;
#endif // WITH_PRESENCE

    return OC_STACK_OK;
}

void OCStopClient()
{
#ifdef WITH_PRESENCE
    // Ensure that the TTL associated with ANY and ALL presence notifications originating from
    // here send with the code "OC_STACK_PRESENCE_STOPPED" result.
    presenceResource.presenceTTL = 0;
#endif // WITH_PRESENCE
}

void FixUpClientResponse(OCClientResponse *cr)
{
    VERIFY_NON_NULL_NR(cr, FATAL);

    cr->addr = &cr->devAddr;
    cr->connType = (OCConnectivityType)
        ((cr->devAddr.adapter << CT_ADAPTER_SHIFT) | (cr->devAddr.flags & CT_MASK_FLAGS));
}

//-----------------------------------------------------------------------------
// Internal API function
//-----------------------------------------------------------------------------

OCStackResult CAToOCStackResult(CAResponseResult_t caCode)
{
    OCStackResult ret = OC_STACK_ERROR;

    switch(caCode)
    {
        case CA_CREATED:
            ret = OC_STACK_RESOURCE_CREATED;
            break;
        case CA_DELETED:
            ret = OC_STACK_RESOURCE_DELETED;
            break;
        case CA_CHANGED:
        case CA_CONTENT:
            ret = OC_STACK_OK;
            break;
        case CA_BAD_REQ:
            ret = OC_STACK_INVALID_QUERY;
            break;
        case CA_UNAUTHORIZED_REQ:
            ret = OC_STACK_UNAUTHORIZED_REQ;
            break;
        case CA_BAD_OPT:
            ret = OC_STACK_INVALID_OPTION;
            break;
        case CA_NOT_FOUND:
            ret = OC_STACK_NO_RESOURCE;
            break;
        case CA_RETRANSMIT_TIMEOUT:
            ret = OC_STACK_COMM_ERROR;
            break;
        default:
            break;
    }
    return ret;
}
uint32_t GetTicks(uint32_t afterMilliSeconds);


#ifdef WITH_PRESENCE
OCStackResult ResetPresenceTTL(ClientCB *cbNode, uint32_t maxAgeSeconds)
{
    uint32_t lowerBound  = 0;
    uint32_t higherBound = 0;

    if (!cbNode || !cbNode->presence || !cbNode->presence->timeOut)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OC_LOG_V(INFO, TAG, "Update presence TTL, time is %u", GetTicks(0));

    cbNode->presence->TTL = maxAgeSeconds;

    for (int index = 0; index < PresenceTimeOutSize; index++)
    {
        // Guard against overflow
        if (cbNode->presence->TTL < (UINT32_MAX/(MILLISECONDS_PER_SECOND*PresenceTimeOut[index]))
                                     * 100)
        {
            lowerBound = GetTicks((PresenceTimeOut[index] *
                                  cbNode->presence->TTL *
                                  MILLISECONDS_PER_SECOND)/100);
        }
        else
        {
            lowerBound = GetTicks(UINT32_MAX);
        }

        if (cbNode->presence->TTL < (UINT32_MAX/(MILLISECONDS_PER_SECOND*PresenceTimeOut[index+1]))
                                     * 100)
        {
            higherBound = GetTicks((PresenceTimeOut[index + 1] *
                                   cbNode->presence->TTL *
                                   MILLISECONDS_PER_SECOND)/100);
        }
        else
        {
            higherBound = GetTicks(UINT32_MAX);
        }

        cbNode->presence->timeOut[index] = OCGetRandomRange(lowerBound, higherBound);

        OC_LOG_V(DEBUG, TAG, "lowerBound timeout  %d", lowerBound);
        OC_LOG_V(DEBUG, TAG, "higherBound timeout %d", higherBound);
        OC_LOG_V(DEBUG, TAG, "timeOut entry  %d", cbNode->presence->timeOut[index]);
    }

    cbNode->presence->TTLlevel = 0;

    OC_LOG_V(DEBUG, TAG, "this TTL level %d", cbNode->presence->TTLlevel);
    return OC_STACK_OK;
}

/**
 * The cononical presence allows constructed URIs to be string compared.
 *
 * requestUri must be a char array of size URI_RESOURCE_LENGTH
 */
static int FormCanonicalPresenceUri(const OCDevAddr *devAddr, char *resourceUri, char *presenceUri)
{
    (void)resourceUri;

    if (devAddr->adapter == OC_ADAPTER_IP)
    {
        if ((devAddr->flags & OC_IP_USE_V6) && !(devAddr->flags & OC_IP_USE_V4))
        {
            if ('\0' == devAddr->addr[0])  // multicast
            {
                return snprintf(presenceUri, URI_RESOURCE_LENGTH, OC_RSRVD_PRESENCE_URI);
            }
            else
            {
                return snprintf(presenceUri, URI_RESOURCE_LENGTH, "coap://[%s]:%u%s",
                        devAddr->addr, devAddr->port, OC_RSRVD_PRESENCE_URI);
            }
        }
        else
        {
            if ('\0' == devAddr->addr[0])  // multicast
            {
                /* the next two lines require casting away const modifier */
                //OICStrcpy(devAddr->addr, sizeof (devAddr->addr), OC_MULTICAST_IP);
                //devAddr->port = OC_MULTICAST_PORT;
            }
            return snprintf(presenceUri, URI_RESOURCE_LENGTH, "coap://%s:%u%s",
                    devAddr->addr, devAddr->port, OC_RSRVD_PRESENCE_URI);
        }
    }

    // might work for other adapters (untested, but better than nothing)
    return snprintf(presenceUri, URI_RESOURCE_LENGTH, "coap://%s%s", devAddr->addr,
                    OC_RSRVD_PRESENCE_URI);
}

OCResourceTypeCB *findResourceType(OCResourceTypeCB *resourceTypeList, const char *resourceTypeName)
{
    OCResourceTypeCB *ptr = resourceTypeList;
    for (; resourceTypeName && ptr; ptr = ptr->next)
    {
        if (strcmp(resourceTypeName, (ptr->name)) == 0)
        {
            break;
        }
    }
    return ptr;
}

OCStackResult HandlePresenceResponse(const CAMessageInfo_t *responseInfo)
{
    OCStackApplicationResult cbResult = OC_STACK_DELETE_TRANSACTION;
    ClientCB * cbNode = NULL;
    char *resourceTypeName = NULL;
    OCClientResponse response = {.devAddr = {.adapter = OC_DEFAULT_ADAPTER}};
    OCStackResult result = OC_STACK_ERROR;
    uint32_t maxAge = 0;
    int uriLen;
    char presenceUri[URI_RESOURCE_LENGTH];

    int presenceSubscribe = 0;
    int multicastPresenceSubscribe = 0;
    const OCDevAddr *devAddr = &(responseInfo->devAddr);

    if (responseInfo->info.coapCode != CA_SUCCESS)
    {
        OC_LOG_V(ERROR, TAG, "HandlePresenceResponse failed %d", responseInfo->info.coapCode);
        return OC_STACK_ERROR;
    }

    // check for unicast presence
    uriLen = FormCanonicalPresenceUri(devAddr, OC_RSRVD_PRESENCE_URI, presenceUri);
    if (uriLen < 0 || (size_t)uriLen >= sizeof (presenceUri))
    {
        return OC_STACK_INVALID_URI;
    }

    cbNode = GetClientCB(NULL, 0, presenceUri);
    if (cbNode)
    {
        presenceSubscribe = 1;
    }
    else
    {
        // check for multiicast presence
        OCDevAddr ep = { .adapter = devAddr->adapter,
                            .flags = devAddr->flags };
        OICStrcpy(ep.addr, sizeof(ep.addr), OC_MULTICAST_IP);
        ep.port = OC_MULTICAST_PORT;

        uriLen = FormCanonicalPresenceUri(&ep, OC_RSRVD_PRESENCE_URI, presenceUri);

        cbNode = GetClientCB(NULL, 0, presenceUri);
        if (cbNode)
        {
            multicastPresenceSubscribe = 1;
        }
    }

    if (!presenceSubscribe && !multicastPresenceSubscribe)
    {
        OC_LOG(ERROR, TAG, PCF("Received a presence notification, but no callback, ignoring"));
        goto exit;
    }

    response.payload = NULL;
    response.result = OC_STACK_OK;
    response.devAddr = *devAddr;

    FixUpClientResponse(&response);

    if (responseInfo->info.parsedPayload)
    {
        OCPresencePayload *pp = ((OCPresencePayload*)responseInfo->info.parsedPayload);
        response.sequenceNumber = pp->sequenceNumber;
        resourceTypeName = pp->resourceType;
        maxAge = pp->maxAge;
    }

    if (presenceSubscribe)
    {
        if(cbNode->sequenceNumber == response.sequenceNumber)
        {
            OC_LOG(INFO, TAG, PCF("No presence change"));
            goto exit;
        }

        if (maxAge == 0)
        {
            OC_LOG(INFO, TAG, PCF("Stopping presence"));
            response.result = OC_STACK_PRESENCE_STOPPED;
            if(cbNode->presence)
            {
                OICFree(cbNode->presence->timeOut);
                OICFree(cbNode->presence);
                cbNode->presence = NULL;
            }
        }
        else
        {
            if (!cbNode->presence)
            {
                cbNode->presence = (OCPresence *)OICMalloc(sizeof (OCPresence));

                VERIFY_NON_NULL_V(cbNode->presence);
                cbNode->presence->timeOut = NULL;
                cbNode->presence->timeOut = (uint32_t *)
                        OICMalloc(PresenceTimeOutSize * sizeof(uint32_t));
            }

            ResetPresenceTTL(cbNode, maxAge);

            cbNode->sequenceNumber = response.sequenceNumber;

            // Ensure that a filter is actually applied.
            if (resourceTypeName && cbNode->filterResourceType)
            {
                if (!findResourceType(cbNode->filterResourceType, resourceTypeName))
                {
                    goto exit;
                }
            }
        }
    }
    else
    {
        // This is the multicast case
        OCMulticastNode* mcNode = NULL;
        mcNode = GetMCPresenceNode(presenceUri);

        if(mcNode != NULL)
        {
            if(mcNode->nonce == response.sequenceNumber)
            {
                OC_LOG(INFO, TAG, PCF("No presence change (Multicast)"));
                goto exit;
            }
            mcNode->nonce = response.sequenceNumber;

            if(maxAge == 0)
            {
                OC_LOG(INFO, TAG, PCF("Stopping presence"));
                response.result = OC_STACK_PRESENCE_STOPPED;
            }
        }
        else
        {
            result = AddMCPresenceNode(&mcNode, presenceUri, response.sequenceNumber);
            if(result == OC_STACK_NO_MEMORY)
            {
                OC_LOG(INFO, TAG,
                    PCF("No Memory for Multicast Presence Node"));
                goto exit;
            }
            // presence node now owns uri
        }

        // Ensure that a filter is actually applied.
        if(resourceTypeName && cbNode->filterResourceType)
        {
            if(!findResourceType(cbNode->filterResourceType, resourceTypeName))
            {
                goto exit;
            }
        }
    }

    cbResult = cbNode->callBack(cbNode->context, cbNode->handle, &response);

    if (cbResult == OC_STACK_DELETE_TRANSACTION)
    {
        FindAndDeleteClientCB(cbNode);
    }

exit:
    OCPayloadDestroy(response.payload);
    return result;
}

#endif

bool HandleCAResponsesClient(const CAMessageInfo_t* responseInfo)
{
#ifdef WITH_PRESENCE
    if (!strcmp(responseInfo->info.resourcePath, OC_RSRVD_PRESENCE_URI))
    {
        HandlePresenceResponse(responseInfo);
        return true;
    }
#endif

    ClientCB *cbNode = GetClientCB(&(responseInfo->info.token), 0, NULL);
    if (!cbNode)
    {
        return false;
    }

    OC_LOG(INFO, TAG, PCF("There is a cbNode associated with the response token"));
    if (responseInfo->info.coapCode == CA_EMPTY)
    {
        OC_LOG(INFO, TAG, PCF("Receiving A ACK/RESET for this token"));
        // We do not have a case for the client to receive a RESET
        if (responseInfo->info.coapType == OC_MSG_ACKNOWLEDGE)
        {
            //This is the case of receiving an ACK on a request to a slow resource!
            OC_LOG(INFO, TAG, PCF("This is a pure ACK"));
            //TODO: should we inform the client
            //      app that at least the request was received at the server?
        }
    }
    else if (responseInfo->info.coapCode == CA_RETRANSMIT_TIMEOUT)
    {
        OC_LOG(INFO, TAG, PCF("Receiving A Timeout for this token"));
        OC_LOG(INFO, TAG, PCF("Calling into application address space"));

        OCClientResponse response =
            {.devAddr = {.adapter = OC_DEFAULT_ADAPTER}};
        response.devAddr = responseInfo->devAddr;
        FixUpClientResponse(&response);
        response.resourceUri = responseInfo->info.resourcePath;
        memcpy(response.identity.id, responseInfo->info.identity.id,
                                            sizeof (response.identity.id));
        response.identity.id_length = responseInfo->info.identity.id_length;

        response.result = CAToOCStackResult(responseInfo->info.coapCode);
        cbNode->callBack(cbNode->context, cbNode->handle, &response);
        FindAndDeleteClientCB(cbNode);
    }
    else
    {
        OC_LOG(INFO, TAG, PCF("This is a regular response, A client call back is found"));
        OC_LOG(INFO, TAG, PCF("Calling into application address space"));

        OCClientResponse response =
            {.devAddr = {.adapter = OC_DEFAULT_ADAPTER}};
        response.devAddr = responseInfo->devAddr;
        FixUpClientResponse(&response);
        response.resourceUri = responseInfo->info.resourcePath;
        memcpy(response.identity.id, responseInfo->info.identity.id,
                                            sizeof (response.identity.id));
        response.identity.id_length = responseInfo->info.identity.id_length;
        response.sequenceNumber = responseInfo->info.observationOption;

        response.result = CAToOCStackResult(responseInfo->info.coapCode);

        response.numRcvdVendorSpecificHeaderOptions = 0;
        if (responseInfo->info.numOptions > 0)
        {
            response.numRcvdVendorSpecificHeaderOptions = responseInfo->info.numOptions;

            for (uint8_t i = 0; i < responseInfo->info.numOptions; i++)
            {
                memcpy (&(response.rcvdVendorSpecificHeaderOptions[i]),
                        &(responseInfo->info.options[i]), sizeof(OCHeaderOption));
            }
        }

        if (cbNode->method == OC_REST_OBSERVE &&
            response.sequenceNumber > OC_OFFSET_SEQUENCE_NUMBER &&
            response.sequenceNumber <= cbNode->sequenceNumber)
        {
            OC_LOG_V(INFO, TAG, PCF("Received stale notification. Number :%d"),
                                             response.sequenceNumber);
        }
        else
        {
            OCStackApplicationResult appFeedback = cbNode->callBack(cbNode->context,
                                                                    cbNode->handle,
                                                                    &response);
            cbNode->sequenceNumber = response.sequenceNumber;

            if (appFeedback == OC_STACK_DELETE_TRANSACTION)
            {
                FindAndDeleteClientCB(cbNode);
            }
            else
            {
                // To keep discovery callbacks active.
                cbNode->TTL = GetTicks(MAX_CB_TIMEOUT_SECONDS *
                                        MILLISECONDS_PER_SECOND);
            }
        }

        //Need to send ACK when the response is CON
        if (responseInfo->info.coapType == OC_MSG_CONFIRM)
        {
            SendDirectStackResponse(responseInfo, OC_MSG_ACKNOWLEDGE);
        }

        OCPayloadDestroy(response.payload);
    }
    return true;
}

//-----------------------------------------------------------------------------
// Public APIs
//-----------------------------------------------------------------------------
#ifdef RA_ADAPTER
OCStackResult OCSetRAInfo(const OCRAInfo_t *raInfo)
{
    if (!raInfo           ||
        !raInfo->username ||
        !raInfo->hostname ||
        !raInfo->xmpp_domain)
    {

        return OC_STACK_INVALID_PARAM;
    }
    OCStackResult result = CAResultToOCResult(CASetRAInfo((const CARAInfo_t *) raInfo));
    gRASetInfo = (result == OC_STACK_OK)? true : false;

    return result;
}
#endif

OCMessageType_t qualityOfServiceToMessageType(OCQualityOfService qos)
{
    switch (qos)
    {
        case OC_HIGH_QOS:
            return OC_MSG_CONFIRM;
        case OC_LOW_QOS:
        case OC_MEDIUM_QOS:
        case OC_NA_QOS:
        default:
            return OC_MSG_NONCONFIRM;
    }
}

OCStackResult verifyUriQueryLength(const char *inputUri, uint16_t uriLen)
{
    char *query;

    query = strchr (inputUri, '?');

    if (query != NULL)
    {
        if((query - inputUri) > MAX_URI_LENGTH)
        {
            return OC_STACK_INVALID_URI;
        }

        if ((inputUri + uriLen - 1 - query) > URI_QUERY_LENGTH)
        {
            return OC_STACK_INVALID_QUERY;
        }
    }
    else if(uriLen > MAX_URI_LENGTH)
    {
        return OC_STACK_INVALID_URI;
    }
    return OC_STACK_OK;
}

/**
 *  A request uri consists of the following components in order:
 *                              example
 *  optional prefix             "coap://"
 *  optionally one of
 *      IPv6 address            "[1234::5678]"
 *      IPv4 address            "192.168.1.1"
 *  optional port               ":5683"
 *  resource uri                "/oc/core..."
 *
 *  for PRESENCE requests, extract resource type.
 */
static OCStackResult ParseRequestUri(const char *fullUri,
                                        OCTransportAdapter adapter,
                                        OCTransportFlags flags,
                                        OCDevAddr *endpoint,
                                        char *resourceUri,
                                        char *resourceType)
{
    char *colon = NULL;
    char *end;

    // delimit url prefix, if any
    const char *start = fullUri;
    char *slash2 = strstr(start, "//");
    if (slash2)
    {
        start = slash2 + 2;
    }
    char *slash = strchr(start, '/');
    if (!slash)
    {
        return OC_STACK_INVALID_URI;
    }

    // processs url prefix, if any
    size_t urlLen = slash - start;
    // port
    uint16_t port = 0;
    size_t len = 0;
    if (urlLen)
    {   // construct OCDevAddr
        if (start[0] == '[')
        {   // ipv6 address
            char *close = strchr(++start, ']');
            if (!close || close > slash)
            {
                return OC_STACK_INVALID_URI;
            }
            end = close;
            if (close[1] == ':')
            {
                colon = close + 1;
            }
            adapter = (OCTransportAdapter)(adapter | OC_ADAPTER_IP);
            flags = (OCTransportFlags)(flags | OC_IP_USE_V6);
        }
        else
        {
            char *dot = strchr(start, '.');
            if (dot && dot < slash)
            {   // ipv4 address
                colon = strchr(start, ':');
                end = (colon && colon < slash) ? colon : slash;
                adapter = (OCTransportAdapter)(adapter | OC_ADAPTER_IP);
                flags = (OCTransportFlags)(flags | OC_IP_USE_V4);
            }
            else
            {   // MAC address
                end = slash;
            }
        }
        len = end - start;
        if (len >= sizeof(endpoint->addr))
        {
            return OC_STACK_INVALID_URI;
        }
        // collect port, if any
        if (colon && colon < slash)
        {
            for (colon++; colon < slash; colon++)
            {
                char c = colon[0];
                if (c < '0' || c > '9')
                {
                    return OC_STACK_INVALID_URI;
                }
                port = 10 * port + c - '0';
            }
        }

        len = end - start;
        if (len >= sizeof(endpoint->addr))
        {
            return OC_STACK_INVALID_URI;
        }

        OICStrcpyPartial(endpoint->addr, sizeof(endpoint->addr), start, len);
        endpoint->port = port;
        endpoint->adapter = adapter;
        endpoint->flags = flags;
        if (!strncmp(fullUri, "coaps:", 6))
        {
            endpoint->flags = (OCTransportFlags)(endpoint->flags|OC_SECURE);
        }
    }

    // process resource uri, if any
    if (slash)
    {   // request uri and query
        size_t ulen = strlen(slash); // resource uri length
        size_t tlen = 0;      // resource type length
        char *type = NULL;

        OICStrcpy(resourceUri, URI_RESOURCE_LENGTH, slash); // resource uri

        static const char strPresence[] = "/oic/ad?rt=";
        static const size_t lenPresence = sizeof(strPresence) - 1;
        if (!strncmp(slash, strPresence, lenPresence))
        {
            type = slash + lenPresence;
            tlen = ulen - lenPresence;
            OICStrcpy(resourceType, (tlen+1), type); // resource type
        }
    }

    return OC_STACK_OK;
}

OCDoHandle GenerateInvocationHandle()
{
    OCDoHandle handle;
    // Generate token here, it will be deleted when the transaction is deleted
    OCFillRandomMem((uint8_t *)&handle, sizeof (handle));

    return handle;
}
#endif // WITH_CLIENT

/**
 * Discover or Perform requests on a specified resource
 */
OCStackResult OCDoResource(OCDoHandle *handle,
                            OCMethod method,
                            const char *requestUri,
                            const OCDevAddr *destination,
                            OCPayload* payload,
                            OCConnectivityType connectivityType,
                            OCQualityOfService qos,
                            OCCallbackData *cbData,
                            OCHeaderOption *options,
                            uint8_t numOptions)
{
#ifdef WITH_CLIENT
    OC_LOG(INFO, TAG, PCF("Entering OCDoResource"));

    // Validate input parameters
    VERIFY_NON_NULL(cbData, FATAL, OC_STACK_INVALID_CALLBACK);
    VERIFY_NON_NULL(cbData->cb, FATAL, OC_STACK_INVALID_CALLBACK);
    VERIFY_NON_NULL(requestUri , FATAL, OC_STACK_INVALID_URI);

    OCStackResult result = OC_STACK_ERROR;
    ClientCB *clientCB = NULL;
    OCDoHandle resHandle = NULL;
    uint32_t ttl = 0;
    OCTransportAdapter adapter;
    OCTransportFlags flags;
    // the request contents are put here
    CAMessageInfo_t requestInfo = { .type = OC_REQUEST_DATA };
    OCDevAddr *devAddr = &(requestInfo.devAddr);
    // requestUri  will be parsed into the following three variables
    char *resourceUri = requestInfo.info.resourcePath;
    char resourceType[URI_RESOURCE_LENGTH];
#ifdef WITH_PRESENCE
    char presenceUri[URI_RESOURCE_LENGTH];
#endif

    // This validation is broken, but doesn't cause harm
    size_t uriLen = strlen(requestUri );
    if ((result = verifyUriQueryLength(requestUri , uriLen)) != OC_STACK_OK)
    {
        goto exit;
    }

    /*
     * Support original behavior with address on resourceUri argument.
     */
    adapter = (OCTransportAdapter)(connectivityType >> CT_ADAPTER_SHIFT);
    flags = (OCTransportFlags)(connectivityType & CT_MASK_FLAGS);

    result = ParseRequestUri(requestUri, adapter, flags, devAddr, resourceUri, resourceType);

    if (destination)    // destination overrides resourceUri address
    {
        *devAddr = *destination;
    }

    if (result != OC_STACK_OK)
    {
        OC_LOG_V(DEBUG, TAG, "Unable to parse uri: %s", requestUri);
        goto exit;
    }

    bool isMulticast = false;

    switch (method)
    {
    case OC_REST_GET:
    case OC_REST_OBSERVE:
    case OC_REST_OBSERVE_ALL:
    case OC_REST_CANCEL_OBSERVE:
        requestInfo.info.coapCode = OC_GET;
        break;
    case OC_REST_PUT:
        requestInfo.info.coapCode = OC_PUT;
        break;
    case OC_REST_POST:
        requestInfo.info.coapCode = OC_POST;
        break;
    case OC_REST_DELETE:
        requestInfo.info.coapCode = OC_DELETE;
        break;
    case OC_REST_DISCOVER:
        qos = OC_LOW_QOS;
    #ifdef WITH_PRESENCE
        // fall into ...
    case OC_REST_PRESENCE:
    #endif
        if (!devAddr->addr[0])
        {
            isMulticast = true;
        }
        requestInfo.info.coapCode = OC_GET;
        break;
    default:
        result = OC_STACK_INVALID_METHOD;
        goto exit;
    }

    if (!devAddr->addr[0] && !isMulticast)
    {
        OC_LOG_V(DEBUG, TAG, "no destination and no address in requestUri");
        result = OC_STACK_INVALID_PARAM;
        goto exit;
    }

    // fill in request data
    resHandle = GenerateInvocationHandle();

    requestInfo.info.coapType = qualityOfServiceToMessageType(qos);
    CAGenerateToken(&(requestInfo.info.token), USE_TOKEN_LEN);

    if ((method == OC_REST_OBSERVE) || (method == OC_REST_OBSERVE_ALL))
    {
        requestInfo.info.observationOption = OC_OBSERVE_REGISTER;
    }
    else
    {
        requestInfo.info.observationOption = OC_OBSERVE_NO_OPTION;
    }

    if (numOptions > MAX_HEADER_OPTIONS)
    {
        OC_LOG_V(DEBUG, TAG, "too many options");
        result = OC_STACK_INVALID_PARAM;
        goto exit;
    }
    if (numOptions)
    {
        memcpy(requestInfo.info.options, options, numOptions);
        requestInfo.info.numOptions = numOptions;
    }

    uint8_t encodedPayload[MAX_ENCODED_PAYLOAD_LENGTH];
    requestInfo.encodedPayload = encodedPayload;
    requestInfo.encodedSize = 0;   // set non-zero as part of encoding

    if (payload)
    {
        requestInfo.encodedSize = sizeof (encodedPayload);
        result = OCConvertPayload(payload, encodedPayload, &(requestInfo.encodedSize));
        if (result != OC_STACK_OK)
        {
            OC_LOG(ERROR, TAG, PCF("Failed to encode payload"));
            goto exit;
        }
    }

    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("CACreateEndpoint error"));
        goto exit;
    }

    // prepare for response
    #ifdef WITH_PRESENCE
    if (method == OC_REST_PRESENCE)
    {
        FormCanonicalPresenceUri(devAddr, resourceUri, presenceUri);
        resourceUri = presenceUri;
    }
    #endif

    ttl = GetTicks(MAX_CB_TIMEOUT_SECONDS * MILLISECONDS_PER_SECOND);
    result = AddClientCB(&clientCB, cbData, &(requestInfo.info.token),
                         &resHandle, method, devAddr,
                         resourceUri, resourceType, ttl);
    if (OC_STACK_OK != result)
    {
        goto exit;
    }

    OCSendMessage(&requestInfo);

    if (handle)
    {
        *handle = resHandle;
    }

exit:
    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCDoResource error"));
        FindAndDeleteClientCB(clientCB);
        if (handle)
        {
            *handle = 0;
        }
    }

    // We are the owner of the payload object, so we free it
    OCPayloadDestroy(payload);
    return result;
#else
    (void)handle;
    (void)method;
    (void)requestUri;
    (void)destination;
    (void)payload;
    (void)connectivityType;
    (void)qos;
    (void)cbData;
    (void)options;
    (void)numOptions;
    return OC_STACK_ERROR;
#endif // WITH_CLIENT
}

OCStackResult OCCancel(OCDoHandle handle, OCQualityOfService qos, OCHeaderOption * options,
        uint8_t numOptions)
{
#ifdef WITH_CLIENT
    /*
     * This ftn is implemented one of two ways in the case of observation:
     *
     * 1. qos == OC_NON_CONFIRMABLE. When observe is unobserved..
     *      Remove the callback associated on client side.
     *      When the next notification comes in from server,
     *      reply with RESET message to server.
     *      Keep in mind that the server will react to RESET only
     *      if the last notification was sent as CON
     *
     * 2. qos == OC_CONFIRMABLE. When OCCancel is called,
     *      and it is associated with an observe request
     *      (i.e. ClientCB->method == OC_REST_OBSERVE || OC_REST_OBSERVE_ALL),
     *      Send CON Observe request to server with
     *      observe flag = OC_RESOURCE_OBSERVE_DEREGISTER.
     *      Remove the callback associated on client side.
     */
    OCStackResult ret = OC_STACK_OK;
    CAMessageInfo_t requestInfo = {.type = OC_REQUEST_DATA};
    CAInfo_t *info = &(requestInfo.info);
    info->observationOption = OC_OBSERVE_NO_OPTION;
    info->coapCode = OC_GET;

    if (!handle)
    {
        return OC_STACK_INVALID_PARAM;
    }

    ClientCB *clientCB = GetClientCB(NULL, handle, NULL);
    if (!clientCB)
    {
        OC_LOG(ERROR, TAG, PCF("Client callback not found. Called OCCancel twice?"));
        return OC_STACK_OK;
    }

    switch (clientCB->method)
    {
    case OC_REST_OBSERVE:
    case OC_REST_OBSERVE_ALL:
        OC_LOG_V(INFO, TAG, "Canceling observation for resource %s",
                                        clientCB->requestUri);
        if (qos != OC_HIGH_QOS)
        {
            FindAndDeleteClientCB(clientCB);
            break;
        }
        else
        {
            OC_LOG(INFO, TAG, PCF("Cancelling observation as CONFIRMABLE"));
        }

        info->coapType = qualityOfServiceToMessageType(qos);
        info->token = clientCB->token;
        info->observationOption = OC_OBSERVE_DEREGISTER;

        if (numOptions > MAX_HEADER_OPTIONS)
        {
            OC_LOG_V(DEBUG, TAG, "too many options");
            return OC_STACK_INVALID_PARAM;
        }
        if (numOptions)
        {
            memcpy(info->options, options, numOptions);
            info->numOptions = numOptions;
        }
        OICStrcpy(info->resourcePath, sizeof (info->resourcePath), clientCB->requestUri);

        requestInfo.info.coapCode = OC_GET;
        requestInfo.devAddr = clientCB->devAddr;

        OCSendMessage(&requestInfo);
        ret = OC_STACK_OK;
        break;

    #ifdef WITH_PRESENCE
    case OC_REST_PRESENCE:
        FindAndDeleteClientCB(clientCB);
        break;
    #endif

    default:
        ret = OC_STACK_INVALID_METHOD;
        break;
    }

    return ret;
#else
    (void)handle;
    (void)qos;
    (void)options;
    (void)numOptions;
    return OC_STACK_OK;
#endif // WITH_CLIENT
}

#ifdef WITH_CLIENT
uint32_t OCGetRandomRange(uint32_t firstBound, uint32_t secondBound){
    uint32_t base;
    uint32_t diff;
    uint32_t result;
    if(firstBound > secondBound){
        base = secondBound;
        diff = firstBound - secondBound;
    }else if(firstBound < secondBound){
        base = firstBound;
        diff = secondBound - firstBound;
    }else{
        return secondBound;
    }
    result = ((float)OCGetRandom()/((float)(0xFFFFFFFF))*(float)diff) + (float) base;
    return result;
}

#ifdef WITH_PRESENCE

OCStackResult OCProcessPresence()
{
    OCStackResult result = OC_STACK_OK;

    // the following line floods the log with messages that are irrelevant
    // to most purposes.  Uncomment as needed.
    //OC_LOG(INFO, TAG, PCF("Entering RequestPresence"));
    ClientCB* cbNode = NULL;
    OCClientResponse clientResponse;
    OCStackApplicationResult cbResult = OC_STACK_DELETE_TRANSACTION;

    LL_FOREACH(cbList, cbNode)
    {
        if (OC_REST_PRESENCE != cbNode->method || !cbNode->presence)
        {
            continue;
        }

        uint32_t now = GetTicks(0);
        OC_LOG_V(DEBUG, TAG, "this TTL level %d",
                                                cbNode->presence->TTLlevel);
        OC_LOG_V(DEBUG, TAG, "current ticks %d", now);

        if (cbNode->presence->TTLlevel > PresenceTimeOutSize)
        {
            goto exit;
        }

        if (cbNode->presence->TTLlevel < PresenceTimeOutSize)
        {
            OC_LOG_V(DEBUG, TAG, "timeout ticks %d",
                    cbNode->presence->timeOut[cbNode->presence->TTLlevel]);
        }
        if (cbNode->presence->TTLlevel >= PresenceTimeOutSize)
        {
            OC_LOG(DEBUG, TAG, PCF("No more timeout ticks"));

            clientResponse.sequenceNumber = 0;
            clientResponse.result = OC_STACK_PRESENCE_TIMEOUT;
            clientResponse.devAddr = cbNode->devAddr;
            FixUpClientResponse(&clientResponse);
            clientResponse.payload = NULL;

            // Increment the TTLLevel (going to a next state), so we don't keep
            // sending presence notification to client.
            cbNode->presence->TTLlevel++;
            OC_LOG_V(DEBUG, TAG, "moving to TTL level %d",
                                        cbNode->presence->TTLlevel);

            cbResult = cbNode->callBack(cbNode->context, cbNode->handle, &clientResponse);
            if (cbResult == OC_STACK_DELETE_TRANSACTION)
            {
                FindAndDeleteClientCB(cbNode);
            }
        }

        if (now < cbNode->presence->timeOut[cbNode->presence->TTLlevel])
        {
            continue;
        }

        CAInfo_t requestData = {.coapType = OC_MSG_NONCONFIRM};
        CAMessageInfo_t requestInfo = {.type = OC_REQUEST_DATA};

        OC_LOG(DEBUG, TAG, PCF("time to test server presence"));

        requestData.token = cbNode->token;
        OICStrcpy(requestData.resourcePath, sizeof (requestData.resourcePath), OC_RSRVD_PRESENCE_URI);
        requestInfo.info = requestData;
        requestInfo.info.coapCode = OC_GET;
        requestInfo.devAddr = cbNode->devAddr;

        OCSendMessage(&requestInfo);

        cbNode->presence->TTLlevel++;
        OC_LOG_V(DEBUG, TAG, "moving to TTL level %d", cbNode->presence->TTLlevel);
    }
exit:
    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCProcessPresence error"));
    }
    return result;
}

#endif // WITH_PRESENCE

#endif // WITH_CLIENT
