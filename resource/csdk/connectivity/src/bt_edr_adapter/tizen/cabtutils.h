/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/**
 * @file  cabtutils.h
 * @brief  This file provides helper functions for EDR adapter.
 */

#ifndef __CA_BT_UTILS_H_
#define __CA_BT_UTILS_H_

#include <string.h>

#include "cacommon.h"
#include "oic_malloc.h"

#ifndef BLUETOOTH_ADAPTER_TAG
#define BLUETOOTH_ADAPTER_TAG "CA_BLUETOOTH"
#endif //BLUETOOTH_ADAPTER_TAG

#ifndef OIC_BT_SERVICE_ID
#define OIC_BT_SERVICE_ID "12341234-1C25-481F-9DFB-59193D238280"
#endif //OIC_BT_SERVICE_ID

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @fn  CABTIsServiceSupported
 * @brief  Check if the specified list of service UUIDs contains OIC service UUID.
 *
 * @param  serviceUUID  Array of service UUIDs
 * @param  serviceCount  Size of the service UUIDs array.
 * @param  matchService  Service UUID to be checked in the given array of service UUIDs
 *
 * @return  CA_TRUE if service UUID found otherwise CA_FALSE.
 *
 */
CABool_t CABTIsServiceSupported(const char **serviceUUID, const int32_t serviceCount,
                                const char *matchService);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //__CA_BT_UTILS_H_

