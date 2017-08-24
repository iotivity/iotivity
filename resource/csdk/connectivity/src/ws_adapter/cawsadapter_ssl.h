/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
 * This file contains the APIs for WS Adapter's security layer.
 */
#ifndef CA_WS_ADAPTER_SSL_H_
#define CA_WS_ADAPTER_SSL_H_

#include "cacommon.h"
#include "casecurityinterface.h"

#include "openssl/ssl.h"
#include "openssl/err.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * API to initialize WS adapter security.
 *
 * @return  ::CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAWSInitSSLAdapter();

/**
 * API to de-initialize WS adapter security.
 *
 * @return  ::CA_STATUS_OK or Appropriate error code
 */
void CAWSDeInitSSLAdapter();

/**
 * API to register callback to supply X.509-based Public Key Infrastructure.
 * This callback will be invoked while configuring SSL_CTX for certificates.
 *
 * @param[in] infoCallback      Callback to supply PKIX.
 */
void CAWSSetPkixInfoCallback(CAgetPkixInfoHandler infoCallback);

/**
 * API to register callback to supply credential types configured in security layer.
 *
 * @param[in] credTypesCallback      Callback to supply credential types.
 */
void CAWSSetCredentialTypesCallback(CAgetCredentialTypesHandler credTypesCallback);

/**
 * API to register callback to supply PSK identity/key.
 *
 * @param[in] credCallback      Callback to supply pre-shared key and its identity.
 */
void CAWSSetPskCredentialsCallback(CAgetPskCredentialsHandler credCallback);

/**
 * API to Initialize client.
 *
 * @param[in] ctx      OpenSSL context.
 */
void CAWSInitSSLClient(SSL_CTX *ctx);

/**
 * API to configure client's context for SSL communication.
 *
 * @param[in] deviceId      Remote device id.
 *
 * @return  ::CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAWSSetupCipher(const char *deviceId);

/**
 * API to set cipher suite for client's context.
 *
 * @param[in] cipher      cipher value as defined in mbedTLS.
 *
 * @return  ::CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAWSSetCipherSuite(const uint16_t cipher);

/**
 * API to Initialize server.
 *
 * @param[in] ctx      OpenSSL context.
 */
void CAWSInitSSLServer(SSL_CTX *ctx);

/**
 * API to get peer's identity shared at the time of SSL connection establishment.
 *
 * @param[in] endpoint      Peer's endpoint info.
 *
 * @return  ::CARemoteId_t
 */
CARemoteId_t CAWSGetPeerIdentity(CAEndpoint_t *endpoint);

#ifdef __cplusplus
}
#endif

#endif
