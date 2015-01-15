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
* @file caleutils.h
* @brief This file contains the APIs for BT LE communications.
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

jobject CALEGetUuidFromString(JNIEnv *env, const char* uuid);

jobject CALEGetParcelUuid(JNIEnv *env, jobject uuid);

jstring CALEGetLocalDeviceAddress(JNIEnv *env);

jobjectArray CALEGetBondedDevices(JNIEnv *env);

jint CALEGetBTStateOnInfo(JNIEnv *env);

jboolean CALEIsEnableBTAdapter(JNIEnv *env);

jstring CALEGetAddressFromBTDevice(JNIEnv *env, jobject bluetoothDevice);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
