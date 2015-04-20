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

#ifndef IOTVT_SRM_ACLR_H
#define IOTVT_SRM_ACLR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize ACL resource by loading data from persistent storage.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitACLResource();

/**
 * Perform cleanup for ACL resources.
 *
 * @retval  none
 */
void DeInitACLResource();

/**
 * This method is used by PolicyEngine to retrieve ACL for a Subject.
 *
 * @param subjectId - ID of the subject for which ACL is required.
 *
 * @retval  reference to @ref OicSecAcl_t if ACL is found, else NULL
 */
const OicSecAcl_t* GetACLResourceData(const OicUuid_t* subjectId);

#ifdef __cplusplus
}
#endif

#endif //IOTVT_SRM_ACLR_H


