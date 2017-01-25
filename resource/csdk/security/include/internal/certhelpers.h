//******************************************************************
//
// Copyright 2017 Microsoft
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//******************************************************************

#ifndef IOTVT_SRM_CERTHELPERS_H_
#define IOTVT_SRM_CERTHELPERS_H_

#if defined(__WITH_TLS__) || defined(__WITH_DTLS__)

#include "mbedtls/pk.h"

/**
 * Internal certificate request function; used by CSR resource handler
 *
 * @param[in]  subject  Subject to put into the CSR
 * @param[in]  keyPair  Key pair for which to generate the CSR
 * @param[in]  encoding Encoding to use; valid values are OIC_ENCODING_PEM and OIC_ENCODING_DER
 * @param[out] csr      OCByteString containing the CSR in PEM. Caller takes ownership
 *                      of allocated memory and must call OICFree on csr->bytes when finished.
 *                      If encoding is OIC_ENCODING_DER, csr->len is the length of the encoded CSR.
 *                      If encoding is OIC_ENCODING_PEM, csr->len is the length of the PEM string including the
 *                      terminating NULL.
 *
 * @return 0 on success, <0 on failure
 */
int OCInternalCSRRequest(const char *subject, mbedtls_pk_context *keyPair, 
                         OicEncodingType_t encoding, OCByteString *csr);

/**
 * Internal key pair generation function; used by CSR resource handler
 *
 * @param[in,out] keyPair Key object to generate; caller should already have called mbedtls_pk_init on it.
 *
 * @return 0 on success, <0 on failure
 */
int OCInternalGenerateKeyPair(mbedtls_pk_context *keyPair);

#endif

#endif
