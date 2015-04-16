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

#ifndef __CA_LESERVER_H_
#define __CA_LESERVER_H_

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
 * @param  address          [IN] MAC address of remote device.
 * @param  data             [IN] Data received from remote device.
 * @return None
 * @pre  Callback must be registered using CALEServerSetCallback(CAPacketReceiveCallback callback)
 */
typedef void (*CAPacketReceiveCallback)(const char *address,
        const char *data);

/**
 * @brief   initialize server for BLE
 * @param   handle           [IN] thread pool handle object
 * @return  None
 */
void CALEServerInitialize(ca_thread_pool_t handle);

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
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEServerSendUnicastMessage(const char *address, const char *data,
        uint32_t dataLen);

/**
 * @brief   send data for multicast (interface)
 * @param   data             [IN] data for transmission
 * @param   dataLen          [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEServerSendMulticastMessage(const char *data, uint32_t dataLen);

/**
 * @brief   start unicast server
 * @param   address          [IN] remote address
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEServerStartUnicastServer(const char *address);

/**
 * @brief   start multicast server (start advertise)
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEServerStartMulticastServer();

/**
 * @brief   stop unicast server
 * @return  None
 */
CAResult_t CALEServerStopUnicastServer();

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
 * @brief  Get local adapter network information.
 * @param  info              [OUT] local connectivity information
 * @param  size              [OUT] connectivity count
 * @return  None
 */
void CALEServerGetInterfaceInfo(CALocalConnectivity_t **info,
        uint32_t *size);

/**
 * @brief   get local address
 * @param   address          [OUT] local address
 * @return  None
 */
void CALEServerGetLocalAddress(char *address);

/**
 * @brief   send data for unicast (implement)
 * @param   env              [IN] JNI interface pointer
 * @param   address          [IN] remote address
 * @param   data             [IN] data for transmission
 * @param   dataLen          [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEServerSendUnicastMessageImpl(JNIEnv *env, const char *address,
        const char *data, uint32_t dataLen);

/**
 * @brief   send data for multicast (implement)
 * @param   env              [IN] JNI interface pointer
 * @param   address          [IN] remote address
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEServerSendMulticastMessageImpl(JNIEnv *env, const char *data,
        uint32_t dataLen);

/* Android BLE Server Functions */
/**
 * @brief   set context of application
 * @param   env              [IN] JNI interface pointer
 * @param   context          [IN] context of application
 * @return  None
 */
void CALEServerJNISetContext(JNIEnv *env, jobject context);

/**
 * @brief   initialize JNI object
 * @param   env              [IN] JNI interface pointer
 * @param   jvm              [IN] java virtual machine pointer
 * @return  None
 */
void CALeServerJniInit(JNIEnv *env, JavaVM* jvm);

/**
 * @brief   create interface object and initialize the object
 * @return  None
 */
void CALeServerCreateJniInterfaceObject();

/**
 * @brief   start advertise in gatt server
 * @param   env                [IN] JNI interface pointer
 * @param   advertiseCallback  [IN] callback to be notified on reception of
 *                                advertisement result
 * @return  None
 */
void CANativeLEServerStartAdvertise(JNIEnv *env, jobject advertiseCallback);

/**
 * @brief   stop advertise in gatt server
 * @param   env                [IN] JNI interface pointer
 * @param   advertiseCallback  [IN] callback to be notified on reception of
 *                                advertisement result
 * @return  None
 */
void CANativeLEServerStopAdvertise(JNIEnv *env, jobject advertiseCallback);

/**
 * @brief   open a gatt server
 * @param   env                [IN] JNI interface pointer
 * @return  gatt server object
 */
jobject CANativeLEServerOpenGattServer(JNIEnv *env);

/**
 * @brief   create gatt service
 * @param   env                [IN] JNI interface pointer
 * @return  gatt service object
 */
jobject CANativeLEServerCreateGattService(JNIEnv *env);

/**
 * @brief   create gatt service
 * @param   env                [IN] JNI interface pointer
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_ADAPTER_NOT_ENABLED Adapter is not enabled
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CANativeLEServerAddGattService(JNIEnv *env, jobject bluetoothGattServer,
        jobject bluetoothGattService);

/**
 * @brief   start gatt server
 * @param   env                  [IN] JNI interface pointer
 * @param   gattServerCallback   [IN] callback to be notified on reception of
 *                                state change of gatt server
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_ADAPTER_NOT_ENABLED Adapter is not enabled
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEStartGattServer(JNIEnv *env, jobject gattServerCallback);

/**
 * @brief   send data
 * @param   env                  [IN] JNI interface pointer
 * @param   bluetoothDevice      [IN] bluetooth device object
 * @param   data                 [IN] data which send
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_SEND_FAILED  Send request failed
 * @retval  #CA_ADAPTER_NOT_ENABLED Adapter is not enabled
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CALEServerSend(JNIEnv *env, jobject bluetoothDevice, jstring data);

/**
 * @brief   set data in BluetoothGattCharacteristic
 * @param   env                  [IN] JNI interface pointer
 * @param   bluetoothDevice      [IN] bluetooth device object
 * @return  BluetoothGattCharacteristic object
 */
jobject CANativeLEServerSetResponseData(JNIEnv *env, jbyteArray responseData);

/**
 * @brief   send data through notifyCharacteristicChanged api of android
 * @param   env                  [IN] JNI interface pointer
 * @param   bluetoothDevice      [IN] bluetooth device object
 * @param   data                 [IN] data which send
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_SEND_FAILED  Send request failed
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CANativeLEServerSendResponseData(JNIEnv *env, jobject device, jobject responseData);

/**
 * @brief   send a response to a write request to a remote device
 * @param   env                  [IN] JNI interface pointer
 * @param   device               [IN] bluetooth device object
 * @param   requestId            [IN] the id of request
 * @param   status               [IN] the status of the request to be sent to the remote devices
 * @param   offset               [IN] value offset for partial write response
 * @param   value                [IN] the value of the attribute that written (optional)
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_SEND_FAILED  Send request failed
 * @retval  #CA_ADAPTER_NOT_ENABLED Adapter is not enabled
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CANativeLEServerSendResponse(JNIEnv *env, jobject device, jint requestId, jint status,
        jint offset, jbyteArray value);

/**
 * @brief   connect BLE to remote device form gatt server
 * @param   env                  [IN] JNI interface pointer
 * @param   bluetoothDevice      [IN] bluetooth device object
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_ADAPTER_NOT_ENABLED Adapter is not enabled
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CANativeLEServerConnect(JNIEnv *env, jobject bluetoothDevice);

/**
 * @brief   disconnect BLE to remote device form gatt server
 * @param   env                  [IN] JNI interface pointer
 * @param   bluetoothDevice      [IN] bluetooth device object
 * @return  None
 */
void CANativeLEServerDisconnect(JNIEnv *env, jobject bluetoothDevice);

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
 * @return  TRUE or FALSE
 */
jboolean CALEServerIsDeviceInList(JNIEnv *env, const char* remoteAddress);

/**
 * @brief   add device object to the list (connected device list)
 * @param   env                   [IN] JNI interface pointer
 * @param   device                [IN] bluetooth device object
 * @return  None
 */
void CALEServerAddDeviceToList(JNIEnv *env, jobject device);

/**
 * @brief   remove all devices objects in the list (connected device list)
 * @param   env                   [IN] JNI interface pointer
 * @return  None
 */
void CALEServerRemoveAllDevices(JNIEnv *env);

/**
 * @brief   remove target device in the list (connected device list)
 * @param   env                   [IN] JNI interface pointer
 * @param   gatt                  [IN] Gatt profile object
 * @return  None
 */
void CALEServerRemoveDevice(JNIEnv *env, jstring address);

/**
 * @brief   Reordering for the list (connected device list)
 * @param   index                 [IN] index of device list that want to reordering
 * @return  None
 */
void CALEServerReorderinglist(uint32_t index);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __CA_LESERVER_H_ */

