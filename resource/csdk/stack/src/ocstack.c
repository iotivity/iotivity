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

#include "ocstack.h"
#include "ocstackinternal.h"
#include "ocresourcehandler.h"
#include "occlientcb.h"
#include "ocobserve.h"
#include "ocrandom.h"
#include "ocmalloc.h"
#include "ocserverrequest.h"
#include "ocsecurityinternal.h"

#include "cacommon.h"
#include "cainterface.h"

#ifdef WITH_ARDUINO
#include "Time.h"
#else
#include <sys/time.h>
#endif
#include "coap_time.h"
#include "utlist.h"
#include "pdu.h"

#ifndef ARDUINO
#include <arpa/inet.h>
#endif

#ifndef UINT32_MAX
#define UINT32_MAX   (0xFFFFFFFFUL)
#endif

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef enum
{
    OC_STACK_UNINITIALIZED = 0,
    OC_STACK_INITIALIZED,
    OC_STACK_UNINIT_IN_PROGRESS
} OCStackState;

#ifdef WITH_PRESENCE
typedef enum
{
    OC_PRESENCE_UNINITIALIZED = 0,
    OC_PRESENCE_INITIALIZED
} OCPresenceState;
#endif

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------
static OCStackState stackState = OC_STACK_UNINITIALIZED;

OCResource *headResource = NULL;
static OCResource *tailResource = NULL;
#ifdef WITH_PRESENCE
static OCPresenceState presenceState = OC_PRESENCE_UNINITIALIZED;
static PresenceResource presenceResource;
static uint8_t PresenceTimeOutSize = 0;
static uint32_t PresenceTimeOut[] = {50, 75, 85, 95, 100};
#endif

static OCMode myStackMode;
OCDeviceEntityHandler defaultDeviceHandler;


//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define TAG  PCF("OCStack")
#define VERIFY_SUCCESS(op, successCode) { if ((op) != (successCode)) \
            {OC_LOG_V(FATAL, TAG, "%s failed!!", #op); goto exit;} }
#define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); return (retVal); } }
#define VERIFY_NON_NULL_V(arg) { if (!arg) {OC_LOG_V(FATAL, TAG, "%s is NULL", #arg);\
    goto exit;} }

//TODO: we should allow the server to define this
#define MAX_OBSERVE_AGE (0x2FFFFUL)

#define MILLISECONDS_PER_SECOND   (1000)
/**
 * Parse the presence payload and extract various parameters.
 * Note: Caller should invoke OCFree after done with resType pointer.
 *
 * @param payload Presence payload.
 * @param seqNum Sequence number.
 * @param maxAge Time To Live (in seconds).
 * @param resType Resource type.
 */
// TODO: Not sure if I agree with this.  I think it should be static but it is called in
// stack/test/stacktests.cpp, not included via a header file.  If we intend to allow it
// to be called externally, we should change the name to OCParsePresencePayload and make
// it part of the official public API.  But can't change now due to current API freeze.
// Another option might be to make non-API utility functions for doing stuff like this.
void parsePresencePayload(char* payload, uint32_t* seqNum, uint32_t* maxAge, char** resType);

//-----------------------------------------------------------------------------
// Private internal function prototypes
//-----------------------------------------------------------------------------

/**
 * Generate handle of OCDoResource invocation for callback management.
 *
 * @return Generated OCDoResource handle.
 */
static OCDoHandle GenerateInvocationHandle();

/**
 * Initialize resource data structures, variables, etc.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult initResources();

/**
 * Add a resource to the end of the linked list of resources.
 *
 * @param resource Resource to be added
 */
static void insertResource(OCResource *resource);

/**
 * Find a resource in the linked list of resources.
 *
 * @param resource Resource to be found.
 * @return Pointer to resource that was found in the linked list or NULL if the resource was not
 *         found.
 */
static OCResource *findResource(OCResource *resource);

/**
 * Insert a resource type into a resource's resource type linked list.
 * If resource type already exists, it will not be inserted and the
 * resourceType will be free'd.
 * resourceType->next should be null to avoid memory leaks.
 * Function returns silently for null args.
 *
 * @param resource Resource where resource type is to be inserted.
 * @param resourceType Resource type to be inserted.
 */
static void insertResourceType(OCResource *resource,
        OCResourceType *resourceType);

/**
 * Get a resource type at the specified index within a resource.
 *
 * @param handle Handle of resource.
 * @param index Index of resource type.
 *
 * @return Pointer to resource type if found, NULL otherwise.
 */
static OCResourceType *findResourceTypeAtIndex(OCResourceHandle handle,
        uint8_t index);

/**
 * Insert a resource interface into a resource's resource interface linked list.
 * If resource interface already exists, it will not be inserted and the
 * resourceInterface will be free'd.
 * resourceInterface->next should be null to avoid memory leaks.
 *
 * @param resource Resource where resource interface is to be inserted.
 * @param resourceInterface Resource interface to be inserted.
 */
static void insertResourceInterface(OCResource *resource,
        OCResourceInterface *resourceInterface);

/**
 * Get a resource interface at the specified index within a resource.
 *
 * @param handle Handle of resource.
 * @param index Index of resource interface.
 *
 * @return Pointer to resource interface if found, NULL otherwise.
 */
static OCResourceInterface *findResourceInterfaceAtIndex(
        OCResourceHandle handle, uint8_t index);

/**
 * Delete all of the dynamically allocated elements that were created for the resource type.
 *
 * @param resourceType Specified resource type.
 */
static void deleteResourceType(OCResourceType *resourceType);

/**
 * Delete all of the dynamically allocated elements that were created for the resource interface.
 *
 * @param resourceInterface Specified resource interface.
 */
static void deleteResourceInterface(OCResourceInterface *resourceInterface);

/**
 * Delete all of the dynamically allocated elements that were created for the resource.
 *
 * @param resource Specified resource.
 */
static void deleteResourceElements(OCResource *resource);

/**
 * Delete resource specified by handle.  Deletes resource and all resourcetype and resourceinterface
 * linked lists.
 *
 * @param handle Handle of resource to be deleted.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult deleteResource(OCResource *resource);

/**
 * Delete all of the resources in the resource list.
 */
static void deleteAllResources();

/**
 * Increment resource sequence number.  Handles rollover.
 *
 * @param resPtr Pointer to resource.
 */
static void incrementSequenceNumber(OCResource * resPtr);

/**
 * Verify the lengths of the URI and the query separately.
 *
 * @param inputUri Input URI and query.
 * @param uriLen The length of the initial URI with query.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult verifyUriQueryLength(const char * inputUri,
        uint16_t uriLen);

/**
 * Determine if a request/response must be sent in a block transfer because it is too large to be
 * sent in a single PDU.  This function can be used for either a request or a response.
 * Note:  Either the request or response parameter should be non-NULL (i.e. only one, not both).
 *
 * @param request  NULL or pointer to request.
 * @param response NULL or pointer to response.
 * @param size     0 or size of the request/response.  If 0, strlen is used for determining
 *                 the length of the request/response.
 *
 * @return
 *    false - packet transfer NOT required (i.e. normal request/response).
 *    true  - packet transfer required (i.e. block transfer needed).
 */
static bool OCIsPacketTransferRequired(const char *request, const char *response, size_t size);

/**
 * Retrieves a resource type based upon a query contains only just one
 * resource attribute (and that has to be of type "rt").
 *
 * @remark This API malloc's memory for the resource type. Do not malloc resourceType
 * before passing in.
 *
 * @param query The query part of the URI.
 * @param resourceType The resource type to be populated; pass by reference.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult getResourceType(const char * query, char** resourceType);

/*
 * Attempts to initialize every network interface that the CA Layer might have compiled in.
 *
 * Note: At least one interface must succeed to initialize. If all calls to @ref CASelectNetwork
 * return something other than @ref CA_STATUS_OK, then this function fails.
 *
 * @return ::CA_STATUS_OK on success, some other value upon failure.
 */
static CAResult_t OCSelectNetwork();

/**
 * Get the CoAP ticks after the specified number of milli-seconds.
 *
 * @param afterMilliSeconds Milli-seconds.
 * @return
 *     CoAP ticks
 */
static uint32_t GetTicks(uint32_t afterMilliSeconds);

/**
 * This method is used to create the IPv4 dev_addr structure.
 * Builds a socket interface address using IP address and port number.
 * TODO: Remove in future. Temporary helper function.
 *
 * @param a IPv4 octet 0.
 * @param b IPv4 octet 1.
 * @param c IPv4 octet 2.
 * @param d IPv4 octet 3.
 * @param port Port number.
 * @param ipAddr - IPv4 address.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult OCBuildIPv4Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d,
        uint16_t port, OCDevAddr *ipAddr);

/**
 * Convert CAResponseResult_t to OCStackResult.
 *
 * @param caCode CAResponseResult_t code.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult CAToOCStackResult(CAResponseResult_t caCode);

/**
 * Convert OCStackResult to CAResponseResult_t.
 *
 * @param caCode OCStackResult code.
 * @return ::CA_SUCCESS on success, some other value upon failure.
 */
static CAResponseResult_t OCToCAStackResult(OCStackResult ocCode);

/**
 * Convert OCConnectivityType to CATransportType_t.
 *
 * @param ocConType OCConnectivityType input.
 * @param caConType CATransportType_t output.
 * @return  ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult OCToCATransportType(OCConnectivityType ocConType,
        CATransportType_t* caConType);

/**
 * Convert CATransportType_t to OCConnectivityType.
 *
 * @param caConType CATransportType_t input.
 * @param ocConType OCConnectivityType output.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult CAToOCConnectivityType(CATransportType_t caConType,
        OCConnectivityType *ocConType);

/**
 * Update response.addr appropriately from endPoint.addressInfo.
 *
 * @param address OCDevAddr output.
 * @param endPoint CARemoteEndpoint_t input.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult UpdateResponseAddr(OCDevAddr *address, const CARemoteEndpoint_t* endPoint);

/**
 * Handle response from presence request.
 *
 * @param endPoint CA remote endpoint.
 * @param responseInfo CA response info.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult HandlePresenceResponse(const CARemoteEndpoint_t* endPoint,
        const CAResponseInfo_t* responseInfo);

/**
 * This function will be called back by CA layer when a response is received.
 *
 * @param endPoint CA remote endpoint.
 * @param responseInfo CA response info.
 */
static void HandleCAResponses(const CARemoteEndpoint_t* endPoint,
        const CAResponseInfo_t* responseInfo);

/**
 * This function will be called back by CA layer when a request is received.
 *
 * @param endPoint CA remote endpoint.
 * @param requestInfo CA request info.
 */
static void HandleCARequests(const CARemoteEndpoint_t* endPoint,
        const CARequestInfo_t* requestInfo);

/**
 * Extract query from a URI.
 *
 * @param uri Full URI with query.
 * @param query Pointer to string that will contain query.
 * @param newURI Pointer to string that will contain URI.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult getQueryFromUri(const char * uri, char** resourceType, char ** newURI);

/**
 * Finds a resource type in an OCResourceType link-list.
 *
 * @param resourceTypeList The link-list to be searched through.
 * @param resourceTypeName The key to search for.
 *
 * @return Resource type that matches the key (ie. resourceTypeName) or
 *      NULL if there is either an invalid parameter or this function was unable to find the key.
 */
static OCResourceType *findResourceType(OCResourceType * resourceTypeList,
        const char * resourceTypeName);

/**
 * Reset presence TTL for a ClientCB struct. ttlLevel will be set to 0.
 * TTL will be set to maxAge.
 *
 * @param cbNode Callback Node for which presence ttl is to be reset.
 * @param maxAge New value of ttl in seconds.

 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
static OCStackResult ResetPresenceTTL(ClientCB *cbNode, uint32_t maxAgeSeconds);

//-----------------------------------------------------------------------------
// Internal functions
//-----------------------------------------------------------------------------

uint32_t GetTicks(uint32_t afterMilliSeconds)
{
    coap_tick_t now;
    coap_ticks(&now);

    // Guard against overflow of uint32_t
    if (afterMilliSeconds <= ((UINT32_MAX - (uint32_t)now) * MILLISECONDS_PER_SECOND) /
                             COAP_TICKS_PER_SECOND)
    {
        return now + (afterMilliSeconds * COAP_TICKS_PER_SECOND)/MILLISECONDS_PER_SECOND;
    }
    else
    {
        return UINT32_MAX;
    }
}

OCStackResult OCBuildIPv4Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d,
        uint16_t port, OCDevAddr *ipAddr)
{
    if (!ipAddr )
    {
        OC_LOG(FATAL, TAG, PCF("Invalid argument"));
        return OC_STACK_INVALID_PARAM;
    }

    ipAddr->addr[0] = a;
    ipAddr->addr[1] = b;
    ipAddr->addr[2] = c;
    ipAddr->addr[3] = d;
    ipAddr->addr[4] = (uint8_t)port;
    ipAddr->addr[5] = (uint8_t)(port >> 8);

    return OC_STACK_OK;
}

//-----------------------------------------------------------------------------
// Internal API function
//-----------------------------------------------------------------------------

// This internal function is called to update the stack with the status of
// observers and communication failures
OCStackResult OCStackFeedBack(CAToken_t token, uint8_t tokenLength, uint8_t status)
{
    OCStackResult result = OC_STACK_ERROR;
    ResourceObserver * observer = NULL;
    OCEntityHandlerRequest ehRequest = {};

    switch(status)
    {
    case OC_OBSERVER_NOT_INTERESTED:
        OC_LOG(DEBUG, TAG, PCF("observer is not interested in our notifications anymore"));
        observer = GetObserverUsingToken (token, tokenLength);
        if(observer)
        {
            result = FormOCEntityHandlerRequest(&ehRequest, (OCRequestHandle) NULL,
                    OC_REST_NOMETHOD, (OCResourceHandle) NULL, NULL, NULL, 0,
                    NULL, OC_OBSERVE_DEREGISTER, observer->observeId);
            if(result != OC_STACK_OK)
            {
                return result;
            }
            observer->resource->entityHandler(OC_OBSERVE_FLAG, &ehRequest);
        }
        //observer is not observing anymore
        result = DeleteObserverUsingToken (token, tokenLength);
        if(result == OC_STACK_OK)
        {
            OC_LOG(DEBUG, TAG, PCF("Removed observer successfully"));
        }
        else
        {
            result = OC_STACK_OK;
            OC_LOG(DEBUG, TAG, PCF("Observer Removal failed"));
        }
        break;
    case OC_OBSERVER_STILL_INTERESTED:
        //observer is still interested
        OC_LOG(DEBUG, TAG, PCF("observer is interested in our \
                notifications, reset the failedCount"));
        observer = GetObserverUsingToken (token, tokenLength);
        if(observer)
        {
            observer->forceHighQos = 0;
            observer->failedCommCount = 0;
            result = OC_STACK_OK;
        }
        else
        {
            result = OC_STACK_OBSERVER_NOT_FOUND;
        }
        break;
    case OC_OBSERVER_FAILED_COMM:
        //observer is not reachable
        OC_LOG(DEBUG, TAG, PCF("observer is unreachable"));
        observer = GetObserverUsingToken (token, tokenLength);
        if(observer)
        {
            if(observer->failedCommCount >= MAX_OBSERVER_FAILED_COMM)
            {
                result = FormOCEntityHandlerRequest(&ehRequest, (OCRequestHandle) NULL,
                        OC_REST_NOMETHOD, (OCResourceHandle) NULL, NULL, NULL, 0,
                        NULL, OC_OBSERVE_DEREGISTER, observer->observeId);
                if(result != OC_STACK_OK)
                {
                    return OC_STACK_ERROR;
                }
                observer->resource->entityHandler(OC_OBSERVE_FLAG, &ehRequest);
                //observer is unreachable
                result = DeleteObserverUsingToken (token, tokenLength);
                if(result == OC_STACK_OK)
                {
                    OC_LOG(DEBUG, TAG, PCF("Removed observer successfully"));
                }
                else
                {
                    result = OC_STACK_OK;
                    OC_LOG(DEBUG, TAG, PCF("Observer Removal failed"));
                }
            }
            else
            {
                observer->failedCommCount++;
                result = OC_STACK_CONTINUE;
            }
            observer->forceHighQos = 1;
            OC_LOG_V(DEBUG, TAG, "Failed count for this observer is %d",observer->failedCommCount);
        }
        break;
    default:
        OC_LOG(ERROR, TAG, PCF("Unknown status"));
        result = OC_STACK_ERROR;
        break;
        }
    return result;
}
OCStackResult CAToOCStackResult(CAResponseResult_t caCode)
{
    OCStackResult ret = OC_STACK_ERROR;

    switch(caCode)
    {
        case CA_SUCCESS:
            ret = OC_STACK_OK;
            break;
        case CA_CREATED:
            ret = OC_STACK_RESOURCE_CREATED;
            break;
        case CA_DELETED:
            ret = OC_STACK_RESOURCE_DELETED;
            break;
        case CA_BAD_REQ:
            ret = OC_STACK_INVALID_QUERY;
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

CAResponseResult_t OCToCAStackResult(OCStackResult ocCode)
{
    CAResponseResult_t ret = CA_INTERNAL_SERVER_ERROR;

    switch(ocCode)
    {
        case OC_STACK_OK:
            ret = CA_SUCCESS;
            break;
        case OC_STACK_RESOURCE_CREATED:
            ret = CA_CREATED;
            break;
        case OC_STACK_RESOURCE_DELETED:
            ret = CA_DELETED;
            break;
        case OC_STACK_INVALID_QUERY:
            ret = CA_BAD_REQ;
            break;
        case OC_STACK_INVALID_OPTION:
            ret = CA_BAD_OPT;
            break;
        case OC_STACK_NO_RESOURCE:
            ret = CA_NOT_FOUND;
            break;
        case OC_STACK_COMM_ERROR:
            ret = CA_RETRANSMIT_TIMEOUT;
            break;
        default:
            break;
    }
    return ret;
}

OCStackResult OCToCATransportType(OCConnectivityType ocConType, CATransportType_t* caConType)
{
    OCStackResult ret = OC_STACK_OK;

    switch(ocConType)
    {
        case OC_IPV4:
            *caConType = CA_IPV4;
            break;
        case OC_IPV6:
            *caConType = CA_IPV6;
            break;
        case OC_EDR:
            *caConType = CA_EDR;
            break;
        case OC_LE:
            *caConType = CA_LE;
            break;
        case OC_ALL:
            // Currently OC_ALL represents IPv4
            // Add other connectivity types as they are enabled in future
            *caConType = (CATransportType_t) (CA_IPV4);
            break;
        default:
            ret = OC_STACK_INVALID_PARAM;
            break;
    }
    return ret;
}

OCStackResult CAToOCConnectivityType(CATransportType_t caConType, OCConnectivityType *ocConType)
{
    OCStackResult ret = OC_STACK_OK;

    switch(caConType)
    {
        case CA_IPV4:
            *ocConType = OC_IPV4;
            break;
        case CA_IPV6:
            *ocConType = OC_IPV6;
            break;
        case CA_EDR:
            *ocConType = OC_EDR;
            break;
        case CA_LE:
            *ocConType = OC_LE;
            break;
        default:
            ret = OC_STACK_INVALID_PARAM;
            break;
    }
    return ret;
}

OCStackResult UpdateResponseAddr(OCDevAddr *address, const CARemoteEndpoint_t* endPoint)
{
    OCStackResult ret = OC_STACK_ERROR;
    char * tok = NULL;
    char * savePtr = NULL;
    char * cpAddress = (char *) OCMalloc(strlen(endPoint->addressInfo.IP.ipAddress) + 1);
    if(!cpAddress)
    {
        ret = OC_STACK_NO_MEMORY;
        goto exit;
    }
    memcpy(cpAddress, endPoint->addressInfo.IP.ipAddress,
            strlen(endPoint->addressInfo.IP.ipAddress) + 1);

    // Grabs the first three numbers from the IPv4 address and replaces dots
    for(int i=0; i<4; i++)
    {
        tok = strtok_r(i==0 ? cpAddress : NULL, ".", &savePtr);

        if(!tok)
        {
            ret = OC_STACK_ERROR;
            goto exit;
        }
        address->addr[i] = atoi(tok);
    }

    memcpy(&address->addr[4], &endPoint->addressInfo.IP.port, sizeof(uint16_t));
    ret = OC_STACK_OK;

exit:
    OCFree(cpAddress);
    return ret;
}

static OCStackResult ResetPresenceTTL(ClientCB *cbNode, uint32_t maxAgeSeconds)
{
    uint32_t lowerBound  = 0;
    uint32_t higherBound = 0;

    if (!cbNode || !cbNode->presence || !cbNode->presence->timeOut)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OC_LOG_V(INFO, TAG, "Update presence TTL, time is %u", GetTicks(0));

    cbNode->presence->TTL = maxAgeSeconds;

    for(int index = 0; index < PresenceTimeOutSize; index++)
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

void parsePresencePayload(char* payload, uint32_t* seqNum, uint32_t* maxAge, char** resType)
{
    char * tok = NULL;
    char * savePtr = NULL;
    // The format of the payload is {"oc":[%u:%u:%s]}
    // %u : sequence number,
    // %u : max age
    // %s : Resource Type (Optional)

    if (!payload || !seqNum || !maxAge || !resType)
    {
        return;
    }
    tok = strtok_r(payload, "[:]}", &savePtr);
    payload[strlen(payload)] = ':';

    //Retrieve sequence number
    tok = strtok_r(NULL, "[:]}", &savePtr);
    if(tok == NULL)
    {
        return;
    }
    payload[strlen((char *)payload)] = ':';
    *seqNum = (uint32_t) atoi(tok);

    //Retrieve MaxAge
    tok = strtok_r(NULL, "[:]}", &savePtr);
    if(tok == NULL)
    {
        return;
    }
    *maxAge = (uint32_t) atoi(tok);

    //Retrieve ResourceType
    tok = strtok_r(NULL, "[:]}",&savePtr);
    if(tok == NULL)
    {
        return;
    }

    *resType = (char *)OCMalloc(strlen(tok) + 1);
    if(!*resType)
    {
        return;
    }
    payload[strlen((char *)payload)] = ':';
    strcpy(*resType, tok);
    OC_LOG_V(DEBUG, TAG, "resourceTypeName %s", *resType);

    payload[strlen((char *)payload)] = ']';
}

static OCStackResult HandlePresenceResponse(const CARemoteEndpoint_t* endPoint,
                            const CAResponseInfo_t* responseInfo)
{
    OCStackApplicationResult cbResult = OC_STACK_DELETE_TRANSACTION;
    ClientCB * cbNode = NULL;
    char *resourceTypeName = NULL;
    OCClientResponse response = {};
    OCDevAddr address = {};
    OCStackResult result = OC_STACK_ERROR;
    uint32_t maxAge = 0;

    char *fullUri = NULL;
    char *ipAddress = NULL;
    int presenceSubscribe = 0;
    int multicastPresenceSubscribe = 0;
    size_t addressLen = 0;

    if (responseInfo->result != CA_SUCCESS)
    {
        OC_LOG_V(ERROR, TAG, "HandlePresenceResponse failed %d", responseInfo->result);
        return OC_STACK_ERROR;
    }

    fullUri = (char *) OCMalloc(MAX_URI_LENGTH);

    if(!fullUri)
    {
        OC_LOG(ERROR, TAG, PCF("Memory could not be allocated for fullUri"));
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    addressLen = strlen(endPoint->addressInfo.IP.ipAddress);
    ipAddress = (char *) OCMalloc(addressLen + 1);

    if(!ipAddress)
    {
        OC_LOG(ERROR, TAG, PCF("Memory could not be allocated for ipAddress"));
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    strncpy(ipAddress, endPoint->addressInfo.IP.ipAddress, addressLen);
    ipAddress[addressLen] = '\0';

    snprintf(fullUri, MAX_URI_LENGTH, "coap://%s:%u%s", ipAddress, endPoint->addressInfo.IP.port,
                OC_PRESENCE_URI);

    cbNode = GetClientCB(NULL, 0, NULL, fullUri);

    if(cbNode)
    {
        presenceSubscribe = 1;
    }
    else
    {
        snprintf(fullUri, MAX_URI_LENGTH, "coap://%s:%u%s", OC_MULTICAST_IP, OC_MULTICAST_PORT,
                                                endPoint->resourceUri);
        cbNode = GetClientCB(NULL, 0, NULL, fullUri);
        if(cbNode)
        {
            multicastPresenceSubscribe = 1;
        }
    }

    if(!presenceSubscribe && !multicastPresenceSubscribe)
    {
        OC_LOG(ERROR, TAG, PCF("Received a presence notification, but no callback, ignoring"));
        goto exit;
    }

    // No payload to the application in case of presence
    response.resJSONPayload = NULL;
    response.result = OC_STACK_OK;

    result = UpdateResponseAddr(&address, endPoint);
    if(result != OC_STACK_OK)
    {
        goto exit;
    }

    response.addr = &address;

    result = CAToOCConnectivityType(endPoint->transportType, &(response.connType));
    if(result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("Invalid connectivity type in endpoint"));
        goto exit;
    }

    if(responseInfo->info.payload)
    {
        parsePresencePayload(responseInfo->info.payload,
                                &(response.sequenceNumber),
                                &maxAge,
                                &resourceTypeName);
    }

    if(presenceSubscribe)
    {
        if(cbNode->sequenceNumber == response.sequenceNumber)
        {
            if (cbNode->presence)
            {
                OC_LOG(INFO, TAG, PCF("No presence change. Updating TTL."));

                result = ResetPresenceTTL(cbNode, maxAge);

                if (result != OC_STACK_OK)
                {
                    OC_LOG_V(ERROR, TAG, "ResetPresenceTTL failed with error: %u", result);
                }
            }
            else
            {
                OC_LOG(INFO, TAG, PCF("Not subscribed to presence."));
            }

            goto exit;
        }
        if(maxAge == 0)
        {
            OC_LOG(INFO, TAG, PCF("Stopping presence"));
            response.result = OC_STACK_PRESENCE_STOPPED;
            if(cbNode->presence)
            {
                OCFree(cbNode->presence->timeOut);
                OCFree(cbNode->presence);
                cbNode->presence = NULL;
            }
        }
        else
        {
            if(!cbNode->presence)
            {
                cbNode->presence = (OCPresence *) OCMalloc(sizeof(OCPresence));
                if(!(cbNode->presence))
                {
                    OC_LOG(ERROR, TAG, PCF("Could not allocate memory for cbNode->presence"));
                    result = OC_STACK_NO_MEMORY;
                    goto exit;
                }

                VERIFY_NON_NULL_V(cbNode->presence);
                cbNode->presence->timeOut = NULL;
                cbNode->presence->timeOut = (uint32_t *)
                        OCMalloc(PresenceTimeOutSize * sizeof(uint32_t));
                if(!(cbNode->presence->timeOut)){
                    OC_LOG(ERROR, TAG,
                                  PCF("Could not allocate memory for cbNode->presence->timeOut"));
                    OCFree(cbNode->presence);
                    result = OC_STACK_NO_MEMORY;
                    goto exit;
                }
            }

            ResetPresenceTTL(cbNode, maxAge);

            OC_LOG(INFO, TAG, PCF("Presence changed, calling up the stack"));
            cbNode->sequenceNumber = response.sequenceNumber;

            // Ensure that a filter is actually applied.
            if(resourceTypeName && cbNode->filterResourceType)
            {
                if(!findResourceType(cbNode->filterResourceType, resourceTypeName))
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
        mcNode = GetMCPresenceNode(fullUri);

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
            uint32_t uriLen = strlen(fullUri);
            char* uri = (char *) OCMalloc(uriLen + 1);
            if(uri)
            {
                memcpy(uri, fullUri, (uriLen + 1));
            }
            else
            {
                OC_LOG(ERROR, TAG,
                    PCF("No Memory for URI to store in the presence node"));
                result = OC_STACK_NO_MEMORY;
                goto exit;
            }
            result = AddMCPresenceNode(&mcNode, uri, response.sequenceNumber);
            if(result != OC_STACK_OK)
            {
                OC_LOG(ERROR, TAG,
                    PCF("Unable to add Multicast Presence Node"));
                OCFree(uri);
                goto exit;
            }
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
OCFree(fullUri);
OCFree(ipAddress);
OCFree(resourceTypeName);
return result;
}

void HandleCAResponses(const CARemoteEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo)
{
    OC_LOG(INFO, TAG, PCF("Enter HandleCAResponses"));

    if(NULL == endPoint)
    {
        OC_LOG(ERROR, TAG, PCF("endPoint is NULL"));
        return;
    }

    if(NULL == responseInfo)
    {
        OC_LOG(ERROR, TAG, PCF("responseInfo is NULL"));
        return;
    }

    if(strcmp(endPoint->resourceUri, OC_PRESENCE_URI) == 0)
    {
        HandlePresenceResponse(endPoint, responseInfo);
        return;
    }

    ClientCB *cbNode = GetClientCB(responseInfo->info.token,
            responseInfo->info.tokenLength, NULL, NULL);
    OC_LOG_V(DEBUG, TAG, "Response has the token %s", responseInfo->info.token);
    ResourceObserver * observer = GetObserverUsingToken (responseInfo->info.token,
            responseInfo->info.tokenLength);

    if(cbNode)
    {
        OC_LOG(INFO, TAG, PCF("There is a cbNode associated with the response token"));
        if(responseInfo->result == CA_EMPTY)
        {
            OC_LOG(INFO, TAG, PCF("Receiving A ACK/RESET for this token"));
            // We do not have a case for the client to receive a RESET
            if(responseInfo->info.type == CA_MSG_ACKNOWLEDGE)
            {
                //This is the case of receiving an ACK on a request to a slow resource!
                OC_LOG(INFO, TAG, PCF("This is a pure ACK"));
                //TODO: should we inform the client
                //      app that at least the request was received at the server?
            }
        }
        else if(responseInfo->result == CA_RETRANSMIT_TIMEOUT)
        {
            OC_LOG(INFO, TAG, PCF("Receiving A Timeout for this token"));
            OC_LOG(INFO, TAG, PCF("Calling into application address space"));
            OCClientResponse response = {};
            OCDevAddr address = {};
            OCStackResult result = UpdateResponseAddr(&address, endPoint);
            if(result != OC_STACK_OK)
            {
                OC_LOG(ERROR, TAG, PCF("Error parsing IP address in UpdateResponseAddr"));
                return;
            }

            result = UpdateResponseAddr(&address, endPoint);
            if(result != OC_STACK_OK)
            {
                OC_LOG(ERROR, TAG, PCF("Invalid connectivity type in endpoint"));
                return;
            }
            response.addr = &address;

            response.result = CAToOCStackResult(responseInfo->result);
            cbNode->callBack(cbNode->context,
                    cbNode->handle, &response);
            FindAndDeleteClientCB(cbNode);
        }
        else
        {
            OC_LOG(INFO, TAG, PCF("This is a regular response, A client call back is found"));
            OC_LOG(INFO, TAG, PCF("Calling into application address space"));
            OCClientResponse response = {};
            OCDevAddr address = {};

            OCStackResult result = UpdateResponseAddr(&address, endPoint);
            if(result != OC_STACK_OK)
            {
                OC_LOG(ERROR, TAG, PCF("Error parsing IP address in UpdateResponseAddr"));
                return;
            }
            response.addr = &address;
            // Populate the connectivity type. If this is a discovery response,
            // the resource that will be constructed from this response will make
            // further API calls from this interface.
            result = CAToOCConnectivityType(endPoint->transportType,
                                    &(response.connType));
            if(result != OC_STACK_OK)
            {
                OC_LOG(ERROR, TAG, PCF("Invalid connectivity type in endpoint"));
                return;
            }

            response.result = CAToOCStackResult(responseInfo->result);
            response.resJSONPayload = (const char*)responseInfo->info.payload;
            response.numRcvdVendorSpecificHeaderOptions = 0;
            if(responseInfo->info.numOptions > 0)
            {
                int start = 0;
                //First option always with option ID is COAP_OPTION_OBSERVE if it is available.
                if(responseInfo->info.options[0].optionID == COAP_OPTION_OBSERVE)
                {
                    memcpy (&(response.sequenceNumber),
                            &(responseInfo->info.options[0].optionData), sizeof(uint32_t));
                    response.numRcvdVendorSpecificHeaderOptions = responseInfo->info.numOptions - 1;
                    start = 1;
                }
                else
                {
                    response.numRcvdVendorSpecificHeaderOptions = responseInfo->info.numOptions;
                }

                if(response.numRcvdVendorSpecificHeaderOptions > MAX_HEADER_OPTIONS)
                {
                    OC_LOG(ERROR, TAG, PCF("#header options are more than MAX_HEADER_OPTIONS"));
                    return;
                }

                for (uint8_t i = start; i < responseInfo->info.numOptions; i++)
                {
                    memcpy (&(response.rcvdVendorSpecificHeaderOptions[i-start]),
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
            if(responseInfo->info.type == CA_MSG_CONFIRM)
            {
                SendDirectStackResponse(endPoint, responseInfo->info.messageId, CA_EMPTY,
                        CA_MSG_ACKNOWLEDGE, 0, NULL, NULL, 0);
            }
        }
        return;
    }

    if(observer)
    {
        OC_LOG(INFO, TAG, PCF("There is an observer associated with the response token"));
        if(responseInfo->result == CA_EMPTY)
        {
            OC_LOG(INFO, TAG, PCF("Receiving A ACK/RESET for this token"));
            if(responseInfo->info.type == CA_MSG_RESET)
            {
                OC_LOG(INFO, TAG, PCF("This is a RESET"));
                OCStackFeedBack(responseInfo->info.token, responseInfo->info.tokenLength,
                        OC_OBSERVER_NOT_INTERESTED);
            }
            else if(responseInfo->info.type == CA_MSG_ACKNOWLEDGE)
            {
                OC_LOG(INFO, TAG, PCF("This is a pure ACK"));
                OCStackFeedBack(responseInfo->info.token, responseInfo->info.tokenLength,
                        OC_OBSERVER_STILL_INTERESTED);
            }
        }
        else if(responseInfo->result == CA_RETRANSMIT_TIMEOUT)
        {
            OC_LOG(INFO, TAG, PCF("Receiving Time Out for an observer"));
            OCStackFeedBack(responseInfo->info.token, responseInfo->info.tokenLength,
                    OC_OBSERVER_FAILED_COMM);
        }
        return;
    }

    if(!cbNode && !observer)
    {
        if(myStackMode == OC_CLIENT || myStackMode == OC_CLIENT_SERVER)
        {
            OC_LOG(INFO, TAG, PCF("This is a client, but no cbNode was found for token"));
            if(responseInfo->result == CA_EMPTY)
            {
                OC_LOG(INFO, TAG, PCF("Receiving CA_EMPTY in the ocstack"));
            }
            else
            {
                OC_LOG(INFO, TAG, PCF("Received a response or notification,\
                        but I do not have callback. Sending RESET"));
                SendDirectStackResponse(endPoint, responseInfo->info.messageId, CA_EMPTY,
                        CA_MSG_RESET, 0, NULL, NULL, 0);
            }
        }

        if(myStackMode == OC_SERVER || myStackMode == OC_CLIENT_SERVER)
        {
            OC_LOG(INFO, TAG, PCF("This is a server, but no observer was found for token"));
            if (responseInfo->info.type == CA_MSG_ACKNOWLEDGE)
            {
                OC_LOG_V(INFO, TAG, PCF("Received ACK at server for messageId : %d"),
                                            responseInfo->info.messageId);
            }
            if (responseInfo->info.type == CA_MSG_RESET)
            {
                OC_LOG_V(INFO, TAG, PCF("Received RESET at server for messageId : %d"),
                                            responseInfo->info.messageId);
            }
        }
        return;
    }

    OC_LOG_V(INFO, TAG, PCF("Received payload: %s\n"), (char*)responseInfo->info.payload);
    OC_LOG(INFO, TAG, PCF("Exit HandleCAResponses"));
}

/*
 * This function sends out Direct Stack Responses. These are responses that are not coming
 * from the application entity handler. These responses have no payload and are usually ACKs,
 * RESETs or some error conditions that were caught by the stack.
 */
OCStackResult SendDirectStackResponse(const CARemoteEndpoint_t* endPoint, const uint16_t coapID,
        const CAResponseResult_t responseResult, const CAMessageType_t type,
        const uint8_t numOptions, const CAHeaderOption_t *options,
        CAToken_t token, uint8_t tokenLength)
{
    CAResponseInfo_t respInfo = {};
    respInfo.result = responseResult;
    respInfo.info.messageId = coapID;
    respInfo.info.numOptions = numOptions;
    respInfo.info.options = (CAHeaderOption_t*)options;
    respInfo.info.payload = NULL;
    respInfo.info.token = token;
    respInfo.info.tokenLength = tokenLength;
    respInfo.info.type = type;

    CAResult_t caResult = CASendResponse(endPoint, &respInfo);
    if(caResult != CA_STATUS_OK)
    {
        OC_LOG(ERROR, TAG, PCF("CASendResponse error"));
        return OC_STACK_ERROR;
    }
    return OC_STACK_OK;
}

//This function will be called back by CA layer when a request is received
void HandleCARequests(const CARemoteEndpoint_t* endPoint, const CARequestInfo_t* requestInfo)
{
    OC_LOG(INFO, TAG, PCF("Enter HandleCARequests"));
    if(!endPoint)
    {
        OC_LOG(ERROR, TAG, PCF("endPoint is NULL"));
        return;
    }

    if(!requestInfo)
    {
        OC_LOG(ERROR, TAG, PCF("requestInfo is NULL"));
        return;
    }

    OCStackResult requestResult = OC_STACK_ERROR;

    if(myStackMode == OC_CLIENT)
    {
        //TODO: should the client be responding to requests?
        return;
    }

    OCServerProtocolRequest serverRequest = {};

    OC_LOG_V(INFO, TAG, PCF("Endpoint URI : %s\n"), (char*)endPoint->resourceUri);

    char * newUri = NULL;
    char * query  = NULL;

    requestResult = getQueryFromUri(endPoint->resourceUri, &query, &newUri);

    if (requestResult != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, "getQueryFromUri() failed with OC error code %d\n", requestResult);
        return;
    }
    OC_LOG_V(INFO, TAG, PCF("URI without query: %s\n"), newUri);
    OC_LOG_V(INFO, TAG, PCF("Query : %s\n"), query);

    if(strlen(newUri) < MAX_URI_LENGTH)
    {
        //copy URI
        memcpy (&(serverRequest.resourceUrl), newUri, strlen(newUri));
        OCFree(newUri);
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("URI length exceeds MAX_URI_LENGTH."));
        OCFree(newUri);
        OCFree(query);
        return;
    }
    //copy query
    if(query)
    {
        if(strlen(query) < MAX_QUERY_LENGTH)
        {
            memcpy (&(serverRequest.query), query, strlen(query));
            OCFree(query);
        }
        else
        {
            OC_LOG(ERROR, TAG, PCF("Query length exceeds MAX_QUERY_LENGTH."));
            OCFree(query);
            return;
        }
    }
    //copy request payload
    if (requestInfo->info.payload)
    {
        size_t payloadLen = strlen(requestInfo->info.payload);
        serverRequest.reqTotalSize = payloadLen + 1;
        memcpy (&(serverRequest.reqJSONPayload), requestInfo->info.payload,
                payloadLen);
    }
    else
    {
        serverRequest.reqTotalSize = 1;
        serverRequest.reqJSONPayload[0] = '\0';
    }

    switch (requestInfo->method)
    {
        case CA_GET:
            {
                serverRequest.method = OC_REST_GET;
                break;
            }
        case CA_PUT:
            {
                serverRequest.method = OC_REST_PUT;
                break;
            }
        case CA_POST:
            {
                serverRequest.method = OC_REST_POST;
                break;
            }
        case CA_DELETE:
            {
                serverRequest.method = OC_REST_DELETE;
                break;
            }
        default:
            {
                OC_LOG(ERROR, TAG, PCF("Received CA method %d not supported"));
                SendDirectStackResponse(endPoint, requestInfo->info.messageId, CA_BAD_REQ,
                        requestInfo->info.type, requestInfo->info.numOptions,
                        requestInfo->info.options, requestInfo->info.token,
                        requestInfo->info.tokenLength);
                return;
            }
    }

    OC_LOG_V(INFO, TAG, "HandleCARequests: CA token length = %d",
            requestInfo->info.tokenLength);
    OC_LOG_BUFFER(INFO, TAG, (const uint8_t *)requestInfo->info.token,
            requestInfo->info.tokenLength);
    serverRequest.requestToken = (CAToken_t)OCMalloc(requestInfo->info.tokenLength);
    serverRequest.tokenLength = requestInfo->info.tokenLength;
    // Module Name
    if (!serverRequest.requestToken)
    {
        OC_LOG(FATAL, TAG, "Server Request Token is NULL");
        SendDirectStackResponse(endPoint, requestInfo->info.messageId, CA_INTERNAL_SERVER_ERROR,
                requestInfo->info.type, requestInfo->info.numOptions,
                requestInfo->info.options, requestInfo->info.token,
                requestInfo->info.tokenLength);
        return;
    }
    memcpy(serverRequest.requestToken, requestInfo->info.token, requestInfo->info.tokenLength);

    if (requestInfo->info.type == CA_MSG_CONFIRM)
    {
        serverRequest.qos = OC_HIGH_QOS;
    }
    else
    {
        serverRequest.qos = OC_LOW_QOS;
    }
    // CA does not need the following 2 fields
    // Are we sure CA does not need them? how is it responding to multicast
    serverRequest.delayedResNeeded = 0;
    serverRequest.secured = endPoint->isSecured;

    serverRequest.coapID = requestInfo->info.messageId;

    // copy the address
    serverRequest.addressInfo      = endPoint->addressInfo;
    serverRequest.connectivityType = endPoint->transportType;

    // copy vendor specific header options
    uint8_t tempNum = (requestInfo->info.numOptions);
    GetObserveHeaderOption(&serverRequest.observationOption, requestInfo->info.options, &tempNum);
    if (requestInfo->info.numOptions > MAX_HEADER_OPTIONS)
    {
        OC_LOG(ERROR, TAG,
                PCF("The request info numOptions is greater than MAX_HEADER_OPTIONS"));
        SendDirectStackResponse(endPoint, requestInfo->info.messageId, CA_BAD_OPT,
                requestInfo->info.type, requestInfo->info.numOptions,
                requestInfo->info.options, requestInfo->info.token,
                requestInfo->info.tokenLength);
        OCFree(serverRequest.requestToken);
        return;
    }
    serverRequest.numRcvdVendorSpecificHeaderOptions = tempNum;
    if (serverRequest.numRcvdVendorSpecificHeaderOptions)
    {
        memcpy (&(serverRequest.rcvdVendorSpecificHeaderOptions), requestInfo->info.options,
            sizeof(CAHeaderOption_t)*tempNum);
    }

    requestResult = HandleStackRequests (&serverRequest);

    // Send ACK to client as precursor to slow response
    if(requestResult == OC_STACK_SLOW_RESOURCE)
    {
        SendDirectStackResponse(endPoint, requestInfo->info.messageId, CA_EMPTY,
                    CA_MSG_ACKNOWLEDGE,0, NULL, NULL, 0);
    }
    else if(requestResult != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, PCF("HandleStackRequests failed. error: %d"), requestResult);

        CAResponseResult_t stackResponse = OCToCAStackResult(requestResult);

        SendDirectStackResponse(endPoint, requestInfo->info.messageId, stackResponse,
                requestInfo->info.type, requestInfo->info.numOptions,
                requestInfo->info.options, requestInfo->info.token,
                requestInfo->info.tokenLength);
    }
    // requestToken is fed to HandleStackRequests, which then goes to AddServerRequest.
    // The token is copied in there, and is thus still owned by this function.
    OCFree(serverRequest.requestToken);
    OC_LOG(INFO, TAG, PCF("Exit HandleCARequests"));
}

OCStackResult HandleStackRequests(OCServerProtocolRequest * protocolRequest)
{
    OC_LOG(INFO, TAG, PCF("Entering HandleStackRequests (OCStack Layer)"));
    OCStackResult result = OC_STACK_ERROR;
    ResourceHandling resHandling;
    OCResource *resource;
    if(!protocolRequest)
    {
        OC_LOG(ERROR, TAG, PCF("protocolRequest is NULL"));
        return OC_STACK_INVALID_PARAM;
    }

    OCServerRequest * request = GetServerRequestUsingToken(protocolRequest->requestToken,
            protocolRequest->tokenLength);
    if(!request)
    {
        OC_LOG(INFO, TAG, PCF("This is a new Server Request"));
        result = AddServerRequest(&request, protocolRequest->coapID,
                protocolRequest->delayedResNeeded, protocolRequest->secured, 0,
                protocolRequest->method, protocolRequest->numRcvdVendorSpecificHeaderOptions,
                protocolRequest->observationOption, protocolRequest->qos,
                protocolRequest->query, protocolRequest->rcvdVendorSpecificHeaderOptions,
                protocolRequest->reqJSONPayload, protocolRequest->requestToken,
                protocolRequest->tokenLength,
                protocolRequest->resourceUrl,protocolRequest->reqTotalSize,
                &protocolRequest->addressInfo, protocolRequest->connectivityType);
        if (OC_STACK_OK != result)
        {
            OC_LOG(ERROR, TAG, PCF("Error adding server request"));
            return result;
        }

        if(!request)
        {
            OC_LOG(ERROR, TAG, PCF("Out of Memory"));
            return OC_STACK_NO_MEMORY;
        }

        if(!protocolRequest->reqMorePacket)
        {
            request->requestComplete = 1;
        }
    }
    else
    {
        OC_LOG(INFO, TAG,
                PCF("This is either a repeated or blocked Server Request"));
    }

    if(request->requestComplete)
    {
        OC_LOG(INFO, TAG, PCF("This Server Request is complete"));
        result = DetermineResourceHandling (request, &resHandling, &resource);
        if (result == OC_STACK_OK)
        {
            result = ProcessRequest(resHandling, resource, request);
        }
    }
    else
    {
        OC_LOG(INFO, TAG, PCF("This Server Request is incomplete"));
        result = OC_STACK_CONTINUE;
    }
    return result;
}

bool ParseIPv4Address(char * ipAddrStr, uint8_t * ipAddr, uint16_t * port)
{
    size_t index = 0;
     char *itr, *coap;
     uint8_t dotCount = 0;

     ipAddr[index] = 0;
     *port = 0;
     /* search for scheme */
     itr = ipAddrStr;
    if (!isdigit((char) *ipAddrStr))
    {
        coap = OC_COAP_SCHEME;
        while (*coap && tolower(*itr) == *coap)
        {
            coap++;
            itr++;
        }
    }
    ipAddrStr = itr;

    while (*ipAddrStr)
    {
        if (isdigit(*ipAddrStr))
        {
            ipAddr[index] *= 10;
            ipAddr[index] += *ipAddrStr - '0';
        }
        else if (*ipAddrStr == '.')
        {
            index++;
            dotCount++;
            ipAddr[index] = 0;
        }
        else
        {
            break;
        }
        ipAddrStr++;
    }
    if(*ipAddrStr == ':')
    {
        ipAddrStr++;
        while (*ipAddrStr){
            if (isdigit(*ipAddrStr))
            {
                *port *= 10;
                *port += *ipAddrStr - '0';
            }
            else
            {
                break;
            }
            ipAddrStr++;
        }
    }

    return (3 == dotCount);
}

bool validatePlatformInfo(OCPlatformInfo info)
{

    if (!info.platformID)
    {
        OC_LOG(ERROR, TAG, PCF("No platform ID found."));
        return false;
    }

    if (info.manufacturerName)
    {
        size_t lenManufacturerName = strlen(info.manufacturerName);

        if(lenManufacturerName == 0 || lenManufacturerName > MAX_MANUFACTURER_NAME_LENGTH)
        {
            OC_LOG(ERROR, TAG, PCF("Manufacturer name fails length requirements."));
            return false;
        }
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("No manufacturer name present"));
        return false;
    }

    if (info.manufacturerUrl)
    {
        if(strlen(info.manufacturerUrl) > MAX_MANUFACTURER_URL_LENGTH)
        {
            OC_LOG(ERROR, TAG, PCF("Manufacturer url fails length requirements."));
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
// Public APIs
//-----------------------------------------------------------------------------

OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
{
    if(stackState == OC_STACK_INITIALIZED)
    {
        OC_LOG(INFO, TAG, PCF("Subsequent calls to OCInit() without calling \
                OCStop() between them are ignored."));
        return OC_STACK_OK;
    }

    (void) ipAddr;
    (void) port;
    OCStackResult result = OC_STACK_ERROR;
    OC_LOG(INFO, TAG, PCF("Entering OCInit"));

    // Validate mode
    if (!((mode == OC_CLIENT) || (mode == OC_SERVER) || (mode == OC_CLIENT_SERVER)))
    {
        OC_LOG(ERROR, TAG, PCF("Invalid mode"));
        return OC_STACK_ERROR;
    }
    myStackMode = mode;

    defaultDeviceHandler = NULL;
    OCSeedRandom();

    result = CAResultToOCResult(CAInitialize());
    VERIFY_SUCCESS(result, OC_STACK_OK);

    result = CAResultToOCResult(OCSelectNetwork());
    VERIFY_SUCCESS(result, OC_STACK_OK);

    CARegisterHandler(HandleCARequests, HandleCAResponses);
    switch (myStackMode)
    {
        case OC_CLIENT:
            result = CAResultToOCResult(CAStartDiscoveryServer());
            OC_LOG(INFO, TAG, PCF("Client mode: CAStartDiscoveryServer"));
            break;
        case OC_SERVER:
            result = CAResultToOCResult(CAStartListeningServer());
            OC_LOG(INFO, TAG, PCF("Server mode: CAStartListeningServer"));
            break;
        case OC_CLIENT_SERVER:
            result = CAResultToOCResult(CAStartListeningServer());
            if(result == OC_STACK_OK)
            {
                result = CAResultToOCResult(CAStartDiscoveryServer());
            }
            break;
    }
    VERIFY_SUCCESS(result, OC_STACK_OK);

#if defined(__WITH_DTLS__)
    result = (CARegisterDTLSCredentialsHandler(GetDtlsPskCredentials) == CA_STATUS_OK) ?
        OC_STACK_OK : OC_STACK_ERROR;
    VERIFY_SUCCESS(result, OC_STACK_OK);
#endif // (__WITH_DTLS__)

#ifdef WITH_PRESENCE
    PresenceTimeOutSize = sizeof(PresenceTimeOut)/sizeof(PresenceTimeOut[0]) - 1;
#endif // WITH_PRESENCE

    //Update Stack state to initialized
    stackState = OC_STACK_INITIALIZED;

    // Initialize resource
    if(myStackMode != OC_CLIENT)
    {
        result = initResources();
    }

exit:
    if(result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("Stack initialization error"));
        deleteAllResources();
        CATerminate();
        stackState = OC_STACK_UNINITIALIZED;
    }
    return result;
}

OCStackResult OCStop()
{
    OC_LOG(INFO, TAG, PCF("Entering OCStop"));

    if (stackState == OC_STACK_UNINIT_IN_PROGRESS)
    {
        OC_LOG(DEBUG, TAG, PCF("Stack already stopping, exiting"));
        return OC_STACK_OK;
    }
    else if (stackState != OC_STACK_INITIALIZED)
    {
        OC_LOG(ERROR, TAG, PCF("Stack not initialized"));
        return OC_STACK_ERROR;
    }

    stackState = OC_STACK_UNINIT_IN_PROGRESS;

    #ifdef WITH_PRESENCE
    // Ensure that the TTL associated with ANY and ALL presence notifications originating from
    // here send with the code "OC_STACK_PRESENCE_STOPPED" result.
    presenceResource.presenceTTL = 0;
    #endif // WITH_PRESENCE

    // Free memory dynamically allocated for resources
    deleteAllResources();
    DeleteDeviceInfo();
    DeletePlatformInfo();
    CATerminate();
    // Remove all observers
    DeleteObserverList();
    // Remove all the client callbacks
    DeleteClientCBList();
    // Deinit security blob
    DeinitOCSecurityInfo();
    stackState = OC_STACK_UNINITIALIZED;
    return OC_STACK_OK;
}

CAMessageType_t qualityOfServiceToMessageType(OCQualityOfService qos)
{
    switch (qos)
    {
        case OC_HIGH_QOS:
            return CA_MSG_CONFIRM;
        case OC_LOW_QOS:
        case OC_MEDIUM_QOS:
        case OC_NA_QOS:
        default:
            return CA_MSG_NONCONFIRM;
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

        if((inputUri + uriLen - 1 - query) > MAX_QUERY_LENGTH)
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

OCStackResult OCDoResource(OCDoHandle *handle, OCMethod method, const char *requiredUri,
            const char *referenceUri, const char *request, OCConnectivityType conType,
            OCQualityOfService qos, OCCallbackData *cbData,
            OCHeaderOption * options, uint8_t numOptions)
{
    OCStackResult result = OC_STACK_ERROR;
    ClientCB *clientCB = NULL;
    char * requestUri = NULL;
    char * resourceType = NULL;
    char * query = NULL;
    char * newUri = (char *)requiredUri;
    (void) referenceUri;
    CARemoteEndpoint_t* endpoint = NULL;
    CAResult_t caResult;
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    OCDoHandle resHandle = NULL;
    CAInfo_t requestData ={};
    CARequestInfo_t requestInfo ={};
    CAGroupEndpoint_t grpEnd = {};

    OC_LOG(INFO, TAG, PCF("Entering OCDoResource"));

    // Validate input parameters
    VERIFY_NON_NULL(cbData, FATAL, OC_STACK_INVALID_CALLBACK);
    VERIFY_NON_NULL(cbData->cb, FATAL, OC_STACK_INVALID_CALLBACK);

    //TODO ("Need to form the final query by concatenating require and reference URI's");
    VERIFY_NON_NULL(requiredUri, FATAL, OC_STACK_INVALID_URI);

    uint16_t uriLen = strlen(requiredUri);

    // ToDo: We should also check if the requiredUri has a mutlicast address,
    // then qos has to be OC_Low_QOS
    switch (method)
    {
        case OC_REST_GET:
        case OC_REST_OBSERVE:
        case OC_REST_OBSERVE_ALL:
        case OC_REST_CANCEL_OBSERVE:
            requestInfo.method = CA_GET;
            break;

        case OC_REST_PUT:
            requestInfo.method = CA_PUT;
            break;

        case OC_REST_POST:
            requestInfo.method = CA_POST;
            break;

        case OC_REST_DELETE:
            requestInfo.method = CA_DELETE;
            break;

        #ifdef WITH_PRESENCE
        case OC_REST_PRESENCE:
            // Replacing method type with GET because "presence"
            // is a stack layer only implementation.
            requestInfo.method = CA_GET;
            break;
        #endif

        default:
            result = OC_STACK_INVALID_METHOD;
            goto exit;
    }

    if((result = verifyUriQueryLength(requiredUri, uriLen)) != OC_STACK_OK)
    {
        goto exit;
    }

    if((request) && (strlen(request) > MAX_REQUEST_LENGTH))
    {
        result = OC_STACK_INVALID_PARAM;
        goto exit;
    }

#ifdef WITH_PRESENCE
    if(method == OC_REST_PRESENCE)
    {
        result = getQueryFromUri(requiredUri, &query, &newUri);

        if(result != OC_STACK_OK)
        {
            OC_LOG_V(ERROR, TAG, "Invalid Param from getQueryFromUri: %d, URI is %s",
                    result, requiredUri);
            goto exit;
        }

        if(query)
        {
            result = getResourceType((char *) query, &resourceType);
            OCFree(query);
            if(resourceType)
            {
                OC_LOG_V(DEBUG, TAG, "Got Resource Type: %s", resourceType);
            }
            else
            {
                OC_LOG(DEBUG, TAG, PCF("Resource type is NULL."));
            }
        }
        else
        {
            OC_LOG(DEBUG, TAG, PCF("Query string is NULL."));
        }
        if(result != OC_STACK_OK)
        {
            goto exit;
        }
    }
#endif // WITH_PRESENCE

    requestUri = (char *) OCMalloc(uriLen + 1);
    if(requestUri)
    {
        memcpy(requestUri, newUri, (uriLen + 1));
    }
    else
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    resHandle = GenerateInvocationHandle();
    if(!resHandle)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    // create token
    caResult = CAGenerateToken(&token, tokenLength);
    if (caResult != CA_STATUS_OK)
    {
        OC_LOG(ERROR, TAG, PCF("CAGenerateToken error"));
        CADestroyToken(token);
        result = CAResultToOCResult (caResult);
        goto exit;
    }

    requestData.type = qualityOfServiceToMessageType(qos);

    requestData.token = token;
    requestData.tokenLength = tokenLength;

    if ((method == OC_REST_OBSERVE) || (method == OC_REST_OBSERVE_ALL))
    {
        result = CreateObserveHeaderOption (&(requestData.options), options,
                                    numOptions, OC_OBSERVE_REGISTER);
        if (result != OC_STACK_OK)
        {
            CADestroyToken(token);
            goto exit;
        }
        requestData.numOptions = numOptions + 1;
    }
    else
    {
        requestData.options = (CAHeaderOption_t*)options;
        requestData.numOptions = numOptions;
    }

    requestData.payload = (char *)request;

    requestInfo.info = requestData;

    CATransportType_t caConType;

    result = OCToCATransportType((OCConnectivityType) conType, &caConType);
    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("Invalid Connectivity Type"));
        goto exit;
    }

    // send request
    if(conType == OC_ALL)
    {
        grpEnd.transportType = caConType;

        grpEnd.resourceUri = (CAURI_t) OCMalloc(uriLen + 1);
        if(!grpEnd.resourceUri)
        {
            result = OC_STACK_NO_MEMORY;
            CADestroyToken(token);
            goto exit;
        }
        strncpy(grpEnd.resourceUri, requiredUri, (uriLen + 1));

        caResult = CASendRequestToAll(&grpEnd, &requestInfo);
    }
    else
    {
        caResult = CACreateRemoteEndpoint(newUri, caConType, &endpoint);

        if (caResult != CA_STATUS_OK)
        {
            OC_LOG(ERROR, TAG, PCF("CACreateRemoteEndpoint error"));
            result = CAResultToOCResult (caResult);
            CADestroyToken(token);
            goto exit;
        }

        caResult = CASendRequest(endpoint, &requestInfo);
    }

    if (caResult != CA_STATUS_OK)
    {
        OC_LOG(ERROR, TAG, PCF("CASendRequest"));
        result = CAResultToOCResult (caResult);
        CADestroyToken(token);
        goto exit;
    }

    result = AddClientCB(&clientCB, cbData, token, tokenLength, &resHandle, method,
                             requestUri, resourceType, conType,
                             GetTicks(MAX_CB_TIMEOUT_SECONDS * MILLISECONDS_PER_SECOND));
    if(result != OC_STACK_OK)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    if(handle)
    {
        *handle = resHandle;
    }

exit:
    if(newUri != requiredUri)
    {
        OCFree(newUri);
    }
    if (result != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, PCF("OCDoResource error no %d"), result);
        FindAndDeleteClientCB(clientCB);
        OCFree(resHandle);
        OCFree(requestUri);
        OCFree(resourceType);
    }
    CADestroyRemoteEndpoint(endpoint);
    OCFree(grpEnd.resourceUri);

    if (requestData.options  && requestData.numOptions > 0)
    {
        if ((method == OC_REST_OBSERVE) || (method == OC_REST_OBSERVE_ALL))
        {
            OCFree(requestData.options);
        }
    }
    return result;
}

OCStackResult OCCancel(OCDoHandle handle, OCQualityOfService qos, OCHeaderOption * options,
        uint8_t numOptions)
{
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
    CARemoteEndpoint_t* endpoint = NULL;
    CAResult_t caResult;
    CAInfo_t requestData = {};
    CARequestInfo_t requestInfo = {};

    if(!handle)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OC_LOG(INFO, TAG, PCF("Entering OCCancel"));

    ClientCB *clientCB = GetClientCB(NULL, 0, handle, NULL);

    if(clientCB)
    {
        switch (clientCB->method)
        {
            case OC_REST_OBSERVE:
            case OC_REST_OBSERVE_ALL:
                if(qos == OC_HIGH_QOS)
                {
                    requestData.type =  qualityOfServiceToMessageType(qos);
                    requestData.token = clientCB->token;
                    requestData.tokenLength = clientCB->tokenLength;

                    if (CreateObserveHeaderOption (&(requestData.options),
                            options, numOptions, OC_OBSERVE_DEREGISTER) != OC_STACK_OK)
                    {
                        return OC_STACK_ERROR;
                    }
                    requestData.numOptions = numOptions + 1;
                    requestInfo.method = CA_GET;
                    requestInfo.info = requestData;

                    CATransportType_t caConType;
                    ret = OCToCATransportType(clientCB->conType, &caConType);
                    if(ret != OC_STACK_OK)
                    {
                        goto Error;
                    }

                    caResult = CACreateRemoteEndpoint((char *)clientCB->requestUri,
                            caConType, &endpoint);
                    if (caResult != CA_STATUS_OK)
                    {
                        OC_LOG(ERROR, TAG, PCF("CACreateRemoteEndpoint error"));
                        ret = OC_STACK_ERROR;
                        goto Error;
                    }

                    // send request
                    caResult = CASendRequest(endpoint, &requestInfo);
                    if (caResult != CA_STATUS_OK)
                    {
                        OC_LOG(ERROR, TAG, PCF("CASendRequest error"));
                        ret = OC_STACK_ERROR;
                    }
                    ret = CAResultToOCResult (caResult);
                }
                else
                {
                    FindAndDeleteClientCB(clientCB);
                }
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
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("Client callback not found. Called OCCancel twice?"));
    }

    Error:
    CADestroyRemoteEndpoint(endpoint);
    if (requestData.numOptions > 0)
    {
        OCFree(requestData.options);
    }

    return ret;
}

#ifdef WITH_PRESENCE
OCStackResult OCProcessPresence()
{
    OCStackResult result = OC_STACK_OK;
    uint8_t ipAddr[4] = { 0 };
    uint16_t port = 0;

    OC_LOG(INFO, TAG, PCF("Entering RequestPresence"));
    ClientCB* cbNode = NULL;
    OCDevAddr dst = {};
    OCClientResponse clientResponse ={};
    OCStackApplicationResult cbResult = OC_STACK_DELETE_TRANSACTION;

    LL_FOREACH(cbList, cbNode)
    {
        if(OC_REST_PRESENCE == cbNode->method)
        {
            if(cbNode->presence)
            {
                uint32_t now = GetTicks(0);
                OC_LOG_V(DEBUG, TAG, "this TTL level %d",
                                                        cbNode->presence->TTLlevel);
                OC_LOG_V(DEBUG, TAG, "current ticks %d", now);


                if(cbNode->presence->TTLlevel >= (PresenceTimeOutSize + 1))
                {
                    goto exit;
                }

                if(cbNode->presence->TTLlevel < PresenceTimeOutSize)
                {
                    OC_LOG_V(DEBUG, TAG, "timeout ticks %d",
                            cbNode->presence->timeOut[cbNode->presence->TTLlevel]);
                }

                if(cbNode->presence->TTLlevel >= PresenceTimeOutSize)
                {
                    OC_LOG(DEBUG, TAG, PCF("No more timeout ticks"));
                    if (ParseIPv4Address(cbNode->requestUri, ipAddr, &port))
                    {
                        OCBuildIPv4Address(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3], port,
                                &dst);

                        clientResponse.sequenceNumber = 0;
                        clientResponse.result = OC_STACK_PRESENCE_TIMEOUT;
                        clientResponse.addr = (OCDevAddr *) &dst;
                        clientResponse.resJSONPayload = NULL;

                        // Increment the TTLLevel (going to a next state), so we don't keep
                        // sending presence notification to client.
                        cbNode->presence->TTLlevel++;
                        OC_LOG_V(DEBUG, TAG, "moving to TTL level %d",
                                                cbNode->presence->TTLlevel);
                    }
                    else
                    {
                        result = OC_STACK_INVALID_IP;
                        goto exit;
                    }

                    cbResult = cbNode->callBack(cbNode->context, cbNode->handle, &clientResponse);
                    if (cbResult == OC_STACK_DELETE_TRANSACTION)
                    {
                        FindAndDeleteClientCB(cbNode);
                    }
                }

                if(now >= cbNode->presence->timeOut[cbNode->presence->TTLlevel])
                {
                    CAResult_t caResult = CA_STATUS_OK;
                    CARemoteEndpoint_t* endpoint = NULL;
                    CAInfo_t requestData ={};
                    CARequestInfo_t requestInfo = {};

                    OC_LOG(DEBUG, TAG, PCF("time to test server presence"));


                    CATransportType_t caConType;
                    result = OCToCATransportType(cbNode->conType, &caConType);
                    caResult = CACreateRemoteEndpoint((char *)cbNode->requestUri, caConType,
                                                        &endpoint);
                    if (caResult != CA_STATUS_OK || result != OC_STACK_OK)
                    {
                        OC_LOG(ERROR, TAG, PCF("CACreateRemoteEndpoint error"));
                        goto exit;
                    }

                    requestData.type = CA_MSG_NONCONFIRM;
                    requestData.token = cbNode->token;
                    requestData.tokenLength = cbNode->tokenLength;
                    requestInfo.method = CA_GET;
                    requestInfo.info = requestData;

                    caResult = CASendRequest(endpoint, &requestInfo);
                    CADestroyRemoteEndpoint(endpoint);

                    if (caResult != CA_STATUS_OK)
                    {
                        OC_LOG(ERROR, TAG, PCF("CASendRequest error"));
                        goto exit;
                    }

                    cbNode->presence->TTLlevel++;
                    OC_LOG_V(DEBUG, TAG, "moving to TTL level %d",
                                                            cbNode->presence->TTLlevel);
                }
            }
        }
    }
exit:
    if (result != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, PCF("OCProcessPresence error no %d"), result);
    }
    return result;
}
#endif // WITH_PRESENCE

OCStackResult OCProcess()
{
    #ifdef WITH_PRESENCE
    OCProcessPresence();
    #endif
    CAHandleRequestResponse();

    return OC_STACK_OK;
}

#ifdef WITH_PRESENCE
OCStackResult OCStartPresence(const uint32_t ttl)
{
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    OCChangeResourceProperty(
            &(((OCResource *)presenceResource.handle)->resourceProperties),
            OC_ACTIVE, 1);

    if (OC_MAX_PRESENCE_TTL_SECONDS < ttl)
    {
        presenceResource.presenceTTL = OC_MAX_PRESENCE_TTL_SECONDS;
        OC_LOG(INFO, TAG, PCF("Setting Presence TTL to max value"));
    }
    else if (0 == ttl)
    {
        presenceResource.presenceTTL = OC_DEFAULT_PRESENCE_TTL_SECONDS;
        OC_LOG(INFO, TAG, PCF("Setting Presence TTL to default value"));
    }
    else
    {
        presenceResource.presenceTTL = ttl;
    }
    OC_LOG_V(DEBUG, TAG, "Presence TTL is %lu seconds", presenceResource.presenceTTL);

    if (OC_PRESENCE_UNINITIALIZED == presenceState)
    {
        presenceState = OC_PRESENCE_INITIALIZED;

        CAAddress_t addressInfo;
        strncpy(addressInfo.IP.ipAddress, OC_MULTICAST_IP, CA_IPADDR_SIZE);
        addressInfo.IP.port = OC_MULTICAST_PORT;

        CAToken_t caToken = NULL;
        CAResult_t caResult = CAGenerateToken(&caToken, tokenLength);
        if (caResult != CA_STATUS_OK)
        {
            OC_LOG(ERROR, TAG, PCF("CAGenerateToken error"));
            CADestroyToken(caToken);
            return OC_STACK_ERROR;
        }

        CATransportType_t connType;
        OCToCATransportType(OC_ALL, &connType );
        AddObserver(OC_PRESENCE_URI, NULL, 0, caToken, tokenLength,
                (OCResource *)presenceResource.handle, OC_LOW_QOS,
                &addressInfo, connType);
        CADestroyToken(caToken);
    }

    // Each time OCStartPresence is called
    // a different random 32-bit integer number is used
    ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();

    return SendPresenceNotification(NULL);
}

OCStackResult OCStopPresence()
{
    OCStackResult result = OC_STACK_ERROR;

    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
    }

    // make resource inactive
    result = OCChangeResourceProperty(
            &(((OCResource *) presenceResource.handle)->resourceProperties),
            OC_ACTIVE, 0);

    if(result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG,
                      PCF("Changing the presence resource properties to ACTIVE not successful"));
        return result;
    }

    return SendStopNotification();
}
#endif

OCStackResult OCSetDefaultDeviceEntityHandler(OCDeviceEntityHandler entityHandler)
{
    defaultDeviceHandler = entityHandler;

    return OC_STACK_OK;
}

OCStackResult OCSetPlatformInfo(OCPlatformInfo platformInfo)
{
    OC_LOG(INFO, TAG, PCF("Entering OCSetPlatformInfo"));

    if(myStackMode ==  OC_SERVER || myStackMode == OC_CLIENT_SERVER)
    {
        if (validatePlatformInfo(platformInfo))
        {
            return SavePlatformInfo(platformInfo);
        }
        else
        {
            return OC_STACK_INVALID_PARAM;
        }
    }
    else
    {
        return OC_STACK_ERROR;
    }
}

OCStackResult OCSetDeviceInfo(OCDeviceInfo deviceInfo)
{
    // TODO: Implement this.
    OC_LOG(ERROR, TAG, "Implement OCSetDeviceInfo !!");

    // Returning ok to make samples work.
    return OC_STACK_OK;
}

OCStackResult OCCreateResource(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceInterfaceName,
        const char *uri, OCEntityHandler entityHandler,
        uint8_t resourceProperties)
{

    OCResource *pointer = NULL;
    char *str = NULL;
    size_t size = 0;
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering OCCreateResource"));

    if(myStackMode == OC_CLIENT)
    {
        return OC_STACK_INVALID_PARAM;
    }
    // Validate parameters
    if(!uri || uri[0]=='\0' || strlen(uri)>=MAX_URI_LENGTH )
    {
        OC_LOG(ERROR, TAG, PCF("URI is invalid"));
        return OC_STACK_INVALID_URI;
    }
    // Is it presented during resource discovery?
    if (!handle || !resourceTypeName)
    {
        OC_LOG(ERROR, TAG, PCF("Input parameter is NULL"));
        return OC_STACK_INVALID_PARAM;
    }

    if(!resourceInterfaceName || strlen(resourceInterfaceName) == 0)
    {
        resourceInterfaceName = OC_RSRVD_INTERFACE_DEFAULT;
    }

    // Make sure resourceProperties bitmask has allowed properties specified
    if (resourceProperties
            > (OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | OC_SLOW | OC_SECURE))
    {
        OC_LOG(ERROR, TAG, PCF("Invalid property"));
        return OC_STACK_INVALID_PARAM;
    }

    // If the headResource is NULL, then no resources have been created...
    pointer = headResource;
    if (pointer)
    {
        // At least one resources is in the resource list, so we need to search for
        // repeated URLs, which are not allowed.  If a repeat is found, exit with an error
        while (pointer)
        {
            if (strncmp(uri, pointer->uri, MAX_URI_LENGTH) == 0)
            {
                OC_LOG(ERROR, TAG, PCF("URI already in use"));
                return OC_STACK_INVALID_PARAM;
            }
            pointer = pointer->next;
        }
    }
    // Create the pointer and insert it into the resource list
    pointer = (OCResource *) OCCalloc(1, sizeof(OCResource));
    if (!pointer)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }
    pointer->sequenceNum = OC_OFFSET_SEQUENCE_NUMBER;

    insertResource(pointer);

    // Set the uri
    size = strlen(uri) + 1;
    str = (char *) OCMalloc(size);
    if (!str)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }
    strncpy(str, uri, size);
    pointer->uri = str;

    // Set properties.  Set OC_ACTIVE
    pointer->resourceProperties = (OCResourceProperty) (resourceProperties
            | OC_ACTIVE);

    // Add the resourcetype to the resource
    result = BindResourceTypeToResource(pointer, resourceTypeName);
    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("Error adding resourcetype"));
        goto exit;
    }

    // Add the resourceinterface to the resource
    result = BindResourceInterfaceToResource(pointer, resourceInterfaceName);
    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("Error adding resourceinterface"));
        goto exit;
    }

    // If an entity handler has been passed, attach it to the newly created
    // resource.  Otherwise, set the default entity handler.
    if (entityHandler)
    {
        pointer->entityHandler = entityHandler;
    }
    else
    {
        pointer->entityHandler = defaultResourceEHandler;
    }

    *handle = pointer;
    result = OC_STACK_OK;

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(pointer->rsrcType);
    }
    #endif
exit:
    if (result != OC_STACK_OK)
    {
        // Deep delete of resource and other dynamic elements that it contains
        deleteResource(pointer);
        OCFree(str);
    }
    return result;
}

OCStackResult OCCreateResourceWithHost(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceInterfaceName,
        const char *host,
        const char *uri,
        OCEntityHandler entityHandler,
        uint8_t resourceProperties)
{
    OC_LOG(INFO, TAG, PCF("Entering OCCreateResourceWithHost"));
    char *str = NULL;
    size_t size = 0;

    if(!host)
    {
        OC_LOG(ERROR, TAG, PCF("Added resource host is NULL."));
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult result = OC_STACK_ERROR;

    result = OCCreateResource(handle, resourceTypeName, resourceInterfaceName,
                                uri, entityHandler, resourceProperties);

    if (result == OC_STACK_OK)
    {
        // Set the uri
        size = strlen(host) + 1;
        str = (char *) OCMalloc(size);
        if (!str)
        {
            OC_LOG(ERROR, TAG, PCF("Memory could not be allocated."));
            return OC_STACK_NO_MEMORY;
        }
        strncpy(str, host, size);

        ((OCResource *) *handle)->host = str;
    }

    return result;
}

OCStackResult OCBindResource(
        OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
{
    OCResource *resource = NULL;
    uint8_t i = 0;

    OC_LOG(INFO, TAG, PCF("Entering OCBindResource"));

    // Validate parameters
    VERIFY_NON_NULL(collectionHandle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(resourceHandle, ERROR, OC_STACK_ERROR);
    // Container cannot contain itself
    if (collectionHandle == resourceHandle)
    {
        OC_LOG(ERROR, TAG, PCF("Added handle equals collection handle"));
        return OC_STACK_INVALID_PARAM;
    }

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *) collectionHandle);
    if (!resource)
    {
        OC_LOG(ERROR, TAG, PCF("Collection handle not found"));
        return OC_STACK_INVALID_PARAM;
    }

    // Look for an open slot to add add the child resource.
    // If found, add it and return success
    for (i = 0; i < MAX_CONTAINED_RESOURCES; i++)
    {
        if (!resource->rsrcResources[i])
        {
            resource->rsrcResources[i] = (OCResource *) resourceHandle;
            OC_LOG(INFO, TAG, PCF("resource bound"));

            #ifdef WITH_PRESENCE
            if(presenceResource.handle)
            {
                ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
                SendPresenceNotification(((OCResource *) resourceHandle)->rsrcType);
            }
            #endif
            return OC_STACK_OK;

        }
    }

    // Unable to add resourceHandle, so return error
    return OC_STACK_ERROR;
}

OCStackResult OCUnBindResource(
        OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
{
    OCResource *resource = NULL;
    uint8_t i = 0;

    OC_LOG(INFO, TAG, PCF("Entering OCUnBindResource"));

    // Validate parameters
    VERIFY_NON_NULL(collectionHandle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(resourceHandle, ERROR, OC_STACK_ERROR);
    // Container cannot contain itself
    if (collectionHandle == resourceHandle)
    {
        OC_LOG(ERROR, TAG, PCF("removing handle equals collection handle"));
        return OC_STACK_INVALID_PARAM;
    }

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *) collectionHandle);
    if (!resource)
    {
        OC_LOG(ERROR, TAG, PCF("Collection handle not found"));
        return OC_STACK_INVALID_PARAM;
    }

    // Look for an open slot to add add the child resource.
    // If found, add it and return success
    for (i = 0; i < MAX_CONTAINED_RESOURCES; i++)
    {
        if (resourceHandle == resource->rsrcResources[i])
        {
            resource->rsrcResources[i] = (OCResource *) NULL;
            OC_LOG(INFO, TAG, PCF("resource unbound"));

            // Send notification when resource is unbounded successfully.
            #ifdef WITH_PRESENCE
            if(presenceResource.handle)
            {
                ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
                SendPresenceNotification(((OCResource *) resourceHandle)->rsrcType);
            }
            #endif
            return OC_STACK_OK;
        }
    }

    OC_LOG(INFO, TAG, PCF("resource not found in collection"));

    // Unable to add resourceHandle, so return error
    return OC_STACK_ERROR;
}

OCStackResult BindResourceTypeToResource(OCResource* resource,
                                            const char *resourceTypeName)
{
    OCResourceType *pointer = NULL;
    char *str = NULL;
    size_t size = 0;
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering BindResourceTypeToResource"));

    // Validate parameters
    VERIFY_NON_NULL(resourceTypeName, ERROR, OC_STACK_INVALID_PARAM);
    // TODO:  Does resource attribute representation really have to be maintained in stack?
    // Is it presented during resource discovery?

    // Create the resourcetype and insert it into the resource list
    pointer = (OCResourceType *) OCCalloc(1, sizeof(OCResourceType));
    if (!pointer)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    // Set the resourceTypeName
    size = strlen(resourceTypeName) + 1;
    str = (char *) OCMalloc(size);
    if (!str)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }
    strncpy(str, resourceTypeName, size);
    pointer->resourcetypename = str;

    insertResourceType(resource, pointer);
    result = OC_STACK_OK;

    exit:
    if (result != OC_STACK_OK)
    {
        OCFree(pointer);
        OCFree(str);
    }

    return result;
}

OCStackResult BindResourceInterfaceToResource(OCResource* resource,
        const char *resourceInterfaceName)
{
    OCResourceInterface *pointer = NULL;
    char *str = NULL;
    size_t size = 0;
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering BindResourceInterfaceToResource"));

    // Validate parameters
    VERIFY_NON_NULL(resourceInterfaceName, ERROR, OC_STACK_INVALID_PARAM);

    //TODO ("Make sure that the resourceinterface name doesn't already exist in the resource");

    // Create the resourceinterface and insert it into the resource list
    pointer = (OCResourceInterface *) OCCalloc(1, sizeof(OCResourceInterface));
    if (!pointer)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    // Set the resourceinterface name
    size = strlen(resourceInterfaceName) + 1;
    str = (char *) OCMalloc(size);
    if (!str)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }
    strncpy(str, resourceInterfaceName, size);
    pointer->name = str;

    // Bind the resourceinterface to the resource
    insertResourceInterface(resource, pointer);

    result = OC_STACK_OK;

    exit:
    if (result != OC_STACK_OK)
    {
        OCFree(pointer);
        OCFree(str);
    }

    return result;
}

OCStackResult OCBindResourceTypeToResource(OCResourceHandle handle,
        const char *resourceTypeName)
{

    OCStackResult result = OC_STACK_ERROR;
    OCResource *resource = NULL;

    // Make sure resource exists
    resource = findResource((OCResource *) handle);
    if (!resource)
    {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_ERROR;
    }

    // call internal function
    result = BindResourceTypeToResource(resource, resourceTypeName);

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(resource->rsrcType);
    }
    #endif

    return result;
}

OCStackResult OCBindResourceInterfaceToResource(OCResourceHandle handle,
        const char *resourceInterfaceName)
{

    OCStackResult result = OC_STACK_ERROR;
    OCResource *resource = NULL;

    // Make sure resource exists
    resource = findResource((OCResource *) handle);
    if (!resource)
    {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_ERROR;
    }

    // call internal function
    result = BindResourceInterfaceToResource(resource, resourceInterfaceName);

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(resource->rsrcType);
    }
    #endif

    return result;
}

OCStackResult OCGetNumberOfResources(uint8_t *numResources)
{
    OCResource *pointer = headResource;

    OC_LOG(INFO, TAG, PCF("Entering OCGetNumberOfResources"));
    VERIFY_NON_NULL(numResources, ERROR, OC_STACK_INVALID_PARAM);
    *numResources = 0;
    while (pointer)
    {
        *numResources = *numResources + 1;
        pointer = pointer->next;
    }
    return OC_STACK_OK;
}

OCResourceHandle OCGetResourceHandle(uint8_t index)
{
    OCResource *pointer = headResource;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceHandle"));

    // Iterate through the list
    for( uint8_t i = 0; i < index && pointer; ++i)
    {
        pointer = pointer->next;
    }
    return (OCResourceHandle) pointer;
}

OCStackResult OCDeleteResource(OCResourceHandle handle)
{
    OC_LOG(INFO, TAG, PCF("Entering OCDeleteResource"));

    if (!handle)
    {
        OC_LOG(ERROR, TAG, PCF("Invalid param"));
        return OC_STACK_INVALID_PARAM;
    }

    OCResource *resource = findResource((OCResource *) handle);
    if (resource == NULL)
    {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_NO_RESOURCE;
    }

    if (deleteResource((OCResource *) handle) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("Error deleting resource"));
        return OC_STACK_ERROR;
    }

    return OC_STACK_OK;
}

const char *OCGetResourceUri(OCResourceHandle handle)
{
    OCResource *resource = NULL;
    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceUri"));

    resource = findResource((OCResource *) handle);
    if (resource)
    {
        return resource->uri;
    }
    return (const char *) NULL;
}

OCResourceProperty OCGetResourceProperties(OCResourceHandle handle)
{
    OCResource *resource = NULL;
    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceProperties"));

    resource = findResource((OCResource *) handle);
    if (resource)
    {
        return resource->resourceProperties;
    }
    return (OCResourceProperty)-1;
}

OCStackResult OCGetNumberOfResourceTypes(OCResourceHandle handle,
        uint8_t *numResourceTypes)
{
    OCResource *resource = NULL;
    OCResourceType *pointer = NULL;

    OC_LOG(INFO, TAG, PCF("Entering OCGetNumberOfResourceTypes"));
    VERIFY_NON_NULL(numResourceTypes, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);

    *numResourceTypes = 0;

    resource = findResource((OCResource *) handle);
    if (resource)
    {
        pointer = resource->rsrcType;
        while (pointer)
        {
            *numResourceTypes = *numResourceTypes + 1;
            pointer = pointer->next;
        }
    }
    return OC_STACK_OK;
}

const char *OCGetResourceTypeName(OCResourceHandle handle, uint8_t index)
{
    OCResourceType *resourceType = NULL;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceTypeName"));

    resourceType = findResourceTypeAtIndex(handle, index);
    if (resourceType)
    {
        return resourceType->resourcetypename;
    }
    return (const char *) NULL;
}

OCStackResult OCGetNumberOfResourceInterfaces(OCResourceHandle handle,
        uint8_t *numResourceInterfaces)
{
    OCResourceInterface *pointer = NULL;
    OCResource *resource = NULL;

    OC_LOG(INFO, TAG, PCF("Entering OCGetNumberOfResourceInterfaces"));

    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(numResourceInterfaces, ERROR, OC_STACK_INVALID_PARAM);

    *numResourceInterfaces = 0;
    resource = findResource((OCResource *) handle);
    if (resource)
    {
        pointer = resource->rsrcInterface;
        while (pointer)
        {
            *numResourceInterfaces = *numResourceInterfaces + 1;
            pointer = pointer->next;
        }
    }
    return OC_STACK_OK;
}

const char *OCGetResourceInterfaceName(OCResourceHandle handle, uint8_t index)
{
    OCResourceInterface *resourceInterface = NULL;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceInterfaceName"));

    resourceInterface = findResourceInterfaceAtIndex(handle, index);
    if (resourceInterface)
    {
        return resourceInterface->name;
    }
    return (const char *) NULL;
}

OCResourceHandle OCGetResourceHandleFromCollection(OCResourceHandle collectionHandle,
        uint8_t index)
{
    OCResource *resource = NULL;

    OC_LOG(INFO, TAG, PCF("Entering OCGetContainedResource"));

    if (index >= MAX_CONTAINED_RESOURCES)
    {
        return NULL;
    }

    resource = findResource((OCResource *) collectionHandle);
    if (!resource)
    {
        return NULL;
    }

    return resource->rsrcResources[index];
}

OCStackResult OCBindResourceHandler(OCResourceHandle handle,
        OCEntityHandler entityHandler)
{
    OCResource *resource = NULL;

    OC_LOG(INFO, TAG, PCF("Entering OCBindResourceHandler"));

    // Validate parameters
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *)handle);
    if (!resource)
    {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_ERROR;
    }

    // Bind the handler
    resource->entityHandler = entityHandler;

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        SendPresenceNotification(resource->rsrcType);
    }
    #endif

    return OC_STACK_OK;
}

OCEntityHandler OCGetResourceHandler(OCResourceHandle handle)
{
    OCResource *resource = NULL;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceHandler"));

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *)handle);
    if (!resource)
    {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return NULL;
    }

    // Bind the handler
    return resource->entityHandler;
}

void incrementSequenceNumber(OCResource * resPtr)
{
    // Increment the sequence number
    resPtr->sequenceNum += 1;
    if (resPtr->sequenceNum == MAX_SEQUENCE_NUMBER)
    {
        resPtr->sequenceNum = OC_OFFSET_SEQUENCE_NUMBER+1;
    }
    return;
}

#ifdef WITH_PRESENCE
OCStackResult SendPresenceNotification(OCResourceType *resourceType)
{
    OCResource *resPtr = NULL;
    OCStackResult result = OC_STACK_ERROR;
    OCMethod method = OC_REST_PRESENCE;
    uint32_t maxAge = 0;
    resPtr = findResource((OCResource *) presenceResource.handle);
    if(NULL == resPtr)
    {
        return OC_STACK_NO_RESOURCE;
    }

    if((((OCResource *) presenceResource.handle)->resourceProperties) & OC_ACTIVE)
    {
        maxAge = presenceResource.presenceTTL;

        result = SendAllObserverNotification(method, resPtr, maxAge, resourceType, OC_LOW_QOS);
    }

    return result;
}

OCStackResult SendStopNotification()
{
    OCResource *resPtr = NULL;
    OCStackResult result = OC_STACK_ERROR;
    OCMethod method = OC_REST_PRESENCE;
    resPtr = findResource((OCResource *) presenceResource.handle);
    if(NULL == resPtr)
    {
        return OC_STACK_NO_RESOURCE;
    }

    // maxAge is 0. ResourceType is NULL.
    result = SendAllObserverNotification(method, resPtr, 0, NULL, OC_LOW_QOS);

    return result;
}

#endif // WITH_PRESENCE
OCStackResult OCNotifyAllObservers(OCResourceHandle handle, OCQualityOfService qos)
{

    OC_LOG(INFO, TAG, PCF("Entering OCNotifyAllObservers"));

    OCResource *resPtr = NULL;
    OCStackResult result = OC_STACK_ERROR;
    OCMethod method = OC_REST_NOMETHOD;
    uint32_t maxAge = 0;

    OC_LOG(INFO, TAG, PCF("Entering OCNotifyAllObservers"));
    #ifdef WITH_PRESENCE
    if(handle == presenceResource.handle)
    {
        return OC_STACK_OK;
    }
    #endif // WITH_PRESENCE
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_ERROR);

    // Verify that the resource exists
    resPtr = findResource ((OCResource *) handle);
    if (NULL == resPtr)
    {
        return OC_STACK_NO_RESOURCE;
    }
    else
    {
        //only increment in the case of regular observing (not presence)
        incrementSequenceNumber(resPtr);
        method = OC_REST_OBSERVE;
        maxAge = MAX_OBSERVE_AGE;
        #ifdef WITH_PRESENCE
        result = SendAllObserverNotification (method, resPtr, maxAge, NULL, qos);
        #else
        result = SendAllObserverNotification (method, resPtr, maxAge, qos);
        #endif
        return result;
    }
}

OCStackResult
OCNotifyListOfObservers (OCResourceHandle handle,
                         OCObservationId  *obsIdList,
                         uint8_t          numberOfIds,
                         const char       *notificationJSONPayload,
                         OCQualityOfService qos)
{
    OC_LOG(INFO, TAG, PCF("Entering OCNotifyListOfObservers"));

    OCResource *resPtr = NULL;
    //TODO: we should allow the server to define this
    uint32_t maxAge = MAX_OBSERVE_AGE;

    VERIFY_NON_NULL(handle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(obsIdList, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(notificationJSONPayload, ERROR, OC_STACK_ERROR);

    // Verify that the resource exists
    resPtr = findResource ((OCResource *) handle);
    if (NULL == resPtr || myStackMode == OC_CLIENT)
    {
        return OC_STACK_NO_RESOURCE;
    }
    else
    {
        incrementSequenceNumber(resPtr);
    }
    return (SendListObserverNotification(resPtr, obsIdList, numberOfIds,
            notificationJSONPayload, maxAge, qos));
}

OCStackResult OCDoResponse(OCEntityHandlerResponse *ehResponse)
{
    OCStackResult result = OC_STACK_ERROR;
    OCServerRequest *serverRequest = NULL;

    OC_LOG(INFO, TAG, PCF("Entering OCDoResponse"));

    // Validate input parameters
    VERIFY_NON_NULL(ehResponse, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(ehResponse->requestHandle, ERROR, OC_STACK_INVALID_PARAM);

    // TODO: Placeholder for creating a response entry when implementing
    // block transfer feature

    // If a response payload is present, check if block transfer is required
    if (ehResponse->payload && OCIsPacketTransferRequired(NULL,
            (const char *)ehResponse->payload, ehResponse->payloadSize))
    {
        OC_LOG(INFO, TAG, PCF("Block transfer required"));

        // Persistent response buffer is needed for block transfer
        if (!ehResponse->persistentBufferFlag)
        {
            OC_LOG(WARNING, TAG, PCF("Persistent response buffer required"));
            return OC_STACK_PERSISTENT_BUFFER_REQUIRED;
        }
        // TODO: Placeholder for block transfer handling
        // TODO: Placeholder for setting the the response handle in the OCServerResponse struct
            // when implementing the block transfer feature
    }
    else
    {
        // Normal response
        // Get pointer to request info
        serverRequest = GetServerRequestUsingHandle((OCServerRequest *)ehResponse->requestHandle);
        if(serverRequest)
        {
            result = serverRequest->ehResponseHandler(ehResponse);
        }
    }
    return result;
}

//-----------------------------------------------------------------------------
// Private internal function definitions
//-----------------------------------------------------------------------------
static OCDoHandle GenerateInvocationHandle()
{
    OCDoHandle handle = NULL;
    // Generate token here, it will be deleted when the transaction is deleted
    handle = (OCDoHandle) OCMalloc(sizeof(uint8_t[CA_MAX_TOKEN_LEN]));
    if (handle)
    {
        OCFillRandomMem((uint8_t*)handle, sizeof(uint8_t[CA_MAX_TOKEN_LEN]));
    }

    return handle;
}

#ifdef WITH_PRESENCE
OCStackResult OCChangeResourceProperty(OCResourceProperty * inputProperty,
        OCResourceProperty resourceProperties, uint8_t enable)
{
    if (!inputProperty)
    {
        return OC_STACK_INVALID_PARAM;
    }
    if (resourceProperties
            > (OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | OC_SLOW))
    {
        OC_LOG(ERROR, TAG, PCF("Invalid property"));
        return OC_STACK_INVALID_PARAM;
    }
    if(!enable)
    {
        *inputProperty = (OCResourceProperty) (*inputProperty & ~(resourceProperties));
    }
    else
    {
        *inputProperty = (OCResourceProperty) (*inputProperty | resourceProperties);
    }
    return OC_STACK_OK;
}
#endif

OCStackResult initResources()
{
    OCStackResult result = OC_STACK_OK;
    // Init application resource vars
    headResource = NULL;
    tailResource = NULL;
    // Init Virtual Resources
    #ifdef WITH_PRESENCE
    presenceResource.presenceTTL = OC_DEFAULT_PRESENCE_TTL_SECONDS;
    //presenceResource.token = OCGenerateCoAPToken();
    result = OCCreateResource(&presenceResource.handle,
            OC_RSRVD_RESOURCE_TYPE_PRESENCE,
            "core.r",
            OC_PRESENCE_URI,
            NULL,
            OC_OBSERVABLE);
    //make resource inactive
    result = OCChangeResourceProperty(
            &(((OCResource *) presenceResource.handle)->resourceProperties),
            OC_ACTIVE, 0);
    #endif
    return result;
}

void insertResource(OCResource *resource)
{
    if (!headResource)
    {
        headResource = resource;
        tailResource = resource;
    }
    else
    {
        tailResource->next = resource;
        tailResource = resource;
    }
    resource->next = NULL;
}

OCResource *findResource(OCResource *resource)
{
    OCResource *pointer = headResource;

    while (pointer)
    {
        if (pointer == resource)
        {
            return resource;
        }
        pointer = pointer->next;
    }
    return NULL;
}

void deleteAllResources()
{
    OCResource *pointer = headResource;
    OCResource *temp = NULL;

    while (pointer)
    {
        temp = pointer->next;
        #ifdef WITH_PRESENCE
        if(pointer != (OCResource *) presenceResource.handle)
        {
        #endif // WITH_PRESENCE
            deleteResource(pointer);
        #ifdef WITH_PRESENCE
        }
        #endif // WITH_PRESENCE
        pointer = temp;
    }

    #ifdef WITH_PRESENCE
    // Ensure that the last resource to be deleted is the presence resource. This allows for all
    // presence notification attributed to their deletion to be processed.
    deleteResource((OCResource *) presenceResource.handle);
    #endif // WITH_PRESENCE
}

OCStackResult deleteResource(OCResource *resource)
{
    OCResource *prev = NULL;
    OCResource *temp = NULL;

    temp = headResource;
    while (temp)
    {
        if (temp == resource)
        {
            // Invalidate all Resource Properties.
            resource->resourceProperties = (OCResourceProperty) 0;
            #ifdef WITH_PRESENCE
            if(resource != (OCResource *) presenceResource.handle)
            {
            #endif // WITH_PRESENCE
                OCNotifyAllObservers((OCResourceHandle)resource, OC_HIGH_QOS);
            #ifdef WITH_PRESENCE
            }

            if(presenceResource.handle)
            {
                ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
                if(resource != (OCResource *) presenceResource.handle)
                {
                    SendPresenceNotification(resource->rsrcType);
                }
                else
                {
                    SendPresenceNotification(NULL);
                }
            }
            #endif
            // Only resource in list.
            if (temp == headResource && temp == tailResource)
            {
                headResource = NULL;
                tailResource = NULL;
            }
            // Deleting head.
            else if (temp == headResource)
            {
                headResource = temp->next;
            }
            // Deleting tail.
            else if (temp == tailResource)
            {
                tailResource = prev;
                tailResource->next = NULL;
            }
            else
            {
                prev->next = temp->next;
            }

            deleteResourceElements(temp);
            OCFree(temp);
            return OC_STACK_OK;
        }
        else
        {
            prev = temp;
            temp = temp->next;
        }
    }

    return OC_STACK_ERROR;
}

void deleteResourceElements(OCResource *resource)
{
    if (!resource)
    {
        return;
    }

    // remove URI
    OCFree(resource->uri);

    // Delete resourcetype linked list
    deleteResourceType(resource->rsrcType);

    // Delete resourceinterface linked list
    deleteResourceInterface(resource->rsrcInterface);
}

void deleteResourceType(OCResourceType *resourceType)
{
    OCResourceType *pointer = resourceType;
    OCResourceType *next = NULL;

    while (pointer)
    {
        next = pointer->next;
        OCFree(pointer->resourcetypename);
        OCFree(pointer);
        pointer = next;
    }
}

void deleteResourceInterface(OCResourceInterface *resourceInterface)
{
    OCResourceInterface *pointer = resourceInterface;
    OCResourceInterface *next = NULL;

    while (pointer)
    {
        next = pointer->next;
        OCFree(pointer->name);
        OCFree(pointer);
        pointer = next;
    }
}

void insertResourceType(OCResource *resource, OCResourceType *resourceType)
{
    OCResourceType *pointer = NULL;
    OCResourceType *previous = NULL;
    if (!resource || !resourceType)
    {
        return;
    }
    // resource type list is empty.
    else if (!resource->rsrcType)
    {
        resource->rsrcType = resourceType;
    }
    else
    {
        pointer = resource->rsrcType;

        while (pointer)
        {
            // resource type already exists. Free 2nd arg and return.
            if (!strcmp(resourceType->resourcetypename, pointer->resourcetypename))
            {
                OCFree(resourceType->resourcetypename);
                OCFree(resourceType);
                return;
            }
            previous = pointer;
            pointer = pointer->next;
        }
        previous->next = resourceType;
    }
    resourceType->next = NULL;
}

OCResourceType *findResourceTypeAtIndex(OCResourceHandle handle, uint8_t index)
{
    OCResource *resource = NULL;
    OCResourceType *pointer = NULL;

    // Find the specified resource
    resource = findResource((OCResource *) handle);
    if (!resource)
    {
        return NULL;
    }

    // Make sure a resource has a resourcetype
    if (!resource->rsrcType)
    {
        return NULL;
    }

    // Iterate through the list
    pointer = resource->rsrcType;
    for(uint8_t i = 0; i< index && pointer; ++i)
    {
        pointer = pointer->next;
    }
    return pointer;
}

OCResourceType *findResourceType(OCResourceType * resourceTypeList, const char * resourceTypeName)
{
    if(resourceTypeList && resourceTypeName)
    {
        OCResourceType * rtPointer = resourceTypeList;
        while(resourceTypeName && rtPointer)
        {
            if(rtPointer->resourcetypename &&
                    strcmp(resourceTypeName, (const char *)
                    (rtPointer->resourcetypename)) == 0)
            {
                break;
            }
            rtPointer = rtPointer->next;
        }
        return rtPointer;
    }
    return NULL;
}

/*
 * Insert a new interface into interface linked list only if not already present.
 * If alredy present, 2nd arg is free'd.
 * Default interface will always be first if present.
 */
void insertResourceInterface(OCResource *resource, OCResourceInterface *newInterface)
{
    OCResourceInterface *pointer = NULL;
    OCResourceInterface *previous = NULL;

    newInterface->next = NULL;

    OCResourceInterface **firstInterface = &(resource->rsrcInterface);

    if (!*firstInterface)
    {
        *firstInterface = newInterface;
    }
    else if (strcmp(newInterface->name, OC_RSRVD_INTERFACE_DEFAULT) == 0)
    {
        if (strcmp((*firstInterface)->name, OC_RSRVD_INTERFACE_DEFAULT) == 0)
        {
            OCFree(newInterface->name);
            OCFree(newInterface);
            return;
        }
        else
        {
            newInterface->next = *firstInterface;
            *firstInterface = newInterface;
        }
    }
    else
    {
        pointer = *firstInterface;
        while (pointer)
        {
            if (strcmp(newInterface->name, pointer->name) == 0)
            {
                OCFree(newInterface->name);
                OCFree(newInterface);
                return;
            }
            previous = pointer;
            pointer = pointer->next;
        }
        previous->next = newInterface;
    }
}

OCResourceInterface *findResourceInterfaceAtIndex(OCResourceHandle handle,
        uint8_t index)
{
    OCResource *resource = NULL;
    OCResourceInterface *pointer = NULL;

    // Find the specified resource
    resource = findResource((OCResource *) handle);
    if (!resource)
    {
        return NULL;
    }

    // Make sure a resource has a resourceinterface
    if (!resource->rsrcInterface)
    {
        return NULL;
    }

    // Iterate through the list
    pointer = resource->rsrcInterface;

    for (uint8_t i = 0; i < index && pointer; ++i)
    {
        pointer = pointer->next;
    }
    return pointer;
}

bool OCIsPacketTransferRequired(const char *request, const char *response, size_t size)
{
    bool result = false;

    // Determine if we are checking a request or a response
    if (request)
    {
        // If size is greater than 0, use it for the request size value, otherwise
        // assume request is null terminated and use strlen for size value
        if ((size > MAX_REQUEST_LENGTH) || (strlen(request) > MAX_REQUEST_LENGTH))
        {
            result = true;
        }
    }
    else if (response)
    {
        // If size is greater than 0, use it for the response size value, otherwise
        // assume response is null terminated and use strlen for size value
        if ((size > MAX_RESPONSE_LENGTH) || (strlen(response) > MAX_RESPONSE_LENGTH))
        {
            result = true;
        }
    }
    return result;
}

OCStackResult getResourceType(const char * query, char** resourceType)
{
    if(!query)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult result = OC_STACK_ERROR;

    if(strncmp(query, "rt=", 3) == 0)
    {
        *resourceType = (char *) OCMalloc(strlen(query)-3 + 1);
        if(!*resourceType)
        {
            result = OC_STACK_NO_MEMORY;
        }
        else
        {
            strcpy((char *)*resourceType, ((const char *)&query[3]));
            result = OC_STACK_OK;
        }
    }

    return result;
}

/*
 * This function splits the uri using the '?' delimiter.
 * "uriWithoutQuery" is the block of characters between the beginning
 * till the delimiter or '\0' which ever comes first.
 * "query" is whatever is to the right of the delimiter if present.
 * No delimiter sets the query to NULL.
 * If either are present, they will be malloc'ed into the params 2, 3.
 * The first param, *uri is left untouched.

 * NOTE: This function does not account for whitespace at the end of the uri NOR
 *       malformed uri's with '??'. Whitespace at the end will be assumed to be
 *       part of the query.
 */
OCStackResult getQueryFromUri(const char * uri, char** query, char ** uriWithoutQuery)
{
    if(!uri)
    {
        return OC_STACK_INVALID_URI;
    }
    if(!query || !uriWithoutQuery)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *query           = NULL;
    *uriWithoutQuery = NULL;

    size_t uriWithoutQueryLen = 0;
    size_t queryLen = 0;
    size_t uriLen = strlen(uri);

    char *pointerToDelimiter = strstr(uri, "?");

    uriWithoutQueryLen = pointerToDelimiter == NULL ? uriLen : pointerToDelimiter - uri;
    queryLen = pointerToDelimiter == NULL ? 0 : uriLen - uriWithoutQueryLen - 1;

    if (uriWithoutQueryLen)
    {
        *uriWithoutQuery =  (char *) OCCalloc(uriWithoutQueryLen + 1, 1);
        if (!*uriWithoutQuery)
        {
            goto exit;
        }
        strncpy(*uriWithoutQuery, uri, uriWithoutQueryLen);
    }
    else
    {
        return OC_STACK_INVALID_PARAM;
    }

    if (queryLen)
    {
        *query = (char *) OCCalloc(queryLen + 1, 1);
        if (!*query)
        {
            OCFree(*uriWithoutQuery);
            *uriWithoutQuery = NULL;
            goto exit;
        }
        strncpy(*query, pointerToDelimiter + 1, queryLen);
    }

    return OC_STACK_OK;

    exit:
        return OC_STACK_NO_MEMORY;
}

const uint8_t* OCGetServerInstanceID(void)
{
    static bool generated = false;
    static ServerID sid;
    if(generated)
    {
        return sid;
    }

    if (OCGenerateUuid(sid) != RAND_UUID_OK)
    {
        OC_LOG(FATAL, TAG, PCF("Generate UUID for Server Instance failed!"));
        return NULL;
    }
    generated = true;
    return sid;
}

const char* OCGetServerInstanceIDString(void)
{
    static bool generated = false;
    static char sidStr[UUID_STRING_SIZE];

    if(generated)
    {
        return sidStr;
    }

    const uint8_t* sid = OCGetServerInstanceID();

    if(OCConvertUuidToString(sid, sidStr) != RAND_UUID_OK)
    {
        OC_LOG(FATAL, TAG, PCF("Generate UUID String for Server Instance failed!"));
        return NULL;
    }

    generated = true;
    return sidStr;
}

int32_t OCDevAddrToIPv4Addr(OCDevAddr *ipAddr, uint8_t *a, uint8_t *b,
        uint8_t *c, uint8_t *d )
{
    if ( !ipAddr || !a || !b || !c || !d )
    {
        OC_LOG(FATAL, TAG, PCF("Invalid argument"));
        return OC_STACK_INVALID_PARAM;
    }

    *a = ipAddr->addr[0];
    *b = ipAddr->addr[1];
    *c = ipAddr->addr[2];
    *d = ipAddr->addr[3];

    return OC_STACK_OK;
}

int32_t OCDevAddrToPort(OCDevAddr *ipAddr, uint16_t *port)
{
    if ( !ipAddr || !port )
    {
        OC_LOG(FATAL, TAG, PCF("Invalid argument"));
        return OC_STACK_INVALID_PARAM;
    }

    *port = (ipAddr->addr[5]<< 8) | ipAddr->addr[4];

    return OC_STACK_OK;
}

CAResult_t OCSelectNetwork()
{
    CAResult_t retResult = CA_STATUS_FAILED;
    CAResult_t caResult = CA_STATUS_OK;

    CATransportType_t connTypes[] = {
            CA_IPV4,
            CA_EDR,
            CA_LE};
    int numConnTypes = sizeof(connTypes)/sizeof(connTypes[0]);

    for(int i = 0; i<numConnTypes; i++)
    {
        // Ignore CA_NOT_SUPPORTED error. The CA Layer may have not compiled in the interface.
        if(caResult == CA_STATUS_OK || caResult == CA_NOT_SUPPORTED)
        {
           caResult = CASelectNetwork(connTypes[i]);
           if(caResult == CA_STATUS_OK)
           {
               retResult = CA_STATUS_OK;
           }
        }
    }

    if(retResult != CA_STATUS_OK)
    {
        return caResult; // Returns error of appropriate transport that failed fatally.
    }

    return retResult;
}

OCStackResult CAResultToOCResult(CAResult_t caResult)
{
    switch (caResult)
    {
        case CA_STATUS_OK:
            return OC_STACK_OK;
        case CA_STATUS_INVALID_PARAM:
            return OC_STACK_INVALID_PARAM;
        case CA_ADAPTER_NOT_ENABLED:
            return OC_STACK_ADAPTER_NOT_ENABLED;
        case CA_SERVER_STARTED_ALREADY:
            return OC_STACK_OK;
        case CA_SERVER_NOT_STARTED:
            return OC_STACK_ERROR;
        case CA_DESTINATION_NOT_REACHABLE:
            return OC_STACK_COMM_ERROR;
        case CA_SOCKET_OPERATION_FAILED:
            return OC_STACK_COMM_ERROR;
        case CA_SEND_FAILED:
            return OC_STACK_COMM_ERROR;
        case CA_RECEIVE_FAILED:
            return OC_STACK_COMM_ERROR;
        case CA_MEMORY_ALLOC_FAILED:
            return OC_STACK_NO_MEMORY;
        case CA_REQUEST_TIMEOUT:
            return OC_STACK_TIMEOUT;
        case CA_DESTINATION_DISCONNECTED:
            return OC_STACK_COMM_ERROR;
        case CA_STATUS_FAILED:
            return OC_STACK_ERROR;
        case CA_NOT_SUPPORTED:
            return OC_STACK_NOTIMPL;
        default:
            return OC_STACK_ERROR;
    }
}
