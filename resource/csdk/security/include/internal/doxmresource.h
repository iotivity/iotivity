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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize DOXM resource by loading data from persistent storage.
 *
 * @param[in] payload is a pointer of CBOR doxm payload.
 * @param[in] size is CBOR doxm payload size.
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult InitDoxmResource(uint8_t *payload,  size_t size);

/**
 * Perform cleanup for DOXM resources.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult DeInitDoxmResource();

/**
 * This method is used by SRM to retrieve DOXM resource data..
 *
 * @retval  reference to @ref OicSecDoxm_t, binary format of Doxm resource data
 */
const OicSecDoxm_t* GetDoxmResourceData();

/**
 * This method converts OCRepPayload into binary doxm.
 * The OCRepPayload can be from persistent database or
 * or received as PUT/POST request.
 *
 * @param[in] payload  doxm data.
 * @return pointer to OicSecDoxm_t.
 *
 * @note Caller needs to invoke OCFree after done
 *       using the return pointer
 */
OicSecDoxm_t* PayloadToDoxm(const OCRepPayload* payload);

/**
 * This method converts DOXM data into OCRepPayload.
 * Caller needs to invoke 'free' when finished done using
 * return string.
 *
 * @param[in] doxm  Pointer to OicSecDoxm_t.
 * @return pointer to OCRepPayload.
 *
 * @note Caller needs to invoke OCFree after done
 *       using the return pointer
 */
OCRepPayload* DoxmToPayload(const OicSecDoxm_t * doxm);
/**
 * This method returns the SRM device ID for this device.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult GetDoxmDeviceID(OicUuid_t *deviceID);

/**
 * @brief Gets the OicUuid_t value for the owner of this device.
 *
 * @return OC_STACK_OK if devOwner is a valid UUID, otherwise OC_STACK_ERROR.
 */
OCStackResult GetDoxmDevOwnerId(OicUuid_t *devOwner);

/** This function deallocates the memory for OicSecDoxm_t .
 *
 * @param[in] doxm  Pointer to OicSecDoxm_t.
 */
void DeleteDoxmBinData(OicSecDoxm_t* doxm);

/**
 * This internal method is the entity handler for DOXM resources.
 *
 * @param[in] flag  for request or observe.
 * @param[in] ehRequest      pointer to the OCEntityHandlerRequest struct that is created.
 * @param[in] callbackParam  Parameter passed back when entityHandler is called.
 * @retval  OC_EH_OK for Success, otherwise some error value.
 */
OCEntityHandlerResult DoxmEntityHandler(OCEntityHandlerFlag flag,
            OCEntityHandlerRequest * ehRequest,
            void* callbackParam);

/**
 * Get the default value.
 * @retval  the gDefaultDoxm pointer;
 */
OicSecDoxm_t* GetDoxmDefault();

/**
 * This method converts SVR buffers into OCRepPayload and updates the persistent storage.
 *
 * @param[out] payload is a pointer of CBOR doxm payload.
 * @param[out] size is CBOR doxm payload size.
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult ConvertDoxmData(uint8_t **payload,  size_t *size);

#ifdef __cplusplus
}
#endif

#endif //IOTVT_SRM_DOXMR_H


