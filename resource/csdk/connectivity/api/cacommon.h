/******************************************************************
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
 * @brief This file contains the common data structures between Resource , CA and adapters
 */

#ifndef __CA_COMMON_H_
#define __CA_COMMON_H_

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 @brief IP address Length
 */
#define CA_IPADDR_SIZE 16

/**
 @brief Mac address length for BT port
 */
#define CA_MACADDR_SIZE 18

/**
 @brief Max header options data length
 */
#define CA_MAX_HEADER_OPTION_DATA_LENGTH 16

/**
 @brief Max URI length
 */
#define CA_MAX_URI_LENGTH 128

/**
@brief option types - the highest option number 63
*/
#define CA_OPTION_IF_MATCH 1
#define CA_OPTION_ETAG 4
#define CA_OPTION_IF_NONE_MATCH 5
#define CA_OPTION_LOCATION_PATH 8
#define CA_OPTION_URI_PATH 11
#define CA_OPTION_CONTENT_FORMAT 12
#define CA_OPTION_CONTENT_TYPE COAP_OPTION_CONTENT_FORMAT
#define CA_OPTION_MAXAGE 14
#define CA_OPTION_URI_QUERY 15
#define CA_OPTION_ACCEPT 17
#define CA_OPTION_LOCATION_QUERY 20
#define CA_OPTION_OBSERVE 6

/**
 @brief Max length of ID
 */
#define DTLS_PSK_ID_LEN 16

/**
 @brief Max length of PSK
 */
#define DTLS_PSK_PSK_LEN 16

/**
 @brief version
 */
#define DtlsPskCredsBlobVer_1 1

/**
 @brief current version
 */
#define DtlsPskCredsBlobVer_CurrentVersion DtlsPskCredsBlobVer_1


/**
 @brief Payload information from resource model
 */
typedef char *CAPayload_t;

/**
 @brief URI for the OIC base.CA considers relative URI as the URI.
 */
typedef char *CAURI_t;

/**
 @brief Token information for mapping the request and responses by resource model
 */
typedef char *CAToken_t;

/**
 @brief Boolean value used for specifying the success or failure
 */

typedef enum
{
    CA_FALSE = 0,
    CA_TRUE
} CABool_t;

/**
 @brief Different connectivities that are handled in Connectivity Abstraction
 */
typedef enum
{
    CA_ETHERNET = (1 << 0),
    CA_WIFI = (1 << 1),
    CA_EDR = (1 << 2),
    CA_LE = (1 << 3)
} CAConnectivityType_t;

/**
 @brief Information about the network status.CA_INTERFACE_UP means connectivity is available
 */
typedef enum
{
    CA_INTERFACE_UP,
    CA_INTERFACE_DOWN
} CANetworkStatus_t;

/**
 @brief  Address of the local or remote endpoint
 */
typedef union
{
    /**
     @brief BT Mac Information
     */
    struct
    {
        /** @brief BT mac address **/
        char btMacAddress[CA_MACADDR_SIZE];
    } BT;

    /**
     @brief LE MAC Information
     */
    struct
    {
        /** @brief BLE mac address **/
        char leMacAddress[CA_MACADDR_SIZE];
    } LE;

    /**
     @brief IP Information for wifi and ethernet ports
     */
    struct
    {
        /** Ip address of the interface**/
        char ipAddress[CA_IPADDR_SIZE];
        /** port information**/
        uint32_t port;
    } IP;
} CAAddress_t;

/**
 @brief Message Type for Base source code
 */
typedef enum
{
    CA_MSG_CONFIRM = 0,  /* confirmable message (requires ACK/RST) */
    CA_MSG_NONCONFIRM,   /* non-confirmable message (one-shot message) */
    CA_MSG_ACKNOWLEDGE,  /* used to acknowledge confirmable messages */
    CA_MSG_RESET         /* indicates error in received messages */
} CAMessageType_t;

/**
 @brief Allowed method to be used by resource model
 */
typedef enum
{
    CA_GET = 1,
    CA_POST,
    CA_PUT,
    CA_DELETE
} CAMethod_t;

/**
 @brief Remote endpoint information for connectivities
 */
typedef struct
{
    /** Resource URI information **/
    CAURI_t resourceUri;
    /** Remote Endpoint address **/
    CAAddress_t addressInfo;
    /** Connectivity of the endpoint**/
    CAConnectivityType_t connectivityType;
    /** Secure connection**/
    CABool_t isSecured;
} CARemoteEndpoint_t;


/**
 @brief Group endpoint information for connectivities
 */
typedef struct
{
    /** Resource URI information **/
    CAURI_t resourceUri;
    /** Connectivity of the endpoint**/
    CAConnectivityType_t connectivityType;
} CAGroupEndpoint_t;

/**
 @brief Local Connectivity information
 */
typedef struct
{
    /** address of the interface  **/
    CAAddress_t addressInfo;
    /** Connectivity type that localconnectivity avaialble **/
    CAConnectivityType_t type;
    /** Secure connection**/
    CABool_t isSecured;
} CALocalConnectivity_t;

/**
 @brief Enums for CA return values
 */
typedef enum
{
    /* Success status code - START HERE */
    CA_STATUS_OK = 0,
    CA_STATUS_INVALID_PARAM,
    CA_ADAPTER_NOT_ENABLED,
    CA_SERVER_STARTED_ALREADY,
    CA_SERVER_NOT_STARTED,
    CA_DESTINATION_NOT_REACHABLE,
    CA_SOCKET_OPERATION_FAILED,
    CA_SEND_FAILED,
    CA_RECEVIE_FAILED,
    CA_MEMORY_ALLOC_FAILED,
    CA_REQUEST_TIMEOUT,
    CA_DESTINATION_DISCONNECTED,
    CA_STATUS_FAILED,
    CA_NOT_SUPPORTED
    /* Result code - END HERE */
} CAResult_t;

/**
 @brief Enums for CA Response  values
 */
typedef enum
{
    /* Success status code - START HERE */
    CA_SUCCESS = 200,
    CA_CREATED = 201,
    CA_DELETED = 202,
    CA_BAD_REQ = 400,
    CA_BAD_OPT = 402,
    CA_NOT_FOUND = 404,
    CA_RETRANSMIT_TIMEOUT = 500
    /* Response status code - END HERE */
} CAResponseResult_t;

/**
 @brief Transport Protocol IDs for additional options
 */
typedef enum
{
    CA_INVALID_ID = (1 << 0),
    CA_COAP_ID = (1 << 1)
} CATransportProtocolID_t;

/**
 * @brief Header options structure to be filled
 *
 * This structure is used to hold header information.
 */
typedef struct
{
    /** The protocol ID this option applies to**/
    CATransportProtocolID_t protocolID;
    /** The header option ID which will be added to communication packets**/
    uint16_t optionID;
    /** its length   **/
    uint16_t optionLength;
    /** optional data values**/
    uint8_t optionData[CA_MAX_HEADER_OPTION_DATA_LENGTH];
} CAHeaderOption_t;

/**
 * @brief Base Information received
 *
 * This structure is used to hold request & response base information
 */
typedef struct
{
    /**Qos for the request **/
    CAMessageType_t type;
    /** Message id.
     * if message id is zero, it will generated by CA inside.
     * otherwise, you can use it.**/
    uint16_t messageId;
    /** Token for CA**/
    CAToken_t token;
    /** Header Options for the request **/
    CAHeaderOption_t *options;
    /** Number of Header options**/
    uint8_t numOptions;
    /** payload of the request **/
    CAPayload_t payload;
} CAInfo_t;

/**
 * @brief Request Information to be sent
 *
 * This structure is used to hold request information
 */
typedef struct
{
    /** Name of the Method Allowed **/
    CAMethod_t method;
    /** Information of the request. **/
    CAInfo_t info;
} CARequestInfo_t;

/**
 * @brief Response information received
 *
 * This structure is used to hold response information
 */
typedef struct
{
    /**Result for response by resource model**/
    CAResponseResult_t result;
    /**Information of the response.**/
    CAInfo_t info;
} CAResponseInfo_t;

/**
 * Credentials for a device. Includes identity and the associated PSK.
 */
typedef struct
{
    unsigned char clientIdentity[DTLS_PSK_ID_LEN];
    unsigned char rsClientPsk[DTLS_PSK_PSK_LEN];
} CADtlsPskCreds_t;

/**
 * Binary blob containing device identity and the credentials for all devices
 * trusted by this device.
 */
typedef struct
{
    uint16_t blobVer;                        /**< version of the blob */
    uint16_t reserved;                       /**< reserved for future use */
    unsigned char rsIdentity[DTLS_PSK_ID_LEN]; /**< identity of self */
    uint32_t num;                            /**< number of credentials in this blob */
    CADtlsPskCreds_t *creds;                 /**< list of credentials. Size of this
                                                 array is determined by 'num' variable. */
} CADtlsPskCredsBlob_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //#ifndef __CA_COMMON_H_
