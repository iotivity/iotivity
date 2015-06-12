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
 * @file
 * @brief This file contains the APIs for BT communications.
 */
#ifndef CA_EDR_UTILS_H_
#define CA_EDR_UTILS_H_

#include "cacommon.h"
#include "cathreadpool.h"
#include "uarraylist.h"
#include "caedrinterface.h"
#include "jni.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Get address from device socket
 * @param   env                 [IN] JNI interface pointer
 * @param   bluetoothSocketObj  [IN] bluetooth socket object
 * @return  Bluetooth device address
 */
jstring CAEDRNativeGetAddressFromDeviceSocket(JNIEnv *env, jobject bluetoothSocketObj);

/**
 * @brief   Get local device address
 * @param   env                 [IN] JNI interface pointer
 * @return  Bluetooth device address
 */
jstring CAEDRNativeGetLocalDeviceAddress(JNIEnv *env);

/**
 * @brief   Get bonded devices list
 * @param   env                 [IN] JNI interface pointer
 * @return  Bonded devices list
 */
jobjectArray CAEDRNativeGetBondedDevices(JNIEnv *env);

/**
 * @brief   Get Bluetooth device state
 * @param   env                 [IN] JNI interface pointer
 * @return  Bluetooth device state
 */
jint CAEDRNativeGetBTStateOnInfo(JNIEnv *env);

/**
 * @brief   Check the BT Adapter enable
 * @param   env                 [IN] JNI interface pointer
 * @return  JNI_TRUE or JNI_FALSE
 */
jboolean CAEDRNativeIsEnableBTAdapter(JNIEnv *env);

/**
 * @brief   Get address from BT device
 * @param   env                 [IN] JNI interface pointer
 * @param   bluetoothSocketObj  [IN] bluetooth socket object
 * @return  Bluetooth device address
 */
jstring CAEDRNativeGetAddressFromBTDevice(JNIEnv *env, jobject bluetoothDevice);

/**
 * BT State Info List
 */

/**
 * @brief  This function will create the device state list.
 * @return None
 */
void CAEDRNativeCreateDeviceStateList();

/**
 * @brief  Update connection state of device
 * @param  state            [IN] connection state
 * @param  address          [IN] remote address
 * @return None
 */
void CAEDRUpdateDeviceState(CAConnectedState_t state, const char *address);

/**
 * @brief  Add device object to the list
 * @param  state            [IN] connection state object
 * @return None
 */
void CAEDRNativeAddDeviceStateToList(state_t *state);

/**
 * @brief  Check whether the device exist in the list or not
 * @param  remoteAddress    [IN] remote address
 * @return true or false
 */
bool CAEDRNativeIsDeviceInList(const char *remoteAddress);

/**
 * @brief   Close all socket
 * @param   env       [IN] JNI interface pointer
 * @return  NONE
 */
void CAEDRNativeSocketCloseToAll(JNIEnv *env);

/**
 * @brief   Remove all device objects in the list
 * @return  None
 */
void CAEDRNativeRemoveAllDeviceState();

/**
 * @brief   Remove target device in the list
 * @param   remoteAddress    [IN] remote address
 * @return  None
 */
void CAEDRNativeRemoveDevice(const char *remoteAddress);

/**
 * @brief   Get current device connection state
 * @param   remoteAddress    [IN] remote address
 * @return  STATE_DISCONNECTED or STATE_CONNECTED
 */
CAConnectedState_t CAEDRIsConnectedDevice(const char *remoteAddress);

/**
 * @brief   Reordering for the list
 * @param   index           [IN] index of device list that want to reordering
 * @return  None
 */
void CAEDRReorderingDeviceList(uint32_t index);

/**
 * Bluetooth Socket Object List
 */

/**
 * @brief  This function will create the device socket list.
 * @return None
 */
void CAEDRNativeCreateDeviceSocketList();

/**
 * @brief  Add device object to the list
 * @param  env              [IN] JNI interface pointer
 * @param  deviceSocket     [IN] device socket object
 * @return None
 */
void CAEDRNativeAddDeviceSocketToList(JNIEnv *env, jobject deviceSocket);

/**
 * @brief  Add device object to the list
 * @param  env              [IN] JNI interface pointer
 * @param  remoteAddress    [IN] remote address
 * @return true or false
 */
bool CAEDRNativeIsDeviceSocketInList(JNIEnv *env, const char *remoteAddress);

/**
 * @brief  Add device object to the list
 * @param  env              [IN] JNI interface pointer
 * @return None
 */
void CAEDRNativeRemoveAllDeviceSocket(JNIEnv *env);

/**
 * @brief  Add device object to the list
 * @param  env              [IN] JNI interface pointer
 * @param  deviceSocket     [IN] device socket object
 * @return None
 */
void CAEDRNativeRemoveDeviceSocket(JNIEnv *env, jobject deviceSocket);

/**
 * @brief   Remove device socket
 * @param   env             [IN] JNI interface pointer
 * @param   address         [IN] remote address
 * @return  NONE
 */
void CAEDRNativeRemoveDeviceSocketBaseAddr(JNIEnv *env, jstring address);

/**
 * @brief  Get device socket object from the list
 * @param  idx              [IN] index of device list
 * @return Device socket object or NULL
 */
jobject CAEDRNativeGetDeviceSocket(uint32_t idx);

/**
 * @brief   Get device socket address
 * @param   env             [IN] JNI interface pointer
 * @param   remoteAddress   [IN] remote address
 * @return  Device socket object or NULL
 */
jobject CAEDRNativeGetDeviceSocketBaseAddr(JNIEnv *env, const char* remoteAddress);

/**
 * @brief  Get length of device socket list
 * @return length of list
 */
uint32_t CAEDRGetSocketListLength();

/**
 * @brief   Reordering for the list
 * @param   index           [IN] index of device list that want to reordering
 * @return  None
 */
void CAEDRReorderingDeviceSocketList(uint32_t index);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_EDR_UTILS_H_ */

