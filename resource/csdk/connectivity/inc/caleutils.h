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

#ifndef __CA_LEUTILES_H_
#define __CA_LEUTILES_H_

#include "cacommon.h"
#include "uthreadpool.h"
#include "uarraylist.h"
#include "jni.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   get uuid(jni object) from uuid(character)
 * @param   env              [IN] JNI interface pointer
 * @param   uuid             [IN] uuid(character)
 * @return  uuid(jni object)
 */
jobject CALEGetUuidFromString(JNIEnv *env, const char* uuid);

/**
 * @brief   get parcel uuid object
 * @param   env              [IN] JNI interface pointer
 * @param   uuid             [IN] uuid (jni object)
 * @return  parcel uuid object
 */
jobject CALEGetParcelUuid(JNIEnv *env, jobject uuid);

/**
 * @brief   get address from a local device
 * @param   env              [IN] JNI interface pointer
 * @return  local address
 */
jstring CALEGetLocalDeviceAddress(JNIEnv *env);

/**
 * @brief   get bonded list
 * @param   env              [IN] JNI interface pointer
 * @return  bonded list
 */
jobjectArray CALEGetBondedDevices(JNIEnv *env);

/**
 * @brief   get constants information of bluetooth state-on
 * @param   env              [IN] JNI interface pointer
 * @return  constants information of bluetooth state-on
 */
jint CALEGetBTStateOnInfo(JNIEnv *env);

/**
 * @brief   get bluetooth adapter state information
 * @param   env              [IN] JNI interface pointer
 * @return  true if the local adapter is turned on
 */
jboolean CALEIsEnableBTAdapter(JNIEnv *env);

/**
 * @brief   get address from remote device
 * @param   env              [IN] JNI interface pointer
 * @param   bluetoothDevice  [IN] bluetooth device object
 * @return  remote address
 */
jstring CALEGetAddressFromBTDevice(JNIEnv *env, jobject bluetoothDevice);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

