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

#ifndef IOTVT_SRM_PSTATR_H
#define IOTVT_SRM_PSTATR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize Pstat resource by loading data from persistent storage.
 *
 * @param[in] payload is a pointer of CBOR pstat payload.
 * @param[in] size is CBOR pstat payload size.
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult InitPstatResource(uint8_t *payload,  size_t size);

/**
 * Perform cleanup for Pstat resources.
 *
 * @param[in] pstat pointer to OicSecPstat_t.
 */
void DeletePstatBinData(OicSecPstat_t* pstat);
/**
 * Perform cleanup for Pstat resources.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult DeInitPstatResource();

/**
 * This method converts OCRepPayload into pstat.
 *
 * @param[in] payload  for pstat.
 * @return pointer to OicSecPstat_t.
 */
OicSecPstat_t* PayloadToPstat(const OCRepPayload* payload);

/**
 * This method converts pstat data into OCRepPayload.
 *
 * @param[in] pstat  pstat data in binary format.
 * @return pointer to OCRepPayload.
 */
OCRepPayload* PstatToPayload(const OicSecPstat_t * pstat);

/** This function deallocates the memory for OicSecPstat_t.
 *
 * @param[in] pstat  Pointer to OicSecPstat_t.
 */
void DeletePstatBinData(OicSecPstat_t* pstat);

/**
 * This method converts SVR buffers in to OCRepPayload and updates the persistent storage.
 *
 * @param[out] payload is a pointer of CBOR pstat payload.
 * @param[out] size is CBOR pstat payload size.
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult ConvertPstatData(uint8_t **payload, size_t *size);

/**
 * This internal method is the entity handler for pstat resources.
 *
 * @param[in] flag  for request or observe.
 * @param[in] ehRequest      pointer to the OCEntityHandlerRequest struct that is created.
 * @param[in] callbackParam  Parameter passed back when entityHandler is called.
 * @return ::OC_EH_OK for Success, otherwise some error value.
 */
OCEntityHandlerResult PstatEntityHandler(OCEntityHandlerFlag flag,
            OCEntityHandlerRequest * ehRequest,
            void* callbackParam);

#ifdef __cplusplus
}
#endif

#endif //IOTVT_SRM_PSTATR_H

