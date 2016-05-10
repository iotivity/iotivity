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

/*
 * Enumerations and simple structures
 *
 * These are separate because they have no dependencies.
 */

#ifndef OC_ENUMS_H_
#define OC_ENUMS_H_

#include "occonstants.h"

/**
 * These enums (OCTransportAdapter and OCTransportFlags) must
 * be kept synchronized with OCConnectivityType (below) as well as
 * CATransportAdapter and CATransportFlags (in CACommon.h).
 */
typedef enum
{
    /** value zero indicates discovery.*/
    OC_DEFAULT_ADAPTER = 0,

    /** IPv4 and IPv6, including 6LoWPAN.*/
    OC_ADAPTER_IP           = (1 << 0),

    /** GATT over Bluetooth LE.*/
    OC_ADAPTER_GATT_BTLE    = (1 << 1),

    /** RFCOMM over Bluetooth EDR.*/
    OC_ADAPTER_RFCOMM_BTEDR = (1 << 2),

#ifdef RA_ADAPTER
    /**Remote Access over XMPP.*/
   OC_ADAPTER_REMOTE_ACCESS = (1 << 3)
#endif

} OCTransportAdapter;

/**
 *  Enum layout assumes some targets have 16-bit integer (e.g., Arduino).
 */
typedef enum
{
    /** default flag is 0*/
    OC_DEFAULT_FLAGS = 0,

    /** Insecure transport is the default (subject to change).*/
    /** secure the transport path*/
    OC_FLAG_SECURE     = (1 << 4),

    /** IPv4 & IPv6 auto-selection is the default.*/
    /** IP adapter only.*/
    OC_IP_USE_V6       = (1 << 5),

    /** IP adapter only.*/
    OC_IP_USE_V4       = (1 << 6),

    /** internal use only.*/
    OC_MULTICAST       = (1 << 7),   // internal use only

    /** Link-Local multicast is the default multicast scope for IPv6.
     *  These are placed here to correspond to the IPv6 multicast address bits.*/

    /** IPv6 Interface-Local scope (loopback).*/
    OC_SCOPE_INTERFACE = 0x1,

    /** IPv6 Link-Local scope (default).*/
    OC_SCOPE_LINK      = 0x2,

    /** IPv6 Realm-Local scope. */
    OC_SCOPE_REALM     = 0x3,

    /** IPv6 Admin-Local scope. */
    OC_SCOPE_ADMIN     = 0x4,

    /** IPv6 Site-Local scope. */
    OC_SCOPE_SITE      = 0x5,

    /** IPv6 Organization-Local scope. */
    OC_SCOPE_ORG       = 0x8,

    /**IPv6 Global scope. */
    OC_SCOPE_GLOBAL    = 0xE,

} OCTransportFlags;

/** Bit mask for scope.*/
#define OC_MASK_SCOPE    (0x000F)

/** Bit mask for Mods.*/
#define OC_MASK_MODS     (0x0FF0)
#define OC_IPFAMILY_MASK (OC_IP_USE_V6|OC_IP_USE_V4)

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

/**
 * This enum type includes elements of both ::OCTransportAdapter and ::OCTransportFlags.
 * It is defined conditionally because the smaller definition limits expandability on 32/64 bit
 * integer machines, and the larger definition won't fit into an enum on 16-bit integer machines
 * like Arduino.
 *
 * This structure must directly correspond to ::OCTransportAdapter and ::OCTransportFlags.
 */
typedef enum
{
    /** use when defaults are ok. */
    CT_DEFAULT = 0,

    /** IPv4 and IPv6, including 6LoWPAN.*/
    CT_ADAPTER_IP           = (1 << 16),

    /** GATT over Bluetooth LE.*/
    CT_ADAPTER_GATT_BTLE    = (1 << 17),

    /** RFCOMM over Bluetooth EDR.*/
    CT_ADAPTER_RFCOMM_BTEDR = (1 << 18),

    #ifdef RA_ADAPTER
    /** Remote Access over XMPP.*/
    CT_ADAPTER_REMOTE_ACCESS = (1 << 19),
    #endif

    /** Insecure transport is the default (subject to change).*/

    /** secure the transport path.*/
    CT_FLAG_SECURE     = (1 << 4),

    /** IPv4 & IPv6 autoselection is the default.*/

    /** IP adapter only.*/
    CT_IP_USE_V6       = (1 << 5),

    /** IP adapter only.*/
    CT_IP_USE_V4       = (1 << 6),

    /** Link-Local multicast is the default multicast scope for IPv6.
     * These are placed here to correspond to the IPv6 address bits.*/

    /** IPv6 Interface-Local scope(loopback).*/
    CT_SCOPE_INTERFACE = 0x1,

    /** IPv6 Link-Local scope (default).*/
    CT_SCOPE_LINK      = 0x2,

    /** IPv6 Realm-Local scope.*/
    CT_SCOPE_REALM     = 0x3,

    /** IPv6 Admin-Local scope.*/
    CT_SCOPE_ADMIN     = 0x4,

    /** IPv6 Site-Local scope.*/
    CT_SCOPE_SITE      = 0x5,

    /** IPv6 Organization-Local scope.*/
    CT_SCOPE_ORG       = 0x8,

    /** IPv6 Global scope.*/
    CT_SCOPE_GLOBAL    = 0xE,
} OCConnectivityType;

/** bit shift required for connectivity adapter.*/
#define CT_ADAPTER_SHIFT 16

/** Mask Flag.*/
#define CT_MASK_FLAGS 0xFFFF

/** Mask Adapter.*/
#define CT_MASK_ADAPTER 0xFFFF0000

/**
 *  OCDoResource methods to dispatch the request
 */
typedef enum
{
    OC_REST_NOMETHOD       = 0,

    /** Read.*/
    OC_REST_GET            = 1,

    /** Write.*/
    OC_REST_PUT            = 2,

    /** Update.*/
    OC_REST_POST           = 3,

    /** Delete.*/
    OC_REST_DELETE         = 4,

    /** Register observe request for most up date notifications ONLY.*/
    OC_REST_OBSERVE        = 5,

    /** Register observe request for all notifications, including stale notifications.*/
    OC_REST_OBSERVE_ALL    = 6,

    /** De-register observation, intended for internal use.*/
    OC_REST_CANCEL_OBSERVE = 7,

    /** Subscribe for all presence notifications of a particular resource.*/
    OC_REST_PRESENCE       = 8,

    /** Allows OCDoResource caller to do multicast discovery.*/
    OC_REST_DISCOVER       = 9
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
 * Quality of Service attempts to abstract the guarantees provided by the underlying transport
 * protocol. The precise definitions of each quality of service level depend on the
 * implementation. In descriptions below are for the current implementation and may changed
 * over time.
 */
typedef enum
{
    /** Packet delivery is best effort.*/
    OC_LOW_QOS = 0,

    /** Packet delivery is best effort.*/
    OC_MEDIUM_QOS,

    /** Acknowledgments are used to confirm delivery.*/
    OC_HIGH_QOS,

    /** No Quality is defined, let the stack decide.*/
    OC_NA_QOS
} OCQualityOfService;

/**
 * Resource Properties.
 * The value of a policy property is defined as bitmap.
 * The LSB represents OC_DISCOVERABLE and Second LSB bit represents OC_OBSERVABLE and so on.
 * Not including the policy property is equivalent to zero.
 *
 */
typedef enum
{
    /** When none of the bits are set, the resource is non-discoverable &
     *  non-observable by the client.*/
    OC_RES_PROP_NONE = (0),

    /** When this bit is set, the resource is allowed to be discovered by clients.*/
    OC_DISCOVERABLE  = (1 << 0),

    /** When this bit is set, the resource is allowed to be observed by clients.*/
    OC_OBSERVABLE    = (1 << 1),

    /** When this bit is set, the resource is initialized, otherwise the resource
     *  is 'inactive'. 'inactive' signifies that the resource has been marked for
     *  deletion or is already deleted.*/
    OC_ACTIVE        = (1 << 2),

    /** When this bit is set, the resource has been marked as 'slow'.
     * 'slow' signifies that responses from this resource can expect delays in
     *  processing its requests from clients.*/
    OC_SLOW          = (1 << 3),

    /** When this bit is set, the resource is a secure resource.*/
    OC_SECURE        = (1 << 4),

    /** When this bit is set, the resource is allowed to be discovered only
     *  if discovery request contains an explicit querystring.
     *  Ex: GET /oic/res?rt=oic.sec.acl */
    OC_EXPLICIT_DISCOVERABLE   = (1 << 5),

    OC_VALID_RESOURCE_PROPERTY = (1 << 6) - 1
} OCResourceProperty;

/**
 * Transport Protocol IDs.
 */
typedef enum
{
    /** For invalid ID.*/
    OC_INVALID_ID   = (1 << 0),

    /* For coap ID.*/
    OC_COAP_ID      = (1 << 1)
} OCTransportProtocolID;

/**
 * Declares Stack Results & Errors.
 */
typedef enum
{
    /** Success status code - START HERE.*/
    OC_STACK_OK = 0,
    OC_STACK_RESOURCE_CREATED,
    OC_STACK_RESOURCE_DELETED,
    OC_STACK_CONTINUE,
    /** Success status code - END HERE.*/

    /** Error status code - START HERE.*/
    OC_STACK_INVALID_URI = 20,
    OC_STACK_INVALID_QUERY,
    OC_STACK_INVALID_IP,
    OC_STACK_INVALID_PORT,
    OC_STACK_INVALID_CALLBACK,
    OC_STACK_INVALID_METHOD,

    /** Invalid parameter.*/
    OC_STACK_INVALID_PARAM,
    OC_STACK_INVALID_OBSERVE_PARAM,
    OC_STACK_NO_MEMORY,
    OC_STACK_COMM_ERROR,
    OC_STACK_TIMEOUT,
    OC_STACK_ADAPTER_NOT_ENABLED,
    OC_STACK_NOTIMPL,

    /** Resource not found.*/
    OC_STACK_NO_RESOURCE,

    /** e.g: not supported method or interface.*/
    OC_STACK_RESOURCE_ERROR,
    OC_STACK_SLOW_RESOURCE,
    OC_STACK_DUPLICATE_REQUEST,

    /** Resource has no registered observers.*/
    OC_STACK_NO_OBSERVERS,
    OC_STACK_OBSERVER_NOT_FOUND,
    OC_STACK_VIRTUAL_DO_NOT_HANDLE,
    OC_STACK_INVALID_OPTION,

    /** The remote reply contained malformed data.*/
    OC_STACK_MALFORMED_RESPONSE,
    OC_STACK_PERSISTENT_BUFFER_REQUIRED,
    OC_STACK_INVALID_REQUEST_HANDLE,
    OC_STACK_INVALID_DEVICE_INFO,
    OC_STACK_INVALID_JSON,

    /** Request is not authorized by Resource Server. */
    OC_STACK_UNAUTHORIZED_REQ,

    /** Insert all new error codes here!.*/
    OC_STACK_PRESENCE_STOPPED = 128,
    OC_STACK_PRESENCE_TIMEOUT,
    OC_STACK_PRESENCE_DO_NOT_HANDLE,
    /** ERROR in stack.*/
    OC_STACK_ERROR = 255
    /** Error status code - END HERE.*/
} OCStackResult;

/**
 * Action associated with observation.
 */
typedef enum
{
    /** To Register. */
    OC_OBSERVE_REGISTER = 0,

    /** To Deregister. */
    OC_OBSERVE_DEREGISTER = 1,

    /** Others. */
    OC_OBSERVE_NO_OPTION = 2
} OCObserveAction;

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

typedef enum
{
    OCREP_PROP_NULL,
    OCREP_PROP_INT,
    OCREP_PROP_DOUBLE,
    OCREP_PROP_BOOL,
    OCREP_PROP_STRING,
    OCREP_PROP_BYTE_STRING, // not implemented yet
    OCREP_PROP_OBJECT,
    OCREP_PROP_ARRAY
} OCRepPayloadPropType;

/**
 * Entity's state
 */
typedef enum
{
    /** Request state.*/
    OC_REQUEST_FLAG = (1 << 1),
    /** Observe state.*/
    OC_OBSERVE_FLAG = (1 << 2)
} OCEntityHandlerFlag;

/**
 * Possible returned values from client application.
 */
typedef enum
{
    OC_STACK_DELETE_TRANSACTION = 0,
    OC_STACK_KEEP_TRANSACTION
} OCStackApplicationResult;

/**
 *  OIC Virtual resources supported by every OIC device.
 */
typedef enum
{
    /** unknown URI.*/
    OC_UNKNOWN_URI = 0,

    /** "/oic/res".*/
    OC_WELL_KNOWN_URI,

    /** "/oic/d" .*/
    OC_DEVICE_URI,

    /** "/oic/p" .*/
    OC_PLATFORM_URI,

    /** "/oic/res/d/type" .*/
    OC_RESOURCE_TYPES_URI,

    #ifdef WITH_PRESENCE
    /** "/oic/ad" .*/
    OC_PRESENCE,
    #endif

    /** Max items in the list */
    OC_MAX_VIRTUAL_RESOURCES    //<s Max items in the list
} OCVirtualResources;


/**
 * @enum CAMethod_t
 * Allowed method to be used by resource model
 */
typedef enum
{
    OC_GET = 1, /**< GET Method  */
    OC_POST,    /**< POST Method */
    OC_PUT,     /**< PUT Method */
    OC_DELETE   /**< DELETE Method */
} OCMethod_t;

/**
 * @enum CAMessageType_t
 * Message Type for Base source code
 */
typedef enum
{
    OC_MSG_CONFIRM = 0,  /**< confirmable message (requires ACK/RST) */
    OC_MSG_NONCONFIRM,   /**< non-confirmable message (one-shot message) */
    OC_MSG_ACKNOWLEDGE,  /**< used to acknowledge confirmable messages */
    OC_MSG_RESET         /**< used to indicates not-interested
                              or error (lack of context) in received messages */
} OCMessageType_t;

/**
 * @enum CAResponseResult_t
 * Enums for CA Response values
 */
typedef enum
{
    /* Response status code - START HERE */
    CA_EMPTY = 0,                    /**< Empty */
    CA_SUCCESS = 200,                /**< Success */
    CA_CREATED = 201,                /**< Created */
    CA_DELETED = 202,                /**< Deleted */
    CA_VALID = 203,                  /**< Valid */
    CA_CHANGED = 204,                /**< Changed */
    CA_CONTENT = 205,                /**< Content */
    CA_CONTINUE = 231,               /**< Continue */
    CA_BAD_REQ = 400,                /**< Bad Request */
    CA_UNAUTHORIZED_REQ = 401,       /**< Unauthorized Request */
    CA_BAD_OPT = 402,                /**< Bad Option */
    CA_FORBIDDEN_REQ = 403,          /**< Forbidden Request */
    CA_NOT_FOUND = 404,              /**< Not found */
    CA_REQUEST_ENTITY_INCOMPLETE = 408, /**< Request Entity Incomplete */
    CA_REQUEST_ENTITY_TOO_LARGE = 413,  /**< Request Entity Too Large */
    CA_INTERNAL_SERVER_ERROR = 500,  /**< Internal Server Error */
    CA_RETRANSMIT_TIMEOUT = 504      /**< Retransmit timeout */
    /* Response status code - END HERE */
} CAResponseResult_t;

typedef enum
{
    OC_REQUEST_DATA = 1,
    OC_RESPONSE_DATA,
    OC_ERROR_DATA
} OCDataType_t;

/**
 * The OCPresenceTrigger enum delineates the three spec-compliant modes for
 * "Trigger." These enum values are then mapped to  strings
 * "create", "change", "delete", respectively, before getting encoded into
 * the payload.
 */
typedef enum
{
    /** The creation of a resource is associated with this invocation. */
    OC_PRESENCE_TRIGGER_CREATE = 0,

    /** The change/update of a resource is associated this invocation. */
    OC_PRESENCE_TRIGGER_CHANGE = 1,

    /** The deletion of a resource is associated with this invocation.*/
    OC_PRESENCE_TRIGGER_DELETE = 2
} OCPresenceTrigger;

typedef enum MemoryAllocationType
{
    MAT_none = -1,
    // enum values match index in allocTable
    MAT_message = 0,    // OCRequestInfo
    MAT_payload,        // OCPayload
    MAT_resource,       // OCResource
    MAT_observer,       // ResourceObserver
    MAT_collection,     // OCServerResponse
    MAT_entity,         // OCEntityHandlerRequest
    MAT_PDUs,           // CoAP PDU buffer
    MAT_count           // number of memory allocation types
} MAType_t;

/**
 * Handle to an OCDoResource invocation.
 */
typedef uint32_t OCDoHandle;

/**
 * Handle to an OCResource object owned by the OCStack.
 */
typedef void *OCResourceHandle;

/**
 * Handle to an OCRequest object owned by the OCStack.
 */
typedef void *OCRequestHandle;

/**
 * Unique identifier for each observation request. Used when observations are
 * registered or de-registered. Used by entity handler to signal specific
 * observers to be notified of resource changes.
 * There can be maximum of 256 observations per server.
 */
typedef uint8_t OCObservationId;

/**
 * This typedef is to represent our Server Instance identification.
 */
typedef uint8_t ServerID[16];

/*
 * structures with simple types and types defined in this file.
 */

/*
 * remoteEndpoint identity
 */
typedef struct
{
    uint16_t id_length;
    unsigned char id[MAX_ENDPOINT_IDENTITY_LEN];
} OCRemoteId_t;

typedef struct
{
    /** Action associated with observation request.*/
    OCObserveAction action;

    /** Identifier for observation being registered/deregistered.*/
    OCObservationId obsId;
} OCObservationInfo;

/**
 * This structure will be used to define the vendor specific header options to be included
 * in communication packets.
 */
typedef struct OCHeaderOption
{
    /** The protocol ID this option applies to.*/
    OCTransportProtocolID protocolID;

    /** The header option ID which will be added to communication packets.*/
    uint16_t optionID;

    /** its length 191.*/
    uint16_t optionLength;

    /** pointer to its data.*/
    uint8_t optionData[MAX_HEADER_OPTION_DATA_LENGTH];

#ifdef SUPPORTS_DEFAULT_CTOR
    OCHeaderOption() = default;
    OCHeaderOption(OCTransportProtocolID pid,
                   uint16_t optId,
                   uint16_t optlen,
                   const uint8_t *optData)
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
 * End point identity.
 */
typedef struct
{
    /** Identity Length */
    uint16_t id_length;

    /** Array of end point identity.*/
    unsigned char id[MAX_IDENTITY_SIZE];
} OCIdentity;

/**
 * Data structure to encapsulate IPv4/IPv6/Contiki/lwIP device addresses.
 * OCDevAddr must be the same as CAEndpoint (in CACommon.h).
 */
typedef struct
{
    /** adapter type.*/
    OCTransportAdapter      adapter;

    /** transport modifiers.*/
    OCTransportFlags        flags;

    /** for IP.*/
    uint16_t                port;

    /** address for all adapters.*/
    char                    addr[MAX_ADDR_STR_SIZE];

    /** usually zero for default interface.*/
    uint32_t                interface;

} OCDevAddr;

typedef struct {
    union {
        uint64_t value;
        uint8_t chars[MAX_TOKEN_LEN];
    } t;
    int size;
} OCToken_t;

typedef struct MemoryAllocationEntry
{
    MAType_t type;
    MAType_t share;   // use another table (don't chain)
    int size;         // size of allocation
    int minAllocs;    // initial allocations
    int maxAllocs;    // report allocation failure if more needed
    int nBuffers;     // current number of buffers
    int nAvail;       // current number of buffers available
    void *emergency;  // for reporting errors only
    void **availableBuffers;   // available buffers
#ifdef WITH_DEBUG
    void **allocatedBuffers;   // allocated buffers
#endif
} MAEntry_t;

#endif // OC_ENUMS_H_
