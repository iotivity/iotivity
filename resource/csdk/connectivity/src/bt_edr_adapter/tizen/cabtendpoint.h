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
 * @file    cabtendpoint.h
 * @brief   This    file provides the APIs to send data on established RFCOMM connections.
 */

#ifndef __CA_BT_ENDPOINT_H_
#define __CA_BT_ENDPOINT_H_

#include <bluetooth.h>

#include "cacommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @fn  CABTSendData
 * @brief  Send data over RFCOMM connection
 *
 * @param  serverFD  The RFCOMM connection socket file descriptor
 * @param  data  The data needs to be sent
 * @param  dataLength  The length of data
 * @param  sentDataLen  The length of data which actually sent successful
 *
 * @return  CA_STATUS_OK on success otherwise proper error code.
 * @retval  CA_STATUS_OK  Successful
 * @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval  CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CABTSendData(const int32_t serverFD, const void *data, const uint32_t dataLength,
                        uint32_t *sentDataLen);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //__CA_BT_ENDPOINT_H_

