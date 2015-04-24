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
 * This file contains the APIs for BT EDR communications.
 */

#ifndef __CA_EDRCORE_H_
#define __CA_EDRCORE_H_

#include <stdbool.h>

#include "cacommon.h"
#include "uthreadpool.h"
#include "jni.h"


#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*CAPacketReceiveCallback)(const char *address, const char *data);


/**
 * @brief   Initialize JNI object
 * @param   env              [IN] JNI interface pointer
 * @param   jvm              [IN] java virtual machine pointer
 * @return  None
 */
void CAEDRServerJniInit(JNIEnv *env, JavaVM *jvm);

/**
 * @brief  Initialize server for EDR
 * @param   handle           [IN] thread pool handle object
 * @return  None
 */
void CAEDRServerInitialize(u_thread_pool_t handle);

/**
 * @brief   Start unicast server
 * @param   address         [IN] remote address
 * @param   isSecured       [IN] unicast server type
 * @return  Returns -1 on error and 0 on success
 */
int32_t CAEDRStartUnicastServer(const char *address, bool isSecured);

/**
 * @brief   Start multicast server
 * @param   isSecured       [IN] multicst server type
 * @return  Returns -1 on error and 0 on success
 */
int32_t CAEDRStartMulticastServer(bool isSecured);

/**
 * @brief   Stop unicast server
 * @param   serverID        [IN] unicast server id
 * @return  Returns -1 on error and 0 on success
 */
int32_t CAEDRStopUnicastServer(int32_t serverID);

/**
 * @brief   Stop multicast server
 * @param   serverID        [IN] multicast server id
 * @return  Returns -1 on error and 0 on success
 */
int32_t CAEDRStopMulticastServer(int32_t serverID);

/**
 * EDR Method
 */

/**
 * @brief  This function will read the data from remote device.
 * @param  env              [IN] JNI interface pointer
 * @param  id               [IN] index of remote address
 * @param  type             [IN] EDR server type
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRNativeReadData(JNIEnv *env, uint32_t id, CAAdapterServerType_t type);

/**
 * @brief  This function will listen the connection from remote device.
 * @param  env              [IN] JNI interface pointer
 * @return server socket object or NULL
 */
jobject CAEDRNativeListen(JNIEnv *env);

/**
 * @brief  This function will accept the connection from remote device.
 * @param  env                  [IN] JNI interface pointer
 * @param  severSocketObject    [IN] server socket object
 * @return None
 */
void CAEDRNativeAccept(JNIEnv *env, jobject severSocketObject);

/**
 * @brief  This function will accept the connection from remote device.
 * @param  env              [IN] JNI interface pointer
 * @param  address          [IN] remote address
 * @param  id               [IN] index of remote address
 * @return None
 */
void CAEDRNativeServerSocketClose(JNIEnv *env, const char *address, uint32_t id);

/**
 * BT State List
 */

/**
 * @brief  This function will create the device state list.
 * @return None
 */
void CAEDRServerNativeCreateDeviceStateList();

/**
 * @brief  Update connection state of device
 * @param  state            [IN] connection state
 * @param  address          [IN] remote address
 * @return None
 */
void CAEDRServerUpdateDeviceState(uint32_t state, const char *address);

/**
 * @brief  Add device object to the list
 * @param  state            [IN] connection state object
 * @return None
 */
void CAEDRServerNativeAddDeviceStateToList(state_t *state);

/**
 * @brief  Check whether the device exist in the list or not
 * @param  remoteAddress    [IN] remote address
 * @return TRUE or FALSE
 */
jboolean CAEDRServerNativeIsDeviceInList(const char *remoteAddress);

/**
 * @brief   Remove all device objects in the list
 * @return  None
 */
void CAEDRServerNativeRemoveAllDeviceState();

/**
 * @brief   Remove target device in the list
 * @param   remoteAddress    [IN] remote address
 * @return  None
 */
void CAEDRServerNativeRemoveDevice(const char *remoteAddress);

/**
 * @brief  Check whether the state is connected or disconnected
 * @param  remoteAddress    [IN] remote address
 * @return TRUE or FALSE
 */
jboolean CAEDRServerIsConnectedDevice(const char *remoteAddress);

/**
 * @brief   Reordering for the list
 * @param   index           [IN] index of device list that want to reordering
 * @return  None
 */
void CAEDRServerReorderingDeviceList(uint32_t index);

/**
 * Bluetooth Socket Object List
 */

/**
 * @brief  This function will create the device socket list.
 * @return None
 */
void CAEDRServerNativeCreateDeviceSocketList();

/**
 * @brief  Add device object to the list
 * @param  env              [IN] JNI interface pointer
 * @param  deviceSocket     [IN] device socket object
 * @return None
 */
void CAEDRServerNativeAddDeviceSocketToList(JNIEnv *env, jobject deviceSocket);

/**
 * @brief  Add device object to the list
 * @param  env              [IN] JNI interface pointer
 * @param  remoteAddress    [IN] remote address
 * @return TRUE or FALSE
 */
jboolean CAEDRServerNativeIsDeviceSocketInList(JNIEnv *env, const char *remoteAddress);

/**
 * @brief  Add device object to the list
 * @param  env              [IN] JNI interface pointer
 * @return None
 */
void CAEDRServerNativeRemoveAllDeviceSocket(JNIEnv *env);

/**
 * @brief  Add device object to the list
 * @param  env              [IN] JNI interface pointer
 * @param  deviceSocket     [IN] device socket object
 * @return None
 */
void CAEDRServerNativeRemoveDeviceSocket(JNIEnv *env, jobject deviceSocket);

/**
 * @brief  Get device socket object from the list
 * @param  idx              [IN] index of device list
 * @return Device socket object or NULL
 */
jobject CAEDRServerNativeGetDeviceSocket(uint32_t idx);

/**
 * @brief  Get length of device socket list
 * @return length of list
 */
uint32_t CAEDRServerGetSocketListLength();

/**
 * @brief   Reordering for the list
 * @param   index           [IN] index of device list that want to reordering
 * @return  None
 */
void CAEDRServerReorderingDeviceSocketList(uint32_t index);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

