/* *****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
 * This file provides interface API's between the websocket
 * client/server modules and the websocket adapter interface.
 */

#ifndef CA_WS_INTERFACE_H_
#define CA_WS_INTERFACE_H_

#include "cacommon.h"
#include "cawsadapter.h"
#include "cathreadpool.h"
#include "uarraylist.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Start WS client/server
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAWSStart();

/**
 * Stops WS client/servers and close the sockets.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAWSStop();

/**
 * Sends data to the endpoint as client/server.
 * @param[in]   endpoint       Remote Endpoint information (like ipaddress,
 *                              port, reference uri and transport type) to
 *                              which the unicast data has to be sent.
 * @param[in]   data           Data which is required to be sent.
 * @param[in]   dataLen        Size of data to be sent.
 * @note  dataLen must be > 0 and <= INT_MAX.
 * @return  ::CA_STATUS_OK or Apprpriate error code.
 */
CAResult_t CAWSSendData(const CAEndpoint_t *endpoint, const void *data,
                        uint32_t dataLength);

/**
 * Used to set callback to be notified on reception of any data from
 * remote OIC devices.
 *
 * @param[in]  networkPacketCallback    The callback which will be notified
 *                                       when data is received
 */
void CAWSSetPacketReceivedCallback(CANetworkPacketReceivedCallback networkPacketCallback);

/**
 * Used to set callback to be notified on connection state changes.
 *
 * @param[in]  connCallback    The callback which will be notified
 *                                       when connection state is changed.
 */
void CAWSSetConnectionChangeCallback(CAConnectionChangeCallback connCallback);

/**
 * set error callback to notify error in WS adapter.
 *
 * @param[in]  errorCallback Callback function to notify the error
 * in the WS adapter.
 */
void CAWSSetErrorHandleCallback(CAErrorHandleCallback errorCallback);

/**
 * Get WS Connectivity network information.
 * @param[out]   info        Local connectivity information structures.
 * @note info is allocated in this API and should be freed by the caller.
 * @param[out]   size        Number of local connectivity structures.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
CAResult_t CAWSGetInterfaceInformation(CAEndpoint_t **info, size_t *size);

#ifdef __cplusplus
}
#endif

#endif /* CA_WS_INTERFACE_H_ */
