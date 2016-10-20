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

#ifndef IOTVT_SRM_DOXM_H
#define IOTVT_SRM_DOXM_H

#include "octypes.h"
#ifdef _ENABLE_MULTIPLE_OWNER_
#include "cacommon.h"
#endif //_ENABLE_MULTIPLE_OWNER_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize DOXM resource by loading data from persistent storage.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult InitDoxmResource();

/**
 * Perform cleanup for DOXM resources.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult DeInitDoxmResource();

/**
 * This method is used by SRM to retrieve DOXM resource data..
 *
 * @return reference to @ref OicSecDoxm_t, binary format of Doxm resource data.
 */
const OicSecDoxm_t* GetDoxmResourceData();

/**
 * This method converts CBOR DOXM into binary DOXM.
 * The CBOR DOXM can be from persistent database or
 * or received as PUT/POST request.
 *
 * @param cborPayload is a doxm data in cbor.
 * @note Caller needs to invoke OCFree after done using the return pointer.
 * @param doxm is the pointer to @ref OicSecDoxm_t.
 * @param size of the cborPayload. In case value is 0, CBOR_SIZE value is assigned.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult CBORPayloadToDoxm(const uint8_t *cborPayload, size_t size,
                                OicSecDoxm_t **doxm);

/**
 * This method converts DOXM data into CBOR format.
 * Caller needs to invoke 'free' when finished done using
 * return string.
 *
 * @param doxm Pointer to @ref OicSecDoxm_t.
 * @note Caller needs to invoke OCFree after done using the return pointer.
 * @param cborPayload is the payload of the cbor.
 * @param cborSize is the size of the cbor payload. Passed parameter should not be NULL.
 * @param rwOnly indicates whether convertingpayload has all properties or read-write properties only.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult DoxmToCBORPayload(const OicSecDoxm_t * doxm, uint8_t **cborPayload,
                                size_t *cborSize, bool rwOnly);

/**
 * This method returns the SRM device ID for this device.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult GetDoxmDeviceID(OicUuid_t *deviceID);

/**
 * This method changes the SRM device ID for this device.
 * This api will update device Id iff device is in unowned state.
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult SetDoxmDeviceID(const OicUuid_t *deviceID);


/**
 * Gets the OicUuid_t value for the owner of this device.
 *
 * @param devownerid a pointer to be assigned to the devownerid property
 * @return ::OC_STACK_OK if devownerid is assigned correctly, else ::OC_STACK_ERROR.
 */
OCStackResult GetDoxmDevOwnerId(OicUuid_t *devownerid);

/**
 * Gets the bool state of "isOwned" property on the doxm resource.
 *
 * @param isOwned a pointer to be assigned to isOwned property
 * @return ::OC_STACK_OK if isOwned is assigned correctly, else ::OC_STACK_ERROR.
 */
OCStackResult GetDoxmIsOwned(bool *isOwned);

/**
 * Gets the OicUuid_t value for the rowneruuid of the doxm resource.
 *
 * @param rowneruuid a pointer to be assigned to the rowneruuid property
 * @return ::OC_STACK_OK if rowneruuid is assigned correctly, else ::OC_STACK_ERROR.
 */
OCStackResult GetDoxmRownerId(OicUuid_t *rowneruuid);

#ifdef _ENABLE_MULTIPLE_OWNER_
/**
 * Compare the UUID to SubOwner.
 *
 * @param[in] uuid device UUID
 *
 * @return true if uuid exists in the SubOwner list of doxm, else false.
 */
bool IsSubOwner(const OicUuid_t* uuid);
#endif //_ENABLE_MULTIPLE_OWNER_

/** This function deallocates the memory for OicSecDoxm_t .
 *
 * @param doxm is the pointer to @ref OicSecDoxm_t.
 */
void DeleteDoxmBinData(OicSecDoxm_t* doxm);

/**
 * Function to restore doxm resurce to initial status.
 * This function will use in case of error while ownership transfer
 */
void RestoreDoxmToInitState();

#if defined(__WITH_DTLS__) && defined(_ENABLE_MULTIPLE_OWNER_)
/**
 * Callback function to handle MOT DTLS handshake result.
 * @param[out]   object           remote device information.
 * @param[out]   errorInfo        CA Error information.
 */
void MultipleOwnerDTLSHandshakeCB(const CAEndpoint_t *object,
                                const CAErrorInfo_t *errorInfo);
#endif //__WITH_DTLS__ && _ENABLE_MULTIPLE_OWNER_

#ifdef __cplusplus
}
#endif

#endif //IOTVT_SRM_DOXMR_H
