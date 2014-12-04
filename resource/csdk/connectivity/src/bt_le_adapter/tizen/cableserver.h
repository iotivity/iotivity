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
#ifndef __TZ_BLE_SERVER_H_
#define __TZ_BLE_SERVER_H_

#include <bluetooth.h>
#include <bluetooth_type.h>
#include <bluetooth_product.h>

#include "caadapterinterface.h"
#include "logger.h"
#include "uthreadpool.h"

/**
* @fn  CAStartBleGattServer
* @brief  Used to start Gatt Server thread for service creation and advertise ble service.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CAStartBleGattServer();

/**
* @fn  CAStartBleGattServerThread
* @brief  This is thread which will be used for creating ble service and advertise ble service.
*              1. Create New OIC Service 2. Add two read & write characteristics to service.
*              3. Register Service     4. Advertise service.
*
* @param[in] data - Currently it will be NULL.
*
* @return  NULL on sucess or failure.
*
*/
void *CAStartBleGattServerThread(void *data);

/**
* @fn  CAStopBleGattServer
* @brief  Used to terminate BLE Gatt Service.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CAStopBleGattServer();

/**
* @fn  CATerminateBleGattServer
* @brief  Used to stop Gatt Server thread and remove service registration, stop advertising.
*
* @return  void
*
*/
void CATerminateBleGattServer();

/**
* @fn  CAInitBleGattService
* @brief  Used to initialize gatt service using _bt_gatt_init_service api.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CAInitBleGattService();

/**
* @fn  CADeInitBleGattService
* @brief  Used to de-initialize gatt service using _bt_gatt_deinit_service api.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CADeInitBleGattService();

/**
* @fn  CAInitGattServerMutexVaraibles
* @brief  Used to initialize all required mutex varaibles for GATT server implementation.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CAInitGattServerMutexVaraibles();


/**
* @fn  CATerminateGattServerMutexVaraibles
* @brief  Used to terminate all required mutex varaibles for GATT server implementation.
*
* @return void
*
*/
void CATerminateGattServerMutexVaraibles();

/**
* @fn  CASetBleServerThreadPoolHandle
* @brief  Used to Set the gThreadPool handle which is required for spawning new thread.
*
* @param[in] handle - Thread pool handle which is given by above layer for using thread creation task.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
void CASetBleServerThreadPoolHandle(u_thread_pool_t handle);

/**
* @fn  CAAddNewBleServiceInGattServer
* @brief  Used to add new OIC service in GATT server using bt_gatt_add_service api and
*               internally store service path(outparam) in global variable
*
* @param[in] serviceUUID - unique identifier for BLE OIC service.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CAAddNewBleServiceInGattServer(const char *serviceUUID);

/**
* @fn  CARemoveBleServiceFromGattServer
* @brief  Used to remove already registered service from Gatt Server using bt_gatt_remove_service api.
*
* @param[in] svcPath - unique identifier for BLE OIC service which is outparam of bt_gatt_add_service api.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CARemoveBleServiceFromGattServer(const char *svcPath);

/**
* @fn  CARemoveAllBleServicesFromGattServer
* @brief  Used to remove all the registered service from Gatt Server using bt_gatt_delete_services api.
*
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CARemoveAllBleServicesFromGattServer();

/**
* @fn  CARegisterBleServicewithGattServer
* @brief  Used to register the service in Gatt Server using bt_gatt_register_service api.
*
* @param[in] svcPath - unique identifier for BLE OIC service which is outparam of bt_gatt_add_service api.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CARegisterBleServicewithGattServer(const char *svcPath);

/**
* @fn  CAAddNewCharacteristicsToGattServer
* @brief  Used to add new characteristics(Read/Write) to the service in Gatt Server using bt_gatt_add_characteristic api.
*
* @param[in]  svcPath service path to which this characteristic belongs to.
* @param[in]  charUUID  - Gatt characteristic uuid.
* @param[in]  charValue -Gatt characteristic value.
* @param[in]  charValueLen - Caracteristic value length.
* @param[in]  read    - boolean varaible for checking whether read characteristics or write characteristics.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CAAddNewCharacteristicsToGattServer(const char *svcPath, char *charUUID, char *charValue,
        int32_t charValueLen, int32_t read);

/**
* @fn  CARemoveCharacteristicsFromGattServer
* @brief  Used to remove characteristics(Read/Write) from the service in Gatt Server.
*
* @param[in]  charPath    characteristic path registered on the interface and unique identifier for added characteristics.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CARemoveCharacteristicsFromGattServer(const char *charPath);

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
*
*/
CAResult_t CAUpdateCharacteristicsInGattServer(const char *charValue, int32_t charValueLen);

/**
* @fn  CABleGattRemoteCharacteristicWriteCb
* @brief  This is the callback which will be called when client update one of the characteristics with data.
*
* @param[in]  charPath  -  characteristic path registered on the interface and unique identifier for added characteristics.
* @param[in]  charValue  - data which is send by client.
* @param[in]  charValueLen  - length of the data.
* @param[in]  userData  - user data
*
* @return  void
*
*/
void CABleGattRemoteCharacteristicWriteCb(char *charPath,
        unsigned char *charValue, int32_t charValueLen, void *userData);

/**
* @fn  CASetBLEReqRespServerCallback
* @brief  used to store upper layer callback locally which will be used to send the data to application
*
* @param[in]  CANetworkPacketReceivedCallback  -  upper layer callback function to pass the data to CA layer.
*
* @return  void
*
*/
void CASetBLEReqRespServerCallback(CANetworkPacketReceivedCallback callback);

/**
* @fn  CALEReadDataFromLEServer
* @brief  synchronous function for reading characteristic value.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CALEReadDataFromLEServer();

/**
* @fn  CABleServerSenderQueueEnqueueMessage
* @brief  Used to enqueue the message into sender queue using CAAdapterEnqueueMessage and make signal to the thread to process.
*
* @param[in]  remoteEndpoint  Remote device information
* @param[in]  data  data which we wants to send to remote device
* @param[in]  dataLen  Length of data.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CABleServerSenderQueueEnqueueMessage(const CARemoteEndpoint_t *remoteEndpoint,
        void *data, uint32_t dataLen);

/**
* @fn  CABleServerSenderQueueProcessor
* @brief  This is the thread which will be used for processing receiver queue.
*
* @return  NULL on success or failure.
*
*/
void *CABleServerSenderQueueProcessor();

#endif //#ifndef __TZ_BLE_SERVER_H_
