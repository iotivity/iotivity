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
 * This file contains the APIs for BT LE communications.
 */

#ifndef __CA_LECORE_H_
#define __CA_LECORE_H_

#include "cacommon.h"
#include "cathreadpool.h"
#include "jni.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Callback to be notified on reception of any data from remote devices.
 * @param  address          [IN] MAC address of remote device.
 * @param  data             [IN] Data received from remote device.
 * @return NONE
 * @pre  Callback must be registered using CALESetCallback(CAPacketReceiveCallback callback)
 */
typedef void (*CAPacketReceiveCallback)(const char *address, const char *data);

/**
 * @brief   set context of application
 * @param   env              [IN] JNI interface pointer
 * @param   context          [IN] context of application
 * @return  None
 */
void CALEClientJNISetContext(JNIEnv *env, jobject context);

/**
 * @brief   create interface object and initialize the object
 * @return  None
 */
void CALeCreateJniInterfaceObject();

/**
 * @brief   initialize client for BLE
 * @param   handle           [IN] thread pool handle object
 * @return  None
 */
void CALEInitialize(ca_thread_pool_t handle);

/**
 * @brief   terminate client for BLE
 * @return  None
 */
void CALETerminate();

/**
 * @brief   for destroy sending routine
 * @param   env              [IN] JNI interface pointer
 * @param   gatt             [IN] Gatt profile object
 * @return  None
 */
void CANativeSendFinish(JNIEnv *env, jobject gatt);

/**
 * @brief   send data for unicast (interface)
 * @param   address          [IN] remote address
 * @param   data             [IN] data for transmission
 * @param   dataLen          [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALESendUnicastMessage(const char *address, const char *data, const uint32_t dataLen);

/**
 * @brief   send data for multicast (interface)
 * @param   data             [IN] data for transmission
 * @param   dataLen          [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALESendMulticastMessage(const char *data, const uint32_t dataLen);

/**
 * @brief   start unicast server
 * @param   address          [IN] remote address
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEStartUnicastServer(const char *address);

/**
 * @brief   start multicast server (start discovery)
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEStartMulticastServer();

/**
 * @brief   stop unicast server
 * @return  None
 */
void CALEStopUnicastServer();

/**
 * @brief   stop multicast server (stop discovery)
 * @return  None
 */
void CALEStopMulticastServer();

/**
 * @brief   set this callback for receiving data packets from peer devices.
 * @param   callback         [IN] callback to be notified on reception of
 *                                unicast/multicast data packets.
 * @return  None
 */
void CALESetCallback(CAPacketReceiveCallback callback);

/**
 * @brief   get local address (interface)
 * @param   address          [OUT] local address
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEGetInterfaceInfo(char **address);

/**
 * @brief   get local address (inplement)
 * @param   address          [OUT] local address
 * @return  None
 */
void CALEGetLocalAddress(char** address);

/**
 * @brief   send data for unicast (implement)
 * @param   address          [IN] remote address
 * @param   data             [IN] data for transmission
 * @param   dataLen          [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALESendUnicastMessageImpl(const char *address, const char *data, const uint32_t dataLen);

/**
 * @brief   send data for multicast (implement)
 * @param   address          [IN] remote address
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALESendMulticastMessageImpl(const char *data, const uint32_t dataLen);

/**
 * BT Common Method : JNI
 */
/**
 * @brief   get address from bluetooth gatt object
 * @param   env              [IN] JNI interface pointer
 * @param   gatt             [IN] Gatt profile object
 * @return  bluetooth address
 */
jstring CANativeGetAddressFromGattObj(JNIEnv *env, jobject gatt);

/**
 * @brief   get remote address from bluetooth socket object
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothSocketObj    [IN] bluetooth socket
 * @return  bluetooth address
 */
jstring CANativeGetRemoteAddress(JNIEnv *env, jobject bluetoothSocketObj);

/**
 * @brief   close gatt
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @return  None
 */
void CANativeGattClose(JNIEnv *env, jobject bluetoothGatt);

/**
 * @brief   start to scan whole bluetooth devices (interface)
 * @return  None
 */
void CANativeLEStartScan();

/**
 * @brief   start to scan whole bluetooth devices (implement)
 * @param   env              [IN] JNI interface pointer
 * @param   callback         [IN] callback to receive device object by scanning
 * @return  None
 */
void CANativeLEStartScanImpl(JNIEnv *env, jobject callback);

/**
 * @brief   start to scan target bluetooth devices for service uuid (implement)
 * @param   env              [IN] JNI interface pointer
 * @param   uuids            [IN] target UUID
 * @param   callback         [IN] callback to receive device object by scanning
 * @return  None
 */
void CANativeLEStartScanWithUUIDImpl(JNIEnv *env, jobjectArray uuids, jobject callback);

/**
 * @brief   get uuid object
 * @param   env              [IN] JNI interface pointer
 * @param   uuid             [IN] uuid
 * @return  uuid object
 */
jobject CANativeGetUUIDObject(JNIEnv *env, const char *uuid);

/**
 * @brief   stop scan (interface)
 * @return  None
 */
void CANativeLEStopScan();

/**
 * @brief   stop scan (implement)
 * @param   env              [IN] JNI interface pointer
 * @param   callback         [IN] callback to receive device object by scanning
 * @return  None
 */
void CANativeLEStopScanImpl(JNIEnv *env, jobject callback);

/**
 * @brief   connect to gatt server hosted
 * @param   env              [IN] JNI interface pointer
 * @param   bluetoothDevice  [IN] bluetooth Device object
 * @param   context          [IN] context of application
 * @param   autoconnect      [IN] whether to directly connect to the remote device(false) or
 *                                to automatically connect as soon as the remote device
 *                                becomes available
 * @param   callback         [IN] callback for connection state change
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CANativeLEConnect(JNIEnv *env, jobject bluetoothDevice, jobject context,
                          jboolean autoconnect, jobject callback);

/**
 * @brief   disconnect to gatt server by a target device
 * @param   env              [IN] JNI interface pointer
 * @param   bluetoothGatt    [IN] Gatt profile object
 * @return  None
 */
void CANativeLEDisconnect(JNIEnv *env, jobject bluetoothGatt);

/**
 * @brief   disconnect to gatt server by whole devices
 * @param   env              [IN] JNI interface pointer
 * @return  None
 */
void CANativeLEDisconnectAll(JNIEnv *env);

/**
 * @brief   start discovery server
 * @param   env              [IN] JNI interface pointer
 * @param   bluetoothGatt    [IN] Gatt profile object
 * @return  None
 */
void CANativeLEDiscoverServices(JNIEnv *env, jobject bluetoothGatt);

/**
 * @brief   request to write gatt characteristic
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @param   gattCharacteristic    [IN] characteristic object that contain data to send
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CANativeWriteCharacteristic(JNIEnv *env, jobject bluetoothGatt, jobject gattCharacteristic);

/**
 * @brief   request to read gatt characteristic
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @return  None
 */
void CANativeReadCharacteristic(JNIEnv *env, jobject bluetoothGatt);

/**
 * @brief   enable notification for a target device
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @param   uuid                  [IN] for set BluetoothGattCharacteristic object
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_ADAPTER_NOT_ENABLED Adapter is not enabled
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CANativeSetCharacteristicNotification(JNIEnv *env, jobject bluetoothGatt,
                                                 const char* uuid);

/**
 * @brief   create gatt characteristic object
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @param   data                  [IN] for make Characteristic with data
 * @return  Gatt Characteristic object
 */
jobject CANativeCreateGattCharacteristic(JNIEnv *env, jobject bluetoothGatt, jbyteArray data);

/**
 * @brief   get gatt service
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @param   characterUUID         [IN] for make BluetoothGattCharacteristic object
 * @return  Gatt Service
 */
jobject CANativeGetGattService(JNIEnv *env, jobject bluetoothGatt, jstring characterUUID);

/**
 * @brief   get value from characteristic
 * @param   env                   [IN] JNI interface pointer
 * @param   characteristic        [IN] Characteristic object
 * @return  value in characteristic
 */
jbyteArray CANativeGetValueFromCharacteristic(JNIEnv *env, jobject characteristic);

/**
 * @brief   create UUID List
 * @return  None
 */
void CANativeCreateUUIDList();

/**
 * BluetoothDevice List
 */
/**
 * @brief   create scan device list
 * @param   env                   [IN] JNI interface pointer
 * @return  None
 */
void CANativeCreateScanDeviceList(JNIEnv *env);

/**
 * @brief   add device object to scan device list
 * @param   env                   [IN] JNI interface pointer
 * @param   device                [IN] bluetooth device object
 * @return  None
 */
void CANativeAddScanDeviceToList(JNIEnv *env, jobject device);

/**
 * @brief   check whether the device exist in list or not
 * @param   env                   [IN] JNI interface pointer
 * @param   remoteAddress         [IN] remote address
 * @return  TRUE or FALSE
 */
jboolean CANativeIsDeviceInList(JNIEnv *env, const char *remoteAddress);

/**
 * @brief   remove all devices in scan device list
 * @param   env                   [IN] JNI interface pointer
 * @return  None
 */
void CANativeRemoveAllDevices(JNIEnv *env);

/**
 * @brief   remove target device in scan device list
 * @param   env                   [IN] JNI interface pointer
 * @param   remoteAddress         [IN] remote address
 * @return  None
 */
void CANativeRemoveDevice(JNIEnv *env, jstring remoteAddress);

/**
 * @brief   Reordering for scan device list
 * @param   index                 [IN] index of device list that want to reordering
 * @return  None
 */
void CAReorderingDeviceList(uint32_t index);

/**
 * BluetoothGatt List
 */
/**
 * @brief   create gatt object list
 * @param   env                   [IN] JNI interface pointer
 * @return  None
 */
void CANativeCreateGattObjList(JNIEnv *env);

/**
 * @brief   add gatt object to gatt object list
 * @param   env                   [IN] JNI interface pointer
 * @param   gatt                  [IN] Gatt profile object
 * @return  None
 */
void CANativeAddGattobjToList(JNIEnv *env, jobject gatt);

/**
 * @brief   check whether the gatt object exist in list or not
 * @param   env                   [IN] JNI interface pointer
 * @param   remoteAddress         [IN] remote address
 * @return  TRUE or FALSE
 */
jboolean CANativeIsGattObjInList(JNIEnv *env, const char *remoteAddress);

/**
 * @brief   remove all gatt objects in gatt object list
 * @param   env                   [IN] JNI interface pointer
 * @return  None
 */
void CANativeRemoveAllGattObjsList(JNIEnv *env);

/**
 * @brief   remove target device in gatt object list
 * @param   env                   [IN] JNI interface pointer
 * @param   gatt                  [IN] Gatt profile object
 * @return  None
 */
void CANativeRemoveGattObj(JNIEnv *env, jobject gatt);

/**
 * @brief   Reordering for gatt object list
 * @param   index                 [IN] index of device list that want to reordering
 * @return  None
 */
void CAReorderingGattList(uint32_t index);

/**
 * @brief   check whether target device which send data is exist or not
 * @param   env                   [IN] JNI interface pointer
 * @return  None
 */
void CANativeupdateSendCnt(JNIEnv *env);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

