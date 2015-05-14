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
 * @brief This file contains the common data structures between Resource , CA and adapters
 */

#ifndef CA_COMMON_H_
#define CA_COMMON_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief IP address Length
 */
#define CA_IPADDR_SIZE 16

/**
 * @brief Mac address length for BT port
 */
#define CA_MACADDR_SIZE 18

/**
 * @brief Max header options data length
 */
#define CA_MAX_HEADER_OPTION_DATA_LENGTH 16

/**
* @brief Max token length
*/
#define CA_MAX_TOKEN_LEN (8)

/**
 * @brief Max URI length
 */
#ifdef ARDUINO
#define CA_MAX_URI_LENGTH 128  /* maximum size of URI for embedded platforms*/
#else
#define CA_MAX_URI_LENGTH 512 /* maximum size of URI for other platforms*/
#endif

/**
 * @brief Max PDU length supported
 */
#ifdef ARDUINO
#define COAP_MAX_PDU_SIZE           320  /* maximum size of a CoAP PDU for embedded platforms*/
#else
#define COAP_MAX_PDU_SIZE           1400 /* maximum size of a CoAP PDU for big platforms*/
#endif

/**
 * @brief option types - the highest option number 63
 */
#define CA_OPTION_IF_MATCH 1
#define CA_OPTION_ETAG 4
#define CA_OPTION_IF_NONE_MATCH 5
#define CA_OPTION_OBSERVE 6
#define CA_OPTION_LOCATION_PATH 8
#define CA_OPTION_URI_PATH 11
#define CA_OPTION_CONTENT_FORMAT 12
#define CA_OPTION_CONTENT_TYPE COAP_OPTION_CONTENT_FORMAT
#define CA_OPTION_MAXAGE 14
#define CA_OPTION_URI_QUERY 15
#define CA_OPTION_ACCEPT 17
#define CA_OPTION_LOCATION_QUERY 20

/**
 * @brief Payload information from resource model
 */
typedef char *CAPayload_t;

/**
 * @brief URI for the OIC base.CA considers relative URI as the URI.
 */
typedef char *CAURI_t;

/**
 * @brief Token information for mapping the request and responses by resource model
 */
typedef char *CAToken_t;

/**
 * @enum CATransportType_t
 * @brief Different connectivities that are handled in Connectivity Abstraction
 */
typedef enum
{
    CA_IPV4 = (1 << 0),     /**< IPV4 Transport Type */
    CA_IPV6 = (1 << 1),     /**< IPV6 Transport Type */
    CA_EDR = (1 << 2),      /**< EDR Transport Type */
    CA_LE = (1 << 3)        /**< LE Transport Type */
} CATransportType_t;

/**
 * @enum CANetworkStatus_t
 * @brief Information about the network status.
 */
typedef enum
{
    CA_INTERFACE_DOWN,   /**< Connection is not available */
    CA_INTERFACE_UP    /**< Connection is Available */
} CANetworkStatus_t;

/**
 * @brief  Address of the local or remote endpoint
 */
typedef union
{
    /**
     * @brief BT Mac Information
     */
    struct
    {
        char btMacAddress[CA_MACADDR_SIZE];   /**< BT mac address **/
    } BT;

    /**
     * @brief LE MAC Information
     */
    struct
    {
        char leMacAddress[CA_MACADDR_SIZE];   /**< BLE mac address **/
    } LE;

    /**
     * @brief IP Information
     */
    struct
    {
        char ipAddress[CA_IPADDR_SIZE]; /**< Ip address of the interface**/
        uint16_t port;                  /**< port information**/
    } IP;
} CAAddress_t;

/**
 * @enum CAMessageType_t
 * @brief Message Type for Base source code
 */
typedef enum
{
    CA_MSG_CONFIRM = 0,  /**< confirmable message (requires ACK/RST) */
    CA_MSG_NONCONFIRM,   /**< non-confirmable message (one-shot message) */
    CA_MSG_ACKNOWLEDGE,  /**< used to acknowledge confirmable messages */
    CA_MSG_RESET         /**< used to indicates not-interested or error (lack of context)in
                                                  received messages */
} CAMessageType_t;

/**
 * @enum CAMethod_t
 * @brief Allowed method to be used by resource model
 */
typedef enum
{
    CA_GET = 1, /**< GET Method  */
    CA_POST,    /**< POST Method */
    CA_PUT,     /**< PUT Method */
    CA_DELETE   /**< DELETE Method */
} CAMethod_t;

/**
 * @brief Remote endpoint information for connectivities
 */
typedef struct
{

    CAURI_t resourceUri;                    /**< Resource URI information **/
    CAAddress_t addressInfo;                /**< Remote Endpoint address **/
    CATransportType_t transportType;  /**< Transport Type of the endpoint**/
    bool isSecured;                     /**< Secure connection**/
} CARemoteEndpoint_t;


/**
 * @brief Group endpoint information for connectivities
 */
typedef struct
{
    CAURI_t resourceUri;                    /**< Resource URI information **/
    CATransportType_t transportType;  /**< Transport type of the endpoint**/
} CAGroupEndpoint_t;

/**
 @brief Local Connectivity information
 */
typedef struct
{
    CAAddress_t addressInfo;    /**< Address of the interface  **/
    CATransportType_t type;  /**< Transport type of local device **/
    bool isSecured;         /**< Secure connection**/
} CALocalConnectivity_t;

/**
 * @enum CAResult_t
 * @brief Enums for CA return values
 */
typedef enum
{
    // Result code - START HERE
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
    CA_STATUS_FAILED =255           /**< Failure */
    /* Result code - END HERE */
} CAResult_t;

/**
 * @enum CAResponseResult_t
 * @brief Enums for CA Response values
 */
typedef enum
{
    /* Response status code - START HERE */
    CA_EMPTY = 0,                    /**< Empty */
    CA_SUCCESS = 200,                /**< Success */
    CA_CREATED = 201,                /**< Created */
    CA_DELETED = 202,                /**< Deleted */
    CA_BAD_REQ = 400,                /**< Bad Request */
    CA_BAD_OPT = 402,                /**< Bad Option */
    CA_NOT_FOUND = 404,              /**< Not found */
    CA_INTERNAL_SERVER_ERROR = 500,  /**< Internal Server Error */
    CA_RETRANSMIT_TIMEOUT = 504      /**< Retransmit timeout */
    /* Response status code - END HERE */
} CAResponseResult_t;

/**
 * @enum CATransportProtocolID_t
 * @brief Transport Protocol IDs for additional options
 */
typedef enum
{
    CA_INVALID_ID = (1 << 0),   /**< Invalid ID */
    CA_COAP_ID = (1 << 1)       /**< COAP ID */
} CATransportProtocolID_t;

/**
 * @enum CAAdapterState_t
 * @brief Adapter State to indicate the network changed notifications.
 */
typedef enum
{
    CA_ADAPTER_DISABLED,   /**< Adapter is Disabled */
    CA_ADAPTER_ENABLED     /**< Adapter is Enabled */
} CAAdapterState_t;

/**
 * @brief Header options structure to be filled
 *
 * This structure is used to hold header information.
 */
typedef struct
{
    CATransportProtocolID_t protocolID;                     /**< Protocol ID of the Option */
    uint16_t optionID;                                      /**< The header option ID which will be
                                                            added to communication packets */
    uint16_t optionLength;                                  /**< Option Length **/
    uint8_t optionData[CA_MAX_HEADER_OPTION_DATA_LENGTH];   /**< Optional data values**/
} CAHeaderOption_t;

/**
 * @brief Base Information received
 *
 * This structure is used to hold request & response base information
 */
typedef struct
{

    CAMessageType_t type;       /**< Qos for the request */
    uint16_t messageId;         /**< Message id.
                                 * if message id is zero, it will generated by CA inside.
                                 * otherwise, you can use it */
    CAToken_t token;            /**< Token for CA */
    uint8_t tokenLength;        /**< token length*/
    CAHeaderOption_t *options;  /** Header Options for the request */
    uint8_t numOptions;         /**< Number of Header options */
    CAPayload_t payload;        /**< payload of the request  */
} CAInfo_t;

/**
 * @brief Request Information to be sent
 *
 * This structure is used to hold request information
 */
typedef struct
{
    CAMethod_t method;  /**< Name of the Method Allowed */
    CAInfo_t info;      /**< Information of the request. */
} CARequestInfo_t;

/**
 * @brief Response information received
 *
 * This structure is used to hold response information
 */
typedef struct
{
    CAResponseResult_t result;  /**< Result for response by resource model */
    CAInfo_t info;              /**< Information of the response */
} CAResponseInfo_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_COMMON_H_ */

