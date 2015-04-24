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
#include "caedrserver.h"
#include "logger.h"
#include "oic_malloc.h"
#include "uthreadpool.h" /* for thread pool */
#include "umutex.h"
#include "uarraylist.h"
#include "caadapterutils.h"

//#define DEBUG_MODE
#define TAG PCF("CA_EDR_SERVER")

static const char METHODID_OBJECTNONPARAM[] = "()Landroid/bluetooth/BluetoothAdapter;";
static const char METHODID_STRINGNONPARAM[] = "()Ljava/lang/String;";
static const char CLASSPATH_BT_ADPATER[] = "android/bluetooth/BluetoothAdapter";
static const char CLASSPATH_BT_UUID[] = "java/util/UUID";

static const uint32_t MAX_PDU_BUFFER = 1024;

static u_arraylist_t *g_deviceStateList = NULL;
static u_arraylist_t *g_deviceObjectList = NULL;

static u_thread_pool_t g_threadPoolHandle = NULL;

static JavaVM *g_jvm;

static jbyteArray g_sendBuffer;

/**
 * @var g_mMutexSocketListManager
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

// callback instance

/**
 * @var g_EDRPacketReceivedCallback
 * @brief Maintains the callback to be notified when data received from remote Bluetooth device
 */
static CAEDRDataReceivedCallback g_EDRPacketReceivedCallback = NULL;

static void CAReceiveHandler(void *data)
{
    OIC_LOG(DEBUG, TAG, "start CAReceiveHandler..");
    OIC_LOG(DEBUG, TAG, "IN");
    // Input validation
    VERIFY_NON_NULL_VOID(data, TAG, "Invalid thread context");

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "CAReceiveHandler - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = JNI_TRUE;
    }

    CAAdapterReceiveThreadContext_t *ctx = (CAAdapterReceiveThreadContext_t *)data;

    while (true != *(ctx->stopFlag))
    {
        uint32_t idx;

        // if new socket object is added in socket list after below logic is ran.
        // new socket will be started to read after next while loop
        uint32_t length = CAEDRServerGetSocketListLength();
        if (0 == length)
        {
            OIC_LOG(DEBUG, TAG, "socket list is empty");
        }

        for (idx = 0; idx < length; idx++)
        {
            OIC_LOG(DEBUG, TAG, "start CAEDRNativeReadData");
            CAEDRNativeReadData(env, idx, ctx->type);
        }

    }

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    if(ctx)
        OICFree(ctx);

    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CAAcceptHandler(void *data)
{
    if (!data)
    {
        OIC_LOG(ERROR, TAG, "[EDR] CAAcceptHandler: data is null");
        return;
    }
    OIC_LOG(DEBUG, TAG, "[EDR] AcceptThread start");

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "CAAcceptHandler - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = JNI_TRUE;
    }

    jobject jni_obj_BTSeverSocket = CAEDRNativeListen(env);
    if (!jni_obj_BTSeverSocket)
    {
        OIC_LOG(ERROR, TAG, "[EDR] AcceptThread: jni_obj_BTSeverSocket is null");

        if(isAttached)
            (*g_jvm)->DetachCurrentThread(g_jvm);

        return;
    }

    CAAdapterAcceptThreadContext_t *ctx = (CAAdapterAcceptThreadContext_t *)data;

    while (true != *(ctx->stopFlag))
    {
        OIC_LOG(DEBUG, TAG, "[EDR] AcceptThread running");
        CAEDRNativeAccept(env, jni_obj_BTSeverSocket);
    }

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    if(ctx)
        OICFree(ctx);

    OIC_LOG(DEBUG, TAG, "[EDR] AcceptThread finish");

    return;
}


// adapter common method

CAResult_t CAEDRServerStart(const char *serviceUUID, int32_t *serverFD, u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAEDRServerInitialize(handle);
    // FIXME
    CAEDRStartUnicastServer(NULL, false);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRServerStop(int serverFD)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAEDRStopUnicastServer(-1);
    CAEDRStopMulticastServer(-1);

    g_stopAccept = TRUE;
    g_stopMulticast = TRUE;
    g_stopUnicast = TRUE;

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRSetPacketReceivedCallback(CAEDRDataReceivedCallback packetReceivedCallback)
{
    g_EDRPacketReceivedCallback = packetReceivedCallback;
}

/**
 * Destroy Mutex
 */
static void CAEDRServerDestroyMutex()
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


/*
 * Create Mutex
 */
static CAResult_t CAEDRServerCreateMutex()
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

        CAEDRServerDestroyMutex();
        return CA_STATUS_FAILED;
    }

    g_mutexSocketListManager = u_mutex_new();
    if (!g_mutexSocketListManager)
    {
        OIC_LOG(ERROR, TAG, "Failed to created mutex!");

        CAEDRServerDestroyMutex();
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRServerJniInit(JNIEnv *env, JavaVM* jvm)
{
    OIC_LOG(DEBUG, TAG, "CAEDRServerJniInit");
    g_jvm = jvm;
}

void CAEDRServerInitialize(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "CAEDRServerInitialize");

    g_threadPoolHandle = handle;

    // init mutex
    CAEDRServerCreateMutex();

    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "CAEDRServerInitialize - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = JNI_TRUE;
    }

    jstring jni_address = CAEDRNativeGetLocalDeviceAddress(env);
    if (jni_address)
    {
        const char* localAddress = (*env)->GetStringUTFChars(env, jni_address, NULL);
        OIC_LOG_V(DEBUG, TAG, "My BT Address is %s", localAddress);
        (*env)->ReleaseStringUTFChars(env, jni_address, localAddress);
    }

    CAEDRServerNativeCreateDeviceStateList();
    CAEDRServerNativeCreateDeviceSocketList();

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    CAAdapterReceiveThreadContext_t *ctx = (CAAdapterReceiveThreadContext_t *)
                                               OICMalloc(sizeof(CAAdapterReceiveThreadContext_t));
    if (!ctx)
    {
        OIC_LOG(ERROR, TAG, "Out of memory!");
        return;
    }

    ctx->stopFlag = &g_stopAccept;
    if (CA_STATUS_OK != u_thread_pool_add_task(g_threadPoolHandle, CAAcceptHandler, (void *)ctx))
    {
        OIC_LOG(ERROR, TAG, "Failed to create read thread!");
        OICFree((void *)ctx);
        return;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAEDRServerTerminate()
{
    OIC_LOG(DEBUG, TAG, "CAEDRServerTerminate");

    if (!g_jvm)
    {
        return;
    }
    jboolean isAttached = JNI_FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG(DEBUG, TAG, "CAEDRServerTerminate - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = JNI_TRUE;
    }

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    CAEDRServerNativeRemoveAllDeviceState();
    CAEDRServerNativeRemoveAllDeviceSocket(env);

    // delete mutex
    CAEDRServerDestroyMutex();
}

int32_t CAEDRStartUnicastServer(const char* address, bool isSecured)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRStartUnicastServer(%s)", address);

    u_mutex_lock(g_mutexUnicastServer);

    /**
      * The task to listen for data from unicast is added to the thread pool.
      * This is a blocking call is made where we try to receive some data..
      * We will keep waiting until some data is received.
      * This task will be terminated when thread pool is freed on stopping the adapters.
      * Thread context will be freed by thread on exit.
      */
    CAAdapterReceiveThreadContext_t *ctx = (CAAdapterReceiveThreadContext_t *)
                                           OICMalloc(sizeof(CAAdapterReceiveThreadContext_t));
    if (!ctx)
    {
        OIC_LOG(ERROR, TAG, "Out of memory!");
        u_mutex_unlock(g_mutexUnicastServer);
        return CA_MEMORY_ALLOC_FAILED;
    }

    ctx->stopFlag = &g_stopUnicast;
    ctx->type = isSecured ? CA_SECURED_UNICAST_SERVER : CA_UNICAST_SERVER;
    if (CA_STATUS_OK != u_thread_pool_add_task(g_threadPoolHandle, CAReceiveHandler, (void *)ctx))
    {
        OIC_LOG(ERROR, TAG, "Failed to create read thread!");
        u_mutex_unlock(g_mutexUnicastServer);
        OICFree((void *)ctx);
        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(g_mutexUnicastServer);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

int32_t CAEDRStartMulticastServer(bool isSecured)
{
    OIC_LOG(DEBUG, TAG, "CAEDRStartMulticastServer");

    u_mutex_lock(g_mutexMulticastServer);

    /**
      * The task to listen to data from multicast socket is added to the thread pool.
      * This is a blocking call is made where we try to receive some data.
      * We will keep waiting until some data is received.
      * This task will be terminated when thread pool is freed on stopping the adapters.
      * Thread context will be freed by thread on exit.
      */
    CAAdapterReceiveThreadContext_t *ctx = (CAAdapterReceiveThreadContext_t *)
                                           OICMalloc(sizeof(CAAdapterReceiveThreadContext_t));
    if (!ctx)
    {
        OIC_LOG(ERROR, TAG, "Out of memory!");
        u_mutex_unlock(g_mutexMulticastServer);

        return CA_MEMORY_ALLOC_FAILED;
    }

    ctx->stopFlag = &g_stopMulticast;
    ctx->type = CA_MULTICAST_SERVER;

    g_stopMulticast = false;
    if (CA_STATUS_OK != u_thread_pool_add_task(g_threadPoolHandle, CAReceiveHandler, (void *)ctx))
    {
        OIC_LOG(ERROR, TAG, "thread_pool_add_task failed!");

        g_stopMulticast = true;
        u_mutex_unlock(g_mutexMulticastServer);
        OICFree((void *)ctx);

        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(g_mutexMulticastServer);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

int32_t CAEDRStopUnicastServer(int32_t serverID)
{
    OIC_LOG(DEBUG, TAG, "CAEDRStopUnicastServer");

    u_mutex_lock(g_mutexUnicastServer);
    g_stopUnicast = true;
    u_mutex_unlock(g_mutexUnicastServer);

    return 0;
}

int32_t CAEDRStopMulticastServer(int32_t serverID)
{
    OIC_LOG(DEBUG, TAG, "CAEDRStopMulticastServer");

    u_mutex_lock(g_mutexMulticastServer);
    g_stopMulticast = true;
    u_mutex_unlock(g_mutexMulticastServer);

    OIC_LOG(INFO, TAG, "Multicast server stopped");

    return 0;
}

CAResult_t CAEDRNativeReadData(JNIEnv *env, uint32_t id, CAAdapterServerType_t type)
{
    if(!((*env)->ExceptionCheck(env)))
    {
        // check whether this socket object is connected or not.
        jobject jni_obj_socket = CAEDRServerNativeGetDeviceSocket(id);
        if(!jni_obj_socket)
        {
            return CA_STATUS_INVALID_PARAM;
        }

        jstring jni_str_address = CAEDRNativeGetAddressFromDeviceSocket(env, jni_obj_socket);
        const char* address = (*env)->GetStringUTFChars(env, jni_str_address, NULL);
        if(STATE_DISCONNECTED == CAEDRServerIsConnectedDevice(address))
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData: it is not connected yet..");
            return CA_STATUS_FAILED;
        }

        // start to read through InputStream
        jclass jni_cid_BTsocket = (*env)->FindClass(env, "android/bluetooth/BluetoothSocket");
        jmethodID jni_mid_getInputStream = (*env)->GetMethodID(env,
                jni_cid_BTsocket, "getInputStream", "()Ljava/io/InputStream;");
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btReadData:  get InputStream..%d, %s", id, address);

        if(!jni_obj_socket)
        {
            OIC_LOG(ERROR, TAG, "[EDR][Native] jni_obj_socket is not available anymore..");
            return CA_STATUS_FAILED;
        }

        jobject jni_obj_inputStream = (*env)->CallObjectMethod(env,
                jni_obj_socket, jni_mid_getInputStream);
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData:  ready inputStream..");

        jclass jni_cid_InputStream = (*env)->FindClass(env, "java/io/InputStream");
        jmethodID jni_mid_read = (*env)->GetMethodID(env, jni_cid_InputStream, "read", "([BII)I");

        jbyteArray jbuf = (*env)->NewByteArray(env, MAX_PDU_BUFFER);
        jlong nread;

        if(!jni_obj_socket)
        {
            OIC_LOG(ERROR, TAG, "[EDR][Native] jni_obj_socket is not available anymore...");
            return CA_STATUS_FAILED;
        }

        while ((nread = (*env)->CallIntMethod(env, jni_obj_inputStream,
            jni_mid_read, jbuf, (jint)0, MAX_PDU_BUFFER)) != -1)
        {
            if((*env)->ExceptionCheck(env))
            {
                OIC_LOG(ERROR, TAG, "[EDR][Native] btReadData: read Error!!!");
                (*env)->ExceptionDescribe(env);
                (*env)->ExceptionClear(env);
                return CA_STATUS_FAILED;
            }
            OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData: reading");
            jbyte* buf = (*env)->GetByteArrayElements(env, jbuf, NULL);
            jint length = strlen((char*)buf);
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btReadData: read %s", buf);
            if (buf == NULL)
            {
                return CA_STATUS_FAILED;
            }

            switch (type)
            {
                case CA_UNICAST_SERVER:
                case CA_MULTICAST_SERVER:
                    // Notify data to upper layer
                    if (g_EDRPacketReceivedCallback)
                    {
                        uint32_t sentLength = 0;
                        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] data will be sent \
                                to callback routine: %s, %d", buf, length);
                        g_EDRPacketReceivedCallback(address, (void*)buf, length, &sentLength);
                    }
                    break;

                default:
                    // Should never occur
                    OIC_LOG(DEBUG, TAG, "Invalid server type");
                    return CA_STATUS_FAILED;
            }
            (*env)->ReleaseByteArrayElements(env, jbuf, buf, 0);

            // close socket after data was sent
            CAEDRNativeServerSocketClose(env, address, id);
            break;
        }
    }
    else
    {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData: env error!!");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

jobject CAEDRNativeListen(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] btListen");

    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] btListen: jni_cid_BTAdapter is null");
        return NULL;
    }

    // get BTadpater
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env,
            jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] btListen: jni_cid_BTAdapter is null");
        return NULL;
    }

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env,
            jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] btListen: jni_obj_BTAdapter is null");
        return NULL;
    }

    // get listen method ID
    jmethodID jni_mid_listen = (*env)->GetMethodID(env, jni_cid_BTAdapter,
                  "listenUsingInsecureRfcommWithServiceRecord",
                  "(Ljava/lang/String;Ljava/util/UUID;)Landroid/bluetooth/BluetoothServerSocket;");
    if(!jni_mid_listen)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] btListen: jni_mid_listen is null");
        return NULL;
    }
    // listenUsingInsecureRfcommWithServiceRecord  / listenUsingRfcommWithServiceRecord
    // setting UUID
    jclass jni_cid_uuid = (*env)->FindClass(env, CLASSPATH_BT_UUID);
    if(!jni_cid_uuid)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] btListen: jni_mid_listen is null");
        return NULL;
    }

    jmethodID jni_mid_fromString = (*env)->GetStaticMethodID(env, jni_cid_uuid,
            "fromString", "(Ljava/lang/String;)Ljava/util/UUID;");
    if(!jni_mid_fromString)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] btListen: jni_mid_fromString is null");
        return NULL;
    }

    jobject jni_obj_uuid = (*env)->CallStaticObjectMethod(env, jni_cid_uuid,
            jni_mid_fromString, OIC_EDR_SERVICE_ID);
    if(!jni_obj_uuid)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] btListen: jni_obj_uuid is null");
        return NULL;
    }

    // create socket
    jstring jni_name = (*env)->NewStringUTF(env, "BluetoothTestSecure");
    if (!jni_name)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] btListen: jni_name is null");
        return NULL;
    }
    jobject jni_obj_BTSeverSocket = (*env)->CallObjectMethod(env,
            jni_obj_BTAdapter, jni_mid_listen, jni_name, jni_obj_uuid);
    if(!jni_obj_BTSeverSocket)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] btListen: jni_obj_BTSeverSocket is null");
        return NULL;
    }

    g_serverSocketObject = (*env)->NewGlobalRef(env, jni_obj_BTSeverSocket);

    return jni_obj_BTSeverSocket;
}

void CAEDRNativeAccept(JNIEnv *env, jobject severSocketObject)
{
    if(severSocketObject != NULL)
    {
        jclass jni_cid_BTServerSocket = (*env)->FindClass(env,
                "android/bluetooth/BluetoothServerSocket");
        if(!jni_cid_BTServerSocket)
        {
            OIC_LOG(ERROR, TAG, "[EDR][Native] btAccept: jni_cid_BTServerSocket is null");
            return;
        }

        jmethodID jni_mid_accept = (*env)->GetMethodID(env,
                jni_cid_BTServerSocket, "accept",
                "(I)Landroid/bluetooth/BluetoothSocket;");
        if(!jni_mid_accept)
        {
            OIC_LOG(ERROR, TAG, "[EDR][Native] btAccept: jni_mid_accept is null");
            return;
        }

        OIC_LOG(DEBUG, TAG, "[EDR][Native] btAccept: initiating accept...");

        jobject jni_obj_BTSocket = NULL;
        jni_obj_BTSocket = (*env)->CallObjectMethod(env, severSocketObject, jni_mid_accept, 1000);
        if(!jni_obj_BTSocket)
        {
            OIC_LOG(ERROR, TAG, "[EDR][Native] btAccept: jni_obj_BTSocket is null");
            return;
        }

        // get remote address
        jstring j_str_address = CAEDRNativeGetAddressFromDeviceSocket(env, jni_obj_BTSocket);

        const char* address = (*env)->GetStringUTFChars(env, j_str_address, NULL);
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btAccept: address is %s", address);

        // set socket to list
        jobject jni_socket = (*env)->NewGlobalRef(env, jni_obj_BTSocket);
        CAEDRServerNativeAddDeviceSocketToList(env, jni_socket);

        OIC_LOG(DEBUG, TAG, "[EDR][Native] btAccept: Accepted!!");

        // update state
        CAEDRServerUpdateDeviceState(STATE_CONNECTED, address);
        (*env)->ReleaseStringUTFChars(env, j_str_address, address);
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btAccept: severSocket is close previously");
    }
}

void CAEDRNativeServerSocketClose(JNIEnv *env, const char *address, uint32_t id)
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

    jobject jni_obj_socket = CAEDRServerNativeGetDeviceSocket(id);
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
    CAEDRServerNativeRemoveDeviceSocket(env, jni_obj_socket);

    // update state
    CAEDRServerUpdateDeviceState(STATE_DISCONNECTED, address);

    OIC_LOG(DEBUG, TAG, "[EDR][Native] close: disconnected");
}

/**
 * BT State List
 */
void CAEDRServerNativeCreateDeviceStateList()
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] CAEDRNativeCreateDeviceStateList");

    // create new object array
    if (g_deviceStateList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Create device list");

        g_deviceStateList = u_arraylist_create();
        if (g_deviceStateList == NULL)
        {
            OIC_LOG(DEBUG, TAG, "Create device list is null");
        }
    }
}

void CAEDRServerUpdateDeviceState(uint32_t state, const char *address)
{
    state_t *newstate = (state_t*) OICCalloc(1, sizeof(state_t));
    if (!newstate) {
        OIC_LOG(ERROR, TAG, "[BLE][Native] newstate is null");
        return;
    }
    strcpy(newstate->address, address);
    newstate->state = state;

    CAEDRServerNativeAddDeviceStateToList(newstate);
}

void CAEDRServerNativeAddDeviceStateToList(state_t *state)
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

    if(!CAEDRServerNativeIsDeviceInList(state->address)) {
        CAEDRServerNativeRemoveDevice(state->address); // delete previous state for update new state
        u_arraylist_add(g_deviceStateList, state);          // update new state
        OIC_LOG(DEBUG, TAG, "Set State to Connected State List");
    }
}

jboolean CAEDRServerNativeIsDeviceInList(const char* remoteAddress){

    if (!remoteAddress) {
        OIC_LOG(ERROR, TAG, "[BLE][Native] remoteAddress is null");
        return JNI_TRUE;
    }
    jint index;
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

void CAEDRServerNativeRemoveAllDeviceState()
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

void CAEDRServerNativeRemoveDevice(const char *remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "CAEDRNativeRemoveDevice");

    if(!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] gdeviceStateList is null");
        return;
    }
    if (!remoteAddress) {
        OIC_LOG(ERROR, TAG, "[BLE][Native] remoteAddress is null");
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
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] remove object : %s", remoteAddress);
            OICFree(state);

            CAEDRServerReorderingDeviceList(index);
            break;
        }
    }
    OIC_LOG(DEBUG, TAG, "[EDR][Native] there are no target object");
    return;
}

jboolean CAEDRServerIsConnectedDevice(const char *remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "CAEDRServerIsConnectedDevice");

    if(!g_deviceStateList)
    {
        OIC_LOG(ERROR, TAG, "[EDR][Native] gdeviceStateList is null");
        return JNI_FALSE;
    }
    if (!remoteAddress) {
        OIC_LOG(ERROR, TAG, "[BLE][Native] remoteAddress is null");
        return JNI_FALSE;
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
    OIC_LOG(DEBUG, TAG, "[EDR][Native] there are no target object");
    return JNI_FALSE;
}

void CAEDRServerReorderingDeviceList(uint32_t index)
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
void CAEDRServerNativeCreateDeviceSocketList()
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CAEDRServerNativeCreateDeviceSocketList");

    // create new object array
    if (g_deviceObjectList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Create Device object list");

        g_deviceObjectList = u_arraylist_create();
        if (g_deviceObjectList == NULL)
        {
            OIC_LOG(DEBUG, TAG, "Create Device object list is null");
        }
    }
}

void CAEDRServerNativeAddDeviceSocketToList(JNIEnv *env, jobject deviceSocket)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CAEDRServerNativeAddDeviceSocketToList");

    if(!deviceSocket)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] Device is null");
        return;
    }

    if(!g_deviceObjectList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] gdeviceObjectList is null");
        return;
    }

    jstring jni_remoteAddress = CAEDRNativeGetAddressFromDeviceSocket(env, deviceSocket);
    if(!jni_remoteAddress)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jni_remoteAddress is null");
        return;
    }

    u_mutex_lock(g_mutexSocketListManager);

    const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

    if(!CAEDRServerNativeIsDeviceSocketInList(env, remoteAddress))
    {
        jobject gDeviceSocket = (*env)->NewGlobalRef(env, deviceSocket);
        u_arraylist_add(g_deviceObjectList, gDeviceSocket);
        OIC_LOG(DEBUG, TAG, "Set Object to Array as Element");
    }

    (*env)->ReleaseStringUTFChars(env, jni_remoteAddress, remoteAddress);
    u_mutex_unlock(g_mutexSocketListManager);
}

jboolean CAEDRServerNativeIsDeviceSocketInList(JNIEnv *env, const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeIsDeviceObjInList");

    jint index;
    for (index = 0; index < u_arraylist_length(g_deviceObjectList); index++)
    {

        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceObjectList, index);
        if(!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jarrayObj is null");
            return JNI_TRUE;
        }

        jstring jni_setAddress = CAEDRNativeGetAddressFromDeviceSocket(env, jarrayObj);
        if(!jni_setAddress)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jni_setAddress is null");
            return JNI_TRUE;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        if (!setAddress || !remoteAddress) {
            OIC_LOG(ERROR, TAG, "[BLE][Native] setAddress or remoteAddress is null");
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

void CAEDRServerNativeRemoveAllDeviceSocket(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CANativeRemoveAllDeviceObjsList");

    if(!g_deviceObjectList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] gdeviceObjectList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(g_deviceObjectList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceObjectList, index);
        if(!jarrayObj)
        {
            OIC_LOG(ERROR, TAG, "[BLE][Native] jarrayObj is null");
            return;
        }
        (*env)->DeleteGlobalRef(env, jarrayObj);
    }

    OICFree(g_deviceObjectList);
    g_deviceObjectList = NULL;
    return;
}

void CAEDRServerNativeRemoveDeviceSocket(JNIEnv *env, jobject deviceSocket)
{
    OIC_LOG(DEBUG, TAG, "CAEDRServerNativeRemoveDeviceSocket");

    if(!g_deviceObjectList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] gdeviceObjectList is null");
        return;
    }

    u_mutex_lock(g_mutexSocketListManager);

    jint index;
    for (index = 0; index < u_arraylist_length(g_deviceObjectList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(g_deviceObjectList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is null");
            continue;
        }

        jstring jni_setAddress = CAEDRNativeGetAddressFromDeviceSocket(env, jarrayObj);
        if(!jni_setAddress)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_setAddress is null");
            continue;
        }
        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        (*env)->ReleaseStringUTFChars(env, jni_setAddress, setAddress);

        jstring jni_remoteAddress = CAEDRNativeGetAddressFromDeviceSocket(env, deviceSocket);
        if(!jni_remoteAddress)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_remoteAddress is null");
            continue;
        }
        const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);
        (*env)->ReleaseStringUTFChars(env, jni_remoteAddress, remoteAddress);

        if (!setAddress || !remoteAddress) {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] setAddress or remoteAddress is null");
            continue;
        }
        if(!strcmp(setAddress, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "[BLE][Native] remove object : %s", remoteAddress);
            (*env)->DeleteGlobalRef(env, jarrayObj);

            CAEDRServerReorderingDeviceSocketList(index);
            break;
        }
    }
    u_mutex_unlock(g_mutexSocketListManager);

    OIC_LOG(DEBUG, TAG, "[BLE][Native] there are no target object");
    return;
}

jobject CAEDRServerNativeGetDeviceSocket(uint32_t idx)
{
    OIC_LOG(DEBUG, TAG, "CAEDRServerNativeGetDeviceSocket");

    if(idx < 0)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] index is not available");
        return NULL;
    }

    if(!g_deviceObjectList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] gdeviceObjectList is null");
        return NULL;
    }

    jobject jarrayObj = (jobject) u_arraylist_get(g_deviceObjectList, idx);
    if(!jarrayObj)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] jarrayObj is not available");
        return NULL;
    }
    return jarrayObj;
}

uint32_t CAEDRServerGetSocketListLength()
{
    if(!g_deviceObjectList)
    {
        OIC_LOG(ERROR, TAG, "[BLE][Native] gdeviceObjectList is null");
        return 0;
    }

    uint32_t length = u_arraylist_length(g_deviceObjectList);

    return length;
}

void CAEDRServerReorderingDeviceSocketList(uint32_t index)
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

void CAEDRServerSetPacketReceivedCallback
    (CAEDRDataReceivedCallback packetReceivedCallback)
{
    g_EDRPacketReceivedCallback = packetReceivedCallback;
}



