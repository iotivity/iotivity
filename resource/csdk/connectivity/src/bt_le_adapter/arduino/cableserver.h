
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
 * @return - int
 */
CAResult_t CAInitializeBle();

/**
 * @brief API to send received data to upper layer
 * @param[in] data - data received from BLE characteristics
 * @param[in] dataLen - received data Length
 * @param[in] senderAdrs - sender Address.
 * @param[in] senderPort - sender port
 * @return - Error Code
 */
void CANotifyCallback(void *data, int32_t dataLen, char *senderAdrs, int32_t senderPort);

/**
 * @brief API to add new service in Gatt Server
 * @param[in] service_uuid - unique identifier for each service.
 * @return - Error Code
 */
CAResult_t CAAddNewBleServiceInGattServer(const char *service_uuid);

/**
 * @brief API to remove the added service from  Ble Gatt server.
 * @param[in] svc_path - unique identifier for each service.
 * @return - Error Code
 */
CAResult_t CARemoveBleServiceFromGattServer(const char *svc_path);

/**
 * @brief API to remove all the services from Ble Gatt server.
 * @return - Error Code
 */
CAResult_t CARemoveAllBleServicesFromGattServer();

/**
 * @brief API to register service with GattService after adding the service.
 * @param[in] svc_path - unique identifier for each service.
 * @return - Error Code
 */
CAResult_t CARegisterBleServicewithGattServer(const char *svc_path);

/**
 * @brief API to add new characteristics to the service
 * @param[in]  svc_path service path to which this characteristic belongs to.
 * @param[in]  char_uuid    Gatt characteristic uuid.
 * @param[in]  char_value   Gatt characteristic value.
 * @param[in]  value_len    Caracteristic value length.
 * @param[in]  char_flags   Characteristic flags.
 * @param[in]  flags_length Caracteristic flags length.
 * @param[out] char_path    characteristic path registered on the interface.
 * @return - Error Code
 */
CAResult_t CAAddNewCharacteristicsToGattServer(const char *svc_path,
        const char *char_uuid,
        const char *char_value,
        int char_value_len,
        int read);

/**
 * @brief API to remove the characteristics.
 * @param[in]  char_path    characteristic path registered on the interface and unique
 * identifier for added characteristics.
 * @return - Error Code
 */
CAResult_t CARemoveCharacteristicsFromGattServer(const char *char_path);

/**
 * @brief API to check whether data is available in BLE shield
 * @return - unsigned char
 */
unsigned char CAIsBleDataAvailable();

/**
 * @brief API to check whether client is connected with BLE Shield
 * @return - unsigned char
 */
unsigned char CAIsBleConnected();

/**
 * @brief API to read data from BLE shield
 * @return - char
 */
char CAReadBleData();

/**
 * @brief API to perform BLE events
 * @return - Error Code
 */
CAResult_t CABleDoEvents();

/**
 * @brief  API used to send data to connected ble client
 *
 * @param[in]  data  -  data to be sent
 * @param[in]  len  -  data length
 *
 * @return  Error Code
 */
CAResult_t CAWriteBleData(unsigned char *data, uint8_t len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //#ifndef _BLE_ADAPTER_ARDUINO_H_
