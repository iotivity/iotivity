#include <jni.h>
#include <stdio.h>
#include <android/log.h>
#include "com_iotivity_jar_CALeInterface.h"

#define  LOG_TAG   "jni_BLE_Server"
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static const char *OIC_GATT_SERVICE_UUID = "713d0000-503e-4c75-ba94-3148f18d941e";
static const char *OIC_GATT_CHARACTERISTIC_RESPONSE_UUID = "713d0002-503e-4c75-ba94-3148f18d941e";
static const char *OIC_GATT_CHARACTERISTIC_REQUEST_UUID = "713d0003-503e-4c75-ba94-3148f18d941e";

static jobject gContext;
static jobject gBluetoothGattServer;
static jobject gBluetoothGattServerCallback;
static jobject gAdvertiseCallback;

jobject getUuidFromString(JNIEnv *env, const char* uuid)
{

    LOGI("[BLE Server] getUuidFromString");

    jclass jni_cid_UUID = (*env)->FindClass(env, "java/util/UUID");
    if (!jni_cid_UUID)
    {
        LOGI("[BLE Server] jni_cid_UUID is null");
        return NULL;
    }

    jmethodID jni_mid_fromString = (*env)->GetStaticMethodID(env, jni_cid_UUID, "fromString",
            "(Ljava/lang/String;)Ljava/util/UUID;");
    if (!jni_mid_fromString)
    {
        LOGI("[BLE Server] jni_mid_fromString is null");
        return NULL;
    }

    jstring str_serviceUUID = (*env)->NewStringUTF(env, uuid);
    LOGI("[BLE Server] uuid: %s", uuid);
    jobject jni_obj_serviceUUID = (*env)->CallStaticObjectMethod(env, jni_cid_UUID,
            jni_mid_fromString, str_serviceUUID);
    if (!jni_obj_serviceUUID)
    {
        LOGI("[BLE][Server] jni_obj_serviceUUID is null");
        return NULL;
    }

    return jni_obj_serviceUUID;
}

jobject getParcelUuid(JNIEnv *env, jobject uuid)
{

    jclass jni_cid_ParcelUuid = (*env)->FindClass(env, "android/os/ParcelUuid");
    if (!jni_cid_ParcelUuid)
    {
        LOGI("[BLE Server] jni_cid_ParcelUuid is null");
        return;
    }

    jmethodID jni_mid_ParcelUuid = (*env)->GetMethodID(env, jni_cid_ParcelUuid, "<init>",
            "(Ljava/util/UUID;)V");
    if (!jni_mid_ParcelUuid)
    {
        LOGI("[BLE Server] jni_mid_ParcelUuid is null");
        return;
    }

    jobject jni_ParcelUuid = (*env)->NewObject(env, jni_cid_ParcelUuid, jni_mid_ParcelUuid, uuid);
    if (!jni_ParcelUuid)
    {
        LOGI("[BLE Server] jni_ParcelUuid is null");
        return;
    }

    return jni_ParcelUuid;
}

jobject setData(JNIEnv *env, char* responseData)
{

    LOGI("[BLE Server] set response");

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattServer");
    jclass jni_cid_bluetoothGattService = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattService");
    jclass jni_cid_bluetoothGattCharacteristic = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattCharacteristic");
    LOGI("[BLE Server] get classes");

    jmethodID jni_mid_getService = (*env)->GetMethodID(env, jni_cid_bluetoothGattService,
            "getService", "(Ljava/util/UUID;)Landroid/bluetooth/BluetoothGattService;");
    LOGI("[BLE Server] get methodID");

    jobject jni_obj_serviceUUID = getUuidFromString(env, OIC_GATT_SERVICE_UUID);
    LOGI("[BLE Server] get serviceUUID");

    if (!gBluetoothGattServer)
    {
        LOGI("[BLE Server] gBluetoothGattServer is null");
        return NULL;
    }

    jobject jni_obj_bluetoothGattService = (*env)->CallObjectMethod(env, gBluetoothGattServer,
            jni_mid_getService, jni_obj_serviceUUID);
    LOGI("[BLE Server] jni_obj_bluetoothGattService");

    jmethodID jni_mid_getCharacteristic = (*env)->GetMethodID(env, jni_cid_bluetoothGattServer,
            "getCharacteristic",
            "(Ljava/util/UUID;)Landroid/bluetooth/BluetoothGattCharacteristic;");

    jobject jni_obj_responseUUID = getUuidFromString(env, OIC_GATT_CHARACTERISTIC_RESPONSE_UUID);
    jobject jni_obj_bluetoothGattCharacteristic = (*env)->CallObjectMethod(env,
            jni_obj_bluetoothGattService, jni_mid_getCharacteristic, jni_obj_responseUUID);
    LOGI("[BLE Server] jni_obj_bluetoothGattCharacteristic");

    jmethodID jni_mid_setValue = (*env)->GetMethodID(env, jni_cid_bluetoothGattCharacteristic,
            "setValue", "(Ljava/land/String;)Z");
    jstring str_responseData = (*env)->NewStringUTF(env, responseData);
    jboolean jni_boolean_setValue = (*env)->CallBooleanMethod(env,
            jni_obj_bluetoothGattCharacteristic, jni_mid_setValue, str_responseData);

    return jni_obj_bluetoothGattCharacteristic;
}

jboolean sendData(JNIEnv *env, jobject device, jobject responseData)
{

    if (!device)
    {
        LOGI("[BLE Server] device is null");
        return JNI_FALSE;
    }

    if (!responseData)
    {
        LOGI("[BLE Server] responseData is null");
        return JNI_FALSE;
    }

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattServer");

    jmethodID jni_mid_notifyCharacteristicChanged =
            (*env)->GetMethodID(env, jni_cid_bluetoothGattServer, "notifyCharacteristicChanged",
                    "(Landroid/bluetooth/BluetoothDevice;Landroid/bluetooth/BluetoothGattCharacteristic;Z)Z");
    jboolean jni_boolean_notifyCharacteristicChanged = (*env)->CallBooleanMethod(env,
            gBluetoothGattServer, jni_mid_notifyCharacteristicChanged, device, responseData,
            JNI_TRUE);

    if (!jni_boolean_notifyCharacteristicChanged)
        return JNI_FALSE;

    return JNI_TRUE;
}

jboolean sendResponse(JNIEnv *env, jobject device, jint requestId, jint status, jint offset)
{

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattServer");
    jmethodID jni_mid_sendResponse = (*env)->GetMethodID(env, jni_cid_bluetoothGattServer,
            "sendResponse", "(Landroid/bluetooth/BluetoothDevice;III)Z");
    (*env)->CallVoidMethod(env, gBluetoothGattServer, jni_mid_sendResponse, device, requestId,
            status, offset);
    LOGI("[BLE Server] sendResponse");

}

void startAdvertize(JNIEnv *env, jobject advertiseCallback)
{

    jclass jni_cid_AdvertiseSettings = (*env)->FindClass(env,
            "android/bluetooth/le/AdvertiseSettings$Builder");

    jmethodID jni_mid_AdvertiseSettings = (*env)->GetMethodID(env, jni_cid_AdvertiseSettings,
            "<init>", "()V");

    jobject jni_AdvertiseSettings = (*env)->NewObject(env, jni_cid_AdvertiseSettings,
            jni_mid_AdvertiseSettings);
    if (!jni_AdvertiseSettings)
    {
        LOGI("[BLE Server] jni_AdvertiseSettings is null");
        return;
    }

    jmethodID jni_mid_setConnectable = (*env)->GetMethodID(env, jni_cid_AdvertiseSettings,
            "setConnectable", "(Z)Landroid/bluetooth/le/AdvertiseSettings$Builder;");

    jobject jni_obj_setConnectable = (*env)->CallObjectMethod(env, jni_AdvertiseSettings,
            jni_mid_setConnectable, JNI_TRUE);
    if (!jni_obj_setConnectable)
    {
        LOGI("[BLE][Server] jni_obj_setConnectable is null");
        return;
    }

    jmethodID jni_mid_setTimeout = (*env)->GetMethodID(env, jni_cid_AdvertiseSettings, "setTimeout",
            "(I)Landroid/bluetooth/le/AdvertiseSettings$Builder;");

    jobject jni_obj_setTimeout = (*env)->CallObjectMethod(env, jni_AdvertiseSettings,
            jni_mid_setTimeout, 10000);
    if (!jni_obj_setTimeout)
    {
        LOGI("[BLE][Server] jni_obj_setTimeout is null");
        return;
    }

    jclass jni_cid_AdvertiseDataBuilder = (*env)->FindClass(env,
            "android/bluetooth/le/AdvertiseData$Builder");

    jmethodID jni_mid_AdvertiseDataBuilder = (*env)->GetMethodID(env, jni_cid_AdvertiseDataBuilder,
            "<init>", "()V");

    jobject jni_AdvertiseDataBuilder = (*env)->NewObject(env, jni_cid_AdvertiseDataBuilder,
            jni_mid_AdvertiseDataBuilder);
    if (!jni_AdvertiseDataBuilder)
    {
        LOGI("[BLE Server] jni_AdvertiseDataBuilder is null");
        return;
    }

    jobject jni_obj_serviceUUID = getUuidFromString(env, OIC_GATT_SERVICE_UUID);
    jobject jni_ParcelUuid = getParcelUuid(env, jni_obj_serviceUUID);
    jmethodID jni_mid_addServiceUuid = (*env)->GetMethodID(env, jni_cid_AdvertiseDataBuilder,
            "addServiceUuid",
            "(Landroid/os/ParcelUuid;)Landroid/bluetooth/le/AdvertiseData$Builder;");

    jobject jni_obj_addServiceUuid = (*env)->CallObjectMethod(env, jni_AdvertiseDataBuilder,
            jni_mid_addServiceUuid, jni_ParcelUuid);
    if (!jni_obj_addServiceUuid)
    {
        LOGI("[BLE Server] jni_obj_addServiceUuid is null");
        return;
    }

    jclass jni_cid_BTAdapter = (*env)->FindClass(env, "android/bluetooth/BluetoothAdapter");
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env, jni_cid_BTAdapter,
            "getDefaultAdapter", "()Landroid/bluetooth/BluetoothAdapter;");

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter,
            jni_mid_getDefaultAdapter);
    if (!jni_obj_BTAdapter)
    {
        LOGI("[BLE Server] jni_obj_BTAdapter is null");
        return;
    }

    jmethodID jni_mid_getBluetoothLeAdvertiser = (*env)->GetMethodID(env, jni_cid_BTAdapter,
            "getBluetoothLeAdvertiser", "()Landroid/bluetooth/le/BluetoothLeAdvertiser;");

    jobject jni_obj_getBluetoothLeAdvertiser = (*env)->CallObjectMethod(env, jni_obj_BTAdapter,
            jni_mid_getBluetoothLeAdvertiser);
    if (!jni_obj_getBluetoothLeAdvertiser)
    {
        LOGI("[BLE Server] jni_obj_getBluetoothLeAdvertiser is null");
        return;
    }

    jmethodID jni_mid_build_LeAdvertiseSettings = (*env)->GetMethodID(env,
            jni_cid_AdvertiseSettings, "build", "()Landroid/bluetooth/le/AdvertiseSettings;");

    jobject jni_obj_build_LeAdvertiseSettings = (*env)->CallObjectMethod(env, jni_AdvertiseSettings,
            jni_mid_build_LeAdvertiseSettings);
    if (!jni_obj_build_LeAdvertiseSettings)
    {
        LOGI("[BLE Server] jni_obj_build_LeAdvertiseSettings is null");
        return;
    }

    jmethodID jni_mid_build_LeAdvertiseData = (*env)->GetMethodID(env, jni_cid_AdvertiseDataBuilder,
            "build", "()Landroid/bluetooth/le/AdvertiseData;");

    jobject jni_obj_build_LeAdvertiseData = (*env)->CallObjectMethod(env, jni_AdvertiseDataBuilder,
            jni_mid_build_LeAdvertiseData);
    if (!jni_obj_build_LeAdvertiseData)
    {
        LOGI("[BLE Server] jni_obj_build_LeAdvertiseData is null");
        return;
    }

    jclass jni_cid_leAdvertiser = (*env)->FindClass(env,
            "android/bluetooth/le/BluetoothLeAdvertiser");

    jmethodID jni_mid_startAdvertising =
            (*env)->GetMethodID(env, jni_cid_leAdvertiser, "startAdvertising",
                    "(Landroid/bluetooth/le/AdvertiseSettings;Landroid/bluetooth/le/AdvertiseData;Landroid/bluetooth/le/AdvertiseCallback;)V");

    (*env)->CallVoidMethod(env, jni_obj_getBluetoothLeAdvertiser, jni_mid_startAdvertising,
            jni_obj_build_LeAdvertiseSettings, jni_obj_build_LeAdvertiseData, advertiseCallback);

    LOGI("[BLE Server] jni start Advertising");
}

jobject openGattServer(JNIEnv *env)
{

    jclass jni_cid_context = (*env)->FindClass(env, "android/content/Context");
    jfieldID jni_fid_bluetoothService = (*env)->GetStaticFieldID(env, jni_cid_context,
            "BLUETOOTH_SERVICE", "Ljava/lang/String;");
    jobject jni_obj_bluetoothService = (*env)->GetStaticObjectField(env, jni_cid_context,
            jni_fid_bluetoothService);

    jmethodID jni_mid_getSystemService = (*env)->GetMethodID(env, jni_cid_context,
            "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jobject jni_obj_bluetoothManager = (*env)->CallObjectMethod(env, gContext,
            jni_mid_getSystemService, jni_obj_bluetoothService);

    jclass jni_cid_bluetoothManager = (*env)->FindClass(env, "android/bluetooth/BluetoothManager");
    jmethodID jni_mid_openGattServer =
            (*env)->GetMethodID(env, jni_cid_bluetoothManager, "openGattServer",
                    "(Landroid/content/Context;Landroid/bluetooth/BluetoothGattServerCallback;)Landroid/bluetooth/BluetoothGattServer;");
    jobject jni_obj_bluetoothGattServer = (*env)->CallObjectMethod(env, jni_obj_bluetoothManager,
            jni_mid_openGattServer, gContext, gBluetoothGattServerCallback);

    return jni_obj_bluetoothGattServer;
}

jobject createGattService(JNIEnv *env)
{

    jclass jni_cid_bluetoothGattService = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattService");
    jmethodID jni_mid_bluetoothGattService = (*env)->GetMethodID(env, jni_cid_bluetoothGattService,
            "<init>", "(Ljava/util/UUID;I)V");

    // service uuid object
    jobject jni_obj_serviceUUID = getUuidFromString(env, OIC_GATT_SERVICE_UUID);

    // service type object
    jfieldID jni_fid_serviceType = (*env)->GetStaticFieldID(env, jni_cid_bluetoothGattService,
            "SERVICE_TYPE_PRIMARY", "I");
    jobject jni_obj_serviceType = (*env)->GetStaticObjectField(env, jni_cid_bluetoothGattService,
            jni_fid_serviceType);

    jobject jni_bluetoothGattService = (*env)->NewObject(env, jni_cid_bluetoothGattService,
            jni_mid_bluetoothGattService, jni_obj_serviceUUID, jni_obj_serviceType);
    if (!jni_bluetoothGattService)
    {
        LOGI("[BLE Server] fail to create gatt service instance!");
        return NULL;
    }

    jclass jni_cid_bluetoothGattCharacteristic = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattCharacteristic");
    jmethodID jni_mid_bluetoothGattCharacteristic = (*env)->GetMethodID(env,
            jni_cid_bluetoothGattCharacteristic, "<init>", "(Ljava/util/UUID;II)V");

    // characteristic uuid for response
    jobject jni_obj_readUuid = getUuidFromString(env, OIC_GATT_CHARACTERISTIC_RESPONSE_UUID);

    // characteristic properties
    jfieldID jni_fid_readProperties = (*env)->GetStaticFieldID(env,
            jni_cid_bluetoothGattCharacteristic, "PROPERTY_READ", "I");
    jobject jni_obj_readProperties = (*env)->GetStaticObjectField(env,
            jni_cid_bluetoothGattCharacteristic, jni_fid_readProperties);

    // characteristic permissions
    jfieldID jni_fid_readPermissions = (*env)->GetStaticFieldID(env,
            jni_cid_bluetoothGattCharacteristic, "PERMISSION_READ", "I");
    jobject jni_obj_readPermissions = (*env)->GetStaticObjectField(env,
            jni_cid_bluetoothGattCharacteristic, jni_fid_readPermissions);

    jobject jni_readCharacteristic = (*env)->NewObject(env, jni_cid_bluetoothGattCharacteristic,
            jni_mid_bluetoothGattCharacteristic, jni_obj_readUuid, jni_obj_readProperties,
            jni_obj_readPermissions);
    if (!jni_readCharacteristic)
    {
        LOGI("[BLE Server] fail to create Response Characteristic");
        return NULL;
    }

    jobject jni_obj_writeUuid = getUuidFromString(env, OIC_GATT_CHARACTERISTIC_REQUEST_UUID);

    // characteristic properties
    jfieldID jni_fid_writeProperties = (*env)->GetStaticFieldID(env,
            jni_cid_bluetoothGattCharacteristic, "PROPERTY_WRITE", "I");
    jobject jni_obj_writeProperties = (*env)->GetStaticObjectField(env,
            jni_cid_bluetoothGattCharacteristic, jni_fid_writeProperties);

    // characteristic permissions
    jfieldID jni_fid_writePermissions = (*env)->GetStaticFieldID(env,
            jni_cid_bluetoothGattCharacteristic, "PERMISSION_WRITE", "I");
    jobject jni_obj_writePermissions = (*env)->GetStaticObjectField(env,
            jni_cid_bluetoothGattCharacteristic, jni_fid_writePermissions);

    jobject jni_writeCharacteristic = (*env)->NewObject(env, jni_cid_bluetoothGattCharacteristic,
            jni_mid_bluetoothGattCharacteristic, jni_obj_writeUuid, jni_obj_writeProperties,
            jni_obj_writePermissions);
    if (!jni_writeCharacteristic)
    {
        LOGI("[BLE Server] fail to create Request Characteristic");
        return NULL;
    }

    jmethodID jni_mid_addCharacteristic = (*env)->GetMethodID(env, jni_cid_bluetoothGattService,
            "addCharacteristic", "(Landroid/bluetooth/BluetoothGattCharacteristic;)Z");
    jboolean jni_boolean_addReadCharacteristic = (*env)->CallBooleanMethod(env,
            jni_bluetoothGattService, jni_mid_addCharacteristic, jni_readCharacteristic);
    if (jni_boolean_addReadCharacteristic == JNI_FALSE)
    {
        LOGI("[BLE Server] fail to add jni_boolean_addReadCharacteristic!!");
        return NULL;
    }

    jboolean jni_boolean_addWriteCharacteristic = (*env)->CallBooleanMethod(env,
            jni_bluetoothGattService, jni_mid_addCharacteristic, jni_writeCharacteristic);
    if (jni_boolean_addWriteCharacteristic == JNI_FALSE)
    {
        LOGI("[BLE Server] fail to add jni_boolean_addReadCharacteristic!!");
        return NULL;
    }

    LOGI("[BLE Server] jni gatt characteristic added!!");
    return jni_bluetoothGattService;
}

void startGattServer(JNIEnv *env, jobject gattServerCallback)
{

    gBluetoothGattServerCallback = (*env)->NewGlobalRef(env, gattServerCallback);

    // open gatt server
    jobject bluetoothGattServer = openGattServer(env);

    gBluetoothGattServer = (*env)->NewGlobalRef(env, bluetoothGattServer);
    LOGI("[BLE Server] Bluetooth Gatt Server started!!");

    // get bluetooth gatt service
    jobject bluetoothGattService = createGattService(env);

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattServer");
    jmethodID jni_mid_addService = (*env)->GetMethodID(env, jni_cid_bluetoothGattServer,
            "addService", "(Landroid/bluetooth/BluetoothGattService;)Z");
    jboolean jni_boolean_addService = (*env)->CallBooleanMethod(env, bluetoothGattServer,
            jni_mid_addService, bluetoothGattService);

    LOGI("[BLE Server] jni gatt service added!!");
}

void connect(JNIEnv *env, jobject bluetoothDevice)
{

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattServer");
    jmethodID jni_mid_connect = (*env)->GetMethodID(env, jni_cid_bluetoothGattServer, "connect",
            "(Landroid/bluetooth/BluetoothDevice;Z)Z");
    jboolean jni_boolean_connect = (*env)->CallBooleanMethod(env, gBluetoothGattServer,
            jni_mid_connect, bluetoothDevice, JNI_FALSE);
    LOGI("[BLE Server] connection requested!!");
}

void disconnect(JNIEnv *env, jobject bluetoothDevice)
{

    jclass jni_cid_bluetoothGattServer = (*env)->FindClass(env,
            "android/bluetooth/BluetoothGattServer");
    jmethodID jni_mid_cancelConnection = (*env)->GetMethodID(env, jni_cid_bluetoothGattServer,
            "cancelConnection", "(Landroid/bluetooth/BluetoothDevice;)V");
    (*env)->CallVoidMethod(env, gBluetoothGattServer, jni_mid_cancelConnection, bluetoothDevice);
    LOGI("[BLE Server] disconnection requested!!");
}

