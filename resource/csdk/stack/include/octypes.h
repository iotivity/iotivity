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

#ifndef OCTYPES_H_
#define OCTYPES_H_

#include "ocstackconfig.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#define WITH_PRESENCE
//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

//TODO: May want to refactor this in upcoming sprints.
//Don't want to expose to application layer that lower level stack is using CoAP.

/// Authority + URI string to prefix well known queries
#define OC_WELL_KNOWN_QUERY                  "224.0.1.187:5683/oc/core"
#define OC_MULTICAST_DISCOVERY_URI           "/oc/core"
#define OC_EXPLICIT_DEVICE_DISCOVERY_URI     "224.0.1.187:5683/oc/core/d?rt=core.led"
/// Multicast address and port string to prefix multicast queries
#define OC_MULTICAST_PREFIX                  "224.0.1.187:5683"
/// IP Multicast address to use for multicast requests
#define OC_MULTICAST_IP                      "224.0.1.187"
/// IP Multicast port to use for multicast requests
#define OC_MULTICAST_PORT                    5683

#ifdef WITH_PRESENCE
#define OC_DEFAULT_PRESENCE_TTL_SECONDS (60)
/// OC_MAX_PRESENCE_TTL_SECONDS sets the maximum time to live (TTL) for presence.
/// NOTE: Changing the setting to a longer duration may lead to unsupported and untested
/// operation.
#define OC_MAX_PRESENCE_TTL_SECONDS     (60 * 60 * 24) // 60 sec/min * 60 min/hr * 24 hr/day
#define OC_PRESENCE_URI                      "/oic/ad"
#endif
/**
 * Attributes used to form a proper OIC conforming JSON message.
 */
#define OC_RSRVD_OC                     "oic"
#define OC_RSRVD_PAYLOAD                "payload"
#define OC_RSRVD_HREF                   "href"
#define OC_RSRVD_PROPERTY               "prop"
#define OC_RSRVD_REPRESENTATION         "rep"
#define OC_RSRVD_CONTENT_TYPE           "ct"
#define OC_RSRVD_RESOURCE_TYPE          "rt"
#define OC_RSRVD_RESOURCE_TYPE_PRESENCE "oic.wk.ad"
#define OC_RSRVD_INTERFACE              "if"
#define OC_RSRVD_DEVICE_ID              "di"
#define OC_RSRVD_DEVICE_NAME            "dn"
#define OC_RSRVD_INTERFACE_DEFAULT      "oic.if.baseline"
#define OC_RSRVD_INTERFACE_LL           "oic.if.ll"
#define OC_RSRVD_INTERFACE_BATCH        "oic.if.b"
#define OC_RSRVD_INTERFACE_GROUP        "oc.mi.grp"
#define OC_RSRVD_FW_VERSION             "mnfv"
#define OC_RSRVD_HOST_NAME              "hn"
#define OC_RSRVD_VERSION                "icv"
#define OC_RSRVD_OBSERVABLE             "obs"
#define OC_RSRVD_SECURE                 "sec"
#define OC_RSRVD_HOSTING_PORT           "port"
#define OC_RSRVD_SERVER_INSTANCE_ID     "sid"

  //**** Platform ****
#define OC_RSRVD_PLATFORM_ID            "pi"
#define OC_RSRVD_MFG_NAME               "mnmn"
#define OC_RSRVD_MFG_URL                "mnml"
#define OC_RSRVD_MODEL_NUM              "mnmo"
#define OC_RSRVD_MFG_DATE               "mndt"
#define OC_RSRVD_PLATFORM_VERSION       "mnpv"
#define OC_RSRVD_OS_VERSION             "mnos"
#define OC_RSRVD_HARDWARE_VERSION       "mnhw"
#define OC_RSRVD_FIRMWARE_VERSION       "mnfv"
#define OC_RSRVD_SUPPORT_URL            "mnsl"
#define OC_RSRVD_SYSTEM_TIME             "st"
//*******************

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/**
 * Data structure to encapsulate IPv4/IPv6/Contiki/lwIP device addresses.
 */
typedef struct OCDevAddr
{
    uint32_t     size;                    ///< length of the address stored in addr field.
    uint8_t      addr[DEV_ADDR_SIZE_MAX]; ///< device address.
} OCDevAddr;

/**
 * OC Virtual resources supported by every OC device.
 */
typedef enum
{
    OC_WELL_KNOWN_URI= 0,       ///< "/oc/core"
    OC_DEVICE_URI,              ///< "/oc/core/d"
    OC_PLATFORM_URI,            ///< "/oic/p"
    OC_RESOURCE_TYPES_URI,      ///< "/oc/core/d/type"
    #ifdef WITH_PRESENCE
    OC_PRESENCE,                ///< "/oic/ad"
    #endif
    OC_MAX_VIRTUAL_RESOURCES    ///<s Max items in the list
} OCVirtualResources;

/**
 * Standard RESTful HTTP Methods.
 */
typedef enum
{
    OC_REST_NOMETHOD    = 0,
    OC_REST_GET         = (1 << 0),     ///< Read
    OC_REST_PUT         = (1 << 1),     ///< Write
    OC_REST_POST        = (1 << 2),     ///< Update
    OC_REST_DELETE      = (1 << 3),     ///< Delete
    /// Register observe request for most up date notifications ONLY.
    OC_REST_OBSERVE     = (1 << 4),
    /// Register observe request for all notifications, including stale notifications.
    OC_REST_OBSERVE_ALL = (1 << 5),
    /// Deregister observation, intended for internal use
    OC_REST_CANCEL_OBSERVE = (1 << 6),
    #ifdef WITH_PRESENCE
    /// Subscribe for all presence notifications of a particular resource.
    OC_REST_PRESENCE    = (1 << 7)
    #endif
} OCMethod;

/**
 * Host Mode of Operation.
 */
typedef enum
{
    OC_CLIENT = 0,
    OC_SERVER,
    OC_CLIENT_SERVER
} OCMode;

/**
 * Quality of Service.
 *
 * OCQualityOfService attempts to abstract the guarantees provided by the underlying transport
 * protocol.  The precise definitions of each quality of service level depend on the
 * implementation.  In descriptions below are for the current implementation and may changed
 * over time.
 */
typedef enum
{
    /// Packet delivery is best effort
    OC_LOW_QOS = 0,
    /// Packet delivery is best effort
    OC_MEDIUM_QOS,
    /// Acknowledgements are used to confirm delivery
    OC_HIGH_QOS,
    /// No Quality is defined, let the stack decide
    OC_NA_QOS
} OCQualityOfService;

/**
 * Resource Properties.
 *
 * ::OC_ACTIVE       When this bit is set, the resource is initialized, otherwise the resource
 *                   is 'inactive'. 'inactive' signifies that the resource has been marked for
 *                   deletion or is already deleted.
 * ::OC_DISCOVERABLE When this bit is set, the resource is allowed to be discovered by clients.
 * ::OC_OBSERVABLE   When this bit is set, the resource is allowed to be observed by clients.
 * ::OC_SLOW         When this bit is set, the resource has been marked as 'slow'. 'slow' signifies
 *                   that responses from this resource can expect delays in processing its
 *                   requests from clients.
 * ::OC_SECURE       When this bit is set, the resource is a secure resource.
 */
typedef enum
{
    OC_ACTIVE       = (1 << 0),
    OC_DISCOVERABLE = (1 << 1),
    OC_OBSERVABLE   = (1 << 2),
    OC_SLOW         = (1 << 3),
    OC_SECURE       = (1 << 4)
} OCResourceProperty;

/**
 * Transport Protocol IDs.
 */
typedef enum
{
    OC_INVALID_ID   = (1 << 0),
    OC_COAP_ID      = (1 << 1)
} OCTransportProtocolID;

/**
 * Adaptor types.
 */
typedef enum
{
    OC_IPV4 = 0,
    OC_IPV6,
    OC_EDR,
    OC_LE,
    OC_ALL // Multicast message: send over all the interfaces.
} OCConnectivityType;

/**
 * Declares Stack Results & Errors.
 */
typedef enum
{
    /* Success status code - START HERE */
    OC_STACK_OK = 0,
    OC_STACK_RESOURCE_CREATED,
    OC_STACK_RESOURCE_DELETED,
    OC_STACK_CONTINUE,
    /* Success status code - END HERE */
    /* Error status code - START HERE */
    OC_STACK_INVALID_URI = 20,
    OC_STACK_INVALID_QUERY,
    OC_STACK_INVALID_IP,
    OC_STACK_INVALID_PORT,
    OC_STACK_INVALID_CALLBACK,
    OC_STACK_INVALID_METHOD,
    OC_STACK_INVALID_PARAM,
    OC_STACK_INVALID_OBSERVE_PARAM,
    OC_STACK_NO_MEMORY,
    OC_STACK_COMM_ERROR,
    OC_STACK_TIMEOUT,
    OC_STACK_ADAPTER_NOT_ENABLED,
    OC_STACK_NOTIMPL,
    OC_STACK_NO_RESOURCE,               /* resource not found */
    OC_STACK_RESOURCE_ERROR,            /* ex: not supported method or interface */
    OC_STACK_SLOW_RESOURCE,
    OC_STACK_DUPLICATE_REQUEST,
    OC_STACK_NO_OBSERVERS,              /* resource has no registered observers */
    OC_STACK_OBSERVER_NOT_FOUND,
    OC_STACK_VIRTUAL_DO_NOT_HANDLE,
    OC_STACK_INVALID_OPTION,
    OC_STACK_MALFORMED_RESPONSE,        /* the remote reply contained malformed data */
    OC_STACK_PERSISTENT_BUFFER_REQUIRED,
    OC_STACK_INVALID_REQUEST_HANDLE,
    OC_STACK_INVALID_DEVICE_INFO,
    OC_STACK_INVALID_JSON,
    /* NOTE: Insert all new error codes here!*/
    #ifdef WITH_PRESENCE
    OC_STACK_PRESENCE_STOPPED = 128,
    OC_STACK_PRESENCE_TIMEOUT,
    OC_STACK_PRESENCE_DO_NOT_HANDLE,
    #endif
    OC_STACK_ERROR = 255
    /* Error status code - END HERE */
} OCStackResult;

/**
 * Handle to an @ref OCDoResource invocation.
 */
typedef void * OCDoHandle;

/**
 * Handle to an OCResource object owned by the OCStack.
 */
typedef void * OCResourceHandle;

typedef void * OCRequestHandle;

/**
 * Unique identifier for each observation request. Used when observations are
 * registered or deregistering. Used by entity handler to signal specific
 * observers to be notified of resource changes.
 * There can be maximum of 256 observations per server.
 */
typedef uint8_t OCObservationId;

/**
 * Action associated with observation.
 */
typedef enum
{
    OC_OBSERVE_REGISTER = 0,
    OC_OBSERVE_DEREGISTER = 1,
    OC_OBSERVE_NO_OPTION = 2
} OCObserveAction;

typedef struct
{
    // Action associated with observation request
    OCObserveAction action;
    // Identifier for observation being registered/deregistered
    OCObservationId obsId;
} OCObservationInfo;

/**
 * Possible returned values from entity handler.
 */
typedef enum
{
    OC_EH_OK = 0,
    OC_EH_ERROR,
    OC_EH_RESOURCE_CREATED,
    OC_EH_RESOURCE_DELETED,
    OC_EH_SLOW,
    OC_EH_FORBIDDEN,
    OC_EH_RESOURCE_NOT_FOUND
} OCEntityHandlerResult;

/**
 * This structure will be used to define the vendor specific header options to be included
 * in communication packets.
 */
typedef struct OCHeaderOption
{
    // The protocol ID this option applies to
    OCTransportProtocolID protocolID;
    // The header option ID which will be added to communication packets
    uint16_t optionID;
    // its length   191
    uint16_t optionLength;
    // pointer to its data
    uint8_t optionData[MAX_HEADER_OPTION_DATA_LENGTH];
} OCHeaderOption;

/**
 * Incoming requests handled by the server. Requests are passed in as a parameter to the
 * @ref OCEntityHandler callback API.
 * @brief The @ref OCEntityHandler callback API must be implemented in the application in order
 * to receive these requests.
 */
typedef struct
{
    // Associated resource
    OCResourceHandle resource;
    OCRequestHandle requestHandle;
    // the REST method retrieved from received request PDU
    OCMethod method;
    // resource query send by client
    char * query;
    // Information associated with observation - valid only when OCEntityHandler
    // flag includes OC_OBSERVE_FLAG
    OCObservationInfo obsInfo;
    // An array of the received vendor specific header options
    uint8_t numRcvdVendorSpecificHeaderOptions;
    OCHeaderOption * rcvdVendorSpecificHeaderOptions;
    // reqJSON is retrieved from the payload of the received request PDU
    char * reqJSONPayload;
} OCEntityHandlerRequest;

/**
 * Response from queries to remote servers. Queries are made by calling the @ref OCDoResource API.
 */
typedef struct
{
    /// Address of remote server
    OCDevAddr * addr;
    /// Indicates adaptor type on which the response was received
    OCConnectivityType connType;
    /// the is the result of our stack, OCStackResult should contain coap/other error codes;
    OCStackResult result;
    /// If associated with observe, this will represent the sequence of notifications from server.
    uint32_t sequenceNumber;
    /// resJSONPayload is retrieved from the payload of the received request PDU
    const char * resJSONPayload;
    /// An array of the received vendor specific header options
    uint8_t numRcvdVendorSpecificHeaderOptions;
    OCHeaderOption rcvdVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];
} OCClientResponse;

/**
 * This structure describes the platform properties. All non-Null properties will be included
 * in a platform discovery request.
 */
typedef struct
{
    char *platformID;
    char *manufacturerName;
    char *manufacturerUrl;
    char *modelNumber;
    char *dateOfManufacture;
    char *platformVersion;
    char *operatingSystemVersion;
    char *hardwareVersion;
    char *firmwareVersion;
    char *supportUrl;
    char *systemTime;

} OCPlatformInfo;

/**
 * TODO : Modify these. This is just so sample apps compile.
 */
typedef struct
{
    char *deviceName;
    char *hostName;
    char *deviceUUID;
    char *contentType;
    char *version;
    char *manufacturerName;
    char *manufacturerUrl;
    char *modelNumber;
    char *dateOfManufacture;
    char *platformVersion;
    char *firmwareVersion;
    char *supportUrl;
} OCDeviceInfo;

typedef struct
{
    // Request handle is passed to server via the entity handler for each incoming request.
    // Stack assigns when request is received, server sets to indicate what request response is for
    OCRequestHandle requestHandle;
    // Resource handle
    OCResourceHandle resourceHandle;
    // Allow the entity handler to pass a result with the response
    OCEntityHandlerResult  ehResult;
    // this is the pointer to server payload data to be transferred
    char *payload;
    // size of server payload data.  I don't think we should rely on null terminated data for size
    uint16_t payloadSize;
    // An array of the vendor specific header options the entity handler wishes to use in response
    uint8_t numSendVendorSpecificHeaderOptions;
    OCHeaderOption sendVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];
    // URI of new resource that entity handler might create
    char resourceUri[MAX_URI_LENGTH];
    // Server sets to true for persistent response buffer, false for non-persistent response buffer
    uint8_t persistentBufferFlag;
} OCEntityHandlerResponse;

typedef enum
{
    OC_REQUEST_FLAG = (1 << 1),
    OC_OBSERVE_FLAG = (1 << 2)
} OCEntityHandlerFlag; //entity_handler_flag_t ;

/**
 * Possible returned values from client application.
 */
typedef enum
{
    OC_STACK_DELETE_TRANSACTION = 0,//!< OC_STACK_DELETE_TRANSACTION
    OC_STACK_KEEP_TRANSACTION       //!< OC_STACK_KEEP_TRANSACTION
} OCStackApplicationResult;

//-----------------------------------------------------------------------------
// Callback function definitions
//-----------------------------------------------------------------------------

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
typedef struct
{
    void *context;
    /// The pointer to a function the stack will call to handle the requests
    OCClientResponseHandler cb;
    /// A pointer to a function to delete the context when this callback is removed
    OCClientContextDeleter cd;
} OCCallbackData;

/**
 * Application server implementations must implement this callback to consume requests OTA.
 * Entity handler callback needs to fill the resPayload of the entityHandlerRequest.
 */
typedef OCEntityHandlerResult (*OCEntityHandler)
(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest);

/**
 * Device Entity handler need to use this call back instead of OCEntityHandler.
 */
typedef OCEntityHandlerResult (*OCDeviceEntityHandler)
(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest, char* uri);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* OCTYPES_H_ */
