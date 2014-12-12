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
 * @file caedradapter.h
 * @brief This file contains the APIs for EDR adapters to be implemented
 */
#ifndef __CA_EDRADAPTER_H_
#define __CA_EDRADAPTER_H_

/**
 * BT Interface AP
 **/
#include "cacommon.h"
#include "caadapterinterface.h"
#ifndef ARDUINO
#include "uthreadpool.h" /* for thread pool */
#endif  //ARDUINO

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @fn  CAInitializeEDR
 * @brief  Initialize EDR connectivity interface.
 *
 * @param[in]  registerCallback  To register EDR interfaces to Connectivity Abstraction Layer
 * @param[in]  reqRespCallback  Callback to be notified on receival of request/responses from
 * peer bluetooth devices.
 * @param[in]  netCallback  Callback to be notified when network adapter state changes.
 * @param[in]  threadPool  Thread pool for handling asynchronous tasks.
 *
 * @return  #CA_STATUS_OK or #CA_ADAPTER_NOT_ENABLED on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM  Invalid input parameters
 * @retval  #CA_ADAPTER_NOT_ENABLED Initialization is successful, but bluetooth adapter is
 * not enabled
 * @retval  #CA_STATUS_FAILED Operation failed
 */
#ifdef ARDUINO
CAResult_t CAInitializeEDR(CARegisterConnectivityCallback registerCallback,
                           CANetworkPacketReceivedCallback reqRespCallback,
                           CANetworkChangeCallback netCallback);
#else
CAResult_t CAInitializeEDR(CARegisterConnectivityCallback registerCallback,
                           CANetworkPacketReceivedCallback reqRespCallback,
                           CANetworkChangeCallback netCallback,
                           u_thread_pool_t handle);
#endif //#ifdef ARDUINO


/**
 * @fn  CAStartEDR
 * @brief  Starting EDR connectivity adapters. As its peer to peer it doesnot require to start
 * any servers.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_ADAPTER_NOT_ENABLED Bluetooth adapter is not enabled
 * @retval  #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CAStartEDR();

/**
 * @fn  CAStartEDRListeningServer
 * @brief  Starting listening server for receiving multicast search requests.
 * Starts RFCOMM Server with prefixed UUID as per OIC specification.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_SERVER_STARTED_ALREADY  Server is already started and running for the predefined
 *                                                            service UUID
 * @retval  #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CAStartEDRListeningServer();

/**
 * @fn  CAStartEDRDiscoveryServer
 * @brief  Starting discovery server for receiving multicast advertisements.
 * Starts RFCOMM Server with prefixed UUID as per OIC specification.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_SERVER_STARTED_ALREADY  Server is already started and running for the predefined
 * service UUID
 * @retval  #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CAStartEDRDiscoveryServer();

/**
 * @fn  CASendEDRUnicastData
 * @brief  Sends data to the peer bluetooth OIC device using the adapter connectivity.
 *
 * @param[in]  remoteEndpoint  Information about peer device to which data needs to be send.
 * @param[in]  data  Data which needs to be send to peer OIC device.
 * @param[in]  dataLength  Length of data in bytes.
 *
 * @return  Number of bytes sent on the network. 0 indicates failed to send data.
 *
 */
uint32_t CASendEDRUnicastData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                              uint32_t dataLength);

/**
 * @fn  CASendEDRMulticastData
 * @brief  Sends multicast data to all discovered bluetooth OIC devices using the adapter
 * connectivity.
 *
 * @param[in]  data  Data which needs to be send to all discovered bluetooth OIC device.
 * @param[in]  dataLength  Length of data in bytes.
 *
 * @return  Number of bytes sent on the network. 0 indicates failed to send data.
 *
 */
uint32_t CASendEDRMulticastData(void *data, uint32_t dataLength);

/**
 * @fn  CAGetEDRInterfaceInformation
 * @brief  Get EDR Connectivity network information.
 *
 * @param[out]  info  Array of local connectivity information structures.
 * @param[out]  size  Size of the array @info.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM  Invalid input parameters
 * @retval  #CA_MEMORY_ALLOC_FAILED  Failed to allocate memory
 * @retval  #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CAGetEDRInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size);

/**
 * @fn  CAReadEDRData
 * @brief  Read Synchronous API callback.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CAReadEDRData();

/**
 * @fn  CAStopEDR
 * @brief  Stopping the adapters and close socket connections
 * EDR Stops all RFCOMM servers and close sockets.
 *
 * @return  #CA_STATUS_OK on success
 *
 */
CAResult_t CAStopEDR();

/**
 * @fn  CATerminateEDR
 * @brief  Terminate the EDR connectivity adapter.
 * Configuration information will be deleted from further use.
 *
 */
void CATerminateEDR();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  //__CA_EDRADAPTER_H_
