#include <stdio.h>
#include <string.h>
#include <jni.h>
#include "calecore.h"
#include "logger.h"
#include "oic_malloc.h"
#include "uthreadpool.h" /* for thread pool */
#include "uarraylist.h"
#include "com_iotivity_jar_CALeInterface.h"

#define TAG PCF("CA")

#define METHODID_OBJECTNONPARAM   "()Landroid/bluetooth/BluetoothAdapter;"
#define METHODID_INTNONPARAM   "()I"
#define METHODID_STRINGNONPARAM   "()Ljava/lang/String;"
#define METHODID_OBJECT_STRINGUUIDPARAM   "(Ljava/lang/String;Ljava/util/UUID;)Ljava/lang/Object;"
#define METHODID_ONRESPONSE_PARAM  "(Ljava/lang/String;)V"
#define CLASSPATH_BT_ADPATER "android/bluetooth/BluetoothAdapter"
#define CLASSPATH_BT_UUID "java/util/UUID"

static const uint32_t STATE_CONNECTED = 2;
static const uint32_t STATE_DISCONNECTED = 0;
static const uint32_t GATT_SUCCESS = 0;

static JavaVM *g_jvm;
static u_arraylist_t *gdeviceList = NULL;
static u_arraylist_t *gGattObjectList = NULL;
static CAPacketReceiveCallback gPacketReceiveCallback = NULL;
static u_thread_pool_t gThreadPoolHandle = NULL;
static jobject gLeScanCallback;
static jobject gLeGattCallback;
static jobject gContext;
static jboolean gIsStartServer;

////////////////////////////////////////////////////////////////////////////////////////////////////
void CALeCreateJniInterfaceObject()
{
    OIC_LOG_V(DEBUG, TAG, "CALeCreateJniInterfaceObject");

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

    jclass LeJniInterface = (*env)->FindClass(env, "com/iotivity/jar/CALeInterface");
    if (!LeJniInterface)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get CALeInterface class");
        return;
    }

    jmethodID LeInterfaceConstructorMethod =
            (*env)->GetMethodID(env, LeJniInterface, "<init>", "()V");
    if (!LeInterfaceConstructorMethod)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get CALeInterface constructor method");
        return;
    }

    jobject jni_instance = (*env)->NewObject(env, LeJniInterface, LeInterfaceConstructorMethod);
    gContext = (*env)->NewGlobalRef(env, jni_instance);
    OIC_LOG_V(DEBUG, TAG, "Create CALeInterface instance");

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);
}

JNIEXPORT jint JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    OIC_LOG_V(DEBUG, TAG, "JNI_OnLoad in calecore");

    JNIEnv* env;
    if((*jvm)->GetEnv(jvm, (void**)&env, JNI_VERSION_1_6) != JNI_OK)
    {
        return -1;
    }
    g_jvm = jvm;  /* cache the JavaVM pointer */

    //JVM required for WifiCore to work with JNI interface
    CAWiFiJniInit(jvm);

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *jvm, void *reserved)
{
    OIC_LOG_V(DEBUG, TAG, "JNI_OnUnload in calecore");

    JNIEnv* env;
    if((*jvm)->GetEnv(jvm, (void**)&env, JNI_VERSION_1_6) != JNI_OK)
    {
        return;
    }
    g_jvm = 0;
    return;
}

JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CARegisterLeScanCallback(JNIEnv *env,
        jobject obj, jobject callback)
{
    OIC_LOG_V(DEBUG, TAG, "CARegisterLeScanCallback");

    gLeScanCallback = (*env)->NewGlobalRef(env, callback);
}

JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CARegisterLeGattCallback(JNIEnv *env,
        jobject obj, jobject callback)
{
    OIC_LOG_V(DEBUG, TAG, "CARegisterLeGattCallback");

    gLeGattCallback = (*env)->NewGlobalRef(env, callback);
}

JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CALeScanCallback (JNIEnv *env,
        jobject obj, jobject device, jint rssi, jbyteArray scanRecord)
{
    CANativeAddScanDeviceToList(env, device);
}

/*
 * Class:     com_iotivity_jar_CALeInterface
 * Method:    CALeGattConnectionStateChangeCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;II)V
 */
JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CALeGattConnectionStateChangeCallback
  (JNIEnv *env, jobject obj, jobject gatt, jint status, jint newstate)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattConnectionStateChangeCallback - status %d, newstate %d", status, newstate);

    if(GATT_SUCCESS == status && STATE_CONNECTED == newstate)
    {
        if(gatt) {
            CANativeAddGattobjToList(env, gatt);
            CANativeLEDiscoverServices(env, gatt);
        }
    }
    else if (GATT_SUCCESS == status && STATE_DISCONNECTED == newstate)
    {
        if(gatt) {
            CANativeRemoveGattObj(env, gatt);
        }
    }
}

/*
 * Class:     com_iotivity_jar_CALeInterface
 * Method:    CALeGattServicesDiscoveredCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;I)V
 */
JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CALeGattServicesDiscoveredCallback
  (JNIEnv *env, jobject obj, jobject gatt, jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattServicesDiscoveredCallback - status %d: ", status);

    if(0 == status)
    {
        jboolean ret = CANativeSetCharacteristicNoti(env, gatt);
        if(1 == ret)
        {
            jstring data = (*env)->NewStringUTF(env, "HelloWorld");
            jobject jni_obj_character = CANativeCreateGattCharacteristic(env, gatt, data);
            if(jni_obj_character)
            {
                CANativeLESendData(env, gatt, jni_obj_character);
            }
        }
    }
}

/*
 * Class:     com_iotivity_jar_CALeInterface
 * Method:    CALeGattCharacteristicReadCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;I)V
 */
JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CALeGattCharacteristicReadCallback
  (JNIEnv *env, jobject obj, jobject gatt, jobject characteristic, jstring data, jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicReadCallback - status : %d", status);

    const char* readData = (*env)->GetStringUTFChars(env, data, NULL);
    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicReadCallback - read data : %s", readData);
}

/*
 * Class:     com_iotivity_jar_CALeInterface
 * Method:    CALeGattCharacteristicWritjclasseCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;I)V
 */
JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CALeGattCharacteristicWriteCallback
  (JNIEnv *env, jobject obj, jobject gatt, jobject characteristic, jstring data, jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicWriteCallback - status : %d", status);

    const char* writeData = (*env)->GetStringUTFChars(env, data, NULL);
    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicWriteCallback - write data : %s", writeData);

//    jobjectArray jni_obj_data_array = CANativeGetValueFromCharacteristic(env, characteristic);
//    if(!jni_obj_data_array)
//    {
//        OIC_LOG_V(DEBUG, TAG, "jni_obj_data_array is null");
//        return;
//    }
}

/*
 * Class:     com_iotivity_jar_CALeInterface
 * Method:    CALeGattCharacteristicChangedCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;)V
 */
JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CALeGattCharacteristicChangedCallback
  (JNIEnv *env, jobject obj, jobject gatt, jobject characteristic, jstring data)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicChangedCallback");

    const char* changedData = (*env)->GetStringUTFChars(env, data, NULL);
    OIC_LOG_V(DEBUG, TAG, "CALeGattCharacteristicChangedCallback - data : %s", changedData);

    CANativeLEDisconnect(env, gatt);
}

/*
 * Class:     com_iotivity_jar_CALeInterface
 * Method:    CALeGattDescriptorReadCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattDescriptor;I)V
 */
JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CALeGattDescriptorReadCallback
  (JNIEnv *env, jobject obj, jobject gatt, jobject descriptor, jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattDescriptorReadCallback - status %d: ", status);
}

/*
 * Class:     com_iotivity_jar_CALeInterface
 * Method:    CALeGattDescriptorWriteCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattDescriptor;I)V
 */
JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CALeGattDescriptorWriteCallback
  (JNIEnv *env, jobject obj, jobject gatt, jobject descriptor, jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattDescriptorWriteCallback - status %d: ", status);
}

/*
 * Class:     com_iotivity_jar_CALeInterface
 * Method:    CALeGattReliableWriteCompletedCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;I)V
 */
JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CALeGattReliableWriteCompletedCallback
  (JNIEnv *env, jobject obj, jobject gatt, jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattReliableWriteCompletedCallback - status %d: ", status);
}

/*
 * Class:     com_iotivity_jar_CALeInterface
 * Method:    CALeGattReadRemoteRssiCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;II)V
 */
JNIEXPORT void JNICALL Java_com_iotivity_jar_CALeInterface_CALeGattReadRemoteRssiCallback
  (JNIEnv *env, jobject obj, jobject gatt, jint rssi, jint status)
{
    OIC_LOG_V(DEBUG, TAG, "CALeGattReadRemoteRssiCallback - rssi %d,  status %d: ", rssi, status);
}


void CALEInitialize(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "CALEInitialize");

    gThreadPoolHandle = handle;

    CALeCreateJniInterfaceObject(); /* create java CALeInterface instance*/
}

void CALETerminate()
{
    OIC_LOG(DEBUG, TAG, "CALETerminate");

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

    CANativeLEDisconnectAll(env);

    if(gLeScanCallback)
    {
        CANativeLEStopScanImpl(env, gLeScanCallback);
    }

    if(gLeScanCallback)
    {
        (*env)->DeleteGlobalRef(env, gLeScanCallback);
    }

    if(gLeGattCallback)
    {
        (*env)->DeleteGlobalRef(env, gLeGattCallback);
    }

    if(gContext)
    {
        (*env)->DeleteGlobalRef(env, gContext);
    }

    CANativeRemoveAllDevices(env);
    CANativeRemoveAllGattObjsList(env);
    gIsStartServer = FALSE;

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

}

int32_t CALESendUnicastMessage(const char* address, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALESendUnicastMessage(%s, %s)", address, data);

    CALESendUnicastMessageImpl(address, data, dataLen);
    return 0;
}

int32_t CALESendMulticastMessage(const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALESendMulticastMessage(%s)", data);

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

    CALESendMulticastMessageImpl(env, data, dataLen);

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    return 0;
}

int32_t CALEStartUnicastServer(const char* address)
{
    OIC_LOG_V(DEBUG, TAG, "CALEStartUnicastServer(%s)", address);

    return 0;
}

int32_t CALEStartMulticastServer()
{
    OIC_LOG_V(DEBUG, TAG, "CALEStartMulticastServer");

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
            return;
        }
        isAttached = TRUE;
    }

    gIsStartServer = TRUE;
    CANativeLEStartScan();

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    return 0;
}

int32_t CALEStopUnicastServer(int32_t serverID)
{
    OIC_LOG(DEBUG, TAG, "CALEStopUnicastServer");

    return 0;
}

int32_t CALEStopMulticastServer(int32_t serverID)
{
    OIC_LOG(DEBUG, TAG, "CALEStopMulticastServer");
    gIsStartServer = FALSE;
    CANativeLEStopScan();
    return 0;
}

void CALESetCallback(CAPacketReceiveCallback callback)
{
    gPacketReceiveCallback = callback;
}

void CALEGetInterfaceInfo(CALocalConnectivity_t **info, uint32_t* size)
{
    return;
}

void CALEGetLocalAddress(char* address)
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

    jstring jni_address = CANativeGetLocalDeviceAddress(env);
    const char* localAddress = (*env)->GetStringUTFChars(env, jni_address, NULL);
    memcpy(address, localAddress, strlen(localAddress));

    OIC_LOG_V(DEBUG, TAG, "Local Address : %s", address);
    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);
}

int32_t CALESendUnicastMessageImpl(const char* address, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CALESendUnicastMessageImpl, address: %s, data: %s", address, data);

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

    // connect to gatt server
    CANativeLEStopScanImpl(env, gLeScanCallback);

    jobject jni_obj_bluetoothDevice = NULL;
    if(gContext && gdeviceList)
    {
        jint index;
        for (index = 0; index < u_arraylist_length(gdeviceList); index++)
        {
            jobject jarrayObj = (jobject) u_arraylist_get(gdeviceList, index);
            if(!jarrayObj)
            {
                OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is null");
                return;
            }

            jstring jni_setAddress = CANativeGetAddressFromBTDevice(env, jarrayObj);
            if(!jni_setAddress)
            {
                OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_setAddress is null");
                return;
            }
            const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);

            if(!strcmp(setAddress, address))
            {
                jni_obj_bluetoothDevice = jarrayObj;
                break;
            }
        }

        if(jni_obj_bluetoothDevice)
        {
            jboolean autoConnect = FALSE;
            CANativeLEConnect(env, jni_obj_bluetoothDevice, gContext, autoConnect, gLeGattCallback);
        }
    }

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

    return 0;
}

int32_t CALESendMulticastMessageImpl(JNIEnv *env, const char* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CASendMulticastMessageImpl, send to, data: %s", data);

    if(!gdeviceList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gdeviceList is null");
        return 0;
    }

    // connect to gatt server
    CANativeLEStopScanImpl(env, gLeScanCallback);

    // reset gatt list
    CANativeRemoveAllGattObjsList(env);
    CANativeCreateGattObjList(env);

    jint index;
    for (index = 0; index < u_arraylist_length(gdeviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gdeviceList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is null");
            return;
        }

        CANativeLEConnect(env, jarrayObj, gContext, FALSE, gLeGattCallback);
        sleep(1);
    }
//    CANativeLEStartScan();

    return 0;
}

/**
 * BT common
 */
jstring CANativeGetLocalDeviceAddress(JNIEnv* env)
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

jobjectArray CANativeBondedDevices(JNIEnv *env)
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

jint CANativeGetBTStateOnInfo(JNIEnv *env)
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
        return;
    }
    jint jni_int_val = (*env)->GetStaticIntField(env, jni_cid_BTAdapter, jni_fid_stateon);

    OIC_LOG_V(DEBUG, TAG, "[BLE][Native] bluetooth STATE_ON state integer value : %d", jni_int_val);

    return jni_int_val;
}

jboolean CANativeIsEnableBTAdapter(JNIEnv *env)
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

jstring CANativeGetAddressFromBTDevice(JNIEnv *env, jobject bluetoothDevice)
{
    jclass jni_cid_device_list = (*env)->FindClass(env, "android/bluetooth/BluetoothDevice");
    if(!jni_cid_device_list)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_device_list is null");
        return 0;
    }

    jmethodID jni_mid_getAddress = (*env)->GetMethodID(env, jni_cid_device_list, "getAddress",
            "()Ljava/lang/String;");
    if(!jni_mid_getAddress)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_getAddress is null");
        return 0;
    }

    jstring jni_address = (jstring)(*env)->CallObjectMethod(env, bluetoothDevice, jni_mid_getAddress);
    if(!jni_address)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_address is null");
        return 0;
    }
    return jni_address;
}

jstring CANativeGetAddressFromGattObj(JNIEnv *env, jobject gatt)
{
    if(!gatt)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gatt is null");
        return 0;
    }

    jclass jni_cid_gattdevice_list = (*env)->FindClass(env, "android/bluetooth/BluetoothGatt");
    if(!jni_cid_gattdevice_list)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_gattdevice_list is null");
        return 0;
    }

    jmethodID jni_mid_getDevice = (*env)->GetMethodID(env, jni_cid_gattdevice_list, "getDevice",
            "()Landroid/bluetooth/BluetoothDevice;");
    if(!jni_mid_getDevice)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_getDevice is null");
        return 0;
    }

    jobject jni_obj_device = (*env)->CallObjectMethod(env, gatt, jni_mid_getDevice);
    if(!jni_obj_device)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_obj_device is null");
        return 0;
    }

    jstring jni_address = CANativeGetAddressFromBTDevice(env, jni_obj_device);
    if(!jni_address)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_address is null");
        return 0;
    }

    return jni_address;
}

/**
 * BLE layer
 */
void CANativeGattClose(JNIEnv *env, jobject bluetoothGatt)
{
    // GATT CLOSE
    OIC_LOG(DEBUG, TAG, "[BLE][Native] GATT CLOSE");

    // get BluetoothGatt class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get BluetoothGatt class");
    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, "android/bluetooth/BluetoothGatt");
    if(!jni_cid_BluetoothGatt)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] get gatt disconnect method");
    jmethodID jni_mid_closeGatt = (*env)->GetMethodID(env, jni_cid_BluetoothGatt, "close","()V");
    if(!jni_mid_closeGatt)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_closeGatt is null");
        return;
    }

    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] request close Gatt");
    (*env)->CallVoidMethod(env, bluetoothGatt, jni_mid_closeGatt);
}

void CANativeLEStartScan()
{
    if(!gIsStartServer)
    {
        OIC_LOG_V(DEBUG, TAG, "server is not started yet..scan will be passed");
        return;
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
            return;
        }
        isAttached = TRUE;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeLEStartScan");
    // create new object array
    jclass jni_cid_uuid_list = (*env)->FindClass(env, CLASSPATH_BT_UUID);
    if(!jni_cid_uuid_list)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_uuid_list is null");
        return;
    }

    jobjectArray jni_obj_uuid_list = (jobjectArray)(*env)->NewObjectArray(env, 1, jni_cid_uuid_list, NULL);
    if(!jni_obj_uuid_list)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_obj_uuid_list is null");
        return;
    }

    // remove previous list and create list again
    CANativeRemoveAllDevices(env);
    CANativeCreateScanDeviceList(env);

    // make uuid list
    jobject jni_obj_uuid = CANativeGetUUIDObject(env, "713d0000-503e-4c75-ba94-3148f18d941e");
    (*env)->SetObjectArrayElement(env, jni_obj_uuid_list, 0, jni_obj_uuid);

    // scan gatt server with UUID
    if(gLeScanCallback && jni_obj_uuid_list)
    {
//        CANativeLEStartScanWithUUIDImpl(env, jni_obj_uuid_list, gLeScanCallback);
        CANativeLEStartScanImpl(env, gLeScanCallback);
    }

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

}

void CANativeLEStartScanImpl(JNIEnv *env, jobject callback)
{
    if(!CANativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    // get default bt adapter class
    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getState From BTAdapter: jni_cid_BTAdapter is null");
        return;
    }

    // get remote bt adapter method
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env, jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);
    if(!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getState From BTAdapter: jni_mid_getDefaultAdapter is null");
        return;
    }

    // get start le scan method
    jmethodID jni_mid_startLeScan = (*env)->GetMethodID(env, jni_cid_BTAdapter, "startLeScan",
            "(Landroid/bluetooth/BluetoothAdapter$LeScanCallback;)Z");
    if(!jni_mid_startLeScan)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] startLeScan: jni_mid_startLeScan is null");
        return;
    }

    // gat bt adapter object
    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getState From BTAdapter: jni_obj_BTAdapter is null");
        return;
    }

    // call start le scan method
    jboolean jni_obj_startLeScan = (*env)->CallBooleanMethod(env, jni_obj_BTAdapter, jni_mid_startLeScan, callback);
    if(!jni_obj_startLeScan)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] bleConnect: jni_obj_startLeScan is null");
        return;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] startLeScan..");
    }
}

jobject CANativeGetUUIDObject(JNIEnv *env, const char* uuid)
{
    // setting UUID
    jclass jni_cid_uuid = (*env)->FindClass(env, CLASSPATH_BT_UUID);
    if(!jni_cid_uuid)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_uuid is null");
        return NULL;
    }

    jmethodID jni_mid_fromString = (*env)->GetStaticMethodID(env, jni_cid_uuid, "fromString", "(Ljava/lang/String;)Ljava/util/UUID;");
    if(!jni_mid_fromString)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_fromString is null");
        return NULL;
    }

    jstring jni_uuid = (*env)->NewStringUTF(env, uuid);
    jobject jni_obj_uuid = (*env)->CallStaticObjectMethod(env, jni_cid_uuid, jni_mid_fromString, jni_uuid);
    if(!jni_obj_uuid)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_obj_uuid is null");
        return NULL;
    }

    return jni_obj_uuid;
}

void CANativeLEStartScanWithUUIDImpl(JNIEnv *env, jobjectArray uuids, jobject callback)
{
    // get default bt adapter class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get default bt adapter class");

    if(!CANativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getState From BTAdapter: jni_cid_BTAdapter is null");
        return;
    }

    // get remote bt adapter method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get remote bt adapter method");
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env, jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);
    if(!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getState From BTAdapter: jni_mid_getDefaultAdapter is null");
        return;
    }

    // get start le scan method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get start le scan method");
    jmethodID jni_mid_startLeScan = (*env)->GetMethodID(env, jni_cid_BTAdapter, "startLeScan",
            "([Ljava/util/UUID;Landroid/bluetooth/BluetoothAdapter$LeScanCallback;)Z");
    if(!jni_mid_startLeScan)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] startLeScan: jni_mid_startLeScan is null");
        return;
    }

    // get bt adapter object
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get bt adapter object");
    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getState From BTAdapter: jni_obj_BTAdapter is null");
        return;
    }

    // call start le scan method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] call start le scan service method");
    jboolean jni_obj_startLeScan = (*env)->CallBooleanMethod(env, jni_obj_BTAdapter, jni_mid_startLeScan, uuids, callback);
    if(!jni_obj_startLeScan)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] bleConnect: jni_obj_startLeScan is null");
        return;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] startLeScan..");
    }
}

void CANativeLEStopScan()
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

    CANativeLEStopScanImpl(env, gLeScanCallback);

    if(isAttached)
        (*g_jvm)->DetachCurrentThread(g_jvm);

}

void CANativeLEStopScanImpl(JNIEnv *env, jobject callback)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeLEStopScan");

    if(!CANativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    // get default bt adapter class
    jclass jni_cid_BTAdapter = (*env)->FindClass(env,  CLASSPATH_BT_ADPATER);
    if(!jni_cid_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getState From BTAdapter: jni_cid_BTAdapter is null");
        return;
    }

    // get remote bt adapter method
    jmethodID jni_mid_getDefaultAdapter = (*env)->GetStaticMethodID(env, jni_cid_BTAdapter, "getDefaultAdapter", METHODID_OBJECTNONPARAM);
    if(!jni_mid_getDefaultAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getState From BTAdapter: jni_mid_getDefaultAdapter is null");
        return;
    }

    // get start le scan method
    jmethodID jni_mid_stopLeScan = (*env)->GetMethodID(env, jni_cid_BTAdapter, "stopLeScan",
            "(Landroid/bluetooth/BluetoothAdapter$LeScanCallback;)V");
    if(!jni_mid_stopLeScan)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] stopLeScan: jni_mid_stopLeScan is null");
        return;
    }

    // gat bt adapter object
    jobject jni_obj_BTAdapter = (*env)->CallStaticObjectMethod(env, jni_cid_BTAdapter, jni_mid_getDefaultAdapter);
    if(!jni_obj_BTAdapter)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] getState From BTAdapter: jni_obj_BTAdapter is null");
        return;
    }

    // call start le scan method
    (*env)->CallVoidMethod(env, jni_obj_BTAdapter, jni_mid_stopLeScan, callback);
}

void CANativeLEConnect(JNIEnv *env, jobject bluetoothDevice, jobject context, jboolean autoconnect, jobject callback)
{
    if(!CANativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    jstring jni_address = CANativeGetAddressFromBTDevice(env, bluetoothDevice);
    const char * addr = (*env)->GetStringUTFChars(env, jni_address, NULL);
    OIC_LOG_V(DEBUG, TAG, "[BLE][Native] request connectGatt to %s", addr);

    // GATT CONNECT

    // get BluetoothDevice class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get BluetoothDevice class");
    jclass jni_cid_BluetoothDevice = (*env)->FindClass(env, "android/bluetooth/BluetoothDevice");
    if(!jni_cid_BluetoothDevice)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] bleConnect: jni_cid_BluetoothDevice is null");
        return;
    }

    // get connectGatt method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get connectGatt method");
    jmethodID jni_mid_connectGatt = (*env)->GetMethodID(env, jni_cid_BluetoothDevice, "connectGatt",
            "(Landroid/content/Context;ZLandroid/bluetooth/BluetoothGattCallback;)Landroid/bluetooth/BluetoothGatt;");
    if(!jni_mid_connectGatt)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] bleConnect: jni_mid_connectGatt is null");
        return;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] Call object method - connectGatt");
    jobject jni_obj_connectGatt = (*env)->CallObjectMethod(env, bluetoothDevice, jni_mid_connectGatt, context, autoconnect, callback);
    if(!jni_obj_connectGatt)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] bleConnect: connectGatt was failed..obj will be removed");
        CANativeRemoveDevice(env, jni_address);
        return;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] bleConnect: connecting..");
    }
}

void CANativeLEDisconnect(JNIEnv *env, jobject bluetoothGatt)
{
    if(!CANativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    // GATT DISCONNECT
    OIC_LOG(DEBUG, TAG, "[BLE][Native] GATT DISCONNECT");

    // get BluetoothGatt class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get BluetoothGatt classjobject bluetoothGatt");
    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, "android/bluetooth/BluetoothGatt");
    if(!jni_cid_BluetoothGatt)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] get gatt disconnect method");
    jmethodID jni_mid_disconnectGatt = (*env)->GetMethodID(env, jni_cid_BluetoothGatt, "disconnect","()V");
    if(!jni_mid_disconnectGatt)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_disconnectGatt is null");
        return;
    }

    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] request disconnectGatt");
    (*env)->CallVoidMethod(env, bluetoothGatt, jni_mid_disconnectGatt);

}

void CANativeLEDiscoverServices(JNIEnv *env, jobject bluetoothGatt)
{
    if(!CANativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    // GATT SERVICE DISCOVERY
    OIC_LOG(DEBUG, TAG, "[BLE][Native] GATT SERVICE DISCOVERY");

    // get BluetoothGatt class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get BluetoothGatt class");
    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, "android/bluetooth/BluetoothGatt");
    if(!jni_cid_BluetoothGatt)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] discovery gatt services method");
    jmethodID jni_mid_discoverServices = (*env)->GetMethodID(env, jni_cid_BluetoothGatt, "discoverServices","()Z");
    if(!jni_mid_discoverServices)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_discoverServices is null");
        return;
    }
    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] request discovery gatt services");
    (*env)->CallBooleanMethod(env, bluetoothGatt, jni_mid_discoverServices);
}

void CANativeLESendData(JNIEnv *env, jobject bluetoothGatt, jobject gattCharacteristic)
{
    if(!CANativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    // WRITE GATT CHARACTERISTIC
    OIC_LOG(DEBUG, TAG, "[BLE][Native] WRITE GATT CHARACTERISTIC");

    // get BluetoothGatt class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get BluetoothGatt class");
    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, "android/bluetooth/BluetoothGatt");
    if(!jni_cid_BluetoothGatt)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] write characteristic method");
    jmethodID jni_mid_writeCharacteristic = (*env)->GetMethodID(env, jni_cid_BluetoothGatt, "writeCharacteristic", "(Landroid/bluetooth/BluetoothGattCharacteristic;)Z");
    if(!jni_mid_writeCharacteristic)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_writeCharacteristic is null");
        return;
    }

    // call disconnect gatt method
    OIC_LOG(DEBUG, TAG, "[BLE][Native] request write gatt characteristic");
    (*env)->CallBooleanMethod(env, bluetoothGatt, jni_mid_writeCharacteristic, gattCharacteristic);

}

jboolean CANativeSetCharacteristicNoti(JNIEnv *env, jobject bluetoothGatt)
{
    if(!CANativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    // get BluetoothGatt class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeSetCharacteristicNoti");
    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, "android/bluetooth/BluetoothGatt");
    if(!jni_cid_BluetoothGatt)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return FALSE;
    }

    jobject jni_obj_GattCharacteristic = CANativeGetGattService(env, bluetoothGatt, "713d0002-503e-4c75-ba94-3148f18d941e");
    if(!jni_obj_GattCharacteristic)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_obj_GattCharacteristic is null");
        return FALSE;
    }

    // set Characteristic Notification
    OIC_LOG(DEBUG, TAG, "[BLE][Native] get gatt disconnect method");
    jmethodID jni_mid_setNotification = (*env)->GetMethodID(env, jni_cid_BluetoothGatt, "setCharacteristicNotification",
            "(Landroid/bluetooth/BluetoothGattCharacteristic;Z)Z");
    if(!jni_mid_setNotification)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_getService is null");
        return FALSE;
    }

    jboolean enable = TRUE;
    jboolean ret = (*env)->CallBooleanMethod(env, bluetoothGatt, jni_mid_setNotification, jni_obj_GattCharacteristic, enable);
    if(1 == ret)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] setCharacteristicNotification is success");
        return TRUE;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] setCharacteristicNotification is failed");
        return FALSE;
    }
}

jobject CANativeGetGattService(JNIEnv *env, jobject bluetoothGatt, jstring characterUUID)
{
    if(!CANativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    // get BluetoothGatt class
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeGetGattService");
    jclass jni_cid_BluetoothGatt = (*env)->FindClass(env, "android/bluetooth/BluetoothGatt");
    if(!jni_cid_BluetoothGatt)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_BluetoothGatt is null");
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] get gatt disconnect method");
    jmethodID jni_mid_getService = (*env)->GetMethodID(env, jni_cid_BluetoothGatt, "getService",
            "(Ljava/util/UUID;)Landroid/bluetooth/BluetoothGattService;");
    if(!jni_mid_getService)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_getService is null");
        return NULL;
    }

    jobject jni_obj_service_uuid = CANativeGetUUIDObject(env, "713d0000-503e-4c75-ba94-3148f18d941e");
    if(!jni_obj_service_uuid)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_obj_service_uuid is null");
        return NULL;
    }

    // get bluetooth gatt service
    OIC_LOG(DEBUG, TAG, "[BLE][Native] request to get service");
    jobject jni_obj_gattService = (*env)->CallObjectMethod(env, bluetoothGatt, jni_mid_getService, jni_obj_service_uuid);

    // get bluetooth gatt service class
    jclass jni_cid_BluetoothGattService = (*env)->FindClass(env, "android/bluetooth/BluetoothGattService");
    if(!jni_cid_BluetoothGattService)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_BluetoothGattService is null");
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] get gatt getCharacteristic method");
    jmethodID jni_mid_getCharacteristic = (*env)->GetMethodID(env, jni_cid_BluetoothGattService, "getCharacteristic",
            "(Ljava/util/UUID;)Landroid/bluetooth/BluetoothGattCharacteristic;");
    if(!jni_mid_getCharacteristic)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_getCharacteristic is null");
        return NULL;
    }

    const char* uuid = (*env)->GetStringUTFChars(env, characterUUID, NULL);
    jobject jni_obj_tx_uuid = CANativeGetUUIDObject(env, uuid);
    if(!jni_obj_tx_uuid)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_obj_tx_uuid is null");
        return NULL;
    }

    // get
    OIC_LOG(DEBUG, TAG, "[BLE][Native] request to get Characteristic");
    jobject jni_obj_GattCharacteristic = (*env)->CallObjectMethod(env, jni_obj_gattService, jni_mid_getCharacteristic, jni_obj_tx_uuid);

    return jni_obj_GattCharacteristic;
}

jobject CANativeCreateGattCharacteristic(JNIEnv *env, jobject bluetoothGatt, jstring data)
{
    if(!CANativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeCreateGattCharacteristic");
    jobject jni_obj_GattCharacteristic = CANativeGetGattService(env, bluetoothGatt, "713d0003-503e-4c75-ba94-3148f18d941e");
    if(!jni_obj_GattCharacteristic)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_obj_GattCharacteristic is null");
        return NULL;
    }

    jclass jni_cid_BTGattCharacteristic = (*env)->FindClass(env, "android/bluetooth/BluetoothGattCharacteristic");
    if(!jni_cid_BTGattCharacteristic)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_BTGattCharacteristic is null");
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] set value in Characteristic");
    jmethodID jni_mid_setValue = (*env)->GetMethodID(env, jni_cid_BTGattCharacteristic, "setValue",
            "(Ljava/lang/String;)Z");
    if(!jni_mid_setValue)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_setValue is null");
        return NULL;
    }

    jboolean ret = (*env)->CallBooleanMethod(env, jni_obj_GattCharacteristic, jni_mid_setValue, data);
    if(1 == ret)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] the locally stored value has been set");
        return jni_obj_GattCharacteristic;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] the locally stored value hasn't been set");
        return NULL;
    }
}

jbyteArray CANativeGetValueFromCharacteristic(JNIEnv *env, jobject characteristic)
{
    if(!CANativeIsEnableBTAdapter(env))
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] BT adpater is not enable");
        return;
    }

    jclass jni_cid_BTGattCharacteristic = (*env)->FindClass(env, "android/bluetooth/BluetoothGattCharacteristic");
    if(!jni_cid_BTGattCharacteristic)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_cid_BTGattCharacteristic is null");
        return NULL;
    }

    OIC_LOG(DEBUG, TAG, "[BLE][Native] get value in Characteristic");
    jmethodID jni_mid_getValue = (*env)->GetMethodID(env, jni_cid_BTGattCharacteristic, "getValue",
            "()[B");
    if(!jni_mid_getValue)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_mid_getValue is null");
        return NULL;
    }

    jbyteArray jni_obj_data_array =  (*env)->CallObjectMethod(env, characteristic, jni_mid_getValue);
    return jni_obj_data_array;
}

void CANativeCreateScanDeviceList(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeCreateScanDeviceList");

    // create new object array
    if (gdeviceList == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "Create device list");

        gdeviceList = u_arraylist_create();
    }
}

void CANativeAddScanDeviceToList(JNIEnv *env, jobject device)
{
    if(!device)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] device is null");
        return;
    }

    if(!gdeviceList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gdevice_list is null");
        return;
    }

    jstring jni_remoteAddress = CANativeGetAddressFromBTDevice(env, device);
    if(!jni_remoteAddress)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_remoteAddress is null");
        return;
    }

    const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

    if(!CANativeIsDeviceInList(env, remoteAddress)) {
        jobject gdevice = (*env)->NewGlobalRef(env, device);
        u_arraylist_add(gdeviceList, gdevice);
        OIC_LOG_V(DEBUG, TAG, "Set Object to Array as Element");
    }
}

jboolean CANativeIsDeviceInList(JNIEnv *env, const char* remoteAddress){
    // get address value from device list

    jint index;
    for (index = 0; index < u_arraylist_length(gdeviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gdeviceList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is null");
            return TRUE;
        }

        jstring jni_setAddress = CANativeGetAddressFromBTDevice(env, jarrayObj);
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

    OIC_LOG_V(DEBUG, TAG, "there are no the device in list. we can add");
    return FALSE;
}

void CANativeRemoveAllDevices(JNIEnv *env)
{
    OIC_LOG_V(DEBUG, TAG, "CANativeRemoveAllDevices");

    if(!gdeviceList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gdeviceList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(gdeviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gdeviceList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is null");
            return;
        }
        (*env)->DeleteGlobalRef(env, jarrayObj);
    }

    OICFree(gdeviceList);
    gdeviceList = NULL;
    return;
}

void CANativeRemoveDevice(JNIEnv *env, jstring address)
{
    OIC_LOG_V(DEBUG, TAG, "CANativeRemoveDevice");

    if(!gdeviceList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gdeviceList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(gdeviceList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gdeviceList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is null");
            return;
        }

        jstring jni_setAddress = CANativeGetAddressFromBTDevice(env, jarrayObj);
        if(!jni_setAddress)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_setAddress is null");
            return;
        }
        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);
        const char* remoteAddress = (*env)->GetStringUTFChars(env, address, NULL);

        if(!strcmp(setAddress, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "[BLE][Native] remove object : %s", remoteAddress);
            (*env)->DeleteGlobalRef(env, jarrayObj);

            CAReorderingDeviceList(index);
            return;
        }
    }
    OIC_LOG(DEBUG, TAG, "[BLE][Native] there are no target object");
    return;
}

void CAReorderingDeviceList(uint32_t index)
{
    if (index >= gdeviceList->length)
    {
        return;
    }

    if (index < gdeviceList->length - 1)
    {
        memmove(&gdeviceList->data[index], &gdeviceList->data[index + 1],
                (gdeviceList->length - index - 1) * sizeof(void *));
    }

    gdeviceList->size--;
    gdeviceList->length--;
}

/**
 * Gatt Object List
 */
void CANativeCreateGattObjList(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeCreateGattObjList");

    // create new object array
    if (gGattObjectList == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "Create Gatt object list");

        gGattObjectList = u_arraylist_create();
    }
}

void CANativeAddGattobjToList(JNIEnv *env, jobject gatt)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeAddGattobjToList");

    if(!gatt)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gatt is null");
        return;
    }

    if(!gGattObjectList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gGattObjectList is null");
        return;
    }

    jstring jni_remoteAddress = CANativeGetAddressFromGattObj(env, gatt);
    if(!jni_remoteAddress)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_remoteAddress is null");
        return;
    }

    const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

    if(!CANativeIsGattObjInList(env, remoteAddress))
    {
        jobject gGatt = (*env)->NewGlobalRef(env, gatt);
        u_arraylist_add(gGattObjectList, gGatt);
        OIC_LOG_V(DEBUG, TAG, "Set Object to Array as Element");
    }
}

jboolean CANativeIsGattObjInList(JNIEnv *env, const char* remoteAddress)
{
    OIC_LOG(DEBUG, TAG, "[BLE][Native] CANativeIsGattObjInList");

    jint index;
    for (index = 0; index < u_arraylist_length(gGattObjectList); index++)
    {

        jobject jarrayObj = (jobject) u_arraylist_get(gGattObjectList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is null");
            return TRUE;
        }

        jstring jni_setAddress = CANativeGetAddressFromGattObj(env, jarrayObj);
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

    OIC_LOG_V(DEBUG, TAG, "there are no the gatt obejct in list. we can add");
    return FALSE;
}

void CANativeRemoveAllGattObjsList(JNIEnv *env)
{
    OIC_LOG_V(DEBUG, TAG, "CANativeRemoveAllGattObjsList");

    if(!gGattObjectList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gGattObjectList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(gGattObjectList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gGattObjectList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is null");
            return;
        }
        (*env)->DeleteGlobalRef(env, jarrayObj);
    }

    OICFree(gGattObjectList);
    gGattObjectList = NULL;
    return;
}

void CANativeLEDisconnectAll(JNIEnv *env)
{
    OIC_LOG_V(DEBUG, TAG, "CANativeLEDisconnectAll");

    if(!gGattObjectList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gGattObjectList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(gGattObjectList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gGattObjectList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is null");
            return;
        }
        CANativeLEDisconnect(env, jarrayObj);
    }

    OICFree(gGattObjectList);
    gGattObjectList = NULL;
    return;
}

void CANativeRemoveGattObj(JNIEnv *env, jobject gatt)
{
    OIC_LOG_V(DEBUG, TAG, "CANativeRemoveGattObj");

    if(!gGattObjectList)
    {
        OIC_LOG(DEBUG, TAG, "[BLE][Native] gGattObjectList is null");
        return;
    }

    jint index;
    for (index = 0; index < u_arraylist_length(gGattObjectList); index++)
    {
        jobject jarrayObj = (jobject) u_arraylist_get(gGattObjectList, index);
        if(!jarrayObj)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jarrayObj is null");
            return;
        }

        jstring jni_setAddress = CANativeGetAddressFromGattObj(env, jarrayObj);
        if(!jni_setAddress)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_setAddress is null");
            return;
        }
        const char* setAddress = (*env)->GetStringUTFChars(env, jni_setAddress, NULL);

        jstring jni_remoteAddress = CANativeGetAddressFromGattObj(env, gatt);
        if(!jni_remoteAddress)
        {
            OIC_LOG(DEBUG, TAG, "[BLE][Native] jni_remoteAddress is null");
            return;
        }
        const char* remoteAddress = (*env)->GetStringUTFChars(env, jni_remoteAddress, NULL);

        if(!strcmp(setAddress, remoteAddress))
        {
            OIC_LOG_V(DEBUG, TAG, "[BLE][Native] remove object : %s", remoteAddress);
            (*env)->DeleteGlobalRef(env, jarrayObj);

            CAReorderingGattList(index);
            return;
        }
    }
    OIC_LOG(DEBUG, TAG, "[BLE][Native] there are no target object");
    return;
}

void CAReorderingGattList(uint32_t index)
{
    if (index >= gGattObjectList->length)
    {
        return;
    }

    if (index < gGattObjectList->length - 1)
    {
        memmove(&gGattObjectList->data[index], &gGattObjectList->data[index + 1],
                (gGattObjectList->length - index - 1) * sizeof(void *));
    }

    gGattObjectList->size--;
    gGattObjectList->length--;
}
