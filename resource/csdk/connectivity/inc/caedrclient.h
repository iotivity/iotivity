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
 * @file caedrcore.h
 * @brief This file contains the APIs for BT EDR communications.
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

/**
* @brief   Set context of application
* @param   context          [IN] context of application
* @return  None
*/
void CAEDRJniSetContext(jobject context);

/**
* @brief   Create JNI Object
* @param   context          [IN] context of application
* @return  None
*/
void CAEDRCreateJNIInterfaceObject(jobject context);

/**
 * @brief   Initialize client for EDR
 * @param   handle           [IN] thread pool handle object
 * @return  None
 */
void CAEDRInitialize(u_thread_pool_t handle);

/**
 * @brief   Terminate server for EDR
 * @return  None
 */
void CAEDRTerminate();

/**
 * @brief   Initialize JNI object
 * @param   env              [IN] JNI interface pointer
 * @param   jvm              [IN] java virtual machine pointer
 * @return  None
 */
void CAEDRCoreJniInit(JNIEnv *env, JavaVM *jvm);

/**
 * @brief   Send data for unicast
 * @param   address         [IN] remote address
 * @param   data            [IN] data for transmission
 * @param   dataLen         [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEDRSendUnicastMessage(const char *address,
                            const char *data, uint32_t dataLen);

/**
 * @brief   Send data for multicast
 * @param   data            [IN] data for transmission
 * @param   dataLen         [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEDRSendMulticastMessage(const char *data, uint32_t dataLen);

/**
 * @brief   Get Local EDR Address
 * @param   address         [OUT] local address
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEDRGetInterfaceInfo(char **address);

/**
 * @brief   Get address from a local device
 * @param   address         [OUT] local address
 * @return  None
 */
void CAEDRGetLocalAddress(char **address);

/**
 * @brief   Send data for unicast (implement)
 * @param   address         [IN] remote address
 * @param   data            [IN] data for transmission
 * @param   dataLen         [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEDRSendUnicastMessageImpl(const char *address,
                        const char *data, uint32_t dataLen);

/**
 * @brief   Send data for multicast (implement)
 * @param   env             [IN] JNI interface pointer
 * @param   data            [IN] data for transmission
 * @param   dataLen         [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEDRSendMulticastMessageImpl(JNIEnv *env, const char *data, uint32_t dataLen);

/**
 * EDR Method
 */

/**
 * @brief  This function will send the data to remote device.
 * @param  env              [IN] JNI interface pointer
 * @param  address          [IN] Remote Address
 * @param  data             [IN] Data to be transmitted from EDR
 * @param  id               [IN] index of remote address
 * @return None
 */
void CAEDRNativeSendData(JNIEnv *env, const char *address, const char *data, uint32_t id);

/**
 * @brief  This function will connect to remote device.
 * @param  env              [IN] JNI interface pointer
 * @param  address          [IN] Remote Address
 * @param  id               [IN] index of remote address
 * @return None
 */
void CAEDRNativeConnect(JNIEnv *env, const char *address, uint32_t id);

/**
 * @brief  This function will close socket.
 * @param  env              [IN] JNI interface pointer
 * @param  address          [IN] Remote Address
 * @param  id               [IN] index of remote address
 * @return None
 */
void CAEDRNativeSocketClose(JNIEnv *env, const char *address, uint32_t id);

/**
 * BT State List
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
 * @return TRUE or FALSE
 */
jboolean CAEDRNativeIsDeviceInList(const char *remoteAddress);

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
 * @brief  Get current device connection state
 * @param  remoteAddress    [IN] remote address
 * @return STATE_DISCONNECTED or STATE_CONNECTED
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
 * @return TRUE or FALSE
 */
jboolean CAEDRNativeIsDeviceSocketInList(JNIEnv *env, const char *remoteAddress);

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
 * @brief  Get device socket object from the list
 * @param  idx              [IN] index of device list
 * @return Device socket object or NULL
 */
jobject CAEDRNativeGetDeviceSocket(uint32_t idx);

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

#endif

