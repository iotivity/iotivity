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

static u_arraylist_t *gdeviceStateList = NULL;
static u_arraylist_t *gdeviceObjectList = NULL;

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
    memset(newstate->address, 0, CA_MACADDR_SIZE);
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

    if(CAEDRNativeIsDeviceInList(state->address)) {
        CAEDRNativeRemoveDevice(state->address); // delete previous state for update new state
    }
    u_arraylist_add(gdeviceStateList, state);          // update new state
    OIC_LOG_V(DEBUG, TAG, "Set State Info to List : %d", state->state);
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
    OIC_LOG_V(DEBUG, TAG, "CAEDRNativeRemoveDeviceforStateList");

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
            OIC_LOG_V(DEBUG, TAG, "[EDR][Native] remove state : %s", remoteAddress);
            OICFree(state);

            CAEDRReorderingDeviceList(index);
            break;
        }
    }
    return;
}

jboolean CAEDRIsConnectedDevice(const char* remoteAddress)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRIsConnectedDevice");

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

    const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

    if(!CAEDRNativeIsDeviceSocketInList(env, remoteAddress))
    {
        jobject gDeviceSocker = (*env)->NewGlobalRef(env, deviceSocket);
        u_arraylist_add(gdeviceObjectList, gDeviceSocker);
        OIC_LOG_V(DEBUG, TAG, "Set Socket Object to Array");
    }
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

void CAEDRNativeSocketCloseToAll(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "[EDR][Native] CAEDRNativeSocketCloseToAll");

    if(!gdeviceObjectList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gdeviceObjectList is null");
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

    jint index;
    for (index = 0; index < u_arraylist_length(gdeviceObjectList); index++)
    {
        jobject jni_obj_socket = (jobject) u_arraylist_get(gdeviceObjectList, index);
        if(!jni_obj_socket)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] socket obj is null");
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
    }
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

    OIC_LOG(DEBUG, TAG, "[BLE][Native] there are no target object");
    return;
}

void CAEDRNativeRemoveDeviceSocketBaseAddr(JNIEnv *env, jstring address)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRNativeRemoveDeviceSocket");

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
            continue;
        }

        jstring jni_setAddress = CAEDRNativeGetAddressFromDeviceSocket(env, jarrayObj);
        if(!jni_setAddress)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_setAddress is null");
            continue;
        }
        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        const char* remoteAddress = (*env)->GetStringUTFChars(env, address, NULL);

        if(!strcmp(setAddress, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "[BLE][Native] remove object : %s", remoteAddress);
            (*env)->DeleteGlobalRef(env, jarrayObj);

            CAEDRReorderingDeviceSocketList(index);
            break;
        }
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] there are no target object");
    return;
}

jobject CAEDRNativeGetDeviceSocket(uint32_t idx)
{
    OIC_LOG_V(DEBUG, TAG, "CAEDRNativeGetDeviceSocket");

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
