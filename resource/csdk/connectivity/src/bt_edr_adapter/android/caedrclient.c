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

#include "caedrinterface.h"
#include "caedrutils.h"
#include "caedrclient.h"
#include "caleserver.h"
#include "logger.h"
#include "oic_malloc.h"
#include "uthreadpool.h" /* for thread pool */
#include "umutex.h"
#include "uarraylist.h"
#include "caadapterutils.h"

//#define DEBUG_MODE
#define TAG PCF("CA_EDR_CLIENT")

static const char *METHODID_OBJECTNONPARAM = "()Landroid/bluetooth/BluetoothAdapter;";
static const char *METHODID_STRINGNONPARAM = "()Ljava/lang/String;";
static const char *CLASSPATH_BT_ADPATER = "android/bluetooth/BluetoothAdapter";
static const char *CLASSPATH_BT_UUID = "java/util/UUID";

static const uint32_t MAX_PDU_BUFFER = 1024;

static u_arraylist_t *g_deviceStateList = NULL;
static u_arraylist_t *g_deviceObjectList = NULL;

static u_thread_pool_t g_threadPoolHandle = NULL;

static JavaVM *g_jvm;
static jobject g_context;

static jbyteArray g_sendBuffer;

/**
 * @var g_mutexSocketListManager
 * @brief Mutex to synchronize socket list update
 */
static u_mutex g_mutexSocketListManager;

// server socket instance
static jobject g_serverSocketObject = NULL;

/**
 * @var g_mutexUnicastServer
 * @brief Mutex to synchronize unicast server
 */
static u_mutex g_mutexUnicastServer = NULL;

/**
 * @var g_stopUnicast
 * @brief Flag to control the Receive Unicast Data Thread
 */
static bool g_stopUnicast = false;

/**
 * @var g_mutexMulticastServer
 * @brief Mutex to synchronize secure multicast server
 */
static u_mutex g_mutexMulticastServer = NULL;

/**
 * @var g_stopMulticast
 * @brief Flag to control the Receive Multicast Data Thread
 */
static bool g_stopMulticast = false;

/**
 * @var g_stopAccept
 * @brief Flag to control the Accept Thread
 */
static bool g_stopAccept = false;

typedef struct send_data {
    char* address;
    char* data;
    uint32_t id;
} data_t;

/**
 @brief Thread context information for unicast, multicast and secured unicast server
 */
typedef struct
{
    bool *stopFlag;
    CAAdapterServerType_t type;
} CAAdapterReceiveThreadContext_t;

typedef struct
{
    bool *stopFlag;
} CAAdapterAcceptThreadContext_t;


// TODO: It will be updated when android EDR support is added
CAResult_t CAEDRGetInterfaceInformation(CALocalConnectivity_t **info)
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRClientTerminate()
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAEDRTerminate();
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAEDRManagerReadData(void)
{
    OIC_LOG(DEBUG, TAG, "IN");
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_NOT_SUPPORTED;
}

CAResult_t CAEDRClientSendUnicastData(const char *remoteAddress, const char *serviceUUID,
                                      const void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAEDRSendUnicastMessage(remoteAddress, (const char*) data, dataLength);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}


CAResult_t CAEDRClientSendMulticastData(const char *serviceUUID, const void *data,
                                        uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAEDRSendMulticastMessage((const char*) data, dataLength);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

// TODO: It will be updated when android EDR support is added
void CAEDRClientUnsetCallbacks(void)
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG(DEBUG, TAG, "OUT");
}

// TODO: It will be updated when android EDR support is added
void CAEDRClientDisconnectAll(void)
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAEDRGetAdapterEnableState(bool *state)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (!g_jvm)
    {
        OIC_LOG(ERROR, TAG, "g_jvm is null");
        return CA_STATUS_INVALID_PARAM;
    }
    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "CAEDRGetAdapterEnableState - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG(DEBUG, TAG, "AttachCurrentThread failed");
            return CA_STATUS_INVALID_PARAM;
        }
        isAttached = JNI_TRUE;
    }
    jboolean ret = CAEDRNativeIsEnableBTAdapter(env);
    if(ret)
    {
        *state = true;
    }
    else
    {
        *state = false;
    }

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//FIXME getting context

void CAEDRJniSetContext(jobject context)
{
    OIC_LOG(DEBUG, TAG, "caedrSetObject");

    if (!context)
    {
        OIC_LOG(ERROR, TAG, "context is null");
        return;
    }
    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "CAEDRInitialize - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = JNI_TRUE;
    }

    g_context = (*env)->NewGlobalRef(env, context);

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);
}

void CAEDRCreateJNIInterfaceObject(jobject context)
{
    JNIEnv* env;
    OIC_LOG(DEBUG, TAG, "[EDRCore] CAEDRCreateJNIInterfaceObject");

    if ((*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "[EDRCore] Could not get JNIEnv pointer");
        return;
    }

    //getApplicationContext
    jclass contextClass = (*env)->FindClass(env, "android/content/Context");
    if (contextClass == 0)
    {
        OIC_LOG(DEBUG, TAG, "[EDRCore] Could not get context object class");
        return;
    }

    jmethodID getApplicationContextMethod = (*env)->GetMethodID(env, contextClass,
            "getApplicationContext", "()Landroid/content/Context;");
    if (getApplicationContextMethod == 0)
    {
        OIC_LOG(DEBUG, TAG, "[EDRCore] Could not get getApplicationContext method");
        return;
    }

    jobject gApplicationContext = (*env)->CallObjectMethod(env, context,
            getApplicationContextMethod);
    OIC_LOG_V(DEBUG, TAG,
            "[WIFICore] Saving Android application context object %p", gApplicationContext);

   //Create WiFiInterface instance
    jclass WiFiJniInterface = (*env)->FindClass(env, "com/iotivity/jar/CAEDRInterface");
    if (!WiFiJniInterface)
    {
        OIC_LOG(DEBUG, TAG, "[EDRCore] Could not get CAWiFiInterface class");
        return;
    }

    jmethodID WiFiInterfaceConstructorMethod = (*env)->GetMethodID(env,
            WiFiJniInterface, "<init>", "(Landroid/content/Context;)V");
    if (!WiFiInterfaceConstructorMethod)
    {
        OIC_LOG(ERROR, TAG, "[EDRCore] Could not get CAWiFiInterface constructor method");
        return;
    }

    (*env)->NewObject(env, WiFiJniInterface, WiFiInterfaceConstructorMethod, gApplicationContext);
    OIC_LOG(DEBUG, TAG, "[EDRCore] Create CAWiFiInterface instance");
    OIC_LOG(DEBUG, TAG, "[EDRCore] NewObject Success");

}

static void CAEDRDestroyMutex()
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (g_mutexUnicastServer)
    {
        u_mutex_free(g_mutexUnicastServer);
        g_mutexUnicastServer = NULL;
    }

    if (g_mutexMulticastServer)
    {
        u_mutex_free(g_mutexMulticastServer);
        g_mutexMulticastServer = NULL;
    }

    if(g_mutexSocketListManager)
    {
        u_mutex_free(g_mutexSocketListManager);
        g_mutexSocketListManager = NULL;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

static CAResult_t CAEDRCreateMutex()
{
    OIC_LOG(DEBUG, TAG, "IN");

    g_mutexUnicastServer = u_mutex_new();
    if (!g_mutexUnicastServer)
    {
        OIC_LOG(ERROR, TAG, "Failed to created mutex!");
        return CA_STATUS_FAILED;
    }

    g_mutexMulticastServer = u_mutex_new();
    if (!g_mutexMulticastServer)
    {
        OIC_LOG(ERROR, TAG, "Failed to created mutex!");

        CAEDRDestroyMutex();
        return CA_STATUS_FAILED;
    }

    g_mutexSocketListManager = u_mutex_new();
    if (!g_mutexSocketListManager)
    {
        OIC_LOG(ERROR, TAG, "Failed to created mutex!");

        CAEDRDestroyMutex();
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRInitialize(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "CAEDRInitialize");

    g_threadPoolHandle = handle;

    // init mutex
    CAEDRCreateMutex();

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "CAEDRInitialize - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = JNI_TRUE;
    }

    jstring jni_address = CAEDRNativeGetLocalDeviceAddress(env);
    if(jni_address)
    {
        const char* localAddress = (*env)->GetStringUTFChars(env, jni_address, NULL);
        OIC_LOG_V(DEBUG, TAG, "My BT Address is %s", localAddress);
    }

    CAEDRNativeCreateDeviceStateList();
    CAEDRNativeCreateDeviceSocketList();

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

 //   CAEDRCreateJNIInterfaceObject(gContext); /* create java CAEDRInterface instance*/

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAEDRTerminate()
{
    OIC_LOG(DEBUG, TAG, "CAEDRTerminate");

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "CAEDRTerminate - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = JNI_TRUE;
    }

    g_stopAccept = true;
    g_stopMulticast = true;
    g_stopUnicast = true;

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    if(g_context)
    {
        (*env)->DeleteGlobalRef(env, g_context);
    }
    // delete mutex
    CAEDRDestroyMutex();

    CAEDRNativeRemoveAllDeviceState();
    CAEDRNativeRemoveAllDeviceSocket(env);
}

void CAEDRCoreJniInit(JNIEnv *env, JavaVM *jvm)
{
    OIC_LOG(DEBUG, TAG, "CAEdrClientJniInit");
    g_jvm = jvm;

    CAEDRServerJniInit(env, jvm);
}

CAResult_t CAEDRSendUnicastMessage(const char* address, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRSendUnicastMessage(%s, %s)", address, data);

    CAEDRSendUnicastMessageImpl(address, data, dataLen);
    return CA_STATUS_OK;
}

CAResult_t CAEDRSendMulticastMessage(const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRSendMulticastMessage(%s)", data);

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "CAEDRSendMulticastMessage - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG(DEBUG, TAG, "AttachCurrentThread failed");
            return CA_STATUS_INVALID_PARAM;
        }
        isAttached = JNI_TRUE;
    }

    CAEDRSendMulticastMessageImpl(env, data, dataLen);

    OIC_LOG(DEBUG, TAG, "sent data");

    if(isAttached)
    {
        OIC_LOG(DEBUG, TAG, "DetachCurrentThread");
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    OIC_LOG(DEBUG, TAG, "OUT - CAEDRSendMulticastMessage");
    return CA_STATUS_OK;
}

CAResult_t CAEDRGetInterfaceInfo(char **address)
{
    CAEDRGetLocalAddress(address);
    return CA_STATUS_OK;
}

void CAEDRGetLocalAddress(char **address)
{
    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "CAEDRGetLocalAddress - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
        if(res != JNI_OK)
        {
            OIC_LOG(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = JNI_TRUE;
    }

    jstring jni_address = CAEDRNativeGetLocalDeviceAddress(env);
    if(jni_address)
    {
        const char* localAddress = (*env)->GetStringUTFChars(env, jni_address, NULL);
        *address = (char*)OICMalloc(strlen(localAddress) + 1);
        if (*address == NULL)
        {
            if (isAttached)
                (*g_jvm)->DetachCurrentThread(g_jvm);
            return;
        }
        memcpy(*address, localAddress, strlen(localAddress));
    }

    OIC_LOG_V(DEBUG, TAG, "Local Address : %s", *address);
    if (isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);
}

CAResult_t CAEDRSendUnicastMessageImpl(const char* address, const char* data,
    uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRSendUnicastMessageImpl, address: %s, data: %s", address, data);

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "CAEDRSendUnicastMessageImpl - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
        if(res != JNI_OK)
        {
            OIC_LOG(DEBUG, TAG, "AttachCurrentThread failed");
            return CA_STATUS_INVALID_PARAM;
        }
        isAttached = TRUE;
    }

    OIC_LOG(DEBUG, TAG, "[EDR][Native] set byteArray for data");
    if(g_sendBuffer)
    {
        (*env)->DeleteGlobalRef(env, g_sendBuffer);
    }
    jbyteArray jni_arr = (*env)->NewByteArray(env, dataLen);
    (*env)->SetByteArrayRegion(env, jni_arr, 0, dataLen, (jbyte*)data);
    g_sendBuffer = (jbyteArray)(*env)->NewGlobalRef(env, jni_arr);

    // get bonded device list
    jobjectArray jni_arrayPairedDevices = CAEDRNativeGetBondedDevices(env);
    if(!jni_arrayPairedDevices)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_arrayPairedDevices is empty");
        if (isAttached)
            (*g_jvm)->DetachCurrentThread(g_jvm);
        return CA_STATUS_INVALID_PARAM;
    }
    // Get information from array of devices
    jsize length = (*env)->GetArrayLength(env, jni_arrayPairedDevices);
    jsize i;
    for( i = 0 ; i < length ; i++ )
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] start to check device");
        // get name, address from BT device
        jobject j_obj_device = (*env)->GetObjectArrayElement(env, jni_arrayPairedDevices, i);

        jclass jni_cid_BTDevice = (*env)->FindClass(env, "android/bluetooth/BluetoothDevice");
        jmethodID j_mid_getName = (*env)->GetMethodID(env, jni_cid_BTDevice,
                "getName", "()Ljava/lang/String;");
        jmethodID j_mid_getAddress = (*env)->GetMethodID(env, jni_cid_BTDevice,
                "getAddress", "()Ljava/lang/String;");

        jstring j_str_name = (*env)->CallObjectMethod(env, j_obj_device, j_mid_getName);

        if(j_str_name)
        {
            const char * name = (*env)->GetStringUTFChars(env, j_str_name, NULL);
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] getBondedDevices: ~~device name is %s", name);
            (*env)->ReleaseStringUTFChars(env, j_str_name, name);
        }

        jstring j_str_address = (*env)->CallObjectMethod(env, j_obj_device, j_mid_getAddress);
        const char * remoteAddress = (*env)->GetStringUTFChars(env, j_str_address, NULL);
        OIC_LOG_V(DEBUG, TAG,
                "[EDR][Native] getBondedDevices: ~~device address is %s", remoteAddress);

        if (!remoteAddress) {
            OIC_LOG(ERROR, TAG, "[EDR][Native] remoteAddress is null");
            if (isAttached)
                (*g_jvm)->DetachCurrentThread(g_jvm);
            return CA_STATUS_INVALID_PARAM;
        }
        if (!address) {
            OIC_LOG(ERROR, TAG, "[EDR][Native] address is null");
            if (isAttached)
                (*g_jvm)->DetachCurrentThread(g_jvm);
            return CA_STATUS_INVALID_PARAM;
        }
        // find address
        if(!strcmp(remoteAddress, address))
        {
            CAEDRNativeSendData(env, remoteAddress, data, i);
        }
    }

    if (isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    return CA_STATUS_OK;
}

CAResult_t CAEDRSendMulticastMessageImpl(JNIEnv *env, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CASendMulticastMessageImpl, send to, data: %s, %d", data, dataLen);

    // get bonded device list
    jobjectArray jni_arrayPairedDevices = CAEDRNativeGetBondedDevices(env);
    if(!jni_arrayPairedDevices)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_arrayPairedDevices is empty");
        return CA_STATUS_INVALID_PARAM;
    }
    // Get information from array of devices
    jsize length = (*env)->GetArrayLength(env, jni_arrayPairedDevices);
    jsize i;
    for( i = 0 ; i < length ; i++ )
    {
        // get name, address from BT device
        jobject j_obj_device = (*env)->GetObjectArrayElement(env, jni_arrayPairedDevices, i);

        jclass jni_cid_BTDevice = (*env)->FindClass(env, "android/bluetooth/BluetoothDevice");
        jmethodID j_mid_getName = (*env)->GetMethodID(env, jni_cid_BTDevice,
                "getName", "()Ljava/lang/String;");
        jmethodID j_mid_getAddress = (*env)->GetMethodID(env, jni_cid_BTDevice,
                "getAddress", "()Ljava/lang/String;");

        jstring j_str_name = (*env)->CallObjectMethod(env, j_obj_device, j_mid_getName);

        if(j_str_name)
        {
            const char * name = (*env)->GetStringUTFChars(env, j_str_name, NULL);
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] getBondedDevices: ~~device name is %s", name);
            (*env)->ReleaseStringUTFChars(env, j_str_name, name);
        }

        jstring j_str_address = (*env)->CallObjectMethod(env, j_obj_device, j_mid_getAddress);
        const char * remoteAddress = (*env)->GetStringUTFChars(env, j_str_address, NULL);
        OIC_LOG_V(DEBUG, TAG,
                "[EDR][Native] getBondedDevices: ~~device address is %s", remoteAddress);

        // find address
        CAEDRNativeSendData(env, remoteAddress, data, i);
    }

    return CA_STATUS_OK;
}

/**
 * EDR Method
 */
void CAEDRNativeSendData(JNIEnv *env, const char *address, const char *data, uint32_t id)
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData logic start");

    if(STATE_DISCONNECTED == CAEDRIsConnectedDevice(address))
    {
        // connect before send data
        OIC_LOG(DEBUG, TAG, "[EDR][Native] connect before send data");

        if(NULL == address)
        {
            OIC_LOG(ERROR, TAG, "[EDR][Native] remote address is empty");
            return;
        }
        else
        {
            CAEDRNativeConnect(env, address, id);
        }
    }

    if(STATE_CONNECTED == CAEDRIsConnectedDevice(address))
    {
        if(!((*env)->ExceptionCheck(env)))
        {
            jclass jni_cid_BTsocket = (*env)->FindClass(env, "android/bluetooth/BluetoothSocket");
            if(!jni_cid_BTsocket)
            {
                OIC_LOG(ERROR, TAG, "[EDR][Native] btSendData: jni_cid_BTsocket is null");
                return;
            }

            jmethodID jni_mid_getOutputStream = (*env)->GetMethodID(env,
                    jni_cid_BTsocket, "getOutputStream",
                    "()Ljava/io/OutputStream;");
            if(!jni_mid_getOutputStream)
            {
                OIC_LOG(ERROR, TAG, "[EDR][Native] btSendData: jni_mid_getOutputStream is null");
                return;
            }
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btSendData: Get MethodID for i/o stream..%d", id);

            jobject jni_obj_socket = CAEDRNativeGetDeviceSocket(id);
            if(!jni_obj_socket)
            {
                OIC_LOG(ERROR, TAG, "[EDR][Native] btSendData: jni_socket is not available");
                return;
            }

            jobject jni_obj_outputStream = (*env)->CallObjectMethod(env,
                    jni_obj_socket, jni_mid_getOutputStream);
            if(!jni_obj_outputStream)
            {
                OIC_LOG(ERROR, TAG, "[EDR][Native] btSendData: jni_obj_outputStream is null");
                return;
            }

            OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: ready outputStream..");

            jclass jni_cid_OutputStream = (*env)->FindClass(env, "java/io/OutputStream");
            if(!jni_cid_OutputStream)
            {
                OIC_LOG(ERROR, TAG, "[EDR][Native] btSendData: jni_cid_OutputStream is null");
                return;
            }

            jmethodID jni_mid_write = (*env)->GetMethodID(env,
                    jni_cid_OutputStream, "write", "([BII)V");
            if(!jni_mid_write)
            {
                OIC_LOG(ERROR, TAG, "[EDR][Native] btSendData: jni_mid_write is null");
                return;
            }

            jbyteArray jbuf;
            int length = strlen(data);
            jbuf = (*env)->NewByteArray(env, length);
            (*env)->SetByteArrayRegion(env, jbuf, 0, length, (jbyte*)data);

            (*env)->CallVoidMethod(env, jni_obj_outputStream, jni_mid_write,
                    jbuf, (jint) 0, (jint) length);

            if((*env)->ExceptionCheck(env))
            {
                OIC_LOG(ERROR, TAG, "[EDR][Native] btSendData: Write Error!!!");
                (*env)->ExceptionDescribe(env);
                (*env)->ExceptionClear(env);
                return;
            }

            OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: Write Success");

            // remove socket to list
            CAEDRNativeRemoveDeviceSocket(env, jni_obj_socket);

            // update state
            CAEDRUpdateDeviceState(STATE_DISCONNECTED, address);
        }
        else
        {
            (*env)->ExceptionDescribe(env);
            (*env)->ExceptionClear(env);
            OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: error!!");
            return;
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: BT connection is not completed!!");
    }
}

void CAEDRNativeConnect(JNIEnv *env, const char *address, uint32_t id)
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect..");

    jclass jni_cid_BTAdapter = (*env)->FindClass(env, CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_cid_BTAdapter is null");
        return;
    }

    // get BTadpater
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env,
            jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);
    if(!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_mid_getDefaultAdapter is null");
        return;
    }

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env,
            jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_obj_BTAdapter is null");
        return;
    }

    // get remote bluetooth device
    jmethodID jni_mid_getRemoteDevice = (*env)->GetMethodID(env,
            jni_cid_BTAdapter, "getRemoteDevice",
            "(Ljava/lang/String;)Landroid/bluetooth/BluetoothDevice;");
    if(!jni_mid_getRemoteDevice)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_mid_getRemoteDevice is null");
        return;
    }

    //jstring jni_address = (*env)->NewStringUTF(env, "B8:5E:7B:54:52:1C");
    jstring jni_address = (*env)->NewStringUTF(env, address);
    jobject jni_obj_remoteBTDevice = (*env)->CallObjectMethod(env,
            jni_obj_BTAdapter, jni_mid_getRemoteDevice, jni_address);
    if(!jni_obj_remoteBTDevice)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_obj_remoteBTDevice is null");
        return;
    }

    // get create Rfcomm Socket method ID
    jclass jni_cid_BluetoothDevice = (*env)->FindClass(env, "android/bluetooth/BluetoothDevice");
    if(!jni_cid_BluetoothDevice)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_cid_BluetoothDevice is null");
        return;
    }

    jmethodID jni_mid_createSocket = (*env)->GetMethodID(env,
            jni_cid_BluetoothDevice,
            "createInsecureRfcommSocketToServiceRecord",
            "(Ljava/util/UUID;)Landroid/bluetooth/BluetoothSocket;");
    if(!jni_mid_createSocket) {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_mid_createSocket is null");
        return;
    }

    // createInsecureRfcommSocketToServiceRecord / createRfcommSocketToServiceRecord
    // setting UUID
    jclass jni_cid_uuid = (*env)->FindClass(env, CLASSPATH_BT_UUID);
    if(!jni_cid_uuid)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_cid_uuid is null");
        return;
    }

    jmethodID jni_mid_fromString = (*env)->GetStaticMethodID(env, jni_cid_uuid, "fromString",
            "(Ljava/lang/String;)Ljava/util/UUID;");
    if(!jni_mid_fromString)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_mid_fromString is null");
        return;
    }

    jobject jni_obj_uuid = (*env)->CallStaticObjectMethod(env, jni_cid_uuid,
            jni_mid_fromString, OIC_EDR_SERVICE_ID);
    if(!jni_obj_uuid)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_obj_uuid is null");
        return;
    }
    // create socket
    jobject jni_obj_BTSocket = (*env)->CallObjectMethod(env,
            jni_obj_remoteBTDevice, jni_mid_createSocket, jni_obj_uuid);
    if(!jni_obj_BTSocket)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_obj_BTSocket is null");
        return;
    }

    // connect
    jclass jni_cid_BTSocket = (*env)->FindClass(env, "android/bluetooth/BluetoothSocket");
    if(!jni_cid_BTSocket)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_cid_BTSocket is null");
        return;
    }

    jmethodID jni_mid_connect = (*env)->GetMethodID(env, jni_cid_BTSocket, "connect", "()V");
    if(!jni_mid_connect)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_mid_connect is null");
        return;
    }

    OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: initiating connection...");
    (*env)->CallVoidMethod(env, jni_obj_BTSocket, jni_mid_connect);

    if((*env)->ExceptionCheck(env))
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: Connect is Failed!!!");
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return;
    }

    // set socket to list
    jobject jni_socket = (*env)->NewGlobalRef(env, jni_obj_BTSocket);
    CAEDRNativeAddDeviceSocketToList(env, jni_socket);

    // update state
    CAEDRUpdateDeviceState(STATE_CONNECTED, address);

    OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: connected");
}

void CAEDRNativeSocketClose(JNIEnv *env, const char *address, uint32_t id)
{

    jclass jni_cid_BTSocket = (*env)->FindClass(env, "android/bluetooth/BluetoothSocket");
    if(!jni_cid_BTSocket)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] close: jni_cid_BTSocket is null");
        return;
    }

    jmethodID jni_mid_close = (*env)->GetMethodID(env, jni_cid_BTSocket, "close", "()V");
    if(!jni_mid_close)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] close: jni_mid_close is null");
        return;
    }

    jobject jni_obj_socket = CAEDRNativeGetDeviceSocket(id);
    if(!jni_obj_socket)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] close: jni_obj_socket is not available");
        return;
    }

    (*env)->CallVoidMethod(env, jni_obj_socket, jni_mid_close);

    if((*env)->ExceptionCheck(env))
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] close: close is Failed!!!");
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return;
    }

    // remove socket to list
    CAEDRNativeRemoveDeviceSocket(env, jni_obj_socket);

    // update state
    CAEDRUpdateDeviceState(STATE_DISCONNECTED, address);

    OIC_LOG(DEBUG, TAG, "[EDR][Native] close: disconnected");
}


/**
 * BT State List
 */
void CAEDRNativeCreateDeviceStateList()
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] CAEDRNativeCreateDeviceStateList");

    // create new object array
    if (g_deviceStateList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Create device list");

        g_deviceStateList = u_arraylist_create();
    }
}

void CAEDRUpdateDeviceState(CAConnectedState_t state, const char *address)
{
    state_t *newstate = (state_t*) OICCalloc(1, sizeof(state_t));
    if (!newstate) {
        OIC_LOG(ERROR, TAG, "[EDR][Native] newstate is null");
        return;
    }
    strcpy(newstate->address, address);
    newstate->state = state;

    CAEDRNativeAddDeviceStateToList(newstate);
}

void CAEDRNativeAddDeviceStateToList(state_t *state)
{
    if(!state)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] device is null");
        return;
    }

    if(!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] gdevice_list is null");
        return;
    }

    if(CAEDRNativeIsDeviceInList(state->address)) {
        CAEDRNativeRemoveDevice(state->address); // delete previous state for update new state
    }
    u_arraylist_add(g_deviceStateList, state);          // update new state
    OIC_LOG_V(DEBUG, TAG, "Set State Info to List : %d", state->state);
}

jboolean CAEDRNativeIsDeviceInList(const char* remoteAddress){

    jint index;
    if (!remoteAddress) {
        OIC_LOG(ERROR, TAG, "[EDR][Native] remoteAddress is null");
        return JNI_TRUE;
    }
    for (index = 0; index < u_arraylist_length(g_deviceStateList); index++)
    {
        state_t* state = (state_t*) u_arraylist_get(g_deviceStateList, index);
        if(!state)
        {
            OIC_LOG(ERROR, TAG, "[EDR][Native] state_t object is null");
            return JNI_TRUE;
        }

        if(!strcmp(remoteAddress, state->address))
        {
            OIC_LOG(DEBUG, TAG, "the device is already set");
            return JNI_TRUE;
        }
        else
        {
            continue;
        }
    }

    OIC_LOG(DEBUG, TAG, "there are no the device in list.");
    return JNI_FALSE;
}

void CAEDRNativeRemoveAllDeviceState()
{
    OIC_LOG(DEBUG, TAG, "CAEDRNativeRemoveAllDevices");

    if(!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] gdeviceStateList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(g_deviceStateList); index++)
    {
        state_t* state = (state_t*) u_arraylist_get(g_deviceStateList, index);
        if(!state)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jarrayObj is null");
            continue;
        }
        OICFree(state);
    }

    OICFree(g_deviceStateList);
    g_deviceStateList = NULL;
    return;
}

void CAEDRNativeRemoveDevice(const char *remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "CAEDRNativeRemoveDeviceforStateList");

    if(!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] gdeviceStateList is null");
        return;
    }
    if (!remoteAddress) {
        OIC_LOG(ERROR, TAG, "[EDR][Native] remoteAddress is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(g_deviceStateList); index++)
    {
        state_t* state = (state_t*) u_arraylist_get(g_deviceStateList, index);
        if(!state)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] state_t object is null");
            continue;
        }

        if(!strcmp(state->address, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] remove state : %s", remoteAddress);
            OICFree(state);

            CAEDRReorderingDeviceList(index);
            break;
        }
    }
    return;
}

CAConnectedState_t CAEDRIsConnectedDevice(const char *remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "CAEDRIsConnectedDevice");

    if(!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] gdeviceStateList is null");
        return STATE_DISCONNECTED;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(g_deviceStateList); index++)
    {
        state_t* state = (state_t*) u_arraylist_get(g_deviceStateList, index);
        if(!state)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] state_t object is null");
            continue;
        }

        if(!strcmp(state->address, remoteAddress))
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] check whether it is connected or not");

            return state->state;
        }
    }
    return STATE_DISCONNECTED;
}

void CAEDRReorderingDeviceList(uint32_t index)
{
    if (index >= g_deviceStateList->length)
    {
        return;
    }

    if (index < g_deviceStateList->length - 1)
    {
        memmove(&g_deviceStateList->data[index], &g_deviceStateList->data[index + 1],
                (g_deviceStateList->length - index - 1) * sizeof(void *));
    }

    g_deviceStateList->size--;
    g_deviceStateList->length--;
}

/**
 * Device Socket Object List
 */
void CAEDRNativeCreateDeviceSocketList()
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] CAEDRNativeCreateDeviceSocketList");

    // create new object array
    if (g_deviceObjectList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Create Device object list");

        g_deviceObjectList = u_arraylist_create();
    }
}

void CAEDRNativeAddDeviceSocketToList(JNIEnv *env, jobject deviceSocket)
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] CANativeAddDeviceobjToList");

    if(!deviceSocket)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] Device is null");
        return;
    }

    if(!g_deviceObjectList)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] gdeviceObjectList is null");
        return;
    }

    jstring jni_remoteAddress = CAEDRNativeGetAddressFromDeviceSocket(env, deviceSocket);
    if(!jni_remoteAddress)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] jni_remoteAddress is null");
        return;
    }

    u_mutex_lock(g_mutexSocketListManager);

    const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

    if(!CAEDRNativeIsDeviceSocketInList(env, remoteAddress))
    {
        jobject gDeviceSocker = (*env)->NewGlobalRef(env, deviceSocket);
        u_arraylist_add(g_deviceObjectList, gDeviceSocker);
        OIC_LOG(DEBUG, TAG, "Set Socket Object to Array");
    }

    u_mutex_unlock(g_mutexSocketListManager);
}

jboolean CAEDRNativeIsDeviceSocketInList(JNIEnv *env, const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] CANativeIsDeviceObjInList");

    jint index;

    if (!remoteAddress) {
        OIC_LOG(ERROR, TAG, "[EDR][Native] remoteAddress is null");
        return JNI_TRUE;
    }
    for (index = 0; index < u_arraylist_length(g_deviceObjectList); index++)
    {

        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceObjectList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jarrayObj is null");
            return JNI_TRUE;
        }

        jstring jni_setAddress = CAEDRNativeGetAddressFromDeviceSocket(env, jarrayObj);
        if(!jni_setAddress)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_setAddress is null");
            return JNI_TRUE;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        if(!setAddress)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] setAddress is null");
            return JNI_TRUE;
        }

        if(!strcmp(remoteAddress, setAddress))
        {
            OIC_LOG(DEBUG, TAG, "the device is already set");
            return JNI_TRUE;
        }
        else
        {
            continue;
        }
    }

    OIC_LOG(DEBUG, TAG, "there are no the Device obejct in list. we can add");
    return JNI_FALSE;
}

void CAEDRNativeRemoveAllDeviceSocket(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CANativeRemoveAllDeviceObjsList");

    if(!g_deviceObjectList)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] gdeviceObjectList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(g_deviceObjectList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceObjectList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jarrayObj is null");
            return;
        }
        (*env)->DeleteGlobalRef(env, jarrayObj);
    }

    OICFree(g_deviceObjectList);
    g_deviceObjectList = NULL;
    return;
}

void CAEDRNativeRemoveDeviceSocket(JNIEnv *env, jobject deviceSocket)
{
    OIC_LOG(DEBUG, TAG, "CAEDRNativeRemoveDeviceSocket");

    if(!g_deviceObjectList)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] gdeviceObjectList is null");
        return;
    }

    u_mutex_lock(g_mutexSocketListManager);

    jint index;
    for (index = 0; index < u_arraylist_length(g_deviceObjectList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceObjectList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jarrayObj is null");
            continue;
        }

        jstring jni_setAddress = CAEDRNativeGetAddressFromDeviceSocket(env, jarrayObj);
        if(!jni_setAddress)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_setAddress is null");
            continue;
        }
        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);

        jstring jni_remoteAddress = CAEDRNativeGetAddressFromDeviceSocket(env, deviceSocket);
        if(!jni_remoteAddress)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_remoteAddress is null");
            continue;
        }
        const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

        if(!strcmp(setAddress, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] remove object : %s", remoteAddress);
            (*env)->DeleteGlobalRef(env, jarrayObj);

            CAEDRReorderingDeviceSocketList(index);
            break;
        }
    }
    u_mutex_unlock(g_mutexSocketListManager);

    OIC_LOG(DEBUG, TAG, "[EDR][Native] there are no target object");
    return;
}

jobject CAEDRNativeGetDeviceSocket(uint32_t idx)
{
    OIC_LOG(DEBUG, TAG, "CAEDRNativeGetDeviceSocket");

    if(idx < 0)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] index is not available");
        return NULL;
    }

    if(!g_deviceObjectList)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] gdeviceObjectList is null");
        return NULL;
    }

    jobject jarrayObj = (jobject) u_arraylist_get(g_deviceObjectList, idx);
    if(!jarrayObj)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] jarrayObj is not available");
        return NULL;
    }
    return jarrayObj;
}

uint32_t CAEDRGetSocketListLength()
{
    if(!g_deviceObjectList)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] gdeviceObjectList is null");
        return 0;
    }

    uint32_t length = u_arraylist_length(g_deviceObjectList);

    return length;
}

void CAEDRReorderingDeviceSocketList(uint32_t index)
{
    if (index >= g_deviceObjectList->length)
    {
        return;
    }

    if (index < g_deviceObjectList->length - 1)
    {
        memmove(&g_deviceObjectList->data[index], &g_deviceObjectList->data[index + 1],
                (g_deviceObjectList->length - index - 1) * sizeof(void *));
    }

    g_deviceObjectList->size--;
    g_deviceObjectList->length--;
}

void CAEDRInitializeClient(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAEDRInitialize(handle);
    OIC_LOG(DEBUG, TAG, "OUT");
}


