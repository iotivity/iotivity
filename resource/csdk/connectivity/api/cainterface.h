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
 * Callback function type for request delivery.
 * @param[out]   object       Endpoint object from which the request is received.
 *                            It contains endpoint address based on the connectivity type.
 * @param[out]   requestInfo  Info for resource model to understand about the request.
 */
typedef void (*CARequestCallback)(const CAEndpoint_t *object,
                                  const CARequestInfo_t *requestInfo);

/**
 * Callback function type for response delivery.
 * @param[out]   object           Endpoint object from which the response is received.
 * @param[out]   responseInfo     Identifier which needs to be mapped with response.
 */
typedef void (*CAResponseCallback)(const CAEndpoint_t *object,
                                   const CAResponseInfo_t *responseInfo);
/**
 * Callback function type for error.
 * @param[out]   object           remote device information.
 * @param[out]   errorInfo        CA Error information.
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
   unsigned char identity[DTLS_PSK_ID_LEN]; /** identity of self. */
   uint32_t num;                            /** number of credentials in this blob. */
   OCDtlsPskCreds *creds;                   /** list of credentials. Size of this
                                                array is determined by 'num' variable. */
} CADtlsPskCredsBlob_t;

/**
 * Callback function type for getting DTLS credentials.
 * @param[out]   credInfo     DTLS credentials info. Handler has to allocate new memory for.
 *                            both credInfo and credInfo->creds which is then freed by CA.
 */
typedef void (*CAGetDTLSCredentialsHandler)(CADtlsPskCredsBlob_t **credInfo);
#endif //__WITH_DTLS__

/**
 * Initialize the connectivity abstraction module.
 * It will initialize adapters, thread pool and other modules based on the platform
 * compilation options.
 *
 * @return  ::CA_STATUS_OK or ::CA_STATUS_FAILED or ::CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CAInitialize();

/**
 * Terminate the connectivity abstraction module.
 * All threads, data structures are destroyed for next initializations.
 */
void CATerminate();

/**
 * Starts listening servers.
 * This API is used by resource hosting server for listening multicast requests.
 * Based on the adapters configurations, different kinds of servers are started.
 * @return  ::CA_STATUS_OK or ::CA_STATUS_FAILED
 */
CAResult_t CAStartListeningServer();

/**
 * Starts discovery servers.
 * This API is used by resource required clients for listening multicast requests.
 * Based on the adapters configurations, different kinds of servers are started.
 * @return  ::CA_STATUS_OK or ::CA_STATUS_FAILED
 */
CAResult_t CAStartDiscoveryServer();

/**
 * Register request callbacks and response callbacks.
 *          Requests and responses are delivered these callbacks.
 * @param[in]   ReqHandler    Request callback ( for GET,PUT ..etc).
 * @param[in]   RespHandler   Response Handler Callback.
 * @see     CARequestCallback
 * @see     CAResponseCallback
 * @see     CAErrorCallback
 */
void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler,
                       CAErrorCallback ErrorHandler);

#ifdef __WITH_DTLS__
/**
 * Register callback to get DTLS PSK credentials.
 * @param[in]   GetDTLSCredentials    GetDTLS Credetials callback.
 * @return  ::CA_STATUS_OK
 */
CAResult_t CARegisterDTLSCredentialsHandler(CAGetDTLSCredentialsHandler GetDTLSCredentials);
#endif //__WITH_DTLS__

/**
 * Create an endpoint description.
 * @param[in]   flags                 how the adapter should be used.
 * @param[in]   adapter               which adapter to use.
 * @param[in]   addr                  string representation of address.
 * @param[in]   port                  port (for IP_ADAPTER).
 * @param[in]   endpoint              Endpoint which contains the above.
 * @return  ::CA_STATUS_OK or ::CA_STATUS_FAILED
 * @remark  The created Remote endpoint can be freed using CADestroyEndpoint().
 * @see     CADestroyEndpoint
 */
CAResult_t CACreateEndpoint(CATransportFlags_t flags,
                            CATransportAdapter_t adapter,
                            const char *addr,
                            uint16_t port,
                            CAEndpoint_t **object);

/**
 * Destroy the remote endpoint created.
 * @param[in]   object   Remote Endpoint object created with CACreateEndpoint.
 */
void CADestroyEndpoint(CAEndpoint_t *object);

/**
 * Generating the token for matching the request and response.
 * @param[in]   token            Token for the request.
 * @param[in]   tokenLength      length of the token.
 * @return  ::CA_STATUS_OK or ::CA_STATUS_FAILED or
 *          ::CA_MEMORY_ALLOC_FAILED or ::CA_STATUS_NOT_INITIALIZED
 * @remark  Token memory is destroyed by the caller using CADestroyToken().
 * @see     CADestroyToken
 */
CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength);

/**
 * Destroy the token generated by CAGenerateToken.
 * @param[in]   token    token to be freed.
 */
void CADestroyToken(CAToken_t token);

/**
 * Send control Request on a resource.
 * @param[in]   object       Endpoint where the payload need to be sent.
 *                           This endpoint is delivered with Request or response callback.
 * @param[in]   requestInfo  Information for the request.
 * @return  ::CA_STATUS_OK ::CA_STATUS_FAILED ::CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CASendRequest(const CAEndpoint_t *object, const CARequestInfo_t *requestInfo);

/**
 * Send the response.
 * @param[in]   object           Endpoint where the payload need to be sent.
 *                               This endpoint is delivered with Request or response callback.
 * @param[in]   responseInfo     Information for the response.
 * @return  ::CA_STATUS_OK or  ::CA_STATUS_FAILED or ::CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CASendResponse(const CAEndpoint_t *object, const CAResponseInfo_t *responseInfo);

/**
 * Send notification to the remote object.
 * @param[in]   object           Endpoint where the payload need to be sent.
 *                               This endpoint is delivered with Request or response callback.
 * @param[in]   responseInfo     Information for the response.
 * @return  ::CA_STATUS_OK or ::CA_STATUS_FAILED or ::CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CASendNotification(const CAEndpoint_t *object,
                      const  CAResponseInfo_t *responseInfo);

/**
 * Select network to use.
 * @param[in]   interestedNetwork    Connectivity Type enum.
 * @return  ::CA_STATUS_OK or ::CA_NOT_SUPPORTED or
 *          ::CA_STATUS_FAILED or ::CA_NOT_SUPPORTED
 */
CAResult_t CASelectNetwork(CATransportAdapter_t interestedNetwork);

/**
 * Select network to unuse.
 * @param[in]   nonInterestedNetwork     Connectivity Type enum.
 * @return  ::CA_STATUS_OK or ::CA_NOT_SUPPORTED or ::CA_STATUS_FAILED
 */
CAResult_t CAUnSelectNetwork(CATransportAdapter_t nonInterestedNetwork);

/**
 * Get network information.
 * It should be destroyed by the caller as it Get Information.
 * @param[out]   info     LocalConnectivity objects
 * @param[out]   size     No Of Array objects
 * @return  ::CA_STATUS_OK or ::CA_STATUS_FAILED or
 *          ::CA_STATUS_INVALID_PARAM or ::CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CAGetNetworkInformation(CAEndpoint_t **info, uint32_t *size);

/**
 * To Handle the Request or Response.
 * @return   ::CA_STATUS_OK
 */
CAResult_t CAHandleRequestResponse();

#ifdef RA_ADAPTER
/**
 * Set Remote Access information for XMPP Client.
 * @param[in]   caraInfo          remote access info.
 *
 * @return  ::CA_STATUS_OK
 */
CAResult_t CASetRAInfo(const CARAInfo_t *caraInfo);
#endif


#ifdef __WITH_DTLS__

/**
 * Select the cipher suite for dtls handshake.
 *
 * @param[in] cipher  cipher suite (Note : Make sure endianness).
 *                    0xC018 : TLS_ECDH_anon_WITH_AES_128_CBC_SHA
 *                    0xC0A8 : TLS_PSK_WITH_AES_128_CCM_8
 *                    0xC0AE : TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
 *
 * @retval  ::CA_STATUS_OK    Successful.
 * @retval  ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval  ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CASelectCipherSuite(const uint16_t cipher);

/**
 * Enable TLS_ECDH_anon_WITH_AES_128_CBC_SHA cipher suite in dtls.
 *
 * @param[in] enable  TRUE/FALSE enables/disables anonymous cipher suite.
 *
 * @retval  ::CA_STATUS_OK    Successful.
 * @retval  ::CA_STATUS_FAILED Operation failed.
 *
 * @note anonymous cipher suite should only be enabled for 'JustWorks' provisioning.
 */
CAResult_t CAEnableAnonECDHCipherSuite(const bool enable);


/**
 * Generate ownerPSK using PRF.
 * OwnerPSK = TLS-PRF('master key' , 'oic.sec.doxm.jw',
 *                    'ID of new device(Resource Server)',
 *                    'ID of owner smart-phone(Provisioning Server)')
 *
 * @param[in] endpoint  information of network address.
 * @param[in] label  Ownership transfer method e.g)"oic.sec.doxm.jw".
 * @param[in] labelLen  Byte length of label.
 * @param[in] rsrcServerDeviceID  ID of new device(Resource Server).
 * @param[in] rsrcServerDeviceIDLen  Byte length of rsrcServerDeviceID.
 * @param[in] provServerDeviceID  label of previous owner.
 * @param[in] provServerDeviceIDLen  byte length of provServerDeviceID.
 * @param[in,out] ownerPSK  Output buffer for owner PSK.
 * @param[in] ownerPSKSize  Byte length of the ownerPSK to be generated.
 *
 * @retval  ::CA_STATUS_OK    Successful.
 * @retval  ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CAGenerateOwnerPSK(const CAEndpoint_t *endpoint,
                              const uint8_t* label, const size_t labelLen,
                              const uint8_t* rsrcServerDeviceID,
                              const size_t rsrcServerDeviceIDLen,
                              const uint8_t* provServerDeviceID,
                              const size_t provServerDeviceIDLen,
                              uint8_t* ownerPSK, const size_t ownerPSKSize);

/**
 * Initiate DTLS handshake with selected cipher suite.
 *
 * @param[in] endpoint  information of network address.
 *
 * @retval  ::CA_STATUS_OK    Successful.
 * @retval  ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CAInitiateHandshake(const CAEndpoint_t *endpoint);

/**
 * Close the DTLS session.
 *
 * @param[in] endpoint  information of network address.
 *
 * @retval  ::CA_STATUS_OK    Successful.
 * @retval  ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CACloseDtlsSession(const CAEndpoint_t *endpoint);

#endif /* __WITH_DTLS__ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_INTERFACE_H_ */

