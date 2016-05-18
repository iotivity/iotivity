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
 * This file contains the functionalities of GATT Client. Functionalities
 * like LE device discovery, connecting to the LE device with OIC service,
 * registering to the service and there characteristics, registering to the
 * change in the charateristics, setting the value of the characteristcs
 * for the request and response will be done here.
 */

#ifndef TZ_BLE_CLIENT_H_
#define TZ_BLE_CLIENT_H_

#include <bluetooth.h>
#include <bluetooth_type.h>
#include <bluetooth_type_internal.h>
#include <bluetooth_internal.h>

#include "cacommon.h"
#include "caadapterutils.h"
#include "caleutil.h"
#include "caadapterinterface.h"
#include "logger.h"
#include "cathreadpool.h"
#include "caleinterface.h"
#include "oic_malloc.h"


/**
 * This callback is called when a characteristic value is changed by the GATT server.
 *
 * @param[in]  characteristic The attribute handle of characteristic.
 * @param[in]  value          Value of the characteristics of a service.
 * @param[in]  valueLen       Length of data.
 * @param[in]  userData       The user data passed from the request function.
 */
void CALEGattCharacteristicChangedCb(bt_gatt_h characteristic,
                                     char *value, int valueLen, void *userData);
/**
 * This callback will be called when the client write request is completed.
 *
 * @param[in]  result    Result of write value.
 * @param[in]  reqHandle The request GATT handle.
 * @param[in]  userData  User context
 */
void CALEGattCharacteristicWriteCb(int result, bt_gatt_h reqHandle, void *userData);

/**
 * This is the callback which will be called whenever there is change in gatt connection
 * with server(Connected/Disconnected)
 *
 * @param[in]  connected     State of connection
 * @param[in]  remoteAddress Mac address of the remote device in which we made connection.
 */
void CALEGattConnectionStateChanged(bool connected, const char *remoteAddress);

/**
 * This is the callback which will be called when LE advertisement is found.
 *
 * @param[in]  result         The result of Scanning
 * @param[in]  scanInfo       Remote Device information.
 * @param[in]  userData       The user data passed from the request function
 */
void CALEAdapterScanResultCb(int result, bt_adapter_le_device_scan_result_info_s *scanInfo,
                             void *userData);

/**
 * This thread will be used to initialize the Gatt Client and start device discovery.
 * 1. Setting neccessary callbacks for connection, characteristics changed and discovery.
 * 2. Start device discovery
 *
 * @param[in] data Currently it will be NULL(no parameter)
 */
void CAStartLEGattClientThread(void *data);

/**
 * This thread will be used to Start the timer for scanning.
 *
 * @param[in] data Currently it will be NULL(no parameter)
 */
void CAStartTimerThread(void *data);

/**
 * Used to initialize all required mutex variable for Gatt Client implementation.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CAInitGattClientMutexVariables();

/**
 * Used to terminate all required mutex variable for Gatt Client implementation.
 */
void CATerminateGattClientMutexVariables();

/**
 * Used to register required callbacks to LE platform(connection, discovery, etc).
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CALEGattSetCallbacks();

/**
 * Used to unset all the registerd callbacks to BLE platform.
 */
void CALEGattUnSetCallbacks();

/**
 * Used to start LE Scanning.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CALEGattStartDeviceScanning();

/**
 * Used to stop LE discovery for BLE  devices.
 */
void CALEGattStopDeviceScanning();

/**
 * This is the thread  which will be used for making gatt connection with
 * remote devices.
 * @param[in] remoteAddress MAC address of remote device to connect.
 */
void CAGattConnectThread (void *remoteAddress);

/**
 * Used to do connection with remote device.
 *
 * @param[in] remoteAddress Remote address inwhich we wants to connect with.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CALEGattConnect(const char *remoteAddress);

/**
 * Used to do disconnection with remote device.
 * @param[in] remoteAddress Remote address inwhich we wants to disconnect with.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CALEGattDisConnect(const char *remoteAddress);

/**
 * This is thread which will be spawned for discovering ble services. Once
 * called discover api, then it will be terminated.
 * @param[in] remoteAddress Mac address of the remote device in which we
 *                           want to search services.
 */
void CADiscoverLEServicesThread (void *remoteAddress);

/**
 * Used to discover the services and characteristics that is advertised by Gatt
 * Server.
 *
 * @param[in] remoteAddress MAC address of remote device in which we want to discover
 *                      the services and characteristics.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
CAResult_t CALEGattDiscoverServices(const char *remoteAddress);
#endif /* TZ_BLE_CLIENT_H_ */
