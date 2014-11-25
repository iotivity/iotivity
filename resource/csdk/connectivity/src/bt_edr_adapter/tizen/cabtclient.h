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
 * @file    cabtclient.h
 * @brief   This    file provides the APIs to establish RFCOMM connection with remote bluetooth device
 */

#ifndef __CA_BT_CLIENT_H_
#define __CA_BT_CLIENT_H_

#include <string.h>
#include <bluetooth.h>

#include "cacommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @fn  CABTClientConnect
 * @brief  Establishes RFCOMM connection with remote bluetooth device
 *
 * @param  remoteAddress  The address of remote bluetooth device to which connection needs to be established
 * @param  serviceUUID  The UUID of service to which RFCOMM connection needs to established
 *
 * @return  CA_STATUS_OK on success otherwise proper error code.
 * @retval  CA_STATUS_OK  Successful
 * @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval  CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CABTClientConnect(const char *remoteAddress, const char *serviceUUID);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //__CA_BT_CLIENT_H_