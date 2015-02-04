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
 * @file cainterface.h
 * @brief This file contains the APIs for Resource Model to use
 */

#ifndef __CA_INTERFACE_H_
#define __CA_INTERFACE_H_

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
typedef void (*CARequestCallback)(const CARemoteEndpoint_t *object,
                                  const CARequestInfo_t *requestInfo);

/**
 * @brief   Callback function type for response delivery.
 * @param   object          [OUT] Endpoint object from which the response is received.
 * @param   responseInfo    [OUT] Identifier which needs to be mapped with response.
 * @return  NONE
 */
typedef void (*CAResponseCallback)(const CARemoteEndpoint_t *object,
                                   const CAResponseInfo_t *responseInfo);

#ifdef __WITH_DTLS__
/**
 * @brief   Callback function type for getting DTLS credentials.
 * @param   credInfo          [OUT] DTLS credentials info
 * @return  NONE
 */
typedef void (*CAGetDTLSCredentialsHandler)(OCDtlsPskCredsBlob **credInfo);
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
 * @return  NONE
 */
void CARegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler);

#ifdef __WITH_DTLS__
/**
 * @brief   Register callback to get DTLS PSK credentials.
 * @param   GetDTLSCredentials   [IN] GetDTLS Credetials callback
 * @return  #CA_STATUS_OK
 */
CAResult_t CARegisterDTLSCredentialsHandler(
                                                   CAGetDTLSCredentialsHandler GetDTLSCredentials);
#endif //__WITH_DTLS__

/**
 * @brief   Create a Remote endpoint if the URI is available already.
 *          This is a Helper function which can be used before calling
 *          CASendRequest / CASendNotification.
 * @param   uri                 [IN]  Absolute URI of the resource to be used to generate the
 *                                    Remote endpoint
 *                                    \n For ex : coap://10.11.12.13:4545/resource_uri ( for IP)
 *                                    \n coap://10:11:12:13:45:45/resource_uri ( for BT)
 * @param   connectivityType    [IN]  Connectivity type of the endpoint
 * @param   object              [OUT] Endpoint object which contains the above parsed data
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED
 * @remark  The created Remote endpoint can be freed using CADestroyRemoteEndpoint() API.
 * @see     CADestroyRemoteEndpoint
 */
CAResult_t CACreateRemoteEndpoint(const CAURI_t uri,
                                  const CAConnectivityType_t connectivityType,
                                  CARemoteEndpoint_t **object);

/**
 * @brief   Destroy the remote endpoint created
 * @param   object  [IN] Remote Endpoint object created with CACreateRemoteEndpoint
 * @return  NONE
 */
void CADestroyRemoteEndpoint(CARemoteEndpoint_t *object);

/**
 * @brief   Generating the token for matching the request and response.
 * @param   token   [OUT] Token for the request
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED or #CA_MEMORY_ALLOC_FAILED
 * @remark  Token memory is destroyed by the caller using CADestroyToken().
 * @see     CADestroyToken
 */
CAResult_t CAGenerateToken(CAToken_t *token);

/**
 * @brief   Destroy the token generated by CAGenerateToken
 * @param   token   [IN] Token for the request
 * @return  NONE
 */
void CADestroyToken(CAToken_t token);

/**
 * @brief   Find the resource in the network. This API internally sends multicast messages on all
 *          selected connectivity adapters. Responses are delivered via response callbacks.
 *
 * @param   resourceUri [IN] Uri to send multicast search request. Must contain only relative
 *                           path of Uri to be search.
 * @param   token       [IN] Token for the request
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED
 */
CAResult_t CAFindResource(const CAURI_t resourceUri, const CAToken_t token);

/**
 * @brief   Send control Request on a resource
 * @param   object      [IN] Remote Endpoint where the payload need to be sent.
 *                           This Remote endpoint is delivered with Request or response callback.
 * @param   requestInfo [IN] Information for the request.
 * @return  #CA_STATUS_OK #CA_STATUS_FAILED #CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CASendRequest(const CARemoteEndpoint_t *object,const CARequestInfo_t *requestInfo);

/**
 * @brief   Send control Request on a resource to multicast group
 * @param   object      [IN] Group Endpoint where the payload need to be sent.
 *                           This Remote endpoint is delivered with Request or response callback.
 * @param   requestInfo [IN] Information for the request.
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED or #CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CASendRequestToAll(const CAGroupEndpoint_t *object,
                              const CARequestInfo_t *requestInfo);

/**
 * @brief   Send the response
 * @param   object          [IN] Remote Endpoint where the payload need to be sent.
 *                               This Remote endpoint is delivered with Request or response callback
 * @param   responseInfo    [IN] Information for the response
 * @return  #CA_STATUS_OK or  #CA_STATUS_FAILED or #CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CASendResponse(const CARemoteEndpoint_t *object,
                const CAResponseInfo_t *responseInfo);

/**
 * @brief   Send notification to the remote object
 * @param   object          [IN] Remote Endpoint where the payload need to be sent.
 *                               This Remote endpoint is delivered with Request or response callback.
 * @param   responseInfo    [IN] Information for the response.
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED or #CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CASendNotification(const CARemoteEndpoint_t *object,
                      const  CAResponseInfo_t *responseInfo);

/**
 * @brief   To advertise the resource
 * @param   resourceUri       [IN] URI to be advertised
 * @param   token               [IN] Token for the request
 * @param   options             [IN] Header options information
 * @param   numOptions      [IN] Number of options
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED or #CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CAAdvertiseResource(const CAURI_t resourceUri,const CAToken_t token,
                               const CAHeaderOption_t *options,const uint8_t numOptions);

/**
 * @brief   Select network to use
 * @param   interestedNetwork   [IN] Connectivity Type enum
 * @return  #CA_STATUS_OK or #CA_NOT_SUPPORTED or #CA_STATUS_FAILED or #CA_NOT_SUPPORTED
 */
CAResult_t CASelectNetwork(const uint32_t interestedNetwork);

/**
 * @brief   Select network to unuse
 * @param   nonInterestedNetwork    [IN] Connectivity Type enum
 * @return  #CA_STATUS_OK or #CA_NOT_SUPPORTED or #CA_STATUS_FAILED
 */
CAResult_t CAUnSelectNetwork(const uint32_t nonInterestedNetwork);

/**
 * @brief   Get network information
 *          It should be destroyed by the caller as it Get Information.
 * @param   info    [OUT] LocalConnectivity objects
 * @param   size    [OUT] No Of Array objects
 * @return  #CA_STATUS_OK or #CA_STATUS_FAILED or #CA_STATUS_INVALID_PARAM or
*                #CA_MEMORY_ALLOC_FAILED
 */
CAResult_t CAGetNetworkInformation(CALocalConnectivity_t **info, uint32_t *size);

/**
 * @brief    To Handle the Request or Response
 * @return   #CA_STATUS_OK
 */
CAResult_t CAHandleRequestResponse();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

