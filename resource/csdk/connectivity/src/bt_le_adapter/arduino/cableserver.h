
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
 * @file cableserver.h
 * @brief This file contains the APIs related to the GATT Server functionalities.
 *        Creation of the GattServer with the characteristics. Enabling the advertisement
 *        and updating the characteristics for the response and notifying the change of
 *        characteristcs for the request will be done here
 */

#ifndef _BLE_ADAPTER_ARDUINO_H_
#define _BLE_ADAPTER_ARDUINO_H_

#include "cacommon.h"
#include "caadapterinterface.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief API to initialize Arduino BLE module and advertise the service
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAInitializeBle();

/**
 * @brief API to Terminate Arduino BLE module and advertise the service
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CATerminateBle();

/**
 * @brief Send the received data to Connectivity Abstraction layer.
 * @param data       [IN] Data received from BLE characteristics
 * @param dataLen    [IN] Received data Length
 * @param senderAdrs [IN] Sender Address.
 * @param senderPort [IN] Sender port
 * @return #CA_STATUS_OK or Appropriate error code
 */
void CANotifyCallback(void *data, int32_t dataLen, char *senderAdrs, int32_t senderPort);

/**
 * @brief Adds a new service in Gatt Server
 * @param service_uuid [IN] Unique identifier for each service.
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAAddNewBleServiceInGattServer(const char *service_uuid);

/**
 * @brief Remove the added service from  Ble Gatt server.
 * @param svc_path [IN] Unique identifier for each service.
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CARemoveBleServiceFromGattServer(const char *svc_path);

/**
 * @brief Remove all the services from Ble Gatt server.
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CARemoveAllBleServicesFromGattServer();

/**
 * @brief Register service with GattService after adding the service.
 * @param svc_path [IN] Unique identifier for each service.
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CARegisterBleServicewithGattServer(const char *svc_path);

/**
 * @brief Add new characteristics to the service
 * @param svc_path        [IN] Service path to which this characteristic belongs to.
 * @param char_uuid       [IN] Gatt characteristic uuid.
 * @param char_value      [IN] Gatt characteristic value.
 * @param char_value_len  [IN] Characteristic value length.
 * @param read            [IN] Boolean variable for checking whether read characteristics or
 *                             write characteristics.
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAAddNewCharacteristicsToGattServer(const char *svc_path, const char *char_uuid,
                                               const char *char_value, int char_value_len,
                                               int read);

/**
 * @brief Remove the characteristics from BLE GATT server
 * @param  char_path  [IN] characteristic path registered on the interface and unique
 *                         identifier for added characteristics.
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CARemoveCharacteristicsFromGattServer(const char *char_path);

/**
 * @brief API to check whether data is available in BLE shield
 * @return - Received buffer length
 */
unsigned char CAIsBleDataAvailable();

/**
 * @brief API to check whether client is connected with BLE Shield
 * @return - Connection state
 */
unsigned char CAIsBleConnected();

/**
 * @brief API to read data from BLE shield
 * @return - Data read
 */
char CAReadBleData();

/**
 * @brief API to perform BLE events
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CABleDoEvents();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //#ifndef _BLE_ADAPTER_ARDUINO_H_
