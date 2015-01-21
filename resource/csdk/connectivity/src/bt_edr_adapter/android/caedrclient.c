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
#include "caadapterutils.h"
#include "com_iotivity_jar_CAEDRInterface.h"

//#define DEBUG_MODE
#define TAG PCF("CA_EDR_CLIENT")

static const char *METHODID_OBJECTNONPARAM = "()Landroid/bluetooth/BluetoothAdapter;";
static const char *METHODID_STRINGNONPARAM = "()Ljava/lang/String;";
static const char *CLASSPATH_BT_ADPATER = "android/bluetooth/BluetoothAdapter";
static const char *CLASSPATH_BT_UUID = "java/util/UUID";

static const uint32_t STATE_CONNECTED = 1;
static const uint32_t STATE_DISCONNECTED = 0;

static u_thread_pool_t gThreadPoolHandle = NULL;

static JavaVM *g_jvm;
static jobject gContext;

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

/**
 * implement for BT-EDR adapter common method
 */
CAResult_t CAEDRGetInterfaceInformation(CALocalConnectivity_t **info)
{
    OIC_LOG_V(DEBUG, TAG, "IN - CAEDRGetInterfaceInformation");

    CALocalConnectivity_t *netInfo = NULL;

    int32_t netInfoSize = 1;

    netInfo = (CALocalConnectivity_t *) OICMalloc(sizeof(CALocalConnectivity_t) * netInfoSize);
    if(NULL == netInfo)
    {
        OIC_LOG_V(ERROR, TAG, "Invalid input..");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memset(netInfo, 0, sizeof(CALocalConnectivity_t) * netInfoSize);

    char *macAddress = NULL;
    CAResult_t ret = CAEDRGetInterfaceInfo(&macAddress);
    OIC_LOG_V(ERROR, TAG, "address : %s", macAddress);
    if (CA_STATUS_OK != ret || NULL == macAddress)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to get interface info [%d]", ret);

        OICFree(netInfo);
        OICFree(macAddress);
        return ret;
    }

    // Create local endpoint using util function
    CALocalConnectivity_t *endpoint = CAAdapterCreateLocalEndpoint(CA_EDR, macAddress);
    if (NULL == endpoint)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to create Local Endpoint!",
                  CA_MEMORY_ALLOC_FAILED);
        OICFree(netInfo);
        OICFree(macAddress);
        return CA_MEMORY_ALLOC_FAILED;
    }

    // copy unicast server information
    endpoint->isSecured = CA_FALSE;
    memcpy(&netInfo[0], endpoint, sizeof(CALocalConnectivity_t));
    *info = netInfo;

    OICFree(macAddress);
    CAAdapterFreeLocalEndpoint(endpoint);

    OIC_LOG_V(DEBUG, TAG, "OUT - CAEDRGetInterfaceInformation");
    return CA_STATUS_OK;
}

void CAEDRTerminateClient()
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAEDRTerminate();
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAEDRManagerReadData(void)
{
    OIC_LOG_V(DEBUG, TAG, "IN");
    OIC_LOG_V(DEBUG, TAG, "OUT");
    return CA_NOT_SUPPORTED;
}

CAResult_t CAEDRClientSendUnicastData(const char *remoteAddress, const char *serviceUUID,
                                      void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, TAG, "IN");
    CAEDRSendUnicastMessage(remoteAddress, (const char*) data, dataLength);
    OIC_LOG_V(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}


CAResult_t CAEDRClientSendMulticastData(const char *serviceUUID, void *data,
                                        uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, TAG, "IN");
    CAEDRSendMulticastMessage((const char*) data, dataLength);
    OIC_LOG_V(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRClientUnsetCallbacks(void)
{
    OIC_LOG_V(DEBUG, TAG, "IN");

    OIC_LOG_V(DEBUG, TAG, "OUT");
}

void CAEDRClientDisconnectAll(void)
{
    OIC_LOG_V(DEBUG, TAG, "IN");

    OIC_LOG_V(DEBUG, TAG, "OUT");
}

CAResult_t CAEDRGetAdapterEnableState(CABool_t *state)
{
    OIC_LOG_V(DEBUG, TAG, "IN");

    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "CAEDRGetAdapterEnableState - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG_V(DEBUG, TAG, "AttachCurrentThread failed");
            return CA_STATUS_FAILED;
        }
        isAttached = TRUE;
    }
    jboolean ret = CAEDRNativeIsEnableBTAdapter(env);
    if(ret)
    {
        *state = CA_TRUE;
    }
    else
    {
        *state = CA_FALSE;
    }

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    OIC_LOG_V(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//FIXME getting context

void CAEDRJniSetContext()
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRJniSetContext");

    gContext = CANativeJNIGetContext();
}

void CAEDRCreateJNIInterfaceObject(jobject context)
{
    JNIEnv* env;
    OIC_LOG_V(DEBUG, TAG, "[EDRCore] CAEDRCreateJNIInterfaceObject");

    if ((*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "[EDRCore] Could not get JNIEnv pointer");
        return;
    }

    //getApplicationContext
    jclass contextClass = (*env)->FindClass(env, "android/content/Context");
    if (contextClass == 0)
    {
        OIC_LOG_V(DEBUG, TAG, "[EDRCore] Could not get context object class");
        return;
    }

    jmethodID getApplicationContextMethod = (*env)->GetMethodID(env, contextClass,
            "getApplicationContext", "()Landroid/content/Context;");
    if (getApplicationContextMethod == 0)
    {
        OIC_LOG_V(DEBUG, TAG, "[EDRCore] Could not get getApplicationContext method");
        return;
    }

    //Create EDRJniInterface instance
    jclass EDRJniInterface = (*env)->FindClass(env, "com/iotivity/jar/CAEDRInterface");
    if (!EDRJniInterface)
    {
        OIC_LOG_V(DEBUG, TAG, "[EDRCore] Could not get CAEDRInterface class");
        return;
    }

    jmethodID EDRInterfaceConstructorMethod = (*env)->GetMethodID(env,
            EDRJniInterface, "<init>", "(Landroid/content/Context;)V");
    if (!EDRInterfaceConstructorMethod)
    {
        OIC_LOG_V(DEBUG, TAG, "[EDRCore] Could not get CAEDRInterface constructor method");
        return;
    }

    (*env)->NewObject(env, EDRJniInterface, EDRInterfaceConstructorMethod, context);
    OIC_LOG_V(DEBUG, TAG, "[EDRCore] NewObject Success");

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

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRInitialize(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "CAEDRInitialize");

    gThreadPoolHandle = handle;

    CAEDRCoreJniInit();

    CAEDRJniSetContext();

    // init mutex
    CAEDRCreateMutex();

    jboolean isAttached = FALSE;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if(res != JNI_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "CAEDRInitialize - Could not get JNIEnv pointer");
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
        OIC_LOG_V(DEBUG, TAG, "My BT Address is %s ", localAddress);
    }

    CAEDRNativeCreateDeviceStateList();
    CAEDRNativeCreateDeviceSocketList();

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    if(gContext)
    {
        //FIXME
        CAEDRCreateJNIInterfaceObject(gContext); /* create java CAEDRInterface instance*/
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
        OIC_LOG_V(DEBUG, TAG, "CAEDRTerminate - Could not get JNIEnv pointer");
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

    CAEDRNativeSocketCloseToAll(env);

    // delete mutex
    CAEDRDestroyMutex();

    CAEDRNativeRemoveAllDeviceState();
    CAEDRNativeRemoveAllDeviceSocket(env);

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);
}

void CAEDRCoreJniInit()
{
    OIC_LOG_V(DEBUG, TAG, "CAEdrClientJniInit");
    g_jvm = CANativeJNIGetJavaVM();
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
        OIC_LOG_V(DEBUG, TAG, "CAEDRSendMulticastMessage - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if(res != JNI_OK)
        {
            OIC_LOG_V(DEBUG, TAG, "AttachCurrentThread failed");
            return 0;
        }
        isAttached = TRUE;
    }

    CAEDRSendMulticastMessageImpl(env, data, dataLen);

    OIC_LOG_V(DEBUG, TAG, "sent data");

    if(isAttached)
    {
        OIC_LOG_V(DEBUG, TAG, "DetachCurrentThread");
//        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    OIC_LOG_V(DEBUG, TAG, "OUT - CAEDRSendMulticastMessage");
    return 1;
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
        OIC_LOG_V(DEBUG, TAG, "CAEDRGetLocalAddress - Could not get JNIEnv pointer");
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
        OIC_LOG_V(DEBUG, TAG, "CAEDRSendUnicastMessageImpl - Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
        if(res != JNI_OK)
        {
            OIC_LOG_V(DEBUG, TAG, "AttachCurrentThread failed");
            return 0;
        }
        isAttached = TRUE;
    }

    OIC_LOG(DEBUG, TAG, "[EDR][Native] set byteArray for data");

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
        OIC_LOG_V(DEBUG, TAG, "[EDR][Native] start to check device");
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
            CAEDRNativeSendData(env, remoteAddress, data, dataLen, i);
        }
    }

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    return 1;
}

int32_t CAEDRSendMulticastMessageImpl(JNIEnv *env, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CASendMulticastMessageImpl, send to, data: %s, %d", data, dataLen);

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
        CAEDRNativeSendData(env, remoteAddress, data, dataLen, i);
    }

    return 1;
}

/**
 * EDR Method
 */
void CAEDRNativeSendData(JNIEnv *env, const char* address, const char* data, uint32_t dataLength, uint32_t id)
{
    OIC_LOG_V(DEBUG, TAG, "[EDR][Native] btSendData logic start : %s, %d", data, dataLength);

    if(!CAEDRNativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "BT adpater is not enable");
        return;
    }

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

            jobject jni_obj_socket = CAEDRNativeGetDeviceSocket(id);
            if(!jni_obj_socket)
            {
                OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: jni_socket is not available");
                return;
            }

            jobject jni_obj_outputStream = (*env)->CallObjectMethod(env, jni_obj_socket, jni_mid_getOutputStream);
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

            jbyteArray jbuf;
            jbuf = (*env)->NewByteArray(env, dataLength);
            (*env)->SetByteArrayRegion(env, jbuf, 0, dataLength, (jbyte*)data);

            (*env)->CallVoidMethod(env, jni_obj_outputStream, jni_mid_write, jbuf, (jint)0, (jint)dataLength);

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
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[EDR][Native] btSendData: BT connection is not completed!!");
    }
}

void CAEDRNativeConnect(JNIEnv *env, const char* address, uint32_t id)
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] btConnect..");

    if(!CAEDRNativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "BT adpater is not enable");
        return;
    }

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

void CAEDRInitializeClient(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAEDRInitialize(handle);
    OIC_LOG(DEBUG, TAG, "OUT");
}
