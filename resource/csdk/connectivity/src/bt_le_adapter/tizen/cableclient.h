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
#include <bluetooth_product.h>

#include "cacommon.h"
#include "caadapterutils.h"
#include "cableutil.h"
#include "caadapterinterface.h"
#include "logger.h"
#include "cathreadpool.h"
#include "caleinterface.h"
#include "oic_malloc.h"


/**
 * @brief  This is the callback which will be called after the characteristic value changes happen.
 *
 * @param  characteristic [IN] The attribute handle of characteristic
 * @param  value          [IN] Value of the characteristics of a service.
 * @param  valueLen       [IN] length of data.
 * @param  userData       [IN] The user data passed from the request function
 * @return  NONE
 */
void CABleGattCharacteristicChangedCb(bt_gatt_attribute_h characteristic,
                            unsigned char *value, int valueLen, void *userData);
/**
 * @brief  This is the callback which will be called after the characteristics changed.
 *
 * @param  result   [IN] result of write value
 * @param  userData [IN] user context
 *
 * @return  NONE
 */
void CABleGattCharacteristicWriteCb(int result, void *userData);

/**
 * @brief  This is the callback which will be called when descriptor of characteristics is found.
 *
 * @param  result         [IN] The result of discovering
 * @param  format         [IN] format of descriptor.
 * @param  total          [IN] The total number of descriptor in a characteristic
 * @param  descriptor     [IN] The attribute handle of descriptor
 * @param  characteristic [IN] The attribute handle of characteristic
 * @param  userData       [IN] The user data passed from the request function
 * @return  NONE
 */
void CABleGattDescriptorDiscoveredCb(int result, unsigned char format, int total,
                                     bt_gatt_attribute_h descriptor,
                                     bt_gatt_attribute_h characteristic, void *userData);

/**
 * @brief  This is the callback which will be called after the characteristics are discovered by
 *         bt_gatt_discover_characteristics()
 *
 * @param  result         [IN] The result of discovering
 * @param  inputIndex     [IN] The index of characteristics in a service, starts from 0
 * @param  total          [IN] The total number of characteristics in a service
 * @param  characteristic [IN] The attribute handle of characteristic
 * @param  userData       [IN] The user data passed from the request function
 *
 * @return  0 on failure and 1 on success.
 */
bool CABleGattCharacteristicsDiscoveredCb(int result, int inputIndex, int total,
                                          bt_gatt_attribute_h characteristic, void *userData);

/**
 * @brief  This is the callback which will be called when we get the primary services repeatedly.
 *
 * @param service  [IN] The attribute handle of service. Unique identifier for service.
 * @param index    [IN] The current index of the service
 * @param count    [IN] Total number of services available in remote device
 * @param userData [IN] user data
 *
 * @return  0 on failure and 1 on success.
 */
bool CABleGattPrimaryServiceCb(bt_gatt_attribute_h service, int index, int count,
                                   void *userData);

/**
 * @brief  This is the callback which will be called whenever there is change in gatt connection
 *         with server(Connected/Disconnected)
 *
 * @param  result        [IN] The result of discovering
 * @param  connected     [IN] State of connection
 * @param  remoteAddress [IN] Mac address of the remote device in which we made connection.
 * @param  userData      [IN] The user data passed from the request function
 *
 * @return  NONE
 */
void CABleGattConnectionStateChangedCb(int result, bool connected,
                const char *remoteAddress,void *userData);

/**
 * @brief  This is the callback which will be called when the device discovery state changes.
 *
 * @param  result         [IN] The result of discovering
 * @param  discoveryState [IN] State of the discovery(FOUND/STARTED/ FINISHED)
 * @param  discoveryInfo  [IN] Remote Device information.
 * @param  userData       [IN] The user data passed from the request function
 *
 * @return  NONE
 */
void CABtAdapterLeDeviceDiscoveryStateChangedCb(int result,
        bt_adapter_le_device_discovery_state_e discoveryState,
        bt_adapter_le_device_discovery_info_s *discoveryInfo,
        void *userData);

/**
 * @brief  Used to print device information(Util method)
 * @param discoveryInfo [IN] Device information structure.
 * @return  NONE
 */
void CAPrintDiscoveryInformation(const bt_adapter_le_device_discovery_info_s *discoveryInfo);

/**
 * @brief This thread will be used to initialize the Gatt Client and start device discovery.
 *        1. Set scan parameters
 *        2. Setting neccessary callbacks for connection, characteristics changed and discovery.
 *        3. Start device discovery
 *
 * @param data [IN] Currently it will be NULL(no parameter)
 *
 * @return NONE
 *
 */
void CAStartBleGattClientThread(void *data);

/**
 * @brief  Used to initialize all required mutex variable for Gatt Client implementation.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAInitGattClientMutexVariables();

/**
 * @brief  Used to terminate all required mutex variable for Gatt Client implementation.
 * @return NONE
 */
void CATerminateGattClientMutexVariables();

/**
 * @brief  Used to clear NonOICDeviceList
 * @return NONE
 */
void CAClearNonOICDeviceList();

/**
 * @brief  Used to set scan parameter of starting discovery.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABleGattSetScanParameter();

/**
 * @brief  Used to register required callbacks to BLE platform(connection, discovery, etc).
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABleGattSetCallbacks();

/**
 * @brief  Used to unset all the registerd callbacks to BLE platform
 * @return NONE
 */
void CABleGattUnSetCallbacks();

/**
 * @brief  Used to watch all the changes happening in characteristics of the service.
 *
 * @param service [IN] The attribute handle of the service.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABleGattWatchCharacteristicChanges(bt_gatt_attribute_h service);

/**
 * @brief  Used to unwatch characteristics changes using bt_gatt_unwatch_characteristic_changes
 * @return NONE
 */
void CABleGattUnWatchCharacteristicChanges();

/**
 * @brief  Used to start LE discovery for BLE  devices
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABleGattStartDeviceDiscovery();

/**
 * @brief  Used to stop LE discovery for BLE  devices
 * @return NONE
 */
void CABleGattStopDeviceDiscovery();

/**
 * @brief  This is the thread  which will be used for making gatt connection with remote devices
 * @param remoteAddress [IN] MAC address of remote device to connect
 * @return NONE
 */
void CAGattConnectThread (void *remoteAddress);

/**
 * @brief  Used to do connection with remote device
 *
 * @param remoteAddress [IN] Remote address inwhich we wants to connect with
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABleGattConnect(const char *remoteAddress);

/**
 * @brief  Used to do disconnection with remote device
 * @param remoteAddress [IN] Remote address inwhich we wants to disconnect with
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABleGattDisConnect(const char *remoteAddress);

/**
 * @brief  This is thread which will be spawned for discovering ble services. Once called discover
 *         api, then it will be terminated.
 * @param remoteAddress [IN] Mac address of the remote device in which we want to search services.
 * @return  NONE
 */
void CADiscoverBLEServicesThread (void *remoteAddress);

/**
 * @brief Used to discover the services that is advertised by Gatt Server asynchrounously.
 *
 * @param remoteAddress [IN] MAC address of remote device in which we want to discover the services.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABleGattDiscoverServices(const char *remoteAddress);

/**
 * @brief  This is the thread which will be used for finding characteristic of a service.
 *
 * @param  stServiceInfo [IN] Service Information which contains the remote address, service
 *                            handle and characteristic handle.
 * @return  NONE
 */
void CADiscoverCharThread(void *stServiceInfo);

/**
 * @brief  Used to discover characteristics of service using  bt_gatt_discover_characteristics api.
 *
 * @param service       [IN]  The attribute handle for service.
 * @param remoteAddress [IN]  Remote address inwhich we wants to discover characteristics of
 *                            given service handle.
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABleGattDiscoverCharacteristics(bt_gatt_attribute_h service,
                    const char *remoteAddress);

/**
 * @brief  This is the thread which will be used for finding descriptor of characteristic.
 *
 * @param  stServiceInfo [IN] Service Information which contains the remote address, service
 *                            handle and characteristic handle.
 * @return  NONE
 */
void CADiscoverDescriptorThread(void *stServiceInfo);

/**
 * @brief  This is thread which will be used for calling CASetCharacteristicDescriptorValue api.
 *
 * @param service       [IN]  The attribute handle for characteristics.
 * @param remoteAddress [IN]  Remote address inwhich we wants to discover descriptor of given
 *                            char handle.
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABleGattDiscoverDescriptor(bt_gatt_attribute_h service,
                const char *remoteAddress);

/**
 * @brief  This is thread which will be used for calling CASetCharacteristicDescriptorValue api.
 *
 * @param  stServiceInfo [IN] Service Information which contains the remote address, service
 *                            handle and characteristic handle.
 * @return NONE
 */
void CASetCharacteristicDescriptorValueThread(void *stServiceInfo);

/**
 * @brief  Used to set characteristic descriptor value using
 *         bt_gatt_set_characteristic_desc_value_request api.
 * @param  stGattCharDescriptorInfo [IN] Structure which contains char handle and descriptor handle.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CASetCharacteristicDescriptorValue
            (stGattCharDescriptor_t *stGattCharDescriptorInfo);

/**
 * @brief  Used to enqueue the message into sender queue using CAAdapterEnqueueMessage and make
 *         signal to the thread to process.
 *
 * @param  remoteEndpoint [IN] Remote device information
 * @param  data           [IN] Data to be sent to remote device
 * @param  dataLen        [IN] Length of data.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CABleClientSenderQueueEnqueueMessage
                            (const CARemoteEndpoint_t *remoteEndpoint,
                                                const void *data, uint32_t dataLen);

/**
 * @brief  This is the thread which will be used for processing sender queue.
 *
 * @return  NONE
 */
void CABleClientSenderQueueProcessor();

/**
 * @brief Synchronous function for reading characteristic value.
 *
 * @return #CA_STATUS_OK or Appropriate error code
 * @retval #CA_STATUS_OK  Successful
 * @retval #CA_STATUS_INVALID_PARAM  Invalid input argumets
 * @retval #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEReadDataFromLEClient();

#endif /* TZ_BLE_CLIENT_H_ */

