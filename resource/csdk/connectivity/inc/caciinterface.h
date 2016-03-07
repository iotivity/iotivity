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
 * @file
 * This file provides APIs CI client/server/network monitor modules.
 */

#ifndef CA_CI_INTERFACE_H_
#define CA_CI_INTERFACE_H_

#include <stdbool.h>

#include "cacommon.h"
#include "cathreadpool.h"
#include "uarraylist.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Callback to be notified on reception of any data from remote OIC devices.
 *
 * @param[in]  endpoint      network endpoint description.
 * @param[in]  data          Data received from remote OIC device.
 * @param[in]  dataLength    Length of data in bytes.
 * @pre  Callback must be registered using CAIPSetPacketReceiveCallback().
 */
typedef void (*CACIPacketReceivedCallback)(const CAEndpoint_t *endpoint,
                                           const void *data,
                                           uint32_t dataLength);

/**
  * Callback to notify error in the CI adapter.
  *
  * @param[in]  endpoint      network endpoint description.
  * @param[in]  data          Data sent/received.
  * @param[in]  dataLength    Length of data in bytes.
  * @param[in]  result        result of request from R.I.
  * @pre  Callback must be registered using CAIPSetPacketReceiveCallback().
 */
typedef void (*CACIErrorHandleCallback)(const CAEndpoint_t *endpoint, const void *data,
                                        uint32_t dataLength, CAResult_t result);

/**
 * set error callback to notify error in CI adapter.
 *
 * @param[in]  errorHandleCallback Callback function to notify the error
 * in the CI adapter.
 */
void CACISetErrorHandler(CACIErrorHandleCallback errorHandleCallback);

/**
 * Start CI server.
 *
 * @param   threadPool   Thread pool for managing Unicast server threads.
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM Invalid input data.
 * @retval ::CA_STATUS_FAILED Initialization failed.
 */
CAResult_t CACIStartServer(const ca_thread_pool_t threadPool);

/**
 * Stop CI server.
 */
void CACIStopServer();

/**
 * Set this callback for receiving data packets from peer devices.
 *
 * @param[in]  callback    Callback to be notified on reception of unicast data packets.
 */
void CACISetPacketReceiveCallback(CACIPacketReceivedCallback callback);

/**
 * API to send unicast CI data.
 *
 * @param[in]  endpoint          complete network address to send to.
 * @param[in]  data              Data to be send.
 * @param[in]  dataLength        Length of data in bytes.
 * @param[in]  isMulticast       Whether data needs to be sent to multicast ip.
 */
void CACISendData(CAEndpoint_t *endpoint, const void *data, uint32_t dataLength,
                  bool isMulticast);

/**
 * Get a list of CAInterface_t items.
 *
 * @return  List of CAInterface_t items.
 */
u_arraylist_t *CACIGetInterfaceInformation(int desiredIndex);

/**
 * Connect to CI Server.
 * @param[in]   ciServerInfo   CI Server information.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAConnectToCIServer(CACIServerInfo_t *ciServerInfo);

/**
 * Disconnect from CI Server.
 * @param[in]   ciServerInfo   CI Server information.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CADisconnectFromCIServer(const CACIServerInfo_t *ciServerInfo);

/**
 * Get CI Connection Information from list.
 * @param[in]   addr    CI Server address.
 * @param[in]   port    CI Server port.
 * @param[out]  index   index of array list.
 * @return  CI Server Information structure.
 */
CACIServerInfo_t *CAGetCIServerInfoFromList(const char *addr, const uint16_t port,
                                            uint32_t *index);

#ifdef __cplusplus
}
#endif

#endif /* CA_CI_INTERFACE_H_ */
