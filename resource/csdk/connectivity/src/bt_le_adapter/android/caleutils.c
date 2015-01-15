#include <jni.h>
#include <stdio.h>
#include <android/log.h>
#include "caleutils.h"
#include "logger.h"
#include "oic_malloc.h"
#include "uthreadpool.h"
#include "uarraylist.h"

#define TAG PCF("CA_LE_UTILS")

#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define METHODID_OBJECTNONPARAM   "()Landroid/bluetooth/BluetoothAdapter;"
#define METHODID_STRINGNONPARAM   "()Ljava/lang/String;"
#define CLASSPATH_BT_ADPATER "android/bluetooth/BluetoothAdapter"

jobject CALEGetUuidFromString(JNIEnv *env, const char* uuid)
{

    OIC_LOG_V(DEBUG, TAG, "CALEGetUuidFromString");

    jclass jni_cid_UUID = (*env)->FindClass(env, "java/util/UUID");

    jmethodID jni_mid_fromString = (*env)->GetStaticMethodID(env, jni_cid_UUID,
            "fromString", "(Ljava/lang/String;)Ljava/util/UUID;");

    jstring str_uuid = (*env)->NewStringUTF(env, uuid);

    jobject jni_obj_uuid = (*env)->CallStaticObjectMethod(env, jni_cid_UUID,
            jni_mid_fromString, str_uuid);
    if (!jni_obj_uuid)
    {
        OIC_LOG_V(DEBUG, TAG, "Fail to get jni uuid object");
        return NULL;
    }

    return jni_obj_uuid;
}

jobject CALEGetParcelUuid(JNIEnv *env, jobject uuid)
{
    OIC_LOG_V(DEBUG, TAG, "CALEGetParcelUuid");

    jclass jni_cid_ParcelUuid = (*env)->FindClass(env, "android/os/ParcelUuid");

    jmethodID jni_mid_ParcelUuid = (*env)->GetMethodID(env, jni_cid_ParcelUuid,
            "<init>", "(Ljava/util/UUID;)V");

    jobject jni_ParcelUuid = (*env)->NewObject(env, jni_cid_ParcelUuid,
            jni_mid_ParcelUuid, uuid);
    if (!jni_ParcelUuid)
    {
        OIC_LOG_V(DEBUG, TAG, "Fail to get jni ParcelUuid");
        return NULL;
    }

    return jni_ParcelUuid;
}

jstring CALEGetLocalDeviceAddress(JNIEnv* env)
{
    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getAddress: jni_cid_BTAdapter is null");
        return NULL;
    }

    jmethodID jni_mid_getDefaultAdapter =
            (*env)->GetStaticMethodID(env, jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);
    if(!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getAddress: jni_mid_getDefaultAdapter is null");
        return NULL;
    }

    jmethodID jni_mid_getAddress = (*env)->GetMethodID(env, jni_cid_BTAdapter, "getAddress", METHODID_STRINGNONPARAM);
    if(!jni_mid_getAddress)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getAddress: jni_mid_getAddress is null");
        return NULL;
    }

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getAddress: jni_obj_BTAdapter is null");
        return NULL;
    }

    jstring jni_str_address = (jstring)(*env)->CallObjectMethod(env, jni_obj_BTAdapter, jni_mid_getAddress);
    if(!jni_str_address)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getAddress: jni_str_address is null");
        return NULL;
    }

    return jni_str_address;
}

jobjectArray CALEBondedDevices(JNIEnv *env)
{
    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getBondedDevices: jni_cid_BTAdapter is null");
        return NULL;
    }

    jmethodID jni_mid_getDefaultAdapter =
            (*env)->GetStaticMethodID(env, jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getBondedDevices: bluetooth adapter is null");
        return NULL;
    }

    // Get a list of currently paired devices
    jmethodID jni_mid_getBondedDevices = (*env)->GetMethodID(env, jni_cid_BTAdapter,
            "getBondedDevices", "()Ljava/util/Set;");
    if(!jni_mid_getBondedDevices)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getBondedDevices: jni_mid_getBondedDevicesr is null");
        return NULL;
    }

    jobject jni_obj_setPairedDevices = (*env)->CallObjectMethod(env, jni_obj_BTAdapter, jni_mid_getBondedDevices);
    if(!jni_obj_setPairedDevices)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getBondedDevices: jni_obj_setPairedDevices is null");
        return NULL;
    }

    // Convert the set to an object array
    // object[] array = Set<BluetoothDevice>.toArray();
    jclass jni_cid_Set = (*env)->FindClass(env,  "java/util/Set");
    jmethodID jni_mid_toArray = (*env)->GetMethodID(env, jni_cid_Set, "toArray", "()[Ljava/lang/Object;");

    if(!jni_mid_toArray)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getBondedDevices: jni_mid_toArray is null");
        return NULL;
    }

    jobjectArray jni_arrayPairedDevices = (jobjectArray)((*env)->CallObjectMethod(env,
            jni_obj_setPairedDevices, jni_mid_toArray));
    if(!jni_arrayPairedDevices)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getBondedDevices: jni_arrayPairedDevices is null");
        return NULL;
    }

    return jni_arrayPairedDevices;
}

jint CALEGetBTStateOnInfo(JNIEnv *env)
{
    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getBTStateOnInfo: jni_cid_BTAdapter is null");
        return -1;
    }

    jfieldID jni_fid_stateon = (*env)->GetStaticFieldID(env, jni_cid_BTAdapter, "STATE_ON", "I");
    if (jni_fid_stateon == 0)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] get_field_state is 0");
        return -1;
    }
    jint jni_int_val = (*env)->GetStaticIntField(env, jni_cid_BTAdapter, jni_fid_stateon);

    OIC_LOG_V(DEBUG, TAG, "[BLE][Native] bluetooth STATE_ON state integer value : %d", jni_int_val);

    return jni_int_val;
}

jboolean CALEIsEnableBTAdapter(JNIEnv *env)
{
    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_BTAdapter: jni_cid_BTAdapter is null");
        return FALSE;
    }

    jmethodID jni_mid_getDefaultAdapter =
            (*env)->GetStaticMethodID(env, jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);
    if(!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_getDefaultAdapter is null");
        return FALSE;
    }

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_obj_BTAdapter is null");
        return FALSE;
    }

    // isEnable()
    jmethodID jni_mid_isEnable = (*env)->GetMethodID(env, jni_cid_BTAdapter, "isEnabled",
            "()Z");
    if(!jni_mid_isEnable)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_isEnable is null");
        return FALSE;
    }

    jboolean jni_isEnable = (*env)->CallBooleanMethod(env, jni_obj_BTAdapter, jni_mid_isEnable);
    OIC_LOG_V(DEBUG, TAG, "[BLE][Native] adapter state is %d", jni_isEnable);

    return jni_isEnable;
}

jstring CALEGetAddressFromBTDevice(JNIEnv *env, jobject bluetoothDevice)
{
    jclass jni_cid_device_list = (*env)->FindClass(env, "android/bluetooth/BluetoothDevice");
    if(!jni_cid_device_list)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_device_list is null");
        return NULL;
    }

    jmethodID jni_mid_getAddress = (*env)->GetMethodID(env, jni_cid_device_list, "getAddress",
            "()Ljava/lang/String;");
    if(!jni_mid_getAddress)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_getAddress is null");
        return NULL;
    }

    jstring jni_address = (jstring)(*env)->CallObjectMethod(env, bluetoothDevice, jni_mid_getAddress);
    if(!jni_address)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_address is null");
        return NULL;
    }
    return jni_address;
}
