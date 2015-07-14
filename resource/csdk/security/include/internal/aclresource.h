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
 * @param[in] payload is a pointer of CBOR acl payload.
 * @param[in] size is CBOR acl payload size.
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult InitACLResource(uint8_t *payload, size_t size);

/**
 * Perform cleanup for ACL resources.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult DeInitACLResource();

/**
 * Perform cleanup for ACL resources.
 *
 * @param[in] acl  pointer to OicSecAcl_t.
 */
void DeleteACLList(OicSecAcl_t* acl);

/**
 * This method is used by PolicyEngine to retrieve ACL for a Subject.
 *
 * @param[in] subjectId ID of the subject for which ACL is required.
 * @param[out] savePtr is used internally by @ref GetACLResourceData to maintain index between
 *                successive calls for same subjectId.
 *
 * @return reference to acl if ACL is found, else NULL.
 *
 * @note On the first call to @ref GetACLResourceData, savePtr should point to NULL.
 */
const OicSecAcl_t* GetACLResourceData(const OicUuid_t* subjectId, OicSecAcl_t **savePtr);

/**
 * This function converts ACL data into OCRepPayload.
 * Caller needs to invoke 'free' when done using
 * returned OCRepPayload pointer.
 *
 * @param[in] acl  instance of OicSecAcl_t structure.
 *
* @return reference to acl if ACL for successful operation, otherwise NULL.
 */

OCRepPayload* AclToPayload(const OicSecAcl_t * acl);
/**
 * This function converts OCRepPayload to ACL data.
 * Caller needs to invoke 'free' when done using
 * returned OicSecAcl_t pointer.
 *
 * @param[in] payload  instance of OCRepPayload.
 *
 * @return  pointer to OicSecAcl_t.
 */
OicSecAcl_t* PayloadToAcl(const OCRepPayload* payload);

/**
 * This internal method is to retrieve the default ACL.
 * If SVR database in persistent storage got corrupted or
 * is not available for some reason, a default ACL is created
 * which allows user to initiate ACL provisioning again.
 *
 * @param[out] defaultAcl pointer of OicSecAcl_t*.
 */
OCStackResult  GetDefaultACL(OicSecAcl_t** defaultAcl);

/**
 * This internal method is the entity handler for ACl resources.
 *
 * @param[in] flag  for request or observe.
 * @param[in] ehRequest      pointer to the OCEntityHandlerRequest struct that is created.
 * @param[in] callbackParam  parameter passed back when entityHandler is called.
 * @return  OC_EH_OK for Success, otherwise some error value.
 */
OCEntityHandlerResult ACLEntityHandler(OCEntityHandlerFlag flag,
            OCEntityHandlerRequest * ehRequest,
            void* callbackParameter);

/**
 * This method converts SVR buffers into OCRepPayload and updates the persistent storage.
 *
 * @param[out] payload is a pointer of CBOR acl payload.
 * @param[out] size is CBOR acl payload size.
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult ConvertAclData(uint8_t **payload,  size_t *size);

#ifdef __cplusplus
}
#endif

#endif //IOTVT_SRM_ACLR_H


