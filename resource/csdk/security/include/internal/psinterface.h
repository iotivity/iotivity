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

#ifndef IOTVT_SRM_PSI_H
#define IOTVT_SRM_PSI_H

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Reads the Secure Virtual Database from PS
 *
 * @note Caller of this method MUST use OCFree() method to release memory
 *       referenced by return value.
 *
 * @param[in] svrData is a pointer of OicSvr_t.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult ReadSVDataFromPS(OicSvr_t* svrData);

/**
 * Writes the Secure Virtual Database to PS
 *
 * @param[in] svrData is a pointer of OicSvr_t.
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult WriteSVDatabase(OicSvr_t* svrData);

/**
 * This method converts SVR buffers in to OCRepPayload and updates the persistent storage.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult UpdateSVRData();

#ifdef __cplusplus
}
#endif

#endif //IOTVT_SRM_PSI_H
