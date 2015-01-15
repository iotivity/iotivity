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
 * @file cawifiadapter.h
 * @brief This file contains the APIs for WiFi Adapter.
 */
#ifndef __CA_WIFI_ADAPTER_H__
#define __CA_WIFI_ADAPTER_H__

#include "cacommon.h"
#include "caadapterinterface.h"
#include "uthreadpool.h" /* for thread pool */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initialize WIFI Interface.
 * @param registerCallback      [IN] Callback to register WIFI interfaces to Connectivity
 *                                   Abstraction Layer
 * @param networkPacketCallback [IN] Callback to notify request and response messages from server(s)
 *                                   started at Connectivity Abstraction Layer.
 * @param netCallback           [IN] Callback to intimate the network additions to Connectivity
 *                                   Abstraction Layer.
 * @param handle                [IN] Threadpool Handle
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAInitializeWifi(CARegisterConnectivityCallback registerCallback,
                            CANetworkPacketReceivedCallback networkPacketCallback,
                            CANetworkChangeCallback netCallback, u_thread_pool_t handle);

/**
 * @brief Start WIFI Interface adapter.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStartWIFI();

/**
 * @brief Start Listening Server for receiving multicast search requests
 * Transport Specific Behavior:
 * WIFI Start multicast server on a particular interface and prefixed port
 * as per OIC Specification.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStartWIFIListeningServer();

/**
 * @brief Start WIFI Discovery Server for receiving multicast advertisements
 * Transport Specific Behavior:
 * WIFI Start multicast server on a particular interface and prefixed port
 * number as per OIC Specification
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStartWIFIDiscoveryServer();

/**
 * @brief Sends data to the endpoint using the adapter connectivity.
 * @param endpoint  [IN] Remote Endpoint information (like ipaddress, port, reference uri and
 *                       connectivity type) to which the unicast data has to be sent.
 * @param data      [IN] Data to be sent.
 * @param dataLen   [IN] Size of data to be sent.
 * @return The number of bytes sent on the network. Returns 0 on error.
 * @remarks  dataLen must be > 0.
 */
uint32_t CASendWIFIUnicastData(const CARemoteEndpoint_t *endpoint, void *data,
                               uint32_t dataLen);

/**
 * @brief Sends Multicast data to the endpoint using the WIFI connectivity.
 * @param   data        [IN] Data which required to be sent.
 * @param   dataLen     [IN] Size of data to be sent.
 * @return  The number of bytes sent on the network. Returns 0 on error.
 * @remarks  dataLen must be > 0.
 */
uint32_t CASendWIFIMulticastData(void *data, uint32_t dataLen);

/**
 * @brief Get WIFI Connectivity network information
 * @param   info  [OUT] Local connectivity information structures
 * @param   size  [OUT] Number of local connectivity structures.
 * @return  #CA_STATUS_OK or Appropriate error code
 * @remarks info is allocated in this API and should be freed by the caller.
 */
CAResult_t CAGetWIFIInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size);

/**
 * @brief Read Synchronous API callback.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAReadWIFIData();

/**
 * @brief WIFI Stops Unicast, Multicast servers and close the sockets.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStopWIFI();

/**
 * @brief Terminate the WIFI connectivity adapter.
 * Configuration information will be deleted from further use
 * @return  NONE
 */
void CATerminateWIfI();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // #ifndef __CA_WIFI_ADAPTER_H__
