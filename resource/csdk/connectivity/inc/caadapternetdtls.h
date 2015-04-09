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
#ifndef _CA_ADAPTER_NET_DTLS_H
#define _CA_ADAPTER_NET_DTLS_H

#include "dtls.h"
#include "uarraylist.h"
#include "umutex.h"
#include "caadapterutils.h"
#include "ocsecurityconfig.h"
#include "cainterface.h"

#define MAX_SUPPORTED_ADAPTERS 2

/**
 * @brief The implementation will be provided by OIC RI layer.
 */
extern void OCGetDtlsPskCredentials(OCDtlsPskCredsBlob **credInfo);

typedef void (*CAPacketReceivedCallback)(const char *ipAddress, const uint32_t port,
        const void *data, const uint32_t dataLength, const CABool_t isSecured);

typedef uint32_t (*CAPacketSendCallback)(const char *ipAddress, const uint32_t port,
        const void *data, const uint32_t dataLength);

/**
 * @struct stCAAdapterCallbacks_t
 * @brief  Data structure for holding the send and recv callbacks.
 */
typedef struct CAAdapterCallbacks
{
    CAPacketReceivedCallback recvCallback;
    CAPacketSendCallback sendCallback;
} stCAAdapterCallbacks_t;

/**
 * @struct stCADtlsContext_t
 * @brief  Data structure for holding the tinyDTLS interface
 *              related info.
 */
typedef struct stCADtlsContext
{
    u_arraylist_t  *cacheList;          /**< pdu's are cached until DTLS session is formed */
    struct dtls_context_t *dtlsContext;    /**< pointer to tinyDTLS context */
    struct stPacketInfo *packetInfo;          /**< used by callback during
                                                                    decryption to hold address/length */
    dtls_handler_t callbacks;           /**< pointer to callbacks needed by tinyDTLS */
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

/**
 * @struct stGattServiceInfo_t
 * @brief structure to have address information.
 *
 */
typedef struct
{
    socklen_t size;       /**< size of addr */
    union
    {
        struct sockaddr     sa;
        struct sockaddr_storage st;
        struct sockaddr_in  sin;
        struct sockaddr_in6 sin6;
    } addr;
    uint8_t ifIndex;
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
 * @brief adapter types
 *
 */
typedef enum
{
    DTLS_ETHERNET = 0,
    DTLS_WIFI
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
 * Select the cipher suite for dtls handshake
 *
 * @param[in] cipher    cipher suite
 *                             0xC018 : TLS_ECDH_anon_WITH_AES_128_CBC_SHA
 *                             0xC0A8 : TLS_PSK_WITH_AES_128_CCM_8
 *                             0xC0AE : TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
 * @return  0 on success otherwise a positive error value
 * @retval  CA_STATUS_OK    Successful
 * @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval  CA_STATUS_FAILED Operation failed
 */
CAResult_t CADtlsSelectCipherSuite(const dtls_cipher_t cipher);

/**
 * Select the cipher suite for dtls handshake
 *
 * @param[in] enable  0, enabling TLS_ECDH_anon_WITH_AES_128_CBC_SHA
 *                     other value, disabling TLS_ECDH_anon_WITH_AES_128_CBC_SHA
 *
 * @return  0 on success otherwise a positive error value
 * @retval  CA_STATUS_OK    Successful
 * @retval  CA_STATUS_FAILED Operation failed
 */
CAResult_t CADtlsEnablesAnonEcdh(const uint8_t enable);

/**
 * Initiate DTLS handshake with selected cipher suite
 *
 * @param[in] addrInfo  information of network address
 * @param[in] connType  connectivity type
 *
 * @return  0 on success otherwise a positive error value
 * @retval  CA_STATUS_OK    Successful
 * @retval  CA_STATUS_FAILED Operation failed
 */
CAResult_t CADtlsInitiateHandshake(const CAAddress_t* addrInfo,
                                  const CAConnectivityType_t connType);

/**
 * Generate ownerPSK using PRF
 * OwnerPSK = TLS-PRF('master key' , 'oic.sec.doxm.jw',
 *                                    'ID of new device(Resource Server)',
 *                                    'ID of owner smart-phone(Provisioning Server)')
 *
 * @param[in] addrInfo  information of network address
 * @param[in] connType  connectivity type
 * @param[in] label  Ownership transfer method e.g)"oic.sec.doxm.jw"
 * @param[in] labelLen  Byte length of label
 * @param[in] rsrcServerDeviceID  ID of new device(Resource Server)
 * @param[in] rsrcServerDeviceIDLen  Byte length of rsrcServerDeviceID
 * @param[in] provServerDeviceID  label of previous owner
 * @param[in] provServerDeviceIDLen  byte length of provServerDeviceID
 * @param[in,out] ownerPSK  Output buffer for owner PSK
 * @param[in] ownerPSKSize  Byte length of the ownerPSK to be generated
 *
 * @return  0 on success otherwise a positive error value
 * @retval  CA_STATUS_OK    Successful
 * @retval  CA_STATUS_FAILED Operation failed
 */
CAResult_t CADtlsGenerateOwnerPSK(const CAAddress_t* addrInfo,
                    const CAConnectivityType_t connType,
                    const uint8_t* label, const size_t labelLen,
                    const uint8_t* rsrcServerDeviceID, const size_t rsrcServerDeviceIDLen,
                    const uint8_t* provServerDeviceID, const size_t provServerDeviceIDLen,
                    uint8_t* ownerPSK, const size_t ownerPSKSize);
;

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
                                   const uint32_t port,
                                   const void *data,
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
                                   const uint32_t port,
                                   uint8_t *data,
                                   uint32_t dataLen,
                                   eDtlsAdapterType_t type);

#endif //_CA_ADAPTER_NET_DTLS_H

