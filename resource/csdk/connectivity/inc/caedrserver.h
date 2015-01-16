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

typedef void (*CAPacketReceiveCallback)(const char *address, const char *data);


//getting jvm
void CAEDRServerJniInit(JNIEnv *env, JavaVM *jvm);

void CAEDRServerInitialize(u_thread_pool_t handle);

void CAEDRServerTerminate();

int32_t CAEDRStartUnicastServer(const char *address, bool isSecured);

int32_t CAEDRStartMulticastServer(bool isSecured);

int32_t CAEDRStopUnicastServer(int32_t serverID);

int32_t CAEDRStopMulticastServer(int32_t serverID);

/**
 * EDR Method
 */
uint32_t CAEDRNativeReadData(JNIEnv *env, uint32_t id, CAAdapterServerType_t type);

jobject CAEDRNativeListen(JNIEnv *env);

void CAEDRNativeAccept(JNIEnv *env, jobject severSocketObject);

void CAEDRNativeServerSocketClose(JNIEnv *env, const char* address, uint32_t id);

/**
 * BT State List
 */
void CAEDRServerNativeCreateDeviceStateList();

void CAEDRServerUpdateDeviceState(uint32_t state, const char* address);

void CAEDRServerNativeAddDeviceStateToList(state_t* state);

jboolean CAEDRServerNativeIsDeviceInList(const char *remoteAddress);

void CAEDRServerNativeRemoveAllDeviceState();

void CAEDRServerNativeRemoveDevice(const char* remoteAddress);

jboolean CAEDRServerIsConnectedDevice(const char* remoteAddress);

void CAEDRServerReorderingDeviceList(uint32_t index);

/**
 * Bluetooth Socket Object List
 */
void CAEDRServerNativeCreateDeviceSocketList();

void CAEDRServerNativeAddDeviceSocketToList(JNIEnv *env, jobject deviceSocket);

jboolean CAEDRServerNativeIsDeviceSocketInList(JNIEnv *env, const char *remoteAddress);

void CAEDRServerNativeRemoveAllDeviceSocket(JNIEnv *env);

void CAEDRServerNativeRemoveDeviceSocket(JNIEnv *env, jobject deviceSocket);

jobject CAEDRServerNativeGetDeviceSocket(uint32_t idx);

uint32_t CAEDRServerGetSocketListLength();

void CAEDRServerReorderingDeviceSocketList(uint32_t index);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
