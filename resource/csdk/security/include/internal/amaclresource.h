//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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

#ifndef IOTVT_SRM_AMACLR_H
#define IOTVT_SRM_AMACLR_H

/**
 * @file
 *
 * This file contains the APIs for the /oic/sec/amacl resource is an ACL structure that
 * specifies which resources will use an Access Manager Service (AMS) to resolve access decisions.
 * It dynamically obtains an ACL using an AMS.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize Amacl resource by loading data from persistent storage.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitAmaclResource();

/**
 * Perform cleanup for Amacl resources.
 *
 * @retval  none
 */
void DeInitAmaclResource();

/**
 * This method is used by PolicyEngine to retrieve Amacl for a Subject.
 *
 * @param subjectId ID of the subject for which Amacl is required.
 * @param savePtr is used internally by @ref GetAmaclResourceData to maintain index between
 *                successive calls for same subjectId.
 *
 * @retval  reference to @ref OicSecAmacl_t if Amacl is found, else NULL
 *
 * @note On the first call to @ref GetAmaclResourceData, savePtr should point to NULL
 */
const OicSecAmacl_t* GetAmaclResourceData(const OicUuid_t* subjectId, OicSecAmacl_t **savePtr);

/**
 * This function converts Amacl data into JSON format.
 * Caller needs to invoke 'free' when done using
 * returned string.
 * @param Amacl  instance of OicSecAmacl_t structure.
 *
 * @retval  pointer to Amacl in json format.
 */
char* BinToAmaclJSON(const OicSecAmacl_t * amacl);

#ifdef __cplusplus
}
#endif

#endif //IOTVT_SRM_AMACLR_H


