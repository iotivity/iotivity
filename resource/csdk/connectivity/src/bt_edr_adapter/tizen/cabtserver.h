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
 * @file    cabtserver.h
 * @brief   This    file provides the APIs to start and stop RFCOMM server.
 */

#ifndef __CA_BT_SERVER_H_
#define __CA_BT_SERVER_H_

#include <string.h>
#include <bluetooth.h>

#include "cacommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @fn  CABTServerStart
 * @brief  Start RFCOMM server for given service UUID
 *
 * @param[in]  serviceUUID  The UUID of service with which RFCOMM server needs to be started.
 * @param[out]  serverFD  The RFCOMM server socket file descriptor.
 *
 * @return  CA_STATUS_OK on success otherwise proper error code.
 * @retval  CA_STATUS_OK  Successful
 * @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval  CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CABTServerStart(const char *serviceUUID, int32_t *serverFD);

/**
 * @fn  CABTServerStop
 * @brief  Stop RFCOMM server
 *
 * @param[in]  serverFD  The RFCOMM server socket file descriptor which needs to be stopped.
 *
 * @return  CA_STATUS_OK on success otherwise proper error code.
 * @retval  CA_STATUS_OK  Successful
 * @retval  CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CABTServerStop(const int32_t serverFD);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //__CA_BT_SERVER_H_