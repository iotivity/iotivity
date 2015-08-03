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
 * @file
 *
 * This file contains the APIs for Resource Model to use.
 */

#ifndef CA_INTERFACE_H_
#define CA_INTERFACE_H_

/**
 * Connectivity Abstraction Interface APIs.
 */
#include "cacommon.h"

#ifdef __WITH_DTLS__
#include "ocsecurityconfig.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Callback function type for request delivery.
 * @param   object      [OUT] Endpoint object from which the request is received. It contains
 *                            endpoint address based on the connectivity type.
 * @param   requestInfo [OUT] Info for resource model to understand about the request.
 * @return  NONE
 */
typedef void (*CARequestCallback)(const CAEndpoint_t *object,
                                  const CARequestInfo_t *requestInfo);

/**
 * @brief   Callback function type for response delivery.
 * @param   object          [OUT] Endpoint object from which the response is received.
 * @param   responseInfo    [OUT] Identifier which needs to be mapped with response.
 * @return  NONE
 */
typedef void (*CAResponseCallback)(const CAEndpoint_t *object,
                                   const CAResponseInfo_t *responseInfo);
/**
 * @brief   Callback function type for error
 * @param   object          [OUT] remote device information
 * @param   errorInfo       [OUT] CA Error information
 * @return  NONE
 */
typedef void (*CAErrorCallback)(const CAEndpoint_t *object,
                                const CAErrorInfo_t *errorInfo);

#ifdef __WITH_DTLS__

/**
 * Binary blob containing device identity and the credentials for all devices
 * trusted by this device.
 */
typedef struct
{
   unsigned char identity[DTLS_PSK_ID_LEN]; /** identity of self */
   uint32_t num;                            /** number of credentials in this blob */
   OCDtlsPskCreds *creds;                   /** list of credentials. Size of this
                                                array is determined by 'num' variable. */
} CADtlsPskCredsBlob_t;

/**
 * @brief   Callback function type for getting DTLS credentials.
 * @param   credInfo          [OUT] DTLS credentials info. Handler has to allocate new memory for
 *                                  both credInfo and credInfo->creds which is then freed by CA
 * @return  NONE
 */
typedef void (*CAGetDTLSCredentialsHandler)(CADtlsPskCredsBlob_t **credInfo);
#endif //__WITH_DTLS__

/**
 * @brief   Initialize the connectivity abstraction module.
 *          It will initialize adapters, thread pool and other modules based on the platform
 *          compilation options.
 *
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED or #CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CAInitialize();

/**
 * @brief   Terminate the connectivity abstraction module.
 *          All threads, data structures are destroyed for next initializations.
 * @return  NONE
 */
void CATerminate();

/**
 * @brief   Starts listening servers.
 *          This API is used by resource hosting server for listening multicast requests.
 *          Based on the adapters configurations, different kinds of servers are started.
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED
 */
CAResult_t CAStartListeningServer();

/**
 * @brief   Starts discovery servers.
 *          This API is used by resource required clients for listening multicast requests.
 *          Based on the adapters configurations, different kinds of servers are started.
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED
 */
CAResult_t CAStartDiscoveryServer();

/**
 * @brief   Register request callbacks and response callbacks.
 *          Requests and responses are delivered these callbacks .
 * @param   ReqHandler   [IN] Request callback ( for GET,PUT ..etc)
 * @param   RespHandler  [IN] Response Handler Callback
 * @see     CARequestCallback
 * @see     CAResponseCallback
 * @see     CAErrorCallback
 * @return  NONE
 */
void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler,
                       CAErrorCallback ErrorHandler);

#ifdef __WITH_DTLS__
/**
 * @brief   Register callback to get DTLS PSK credentials.
 * @param   GetDTLSCredentials   [IN] GetDTLS Credetials callback
 * @return  #CA_STATUS_OK
 */
CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSCredentialsHandler GetDTLSCredentials);
#endif //__WITH_DTLS__

/**
 * @brief   Create an endpoint description
 * @param   flags               [IN]  how the adapter should be used
 * @param   adapter             [IN]  which adapter to use
 * @param   addr                [IN]  string representation of address
 * @param   port                [IN]  port (for IP_ADAPTER)
 * @param   endpoint            [OUT] Endpoint which contains the above
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED
 * @remark  The created Remote endpoint can be freed using CADestroyEndpoint().
 * @see     CADestroyEndpoint
 */
CAResult_t CACreateEndpoint(CATransportFlags_t flags,
                            CATransportAdapter_t adapter,
                            const char *addr,
                            uint16_t port,
                            CAEndpoint_t **object);

/**
 * @brief   Destroy the remote endpoint created
 * @param   object  [IN] Remote Endpoint object created with CACreateEndpoint
 * @return  NONE
 */
void CADestroyEndpoint(CAEndpoint_t *object);

/**
 * @brief   Generating the token for matching the request and response.
 * @param   token          [OUT] Token for the request
 * @param   tokenLength    [IN]  length of the token
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED or #CA_MEMORY_ALLOC_FAILED
 *          or #CA_STATUS_NOT_INITIALIZED
 * @remark  Token memory is destroyed by the caller using CADestroyToken().
 * @see     CADestroyToken
 */
CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength);

/**
 * @brief   Destroy the token generated by CAGenerateToken
 * @param   token   [IN] token to be freed
 * @return  NONE
 */
void CADestroyToken(CAToken_t token);

/**
 * @brief   Send control Request on a resource
 * @param   object      [IN] Endpoint where the payload need to be sent.
 *                           This endpoint is delivered with Request or response callback.
 * @param   requestInfo [IN] Information for the request.
 * @return  #CA_STATUS_OK #CA_STATUS_FAILED #CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo);

/**
 * @brief   Send the response
 * @param   object          [IN] Endpoint where the payload need to be sent.
 *                               This endpoint is delivered with Request or response callback
 * @param   responseInfo    [IN] Information for the response
 * @return  #CA_STATUS_OK or  #CA_STATUS_FAILED or #CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CASendResponse(const CAEndpoint_t *object, const CAResponseInfo_t *responseInfo);

/**
 * @brief   Send notification to the remote object
 * @param   object          [IN] Endpoint where the payload need to be sent.
 *                               This endpoint is delivered with Request or response callback.
 * @param   responseInfo    [IN] Information for the response.
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED or #CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CASendNotification(const CAEndpoint_t *object,
                      const  CAResponseInfo_t *responseInfo);

/**
 * @brief   Select network to use
 * @param   interestedNetwork   [IN] Connectivity Type enum
 * @return  #CA_STATUS_OK or #CA_NOT_SUPPORTED or #CA_STATUS_FAILED or #CA_NOT_SUPPORTED
 */
CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork);

/**
 * @brief   Select network to unuse
 * @param   nonInterestedNetwork    [IN] Connectivity Type enum
 * @return  #CA_STATUS_OK or #CA_NOT_SUPPORTED or #CA_STATUS_FAILED
 */
CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork);

/**
 * @brief   Get network information
 *          It should be destroyed by the caller as it Get Information.
 * @param   info    [OUT] LocalConnectivity objects
 * @param   size    [OUT] No Of Array objects
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED or #CA_STATUS_INVALID_PARAM or
*                #CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CAGetNetworkInformation(CAEndpoint_t **info, uint32_t *size);

/**
 * @brief    To Handle the Request or Response
 * @return   #CA_STATUS_OK
 */
CAResult_t CAHandleRequestResponse();

#ifdef RA_ADAPTER
/**
 * @brief   Set Remote Access information for XMPP Client.
 * @param   caraInfo          [IN] remote access info.
 *
 * @return  #CA_STATUS_OK
 */
CAResult_t CASetRAInfo(const CARAInfo_t *caraInfo);
#endif


#ifdef __WITH_DTLS__

/**
 * Select the cipher suite for dtls handshake
 *
 * @param[IN] cipher  cipher suite (Note : Make sure endianness)
 *                               0xC018 : TLS_ECDH_anon_WITH_AES_128_CBC_SHA
 *                               0xC0A8 : TLS_PSK_WITH_AES_128_CCM_8
 *                               0xC0AE : TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
 *
 * @retval  CA_STATUS_OK    Successful
 * @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval  CA_STATUS_FAILED Operation failed
 */
CAResult_t CASelectCipherSuite(const uint16_t cipher);

/**
 * Enable TLS_ECDH_anon_WITH_AES_128_CBC_SHA cipher suite in dtls
 *
 * @param[IN] enable  TRUE/FALSE enables/disables anonymous cipher suite
 *
 * @retval  CA_STATUS_OK    Successful
 * @retval  CA_STATUS_FAILED Operation failed
 *
 * @note anonymous cipher suite should only be enabled for 'JustWorks' provisioning.
 */
CAResult_t CAEnableAnonECDHCipherSuite(const bool enable);


/**
 * Generate ownerPSK using PRF
 * OwnerPSK = TLS-PRF('master key' , 'oic.sec.doxm.jw',
 *                                    'ID of new device(Resource Server)',
 *                                    'ID of owner smart-phone(Provisioning Server)')
 *
 * @param[IN] endpoint  information of network address
 * @param[IN] label  Ownership transfer method e.g)"oic.sec.doxm.jw"
 * @param[IN] labelLen  Byte length of label
 * @param[IN] rsrcServerDeviceID  ID of new device(Resource Server)
 * @param[IN] rsrcServerDeviceIDLen  Byte length of rsrcServerDeviceID
 * @param[IN] provServerDeviceID  label of previous owner
 * @param[IN] provServerDeviceIDLen  byte length of provServerDeviceID
 * @param[IN,OUT] ownerPSK  Output buffer for owner PSK
 * @param[IN] ownerPSKSize  Byte length of the ownerPSK to be generated
 *
 * @retval  CA_STATUS_OK    Successful
 * @retval  CA_STATUS_FAILED Operation failed
 */
CAResult_t CAGenerateOwnerPSK(const CAEndpoint_t *endpoint,
                              const uint8_t* label, const size_t labelLen,
                              const uint8_t* rsrcServerDeviceID,
                              const size_t rsrcServerDeviceIDLen,
                              const uint8_t* provServerDeviceID,
                              const size_t provServerDeviceIDLen,
                              uint8_t* ownerPSK, const size_t ownerPSKSize);

/**
 * Initiate DTLS handshake with selected cipher suite
 *
 * @param[IN] endpoint  information of network address
 *
 * @retval  CA_STATUS_OK    Successful
 * @retval  CA_STATUS_FAILED Operation failed
 */
CAResult_t CAInitiateHandshake(const CAEndpoint_t *endpoint);

/**
 * Close the DTLS session
 *
 * @param[IN] endpoint  information of network address
 *
 * @retval  CA_STATUS_OK    Successful
 * @retval  CA_STATUS_FAILED Operation failed
 */
CAResult_t CACloseDtlsSession(const CAEndpoint_t *endpoint);

#endif /* __WITH_DTLS__ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_INTERFACE_H_ */

