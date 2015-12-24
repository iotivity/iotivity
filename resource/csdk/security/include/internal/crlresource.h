//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef IOTVT_SRM_CRLR_H
#define IOTVT_SRM_CRLR_H

#include "octypes.h"
#include "securevirtualresourcetypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function stores CRL in SRM
 * @param crl - CRL
 *
 * @returns OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult UpdateCRLResource(const OicSecCrl_t *crl);

/**
 * This function get encoded with base64 CRL from SRM
 *
 * @returns encoded CRL with base64 format. NULL if error occured (e.g. CRL did not set)
 * @note Caller responsible for resulting string memory (use OICFree to remove it)
 */
char* GetBase64CRL();
/**
 * This function get encoded with DER CRL from SRM
 *
 * @returns encoded CRL with DER format. array len is 0 if error occured (e.g. CRL did not set)
 */
void  GetDerCrl(ByteArray crlArray);

/**
 * This method converts JSON CRL into binary CRL.
 * The JSON CRL can be from persistent database or received as PUT/POST request.
 *
 * @param[in] jsonStr  CRL data in json string.
 * @param[in] isIncResName if resource name is included into payload, it is true.
 * @return pointer to OicSecCrl_t.
 *
 * @note Caller needs to invoke OCFree after done using the return pointer
 */
OicSecCrl_t * JSONToCrlBin(const char * jsonStr, const bool isIncResName);

/**
 * Initialize CLR resource by loading data from persistent storage.
 *
 * @returns OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitCRLResource();

/**
 * Perform cleanup for CRL resources.
 */
void DeInitCRLResource();

OicSecCrl_t *GetCRLResource();

OCEntityHandlerResult CRLEntityHandler(OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest,
                                        void* callbackParameter);
#ifdef __cplusplus
}
#endif

#endif //IOTVT_SRM_CRLR_H


