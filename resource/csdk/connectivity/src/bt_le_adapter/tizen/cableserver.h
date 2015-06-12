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
 * This file contains the APIs related to the GATT Server functionalities.
 * Creation of the GattServer with the characteristics. Enabling the
 * advertisement and updating the characteristics for the response and
 * notifying the change of characteristcs for the request will be done here.
 * LE adapter will interact with this sub module.
 */

#ifndef TZ_BLE_SERVER_H_
#define TZ_BLE_SERVER_H_

#include "caadapterinterface.h"
#include "logger.h"
#include "cathreadpool.h"
#include "caleinterface.h"

/**
 * @brief  This is thread which will be used for creating ble service and advertise ble service.
 *         1. Create New OIC Service 2. Add two read & write characteristics to service.
 *         3. Register Service     4. Advertise service.
 *
 * @param data [IN] Currently it will be NULL.
 * @return  NONE
 */
void CAStartBleGattServerThread(void *data);

/**
 * @brief  Used to initialize gatt service using _bt_gatt_init_service api.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAInitBleGattService();

/**
 * @brief  Used to de-initialize gatt service using _bt_gatt_deinit_service api.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CADeInitBleGattService();

/**
 * @brief  Used to initialize all required mutex variables for GATT server implementation.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAInitGattServerMutexVariables();


/**
 * @brief  Used to terminate all required mutex variables for GATT server implementation.
 * @return NONE
 */
void CATerminateGattServerMutexVariables();

/**
 * @brief  Used to add new OIC service in GATT server using bt_gatt_add_service api and
 *         internally store service path(outparam) in global variable.
 *
 * @param  serviceUUID [IN] unique identifier for BLE OIC service.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAAddNewBleServiceInGattServer(const char *serviceUUID);

/**
 * @brief  Used to remove already registered service from Gatt Server using
 *         bt_gatt_remove_service api.
 * @param svcPath [IN] unique identifier for BLE OIC service which is outparam of
 *                     bt_gatt_add_service api.
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CARemoveBleServiceFromGattServer(const char *svcPath);

/**
 * @brief  Used to remove all the registered service from Gatt Server using
 *         bt_gatt_delete_services api.
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CARemoveAllBleServicesFromGattServer();

/**
 * @brief  Used to register the service in Gatt Server using bt_gatt_register_service api.
 *
 * @param svcPath [IN] unique identifier for BLE OIC service which is outparam of
 *                     bt_gatt_add_service api.
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CARegisterBleServicewithGattServer(const char *svcPath);

/**
 * @brief  Used to add new characteristics(Read/Write) to the service in Gatt Server using
 *         bt_gatt_add_characteristic api.
 * @param svcPath        [IN] Service path to which this characteristic belongs to.
 * @param charUUID       [IN] Gatt characteristic uuid.
 * @param charValue      [IN] Gatt characteristic value.
 * @param charValueLen   [IN] Characteristic value length.
 * @param read           [IN] Boolean variable for checking whether read characteristics or
 *                            write characteristics.
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAAddNewCharacteristicsToGattServer(const char *svcPath, const char *charUUID,
                                               const char *charValue, int charValueLen,
                                               bool read);

/**
 * @brief  Used to remove characteristics(Read/Write) from the service in Gatt Server.
 *
 * @param  charPath  [IN] Characteristic path registered on the interface and unique identifier
 *                        for added characteristics.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CARemoveCharacteristicsFromGattServer(const char *charPath);

/**
 * @brief  This is the callback which will be called when client update one of the characteristics
 *         with data.
 * @param  charPath      [IN] characteristic path registered on the interface and unique
 *                            identifier for added characteristics.
 * @param  charValue     [IN] data which is send by client.
 * @param  charValueLen  [IN] length of the data.
 * @param  remoteAddress [IN] remote device bluetooth address in which data is received.
 * @param  userData      [IN] user data
 *
 * @return NONE
 */
void CABleGattRemoteCharacteristicWriteCb(char *charPath, unsigned char *charValue,
                                          int charValueLen, const char  *remoteAddress,
                                          void *userData);

/**
 * @brief  This is the callback which will be called whenever there is change in gatt connection
 *         with Client(Connected/Disconnected).
 *
 * @param  result        [IN] The result of discovering.
 * @param  connected     [IN] State of connection.
 * @param  remoteAddress [IN] Mac address of the remote device in which we made connection.
 * @param  userData      [IN] The user data passed from the request function.
 *
 * @return  NONE
 */
void CABleGattServerConnectionStateChangedCb(int result, bool connected,
                                             const char *remoteAddress, void *userData);

/**
 * @brief  Synchronous function for reading characteristic value.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEReadDataFromLEServer();

/**
 * @brief  Used to enqueue the message into sender queue using CAAdapterEnqueueMessage and make
 *         signal to the thread to process.
 *
 * @param  remoteEndpoint [IN] Remote device information.
 * @param  data           [IN] Data to be sent to remote device.
 * @param  dataLen        [IN] Length of data.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABleServerSenderQueueEnqueueMessage
                (const CARemoteEndpoint_t *remoteEndpoint, const void *data, uint32_t dataLen);

/**
 * @brief  This is the thread which will be used for processing receiver queue.
 * @return NONE
 */
void *CABleServerSenderQueueProcessor();

#endif /* TZ_BLE_SERVER_H_ */

