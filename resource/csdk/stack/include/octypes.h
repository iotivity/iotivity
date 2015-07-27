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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#ifdef __cplusplus
#include <string.h>

extern "C" {
#endif // __cplusplus
#define WITH_PRESENCE
#include "ocpresence.h"
//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

/**
 * OIC Virtual resources supported by every OIC device.
 */
#define OC_RSRVD_WELL_KNOWN_URI               "/oic/res"
#define OC_RSRVD_DEVICE_URI                   "/oic/d"
#define OC_RSRVD_PLATFORM_URI                 "/oic/p"
#define OC_RSRVD_RESOURCE_TYPES_URI           "/oic/res/types/d"
#ifdef WITH_PRESENCE
#define OC_RSRVD_PRESENCE_URI                 "/oic/ad"
#define OC_DEFAULT_PRESENCE_TTL_SECONDS (60)
/// OC_MAX_PRESENCE_TTL_SECONDS sets the maximum time to live (TTL) for presence.
/// NOTE: Changing the setting to a longer duration may lead to unsupported and untested
/// operation.
#define OC_MAX_PRESENCE_TTL_SECONDS     (60 * 60 * 24) // 60 sec/min * 60 min/hr * 24 hr/day
#endif

///Separtor for multiple query string
#define OC_QUERY_SEPARATOR                "&;"

/**
 * Attributes used to form a proper OIC conforming JSON message.
 */
//#define OC_RSRVD_OC                     "oic"
#define OC_RSRVD_PAYLOAD                "payload"
#define OC_RSRVD_HREF                   "href"
#define OC_RSRVD_PROPERTY               "prop"
#define OC_RSRVD_REPRESENTATION         "rep"
#define OC_RSRVD_CONTENT_TYPE           "ct"
#define OC_RSRVD_RESOURCE_TYPE          "rt"
#define OC_RSRVD_RESOURCE_TYPE_PRESENCE "oic.wk.ad"
#define OC_RSRVD_INTERFACE              "if"
#define OC_RSRVD_TTL                    "ttl"
#define OC_RSRVD_NONCE                  "non"
#define OC_RSRVD_TRIGGER                "trg"
#define OC_RSRVD_LINKS                  "links"

#define OC_RSRVD_INTERFACE_DEFAULT      "oic.if.baseline"
#define OC_RSRVD_INTERFACE_LL           "oic.if.ll"
#define OC_RSRVD_INTERFACE_BATCH        "oic.if.b"
#define OC_RSRVD_INTERFACE_GROUP        "oic.mi.grp"
#define OC_RSRVD_MFG_DATE               "mndt"
#define OC_RSRVD_FW_VERSION             "mnfv"
#define OC_RSRVD_HOST_NAME              "hn"
#define OC_RSRVD_VERSION                "icv"
#define OC_RSRVD_POLICY                 "p"
#define OC_RSRVD_BITMAP                 "bm"
#define OC_RSRVD_SECURE                 "sec"
#define OC_RSRVD_HOSTING_PORT           "port"
#define OC_RSRVD_SERVER_INSTANCE_ID     "sid"

//**** Presence "Announcement Triggers" ****
#define OC_RSRVD_TRIGGER_CREATE         "create"
#define OC_RSRVD_TRIGGER_CHANGE         "change"
#define OC_RSRVD_TRIGGER_DELETE         "delete"
//*******************

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

//**** Device ****
#define OC_RSRVD_DEVICE_ID              "di"
#define OC_RSRVD_DEVICE_NAME            "n"
#define OC_RSRVD_SPEC_VERSION           "lcv"
#define OC_RSRVD_DATA_MODEL_VERSION     "dmv"

#define OC_SPEC_VERSION                "0.9.0"
#define OC_DATA_MODEL_VERSION          "sec.0.95"
//*******************

// These provide backward compatibility - their use is deprecated
#ifndef GOING_AWAY
#define OC_MULTICAST_PREFIX                  "224.0.1.187:5683"
#define OC_MULTICAST_IP                      "224.0.1.187"
#define OC_MULTICAST_PORT                    5683
#endif // GOING_AWAY

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
#ifdef RA_ADAPTER
#define MAX_ADDR_STR_SIZE (256)
#else
#define MAX_ADDR_STR_SIZE (40)
 #endif

#define MAX_IDENTITY_SIZE (32)


/*
 * These enums (OCTransportAdapter and OCTransportFlags) must
 * be kept synchronized with OCConnectivityType (below) as well as
 * CATransportAdapter and CATransportFlags (in CACommon.h).
 */

typedef enum
{
    OC_DEFAULT_ADAPTER = 0,

    // value zero indicates discovery
    OC_ADAPTER_IP            = (1 << 0),   // IPv4 and IPv6, including 6LoWPAN
    OC_ADAPTER_GATT_BTLE     = (1 << 1),   // GATT over Bluetooth LE
    OC_ADAPTER_RFCOMM_BTEDR  = (1 << 2),   // RFCOMM over Bluetooth EDR

    #ifdef RA_ADAPTER
    OC_ADAPTER_REMOTE_ACCESS = (1 << 3)    // Remote Access over XMPP.
    #endif
} OCTransportAdapter;

// enum layout assumes some targets have 16-bit integer (e.g., Arduino)
typedef enum
{
    OC_DEFAULT_FLAGS = 0,

    // Insecure transport is the default (subject to change)
    OC_FLAG_SECURE     = (1 << 4),   // secure the transport path

    // IPv4 & IPv6 autoselection is the default
    OC_IP_USE_V6       = (1 << 5),   // IP adapter only
    OC_IP_USE_V4       = (1 << 6),   // IP adapter only

    OC_RESERVED1       = (1 << 7),   // internal use only

    // Link-Local multicast is the default multicast scope for IPv6.
    // These are placed here to correspond to the IPv6 multicast address bits.
    OC_SCOPE_INTERFACE = 0x1, // IPv6 Interface-Local scope (loopback)
    OC_SCOPE_LINK      = 0x2, // IPv6 Link-Local scope (default)
    OC_SCOPE_REALM     = 0x3, // IPv6 Realm-Local scope
    OC_SCOPE_ADMIN     = 0x4, // IPv6 Admin-Local scope
    OC_SCOPE_SITE      = 0x5, // IPv6 Site-Local scope
    OC_SCOPE_ORG       = 0x8, // IPv6 Organization-Local scope
    OC_SCOPE_GLOBAL    = 0xE, // IPv6 Global scope
} OCTransportFlags;

#define OC_MASK_SCOPE    (0x000F)
#define OC_MASK_MODS     (0x0FF0)
#define OC_MASK_FAMS     (OC_IP_USE_V6|OC_IP_USE_V4)

/*
 * endpoint identity
 */
typedef struct
{
    uint16_t id_length;
    unsigned char id[MAX_IDENTITY_SIZE];
} OCIdentity;

/**
 * Data structure to encapsulate IPv4/IPv6/Contiki/lwIP device addresses.
 *
 * OCDevAddr must be the same as CAEndpoint (in CACommon.h).
 */
typedef struct
{
    OCTransportAdapter      adapter;    // adapter type
    OCTransportFlags        flags;      // transport modifiers
    char                    addr[MAX_ADDR_STR_SIZE]; // address for all adapters
    uint32_t                interface;  // usually zero for default interface
    uint16_t                port;       // for IP
    OCIdentity              identity;   // secure node identity
} OCDevAddr;

/*
 * OCConnectivityType includes elements of both OCTransportAdapter
 * and OCTransportFlags. It is defined conditionally because the
 * smaller definition limits expandability on 32/64 bit integer machines,
 * and the larger definition won't fit into an enum on 16-bit integer
 * machines like Arduino.
 *
 * This structure must directly correspond to OCTransportAdapter
 * and OCTransportFlags.
 */
typedef enum
{
    CT_DEFAULT = 0,                // use when defaults are ok

    #if defined (__UINT32_MAX__) && (__UINT32_MAX__ == 65535) // 16-bit int
    CT_ADAPTER_IP           = (1 << 10),  // IPv4 and IPv6, including 6LoWPAN
    CT_ADAPTER_GATT_BTLE    = (1 << 11),  // GATT over Bluetooth LE
    CT_ADAPTER_RFCOMM_BTEDR = (1 << 12),  // RFCOMM over Bluetooth EDR
    #ifdef RA_ADAPTER
    CT_ADAPTER_REMOTE_ACCESS = (1 << 13),  // Remote Access over XMPP
    #endif
    #define CT_ADAPTER_SHIFT 10
    #define CT_MASK_FLAGS 0x03FF
    #define CT_MASK_ADAPTER 0xFC00
    #else   // assume 32-bit int
    CT_ADAPTER_IP           = (1 << 16),  // IPv4 and IPv6, including 6LoWPAN
    CT_ADAPTER_GATT_BTLE    = (1 << 17),  // GATT over Bluetooth LE
    CT_ADAPTER_RFCOMM_BTEDR = (1 << 18),  // RFCOMM over Bluetooth EDR
    #ifdef RA_ADAPTER
    CT_ADAPTER_REMOTE_ACCESS = (1 << 19),  // Remote Access over XMPP
    #endif
    #define CT_ADAPTER_SHIFT 16
    #define CT_MASK_FLAGS 0xFFFF
    #define CT_MASK_ADAPTER 0xFFFF0000
    #endif

    // Insecure transport is the default (subject to change)
    CT_FLAG_SECURE     = (1 << 4), // secure the transport path

    // IPv4 & IPv6 autoselection is the default
    CT_IP_USE_V6       = (1 << 5), // IP adapter only
    CT_IP_USE_V4       = (1 << 6), // IP adapter only

    // Link-Local multicast is the default multicast scope for IPv6.
    // These are placed here to correspond to the IPv6 address bits.
    CT_SCOPE_INTERFACE = 0x1, // IPv6 Interface-Local scope (loopback)
    CT_SCOPE_LINK      = 0x2, // IPv6 Link-Local scope (default)
    CT_SCOPE_REALM     = 0x3, // IPv6 Realm-Local scope
    CT_SCOPE_ADMIN     = 0x4, // IPv6 Admin-Local scope
    CT_SCOPE_SITE      = 0x5, // IPv6 Site-Local scope
    CT_SCOPE_ORG       = 0x8, // IPv6 Organization-Local scope
    CT_SCOPE_GLOBAL    = 0xE, // IPv6 Global scope
} OCConnectivityType;

/**
 *  OCDoResource methods
 */
typedef enum
{
    OC_REST_NOMETHOD       = 0,
    OC_REST_GET            = (1 << 0),     ///< Read
    OC_REST_PUT            = (1 << 1),     ///< Write
    OC_REST_POST           = (1 << 2),     ///< Update
    OC_REST_DELETE         = (1 << 3),     ///< Delete
    /// Register observe request for most up date notifications ONLY.
    OC_REST_OBSERVE        = (1 << 4),
    /// Register observe request for all notifications, including stale notifications.
    OC_REST_OBSERVE_ALL    = (1 << 5),
    /// Deregister observation, intended for internal use
    OC_REST_CANCEL_OBSERVE = (1 << 6),
    #ifdef WITH_PRESENCE
    /// Subscribe for all presence notifications of a particular resource.
    OC_REST_PRESENCE       = (1 << 7),
    #endif
    /// Allows OCDoResource caller to do discovery.
    OC_REST_DISCOVER       = (1 << 8)
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
 * The value of a policy property is defined as bitmap.
 * The LSB represents OC_DISCOVERABLE and Second LSB bit represents OC_OBSERVABLE and so on.
 * Not including the policy property is equivalent to zero.
 *
 * ::OC_RES_PROP_NONE When none of the bits are set, the resource is non-discoverable &
 *                    non-observable by the client.
 * ::OC_DISCOVERABLE When this bit is set, the resource is allowed to be discovered by clients.
 * ::OC_OBSERVABLE   When this bit is set, the resource is allowed to be observed by clients.
 * ::OC_ACTIVE        When this bit is set, the resource is initialized, otherwise the resource
 *                    is 'inactive'. 'inactive' signifies that the resource has been marked for
 *                    deletion or is already deleted.
 * ::OC_SLOW          When this bit is set, the resource has been marked as 'slow'. 'slow'
 *                    signifies that responses from this resource can expect delays in
 *                    processing its requests from clients.
 * ::OC_SECURE        When this bit is set, the resource is a secure resource.
 * ::OC_EXPLICIT_DISCOVERABLE   When this bit is set, the resource is allowed to be discovered only
 *                              if discovery request contains an explicit querystring.
 *                              Ex: GET /oic/res?rt=oic.sec.acl
 */
typedef enum
{
    OC_RES_PROP_NONE = (0),
    OC_DISCOVERABLE  = (1 << 0),
    OC_OBSERVABLE    = (1 << 1),
    OC_ACTIVE        = (1 << 2),
    OC_SLOW          = (1 << 3),
    OC_SECURE        = (1 << 4),
    OC_EXPLICIT_DISCOVERABLE   = (1 << 5)
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
    OC_STACK_UNAUTHORIZED_REQ,          /**< Request is not authorized by Resource Server. */
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


/**
 * Persistent storage handlers. An app must provide OCPersistentStorage handler pointers when it
 * calls OCRegisterPersistentStorageHandler.
 */
typedef struct {
    /*
     *  Persistent storage open handler points to default file path.
     *  Application can point to appropriate SVR database path for its Iotivity Server.
     */
    FILE* (* open)(const char *path, const char *mode);
    // Persistent storage read handler
    size_t (* read)(void *ptr, size_t size, size_t nmemb, FILE *stream);
    // Persistent storage write handler
    size_t (* write)(const void *ptr, size_t size, size_t nmemb, FILE *stream);
    // Persistent storage close handler
    int (* close)(FILE *fp);
    // Persistent storage unlink handler
    int (* unlink)(const char *path);
} OCPersistentStorage;

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

#ifdef __cplusplus
    OCHeaderOption() = default;
    OCHeaderOption(OCTransportProtocolID pid,
                   uint16_t optId,
                   uint16_t optlen,
                   const uint8_t* optData)
        : protocolID(pid),
          optionID(optId),
          optionLength(optlen)
    {

        // parameter includes the null terminator.
        optionLength = optionLength < MAX_HEADER_OPTION_DATA_LENGTH ?
                        optionLength : MAX_HEADER_OPTION_DATA_LENGTH;
        memcpy(optionData, optData, optionLength);
        optionData[optionLength - 1] = '\0';
    }
#endif
} OCHeaderOption;

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

#ifdef RA_ADAPTER
/**
 * @brief CA Remote Access information for XMPP Client
 *
 */
typedef struct
{
    char *hostname;     /**< XMPP server hostname */
    uint16_t   port;    /**< XMPP server serivce port */
    char *xmpp_domain;  /**< XMPP login domain */
    char *username;     /**< login username */
    char *password;     /**< login password */
    char *resource;     /**< specific resource for login */
    char *user_jid;     /**< specific JID for login */
} OCRAInfo_t;
#endif  /* RA_ADAPTER */

/**
 * This structure is expected as input for device properties.
 * device name is mandatory and expected from the application.
 * device id of type UUID will be generated by the stack.
 */
typedef struct
{
    char *deviceName;

} OCDeviceInfo;

// Enum to describe the type of object held by the OCPayload object
typedef enum
{
    PAYLOAD_TYPE_INVALID,
    PAYLOAD_TYPE_DISCOVERY,
    PAYLOAD_TYPE_DEVICE,
    PAYLOAD_TYPE_PLATFORM,
    PAYLOAD_TYPE_REPRESENTATION,
    PAYLOAD_TYPE_SECURITY,
    PAYLOAD_TYPE_PRESENCE
} OCPayloadType;

typedef struct
{
    // The type of message that was received
    OCPayloadType type;
} OCPayload;

typedef enum
{
    OCREP_PROP_NULL,
    OCREP_PROP_INT,
    OCREP_PROP_DOUBLE,
    OCREP_PROP_BOOL,
    OCREP_PROP_STRING,
    OCREP_PROP_OBJECT,
    OCREP_PROP_ARRAY
}OCRepPayloadPropType;

#define MAX_REP_ARRAY_DEPTH 3
typedef struct
{
    OCRepPayloadPropType type;
    size_t dimensions[MAX_REP_ARRAY_DEPTH];

    union
    {
        int64_t* iArray;
        double* dArray;
        bool* bArray;
        char** strArray;
        struct OCRepPayload** objArray;
    };
} OCRepPayloadValueArray;

typedef struct OCRepPayloadValue
{
    char* name;
    OCRepPayloadPropType type;
    union
    {
        int64_t i;
        double d;
        bool b;
        char* str;
        struct OCRepPayload* obj;
        OCRepPayloadValueArray arr;
    };
    struct OCRepPayloadValue* next;

} OCRepPayloadValue;

typedef struct OCStringLL
{
    struct OCStringLL *next;
    char* value;
} OCStringLL;

// used for get/set/put/observe/etc representations
typedef struct OCRepPayload
{
    OCPayload base;
    char* uri;
    OCStringLL* types;
    OCStringLL* interfaces;
    OCRepPayloadValue* values;
    struct OCRepPayload* next;
} OCRepPayload;

// used inside a discovery payload
typedef struct OCResourcePayload
{
    char* uri;
    uint8_t* sid;
    OCStringLL* types;
    OCStringLL* interfaces;
    uint8_t bitmap;
    bool secure;
    uint16_t port;
    struct OCResourcePayload* next;
} OCResourcePayload;

typedef struct
{
    OCPayload base;
    OCResourcePayload* resources;
} OCDiscoveryPayload;

typedef struct
{
    OCPayload base;
    char* uri;
    uint8_t* sid;
    char* deviceName;
    char* specVersion;
    char* dataModelVersion;
} OCDevicePayload;

typedef struct
{
    OCPayload base;
    char* uri;
    OCPlatformInfo info;
} OCPlatformPayload;

typedef struct
{
    OCPayload base;
    char* securityData;
} OCSecurityPayload;
#ifdef WITH_PRESENCE
typedef struct
{
    OCPayload base;
    uint32_t sequenceNumber;
    uint32_t maxAge;
    OCPresenceTrigger trigger;
    char* resourceType;
} OCPresencePayload;
#endif

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
    // description of endpoint that sent the request
    OCDevAddr devAddr;
    // resource query send by client
    char * query;
    // Information associated with observation - valid only when OCEntityHandler
    // flag includes OC_OBSERVE_FLAG
    OCObservationInfo obsInfo;
    // An array of the received vendor specific header options
    uint8_t numRcvdVendorSpecificHeaderOptions;
    OCHeaderOption * rcvdVendorSpecificHeaderOptions;
    // the payload from the request PDU
    OCPayload *payload;
} OCEntityHandlerRequest;

/**
 * Response from queries to remote servers. Queries are made by calling the @ref OCDoResource API.
 */
typedef struct
{
    /// Address of remote server
    OCDevAddr devAddr;
    OCDevAddr *addr;            // backward compatibility (points to devAddr)
    OCConnectivityType connType;  // backward compatibility
    /// the is the result of our stack, OCStackResult should contain coap/other error codes;
    OCStackResult result;
    /// If associated with observe, this will represent the sequence of notifications from server.
    uint32_t sequenceNumber;
    /// resourceURI
    const char * resourceUri;
    // the payload for the response PDU
    OCPayload *payload;
    /// An array of the received vendor specific header options
    uint8_t numRcvdVendorSpecificHeaderOptions;
    OCHeaderOption rcvdVendorSpecificHeaderOptions[MAX_HEADER_OPTIONS];
} OCClientResponse;

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
    OCPayload* payload;
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
typedef struct OCCallbackData
{
    void *context;
    /// The pointer to a function the stack will call to handle the requests
    OCClientResponseHandler cb;
    /// A pointer to a function to delete the context when this callback is removed
    OCClientContextDeleter cd;
#ifdef __cplusplus
    OCCallbackData() = default;
    OCCallbackData(void* ctx, OCClientResponseHandler callback, OCClientContextDeleter deleter)
        :context(ctx), cb(callback), cd(deleter){}
#endif
} OCCallbackData;

/**
 * Application server implementations must implement this callback to consume requests OTA.
 * Entity handler callback needs to fill the resPayload of the entityHandlerRequest.
 */
typedef OCEntityHandlerResult (*OCEntityHandler)
(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest, void* callbackParam);

/**
 * Device Entity handler need to use this call back instead of OCEntityHandler.
 */
typedef OCEntityHandlerResult (*OCDeviceEntityHandler)
(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest, char* uri, void* callbackParam);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* OCTYPES_H_ */
