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
 * This file contains the APIs for Ethernet Adapter.
 */

#ifndef __CA_ETHERNET_ADAPTER_H__
#define __CA_ETHERNET_ADAPTER_H__

#include "cacommon.h"
#include "caadapterinterface.h"
#include "cathreadpool.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief API to initialize Ethernet Interface.
 * @param registerCallback      [IN] Callback to register ETHERNET interfaces to Connectivity
 *                                   Abstraction Layer
 * @param networkPacketCallback [IN] Callback to notify request and response messages from server(s)
 *                                   started at Connectivity Abstraction Layer.
 * @param netCallback           [IN] Callback to notify the network additions to Connectivity
 *                                   Abstraction Layer.
 * @param handle                [IN] Threadpool Handle
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAInitializeEthernet(CARegisterConnectivityCallback registerCallback,
                                CANetworkPacketReceivedCallback networkPacketCallback,
                                CANetworkChangeCallback netCallback, ca_thread_pool_t handle);

/**
 * @brief Start Ethernet Interface adapter.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStartEthernet();

/**
 * @brief Start listening server for receiving multicast search requests
 * Transport Specific Behavior:
 * Ethernet Starts Multicast Server on a particular interface and prefixed port number and
 * as per OIC Specification.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStartEthernetListeningServer();

/**
 * @brief Start discovery servers for receiving multicast advertisements
 * Transport Specific Behavior:
 * Ethernet Starts Start multicast server on a particular interface and prefixed port
 * number as per OIC Specification
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStartEthernetDiscoveryServer();

/**
 * @brief Sends data to the endpoint using the adapter connectivity.
 * @param   endpoint    [IN]    Remote Endpoint information (like ipaddress , port,
 * reference uri and connectivity type) to which the unicast data has to be sent.
 * @param   data        [IN]    Data which is required to be sent.
 * @param   dataLen     [IN]    Size of data to be sent.
 * @return The number of bytes sent on the network. Return value equal to -1 indicates error.
 * @remarks dataLen must be > 0.
 */
int32_t CASendEthernetUnicastData(const CARemoteEndpoint_t *endpoint, const void *data,
                                   uint32_t dataLen);

/**
 * @brief Sends Multicast data to the endpoint using the Ethernet connectivity.
 * @param   data        [IN]    Data which required to be sent.
 * @param   dataLen     [IN]    Size of data to be sent.
 * @return The number of bytes sent on the network. Return value equal to -1 indicates error.
 * @remarks dataLen must be > 0.
 */
int32_t CASendEthernetMulticastData(const void *data, uint32_t dataLen);

/**
 * @brief Get Ethernet Connectivity network information
 * @param   info        [OUT]   Local connectivity information structures
 * @param   size        [OUT]   Number of local connectivity structures.
 * @return  #CA_STATUS_OK or Appropriate error code
 * @remarks info is allocated in this API and should be freed by the caller.
 */
CAResult_t CAGetEthernetInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size);

/**
 * @brief Read Synchronous API callback.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAReadEthernetData();

/**
 * @brief Stops Unicast, Multicast servers and close the sockets.
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAStopEthernet();

/**
 * @brief Terminate the Ethernet connectivity adapter.
 * Configuration information will be deleted from further use
 * @return  NONE
 */
void CATerminateEthernet();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // #ifndef __CA_ETHERNET_ADAPTER_H__

