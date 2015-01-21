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
#include "com_iotivity_jar_CAEDRInterface.h"

//#define DEBUG_MODE
#define TAG PCF("CA_EDR_SERVER")

static const char *METHODID_OBJECTNONPARAM = "()Landroid/bluetooth/BluetoothAdapter;";
static const char *CLASSPATH_BT_ADPATER = "android/bluetooth/BluetoothAdapter";
static const char *CLASSPATH_BT_UUID = "java/util/UUID";

static const uint32_t STATE_CONNECTED = 1;
static const uint32_t STATE_DISCONNECTED = 0;

static const uint32_t MAX_PDU_BUFFER = 1024;
static const uint32_t MAX_CONNECTED_BT_SUPPORT_CNT = 7;

static u_thread_pool_t gThreadPoolHandle = NULL;

static JavaVM *g_jvm;
static jobject gContext;

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
 * @var gMutexMulticastServer
 * @brief Mutex to synchronize multicast receiver
 */
static u_mutex gMutexReceiver = NULL;

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


static jobject gInputStream = NULL;

static jobject gSeverSocket = NULL;

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

/**
 * @var gEDRNetworkChangeCallback
 * @brief Maintains the callback to be notified when data received from remote Bluetooth device
 */
static CAEDRDataReceivedCallback gEDRPacketReceivedCallback = NULL;

static void CAReceiveHandler(void *data)
{
    OIC_LOG(DEBUG, TAG, "IN - CAReceiveHandler..");
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
        uint32_t length = CAEDRGetSocketListLength();
        if(0 == length)
        {
            OIC_LOG_V(DEBUG, TAG, "socket list is empty");
            sleep(1);
        }

        for(idx = 0 ; idx < length ; idx++)
        {
            OIC_LOG_V(DEBUG, TAG, "start CAEDRNativeReadData");
            CAEDRNativeReadData(env, idx, ctx->type);
            sleep(1);
        }
    }

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    if(ctx)
        OICFree(ctx);

    OIC_LOG(DEBUG, TAG, "OUT - CAReceiveHandler");
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

    gSeverSocket = (*env)->NewGlobalRef(env, jni_obj_BTSeverSocket);

    CAAdapterAcceptThreadContext_t *ctx = (CAAdapterAcceptThreadContext_t *)data;

    if(NULL == ctx)
    {
        OIC_LOG(DEBUG, TAG, "[EDR] AcceptThread: ctx is null");
        return;
    }

    // it should be initialized for restart accept thread
    gStopAccept = FALSE;

    while (TRUE != *(ctx->stopFlag))
    {
        OIC_LOG(DEBUG, TAG, "[EDR] AcceptThread running");

        // when BT state is changed with Off. its thread will be stopped
        if(!CAEDRNativeIsEnableBTAdapter(env))
        {
            OIC_LOG(DEBUG, TAG, "BT adpater is not enable");
            gStopAccept = TRUE;
            gSeverSocket = NULL;
        }
        else
        {
            CAEDRNativeAccept(env, gSeverSocket);
        }
    }

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    if(ctx)
        OICFree(ctx);

    OIC_LOG(DEBUG, TAG, "[EDR] AcceptThread finish");

    return;
}

/**
 * implement for adapter common method
 */
CAResult_t CAEDRServerStart(const char *serviceUUID, int32_t *serverFD, u_thread_pool_t handle)
{
    OIC_LOG_V(DEBUG, TAG, "IN");
    CAEDRServerInitialize(handle);
    // FIXME
    CAEDRStartUnicastServer(NULL, FALSE);

    *serverFD = 1;
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

void CAEDRServerJniInit()
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRServerJniInit");
    g_jvm = CANativeJNIGetJavaVM();
}

void CAEDRServerInitialize(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "CAEDRServerInitialize");

    gThreadPoolHandle = handle;

    CAEDRServerStartAccpetThread();

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAEDRServerStartAccpetThread()
{
    CAEDRServerJniInit();

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

    CAEDRNativeCreateDeviceStateList();
    CAEDRNativeCreateDeviceSocketList();

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

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

    CAEDRNatvieCloseServerTask(env);

    CAEDRNativeSocketCloseToAll(env);

    // delete mutex
    CAEDRServerDestroyMutex();

    CAEDRNativeRemoveAllDeviceState();
    CAEDRNativeRemoveAllDeviceSocket(env);

    if(isAttached)
            (*g_jvm)->DetachCurrentThread(g_jvm);
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
    if(!CAEDRNativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "BT adpater is not enable");
        return 0;
    }

    if(!((*env)->ExceptionCheck(env)))
    {
        // check whether this socket object is connected or not.
        jobject jni_obj_socket = CAEDRNativeGetDeviceSocket(id);
        if(!jni_obj_socket)
        {
            return 0;
        }

        jstring jni_str_address = CAEDRNativeGetAddressFromDeviceSocket(env, jni_obj_socket);
        const char* address = (*env)->GetStringUTFChars(env, jni_str_address, NULL);

        // check it whether is still connected or not through google api
        jboolean ret = CAEDRIsConnectedForSocket(env, jni_obj_socket);
        if(!ret)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] this device is not connected now...let close socket");
        }

        // check it whether is still connected or not through socket state list
        if(STATE_DISCONNECTED == CAEDRIsConnectedDevice(address))
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData: it is not connected yet..");

            // remove socket to list
            // this code is related to below read fail exception code
            CAEDRNativeRemoveDeviceSocket(env, jni_obj_socket);

            return 0;
        }

        // start to read through InputStream
        jclass jni_cid_BTsocket = (*env)->FindClass(env, "android/bluetooth/BluetoothSocket");
        jmethodID jni_mid_getInputStream = (*env)->GetMethodID(env, jni_cid_BTsocket, "getInputStream", "()Ljava/io/InputStream;");
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btReadData:  get InputStream..%d, %s", id, address);

        if(!jni_obj_socket)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_obj_socket is not available anymore..");
            return 0;
        }

        jobject jni_obj_inputStream = (*env)->CallObjectMethod(env, jni_obj_socket, jni_mid_getInputStream);
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData:  ready inputStream...");

        gInputStream = (*env)->NewGlobalRef(env, jni_obj_inputStream);

        jclass jni_cid_InputStream = (*env)->FindClass(env, "java/io/InputStream");
        jmethodID jni_mid_read = (*env)->GetMethodID(env, jni_cid_InputStream, "read", "([BII)I");

        jbyteArray jbuf = (*env)->NewByteArray(env, MAX_PDU_BUFFER);
        jlong nread;

        if(!jni_obj_socket)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_obj_socket is not available anymore...");
            return 0;
        }

        nread = (*env)->CallIntMethod(env, gInputStream, jni_mid_read, jbuf, (jint)0, MAX_PDU_BUFFER);

        OIC_LOG(DEBUG, TAG, "[EDR][Native] read something from InputStream");

        if(-1 == nread)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] read buffer is empty...");
            return 0;
        }

        if((*env)->ExceptionCheck(env))
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData: read Error!!!");
            (*env)->ExceptionDescribe(env);
            (*env)->ExceptionClear(env);

            // update state to disconnect
            // the socket will be close next read thread routine
            CAEDRUpdateDeviceState(STATE_DISCONNECTED, address);
            return 0;
        }

        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btReadData: reading");
        jbyte* buf = (*env)->GetByteArrayElements(env, jbuf, NULL);
        if (buf == NULL)
        {
            return 0;
        }

        jint length = strlen((char*)buf);
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btReadData: read %s, %d", buf, length);

        char responseData[MAX_PDU_BUFFER];
        memset(responseData, 0, MAX_PDU_BUFFER);
        strncpy(responseData, (char*)buf, length);
        responseData[length] = '\0';

        switch (type)
        {
            case CA_UNICAST_SERVER:
            case CA_MULTICAST_SERVER:
                // Notify data to upper layer
                if (gEDRPacketReceivedCallback)
                {
                  //  u_mutex_lock(gMutexSocketListManager);
                    uint32_t sentLength = 0;
                    OIC_LOG_V(DEBUG, TAG, "[EDR][Native] data will be sent to callback routine: %s, %d", responseData, length);
//                        sleep(3);
                    gEDRPacketReceivedCallback(address, (void*)responseData, length, &sentLength);
                  //  u_mutex_unlock(gMutexSocketListManager);
                }
                break;
#ifdef __WITH_DTLS__
            case CA_SECURED_UNICAST_SERVER:
                break;
#endif //__WITH_DTLS__
            default:
                // Should never occur
                OIC_LOG_V(DEBUG, TAG, "Invalid server type");
                return 0;
        }
        (*env)->ReleaseByteArrayElements(env, jbuf, buf, 0);
    }
    else
    {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData: env error!!");
        return 0;
    }

    return 1;
}

jboolean CAEDRIsConnectedForSocket(JNIEnv *env, jobject socket)
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] CAEDRIsConnectedForSocket...");

    if(!CAEDRNativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "BT adpater is not enable");
        return FALSE;
    }

    if(!socket)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] CAEDRIsConnectedForSocket - socket is null");
        return FALSE;
    }

    jclass jni_cid_BTsocket = (*env)->FindClass(env, "android/bluetooth/BluetoothSocket");
    if(!jni_cid_BTsocket)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] CAEDRIsConnectedForSocket - jni_cid_BTsocket is null");
        return FALSE;
    }

    jmethodID jni_mid_isConnected = (*env)->GetMethodID(env, jni_cid_BTsocket, "isConnected", "()Z");
    if(!jni_mid_isConnected)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] CAEDRIsConnectedForSocket - jni_mid_isConnected is null.");
        return FALSE;
    }

    jboolean jni_isConnected = (*env)->CallBooleanMethod(env, socket, jni_mid_isConnected);

    return jni_isConnected;
}

void CANativeStartListenTask(JNIEnv *env)
{
    jobject jni_obj_BTSeverSocket = CAEDRNativeListen(env);
    if(!jni_obj_BTSeverSocket) {
        OIC_LOG(DEBUG, TAG, "[EDR] AcceptThread: jni_obj_BTSeverSocket is null");
        return;
    }

    gSeverSocket = (*env)->NewGlobalRef(env, jni_obj_BTSeverSocket);
}

jobject CAEDRNativeListen(JNIEnv *env)
{
    if(!CAEDRNativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "BT adpater is not enable");
        return NULL;
    }

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

            if((*env)->ExceptionCheck(env))
            {
                OIC_LOG(DEBUG, TAG, "[EDR][Native] btAccept: socket might closed or timeout!!!");
                (*env)->ExceptionDescribe(env);
                (*env)->ExceptionClear(env);
                return;
            }
        }

        // get remote address
        jstring j_str_address = CAEDRNativeGetAddressFromDeviceSocket(env, jni_obj_BTSocket);

        const char* address = (*env)->GetStringUTFChars(env, j_str_address, NULL);
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btAccept: address is %s", address);

        // set socket to list
        jobject jni_socket = (*env)->NewGlobalRef(env, jni_obj_BTSocket);
        CAEDRNativeAddDeviceSocketToList(env, jni_socket);

        OIC_LOG(DEBUG, TAG, "[EDR][Native] btAccept: Accepted!!");

        // update state
        CAEDRUpdateDeviceState(STATE_CONNECTED, address);
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btAccept: severSocket is close previously");
    }
}

void CAEDRNativeServerSocketClose(JNIEnv *env, const char* address, uint32_t id)
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] CAEDRNativeServerSocketClose");

    if(!CAEDRNativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "BT adpater is not enable");
        return;
    }

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

    jobject jni_obj_socket = CAEDRNativeGetDeviceSocket(id);
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
    CAEDRNativeRemoveDeviceSocket(env, jni_obj_socket);

    // update state
    CAEDRUpdateDeviceState(STATE_DISCONNECTED, address);

    OIC_LOG(DEBUG, TAG, "[EDR][Native] close: disconnected");
}

/**
 * InputStream & BluetoothServerSocket will be close for Terminating
 */
void CAEDRNatvieCloseServerTask(JNIEnv* env)
{
    if(gInputStream)
    {
        OIC_LOG_V(DEBUG, TAG, "InputStream will be close");
        jclass jni_cid_InputStream = (*env)->FindClass(env, "java/io/InputStream");
        jmethodID jni_mid_close = (*env)->GetMethodID(env, jni_cid_InputStream, "close", "()V");
        (*env)->CallVoidMethod(env, gInputStream, jni_mid_close);
        (*env)->DeleteGlobalRef(env, gInputStream);
    }

    if(gSeverSocket)
    {
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] Accept Resource will be close");

        jclass jni_cid_BTServerSocket = (*env)->FindClass(env, "android/bluetooth/BluetoothServerSocket");
        if(!jni_cid_BTServerSocket)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_cid_BTServerSocket is null");
            return;
        }

        jmethodID jni_mid_accept = (*env)->GetMethodID(env, jni_cid_BTServerSocket, "close", "()V");
        if(!jni_mid_accept)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_mid_accept is null");
            return;
        }
        (*env)->CallVoidMethod(env, gSeverSocket, jni_mid_accept);
        (*env)->DeleteGlobalRef(env, gSeverSocket);

        OIC_LOG(DEBUG, TAG, "[EDR][Native] close accept obj");
    }
}

void CAEDRServerSetPacketReceivedCallback(CAEDRDataReceivedCallback packetReceivedCallback)
{
    gEDRPacketReceivedCallback = packetReceivedCallback;
}

