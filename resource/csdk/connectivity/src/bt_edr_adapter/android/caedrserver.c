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

static const char *METHODID_OBJECTNONPARAM = "()Landroid/bluetooth/BluetoothAdapter;";
static const char *METHODID_STRINGNONPARAM = "()Ljava/lang/String;";
static const char *CLASSPATH_BT_ADPATER = "android/bluetooth/BluetoothAdapter";
static const char *CLASSPATH_BT_UUID = "java/util/UUID";

static const uint32_t STATE_CONNECTED = 1;
static const uint32_t STATE_DISCONNECTED = 0;

static const uint32_t MAX_PDU_BUFFER = 1024;

static u_arraylist_t *gdeviceStateList = NULL;
static u_arraylist_t *gdeviceObjectList = NULL;

static u_thread_pool_t gThreadPoolHandle = NULL;

static JavaVM *g_jvm;
static jobject gContext;

static jbyteArray gSendBuffer;

/**
 * @var gMutexSocketListManager
 * @brief Mutex to synchronize socket list update
 */
static u_mutex gMutexSocketListManager;

// server socket instance
static jobject gServerSocketObject = NULL;

/**
 * @var gMutexUnicastServer
 * @brief Mutex to synchronize unicast server
 */
static u_mutex gMutexUnicastServer = NULL;

/**
 * @var gStopUnicast
 * @brief Flag to control the Receive Unicast Data Thread
 */
static bool gStopUnicast = FALSE;

/**
 * @var gMutexMulticastServer
 * @brief Mutex to synchronize secure multicast server
 */
static u_mutex gMutexMulticastServer = NULL;

/**
 * @var gStopMulticast
 * @brief Flag to control the Receive Multicast Data Thread
 */
static bool gStopMulticast = FALSE;

/**
 * @var gStopAccept
 * @brief Flag to control the Accept Thread
 */
static bool gStopAccept = FALSE;

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
 * @var gEDRNetworkChangeCallback
 * @brief Maintains the callback to be notified when data received from remote Bluetooth device
 */
static CAEDRDataReceivedCallback gEDRPacketReceivedCallback = NULL;

static void CAReceiveHandler(void *data)
{
    OIC_LOG_V(DEBUG, TAG, "start CAReceiveHandler..");
    OIC_LOG(DEBUG, TAG, "IN");
    // Input validation
    VERIFY_NON_NULL_VOID(data, TAG, "Invalid thread context");

    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "CAReceiveHandler - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG_V(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = TRUE;
    }

    CAAdapterReceiveThreadContext_t *ctx = (CAAdapterReceiveThreadContext_t *)data;

    while (TRUE != *(ctx->stopFlag))
    {
        uint32_t idx;

        // if new socket object is added in socket list after below logic is ran.
        // new socket will be started to read after next while loop
        uint32_t length = CAEDRServerGetSocketListLength();
        if(0 == length)
        {
            OIC_LOG_V(DEBUG, TAG, "socket list is empty");
            sleep(1);
        }

//        for(idx = 0 ; idx < length ; idx++)
//        {
//            OIC_LOG_V(DEBUG, TAG, "start CAEDRNativeReadData");
            CAEDRNativeReadData(env, idx, ctx->type);
//        }
    }

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    if(ctx)
        OICFree(ctx);

    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CAAcceptHandler(void *data)
{
    int status;

    OIC_LOG(DEBUG, TAG, "[EDR] AcceptThread start");

    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "CAAcceptHandler - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG_V(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = TRUE;
    }

    jobject jni_obj_BTSeverSocket = CAEDRNativeListen(env);
    if(!jni_obj_BTSeverSocket) {
        OIC_LOG(DEBUG, TAG, "[EDR] AcceptThread: jni_obj_BTSeverSocket is null");

        if(isAttached)
            (*g_jvm)->DetachCurrentThread(g_jvm);

        return;
    }

    CAAdapterAcceptThreadContext_t *ctx = (CAAdapterAcceptThreadContext_t *)data;

    while (TRUE != *(ctx->stopFlag))
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
    OIC_LOG_V(DEBUG, TAG, "IN");
    CAEDRServerInitialize(handle);
    // FIXME
    CAEDRStartUnicastServer(NULL, FALSE);
    OIC_LOG_V(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEDRServerStop(const int32_t serverFD)
{
    OIC_LOG_V(DEBUG, TAG, "IN");
    CAEDRStopUnicastServer(-1);
    CAEDRStopMulticastServer(-1);
    CAEDRServerTerminate();
    OIC_LOG_V(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRSetPacketReceivedCallback(CAEDRDataReceivedCallback packetReceivedCallback)
{
    gEDRPacketReceivedCallback = packetReceivedCallback;
}

/**
 * Destroy Mutex
 */
static void CAEDRServerDestroyMutex()
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (gMutexUnicastServer)
    {
        u_mutex_free(gMutexUnicastServer);
        gMutexUnicastServer = NULL;
    }

#ifdef __WITH_DTLS__

#endif

    if (gMutexMulticastServer)
    {
        u_mutex_free(gMutexMulticastServer);
        gMutexMulticastServer = NULL;
    }

    if(gMutexSocketListManager)
    {
        u_mutex_free(gMutexSocketListManager);
        gMutexSocketListManager = NULL;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}


/*
 * Create Mutex
 */
static CAResult_t CAEDRServerCreateMutex()
{
    OIC_LOG(DEBUG, TAG, "IN");

    gMutexUnicastServer = u_mutex_new();
    if (!gMutexUnicastServer)
    {
        OIC_LOG(ERROR, TAG, "Failed to created mutex!");
        return CA_STATUS_FAILED;
    }

#ifdef __WITH_DTLS__

#endif

    gMutexMulticastServer = u_mutex_new();
    if (!gMutexMulticastServer)
    {
        OIC_LOG(ERROR, TAG, "Failed to created mutex!");

        CAEDRServerDestroyMutex();
        return CA_STATUS_FAILED;
    }

    gMutexSocketListManager = u_mutex_new();
    if (!gMutexSocketListManager)
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
    OIC_LOG_V(DEBUG, TAG, "CAEDRServerJniInit");
    g_jvm = jvm;
}

void CAEDRServerInitialize(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "CAEDRServerInitialize");

    gThreadPoolHandle = handle;

    // init mutex
    CAEDRServerCreateMutex();

    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "CAEDRServerInitialize - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG_V(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = TRUE;
    }

    jstring jni_address = CAEDRNativeGetLocalDeviceAddress(env);
    if(jni_address)
    {
        const char* localAddress = (*env)->GetStringUTFChars(env, jni_address, NULL);
        OIC_LOG_V(DEBUG, TAG, "My BT Address is %s", localAddress);
    }

    CAEDRServerNativeCreateDeviceStateList();
    CAEDRServerNativeCreateDeviceSocketList();

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

//    CAEDRCreateJNIInterfaceObject(gContext); /* create java CAEDRInterface instance*/

    CAAdapterReceiveThreadContext_t *ctx = (CAAdapterReceiveThreadContext_t *)
                                               OICMalloc(sizeof(CAAdapterReceiveThreadContext_t));
    if (!ctx)
    {
        OIC_LOG(ERROR, TAG, "Out of memory!");
        return;
    }

    ctx->stopFlag = &gStopAccept;
    if (CA_STATUS_OK != u_thread_pool_add_task(gThreadPoolHandle, CAAcceptHandler, (void *)ctx))
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

    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "CAEDRServerTerminate - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG_V(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = TRUE;
    }

    gStopAccept = TRUE;
    gStopMulticast = TRUE;
    gStopUnicast = TRUE;

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    // delete mutex
    CAEDRServerDestroyMutex();

    CAEDRServerNativeRemoveAllDeviceState();
    CAEDRServerNativeRemoveAllDeviceSocket(env);
}

int32_t CAEDRStartUnicastServer(const char* address, bool isSecured)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRStartUnicastServer(%s)", address);

    u_mutex_lock(gMutexUnicastServer);

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
        u_mutex_unlock(gMutexUnicastServer);
        return CA_MEMORY_ALLOC_FAILED;
    }

    ctx->stopFlag = &gStopUnicast;
    ctx->type = isSecured ? CA_SECURED_UNICAST_SERVER : CA_UNICAST_SERVER;
    if (CA_STATUS_OK != u_thread_pool_add_task(gThreadPoolHandle, CAReceiveHandler, (void *)ctx))
    {
        OIC_LOG(ERROR, TAG, "Failed to create read thread!");
        u_mutex_unlock(gMutexUnicastServer);
        OICFree((void *)ctx);
        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(gMutexUnicastServer);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

int32_t CAEDRStartMulticastServer(bool isSecured)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRStartMulticastServer");

    u_mutex_lock(gMutexMulticastServer);

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
        u_mutex_unlock(gMutexMulticastServer);

        return CA_MEMORY_ALLOC_FAILED;
    }

    ctx->stopFlag = &gStopMulticast;
    ctx->type = CA_MULTICAST_SERVER;

    gStopMulticast = FALSE;
    if (CA_STATUS_OK != u_thread_pool_add_task(gThreadPoolHandle, CAReceiveHandler, (void *)ctx))
    {
        OIC_LOG(ERROR, TAG, "thread_pool_add_task failed!");

        gStopMulticast = TRUE;
        u_mutex_unlock(gMutexMulticastServer);

        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(gMutexMulticastServer);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

int32_t CAEDRStopUnicastServer(int32_t serverID)
{
    OIC_LOG(DEBUG, TAG, "CAEDRStopUnicastServer");

    u_mutex_lock(gMutexUnicastServer);
    gStopUnicast = TRUE;
    u_mutex_unlock(gMutexUnicastServer);

    return 0;
}

int32_t CAEDRStopMulticastServer(int32_t serverID)
{
    OIC_LOG(DEBUG, TAG, "CAEDRStopMulticastServer");

    u_mutex_lock(gMutexMulticastServer);
    gStopMulticast = true;
    u_mutex_unlock(gMutexMulticastServer);

    OIC_LOG_V(INFO, TAG, "Multicast server stopped");

    return 0;
}

uint32_t CAEDRNativeReadData(JNIEnv *env, uint32_t id, CAAdapterServerType_t type)
{
    if(!((*env)->ExceptionCheck(env)))
    {
        // check whether this socket object is connected or not.
        jobject jni_obj_socket = CAEDRServerNativeGetDeviceSocket(id);
        if(!jni_obj_socket)
        {
            return -1;
        }

        jstring jni_str_address = CAEDRNativeGetAddressFromDeviceSocket(env, jni_obj_socket);
        const char* address = (*env)->GetStringUTFChars(env, jni_str_address, NULL);
        if(STATE_DISCONNECTED == CAEDRServerIsConnectedDevice(address))
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData: it is not connected yet..");
            return -1;
        }

        // start to read through InputStream
        jclass jni_cid_BTsocket = (*env)->FindClass(env, "android/bluetooth/BluetoothSocket");
        jmethodID jni_mid_getInputStream = (*env)->GetMethodID(env, jni_cid_BTsocket, "getInputStream", "()Ljava/io/InputStream;");
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btReadData:  get InputStream..%d, %s", id, address);

        if(!jni_obj_socket)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_obj_socket is not available anymore..");
            return -1;
        }

        jobject jni_obj_inputStream = (*env)->CallObjectMethod(env, jni_obj_socket, jni_mid_getInputStream);
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData:  ready inputStream..");

        jclass jni_cid_InputStream = (*env)->FindClass(env, "java/io/InputStream");
        jmethodID jni_mid_read = (*env)->GetMethodID(env, jni_cid_InputStream, "read", "([BII)I");

        jbyteArray jbuf = (*env)->NewByteArray(env, MAX_PDU_BUFFER);
        jlong nread;

        if(!jni_obj_socket)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_obj_socket is not available anymore...");
            return -1;
        }

        while ((nread = (*env)->CallIntMethod(env, jni_obj_inputStream, jni_mid_read, jbuf, (jint)0, MAX_PDU_BUFFER)) != -1)
        {
            if((*env)->ExceptionCheck(env))
            {
                OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData: read Error!!!");
                (*env)->ExceptionDescribe(env);
                (*env)->ExceptionClear(env);
                return -1;
            }
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btReadData: reading");
            jbyte* buf = (*env)->GetByteArrayElements(env, jbuf, NULL);
            jint length = strlen((char*)buf);
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btReadData: read %s", buf);
            if (buf == NULL)
            {
                return -1;
            }

            switch (type)
            {
                case CA_UNICAST_SERVER:
                case CA_MULTICAST_SERVER:
                    // Notify data to upper layer
                    if (gEDRPacketReceivedCallback)
                    {
                        uint32_t sentLength = 0;
                        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] data will be sent to callback routine: %s, %d", buf, length);
                        gEDRPacketReceivedCallback(address, (void*)buf, length, &sentLength);
                    }
                    break;
#ifdef __WITH_DTLS__
                case CA_SECURED_UNICAST_SERVER:
                    break;
#endif //__WITH_DTLS__
                default:
                    // Should never occur
                    OIC_LOG_V(DEBUG, TAG, "Invalid server type");
                    return -1;
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
        return -1;
    }

    return 0;
}

jobject CAEDRNativeListen(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] btListen");

    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btListen: jni_cid_BTAdapter is null");
        return NULL;
    }

    // get BTadpater
    jmethodID jni_mid_getDefaultAdapter =(*env)->GetStaticMethodID(env, jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btListen: jni_cid_BTAdapter is null");
        return NULL;
    }

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btListen: jni_obj_BTAdapter is null");
        return NULL;
    }

    // get listen method ID
    jmethodID jni_mid_listen = (*env)->GetMethodID(env, jni_cid_BTAdapter,
            "listenUsingInsecureRfcommWithServiceRecord","(Ljava/lang/String;Ljava/util/UUID;)Landroid/bluetooth/BluetoothServerSocket;");
    if(!jni_mid_listen)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btListen: jni_mid_listen is null");
        return NULL;
    }
    // listenUsingInsecureRfcommWithServiceRecord  / listenUsingRfcommWithServiceRecord
    // setting UUID
    jclass jni_cid_uuid = (*env)->FindClass(env, CLASSPATH_BT_UUID);
    if(!jni_cid_uuid)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btListen: jni_mid_listen is null");
        return NULL;
    }

    jmethodID jni_mid_fromString = (*env)->GetStaticMethodID(env, jni_cid_uuid, "fromString", "(Ljava/lang/String;)Ljava/util/UUID;");
    if(!jni_mid_fromString)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btListen: jni_mid_fromString is null");
        return NULL;
    }

//  jstring uuid = (*env)->NewStringUTF(env, "0000111f-0000-1000-8000-00805f9b8033");
    jstring jni_uuid = (*env)->NewStringUTF(env, "00000000-0000-0000-0000-0000cdab0000");
    jobject jni_obj_uuid = (*env)->CallStaticObjectMethod(env, jni_cid_uuid, jni_mid_fromString, jni_uuid);
    if(!jni_obj_uuid)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btListen: jni_obj_uuid is null");
        return NULL;
    }

    // create socket
    jstring jni_name = (*env)->NewStringUTF(env, "BluetoothTestSecure");
    jobject jni_obj_BTSeverSocket = (*env)->CallObjectMethod(env, jni_obj_BTAdapter, jni_mid_listen, jni_name, jni_obj_uuid);
    if(!jni_obj_BTSeverSocket)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btListen: jni_obj_BTSeverSocket is null");
        return NULL;
    }

    gServerSocketObject = (*env)->NewGlobalRef(env, jni_obj_BTSeverSocket);

    return jni_obj_BTSeverSocket;
}

void CAEDRNativeAccept(JNIEnv *env, jobject severSocketObject)
{
    if(severSocketObject != NULL)
    {
        jclass jni_cid_BTServerSocket = (*env)->FindClass(env, "android/bluetooth/BluetoothServerSocket");
        if(!jni_cid_BTServerSocket)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] btAccept: jni_cid_BTServerSocket is null");
            return;
        }

        jmethodID jni_mid_accept = (*env)->GetMethodID(env, jni_cid_BTServerSocket, "accept", "()Landroid/bluetooth/BluetoothSocket;");
        if(!jni_mid_accept)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] btAccept: jni_mid_accept is null");
            return;
        }

        OIC_LOG(DEBUG, TAG, "[EDR][Native] btAccept: initiating accept...");

        jobject jni_obj_BTSocket = NULL;
        jni_obj_BTSocket = (*env)->CallObjectMethod(env, severSocketObject, jni_mid_accept);
        if(!jni_obj_BTSocket)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] btAccept: jni_obj_BTSocket is null");
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
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btAccept: severSocket is close previously");
    }
}

void CAEDRNativeServerSocketClose(JNIEnv *env, const char* address, uint32_t id)
{

    jclass jni_cid_BTSocket = (*env)->FindClass(env, "android/bluetooth/BluetoothSocket");
    if(!jni_cid_BTSocket)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] close: jni_cid_BTSocket is null");
        return;
    }

    jmethodID jni_mid_close = (*env)->GetMethodID(env, jni_cid_BTSocket, "close", "()V");
    if(!jni_mid_close)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] close: jni_mid_close is null");
        return;
    }

    jobject jni_obj_socket = CAEDRServerNativeGetDeviceSocket(id);
    if(!jni_obj_socket)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] close: jni_obj_socket is not available");
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
    if (gdeviceStateList == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "Create device list");

        gdeviceStateList = u_arraylist_create();
    }
}

void CAEDRServerUpdateDeviceState(uint32_t state, const char* address)
{
    state_t *newstate = (state_t*) OICMalloc( sizeof(state_t) );
    memset(newstate->address, 0, strlen(newstate->address));
    strcpy(newstate->address, address);
    newstate->state = state;

    CAEDRServerNativeAddDeviceStateToList(newstate);
}

void CAEDRServerNativeAddDeviceStateToList(state_t* state)
{
    if(!state)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] device is null");
        return;
    }

    if(!gdeviceStateList)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] gdevice_list is null");
        return;
    }

    if(!CAEDRServerNativeIsDeviceInList(state->address)) {
        CAEDRServerNativeRemoveDevice(state->address); // delete previous state for update new state
        u_arraylist_add(gdeviceStateList, state);          // update new state
        OIC_LOG_V(DEBUG, TAG, "Set State to Connected State List");
    }
}

jboolean CAEDRServerNativeIsDeviceInList(const char* remoteAddress){

    jint index;
    for (index = 0; index < u_arraylist_length(gdeviceStateList); index++)
    {
        state_t* state = (state_t*) u_arraylist_get(gdeviceStateList, index);
        if(!state)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] state_t object is null");
            return TRUE;
        }

        if(!strcmp(remoteAddress, state->address))
        {
            OIC_LOG_V(DEBUG, TAG, "the device is already set");
            return TRUE;
        }
        else
        {
            continue;
        }
    }

    OIC_LOG_V(DEBUG, TAG, "there are no the device in list.");
    return FALSE;
}

void CAEDRServerNativeRemoveAllDeviceState()
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRNativeRemoveAllDevices");

    if(!gdeviceStateList)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] gdeviceStateList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(gdeviceStateList); index++)
    {
        state_t* state = (state_t*) u_arraylist_get(gdeviceStateList, index);
        if(!state)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jarrayObj is null");
            continue;
        }
        OICFree(state);
    }

    OICFree(gdeviceStateList);
    gdeviceStateList = NULL;
    return;
}

void CAEDRServerNativeRemoveDevice(const char* remoteAddress)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRNativeRemoveDevice");

    if(!gdeviceStateList)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] gdeviceStateList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(gdeviceStateList); index++)
    {
        state_t* state = (state_t*) u_arraylist_get(gdeviceStateList, index);
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

jboolean CAEDRServerIsConnectedDevice(const char* remoteAddress)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRServerIsConnectedDevice");

    if(!gdeviceStateList)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] gdeviceStateList is null");
        return FALSE;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(gdeviceStateList); index++)
    {
        state_t* state = (state_t*) u_arraylist_get(gdeviceStateList, index);
        if(!state)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] state_t object is null");
            continue;
        }

        if(!strcmp(state->address, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] check whether it is connected or not");

            return state->state;
        }
    }
    OIC_LOG(DEBUG, TAG, "[EDR][Native] there are no target object");
    return FALSE;
}

void CAEDRServerReorderingDeviceList(uint32_t index)
{
    if (index >= gdeviceStateList->length)
    {
        return;
    }

    if (index < gdeviceStateList->length - 1)
    {
        memmove(&gdeviceStateList->data[index], &gdeviceStateList->data[index + 1],
                (gdeviceStateList->length - index - 1) * sizeof(void *));
    }

    gdeviceStateList->size--;
    gdeviceStateList->length--;
}

/**
 * Device Socket Object List
 */
void CAEDRServerNativeCreateDeviceSocketList()
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CAEDRServerNativeCreateDeviceSocketList");

    // create new object array
    if (gdeviceObjectList == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "Create Device object list");

        gdeviceObjectList = u_arraylist_create();
    }
}

void CAEDRServerNativeAddDeviceSocketToList(JNIEnv *env, jobject deviceSocket)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CAEDRServerNativeAddDeviceSocketToList");

    if(!deviceSocket)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] Device is null");
        return;
    }

    if(!gdeviceObjectList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gdeviceObjectList is null");
        return;
    }

    jstring jni_remoteAddress = CAEDRNativeGetAddressFromDeviceSocket(env, deviceSocket);
    if(!jni_remoteAddress)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_remoteAddress is null");
        return;
    }

    u_mutex_lock(gMutexSocketListManager);

    const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

    if(!CAEDRServerNativeIsDeviceSocketInList(env, remoteAddress))
    {
        jobject gDeviceSocker = (*env)->NewGlobalRef(env, deviceSocket);
        u_arraylist_add(gdeviceObjectList, gDeviceSocker);
        OIC_LOG_V(DEBUG, TAG, "Set Object to Array as Element");
    }

    u_mutex_unlock(gMutexSocketListManager);
}

jboolean CAEDRServerNativeIsDeviceSocketInList(JNIEnv *env, const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeIsDeviceObjInList");

    jint index;
    for (index = 0; index < u_arraylist_length(gdeviceObjectList); index++)
    {

        jobject jarrayObj = (jobject) u_arraylist_get(gdeviceObjectList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is null");
            return TRUE;
        }

        jstring jni_setAddress = CAEDRNativeGetAddressFromDeviceSocket(env, jarrayObj);
        if(!jni_setAddress)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_setAddress is null");
            return TRUE;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);

        if(!strcmp(remoteAddress, setAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "the device is already set");
            return TRUE;
        }
        else
        {
            continue;
        }
    }

    OIC_LOG_V(DEBUG, TAG, "there are no the Device obejct in list. we can add");
    return FALSE;
}

void CAEDRServerNativeRemoveAllDeviceSocket(JNIEnv *env)
{
    OIC_LOG_V(DEBUG, TAG, "CANativeRemoveAllDeviceObjsList");

    if(!gdeviceObjectList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gdeviceObjectList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(gdeviceObjectList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gdeviceObjectList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is null");
            return;
        }
        (*env)->DeleteGlobalRef(env, jarrayObj);
    }

    OICFree(gdeviceObjectList);
    gdeviceObjectList = NULL;
    return;
}

void CAEDRServerNativeRemoveDeviceSocket(JNIEnv *env, jobject deviceSocket)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRServerNativeRemoveDeviceSocket");

    if(!gdeviceObjectList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gdeviceObjectList is null");
        return;
    }

    u_mutex_lock(gMutexSocketListManager);

    jint index;
    for (index = 0; index < u_arraylist_length(gdeviceObjectList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gdeviceObjectList, index);
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

        jstring jni_remoteAddress = CAEDRNativeGetAddressFromDeviceSocket(env, deviceSocket);
        if(!jni_remoteAddress)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_remoteAddress is null");
            continue;
        }
        const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

        if(!strcmp(setAddress, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "[BLE][Native] remove object : %s", remoteAddress);
            (*env)->DeleteGlobalRef(env, jarrayObj);

            CAEDRServerReorderingDeviceSocketList(index);
            break;
        }
    }
    u_mutex_unlock(gMutexSocketListManager);

    OIC_LOG(DEBUG, TAG, "[BLE][Native] there are no target object");
    return;
}

jobject CAEDRServerNativeGetDeviceSocket(uint32_t idx)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRServerNativeGetDeviceSocket");

    if(idx < 0)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] index is not available");
        return NULL;
    }

    if(!gdeviceObjectList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gdeviceObjectList is null");
        return NULL;
    }

    jobject jarrayObj = (jobject) u_arraylist_get(gdeviceObjectList, idx);
    if(!jarrayObj)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is not available");
        return NULL;
    }
    return jarrayObj;
}

uint32_t CAEDRServerGetSocketListLength()
{
    if(!gdeviceObjectList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gdeviceObjectList is null");
        return 0;
    }

    uint32_t length = u_arraylist_length(gdeviceObjectList);

    return length;
}

void CAEDRServerReorderingDeviceSocketList(uint32_t index)
{
    if (index >= gdeviceObjectList->length)
    {
        return;
    }

    if (index < gdeviceObjectList->length - 1)
    {
        memmove(&gdeviceObjectList->data[index], &gdeviceObjectList->data[index + 1],
                (gdeviceObjectList->length - index - 1) * sizeof(void *));
    }

    gdeviceObjectList->size--;
    gdeviceObjectList->length--;
}

void CAEDRServerSetPacketReceivedCallback(CAEDRDataReceivedCallback packetReceivedCallback)
{
    gEDRPacketReceivedCallback = packetReceivedCallback;
}


