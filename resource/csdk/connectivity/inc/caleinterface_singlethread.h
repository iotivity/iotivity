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
 * @file caleinterface_singlethread.h
 * @brief This file provides APIs for BLE modules
 */

#ifndef _CA_LE_INTERFACE_SINGLETHREAD_H_
#define _CA_LE_INTERFACE_SINGLETHREAD_H_

#include <stdbool.h>

#include "cacommon.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @ENUM TRANSFER_TYPE
 * @brief Provide info about different mode of data transfer
 *
 * This enum is used to differentiate between unicast and multicast data transfer.
 */
typedef enum
{
    MULTICAST,    /**< When this enum is selected, data will be updated to all OIC servers. */
    UNICAST    /**< When this enum is selected, data will be updated to desired OIC Server. */
} TRANSFER_TYPE;

/**
* @fn  CAStartBleGattServer
* @brief  Used to start Gatt Server thread for service creation and advertise ble service.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*/
CAResult_t CAStartBleGattServer();

/**
* @fn  CAStopBleGattServer
* @brief  Used to terminate BLE Gatt Service.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*/
CAResult_t CAStopBleGattServer();

/**
* @fn  CASetBLEReqRespServerCallback
* @brief  used to store upper layer callback locally which will be used to send the data
* to application
* @param[in]  CANetworkPacketReceivedCallback  -  upper layer callback function
* to pass the data to CA layer.
* @return  void
*/
void CASetBLEReqRespServerCallback(CABLEServerDataReceivedCallback callback);

/**
* @fn  CAUpdateCharacteristicsInGattServer
* @brief  Used to update characteristics(Read/Write) value that we want to send to particular client.
*              Both unicast and multicast will use the same api. In mulicast, we will be sending in loop to all clients.
*
* @param[in]  charValue  - data that we want to send to client(unicast)/clients(multicast)
* @param[in]  charValueLen  - length of the data.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*/
CAResult_t CAUpdateCharacteristicsInGattServer(const char *charValue, const uint32_t charValueLen);

/**
* @fn  CAStartBLEGattClient
* @brief  Used to start CAStartBleGattClientThread for initializing Gatt Client
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*/
CAResult_t CAStartBLEGattClient();

/**
* @fn  CAStopBLEGattClient
* @brief  Used to stop Gatt Client gracefully in turn it will call CATerminateBLEGattClient function.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*/
void CAStopBLEGattClient();

/**
* @fn  CATerminateBLEGattClient
* @brief  Used to unset all the callbacks and stop service discovery
*
* @return  void
*/
void CATerminateBLEGattClient();

/**
* @fn  CAUpdateCharacteristicsToGattServer
* @brief  Sets the value of characteristic and update the value to GATTServer(unicast).
*
* @param[in]  remoteAddress  The address of the remote device
* @param[in]  data  The value of characteristic (byte array)
* @param[in]  dataLen  The length of value
* @param[in]  TRANSFER_TYPE (UNICAST/MULTICAST)
* @param[in]  position  The unique index of each ble server. Used for multicast feature.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*/
CAResult_t  CAUpdateCharacteristicsToGattServer(const char *remoteAddress, const char  *data,
        const int32_t dataLen, TRANSFER_TYPE type, const int32_t position);

/**
 * @fn  CAUpdateCharacteristicsToAllGattServers
 * @brief  Sets the value of characteristic and update the value to All registered GATTServer -> Multicast
 *
 * @param[in]  data  The value of characteristic (byte array)
 * @param[in]  dataLen  The length of value
 *
 * @return  0 on success otherwise a positive error value.
 * @retval  CA_STATUS_OK  Successful
 * @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval  CA_STATUS_FAILED Operation failed
 */
CAResult_t  CAUpdateCharacteristicsToAllGattServers(const char  *data, const int32_t dataLen);

/**
 * @fn  CASetBLEReqRespClientCallback
 * @brief  used to store upper layer callback locally which will be used to send the data to application
 *
 * @param[in]  CANetworkPacketReceivedCallback  -  upper layer callback function to pass the data to CA layer.
 *
 * @return  void
 */
void CASetBLEReqRespClientCallback(CABLEClientDataReceivedCallback callback);

#ifdef __cplusplus
}
#endif

#endif //_CA_LE_INTERFACE_SINGLETHREAD_H_
