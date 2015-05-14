/* ****************************************************************
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
 * @file
 *
 * This file contains common utility function for CA transport adaptors.
 */

#ifndef CA_INTERFACE_CONTROLLER_H_
#define CA_INTERFACE_CONTROLLER_H_

#include "caadapterinterface.h"
#include "cathreadpool.h" /* for thread pool */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Initializes different adapters based on the compilation flags.
 * @param   handle         [IN]    thread pool handle created by message handler for different adapters.
 * @return  none
 */
void CAInitializeAdapters(ca_thread_pool_t handle);

/**
 * @brief   Set the received packets callback for message handler
 * @param   callback       [IN]    message handler callback to receive packets from different adapters.
 * @return  none
 */
void CASetPacketReceivedCallback(CANetworkPacketReceivedCallback callback);

/**
 * @brief   Set the network status changed callback for message handler
 * @param   callback       [IN]    message handler network status callback to receive network changes.
 * @return  none
 */
void CASetNetworkChangeCallback(CANetworkChangeCallback callback);

/**
 * @brief   Starting different connectivity adapters based on the network selection.
 * @param   transportType   [IN]    interested network for starting
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAStartAdapter(CATransportType_t transportType);

/**
 * @brief   Stopping different connectivity adapters based on the network un-selection.
 * @param   transportType   [IN]    network type that want to stop
 * @return  none
 */
void CAStopAdapter(CATransportType_t transportType);

/**
 * @brief   Get network information such as ipaddress and mac information
 * @param   info           [OUT]    connectivity information such as ipaddress and mac information
 * @param   size           [OUT]    number of connectivity information structures
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAGetNetworkInfo(CALocalConnectivity_t **info, uint32_t *size);

/**
 * @brief   Sends unicast data to the remote endpoint
 * @param   endpoint       [IN]    endpoint information where the data has to be sent
 * @param   data           [IN]    data that needs to be sent
 * @param   length         [IN]    length of the data that needs to be sent
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CASendUnicastData(const CARemoteEndpoint_t *endpoint, const void *data, uint32_t length);

/**
 * @brief   Sends multicast data to all endpoints in the network.
 * @param   data           [IN]    data that needs to be sent
 * @param   length         [IN]    length of the data that needs to be sent
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */

CAResult_t CASendMulticastData(const void *data, uint32_t length);

/**
 * @brief   Start listening servers to receive search requests from clients
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAStartListeningServerAdapters();

/**
 * @brief   Start discovery servers to receive advertisements from server
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAStartDiscoveryServerAdapters();

/**
 * @brief   Terminates  the adapters which are initialized during the initialization
 * @return  none
 */
void CATerminateAdapters();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_INTERFACE_CONTROLLER_H_ */

