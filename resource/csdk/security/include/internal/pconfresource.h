/* *****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/

#ifndef IOTVT_SRM_PCONF_H
#define IOTVT_SRM_PCONF_H

#include "octypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize PCONF resource by loading data from persistent storage.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitPconfResource();

/**
 * Perform cleanup for PCONF resources.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult DeInitPconfResource();

/**
 * This method is used by SRM to retrieve PCONF resource data..
 *
 * @retval  reference to @ref OicSecPconf_t, binary format of Pconf resource data
 */
const OicSecPconf_t* GetPconfResourceData();

/**
 * This method converts JSON PCONF into binary PCONF.
 * The JSON PCONF can be from persistent database or
 * or received as PUT request.
 *
 * @param[in] jsonStr  pconf data in json string.
 * @return pointer to OicSecPconf_t.
 *
 * @note Caller needs to invoke OCFree after done
 *       using the return pointer
 */
OicSecPconf_t * JSONToPconfBin(const char * jsonStr);

/**
 * This method converts PCONF data into JSON format.
 * Caller needs to invoke 'free' when finished done using
 * return string
 *
 * @param[in] pconf  Pointer to OicSecPconf_t.
 * @return pointer to json string.
 *
 * @note Caller needs to invoke OCFree after done
 *       using the return pointer
 */
char * BinToPconfJSON(const OicSecPconf_t * pconf);

/**
 * This method might be used to add a paired device id after direct-pairing process complete.
 *
 * @param pdeviceId ID of the paired device.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult AddPairedDevice(OicUuid_t *pdeviceId);

/**
 * This method might be used by PolicyEngine to retrieve PDACL for a Subject.
 *
 * @param subjectId ID of the subject for which PDACL is required.
 * @param savePtr is used internally by @ref GetACLResourceData to maintain index between
 *                successive calls for same subjectId.
 *
 * @retval  reference to @ref OicSecPdAcl_t if PDACL is found, else NULL
 */
const OicSecPdAcl_t* GetPdAclData(const OicUuid_t* subjectId, OicSecPdAcl_t **savePtr);

/**
 * This method return whether device is paired or not.
 *
 * @param pdeviceId Target device ID to find in paired list.
 * @retval  ture if device is already paired, else false
 */
bool IsPairedDevice(const OicUuid_t* pdeviceId);

/** This function deallocates the memory for OicSecPconf_t .
 *
 * @param[in] pconf  Pointer to OicSecPconf_t.
 */
void DeletePconfBinData(OicSecPconf_t* pconf);

/**
 * This function frees OicSecPdAcl_t object's fields and object itself.
 */
void FreePdAclList(OicSecPdAcl_t* pdacls);


#ifdef __cplusplus
}
#endif

#endif //IOTVT_SRM_PCONF_H



