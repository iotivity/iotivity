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

#ifndef OCSTACK_H_
#define OCSTACK_H_

#include <netinet/ip.h>

#include <ocsocket.h>
#include <logger.h>
#include <ocrandom.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

//May want to refactor this in upcoming sprints. Don't want to expose to application layer that lower level stack is using CoAP.
#define OC_WELL_KNOWN_QUERY                  PCF("coap://224.0.1.187:5683/.well-known/core")
#define OC_EXPLICIT_DEVICE_DISCOVERY_URI     PCF("coap://224.0.1.187:5683/oc/core?rt=core.led")

#define USE_RANDOM_PORT (0)
#define MAX_TOKEN_LENGTH (8)
#define MAX_RESPONSE_LENGTH (128)
#define MAX_REQUEST_LENGTH (128)
#define MAX_URI_LENGTH (64)
#define MAX_QUERY_LENGTH (64)
#define MAX_CONTAINED_RESOURCES  (5)

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/**
 * Standard RESTful HTTP Methods
 */
typedef enum {
    OC_REST_NOMETHOD = 0,
    OC_REST_GET      = (1 << 0),  // Read
    OC_REST_PUT      = (1 << 1),  // Write
    OC_REST_POST     = (1 << 2),  // Update
    OC_REST_DELETE   = (1 << 3),  // Delete
    OC_REST_OBSERVE  = (1 << 4)   // Register observe request
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
    OC_STACK_NO_MEMORY,
    OC_STACK_COMM_ERROR,
    OC_STACK_INVALID_PARAM,
    OC_STACK_NOTIMPL,
    OC_STACK_NO_RESOURCE, /* resource not found*/
    OC_STACK_RESOURCE_ERROR, /*ex: not supported method or interface*/
    OC_STACK_SLOW_RESOURCE,
    OC_STACK_ERROR
} OCStackResult;

/**
 * Handle to an OCResource object owned by the OCStack.
 */
typedef void * OCResourceHandle;

typedef struct {
    uint8_t token[MAX_TOKEN_LENGTH];
    size_t tokenLength;
} OCToken;

/**
 * Incoming requests handled by the server. Requests are passed in as a parameter to the @ref OCEntityHandler callback API.
 * @brief The @ref OCEntityHandler callback API must be implemented in the application in order to receive these requests.
 */
typedef struct {
    // Associated resource
    OCResourceHandle resource;
    // the REST method retrieved from received request PDU
    OCMethod method;
    // reqJSON is retrieved from the payload of the received request PDU
    unsigned const char * reqJSONPayload;
    // token is copied from the received request PDU
    OCToken * token;
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
    // reqJSON is retrieved from the payload of the received request PDU
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
typedef OCStackApplicationResult (* OCClientApplicationCBType)(void *ctx, OCClientResponse * clientResponse);

typedef struct {
    void *context;
    OCClientApplicationCBType cb;
} OCCallbackData;

/**
 * Application server implementations must implement this callback to consume requests OTA.
 */
typedef void (*OCEntityHandler) (OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest);


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
 * Discover OC resources.
 *
 * @param method             - method to perform on the resource
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
OCStackResult OCDoResource(OCMethod method, const char  *requiredUri, const char  *referenceUri,
//                const char *request, OCQualityOfService qos, OCClientApplicationCBType clientApplicationCB);
                const char *request, OCQualityOfService qos, OCCallbackData *cbData);



/**
 * Create a resource.
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
                               const char *resourceInterfaceName,
                               uint8_t allowedMethods,
                               const char *uri,
                               OCEntityHandler entityHandler,
                               uint8_t resourceProperties);

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
OCStackResult OCBindContainedResourceToResource(OCResourceHandle containerHandle, OCResourceHandle addedResourceHandle);

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
                                           const char *resourceAttributeRepresentation);
/**
 * Bind a resource interface to a resource.
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
                                                const char *resourceInterfaceName,
                                                uint8_t allowedMethods);

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
 * Get attributes of resource type of the resource.
 *
 * @param handle - handle of resource
 * @param index - index of resource, 0 to Count - 1
 *
 * @return
 *    resource type attributes - if resource found
 *    NULL - resource not found
 */
const char *OCGetResourceAttributeRepresentation(OCResourceHandle handle, uint8_t index);

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
 * Get name of resource interface of the resource.
 *
 * @param containerHandle - handle of container resource
 * @param index - index of contained resource, 0 to Count - 1
 *
 * @return
 *    handle to contained resource - if resource found
 *    NULL - resource not found
 */
OCResourceHandle OCGetContainedResourceHandle(OCResourceHandle containerHandle, uint8_t index);

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
