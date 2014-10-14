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
#include "ocstack.h"
#include "ocstackinternal.h"
#include "ocresource.h"
#include "occlientcb.h"
#include "ocobserve.h"
#include "ocrandom.h"
#include "debug.h"
#include "occoap.h"
#include "ocmalloc.h"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef enum {
    OC_STACK_UNINITIALIZED = 0, OC_STACK_INITIALIZED
} OCStackState;

#ifdef WITH_PRESENCE
typedef enum {
    OC_PRESENCE_UNINITIALIZED = 0, OC_PRESENCE_INITIALIZED
} OCPresenceState;
#endif

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------
static OCStackState stackState = OC_STACK_UNINITIALIZED;

OCResource *headResource = NULL;
#ifdef WITH_PRESENCE
static OCPresenceState presenceState = OC_PRESENCE_UNINITIALIZED;
static PresenceResource presenceResource;
uint8_t PresenceTimeOutSize = 4;
uint32_t PresenceTimeOut[] = {50, 75, 85, 95, 100};
#endif

OCMode myStackMode;
OCDeviceEntityHandler defaultDeviceHandler;

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define TAG  PCF("OCStack")
#define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); return (retVal); } }

//-----------------------------------------------------------------------------
// Internal API function
//-----------------------------------------------------------------------------

//This function will be called back by occoap layer when a request is received
OCStackResult HandleStackRequests(OCRequest * request)
{
    OC_LOG(INFO, TAG, PCF("Entering HandleStackRequests (OCStack Layer)"));

    OCStackResult result = OC_STACK_ERROR;
    ResourceHandling resHandling;
    OCResource *resource;

    VERIFY_NON_NULL(request, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(request->entityHandlerRequest, ERROR, OC_STACK_ERROR);

    result = DetermineResourceHandling (request, &resHandling, &resource);

    if (result == OC_STACK_OK)
    {
        result = BuildJSONResponse(resHandling, resource, request);
    }

    return result;
}

//This function will be called back by occoap layer when a response is received
void HandleStackResponses(OCResponse * response)
{
    OCStackApplicationResult result = OC_STACK_DELETE_TRANSACTION;
    OC_LOG(INFO, TAG, PCF("Entering HandleStackResponses (OCStack Layer)"));

    if (response->cbNode)
    {
        OC_LOG(INFO, TAG, PCF("Calling into application address space"));
        result = response->cbNode->callBack(response->cbNode->context,
                response->cbNode->handle, response->clientResponse);
        if (result == OC_STACK_DELETE_TRANSACTION ||
                response->clientResponse->result == OC_STACK_COMM_ERROR ||
                response->clientResponse->result == OC_STACK_RESOURCE_DELETED)
        {
            FindAndDeleteClientCB(response->cbNode);
        }
    }
}

int ParseIPv4Address(unsigned char * ipAddrStr, uint8_t * ipAddr, uint16_t * port)
{
    size_t index = 0;
    unsigned char *itr, *coap;
    uint8_t dotCount = 0;

    ipAddr[index] = 0;
    *port = 0;
    /* search for scheme */
    itr = ipAddrStr;
    if (!isdigit((unsigned char) *ipAddrStr))
    {
        coap = (unsigned char *) OC_COAP_SCHEME;
        while (*coap && tolower(*itr) == *coap)
        {
            coap++;
            itr++;
        }
    }
    ipAddrStr = itr;

    while (*ipAddrStr) {
        if (isdigit((unsigned char) *ipAddrStr))
        {
            ipAddr[index] *= 10;
            ipAddr[index] += *ipAddrStr - '0';
        }
        else if ((unsigned char) *ipAddrStr == '.')
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
            if (isdigit((unsigned char) *ipAddrStr))
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


    if (ipAddr[0] < 255 && ipAddr[1] < 255 && ipAddr[2] < 255 && ipAddr[3] < 255
            && dotCount == 3)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Private internal function prototypes
//-----------------------------------------------------------------------------

static OCDoHandle GenerateInvocationHandle();
static OCStackResult initResources();
static void insertResource(OCResource *resource);
static OCResource *findResource(OCResource *resource);
static void insertResourceType(OCResource *resource,
        OCResourceType *resourceType);
static OCResourceType *findResourceTypeAtIndex(OCResourceHandle handle,
        uint8_t index);
static void insertResourceInterface(OCResource *resource,
        OCResourceInterface *resourceInterface);
static OCResourceInterface *findResourceInterfaceAtIndex(
        OCResourceHandle handle, uint8_t index);
static void deleteResourceType(OCResourceType *resourceType);
static void deleteResourceInterface(OCResourceInterface *resourceInterface);
static void deleteResourceElements(OCResource *resource);
static int deleteResource(OCResource *resource);
static void deleteAllResources();
static void incrementSequenceNumber(OCResource * resPtr);
static OCStackResult verifyUriQueryLength(const char * inputUri,
        uint16_t uriLen);


//-----------------------------------------------------------------------------
// Public APIs
//-----------------------------------------------------------------------------

/**
 * Initialize the OC Stack.  Must be called prior to starting the stack.
 *
 * @param ipAddr
 *     IP Address of host device
 * @param port
 *     Port of host device
 * @param mode
 *     Host device is client, server, or client-server
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack init error
 */
OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
{
    OCStackResult result = OC_STACK_ERROR;
    OC_LOG(INFO, TAG, PCF("Entering OCInit"));

    if (ipAddr)
    {
        OC_LOG_V(INFO, TAG, "IP Address = %s", ipAddr);
    }

    switch (mode)
    {
    case OC_CLIENT:
        OC_LOG(INFO, TAG, PCF("Client mode"));
        break;
    case OC_SERVER:
        OC_LOG(INFO, TAG, PCF("Server mode"));
        break;
    case OC_CLIENT_SERVER:
        OC_LOG(INFO, TAG, PCF("Client-server mode"));
        break;
    default:
        OC_LOG(ERROR, TAG, PCF("Invalid mode"));
        return OC_STACK_ERROR;
        break;
    }
    myStackMode = mode;

    defaultDeviceHandler = NULL;

    // Make call to OCCoAP layer
    result = OCInitCoAP(ipAddr, (uint16_t) port, myStackMode);
    if (result == OC_STACK_OK)
    {
        stackState = OC_STACK_INITIALIZED;
    }
    // Initialize resource
    if(result == OC_STACK_OK && myStackMode != OC_CLIENT)
    {
        result = initResources();
    }
    if(result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("Stack initialization error"));
    }
    return result;
}

/**
 * Stop the OC stack.  Use for a controlled shutdown.
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack not initialized
 */
OCStackResult OCStop()
{
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering OCStop"));

    if (stackState != OC_STACK_INITIALIZED)
    {
        OC_LOG(ERROR, TAG, PCF("Stack not initialized"));
        return OC_STACK_ERROR;
    }

    // Make call to OCCoAP layer
    if (OCStopCoAP() == OC_STACK_OK)
    {
        // Remove all observers
        DeleteObserverList();
        // Remove all the client callbacks
        DeleteClientCBList();
        stackState = OC_STACK_UNINITIALIZED;
        result = OC_STACK_OK;
    } else {
        result = OC_STACK_ERROR;
    }

    // Free memory dynamically allocated for resources
    deleteAllResources();

    if (result != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, PCF("Stack stop error"));
    }

    return result;
}

/**
 * Verify the lengths of the URI and the query separately
 *
 * @param inputUri       - Input URI and query.
 * @param uriLen         - The length of the initial URI with query.
 *
 * Note: The '?' that appears after the URI is not considered as
 * a part of the query.
 */
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

/**
 * Discover or Perform requests on a specified resource (specified by that Resource's respective URI).
 *
 * @param handle             - @ref OCDoHandle to refer to the request sent out on behalf of calling this API.
 * @param method             - @ref OCMethod to perform on the resource
 * @param requiredUri        - URI of the resource to interact with
 * @param referenceUri       - URI of the reference resource
 * @param request            - JSON encoded request
 * @param qos                - quality of service
 * @param cbData             - struct that contains asynchronous callback function that is invoked
 *                             by the stack when discovery or resource interaction is complete
 * @param options            - The address of an array containing the vendor specific header
 *                             header options to be sent with the request
 * @param numOptions         - Number of vendor specific header options to be included
 *
 * @return
 *     OC_STACK_OK               - no errors
 *     OC_STACK_INVALID_CALLBACK - invalid callback function pointer
 *     OC_STACK_INVALID_METHOD   - invalid resource method
 *     OC_STACK_INVALID_URI      - invalid required or reference URI
 */

OCStackResult OCDoResource(OCDoHandle *handle, OCMethod method, const char *requiredUri,
                           const char *referenceUri, const char *request,
                           OCQualityOfService qos, OCCallbackData *cbData,
                           OCHeaderOption * options, uint8_t numOptions)
{
    OCStackResult result = OC_STACK_ERROR;
    OCCoAPToken token;
    ClientCB *clientCB = NULL;
    unsigned char * requestUri = NULL;
    (void) referenceUri;

    OC_LOG(INFO, TAG, PCF("Entering OCDoResource"));

    // Validate input parameters
    VERIFY_NON_NULL(cbData, FATAL, OC_STACK_INVALID_CALLBACK);
    VERIFY_NON_NULL(cbData->cb, FATAL, OC_STACK_INVALID_CALLBACK);

    TODO ("Need to form the final query by concatenating require and reference URI's");
    VERIFY_NON_NULL(requiredUri, FATAL, OC_STACK_INVALID_URI);

    uint16_t uriLen = strlen(requiredUri);

    // ToDo: We should also check if the requiredUri has a mutlicast address, then qos has to be OC_Low_QOS
    switch (method)
    {
        case OC_REST_GET:
        case OC_REST_PUT:
        case OC_REST_POST:
        case OC_REST_OBSERVE:
        case OC_REST_OBSERVE_ALL:
        case OC_REST_CANCEL_OBSERVE:
            break;
        #ifdef WITH_PRESENCE
        case OC_REST_PRESENCE:
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

    requestUri = (unsigned char *) OCMalloc(uriLen + 1);
    if(requestUri)
    {
        memcpy(requestUri, requiredUri, (uriLen + 1));
    }
    else
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    *handle = GenerateInvocationHandle();
    if(!*handle)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    // Generate token which will be used by OCStack to match responses received
    // with the request
    OCGenerateCoAPToken(&token);

    if((result = AddClientCB(&clientCB, cbData, &token, *handle, method, requestUri)) != OC_STACK_OK)
    {
        result = OC_STACK_NO_MEMORY;
        goto exit;
    }

    // Make call to OCCoAP layer
    result = OCDoCoAPResource(method, qos, &token, requiredUri, request, options, numOptions);

exit:
    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCDoResource error"));
        FindAndDeleteClientCB(clientCB);
    }
    return result;
}

/**
 * Cancel a request associated with a specific @ref OCDoResource invocation.
 *
 * @param handle - Used to identify a specific OCDoResource invocation.
 * @param qos    - used to specify Quality of Service (read below for more info)
 * @param options- used to specify vendor specific header options when sending
 *                 explicit observe cancellation
 * @param numOptions- Number of header options to be included
 *
 * @return
 *     OC_STACK_OK               - No errors; Success
 *     OC_STACK_INVALID_PARAM    - The handle provided is invalid.
 */
OCStackResult OCCancel(OCDoHandle handle, OCQualityOfService qos, OCHeaderOption * options,
        uint8_t numOptions) {
    /*
     * This ftn is implemented one of two ways in the case of observation:
     *
     * 1. qos == OC_NON_CONFIRMABLE. When observe is unobserved..
     *      Remove the callback associated on client side.
     *      When the next notification comes in from server,
     *      reply with RESET message to server.
     *      Keep in mind that the server will react to RESET only
     *      if the last notification was sent ans CON
     *
     * 2. qos == OC_CONFIRMABLE. When OCCancel is called,
     *      and it is associated with an observe request
     *      (i.e. ClientCB->method == OC_REST_OBSERVE || OC_REST_OBSERVE_ALL),
     *      Send CON Observe request to server with
     *      observe flag = OC_RESOURCE_OBSERVE_DEREGISTER.
     *      Remove the callback associated on client side.
     */
    OCStackResult ret = OC_STACK_OK;

    if(!handle) {
        return OC_STACK_INVALID_PARAM;
    }

    OC_LOG(INFO, TAG, PCF("Entering OCCancel"));

    ClientCB *clientCB = GetClientCB(NULL, handle, NULL);

    if(clientCB) {
        switch (clientCB->method)
        {
            case OC_REST_OBSERVE:
            case OC_REST_OBSERVE_ALL:
                if(qos == OC_HIGH_QOS)
                {
                    ret = OCDoCoAPResource(OC_REST_CANCEL_OBSERVE, qos,
                            &(clientCB->token), (const char *) clientCB->requestUri, NULL, options,
                            numOptions);
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
                return OC_STACK_INVALID_METHOD;
        }
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
    OCDevAddr dst;
    OCClientResponse clientResponse;
    OCResponse * response = NULL;

    LL_FOREACH(cbList, cbNode) {
        if(OC_REST_PRESENCE == cbNode->method)
        {
            if(cbNode->presence)
            {
                uint32_t now = GetTime(0);
                OC_LOG_V(DEBUG, TAG, "----------------this TTL level %d", cbNode->presence->TTLlevel);
                OC_LOG_V(DEBUG, TAG, "----------------current ticks %d", now);
                if(cbNode->presence->TTLlevel != PresenceTimeOutSize){
                    OC_LOG_V(DEBUG, TAG, "----------------timeout ticks %d",
                            cbNode->presence->timeOut[cbNode->presence->TTLlevel]);
                }
                if(cbNode->presence->TTLlevel >= PresenceTimeOutSize)
                {
                    OC_LOG(DEBUG, TAG, "----------------No more timeout ticks");
                    if (ParseIPv4Address( cbNode->requestUri, ipAddr, &port))
                    {
                        OCBuildIPv4Address(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3], port,
                                &dst);
                        result = FormOCClientResponse(&clientResponse, OC_STACK_PRESENCE_STOPPED,
                                (OCDevAddr *) &dst, 0, NULL);
                        if(result != OC_STACK_OK)
                        {
                            goto exit;
                        }
                        result = FormOCResponse(&response, cbNode, 0, &clientResponse);
                        if(result != OC_STACK_OK)
                        {
                            goto exit;
                        }
                    }
                    else
                    {
                        result = OC_STACK_INVALID_IP;
                        goto exit;
                    }
                    HandleStackResponses(response);
                }
                if(now >= cbNode->presence->timeOut[cbNode->presence->TTLlevel])
                {
                    OC_LOG(DEBUG, TAG, "time to test server presence ==========");
                    OCCoAPToken token;
                    OCGenerateCoAPToken(&token);
                    result = OCDoCoAPResource(OC_REST_GET, OC_LOW_QOS,
                            &token, (const char *)cbNode->requestUri, NULL, NULL, 0);
                    if(result != OC_STACK_OK)
                    {
                        goto exit;
                    }
                    cbNode->presence->TTLlevel++;
                    OC_LOG_V(DEBUG, TAG, "----------------moving to TTL level %d", cbNode->presence->TTLlevel);
                }
            }
        }
    }
exit:
    if (result != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCProcessPresence error"));
    }
    return result;
}
#endif

/**
 * Called in main loop of OC client or server.  Allows low-level processing of
 * stack services.
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCProcess() {

    OC_LOG(INFO, TAG, PCF("Entering OCProcess"));
    #ifdef WITH_PRESENCE
    OCProcessPresence();
    #endif
    OCProcessCoAP();

    return OC_STACK_OK;
}

#ifdef WITH_PRESENCE
/**
 * When operating in @ref OCServer or @ref OCClientServer mode, this API will start sending out
 * presence notifications to clients via multicast. Once this API has been called with a success,
 * clients may query for this server's presence and this server's stack will respond via multicast.
 *
 * Server can call this function when it comes online for the first time, or when it comes back
 * online from offline mode, or when it re enters network.
 *
 * @param ttl - Time To Live in seconds
 * Note: If ttl is '0', then the default stack value will be used (60 Seconds).
 *
 * @return
 *     OC_STACK_OK      - No errors; Success
 */
OCStackResult OCStartPresence(const uint32_t ttl)
{
    OCChangeResourceProperty(
            &(((OCResource *)presenceResource.handle)->resourceProperties),
            OC_ACTIVE, 1);

    if(ttl > 0)
    {
        presenceResource.presenceTTL = ttl;
    }

    if(OC_PRESENCE_UNINITIALIZED == presenceState)
    {
        OCDevAddr multiCastAddr;
        OCCoAPToken token;

        presenceState = OC_PRESENCE_INITIALIZED;
        OCGenerateCoAPToken(&token);
        OCBuildIPv4Address(224, 0, 1, 187, 5683, &multiCastAddr);
        //add the presence observer
        AddObserver(OC_PRESENCE_URI, NULL, 0, &token, &multiCastAddr,
            (OCResource *)presenceResource.handle, OC_LOW_QOS);
    }

    // Each time OCStartPresence is called
    // a different random 32-bit integer number is used
    ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();

    return OCNotifyAllObservers(presenceResource.handle, OC_LOW_QOS);
}

/**
 * When operating in @ref OCServer or @ref OCClientServer mode, this API will stop sending out
 * presence notifications to clients via multicast. Once this API has been called with a success,
 * this server's stack will not respond to clients querying for this server's presence.
 *
 * Server can call this function when it is terminating, going offline, or when going
 * away from network.
 *
 * @return
 *     OC_STACK_OK      - No errors; Success
 */
OCStackResult OCStopPresence()
{
    OCStackResult result = OC_STACK_ERROR;
    //make resource inactive
    result = OCChangeResourceProperty(
            &(((OCResource *) presenceResource.handle)->resourceProperties),
            OC_ACTIVE, 0);
    result = OCNotifyAllObservers(presenceResource.handle, OC_LOW_QOS);
    return result;
}
#endif


OCStackResult OCSetDefaultDeviceEntityHandler(OCDeviceEntityHandler entityHandler)
{
    defaultDeviceHandler = entityHandler;

    return OC_STACK_OK;
}

/**
 * Create a resource
 *
 * @param handle - pointer to handle to newly created resource.  Set by ocstack.  Used to refer to resource
 * @param resourceTypeName - name of resource type.  Example: "core.led"
 * @param resourceInterfaceName - name of resource interface.  Example: "core.rw"
 * @param uri - URI of the resource.  Example:  "/a/led"
 * @param entityHandler - entity handler function that is called by ocstack to handle requests, etc
 *                        NULL for default entity handler
 * @param resourceProperties - properties supported by resource.  Example: OC_DISCOVERABLE|OC_OBSERVABLE
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCCreateResource(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceInterfaceName,
        const char *uri, OCEntityHandler entityHandler,
        uint8_t resourceProperties) {

    OCResource *pointer = NULL;
    char *str = NULL;
    size_t size;
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering OCCreateResource"));

    if(myStackMode == OC_CLIENT)
    {
        return result;
    }
    // Validate parameters
    // Is it presented during resource discovery?
    if (!handle || !resourceTypeName || !uri) {
        OC_LOG(ERROR, TAG, PCF("Input parameter is NULL"));
        return OC_STACK_INVALID_PARAM;
    }

    if(!resourceInterfaceName || strlen(resourceInterfaceName) == 0) {
        resourceInterfaceName = OC_RSRVD_INTERFACE_DEFAULT;
    }

    // Make sure resourceProperties bitmask has allowed properties specified
    if (resourceProperties
            > (OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | OC_SLOW)) {
        OC_LOG(ERROR, TAG, PCF("Invalid property"));
        return OC_STACK_INVALID_PARAM;
    }

    // If the headResource is NULL, then no resources have been created...
    pointer = headResource;
    if (pointer) {
        // At least one resources is in the resource list, so we need to search for
        // repeated URLs, which are not allowed.  If a repeat is found, exit with an error
        while (pointer) {
            if (strcmp(uri, pointer->uri) == 0) {
                OC_LOG(ERROR, TAG, PCF("URI already in use"));
                return OC_STACK_INVALID_PARAM;
            }
            pointer = pointer->next;
        }
    }
    // Create the pointer and insert it into the resource list
    pointer = (OCResource *) OCMalloc(sizeof(OCResource));
    if (!pointer) {
        goto exit;
    }
    memset(pointer, 0, sizeof(OCResource));
    pointer->sequenceNum = OC_OFFSET_SEQUENCE_NUMBER;

    insertResource(pointer);

    // Set the uri
    size = strlen(uri) + 1;
    str = (char *) OCMalloc(size);
    if (!str) {
        goto exit;
    }
    strncpy(str, uri, size);
    pointer->uri = str;

    // Set properties.  Set OC_ACTIVE
    pointer->resourceProperties = (OCResourceProperty) (resourceProperties
            | OC_ACTIVE);

    // Add the resourcetype to the resource
    result = OCBindResourceTypeToResource((OCResourceHandle) pointer,
            resourceTypeName);
    if (result != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, PCF("Error adding resourcetype"));
        goto exit;
    }

    // Add the resourceinterface to the resource
    result = OCBindResourceInterfaceToResource((OCResourceHandle) pointer,
            resourceInterfaceName);
    if (result != OC_STACK_OK) {
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
        OCNotifyAllObservers(presenceResource.handle, OC_LOW_QOS);
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

/**
 * Add a resource to a collection resource.
 *
 * @param collectionHandle - handle to the collection resource
 * @param resourceHandle - handle to resource to be added to the collection resource
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 *     OC_STACK_INVALID_PARAM - invalid collectionhandle
 */
OCStackResult OCBindResource(
        OCResourceHandle collectionHandle, OCResourceHandle resourceHandle) {
    OCResource *resource;
    uint8_t i;

    OC_LOG(INFO, TAG, PCF("Entering OCBindResource"));

    // Validate parameters
    VERIFY_NON_NULL(collectionHandle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(resourceHandle, ERROR, OC_STACK_ERROR);
    // Container cannot contain itself
    if (collectionHandle == resourceHandle) {
        OC_LOG(ERROR, TAG, PCF("Added handle equals collection handle"));
        return OC_STACK_INVALID_PARAM;
    }

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *) collectionHandle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Collection handle not found"));
        return OC_STACK_INVALID_PARAM;
    }

    // Look for an open slot to add add the child resource.
    // If found, add it and return success
    for (i = 0; i < MAX_CONTAINED_RESOURCES; i++) {
        if (!resource->rsrcResources[i]) {
            resource->rsrcResources[i] = (OCResource *) resourceHandle;
            OC_LOG(INFO, TAG, PCF("resource bound"));
            return OC_STACK_OK;
        }
    }

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        OCNotifyAllObservers(presenceResource.handle, OC_LOW_QOS);
    }
    #endif

    // Unable to add resourceHandle, so return error
    return OC_STACK_ERROR;
}

/**
 * Remove a resource from a collection resource.
 *
 * @param collectionHandle - handle to the collection resource
 * @param resourceHandle - handle to resource to be added to the collection resource
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 *     OC_STACK_INVALID_PARAM - invalid collectionHandle
 */
OCStackResult OCUnBindResource(
        OCResourceHandle collectionHandle, OCResourceHandle resourceHandle) {
    OCResource *resource;
    uint8_t i;

    OC_LOG(INFO, TAG, PCF("Entering OCUnBindResource"));

    // Validate parameters
    VERIFY_NON_NULL(collectionHandle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(resourceHandle, ERROR, OC_STACK_ERROR);
    // Container cannot contain itself
    if (collectionHandle == resourceHandle) {
        OC_LOG(ERROR, TAG, PCF("removing handle equals collection handle"));
        return OC_STACK_INVALID_PARAM;
    }

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *) collectionHandle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Collection handle not found"));
        return OC_STACK_INVALID_PARAM;
    }

    // Look for an open slot to add add the child resource.
    // If found, add it and return success
    for (i = 0; i < MAX_CONTAINED_RESOURCES; i++) {
        if (resourceHandle == resource->rsrcResources[i]) {
            resource->rsrcResources[i] = (OCResource *) NULL;
            OC_LOG(INFO, TAG, PCF("resource unbound"));
            return OC_STACK_OK;
        }
    }

    OC_LOG(INFO, TAG, PCF("resource not found in collection"));

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        OCNotifyAllObservers(presenceResource.handle, OC_LOW_QOS);
    }
    #endif

    // Unable to add resourceHandle, so return error
    return OC_STACK_ERROR;
}


/**
 * Bind a resourcetype to a resource.
 *
 * @param handle - handle to the resource
 * @param resourceTypeName - name of resource type.  Example: "core.led"
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCBindResourceTypeToResource(OCResourceHandle handle,
        const char *resourceTypeName) {

    OCResourceType *pointer = NULL;
    char *str = NULL;
    size_t size;
    OCResource *resource;
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering OCBindResourceTypeToResource"));

    // Validate parameters
    VERIFY_NON_NULL(resourceTypeName, ERROR, OC_STACK_INVALID_PARAM);
    // TODO:  Does resource attribute resentation really have to be maintained in stack?
    // Is it presented during resource discovery?

    // Make sure resource exists
    resource = findResource((OCResource *) handle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_ERROR;
    }

    TODO ("Make sure that the resourcetypename doesn't already exist in the resource");

    // Create the resourcetype and insert it into the resource list
    pointer = (OCResourceType *) OCMalloc(sizeof(OCResourceType));
    if (!pointer) {
        goto exit;
    }
    memset(pointer, 0, sizeof(OCResourceType));

    // Set the resourceTypeName
    size = strlen(resourceTypeName) + 1;
    str = (char *) OCMalloc(size);
    if (!str) {
        goto exit;
    }
    strncpy(str, resourceTypeName, size);
    pointer->resourcetypename = str;

    insertResourceType(resource, pointer);
    result = OC_STACK_OK;

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        OCNotifyAllObservers(presenceResource.handle, OC_LOW_QOS);
    }
    #endif

    exit: if (result != OC_STACK_OK) {
        OCFree(pointer);
        OCFree(str);
    }
    return result;
}

/**
 * Bind a resourceinterface to a resource.
 *
 * @param handle - handle to the resource
 * @param resourceInterfaceName - name of resource interface.  Example: "core.rw"
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCBindResourceInterfaceToResource(OCResourceHandle handle,
        const char *resourceInterfaceName) {

    OCResourceInterface *pointer = NULL;
    char *str = NULL;
    size_t size;
    OCResource *resource;
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering OCBindResourceInterfaceToResource"));

    // Validate parameters
    VERIFY_NON_NULL(resourceInterfaceName, ERROR, OC_STACK_INVALID_PARAM);

    // Make sure resource exists
    resource = findResource((OCResource *) handle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_INVALID_PARAM;
    }

    TODO ("Make sure that the resourceinterface name doesn't already exist in the resource");

    // Create the resourceinterface and insert it into the resource list
    pointer = (OCResourceInterface *) OCMalloc(sizeof(OCResourceInterface));
    if (!pointer) {
        goto exit;
    }
    memset(pointer, 0, sizeof(OCResourceInterface));

    // Set the resourceinterface name
    size = strlen(resourceInterfaceName) + 1;
    str = (char *) OCMalloc(size);
    if (!str) {
        goto exit;
    }
    strncpy(str, resourceInterfaceName, size);
    pointer->name = str;

    // Bind the resourceinterface to the resource
    insertResourceInterface(resource, pointer);

    result = OC_STACK_OK;

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        OCNotifyAllObservers(presenceResource.handle, OC_LOW_QOS);
    }
    #endif

    exit: if (result != OC_STACK_OK) {
        OCFree(pointer);
        OCFree(str);
    }
    return result;
}

/**
 * Get the number of resources that have been created in the stack.
 *
 * @param numResources - pointer to count variable
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error

 */
OCStackResult OCGetNumberOfResources(uint8_t *numResources) {
    OCResource *pointer = headResource;

    OC_LOG(INFO, TAG, PCF("Entering OCGetNumberOfResources"));
    VERIFY_NON_NULL(numResources, ERROR, OC_STACK_INVALID_PARAM);
    *numResources = 0;
    while (pointer) {
        *numResources = *numResources + 1;
        pointer = pointer->next;
    }
    return OC_STACK_OK;
}

/**
 * Get a resource handle by index.
 *
 * @param index - index of resource, 0 to Count - 1
 *
 * @return
 *    Resource handle - if found
 *    NULL - if not found
 */
OCResourceHandle OCGetResourceHandle(uint8_t index) {
    OCResource *pointer = headResource;
    uint8_t i = 0;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceHandle"));

    // Iterate through the list
    while ((i < index) && pointer) {
        i++;
        pointer = pointer->next;
    }
    return (OCResourceHandle) pointer;
}

/**
 * Delete resource specified by handle.  Deletes resource and all resourcetype and resourceinterface
 * linked lists.
 *
 * @param handle - handle of resource to be deleted
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCDeleteResource(OCResourceHandle handle) {
    OC_LOG(INFO, TAG, PCF("Entering OCDeleteResource"));

    if (!handle) {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_INVALID_PARAM;
    }

    if (deleteResource((OCResource *) handle) == 0) {
        OC_LOG(ERROR, TAG, PCF("Error deleting resource"));
        return OC_STACK_ERROR;
    }

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        OCNotifyAllObservers(presenceResource.handle, OC_LOW_QOS);
    }
    #endif

    return OC_STACK_OK;
}

/**
 * Get the URI of the resource specified by handle.
 *
 * @param handle - handle of resource
 * @return
 *    URI string - if resource found
 *    NULL - resource not found
 */
const char *OCGetResourceUri(OCResourceHandle handle) {
    OCResource *resource;
    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceUri"));

    resource = findResource((OCResource *) handle);
    if (resource) {
        return resource->uri;
    }
    return (const char *) NULL;
}

/**
 * Get the properties of the resource specified by handle.
 * NOTE: that after a resource is created, the OC_ACTIVE property is set
 * for the resource by the stack.
 *
 * @param handle - handle of resource
 * @return
 *    property bitmap - if resource found
 *    NULL - resource not found
 */
uint8_t OCGetResourceProperties(OCResourceHandle handle) {
    OCResource *resource;
    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceProperties"));

    resource = findResource((OCResource *) handle);
    if (resource) {
        return resource->resourceProperties;
    }
    return 0;
}

/**
 * Get the number of resource types of the resource.
 *
 * @param handle - handle of resource
 * @param numResourceTypes - pointer to count variable
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCGetNumberOfResourceTypes(OCResourceHandle handle,
        uint8_t *numResourceTypes) {
    OCResource *resource;
    OCResourceType *pointer;

    OC_LOG(INFO, TAG, PCF("Entering OCGetNumberOfResourceTypes"));
    VERIFY_NON_NULL(numResourceTypes, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);

    *numResourceTypes = 0;

    resource = findResource((OCResource *) handle);
    if (resource) {
        pointer = resource->rsrcType;
        while (pointer) {
            *numResourceTypes = *numResourceTypes + 1;
            pointer = pointer->next;
        }
    }
    return OC_STACK_OK;
}

/**
 * Get name of resource type of the resource.
 *
 * @param handle - handle of resource
 * @param index - index of resource, 0 to Count - 1
 *
 * @return
 *    resource type name - if resource found
 *    NULL - resource not found
 */
const char *OCGetResourceTypeName(OCResourceHandle handle, uint8_t index) {
    OCResourceType *resourceType;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceTypeName"));

    resourceType = findResourceTypeAtIndex(handle, index);
    if (resourceType) {
        return resourceType->resourcetypename;
    }
    return (const char *) NULL;
}



/**
 * Get the number of resource interfaces of the resource.
 *
 * @param handle - handle of resource
 * @param numResources - pointer to count variable
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCGetNumberOfResourceInterfaces(OCResourceHandle handle,
        uint8_t *numResourceInterfaces) {
    OCResourceInterface *pointer;
    OCResource *resource;

    OC_LOG(INFO, TAG, PCF("Entering OCGetNumberOfResourceInterfaces"));

    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(numResourceInterfaces, ERROR, OC_STACK_INVALID_PARAM);

    *numResourceInterfaces = 0;
    resource = findResource((OCResource *) handle);
    if (resource) {
        pointer = resource->rsrcInterface;
        while (pointer) {
            *numResourceInterfaces = *numResourceInterfaces + 1;
            pointer = pointer->next;
        }
    }
    return OC_STACK_OK;
}

/**
 * Get name of resource interface of the resource.
 *
 * @param handle - handle of resource
 * @param index - index of resource, 0 to Count - 1
 *
 * @return
 *    resource interface name - if resource found
 *    NULL - resource not found
 */
const char *OCGetResourceInterfaceName(OCResourceHandle handle, uint8_t index) {
    OCResourceInterface *resourceInterface;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceInterfaceName"));

    resourceInterface = findResourceInterfaceAtIndex(handle, index);
    if (resourceInterface) {
        return resourceInterface->name;
    }
    return (const char *) NULL;
}

/**
 * Get resource handle from the collection resource by index.
 *
 * @param collectionHandle - handle of collection resource
 * @param index - index of contained resource, 0 to Count - 1
 *
 * @return
 *    handle to resource - if resource found
 *    NULL - resource not found
 */
OCResourceHandle OCGetResourceHandleFromCollection(OCResourceHandle collectionHandle,
        uint8_t index) {
    OCResource *resource;

    OC_LOG(INFO, TAG, PCF("Entering OCGetContainedResource"));

    if (index >= MAX_CONTAINED_RESOURCES) {
        return NULL;
    }

    resource = findResource((OCResource *) collectionHandle);
    if (!resource) {
        return NULL;
    }

    return resource->rsrcResources[index];
}

/**
 * Bind an entity handler to the resource.
 *
 * @param handle - handle to the resource that the contained resource is to be bound
 * @param entityHandler - entity handler function that is called by ocstack to handle requests, etc
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCBindResourceHandler(OCResourceHandle handle,
        OCEntityHandler entityHandler) {
    OCResource *resource;

    OC_LOG(INFO, TAG, PCF("Entering OCBindResourceHandler"));

    // Validate parameters
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);
    //VERIFY_NON_NULL(entityHandler, ERROR, OC_STACK_INVALID_PARAM);

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *)handle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_ERROR;
    }

    // Bind the handler
    resource->entityHandler = entityHandler;

    #ifdef WITH_PRESENCE
    if(presenceResource.handle)
    {
        ((OCResource *)presenceResource.handle)->sequenceNum = OCGetRandom();
        OCNotifyAllObservers(presenceResource.handle, OC_LOW_QOS);
    }
    #endif

    return OC_STACK_OK;
}

/**
 * Get the entity handler for a resource.
 *
 * @param handle - handle of resource
 *
 * @return
 *    entity handler - if resource found
 *    NULL - resource not found
 */
OCEntityHandler OCGetResourceHandler(OCResourceHandle handle) {
    OCResource *resource;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceHandler"));

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *)handle);
    if (!resource) {
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

/**
 * Notify observers that an observed value has changed.
 *
 * @param handle - handle of resource
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_NO_RESOURCE - invalid resource handle
 *     OC_STACK_NO_OBSERVERS - no more observers intrested in resource
 */
OCStackResult OCNotifyAllObservers(OCResourceHandle handle, OCQualityOfService qos) {
    OCResource *resPtr = NULL;
    OCStackResult result;
    OCMethod method = OC_REST_NOMETHOD;
    uint32_t maxAge = 0;

    OC_LOG(INFO, TAG, PCF("Entering OCNotifyAllObservers"));

    VERIFY_NON_NULL(handle, ERROR, OC_STACK_ERROR);

    // Verify that the resource exists
    resPtr = findResource ((OCResource *) handle);
    if (NULL == resPtr || myStackMode == OC_CLIENT)
    {
        return OC_STACK_NO_RESOURCE;
    } else {
        #ifdef WITH_PRESENCE
        if(strcmp(resPtr->uri, OC_PRESENCE_URI))
        {
        #endif
            //only increment in the case of regular observing (not presence)
            incrementSequenceNumber(resPtr);
            method = OC_REST_OBSERVE;
            maxAge = 0x2FFFF;
        #ifdef WITH_PRESENCE
        }
        else
        {
            method = OC_REST_PRESENCE;
            if((((OCResource *) presenceResource.handle)->resourceProperties) & OC_ACTIVE)
            {
                maxAge = presenceResource.presenceTTL;
            }
            else
            {
                maxAge = 0;
            }

        }
        #endif
        result = SendObserverNotification (method, resPtr, maxAge, qos);
        return result;
    }
}

OCStackResult
OCNotifyListOfObservers (OCResourceHandle handle,
                         OCObservationId  *obsIdList,
                         uint8_t          numberOfIds,
                         unsigned char    *notificationJSONPayload,
                         OCQualityOfService qos)
{
    OC_LOG(INFO, TAG, PCF("Entering OCNotifyListOfObservers"));

    VERIFY_NON_NULL(handle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(obsIdList, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(notificationJSONPayload, ERROR, OC_STACK_ERROR);

    uint8_t numIds = numberOfIds;
    ResourceObserver *observation;
    OCResource *resPtr = NULL;
    uint32_t maxAge = 0;
    unsigned char bufNotify[MAX_RESPONSE_LENGTH] = {0};
    unsigned char *currPtr;
    uint8_t numSentNotification = 0;

    // Verify the notification payload length does not exceed the maximim
    // the stack can handle
    if ((strlen((char *)notificationJSONPayload) +
         OC_JSON_PREFIX_LEN + OC_JSON_SUFFIX_LEN) > MAX_RESPONSE_LENGTH)
    {
        OC_LOG(INFO, TAG, PCF("Observe notification message length too long"));
        return OC_STACK_ERROR;
    }

    // Verify that the resource exists
    resPtr = findResource ((OCResource *) handle);
    if (NULL == resPtr || myStackMode == OC_CLIENT)
    {
        return OC_STACK_NO_RESOURCE;
    }
    else
    {
        incrementSequenceNumber(resPtr);
        //TODO: we should allow the serve to define thisl
        maxAge = 0x2FFFF;
    }

    while (numIds)
    {
        OC_LOG_V(INFO, TAG, "Need to notify observation id %d", *obsIdList);
        observation = NULL;
        observation = GetObserverUsingId (*obsIdList);
        if (observation)
        {
            // Found observation - verify if it matches the resource handle
            if (observation->resource == resPtr)
            {
                strcpy((char*)bufNotify, OC_JSON_PREFIX);
                currPtr = bufNotify + OC_JSON_PREFIX_LEN;
                memcpy (currPtr, notificationJSONPayload, strlen((char *)notificationJSONPayload));
                currPtr += strlen((char *)notificationJSONPayload);
                strcpy((char*)currPtr, OC_JSON_SUFFIX);

                // send notifications based on the qos of the request
                // The qos passed as a parameter overrides what the client requested
                // If we want the client preference taking high priority add:
                // QoS = resourceObserver->qos;
                if(qos == OC_NA_QOS){
                    qos = observation->qos;
                }
                if(qos != OC_HIGH_QOS)
                {
                    OC_LOG_V(INFO, TAG, "Current NON count for this observer is %d",
                            observation->lowQosCount);
                    if(observation->forceHighQos \
                            || observation->lowQosCount >= MAX_OBSERVER_NON_COUNT)
                    {
                        observation->lowQosCount = 0;
                        // at some point we have to to send CON to check on the
                        // availability of observer
                        OC_LOG(INFO, TAG, PCF("This time we are sending the \
                                notification as High qos"));
                        qos = OC_HIGH_QOS;
                    }
                    else
                    {
                        observation->lowQosCount++;
                    }
                }
                OCSendCoAPNotification (observation->resUri, observation->addr,
                                        qos, &(observation->token),
                                        bufNotify, resPtr, maxAge);
                numSentNotification++;
            }
        }
        obsIdList++;
        numIds--;
    }
    if(numSentNotification == numberOfIds)
    {
        return OC_STACK_OK;
    }
    else if(numSentNotification == 0)
    {
        return OC_STACK_NO_OBSERVERS;
    }
    else
    {
        //TODO: we need to signal that not every one in the
        // list got an update, should we also indicate who did not receive on?
        return OC_STACK_OK;
    }
}

//-----------------------------------------------------------------------------
// Private internal function definitions
//-----------------------------------------------------------------------------
/**
 * Generate handle of OCDoResource invocation for callback management.
 */
static OCDoHandle GenerateInvocationHandle()
{
    OCDoHandle handle = NULL;
    // Generate token here, it will be deleted when the transaction is deleted
    handle = (OCDoHandle) malloc(sizeof(uint8_t[MAX_TOKEN_LENGTH]));
    if (handle)
    {
        OCFillRandomMem((uint8_t*)handle, sizeof(uint8_t[MAX_TOKEN_LENGTH]));
    }

    return handle;
}
#ifdef WITH_PRESENCE
OCStackResult OCChangeResourceProperty(OCResourceProperty * inputProperty,
        OCResourceProperty resourceProperties, uint8_t enable)
{
    if (resourceProperties
            > (OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | OC_SLOW)) {
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

/**
 * Initialize resource data structures, variables, etc.
 */
OCStackResult initResources() {
    OCStackResult result = OC_STACK_OK;
    // Init application resource vars
    headResource = NULL;
    // Init Virtual Resources
    #ifdef WITH_PRESENCE
    presenceResource.presenceTTL = OC_DEFAULT_PRESENCE_TTL;
    //presenceResource.token = OCGenerateCoAPToken();
    result = OCCreateResource(&presenceResource.handle,
            "core.presence",
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

/**
 * Add a resource to the end of the linked list of resources.
 *
 * @param resource - resource to be added
 */
void insertResource(OCResource *resource) {
    OCResource *pointer;

    if (!headResource) {
        headResource = resource;
    } else {
        pointer = headResource;

        while (pointer->next) {
            pointer = pointer->next;
        }
        pointer->next = resource;
    }
    resource->next = NULL;
}

/**
 * Find a resource in the linked list of resources.
 *
 * @param resource - resource to be found
 * @return
 *     NULL                - resource not found
 *     pointer to resource - pointer to resource that was found in the linked list
 */
OCResource *findResource(OCResource *resource) {
    OCResource *pointer = headResource;

    while (pointer) {
        if (pointer == resource) {
            return resource;
        }
        pointer = pointer->next;
    }
    return NULL;
}

void deleteAllResources()
{
    OCResource *pointer = headResource;
    OCResource *temp;

    while (pointer)
    {
        temp = pointer->next;
        deleteResource(pointer);
        pointer = temp;
    }
}

/**
 * Delete the resource from the linked list.
 *
 * @param resource - resource to be deleted
 * @return
 *    0 - error
 *    1 - success
 */
int deleteResource(OCResource *resource) {
    OCResource *prev = NULL;
    OCResource *temp;

    temp = headResource;
    while (temp) {
        if (temp == resource) {
            if (temp == headResource) {
                headResource = temp->next;
            } else {
                prev->next = temp->next;
            }

            resource->resourceProperties = (OCResourceProperty) 0; // Invalidate all Resource Properties.
            OCNotifyAllObservers((OCResourceHandle)resource, OC_HIGH_QOS);

            deleteResourceElements(temp);
            OCFree(temp);
            return 1;
        } else {
            prev = temp;
            temp = temp->next;
        }
    }

    return 0;
}

/**
 * Delete all of the dynamically allocated elements that were created for the resource.
 *
 * @param resource - specified resource
 */
void deleteResourceElements(OCResource *resource) {
    if (!resource) {
        return;
    }

    // remove URI
    OCFree(resource->uri);

    // Delete resourcetype linked list
    deleteResourceType(resource->rsrcType);

    // Delete resourceinterface linked list
    deleteResourceInterface(resource->rsrcInterface);
}

/**
 * Delete all of the dynamically allocated elements that were created for the resource type.
 *
 * @param resourceType - specified resource type
 */
void deleteResourceType(OCResourceType *resourceType) {
    OCResourceType *pointer = resourceType;
    OCResourceType *next;

    while (pointer) {
        next = pointer->next;
        OCFree(pointer->resourcetypename);
        OCFree(pointer);
        pointer = next;
    }
}

/**
 * Delete all of the dynamically allocated elements that were created for the resource interface.
 *
 * @param resourceInterface - specified resource interface
 */
void deleteResourceInterface(OCResourceInterface *resourceInterface) {
    OCResourceInterface *pointer = resourceInterface;
    OCResourceInterface *next;

    while (pointer) {
        next = pointer->next;
        OCFree(pointer->name);
        OCFree(pointer);
        pointer = next;
    }
}

/**
 * Insert a resource type into a resource's resource type linked list.
 *
 * @param resource - resource where resource type is to be inserted
 * @param resourceType - resource type to be inserted
 */
void insertResourceType(OCResource *resource, OCResourceType *resourceType) {
    OCResourceType *pointer;

    if (!resource->rsrcType) {
        resource->rsrcType = resourceType;
    } else {
        pointer = resource->rsrcType;
        while (pointer->next) {
            pointer = pointer->next;
        }
        pointer->next = resourceType;
    }
    resourceType->next = NULL;
}

/**
 * Get a resource type at the specified index within a resource.
 *
 * @param handle - handle of resource
 * @param index - index of resource type
 *
 * @return
 *    resourcetype - if found
 *    NULL - not found
 */
OCResourceType *findResourceTypeAtIndex(OCResourceHandle handle, uint8_t index) {
    OCResource *resource;
    OCResourceType *pointer;
    uint8_t i;

    // Find the specified resource
    resource = findResource((OCResource *) handle);
    if (!resource) {
        return NULL;
    }

    // Make sure a resource has a resourcetype
    if (!resource->rsrcType) {
        return NULL;
    }

    // Iterate through the list
    pointer = resource->rsrcType;
    i = 0;
    while ((i < index) && pointer) {
        i++;
        pointer = pointer->next;
    }
    return pointer;
}

/**
 * Insert a resource interface into a resource's resource interface linked list.
 *
 * @param resource - resource where resource interface is to be inserted
 * @param resourceInterface - resource interface to be inserted
 */
void insertResourceInterface(OCResource *resource,
        OCResourceInterface *resourceInterface) {
    OCResourceInterface *pointer;

    if (!resource->rsrcInterface) {
        resource->rsrcInterface = resourceInterface;
    } else {
        pointer = resource->rsrcInterface;
        while (pointer->next) {
            pointer = pointer->next;
        }
        pointer->next = resourceInterface;
    }
    resourceInterface->next = NULL;
}

/**
 * Get a resource interface at the specified index within a resource.
 *
 * @param handle - handle of resource
 * @param index - index of resource interface
 *
 * @return
 *    resourceinterface - if found
 *    NULL - not found
 */
OCResourceInterface *findResourceInterfaceAtIndex(OCResourceHandle handle,
        uint8_t index) {
    OCResource *resource;
    OCResourceInterface *pointer;
    uint8_t i = 0;

    // Find the specified resource
    resource = findResource((OCResource *) handle);
    if (!resource) {
        return NULL;
    }

    // Make sure a resource has a resourceinterface
    if (!resource->rsrcInterface) {
        return NULL;
    }

    // Iterate through the list
    pointer = resource->rsrcInterface;

    while ((i < index) && pointer) {
        i++;
        pointer = pointer->next;
    }
    return pointer;
}


