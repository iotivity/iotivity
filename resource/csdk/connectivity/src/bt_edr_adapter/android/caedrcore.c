#include <stdio.h>
#include <string.h>
#include <jni.h>

#include "caedrcore.h"
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

static const uint32_t STATE_CONNECTED = 1;
static const uint32_t STATE_DISCONNECTED = 0;

static const uint32_t MAX_PDU_BUFFER = 1400;

static u_arraylist_t *gdeviceStateList = NULL;
static u_arraylist_t *gdeviceObjectList = NULL;

static CAPacketReceiveCallback gPacketReceiveCallback = NULL;
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


static void CAReceiveHandler(void *data)
{
    OIC_LOG(DEBUG, TAG, "IN");
    // Input validation
    VERIFY_NON_NULL_VOID(data, TAG, "Invalid thread context");

    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get JNIEnv pointer");
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
        for(idx = 0 ; idx > length ; idx++)
        {
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
    int status;

    OIC_LOG(DEBUG, TAG, "[EDR] AcceptThread start");

    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get JNIEnv pointer");
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

////////////////////////////////////////////////////////////////////////////////////////////////////
//FIXME getting context
void CAEDRClientJNISetContext(JNIEnv *env, jobject context)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRClientJNISetContext");

    if(context == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "context is null");
        return;
    }

    gContext = (*env)->NewGlobalRef(env, context);
}

void CAEDRCreateJniInterfaceObject()
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRCreateJniInterfaceObject");

//    jboolean isAttached = FALSE;
//    JNIEnv* env;
//    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
//    if(res != JNI_OK)
//    {
//        OIC_LOG_V(DEBUG, TAG, "Could not get JNIEnv pointer");
//        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
//
//        if(res != JNI_OK)
//        {
//            OIC_LOG_V(DEBUG, TAG, "AttachCurrentThread failed");
//            return;
//        }
//        isAttached = TRUE;
//    }
//
//    jclass LeJniInterface = (*env)->FindClass(env, "com/iotivity/jar/CAEDRInterface");
//    if (!LeJniInterface)
//    {
//        OIC_LOG_V(DEBUG, TAG, "Could not get CAEDRInterface class");
//        return;
//    }
//
//    jmethodID LeInterfaceConstructorMethod =
//            (*env)->GetMethodID(env, LeJniInterface, "<init>", "(Landroid/content/Context;)V");
//    if (!LeInterfaceConstructorMethod)
//    {
//        OIC_LOG_V(DEBUG, TAG, "Could not get CAEDRInterface constructor method");
//        return;
//    }
//
//    (*env)->NewObject(env, LeJniInterface, LeInterfaceConstructorMethod, gContext);
//    OIC_LOG_V(DEBUG, TAG, "Create CAEDRInterface instance");
//
//    if(isAttached)
//        (*g_jvm)->DetachCurrentThread(g_jvm);
}

static void CAEDRDestroyMutex()
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

static CAResult_t CAEDRCreateMutex()
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

        CAEDRDestroyMutex();
        return CA_STATUS_FAILED;
    }

    gMutexSocketListManager = u_mutex_new();
    if (!gMutexSocketListManager)
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

    gThreadPoolHandle = handle;

    // init mutex
    CAEDRCreateMutex();

    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG_V(DEBUG, TAG, "AttachCurrentThread failed");
            return;
        }
        isAttached = TRUE;
    }

    CAEDRNativeCreateDeviceStateList();
    CAEDRNativeCreateDeviceSocketList();

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

//    CAEDRCreateJniInterfaceObject(); /* create java CAEDRInterface instance*/

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

void CAEDRTerminate()
{
    OIC_LOG(DEBUG, TAG, "CAEDRTerminate");

    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get JNIEnv pointer");
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
    CAEDRDestroyMutex();

    if(gContext)
    {
        (*env)->DeleteGlobalRef(env, gContext);
    }
    CAEDRNativeRemoveAllDeviceState();
    CAEDRNativeRemoveAllDeviceSocket(env);
}

void CAEDRCoreJniInit(JNIEnv *env, JavaVM* jvm)
{
    OIC_LOG_V(DEBUG, TAG, "CAEdrClientJniInit");
    g_jvm = jvm;
}

int32_t CAEDRSendUnicastMessage(const char* address, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRSendUnicastMessage(%s, %s)", address, data);

    CAEDRSendUnicastMessageImpl(address, data, dataLen);
    return 0;
}

int32_t CAEDRSendMulticastMessage(const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRSendMulticastMessage(%s)", data);

    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG_V(DEBUG, TAG, "AttachCurrentThread failed");
            return 0;
        }
        isAttached = TRUE;
    }

    CAEDRSendMulticastMessageImpl(env, data, dataLen);

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    return 1;
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

    // FIXME
    isSecured = FALSE;
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

void CAEDRSetCallback(CAPacketReceiveCallback callback)
{
    gPacketReceiveCallback = callback;
}

CAResult_t CAEDRGetInterfaceInfo(char **address)
{
    CAEDRGetLocalAddress(address);
    return CA_STATUS_OK;
}

void CAEDRGetLocalAddress(char** address)
{
    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get JNIEnv pointer");
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
        *address = (char*)OICMalloc(strlen(localAddress) + 1);
        if (*address == NULL)
        {
            return;
        }
        memcpy(*address, localAddress, strlen(localAddress));
    }

    OIC_LOG_V(DEBUG, TAG, "Local Address : %s", *address);
    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);
}

int32_t CAEDRSendUnicastMessageImpl(const char* address, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRSendUnicastMessageImpl, address: %s, data: %s", address, data);

    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
        if(res != JNI_OK)
        {
            OIC_LOG_V(DEBUG, TAG, "AttachCurrentThread failed");
            return 0;
        }
        isAttached = TRUE;
    }

    OIC_LOG(DEBUG, TAG, "[EDR][Native] set byteArray for data");
    if(gSendBuffer)
    {
        (*env)->DeleteGlobalRef(env, gSendBuffer);
    }
    jbyteArray jni_arr = (*env)->NewByteArray(env, dataLen);
    (*env)->SetByteArrayRegion(env, jni_arr, 0, dataLen, (jbyte*)data);
    gSendBuffer = (jbyteArray)(*env)->NewGlobalRef(env, jni_arr);

    // get bonded device list
    jobjectArray jni_arrayPairedDevices = CAEDRNativeGetBondedDevices(env);
    if(!jni_arrayPairedDevices)
    {
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] jni_arrayPairedDevices is empty");
        return 0;
    }
    // Get information from array of devices
    jsize length = (*env)->GetArrayLength(env, jni_arrayPairedDevices);
    jsize i;
    for( i = 0 ; i < length ; i++ )
    {
        // get name, address from BT device
        jobject j_obj_device = (*env)->GetObjectArrayElement(env, jni_arrayPairedDevices, i);

        jclass jni_cid_BTDevice = (*env)->FindClass(env, "android/bluetooth/BluetoothDevice");
        jmethodID j_mid_getName = (*env)->GetMethodID(env, jni_cid_BTDevice, "getName", "()Ljava/lang/String;");
        jmethodID j_mid_getAddress = (*env)->GetMethodID(env, jni_cid_BTDevice, "getAddress", "()Ljava/lang/String;");

        jstring j_str_name = (*env)->CallObjectMethod(env, j_obj_device, j_mid_getName);

        if(j_str_name)
        {
            const char * name = (*env)->GetStringUTFChars(env, j_str_name, NULL);
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] getBondedDevices: ~~device name is %s", name);
            (*env)->ReleaseStringUTFChars(env, j_str_name, name);
        }

        jstring j_str_address = (*env)->CallObjectMethod(env, j_obj_device, j_mid_getAddress);
        const char * remoteAddress = (*env)->GetStringUTFChars(env, j_str_address, NULL);
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] getBondedDevices: ~~device address is %s", remoteAddress);

        // find address
        if(!strcmp(remoteAddress, address))
        {
            CAEDRNativeSendData(env, remoteAddress, data, i);
        }
    }

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    return 1;
}

int32_t CAEDRSendMulticastMessageImpl(JNIEnv *env, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CASendMulticastMessageImpl, send to, data: %s, %d", data, dataLen);

    OIC_LOG(DEBUG, TAG, "[EDR][Native] set byteArray for data");
    if(gSendBuffer)
    {
        (*env)->DeleteGlobalRef(env, gSendBuffer);
    }
    jbyteArray jni_arr = (*env)->NewByteArray(env, dataLen);
    (*env)->SetByteArrayRegion(env, jni_arr, 0, dataLen, (jbyte*)data);
    gSendBuffer = (jbyteArray)(*env)->NewGlobalRef(env, jni_arr);

    // get bonded device list
    jobjectArray jni_arrayPairedDevices = CAEDRNativeGetBondedDevices(env);
    if(!jni_arrayPairedDevices)
    {
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] jni_arrayPairedDevices is empty");
        return 0;
    }
    // Get information from array of devices
    jsize length = (*env)->GetArrayLength(env, jni_arrayPairedDevices);
    jsize i;
    for( i = 0 ; i < length ; i++ )
    {
        // get name, address from BT device
        jobject j_obj_device = (*env)->GetObjectArrayElement(env, jni_arrayPairedDevices, i);

        jclass jni_cid_BTDevice = (*env)->FindClass(env, "android/bluetooth/BluetoothDevice");
        jmethodID j_mid_getName = (*env)->GetMethodID(env, jni_cid_BTDevice, "getName", "()Ljava/lang/String;");
        jmethodID j_mid_getAddress = (*env)->GetMethodID(env, jni_cid_BTDevice, "getAddress", "()Ljava/lang/String;");

        jstring j_str_name = (*env)->CallObjectMethod(env, j_obj_device, j_mid_getName);

        if(j_str_name)
        {
            const char * name = (*env)->GetStringUTFChars(env, j_str_name, NULL);
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] getBondedDevices: ~~device name is %s", name);
            (*env)->ReleaseStringUTFChars(env, j_str_name, name);
        }

        jstring j_str_address = (*env)->CallObjectMethod(env, j_obj_device, j_mid_getAddress);
        const char * remoteAddress = (*env)->GetStringUTFChars(env, j_str_address, NULL);
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] getBondedDevices: ~~device address is %s", remoteAddress);

        // find address
        CAEDRNativeSendData(env, remoteAddress, data, i);
    }

    return 1;
}

/**
 * EDR Method
 */
void CAEDRNativeSendData(JNIEnv *env, const char* address, const char* data, uint32_t id)
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData logic start");

    if(STATE_DISCONNECTED == CAEDRIsConnectedDevice(address))
    {
        // connect before send data
        OIC_LOG(DEBUG, TAG, "[EDR][Native] connect before send data");

        if(NULL == address)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] remote address is empty");
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
                OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: jni_cid_BTsocket is null");
                return;
            }

            jmethodID jni_mid_getOutputStream = (*env)->GetMethodID(env, jni_cid_BTsocket, "getOutputStream", "()Ljava/io/OutputStream;");
            if(!jni_mid_getOutputStream)
            {
                OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: jni_mid_getOutputStream is null");
                return;
            }
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btSendData: Get MethodID for i/o stream..%d", id);

            jobject jni_socket = CAEDRNativeGetDeviceSocket(id);
            if(!jni_socket)
            {
                OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: jni_socket is not available");
                return;
            }

            jobject jni_obj_outputStream = (*env)->CallObjectMethod(env, jni_socket, jni_mid_getOutputStream);
            if(!jni_obj_outputStream)
            {
                OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: jni_obj_outputStream is null");
                return;
            }

            OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: ready outputStream..");

            jclass jni_cid_OutputStream = (*env)->FindClass(env, "java/io/OutputStream");
            if(!jni_cid_OutputStream)
            {
                OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: jni_cid_OutputStream is null");
                return;
            }

            jmethodID jni_mid_write = (*env)->GetMethodID(env, jni_cid_OutputStream, "write", "([BII)V");
            if(!jni_mid_write)
            {
                OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: jni_mid_write is null");
                return;
            }

//          jbyte buf[BUFSIZE] = "aaaaaaaaaaassssssssssa";

            jbyteArray jbuf;
//          size_t nread = 20;
            int length = strlen(data);
            jbuf = (*env)->NewByteArray(env, length);
            (*env)->SetByteArrayRegion(env, jbuf, 0, length, (jbyte*)data);

            (*env)->CallVoidMethod(env, jni_obj_outputStream, jni_mid_write, jbuf, (jint)0, (jint)length);

            if((*env)->ExceptionCheck(env))
            {
                OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: Write Error!!!");
                (*env)->ExceptionDescribe(env);
                (*env)->ExceptionClear(env);
                return;
            }

            OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: Write Success");
        }
        else
        {
            (*env)->ExceptionDescribe(env);
            (*env)->ExceptionClear(env);
            OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: error!!");
            return;
        }

        // close socket after data was sent
        CAEDRNativeSocketClose(env, address, id);
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: BT connection is not completed!!");
    }
}

int CAEDRNativeReadData(JNIEnv *env, uint32_t id, CAAdapterServerType_t type)
{
    if(!((*env)->ExceptionCheck(env)))
    {
        // check whether this socket object is connected or not.
        jobject jni_obj_socket = CAEDRNativeGetDeviceSocket(id);
        if(!jni_obj_socket)
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: jni_socket is not available");
            return -1;
        }

        jstring jni_str_address = CAEDRNativeGetAddressFromDeviceSocket(env, jni_obj_socket);
        const char* address = (*env)->GetStringUTFChars(env, jni_str_address, NULL);
        if(STATE_DISCONNECTED == CAEDRIsConnectedDevice(address))
        {
            OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: it is not connected yet..");
            return -1;
        }

        // start to read through InputStream
        jclass jni_cid_BTsocket = (*env)->FindClass(env, "android/bluetooth/BluetoothSocket");
        jmethodID jni_mid_getInputStream = (*env)->GetMethodID(env, jni_cid_BTsocket, "getInputStream", "()Ljava/io/InputStream;");
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btReadData:  get InputStream..%d", id);

        jobject jni_obj_inputStream = (*env)->CallObjectMethod(env, jni_obj_socket, jni_mid_getInputStream);
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData:  ready inputStream..");

        jclass jni_cid_InputStream = (*env)->FindClass(env, "java/io/InputStream");
        jmethodID jni_mid_read = (*env)->GetMethodID(env, jni_cid_InputStream, "read", "([BII)I");

        jbyteArray jbuf = (*env)->NewByteArray(env, MAX_PDU_BUFFER);
        jlong nread;
        while ((nread = (*env)->CallIntMethod(env, jni_obj_inputStream, jni_mid_read, jbuf, (jint)0, MAX_PDU_BUFFER)) != -1)
        {
            if((*env)->ExceptionCheck(env))
            {
                OIC_LOG(DEBUG, TAG, "[EDR][Native] btReadData: read Error!!!");
                (*env)->ExceptionDescribe(env);
                (*env)->ExceptionClear(env);
                return -1;
            }

            jbyte* buf = (*env)->GetByteArrayElements(env, jbuf, NULL);
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
                    if (gPacketReceiveCallback)
                    {
//                            gPacketReceiveCallback(srcIPAddress, srcPort, recvBuffer, recvLen, false);
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

void CAEDRNativeConnect(JNIEnv *env, const char* address, uint32_t id)
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect..");

    jclass jni_cid_BTAdapter = (*env)->FindClass(env, CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_cid_BTAdapter is null");
        return;
    }

    // get BTadpater
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env, jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);
    if(!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_mid_getDefaultAdapter is null");
        return;
    }

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_obj_BTAdapter is null");
        return;
    }

    // get remote bluetooth device
    jmethodID jni_mid_getRemoteDevice = (*env)->GetMethodID(env, jni_cid_BTAdapter, "getRemoteDevice",
         "(Ljava/lang/String;)Landroid/bluetooth/BluetoothDevice;");
    if(!jni_mid_getRemoteDevice)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_mid_getRemoteDevice is null");
        return;
    }

    //jstring jni_address = (*env)->NewStringUTF(env, "B8:5E:7B:54:52:1C");
    jstring jni_address = (*env)->NewStringUTF(env, address);
    jobject jni_obj_remoteBTDevice = (*env)->CallObjectMethod(env, jni_obj_BTAdapter, jni_mid_getRemoteDevice, jni_address);
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

    jmethodID jni_mid_createSocket = (*env)->GetMethodID(env, jni_cid_BluetoothDevice,
         "createInsecureRfcommSocketToServiceRecord","(Ljava/util/UUID;)Landroid/bluetooth/BluetoothSocket;");
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

    jstring jni_uuid = (*env)->NewStringUTF(env, "00000000-0000-0000-0000-0000cdab0000");
    jobject jni_obj_uuid = (*env)->CallStaticObjectMethod(env, jni_cid_uuid, jni_mid_fromString, jni_uuid);
    if(!jni_obj_uuid)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect: jni_obj_uuid is null");
        return;
    }
    // create socket
    jobject jni_obj_BTSocket = (*env)->CallObjectMethod(env, jni_obj_remoteBTDevice, jni_mid_createSocket, jni_obj_uuid);
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

void CAEDRNativeSocketClose(JNIEnv *env, const char* address, uint32_t id)
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
 * BT common
 */

// get address from bluetooth socket
jstring CAEDRNativeGetAddressFromDeviceSocket(JNIEnv *env, jobject bluetoothSocketObj)
{
    jclass jni_cid_BTSocket = (*env)->FindClass(env, "android/bluetooth/BluetoothSocket");
    if(!jni_cid_BTSocket)
    {
        OIC_LOG_V(DEBUG, TAG, "[EDR] getRemoteAddress: jni_cid_BTSocket is null");
        return NULL;
    }

    jmethodID jni_mid_getRemoteDevice = (*env)->GetMethodID(env, jni_cid_BTSocket, "getRemoteDevice",
         "()Landroid/bluetooth/BluetoothDevice;");
    if(!jni_mid_getRemoteDevice)
    {
        OIC_LOG_V(DEBUG, TAG, "[EDR] getRemoteAddress: jni_mid_getRemoteDevice is null");
        return NULL;
    }

    jobject jni_obj_remoteBTDevice = (*env)->CallObjectMethod(env, bluetoothSocketObj, jni_mid_getRemoteDevice);
    if(!jni_obj_remoteBTDevice)
    {
        OIC_LOG_V(DEBUG, TAG, "[EDR] getRemoteAddress: jni_obj_remoteBTDevice is null");
        return NULL;
    }

    jclass jni_cid_BTDevice = (*env)->FindClass(env, "android/bluetooth/BluetoothDevice");
    jmethodID j_mid_getAddress = (*env)->GetMethodID(env, jni_cid_BTDevice, "getAddress", "()Ljava/lang/String;");

    jstring j_str_address = (*env)->CallObjectMethod(env, jni_obj_remoteBTDevice, j_mid_getAddress);
    if(j_str_address)
    {
        const char * address = (*env)->GetStringUTFChars(env, j_str_address, NULL);
        OIC_LOG_V(DEBUG, TAG, "[EDR] getRemoteAddress: ~~ remote device address is %s", address);
//      (*env)->ReleaseStringUTFChars(env, j_str_address, address);
    } else {
        return NULL;
    }
    return j_str_address;
}

jstring CAEDRNativeGetLocalDeviceAddress(JNIEnv* env)
{
    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] getAddress: jni_cid_BTAdapter is null");
        return NULL;
    }

    jmethodID jni_mid_getDefaultAdapter =
            (*env)->GetStaticMethodID(env, jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);
    if(!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] getAddress: jni_mid_getDefaultAdapter is null");
        return NULL;
    }

    jmethodID jni_mid_getAddress = (*env)->GetMethodID(env, jni_cid_BTAdapter, "getAddress", METHODID_STRINGNONPARAM);
    if(!jni_mid_getAddress)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] getAddress: jni_mid_getAddress is null");
        return NULL;
    }

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] getAddress: jni_obj_BTAdapter is null");
        return NULL;
    }

    jstring jni_str_address = (jstring)(*env)->CallObjectMethod(env, jni_obj_BTAdapter, jni_mid_getAddress);
    if(!jni_str_address)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] getAddress: jni_str_address is null");
        return NULL;
    }

    return jni_str_address;
}

jobjectArray CAEDRNativeGetBondedDevices(JNIEnv *env)
{
    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] getBondedDevices: jni_cid_BTAdapter is null");
        return NULL;
    }

    jmethodID jni_mid_getDefaultAdapter =
            (*env)->GetStaticMethodID(env, jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] getBondedDevices: bluetooth adapter is null");
        return NULL;
    }

    // Get a list of currently paired devices
    jmethodID jni_mid_getBondedDevices = (*env)->GetMethodID(env, jni_cid_BTAdapter,
            "getBondedDevices", "()Ljava/util/Set;");
    if(!jni_mid_getBondedDevices)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] getBondedDevices: jni_mid_getBondedDevicesr is null");
        return NULL;
    }

    jobject jni_obj_setPairedDevices = (*env)->CallObjectMethod(env, jni_obj_BTAdapter, jni_mid_getBondedDevices);
    if(!jni_obj_setPairedDevices)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] getBondedDevices: jni_obj_setPairedDevices is null");
        return NULL;
    }

    // Convert the set to an object array
    // object[] array = Set<BluetoothDevice>.toArray();
    jclass jni_cid_Set = (*env)->FindClass(env,  "java/util/Set");
    jmethodID jni_mid_toArray = (*env)->GetMethodID(env, jni_cid_Set, "toArray", "()[Ljava/lang/Object;");

    if(!jni_mid_toArray)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] getBondedDevices: jni_mid_toArray is null");
        return NULL;
    }

    jobjectArray jni_arrayPairedDevices = (jobjectArray)((*env)->CallObjectMethod(env,
            jni_obj_setPairedDevices, jni_mid_toArray));
    if(!jni_arrayPairedDevices)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] getBondedDevices: jni_arrayPairedDevices is null");
        return NULL;
    }

    return jni_arrayPairedDevices;
}

jint CAEDRNativeGetBTStateOnInfo(JNIEnv *env)
{
    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] getBTStateOnInfo: jni_cid_BTAdapter is null");
        return -1;
    }

    jfieldID jni_fid_stateon = (*env)->GetStaticFieldID(env, jni_cid_BTAdapter, "STATE_ON", "I");
    if (jni_fid_stateon == 0)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] get_field_state is 0");
        return -1;
    }
    jint jni_int_val = (*env)->GetStaticIntField(env, jni_cid_BTAdapter, jni_fid_stateon);

    OIC_LOG_V(DEBUG, TAG, "[EDR][Native] bluetooth STATE_ON state integer value : %d", jni_int_val);

    return jni_int_val;
}

jboolean CAEDRNativeIsEnableBTAdapter(JNIEnv *env)
{
    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_cid_BTAdapter: jni_cid_BTAdapter is null");
        return FALSE;
    }

    jmethodID jni_mid_getDefaultAdapter =
            (*env)->GetStaticMethodID(env, jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);
    if(!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_mid_getDefaultAdapter is null");
        return FALSE;
    }

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_obj_BTAdapter is null");
        return FALSE;
    }

    // isEnable()
    jmethodID jni_mid_isEnable = (*env)->GetMethodID(env, jni_cid_BTAdapter, "isEnabled",
            "()Z");
    if(!jni_mid_isEnable)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_mid_isEnable is null");
        return FALSE;
    }

    jboolean jni_isEnable = (*env)->CallBooleanMethod(env, jni_obj_BTAdapter, jni_mid_isEnable);
    OIC_LOG_V(DEBUG, TAG, "[EDR][Native] adapter state is %d", jni_isEnable);

    return jni_isEnable;
}

jstring CAEDRNativeGetAddressFromBTDevice(JNIEnv *env, jobject bluetoothDevice)
{
    jclass jni_cid_device_list = (*env)->FindClass(env, "android/bluetooth/BluetoothDevice");
    if(!jni_cid_device_list)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_cid_device_list is null");
        return NULL;
    }

    jmethodID jni_mid_getAddress = (*env)->GetMethodID(env, jni_cid_device_list, "getAddress",
            "()Ljava/lang/String;");
    if(!jni_mid_getAddress)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_mid_getAddress is null");
        return NULL;
    }

    jstring jni_address = (jstring)(*env)->CallObjectMethod(env, bluetoothDevice, jni_mid_getAddress);
    if(!jni_address)
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] jni_address is null");
        return NULL;
    }
    return jni_address;
}

/**
 * BT State List
 */
void CAEDRNativeCreateDeviceStateList()
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] CAEDRNativeCreateDeviceStateList");

    // create new object array
    if (gdeviceStateList == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "Create device list");

        gdeviceStateList = u_arraylist_create();
    }
}

void CAEDRUpdateDeviceState(uint32_t state, const char* address)
{
    state_t *newstate = (state_t*) OICMalloc( sizeof(state_t) );
    memset(newstate->address, 0, strlen(newstate->address));
    strcpy(newstate->address, address);
    newstate->state = state;

    CAEDRNativeAddDeviceStateToList(newstate);
}

void CAEDRNativeAddDeviceStateToList(state_t* state)
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

    if(!CAEDRNativeIsDeviceInList(state->address)) {
        CAEDRNativeRemoveDevice(state->address); // delete previous state for update new state
        u_arraylist_add(gdeviceStateList, state);          // update new state
        OIC_LOG_V(DEBUG, TAG, "Set State to Connected State List");
    }
}

jboolean CAEDRNativeIsDeviceInList(const char* remoteAddress){

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

void CAEDRNativeRemoveAllDeviceState()
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

void CAEDRNativeRemoveDevice(const char* remoteAddress)
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

            CAEDRReorderingDeviceList(index);
            break;
        }
    }
    OIC_LOG(DEBUG, TAG, "[EDR][Native] there are no target object");
    return;
}

jboolean CAEDRIsConnectedDevice(const char* remoteAddress)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRNativeRemoveDevice");

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

void CAEDRReorderingDeviceList(uint32_t index)
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
void CAEDRNativeCreateDeviceSocketList()
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CAEDRNativeCreateDeviceSocketList");

    // create new object array
    if (gdeviceObjectList == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "Create Device object list");

        gdeviceObjectList = u_arraylist_create();
    }
}

void CAEDRNativeAddDeviceSocketToList(JNIEnv *env, jobject deviceSocket)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeAddDeviceobjToList");

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

    if(!CAEDRNativeIsDeviceSocketInList(env, remoteAddress))
    {
        jobject gDeviceSocker = (*env)->NewGlobalRef(env, deviceSocket);
        u_arraylist_add(gdeviceObjectList, gDeviceSocker);
        OIC_LOG_V(DEBUG, TAG, "Set Object to Array as Element");
    }

    u_mutex_unlock(gMutexSocketListManager);
}

jboolean CAEDRNativeIsDeviceSocketInList(JNIEnv *env, const char* remoteAddress)
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

void CAEDRNativeRemoveAllDeviceSocket(JNIEnv *env)
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

void CAEDRNativeRemoveDeviceSocket(JNIEnv *env, jobject deviceSocket)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRNativeRemoveDeviceSocket");

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

            CAEDRReorderingDeviceSocketList(index);
            break;
        }
    }
    u_mutex_unlock(gMutexSocketListManager);

    OIC_LOG(DEBUG, TAG, "[BLE][Native] there are no target object");
    return;
}

jobject CAEDRNativeGetDeviceSocket(uint32_t idx)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRNativeGetDeviceSocket");

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

uint32_t CAEDRGetSocketListLength()
{
    if(!gdeviceObjectList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gdeviceObjectList is null");
        return 0;
    }

    uint32_t length = u_arraylist_length(gdeviceObjectList);

    return length;
}

void CAEDRReorderingDeviceSocketList(uint32_t index)
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
