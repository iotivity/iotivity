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

#ifndef IOTVT_SRM_DPAIRING_H
#define IOTVT_SRM_DPAIRING_H

#include "octypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize DPAIRING resource by loading data from persistent storage.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitDpairingResource();

/**
 * Perform cleanup for DPAIRING resources.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult DeInitDpairingResource();

/**
 * This method converts JSON DPAIRING into binary DPAIRING.
 * The JSON DPAIRING can be from persistent database or
 * or received as POST request.
 *
 * @param[in] jsonStr  pconf data in json string.
 * @return pointer to OicSecDpairing_t.
 *
 * @note Caller needs to invoke OCFree after done
 *       using the return pointer
 */
OicSecDpairing_t * JSONToDpairingBin(const char * jsonStr);

/**
 * This method converts DPAIRING data into JSON format.
 * Caller needs to invoke 'free' when finished done using
 * return string
 *
 * @param[in] dpair  Pointer to OicSecDpairing_t.
 * @return pointer to json string.
 *
 * @note Caller needs to invoke OCFree after done
 *       using the return pointer
 */
char * BinToDpairingJSON(const OicSecDpairing_t * dpair);

/** This function deallocates the memory for OicSecPconf_t .
 *
 * @param[in] dpair  Pointer to OicSecDpairing_t.
 */
void DeleteDpairingBinData(OicSecDpairing_t* dpair);

#ifdef __WITH_DTLS__
/**
 * Function to save PairingPSK.
 *
 * @param[in] endpoint   current endpoint.
 * @param[in] peerDevID   peer device indentitiy.
 * @param[in] isPairingServer   indicate if it generates PairingPSK for server or client.
 *
 * @return  OC_STACK_OK on success
 */
OCStackResult SavePairingPSK(OCDevAddr *endpoint,
            OicUuid_t *peerDevID, OicUuid_t *owner, bool isPairingServer);
#endif // __WITH_DTLS__


#ifdef __cplusplus
}
#endif

#endif //IOTVT_SRM_DPAIRING_H




