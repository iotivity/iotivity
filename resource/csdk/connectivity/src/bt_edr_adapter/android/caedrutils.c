#include <jni.h>
#include <stdio.h>
#include <android/log.h>
#include "caedrutils.h"
#include "logger.h"
#include "oic_malloc.h"
#include "uthreadpool.h"
#include "uarraylist.h"

#define TAG PCF("CA_EDR_UTILS")

static const char *METHODID_OBJECTNONPARAM = "()Landroid/bluetooth/BluetoothAdapter;";
static const char *METHODID_STRINGNONPARAM = "()Ljava/lang/String;";
static const char *CLASSPATH_BT_ADPATER = "android/bluetooth/BluetoothAdapter";

/**
 * BT common
 */

// get address from bluetooth socket
jstring CAEDRNativeGetAddressFromDeviceSocket(JNIEnv *env, jobject bluetoothSocketObj)
{
    if(!bluetoothSocketObj)
    {
        return NULL;
    }

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
