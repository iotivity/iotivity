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
#ifndef _BLE_CLIENT_
#define _BLE_CLIENT_

#include <bluetooth.h>
#include <bluetooth_type.h>
#include <bluetooth_product.h>

#include "cacommon.h"
#include "caadapterutils.h"
#include "cableclientutil.h"
#include "caadapterinterface.h"
#include "logger.h"
#include "uthreadpool.h"
#include "caleinterface.h"


/**
* @fn  CABleGattCharacteristicChangedCb
* @brief  This is the callback which will be called after the characteristic value changes happen.
*
* @param[in]  characteristic  The attribute handle of characteristic
* @param[in]  value  Value of the characteristics of a service.
* @param[in]  valueLen  length of data.
* @param[in]  userData  The user data passed from the request function
*
* @return  0 on failure and 1 on success.
*
*/
void CABleGattCharacteristicChangedCb(bt_gatt_attribute_h characteristic, unsigned char *value,
                                      int32_t valueLen, void *userData);
/**
* @fn  CABleGattCharacteristicWriteCb
* @brief  This is the callback which will be called after the characteristics changed.
*
* @param[in]  result  result of write value
* @param[in]  userData  user context
*
* @return  void
*
*/
void CABleGattCharacteristicWriteCb(int32_t result, void *userData);

/**
* @fn  CABleGattDescriptorDiscoveredCb
* @brief  This is the callback which will be called when descriptor of characteristics is found.
*
* @param[in]  result  The result of discovering
* @param[in]  format  format of descriptor.
* @param[in]  total  The total number of descriptor in a characteristic
* @param[in]  descriptor  The attribute handle of descriptor
* @param[in]  characteristic  The attribute handle of characteristic
* @param[in]  userData  The user data passed from the request function
*
* @return  void
*
*/
void CABleGattDescriptorDiscoveredCb(int32_t result, unsigned char format, int32_t total,
                                     bt_gatt_attribute_h descriptor, bt_gatt_attribute_h characteristic, void *userData);

/**
* @fn  CABleGattCharacteristicsDiscoveredCb
* @brief  This is the callback which will be called after the characteristics are discovered by
*            bt_gatt_discover_characteristics()
*
* @param[in]  result  The result of discovering
* @param[in]  inputIndex  The index of characteristics in a service, starts from 0
* @param[in]  total  The total number of characteristics in a service
* @param[in]  characteristic  The attribute handle of characteristic
* @param[in]  userData  The user data passed from the request function
*
* @return  0 on failure and 1 on success.
*
*/
bool CABleGattCharacteristicsDiscoveredCb(int32_t result, int32_t inputIndex, int32_t total,
        bt_gatt_attribute_h characteristic, void *userData);

/**
* @fn  CABtGattBondCreatedCb
* @brief  This is the callback which will be called when bond created with remote device.
*
* @param[in]  result  The result of bond creation.
* @param[in]  device_info  remote device information
* @param[in]  userData  The user data passed from the request function
*
* @return  void
*
*/
void CABtGattBondCreatedCb(int32_t result, bt_device_info_s *device_info, void *user_data);

/**
* @fn  CABleGattPrimaryServiceCb
* @brief  This is the callback which will be called when we get the primary services repeatedly.
*
* @param[in] service -The attribute handle of service. Unique identifier for service.
* @param[in] index -The current index of the service
* @param[in] count -Total number of services available in remote device
* @param[in] userData - user data
*
* @return  0 on failure and 1 on success.
*
*/
bool CABleGattPrimaryServiceCb(bt_gatt_attribute_h service, int32_t index, int32_t count,
                                   void *userData);

/**
* @fn  CABleGattConnectionStateChangedCb
* @brief  This is the callback which will be called whenever there is change in gatt connection
*            with server(Connected/Disconnected)
*
* @param[in]  result  The result of discovering
* @param[in]  connected  State of connection
* @param[in]  remoteAddress  Mac address of the remote device in which we made connection.
* @param[in]  userData  The user data passed from the request function
*
* @return  void
*
*/
void CABleGattConnectionStateChangedCb(int32_t result, bool connected, const char *remoteAddress,
                                       void *userData);

/**
* @fn  CABtAdapterLeDeviceDiscoveryStateChangedCb
* @brief  This is the callback which will be called when the device discovery state changes.
*
* @param[in]  result  The result of discovering
* @param[in]  discoveryState  State of the discovery(FOUND/STARTED/ FINISHED)
* @param[in]  discoveryInfo  Remote Device information.
* @param[in]  userData  The user data passed from the request function
*
* @return  void
*
*/
void CABtAdapterLeDeviceDiscoveryStateChangedCb(int32_t result,
        bt_adapter_le_device_discovery_state_e discoveryState,
        bt_adapter_le_device_discovery_info_s *discoveryInfo,
        void *userData);

/**
* @fn  CAPrintDiscoveryInformation
* @brief  Used to print device information(Util method)
*
* @param[in]  discoveryInfo  Device information structure.
*
* @return  void
*
*/
void CAPrintDiscoveryInformation(bt_adapter_le_device_discovery_info_s *discoveryInfo);

/**
* @fn  CASetBleClientThreadPoolHandle
* @brief  Used to Set the gThreadPool handle which is required for spawning new thread.
*
* @param[in] handle - Thread pool handle which is given by above layer for using thread creation task.
*
* @return  void
*
*/
void CASetBleClientThreadPoolHandle(u_thread_pool_t handle);

/**
* @fn  CAStartBleGattClientThread
* @brief  This is thread which will be used to initialize the Gatt Client and start device discovery.
*              1. Set scan parameters 2.Setting neccessary callbacks for connection, characteristics changed and
*                  discovery and bond creation.
*              3. Start device discovery
*
* @param[in] data - Currently it will be NULL(no parameter)
*
* @return  NULL on success or failure.
*
*/
void *CAStartBleGattClientThread(void *data);

/**
* @fn  CAInitGattClientMutexVaraibles
* @brief  Used to initialize all required mutex variable for Gatt Client implementation.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CAInitGattClientMutexVaraibles();

/**
* @fn  CATerminateGattClientMutexVaraibles
* @brief  Used to terminate all required mutex variable for Gatt Client implementation.
*
* @return  void
*
*/
void CATerminateGattClientMutexVariables();

/**
* @fn  CAClearNonOICDeviceList
* @brief  Used to clear NonOICDeviceList
*
* @return  void
*
*/
void CAClearNonOICDeviceList();

/**
* @fn  CABleGattSetScanParameter
* @brief  Used to set scan parameter of starting discovery.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CABleGattSetScanParameter();

/**
* @fn  CABleGattSetCallbacks
* @brief  Used to register required callbacks to BLE platform(connection, discovery, bond and etc).
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CABleGattSetCallbacks();

/**
* @fn  CABleGattUnSetCallbacks
* @brief  Used to unset all the registerd callbacks to BLE platform
*
* @return  void
*
*/
void CABleGattUnSetCallbacks();

/**
* @fn  CABleGattWatchCharacteristicChanges
* @brief  Used to watch all the changes happening in characteristics of the service.
*
* @param[in] service -  The attribute handle of the service.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CABleGattWatchCharacteristicChanges(bt_gatt_attribute_h service);

/**
* @fn  CABleGattUnWatchCharacteristicChanges
* @brief  Used to unwatch characteristics changes using bt_gatt_unwatch_characteristic_changes
*
* @return  void
*
*/
void CABleGattUnWatchCharacteristicChanges();

/**
* @fn  CABleGattStartDeviceDiscovery
* @brief  Used to start LE discovery for BLE  devices
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CABleGattStartDeviceDiscovery();

/**
* @fn  CABleGattStopDeviceDiscovery
* @brief  Used to stop LE discovery for BLE  devices
*
* @return  void
*
*/
void CABleGattStopDeviceDiscovery();

/**
* @fn  CAGattConnectThread
* @brief  This is the thread  which will be used for making gatt connection with remote devices
*
* @param[in]  remoteAddress  Mac address of remote device which wants to connect with.
*
* @return  NULL on success or failure.
*
*/
void *CAGattConnectThread (void *remoteAddress);

/**
* @fn  CABleGattConnect
* @brief  Used to do connection with remote device
*
* @param[in] remoteAddress -  Remote address inwhich we wants to connect with
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CABleGattConnect(const char *remoteAddress);

/**
* @fn  CABleGattDisConnect
* @brief  Used to do disconnection with remote device
*
* @param[in] remoteAddress -  Remote address inwhich we wants to disconnect with
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CABleGattDisConnect(const char *remoteAddress);

/**
* @fn  CADiscoverBLEServicesThread
* @brief  This is thread which will be spawned for discovering ble services. Once called discover api,
*            then it will be terminated.
*
* @param[in]  remoteAddress  Mac address of the remote device in which we want to search services.
*
* @return  NULL for success or failure.
*
*/
void *CADiscoverBLEServicesThread (void *remoteAddress);

/**
* @fn  CABleGattDiscoverServices
* @brief  Used to discover the services that is advertised by Gatt Server asynchrounously.
*
* @param[in] remoteAddress - mac address of remote device inwhich we want to discover the services.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CABleGattDiscoverServices(const char *remoteAddress);

/**
* @fn  CADiscoverCharThread
* @brief  This is the thread which will be used for finding characteristic of a service.
*
* @param[in]  stServiceInfo  Service Information which contains the remote address and service handle,
*                                         characteristic handle.
*
* @return  NULL on success or failure.
*
*/
void *CADiscoverCharThread(void *stServiceInfo);

/**
* @fn  CABleGattDiscoverCharacteristics
* @brief  Used to discover characteristics of service using  bt_gatt_discover_characteristics api.
*
* @param[in] service -  The attribute handle for service.
* @param[in] remoteAddress -  Remote address inwhich we wants to discover characteristics of given service handle.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CABleGattDiscoverCharacteristics(bt_gatt_attribute_h service, const char *remoteAddress);

/**
* @fn  CADiscoverDescriptorThread
* @brief  This is the thread which will be used for finding descriptor of characteristic.
*
* @param[in]  stServiceInfo  Service Information which contains the remote address and service handle,
*                                         characteristic handle.
*
* @return  NULL on success or failure.
*
*/
void *CADiscoverDescriptorThread(void *stServiceInfo);

/**
* @fn  CABleGattDiscoverDescriptor
* @brief  This is thread which will be used for calling CASetCharacteristicDescriptorValue api.
*
* @param[in] service -  The attribute handle for characteristics.
* @param[in] remoteAddress -  Remote address inwhich we wants to discover descriptor of given char handle.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CABleGattDiscoverDescriptor(bt_gatt_attribute_h service, const char *remoteAddress);

/**
* @fn  CASetCharacteristicDescriptorValueThread
* @brief  This is thread which will be used for calling CASetCharacteristicDescriptorValue api.
*
* @param[in] stServiceInfo -  Service Information which contains the remote address and service handle,
*                                          characteristic handle.
*
* @return NULL on sucess or failure.
*
*/
void *CASetCharacteristicDescriptorValueThread(void *stServiceInfo);

/**
* @fn  CASetCharacteristicDescriptorValue
* @brief  Used to set characteristic descriptor value using bt_gatt_set_characteristic_desc_value_request api.
*
* @param[in]  stGattCharDescriptor_t  structure which contains char handle and descriptor handle.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CASetCharacteristicDescriptorValue(stGattCharDescriptor_t *stGattCharDescriptorInfo);

/**
* @fn  CAGATTCreateBondThread
* @brief  This is the thread  which will be used for creating bond with remote device.
*
* @param[in]  stServiceInfo  Service Information which contains the remote address and service handle,
*                                         characteristic handle.
*
* @return  NULL on success or failure.
*
*/
void *CAGATTCreateBondThread(void *stServiceInfo);

/**
* @fn  CABleGATTCreateBond
* @brief  Used to make LE bond with remote device(pairng the device) using bt_device_create_bond api.
*
* @param[in] remoteAddress - mac address of remote device which we want to bond with
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CABleGATTCreateBond(const char *remoteAddress);

/**
* @fn  CABleClientSenderQueueEnqueueMessage
* @brief  Used to enqueue the message into sender queue using CAAdapterEnqueueMessage and make signal
*            to the thread to process.
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
CAResult_t CABleClientSenderQueueEnqueueMessage(const CARemoteEndpoint_t *remoteEndpoint,
        void *data, uint32_t dataLen);

/**
* @fn  CABleClientSenderQueueProcessor
* @brief  This is the thread which will be used for processing sender queue.
*
* @return  NULL on success or failure.
*
*/
void *CABleClientSenderQueueProcessor();

/**
* @fn  CALEReadDataFromLEClient
* @brief  synchronous function for reading characteristic value.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CALEReadDataFromLEClient();

#endif //#ifndef _BLE_CLIENT_
