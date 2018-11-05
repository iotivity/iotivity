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

#ifndef U_PARSECHAIN_H_
#define U_PARSECHAIN_H_

#include <mbedtls/ssl.h>
#include "casecurityinterface.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Parse chain of X.509 certificates.
 *
 * @param[out] crt     container for X.509 certificates
 * @param[in]  certs   array of X.509 certificates
 * @param[in]  errNum  number of certificates that failed to parse
 *
 * @return  number of successfully parsed certificates or -1 on error
 */
int ParseChain(mbedtls_x509_crt *crt, const ByteArrayLL_t *certs, int *errNum);

/**
 * Free chain of X.509 certificates.
 *
 * @param[in]  certs   array of X.509 certificates
 */
void FreeCertChain(ByteArrayLL_t *certs);

#ifdef __cplusplus
}
#endif

#endif /* U_ARRAYLIST_H_ */
