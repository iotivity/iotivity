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
 * This file contains common utility function for remote endpoints.
 */

#ifndef CA_REMOTE_HANDLER_H_
#define CA_REMOTE_HANDLER_H_

#include "cacommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Creates a new remote endpoint from the input endpoint
 * @param   endpoint       [IN]    endpoint information where the data has to be sent
 * @return  remote endpoint created
 */
CARemoteEndpoint_t *CACloneRemoteEndpoint(const CARemoteEndpoint_t *endpoint);

/**
 * @brief   Creates a new remote endpoint from the input uri
 * @param   uri                 [IN]    absolute uri information to create remote endpoint
 * @param   transportType       [IN]    transport type of the endpoint
 * @return  remote endpoint created
 */
CARemoteEndpoint_t *CACreateRemoteEndpointUriInternal(const CAURI_t uri,
                                                      const CATransportType_t transportType);

/**
 * @brief   Creates a new remote endpoint from the input and other information
 * @param   resourceUri         [IN]    absolute uri information to create remote endpoint
 * @param   addr                [IN]    address of the endpoint
 * @param   type                [IN]    transport  type of the endpoint
 * @return  remote endpoint created
 */
CARemoteEndpoint_t *CACreateRemoteEndpointInternal(const CAURI_t resourceUri,
                                                   const CAAddress_t addr,
                                                   const CATransportType_t type);

/**
 * @brief   Destroy remote endpoint
 * @param   endpoint       [IN]    endpoint information where the data has to be sent
 * @return  none
 */
void CADestroyRemoteEndpointInternal(CARemoteEndpoint_t *rep);

/**
 * @brief   Creates a new request information
 * @param   request       [IN]    request information that needs to be duplicated
 * @return  remote endpoint created
 */
CARequestInfo_t *CACloneRequestInfo(const CARequestInfo_t *request);

/**
 * @brief   Destroy the request information
 * @param   request       [IN]    request information that needs to be destroyed
 * @return none
 */
void CADestroyRequestInfoInternal(CARequestInfo_t *request);

/**
 * @brief   Creates a new response information
 * @param   response       [IN]    response information that needs to be duplicated
 * @return  remote endpoint created
 */
CAResponseInfo_t *CACloneResponseInfo(const CAResponseInfo_t *response);

/**
 * @brief   Destroy the response information
 * @param   response       [IN]    response information that needs to be destroyed
 * @return
 */
void CADestroyResponseInfoInternal(CAResponseInfo_t *response);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_REMOTE_HANDLER_H_ */

