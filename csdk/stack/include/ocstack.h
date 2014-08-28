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

#ifndef OCSTACK_H_
#define OCSTACK_H_

#include "ocsocket.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

//May want to refactor this in upcoming sprints. Don't want to expose to application layer that lower level stack is using CoAP.
#define OC_WELL_KNOWN_QUERY                  PCF("coap://224.0.1.187:5683/oc/core")
#define OC_EXPLICIT_DEVICE_DISCOVERY_URI     PCF("coap://224.0.1.187:5683/oc/core?rt=core.led")
#define OC_MULTICAST_PREFIX                  PCF("coap://224.0.1.187:5683")

#define USE_RANDOM_PORT (0)
//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/**
 * OC Virtual resources supported by every OC device
 */
typedef enum {
    OC_WELL_KNOWN_URI= 0,    // "/oc/core"
    OC_DEVICE_URI,           // "/oc/core/d"
    OC_RESOURCE_TYPES_URI,   // "/oc/core/d/type"
    OC_MAX_VIRTUAL_RESOURCES         // Max items in the list
} OCVirtualResources;

/**
 * Standard RESTful HTTP Methods
 */
typedef enum {
    OC_REST_NOMETHOD    = 0,
    OC_REST_GET         = (1 << 0),     // Read
    OC_REST_PUT         = (1 << 1),     // Write
    OC_REST_POST        = (1 << 2),     // Update
    OC_REST_DELETE      = (1 << 3),     // Delete
    OC_REST_OBSERVE     = (1 << 4),     // Register observe request for most up date notifications ONLY.
    OC_REST_OBSERVE_ALL = (1 << 5),     // Register observe request for all notifications, including stale notifications.
    OC_REST_PRESENCE    = (1 << 6)      // Subscribe for all presence notifications of a particular resource.
} OCMethod;

/**
 * Host Mode of Operation
 */
typedef enum {
    OC_CLIENT = 0,
    OC_SERVER,
    OC_CLIENT_SERVER
} OCMode;

/**
 * Quality of Service
 */
typedef enum {
    OC_NON_CONFIRMABLE = 0,
    OC_CONFIRMABLE
} OCQualityOfService;

/**
 * Resource Properties
 */
typedef enum {
    OC_ACTIVE       = (1 << 0),  // set == initialized; unset == deleted
    OC_DISCOVERABLE = (1 << 1),  // Discovery of this resource allowed
    OC_OBSERVABLE   = (1 << 2),  // Observe allowed
    OC_SLOW         = (1 << 3)   // Expect delay in processing requests. Send ACK to request and send response later
} OCResourceProperty;

/**
 * Declares Stack Results & Errors
 */
typedef enum {
    OC_STACK_OK = 0,
    OC_STACK_INVALID_URI,
    OC_STACK_INVALID_QUERY,
    OC_STACK_INVALID_IP,
    OC_STACK_INVALID_PORT,
    OC_STACK_INVALID_CALLBACK,
    OC_STACK_INVALID_METHOD,
    OC_STACK_INVALID_PARAM,
    OC_STACK_INVALID_OBSERVE_PARAM,
    OC_STACK_NO_MEMORY,
    OC_STACK_COMM_ERROR,
    OC_STACK_NOTIMPL,
    OC_STACK_NO_RESOURCE, /* resource not found*/
    OC_STACK_RESOURCE_ERROR, /*ex: not supported method or interface*/
    OC_STACK_SLOW_RESOURCE,
    OC_STACK_NO_OBSERVERS, /* resource has no registered observers */
    OC_STACK_OBSERVER_NOT_FOUND,
    OC_STACK_OBSERVER_NOT_ADDED,
    OC_STACK_OBSERVER_NOT_REMOVED,
    OC_STACK_ERROR
} OCStackResult;

/**
 * Handle to an @ref OCDoResource invocation.
 */
typedef void * OCDoHandle;

/**
 * Handle to an OCResource object owned by the OCStack.
 */
typedef void * OCResourceHandle;

/**
 * Incoming requests handled by the server. Requests are passed in as a parameter to the @ref OCEntityHandler callback API.
 * @brief The @ref OCEntityHandler callback API must be implemented in the application in order to receive these requests.
 */
typedef struct {
    // Associated resource
    OCResourceHandle resource;
    // resource query send by client
    unsigned char * query;
    // the REST method retrieved from received request PDU
    OCMethod method;
    // reqJSON is retrieved from the payload of the received request PDU
    unsigned const char * reqJSONPayload;
    // resJSON is allocated in the stack and will be used by entity handler to fill in its response
    unsigned char * resJSONPayload;
    // Length of maximum allowed response
    uint16_t resJSONPayloadLen;
}OCEntityHandlerRequest;

/**
 * Response from queries to remote servers. Queries are made by calling the @ref OCDoResource API.
 */
typedef struct {
    // the is the result of our stack, OCStackResult should contain coap/other error codes;
    OCStackResult result;
    // Address of remote server
    OCDevAddr * addr;
    // If associated with observe, this will represent the sequence of notifications from server.
    uint32_t sequenceNumber;
    // resJSONPayload is retrieved from the payload of the received request PDU
    unsigned  const char * resJSONPayload;
}OCClientResponse;

typedef enum {
    OC_INIT_FLAG    = (1 << 0),
    OC_REQUEST_FLAG = (1 << 1),
    OC_OBSERVE_FLAG = (1 << 2)
} OCEntityHandlerFlag; //entity_handler_flag_t ;

// possible returned values from client application
typedef enum {
    OC_STACK_DELETE_TRANSACTION = 0,
    OC_STACK_KEEP_TRANSACTION
} OCStackApplicationResult;

//-----------------------------------------------------------------------------
// Callback function definitions
//-----------------------------------------------------------------------------

/**
 * Client applications implement this callback to consume responses received from Servers.
 */
typedef OCStackApplicationResult (* OCClientResponseHandler)(void *context, OCDoHandle handle, OCClientResponse * clientResponse);


/*
 * This info is passed from application to OC Stack when initiating a request to Server
 */
typedef struct {
    void *context;
    OCClientResponseHandler cb;
} OCCallbackData;

/**
 * Possible returned values from entity handler
 */
typedef enum {
    OC_EH_OK = 0,
    OC_EH_ERROR
} OCEntityHandlerResult;

/**
 * Application server implementations must implement this callback to consume requests OTA.
 * Entity handler callback needs to fill the resPayload of the entityHandlerRequest.
 */
typedef OCEntityHandlerResult (*OCEntityHandler)
(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest);


//-----------------------------------------------------------------------------
// Function prototypes
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
OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode);

/**
 * Stop the OC stack.  Use for a controlled shutdown.
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack not initialized
 */
OCStackResult OCStop();

/**
 * Called in main loop of OC client or server.  Allows low-level processing of
 * stack services.
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCProcess();

/**
 * Discover or Perform requests on a specified resource (specified by that Resource's respective URI).
 *
 * @param handle             - @ref OCDoHandle to refer to the request sent out on behalf of calling this API.
 * @param method             - @ref OCMethod to perform on the resource
 * @param requiredUri        - URI of the resource to interact with
 * @param referenceUri       - URI of the reference resource
 * @param request            - JSON encoded request
 * @param qos                - quality of service
 * @param clientApplicationCB- asynchronous callback function that is invoked
 *                             by the stack when discovery or resource interaction is complete
 *
 * @return
 *     OC_STACK_OK               - no errors
 *     OC_STACK_INVALID_CALLBACK - invalid callback function pointer
 *     OC_STACK_INVALID_METHOD   - invalid resource method
 *     OC_STACK_INVALID_URI      - invalid required or reference URI
 */
OCStackResult OCDoResource(OCDoHandle *handle, OCMethod method, const char  *requiredUri, const char  *referenceUri,
                const char *request, OCQualityOfService qos, OCCallbackData *cbData);

/**
 * Cancel a request associated with a specific @ref OCDoResource invocation.
 *
 * @param handle - Used to identify a specific OCDoResource invocation.
 *
 * @return
 *     OC_STACK_OK               - No errors; Success
 *     OC_STACK_INVALID_PARAM    - The handle provided is invalid.
 */
OCStackResult OCCancel(OCDoHandle handle);

/**
 * Create a resource.
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
                               const char *uri,
                               OCEntityHandler entityHandler,
                               uint8_t resourceProperties);

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
OCStackResult OCBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle);

/**
 * Remove a resource from a collection resource.
 *
 * @param collectionHandle - handle to the collection resource
 * @param resourceHandle - handle to resource to be removed from the collection resource
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 *     OC_STACK_INVALID_PARAM - invalid collectionhandle
 */
OCStackResult OCUnBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle);

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
                                           const char *resourceTypeName);
/**
 * Bind a resource interface to a resource.
 *
 * @param handle - handle to the resource
 * @param resourceInterfaceName - name of resource interface.  Example: "core.rw"
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCBindResourceInterfaceToResource(OCResourceHandle handle,
                                                const char *resourceInterfaceName);

/**
 * Bind an entity handler to the resource.
 *
 * @param handle - handle to the resource that the contained resource is to be bound
 * @param entityHandler - entity handler function that is called by ocstack to handle requests, etc
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error
 */
OCStackResult OCBindResourceHandler(OCResourceHandle handle, OCEntityHandler entityHandler);

/**
 * Get the number of resources that have been created in the stack.
 *
 * @param numResources - pointer to count variable
 *
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_ERROR - stack process error

 */
OCStackResult OCGetNumberOfResources(uint8_t *numResources);

/**
 * Get a resource handle by index.
 *
 * @param index - index of resource, 0 to Count - 1
 *
 * @return
 *    Resource handle - if found
 *    NULL - if not found
 */
OCResourceHandle OCGetResourceHandle(uint8_t index);

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
OCStackResult OCDeleteResource(OCResourceHandle handle);

/**
 * Get the URI of the resource specified by handle.
 *
 * @param handle - handle of resource
 * @return
 *    URI string - if resource found
 *    NULL - resource not found
 */
const char *OCGetResourceUri(OCResourceHandle handle);

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
uint8_t OCGetResourceProperties(OCResourceHandle handle);

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
OCStackResult OCGetNumberOfResourceTypes(OCResourceHandle handle, uint8_t *numResourceTypes);

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
const char *OCGetResourceTypeName(OCResourceHandle handle, uint8_t index);

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
OCStackResult OCGetNumberOfResourceInterfaces(OCResourceHandle handle, uint8_t *numResourceInterfaces);

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
const char *OCGetResourceInterfaceName(OCResourceHandle handle, uint8_t index);

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
uint8_t OCGetResourceInterfaceAllowedMethods(OCResourceHandle handle, uint8_t index);

/**
 * Get resource handle from the collection resource by index.
 *
 * @param collectionHandle - handle of collection resource
 * @param index - index of contained resource, 0 to Count - 1
 *
 * @return
 *    handle to contained resource - if resource found
 *    NULL - resource not found
 */
OCResourceHandle OCGetResourceHandleFromCollection(OCResourceHandle collectionHandle, uint8_t index);

/**
 * Get the entity handler for a resource.
 *
 * @param handle - handle of resource
 *
 * @return
 *    entity handler - if resource found
 *    NULL - resource not found
 */
OCEntityHandler OCGetResourceHandler(OCResourceHandle handle);

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
OCStackResult OCNotifyObservers(OCResourceHandle handle);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* OCSTACK_H_ */
