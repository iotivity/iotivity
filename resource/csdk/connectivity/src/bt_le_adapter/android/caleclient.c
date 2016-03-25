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

#include "caleclient.h"
#include "caleserver.h"
#include "caleutils.h"
#include "caleinterface.h"
#include "caadapterutils.h"

#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "cathreadpool.h" /* for thread pool */
#include "camutex.h"
#include "uarraylist.h"
#include "org_iotivity_ca_CaLeClientInterface.h"

#define TAG PCF("OIC_CA_LE_CLIENT")

#define MICROSECS_PER_SEC 1000000
#define WAIT_TIME_WRITE_CHARACTERISTIC 10 * MICROSECS_PER_SEC

#define GATT_CONNECTION_PRIORITY_BALANCED   0
#define GATT_FAILURE                        257
#define GATT_INSUFFICIENT_AUTHENTICATION    5
#define GATT_INSUFFICIENT_ENCRYPTION        15
#define GATT_INVALID_ATTRIBUTE_LENGTH       13
#define GATT_INVALID_OFFSET                 7
#define GATT_READ_NOT_PERMITTED             2
#define GATT_REQUEST_NOT_SUPPORTED          6
#define GATT_WRITE_NOT_PERMITTED            3

static ca_thread_pool_t g_threadPoolHandle = NULL;

JavaVM *g_jvm;
static u_arraylist_t *g_deviceList = NULL; // device list to have same UUID
static u_arraylist_t *g_gattObjectList = NULL;
static u_arraylist_t *g_deviceStateList = NULL;

static CAPacketReceiveCallback g_packetReceiveCallback = NULL;
static CABLEErrorHandleCallback g_clientErrorCallback;
static jobject g_leScanCallback = NULL;
static jobject g_leGattCallback = NULL;
static jobject g_context = NULL;
static jobjectArray g_uuidList = NULL;

// it will be prevent to start send logic when adapter has stopped.
static bool g_isStartedLEClient = false;
static bool g_isStartedScan = false;

static jbyteArray g_sendBuffer = NULL;
static uint32_t g_targetCnt = 0;
static uint32_t g_currentSentCnt = 0;
static bool g_isFinishedSendData = false;
static ca_mutex g_SendFinishMutex = NULL;
static ca_mutex g_threadMutex = NULL;
static ca_cond g_threadCond = NULL;
static ca_cond g_deviceDescCond = NULL;

static ca_mutex g_threadSendMutex = NULL;
static ca_mutex g_threadWriteCharacteristicMutex = NULL;
static ca_cond g_threadWriteCharacteristicCond = NULL;
static bool g_isSignalSetFlag = false;

static ca_mutex g_bleReqRespClientCbMutex = NULL;
static ca_mutex g_bleServerBDAddressMutex = NULL;

static ca_mutex g_deviceListMutex = NULL;
static ca_mutex g_gattObjectMutex = NULL;
static ca_mutex g_deviceStateListMutex = NULL;

static ca_mutex g_deviceScanRetryDelayMutex = NULL;
static ca_cond g_deviceScanRetryDelayCond = NULL;

static ca_mutex g_scanMutex = NULL;

static CABLEDataReceivedCallback g_CABLEClientDataReceivedCallback = NULL;

/**
 * check if retry logic for connection routine has to be stopped or not.
 * in case of error value including this method, connection routine has to be stopped.
 * since there is no retry logic for this error reason in this client.
 * @param state constant value of bluetoothgatt.
 * @return true - waiting for background connection in BT platform.
 *         false - connection routine has to be stopped.
 */
static bool CALECheckConnectionStateValue(jint state)
{
    switch(state)
    {
        case GATT_CONNECTION_PRIORITY_BALANCED:
        case GATT_FAILURE:
        case GATT_INSUFFICIENT_AUTHENTICATION:
        case GATT_INSUFFICIENT_ENCRYPTION:
        case GATT_INVALID_ATTRIBUTE_LENGTH:
        case GATT_INVALID_OFFSET:
        case GATT_READ_NOT_PERMITTED:
        case GATT_REQUEST_NOT_SUPPORTED:
        case GATT_WRITE_NOT_PERMITTED:
            return true;
    }
    return false;
}

//getting jvm
void CALEClientJniInit()
{
    OIC_LOG(DEBUG, TAG, "CALEClientJniInit");
    g_jvm = (JavaVM*) CANativeJNIGetJavaVM();
}

void CALEClientJNISetContext()
{
    OIC_LOG(DEBUG, TAG, "CALEClientJNISetContext");
    g_context = (jobject) CANativeJNIGetContext();
}

CAResult_t CALECreateJniInterfaceObject()
{
    OIC_LOG(DEBUG, TAG, "CALECreateJniInterfaceObject");

    if (!g_context)
    {
        OIC_LOG(ERROR, TAG, "g_context is null");
        return CA_STATUS_FAILED;
    }

    if (!g_jvm)
    {
        OIC_LOG(ERROR, TAG, "g_jvm is null");
        return CA_STATUS_FAILED;
    }

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    jclass jni_LEInterface = (*env)->FindClass(env, "org/iotivity/ca/CaLeClientInterface");
    if (!jni_LEInterface)
    {
        OIC_LOG(ERROR, TAG, "Could not get CaLeClientInterface class");
        goto error_exit;
    }

    jmethodID LeInterfaceConstructorMethod = (*env)->GetMethodID(env, jni_LEInterface, "<init>",
                                                                 "(Landroid/content/Context;)V");
    if (!LeInterfaceConstructorMethod)
    {
        OIC_LOG(ERROR, TAG, "Could not get CaLeClientInterface constructor method");
        goto error_exit;
    }

    (*env)->NewObject(env, jni_LEInterface, LeInterfaceConstructorMethod, g_context);
    OIC_LOG(DEBUG, TAG, "Create instance for CaLeClientInterface");

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;

error_exit:

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_FAILED;
}

CAResult_t CALEClientInitialize()
{
    OIC_LOG(DEBUG, TAG, "CALEClientInitialize");

    CALEClientJniInit();

    if (!g_jvm)
    {
        OIC_LOG(ERROR, TAG, "g_jvm is null");
        return CA_STATUS_FAILED;
    }

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    CAResult_t ret = CALECheckPlatformVersion(env, 18);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "it is not supported");

        if (isAttached)
        {
            (*g_jvm)->DetachCurrentThread(g_jvm);
        }

        return ret;
    }

    ret = CALEClientInitGattMutexVaraibles();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALEClientInitGattMutexVaraibles has failed!");
        CALEClientTerminateGattMutexVariables();

        if (isAttached)
        {
            (*g_jvm)->DetachCurrentThread(g_jvm);
        }

        return ret;
    }

    g_deviceDescCond = ca_cond_new();

    // init mutex for send logic
    g_threadCond = ca_cond_new();
    g_threadWriteCharacteristicCond = ca_cond_new();
    g_deviceScanRetryDelayCond = ca_cond_new();

    CALEClientCreateDeviceList();
    CALEClientJNISetContext();

    ret = CALEClientCreateUUIDList();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALEClientCreateUUIDList has failed");

        if (isAttached)
        {
            (*g_jvm)->DetachCurrentThread(g_jvm);
        }

        return ret;
    }

    ret = CALECreateJniInterfaceObject(); /* create java caleinterface instance*/
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALECreateJniInterfaceObject has failed");

        if (isAttached)
        {
            (*g_jvm)->DetachCurrentThread(g_jvm);
        }

        return ret;
    }
    g_isStartedLEClient = true;

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;
}

void CALEClientTerminate()
{
    OIC_LOG(DEBUG, TAG, "CALEClientTerminate");

    if (!g_jvm)
    {
        OIC_LOG(ERROR, TAG, "g_jvm is null");
        return;
    }

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return;
        }
        isAttached = true;
    }

    if (g_leScanCallback)
    {
        (*env)->DeleteGlobalRef(env, g_leScanCallback);
        g_leScanCallback = NULL;
    }

    if (g_leGattCallback)
    {
        (*env)->DeleteGlobalRef(env, g_leGattCallback);
        g_leGattCallback = NULL;
    }

    if (g_sendBuffer)
    {
        (*env)->DeleteGlobalRef(env, g_sendBuffer);
        g_sendBuffer = NULL;
    }

    if (g_uuidList)
    {
        (*env)->DeleteGlobalRef(env, g_uuidList);
        g_uuidList = NULL;
    }

    CAResult_t ret = CALEClientRemoveAllDeviceState();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALEClientRemoveAllDeviceState has failed");
    }

    ret = CALEClientRemoveAllScanDevices(env);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALEClientRemoveAllScanDevices has failed");
    }

    ret = CALEClientRemoveAllGattObjs(env);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALEClientRemoveAllGattObjs has failed");
    }

    CALEClientSetScanFlag(false);
    CALEClientSetSendFinishFlag(true);

    CALEClientTerminateGattMutexVariables();
    CALEClientDestroyJniInterface();

    ca_cond_free(g_deviceDescCond);
    ca_cond_free(g_threadCond);
    ca_cond_free(g_threadWriteCharacteristicCond);
    ca_cond_free(g_deviceScanRetryDelayCond);

    g_deviceDescCond = NULL;
    g_threadCond = NULL;
    g_threadWriteCharacteristicCond = NULL;
    g_deviceScanRetryDelayCond = NULL;

    g_isSignalSetFlag = false;

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
}

CAResult_t CALEClientDestroyJniInterface()
{
    OIC_LOG(DEBUG, TAG, "CALEClientDestroyJniInterface");

    if (!g_jvm)
    {
        OIC_LOG(ERROR, TAG, "g_jvm is null");
        return CA_STATUS_FAILED;
    }

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    jclass jni_LeInterface = (*env)->FindClass(env, "org/iotivity/ca/CaLeClientInterface");
    if (!jni_LeInterface)
    {
        OIC_LOG(ERROR, TAG, "Could not get CaLeClientInterface class");
        goto error_exit;
    }

    jmethodID jni_InterfaceDestroyMethod = (*env)->GetStaticMethodID(env, jni_LeInterface,
                                                                     "destroyLeInterface",
                                                                     "()V");
    if (!jni_InterfaceDestroyMethod)
    {
        OIC_LOG(ERROR, TAG, "Could not get CaLeClientInterface destroy method");
        goto error_exit;
    }

    (*env)->CallStaticVoidMethod(env, jni_LeInterface, jni_InterfaceDestroyMethod);

    if ((*env)->ExceptionCheck(env))
    {
        OIC_LOG(ERROR, TAG, "destroyLeInterface has failed");
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        goto error_exit;
    }

    OIC_LOG(DEBUG, TAG, "Destroy instance for CaLeClientInterface");

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;

error_exit:

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_FAILED;
}

void CALEClientSendFinish(JNIEnv *env, jobject gatt)
{
    OIC_LOG(DEBUG, TAG, "CALEClientSendFinish");
    VERIFY_NON_NULL_VOID(env, TAG, "env is null");

    if (gatt)
    {
        CAResult_t res = CALEClientDisconnect(env, gatt);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CALEClientDisconnect has failed");
        }
    }
    CALEClientUpdateSendCnt(env);
}

CAResult_t CALEClientSendUnicastMessage(const char* address,
                                        const uint8_t* data,
                                        const uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALEClientSendUnicastMessage(%s, %p)", address, data);
    VERIFY_NON_NULL(address, TAG, "address is null");
    VERIFY_NON_NULL(data, TAG, "data is null");

    return CALEClientSendUnicastMessageImpl(address, data, dataLen);
}

CAResult_t CALEClientSendMulticastMessage(const uint8_t* data,
                                          const uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALEClientSendMulticastMessage(%p)", data);
    VERIFY_NON_NULL(data, TAG, "data is null");

    if (!g_jvm)
    {
        OIC_LOG(ERROR, TAG, "g_jvm is null");
        return CA_STATUS_FAILED;
    }

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    CAResult_t ret = CALEClientSendMulticastMessageImpl(env, data, dataLen);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALEClientSendMulticastMessageImpl has failed");
    }

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return ret;
}

CAResult_t CALEClientStartUnicastServer(const char* address)
{
    OIC_LOG_V(DEBUG, TAG, "it is not needed in this platform (%s)", address);

    return CA_NOT_SUPPORTED;
}

CAResult_t CALEClientStartMulticastServer()
{
    OIC_LOG(DEBUG, TAG, "it is not needed in this platform");

    return CA_NOT_SUPPORTED;
}

void CALEClientStopUnicastServer()
{
    OIC_LOG(DEBUG, TAG, "CALEClientStopUnicastServer");
}

void CALEClientStopMulticastServer()
{
    OIC_LOG(DEBUG, TAG, "CALEClientStopMulticastServer");
}

void CALEClientSetCallback(CAPacketReceiveCallback callback)
{
    g_packetReceiveCallback = callback;
}

void CASetBLEClientErrorHandleCallback(CABLEErrorHandleCallback callback)
{
    g_clientErrorCallback = callback;
}

CAResult_t CALEClientIsThereScannedDevices(JNIEnv *env, const char* address)
{
    VERIFY_NON_NULL(env, TAG, "env");

    if (!g_deviceList)
    {
        return CA_STATUS_FAILED;
    }

    if (0 == u_arraylist_length(g_deviceList) // multicast
            || (address && !CALEClientIsDeviceInScanDeviceList(env, address))) // unicast
    {
        // Wait for LE peripherals to be discovered.

        // Number of times to wait for discovery to complete.
        static size_t const RETRIES = 5;

        static uint64_t const TIMEOUT =
            2 * MICROSECS_PER_SEC;  // Microseconds

        bool devicesDiscovered = false;
        for (size_t i = 0; i < RETRIES; ++i)
        {
            OIC_LOG(DEBUG, TAG, "waiting for target device");
            if (ca_cond_wait_for(g_deviceDescCond,
                                 g_threadSendMutex,
                                 TIMEOUT) == CA_WAIT_SUCCESS)
            {
                ca_mutex_lock(g_deviceListMutex);
                size_t scannedDeviceLen = u_arraylist_length(g_deviceList);
                ca_mutex_unlock(g_deviceListMutex);

                if (0 < scannedDeviceLen)
                {
                    if (!address  // multicast
                        || (address && CALEClientIsDeviceInScanDeviceList(env, address))) // unicast
                    {
                      devicesDiscovered = true;
                      break;
                    }
                    else
                    {
                        if (address)
                        {
                            OIC_LOG(INFO, TAG, "waiting..");

                            ca_mutex_lock(g_deviceScanRetryDelayMutex);
                            if (ca_cond_wait_for(g_deviceScanRetryDelayCond,
                                                 g_deviceScanRetryDelayMutex,
                                                 MICROSECS_PER_SEC) == CA_WAIT_SUCCESS)
                            {
                                OIC_LOG(INFO, TAG, "finish to waiting for target device");
                                ca_mutex_unlock(g_deviceScanRetryDelayMutex);
                                break;
                            }
                            ca_mutex_unlock(g_deviceScanRetryDelayMutex);
                            // time out

                            // checking whether a target device is found while waiting for time-out.
                            if (CALEClientIsDeviceInScanDeviceList(env, address))
                            {
                                devicesDiscovered = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        // time out for scanning devices
        if (!devicesDiscovered)
        {
            return CA_STATUS_FAILED;
        }
    }

    return CA_STATUS_OK;
}


CAResult_t CALEClientSendUnicastMessageImpl(const char* address, const uint8_t* data,
                                      const uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALEClientSendUnicastMessageImpl, address: %s, data: %p", address,
              data);
    VERIFY_NON_NULL(address, TAG, "address is null");
    VERIFY_NON_NULL(data, TAG, "data is null");

    if (!g_jvm)
    {
        OIC_LOG(ERROR, TAG, "g_jvm is null");
        return CA_STATUS_FAILED;
    }

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    ca_mutex_lock(g_threadSendMutex);

    CALEClientSetSendFinishFlag(false);

    CAResult_t ret = CALEClientIsThereScannedDevices(env, address);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(INFO, TAG, "there is no scanned device");
        goto error_exit;
    }

    if (g_context && g_deviceList)
    {
        uint32_t length = u_arraylist_length(g_deviceList);
        for (uint32_t index = 0; index < length; index++)
        {
            jobject jarrayObj = (jobject) u_arraylist_get(g_deviceList, index);
            if (!jarrayObj)
            {
                OIC_LOG(ERROR, TAG, "jarrayObj is null");
                goto error_exit;
            }

            jstring jni_setAddress = CALEGetAddressFromBTDevice(env, jarrayObj);
            if (!jni_setAddress)
            {
                OIC_LOG(ERROR, TAG, "jni_setAddress is null");
                goto error_exit;
            }

            const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
            if (!setAddress)
            {
                OIC_LOG(ERROR, TAG, "setAddress is null");
                goto error_exit;
            }

            OIC_LOG_V(DEBUG, TAG, "remote device address is %s", setAddress);

            if (!strcmp(setAddress, address))
            {
                (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);

                // connect to gatt server
                ret = CALEClientStopScan();
                if (CA_STATUS_OK != ret)
                {
                    OIC_LOG(ERROR, TAG, "CALEClientStopScan has failed");
                    goto error_exit;
                }

                if (g_sendBuffer)
                {
                    (*env)->DeleteGlobalRef(env, g_sendBuffer);
                    g_sendBuffer = NULL;
                }
                jbyteArray jni_arr = (*env)->NewByteArray(env, dataLen);
                (*env)->SetByteArrayRegion(env, jni_arr, 0, dataLen, (jbyte*) data);
                g_sendBuffer = (jbyteArray)(*env)->NewGlobalRef(env, jni_arr);

                // Target device to send message is just one.
                g_targetCnt = 1;

                ret = CALEClientSendData(env, jarrayObj);
                if (CA_STATUS_OK != ret)
                {
                    OIC_LOG(ERROR, TAG, "CALEClientSendData in unicast is failed");
                    goto error_exit;
                }

                OIC_LOG(INFO, TAG, "wake up");
                break;
            }
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
        }
    }

    OIC_LOG(DEBUG, TAG, "connection routine is finished for unicast");

    // wait for finish to send data through "CALeGattServicesDiscoveredCallback"
    // if there is no connection state.
    ca_mutex_lock(g_threadMutex);
    if (!g_isFinishedSendData)
    {
        OIC_LOG(DEBUG, TAG, "waiting send finish signal");
        ca_cond_wait(g_threadCond, g_threadMutex);
        OIC_LOG(DEBUG, TAG, "the data was sent");
    }
    ca_mutex_unlock(g_threadMutex);

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    // start LE Scan again
    ret = CALEClientStartScan();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALEClientStartScan has failed");
        ca_mutex_unlock(g_threadSendMutex);
        return res;
    }

    ca_mutex_unlock(g_threadSendMutex);
    OIC_LOG(INFO, TAG, "unicast - send logic has finished");
    return CALECheckSendState(address);

    // error label.
error_exit:

    // start LE Scan again
    ret = CALEClientStartScan();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALEClientStartScan has failed");
        ca_mutex_unlock(g_threadSendMutex);
        if (isAttached)
        {
            (*g_jvm)->DetachCurrentThread(g_jvm);
        }
        return res;
    }

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    if (g_clientErrorCallback)
    {
        g_clientErrorCallback(address, data, dataLen, CA_SEND_FAILED);
    }
    ca_mutex_unlock(g_threadSendMutex);
    return CA_SEND_FAILED;
}

CAResult_t CALEClientSendMulticastMessageImpl(JNIEnv *env, const uint8_t* data,
                                              const uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CASendMulticastMessageImpl, send to, data: %p, %u", data, dataLen);
    VERIFY_NON_NULL(data, TAG, "data is null");
    VERIFY_NON_NULL(env, TAG, "env is null");

    if (!g_deviceList)
    {
        OIC_LOG(ERROR, TAG, "g_deviceList is null");
        return CA_STATUS_FAILED;
    }

    ca_mutex_lock(g_threadSendMutex);

    CALEClientSetSendFinishFlag(false);

    OIC_LOG(DEBUG, TAG, "set byteArray for data");
    if (g_sendBuffer)
    {
        (*env)->DeleteGlobalRef(env, g_sendBuffer);
        g_sendBuffer = NULL;
    }

    CAResult_t res = CALEClientIsThereScannedDevices(env, NULL);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(INFO, TAG, "there is no scanned device");
        goto error_exit;
    }

    // connect to gatt server
    res = CALEClientStopScan();
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "CALEClientStopScan has failed");
        ca_mutex_unlock(g_threadSendMutex);
        return res;
    }
    uint32_t length = u_arraylist_length(g_deviceList);
    g_targetCnt = length;

    jbyteArray jni_arr = (*env)->NewByteArray(env, dataLen);
    (*env)->SetByteArrayRegion(env, jni_arr, 0, dataLen, (jbyte*) data);
    g_sendBuffer = (jbyteArray)(*env)->NewGlobalRef(env, jni_arr);

    for (uint32_t index = 0; index < length; index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is not available");
            continue;
        }

        res = CALEClientSendData(env, jarrayObj);
        if (res != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "BT device - send has failed");
        }

        jstring jni_address = CALEGetAddressFromBTDevice(env, jarrayObj);
        if (!jni_address)
        {
            OIC_LOG(ERROR, TAG, "CALEGetAddressFromBTDevice has failed");
            continue;
        }

        const char* address = (*env)->GetStringUTFChars(env, jni_address, NULL);
        if (!address)
        {
            OIC_LOG(ERROR, TAG, "address is not available");
            continue;
        }

        (*env)->ReleaseStringUTFChars(env, jni_address, address);
    }

    OIC_LOG(DEBUG, TAG, "connection routine is finished for multicast");

    // wait for finish to send data through "CALeGattServicesDiscoveredCallback"
    ca_mutex_lock(g_threadMutex);
    if (!g_isFinishedSendData)
    {
        OIC_LOG(DEBUG, TAG, "waiting send finish signal");
        ca_cond_wait(g_threadCond, g_threadMutex);
        OIC_LOG(DEBUG, TAG, "the data was sent");
    }
    ca_mutex_unlock(g_threadMutex);

    // start LE Scan again
    res = CALEClientStartScan();
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "CALEClientStartScan has failed");
        ca_mutex_unlock(g_threadSendMutex);
        return res;
    }

    ca_mutex_unlock(g_threadSendMutex);
    OIC_LOG(DEBUG, TAG, "OUT - CALEClientSendMulticastMessageImpl");
    return CA_STATUS_OK;

error_exit:
    res = CALEClientStartScan();
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "CALEClientStartScan has failed");
        ca_mutex_unlock(g_threadSendMutex);
        return res;
    }

    ca_mutex_unlock(g_threadSendMutex);
    OIC_LOG(DEBUG, TAG, "OUT - CALEClientSendMulticastMessageImpl");
    return CA_SEND_FAILED;
}

CAResult_t CALECheckSendState(const char* address)
{
    VERIFY_NON_NULL(address, TAG, "address is null");

    ca_mutex_lock(g_deviceStateListMutex);
    CALEState_t* state = CALEClientGetStateInfo(address);
    if (NULL == state)
    {
        OIC_LOG(ERROR, TAG, "state is null");
        ca_mutex_unlock(g_deviceStateListMutex);
        return CA_SEND_FAILED;
    }

    if (STATE_SEND_SUCCESS != state->sendState)
    {
        OIC_LOG(ERROR, TAG, "sendstate is not STATE_SEND_SUCCESS");
        ca_mutex_unlock(g_deviceStateListMutex);
        return CA_SEND_FAILED;
    }

    OIC_LOG(INFO, TAG, "sendstate is STATE_SEND_SUCCESS");
    ca_mutex_unlock(g_deviceStateListMutex);
    return CA_STATUS_OK;
}

CAResult_t CALEClientSendData(JNIEnv *env, jobject device)
{
    OIC_LOG(DEBUG, TAG, "IN - CALEClientSendData");
    VERIFY_NON_NULL(device, TAG, "device is null");
    VERIFY_NON_NULL(env, TAG, "env is null");

    // get BLE address from bluetooth device object.
    char* address = NULL;
    CALEState_t* state = NULL;
    jstring jni_address = CALEClientGetLEAddressFromBTDevice(env, device);
    if (jni_address)
    {
        OIC_LOG(INFO, TAG, "there is gatt object..it's not first connection");
        address = (char*)(*env)->GetStringUTFChars(env, jni_address, NULL);
        if (!address)
        {
            OIC_LOG(ERROR, TAG, "address is not available");
            return CA_STATUS_FAILED;
        }
        ca_mutex_lock(g_deviceStateListMutex);
        state = CALEClientGetStateInfo(address);
        ca_mutex_unlock(g_deviceStateListMutex);
        (*env)->ReleaseStringUTFChars(env, jni_address, address);
    }

    if (!state)
    {
        OIC_LOG(DEBUG, TAG, "state is empty..start to connect LE");

        // cancel previous connection request before connection
        // if there is gatt object in g_gattObjectList.
        if (jni_address)
        {
            address = (char*)(*env)->GetStringUTFChars(env, jni_address, NULL);
            if (!address)
            {
                OIC_LOG(ERROR, TAG, "address is not available");
                return CA_STATUS_FAILED;
            }

            jobject gatt = CALEClientGetGattObjInList(env, address);
            if (gatt)
            {
                CAResult_t res = CALEClientDisconnect(env, gatt);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(INFO, TAG, "there is no gatt object");
                }
            }
            (*env)->ReleaseStringUTFChars(env, jni_address, address);
        }

        // connection request
        jobject newGatt = CALEClientConnect(env, device,
                                            JNI_FALSE);
        if (NULL == newGatt)
        {
            OIC_LOG(ERROR, TAG, "CALEClientConnect has failed");
            return CA_STATUS_FAILED;
        }
    }
    else
    {
        if (STATE_CONNECTED == state->connectedState)
        {
            OIC_LOG(INFO, TAG, "GATT has already connected");
            if (!jni_address)
            {
                OIC_LOG(ERROR, TAG, "jni_address is not available");
                return CA_STATUS_FAILED;
            }

            address = (char*)(*env)->GetStringUTFChars(env, jni_address, NULL);
            if (!address)
            {
                OIC_LOG(ERROR, TAG, "address is not available");
                return CA_STATUS_FAILED;
            }

            jobject gatt = CALEClientGetGattObjInList(env, address);
            if (!gatt)
            {
                OIC_LOG(ERROR, TAG, "CALEClientGetGattObjInList has failed");
                (*env)->ReleaseStringUTFChars(env, jni_address, address);
                return CA_STATUS_FAILED;
            }

            CAResult_t ret = CALESetValueAndWriteCharacteristic(env, gatt);
            if (CA_STATUS_OK != ret)
            {
                OIC_LOG(ERROR, TAG, "CALESetValueAndWriteCharacteristic has failed");
                (*env)->ReleaseStringUTFChars(env, jni_address, address);
                return ret;
            }
            (*env)->ReleaseStringUTFChars(env, jni_address, address);
        }
        else
        {
            OIC_LOG(INFO, TAG, "STATE_DISCONNECTED - start to connect LE");

            // cancel previous connection request before connection
            // if there is gatt object in g_gattObjectList.
            if (jni_address)
            {
                address = (char*)(*env)->GetStringUTFChars(env, jni_address, NULL);
                if (!address)
                {
                    OIC_LOG(ERROR, TAG, "address is not available");
                    return CA_STATUS_FAILED;
                }

                jobject gatt = CALEClientGetGattObjInList(env, address);
                if (gatt)
                {
                    CAResult_t res = CALEClientDisconnect(env, gatt);
                    if (CA_STATUS_OK != res)
                    {
                        OIC_LOG(INFO, TAG, "there is no gatt object");
                    }
                }
                (*env)->ReleaseStringUTFChars(env, jni_address, address);
            }

            OIC_LOG(DEBUG, TAG, "start to connect LE");
            jobject gatt = CALEClientConnect(env, device,
                                             CALEClientGetAutoConnectFlag(env, jni_address));
            if (NULL == gatt)
            {
                OIC_LOG(ERROR, TAG, "CALEClientConnect has failed");
                return CA_STATUS_FAILED;
            }
        }
    }

    return CA_STATUS_OK;
}

jstring CALEClientGetAddressFromGattObj(JNIEnv *env, jobject gatt)
{
    VERIFY_NON_NULL_RET(gatt, TAG, "gatt is null", NULL);
    VERIFY_NON_NULL_RET(env, TAG, "env is null", NULL);

    jmethodID jni_mid_getDevice = CALEGetJNIMethodID(env, CLASSPATH_BT_GATT, "getDevice",
                                                     "()Landroid/bluetooth/BluetoothDevice;");
    if (!jni_mid_getDevice)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_getDevice is null");
        return NULL;
    }

    jobject jni_obj_device = (*env)->CallObjectMethod(env, gatt, jni_mid_getDevice);
    if (!jni_obj_device)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_device is null");
        return NULL;
    }

    jstring jni_address = CALEGetAddressFromBTDevice(env, jni_obj_device);
    if (!jni_address)
    {
        OIC_LOG(ERROR, TAG, "jni_address is null");
        return NULL;
    }

    return jni_address;
}

/**
 * BLE layer
 */
CAResult_t CALEClientGattClose(JNIEnv *env, jobject bluetoothGatt)
{
    // GATT CLOSE
    OIC_LOG(DEBUG, TAG, "Gatt Close");
    VERIFY_NON_NULL(bluetoothGatt, TAG, "bluetoothGatt is null");
    VERIFY_NON_NULL(env, TAG, "env is null");

    // get BluetoothGatt method
    OIC_LOG(DEBUG, TAG, "get BluetoothGatt method");
    jmethodID jni_mid_closeGatt = CALEGetJNIMethodID(env, CLASSPATH_BT_GATT, "close", "()V");
    if (!jni_mid_closeGatt)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_closeGatt is null");
        return CA_STATUS_OK;
    }

    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "request to close GATT");
    (*env)->CallVoidMethod(env, bluetoothGatt, jni_mid_closeGatt);

    if ((*env)->ExceptionCheck(env))
    {
        OIC_LOG(ERROR, TAG, "closeGATT has failed");
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

CAResult_t CALEClientStartScan()
{
    if (!g_isStartedLEClient)
    {
        OIC_LOG(ERROR, TAG, "LE client is not started");
        return CA_STATUS_FAILED;
    }

    if (!g_jvm)
    {
        OIC_LOG(ERROR, TAG, "g_jvm is null");
        return CA_STATUS_FAILED;
    }

    if (g_isStartedScan)
    {
        OIC_LOG(INFO, TAG, "scanning is already started");
        return CA_STATUS_OK;
    }

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");

        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    OIC_LOG(DEBUG, TAG, "CALEClientStartScan");

    CAResult_t ret = CA_STATUS_OK;
    // scan gatt server with UUID
    if (g_leScanCallback && g_uuidList)
    {
#ifdef UUID_SCAN
        ret = CALEClientStartScanWithUUIDImpl(env, g_uuidList, g_leScanCallback);
#else
        ret = CALEClientStartScanImpl(env, g_leScanCallback);
#endif
        if (CA_STATUS_OK != ret)
        {
            if (CA_ADAPTER_NOT_ENABLED == ret)
            {
                OIC_LOG(DEBUG, TAG, "Adapter is disabled");
            }
            else
            {
                OIC_LOG(ERROR, TAG, "start scan has failed");
            }
        }
    }

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return ret;
}

CAResult_t CALEClientStartScanImpl(JNIEnv *env, jobject callback)
{
    VERIFY_NON_NULL(callback, TAG, "callback is null");
    VERIFY_NON_NULL(env, TAG, "env is null");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(INFO, TAG, "BT adapter is not enabled");
        return CA_ADAPTER_NOT_ENABLED;
    }

    // get default bt adapter class
    jclass jni_cid_BTAdapter = (*env)->FindClass(env, CLASSPATH_BT_ADAPTER);
    if (!jni_cid_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "getState From BTAdapter: jni_cid_BTAdapter is null");
        return CA_STATUS_FAILED;
    }

    // get remote bt adapter method
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env, jni_cid_BTAdapter,
                                                                    "getDefaultAdapter",
                                                                    METHODID_OBJECTNONPARAM);
    if (!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_getDefaultAdapter is null");
        return CA_STATUS_FAILED;
    }

    // get start le scan method
    jmethodID jni_mid_startLeScan = (*env)->GetMethodID(env, jni_cid_BTAdapter, "startLeScan",
                                                        "(Landroid/bluetooth/BluetoothAdapter$"
                                                        "LeScanCallback;)Z");
    if (!jni_mid_startLeScan)
    {
        OIC_LOG(ERROR, TAG, "startLeScan: jni_mid_startLeScan is null");
        return CA_STATUS_FAILED;
    }

    // gat bt adapter object
    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter,
                                                               jni_mid_getDefaultAdapter);
    if (!jni_obj_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "getState From BTAdapter: jni_obj_BTAdapter is null");
        return CA_STATUS_FAILED;
    }

    // call start le scan method
    jboolean jni_obj_startLeScan = (*env)->CallBooleanMethod(env, jni_obj_BTAdapter,
                                                             jni_mid_startLeScan, callback);
    if (!jni_obj_startLeScan)
    {
        OIC_LOG(INFO, TAG, "startLeScan is failed");
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "startLeScan is started");
        CALEClientSetScanFlag(true);
    }

    return CA_STATUS_OK;
}

CAResult_t CALEClientStartScanWithUUIDImpl(JNIEnv *env, jobjectArray uuids, jobject callback)
{
    VERIFY_NON_NULL(callback, TAG, "callback is null");
    VERIFY_NON_NULL(uuids, TAG, "uuids is null");
    VERIFY_NON_NULL(env, TAG, "env is null");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(INFO, TAG, "BT adapter is not enabled");
        return CA_ADAPTER_NOT_ENABLED;
    }

    jclass jni_cid_BTAdapter = (*env)->FindClass(env, CLASSPATH_BT_ADAPTER);
    if (!jni_cid_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "getState From BTAdapter: jni_cid_BTAdapter is null");
        return CA_STATUS_FAILED;
    }

    // get remote bt adapter method
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env, jni_cid_BTAdapter,
                                                                    "getDefaultAdapter",
                                                                    METHODID_OBJECTNONPARAM);
    if (!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_getDefaultAdapter is null");
        return CA_STATUS_FAILED;
    }

    // get start le scan method
    jmethodID jni_mid_startLeScan = (*env)->GetMethodID(env, jni_cid_BTAdapter, "startLeScan",
                                                        "([Ljava/util/UUID;Landroid/bluetooth/"
                                                        "BluetoothAdapter$LeScanCallback;)Z");
    if (!jni_mid_startLeScan)
    {
        OIC_LOG(ERROR, TAG, "startLeScan: jni_mid_startLeScan is null");
        return CA_STATUS_FAILED;
    }

    // get bt adapter object
    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter,
                                                               jni_mid_getDefaultAdapter);
    if (!jni_obj_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "getState From BTAdapter: jni_obj_BTAdapter is null");
        return CA_STATUS_FAILED;
    }

    // call start le scan method
    jboolean jni_obj_startLeScan = (*env)->CallBooleanMethod(env, jni_obj_BTAdapter,
                                                             jni_mid_startLeScan, uuids, callback);
    if (!jni_obj_startLeScan)
    {
        OIC_LOG(INFO, TAG, "startLeScan With UUID is failed");
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "startLeScan With UUID is started");
        CALEClientSetScanFlag(true);
    }

    return CA_STATUS_OK;
}

jobject CALEClientGetUUIDObject(JNIEnv *env, const char* uuid)
{
    VERIFY_NON_NULL_RET(uuid, TAG, "uuid is null", NULL);
    VERIFY_NON_NULL_RET(env, TAG, "env is null", NULL);

    // setting UUID
    jclass jni_cid_uuid = (*env)->FindClass(env, CLASSPATH_BT_UUID);
    if (!jni_cid_uuid)
    {
        OIC_LOG(ERROR, TAG, "jni_cid_uuid is null");
        return NULL;
    }

    jmethodID jni_mid_fromString = (*env)->GetStaticMethodID(env, jni_cid_uuid, "fromString",
                                                             "(Ljava/lang/String;)"
                                                             "Ljava/util/UUID;");
    if (!jni_mid_fromString)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_fromString is null");
        return NULL;
    }

    jstring jni_uuid = (*env)->NewStringUTF(env, uuid);
    jobject jni_obj_uuid = (*env)->CallStaticObjectMethod(env, jni_cid_uuid, jni_mid_fromString,
                                                          jni_uuid);
    if (!jni_obj_uuid)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_uuid is null");
        return NULL;
    }

    return jni_obj_uuid;
}

CAResult_t CALEClientStopScan()
{
    if (!g_jvm)
    {
        OIC_LOG(ERROR, TAG, "g_jvm is null");
        return CA_STATUS_FAILED;
    }

    if (!g_isStartedScan)
    {
        OIC_LOG(INFO, TAG, "scanning is already stopped");
        return CA_STATUS_OK;
    }

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    CAResult_t ret = CALEClientStopScanImpl(env, g_leScanCallback);
    if (CA_STATUS_OK != ret)
    {
        if (CA_ADAPTER_NOT_ENABLED == ret)
        {
            OIC_LOG(DEBUG, TAG, "Adapter is disabled");
        }
        else
        {
            OIC_LOG(ERROR, TAG, "CALEClientStopScanImpl has failed");
        }
    }
    else
    {
        CALEClientSetScanFlag(false);
    }

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return ret;
}

void CALEClientSetScanFlag(bool flag)
{
    ca_mutex_lock(g_scanMutex);
    g_isStartedScan = flag;
    ca_mutex_unlock(g_scanMutex);
}

CAResult_t CALEClientStopScanImpl(JNIEnv *env, jobject callback)
{
    OIC_LOG(DEBUG, TAG, "CALEClientStopScanImpl");
    VERIFY_NON_NULL(callback, TAG, "callback is null");
    VERIFY_NON_NULL(env, TAG, "env is null");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(INFO, TAG, "BT adapter is not enabled");
        return CA_ADAPTER_NOT_ENABLED;
    }

    // get default bt adapter class
    jclass jni_cid_BTAdapter = (*env)->FindClass(env, CLASSPATH_BT_ADAPTER);
    if (!jni_cid_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "getState From BTAdapter: jni_cid_BTAdapter is null");
        return CA_STATUS_FAILED;
    }

    // get remote bt adapter method
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env, jni_cid_BTAdapter,
                                                                    "getDefaultAdapter",
                                                                    METHODID_OBJECTNONPARAM);
    if (!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_getDefaultAdapter is null");
        return CA_STATUS_FAILED;
    }

    // get start le scan method
    jmethodID jni_mid_stopLeScan = (*env)->GetMethodID(env, jni_cid_BTAdapter, "stopLeScan",
                                                       "(Landroid/bluetooth/"
                                                       "BluetoothAdapter$LeScanCallback;)V");
    if (!jni_mid_stopLeScan)
    {
        OIC_LOG(ERROR, TAG, "stopLeScan: jni_mid_stopLeScan is null");
        return CA_STATUS_FAILED;
    }

    // gat bt adapter object
    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter,
                                                               jni_mid_getDefaultAdapter);
    if (!jni_obj_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_BTAdapter is null");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "CALL API - request to stop LE Scan");
    // call start le scan method
    (*env)->CallVoidMethod(env, jni_obj_BTAdapter, jni_mid_stopLeScan, callback);
    if ((*env)->ExceptionCheck(env))
    {
        OIC_LOG(ERROR, TAG, "stopLeScan has failed");
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

CAResult_t CALEClientSetAutoConnectFlag(JNIEnv *env, jstring jni_address, jboolean flag)
{
    OIC_LOG(DEBUG, TAG, "IN - CALEClientSetAutoConnectFlag");
    VERIFY_NON_NULL(env, TAG, "env");
    VERIFY_NON_NULL(jni_address, TAG, "jni_address");

    ca_mutex_lock(g_deviceStateListMutex);

    char* address = (char*)(*env)->GetStringUTFChars(env, jni_address, NULL);
    if (!address)
    {
        OIC_LOG(ERROR, TAG, "address is not available");
        return CA_STATUS_FAILED;
    }

    if (CALEClientIsDeviceInList(address))
    {
        CALEState_t* curState = CALEClientGetStateInfo(address);
        if(!curState)
        {
            OIC_LOG(ERROR, TAG, "curState is null");
            (*env)->ReleaseStringUTFChars(env, jni_address, address);
            ca_mutex_unlock(g_deviceStateListMutex);
            return CA_STATUS_FAILED;
        }
        OIC_LOG_V(INFO, TAG, "auto connect flag is set %d", flag);

        curState->autoConnectFlag = flag;
    }

    (*env)->ReleaseStringUTFChars(env, jni_address, address);
    ca_mutex_unlock(g_deviceStateListMutex);
    OIC_LOG(DEBUG, TAG, "OUT - CALEClientSetAutoConnectFlag");
    return CA_STATUS_OK;
}

jboolean CALEClientGetAutoConnectFlag(JNIEnv *env, jstring jni_address)
{
    OIC_LOG(DEBUG, TAG, "IN - CALEClientGetAutoConnectFlag");
    VERIFY_NON_NULL_RET(env, TAG, "env", false);
    VERIFY_NON_NULL_RET(jni_address, TAG, "jni_address", false);

    ca_mutex_lock(g_deviceStateListMutex);

    char* address = (char*)(*env)->GetStringUTFChars(env, jni_address, NULL);
    if (!address)
    {
        OIC_LOG(ERROR, TAG, "address is not available");
        return JNI_FALSE;
    }

    CALEState_t* curState = CALEClientGetStateInfo(address);
    if(!curState)
    {
        OIC_LOG(INFO, TAG, "there is no information. auto connect flag is false");
        (*env)->ReleaseStringUTFChars(env, jni_address, address);
        ca_mutex_unlock(g_deviceStateListMutex);
        return JNI_FALSE;
    }
    OIC_LOG_V(INFO, TAG, "auto connect flag is %d", curState->autoConnectFlag);

    (*env)->ReleaseStringUTFChars(env, jni_address, address);
    ca_mutex_unlock(g_deviceStateListMutex);

    OIC_LOG(DEBUG, TAG, "OUT - CALEClientGetAutoConnectFlag");
    return curState->autoConnectFlag;
}

jobject CALEClientConnect(JNIEnv *env, jobject bluetoothDevice, jboolean autoconnect)
{
    OIC_LOG(DEBUG, TAG, "CALEClientConnect");
    VERIFY_NON_NULL_RET(env, TAG, "env is null", NULL);
    VERIFY_NON_NULL_RET(bluetoothDevice, TAG, "bluetoothDevice is null", NULL);

    // get gatt object from Bluetooth Device object for closeProfileProxy(..)
    jstring jni_address = CALEClientGetLEAddressFromBTDevice(env, bluetoothDevice);
    if (jni_address)
    {
        const char* address = (*env)->GetStringUTFChars(env, jni_address, NULL);
        if (!address)
        {
            OIC_LOG(ERROR, TAG, "address is not available");
            return NULL;
        }

        // close the gatt service
        jobject gatt = CALEClientGetGattObjInList(env, address);
        if (gatt)
        {
            CAResult_t res = CALEClientCloseProfileProxy(env, gatt);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "CALEClientCloseProfileProxy has failed");
                (*env)->ReleaseStringUTFChars(env, jni_address, address);
                return NULL;
            }

            // clean previous gatt object after close profile service
            res = CALEClientRemoveGattObjForAddr(env, jni_address);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "CALEClientRemoveGattObjForAddr has failed");
                (*env)->ReleaseStringUTFChars(env, jni_address, address);
                return NULL;
            }
        }
        (*env)->ReleaseStringUTFChars(env, jni_address, address);
    }

    jobject newGatt = CALEClientGattConnect(env, bluetoothDevice, autoconnect);
    if (!newGatt)
    {
        OIC_LOG(DEBUG, TAG, "re-connection will be started");
        return NULL;
    }

    // add new gatt object into g_gattObjectList
    CAResult_t res = CALEClientAddGattobjToList(env, newGatt);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "CALEClientAddGattobjToList has failed");
        return NULL;
    }

    return newGatt;
}

jobject CALEClientGattConnect(JNIEnv *env, jobject bluetoothDevice, jboolean autoconnect)
{
    OIC_LOG(DEBUG, TAG, "GATT CONNECT");
    VERIFY_NON_NULL_RET(env, TAG, "env is null", NULL);
    VERIFY_NON_NULL_RET(bluetoothDevice, TAG, "bluetoothDevice is null", NULL);

    if (!g_leGattCallback)
    {
        OIC_LOG(INFO, TAG, "g_leGattCallback is null");
        return NULL;
    }

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(INFO, TAG, "BT adapter is not enabled");
        return NULL;
    }

    jstring jni_address = CALEGetAddressFromBTDevice(env, bluetoothDevice);
    if (!jni_address)
    {
        OIC_LOG(ERROR, TAG, "bleConnect: CALEGetAddressFromBTDevice is null");
        return NULL;
    }

    // get BluetoothDevice method
    OIC_LOG(DEBUG, TAG, "get BluetoothDevice method");
    jmethodID jni_mid_connectGatt = CALEGetJNIMethodID(env, "android/bluetooth/BluetoothDevice",
                                                       "connectGatt",
                                                       "(Landroid/content/Context;ZLandroid/"
                                                       "bluetooth/BluetoothGattCallback;)"
                                                       "Landroid/bluetooth/BluetoothGatt;");
    if (!jni_mid_connectGatt)
    {
        OIC_LOG(ERROR, TAG, "bleConnect: jni_mid_connectGatt is null");
        return NULL;
    }

    OIC_LOG(INFO, TAG, "CALL API - connectGatt");
    jobject jni_obj_connectGatt = (*env)->CallObjectMethod(env, bluetoothDevice,
                                                           jni_mid_connectGatt,
                                                           NULL,
                                                           autoconnect, g_leGattCallback);
    if (!jni_obj_connectGatt)
    {
        OIC_LOG(ERROR, TAG, "connectGatt was failed..it will be removed");
        CALEClientRemoveDeviceInScanDeviceList(env, jni_address);
        CALEClientUpdateSendCnt(env);
        return NULL;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "le connecting..please wait..");
    }
    return jni_obj_connectGatt;
}

CAResult_t CALEClientCloseProfileProxy(JNIEnv *env, jobject gatt)
{
    OIC_LOG(DEBUG, TAG, "IN - CALEClientCloseProfileProxy");

    VERIFY_NON_NULL(env, TAG, "env is null");
    VERIFY_NON_NULL(gatt, TAG, "gatt is null");

    jclass jni_cid_BTAdapter = (*env)->FindClass(env, CLASSPATH_BT_ADAPTER);
    if (!jni_cid_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "jni_cid_BTAdapter is null");
        return CA_STATUS_FAILED;
    }

    // get remote bt adapter method
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env, jni_cid_BTAdapter,
                                                                    "getDefaultAdapter",
                                                                    METHODID_OBJECTNONPARAM);
    if (!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_getDefaultAdapter is null");
        return CA_STATUS_FAILED;
    }

    // gat bt adapter object
    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter,
                                                               jni_mid_getDefaultAdapter);
    if (!jni_obj_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_BTAdapter is null");
        return CA_STATUS_FAILED;
    }

    // get closeProfileProxy method
    jmethodID jni_mid_closeProfileProxy = (*env)->GetMethodID(env, jni_cid_BTAdapter,
                                                              "closeProfileProxy",
                                                              "(ILandroid/bluetooth/"
                                                              "BluetoothProfile;)V");
    if (!jni_mid_closeProfileProxy)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_closeProfileProxy is null");
        return CA_STATUS_FAILED;
    }

    jclass jni_cid_BTProfile = (*env)->FindClass(env, CLASSPATH_BT_PROFILE);
    if (!jni_cid_BTProfile)
    {
        OIC_LOG(ERROR, TAG, "jni_cid_BTProfile is null");
        return CA_STATUS_FAILED;
    }

    // GATT - Constant value : 7 (0x00000007)
    jfieldID id_gatt = (*env)->GetStaticFieldID(env, jni_cid_BTProfile,
                                                "GATT", "I");
    if (!id_gatt)
    {
        OIC_LOG(ERROR, TAG, "id_gatt is null");
        return CA_STATUS_FAILED;
    }

    jint jni_gatt = (*env)->GetStaticIntField(env, jni_cid_BTProfile, id_gatt);

    OIC_LOG(DEBUG, TAG, "CALL API - close the connection of the profile proxy to the Service");
    (*env)->CallVoidMethod(env, jni_obj_BTAdapter, jni_mid_closeProfileProxy, jni_gatt, gatt);
    if ((*env)->ExceptionCheck(env))
    {
        OIC_LOG(ERROR, TAG, "closeProfileProxy has failed");
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT - CALEClientCloseProfileProxy");
    return CA_STATUS_OK;
}


CAResult_t CALEClientDisconnect(JNIEnv *env, jobject bluetoothGatt)
{
    OIC_LOG(DEBUG, TAG, "GATT DISCONNECT");
    VERIFY_NON_NULL(env, TAG, "env is null");
    VERIFY_NON_NULL(bluetoothGatt, TAG, "bluetoothGatt is null");

    // get BluetoothGatt method
    OIC_LOG(DEBUG, TAG, "get gatt disconnect method");
    jmethodID jni_mid_disconnectGatt  = CALEGetJNIMethodID(env, CLASSPATH_BT_GATT,
                                                           "disconnect", "()V");
    if (!jni_mid_disconnectGatt)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_disconnectGatt is null");
        return CA_STATUS_FAILED;
    }

    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "CALL API - request disconnect gatt");
    (*env)->CallVoidMethod(env, bluetoothGatt, jni_mid_disconnectGatt);
    if ((*env)->ExceptionCheck(env))
    {
        OIC_LOG(ERROR, TAG, "disconnect has failed");
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "disconnecting Gatt...");

    return CA_STATUS_OK;
}

CAResult_t CALEClientDisconnectAll(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CALEClientDisconnectAll");
    VERIFY_NON_NULL(env, TAG, "env is null");

    if (!g_gattObjectList)
    {
        OIC_LOG(DEBUG, TAG, "already removed for g_gattObjectList");
        return CA_STATUS_OK;
    }

    uint32_t length = u_arraylist_length(g_gattObjectList);
    OIC_LOG_V(DEBUG, TAG, "list length : %d", length);
    for (uint32_t index = 0; index < length; index++)
    {
        OIC_LOG(DEBUG, TAG, "start CALEClientDisconnectAll");
        jobject jarrayObj = (jobject) u_arraylist_get(g_gattObjectList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            continue;
        }
        CAResult_t res = CALEClientDisconnect(env, jarrayObj);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CALEClientDisconnect has failed");
            continue;
        }
    }

    return CA_STATUS_OK;
}

CAResult_t CALEClientDisconnectforAddress(JNIEnv *env, jstring remote_address)
{
    OIC_LOG(DEBUG, TAG, "IN-CALEClientDisconnectforAddress");
    VERIFY_NON_NULL(env, TAG, "env is null");

    if (!g_gattObjectList)
    {
        OIC_LOG(DEBUG, TAG, "already removed for g_gattObjectList");
        return CA_STATUS_OK;
    }

    char* address = (char*)(*env)->GetStringUTFChars(env, remote_address, NULL);
    if (!address)
    {
        OIC_LOG(ERROR, TAG, "address is null");
        return CA_STATUS_FAILED;
    }

    uint32_t length = u_arraylist_length(g_gattObjectList);
    for (uint32_t index = 0; index < length; index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_gattObjectList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            continue;
        }

        jstring jni_setAddress = CALEClientGetAddressFromGattObj(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "jni_setAddress is null");
            (*env)->ReleaseStringUTFChars(env, remote_address, address);
            return CA_STATUS_FAILED;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        if (!setAddress)
        {
            OIC_LOG(ERROR, TAG, "setAddress is null");
            (*env)->ReleaseStringUTFChars(env, remote_address, address);
            return CA_STATUS_FAILED;
        }

        OIC_LOG_V(DEBUG, TAG, "target address : %s, set address : %s", address, setAddress);
        if (!strcmp(address, setAddress))
        {
            CAResult_t res = CALEClientDisconnect(env, jarrayObj);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "CALEClientDisconnect has failed");
                (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
                (*env)->ReleaseStringUTFChars(env, remote_address, address);
                return CA_STATUS_FAILED;
            }
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
            (*env)->ReleaseStringUTFChars(env, remote_address, address);
            return CA_STATUS_OK;
        }
        (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
    }
    (*env)->ReleaseStringUTFChars(env, remote_address, address);

    OIC_LOG(DEBUG, TAG, "OUT-CALEClientDisconnectforAddress");
    return CA_STATUS_OK;
}

CAResult_t CALEClientDiscoverServices(JNIEnv *env, jobject bluetoothGatt)
{
    VERIFY_NON_NULL(env, TAG, "env is null");
    VERIFY_NON_NULL(bluetoothGatt, TAG, "bluetoothGatt is null");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(INFO, TAG, "BT adapter is not enabled");
        return CA_ADAPTER_NOT_ENABLED;
    }

    // get BluetoothGatt.discoverServices method
    OIC_LOG(DEBUG, TAG, "get BluetoothGatt.discoverServices method");
    jmethodID jni_mid_discoverServices = CALEGetJNIMethodID(env, CLASSPATH_BT_GATT,
                                                            "discoverServices", "()Z");
    if (!jni_mid_discoverServices)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_discoverServices is null");
        return CA_STATUS_FAILED;
    }

    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "CALL API - request discovery gatt services");
    jboolean ret = (*env)->CallBooleanMethod(env, bluetoothGatt, jni_mid_discoverServices);
    if (!ret)
    {
        OIC_LOG(ERROR, TAG, "discoverServices has not been started");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

static void CALEWriteCharacteristicThread(void* object)
{
    VERIFY_NON_NULL_VOID(object, TAG, "object is null");

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return;
        }
        isAttached = true;
    }

    jobject gatt = (jobject)object;
    CAResult_t ret = CALESetValueAndWriteCharacteristic(env, gatt);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALESetValueAndWriteCharacteristic has failed");
    }

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
}

CAResult_t CALESetValueAndWriteCharacteristic(JNIEnv* env, jobject gatt)
{
    VERIFY_NON_NULL(gatt, TAG, "gatt is null");
    VERIFY_NON_NULL(env, TAG, "env is null");

    // send data
    jobject jni_obj_character = CALEClientCreateGattCharacteristic(env, gatt, g_sendBuffer);
    if (!jni_obj_character)
    {
        CALEClientSendFinish(env, gatt);
        return CA_STATUS_FAILED;
    }

    CAResult_t ret = CALEClientWriteCharacteristicImpl(env, gatt, jni_obj_character);
    if (CA_STATUS_OK != ret)
    {
        CALEClientSendFinish(env, gatt);
        return CA_STATUS_FAILED;
    }

    // wait for callback for write Characteristic with success to sent data
    OIC_LOG_V(DEBUG, TAG, "callback flag is %d", g_isSignalSetFlag);
    ca_mutex_lock(g_threadWriteCharacteristicMutex);
    if (!g_isSignalSetFlag)
    {
        OIC_LOG(DEBUG, TAG, "wait for callback to notify writeCharacteristic is success");
        if (CA_WAIT_SUCCESS != ca_cond_wait_for(g_threadWriteCharacteristicCond,
                                  g_threadWriteCharacteristicMutex,
                                  WAIT_TIME_WRITE_CHARACTERISTIC))
        {
            OIC_LOG(ERROR, TAG, "there is no response. write has failed");
            g_isSignalSetFlag = false;
            ca_mutex_unlock(g_threadWriteCharacteristicMutex);
            return CA_STATUS_FAILED;
        }
    }
    // reset flag set by writeCharacteristic Callback
    g_isSignalSetFlag = false;
    ca_mutex_unlock(g_threadWriteCharacteristicMutex);

    OIC_LOG(INFO, TAG, "writeCharacteristic success!!");
    return CA_STATUS_OK;
}

CAResult_t CALEClientWriteCharacteristic(JNIEnv *env, jobject gatt)
{
    OIC_LOG(DEBUG, TAG, "IN - CALEClientWriteCharacteristic");
    VERIFY_NON_NULL(env, TAG, "env is null");
    VERIFY_NON_NULL(gatt, TAG, "gatt is null");

    jobject gattParam = (*env)->NewGlobalRef(env, gatt);
    if (CA_STATUS_OK != ca_thread_pool_add_task(g_threadPoolHandle,
                                                CALEWriteCharacteristicThread, (void*)gattParam))
    {
        OIC_LOG(ERROR, TAG, "Failed to create read thread!");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT - CALEClientWriteCharacteristic");
    return CA_STATUS_OK;
}

CAResult_t CALEClientWriteCharacteristicImpl(JNIEnv *env, jobject bluetoothGatt,
                                             jobject gattCharacteristic)
{
    OIC_LOG(DEBUG, TAG, "WRITE GATT CHARACTERISTIC");
    VERIFY_NON_NULL(env, TAG, "env is null");
    VERIFY_NON_NULL(bluetoothGatt, TAG, "bluetoothGatt is null");
    VERIFY_NON_NULL(gattCharacteristic, TAG, "gattCharacteristic is null");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(INFO, TAG, "BT adapter is not enabled");
        return CA_STATUS_FAILED;
    }

    // get BluetoothGatt.write characteristic method
    OIC_LOG(DEBUG, TAG, "write characteristic method");
    jmethodID jni_mid_writeCharacteristic = CALEGetJNIMethodID(env, CLASSPATH_BT_GATT,
                                                               "writeCharacteristic",
                                                               "(Landroid/bluetooth/"
                                                               "BluetoothGattCharacteristic;)Z");
    if (!jni_mid_writeCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_writeCharacteristic is null");
        return CA_STATUS_FAILED;
    }

    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "CALL API - request to write gatt characteristic");
    jboolean ret = (jboolean)(*env)->CallBooleanMethod(env, bluetoothGatt,
                                                       jni_mid_writeCharacteristic,
                                                       gattCharacteristic);
    if (ret)
    {
        OIC_LOG(DEBUG, TAG, "writeCharacteristic is called successfully");
    }
    else
    {
        OIC_LOG(ERROR, TAG, "writeCharacteristic has failed");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

CAResult_t CALEClientReadCharacteristic(JNIEnv *env, jobject bluetoothGatt)
{
    VERIFY_NON_NULL(env, TAG, "env is null");
    VERIFY_NON_NULL(bluetoothGatt, TAG, "bluetoothGatt is null");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(INFO, TAG, "BT adapter is not enabled");
        return CA_STATUS_FAILED;
    }

    jstring jni_uuid = (*env)->NewStringUTF(env, OIC_GATT_CHARACTERISTIC_RESPONSE_UUID);
    if (!jni_uuid)
    {
        OIC_LOG(ERROR, TAG, "jni_uuid is null");
        return CA_STATUS_FAILED;
    }

    jobject jni_obj_GattCharacteristic = CALEClientGetGattService(env, bluetoothGatt, jni_uuid);
    if (!jni_obj_GattCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_GattCharacteristic is null");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "read characteristic method");
    jmethodID jni_mid_readCharacteristic = CALEGetJNIMethodID(env, CLASSPATH_BT_GATT,
                                                              "readCharacteristic",
                                                              "(Landroid/bluetooth/"
                                                              "BluetoothGattCharacteristic;)Z");
    if (!jni_mid_readCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_readCharacteristic is null");
        return CA_STATUS_FAILED;
    }

    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "CALL API - request to read gatt characteristic");
    jboolean ret = (*env)->CallBooleanMethod(env, bluetoothGatt, jni_mid_readCharacteristic,
                                             jni_obj_GattCharacteristic);
    if (ret)
    {
        OIC_LOG(DEBUG, TAG, "readCharacteristic success");
    }
    else
    {
        OIC_LOG(ERROR, TAG, "readCharacteristic has failed");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

CAResult_t CALEClientSetCharacteristicNotification(JNIEnv *env, jobject bluetoothGatt,
                                                   jobject characteristic)
{
    VERIFY_NON_NULL(env, TAG, "env is null");
    VERIFY_NON_NULL(bluetoothGatt, TAG, "bluetoothGatt is null");
    VERIFY_NON_NULL(characteristic, TAG, "characteristic is null");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(INFO, TAG, "BT adapter is not enabled");
        return CA_ADAPTER_NOT_ENABLED;
    }

    // get BluetoothGatt.setCharacteristicNotification method
    OIC_LOG(DEBUG, TAG, "CALEClientSetCharacteristicNotification");
    jmethodID jni_mid_setNotification = CALEGetJNIMethodID(env, CLASSPATH_BT_GATT,
                                                              "setCharacteristicNotification",
                                                              "(Landroid/bluetooth/"
                                                              "BluetoothGattCharacteristic;Z)Z");
    if (!jni_mid_setNotification)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_getService is null");
        return CA_STATUS_FAILED;
    }

    jboolean ret = (*env)->CallBooleanMethod(env, bluetoothGatt, jni_mid_setNotification,
                                             characteristic, JNI_TRUE);
    if (JNI_TRUE == ret)
    {
        OIC_LOG(DEBUG, TAG, "CALL API - setCharacteristicNotification success");
    }
    else
    {
        OIC_LOG(ERROR, TAG, "CALL API - setCharacteristicNotification has failed");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

jobject CALEClientGetGattService(JNIEnv *env, jobject bluetoothGatt, jstring characterUUID)
{
    VERIFY_NON_NULL_RET(env, TAG, "env is null", NULL);
    VERIFY_NON_NULL_RET(bluetoothGatt, TAG, "bluetoothGatt is null", NULL);
    VERIFY_NON_NULL_RET(characterUUID, TAG, "characterUUID is null", NULL);

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(INFO, TAG, "BT adapter is not enabled");
        return NULL;
    }

    // get BluetoothGatt.getService method
    OIC_LOG(DEBUG, TAG, "BluetoothGatt.getService");
    jmethodID jni_mid_getService = CALEGetJNIMethodID(env, CLASSPATH_BT_GATT,
                                                      "getService",
                                                      "(Ljava/util/UUID;)Landroid/bluetooth/"
                                                      "BluetoothGattService;");
    if (!jni_mid_getService)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_getService is null");
        return NULL;
    }

    jobject jni_obj_service_uuid = CALEClientGetUUIDObject(env, OIC_GATT_SERVICE_UUID);
    if (!jni_obj_service_uuid)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_service_uuid is null");
        return NULL;
    }

    // get bluetooth gatt service
    OIC_LOG(DEBUG, TAG, "request to get service");
    jobject jni_obj_gattService = (*env)->CallObjectMethod(env, bluetoothGatt, jni_mid_getService,
                                                           jni_obj_service_uuid);
    if (!jni_obj_gattService)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_gattService is null");
        return NULL;
    }

    // get bluetooth gatt service method
    jmethodID jni_mid_getCharacteristic = CALEGetJNIMethodID(env, "android/bluetooth/"
                                                      "BluetoothGattService",
                                                      "getCharacteristic",
                                                      "(Ljava/util/UUID;)"
                                                      "Landroid/bluetooth/"
                                                      "BluetoothGattCharacteristic;");
    if (!jni_mid_getCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_getCharacteristic is null");
        return NULL;
    }

    const char* uuid = (*env)->GetStringUTFChars(env, characterUUID, NULL);
    if (!uuid)
    {
        OIC_LOG(ERROR, TAG, "uuid is null");
        return NULL;
    }

    jobject jni_obj_tx_uuid = CALEClientGetUUIDObject(env, uuid);
    if (!jni_obj_tx_uuid)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_tx_uuid is null");
        (*env)->ReleaseStringUTFChars(env, characterUUID, uuid);
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "request to get Characteristic");
    jobject jni_obj_GattCharacteristic = (*env)->CallObjectMethod(env, jni_obj_gattService,
                                                                  jni_mid_getCharacteristic,
                                                                  jni_obj_tx_uuid);

    (*env)->ReleaseStringUTFChars(env, characterUUID, uuid);
    return jni_obj_GattCharacteristic;
}

jobject CALEClientCreateGattCharacteristic(JNIEnv *env, jobject bluetoothGatt, jbyteArray data)
{
    OIC_LOG(DEBUG, TAG, "CALEClientCreateGattCharacteristic");
    VERIFY_NON_NULL_RET(env, TAG, "env is null", NULL);
    VERIFY_NON_NULL_RET(bluetoothGatt, TAG, "bluetoothGatt is null", NULL);
    VERIFY_NON_NULL_RET(data, TAG, "data is null", NULL);

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(INFO, TAG, "BT adapter is not enabled");
        return NULL;
    }

    jstring jni_uuid = (*env)->NewStringUTF(env, OIC_GATT_CHARACTERISTIC_REQUEST_UUID);
    if (!jni_uuid)
    {
        OIC_LOG(ERROR, TAG, "jni_uuid is null");
        return NULL;
    }

    jobject jni_obj_GattCharacteristic = CALEClientGetGattService(env, bluetoothGatt, jni_uuid);
    if (!jni_obj_GattCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_GattCharacteristic is null");
        return NULL;
    }

    jclass jni_cid_BTGattCharacteristic = (*env)->FindClass(env, "android/bluetooth"
                                                            "/BluetoothGattCharacteristic");
    if (!jni_cid_BTGattCharacteristic)
    {
        OIC_LOG(ERROR, TAG, "jni_cid_BTGattCharacteristic is null");
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "set value in Characteristic");
    jmethodID jni_mid_setValue = (*env)->GetMethodID(env, jni_cid_BTGattCharacteristic, "setValue",
                                                     "([B)Z");
    if (!jni_mid_setValue)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_setValue is null");
        return NULL;
    }

    jboolean ret = (*env)->CallBooleanMethod(env, jni_obj_GattCharacteristic, jni_mid_setValue,
                                             data);
    if (JNI_TRUE == ret)
    {
        OIC_LOG(DEBUG, TAG, "the locally stored value has been set");
    }
    else
    {
        OIC_LOG(ERROR, TAG, "the locally stored value hasn't been set");
        return NULL;
    }

    // set Write Type
    jmethodID jni_mid_setWriteType = (*env)->GetMethodID(env, jni_cid_BTGattCharacteristic,
                                                         "setWriteType", "(I)V");
    if (!jni_mid_setWriteType)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_setWriteType is null");
        return NULL;
    }

    jfieldID jni_fid_no_response = (*env)->GetStaticFieldID(env, jni_cid_BTGattCharacteristic,
                                                            "WRITE_TYPE_NO_RESPONSE", "I");
    if (!jni_fid_no_response)
    {
        OIC_LOG(ERROR, TAG, "jni_fid_no_response is not available");
        return NULL;
    }

    jint jni_int_val = (*env)->GetStaticIntField(env, jni_cid_BTGattCharacteristic,
                                                 jni_fid_no_response);

    (*env)->CallVoidMethod(env, jni_obj_GattCharacteristic, jni_mid_setWriteType, jni_int_val);

    return jni_obj_GattCharacteristic;
}

jbyteArray CALEClientGetValueFromCharacteristic(JNIEnv *env, jobject characteristic)
{
    VERIFY_NON_NULL_RET(characteristic, TAG, "characteristic is null", NULL);
    VERIFY_NON_NULL_RET(env, TAG, "env is null", NULL);

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(INFO, TAG, "BT adapter is not enabled");
        return NULL;
    }

    jmethodID jni_mid_getValue  = CALEGetJNIMethodID(env, "android/bluetooth/"
                                                     "BluetoothGattCharacteristic",
                                                     "getValue", "()[B");
    if (!jni_mid_getValue)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_getValue is null");
        return NULL;
    }

    jbyteArray jni_obj_data_array = (*env)->CallObjectMethod(env, characteristic,
                                                             jni_mid_getValue);
    return jni_obj_data_array;
}

CAResult_t CALEClientCreateUUIDList()
{
    if (!g_jvm)
    {
        OIC_LOG(ERROR, TAG, "g_jvm is null");
        return CA_STATUS_FAILED;
    }

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    // create new object array
    jclass jni_cid_uuid_list = (*env)->FindClass(env, CLASSPATH_BT_UUID);
    if (!jni_cid_uuid_list)
    {
        OIC_LOG(ERROR, TAG, "jni_cid_uuid_list is null");
        goto error_exit;
    }

    jobjectArray jni_obj_uuid_list = (jobjectArray)(*env)->NewObjectArray(env, 1,
                                                                          jni_cid_uuid_list, NULL);
    if (!jni_obj_uuid_list)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_uuid_list is null");
        goto error_exit;
    }

    // make uuid list
    jobject jni_obj_uuid = CALEClientGetUUIDObject(env, OIC_GATT_SERVICE_UUID);
    if (!jni_obj_uuid)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_uuid is null");
        goto error_exit;
    }
    (*env)->SetObjectArrayElement(env, jni_obj_uuid_list, 0, jni_obj_uuid);

    g_uuidList = (jobjectArray)(*env)->NewGlobalRef(env, jni_obj_uuid_list);

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;

    // error label.
error_exit:

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
    return CA_STATUS_FAILED;
}

CAResult_t CALEClientSetUUIDToDescriptor(JNIEnv *env, jobject bluetoothGatt,
                                         jobject characteristic)
{
    VERIFY_NON_NULL(env, TAG, "env is null");
    VERIFY_NON_NULL(bluetoothGatt, TAG, "bluetoothGatt is null");
    VERIFY_NON_NULL(characteristic, TAG, "characteristic is null");

    if (!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(INFO, TAG, "BT adapter is not enabled");
        return CA_ADAPTER_NOT_ENABLED;
    }

    OIC_LOG(DEBUG, TAG, "CALEClientSetUUIDToDescriptor");
    jmethodID jni_mid_getDescriptor  = CALEGetJNIMethodID(env, "android/bluetooth/"
                                                          "BluetoothGattCharacteristic",
                                                          "getDescriptor",
                                                          "(Ljava/util/UUID;)Landroid/bluetooth/"
                                                          "BluetoothGattDescriptor;");
    if (!jni_mid_getDescriptor)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_getDescriptor is null");
        return CA_STATUS_FAILED;
    }

    jobject jni_obj_cc_uuid = CALEClientGetUUIDObject(env, OIC_GATT_CHARACTERISTIC_CONFIG_UUID);
    if (!jni_obj_cc_uuid)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_cc_uuid is null");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "request to get descriptor");
    jobject jni_obj_descriptor = (*env)->CallObjectMethod(env, characteristic,
                                                          jni_mid_getDescriptor, jni_obj_cc_uuid);
    if (!jni_obj_descriptor)
    {
        OIC_LOG(INFO, TAG, "jni_obj_descriptor is null");
        return CA_NOT_SUPPORTED;
    }

    OIC_LOG(DEBUG, TAG, "set value in descriptor");
    jclass jni_cid_descriptor = (*env)->FindClass(env,
                                                  "android/bluetooth/BluetoothGattDescriptor");
    if (!jni_cid_descriptor)
    {
        OIC_LOG(ERROR, TAG, "jni_cid_descriptor is null");
        return CA_STATUS_FAILED;
    }

    jmethodID jni_mid_setValue = (*env)->GetMethodID(env, jni_cid_descriptor, "setValue", "([B)Z");
    if (!jni_mid_setValue)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_setValue is null");
        return CA_STATUS_FAILED;
    }

    jfieldID jni_fid_NotiValue = (*env)->GetStaticFieldID(env, jni_cid_descriptor,
                                                          "ENABLE_NOTIFICATION_VALUE", "[B");
    if (!jni_fid_NotiValue)
    {
        OIC_LOG(ERROR, TAG, "jni_fid_NotiValue is null");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "get ENABLE_NOTIFICATION_VALUE");

    jboolean jni_setvalue = (*env)->CallBooleanMethod(
            env, jni_obj_descriptor, jni_mid_setValue,
            (jbyteArray)(*env)->GetStaticObjectField(env, jni_cid_descriptor, jni_fid_NotiValue));
    if (jni_setvalue)
    {
        OIC_LOG(DEBUG, TAG, "setValue success");
    }
    else
    {
        OIC_LOG(ERROR, TAG, "setValue has failed");
        return CA_STATUS_FAILED;
    }

    jmethodID jni_mid_writeDescriptor  = CALEGetJNIMethodID(env, "android/bluetooth/BluetoothGatt",
                                                            "writeDescriptor",
                                                            "(Landroid/bluetooth/"
                                                            "BluetoothGattDescriptor;)Z");
    if (!jni_mid_writeDescriptor)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_writeDescriptor is null");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "request to write descriptor");
    jboolean jni_ret = (*env)->CallBooleanMethod(env, bluetoothGatt, jni_mid_writeDescriptor,
                                                 jni_obj_descriptor);
    if (jni_ret)
    {
        OIC_LOG(DEBUG, TAG, "writeDescriptor success");
    }
    else
    {
        OIC_LOG(ERROR, TAG, "writeDescriptor has failed");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

void CALEClientCreateScanDeviceList(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CALEClientCreateScanDeviceList");
    VERIFY_NON_NULL_VOID(env, TAG, "env is null");

    ca_mutex_lock(g_deviceListMutex);
    // create new object array
    if (g_deviceList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Create device list");

        g_deviceList = u_arraylist_create();
    }
    ca_mutex_unlock(g_deviceListMutex);
}

CAResult_t CALEClientAddScanDeviceToList(JNIEnv *env, jobject device)
{
    VERIFY_NON_NULL(device, TAG, "device is null");
    VERIFY_NON_NULL(env, TAG, "env is null");

    ca_mutex_lock(g_deviceListMutex);

    if (!g_deviceList)
    {
        OIC_LOG(ERROR, TAG, "gdevice_list is null");

        CALEClientSetScanFlag(false);
        if(CA_STATUS_OK != CALEClientStopScan())
        {
            OIC_LOG(ERROR, TAG, "CALEClientStopScan has failed");
        }

        ca_mutex_unlock(g_deviceListMutex);
        return CA_STATUS_FAILED;
    }

    jstring jni_remoteAddress = CALEGetAddressFromBTDevice(env, device);
    if (!jni_remoteAddress)
    {
        OIC_LOG(ERROR, TAG, "jni_remoteAddress is null");
        ca_mutex_unlock(g_deviceListMutex);
        return CA_STATUS_FAILED;
    }

    const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);
    if (!remoteAddress)
    {
        OIC_LOG(ERROR, TAG, "remoteAddress is null");
        ca_mutex_unlock(g_deviceListMutex);
        return CA_STATUS_FAILED;
    }

    if (!CALEClientIsDeviceInScanDeviceList(env, remoteAddress))
    {
        jobject gdevice = (*env)->NewGlobalRef(env, device);
        u_arraylist_add(g_deviceList, gdevice);
        ca_cond_signal(g_deviceDescCond);
        OIC_LOG_V(DEBUG, TAG, "Added this BT Device[%s] in the List", remoteAddress);
    }
    (*env)->ReleaseStringUTFChars(env, jni_remoteAddress, remoteAddress);

    ca_mutex_unlock(g_deviceListMutex);

    return CA_STATUS_OK;
}

bool CALEClientIsDeviceInScanDeviceList(JNIEnv *env, const char* remoteAddress)
{
    VERIFY_NON_NULL_RET(env, TAG, "env is null", true);
    VERIFY_NON_NULL_RET(remoteAddress, TAG, "remoteAddress is null", true);

    if (!g_deviceList)
    {
        OIC_LOG(DEBUG, TAG, "g_deviceList is null");
        return true;
    }

    uint32_t length = u_arraylist_length(g_deviceList);
    for (uint32_t index = 0; index < length; index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            return true;
        }

        jstring jni_setAddress = CALEGetAddressFromBTDevice(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "jni_setAddress is null");
            return true;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        if (!setAddress)
        {
            OIC_LOG(ERROR, TAG, "setAddress is null");
            return true;
        }

        if (!strcmp(remoteAddress, setAddress))
        {
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
            return true;
        }

        (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
    }

    OIC_LOG(DEBUG, TAG, "there are no the device in list. we can add");

    return false;
}

CAResult_t CALEClientRemoveAllScanDevices(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CALEClientRemoveAllScanDevices");
    VERIFY_NON_NULL(env, TAG, "env is null");

    ca_mutex_lock(g_deviceListMutex);

    if (!g_deviceList)
    {
        OIC_LOG(ERROR, TAG, "g_deviceList is null");
        ca_mutex_unlock(g_deviceListMutex);
        return CA_STATUS_FAILED;
    }

    uint32_t length = u_arraylist_length(g_deviceList);
    for (uint32_t index = 0; index < length; index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            continue;
        }
        (*env)->DeleteGlobalRef(env, jarrayObj);
        jarrayObj = NULL;
    }

    OICFree(g_deviceList);
    g_deviceList = NULL;

    ca_mutex_unlock(g_deviceListMutex);
    return CA_STATUS_OK;
}

CAResult_t CALEClientRemoveDeviceInScanDeviceList(JNIEnv *env, jstring address)
{
    OIC_LOG(DEBUG, TAG, "CALEClientRemoveDeviceInScanDeviceList");
    VERIFY_NON_NULL(address, TAG, "address is null");
    VERIFY_NON_NULL(env, TAG, "env is null");

    ca_mutex_lock(g_deviceListMutex);

    if (!g_deviceList)
    {
        OIC_LOG(ERROR, TAG, "g_deviceList is null");
        ca_mutex_unlock(g_deviceListMutex);
        return CA_STATUS_FAILED;
    }

    uint32_t length = u_arraylist_length(g_deviceList);
    for (uint32_t index = 0; index < length; index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            ca_mutex_unlock(g_deviceListMutex);
            return CA_STATUS_FAILED;
        }

        jstring jni_setAddress = CALEGetAddressFromBTDevice(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "jni_setAddress is null");
            ca_mutex_unlock(g_deviceListMutex);
            return CA_STATUS_FAILED;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        if (!setAddress)
        {
            OIC_LOG(ERROR, TAG, "setAddress is null");
            ca_mutex_unlock(g_deviceListMutex);
            return CA_STATUS_FAILED;
        }

        const char* remoteAddress = (*env)->GetStringUTFChars(env, address, NULL);
        if (!remoteAddress)
        {
            OIC_LOG(ERROR, TAG, "remoteAddress is null");
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
            ca_mutex_unlock(g_deviceListMutex);
            return CA_STATUS_FAILED;
        }

        if (!strcmp(setAddress, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "remove object : %s", remoteAddress);
            (*env)->DeleteGlobalRef(env, jarrayObj);
            jarrayObj = NULL;
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
            (*env)->ReleaseStringUTFChars(env, address, remoteAddress);

            if (NULL == u_arraylist_remove(g_deviceList, index))
            {
                OIC_LOG(ERROR, TAG, "List removal failed.");
                ca_mutex_unlock(g_deviceListMutex);
                return CA_STATUS_FAILED;
            }
            ca_mutex_unlock(g_deviceListMutex);
            return CA_STATUS_OK;
        }
        (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
        (*env)->ReleaseStringUTFChars(env, address, remoteAddress);
    }

    ca_mutex_unlock(g_deviceListMutex);
    OIC_LOG(DEBUG, TAG, "There are no object in the device list");

    return CA_STATUS_OK;
}

/**
 * Gatt Object List
 */

CAResult_t CALEClientAddGattobjToList(JNIEnv *env, jobject gatt)
{
    OIC_LOG(INFO, TAG, "CALEClientAddGattobjToList");
    VERIFY_NON_NULL(env, TAG, "env is null");
    VERIFY_NON_NULL(gatt, TAG, "gatt is null");

    ca_mutex_lock(g_gattObjectMutex);

    if (!g_gattObjectList)
    {
        OIC_LOG(ERROR, TAG, "g_gattObjectList is not available");
        ca_mutex_unlock(g_gattObjectMutex);
        return CA_STATUS_FAILED;
    }

    jstring jni_remoteAddress = CALEClientGetAddressFromGattObj(env, gatt);
    if (!jni_remoteAddress)
    {
        OIC_LOG(ERROR, TAG, "jni_remoteAddress is null");
        ca_mutex_unlock(g_gattObjectMutex);
        return CA_STATUS_FAILED;
    }

    const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);
    if (!remoteAddress)
    {
        OIC_LOG(ERROR, TAG, "remoteAddress is null");
        ca_mutex_unlock(g_gattObjectMutex);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(INFO, TAG, "remote address : %s", remoteAddress);
    if (!CALEClientIsGattObjInList(env, remoteAddress))
    {
        jobject newGatt = (*env)->NewGlobalRef(env, gatt);
        u_arraylist_add(g_gattObjectList, newGatt);
        OIC_LOG(INFO, TAG, "Set GATT Object to Array as Element");
    }

    (*env)->ReleaseStringUTFChars(env, jni_remoteAddress, remoteAddress);
    ca_mutex_unlock(g_gattObjectMutex);
    return CA_STATUS_OK;
}

bool CALEClientIsGattObjInList(JNIEnv *env, const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "CALEClientIsGattObjInList");
    VERIFY_NON_NULL(env, TAG, "env is null");
    VERIFY_NON_NULL_RET(remoteAddress, TAG, "remoteAddress is null", true);

    uint32_t length = u_arraylist_length(g_gattObjectList);
    for (uint32_t index = 0; index < length; index++)
    {

        jobject jarrayObj = (jobject) u_arraylist_get(g_gattObjectList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            return true;
        }

        jstring jni_setAddress = CALEClientGetAddressFromGattObj(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "jni_setAddress is null");
            return true;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        if (!setAddress)
        {
            OIC_LOG(ERROR, TAG, "setAddress is null");
            return true;
        }

        if (!strcmp(remoteAddress, setAddress))
        {
            OIC_LOG(DEBUG, TAG, "the device is already set");
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
            return true;
        }
        else
        {
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
            continue;
        }
    }

    OIC_LOG(DEBUG, TAG, "There are no GATT object in list. it can be added");
    return false;
}

jobject CALEClientGetGattObjInList(JNIEnv *env, const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "CALEClientGetGattObjInList");
    VERIFY_NON_NULL_RET(env, TAG, "env is null", NULL);
    VERIFY_NON_NULL_RET(remoteAddress, TAG, "remoteAddress is null", NULL);

    ca_mutex_lock(g_gattObjectMutex);
    uint32_t length = u_arraylist_length(g_gattObjectList);
    for (uint32_t index = 0; index < length; index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_gattObjectList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            ca_mutex_unlock(g_gattObjectMutex);
            return NULL;
        }

        jstring jni_setAddress = CALEClientGetAddressFromGattObj(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "jni_setAddress is null");
            ca_mutex_unlock(g_gattObjectMutex);
            return NULL;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        if (!setAddress)
        {
            OIC_LOG(ERROR, TAG, "setAddress is null");
            ca_mutex_unlock(g_gattObjectMutex);
            return NULL;
        }

        if (!strcmp(remoteAddress, setAddress))
        {
            OIC_LOG(DEBUG, TAG, "the device is already set");
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
            ca_mutex_unlock(g_gattObjectMutex);
            return jarrayObj;
        }
        (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
    }

    ca_mutex_unlock(g_gattObjectMutex);
    OIC_LOG(DEBUG, TAG, "There are no the gatt object in list");
    return NULL;
}

CAResult_t CALEClientRemoveAllGattObjs(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CALEClientRemoveAllGattObjs");
    VERIFY_NON_NULL(env, TAG, "env is null");

    ca_mutex_lock(g_gattObjectMutex);
    if (!g_gattObjectList)
    {
        OIC_LOG(DEBUG, TAG, "already removed for g_gattObjectList");
        ca_mutex_unlock(g_gattObjectMutex);
        return CA_STATUS_OK;
    }

    uint32_t length = u_arraylist_length(g_gattObjectList);
    for (uint32_t index = 0; index < length; index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_gattObjectList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            continue;
        }
        (*env)->DeleteGlobalRef(env, jarrayObj);
        jarrayObj = NULL;
    }

    OICFree(g_gattObjectList);
    g_gattObjectList = NULL;
    OIC_LOG(INFO, TAG, "g_gattObjectList is removed");
    ca_mutex_unlock(g_gattObjectMutex);
    return CA_STATUS_OK;
}

CAResult_t CALEClientRemoveGattObj(JNIEnv *env, jobject gatt)
{
    OIC_LOG(DEBUG, TAG, "CALEClientRemoveGattObj");
    VERIFY_NON_NULL(gatt, TAG, "gatt is null");
    VERIFY_NON_NULL(env, TAG, "env is null");

    ca_mutex_lock(g_gattObjectMutex);
    if (!g_gattObjectList)
    {
        OIC_LOG(DEBUG, TAG, "already removed for g_gattObjectList");
        ca_mutex_unlock(g_gattObjectMutex);
        return CA_STATUS_OK;
    }

    uint32_t length = u_arraylist_length(g_gattObjectList);
    for (uint32_t index = 0; index < length; index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_gattObjectList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            ca_mutex_unlock(g_gattObjectMutex);
            return CA_STATUS_FAILED;
        }

        jstring jni_setAddress = CALEClientGetAddressFromGattObj(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "jni_setAddress is null");
            ca_mutex_unlock(g_gattObjectMutex);
            return CA_STATUS_FAILED;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        if (!setAddress)
        {
            OIC_LOG(ERROR, TAG, "setAddress is null");
            ca_mutex_unlock(g_gattObjectMutex);
            return CA_STATUS_FAILED;
        }

        jstring jni_remoteAddress = CALEClientGetAddressFromGattObj(env, gatt);
        if (!jni_remoteAddress)
        {
            OIC_LOG(ERROR, TAG, "jni_remoteAddress is null");
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
            ca_mutex_unlock(g_gattObjectMutex);
            return CA_STATUS_FAILED;
        }

        const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);
        if (!remoteAddress)
        {
            OIC_LOG(ERROR, TAG, "remoteAddress is null");
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
            ca_mutex_unlock(g_gattObjectMutex);
            return CA_STATUS_FAILED;
        }

        if (!strcmp(setAddress, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "remove object : %s", remoteAddress);
            (*env)->DeleteGlobalRef(env, jarrayObj);
            jarrayObj = NULL;
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
            (*env)->ReleaseStringUTFChars(env, jni_remoteAddress, remoteAddress);

            if (NULL == u_arraylist_remove(g_gattObjectList, index))
            {
                OIC_LOG(ERROR, TAG, "List removal failed.");
                ca_mutex_unlock(g_gattObjectMutex);
                return CA_STATUS_FAILED;
            }
            ca_mutex_unlock(g_gattObjectMutex);
            return CA_STATUS_OK;
        }
        (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
        (*env)->ReleaseStringUTFChars(env, jni_remoteAddress, remoteAddress);
    }

    ca_mutex_unlock(g_gattObjectMutex);
    OIC_LOG(DEBUG, TAG, "there are no target object");
    return CA_STATUS_OK;
}

CAResult_t CALEClientRemoveGattObjForAddr(JNIEnv *env, jstring addr)
{
    OIC_LOG(DEBUG, TAG, "CALEClientRemoveGattObjForAddr");
    VERIFY_NON_NULL(addr, TAG, "addr is null");
    VERIFY_NON_NULL(env, TAG, "env is null");

    ca_mutex_lock(g_gattObjectMutex);
    if (!g_gattObjectList)
    {
        OIC_LOG(DEBUG, TAG, "already removed for g_gattObjectList");
        ca_mutex_unlock(g_gattObjectMutex);
        return CA_STATUS_OK;
    }

    uint32_t length = u_arraylist_length(g_gattObjectList);
    for (uint32_t index = 0; index < length; index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_gattObjectList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            ca_mutex_unlock(g_gattObjectMutex);
            return CA_STATUS_FAILED;
        }

        jstring jni_setAddress = CALEClientGetAddressFromGattObj(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "jni_setAddress is null");
            ca_mutex_unlock(g_gattObjectMutex);
            return CA_STATUS_FAILED;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        if (!setAddress)
        {
            OIC_LOG(ERROR, TAG, "setAddress is null");
            ca_mutex_unlock(g_gattObjectMutex);
            return CA_STATUS_FAILED;
        }

        const char* remoteAddress = (*env)->GetStringUTFChars(env, addr, NULL);
        if (!remoteAddress)
        {
            OIC_LOG(ERROR, TAG, "remoteAddress is null");
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
            ca_mutex_unlock(g_gattObjectMutex);
            return CA_STATUS_FAILED;
        }

        if (!strcmp(setAddress, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "remove object : %s", remoteAddress);
            (*env)->DeleteGlobalRef(env, jarrayObj);
            jarrayObj = NULL;
            (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
            (*env)->ReleaseStringUTFChars(env, addr, remoteAddress);
            if (NULL == u_arraylist_remove(g_gattObjectList, index))
            {
                OIC_LOG(ERROR, TAG, "List removal failed.");
                ca_mutex_unlock(g_gattObjectMutex);
                return CA_STATUS_FAILED;
            }
            ca_mutex_unlock(g_gattObjectMutex);
            return CA_STATUS_OK;
        }
        (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);
        (*env)->ReleaseStringUTFChars(env, addr, remoteAddress);
    }

    ca_mutex_unlock(g_gattObjectMutex);
    OIC_LOG(DEBUG, TAG, "there are no target object");
    return CA_STATUS_FAILED;
}

jstring CALEClientGetLEAddressFromBTDevice(JNIEnv *env, jobject bluetoothDevice)
{
    OIC_LOG(DEBUG, TAG, "IN - CALEClientGetLEAddressFromBTDevice");

    VERIFY_NON_NULL_RET(env, TAG, "env", NULL);
    VERIFY_NON_NULL_RET(bluetoothDevice, TAG, "bluetoothDevice", NULL);

    // get Bluetooth Address
    jstring jni_btTargetAddress = CALEGetAddressFromBTDevice(env, bluetoothDevice);
    if (!jni_btTargetAddress)
    {
        OIC_LOG(ERROR, TAG, "CALEGetAddressFromBTDevice has failed");
        return NULL;
    }

    const char* targetAddress = (*env)->GetStringUTFChars(env, jni_btTargetAddress, NULL);
    if (!targetAddress)
    {
        OIC_LOG(ERROR, TAG, "targetAddress is not available");
        return NULL;
    }

    // get method ID of getDevice()
    jmethodID jni_mid_getDevice = CALEGetJNIMethodID(env, CLASSPATH_BT_GATT,
                                                     "getDevice", METHODID_BT_DEVICE);
    if (!jni_mid_getDevice)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_getDevice is null");
        (*env)->ReleaseStringUTFChars(env, jni_btTargetAddress, targetAddress);
        return NULL;
    }

    size_t length = u_arraylist_length(g_gattObjectList);
    for (size_t index = 0; index < length; index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_gattObjectList, index);
        if (!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            (*env)->ReleaseStringUTFChars(env, jni_btTargetAddress, targetAddress);
            return NULL;
        }

        OIC_LOG(DEBUG, TAG, "CALL API - bluetoothGatt.getDevice()");
        jobject jni_obj_device = (*env)->CallObjectMethod(env, jarrayObj, jni_mid_getDevice);
        if (!jni_obj_device)
        {
            OIC_LOG(ERROR, TAG, "jni_obj_device is null");
            (*env)->ReleaseStringUTFChars(env, jni_btTargetAddress, targetAddress);
            return NULL;
        }

        jstring jni_btAddress = CALEGetAddressFromBTDevice(env, jni_obj_device);
        if (!jni_btAddress)
        {
            OIC_LOG(ERROR, TAG, "CALEGetAddressFromBTDevice has failed");
            (*env)->ReleaseStringUTFChars(env, jni_btTargetAddress, targetAddress);
            return NULL;
        }

        const char* btAddress = (*env)->GetStringUTFChars(env, jni_btAddress, NULL);
        if (!btAddress)
        {
            OIC_LOG(ERROR, TAG, "btAddress is not available");
            (*env)->ReleaseStringUTFChars(env, jni_btTargetAddress, targetAddress);
            return NULL;
        }

        OIC_LOG_V(DEBUG, TAG, "targetAddress : %s", targetAddress);
        OIC_LOG_V(DEBUG, TAG, "btAddress : %s", btAddress);
        if (!strcmp(targetAddress, btAddress))
        {
            OIC_LOG(DEBUG, TAG, "Found Gatt object from BT device");

            // get LE address
            jstring jni_LEAddress = CALEClientGetAddressFromGattObj(env, jarrayObj);
            if (!jni_LEAddress)
            {
                OIC_LOG(ERROR, TAG, "jni_LEAddress is null");
            }
            (*env)->ReleaseStringUTFChars(env, jni_btTargetAddress, targetAddress);
            (*env)->ReleaseStringUTFChars(env, jni_btAddress, btAddress);
            (*env)->DeleteLocalRef(env, jni_btAddress);
            (*env)->DeleteLocalRef(env, jni_obj_device);
            return jni_LEAddress;
        }
        (*env)->ReleaseStringUTFChars(env, jni_btAddress, btAddress);
        (*env)->DeleteLocalRef(env, jni_btAddress);
        (*env)->DeleteLocalRef(env, jni_obj_device);
    }

    OIC_LOG(DEBUG, TAG, "OUT - CALEClientGetLEAddressFromBTDevice");
    return NULL;
}

/**
 * BT State List
 */

CAResult_t CALEClientUpdateDeviceState(const char* address, uint32_t connectedState,
                                       uint16_t notificationState, uint16_t sendState)
{
    VERIFY_NON_NULL(address, TAG, "address is null");

    CALEState_t *newstate = (CALEState_t*) OICCalloc(1, sizeof(*newstate));
    if (!newstate)
    {
        OIC_LOG(ERROR, TAG, "out of memory");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (strlen(address) > CA_MACADDR_SIZE)
    {
        OIC_LOG(ERROR, TAG, "address is not proper");
        OICFree(newstate);
        return CA_STATUS_FAILED;
    }

    OICStrcpy(newstate->address, sizeof(newstate->address), address);
    newstate->connectedState = connectedState;
    newstate->notificationState = notificationState;
    newstate->sendState = sendState;
    return CALEClientAddDeviceStateToList(newstate);
}

CAResult_t CALEClientAddDeviceStateToList(CALEState_t* state)
{
    VERIFY_NON_NULL(state, TAG, "state is null");

    ca_mutex_lock(g_deviceStateListMutex);

    if (!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "gdevice_list is null");
        ca_mutex_unlock(g_deviceStateListMutex);
        return CA_STATUS_FAILED;
    }

    if (CALEClientIsDeviceInList(state->address))
    {
        CALEState_t* curState = CALEClientGetStateInfo(state->address);
        if(!curState)
        {
            OIC_LOG(ERROR, TAG, "curState is null");
            ca_mutex_unlock(g_deviceStateListMutex);
            return CA_STATUS_FAILED;
        }

        if (STATE_CHARACTER_NO_CHANGE == state->notificationState)
        {
            state->notificationState = curState->notificationState;
        }
        state->autoConnectFlag = curState->autoConnectFlag;

        // delete previous state for update new state
        CAResult_t res = CALEClientRemoveDeviceState(state->address);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CALEClientRemoveDeviceState has failed");
            ca_mutex_unlock(g_deviceStateListMutex);
            return res;
        }
    }
    u_arraylist_add(g_deviceStateList, state); // update new state
    OIC_LOG_V(INFO, TAG, "Set State Info to List : %d, %d, %s, %d",
              state->connectedState, state->notificationState,
              state->address, state->autoConnectFlag);

    ca_mutex_unlock(g_deviceStateListMutex);
    return CA_STATUS_OK;
}

bool CALEClientIsDeviceInList(const char* remoteAddress)
{
    VERIFY_NON_NULL_RET(remoteAddress, TAG, "remoteAddress is null", false);

    if (!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "g_deviceStateList is null");
        return false;
    }

    uint32_t length = u_arraylist_length(g_deviceStateList);
    for (uint32_t index = 0; index < length; index++)
    {
        CALEState_t* state = (CALEState_t*) u_arraylist_get(g_deviceStateList, index);
        if (!state)
        {
            OIC_LOG(ERROR, TAG, "CALEState_t object is null");
            return false;
        }

        if (!strcmp(remoteAddress, state->address))
        {
            OIC_LOG(DEBUG, TAG, "the device is already set");
            return true;
        }
        else
        {
            continue;
        }
    }

    OIC_LOG(DEBUG, TAG, "there are no the device in list.");
    return false;
}

CAResult_t CALEClientRemoveAllDeviceState()
{
    OIC_LOG(DEBUG, TAG, "CALEClientRemoveAllDeviceState");

    ca_mutex_lock(g_deviceStateListMutex);
    if (!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "g_deviceStateList is null");
        ca_mutex_unlock(g_deviceStateListMutex);
        return CA_STATUS_FAILED;
    }

    uint32_t length = u_arraylist_length(g_deviceStateList);
    for (uint32_t index = 0; index < length; index++)
    {
        CALEState_t* state = (CALEState_t*) u_arraylist_get(g_deviceStateList, index);
        if (!state)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            continue;
        }
        OICFree(state);
    }

    OICFree(g_deviceStateList);
    g_deviceStateList = NULL;
    ca_mutex_unlock(g_deviceStateListMutex);

    return CA_STATUS_OK;
}

CAResult_t CALEClientResetDeviceStateForAll()
{
    OIC_LOG(DEBUG, TAG, "CALEClientResetDeviceStateForAll");

    ca_mutex_lock(g_deviceStateListMutex);
    if (!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "g_deviceStateList is null");
        ca_mutex_unlock(g_deviceStateListMutex);
        return CA_STATUS_FAILED;
    }

    size_t length = u_arraylist_length(g_deviceStateList);
    for (size_t index = 0; index < length; index++)
    {
        CALEState_t* state = (CALEState_t*) u_arraylist_get(g_deviceStateList, index);
        if (!state)
        {
            OIC_LOG(ERROR, TAG, "jarrayObj is null");
            continue;
        }

        // autoConnectFlag value will be not changed,
        // since it has reset only termination case.
        state->connectedState = STATE_DISCONNECTED;
        state->notificationState = STATE_CHARACTER_UNSET;
        state->sendState = STATE_SEND_NONE;
    }
    ca_mutex_unlock(g_deviceStateListMutex);

    return CA_STATUS_OK;
}

CAResult_t CALEClientRemoveDeviceState(const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "CALEClientRemoveDeviceState");
    VERIFY_NON_NULL(remoteAddress, TAG, "remoteAddress is null");

    if (!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "g_deviceStateList is null");
        return CA_STATUS_FAILED;
    }

    uint32_t length = u_arraylist_length(g_deviceStateList);
    for (uint32_t index = 0; index < length; index++)
    {
        CALEState_t* state = (CALEState_t*) u_arraylist_get(g_deviceStateList, index);
        if (!state)
        {
            OIC_LOG(ERROR, TAG, "CALEState_t object is null");
            continue;
        }

        if (!strcmp(state->address, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "remove state : %s", state->address);

            CALEState_t* targetState  = (CALEState_t*)u_arraylist_remove(g_deviceStateList,
                                                                         index);
            if (NULL == targetState)
            {
                OIC_LOG(ERROR, TAG, "List removal failed.");
                return CA_STATUS_FAILED;
            }

            OICFree(targetState);
            return CA_STATUS_OK;
        }
    }

    return CA_STATUS_OK;
}

CALEState_t* CALEClientGetStateInfo(const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "CALEClientGetStateInfo");
    VERIFY_NON_NULL_RET(remoteAddress, TAG, "remoteAddress is null", NULL);

    if (!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "g_deviceStateList is null");
        return NULL;
    }

    uint32_t length = u_arraylist_length(g_deviceStateList);
    OIC_LOG_V(DEBUG, TAG, "CALEClientGetStateInfo : %d", length);

    for (uint32_t index = 0; index < length; index++)
    {
        CALEState_t* state = (CALEState_t*) u_arraylist_get(g_deviceStateList, index);
        if (!state)
        {
            OIC_LOG(ERROR, TAG, "CALEState_t object is null");
            continue;
        }

        OIC_LOG_V(DEBUG, TAG, "target address : %s", remoteAddress);
        OIC_LOG_V(DEBUG, TAG, "state address : %s", state->address);

        if (!strcmp(state->address, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "get state : %s", remoteAddress);
            return state;
        }
    }
    return NULL;
}

bool CALEClientIsConnectedDevice(const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "CALEClientIsConnectedDevice");
    VERIFY_NON_NULL_RET(remoteAddress, TAG, "remoteAddress is null", false);

    ca_mutex_lock(g_deviceStateListMutex);
    if (!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "g_deviceStateList is null");
        ca_mutex_unlock(g_deviceStateListMutex);
        return false;
    }

    uint32_t length = u_arraylist_length(g_deviceStateList);
    for (uint32_t index = 0; index < length; index++)
    {
        CALEState_t* state = (CALEState_t*) u_arraylist_get(g_deviceStateList, index);
        if (!state)
        {
            OIC_LOG(ERROR, TAG, "CALEState_t object is null");
            continue;
        }

        if (!strcmp(state->address, remoteAddress))
        {
            OIC_LOG(DEBUG, TAG, "check whether it is connected or not");

            if (STATE_CONNECTED == state->connectedState)
            {
                ca_mutex_unlock(g_deviceStateListMutex);
                return true;
            }
            else
            {
                ca_mutex_unlock(g_deviceStateListMutex);
                return false;
            }
        }
    }
    ca_mutex_unlock(g_deviceStateListMutex);
    return false;
}

bool CALEClientIsSetCharacteristic(const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "CALEClientIsSetCharacteristic");
    VERIFY_NON_NULL_RET(remoteAddress, TAG, "remoteAddress is null", false);

    ca_mutex_lock(g_deviceStateListMutex);
    if (!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "g_deviceStateList is null");
        ca_mutex_unlock(g_deviceStateListMutex);
        return false;
    }

    uint32_t length = u_arraylist_length(g_deviceStateList);
    for (uint32_t index = 0; index < length; index++)
    {
        CALEState_t* state = (CALEState_t*) u_arraylist_get(g_deviceStateList, index);
        if (!state)
        {
            OIC_LOG(ERROR, TAG, "CALEState_t object is null");
            continue;
        }

        if (!strcmp(state->address, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "check whether it was set or not:%d", state->notificationState);

            if (STATE_CHARACTER_SET == state->notificationState)
            {
                ca_mutex_unlock(g_deviceStateListMutex);
                return true;
            }
            else
            {
                ca_mutex_unlock(g_deviceStateListMutex);
                return false;
            }
        }
    }

    ca_mutex_unlock(g_deviceStateListMutex);
    return false;
}

void CALEClientCreateDeviceList()
{
    OIC_LOG(DEBUG, TAG, "CALEClientCreateDeviceList");

    // create new object array
    if (!g_gattObjectList)
    {
        OIC_LOG(DEBUG, TAG, "Create g_gattObjectList");

        g_gattObjectList = u_arraylist_create();
    }

    if (!g_deviceStateList)
    {
        OIC_LOG(DEBUG, TAG, "Create g_deviceStateList");

        g_deviceStateList = u_arraylist_create();
    }

    if (!g_deviceList)
    {
        OIC_LOG(DEBUG, TAG, "Create g_deviceList");

        g_deviceList = u_arraylist_create();
    }
}

/**
 * Check Sent Count for remove g_sendBuffer
 */
void CALEClientUpdateSendCnt(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CALEClientUpdateSendCnt");

    VERIFY_NON_NULL_VOID(env, TAG, "env is null");
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

        CALEClientSetSendFinishFlag(true);
        OIC_LOG(DEBUG, TAG, "set signal for send data");
    }
    // mutex unlock
    ca_mutex_unlock(g_threadMutex);
}

CAResult_t CALEClientInitGattMutexVaraibles()
{
    if (NULL == g_bleReqRespClientCbMutex)
    {
        g_bleReqRespClientCbMutex = ca_mutex_new();
        if (NULL == g_bleReqRespClientCbMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleServerBDAddressMutex)
    {
        g_bleServerBDAddressMutex = ca_mutex_new();
        if (NULL == g_bleServerBDAddressMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_threadMutex)
    {
        g_threadMutex = ca_mutex_new();
        if (NULL == g_threadMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_threadSendMutex)
    {
        g_threadSendMutex = ca_mutex_new();
        if (NULL == g_threadSendMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_deviceListMutex)
    {
        g_deviceListMutex = ca_mutex_new();
        if (NULL == g_deviceListMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_gattObjectMutex)
    {
        g_gattObjectMutex = ca_mutex_new();
        if (NULL == g_gattObjectMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_deviceStateListMutex)
    {
        g_deviceStateListMutex = ca_mutex_new();
        if (NULL == g_deviceStateListMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_SendFinishMutex)
    {
        g_SendFinishMutex = ca_mutex_new();
        if (NULL == g_SendFinishMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_scanMutex)
    {
        g_scanMutex = ca_mutex_new();
        if (NULL == g_scanMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_threadWriteCharacteristicMutex)
    {
        g_threadWriteCharacteristicMutex = ca_mutex_new();
        if (NULL == g_threadWriteCharacteristicMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_deviceScanRetryDelayMutex)
    {
        g_deviceScanRetryDelayMutex = ca_mutex_new();
        if (NULL == g_deviceScanRetryDelayMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    return CA_STATUS_OK;
}

void CALEClientTerminateGattMutexVariables()
{
    ca_mutex_free(g_bleReqRespClientCbMutex);
    g_bleReqRespClientCbMutex = NULL;

    ca_mutex_free(g_bleServerBDAddressMutex);
    g_bleServerBDAddressMutex = NULL;

    ca_mutex_free(g_threadMutex);
    g_threadMutex = NULL;

    ca_mutex_free(g_threadSendMutex);
    g_threadSendMutex = NULL;

    ca_mutex_free(g_deviceListMutex);
    g_deviceListMutex = NULL;

    ca_mutex_free(g_SendFinishMutex);
    g_SendFinishMutex = NULL;

    ca_mutex_free(g_scanMutex);
    g_scanMutex = NULL;

    ca_mutex_free(g_threadWriteCharacteristicMutex);
    g_threadWriteCharacteristicMutex = NULL;

    ca_mutex_free(g_deviceScanRetryDelayMutex);
    g_deviceScanRetryDelayMutex = NULL;
}

void CALEClientSetSendFinishFlag(bool flag)
{
    OIC_LOG_V(DEBUG, TAG, "g_isFinishedSendData is %d", flag);

    ca_mutex_lock(g_SendFinishMutex);
    g_isFinishedSendData = flag;
    ca_mutex_unlock(g_SendFinishMutex);
}

/**
 * adapter common
 */

CAResult_t CAStartLEGattClient()
{
    // init mutex for send logic
    if (!g_deviceDescCond)
    {
        g_deviceDescCond = ca_cond_new();
    }

    if (!g_threadCond)
    {
        g_threadCond = ca_cond_new();
    }

    if (!g_threadWriteCharacteristicCond)
    {
        g_threadWriteCharacteristicCond = ca_cond_new();
    }

    CAResult_t ret = CALEClientStartScan();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALEClientStartScan has failed");
        return ret;
    }

    g_isStartedLEClient = true;
    return CA_STATUS_OK;
}

void CAStopLEGattClient()
{
    OIC_LOG(DEBUG, TAG, "CAStopBLEGattClient");

    if (!g_jvm)
    {
        OIC_LOG(ERROR, TAG, "g_jvm is null");
        return;
    }

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return;
        }
        isAttached = true;
    }

    CAResult_t ret = CALEClientDisconnectAll(env);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALEClientDisconnectAll has failed");
    }

    ret = CALEClientStopScan();
    if(CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CALEClientStopScan has failed");
    }

    ca_mutex_lock(g_threadMutex);
    OIC_LOG(DEBUG, TAG, "signal - connection cond");
    ca_cond_signal(g_threadCond);
    CALEClientSetSendFinishFlag(true);
    ca_mutex_unlock(g_threadMutex);

    ca_mutex_lock(g_threadWriteCharacteristicMutex);
    OIC_LOG(DEBUG, TAG, "signal - WriteCharacteristic cond");
    ca_cond_signal(g_threadWriteCharacteristicCond);
    ca_mutex_unlock(g_threadWriteCharacteristicMutex);

    ca_mutex_lock(g_deviceScanRetryDelayMutex);
    OIC_LOG(DEBUG, TAG, "signal - delay cond");
    ca_cond_signal(g_deviceScanRetryDelayCond);
    ca_mutex_unlock(g_deviceScanRetryDelayMutex);

    ca_cond_free(g_deviceDescCond);
    ca_cond_free(g_threadCond);
    ca_cond_free(g_threadWriteCharacteristicCond);
    ca_cond_free(g_deviceScanRetryDelayCond);

    g_deviceDescCond = NULL;
    g_threadCond = NULL;
    g_threadWriteCharacteristicCond = NULL;
    g_deviceScanRetryDelayCond = NULL;

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

}

CAResult_t CAInitializeLEGattClient()
{
    OIC_LOG(DEBUG, TAG, "Initialize GATT Client");
    CALEClientInitialize();
    return CA_STATUS_OK;
}

void CATerminateLEGattClient()
{
    OIC_LOG(DEBUG, TAG, "Terminate GATT Client");
    CAStopLEGattClient();
    CALEClientTerminate();
}

CAResult_t  CAUpdateCharacteristicsToGattServer(const char *remoteAddress, const uint8_t  *data,
                                                uint32_t dataLen, CALETransferType_t type,
                                                int32_t position)
{
    OIC_LOG(DEBUG, TAG, "call CALEClientSendUnicastMessage");
    VERIFY_NON_NULL(data, TAG, "data is null");
    VERIFY_NON_NULL(remoteAddress, TAG, "remoteAddress is null");

    if (LE_UNICAST != type || position < 0)
    {
        OIC_LOG(ERROR, TAG, "this request is not unicast");
        return CA_STATUS_INVALID_PARAM;
    }

    return CALEClientSendUnicastMessage(remoteAddress, data, dataLen);
}

CAResult_t CAUpdateCharacteristicsToAllGattServers(const uint8_t *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "call CALEClientSendMulticastMessage");
    VERIFY_NON_NULL(data, TAG, "data is null");

    return CALEClientSendMulticastMessage(data, dataLen);
}

void CASetLEReqRespClientCallback(CABLEDataReceivedCallback callback)
{
    ca_mutex_lock(g_bleReqRespClientCbMutex);
    g_CABLEClientDataReceivedCallback = callback;
    ca_mutex_unlock(g_bleReqRespClientCbMutex);
}

void CASetLEClientThreadPoolHandle(ca_thread_pool_t handle)
{
    g_threadPoolHandle = handle;
}

CAResult_t CAGetLEAddress(char **local_address)
{
    VERIFY_NON_NULL(local_address, TAG, "local_address");
    OIC_LOG(INFO, TAG, "CAGetLEAddress is not support");
    return CA_NOT_SUPPORTED;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caLeRegisterLeScanCallback(JNIEnv *env, jobject obj,
                                                                    jobject callback)
{
    OIC_LOG(DEBUG, TAG, "CaLeRegisterLeScanCallback");
    VERIFY_NON_NULL_VOID(env, TAG, "env is null");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj is null");
    VERIFY_NON_NULL_VOID(callback, TAG, "callback is null");

    g_leScanCallback = (*env)->NewGlobalRef(env, callback);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caLeRegisterGattCallback(JNIEnv *env, jobject obj,
                                                                  jobject callback)
{
    OIC_LOG(DEBUG, TAG, "CaLeRegisterGattCallback");
    VERIFY_NON_NULL_VOID(env, TAG, "env is null");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj is null");
    VERIFY_NON_NULL_VOID(callback, TAG, "callback is null");

    g_leGattCallback = (*env)->NewGlobalRef(env, callback);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caLeScanCallback(JNIEnv *env, jobject obj,
                                                          jobject device)
{
    VERIFY_NON_NULL_VOID(env, TAG, "env is null");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj is null");
    VERIFY_NON_NULL_VOID(device, TAG, "device is null");

    CAResult_t res = CALEClientAddScanDeviceToList(env, device);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "CALEClientAddScanDeviceToList has failed : %d", res);
    }
}

static jstring CALEClientGetAddressFromGatt(JNIEnv *env, jobject gatt)
{
    OIC_LOG(DEBUG, TAG, "IN - CAManagerGetAddressFromGatt");

    VERIFY_NON_NULL_RET(env, TAG, "env is null", NULL);
    VERIFY_NON_NULL_RET(gatt, TAG, "gatt is null", NULL);

    jmethodID jni_mid_getDevice = CALEGetJNIMethodID(env, CLASSPATH_BT_GATT,
                                                     "getDevice", METHODID_BT_DEVICE);
    if (!jni_mid_getDevice)
    {
        OIC_LOG(ERROR, TAG, "jni_mid_getDevice is null");
        return NULL;
    }

    jobject jni_obj_device = (*env)->CallObjectMethod(env, gatt, jni_mid_getDevice);
    if (!jni_obj_device)
    {
        OIC_LOG(ERROR, TAG, "jni_obj_device is null");
        return NULL;
    }

    jstring jni_address = CALEGetAddressFromBTDevice(env, jni_obj_device);
    if (!jni_address)
    {
        OIC_LOG(ERROR, TAG, "jni_address is null");
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "OUT - CAManagerGetAddressFromGatt");
    return jni_address;
}

/*
 * Class:     org_iotivity_ca_jar_caleinterface
 * Method:    CALeGattConnectionStateChangeCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;II)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caLeGattConnectionStateChangeCallback(JNIEnv *env,
                                                                                jobject obj,
                                                                                jobject gatt,
                                                                                jint status,
                                                                                jint newstate)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattConnectionStateChangeCallback - status %d, newstate %d", status,
            newstate);
    VERIFY_NON_NULL_VOID(env, TAG, "env is null");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj is null");
    VERIFY_NON_NULL_VOID(gatt, TAG, "gatt is null");

    jint state_connected = CALEGetConstantsValue(env, CLASSPATH_BT_PROFILE, "STATE_CONNECTED");
    jint state_disconnected = CALEGetConstantsValue(env, CLASSPATH_BT_PROFILE, "STATE_DISCONNECTED");
    jint gatt_success = CALEGetConstantsValue(env, CLASSPATH_BT_GATT, "GATT_SUCCESS");

    if (gatt_success == status && state_connected == newstate) // le connected
    {
        jstring jni_address = CALEClientGetAddressFromGattObj(env, gatt);
        if (!jni_address)
        {
            goto error_exit;
        }

        const char* address = (*env)->GetStringUTFChars(env, jni_address, NULL);
        if (address)
        {
            CAResult_t res = CALEClientUpdateDeviceState(address, STATE_CONNECTED,
                                                         STATE_CHARACTER_NO_CHANGE,
                                                         STATE_SEND_NONE);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "CALEClientUpdateDeviceState has failed");
                (*env)->ReleaseStringUTFChars(env, jni_address, address);
                goto error_exit;
            }
            OIC_LOG_V(INFO, TAG, "ConnectionStateCB - remote address : %s", address);

            (*env)->ReleaseStringUTFChars(env, jni_address, address);
        }

        CAResult_t res = CALEClientAddGattobjToList(env, gatt);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CALEClientAddGattobjToList has failed");
            goto error_exit;
        }

        res = CALEClientDiscoverServices(env, gatt);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CALEClientDiscoverServices has failed");
            goto error_exit;
        }
    }
    else if (state_disconnected == newstate) // le disconnected
    {
        jstring jni_address = CALEClientGetAddressFromGattObj(env, gatt);
        if (!jni_address)
        {
            OIC_LOG(ERROR, TAG, "CALEClientGetAddressFromGattObj has failed");
            goto error_exit;
        }

        const char* address = (*env)->GetStringUTFChars(env, jni_address, NULL);
        if (address)
        {
            CAResult_t res = CALEClientUpdateDeviceState(address, STATE_DISCONNECTED,
                                                                  STATE_CHARACTER_UNSET,
                                                                  STATE_SEND_NONE);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "CALEClientUpdateDeviceState has failed");
                (*env)->ReleaseStringUTFChars(env, jni_address, address);
                goto error_exit;
            }
            OIC_LOG_V(INFO, TAG, "ConnectionStateCB - remote address : %s", address);

            (*env)->ReleaseStringUTFChars(env, jni_address, address);
        }

        CAResult_t res = CALEClientGattClose(env, gatt);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CALEClientGattClose has failed");
        }

        if (GATT_ERROR == status)
        {
            // when we get GATT ERROR(0x85), gatt connection can be called again.
            OIC_LOG(INFO, TAG, "retry gatt connect");

            jstring leAddress = CALEClientGetAddressFromGatt(env, gatt);
            if (!leAddress)
            {
                OIC_LOG(ERROR, TAG, "CALEClientGetAddressFromGatt has failed");
                goto error_exit;
            }

            jobject btObject = CALEGetRemoteDevice(env, leAddress);
            if (!btObject)
            {
                OIC_LOG(ERROR, TAG, "CALEGetRemoteDevice has failed");
                goto error_exit;
            }

            jobject newGatt = CALEClientConnect(env, btObject, JNI_TRUE);
            if (!newGatt)
            {
                OIC_LOG(ERROR, TAG, "CALEClientConnect has failed");
                goto error_exit;
            }

            return;
        }
        else
        {
            if (CALECheckConnectionStateValue(status))
            {
                // this state is unexpected reason to disconnect
                // if the reason is suitable, connection logic of the device will be destroyed.
                OIC_LOG(INFO, TAG, "connection logic destroy");
                goto error_exit;
            }
            else
            {
                // other reason except for gatt_success is expected to running
                // background connection in BT platform.
                OIC_LOG(INFO, TAG, "unknown state or manual disconnected state");
                CALEClientUpdateSendCnt(env);
                return;
            }
        }

        if (g_sendBuffer)
        {
            (*env)->DeleteGlobalRef(env, g_sendBuffer);
            g_sendBuffer = NULL;
        }
    }
    return;

    // error label.
error_exit:

    CALEClientSendFinish(env, gatt);
    return;
}

/*
 * Class:     org_iotivity_ca_jar_caleinterface
 * Method:    CALeGattServicesDiscoveredCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caLeGattServicesDiscoveredCallback(JNIEnv *env,
                                                                             jobject obj,
                                                                             jobject gatt,
                                                                             jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattServicesDiscoveredCallback - status %d: ", status);
    VERIFY_NON_NULL_VOID(env, TAG, "env is null");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj is null");
    VERIFY_NON_NULL_VOID(gatt, TAG, "gatt is null");

    if (0 != status) // discovery error
    {
        CALEClientSendFinish(env, gatt);
        return;
    }

    jstring jni_address = CALEClientGetAddressFromGattObj(env, gatt);
    if (!jni_address)
    {
        CALEClientSendFinish(env, gatt);
        return;
    }

    const char* address = (*env)->GetStringUTFChars(env, jni_address, NULL);
    if (!address)
    {
        CALEClientSendFinish(env, gatt);
        return;
    }

    if (!CALEClientIsSetCharacteristic(address))
    {
        jstring jni_uuid = (*env)->NewStringUTF(env, OIC_GATT_CHARACTERISTIC_RESPONSE_UUID);
        if (!jni_uuid)
        {
            OIC_LOG(ERROR, TAG, "jni_uuid is null");
            goto error_exit;
        }

        jobject jni_obj_GattCharacteristic = CALEClientGetGattService(env, gatt, jni_uuid);
        if (!jni_obj_GattCharacteristic)
        {
            OIC_LOG(ERROR, TAG, "jni_obj_GattCharacteristic is null");
            goto error_exit;
        }

        CAResult_t res = CALEClientSetCharacteristicNotification(env, gatt,
                                                                 jni_obj_GattCharacteristic);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CALEClientSetCharacteristicNotification has failed");
            goto error_exit;
        }

        res = CALEClientSetUUIDToDescriptor(env, gatt, jni_obj_GattCharacteristic);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG_V(INFO, TAG, "Descriptor is not found : %d", res);
            if (g_sendBuffer)
            {
                CAResult_t res = CALEClientWriteCharacteristic(env, gatt);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "CALEClientWriteCharacteristic has failed");
                    goto error_exit;
                }
            }
        }

        res = CALEClientUpdateDeviceState(address, STATE_CONNECTED, STATE_CHARACTER_SET,
                                          STATE_SEND_NONE);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CALEClientUpdateDeviceState has failed");
            goto error_exit;
        }
    }
    else
    {
        if (g_sendBuffer)
        {
            CAResult_t res = CALEClientWriteCharacteristic(env, gatt);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "CALEClientWriteCharacteristic has failed");
                goto error_exit;
            }
        }
    }
    OIC_LOG(INFO, TAG, "ServicesDiscovery is successful");
    (*env)->ReleaseStringUTFChars(env, jni_address, address);
    return;

    // error label.
error_exit:
    OIC_LOG(ERROR, TAG, "ServicesDiscovery has failed");
    (*env)->ReleaseStringUTFChars(env, jni_address, address);
    CALEClientSendFinish(env, gatt);
    return;
}

/*
 * Class:     org_iotivity_ca_jar_caleinterface
 * Method:    CALeGattCharacteristicWritjclasseCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caLeGattCharacteristicWriteCallback(
        JNIEnv *env, jobject obj, jobject gatt, jbyteArray data,
        jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicWriteCallback - status : %d", status);
    VERIFY_NON_NULL_VOID(env, TAG, "env is null");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj is null");
    VERIFY_NON_NULL_VOID(gatt, TAG, "gatt is null");

    // send success & signal
    jstring jni_address = CALEClientGetAddressFromGattObj(env, gatt);
    if (!jni_address)
    {
        goto error_exit;
    }

    const char* address = (*env)->GetStringUTFChars(env, jni_address, NULL);
    if (!address)
    {
        goto error_exit;
    }

    jint gatt_success = CALEGetConstantsValue(env, CLASSPATH_BT_GATT, "GATT_SUCCESS");
    if (gatt_success != status) // error case
    {
        OIC_LOG(ERROR, TAG, "send failure");

        // retry to write
        CAResult_t res = CALEClientWriteCharacteristic(env, gatt);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "WriteCharacteristic has failed");
            ca_mutex_lock(g_threadWriteCharacteristicMutex);
            g_isSignalSetFlag = true;
            ca_cond_signal(g_threadWriteCharacteristicCond);
            ca_mutex_unlock(g_threadWriteCharacteristicMutex);

            CAResult_t res = CALEClientUpdateDeviceState(address, STATE_CONNECTED,
                                                         STATE_CHARACTER_SET,
                                                         STATE_SEND_FAILED);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "CALEClientUpdateDeviceState has failed");
            }

            if (g_clientErrorCallback)
            {
                jint length = (*env)->GetArrayLength(env, data);
                g_clientErrorCallback(address, data, length, CA_SEND_FAILED);
            }

            CALEClientSendFinish(env, gatt);
            goto error_exit;
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "send success");
        CAResult_t res = CALEClientUpdateDeviceState(address, STATE_CONNECTED, STATE_CHARACTER_SET,
                                                     STATE_SEND_SUCCESS);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CALEClientUpdateDeviceState has failed");
        }

        ca_mutex_lock(g_threadWriteCharacteristicMutex);
        OIC_LOG(DEBUG, TAG, "g_isSignalSetFlag is set true and signal");
        g_isSignalSetFlag = true;
        ca_cond_signal(g_threadWriteCharacteristicCond);
        ca_mutex_unlock(g_threadWriteCharacteristicMutex);

        CALEClientUpdateSendCnt(env);
    }

    (*env)->ReleaseStringUTFChars(env, jni_address, address);
    return;

    // error label.
error_exit:

    CALEClientSendFinish(env, gatt);
    return;
}

/*
 * Class:     org_iotivity_ca_jar_caleinterface
 * Method:    CALeGattCharacteristicChangedCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caLeGattCharacteristicChangedCallback(
        JNIEnv *env, jobject obj, jobject gatt, jbyteArray data)
{
    OIC_LOG(DEBUG, TAG, "CALeGattCharacteristicChangedCallback");
    VERIFY_NON_NULL_VOID(env, TAG, "env is null");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj is null");
    VERIFY_NON_NULL_VOID(gatt, TAG, "gatt is null");
    VERIFY_NON_NULL_VOID(data, TAG, "data is null");

    // get Byte Array and convert to uint8_t*
    jint length = (*env)->GetArrayLength(env, data);

    jboolean isCopy;
    jbyte *jni_byte_responseData = (jbyte*) (*env)->GetByteArrayElements(env, data, &isCopy);

    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicChangedCallback - raw data received : %p",
            jni_byte_responseData);

    uint8_t* receivedData = OICMalloc(length);
    if (!receivedData)
    {
        OIC_LOG(ERROR, TAG, "receivedData is null");
        return;
    }

    memcpy(receivedData, jni_byte_responseData, length);
    (*env)->ReleaseByteArrayElements(env, data, jni_byte_responseData, JNI_ABORT);

    jstring jni_address = CALEClientGetAddressFromGattObj(env, gatt);
    if (!jni_address)
    {
        OIC_LOG(ERROR, TAG, "jni_address is null");
        OICFree(receivedData);
        return;
    }

    const char* address = (*env)->GetStringUTFChars(env, jni_address, NULL);
    if (!address)
    {
        OIC_LOG(ERROR, TAG, "address is null");
        OICFree(receivedData);
        return;
    }

    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicChangedCallback - data. : %p, %d",
              receivedData, length);

    ca_mutex_lock(g_bleServerBDAddressMutex);
    uint32_t sentLength = 0;
    g_CABLEClientDataReceivedCallback(address, receivedData, length,
                                      &sentLength);
    ca_mutex_unlock(g_bleServerBDAddressMutex);

    (*env)->ReleaseStringUTFChars(env, jni_address, address);
}

/*
 * Class:     org_iotivity_ca_jar_caleinterface
 * Method:    CALeGattDescriptorWriteCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattDescriptor;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caLeGattDescriptorWriteCallback(JNIEnv *env, jobject obj,
                                                                         jobject gatt,
                                                                         jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattDescriptorWriteCallback - status %d: ", status);
    VERIFY_NON_NULL_VOID(env, TAG, "env is null");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj is null");
    VERIFY_NON_NULL_VOID(gatt, TAG, "gatt is null");

    jint gatt_success = CALEGetConstantsValue(env, CLASSPATH_BT_GATT, "GATT_SUCCESS");
    if (gatt_success != status) // error
    {
        goto error_exit;
    }

    if (g_sendBuffer)
    {
        CAResult_t res = CALEClientWriteCharacteristic(env, gatt);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CALEClientWriteCharacteristic has failed");
            goto error_exit;
        }
    }
    return;

// error label.
error_exit:

    CALEClientSendFinish(env, gatt);
    return;
}
