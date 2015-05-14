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
#ifndef CA_ADAPTER_NET_DTLS_H_
#define CA_ADAPTER_NET_DTLS_H_

#include "dtls.h"
#include "uarraylist.h"
#include "camutex.h"
#include "caadapterutils.h"
#include "ocsecurityconfig.h"
#include "cainterface.h"

/**
 *   Currently DTLS supported adapters(2) WIFI and ETHENET for linux platform.
 */
#define MAX_SUPPORTED_ADAPTERS 2

/**
 * @brief The implementation will be provided by OIC RI layer.
 */
extern void OCGetDtlsPskCredentials(CADtlsPskCredsBlob_t **credInfo);

typedef void (*CAPacketReceivedCallback)(const char *ipAddress, const uint16_t port,
         const void *data, const uint32_t dataLength, const bool isSecured);

typedef uint32_t (*CAPacketSendCallback)(const char *ipAddress, const uint16_t port,
        const void *data, const uint32_t dataLength);

/**
 * @struct stCAAdapterCallbacks_t
 * @brief  Data structure for holding the send and recv callbacks.
 */
typedef struct CAAdapterCallbacks
{
    CAPacketReceivedCallback recvCallback;  /**< Callback used to send data to upper layer. */
    CAPacketSendCallback sendCallback;      /**< Callback used to send data to socket layer. */
} stCAAdapterCallbacks_t;

/**
 * @struct stCADtlsContext_t
 * @brief  Data structure for holding the tinyDTLS interface
 *              related info.
 */
typedef struct stCADtlsContext
{
    u_arraylist_t *cacheList;            /**< PDU's are cached until DTLS session is formed. */
    struct dtls_context_t *dtlsContext;  /**< Pointer to tinyDTLS context. */
    struct stPacketInfo *packetInfo;     /**< used by callback during
                                             decryption to hold address/length. */
    dtls_handler_t callbacks;            /**< Pointer to callbacks needed by tinyDTLS. */
    stCAAdapterCallbacks_t adapterCallbacks[MAX_SUPPORTED_ADAPTERS];
} stCADtlsContext_t;

/**
 * @struct stPacketInfo_t
 * @brief  Data structure for holding the decrypted data address
 *              and length provided by tinyDTLS callback interface.
 */
typedef struct stPacketInfo
{
    uint8_t *dataAddress;
    uint16_t dataLen;
} stPacketInfo_t;

/**
 * @enum eDtlsRet_t
 * @brief tinyDTLS library error codes.
 *
 */
typedef enum
{
    DTLS_OK = 0,
    DTLS_FAIL,
    DTLS_SESSION_INITIATED,
    DTLS_HS_MSG
} eDtlsRet_t;


/** Structure to have address information which will match with DTLS session_t struct.*/
typedef struct
{
    socklen_t size;                 /**< Size of address. */
    union
    {
        struct sockaddr     sa;
        struct sockaddr_storage st;
        struct sockaddr_in  sin;
        struct sockaddr_in6 sin6;
    } addr;                         /**< Address information. */
    uint8_t ifIndex;                /**< Holds adpater index to get callback info. */
} stCADtlsAddrInfo_t;

/**
 * @struct stCACacheMessage_t
 * @brief structure to holds the information of cachemessage and address info.
 *
 */
typedef struct CACacheMessage
{
    void *data;
    uint32_t dataLen;
    stCADtlsAddrInfo_t *destSession;
} stCACacheMessage_t;

/**
 * @enum eDtlsAdapterType_t
 * @brief This enum is used as array index for storing adapter level callbacks.
 *        So Keeping 0 instead of "1 << 0". It is not going to be used as addition
 *        and removal of adapter.
 *
 */
typedef enum
{
    DTLS_IP = 0,
} eDtlsAdapterType_t;

/**
 * @fn  CADTLSSetAdapterCallbacks
 * @brief  Used set send and recv callbacks for different adapters(WIFI,EtherNet)
 *
 * @param[in]  recvCallback  packet received callback
 * @param[in]  sendCallback  packet sent callback
 * @param[in]  type  type of adapter
 *
 * @retval  void
 *
 */
void CADTLSSetAdapterCallbacks(CAPacketReceivedCallback recvCallback,
                               CAPacketSendCallback sendCallback, eDtlsAdapterType_t type);

/**
 * @brief   Register callback to get DTLS PSK credentials.
 * @param   credCallback   [IN] callback to get DTLS credentials
 * @retval  void
 */
void CADTLSSetCredentialsCallback(CAGetDTLSCredentialsHandler credCallback);

/**
 * @fn  CAAdapterNetDtlsInit
 * @brief  initialize tinyDTLS library and other necessary intialization.
 *
 * @return  0 on success otherwise a positive error value.
 * @retval  CA_STATUS_OK  Successful
 * @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval  CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CAAdapterNetDtlsInit();

/**
 * @fn  CAAdapterNetDtlsDeInit
 * @brief  de-inits tinyDTLS library and free the allocated memory.
 *
 * @return  void
 *
 */
void CAAdapterNetDtlsDeInit();

/**
 * @fn  CAAdapterNetDtlsEncrypt
 * @brief  Performs DTLS encryption of the CoAP PDU. If a
 *              DTLS session does not exist yet with the @dst,
 *              a DTLS handshake will be started. In case where
 *              a new DTLS handshake is started, pdu info is
 *              cached to be send when session setup is finished.
 *
 * @param[in]  remoteAddress  address to which data will be sent.
 * @param[in]  port  port to which data will be sent.
 * @param[in]  data  length of data.
 * @param[in]  dataLen  length of given data
 * @param[out]  decdata  output variable to store the starting address
 *                        of decrypted plaintext.
 * @param[out]  cacheFlag  utput variable to indicate if pdu
 *                        is cached and inform the caller to
 *                       NOT free the memory holding pdu.
 * @return  0 on success otherwise a positive error value.
 * @retval  CA_STATUS_OK  Successful
 * @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval  CA_STATUS_FAILED Operation failed
 *
 */

CAResult_t CAAdapterNetDtlsEncrypt(const char *remoteAddress,
                                   const uint16_t port,
                                   void *data,
                                   uint32_t dataLen,
                                   uint8_t *cacheFlag,
                                   eDtlsAdapterType_t type);

/**
 * @fn  CAAdapterNetDtlsDecrypt
 * @brief  Performs DTLS decryption of the data received on
 *            secure port. This method performs in-place decryption
 *            of the cipher-text buffer. If a DTLS handshake message
 *            is received or decryption failure happens, this method
 *            returns -1. If a valid application PDU is decrypted, it
 *            returns the length of the decrypted pdu.
 *
 * @return  0 on success otherwise a positive error value.
 * @retval  CA_STATUS_OK  Successful
 * @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval  CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CAAdapterNetDtlsDecrypt(const char *remoteAddress,
                                   const uint16_t port,
                                   uint8_t *data,
                                   uint32_t dataLen,
                                   eDtlsAdapterType_t type);

#endif /* CA_ADAPTER_NET_DTLS_H_ */


