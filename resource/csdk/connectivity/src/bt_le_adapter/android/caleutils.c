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
//#define METHODID_INTNONPARAM   "()I"
#define METHODID_STRINGNONPARAM   "()Ljava/lang/String;"
//#define METHODID_OBJECT_STRINGUUIDPARAM   "(Ljava/lang/String;Ljava/util/UUID;)Ljava/lang/Object;"
//#define METHODID_ONRESPONSE_PARAM  "(Ljava/lang/String;)V"
#define CLASSPATH_BT_ADPATER "android/bluetooth/BluetoothAdapter"
//#define CLASSPATH_BT_UUID "java/util/UUID"

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

jstring CALEGetAddressFromBTDevice(JNIEnv *env, jobject bluetoothDevice)
{
    OIC_LOG(DEBUG, TAG, "CALEGetAddressFromBTDevice");

    jclass jni_cid_device_list = (*env)->FindClass(env,
            "android/bluetooth/BluetoothDevice");
    jmethodID jni_mid_getAddress = (*env)->GetMethodID(env, jni_cid_device_list,
            "getAddress", "()Ljava/lang/String;");
    jstring jni_address = (jstring)(*env)->CallObjectMethod(env,
            bluetoothDevice, jni_mid_getAddress);
    if (!jni_address)
    {
        OIC_LOG(DEBUG, TAG, "jni_address is null");
        return 0;
    }
    return jni_address;
}

jstring CALEGetLocalDeviceAddress(JNIEnv* env)
{
    jclass jni_cid_BTAdapter = (*env)->FindClass(env, CLASSPATH_BT_ADPATER);
    if (!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "getAddress: jni_cid_BTAdapter is null");
        return NULL;
    }

    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env,
            jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);
    if (!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(DEBUG, TAG, "getAddress: jni_mid_getDefaultAdapter is null");
        return NULL;
    }

    jmethodID jni_mid_getAddress = (*env)->GetMethodID(env, jni_cid_BTAdapter,
            "getAddress", METHODID_STRINGNONPARAM);
    if (!jni_mid_getAddress)
    {
        OIC_LOG(DEBUG, TAG, "getAddress: jni_mid_getAddress is null");
        return NULL;
    }

    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env,
            jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if (!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "getAddress: jni_obj_BTAdapter is null");
        return NULL;
    }

    jstring jni_str_address = (jstring)(*env)->CallObjectMethod(env,
            jni_obj_BTAdapter, jni_mid_getAddress);
    if (!jni_str_address)
    {
        OIC_LOG(DEBUG, TAG, "getAddress: jni_str_address is null");
        return NULL;
    }

    return jni_str_address;
}
