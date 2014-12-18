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
* @file calecore.h
* @brief This file contains the APIs for BT LE communications.
*/
#ifndef __CA_LECORE_H_
#define __CA_LECORE_H_

#include "cacommon.h"
#include "uthreadpool.h"
#include "uarraylist.h"
#include "jni.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*CAPacketReceiveCallback)(const char *address,
        const char *data);

void CALEServerInitialize(u_thread_pool_t handle);

void CALEServerTerminate();

int32_t CALEServerSendUnicastMessage(const char *address, const char *data,
        uint32_t dataLen);

int32_t CALEServerSendMulticastMessage(const char *data, uint32_t dataLen);

int32_t CALEServerStartUnicastServer(const char *address);

int32_t CALEServerStartMulticastServer();

int32_t CALEServerStopUnicastServer(int32_t serverID);

int32_t CALEServerStopMulticastServer(int32_t serverID);

void CALEServerSetCallback(CAPacketReceiveCallback callback);

void CALEServerGetInterfaceInfo(CALocalConnectivity_t **info,
        uint32_t *size);

void CALEServerGetLocalAddress(char *address);

int32_t CALEServerSendUnicastMessageImpl(JNIEnv *env, const char *address,
        const char *data, uint32_t dataLen);

int32_t CALEServerSendMulticastMessageImpl(JNIEnv *env, const char *data,
        uint32_t dataLen);

/* BLE Server Functions */

void CALEServerJNISetContext(JNIEnv *env, jobject context);

void CALeServerJniInit(JNIEnv *env, JavaVM* jvm);

void CALeServerCreateJniInterfaceObject();

void LEServerStartAdvertise(JNIEnv *env, jobject advertiseCallback);

void LEServerStopAdvertise(JNIEnv *env, jobject advertiseCallback);

jobject CALEServerOpenGattServer(JNIEnv *env);

jobject CALEServerCreateGattService(JNIEnv *env);

jboolean CALEServerAddGattService(JNIEnv *env, jobject bluetoothGattServer,
        jobject bluetoothGattService);

jboolean CALEStartGattServer(JNIEnv *env, jobject gattServerCallback);

jboolean CALEServerSend(JNIEnv *env, jobject bluetoothDevice, jstring data);

jobject CALEServerSetResponseData(JNIEnv *env, jbyteArray responseData);

jboolean CALEServerSendResponseData(JNIEnv *env, jobject device, jobject responseData);

jboolean CALEServerSendResponse(JNIEnv *env, jobject device, jint requestId, jint status,
        jint offset, jbyteArray value);

jboolean CALEServerConnect(JNIEnv *env, jobject bluetoothDevice);

void CALEServerDisconnect(JNIEnv *env, jobject bluetoothDevice);

/* BLE Server Utils */
void CALEServerCreateCachedDeviceList();

jboolean CALEServerIsDeviceInList(JNIEnv *env, const char* remoteAddress);

void CALEServerAddDeviceToList(JNIEnv *env, jobject device);

void CALEServerRemoveAllDevices(JNIEnv *env);

void CALEServerRemoveDevice(JNIEnv *env, jstring address);

//void CALEServerReorderinglist(uint32_t index);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
