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
 * Creation of the GATT Server with the characteristics. Enabling the
 * advertisement and updating the characteristics for the response and
 * notifying the change of characteristcs for the request will be done here.
 */

#ifndef BLE_ADAPTER_ARDUINO_H_
#define BLE_ADAPTER_ARDUINO_H_

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
void CANotifyCallback(const void *data, int32_t dataLen, const char *senderAdrs,
                      int32_t senderPort);

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

#endif /* BLE_ADAPTER_ARDUINO_H_ */

