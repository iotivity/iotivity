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
 * @file  cabtmanager.h
 * @brief  This file provides the APIs to control Bluetooth transport
 */

#ifndef __CA_BT_MANAGER_H_
#define __CA_BT_MANAGER_H_

#include <pthread.h>
#include <string.h>
#include <bluetooth.h>

#include "caedradapter.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @fn  CABTManagerInitialize
 * @brief  Initializes bluetooth adapter.
 *
 * @param[in]  threadPool  Thread pool for handling asynchronous tasks.
 *
 * @return  #CA_STATUS_OK or #CA_ADAPTER_NOT_ENABLED on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_ADAPTER_NOT_ENABLED Initialization is successful, but bluetooth adapter is
 *                                                      not enabled.
 * @retval  #CA_STATUS_FAILED Operation failed
 *
 * @see  CABTManagerTerminate()
 */
CAResult_t CABTManagerInitialize(u_thread_pool_t threadPool);

/**
 * @fn  CABTManagerTerminate
 * @brief  Deinitialize with bluetooth adapter.
 *
 * @pre  CABTManagerIntialize() should be invoked before using this API.
 *
 * @see  CABTManagerIntialize()
 *
 */
void CABTManagerTerminate(void);

/**
 * @fn  CABTManagerStart
 * @brief  Starts discovery for nearby OIC bluetooth devices.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 *
 * @pre  CABTManagerIntialize() should be invoked before using this API.
 *
 * @see  CABTManagerIntialize()
 *
 */
CAResult_t CABTManagerStart(void);

/**
 * @fn  CABTManagerStop
 * @brief  Resetting callbacks with bluetooth framework and stop OIC device discovery.
 *
 * @pre  CABTManagerStart() should be invoked before using this API.
 *
 * @see  CABTManagerIntialize()
 * @see  CABTManagerStart()
 *
 */
void CABTManagerStop(void);

/**
 * @fn  CABTManagerSetPacketReceivedCallback
 * @brief  Register callback for receiving packets from remote bluetooth device.
 *
 * @param[in]  packetReceivedCallback  Callback function to register for receiving network packets.
 *
 * @post  CANetworkPacketReceivedCallback() will be called if any data received from peer
 *           connected bluetooth device.
 *
 * @see  CANetworkPacketReceivedCallback()
 *
 */
void CABTManagerSetPacketReceivedCallback(CANetworkPacketReceivedCallback packetReceivedCallback);

/**
 * @fn  CABTManagerSetNetworkChangeCallback
 * @brief  Register callback for receiving local bluetooth adapter state.
 *
 * @param[in]  networkStateChangeCallback  Callback function to register for receiving local bluetooth
 *                                                           adapter status.
 *
 * @post  CANetworkChangeCallback() will be called if bluetooth adatper state changes.
 *
 * @see  CANetworkChangeCallback()
 *
 */
void CABTManagerSetNetworkChangeCallback(CANetworkChangeCallback networkStateChangeCallback);

/**
 * @fn  CABTManagerSendData
 * @brief  Send unicast/multicast data to remote bluetooth device.
 *
 * @param[in]  remoteAddress  Address of remote bluetooth device to which data needs to be sent.
 *                                        This will be NULL in case of sending multicast data.
 * @param[in]  serviceUUID  The UUID of service to which RFCOMM connection needs to established.
 * @param[in]  data  The data needs to be send to remote bluetooth device.
 * @param[in]  dataLength  Length of data.
 * @param[out]  sentLength  Length of data actually sent or added to pending list for sending.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval  #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CABTManagerSendData(const char *remoteAddress, const char *serviceUUID,
                               void *data, uint32_t dataLength, uint32_t *sentLength);

/**
 * @fn  CABTManagerStartServer
 * @brief  Start RFCOMM server for given service UUID.
 *
 * @param[in]  serviceUUID  The UUID of service with which RFCOMM server needs to be started
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_SERVER_STARTED_ALREADY Service on the specified UUID is already running.
 * @retval  #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval  #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CABTManagerStartServer(void);

/**
 * @fn  CABTManagerStopServer
 * @brief  Stop RFCOMM server
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 *
 * @pre  Server must be started with CABTManagerStartServer().
 *
 * @see  CABTManagerStartServer()
 */
CAResult_t CABTManagerStopServer(void);

/**
 * @fn  CABTManagerGetInterface
 * @brief  Get the local bluetooth adapter information.
 *
 * @param[in]  info  Local bluetooth adapter information
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval  #CA_STATUS_FAILED Operation failed
 *
 * @see #CALocalConnectivity_t
 *
 */
CAResult_t CABTManagerGetInterface(CALocalConnectivity_t **info);

#if 0
/**
 * @fn  CABTManagerReadData
 * @brief  All received data will be notified to upper layer.
 *
 * @return  #CA_STATUS_OK on success otherwise proper error code.
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 *
 */
CAResult_t CABTManagerReadData(void);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //__CA_BT_MANAGER_H_

