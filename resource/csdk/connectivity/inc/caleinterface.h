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
 * This file provides APIs for BLE modules.
 */

#ifndef CA_LE_INTERFACE_H_
#define CA_LE_INTERFACE_H_

#include <stdbool.h>

#include "cacommon.h"
#include "caleadapter.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @enum CALETransferType_t
 * @brief Provide info about different mode of data transfer
 *        This enum is used to differentiate between unicast and multicast data transfer.
 */
typedef enum
{
    LE_MULTICAST,    /**< When this enum is selected, data will be updated to all OIC servers. */
    LE_UNICAST       /**< When this enum is selected, data will be updated to desired OIC Server. */
} CALETransferType_t;


/**
 * @brief  Initialize the LE adapter layer. This will be invoked from the CA layer.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAInitializeLEAdapter();

/**
 * @brief  Used to get the current state of the LE adapter.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_ADAPTER_NOT_ENABLED  adapter not enabled
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAGetLEAdapterState();

/**
 * @brief  Used to initialize the network monitor layer of the LE adapter. Mutex variables required
 *         to operate in this layer and other paramters can be initialized in this function.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAInitializeLENetworkMonitor();

/**
 * @brief  Used to terminate the network monitor layer of the LE adapter. The variables intialized
 *         in CAInitializeLEAdapterController() must be cleared in this function.
 * @return NONE
 */
void CATerminateLENetworkMonitor();

/**
 * @brief  This function is used to set the callback for the Device state changes in the adapter.
 *
 * @param  callback  [IN] Callback to notify the Device state change to the CA Layer
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CASetLEAdapterStateChangedCb(CALEDeviceStateChangedCallback callback);

/**
 * @brief  Used to initilaze all the mutex variables required to operate the LE network monitor
 *         layer.
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAInitLENetworkMonitorMutexVariables();

/**
 * @brief  Used to terminate all the mutex variables required to operate the LE network monitor
 *         layer.
 * @return NONE
 */
void CATerminateLENetworkMonitorMutexVariables();

/**
 * @brief  Provides the BD address of the local adapter.
 * @param  local_address [OUT] pointer to the location where bd address needs to be stored.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAGetLEAddress(char **local_address);

/**
 * @brief  Used to start Gatt Server thread for service creation and advertise ble service.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAStartLEGattServer();

/**
 * @brief  Used to stop BLE Gatt Service.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAStopLEGattServer();

/**
 * @brief  Used to stop Gatt Server thread and remove service registration, stop advertising.
 * @return NONE
 */
void CATerminateLEGattServer();

/**
 * @brief  Used to store upper layer callback locally which will be used to send the data to
 *         application
 * @param  callback [IN] Callback function to pass the data to CA layer.
 * @return NONE
 */
void CASetLEReqRespServerCallback(CABLEServerDataReceivedCallback callback);

/**
 * @brief  Used to update characteristics(Read/Write) value that we want to send to particular
 *         client.
 *
 * @param  address     [IN] BD address of Gatt client
 * @param  charValue     [IN] Data that we want to send to client(unicast)
 * @param  charValueLen  [IN] Length of the data.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAUpdateCharacteristicsToGattClient(const char* address, const char *charValue,
                                               const uint32_t charValueLen);

/**
 * @brief  Used to update characteristics(Read/Write) value that we want to multicast to all clients
 *
 * @param  charValue     [IN] Data that we want to send to clients(multicast)
 * @param  charValueLen  [IN] Length of the data.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAUpdateCharacteristicsToAllGattClients(const char *charValue,
                                                   uint32_t charValueLen);

/**
 * @brief  Used to start CAStartBleGattClientThread for initializing Gatt Client
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAStartLEGattClient();

/**
 * @brief  Used to stop Gatt Client gracefully in turn it will call CATerminateBLEGattClient
 *         function.
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
void CAStopLEGattClient();

/**
 * @brief  Used to unset all the callbacks and stop service discovery
 * @return NONE
 */
void CATerminateLEGattClient();

/**
 * @brief API to read the data from characteristics and invoke notifyCallback.
 * @return - NONE
 */
void CACheckLEData();

/**
 * @brief  Sets the value of characteristic and update the value to GATTServer(unicast).
 *
 * @param  remoteAddress [IN] The address of the remote device
 * @param  data          [IN] The value of characteristic (byte array)
 * @param  dataLen       [IN] The length of value
 * @param  type          [IN] Type of the transfer(#CALETransferType_t)
 * @param  position      [IN] The unique index of each ble server. Used for multicast feature.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t  CAUpdateCharacteristicsToGattServer(const char *remoteAddress, const char  *data,
                                                const uint32_t dataLen, CALETransferType_t type,
                                                const int32_t position);

/**
 * @brief  Sets the value of characteristic and update the value to all registered
 *         GATTServer -> Multicast
 * @param  data     [IN] The value of characteristic (byte array)
 * @param  dataLen  [IN] The length of value
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t  CAUpdateCharacteristicsToAllGattServers(const char  *data, uint32_t dataLen);

/**
 * @brief  Used to store upper layer callback locally which will be used to send the data to
 *         application
 * @param  callback  [IN] Callback function to pass the data to CA layer.
 *
 * @return  void
 */
void CASetLEReqRespClientCallback(CABLEClientDataReceivedCallback callback);

/**
 * @brief  Used to Set the gThreadPool handle which is required for spawning new thread.
 *
 * @param handle [IN] Thread pool handle which is given by above layer for using thread
 *                    creation task.
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
void CASetLEServerThreadPoolHandle(ca_thread_pool_t handle);

/**
* @brief  Used to Set the gThreadPool handle which is required for spawning new thread.
* @param handle [IN] Thread pool handle which is given by above layer for using thread creation
*                    task.
* @return NONE
*/
void CASetLEClientThreadPoolHandle(ca_thread_pool_t handle);

/**
 * @brief  Used to unset the callback of adapter connection state change.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAUnSetLEAdapterStateChangedCb();

/**
 * @brief This will be used to notify errors in BLE adapter
 * @param  remoteAddress    [IN] Remote endpoint Address
 * @param  serviceUUID      [IN] Service UUID
 * @param  data             [IN] Data received
 * @param  dataLength       [IN] Length of the data received
 * @param  result           [IN] error code as per CAResult_t
 * @return NONE
 */
typedef void (*CABLEErrorHandleCallback)(const char *remoteAddress, const void *data,
                                         uint32_t dataLength, CAResult_t result);
/**
 * @brief  sets the error handle callback
 * @param  callback [IN] Callback function to update error to the adapter
 * @return NONE
 */
void CASetBLEClientErrorHandleCallback(CABLEErrorHandleCallback callback);


/**
 * @brief  sets the error handle callback
 * @param  callback [IN] Callback function to update error to the adapter
 * @return NONE
 */
void CASetBLEServerErrorHandleCallback(CABLEErrorHandleCallback callback);

#ifdef __cplusplus
}
#endif

#endif /* CA_LE_INTERFACE_H_ */

