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

#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <unistd.h>

#include "calecore.h"
#include "caleserver.h"
#include "caleutils.h"

#include "logger.h"
#include "oic_malloc.h"
#include "cathreadpool.h" /* for thread pool */
#include "camutex.h"
#include "uarraylist.h"
#include "com_iotivity_jar_caleinterface.h"

//#define DEBUG_MODE
#define TAG PCF("CA_LE_CLIENT")

static const char METHODID_OBJECTNONPARAM[] = "()Landroid/bluetooth/BluetoothAdapter;";
static const char METHODID_STRINGNONPARAM[] = "()Ljava/lang/String;";
static const char CLASSPATH_BT_ADPATER[] = "android/bluetooth/BluetoothAdapter";
static const char CLASSPATH_BT_UUID[] = "java/util/UUID";
static const char CLASSPATH_BT_GATT[] = "android/bluetooth/BluetoothGatt";

static const char IOTIVITY_GATT_SERVIE_UUID[] = "713d0000-503e-4c75-ba94-3148f18d941e";
static const char IOTIVITY_GATT_TX_UUID[] = "713d0003-503e-4c75-ba94-3148f18d941e";
static const char IOTIVITY_GATT_RX_UUID[] = "713d0002-503e-4c75-ba94-3148f18d941e";

static const uint32_t STATE_CONNECTED = 2;
static const uint32_t STATE_DISCONNECTED = 0;
static const uint32_t GATT_SUCCESS = 0;

static const size_t MAX_PDU_BUFFER = 1400;

JavaVM *g_jvm;
static u_arraylist_t *g_deviceList = NULL;
static u_arraylist_t *g_gattObjectList = NULL;
static CAPacketReceiveCallback g_packetReceiveCallback = NULL;
static ca_thread_pool_t g_threadPoolHandle = NULL;
static jobject g_leScanCallback;
static jobject g_leGattCallback;
static jobject g_context;
static jobjectArray g_UUIDList;
static jboolean g_isStartServer;
static jboolean g_isFinishSendData;

static jbyteArray g_sendBuffer;
static uint32_t g_targetCnt = 0;
static uint32_t g_currentSentCnt = 0;

/** mutex for synchrnoization **/
static ca_mutex g_threadMutex;
/** conditional mutex for synchrnoization **/
static ca_cond g_threadCond;

///////////////////////////////////////////////////////////////////////////////////////////////////
//FIXME getting context
void CALEClientJNISetContext(JNIEnv *env, jobject context)
{
    OIC_LOG(DEBUG, TAG, "CALEClientJNISetContext");

    if (context == NULL)
    {
        OIC_LOG(ERROR, TAG, "context is null");
        return;
    }

    g_context = (*env)->NewGlobalRef(env, context);
}

void CALeCreateJniInterfaceObject()
{
    OIC_LOG(DEBUG, TAG, "CALeCreateJniInterfaceObject");

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

    jclass LeJniInterface = (*env)->FindClass(env, "com/iotivity/jar/CALeInterface");
    if (!LeJniInterface)
    {
        OIC_LOG(DEBUG, TAG, "Could not get CALeInterface class");
        return;
    }

    jmethodID LeInterfaceConstructorMethod = (*env)->GetMethodID(env, LeJniInterface, "<init>",
                                                                 "(Landroid/content/Context;)V");
    if (!LeInterfaceConstructorMethod)
    {
        OIC_LOG(DEBUG, TAG, "Could not get CALeInterface constructor method");
        return;
    }

    (*env)->NewObject(env, LeJniInterface, LeInterfaceConstructorMethod, g_context);
    OIC_LOG(DEBUG, TAG, "Create CALeInterface instance");

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
}

JNIEXPORT jint JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    OIC_LOG(DEBUG, TAG, "JNI_OnLoad in calecore");

    JNIEnv* env;
    if ((*jvm)->GetEnv(jvm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        return -1;
    }
    g_jvm = jvm; /* cache the JavaVM pointer */

    //JVM required for WifiCore to work with JNI interface
    CAWiFiJniInit(jvm);
    CALeServerJniInit(env, jvm);
    CAEDRCoreJniInit(env, jvm);

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *jvm, void *reserved)
{
    OIC_LOG(DEBUG, TAG, "JNI_OnUnload in calecore");

    JNIEnv* env;
    if ((*jvm)->GetEnv(jvm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        return;
    }
    g_jvm = 0;
    return;
}

void CALEInitialize(ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "CALEInitialize");

    g_threadPoolHandle = handle;

    // init mutex for send logic
    g_threadMutex = ca_mutex_new();
    g_threadCond = ca_cond_new();

    CANativeCreateUUIDList();

    CALeCreateJniInterfaceObject(); /* create java CALeInterface instance*/
}

void CALETerminate()
{
    OIC_LOG(DEBUG, TAG, "CALETerminate");

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

    CANativeLEDisconnectAll(env);

    if (g_leScanCallback)
    {
        CANativeLEStopScanImpl(env, g_leScanCallback);
        sleep(1);
    }

    if (g_leScanCallback)
    {
        (*env)->DeleteGlobalRef(env, g_leScanCallback);
    }

    if (g_leGattCallback)
    {
        (*env)->DeleteGlobalRef(env, g_leGattCallback);
    }

    if (g_context)
    {
        (*env)->DeleteGlobalRef(env, g_context);
    }

    if (g_sendBuffer)
    {
        (*env)->DeleteGlobalRef(env, g_sendBuffer);
    }

    if (g_UUIDList)
    {
        (*env)->DeleteGlobalRef(env, g_UUIDList);
    }

    CANativeRemoveAllDevices(env);
    CANativeRemoveAllGattObjsList(env);
    g_isStartServer = FALSE;
    g_isFinishSendData = FALSE;

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    // delete mutex object
    ca_mutex_free(g_threadMutex);
    g_threadMutex = NULL;
    ca_cond_signal(g_threadCond);
    ca_cond_free(g_threadCond);
}

void CANativeSendFinish(JNIEnv *env, jobject gatt)
{
    OIC_LOG(DEBUG, TAG, "CANativeSendFinish");

    if (gatt)
    {
        CANativeLEDisconnect(env, gatt);
    }
    CANativeupdateSendCnt(env);
}

CAResult_t CALESendUnicastMessage(const char* address, const char* data, const uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALESendUnicastMessage(%s, %s)", address, data);

    return CALESendUnicastMessageImpl(address, data, dataLen);
}

CAResult_t CALESendMulticastMessage(const char* data, const uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALESendMulticastMessage(%s)", data);

    return CALESendMulticastMessageImpl(data, dataLen);
}

CAResult_t CALEStartUnicastServer(const char* address)
{
    OIC_LOG_V(DEBUG, TAG, "CALEStartUnicastServer(%s)", address);

    return CA_STATUS_OK;
}

CAResult_t CALEStartMulticastServer()
{
    OIC_LOG(DEBUG, TAG, "CALEStartMulticastServer");

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

    CANativeLEStartScan();

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;
}

void CALEStopUnicastServer()
{
    OIC_LOG(DEBUG, TAG, "CALEStopUnicastServer");
}

void CALEStopMulticastServer()
{
    OIC_LOG(DEBUG, TAG, "CALEStopMulticastServer");
    g_isStartServer = JNI_FALSE;
    CANativeLEStopScan();
}

void CALESetCallback(CAPacketReceiveCallback callback)
{
    g_packetReceiveCallback = callback;
}

CAResult_t CALEGetInterfaceInfo(char **address)
{
    CALEGetLocalAddress(address);
    return CA_STATUS_OK;
}

void CALEGetLocalAddress(char** address)
{
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
    if (jni_address)
    {
        const char* localAddress = (*env)->GetStringUTFChars(env, jni_address, NULL);
        uint32_t length = strlen(localAddress);
        *address = (char*) OICMalloc(length + 1);
        if (*address == NULL)
        {
            return;
        }
        memcpy(*address, localAddress, length + 1);
    }

    OIC_LOG_V(DEBUG, TAG, "Local Address : %s", *address);
    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
}

CAResult_t CALESendUnicastMessageImpl(const char* address, const char* data, const uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALESendUnicastMessageImpl, address: %s, data: %s", address, data);

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

    OIC_LOG(DEBUG, TAG, "[BLE][Native] set byteArray for data");
    if (g_sendBuffer)
    {
        (*env)->DeleteGlobalRef(env, g_sendBuffer);
    }
    jbyteArray jni_arr = (*env)->NewByteArray(env, dataLen);
    (*env)->SetByteArrayRegion(env, jni_arr, 0, dataLen, (jbyte*) data);
    g_sendBuffer = (jbyteArray)(*env)->NewGlobalRef(env, jni_arr);

    // connect to gatt server
    CANativeLEStopScanImpl(env, g_leScanCallback);
    sleep(1);

    jobject jni_obj_bluetoothDevice = NULL;
    if (g_context && g_deviceList)
    {
        jint index;
        for (index = 0; index < u_arraylist_length(g_deviceList); index++)
        {
            jobject jarrayObj = (jobject) u_arraylist_get(g_deviceList, index);
            if (!jarrayObj)
            {
                OIC_LOG(ERROR, TAG, "[BLE][Native] jarrayObj is null");
                return CA_STATUS_FAILED;
            }

            jstring jni_setAddress = CALEGetAddressFromBTDevice(env, jarrayObj);
            if (!jni_setAddress)
            {
                OIC_LOG(ERROR, TAG, "[BLE][Native] jni_setAddress is null");
                return CA_STATUS_FAILED;
            }
            const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);

            if (!strcmp(setAddress, address))
            {
                jni_obj_bluetoothDevice = jarrayObj;
                break;
            }
        }

        if (jni_obj_bluetoothDevice)
        {
            jboolean autoConnect = FALSE;
            CANativeLEConnect(env, jni_obj_bluetoothDevice, g_context, autoConnect,
                              g_leGattCallback);
        }
    }

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;
}

CAResult_t CALESendMulticastMessageImpl(const char* data, const uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CASendMulticastMessageImpl, send to, data: %s, %d", data, dataLen);

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

    if (!g_deviceList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] g_deviceList is null");
        return CA_STATUS_FAILED;
    }OIC_LOG(DEBUG, TAG, "set wait");

    g_isFinishSendData = JNI_FALSE;

    OIC_LOG(DEBUG, TAG, "[BLE][Native] set byteArray for data");
    if (g_sendBuffer)
    {
        (*env)->DeleteGlobalRef(env, g_sendBuffer);
    }
    jbyteArray jni_arr = (*env)->NewByteArray(env, dataLen);
    (*env)->SetByteArrayRegion(env, jni_arr, 0, dataLen, (jbyte*) data);
    g_sendBuffer = (jbyteArray)(*env)->NewGlobalRef(env, jni_arr);

    // connect to gatt server
    CANativeLEStopScanImpl(env, g_leScanCallback);
    sleep(1);

    // reset gatt list
    CANativeRemoveAllGattObjsList(env);
    CANativeCreateGattObjList(env);

    g_targetCnt = u_arraylist_length(g_deviceList);

    jint index = 0;

    while (index < u_arraylist_length(g_deviceList))
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jarrayObj is null");
            continue;
        }

        if (CA_STATUS_FAILED
                == CANativeLEConnect(env, jarrayObj, g_context, JNI_FALSE, g_leGattCallback))
        {
            // connection failure
        }
        index++;
        sleep(1);
    }

    // wait for finish to send data through "CALeGattServicesDiscoveredCallback"
    if (!g_isFinishSendData)
    {
        ca_mutex_lock(g_threadMutex);
        ca_cond_wait(g_threadCond, g_threadMutex);
        OIC_LOG(DEBUG, TAG, "unset wait");
        ca_mutex_unlock(g_threadMutex);
    }

    // start LE Scan again
#ifdef DEBUG_MODE
    CANativeLEStartScan();
#endif

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;
}

jstring CANativeGetAddressFromGattObj(JNIEnv *env, jobject gatt)
{
    if (!gatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] gatt is null");
        return NULL;
    }

    jclass jni_cid_gattdevice_list = (*env)->FindClass(env, CLASSPATH_BT_GATT);
    if (!jni_cid_gattdevice_list)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_gattdevice_list is null");
        return NULL;
    }

    jmethodID jni_mid_getDevice = (*env)->GetMethodID(env, jni_cid_gattdevice_list, "getDevice",
                                                      "()Landroid/bluetooth/BluetoothDevice;");
    if (!jni_mid_getDevice)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_mid_getDevice is null");
        return NULL;
    }

    jobject jni_obj_device = (*env)->CallObjectMethod(env, gatt, jni_mid_getDevice);
    if (!jni_obj_device)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_obj_device is null");
        return NULL;
    }

    jstring jni_address = CALEGetAddressFromBTDevice(env, jni_obj_device);
    if (!jni_address)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_address is null");
        return NULL;
    }

    return jni_address;
}

/**
 * BLE layer
 */
void CANativeGattClose(JNIEnv *env, jobject bluetoothGatt)
{
    // GATT CLOSE
    OIC_LOG(DEBUG, TAG, "[BLE][Native] GATT CLOSE");

    // get BluetoothGatt class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get BluetoothGatt class");
    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, CLASSPATH_BT_GATT);
    if (!jni_cid_BluetoothGatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return;
    }

    jmethodID jni_mid_closeGatt = (*env)->GetMethodID(env, jni_cid_BluetoothGatt, "close", "()V");
    if (!jni_mid_closeGatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_mid_closeGatt is null");
        return;
    }

    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] request close Gatt");
    (*env)->CallVoidMethod(env, bluetoothGatt, jni_mid_closeGatt);
}

void CANativeLEStartScan()
{
    if (!g_isStartServer)
    {
        OIC_LOG(DEBUG, TAG, "server is not started yet..scan will be passed");
        return;
    }

    jboolean isAttached = FALSE;
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
        isAttached = TRUE;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeLEStartScan");

    // scan gatt server with UUID
    if (g_leScanCallback && g_UUIDList)
    {
        CANativeLEStartScanWithUUIDImpl(env, g_UUIDList, g_leScanCallback);
    }

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

}

void CANativeLEStartScanImpl(JNIEnv *env, jobject callback)
{
    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    // get default bt adapter class
    jclass jni_cid_BTAdapter = (*env)->FindClass(env, CLASSPATH_BT_ADPATER);
    if (!jni_cid_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] getState From BTAdapter: jni_cid_BTAdapter is null");
        return;
    }

    // get remote bt adapter method
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env, jni_cid_BTAdapter,
                                                                    "getDefaultAdapter",
                                                                    METHODID_OBJECTNONPARAM);
    if (!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] getState From BTAdapter: getDefaultAdapter is null");
        return;
    }

    // get start le scan method
    jmethodID jni_mid_startLeScan = (*env)->GetMethodID(
            env, jni_cid_BTAdapter, "startLeScan",
            "(Landroid/bluetooth/BluetoothAdapter$LeScanCallback;)Z");
    if (!jni_mid_startLeScan)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] startLeScan: jni_mid_startLeScan is null");
        return;
    }

    // gat bt adapter object
    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter,
                                                               jni_mid_getDefaultAdapter);
    if (!jni_obj_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] getState From BTAdapter: jni_obj_BTAdapter is null");
        return;
    }

    // call start le scan method
    jboolean jni_obj_startLeScan = (*env)->CallBooleanMethod(env, jni_obj_BTAdapter,
                                                             jni_mid_startLeScan, callback);
    if (!jni_obj_startLeScan)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] startLeScan is failed");
        return;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] startLeScan is started");
    }
}

jobject CANativeGetUUIDObject(JNIEnv *env, const char* uuid)
{
    // setting UUID
    jclass jni_cid_uuid = (*env)->FindClass(env, CLASSPATH_BT_UUID);
    if (!jni_cid_uuid)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_uuid is null");
        return NULL;
    }

    jmethodID jni_mid_fromString = (*env)->GetStaticMethodID(
            env, jni_cid_uuid, "fromString", "(Ljava/lang/String;)Ljava/util/UUID;");
    if (!jni_mid_fromString)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_mid_fromString is null");
        return NULL;
    }

    jstring jni_uuid = (*env)->NewStringUTF(env, uuid);
    jobject jni_obj_uuid = (*env)->CallStaticObjectMethod(env, jni_cid_uuid, jni_mid_fromString,
                                                          jni_uuid);
    if (!jni_obj_uuid)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_obj_uuid is null");
        return NULL;
    }

    return jni_obj_uuid;
}

void CANativeLEStartScanWithUUIDImpl(JNIEnv *env, jobjectArray uuids, jobject callback)
{
    // get default bt adapter class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get default bt adapter class");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    jclass jni_cid_BTAdapter = (*env)->FindClass(env, CLASSPATH_BT_ADPATER);
    if (!jni_cid_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] getState From BTAdapter: jni_cid_BTAdapter is null");
        return;
    }

    // get remote bt adapter method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get remote bt adapter method");
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env, jni_cid_BTAdapter,
                                                                    "getDefaultAdapter",
                                                                    METHODID_OBJECTNONPARAM);
    if (!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] getState From BTAdapter: getDefaultAdapter is null");
        return;
    }

    // get start le scan method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get start le scan method");
    jmethodID jni_mid_startLeScan = (*env)->GetMethodID(
            env, jni_cid_BTAdapter, "startLeScan",
            "([Ljava/util/UUID;Landroid/bluetooth/BluetoothAdapter$LeScanCallback;)Z");
    if (!jni_mid_startLeScan)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] startLeScan: jni_mid_startLeScan is null");
        return;
    }

    // get bt adapter object
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get bt adapter object");
    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter,
                                                               jni_mid_getDefaultAdapter);
    if (!jni_obj_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] getState From BTAdapter: jni_obj_BTAdapter is null");
        return;
    }

    // call start le scan method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] call start le scan service method");
    jboolean jni_obj_startLeScan = (*env)->CallBooleanMethod(env, jni_obj_BTAdapter,
                                                             jni_mid_startLeScan, uuids, callback);
    if (!jni_obj_startLeScan)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] startLeScan With UUID is failed");
        return;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] startLeScan With UUID is started");
    }
}

void CANativeLEStopScan()
{
    jboolean isAttached = FALSE;
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
        isAttached = TRUE;
    }

    CANativeLEStopScanImpl(env, g_leScanCallback);

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

}

void CANativeLEStopScanImpl(JNIEnv *env, jobject callback)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeLEStopScan");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    // get default bt adapter class
    jclass jni_cid_BTAdapter = (*env)->FindClass(env, CLASSPATH_BT_ADPATER);
    if (!jni_cid_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] getState From BTAdapter: jni_cid_BTAdapter is null");
        return;
    }

    // get remote bt adapter method
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env, jni_cid_BTAdapter,
                                                                    "getDefaultAdapter",
                                                                    METHODID_OBJECTNONPARAM);
    if (!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] getState From BTAdapter: getDefaultAdapter is null");
        return;
    }

    // get start le scan method
    jmethodID jni_mid_stopLeScan = (*env)->GetMethodID(
            env, jni_cid_BTAdapter, "stopLeScan",
            "(Landroid/bluetooth/BluetoothAdapter$LeScanCallback;)V");
    if (!jni_mid_stopLeScan)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] stopLeScan: jni_mid_stopLeScan is null");
        return;
    }

    // gat bt adapter object
    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter,
                                                               jni_mid_getDefaultAdapter);
    if (!jni_obj_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_obj_BTAdapter is null");
        return;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] CALL API - request to stop LE Scan");
    // call start le scan method
    (*env)->CallVoidMethod(env, jni_obj_BTAdapter, jni_mid_stopLeScan, callback);
}

CAResult_t CANativeLEConnect(JNIEnv *env, jobject bluetoothDevice, jobject context,
                             jboolean autoconnect, jobject callback)
{
    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return CA_STATUS_FAILED;
    }

    jstring jni_address = CALEGetAddressFromBTDevice(env, bluetoothDevice);
    const char * addr = (*env)->GetStringUTFChars(env, jni_address, NULL);
    OIC_LOG_V(DEBUG, TAG, "[BLE][Native] request connectGatt to %s", addr);

    // GATT CONNECT

    // get BluetoothDevice class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get BluetoothDevice class");
    jclass jni_cid_BluetoothDevice = (*env)->FindClass(env, "android/bluetooth/BluetoothDevice");
    if (!jni_cid_BluetoothDevice)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] bleConnect: jni_cid_BluetoothDevice is null");
        return CA_STATUS_FAILED;
    }

    // get connectGatt method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get connectGatt method");
    jmethodID jni_mid_connectGatt = (*env)->GetMethodID(
            env, jni_cid_BluetoothDevice, "connectGatt",
            "(Landroid/content/Context;ZLandroid/bluetooth/BluetoothGattCallback;)"
            "Landroid/bluetooth/BluetoothGatt;");
    if (!jni_mid_connectGatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] bleConnect: jni_mid_connectGatt is null");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] Call object method - connectGatt");
    jobject jni_obj_connectGatt = (*env)->CallObjectMethod(env, bluetoothDevice,
                                                           jni_mid_connectGatt,
                                                           NULL,
                                                           autoconnect, callback);
    if (!jni_obj_connectGatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] CALL API - connectGatt was failed.obj will be removed");
        CANativeRemoveDevice(env, jni_address);
        CANativeupdateSendCnt(env);
        return CA_STATUS_FAILED;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] CALL API - connecting..");
    }
    return CA_STATUS_OK;
}

void CANativeLEDisconnect(JNIEnv *env, jobject bluetoothGatt)
{
    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    // GATT DISCONNECT
    OIC_LOG(DEBUG, TAG, "[BLE][Native] GATT DISCONNECT");

    // get BluetoothGatt class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get BluetoothGatt classjobject bluetoothGatt");
    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, CLASSPATH_BT_GATT);
    if (!jni_cid_BluetoothGatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] get gatt disconnect method");
    jmethodID jni_mid_disconnectGatt = (*env)->GetMethodID(env, jni_cid_BluetoothGatt, "disconnect",
                                                           "()V");
    if (!jni_mid_disconnectGatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_mid_disconnectGatt is null");
        return;
    }

    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CALL API - request disconnectGatt");
    (*env)->CallVoidMethod(env, bluetoothGatt, jni_mid_disconnectGatt);

}

void CANativeLEDisconnectAll(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CANativeLEDisconnectAll");

    if (!g_gattObjectList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] g_gattObjectList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(g_gattObjectList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_gattObjectList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jarrayObj is null");
            continue;
        }
        CANativeLEDisconnect(env, jarrayObj);
    }

    OICFree(g_gattObjectList);
    g_gattObjectList = NULL;
    return;
}

void CANativeLEDiscoverServices(JNIEnv *env, jobject bluetoothGatt)
{
    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    // GATT SERVICE DISCOVERY
    OIC_LOG(DEBUG, TAG, "[BLE][Native] GATT SERVICE DISCOVERY");

    // get BluetoothGatt class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get BluetoothGatt class");
    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, CLASSPATH_BT_GATT);
    if (!jni_cid_BluetoothGatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] discovery gatt services method");
    jmethodID jni_mid_discoverServices = (*env)->GetMethodID(env, jni_cid_BluetoothGatt,
                                                             "discoverServices", "()Z");
    if (!jni_mid_discoverServices)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_mid_discoverServices is null");
        return;
    }
    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CALL API - request discovery gatt services");
    (*env)->CallBooleanMethod(env, bluetoothGatt, jni_mid_discoverServices);
}

CAResult_t CANativeWriteCharacteristic(JNIEnv *env, jobject bluetoothGatt,
                                       jobject gattCharacteristic)
{
    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return CA_STATUS_FAILED;
    }

    // WRITE GATT CHARACTERISTIC
    OIC_LOG(DEBUG, TAG, "[BLE][Native] WRITE GATT CHARACTERISTIC");

    // get BluetoothGatt class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get BluetoothGatt class");
    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, CLASSPATH_BT_GATT);
    if (!jni_cid_BluetoothGatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] write characteristic method");
    jmethodID jni_mid_writeCharacteristic = (*env)->GetMethodID(
            env, jni_cid_BluetoothGatt, "writeCharacteristic",
            "(Landroid/bluetooth/BluetoothGattCharacteristic;)Z");
    if (!jni_mid_writeCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_mid_writeCharacteristic is null");
        return CA_STATUS_FAILED;
    }

    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CALL API - request to write gatt characteristic");
    jboolean ret = (jboolean)(*env)->CallBooleanMethod(env, bluetoothGatt,
                                                       jni_mid_writeCharacteristic,
                                                       gattCharacteristic);
    if (ret)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] writeCharacteristic is success");
    }
    else
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] writeCharacteristic is failed");
        return CA_STATUS_FAILED;
    }
    return CA_STATUS_OK;
}

void CANativeReadCharacteristic(JNIEnv *env, jobject bluetoothGatt)
{

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, CLASSPATH_BT_GATT);
    if (!jni_cid_BluetoothGatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return;
    }

    jstring jni_uuid = (*env)->NewStringUTF(env, IOTIVITY_GATT_RX_UUID);
    jobject jni_obj_GattCharacteristic = CANativeGetGattService(env, bluetoothGatt, jni_uuid);
    if (!jni_obj_GattCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_obj_GattCharacteristic is null");
        return;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] read characteristic method");
    jmethodID jni_mid_readCharacteristic = (*env)->GetMethodID(
            env, jni_cid_BluetoothGatt, "readCharacteristic",
            "(Landroid/bluetooth/BluetoothGattCharacteristic;)Z");
    if (!jni_mid_readCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_mid_readCharacteristic is null");
        return;
    }

    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CALL API - request to read gatt characteristic");
    jboolean ret = (*env)->CallBooleanMethod(env, bluetoothGatt, jni_mid_readCharacteristic,
                                             jni_obj_GattCharacteristic);
    if (ret)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] readCharacteristic is success");
    }
    else
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] readCharacteristic is failed");
    }
}

CAResult_t CANativeSetCharacteristicNotification(JNIEnv *env, jobject bluetoothGatt,
                                                 const char* uuid)
{
    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return CA_ADAPTER_NOT_ENABLED;
    }

    // get BluetoothGatt class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeSetCharacteristicNotification");
    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, CLASSPATH_BT_GATT);
    if (!jni_cid_BluetoothGatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return CA_STATUS_FAILED;
    }

    jstring jni_uuid = (*env)->NewStringUTF(env, uuid);
    jobject jni_obj_GattCharacteristic = CANativeGetGattService(env, bluetoothGatt, jni_uuid);
    if (!jni_obj_GattCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_obj_GattCharacteristic is null");
        return CA_STATUS_FAILED;
    }

    // set Characteristic Notification
    jmethodID jni_mid_setNotification = (*env)->GetMethodID(
            env, jni_cid_BluetoothGatt, "setCharacteristicNotification",
            "(Landroid/bluetooth/BluetoothGattCharacteristic;Z)Z");
    if (!jni_mid_setNotification)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_mid_getService is null");
        return CA_STATUS_FAILED;
    }

    jboolean enable = TRUE;
    jboolean ret = (*env)->CallBooleanMethod(env, bluetoothGatt, jni_mid_setNotification,
                                             jni_obj_GattCharacteristic, enable);
    if (1 == ret)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] CALL API - setCharacteristicNotification is success");
        return CA_STATUS_OK;
    }
    else
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] CALL API - setCharacteristicNotification is failed");
        return CA_STATUS_FAILED;
    }
}

jobject CANativeGetGattService(JNIEnv *env, jobject bluetoothGatt, jstring characterUUID)
{
    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return NULL;
    }

    // get BluetoothGatt class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeGetGattService");
    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, CLASSPATH_BT_GATT);
    if (!jni_cid_BluetoothGatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return NULL;
    }

    jmethodID jni_mid_getService = (*env)->GetMethodID(
            env, jni_cid_BluetoothGatt, "getService",
            "(Ljava/util/UUID;)Landroid/bluetooth/BluetoothGattService;");
    if (!jni_mid_getService)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_mid_getService is null");
        return NULL;
    }

    jobject jni_obj_service_uuid = CANativeGetUUIDObject(env, IOTIVITY_GATT_SERVIE_UUID);
    if (!jni_obj_service_uuid)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_obj_service_uuid is null");
        return NULL;
    }

    // get bluetooth gatt service
    OIC_LOG(DEBUG, TAG, "[BLE][Native] request to get service");
    jobject jni_obj_gattService = (*env)->CallObjectMethod(env, bluetoothGatt, jni_mid_getService,
                                                           jni_obj_service_uuid);

    // get bluetooth gatt service class
    jclass jni_cid_BluetoothGattService = (*env)->FindClass(
            env, "android/bluetooth/BluetoothGattService");
    if (!jni_cid_BluetoothGattService)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_BluetoothGattService is null");
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] get gatt getCharacteristic method");
    jmethodID jni_mid_getCharacteristic = (*env)->GetMethodID(
            env, jni_cid_BluetoothGattService, "getCharacteristic",
            "(Ljava/util/UUID;)Landroid/bluetooth/BluetoothGattCharacteristic;");
    if (!jni_mid_getCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_mid_getCharacteristic is null");
        return NULL;
    }

    const char* uuid = (*env)->GetStringUTFChars(env, characterUUID, NULL);
    jobject jni_obj_tx_uuid = CANativeGetUUIDObject(env, uuid);
    if (!jni_obj_tx_uuid)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_obj_tx_uuid is null");
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] request to get Characteristic");
    jobject jni_obj_GattCharacteristic = (*env)->CallObjectMethod(env, jni_obj_gattService,
                                                                  jni_mid_getCharacteristic,
                                                                  jni_obj_tx_uuid);

    return jni_obj_GattCharacteristic;
}

jobject CANativeCreateGattCharacteristic(JNIEnv *env, jobject bluetoothGatt, jbyteArray data)
{
    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeCreateGattCharacteristic");
    jstring jni_uuid = (*env)->NewStringUTF(env, IOTIVITY_GATT_TX_UUID);
    jobject jni_obj_GattCharacteristic = CANativeGetGattService(env, bluetoothGatt, jni_uuid);
    if (!jni_obj_GattCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_obj_GattCharacteristic is null");
        return NULL;
    }

    jclass jni_cid_BTGattCharacteristic = (*env)->FindClass(
            env, "android/bluetooth/BluetoothGattCharacteristic");
    if (!jni_cid_BTGattCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_BTGattCharacteristic is null");
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] set value in Characteristic");
    jmethodID jni_mid_setValue = (*env)->GetMethodID(env, jni_cid_BTGattCharacteristic, "setValue",
                                                     "([B)Z");
    if (!jni_mid_setValue)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_mid_setValue is null");
        return NULL;
    }

    jboolean ret = (*env)->CallBooleanMethod(env, jni_obj_GattCharacteristic, jni_mid_setValue,
                                             data);
    if (1 == ret)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] the locally stored value has been set");
        return jni_obj_GattCharacteristic;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] the locally stored value hasn't been set");
        return NULL;
    }
}

jbyteArray CANativeGetValueFromCharacteristic(JNIEnv *env, jobject characteristic)
{
    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] BT adpater is not enable");
        return NULL;
    }

    jclass jni_cid_BTGattCharacteristic = (*env)->FindClass(
            env, "android/bluetooth/BluetoothGattCharacteristic");
    if (!jni_cid_BTGattCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_BTGattCharacteristic is null");
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] get value in Characteristic");
    jmethodID jni_mid_getValue = (*env)->GetMethodID(env, jni_cid_BTGattCharacteristic, "getValue",
                                                     "()[B");
    if (!jni_mid_getValue)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_mid_getValue is null");
        return NULL;
    }

    jbyteArray jni_obj_data_array = (*env)->CallObjectMethod(env, characteristic, jni_mid_getValue);
    return jni_obj_data_array;
}

void CANativeCreateUUIDList()
{
    jboolean isAttached = FALSE;
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
        isAttached = TRUE;
    }

    // create new object array
    jclass jni_cid_uuid_list = (*env)->FindClass(env, CLASSPATH_BT_UUID);
    if (!jni_cid_uuid_list)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_cid_uuid_list is null");
        return;
    }

    jobjectArray jni_obj_uuid_list = (jobjectArray)(*env)->NewObjectArray(env, 1, jni_cid_uuid_list,
    NULL);
    if (!jni_obj_uuid_list)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_obj_uuid_list is null");
        return;
    }

    // remove previous list and create list again
    CANativeRemoveAllDevices(env);
    CANativeCreateScanDeviceList(env);

    // make uuid list
    jobject jni_obj_uuid = CANativeGetUUIDObject(env, IOTIVITY_GATT_SERVIE_UUID);
    (*env)->SetObjectArrayElement(env, jni_obj_uuid_list, 0, jni_obj_uuid);

    g_UUIDList = (jobjectArray)(*env)->NewGlobalRef(env, jni_obj_uuid_list);

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
}

void CANativeCreateScanDeviceList(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeCreateScanDeviceList");

    // create new object array
    if (g_deviceList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Create device list");

        g_deviceList = u_arraylist_create();
    }
}

void CANativeAddScanDeviceToList(JNIEnv *env, jobject device)
{
    if (!device)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] device is null");
        return;
    }

    if (!g_deviceList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] gdevice_list is null");
        return;
    }

    jstring jni_remoteAddress = CALEGetAddressFromBTDevice(env, device);
    if (!jni_remoteAddress)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_remoteAddress is null");
        return;
    }

    const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

    if (!CANativeIsDeviceInList(env, remoteAddress))
    {
        jobject gdevice = (*env)->NewGlobalRef(env, device);
        u_arraylist_add(g_deviceList, gdevice);
        OIC_LOG(DEBUG, TAG, "Set Object to Array as Element");
    }
}

jboolean CANativeIsDeviceInList(JNIEnv *env, const char* remoteAddress)
{
    // get address value from device list

    jint index;
    for (index = 0; index < u_arraylist_length(g_deviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jarrayObj is null");
            return TRUE;
        }

        jstring jni_setAddress = CALEGetAddressFromBTDevice(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jni_setAddress is null");
            return TRUE;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);

        if (!strcmp(remoteAddress, setAddress))
        {
            OIC_LOG(DEBUG, TAG, "the device is already set");
            return TRUE;
        }
    }

    OIC_LOG(DEBUG, TAG, "there are no the device in list. we can add");
    return FALSE;
}

void CANativeRemoveAllDevices(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CANativeRemoveAllDevices");

    if (!g_deviceList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] g_deviceList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(g_deviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jarrayObj is null");
            continue;
        }
        (*env)->DeleteGlobalRef(env, jarrayObj);
    }

    OICFree(g_deviceList);
    g_deviceList = NULL;
    return;
}

void CANativeRemoveDevice(JNIEnv *env, jstring address)
{
    OIC_LOG(DEBUG, TAG, "CANativeRemoveDevice");

    if (!g_deviceList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] g_deviceList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(g_deviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jarrayObj is null");
            return;
        }

        jstring jni_setAddress = CALEGetAddressFromBTDevice(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jni_setAddress is null");
            return;
        }
        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        const char* remoteAddress = (*env)->GetStringUTFChars(env, address, NULL);

        if (!strcmp(setAddress, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "[BLE][Native] remove object : %s", remoteAddress);
            (*env)->DeleteGlobalRef(env, jarrayObj);

            CAReorderingDeviceList(index);
            return;
        }
    }OIC_LOG(DEBUG, TAG, "[BLE][Native] there are no target object");
    return;
}

void CAReorderingDeviceList(uint32_t index)
{
    if (index >= g_deviceList->length)
    {
        return;
    }

    if (index < g_deviceList->length - 1)
    {
        memmove(&g_deviceList->data[index], &g_deviceList->data[index + 1],
                (g_deviceList->length - index - 1) * sizeof(void *));
    }

    g_deviceList->size--;
    g_deviceList->length--;
}

/**
 * Gatt Object List
 */
void CANativeCreateGattObjList(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeCreateGattObjList");

    // create new object array
    if (g_gattObjectList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Create Gatt object list");

        g_gattObjectList = u_arraylist_create();
    }
}

void CANativeAddGattobjToList(JNIEnv *env, jobject gatt)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeAddGattobjToList");

    if (!gatt)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] gatt is null");
        return;
    }

    if (!g_gattObjectList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] g_gattObjectList is null");
        return;
    }

    jstring jni_remoteAddress = CANativeGetAddressFromGattObj(env, gatt);
    if (!jni_remoteAddress)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_remoteAddress is null");
        return;
    }

    const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

    if (!CANativeIsGattObjInList(env, remoteAddress))
    {
        jobject gGatt = (*env)->NewGlobalRef(env, gatt);
        u_arraylist_add(g_gattObjectList, gGatt);
        OIC_LOG(DEBUG, TAG, "Set Object to Array as Element");
    }
}

jboolean CANativeIsGattObjInList(JNIEnv *env, const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeIsGattObjInList");

    jint index;
    for (index = 0; index < u_arraylist_length(g_gattObjectList); index++)
    {

        jobject jarrayObj = (jobject) u_arraylist_get(g_gattObjectList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jarrayObj is null");
            return TRUE;
        }

        jstring jni_setAddress = CANativeGetAddressFromGattObj(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jni_setAddress is null");
            return TRUE;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);

        if (!strcmp(remoteAddress, setAddress))
        {
            OIC_LOG(DEBUG, TAG, "the device is already set");
            return TRUE;
        }
        else
        {
            continue;
        }
    }

    OIC_LOG(DEBUG, TAG, "there are no the gatt obejct in list. we can add");
    return FALSE;
}

void CANativeRemoveAllGattObjsList(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CANativeRemoveAllGattObjsList");

    if (!g_gattObjectList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] g_gattObjectList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(g_gattObjectList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_gattObjectList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jarrayObj is null");
            continue;
        }
        (*env)->DeleteGlobalRef(env, jarrayObj);
    }

    OICFree(g_gattObjectList);
    g_gattObjectList = NULL;
    return;
}

void CANativeRemoveGattObj(JNIEnv *env, jobject gatt)
{
    OIC_LOG(DEBUG, TAG, "CANativeRemoveGattObj");

    if (!g_gattObjectList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] g_gattObjectList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(g_gattObjectList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_gattObjectList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jarrayObj is null");
            return;
        }

        jstring jni_setAddress = CANativeGetAddressFromGattObj(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jni_setAddress is null");
            return;
        }
        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);

        jstring jni_remoteAddress = CANativeGetAddressFromGattObj(env, gatt);
        if (!jni_remoteAddress)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jni_remoteAddress is null");
            return;
        }
        const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

        if (!strcmp(setAddress, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "[BLE][Native] remove object : %s", remoteAddress);
            (*env)->DeleteGlobalRef(env, jarrayObj);

            CAReorderingGattList(index);
            return;
        }
    }OIC_LOG(DEBUG, TAG, "[BLE][Native] there are no target object");
    return;
}

void CAReorderingGattList(uint32_t index)
{
    if (index >= g_gattObjectList->length)
    {
        return;
    }

    if (index < g_gattObjectList->length - 1)
    {
        memmove(&g_gattObjectList->data[index], &g_gattObjectList->data[index + 1],
                (g_gattObjectList->length - index - 1) * sizeof(void *));
    }

    g_gattObjectList->size--;
    g_gattObjectList->length--;
}

/**
 * Check Sent Count for remove g_sendBuffer
 */
void CANativeupdateSendCnt(JNIEnv *env)
{
    // mutex lock
    ca_mutex_lock(g_threadMutex);

    g_currentSentCnt++;

    if (g_targetCnt <= g_currentSentCnt)
    {
        g_targetCnt = 0;
        g_currentSentCnt = 0;

        if (g_sendBuffer)
        {
            (*env)->DeleteGlobalRef(env, g_sendBuffer);
            g_sendBuffer = NULL;
        }
        // notity the thread
        ca_cond_signal(g_threadCond);
        g_isFinishSendData = TRUE;
        OIC_LOG(DEBUG, TAG, "set signal for send data");
    }
    // mutex unlock
    ca_mutex_unlock(g_threadMutex);
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CARegisterLeScanCallback(JNIEnv *env, jobject obj,
                                                             jobject callback)
{
    OIC_LOG(DEBUG, TAG, "CARegisterLeScanCallback");

    g_leScanCallback = (*env)->NewGlobalRef(env, callback);
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CARegisterLeGattCallback(JNIEnv *env, jobject obj,
                                                             jobject callback)
{
    OIC_LOG(DEBUG, TAG, "CARegisterLeGattCallback");

    g_leGattCallback = (*env)->NewGlobalRef(env, callback);
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CALeScanCallback(JNIEnv *env, jobject obj, jobject device,
                                                     jint rssi, jbyteArray scanRecord)
{
    CANativeAddScanDeviceToList(env, device);
}

/*
 * Class:     com_iotivity_jar_caleinterface
 * Method:    CALeGattConnectionStateChangeCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;II)V
 */
JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CALeGattConnectionStateChangeCallback(JNIEnv *env, jobject obj,
                                                                          jobject gatt, jint status,
                                                                          jint newstate)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattConnectionStateChangeCallback - status %d, newstate %d", status,
            newstate);

    if (GATT_SUCCESS == status && STATE_CONNECTED == newstate) // le connected
    {
        if (gatt)
        {
            CANativeAddGattobjToList(env, gatt);
            CANativeLEDiscoverServices(env, gatt);
        }
    }
    else if (GATT_SUCCESS == status && STATE_DISCONNECTED == newstate) // le disconnected
    {

    }
    else // other error
    {
        CANativeSendFinish(env, gatt);
    }
}

/*
 * Class:     com_iotivity_jar_caleinterface
 * Method:    CALeGattServicesDiscoveredCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;I)V
 */
JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CALeGattServicesDiscoveredCallback(JNIEnv *env, jobject obj,
                                                                       jobject gatt, jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattServicesDiscoveredCallback - status %d: ", status);

    if (0 != status) // discovery error
    {
        CANativeSendFinish(env, gatt);
        return;
    }

    // read Characteristic
//    CANativeReadCharacteristic(env, gatt);

    CAResult_t ret = CANativeSetCharacteristicNotification(env, gatt, IOTIVITY_GATT_RX_UUID);
    if (CA_STATUS_OK != ret) // SetCharacteristicNoti is failed
    {
        CANativeSendFinish(env, gatt);
        return;
    }

//        jstring data = (*env)->NewStringUTF(env, "HelloWorld");
    jobject jni_obj_character = CANativeCreateGattCharacteristic(env, gatt, g_sendBuffer);
    if (!jni_obj_character)
    {
        CANativeSendFinish(env, gatt);
        return;
    }

    sleep(1);
    ret = CANativeWriteCharacteristic(env, gatt, jni_obj_character);
    if (CA_STATUS_FAILED == ret)
    {
        CANativeSendFinish(env, gatt);
        return;
    }
}

/*
 * Class:     com_iotivity_jar_caleinterface
 * Method:    CALeGattCharacteristicReadCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;I)V
 */
JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CALeGattCharacteristicReadCallback(JNIEnv *env, jobject obj,
                                                                       jobject gatt,
                                                                       jobject characteristic,
                                                                       jbyteArray data, jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicReadCallback - status : %d", status);
}

/*
 * Class:     com_iotivity_jar_caleinterface
 * Method:    CALeGattCharacteristicWritjclasseCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;I)V
 */
JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CALeGattCharacteristicWriteCallback(JNIEnv *env, jobject obj,
                                                                        jobject gatt,
                                                                        jobject characteristic,
                                                                        jbyteArray data,
                                                                        jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicWriteCallback - status : %d", status);

    jstring jni_address = CANativeGetAddressFromGattObj(env, gatt);
    const char* address = (*env)->GetStringUTFChars(env, jni_address, NULL);

    jboolean isCopy;
    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicWriteCallback - write data : %s",
            (char*)(*env)->GetByteArrayElements(env, data, &isCopy));

#ifdef DEBUG_MODE
    CANativeSendFinish(env, gatt);
#endif

    if (0 != status) // error case
    {
        CANativeSendFinish(env, gatt);
    }
}

/*
 * Class:     com_iotivity_jar_caleinterface
 * Method:    CALeGattCharacteristicChangedCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;)V
 */
JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CALeGattCharacteristicChangedCallback(JNIEnv *env, jobject obj,
                                                                          jobject gatt,
                                                                          jobject characteristic,
                                                                          jbyteArray data)
{
    OIC_LOG(DEBUG, TAG, "CALeGattCharacteristicChangedCallback");

    // get Byte Array and covert to char*
    jint length = (*env)->GetArrayLength(env, data);

    jboolean isCopy;
    jbyte *jni_byte_responseData = (jbyte*) (*env)->GetByteArrayElements(env, data, &isCopy);

    char* recevicedData = (char*) OICMalloc(sizeof(char) * length);
    if (NULL == recevicedData)
    {
        OIC_LOG(ERROR, TAG, "recevicedData is null");
        CANativeSendFinish(env, gatt);

        return;
    }

    memcpy(recevicedData, (const char*) jni_byte_responseData, length);
    recevicedData[length] = '\0';
    (*env)->ReleaseByteArrayElements(env, data, jni_byte_responseData, JNI_ABORT);

    jstring jni_address = CANativeGetAddressFromGattObj(env, gatt);
    const char* address = (*env)->GetStringUTFChars(env, jni_address, NULL);

    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicChangedCallback - data. : %s, %d",
            recevicedData, length);

    // callback
    g_packetReceiveCallback(address, recevicedData);
    (*env)->ReleaseStringUTFChars(env, jni_address, address);

    // LE disconnect and finish BLE write routine
    CANativeSendFinish(env, gatt);
}

/*
 * Class:     com_iotivity_jar_caleinterface
 * Method:    CALeGattDescriptorReadCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattDescriptor;I)V
 */
JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CALeGattDescriptorReadCallback(JNIEnv *env, jobject obj,
                                                                   jobject gatt, jobject descriptor,
                                                                   jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattDescriptorReadCallback - status %d: ", status);
}

/*
 * Class:     com_iotivity_jar_caleinterface
 * Method:    CALeGattDescriptorWriteCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattDescriptor;I)V
 */
JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CALeGattDescriptorWriteCallback(JNIEnv *env, jobject obj,
                                                                    jobject gatt,
                                                                    jobject descriptor, jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattDescriptorWriteCallback - status %d: ", status);
}

/*
 * Class:     com_iotivity_jar_caleinterface
 * Method:    CALeGattReliableWriteCompletedCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;I)V
 */
JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CALeGattReliableWriteCompletedCallback(JNIEnv *env, jobject obj,
                                                                           jobject gatt,
                                                                           jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattReliableWriteCompletedCallback - status %d: ", status);
}

/*
 * Class:     com_iotivity_jar_caleinterface
 * Method:    CALeGattReadRemoteRssiCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;II)V
 */
JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CALeGattReadRemoteRssiCallback(JNIEnv *env, jobject obj,
                                                                   jobject gatt, jint rssi,
                                                                   jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattReadRemoteRssiCallback - rssi %d,  status %d: ", rssi, status);
}
