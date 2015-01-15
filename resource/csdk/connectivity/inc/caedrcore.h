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

typedef struct connected_state {
    char address[CA_MACADDR_SIZE];
    uint32_t state;
} state_t;

/**
 * @enum CAAdapterServerType_t
 * @brief Enum for defining different server types.
 */
typedef enum
{
    CA_UNICAST_SERVER = 0,      /**< Unicast Server */
    CA_MULTICAST_SERVER,        /**< Multicast Server */
    CA_SECURED_UNICAST_SERVER   /**< Secured Unicast Server */
} CAAdapterServerType_t;

typedef void (*CAPacketReceiveCallback)(const char *address, const char *data);

void CAEDRClientJNISetContext(JNIEnv *env, jobject context);

void CAEDRCreateJniInterfaceObject();

void CAEDRInitialize(u_thread_pool_t handle);

void CAEDRTerminate();

void CAEDRCoreJniInit(JNIEnv *env, JavaVM* jvm);

int32_t CAEDRSendUnicastMessage(const char *address, const char *data, uint32_t dataLen);

int32_t CAEDRSendMulticastMessage(const char *data, uint32_t dataLen);

int32_t CAEDRStartUnicastServer(const char *address, bool isSecured);

int32_t CAEDRStartMulticastServer(bool isSecured);

int32_t CAEDRStopUnicastServer(int32_t serverID);

int32_t CAEDRStopMulticastServer(int32_t serverID);

void CAEDRSetCallback(CAPacketReceiveCallback callback);

CAResult_t CAEDRGetInterfaceInfo(char **address);

void CAEDRGetLocalAddress(char** address);

int32_t CAEDRSendUnicastMessageImpl(const char *address, const char *data, uint32_t dataLen);

int32_t CAEDRSendMulticastMessageImpl(JNIEnv *env, const char *data, uint32_t dataLen);

/**
 * EDR Method
 */
void CAEDRNativeSendData(JNIEnv *env, const char* address, const char* data, uint32_t id);

int CAEDRNativeReadData(JNIEnv *env, uint32_t id, CAAdapterServerType_t type);

jobject CAEDRNativeListen(JNIEnv *env);

void CAEDRNativeAccept(JNIEnv *env, jobject severSocketObject);

void CAEDRNativeConnect(JNIEnv *env, const char* address, uint32_t id);

void CAEDRNativeSocketClose(JNIEnv *env, const char* address, uint32_t id);

/**
 * BT Common Method : JNI
 */
jstring CAEDRNativeGetAddressFromDeviceSocket(JNIEnv *env, jobject bluetoothSocketObj);

jstring CAEDRNativeGetLocalDeviceAddress(JNIEnv *env);

jobjectArray CAEDRNativeGetBondedDevices(JNIEnv *env);

jint CAEDRNativeGetBTStateOnInfo(JNIEnv *env);

jboolean CAEDRNativeIsEnableBTAdapter(JNIEnv *env);

jstring CAEDRNativeGetAddressFromBTDevice(JNIEnv *env, jobject bluetoothDevice);

/**
 * BT State List
 */
void CAEDRNativeCreateDeviceStateList();

void CAEDRUpdateDeviceState(uint32_t state, const char* address);

void CAEDRNativeAddDeviceStateToList(state_t* state);

jboolean CAEDRNativeIsDeviceInList(const char *remoteAddress);

void CAEDRNativeRemoveAllDeviceState();

void CAEDRNativeRemoveDevice(const char* remoteAddress);

jboolean CAEDRIsConnectedDevice(const char* remoteAddress);

void CAEDRReorderingDeviceList(uint32_t index);

/**
 * Bluetooth Socket Object List
 */
void CAEDRNativeCreateDeviceSocketList();

void CAEDRNativeAddDeviceSocketToList(JNIEnv *env, jobject deviceSocket);

jboolean CAEDRNativeIsDeviceSocketInList(JNIEnv *env, const char *remoteAddress);

void CAEDRNativeRemoveAllDeviceSocket(JNIEnv *env);

void CAEDRNativeRemoveDeviceSocket(JNIEnv *env, jobject deviceSocket);

jobject CAEDRNativeGetDeviceSocket(uint32_t idx);

uint32_t CAEDRGetSocketListLength();

void CAEDRReorderingDeviceSocketList(uint32_t index);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
