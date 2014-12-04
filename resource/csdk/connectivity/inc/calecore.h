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
 * @file cawificore.h
 * @brief This file contains the APIs for Wi-Fi communications.
 */
#ifndef __CA_LECORE_H_
#define __CA_LECORE_H_

#include "cacommon.h"
#include "uthreadpool.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*CAPacketReceiveCallback)(const char* address, const char* data);

void CALEInitialize(u_thread_pool_t handle);

void CALETerminate();

int32_t CALESendUnicastMessage(const char* address, const char* data, int32_t length);

int32_t CALESendMulticastMessage(const char* m_address, const char* data);

int32_t CALEStartUnicastServer(const char* address, int32_t port);

int32_t CALEStartMulticastServer(const char* m_address, int32_t port);

int32_t CALEStopUnicastServer(int32_t server_id);

int32_t CALEStopMulticastServer(int32_t server_id);

void CALESetCallback(CAPacketReceiveCallback callback);

void CAGetLocalAddress(char* addressBuffer);

int32_t CASendUnicastMessageImpl(const char* address, const char* data);

int32_t CASendMulticastMessageImpl(const char* msg);

/**
 * BT Common Method : JNI
 */
jstring CANativeGetLocalDeviceAddress(JNIEnv* env);

jobjectArray CANativeGetBondedDevices(JNIEnv *env);

jint CANativeGetBTStateOnInfo(JNIEnv *env);

jstring CANativeGetRemoteAddress(JNIEnv *env, jobject bluetoothSocketObj);

void CANativeGattClose(JNIEnv *env, jobject bluetoothGatt);

void CANativeLEStartScan(JNIEnv *env, jobject callback);

void CANativeLEScanService(JNIEnv *env, jobjectArray uuids, jobject callback);

void CANativeLEStopScan(JNIEnv *env, jobject callback);

void CANativeLEConnect(JNIEnv *env, jstring address, jobject context, jboolean autoConnect, jobject callback);

void CANativeLEDisconnect(JNIEnv *env, jobject bluetoothGatt);

void CANativeLEDiscoverServices(JNIEnv *env, jobject bluetoothGatt);

void CANativeLESendData(JNIEnv *env, jobject bluetoothGatt, jobject gattCharacteristic);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
