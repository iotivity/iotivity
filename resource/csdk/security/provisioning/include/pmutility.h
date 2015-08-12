/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef _PM_UTILITY_H_
#define _PM_UTILITY_H_

#include <stdbool.h>
#include "ocstack.h"
#include "pmtypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DEFAULT_SECURE_PORT 5684

#define COAPS_PREFIX "coaps://"
#define COAP_PREFIX "coap://"
#define COAPS_QUERY "coaps://%s:%d%s"
#define COAP_QUERY "coap://%s:%d%s"

/**
 * Macro to verify argument is not equal to NULL.
 * eg: VERIFY_NON_NULL(TAG, ptrData, ERROR,OC_STACK_ERROR);
 */
#define VERIFY_NON_NULL(tag, arg, logLevel, retValue) { if (NULL == (arg)) \
            { OC_LOG((logLevel), tag, (#arg " is NULL")); return retValue; } }

/**
 * Macro to verify success of operation.
 * eg: VERIFY_SUCCESS(TAG, OC_STACK_OK == foo(), ERROR, OC_STACK_ERROR);
 */
#define VERIFY_SUCCESS(tag, op, logLevel, retValue) { if (!(op)) \
            {OC_LOG((logLevel), tag, (#op " failed!!")); return retValue;} }


/**
 * Discover owned/unowned devices in the same IP subnet. .
 *
 * @param[in] waittime      Timeout in seconds.
 * @param[in] isOwned       bool flag for owned / unowned discovery
 * @param[in] ppList        List of OCProvisionDev_t.
 *
 * @return OC_STACK_OK on success otherwise error.
 */
OCStackResult PMDeviceDiscovery(unsigned short waittime, bool isOwned, OCProvisionDev_t **ppList);

/**
 * This function deletes list of provision target devices
 *
 * @param[in] pList         List of OCProvisionDev_t.
 */
void DeleteDeviceList(OCProvisionDev_t **pList);

#ifdef __cplusplus
}
#endif
#endif //_PM_UTILITY_H_
