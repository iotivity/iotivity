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
 * @file cawificlient.h
 * @brief This file contains the APIs for WiFi Client module
 */

#ifndef _CA_WIFI_CLIENT_
#define _CA_WIFI_CLIENT_

#include <stdint.h>
#include "cacommon.h"
#include "logger.h"
#include "camessagequeue.h"
#include "uthreadpool.h"

/**
 * @brief API to Set the send queue handle
 * @param  gSendQueueHandle         [IN] Queue Handle to Send Data
 * @return - None
 */
void CASetSendQueueHandle(CAAdapterMessageQueue_t *gSendQueueHandle, CABool_t flag, u_cond cond);

/**
 * @brief API to Set the Multicast IP and Port Address
 * @param  mAddr            [IN] Multicast IP
 * @param  mPort            [IN] Multicast Port
 * @return - None
 */
void CASetMulticastData(char *mAddr, uint32_t mPort);

/**
 * @brief API to Start the Thread for Send Unicast and Multicast Data
 * @param  threadHandle         [IN] ThreadHandle to Add the task in Thread pool
 * @return - None
 */
CAResult_t CAStartSendDataThread(u_thread_pool_t threadHandle);

/**
 * @brief  To send Unicast or Multicast Data
 * @param  remoteIpAddress      [IN] RemoteEndpoint IP Address
 * @param  port                 [IN] RemoteEndpoint Port Number
 * @param  data                 [IN] Data to be sent to the RemoteEndpoint
 * @param  dataLen              [IN] Length of the data to be sent
 * @param  isMulticast          [IN] Unicast/Multicast flag, 0 for Unicast and 1 for Multicast
 * @return Return               0 on failure and length of data on success
 */
uint32_t CAWIFISendData(const char *remoteIpAddress, const uint32_t port, const char *data,
                        uint32_t dataLen, int16_t isMulticast);

/**
 * @brief API to Invoke SendData.
 * @return - NULL
 */
void *CAStartThreadForSendData();

#endif  // #ifndef _CA_WIFI_CLIENT_