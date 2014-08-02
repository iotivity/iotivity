//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
#include "ocserverrequest.h"
#include "ocresource.h"
#include "occlientcb.h"
#include "ocrandom.h"
#include "debug.h"
#include "occoap.h"
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef enum {
    OC_STACK_UNINITIALIZED = 0, OC_STACK_INITIALIZED
} OCStackState;

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------
static OCStackState stackState = OC_STACK_UNINITIALIZED;
OCResource *headResource = NULL;

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
OCStackResult HandleStackRequests(OCRequest * request) {
    OC_LOG(INFO, TAG, "Entering OCStackHandleReceiveRequest (OCStack Layer)");

    char *filterValue;
    uint8_t filterOn;
    OCStackResult result;

    VERIFY_NON_NULL(request, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(request->entityHandlerRequest, ERROR, OC_STACK_ERROR);

    if (IsVirtualResource((const char*)request->resourceUrl))
    {
        result = ValidateUrlQuery(request->resourceUrl,
                                  request->entityHandlerRequest->query, &filterOn,
                                  &filterValue);

        if (result == OC_STACK_OK)
        {
            result = ProcessResourceDiscoverReq(
                    (const char*) request->entityHandlerRequest->reqJSONPayload,
                    (char *) request->entityHandlerRequest->resJSONPayload, filterOn,
                    filterValue);
        }
    }
    else
    {
        OCResource* resource = FindResourceByUri((const char*)request->resourceUrl);
        if (resource)
        {
            result = resource->entityHandler(OC_REQUEST_FLAG, request->entityHandlerRequest);
        }
        else
        {
            result = OC_STACK_NO_RESOURCE;
        }
    }

    return result;
}

//This function will be called back by occoap layer when a response is received
void HandleStackResponses(OCResponse * response) {
    ClientCB * cbNode = NULL;
    OCStackApplicationResult result = OC_STACK_DELETE_TRANSACTION;
    OC_LOG(INFO, TAG, "Entering OCHandleClientReceiveResponse (OCStack Layer)");

    TODO ("What does the stack does in case of error");
    if (response->clientResponse->result != OC_STACK_OK) {
        OC_LOG(DEBUG, TAG, "The response has an error, should we do anything?");
    }

    cbNode = GetClientCB(response->token);
    if (cbNode) {
        OC_LOG(INFO, TAG, PCF("Calling into application address space"));
        result = cbNode->callBack(cbNode->context, response->clientResponse);
        if (result == OC_STACK_DELETE_TRANSACTION) {
            DeleteClientCB(cbNode);
        }
    }
    TODO ("What does the stack does in case of error");
}

int ParseIPv4Address(unsigned char * ipAddrStr, uint8_t * ipAddr) {
    size_t index = 0;
    unsigned char *itr, *coap;
    uint8_t dotCount = 0;

    /* search for scheme */
    itr = ipAddrStr;
    if (!isdigit((unsigned char) *ipAddrStr)) {
        coap = (unsigned char *) OC_COAP_SCHEME;
        while (*coap && tolower(*itr) == *coap) {
            coap++;
            itr++;
        }
    }
    ipAddrStr = itr;

    while (*ipAddrStr) {
        if (isdigit((unsigned char) *ipAddrStr)) {
            ipAddr[index] *= 10;
            ipAddr[index] += *ipAddrStr - '0';
        } else if ((unsigned char) *ipAddrStr == '.') {
            index++;
            dotCount++;
        } else {
            break;
        }
        ipAddrStr++;
    }

    if (ipAddr[0] < 255 && ipAddr[1] < 255 && ipAddr[2] < 255 && ipAddr[3] < 255
            && dotCount == 3) {
        return 1;
    } else {
        return 0;
    }
}
//-----------------------------------------------------------------------------
// Private internal function prototypes
//-----------------------------------------------------------------------------

static void initResources();
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

//-----------------------------------------------------------------------------
// Default resource entity handler function
//-----------------------------------------------------------------------------
void defaultResourceEHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest * request) {
    TODO ("Implement me!!!!");
    // TODO:  remove silence unused param warnings
    (void) flag;
    (void) request;
}

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
OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode) {
    OC_LOG(INFO, TAG, PCF("Entering OCInit"));

    if (ipAddr) {
        OC_LOG_V(INFO, TAG, "IP Address = %s", ipAddr);
    }

    switch (mode) {
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

    // Initialize resource
    initResources();

    // Make call to OCCoAP layer
    if (OCInitCoAP(ipAddr, (uint16_t) port, mode) == 0) {
        stackState = OC_STACK_INITIALIZED;
        return OC_STACK_OK;
    }

    OC_LOG(ERROR, TAG, PCF("Stack initialization error"));
    return OC_STACK_ERROR;
}

/**
 * Stop the OC stack.  Use for a controlled shutdown.
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack not initialized
 */
OCStackResult OCStop() {
    OC_LOG(INFO, TAG, PCF("Entering OCStop"));

    if (stackState != OC_STACK_INITIALIZED) {
        OC_LOG(ERROR, TAG, PCF("Stack not initialized"));
        return OC_STACK_ERROR;
    }

    // Make call to OCCoAP layer
    if (OCStopCoAP() == 0) {
        // Remove all the client callbacks
        DeleteClientCBList();
        stackState = OC_STACK_UNINITIALIZED;
        return OC_STACK_OK;
    }

    OC_LOG(ERROR, TAG, PCF("Stack stop error"));
    return OC_STACK_ERROR;
}

/**
 * Discover OC resources.
 *
 * @param method             - method to perform on the resource
 * @param requiredUri        - URI of the resource to interact with
 * @param referenceUri       - URI of the reference resource
 * @param request            - JSON encoded request
 * @param qos                - quality of service
 * @param asyncApplicationCB - asynchronous callback function that is invoked
 *                             by the stack when discovery or resource interaction is complete
 *
 * @return
 *     OC_STACK_OK               - no errors
 *     OC_STACK_INVALID_CALLBACK - invalid callback function pointer
 *     OC_STACK_INVALID_METHOD   - invalid resource method
 *     OC_STACK_INVALID_URI      - invalid required or reference URI
 */

OCStackResult OCDoResource(OCMethod method, const char *requiredUri,
                           const char *referenceUri, const char *request,
                           OCQualityOfService qos, OCCallbackData *cbData)
{
    OCToken * token;
    (void) referenceUri;

    OC_LOG(INFO, TAG, PCF("Entering OCDoResource"));

    // Validate input parameters
    VERIFY_NON_NULL(cbData, FATAL, OC_STACK_INVALID_CALLBACK);
    VERIFY_NON_NULL(cbData->cb, FATAL, OC_STACK_INVALID_CALLBACK);

    switch (method)
    {
        case OC_REST_GET:
        case OC_REST_PUT:
            break;
        default:
            return OC_STACK_INVALID_METHOD;
    }

    TODO ("Need to form the final query by concatenating require and reference URI's");
    // Validate required URI
    VERIFY_NON_NULL(requiredUri, FATAL, OC_STACK_INVALID_URI);

    // Generate token here, it will be deleted when the transaction is deleted
    token = (OCToken *) malloc(sizeof(OCToken));
    VERIFY_NON_NULL(token, FATAL, OC_STACK_NO_MEMORY);

    token->tokenLength = MAX_TOKEN_LENGTH;
    OCFillRandomMem(token->token, token->tokenLength);
    OC_LOG_BUFFER(INFO,TAG, token->token, token->tokenLength);

    // Make call to OCCoAP layer
    if (OCDoCoAPResource(method, qos, token, requiredUri, request) == OC_COAP_OK) {
        OC_LOG(INFO, TAG, "Done with this function");
        if (AddClientCB(cbData, token) == OC_STACK_OK)
        {
            return OC_STACK_OK;
        }
    }

    OC_LOG(ERROR, TAG, PCF("Stack stop error"));
    free(token);
    return OC_STACK_ERROR;
}

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
    OCProcessCoAP();

    return OC_STACK_OK;
}
/**
 * Create a resource
 *
 * @param handle - pointer to handle to newly created resource.  Set by ocstack.  Used to refer to resource
 * @param resourceTypeName - name of resource type.  Example: "core.led"
 * @param resourceAttributeRepresentation - attribute representation.  list of attributes:type, with each pair
 *                                          separated by semicolons.  Example:  "state:oc.bt.b;power:oc.bt.i"
 * @param resourceInterfaceName - name of resource interface.  Example: "core.rw"
 * @param allowedMethods - methods permitted on interface.  Example: OC_REST_GET|OC_REST_PUT
 * @param uri - URI of the resource.  Example:  "/a/led"
 * @param entityHandler - entity handler function that is called by ocstack to handle requests, etc
 * @param resourceProperties - properties supported by resource.  Example: OC_DISCOVERABLE|OC_OBSERVABLE
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCCreateResource(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceAttributeRepresentation,
        const char *resourceInterfaceName, uint8_t allowedMethods,
        const char *uri, OCEntityHandler entityHandler,
        uint8_t resourceProperties) {

    OCResource *pointer = NULL;
    char *str = NULL;
    size_t size;
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering OCCreateResource"));

    // Validate parameters
    // TODO:  Does resource attribute representation really have to be maintained in stack?
    // Is it presented during resource discovery?
    //    VERIFY_NON_NULL(resourceAttributeRepresentation, ERROR, OC_STACK_ERROR);
    if (!handle || !resourceTypeName || !resourceInterfaceName || !uri) {
        OC_LOG(ERROR, TAG, PCF("Input parameter is NULL"));
        return OC_STACK_INVALID_PARAM;
    }

    // Make sure allowedMethods bitmask has allowed methods specified
    if ((allowedMethods
            > (OC_REST_GET | OC_REST_PUT | OC_REST_POST | OC_REST_DELETE
                    | OC_REST_OBSERVE))
            || (allowedMethods == OC_REST_NOMETHOD)) {
        OC_LOG(ERROR, TAG, PCF("Invalid method"));
        return OC_STACK_INVALID_PARAM;
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
            resourceTypeName, resourceAttributeRepresentation);
    if (result != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, PCF("Error adding resourcetype"));
        goto exit;
    }

    // Add the resourceinterface to the resource
    result = OCBindResourceInterfaceToResource((OCResourceHandle) pointer,
            resourceInterfaceName, allowedMethods);
    if (result != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, PCF("Error adding resourceinterface"));
        goto exit;
    }

	// added [CL]
	result = OCBindResourceHandler((OCResourceHandle) pointer, entityHandler);
	if (result != OC_STACK_OK) {
		OC_LOG(ERROR, TAG, PCF("Error adding resourceinterface"));
		goto exit;
	}

    *handle = pointer;
    result = OC_STACK_OK;

    exit: if (result != OC_STACK_OK) {
        OCFree(pointer);
        OCFree(str);
    }
    return result;
}

/**
 * Add a resource to a container resource.
 *
 * @param containerHandle - handle to the container resource
 * @param addedResourceHandle - handle to resource to be added to the container resource
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCBindContainedResourceToResource(
        OCResourceHandle containerHandle, OCResourceHandle addedResourceHandle) {
    OCResource *resource;
    uint8_t i;

    OC_LOG(INFO, TAG, PCF("Entering OCBindContainedResourceToResource"));

    // Validate parameters
    VERIFY_NON_NULL(containerHandle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(addedResourceHandle, ERROR, OC_STACK_ERROR);
    // Container cannot contain itself
    if (containerHandle == addedResourceHandle) {
        OC_LOG(ERROR, TAG, PCF("Added handle equals container handle"));
        return OC_STACK_INVALID_PARAM;
    }

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *) containerHandle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_INVALID_PARAM;
    }

    // Look for an open slot to add add the child resource.
    // If found, add it and return success
    for (i = 0; i < MAX_CONTAINED_RESOURCES; i++) {
        if (!resource->rsrcResources[i]) {
            resource->rsrcResources[i] = (OCResourceHandle) addedResourceHandle;
            OC_LOG(INFO, TAG, PCF("resource bound"));
            return OC_STACK_OK;
        }
    }

    // Unable to add addedResourceHandle, so return error
    return OC_STACK_ERROR;
}

/**
 * Bind a resourcetype to a resource.
 *
 * @param handle - handle to the container resource
 * @param resourceTypeName - name of resource type.  Example: "core.led"
 * @param resourceAttributeRepresentation - attribute representation.  list of attributes:type, with each pair
 *                                          separated by semicolons.  Example:  "state:oc.bt.b;power:oc.bt.i"
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCBindResourceTypeToResource(OCResourceHandle handle,
        const char *resourceTypeName,
        const char *resourceAttributeRepresentation) {

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
    // VERIFY_NON_NULL(resourceAttributeRepresentation, ERROR, OC_STACK_ERROR);

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

    // If the resourceAttributeRepresentation is defined, add it.
    if (resourceAttributeRepresentation) {
        size = strlen(resourceAttributeRepresentation) + 1;
        str = (char *) OCMalloc(size);
        if (!str) {
            goto exit;
        }
        strncpy(str, resourceAttributeRepresentation, size);
        pointer->typerepresentation = str;
    }
    // Bind the resourcetype to the resource
    insertResourceType(resource, pointer);

    result = OC_STACK_OK;

    exit: if (result != OC_STACK_OK) {
        OCFree(pointer);
        OCFree(str);
    }
    return result;
}

/**
 * Bind a resourceinterface to a resource.
 *
 * @param handle - handle to the container resource
 * @param resourceInterfaceName - name of resource interface.  Example: "core.rw"
 * @param allowedMethods - methods permitted on interface.  Example: OC_REST_GET|OC_REST_PUT
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCBindResourceInterfaceToResource(OCResourceHandle handle,
        const char *resourceInterfaceName, uint8_t allowedMethods) {

    OCResourceInterface *pointer = NULL;
    char *str = NULL;
    size_t size;
    OCResource *resource;
    OCStackResult result = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Entering OCBindResourceInterfaceToResource"));

    // Validate parameters
    VERIFY_NON_NULL(resourceInterfaceName, ERROR, OC_STACK_INVALID_PARAM);

    // Make sure allowedMethods bitmask has allowed methods specified
    if ((allowedMethods
            > (OC_REST_GET | OC_REST_PUT | OC_REST_POST | OC_REST_DELETE
                    | OC_REST_OBSERVE))
            || (allowedMethods == OC_REST_NOMETHOD)) {
        OC_LOG(ERROR, TAG, PCF("Invalid method"));
        return OC_STACK_INVALID_PARAM;
    }

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

    pointer->allowedMethods = allowedMethods;

    // Bind the resourceinterface to the resource
    insertResourceInterface(resource, pointer);

    result = OC_STACK_OK;
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
 * Get attributes of resource type of the resource.
 *
 * @param handle - handle of resource
 * @param index - index of resource, 0 to Count - 1
 *
 * @return
 *    resource type attributes - if resource found
 *    NULL - resource not found
 */
const char *OCGetResourceAttributeRepresentation(OCResourceHandle handle,
        uint8_t index) {
    OCResourceType *resourceType;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceAttributeRepresentation"));

    resourceType = findResourceTypeAtIndex(handle, index);
    if (resourceType) {
        return resourceType->typerepresentation;
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
 * Get methods of resource interface of the resource.
 *
 * @param handle - handle of resource
 * @param index - index of resource, 0 to Count - 1
 *
 * @return
 *    allowed methods - if resource found
 *    NULL - resource not found
 */
uint8_t OCGetResourceInterfaceAllowedMethods(OCResourceHandle handle,
        uint8_t index) {
    OCResourceInterface *resourceInterface;

    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceInterfaceAllowedMethods"));

    resourceInterface = findResourceInterfaceAtIndex(handle, index);
    if (resourceInterface) {
        return resourceInterface->allowedMethods;
    }
    return (uint8_t) 0;
}

/**
 * Get name of resource interface of the resource.
 *
 * @param containerHandle - handle of container resource
 * @param index - index of contained resource, 0 to Count - 1
 *
 * @return
 *    handle to contained resource - if resource found
 *    NULL - resource not found
 */
OCResourceHandle OCGetContainedResourceHandle(OCResourceHandle containerHandle,
        uint8_t index) {
    OCResource *resource;

    OC_LOG(INFO, TAG, PCF("Entering OCGetContainedResource"));

    if (index >= MAX_CONTAINED_RESOURCES) {
        return NULL;
    }

    resource = findResource((OCResource *) containerHandle);
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
    VERIFY_NON_NULL(entityHandler, ERROR, OC_STACK_INVALID_PARAM);

    // Use the handle to find the resource in the resource linked list
    resource = findResource(handle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_ERROR;
    }

    // Bind the handler
    resource->entityHandler = entityHandler;

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
    resource = findResource(handle);
    if (!resource) {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return NULL;
    }

    // Bind the handler
    return resource->entityHandler;
}

/**
 * Notify observers that an observed value has changed.
 *
 *   **NOTE: This API has NOT been finalized!!!**
 *
 * @param handle - handle of resource
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack not initialized
 */
OCStackResult OCNotifyObservers(OCResourceHandle handle) {

    TODO ("This API has NOT been finalized");

    OC_LOG(INFO, TAG, PCF("Entering OCNotifyObservers"));

    (void) handle;

    return OC_STACK_NOTIMPL;
}

//-----------------------------------------------------------------------------
// Private internal function definitions
//-----------------------------------------------------------------------------

/**
 * Initialize resource data structures, variables, etc.
 */
void initResources() {
    TODO ("Do we need to create a resource for /oc/core???");
    // Init resource vars
    headResource = NULL;
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
        OCFree(pointer->typerepresentation);
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

// TODO: Remove these!  Should be in an ocmalloc module
void *OCMalloc(size_t size) {
    TODO ("This should be moved into an ocmalloc dir and implemented as a separate OC module");
    return malloc(size);
}

void OCFree(void *ptr) {
    TODO ("This should be moved into an ocmalloc dir and implemented as a separate OC module");
    free(ptr);
}

