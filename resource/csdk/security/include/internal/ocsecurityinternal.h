//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#ifndef OC_SECURITY_INTERNAL_H
#define OC_SECURITY_INTERNAL_H

#include "ocsecurityconfig.h"


#ifdef CA_SEC_MERGE_WORKAROUND
/**
 * This is a workaround to enable CA merge into master branch.
 * This will be removed by updating code in CA library to use updated data structure.
 */
typedef struct
{
   uint32_t unused;
   unsigned char identity[DTLS_PSK_ID_LEN];
   uint32_t num;
   OCDtlsPskCreds *creds;
} CADtlsPskCredsBlob;
#endif //CA_SEC_MERGE_WORKAROUND


/**
 * This callback is used by lower stack (i.e. CA layer) to retrieve PSK
 * credentials from RI security layer.
 *
 * Note: When finished, caller should initialize memory to zeroes and
 * invoke OCFree to delete @p credInfo.
 *
 * @param credInfo
 *     binary blob containing PSK credentials
 *
 * @retval none
 */
void GetDtlsPskCredentials(OCDtlsPskCredsBlob **credInfo);


/**
 * This internal API removes/clears the global variable holding the security
 * config data. This needs to be invoked when OIC stack is shutting down.
 *
 * @retval none
 */
void DeinitOCSecurityInfo();

#endif //OC_SECURITY_INTERNAL_H
