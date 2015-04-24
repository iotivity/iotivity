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

#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include <unistd.h>
#include "caleserver.h"
#include "caleutils.h"
#include "logger.h"
#include "oic_malloc.h"
#include "uthreadpool.h"
#include "uarraylist.h"
#include "org_iotivity_ca_caleinterface.h"

#define TAG PCF("CA_LE_SERVER")

/* Service UUID */
static const char OIC_GATT_SERVICE_UUID[] = "713d0000-503e-4c75-ba94-3148f18d941e";
/* Read */
static const char OIC_GATT_CHARACTERISTIC_RESPONSE_UUID[] = "713d0002-503e-4c75-ba94-3148f18d941e";
/* Write */
static const char OIC_GATT_CHARACTERISTIC_REQUEST_UUID[] = "713d0003-503e-4c75-ba94-3148f18d941e";

static JavaVM *g_jvm;
static jobject g_context;
static jobject g_bluetoothGattServer;
static jobject g_bluetoothGattServerCallback;
static jobject g_leAdvertiseCallback;

static CAPacketReceiveCallback g_packetReceiveCallback = NULL;
static u_arraylist_t *g_connectedDeviceList = NULL;
static u_thread_pool_t g_threadPoolHandle = NULL;

static jboolean g_isStartServer;
static jboolean g_isSendingMulticastData;

//getting context
void CALEServerJNISetContext(JNIEnv *env, jobject context)
{
    OIC_LOG(DEBUG, TAG, "CALEServerJNISetContext");

    if (context == NULL)
    {
        OIC_LOG(ERROR, TAG, "context is null");
    }

    g_context = (*env)->NewGlobalRef(env, context);
}

//getting jvm
void CALeServerJniInit(JNIEnv *env, JavaVM *jvm)
{
    OIC_LOG(DEBUG, TAG, "CALeServerJniInit");
    g_jvm = jvm;
}

jobject CANativeLEServerSetResponseData(JNIEnv *env, jbyteArray responseData)
{
    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "CALEServerSetResponseData");

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
                                                           "android/bluetooth/BluetoothGattServer");

    jclass jni_cid_bluetoothGattService = (*env)->FindClass(
            env, "android/bluetooth/BluetoothGattService");

    jclass jni_cid_bluetoothGattCharacteristic = (*env)->FindClass(
            env, "android/bluetooth/BluetoothGattCharacteristic");

    jmethodID jni_mid_getService = (*env)->GetMethodID(
            env, jni_cid_bluetoothGattServer, "getService",
            "(Ljava/util/UUID;)Landroid/bluetooth/BluetoothGattService;");

    jobject jni_obj_serviceUUID = CALEGetUuidFromString(env, OIC_GATT_SERVICE_UUID);

    if (!g_bluetoothGattServer)
    {
        OIC_LOG(ERROR, TAG, "Check BluetoothGattServer status");
        return NULL;
    }
    jobject jni_obj_bluetoothGattService = (*env)->CallObjectMethod(env, g_bluetoothGattServer,
                                                                    jni_mid_getService,
                                                                    jni_obj_serviceUUID);

    jmethodID jni_mid_getCharacteristic = (*env)->GetMethodID(
            env, jni_cid_bluetoothGattService, "getCharacteristic",
            "(Ljava/util/UUID;)Landroid/bluetooth/BluetoothGattCharacteristic;");

    jobject jni_obj_responseUUID = CALEGetUuidFromString(env,
                                                         OIC_GATT_CHARACTERISTIC_RESPONSE_UUID);

    jobject jni_obj_bluetoothGattCharacteristic = (*env)->CallObjectMethod(
            env, jni_obj_bluetoothGattService, jni_mid_getCharacteristic, jni_obj_responseUUID);

    jmethodID jni_mid_setValue = (*env)->GetMethodID(env, jni_cid_bluetoothGattCharacteristic,
                                                     "setValue", "([B)Z");

    jboolean jni_boolean_setValue = (*env)->CallBooleanMethod(env,
                                                              jni_obj_bluetoothGattCharacteristic,
                                                              jni_mid_setValue, responseData);

    if (jni_boolean_setValue == JNI_FALSE)
    {
        OIC_LOG(ERROR, TAG, "Fail to set response data");
    }

    return jni_obj_bluetoothGattCharacteristic;
}

CAResult_t CANativeLEServerSendResponseData(JNIEnv *env, jobject device, jobject responseData)
{

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return CA_ADAPTER_NOT_ENABLED;
    }

    OIC_LOG(DEBUG, TAG, "CALEServerSendResponseData");

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
                                                           "android/bluetooth/BluetoothGattServer");

    jmethodID jni_mid_notifyCharacteristicChanged = (*env)->GetMethodID(
            env, jni_cid_bluetoothGattServer, "notifyCharacteristicChanged",
            "(Landroid/bluetooth/BluetoothDevice;"
            "Landroid/bluetooth/BluetoothGattCharacteristic;Z)Z");

    jboolean jni_boolean_notifyCharacteristicChanged = (*env)->CallBooleanMethod(
            env, g_bluetoothGattServer, jni_mid_notifyCharacteristicChanged, device, responseData,
            JNI_FALSE);

    if (jni_boolean_notifyCharacteristicChanged == JNI_FALSE)
    {
        OIC_LOG(ERROR, TAG, "Fail to notify characteristic");
        return CA_SEND_FAILED;
    }

    return CA_STATUS_OK;
}

CAResult_t CANativeLEServerSendResponse(JNIEnv *env, jobject device, jint requestId, jint status,
                                        jint offset, jbyteArray value)
{

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return CA_ADAPTER_NOT_ENABLED;
    }

    OIC_LOG(DEBUG, TAG, "CALEServerSendResponse");

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
                                                           "android/bluetooth/BluetoothGattServer");

    jmethodID jni_mid_sendResponse = (*env)->GetMethodID(
            env, jni_cid_bluetoothGattServer, "sendResponse",
            "(Landroid/bluetooth/BluetoothDevice;III[B)Z");

    jboolean jni_boolean_sendResponse = (*env)->CallBooleanMethod(env, g_bluetoothGattServer,
                                                                  jni_mid_sendResponse, device,
                                                                  requestId, status, offset, value);

    if (jni_boolean_sendResponse == JNI_FALSE)
    {
        OIC_LOG(ERROR, TAG, "Fail to send response for gatt characteristic write request");
        return CA_SEND_FAILED;
    }

    return CA_STATUS_OK;
}

void CANativeLEServerStartAdvertise(JNIEnv *env, jobject advertiseCallback)
{

    OIC_LOG(DEBUG, TAG, "LEServerStartAdvertise");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    jclass jni_cid_AdvertiseSettings = (*env)->FindClass(
            env, "android/bluetooth/le/AdvertiseSettings$Builder");

    jclass jni_cid_AdvertiseDataBuilder = (*env)->FindClass(
            env, "android/bluetooth/le/AdvertiseData$Builder");

    jclass jni_cid_BTAdapter = (*env)->FindClass(env, "android/bluetooth/BluetoothAdapter");

    jclass jni_cid_leAdvertiser = (*env)->FindClass(env,
                                                    "android/bluetooth/le/BluetoothLeAdvertiser");

    jmethodID jni_mid_AdvertiseSettings = (*env)->GetMethodID(env, jni_cid_AdvertiseSettings,
                                                              "<init>", "()V");

    jmethodID jni_mid_setAdvertiseMode = (*env)->GetMethodID(
            env, jni_cid_AdvertiseSettings, "setAdvertiseMode",
            "(I)Landroid/bluetooth/le/AdvertiseSettings$Builder;");

    jmethodID jni_mid_setConnectable = (*env)->GetMethodID(
            env, jni_cid_AdvertiseSettings, "setConnectable",
            "(Z)Landroid/bluetooth/le/AdvertiseSettings$Builder;");

    jmethodID jni_mid_setTimeout = (*env)->GetMethodID(
            env, jni_cid_AdvertiseSettings, "setTimeout",
            "(I)Landroid/bluetooth/le/AdvertiseSettings$Builder;");

    jmethodID jni_mid_AdvertiseDataBuilder = (*env)->GetMethodID(env, jni_cid_AdvertiseDataBuilder,
                                                                 "<init>", "()V");

    jmethodID jni_mid_addServiceUuid = (*env)->GetMethodID(
            env, jni_cid_AdvertiseDataBuilder, "addServiceUuid",
            "(Landroid/os/ParcelUuid;)Landroid/bluetooth/le/AdvertiseData$Builder;");

    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(
            env, jni_cid_BTAdapter, "getDefaultAdapter", "()Landroid/bluetooth/BluetoothAdapter;");

    jmethodID jni_mid_getBluetoothLeAdvertiser = (*env)->GetMethodID(
            env, jni_cid_BTAdapter, "getBluetoothLeAdvertiser",
            "()Landroid/bluetooth/le/BluetoothLeAdvertiser;");

    jmethodID jni_mid_build_LeAdvertiseSettings = (*env)->GetMethodID(
            env, jni_cid_AdvertiseSettings, "build", "()Landroid/bluetooth/le/AdvertiseSettings;");

    jmethodID jni_mid_build_LeAdvertiseData = (*env)->GetMethodID(
            env, jni_cid_AdvertiseDataBuilder, "build", "()Landroid/bluetooth/le/AdvertiseData;");

    jmethodID jni_mid_startAdvertising = (*env)->GetMethodID(
            env, jni_cid_leAdvertiser, "startAdvertising",
            "(Landroid/bluetooth/le/AdvertiseSettings;Landroid/bluetooth/le/AdvertiseData;"
            "Landroid/bluetooth/le/AdvertiseCallback;)V");

    jobject jni_AdvertiseSettings = (*env)->NewObject(env, jni_cid_AdvertiseSettings,
                                                      jni_mid_AdvertiseSettings);

    // 0: Low power, 1: Balanced
    jobject jni_obj_setAdvertiseMode = (*env)->CallObjectMethod(env, jni_AdvertiseSettings,
                                                                jni_mid_setAdvertiseMode, 0);

    jobject jni_obj_setConnectable = (*env)->CallObjectMethod(env, jni_AdvertiseSettings,
                                                              jni_mid_setConnectable, JNI_TRUE);

    //A value of 0 will disable the time limit
    jobject jni_obj_setTimeout = (*env)->CallObjectMethod(env, jni_AdvertiseSettings,
                                                          jni_mid_setTimeout, 0);

    jobject jni_AdvertiseDataBuilder = (*env)->NewObject(env, jni_cid_AdvertiseDataBuilder,
                                                         jni_mid_AdvertiseDataBuilder);

    jobject jni_obj_serviceUUID = CALEGetUuidFromString(env, OIC_GATT_SERVICE_UUID);

    jobject jni_ParcelUuid = CALEGetParcelUuid(env, jni_obj_serviceUUID);

    jobject jni_obj_addServiceUuid = (*env)->CallObjectMethod(env, jni_AdvertiseDataBuilder,
                                                              jni_mid_addServiceUuid,
                                                              jni_ParcelUuid);

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter,
                                                               jni_mid_getDefaultAdapter);

    jobject jni_obj_getBluetoothLeAdvertiser = (*env)->CallObjectMethod(
            env, jni_obj_BTAdapter, jni_mid_getBluetoothLeAdvertiser);

    jobject jni_obj_build_LeAdvertiseSettings = (*env)->CallObjectMethod(
            env, jni_AdvertiseSettings, jni_mid_build_LeAdvertiseSettings);

    jobject jni_obj_build_LeAdvertiseData = (*env)->CallObjectMethod(env, jni_AdvertiseDataBuilder,
                                                                     jni_mid_build_LeAdvertiseData);

    (*env)->CallVoidMethod(env, jni_obj_getBluetoothLeAdvertiser, jni_mid_startAdvertising,
                           jni_obj_build_LeAdvertiseSettings, jni_obj_build_LeAdvertiseData,
                           advertiseCallback);

    OIC_LOG(DEBUG, TAG, "Advertising started!!");
}

void CANativeLEServerStopAdvertise(JNIEnv *env, jobject advertiseCallback)
{

    OIC_LOG(DEBUG, TAG, "LEServerStopAdvertise");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    jclass jni_cid_BTAdapter = (*env)->FindClass(env, "android/bluetooth/BluetoothAdapter");

    jclass jni_cid_leAdvertiser = (*env)->FindClass(env,
                                                    "android/bluetooth/le/BluetoothLeAdvertiser");

    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(
            env, jni_cid_BTAdapter, "getDefaultAdapter", "()Landroid/bluetooth/BluetoothAdapter;");

    jmethodID jni_mid_getBluetoothLeAdvertiser = (*env)->GetMethodID(
            env, jni_cid_BTAdapter, "getBluetoothLeAdvertiser",
            "()Landroid/bluetooth/le/BluetoothLeAdvertiser;");

    jmethodID jni_mid_stopAdvertising = (*env)->GetMethodID(
            env, jni_cid_leAdvertiser, "stopAdvertising",
            "(Landroid/bluetooth/le/AdvertiseCallback;)V");

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter,
                                                               jni_mid_getDefaultAdapter);

    jobject jni_obj_getBluetoothLeAdvertiser = (*env)->CallObjectMethod(
            env, jni_obj_BTAdapter, jni_mid_getBluetoothLeAdvertiser);

    (*env)->CallVoidMethod(env, jni_obj_getBluetoothLeAdvertiser, jni_mid_stopAdvertising,
                           advertiseCallback);

    OIC_LOG(DEBUG, TAG, "Advertising stopped!!");
}

CAResult_t CALEStartGattServer(JNIEnv *env, jobject gattServerCallback)
{

    OIC_LOG(DEBUG, TAG, "CALEStartGattServer");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return CA_ADAPTER_NOT_ENABLED;
    }

    if (g_isStartServer)
    {
        OIC_LOG(DEBUG, TAG, "Gatt server already started");
    }

    g_bluetoothGattServerCallback = (*env)->NewGlobalRef(env, gattServerCallback);

    // open gatt server
    jobject bluetoothGattServer = CANativeLEServerOpenGattServer(env);
    if (!bluetoothGattServer)
    {
        OIC_LOG(ERROR, TAG, "bluetoothGattServer is null");
        return CA_STATUS_FAILED;
    }

    g_bluetoothGattServer = (*env)->NewGlobalRef(env, bluetoothGattServer);

    // create gatt service
    jobject bluetoothGattService = CANativeLEServerCreateGattService(env);

    // add gatt service
    return CANativeLEServerAddGattService(env, g_bluetoothGattServer, bluetoothGattService);
}

jobject CANativeLEServerOpenGattServer(JNIEnv *env)
{

    OIC_LOG(DEBUG, TAG, "CALEServerOpenGattServer");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return NULL;
    }

    jclass jni_cid_context = (*env)->FindClass(env, "android/content/Context");

    jclass jni_cid_bluetoothManager = (*env)->FindClass(env, "android/bluetooth/BluetoothManager");

    jfieldID jni_fid_bluetoothService = (*env)->GetStaticFieldID(env, jni_cid_context,
                                                                 "BLUETOOTH_SERVICE",
                                                                 "Ljava/lang/String;");

    jmethodID jni_mid_getSystemService = (*env)->GetMethodID(
            env, jni_cid_context, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

    jmethodID jni_mid_openGattServer = (*env)->GetMethodID(
            env, jni_cid_bluetoothManager, "openGattServer",
            "(Landroid/content/Context;Landroid/bluetooth/BluetoothGattServerCallback;)"
            "Landroid/bluetooth/BluetoothGattServer;");

    jobject jni_obj_bluetoothService = (*env)->GetStaticObjectField(env, jni_cid_context,
                                                                    jni_fid_bluetoothService);
    if (!jni_obj_bluetoothService)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_bluetoothService is null");
        return JNI_FALSE;
    }

    jobject jni_obj_bluetoothManager = (*env)->CallObjectMethod(env, g_context,
                                                                jni_mid_getSystemService,
                                                                jni_obj_bluetoothService);
    if (!jni_obj_bluetoothManager)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_bluetoothManager is null");
        return JNI_FALSE;
    }

    jobject jni_obj_bluetoothGattServer = (*env)->CallObjectMethod(env, jni_obj_bluetoothManager,
                                                                   jni_mid_openGattServer,
                                                                   g_context,
                                                                   g_bluetoothGattServerCallback);
    if (!jni_obj_bluetoothGattServer)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_bluetoothGattServer is null");
        return JNI_FALSE;
    }

    return jni_obj_bluetoothGattServer;
}

jobject CANativeLEServerCreateGattService(JNIEnv *env)
{

    OIC_LOG(DEBUG, TAG, "CALEServerCreateGattService");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return NULL;
    }

    jclass jni_cid_bluetoothGattService = (*env)->FindClass(
            env, "android/bluetooth/BluetoothGattService");

    jclass jni_cid_bluetoothGattCharacteristic = (*env)->FindClass(
            env, "android/bluetooth/BluetoothGattCharacteristic");

    jfieldID jni_fid_serviceType = (*env)->GetStaticFieldID(env, jni_cid_bluetoothGattService,
                                                            "SERVICE_TYPE_PRIMARY", "I");

    jfieldID jni_fid_readProperties = (*env)->GetStaticFieldID(env,
                                                               jni_cid_bluetoothGattCharacteristic,
                                                               "PROPERTY_READ", "I");

    jfieldID jni_fid_writeProperties = (*env)->GetStaticFieldID(env,
                                                                jni_cid_bluetoothGattCharacteristic,
                                                                "PROPERTY_WRITE", "I");

    jfieldID jni_fid_readPermissions = (*env)->GetStaticFieldID(env,
                                                                jni_cid_bluetoothGattCharacteristic,
                                                                "PERMISSION_READ", "I");

    jfieldID jni_fid_writePermissions = (*env)->GetStaticFieldID(
            env, jni_cid_bluetoothGattCharacteristic, "PERMISSION_WRITE", "I");

    jmethodID jni_mid_bluetoothGattService = (*env)->GetMethodID(env, jni_cid_bluetoothGattService,
                                                                 "<init>", "(Ljava/util/UUID;I)V");

    jmethodID jni_mid_addCharacteristic = (*env)->GetMethodID(
            env, jni_cid_bluetoothGattService, "addCharacteristic",
            "(Landroid/bluetooth/BluetoothGattCharacteristic;)Z");

    jmethodID jni_mid_bluetoothGattCharacteristic = (*env)->GetMethodID(
            env, jni_cid_bluetoothGattCharacteristic, "<init>", "(Ljava/util/UUID;II)V");

    jobject jni_obj_serviceUUID = CALEGetUuidFromString(env, OIC_GATT_SERVICE_UUID);

    jobject jni_obj_serviceType = (*env)->GetStaticObjectField(env, jni_cid_bluetoothGattService,
                                                               jni_fid_serviceType);

    jobject jni_bluetoothGattService = (*env)->NewObject(env, jni_cid_bluetoothGattService,
                                                         jni_mid_bluetoothGattService,
                                                         jni_obj_serviceUUID, jni_obj_serviceType);

    jobject jni_obj_readUuid = CALEGetUuidFromString(env, OIC_GATT_CHARACTERISTIC_RESPONSE_UUID);

    jint jni_int_readProperties = (*env)->GetStaticIntField(env,
                                                            jni_cid_bluetoothGattCharacteristic,
                                                            jni_fid_readProperties);

    jint jni_int_readPermissions = (*env)->GetStaticIntField(env,
                                                             jni_cid_bluetoothGattCharacteristic,
                                                             jni_fid_readPermissions);

    jobject jni_readCharacteristic = (*env)->NewObject(env, jni_cid_bluetoothGattCharacteristic,
                                                       jni_mid_bluetoothGattCharacteristic,
                                                       jni_obj_readUuid, jni_int_readProperties,
                                                       jni_int_readPermissions);

    jboolean jni_boolean_addReadCharacteristic = (*env)->CallBooleanMethod(
            env, jni_bluetoothGattService, jni_mid_addCharacteristic, jni_readCharacteristic);

    jobject jni_obj_writeUuid = CALEGetUuidFromString(env, OIC_GATT_CHARACTERISTIC_REQUEST_UUID);

    jint jni_int_writeProperties = (*env)->GetStaticIntField(env,
                                                             jni_cid_bluetoothGattCharacteristic,
                                                             jni_fid_writeProperties);

    jint jni_int_writePermissions = (*env)->GetStaticIntField(env,
                                                              jni_cid_bluetoothGattCharacteristic,
                                                              jni_fid_writePermissions);

    jobject jni_writeCharacteristic = (*env)->NewObject(env, jni_cid_bluetoothGattCharacteristic,
                                                        jni_mid_bluetoothGattCharacteristic,
                                                        jni_obj_writeUuid, jni_int_writeProperties,
                                                        jni_int_writePermissions);

    jboolean jni_boolean_addWriteCharacteristic = (*env)->CallBooleanMethod(
            env, jni_bluetoothGattService, jni_mid_addCharacteristic, jni_writeCharacteristic);

    if (jni_boolean_addWriteCharacteristic == JNI_FALSE)
    {
        OIC_LOG(ERROR, TAG, "Fail to add jni_boolean_addReadCharacteristic");
        return NULL;
    }

    return jni_bluetoothGattService;
}

CAResult_t CANativeLEServerAddGattService(JNIEnv *env, jobject bluetoothGattServer,
                                          jobject bluetoothGattService)
{

    OIC_LOG(DEBUG, TAG, "CALEServerAddGattService");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return CA_ADAPTER_NOT_ENABLED;
    }

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
                                                           "android/bluetooth/BluetoothGattServer");

    jmethodID jni_mid_addService = (*env)->GetMethodID(
            env, jni_cid_bluetoothGattServer, "addService",
            "(Landroid/bluetooth/BluetoothGattService;)Z");

    jboolean jni_boolean_addService = (*env)->CallBooleanMethod(env, bluetoothGattServer,
                                                                jni_mid_addService,
                                                                bluetoothGattService);

    if (jni_boolean_addService == JNI_FALSE)
    {
        OIC_LOG(ERROR, TAG, "Fail to add gatt service");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

CAResult_t CANativeLEServerConnect(JNIEnv *env, jobject bluetoothDevice)
{

    OIC_LOG(DEBUG, TAG, "CALEConnect");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return CA_ADAPTER_NOT_ENABLED;
    }

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
                                                           "android/bluetooth/BluetoothGattServer");

    jmethodID jni_mid_connect = (*env)->GetMethodID(env, jni_cid_bluetoothGattServer, "connect",
                                                    "(Landroid/bluetooth/BluetoothDevice;Z)Z");

    jboolean jni_boolean_connect = (*env)->CallBooleanMethod(env, g_bluetoothGattServer,
                                                             jni_mid_connect, bluetoothDevice,
                                                             JNI_FALSE);

    if (jni_boolean_connect == JNI_FALSE)
    {
        OIC_LOG(ERROR, TAG, "Fail to connect");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

void CANativeLEServerDisconnect(JNIEnv *env, jobject bluetoothDevice)
{

    OIC_LOG(DEBUG, TAG, "CALEDisconnect");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
                                                           "android/bluetooth/BluetoothGattServer");

    jmethodID jni_mid_cancelConnection = (*env)->GetMethodID(
            env, jni_cid_bluetoothGattServer, "cancelConnection",
            "(Landroid/bluetooth/BluetoothDevice;)V");

    (*env)->CallVoidMethod(env, g_bluetoothGattServer, jni_mid_cancelConnection, bluetoothDevice);
}

CAResult_t CALEServerSend(JNIEnv *env, jobject bluetoothDevice, jbyteArray responseData)
{

    OIC_LOG(DEBUG, TAG, "CALESend");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return CA_ADAPTER_NOT_ENABLED;
    }

    jobject responseChar = CANativeLEServerSetResponseData(env, responseData);

    CAResult_t result = CANativeLEServerSendResponseData(env, bluetoothDevice, responseChar);

    if (result != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, TAG, "Fail to send response data");
        return result;
    }

    return result;
}

void CALeServerCreateJniInterfaceObject()
{
    OIC_LOG(DEBUG, TAG, "CALeServerCreateJniInterfaceObject");

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (res != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (res != JNI_OK)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = JNI_TRUE;
    }

    // initialize le server

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
}

void CALEServerInitialize(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "CALEServerInitialize");

    g_threadPoolHandle = handle;

    g_isSendingMulticastData = JNI_FALSE;

    CALEServerCreateCachedDeviceList();
}

void CALEServerTerminate()
{
    OIC_LOG(DEBUG, TAG, "CALEServerTerminate");

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (res != JNI_OK)
    {
        OIC_LOG(ERROR, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (res != JNI_OK)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = JNI_TRUE;
    }

    CALEServerStopMulticastServer(0);

    CALEServerRemoveAllDevices(env);

    if (g_leAdvertiseCallback)
    {
        (*env)->DeleteGlobalRef(env, g_leAdvertiseCallback);
    }

    if (g_bluetoothGattServer)
    {
        (*env)->DeleteGlobalRef(env, g_bluetoothGattServer);
    }

    if (g_bluetoothGattServerCallback)
    {
        (*env)->DeleteGlobalRef(env, g_bluetoothGattServerCallback);
    }

    if (g_context)
    {
        (*env)->DeleteGlobalRef(env, g_context);
    }

    g_isStartServer = JNI_FALSE;

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
}

CAResult_t CALEServerSendUnicastMessage(const char* address, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerSendUnicastMessage(%s, %s)", address, data);

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (res != JNI_OK)
    {
        OIC_LOG(ERROR, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (res != JNI_OK)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread failed");
            return CA_STATUS_FAILED;
        }
        isAttached = JNI_TRUE;
    }

    CALEServerSendUnicastMessageImpl(env, address, data, dataLen);

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;
}

CAResult_t CALEServerSendMulticastMessage(const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerSendMulticastMessage(%s)", data);

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (res != JNI_OK)
    {
        OIC_LOG(ERROR, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (res != JNI_OK)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread failed");
            return CA_STATUS_FAILED;
        }
        isAttached = JNI_TRUE;
    }

    CALEServerSendMulticastMessageImpl(env, data, dataLen);

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;
}

CAResult_t CALEServerStartUnicastServer(const char* address)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerStartUnicastServer(%s)", address);

    return CA_STATUS_OK;
}

CAResult_t CALEServerStartMulticastServer()
{
    OIC_LOG(DEBUG, TAG, "CALEServerStartMulticastServer");

    if (g_isStartServer)
    {
        OIC_LOG(ERROR, TAG, "server is already started..it will be skipped");
        return CA_STATUS_FAILED;
    }

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (res != JNI_OK)
    {
        OIC_LOG(ERROR, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (res != JNI_OK)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread failed");
            return CA_STATUS_FAILED;
        }
        isAttached = JNI_TRUE;
    }

    g_isStartServer = JNI_TRUE;

    // start gatt server
    CAResult_t ret = CALEStartGattServer(env, g_bluetoothGattServerCallback);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "Fail to start gatt server");
        return ret;
    }

    // start advertise
    CANativeLEServerStartAdvertise(env, g_leAdvertiseCallback);

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;
}

CAResult_t CALEServerStopUnicastServer()
{
    OIC_LOG(DEBUG, TAG, "CALEServerStopUnicastServer");

    return CA_STATUS_OK;
}

CAResult_t CALEServerStopMulticastServer()
{
    OIC_LOG(DEBUG, TAG, "CALEServerStopMulticastServer");

    if (g_isStartServer == JNI_FALSE)
    {
        OIC_LOG(ERROR, TAG, "server is already stopped..it will be skipped");
        return CA_STATUS_FAILED;
    }

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (res != JNI_OK)
    {
        OIC_LOG(ERROR, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (res != JNI_OK)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread failed");
            return CA_STATUS_FAILED;
        }
        isAttached = JNI_TRUE;
    }

    CANativeLEServerStopAdvertise(env, g_leAdvertiseCallback);

    g_isStartServer = JNI_FALSE;

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;
}

void CALEServerSetCallback(CAPacketReceiveCallback callback)
{
    OIC_LOG(DEBUG, TAG, "CALEServerSetCallback");
    g_packetReceiveCallback = callback;
}

void CALEServerGetInterfaceInfo(CALocalConnectivity_t **info, uint32_t* size)
{
    OIC_LOG(DEBUG, TAG, "CALEServerGetInterfaceInfo");
    return;
}

void CALEServerGetLocalAddress(char* address)
{
    OIC_LOG(DEBUG, TAG, "CALEServerGetLocalAddress");

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (res != JNI_OK)
    {
        OIC_LOG(ERROR, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
        if (res != JNI_OK)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = JNI_TRUE;
    }

    jstring jni_address = CALEGetLocalDeviceAddress(env);
    const char* localAddress = (*env)->GetStringUTFChars(env, jni_address, NULL);
    if (NULL == localAddress)
    {
        OIC_LOG(ERROR, TAG, "there are no local address");
        return;
    }

    memcpy(address, localAddress, strlen(localAddress));

    OIC_LOG_V(DEBUG, TAG, "Local Address : %s", address);
    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
}

CAResult_t CALEServerSendUnicastMessageImpl(JNIEnv *env, const char* address, const char* data,
                                            uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerSendUnicastMessageImpl, address: %s, data: %s", address, data);

    // 1. get device object with address from cache
    // 2. connect to the gatt client device
    // 3. write a characteristic for response
    // 4. notify it
    // 5. disconnect

    jobject jni_obj_bluetoothDevice = NULL;

    if (g_connectedDeviceList == NULL)
    {
        OIC_LOG(ERROR, TAG, "g_connectedDeviceList is null");
    }

    if (g_connectedDeviceList)
    {
        jint index;
        for (index = 0; index < u_arraylist_length(g_connectedDeviceList); index++)
        {
            OIC_LOG(DEBUG, TAG, "check device address");
            jobject jarrayObj = (jobject) u_arraylist_get(g_connectedDeviceList, index);
            if (!jarrayObj)
            {
                OIC_LOG(ERROR, TAG, "jarrayObj is null");
                return CA_STATUS_FAILED;
            }

            jstring jni_setAddress = CALEGetAddressFromBTDevice(env, jarrayObj);
            if (!jni_setAddress)
            {
                OIC_LOG(ERROR, TAG, "jni_setAddress is null");
                return CA_STATUS_FAILED;
            }
            const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);

            if (!strcmp(setAddress, address))
            {
                OIC_LOG(DEBUG, TAG, "device address matched");
                jni_obj_bluetoothDevice = jarrayObj;
                break;
            }
            jni_obj_bluetoothDevice = jarrayObj;
        }

        if (jni_obj_bluetoothDevice)
        {
            jbyteArray jni_bytearr_data = (*env)->NewByteArray(env, dataLen);
            (*env)->SetByteArrayRegion(env, jni_bytearr_data, 0, dataLen, (jbyte*) data);

            CALEServerSend(env, jni_obj_bluetoothDevice, jni_bytearr_data);

        }
        else
        {
            OIC_LOG(ERROR, TAG, "jni_obj_bluetoothDevice is null");
        }
    }
    return CA_STATUS_OK;
}

CAResult_t CALEServerSendMulticastMessageImpl(JNIEnv *env, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerSendMulticastMessageImpl, send to, data: %s", data);

    if (!g_connectedDeviceList)
    {
        OIC_LOG(ERROR, TAG, "g_connectedDeviceList is null");
        return CA_STATUS_FAILED;
    }

    // 1. get all the device objects from cache
    // 2. connect to the gatt client devices
    // 3. write a characteristic for response
    // 4. notify it to every devices
    // 5. disconnect

    jint index;
    for (index = 0; index < u_arraylist_length(g_connectedDeviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_connectedDeviceList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            return CA_STATUS_FAILED;
        }

        g_isSendingMulticastData = JNI_TRUE;
        CANativeLEServerConnect(env, jarrayObj);

        sleep(1);
    }

    return CA_STATUS_OK;
}

void CALEServerCreateCachedDeviceList()
{
    OIC_LOG(DEBUG, TAG, "CALEServerCreateCachedDeviceList");

    // create new object array
    if (g_connectedDeviceList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Create device list");

        g_connectedDeviceList = u_arraylist_create();
    }
}

jboolean CALEServerIsDeviceInList(JNIEnv *env, const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "CALEServerIsDeviceInList");

    if (g_connectedDeviceList == NULL)
        OIC_LOG(ERROR, TAG, "list is null");

    uint32_t len = u_arraylist_length(g_connectedDeviceList);

    uint32_t index;
    for (index = 0; index < u_arraylist_length(g_connectedDeviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_connectedDeviceList, index);

        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            return JNI_FALSE;
        }

        jstring jni_setAddress = CALEGetAddressFromBTDevice(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "jni_setAddress is null");
            return JNI_FALSE;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress,
        NULL);

        if (!strcmp(remoteAddress, setAddress))
        {
            OIC_LOG(DEBUG, TAG, "the device is already set");
            return JNI_TRUE;
        }
        else
        {
            continue;
        }
    }

    OIC_LOG(DEBUG, TAG, "no device in list");
    return JNI_FALSE;
}

void CALEServerAddDeviceToList(JNIEnv *env, jobject device)
{
    if (device == NULL)
    {
        OIC_LOG(ERROR, TAG, "device is null");
        return;
    }

    if (g_connectedDeviceList == NULL)
    {
        OIC_LOG(ERROR, TAG, "list is null");
        return;
    }

    jstring jni_remoteAddress = CALEGetAddressFromBTDevice(env, device);
    if (!jni_remoteAddress)
    {
        OIC_LOG(ERROR, TAG, "jni_remoteAddress is null");
        return;
    }

    const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

    if (CALEServerIsDeviceInList(env, remoteAddress) == JNI_FALSE)
    {
        jobject gdevice = (*env)->NewGlobalRef(env, device);
        u_arraylist_add(g_connectedDeviceList, gdevice);
        OIC_LOG(DEBUG, TAG, "Set Object to Array as Element");
    }
}

void CALEServerRemoveAllDevices(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CALEServerRemoveAllDevices");

    if (!g_connectedDeviceList)
    {
        OIC_LOG(ERROR, TAG, "no deviceList");
        return;
    }

    uint32_t index;
    for (index = 0; index < u_arraylist_length(g_connectedDeviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_connectedDeviceList, index);

        if (jarrayObj)
        {
            (*env)->DeleteGlobalRef(env, jarrayObj);
        }
    }

    OICFree(g_connectedDeviceList);
    g_connectedDeviceList = NULL;
    return;
}

void CALEServerRemoveDevice(JNIEnv *env, jstring address)
{
    OIC_LOG(DEBUG, TAG, "CALEServerRemoveDevice");

    if (!g_connectedDeviceList)
    {
        OIC_LOG(ERROR, TAG, "no deviceList");
        return;
    }

    uint32_t index;
    for (index = 0; index < u_arraylist_length(g_connectedDeviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_connectedDeviceList, index);

        if (jarrayObj)
        {
            jstring jni_setAddress = CALEGetAddressFromBTDevice(env, jarrayObj);
            if (!jni_setAddress)
            {
                OIC_LOG(ERROR, TAG, "wrong device address");
                continue;
            }
            const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress,
            NULL);
            const char* remoteAddress = (*env)->GetStringUTFChars(env, address,
            NULL);

            if (!strcmp(setAddress, remoteAddress))
            {
                OIC_LOG_V(DEBUG, TAG, "device address : %s", remoteAddress);
                (*env)->DeleteGlobalRef(env, jarrayObj);

                CALEServerReorderinglist(index);
                return;
            }
        }
    }OIC_LOG(DEBUG, TAG, "no target object");
    return;
}

void CALEServerReorderinglist(uint32_t index)
{
    if (index >= g_connectedDeviceList->length)
        return;

    if (index < g_connectedDeviceList->length - 1)
    {
        memmove(&g_connectedDeviceList->data[index], &g_connectedDeviceList->data[index + 1],
                (g_connectedDeviceList->length - index - 1) * sizeof(void *));
    }

    g_connectedDeviceList->size--;
    g_connectedDeviceList->length--;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerServiceAddedCallback(JNIEnv *env, jobject obj,
                                                                       jint status,
                                                                       jobject gattService)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - Gatt Service Added Callback(%d)", status);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerCharacteristicReadRequestCallback(
        JNIEnv *env, jobject obj, jobject device, jint requestId, jint offset,
        jobject characteristic, jbyteArray data)
{
    OIC_LOG(DEBUG, TAG, "CALeInterface - Gatt Server Characteristic Read Request Callback");

    CANativeLEServerSendResponse(env, device, requestId, 0, offset, NULL);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerCharacteristicWriteRequestCallback(
        JNIEnv *env, jobject obj, jobject device, jint requestId, jobject characteristic,
        jbyteArray data, jboolean preparedWrite, jboolean responseNeeded, jint offset,
        jbyteArray value)
{
    OIC_LOG(DEBUG, TAG, "CALeInterface - Gatt Server Characteristic Write Request Callback");

    CANativeLEServerSendResponse(env, device, requestId, 0, offset, value);

    if (data == NULL)
    {
        OIC_LOG(ERROR, TAG, "Reqeust data is null");
        return;
    }

    // get Byte Array and covert to char*
    jint length = (*env)->GetArrayLength(env, data);

    jboolean isCopy;
    jbyte *jni_byte_requestData = (jbyte *) (*env)->GetByteArrayElements(env, data, &isCopy);

    char* requestData = NULL;
    requestData = (char*) OICCalloc(1, length + 1);
    if (NULL == requestData)
    {
        OIC_LOG(ERROR, TAG, "requestData is null");
        return;
    }

    strncpy(requestData, (char*) jni_byte_requestData, length);
    requestData[length] = '\0';
    (*env)->ReleaseByteArrayElements(env, data, jni_byte_requestData, JNI_ABORT);

    jstring jni_address = CALEGetAddressFromBTDevice(env, device);
    const char* address = (*env)->GetStringUTFChars(env, jni_address, NULL);
    OIC_LOG_V(DEBUG, TAG, "remote device address : %s", address);

    g_packetReceiveCallback(address, requestData);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerDescriptorReadRequestCallback(JNIEnv *env,
                                                                                jobject obj,
                                                                                jobject device,
                                                                                jint requestId,
                                                                                jint offset,
                                                                                jobject descriptor)
{
    OIC_LOG(DEBUG, TAG, "CALeInterface_CALeGattServerDescriptorReadRequestCallback");
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerDescriptorWriteRequestCallback(
        JNIEnv *env, jobject obj, jobject device, jint requestId, jobject descriptor,
        jboolean preparedWrite, jboolean responseNeeded, jint offset, jbyteArray value)
{
    OIC_LOG(DEBUG, TAG, "CALeInterface_CALeGattServerDescriptorWriteRequestCallback");
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerExecuteWriteCallback(JNIEnv *env, jobject obj,
                                                                       jobject device,
                                                                       jint requestId,
                                                                       jboolean execute)
{
    OIC_LOG(DEBUG, TAG, "CALeInterface_CALeGattServerExecuteWriteCallback");

    CANativeLEServerSendResponse(env, device, requestId, 0, 0, NULL);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerNotificationSentCallback(JNIEnv *env, jobject obj,
                                                                           jobject device,
                                                                           jint status)
{
    OIC_LOG(DEBUG, TAG, "CALeInterface - Gatt Server Notification Sent Callback");
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeAdvertiseStartSuccessCallback(JNIEnv *env, jobject obj,
                                                                      jobject settingsInEffect)
{
    OIC_LOG(DEBUG, TAG, "CALeInterface - LE Advertise Start Success Callback");
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeAdvertiseStartFailureCallback(JNIEnv *env, jobject obj,
                                                                      jint errorCode)
{
    OIC_LOG_V(ERROR, TAG, "CALeInterface - LE Advertise Start Failure Callback(%)", errorCode);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CARegisterLeGattServerCallback(JNIEnv *env, jobject obj,
                                                                   jobject callback)
{
    OIC_LOG(DEBUG, TAG, "CALeInterface - Register Le Gatt Server Callback");

    g_bluetoothGattServerCallback = (*env)->NewGlobalRef(env, callback);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CARegisterBluetoothLeAdvertiseCallback(JNIEnv *env, jobject obj,
                                                                           jobject callback)
{
    OIC_LOG(DEBUG, TAG, "CALeInterface - Register Le Advertise Callback");

    g_leAdvertiseCallback = (*env)->NewGlobalRef(env, callback);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerConnectionStateChangeCallback(JNIEnv *env,
                                                                                jobject obj,
                                                                                jobject device,
                                                                                jint status,
                                                                                jint newState)
{
    OIC_LOG(DEBUG, TAG, "CALeInterface - Gatt Server ConnectionStateChange Callback");

    OIC_LOG_V(DEBUG, TAG, "New connection State: %d", newState);

    if (!device)
    {
        OIC_LOG(ERROR, TAG, "device is null");
        return;
    }

    jclass jni_cid_bluetoothProfile = (*env)->FindClass(env, "android/bluetooth/BluetoothProfile");

    jfieldID jni_fid_state_connected = (*env)->GetStaticFieldID(env, jni_cid_bluetoothProfile,
                                                                "STATE_CONNECTED", "I");

    jfieldID jni_fid_state_disconnected = (*env)->GetStaticFieldID(env, jni_cid_bluetoothProfile,
                                                                   "STATE_DISCONNECTED", "I");

// STATE_CONNECTED
    jint jni_int_state_connected = (*env)->GetStaticIntField(env, jni_cid_bluetoothProfile,
                                                             jni_fid_state_connected);

// STATE_DISCONNECTED
    jint jni_int_state_disconnected = (*env)->GetStaticIntField(env, jni_cid_bluetoothProfile,
                                                                jni_fid_state_disconnected);

    if (newState == jni_int_state_connected)
    {

        OIC_LOG(DEBUG, TAG, "LE CONNECTED");

        jstring jni_remoteAddress = CALEGetAddressFromBTDevice(env, device);
        if (!jni_remoteAddress)
        {
            OIC_LOG(ERROR, TAG, "jni_remoteAddress is null");
            return;
        }

        const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

        if (g_connectedDeviceList == NULL)
        {
            OIC_LOG(ERROR, TAG, "g_connectedDeviceList is null");
        }

        if (CALEServerIsDeviceInList(env, remoteAddress) == JNI_FALSE)
        {
            OIC_LOG(DEBUG, TAG, "add connected device to cache");
            CALEServerAddDeviceToList(env, device);
        }
    }
    else if (newState == jni_int_state_disconnected)
    {
        OIC_LOG(DEBUG, TAG, "LE DISCONNECTED");
    }
}

