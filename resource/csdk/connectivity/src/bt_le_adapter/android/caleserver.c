#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include "caleserver.h"
#include "caleutils.h"
#include "logger.h"
#include "oic_malloc.h"
#include "uthreadpool.h"
#include "uarraylist.h"
#include "com_iotivity_jar_CALeInterface.h"

#define TAG PCF("CA_LE_SERVER")

#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

/* Service UUID */
static const char *OIC_GATT_SERVICE_UUID = "713d0000-503e-4c75-ba94-3148f18d941e";
static const char *OIC_GATT_CHARACTERISTIC_RESPONSE_UUID = "713d0002-503e-4c75-ba94-3148f18d941e"; //read
static const char *OIC_GATT_CHARACTERISTIC_REQUEST_UUID = "713d0003-503e-4c75-ba94-3148f18d941e"; //write

static JavaVM *g_jvm;
static jobject gContext;
static jobject gBluetoothGattServer;
static jobject gBluetoothGattServerCallback;
static jobject gLeAdvertiseCallback;

static CAPacketReceiveCallback gPacketReceiveCallback = NULL;
static u_arraylist_t *gConnectedDeviceList = NULL;
static u_thread_pool_t gThreadPoolHandle = NULL;

static jboolean gIsStartServer;
static jboolean gIsSendingMulticastData;

//getting context
void CALEServerJNISetContext(JNIEnv *env, jobject context)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerJNISetContext");

    if(context == NULL)
        OIC_LOG_V(DEBUG, TAG, "context is null");

    gContext = (*env)->NewGlobalRef(env, context);
}

//getting jvm
void CALeServerJniInit(JNIEnv *env, JavaVM *jvm)
{
    OIC_LOG_V(DEBUG, TAG, "CALeServerJniInit");
    g_jvm = jvm;
}

jobject CANativeLEServerSetResponseData(JNIEnv *env, jbyteArray responseData)
{
    if(!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return NULL;
    }

    OIC_LOG_V(DEBUG, TAG, "CALEServerSetResponseData");

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattServer");

    jclass jni_cid_bluetoothGattService = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattService");

    jclass jni_cid_bluetoothGattCharacteristic = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattCharacteristic");

    jmethodID jni_mid_getService = (*env)->GetMethodID(env,
            jni_cid_bluetoothGattServer, "getService",
            "(Ljava/util/UUID;)Landroid/bluetooth/BluetoothGattService;");

    jobject jni_obj_serviceUUID = CALEGetUuidFromString(env, OIC_GATT_SERVICE_UUID);

    if (!gBluetoothGattServer)
    {
        OIC_LOG_V(DEBUG, TAG, "Check BluetoothGattServer status");
        return NULL;
    }
    jobject jni_obj_bluetoothGattService = (*env)->CallObjectMethod(env,
            gBluetoothGattServer, jni_mid_getService, jni_obj_serviceUUID);

    jmethodID jni_mid_getCharacteristic =
            (*env)->GetMethodID(env, jni_cid_bluetoothGattService,
                    "getCharacteristic",
                    "(Ljava/util/UUID;)Landroid/bluetooth/BluetoothGattCharacteristic;");

    jobject jni_obj_responseUUID = CALEGetUuidFromString(env,
            OIC_GATT_CHARACTERISTIC_RESPONSE_UUID);

    jobject jni_obj_bluetoothGattCharacteristic = (*env)->CallObjectMethod(env,
            jni_obj_bluetoothGattService, jni_mid_getCharacteristic,
            jni_obj_responseUUID);

    jmethodID jni_mid_setValue = (*env)->GetMethodID(env,
            jni_cid_bluetoothGattCharacteristic, "setValue",
            "([B)Z");

    jboolean jni_boolean_setValue = (*env)->CallBooleanMethod(env,
            jni_obj_bluetoothGattCharacteristic, jni_mid_setValue,
            responseData);

    if (jni_boolean_setValue == JNI_FALSE) {
        OIC_LOG_V(DEBUG, TAG, "Fail to set response data");
    }

    return jni_obj_bluetoothGattCharacteristic;
}

jboolean CANativeLEServerSendResponseData(JNIEnv *env, jobject device, jobject responseData)
{

    if(!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return JNI_FALSE;
    }

    OIC_LOG_V(DEBUG, TAG, "CALEServerSendResponseData");

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattServer");

    jmethodID jni_mid_notifyCharacteristicChanged =
            (*env)->GetMethodID(env, jni_cid_bluetoothGattServer,
                    "notifyCharacteristicChanged",
                    "(Landroid/bluetooth/BluetoothDevice;Landroid/bluetooth/BluetoothGattCharacteristic;Z)Z");

    jboolean jni_boolean_notifyCharacteristicChanged =
            (*env)->CallBooleanMethod(env, gBluetoothGattServer,
                    jni_mid_notifyCharacteristicChanged, device, responseData,
                    JNI_TRUE);

    if (jni_boolean_notifyCharacteristicChanged == JNI_FALSE) {
        OIC_LOG_V(DEBUG, TAG, "Fail to notify characteristic");
    }

    return jni_boolean_notifyCharacteristicChanged;
}

jboolean CANativeLEServerSendResponse(JNIEnv *env, jobject device, jint requestId, jint status,
        jint offset, jbyteArray value)
{

    if(!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return JNI_FALSE;
    }

    OIC_LOG_V(DEBUG, TAG, "CALEServerSendResponse");

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattServer");

    jmethodID jni_mid_sendResponse = (*env)->GetMethodID(env,
            jni_cid_bluetoothGattServer, "sendResponse",
            "(Landroid/bluetooth/BluetoothDevice;III[B)Z");

    jboolean jni_boolean_sendResponse = (*env)->CallBooleanMethod(env, gBluetoothGattServer,
            jni_mid_sendResponse, device, requestId, status, offset, value);

    if (jni_boolean_sendResponse == JNI_FALSE) {
        OIC_LOG_V(DEBUG, TAG, "Fail to send response for gatt characteristic write request");
    }

    return jni_boolean_sendResponse;
}

void CANativeLEServerStartAdvertise(JNIEnv *env, jobject advertiseCallback)
{

    OIC_LOG_V(DEBUG, TAG, "LEServerStartAdvertise");

    if(!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    jclass jni_cid_AdvertiseSettings = (*env)->FindClass(env,
            "android/bluetooth/le/AdvertiseSettings$Builder");

    jclass jni_cid_AdvertiseDataBuilder = (*env)->FindClass(env,
            "android/bluetooth/le/AdvertiseData$Builder");

    jclass jni_cid_BTAdapter = (*env)->FindClass(env,
            "android/bluetooth/BluetoothAdapter");

    jclass jni_cid_leAdvertiser = (*env)->FindClass(env,
            "android/bluetooth/le/BluetoothLeAdvertiser");

    jmethodID jni_mid_AdvertiseSettings = (*env)->GetMethodID(env,
            jni_cid_AdvertiseSettings, "<init>", "()V");

    jmethodID jni_mid_setAdvertiseMode = (*env)->GetMethodID(env,
            jni_cid_AdvertiseSettings, "setAdvertiseMode",
                "(I)Landroid/bluetooth/le/AdvertiseSettings$Builder;");

    jmethodID jni_mid_setConnectable = (*env)->GetMethodID(env,
            jni_cid_AdvertiseSettings, "setConnectable",
            "(Z)Landroid/bluetooth/le/AdvertiseSettings$Builder;");

    jmethodID jni_mid_setTimeout = (*env)->GetMethodID(env,
            jni_cid_AdvertiseSettings, "setTimeout",
            "(I)Landroid/bluetooth/le/AdvertiseSettings$Builder;");

    jmethodID jni_mid_AdvertiseDataBuilder = (*env)->GetMethodID(env,
            jni_cid_AdvertiseDataBuilder, "<init>", "()V");

    jmethodID jni_mid_addServiceUuid =
            (*env)->GetMethodID(env, jni_cid_AdvertiseDataBuilder,
                    "addServiceUuid",
                    "(Landroid/os/ParcelUuid;)Landroid/bluetooth/le/AdvertiseData$Builder;");

    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env,
            jni_cid_BTAdapter, "getDefaultAdapter",
            "()Landroid/bluetooth/BluetoothAdapter;");

    jmethodID jni_mid_getBluetoothLeAdvertiser = (*env)->GetMethodID(env,
            jni_cid_BTAdapter, "getBluetoothLeAdvertiser",
            "()Landroid/bluetooth/le/BluetoothLeAdvertiser;");

    jmethodID jni_mid_build_LeAdvertiseSettings = (*env)->GetMethodID(env,
            jni_cid_AdvertiseSettings, "build",
            "()Landroid/bluetooth/le/AdvertiseSettings;");

    jmethodID jni_mid_build_LeAdvertiseData = (*env)->GetMethodID(env,
            jni_cid_AdvertiseDataBuilder, "build",
            "()Landroid/bluetooth/le/AdvertiseData;");

    jmethodID jni_mid_startAdvertising =
            (*env)->GetMethodID(env, jni_cid_leAdvertiser, "startAdvertising",
                    "(Landroid/bluetooth/le/AdvertiseSettings;Landroid/bluetooth/le/AdvertiseData;Landroid/bluetooth/le/AdvertiseCallback;)V");

    jobject jni_AdvertiseSettings = (*env)->NewObject(env,
            jni_cid_AdvertiseSettings, jni_mid_AdvertiseSettings);

    jobject jni_obj_setAdvertiseMode = (*env)->CallObjectMethod(env,
                jni_AdvertiseSettings, jni_mid_setAdvertiseMode, 0); // 0: Low power, 1: Balanced

    jobject jni_obj_setConnectable = (*env)->CallObjectMethod(env,
            jni_AdvertiseSettings, jni_mid_setConnectable, JNI_TRUE);

    jobject jni_obj_setTimeout = (*env)->CallObjectMethod(env,
            jni_AdvertiseSettings, jni_mid_setTimeout, 0); //A value of 0 will disable the time limit

    jobject jni_AdvertiseDataBuilder = (*env)->NewObject(env,
            jni_cid_AdvertiseDataBuilder, jni_mid_AdvertiseDataBuilder);

    jobject jni_obj_serviceUUID = CALEGetUuidFromString(env, OIC_GATT_SERVICE_UUID);

    jobject jni_ParcelUuid = CALEGetParcelUuid(env, jni_obj_serviceUUID);

    jobject jni_obj_addServiceUuid = (*env)->CallObjectMethod(env,
            jni_AdvertiseDataBuilder, jni_mid_addServiceUuid, jni_ParcelUuid);

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env,
            jni_cid_BTAdapter, jni_mid_getDefaultAdapter);

    jobject jni_obj_getBluetoothLeAdvertiser = (*env)->CallObjectMethod(env,
            jni_obj_BTAdapter, jni_mid_getBluetoothLeAdvertiser);

    jobject jni_obj_build_LeAdvertiseSettings = (*env)->CallObjectMethod(env,
            jni_AdvertiseSettings, jni_mid_build_LeAdvertiseSettings);

    jobject jni_obj_build_LeAdvertiseData = (*env)->CallObjectMethod(env,
            jni_AdvertiseDataBuilder, jni_mid_build_LeAdvertiseData);

    (*env)->CallVoidMethod(env, jni_obj_getBluetoothLeAdvertiser,
            jni_mid_startAdvertising, jni_obj_build_LeAdvertiseSettings,
            jni_obj_build_LeAdvertiseData, advertiseCallback);

    OIC_LOG_V(DEBUG, TAG, "Advertising started!!");
}

void CANativeLEServerStopAdvertise(JNIEnv *env, jobject advertiseCallback)
{

    OIC_LOG_V(DEBUG, TAG, "LEServerStopAdvertise");

    if(!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    jclass jni_cid_BTAdapter = (*env)->FindClass(env,
            "android/bluetooth/BluetoothAdapter");

    jclass jni_cid_leAdvertiser = (*env)->FindClass(env,
            "android/bluetooth/le/BluetoothLeAdvertiser");

    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env,
            jni_cid_BTAdapter, "getDefaultAdapter",
            "()Landroid/bluetooth/BluetoothAdapter;");

    jmethodID jni_mid_getBluetoothLeAdvertiser = (*env)->GetMethodID(env,
            jni_cid_BTAdapter, "getBluetoothLeAdvertiser",
            "()Landroid/bluetooth/le/BluetoothLeAdvertiser;");

    jmethodID jni_mid_stopAdvertising =
            (*env)->GetMethodID(env, jni_cid_leAdvertiser, "stopAdvertising",
                    "(Landroid/bluetooth/le/AdvertiseCallback;)V");

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env,
            jni_cid_BTAdapter, jni_mid_getDefaultAdapter);

    jobject jni_obj_getBluetoothLeAdvertiser = (*env)->CallObjectMethod(env,
            jni_obj_BTAdapter, jni_mid_getBluetoothLeAdvertiser);

    (*env)->CallVoidMethod(env, jni_obj_getBluetoothLeAdvertiser,
            jni_mid_stopAdvertising, advertiseCallback);

    OIC_LOG_V(DEBUG, TAG, "Advertising stopped!!");
}

jboolean CALEStartGattServer(JNIEnv *env, jobject gattServerCallback)
{

    OIC_LOG_V(DEBUG, TAG, "CALEStartGattServer");

    if(!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return JNI_FALSE;
    }

    if(gIsStartServer)
        OIC_LOG_V(DEBUG, TAG, "Gatt server already started");

    gBluetoothGattServerCallback = (*env)->NewGlobalRef(env, gattServerCallback);

    // open gatt server
    jobject bluetoothGattServer = CANativeLEServerOpenGattServer(env);
    if(!bluetoothGattServer)
    {
        OIC_LOG_V(DEBUG, TAG, "bluetoothGattServer is null");
        return JNI_FALSE;
    }

    gBluetoothGattServer = (*env)->NewGlobalRef(env, bluetoothGattServer);

    // create gatt service
    jobject bluetoothGattService = CANativeLEServerCreateGattService(env);

    // add gatt service
    return CANativeLEServerAddGattService(env, gBluetoothGattServer, bluetoothGattService);
}

jobject CANativeLEServerOpenGattServer(JNIEnv *env)
{

    OIC_LOG_V(DEBUG, TAG, "CALEServerOpenGattServer");

    if(!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return NULL;
    }

    jclass jni_cid_context = (*env)->FindClass(env, "android/content/Context");

    jclass jni_cid_bluetoothManager = (*env)->FindClass(env,
            "android/bluetooth/BluetoothManager");

    jfieldID jni_fid_bluetoothService = (*env)->GetStaticFieldID(env,
            jni_cid_context, "BLUETOOTH_SERVICE", "Ljava/lang/String;");

    jmethodID jni_mid_getSystemService = (*env)->GetMethodID(env,
            jni_cid_context, "getSystemService",
            "(Ljava/lang/String;)Ljava/lang/Object;");

    jmethodID jni_mid_openGattServer =
            (*env)->GetMethodID(env, jni_cid_bluetoothManager, "openGattServer",
                    "(Landroid/content/Context;Landroid/bluetooth/BluetoothGattServerCallback;)Landroid/bluetooth/BluetoothGattServer;");

    jobject jni_obj_bluetoothService = (*env)->GetStaticObjectField(env,
            jni_cid_context, jni_fid_bluetoothService);
    if(!jni_obj_bluetoothService)
    {
        OIC_LOG_V(DEBUG, TAG, "jni_obj_bluetoothService is null");
        return JNI_FALSE;
    }


    jobject jni_obj_bluetoothManager = (*env)->CallObjectMethod(env, gContext,
            jni_mid_getSystemService, jni_obj_bluetoothService);
    if(!jni_obj_bluetoothManager)
    {
        OIC_LOG_V(DEBUG, TAG, "jni_obj_bluetoothManager is null");
        return JNI_FALSE;
    }

    jobject jni_obj_bluetoothGattServer = (*env)->CallObjectMethod(env,
            jni_obj_bluetoothManager, jni_mid_openGattServer, gContext,
            gBluetoothGattServerCallback);
    if(!jni_obj_bluetoothGattServer)
    {
        OIC_LOG_V(DEBUG, TAG, "jni_obj_bluetoothGattServer is null");
        return JNI_FALSE;
    }

    return jni_obj_bluetoothGattServer;
}

jobject CANativeLEServerCreateGattService(JNIEnv *env)
{

    OIC_LOG_V(DEBUG, TAG, "CALEServerCreateGattService");

    if(!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return NULL;
    }

    jclass jni_cid_bluetoothGattService = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattService");

    jclass jni_cid_bluetoothGattCharacteristic = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattCharacteristic");

    jfieldID jni_fid_serviceType = (*env)->GetStaticFieldID(env,
            jni_cid_bluetoothGattService, "SERVICE_TYPE_PRIMARY", "I");

    jfieldID jni_fid_readProperties = (*env)->GetStaticFieldID(env,
            jni_cid_bluetoothGattCharacteristic, "PROPERTY_READ", "I");

    jfieldID jni_fid_writeProperties = (*env)->GetStaticFieldID(env,
            jni_cid_bluetoothGattCharacteristic, "PROPERTY_WRITE", "I");

    jfieldID jni_fid_readPermissions = (*env)->GetStaticFieldID(env,
            jni_cid_bluetoothGattCharacteristic, "PERMISSION_READ", "I");

    jfieldID jni_fid_writePermissions = (*env)->GetStaticFieldID(env,
            jni_cid_bluetoothGattCharacteristic, "PERMISSION_WRITE", "I");

    jmethodID jni_mid_bluetoothGattService = (*env)->GetMethodID(env,
            jni_cid_bluetoothGattService, "<init>", "(Ljava/util/UUID;I)V");

    jmethodID jni_mid_addCharacteristic = (*env)->GetMethodID(env,
            jni_cid_bluetoothGattService, "addCharacteristic",
            "(Landroid/bluetooth/BluetoothGattCharacteristic;)Z");

    jmethodID jni_mid_bluetoothGattCharacteristic = (*env)->GetMethodID(env,
            jni_cid_bluetoothGattCharacteristic, "<init>",
            "(Ljava/util/UUID;II)V");

    jobject jni_obj_serviceUUID = CALEGetUuidFromString(env, OIC_GATT_SERVICE_UUID);

    jobject jni_obj_serviceType = (*env)->GetStaticObjectField(env,
            jni_cid_bluetoothGattService, jni_fid_serviceType);

    jobject jni_bluetoothGattService = (*env)->NewObject(env,
            jni_cid_bluetoothGattService, jni_mid_bluetoothGattService,
            jni_obj_serviceUUID, jni_obj_serviceType);

    jobject jni_obj_readUuid = CALEGetUuidFromString(env,
            OIC_GATT_CHARACTERISTIC_RESPONSE_UUID);

    jint jni_int_readProperties = (*env)->GetStaticIntField(env,
            jni_cid_bluetoothGattCharacteristic, jni_fid_readProperties);

    jint jni_int_readPermissions = (*env)->GetStaticIntField(env,
            jni_cid_bluetoothGattCharacteristic, jni_fid_readPermissions);

    jobject jni_readCharacteristic = (*env)->NewObject(env,
            jni_cid_bluetoothGattCharacteristic,
            jni_mid_bluetoothGattCharacteristic, jni_obj_readUuid,
            jni_int_readProperties, jni_int_readPermissions);

    jboolean jni_boolean_addReadCharacteristic = (*env)->CallBooleanMethod(env,
            jni_bluetoothGattService, jni_mid_addCharacteristic,
            jni_readCharacteristic);

    jobject jni_obj_writeUuid = CALEGetUuidFromString(env,
            OIC_GATT_CHARACTERISTIC_REQUEST_UUID);

    jint jni_int_writeProperties = (*env)->GetStaticIntField(env,
            jni_cid_bluetoothGattCharacteristic, jni_fid_writeProperties);

    jint jni_int_writePermissions = (*env)->GetStaticIntField(env,
            jni_cid_bluetoothGattCharacteristic, jni_fid_writePermissions);

    jobject jni_writeCharacteristic = (*env)->NewObject(env,
            jni_cid_bluetoothGattCharacteristic,
            jni_mid_bluetoothGattCharacteristic, jni_obj_writeUuid,
            jni_int_writeProperties, jni_int_writePermissions);

    jboolean jni_boolean_addWriteCharacteristic = (*env)->CallBooleanMethod(env,
            jni_bluetoothGattService, jni_mid_addCharacteristic,
            jni_writeCharacteristic);

    if (jni_boolean_addWriteCharacteristic == JNI_FALSE)
    {
        OIC_LOG_V(DEBUG, TAG, "Fail to add jni_boolean_addReadCharacteristic");
        return NULL;
    }

    return jni_bluetoothGattService;
}

jboolean CANativeLEServerAddGattService(JNIEnv *env, jobject bluetoothGattServer,
        jobject bluetoothGattService)
{

    OIC_LOG_V(DEBUG, TAG, "CALEServerAddGattService");

    if(!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return JNI_FALSE;
    }

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattServer");

    jmethodID jni_mid_addService = (*env)->GetMethodID(env,
            jni_cid_bluetoothGattServer, "addService",
            "(Landroid/bluetooth/BluetoothGattService;)Z");

    jboolean jni_boolean_addService = (*env)->CallBooleanMethod(env, bluetoothGattServer,
            jni_mid_addService, bluetoothGattService);

    if (jni_boolean_addService == JNI_FALSE)
    {
        OIC_LOG_V(DEBUG, TAG, "Fail to add gatt service");
    }

    return jni_boolean_addService;
}

jboolean CANativeLEServerConnect(JNIEnv *env, jobject bluetoothDevice)
{

    OIC_LOG_V(DEBUG, TAG, "CALEConnect");

    if(!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return JNI_FALSE;
    }

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattServer");

    jmethodID jni_mid_connect = (*env)->GetMethodID(env,
            jni_cid_bluetoothGattServer, "connect",
            "(Landroid/bluetooth/BluetoothDevice;Z)Z");

    jboolean jni_boolean_connect = (*env)->CallBooleanMethod(env,
            gBluetoothGattServer, jni_mid_connect, bluetoothDevice, JNI_FALSE);

    if(jni_boolean_connect == JNI_FALSE) {
        OIC_LOG_V(DEBUG, TAG, "Fail to connect");
    }

    return jni_boolean_connect;
}

void CANativeLEServerDisconnect(JNIEnv *env, jobject bluetoothDevice)
{

    OIC_LOG_V(DEBUG, TAG, "CALEDisconnect");

    if(!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattServer");

    jmethodID jni_mid_cancelConnection = (*env)->GetMethodID(env,
            jni_cid_bluetoothGattServer, "cancelConnection",
            "(Landroid/bluetooth/BluetoothDevice;)V");

    (*env)->CallVoidMethod(env, gBluetoothGattServer, jni_mid_cancelConnection,
            bluetoothDevice);
}

jboolean CALEServerSend(JNIEnv *env, jobject bluetoothDevice, jbyteArray responseData)
{

    OIC_LOG_V(DEBUG, TAG, "CALESend");

    if(!CALEIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return JNI_FALSE;
    }

    jobject responseChar = CANativeLEServerSetResponseData(env, responseData);

    jboolean result = CANativeLEServerSendResponseData(env, bluetoothDevice, responseChar);

    if(result == JNI_FALSE)
    {
        OIC_LOG_V(DEBUG, TAG, "Fail to send response data");
    }

    return result;
}

void CALeServerCreateJniInterfaceObject()
{
    OIC_LOG_V(DEBUG, TAG, "CALeServerCreateJniInterfaceObject");

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

    // initialize le server

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);
}

void CALEServerInitialize(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "CALEServerInitialize");

    gThreadPoolHandle = handle;

    gIsSendingMulticastData = FALSE;

    CALEServerCreateCachedDeviceList();
}

void CALEServerTerminate()
{
    OIC_LOG(DEBUG, TAG, "CALEServerTerminate");

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

    CALEServerStopMulticastServer(0);

    CALEServerRemoveAllDevices(env);

    if(gLeAdvertiseCallback)
    {
        (*env)->DeleteGlobalRef(env, gLeAdvertiseCallback);
    }

    if(gBluetoothGattServer)
    {
        (*env)->DeleteGlobalRef(env, gBluetoothGattServer);
    }

    if(gBluetoothGattServerCallback)
    {
        (*env)->DeleteGlobalRef(env, gBluetoothGattServerCallback);
    }

    if(gContext)
    {
        (*env)->DeleteGlobalRef(env, gContext);
    }

    gIsStartServer = FALSE;

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);
}

int32_t CALEServerSendUnicastMessage(const char* address, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerSendUnicastMessage(%s, %s)", address, data);

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
            return -1;
        }
        isAttached = TRUE;
    }

    CALEServerSendUnicastMessageImpl(env, address, data, dataLen);

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    return 0;
}

int32_t CALEServerSendMulticastMessage(const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerSendMulticastMessage(%s)", data);

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
            return -1;
        }
        isAttached = TRUE;
    }

    CALEServerSendMulticastMessageImpl(env, data, dataLen);

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    return 0;
}

int32_t CALEServerStartUnicastServer(const char* address)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerStartUnicastServer(%s)", address);

    return 0;
}

int32_t CALEServerStartMulticastServer()
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerStartMulticastServer");

    if(gIsStartServer)
    {
        OIC_LOG_V(DEBUG, TAG, "server is already started..it will be skipped");
        return 0;
    }

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
            return -1;
        }
        isAttached = TRUE;
    }

    gIsStartServer = TRUE;

    // start gatt server
    if( CALEStartGattServer(env, gBluetoothGattServerCallback) == JNI_FALSE) {
        OIC_LOG_V(DEBUG, TAG, "Fail to start gatt server");
        return -1;
    }

    // start advertise
    CANativeLEServerStartAdvertise(env, gLeAdvertiseCallback);

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    return 0;
}

int32_t CALEServerStopUnicastServer(int32_t serverID)
{
    OIC_LOG(DEBUG, TAG, "CALEServerStopUnicastServer");

    return 0;
}

int32_t CALEServerStopMulticastServer(int32_t serverID)
{
    OIC_LOG(DEBUG, TAG, "CALEServerStopMulticastServer");

    if(gIsStartServer == FALSE)
    {
        OIC_LOG_V(DEBUG, TAG, "server is already stopped..it will be skipped");
        return 0;
    }

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
            return -1;
        }
        isAttached = TRUE;
    }

    CANativeLEServerStopAdvertise(env, gLeAdvertiseCallback);

    gIsStartServer = FALSE;

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    return 0;
}

void CALEServerSetCallback(CAPacketReceiveCallback callback)
{
    OIC_LOG(DEBUG, TAG, "CALEServerSetCallback");
    gPacketReceiveCallback = callback;
}

void CALEServerGetInterfaceInfo(CALocalConnectivity_t **info, uint32_t* size)
{
    OIC_LOG(DEBUG, TAG, "CALEServerGetInterfaceInfo");
    return;
}

void CALEServerGetLocalAddress(char* address)
{
    OIC_LOG(DEBUG, TAG, "CALEServerGetLocalAddress");

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

    jstring jni_address = CALEGetLocalDeviceAddress(env);
    const char* localAddress = (*env)->GetStringUTFChars(env, jni_address, NULL);
    memcpy(address, localAddress, strlen(localAddress));

    OIC_LOG_V(DEBUG, TAG, "Local Address : %s", address);
    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);
}

int32_t CALEServerSendUnicastMessageImpl(JNIEnv *env, const char* address, const char* data,
        uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerSendUnicastMessageImpl, address: %s, data: %s", address, data);

    // 1. get device object with address from cache
    // 2. connect to the gatt client device
    // 3. write a characteristic for response
    // 4. notify it
    // 5. disconnect

    jobject jni_obj_bluetoothDevice = NULL;

    if(gConnectedDeviceList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "gConnectedDeviceList is null");
    }

    if(gConnectedDeviceList)
    {
        jint index;
        for (index = 0; index < u_arraylist_length(gConnectedDeviceList); index++)
        {
            OIC_LOG(DEBUG, TAG, "check device address");
            jobject jarrayObj = (jobject) u_arraylist_get(gConnectedDeviceList, index);
            if(!jarrayObj)
            {
                OIC_LOG(DEBUG, TAG, "jarrayObj is null");
                return -1;
            }

            jstring jni_setAddress = CALEGetAddressFromBTDevice(env, jarrayObj);
            if(!jni_setAddress)
            {
                OIC_LOG(DEBUG, TAG, "jni_setAddress is null");
                return -1;
            }
            const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);

            if(jarrayObj == NULL) {
                OIC_LOG(DEBUG, TAG, "jarrayObj is null");
            }

            if(!strcmp(setAddress, address))
            {
                OIC_LOG(DEBUG, TAG, "device address matched");
                jni_obj_bluetoothDevice = jarrayObj;
                break;
            }
            jni_obj_bluetoothDevice = jarrayObj;
        }

        if(jni_obj_bluetoothDevice)
        {
            jbyteArray jni_bytearr_data = (*env)->NewByteArray(env, dataLen);
            (*env)->SetByteArrayRegion(env, jni_bytearr_data, 0, dataLen, (jbyte*)data);

            CALEServerSend(env, jni_obj_bluetoothDevice, jni_bytearr_data);

        } else {
            OIC_LOG(DEBUG, TAG, "jni_obj_bluetoothDevice is null");
        }
    }
    return 0;
}

int32_t CALEServerSendMulticastMessageImpl(JNIEnv *env, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerSendMulticastMessageImpl, send to, data: %s", data);

    if(!gConnectedDeviceList)
    {
        OIC_LOG(DEBUG, TAG, "gConnectedDeviceList is null");
        return 0;
    }

    // 1. get all the device objects from cache
    // 2. connect to the gatt client devices
    // 3. write a characteristic for response
    // 4. notify it to every devices
    // 5. disconnect

    jint index;
    for (index = 0; index < u_arraylist_length(gConnectedDeviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gConnectedDeviceList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "jarrayObj is null");
            return -1;
        }

        gIsSendingMulticastData = TRUE;
        CANativeLEServerConnect(env, jarrayObj);

        sleep(1);
    }

    return 0;
}

JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CARegisterLeGattServerCallback
  (JNIEnv *env, jobject obj, jobject callback)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - Register Le Gatt Server Callback");

    gBluetoothGattServerCallback = (*env)->NewGlobalRef(env, callback);
}

JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CARegisterBluetoothLeAdvertiseCallback
  (JNIEnv *env, jobject obj, jobject callback)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - Register Le Advertise Callback");

    gLeAdvertiseCallback = (*env)->NewGlobalRef(env, callback);
}

JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CALeGattServerConnectionStateChangeCallback
  (JNIEnv *env, jobject obj, jobject device, jint status, jint newState)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - Gatt Server ConnectionStateChange Callback");

    OIC_LOG_V(DEBUG, TAG, "New connection State: %d", newState);

    if (!device)
    {
        OIC_LOG(DEBUG, TAG, "device is null");
        return;
    }

    jclass jni_cid_bluetoothProfile = (*env)->FindClass(env,
            "android/bluetooth/BluetoothProfile");

    jfieldID jni_fid_state_connected = (*env)->GetStaticFieldID(env,
            jni_cid_bluetoothProfile, "STATE_CONNECTED", "I");

    jfieldID jni_fid_state_disconnected = (*env)->GetStaticFieldID(env,
                jni_cid_bluetoothProfile, "STATE_DISCONNECTED", "I");

    // STATE_CONNECTED
    jint jni_int_state_connected = (*env)->GetStaticIntField(env,
            jni_cid_bluetoothProfile, jni_fid_state_connected);

    // STATE_DISCONNECTED
    jint jni_int_state_disconnected = (*env)->GetStaticIntField(env,
                jni_cid_bluetoothProfile, jni_fid_state_disconnected);

    if(newState == jni_int_state_connected)
    {

        OIC_LOG_V(DEBUG, TAG, "LE CONNECTED");

        jstring jni_remoteAddress = CALEGetAddressFromBTDevice(env, device);
        if (!jni_remoteAddress)
        {
            OIC_LOG(DEBUG, TAG, "jni_remoteAddress is null");
            return;
        }

        const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

        if (gConnectedDeviceList == NULL)
        {
            OIC_LOG_V(DEBUG, TAG, "gConnectedDeviceList is null");
        }

        if(CALEServerIsDeviceInList(env, remoteAddress) == JNI_FALSE)
        {
            OIC_LOG_V(DEBUG, TAG, "add connected device to cache");
            CALEServerAddDeviceToList(env, device);
        }

//        // temp data
//        if(gIsSendingMulticastData)
//        {
//            OIC_LOG_V(DEBUG, TAG, "send data");
//
//            const char* data = "HelloWorld~";
//            uint32_t dataLen =  strlen(data);
//            jbyteArray jni_bytearr_data = (*env)->NewByteArray(env, dataLen);
//            (*env)->SetByteArrayRegion(env, jni_bytearr_data, 0, dataLen, (jbyte*)data);
//            CALEServerSend(env, device, jni_bytearr_data);
//            gIsSendingMulticastData = FALSE;
//        }


    }
    else if(newState == jni_int_state_disconnected)
    {
        OIC_LOG_V(DEBUG, TAG, "LE DISCONNECTED");
    }
}

void CALEServerCreateCachedDeviceList()
{
    OIC_LOG(DEBUG, TAG, "CALEServerCreateCachedDeviceList");

    // create new object array
    if (gConnectedDeviceList == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "Create device list");

        gConnectedDeviceList = u_arraylist_create();
    }
}

jboolean CALEServerIsDeviceInList(JNIEnv *env, const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "CALEServerIsDeviceInList");

    if(gConnectedDeviceList == NULL)
        OIC_LOG(DEBUG, TAG, "list is null");

    uint32_t len = u_arraylist_length(gConnectedDeviceList);

    uint32_t index;
    for (index = 0; index < u_arraylist_length(gConnectedDeviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gConnectedDeviceList, index);

        if (!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "jarrayObj is null");
            return JNI_TRUE;
        }

        jstring jni_setAddress = CALEGetAddressFromBTDevice(env, jarrayObj);
        if (!jni_setAddress)
        {
            OIC_LOG(DEBUG, TAG, "jni_setAddress is null");
            return JNI_TRUE;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress,
                NULL);

        if (!strcmp(remoteAddress, setAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "the device is already set");
            return JNI_TRUE;
        }
        else
        {
            continue;
        }
    }

    OIC_LOG_V(DEBUG, TAG, "no device in list");
    return JNI_FALSE;
}

void CALEServerAddDeviceToList(JNIEnv *env, jobject device)
{
    if (device == NULL)
    {
        OIC_LOG(DEBUG, TAG, "device is null");
        return;
    }

    if (gConnectedDeviceList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "list is null");
        return;
    }

    jstring jni_remoteAddress = CALEGetAddressFromBTDevice(env, device);
    if (!jni_remoteAddress)
    {
        OIC_LOG(DEBUG, TAG, "jni_remoteAddress is null");
        return;
    }

    const char* remoteAddress = (*env)->GetStringUTFChars(env,
            jni_remoteAddress, NULL);

    if (CALEServerIsDeviceInList(env, remoteAddress) == JNI_FALSE)
    {
        jobject gdevice = (*env)->NewGlobalRef(env, device);
        u_arraylist_add(gConnectedDeviceList, gdevice);
        OIC_LOG_V(DEBUG, TAG, "Set Object to Array as Element");
    }
}

void CALEServerRemoveAllDevices(JNIEnv *env)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerRemoveAllDevices");

    if (!gConnectedDeviceList)
    {
        OIC_LOG(DEBUG, TAG, "no deviceList");
        return;
    }

    uint32_t index;
    for (index = 0; index < u_arraylist_length(gConnectedDeviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gConnectedDeviceList, index);

        if (jarrayObj)
        {
            (*env)->DeleteGlobalRef(env, jarrayObj);
        }
    }

    OICFree(gConnectedDeviceList);
    gConnectedDeviceList = NULL;
    return;
}

void CALEServerRemoveDevice(JNIEnv *env, jstring address)
{
    OIC_LOG_V(DEBUG, TAG, "CALEServerRemoveDevice");

    if (!gConnectedDeviceList)
    {
        OIC_LOG(DEBUG, TAG, "no deviceList");
        return;
    }

    uint32_t index;
    for (index = 0; index < u_arraylist_length(gConnectedDeviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gConnectedDeviceList, index);

        if (jarrayObj)
        {
            jstring jni_setAddress = CALEGetAddressFromBTDevice(env, jarrayObj);
            if (!jni_setAddress)
            {
                OIC_LOG(DEBUG, TAG, "wrong device address");
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
    }
    OIC_LOG(DEBUG, TAG, "no target object");
    return;
}

void CALEServerReorderinglist(uint32_t index)
{
    if (index >= gConnectedDeviceList->length)
        return;

    if (index < gConnectedDeviceList->length - 1)
    {
        memmove(&gConnectedDeviceList->data[index], &gConnectedDeviceList->data[index + 1],
                (gConnectedDeviceList->length - index - 1) * sizeof(void *));
    }

    gConnectedDeviceList->size--;
    gConnectedDeviceList->length--;
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CALeInterface_CALeGattServerServiceAddedCallback
(JNIEnv *env, jobject obj, jint status, jobject gattService)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - Gatt Service Added Callback(%d)", status);
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CALeInterface_CALeGattServerCharacteristicReadRequestCallback
(JNIEnv *env, jobject obj, jobject device, jint requestId, jint offset, jobject characteristic, jbyteArray data)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - Gatt Server Characteristic Read Request Callback");

    CANativeLEServerSendResponse(env, device, requestId, 0, offset, NULL);
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CALeInterface_CALeGattServerCharacteristicWriteRequestCallback
(JNIEnv *env, jobject obj, jobject device, jint requestId, jobject characteristic, jbyteArray data,
        jboolean preparedWrite, jboolean responseNeeded, jint offset, jbyteArray value)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - Gatt Server Characteristic Write Request Callback");

    CANativeLEServerSendResponse(env, device, requestId, 0, offset, value);

    if(data == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "Reqeust data is null");
        return;
    }

    // get Byte Array and covert to char*
    jint length = (*env)->GetArrayLength(env, data);

    jboolean isCopy;
    jbyte *jni_byte_requestData = (char*)(*env)->GetByteArrayElements(env, data, &isCopy);

    char* requestData = NULL;
    requestData = (char*) OICMalloc (sizeof(char) * length);
    memset(requestData, 0, sizeof(char) * length);
    strncpy(requestData, (char*)jni_byte_requestData, length);
    requestData[length] = '\0';
    (*env)->ReleaseByteArrayElements(env, data, jni_byte_requestData, JNI_ABORT);

    jstring jni_address = CALEGetAddressFromBTDevice(env, device);
    const char* address = (*env)->GetStringUTFChars(env, jni_address, NULL);
    OIC_LOG_V(DEBUG, TAG, "remote device address : %s", address);

    gPacketReceiveCallback(address, requestData);
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CALeInterface_CALeGattServerDescriptorReadRequestCallback
(JNIEnv *env, jobject obj, jobject device, jint requestId, jint offset, jobject descriptor)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface_CALeGattServerDescriptorReadRequestCallback");
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CALeInterface_CALeGattServerDescriptorWriteRequestCallback
(JNIEnv *env, jobject obj, jobject device, jint requestId, jobject descriptor,
        jboolean preparedWrite, jboolean responseNeeded, jint offset, jbyteArray value)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface_CALeGattServerDescriptorWriteRequestCallback");
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CALeInterface_CALeGattServerExecuteWriteCallback
(JNIEnv *env, jobject obj, jobject device, jint requestId, jboolean execute)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface_CALeGattServerExecuteWriteCallback");

    CANativeLEServerSendResponse(env, device, requestId, 0, 0, NULL);
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CALeInterface_CALeGattServerNotificationSentCallback
(JNIEnv *env, jobject obj, jobject device, jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - Gatt Server Notification Sent Callback");
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CALeInterface_CALeAdvertiseStartSuccessCallback
(JNIEnv *env, jobject obj, jobject settingsInEffect)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - LE Advertise Start Success Callback");
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CALeInterface_CALeAdvertiseStartFailureCallback
(JNIEnv *env, jobject obj, jint errorCode)
{
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - LE Advertise Start Failure Callback(%)", errorCode);
}

