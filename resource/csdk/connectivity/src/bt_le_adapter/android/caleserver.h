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
 * @file caleserver.h
 * @brief This file contains the APIs for BT LE communications.
 */
#ifndef CA_LESERVER_H_
#define CA_LESERVER_H_

#include "cacommon.h"
#include "cathreadpool.h"
#include "uarraylist.h"
#include "jni.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Callback to be notified on reception of any data from remote devices.
 * @param  address           [IN] MAC address of remote device.
 * @param  data              [IN] Data received from remote device.
 * @return None
 * @pre  Callback must be registered using CALEServerSetCallback(CAPacketReceiveCallback callback)
 */
typedef void (*CAPacketReceiveCallback)(const char *address, const char *data);

/**
 * @brief   initialize server for BLE
 * @param   handle           [IN] thread pool handle object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerInitialize(ca_thread_pool_t handle);

/**
 * @brief   terminate client for BLE
 * @return  None
 */
void CALEServerTerminate();

/**
 * @brief   send data for unicast (interface)
 * @param   address          [IN] remote address
 * @param   data             [IN] data for transmission
 * @param   dataLen          [IN] data length
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerSendUnicastMessage(const char *address, const char *data, uint32_t dataLen);

/**
 * @brief   send data for multicast (interface)
 * @param   data             [IN] data for transmission
 * @param   dataLen          [IN] data length
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerSendMulticastMessage(const char *data, uint32_t dataLen);

/**
 * @brief   start multicast server (start advertise)
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerStartMulticastServer();

/**
 * @brief   stop multicast server (stop discovery)
 * @return  None
 */
CAResult_t CALEServerStopMulticastServer();

/**
 * @brief   set this callback for receiving data packets from peer devices.
 * @param   callback         [IN] callback to be notified on reception of
 *                                unicast/multicast data packets.
 * @return  None
 */
void CALEServerSetCallback(CAPacketReceiveCallback callback);

/**
 * @brief   send data for unicast (implement)
 * @param   env              [IN] JNI interface pointer
 * @param   address          [IN] remote address
 * @param   data             [IN] data for transmission
 * @param   dataLen          [IN] data length
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerSendUnicastMessageImpl(JNIEnv *env, const char *address, const char *data,
                                            uint32_t dataLen);

/**
 * @brief   send data for multicast (implement)
 * @param   env              [IN] JNI interface pointer
 * @param   data             [IN] data for transmission
 * @param   dataLen          [IN] data length
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerSendMulticastMessageImpl(JNIEnv *env, const char *data, uint32_t dataLen);

/* Android BLE Server Functions */
/**
 * @brief   set context of application
 * @return  None
 */
void CALEServerJNISetContext();

/**
 * @brief   initialize JNI object
 * @return  None
 */
void CALeServerJniInit();

/**
 * @brief   create interface object and initialize the object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerCreateJniInterfaceObject();

/**
 * @brief   start advertise in gatt server
 * @param   env                [IN] JNI interface pointer
 * @param   advertiseCallback  [IN] callback to be notified on reception of
 *                                advertisement result
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerStartAdvertise(JNIEnv *env, jobject advertiseCallback);

/**
 * @brief   stop advertise in gatt server
 * @param   env                [IN] JNI interface pointer
 * @param   advertiseCallback  [IN] callback to be notified on reception of
 *                                advertisement result
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerStopAdvertise(JNIEnv *env, jobject advertiseCallback);

/**
 * @brief   open a gatt server
 * @param   env                [IN] JNI interface pointer
 * @return  gatt server object
 */
jobject CALEServerOpenGattServer(JNIEnv *env);

/**
 * @brief   create gatt service
 * @param   env                [IN] JNI interface pointer
 * @return  gatt service object
 */
jobject CALEServerCreateGattService(JNIEnv *env);

/**
 * @brief   add a descriptor to the characteristic
 * @param   env                [IN] JNI interface pointer
 * @param   characteristic     [IN] Characteristic object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerAddDescriptor(JNIEnv *env, jobject characteristic);

/**
 * @brief   create gatt service
 * @param   env                  [IN] JNI interface pointer
 * @param   bluetoothGattServer  [IN] Bluetooth Gatt Server object
 * @param   bluetoothGattService [IN] Bluetooth Gatt Service object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerAddGattService(JNIEnv *env, jobject bluetoothGattServer,
                                    jobject bluetoothGattService);

/**
 * @brief   start gatt server
 * @param   env                  [IN] JNI interface pointer
 * @param   gattServerCallback   [IN] callback to be notified on reception of
 *                                state change of gatt server
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerStartGattServer(JNIEnv *env, jobject gattServerCallback);

/**
 * @brief   send data
 * @param   env                  [IN] JNI interface pointer
 * @param   bluetoothDevice      [IN] bluetooth device object
 * @param   data                 [IN] data which send
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerSend(JNIEnv *env, jobject bluetoothDevice, jstring data);

/**
 * @brief   set data in BluetoothGattCharacteristic
 * @param   env                  [IN] JNI interface pointer
 * @param   responseData         [IN] data to set in characteristic object
 * @return  BluetoothGattCharacteristic object
 */
jobject CALEServerSetResponseData(JNIEnv *env, jbyteArray responseData);

/**
 * @brief   send data through notifyCharacteristicChanged api of android
 * @param   env                  [IN] JNI interface pointer
 * @param   device               [IN] bluetooth device object
 * @param   responseData         [IN] data which send
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerSendResponseData(JNIEnv *env, jobject device, jobject responseData);

/**
 * @brief   send a response to a write request to a remote device
 * @param   env                  [IN] JNI interface pointer
 * @param   device               [IN] bluetooth device object
 * @param   requestId            [IN] the id of request
 * @param   status               [IN] the status of the request to be sent to the remote devices
 * @param   offset               [IN] value offset for partial write response
 * @param   value                [IN] the value of the attribute that written (optional)
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerSendResponse(JNIEnv *env, jobject device, jint requestId, jint status,
                                  jint offset, jbyteArray value);

/**
 * @brief   connect BLE to remote device form gatt server
 * @param   env                  [IN] JNI interface pointer
 * @param   bluetoothDevice      [IN] bluetooth device object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerConnect(JNIEnv *env, jobject bluetoothDevice);

/**
 * @brief   disconnect LE for all devices
 * @param   env                  [IN] JNI interface pointer
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerDisconnectAllDevices(JNIEnv *env);

/**
 * @brief   disconnect LE to remote device form gatt server
 * @param   env                  [IN] JNI interface pointer
 * @param   bluetoothDevice      [IN] bluetooth device object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerDisconnect(JNIEnv *env, jobject bluetoothDevice);

/* BLE Server Utils */
/**
 * @brief   create connected device list
 * @return  None
 */
void CALEServerCreateCachedDeviceList();

/**
 * @brief   check whether the device exist in the list or not
 * @param   env                   [IN] JNI interface pointer
 * @param   remoteAddress         [IN] remote address
 * @return  true or false
 */
bool CALEServerIsDeviceInList(JNIEnv *env, const char* remoteAddress);

/**
 * @brief   add device object to the list (connected device list)
 * @param   env                   [IN] JNI interface pointer
 * @param   device                [IN] bluetooth device object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerAddDeviceToList(JNIEnv *env, jobject device);

/**
 * @brief   remove all devices objects in the list (connected device list)
 * @param   env                   [IN] JNI interface pointer
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerRemoveAllDevices(JNIEnv *env);

/**
 * @brief   remove target device in the list (connected device list)
 * @param   env                   [IN] JNI interface pointer
 * @param   address               [IN] target address to remove
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerRemoveDevice(JNIEnv *env, jstring address);

/**
 * @brief   Reordering for the list (connected device list)
 * @param   index                 [IN] index of device list that want to reordering
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerReorderinglist(uint32_t index);

/**
 * @brief   initialize mutex
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerInitMutexVaraibles();

/**
 * @brief   terminate mutex
 * @return  None
 */
void CALEServerTerminateMutexVaraibles();

/**
 * @brief   initialize condition
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEServerInitConditionVaraibles();

/**
 * @brief   terminate condition
 * @return  None
 */
void CALEServerTerminateConditionVaraibles();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_LESERVER_H_ */
