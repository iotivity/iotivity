/* ****************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/**
 * @file cacommon.h
 * This file contains the common data structures between Resource , CA and adapters
 */

#ifndef CA_COMMON_H_
#define CA_COMMON_H_

#include "ocmessages.h"
#include "ocglobals.h"

/**
 * IP address Length -- arduino only
 */
#define CA_IPADDR_SIZE 16

/**
 * Max PDU length supported
 */
#ifndef COAP_MAX_PDU_SIZE
#define COAP_MAX_PDU_SIZE (1400) /* maximum size of a CoAP PDU */
#endif

/**
 * Remote Access jabber ID length.
 */
#define CA_RAJABBERID_SIZE 256

/**
 * Mac address length for BT port
 */
#define CA_MACADDR_SIZE 18

/**
 * @enum CAResult_t
 * Enums for CA return values
 */
typedef enum
{
    /* Result code - START HERE */
    CA_STATUS_OK = 0,               /**< Success */
    CA_STATUS_INVALID_PARAM,        /**< Invalid Parameter */
    CA_ADAPTER_NOT_ENABLED,         /**< Adapter is not enabled */
    CA_SERVER_STARTED_ALREADY,      /**< Server is started already */
    CA_SERVER_NOT_STARTED,          /**< Server is not started*/
    CA_DESTINATION_NOT_REACHABLE,   /**< Destination is not reachable */
    CA_SOCKET_OPERATION_FAILED,     /**< Socket operation failed */
    CA_SEND_FAILED,                 /**< Send request failed */
    CA_RECEIVE_FAILED,              /**< Receive failed */
    CA_MEMORY_ALLOC_FAILED,         /**< Memory allocation failed */
    CA_REQUEST_TIMEOUT,             /**< Request is Timeout */
    CA_DESTINATION_DISCONNECTED,    /**< Destination is disconnected */
    CA_NOT_SUPPORTED,               /**< Not supported */
    CA_STATUS_NOT_INITIALIZED,      /**< Not Initialized*/
    CA_RECEIVE_DUPLICATE,           /**< Received message is duplicate */
    CA_STATUS_FAILED = 255          /**< Failure */
    /* Result code - END HERE */
} CAResult_t;

/**
 * @enum CAAdapterState_t
 * Adapter State to indicate the network changed notifications.
 */
typedef enum
{
    CA_ADAPTER_DISABLED,   /**< Adapter is Disabled */
    CA_ADAPTER_ENABLED     /**< Adapter is Enabled */
} CAAdapterState_t;

/**
 * @enum CANetworkStatus_t
 * Information about the network status.
 */
typedef enum
{
    CA_INTERFACE_DOWN,   /**< Connection is not available */
    CA_INTERFACE_UP    /**< Connection is Available */
} CANetworkStatus_t;

/**
 * block size
 * it depends on defined size in libCoAP.
 */
typedef enum
{
    CA_BLOCK_SIZE_16_BYTE = 0,    /**< 16byte */
    CA_BLOCK_SIZE_32_BYTE = 1,    /**< 32byte */
    CA_BLOCK_SIZE_64_BYTE = 2,    /**< 64byte */
    CA_BLOCK_SIZE_128_BYTE = 3,   /**< 128byte */
    CA_BLOCK_SIZE_256_BYTE = 4,   /**< 256byte */
    CA_BLOCK_SIZE_512_BYTE = 5,   /**< 512byte */
    CA_BLOCK_SIZE_1024_BYTE = 6     /**< 1Kbyte */
} CABlockSize_t;

typedef enum
{
    CA_DEFAULT_ADAPTER = 0,

    // value zero indicates discovery
    CA_ADAPTER_IP            = (1 << 0),   // IPv4 and IPv6, including 6LoWPAN
    CA_ADAPTER_GATT_BTLE     = (1 << 1),   // GATT over Bluetooth LE
    CA_ADAPTER_RFCOMM_BTEDR  = (1 << 2),   // RFCOMM over Bluetooth EDR

    #ifdef RA_ADAPTER
    CA_ADAPTER_REMOTE_ACCESS = (1 << 3),   // Remote Access over XMPP.
    #endif

    CA_ALL_ADAPTERS          = 0xffffffff
} CATransportAdapter_t;

typedef enum
{
    CA_DEFAULT_FLAGS = 0,

    // Insecure transport is the default (subject to change)
    CA_SECURE          = (1 << 4),   // secure the transport path
    // IPv4 & IPv6 autoselection is the default
    CA_IPV6            = (1 << 5),   // IP adapter only
    CA_IPV4            = (1 << 6),   // IP adapter only
    // Indication that a message was received by multicast.
    CA_MULTICAST       = (1 << 7),
    // Link-Local multicast is the default multicast scope for IPv6.
    // These correspond in both value and position to the IPv6 address bits.
    CA_SCOPE_INTERFACE = 0x1, // IPv6 Interface-Local scope
    CA_SCOPE_LINK      = 0x2, // IPv6 Link-Local scope (default)
    CA_SCOPE_REALM     = 0x3, // IPv6 Realm-Local scope
    CA_SCOPE_ADMIN     = 0x4, // IPv6 Admin-Local scope
    CA_SCOPE_SITE      = 0x5, // IPv6 Site-Local scope
    CA_SCOPE_ORG       = 0x8, // IPv6 Organization-Local scope
    CA_SCOPE_GLOBAL    = 0xE, // IPv6 Global scope
} CATransportFlags_t;

#define CA_IPFAMILY_MASK (CA_IPV6|CA_IPV4)
#define CA_SCOPE_MASK 0xf     // mask scope bits above

/*
 * remoteEndpoint identity
 */
typedef struct
{
    uint16_t id_length;
    unsigned char id[MAX_ENDPOINT_IDENTITY_LEN];
} CARemoteId_t;

/**
 * Endpoint information for connectivities
 * Must be identical to OCDevAddr.
 */
typedef struct
{
    CATransportAdapter_t    adapter;    // adapter type
    CATransportFlags_t      flags;      // transport modifiers
    uint16_t                port;       // for IP
    char                    addr[MAX_ADDR_STR_SIZE]; // address for all
    uint32_t                interface;  // usually zero for default interface
} CAEndpoint_t;

/**
 * Endpoint information for secure messages
 */
typedef struct
{
    CAEndpoint_t endpoint;      /**< endpoint */
    CARemoteId_t identity;      /**< endpoint identity */
} CASecureEndpoint_t;

#define MAX_ADDR_STR_SIZE_CA MAX_ADDR_STR_SIZE

#endif // CA_COMMON_H_
