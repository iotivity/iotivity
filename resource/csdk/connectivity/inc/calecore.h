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
#include "jni.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*CAPacketReceiveCallback)(const char *address, const char *data);

void CALEClientJNISetContext(JNIEnv *env, jobject context);

void CALeCreateJniInterfaceObject();

void CALEInitialize(u_thread_pool_t handle);

void CALETerminate();

void CANativeSendFinsih(JNIEnv *env, jobject gatt);

int32_t CALESendUnicastMessage(const char *address, const char *data, uint32_t dataLen);

int32_t CALESendMulticastMessage(const char *data, uint32_t dataLen);

int32_t CALEStartUnicastServer(const char *address);

int32_t CALEStartMulticastServer();

int32_t CALEStopUnicastServer(int32_t serverID);

int32_t CALEStopMulticastServer(int32_t serverID);

void CALESetCallback(CAPacketReceiveCallback callback);

void CALEGetInterfaceInfo(CALocalConnectivity_t **info, uint32_t *size);

void CAGetLocalAddress(char *address);

int32_t CALESendUnicastMessageImpl(const char *address, const char *data, uint32_t dataLen);

int32_t CALESendMulticastMessageImpl(JNIEnv *env, const char *data, uint32_t dataLen);

/**
 * BT Common Method : JNI
 */
jstring CANativeGetLocalDeviceAddress(JNIEnv *env);

jobjectArray CANativeGetBondedDevices(JNIEnv *env);

jint CANativeGetBTStateOnInfo(JNIEnv *env);

jboolean CANativeIsEnableBTAdapter(JNIEnv *env);

jstring CANativeGetAddressFromBTDevice(JNIEnv *env, jobject bluetoothDevice);

jstring CANativeGetAddressFromGattObj(JNIEnv *env, jobject gatt);

jstring CANativeGetRemoteAddress(JNIEnv *env, jobject bluetoothSocketObj);

/**
 * BLE Method : JNI
 */
void CANativeGattClose(JNIEnv *env, jobject bluetoothGatt);

void CANativeLEStartScan();

void CANativeLEStartScanImpl(JNIEnv *env, jobject callback);

void CANativeLEStartScanWithUUIDImpl(JNIEnv *env, jobjectArray uuids, jobject callback);

jobject CANativeGetUUIDObject(JNIEnv *env, const char *uuid);

void CANativeLEStopScan();

void CANativeLEStopScanImpl(JNIEnv *env, jobject callback);

int32_t CANativeLEConnect(JNIEnv *env, jobject bluetoothDevice, jobject context,
                          jboolean autoconnect, jobject callback);

void CANativeLEDisconnect(JNIEnv *env, jobject bluetoothGatt);

void CANativeLEDiscoverServices(JNIEnv *env, jobject bluetoothGatt);

jboolean CANativeLESendData(JNIEnv *env, jobject bluetoothGatt, jobject gattCharacteristic);

void CANativeReadCharacteristic(JNIEnv *env, jobject bluetoothGatt);

jboolean CANativeSetCharacteristicNoti(JNIEnv *env, jobject bluetoothGatt, const char* uuid);

jobject CANativeCreateGattCharacteristic(JNIEnv *env, jobject bluetoothGatt, jbyteArray data);

jobject CANativeGetGattService(JNIEnv *env, jobject bluetoothGatt, jstring characterUUID);

jbyteArray CANativeGetValueFromCharacteristic(JNIEnv *env, jobject characteristic);

void CANativeCreateUUIDList();
/**
 * BluetoothDevice List
 */
void CANativeCreateScanDeviceList(JNIEnv *env);

void CANativeAddScanDeviceToList(JNIEnv *env, jobject device);

jboolean CANativeIsDeviceInList(JNIEnv *env, const char *remoteAddress);

void CANativeRemoveAllDevices(JNIEnv *env);

void CANativeRemoveDevice(JNIEnv *env, jstring remoteAddress);

void CAReorderingDeviceList(uint32_t index);

/**
 * BluetoothGatt List
 */
void CANativeCreateGattObjList(JNIEnv *env);

void CANativeAddGattobjToList(JNIEnv *env, jobject gatt);

jboolean CANativeIsGattObjInList(JNIEnv *env, const char *remoteAddress);

void CANativeRemoveAllGattObjsList(JNIEnv *env);

void CANativeLEDisconnectAll(JNIEnv *env);

void CANativeRemoveGattObj(JNIEnv *env, jstring address);

void CAReorderingGattList(uint32_t index);

void CANativeupdateSendCnt(JNIEnv *env);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
