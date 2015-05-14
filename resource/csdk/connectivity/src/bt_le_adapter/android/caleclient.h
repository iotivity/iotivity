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
 * @file caleclient.h
 * @brief This file contains the APIs for BT LE communications.
 */
#ifndef CA_LECLIENT_H_
#define CA_LECLIENT_H_

#include "cacommon.h"
#include "cathreadpool.h"
#include "uarraylist.h"
#include "jni.h"

#ifdef __cplusplus
extern "C"
{
#endif

static const uint16_t STATE_CHARACTER_SET = 2;
static const uint16_t STATE_CHARACTER_UNSET = 1;
static const uint16_t STATE_CHARACTER_NO_CHANGE = 0;

static const uint16_t STATE_SEND_NONE = 0;
static const uint16_t STATE_SEND_SUCCESS = 1;
static const uint16_t STATE_SEND_FAILED = 2;

static const uint32_t STATE_CONNECTED = 2;
static const uint32_t STATE_DISCONNECTED = 0;

typedef struct le_state_info
{
    char address[CA_MACADDR_SIZE];
    uint32_t connectedState;
    uint16_t notificationState;
    uint16_t sendState;
} CALEState_t;

/**
 * @brief Callback to be notified on reception of any data from remote devices.
 * @param  address                [IN] MAC address of remote device.
 * @param  data                   [IN] Data received from remote device.
 * @return None
 * @pre  Callback must be registered using CALESetCallback(CAPacketReceiveCallback callback)
 */
typedef void (*CAPacketReceiveCallback)(const char *address, const char *data);

/**
 * @brief   initialize JNI object
 * @return  None
 */
void CALEClientJniInit();

/**
 * @brief   set context of application
 * @return  None
 */
void CALEClientJNISetContext();

/**
 * @brief   create interface object and initialize the object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientCreateJniInterfaceObject();

/**
 * @brief   initialize client for BLE
 * @param   handle                [IN] thread pool handle object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientInitialize(ca_thread_pool_t handle);

/**
 * @brief   terminate client for BLE
 * @return  None
 */
void CALEClientTerminate();

/**
 * @brief   for destroy sending routine
 * @param   env                   [IN] JNI interface pointer
 * @param   gatt                  [IN] Gatt profile object
 * @return  None
 */
void CALEClientSendFinish(JNIEnv *env, jobject gatt);

/**
 * @brief   send data for unicast (interface)
 * @param   address               [IN] remote address
 * @param   data                  [IN] data for transmission
 * @param   dataLen               [IN] data length
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientSendUnicastMessage(const char *address, const char *data,
                                        const uint32_t dataLen);

/**
 * @brief   send data for multicast (interface)
 * @param   data                  [IN] data for transmission
 * @param   dataLen               [IN] data length
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientSendMulticastMessage(const char *data, const uint32_t dataLen);

/**
 * @brief   start unicast server
 * @param   address               [IN] remote address
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientStartUnicastServer(const char *address);

/**
 * @brief   start multicast server (start discovery)
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientStartMulticastServer();

/**
 * @brief   stop unicast server
 * @return  None
 */
void CALEClientStopUnicastServer();

/**
 * @brief   stop multicast server (stop discovery)
 * @return  None
 */
void CALEClientStopMulticastServer();

/**
 * @brief   set this callback for receiving data packets from peer devices.
 * @param   callback              [IN] callback to be notified on reception of
 *                                unicast/multicast data packets.
 * @return  None
 */
void CALEClientSetCallback(CAPacketReceiveCallback callback);

/**
 * @brief   get local address (interface)
 * @param   address               [OUT] local address
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientGetInterfaceInfo(char **address);

/**
 * @brief   get local address (implement)
 * @param   address               [OUT] local address
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientGetLocalAddress(char** address);

/**
 * @brief   send data for unicast (implement)
 * @param   address               [IN] remote address
 * @param   data                  [IN] data for transmission
 * @param   dataLen               [IN] data length
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientSendUnicastMessageImpl(const char *address, const char *data,
                                            const uint32_t dataLen);

/**
 * @brief   send data for multicast (implement)
 * @param   env                   [IN] JNI interface pointer
 * @param   data                  [IN] data for transmission
 * @param   dataLen               [IN] data length
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientSendMulticastMessageImpl(JNIEnv *env, const char *data,
                                              const uint32_t dataLen);

/**
 * @brief   check whether it is connected or not with remote address.
 * @param   address               [IN] remote address
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALECheckSendState(const char* address);

/**
 * @brief   send data to remote device.
 *          if it isn't connected yet. connect LE before try to send data
 * @param   env                   [IN] JNI interface pointer
 * @param   device                [IN] bluetooth device object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientSendData(JNIEnv *env, jobject device);

/**
 * @brief   get address from bluetooth gatt object
 * @param   env                   [IN] JNI interface pointer
 * @param   gatt                  [IN] Gatt profile object
 * @return  bluetooth address
 */
jstring CALEClientGetAddressFromGattObj(JNIEnv *env, jobject gatt);

/**
 * @brief   get remote address from bluetooth socket object
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothSocketObj    [IN] bluetooth socket
 * @return  bluetooth address
 */
jstring CALEClientGetRemoteAddress(JNIEnv *env, jobject bluetoothSocketObj);

/**
 * @brief   close gatt
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] gatt profile object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientGattClose(JNIEnv *env, jobject bluetoothGatt);

/**
 * @brief   start to scan whole bluetooth devices (interface)
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientStartScan();

/**
 * @brief   start to scan whole bluetooth devices (implement)
 * @param   env                   [IN] JNI interface pointer
 * @param   callback              [IN] callback to receive device object by scanning
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientStartScanImpl(JNIEnv *env, jobject callback);

/**
 * @brief   start to scan target bluetooth devices for service uuid (implement)
 * @param   env                   [IN] JNI interface pointer
 * @param   uuids                 [IN] target UUID
 * @param   callback              [IN] callback to receive device object by scanning
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientStartScanWithUUIDImpl(JNIEnv *env, jobjectArray uuids,
                                           jobject callback);

/**
 * @brief   get uuid object
 * @param   env                   [IN] JNI interface pointer
 * @param   uuid                  [IN] uuid
 * @return  uuid object
 */
jobject CALEClientGetUUIDObject(JNIEnv *env, const char *uuid);

/**
 * @brief   stop scan (interface)
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientStopScan();

/**
 * @brief   stop scan (implement)
 * @param   env                   [IN] JNI interface pointer
 * @param   callback              [IN] callback to receive device object by scanning
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientStopScanImpl(JNIEnv *env, jobject callback);

/**
 * @brief   connect to gatt server hosted
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothDevice       [IN] bluetooth Device object
 * @param   autoconnect           [IN] whether to directly connect to the remote device(false) or
 *                                     to automatically connect as soon as the remote device
 *                                     becomes available
 * @param   callback              [IN] callback for connection state change
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientConnect(JNIEnv *env, jobject bluetoothDevice, jboolean autoconnect,
                             jobject callback);

/**
 * @brief   disconnect to gatt server by a target device
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientDisconnect(JNIEnv *env, jobject bluetoothGatt);

/**
 * @brief   disconnect to gatt server by whole devices
 * @param   env                   [IN] JNI interface pointer
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientDisconnectAll(JNIEnv *env);

/**
 * @brief   start discovery server
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientDiscoverServices(JNIEnv *env, jobject bluetoothGatt);

/**
 * @brief   create GattCharacteristic and call CALEClientWriteCharacteristicImpl
 *          for request to write gatt characteristic
 * @param   env                   [IN] JNI interface pointer
 * @param   gatt                  [IN] Gatt profile object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientWriteCharacteristic(JNIEnv *env, jobject gatt);

/**
 * @brief   request to write gatt characteristic
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @param   gattCharacteristic    [IN] characteristic object that contain data to send
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientWriteCharacteristicImpl(JNIEnv *env, jobject bluetoothGatt,
                                             jobject gattCharacteristic);

/**
 * @brief   request to read gatt characteristic
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientReadCharacteristic(JNIEnv *env, jobject bluetoothGatt);

/**
 * @brief   enable notification for a target device
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @param   characteristic        [IN] Characteristic object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientSetCharacteristicNotification(JNIEnv *env, jobject bluetoothGatt,
                                                  jobject characteristic);

/**
 * @brief   create gatt characteristic object
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @param   data                  [IN] for make Characteristic with data
 * @return  Gatt Characteristic object
 */
jobject CALEClientCreateGattCharacteristic(JNIEnv *env, jobject bluetoothGatt, jbyteArray data);

/**
 * @brief   get gatt service
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @param   characterUUID         [IN] for make BluetoothGattCharacteristic object
 * @return  Gatt Service
 */
jobject CALEClientGetGattService(JNIEnv *env, jobject bluetoothGatt, jstring characterUUID);

/**
 * @brief   get value from characteristic
 * @param   env                   [IN] JNI interface pointer
 * @param   characteristic        [IN] Characteristic object
 * @return  value in characteristic
 */
jbyteArray CALEClientGetValueFromCharacteristic(JNIEnv *env, jobject characteristic);

/**
 * @brief   create UUID List
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientCreateUUIDList();

/**
 * @brief   set UUID to descriptor
 * @param   env                   [IN] JNI interface pointer
 * @param   bluetoothGatt         [IN] Gatt profile object
 * @param   characteristic        [IN] Characteristic object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientSetUUIDToDescriptor(JNIEnv *env, jobject bluetoothGatt,
                                         jobject characteristic);

/**
 * BluetoothDevice List
 */
/**
 * @brief   add device object to scan device list
 * @param   env                   [IN] JNI interface pointer
 * @param   device                [IN] bluetooth device object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientAddScanDeviceToList(JNIEnv *env, jobject device);

/**
 * @brief   check whether the device exist in list or not
 * @param   env                   [IN] JNI interface pointer
 * @param   remoteAddress         [IN] remote address
 * @return  true or false
 */
bool CALEClientIsDeviceInScanDeviceList(JNIEnv *env, const char *remoteAddress);

/**
 * @brief   remove all devices in scan device list
 * @param   env                   [IN] JNI interface pointer
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientRemoveAllScanDevices(JNIEnv *env);

/**
 * @brief   remove target device in scan device list
 * @param   env                   [IN] JNI interface pointer
 * @param   remoteAddress         [IN] remote address
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientRemoveDeviceInScanDeviceList(JNIEnv *env, jstring remoteAddress);

/**
 * BluetoothGatt List
 */

/**
 * @brief   add gatt object to gatt object list
 * @param   env                   [IN] JNI interface pointer
 * @param   gatt                  [IN] Gatt profile object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientAddGattobjToList(JNIEnv *env, jobject gatt);

/**
 * @brief   check whether the gatt object exist in list or not
 * @param   env                   [IN] JNI interface pointer
 * @param   remoteAddress         [IN] remote address
 * @return  true or false
 */
bool CALEClientIsGattObjInList(JNIEnv *env, const char *remoteAddress);

/**
 * @brief   get the gatt object
 * @param   env                   [IN] JNI interface pointer
 * @param   remoteAddress         [IN] remote address
 * @return  gatt object
 */
jobject CALEClientGetGattObjInList(JNIEnv *env, const char* remoteAddress);

/**
 * @brief   remove all gatt objects in gatt object list
 * @param   env                   [IN] JNI interface pointer
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientRemoveAllGattObjs(JNIEnv *env);

/**
 * @brief   remove target device in gatt object list
 * @param   env                   [IN] JNI interface pointer
 * @param   gatt                  [IN] Gatt profile object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientRemoveGattObj(JNIEnv *env, jobject gatt);

/**
 * @brief   remove gatt object of target device for address in gatt object list
 * @param   env                   [IN] JNI interface pointer
 * @param   gatt                  [IN] Gatt profile object
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientRemoveGattObjForAddr(JNIEnv *env, jstring addr);

/**
 * BT State Info List
 */

/**
 * @brief   update new state information
 * @param   address               [IN] remote address
 * @param   connectedState        [IN] connection state
 * @param   notificationState     [IN] whether characteristic notification already set or not
 * @param   sendState             [IN] whether sending was success or not
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientUpdateDeviceState(const char* address, uint32_t connectedState,
                                       uint16_t notificationState, uint16_t sendState);

/**
 * @brief   add new state to state list
 * @param   state                 [IN] new state
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientAddDeviceStateToList(CALEState_t* state);

/**
 * @brief   check whether the remote address is existed or not.
 * @param   address               [IN] remote address
 * @return  true or false
 */
bool CALEClientIsDeviceInList(const char *remoteAddress);

/**
 * @brief   remove all device states
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientRemoveAllDeviceState();

/**
 * @brief   remove the device state for a remote device
 * @param   remoteAddress         [IN] remote address
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientRemoveDeviceState(const char* remoteAddress);

/**
 * @brief   get state information for a remote device
 * @param   remoteAddress         [IN] remote address
 * @return  CALEState_t
 */
CALEState_t* CALEClientGetStateInfo(const char* remoteAddress);

/**
 * @brief   check whether the remote address is connected or not.
 * @param   remoteAddress         [IN] remote address
 * @return  true or false
 */
bool CALEClientIsConnectedDevice(const char* remoteAddress);

/**
 * @brief   check whether the remote address set CharacteristicNotification or not
 * @param   remoteAddress         [IN] remote address
 * @return  true or false
 */
bool CALEClientIsSetCharacteristic(const char* remoteAddress);

/**
 * @brief   create scan device list
 * @return  None
 */
void CALEClientCreateDeviceList();

/**
 * @brief   Reordering for device state list
 * @param   index                 [IN] index of device list that want to reordering
 * @param   list                  [IN] the list to reorder
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientReorderingList(uint32_t index, u_arraylist_t *list);

/**
 * @brief   update the counter which data is sent to remote device
 * @param   env                   [IN] JNI interface pointer
 * @return  None
 */
void CALEClientUpdateSendCnt(JNIEnv *env);

/**
 * @brief   initialize mutex
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CALEClientInitGattMutexVaraibles();

/**
 * @brief   terminate mutex
 * @return  None
 */
void CALEClientTerminateGattMutexVariables();

/**
 * @brief   set send finish flag
 * @param   flag                  [IN] flag
 * @return  None
 */
void CALEClientSetSendFinishFlag(bool flag);

/**
 * @brief   set the flag whether WriteCharacteristicCB is called
 * @param   flag                  [IN] flag
 * @return  None
 */
void CALEClientSetWriteCharacteristicCBFlag(bool flag);

/**
 * @brief   set the flag whether Send Request is called
 * @param   flag                  [IN] flag
 * @return  None
 */
void CALEClientSetTheSendRequestFlag(bool flag);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_LECLIENT_H_ */
