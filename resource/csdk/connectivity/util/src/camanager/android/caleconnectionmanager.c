/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <jni.h>
#include "logger.h"
#include "cautilinterface.h"
#include "camanagerleinterface.h"
#include "camanagerleutil.h"
#include "caleautoconnector.h"
#include "cacommon.h"
#include "cacommonutil.h"
#include "camanagerdevice.h"
#include "caleclient.h"
#include "caleutils.h"

#define TAG "OIC_CA_MANAGER_LE"

static CAAdapterStateChangedCB g_adapterStateCB = NULL;
static CAConnectionStateChangedCB g_connStateCB = NULL;

static const jint SUPPORT_ADNROID_API_LEVEL = 18;
static const jint AUTH_FAIL = 5;
static const jint LINK_LOSS = 8;
static const jint ACCEPT_TIMEOUT_EXCEPTION = 16;
static const jint REMOTE_DISCONNECT = 19;
static const jint LOCAL_DISCONNECT = 22;
static const jint CONNECTION_FAILED_TO_BE_EASTABLISHED = 62;
static const jint USER_REMOVED_BOND = 68;
static JavaVM *g_jvm = NULL;
static jobject g_context = NULL;
static jobject g_connectedDeviceSet = NULL;

void CASetLENetworkMonitorCallbacks(CAAdapterStateChangedCB adapterStateCB,
                                    CAConnectionStateChangedCB connStateCB)
{
    OIC_LOG(DEBUG, TAG, "CASetLENetworkMonitorCallbacks");

    g_adapterStateCB = adapterStateCB;
    g_connStateCB = connStateCB;
}

CAResult_t CASetLEClientAutoConnectionDeviceInfo(const char* address)
{
    OIC_LOG(DEBUG, TAG, "CASetClientAutoConnectionDeviceInfo");
    VERIFY_NON_NULL(address, TAG, "address");

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(DEBUG, TAG, "AttachCurrentThread will be called for JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    OIC_LOG_V(DEBUG, TAG, "set [%s] for Auto Connection", address);
    jstring jni_leAddress = (*env)->NewStringUTF(env, address);

    if (!CAManagerCheckBTAddress(env, jni_leAddress))
    {
        OIC_LOG(ERROR, TAG, "this address is not BT address string format");
        if (isAttached)
        {
            (*g_jvm)->DetachCurrentThread(g_jvm);
        }
        return CA_STATUS_FAILED;
    }

    // if there is target address in SharedPreference. it will be reseted.
    if (CAManagerIsConnectedDeviceAddress(env, g_context,
                                          jni_leAddress,
                                          g_connectedDeviceSet))
    {
        if (!CAManagerRemoveConnectedDeviceAddress(env, g_context,
                                                   jni_leAddress,
                                                   g_connectedDeviceSet))
        {
            OIC_LOG(ERROR, TAG, "Preference - remove has failed");
        }
        else
        {
            OIC_LOG(INFO, TAG, "Preference - remove success");
        }
    }

    // it will be added new target address.
    if (!CAManagerAddConnectedDeviceAddress(env, g_context,
                                            jni_leAddress, g_connectedDeviceSet))
    {
        OIC_LOG(ERROR, TAG, "Preference - putting has failed");
    }
    else
    {
        OIC_LOG(INFO, TAG, "Preference - putting success");
    }

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;
}

CAResult_t CAUnsetLEClientAutoConnectionDeviceInfo(const char* address)
{
    OIC_LOG(DEBUG, TAG, "CAUnsetClientAutoConnectionDeviceInfo");
    VERIFY_NON_NULL(address, TAG, "address");

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(DEBUG, TAG, "AttachCurrentThread will be called for JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    OIC_LOG_V(DEBUG, TAG, "unset [%s] for Auto Connection", address);
    jstring jni_leAddress = (*env)->NewStringUTF(env, address);

    if (!CAManagerCheckBTAddress(env, jni_leAddress))
    {
        OIC_LOG(ERROR, TAG, "this address is not BT address string format");
        if (isAttached)
        {
            (*g_jvm)->DetachCurrentThread(g_jvm);
        }
        return CA_STATUS_FAILED;
    }

    // if there is target address in SharedPreference. it would be removed
    if (CAManagerIsConnectedDeviceAddress(env, g_context,
                                          jni_leAddress,
                                          g_connectedDeviceSet))
    {
        if (!CAManagerRemoveConnectedDeviceAddress(env, g_context,
                                                   jni_leAddress,
                                                   g_connectedDeviceSet))
        {
            OIC_LOG(ERROR, TAG, "Preference - remove has failed");
        }
        else
        {
            OIC_LOG(INFO, TAG, "Preference - remove success");
        }
    }

    // remove target device for auto connection
    CAResult_t ret = CAManagerRemoveData(env, jni_leAddress);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "CAManagerRemoveData has failed");
    }

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;
}

CAResult_t CAManagerLEClientInitialize(JNIEnv *env, JavaVM *jvm, jobject context)
{
    OIC_LOG(DEBUG, TAG, "CAManagerClientInitialize");
    VERIFY_NON_NULL(env, TAG, "env");
    VERIFY_NON_NULL(jvm, TAG, "jvm");
    VERIFY_NON_NULL(context, TAG, "context");

    jint jni_int_sdk = CALEGetBuildVersion(env);
    if (jni_int_sdk < SUPPORT_ADNROID_API_LEVEL)
    {
        OIC_LOG_V(ERROR, TAG, "it is not supported (%d)", jni_int_sdk);
        return CA_STATUS_FAILED;
    }

    g_jvm = jvm;
    g_context = (*env)->NewGlobalRef(env, context);;
    CAManagerInitMutexVaraibles();
    CAManagerInitLEAutoConnection();

    CAManagerCreateACDataList(env);

    // get last connected device list
    jobject set = NULL;
    set = CAManagerGetConnectedDeviceAddress(env, g_context);
    if (!set)
    {
        // create new set<String> object
        set = CAManagerCreateSetString(env);
        if (set)
        {
            OIC_LOG(DEBUG, TAG, "created new SetString");
        }
        else
        {
            OIC_LOG(ERROR, TAG, "CAManagerCreateSetString has failed");
            return CA_STATUS_FAILED;
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "get previous Set<String> object");
    }

    g_connectedDeviceSet = (jobject)(*env)->NewGlobalRef(env, set);
    if (!g_connectedDeviceSet)
    {
        OIC_LOG(ERROR, TAG, "g_connectedDeviceSet is null");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

CAResult_t CAManagerLEClientTerminate(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CAManagerClientTerminate");
    VERIFY_NON_NULL(env, TAG, "env");

    // stop gatt connection
    CAResult_t res = CALEClientDisconnectAll(env);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "CALEClientDisconnectAll has failed");
    }

    res = CAManagerRemoveAllData(env);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "CAManagerRemoveAllData has failed");
    }

    CAManagerTerminateLEAutoConnection();
    CAManagerTerminateMutexVariables();

    if (g_context)
    {
        (*env)->DeleteGlobalRef(env, g_context);
        g_context = NULL;
    }

    if (g_connectedDeviceSet)
    {
        (*env)->DeleteGlobalRef(env, g_connectedDeviceSet);
        g_connectedDeviceSet = NULL;
    }

    return res;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caManagerAdapterStateChangedCallback(
        JNIEnv *env, jobject obj, jint state)
{
    OIC_LOG(DEBUG, TAG, "caManagerAdapterStateChangedCallback");
    VERIFY_NON_NULL_VOID(env, TAG, "env");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj");

    jint state_on = CALEGetConstantsValue(env, CLASSPATH_BT_ADAPTER, "STATE_ON");
    jint state_off = CALEGetConstantsValue(env, CLASSPATH_BT_ADAPTER, "STATE_OFF");
    jint state_turning_off = CALEGetConstantsValue(env, CLASSPATH_BT_ADAPTER, "STATE_TURNING_OFF");

    if (state_on == state)
    {
        OIC_LOG(DEBUG, TAG, "AdapterStateChangedCallback : state_on");
        if (g_adapterStateCB)
        {
            g_adapterStateCB(CA_ADAPTER_GATT_BTLE, true);
        }

        // when BT state is on. recovery flag has to be reset.
        CAManagerSetBTRecovery(false);

        // find target device for autoconnect
        size_t length = CAManagerGetACDataLength();
        OIC_LOG_V(DEBUG, TAG, "target device : %d", length);
        for (size_t idx = 0; idx < length; idx++)
        {
            jstring leAddress = CAManagerGetLEAddressFromACData(env, idx);
            if (leAddress)
            {
                CAResult_t res = CAManagerStartAutoConnection(env, leAddress);
                if (CA_STATUS_OK != res)
                {
                    OIC_LOG(ERROR, TAG, "CAManagerStartAutoConnection has failed");
                    return;
                }
            }
        }
    }
    else if (state_off == state)
    {
        OIC_LOG(DEBUG, TAG, "AdapterStateChangedCallback : state_off");
        if (g_adapterStateCB)
        {
            g_adapterStateCB(CA_ADAPTER_GATT_BTLE, false);
        }

        // reset autoconnect flag for all target devices
        size_t length = CAManagerGetACDataLength();
        for (size_t idx = 0; idx < length; idx++)
        {
            jstring address = CAManagerGetLEAddressFromACData(env, idx);
            if (address)
            {
                CAManagerSetAutoConnectionFlag(env, address, false);
            }
        }

        // check whether BT recovery is needed or not
        if (CAManagerIsRecoveryFlagSet())
        {
            CAManagerProcessRecovery(env, STATE_OFF);
        }
    }
    else if (state_turning_off == state)
    {
        OIC_LOG(DEBUG, TAG, "AdapterStateChangedCallback : state_turning_off");
        return;
    }
    else
    {
        OIC_LOG(INFO, TAG, "AdapterStateChangedCallback state is not available");
        return;
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caManagerBondStateChangedCallback(
        JNIEnv *env, jobject obj, jobject device)
{
    OIC_LOG(DEBUG, TAG, "caManagerBondStateChangedCallback");
    VERIFY_NON_NULL_VOID(env, TAG, "env");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj");
    VERIFY_NON_NULL_VOID(device, TAG, "device");

    // get ble address from Bluetooth Device object
    jstring jni_leAddress = CALEClientGetLEAddressFromBTDevice(env, device);
    if (!jni_leAddress)
    {
        OIC_LOG(INFO, TAG, "unbonded : it isn't same device type");
        return;
    }

    char* leAddress = (char*)(*env)->GetStringUTFChars(env, jni_leAddress, NULL);
    if (!leAddress)
    {
        OIC_LOG(ERROR, TAG, "leAddress is null");
        return;
    }

    // if there is no data, CAData will be created.
    OIC_LOG_V(DEBUG, TAG, "bond none device : %s", leAddress);

    CAResult_t res = CAManagerRemoveData(env, jni_leAddress);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "CAManagerRemoveData has failed");
    }

    (*env)->ReleaseStringUTFChars(env, jni_leAddress, leAddress);

    if (!CAManagerRemoveConnectedDeviceAddress(env, g_context,
                                               jni_leAddress,
                                               g_connectedDeviceSet))
    {
        OIC_LOG(ERROR, TAG, "CAManagerRemoveConnectedDeviceAddress has failed");
    }

    OIC_LOG(INFO, TAG, "bonded state changed bone_none");
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caManagerLeGattConnectionStateChangeCB(
        JNIEnv *env, jobject obj, jobject gatt, jint status, jint newState)
{
    OIC_LOG_V(DEBUG, TAG, "caManagerLeGattConnectionStateChangeCB-status(%d), newState(%d)",
              status, newState);

    VERIFY_NON_NULL_VOID(env, TAG, "env");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj");
    VERIFY_NON_NULL_VOID(gatt, TAG, "gatt");

    jint state_connected = CALEGetConstantsValue(env, CLASSPATH_BT_PROFILE, "STATE_CONNECTED");
    jint state_disconnected = CALEGetConstantsValue(env, CLASSPATH_BT_PROFILE, "STATE_DISCONNECTED");
    jint gatt_success = CALEGetConstantsValue(env, CLASSPATH_BT_GATT, "GATT_SUCCESS");

    jstring jni_address = CAManagerGetAddressFromGatt(env, gatt);
    if (!jni_address)
    {
        OIC_LOG(ERROR, TAG, "CAManagerGetAddressFromGatt is null");
        return;
    }

    char* address = (char*)(*env)->GetStringUTFChars(env, jni_address, NULL);
    if (!address)
    {
        OIC_LOG(ERROR, TAG, "address is null");
        return;
    }

    OIC_LOG_V(INFO, TAG, "connection state : status(%d), addr:(%s), newState(%d)",
              status, address, newState);

    if (gatt_success == status && state_connected == newState) // le connected
    {
        OIC_LOG(DEBUG, TAG, "LE is connected");
        CAResult_t res = CAManagerReadRemoteRssi(env, gatt);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CAManagerReadRemoteRssi has failed");
            (*env)->ReleaseStringUTFChars(env, jni_address, address);
            return;
        }
    }
    else if (state_disconnected == newState)// le disconnected
    {
        if (LINK_LOSS == status || REMOTE_DISCONNECT == status)
        {
            OIC_LOG(DEBUG, TAG, "LE is disconnected");

            if (g_connStateCB)
            {
                OIC_LOG_V(DEBUG, TAG, "LE Disconnected state is %d, %s", newState, address);
                g_connStateCB(CA_ADAPTER_GATT_BTLE, address, false);
                OIC_LOG(DEBUG, TAG, "LE Disconnected state callback is called");
            }

            if (!CAManagerIsMatchedACData(env, jni_address))
            {
                OIC_LOG_V(DEBUG, TAG, "this[%s] is not target address for Auto Connection",
                          address);
                (*env)->ReleaseStringUTFChars(env, jni_address, address);
                return;
            }

            CAManagerSetAutoConnectionFlag(env, jni_address, false);

            CAResult_t res = CAManagerStartAutoConnection(env, jni_address);
            if (CA_STATUS_OK != res)
            {
                (*env)->ReleaseStringUTFChars(env, jni_address, address);
                OIC_LOG(ERROR, TAG, "CAManagerStartAutoConnection has failed");
                return;
            }
        }
        else if (ACCEPT_TIMEOUT_EXCEPTION == status)
        {
            CAManagerProcessRecovery(env, START_RECOVERY);
        }
    }
    (*env)->ReleaseStringUTFChars(env, jni_address, address);
    (*env)->DeleteLocalRef(env, jni_address);
}

/*
 * Class:     org_iotivity_ca_jar_caleinterface
 * Method:    caManagerLeServicesDiscoveredCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caManagerLeServicesDiscoveredCallback(JNIEnv *env,
                                                                               jobject obj,
                                                                               jobject gatt,
                                                                               jint status)
{
    OIC_LOG_V(DEBUG, TAG, "caManagerLeServicesDiscoveredCallback - status %d: ", status);
    VERIFY_NON_NULL_VOID(env, TAG, "env");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj");
    VERIFY_NON_NULL_VOID(gatt, TAG, "gatt");

    if (GATT_SUCCESS == status)
    {
        if (!g_connectedDeviceSet)
        {
            OIC_LOG(ERROR, TAG, "g_connectedDeviceSet is null");
            return;
        }

        jstring jni_address = CAManagerGetAddressFromGatt(env, gatt);
        if (!jni_address)
        {
            OIC_LOG(ERROR, TAG, "CAManagerGetAddressFromGatt is null");
            return;
        }

        char* address = (char*)(*env)->GetStringUTFChars(env, jni_address, NULL);
        if (!address)
        {
            OIC_LOG(ERROR, TAG, "address is null");
            (*env)->DeleteLocalRef(env, jni_address);
            return;
        }
        OIC_LOG_V(DEBUG, TAG, "ServicesDiscovered device : %s", address);

        // target address for auto connection will be set in device list.
        // check set connected address information by user
        jclass jni_cls_set = (*env)->FindClass(env, "java/util/HashSet");
        if (!jni_cls_set)
        {
            OIC_LOG(ERROR, TAG, "jni_cls_set is null");
            (*env)->ReleaseStringUTFChars(env, jni_address, address);
            (*env)->DeleteLocalRef(env, jni_address);
            return;
        }

        jmethodID jni_mid_iterator = (*env)->GetMethodID(env, jni_cls_set, "iterator",
                                                            "()Ljava/util/Iterator;");
        if (!jni_mid_iterator)
        {
            OIC_LOG(ERROR, TAG, "jni_mid_iterator is null");
            (*env)->DeleteLocalRef(env, jni_cls_set);
            (*env)->ReleaseStringUTFChars(env, jni_address, address);
            (*env)->DeleteLocalRef(env, jni_address);
            return;
        }

        jobject jni_obj_iter = (*env)->CallObjectMethod(env, g_connectedDeviceSet, jni_mid_iterator);
        if (!jni_obj_iter)
        {
            OIC_LOG(ERROR, TAG, "jni_obj_iter is null");
            (*env)->DeleteLocalRef(env, jni_cls_set);
            (*env)->ReleaseStringUTFChars(env, jni_address, address);
            (*env)->DeleteLocalRef(env, jni_address);
            return;
        }

        // Get the Iterator method IDs
        jclass jni_cls_iterator = (*env)->FindClass(env, "java/util/Iterator");
        if (!jni_cls_iterator)
        {
            OIC_LOG(ERROR, TAG, "jni_cls_iterator is null");
            (*env)->DeleteLocalRef(env, jni_obj_iter);
            (*env)->DeleteLocalRef(env, jni_cls_set);
            (*env)->ReleaseStringUTFChars(env, jni_address, address);
            (*env)->DeleteLocalRef(env, jni_address);
            return;
        }

        jmethodID jni_mid_hasNext = (*env)->GetMethodID(env, jni_cls_iterator, "hasNext", "()Z");
        if (!jni_mid_hasNext)
        {
            OIC_LOG(ERROR, TAG, "jni_mid_hasNext is null");
            (*env)->DeleteLocalRef(env, jni_cls_iterator);
            (*env)->DeleteLocalRef(env, jni_obj_iter);
            (*env)->DeleteLocalRef(env, jni_cls_set);
            (*env)->ReleaseStringUTFChars(env, jni_address, address);
            (*env)->DeleteLocalRef(env, jni_address);
            return;
        }

        jmethodID jni_mid_next = (*env)->GetMethodID(env, jni_cls_iterator, "next",
                                                        "()Ljava/lang/Object;");
        if (!jni_mid_next)
        {
            OIC_LOG(ERROR, TAG, "jni_mid_next is null");
            (*env)->DeleteLocalRef(env, jni_cls_iterator);
            (*env)->DeleteLocalRef(env, jni_obj_iter);
            (*env)->DeleteLocalRef(env, jni_cls_set);
            (*env)->ReleaseStringUTFChars(env, jni_address, address);
            (*env)->DeleteLocalRef(env, jni_address);
            return;
        }

        // Iterate over the entry Set
        while ((*env)->CallBooleanMethod(env, jni_obj_iter, jni_mid_hasNext))
        {
            jstring jni_str_entry = (jstring)(*env)->CallObjectMethod(env, jni_obj_iter,
                                                                        jni_mid_next);
            if (!jni_str_entry)
            {
                OIC_LOG(ERROR, TAG, "jni_str_entry is null");
                (*env)->DeleteLocalRef(env, jni_cls_iterator);
                (*env)->DeleteLocalRef(env, jni_obj_iter);
                (*env)->DeleteLocalRef(env, jni_cls_set);
                (*env)->ReleaseStringUTFChars(env, jni_address, address);
                (*env)->DeleteLocalRef(env, jni_address);
                return;
            }
            const char* foundAddress = (*env)->GetStringUTFChars(env, jni_str_entry, NULL);
            if (!foundAddress)
            {
                OIC_LOG(ERROR, TAG, "addr is null");
                (*env)->DeleteLocalRef(env, jni_str_entry);
                (*env)->DeleteLocalRef(env, jni_cls_iterator);
                (*env)->DeleteLocalRef(env, jni_obj_iter);
                (*env)->DeleteLocalRef(env, jni_cls_set);
                (*env)->ReleaseStringUTFChars(env, jni_address, address);
                (*env)->DeleteLocalRef(env, jni_address);
                return;
            }
            OIC_LOG_V(INFO, TAG, "found last connected address [%s] from SharedPreferences",
                      foundAddress);

            if (!strcmp(foundAddress, address))
            {
                // if BLE address is matched each other
                // this address will be added into auto connection list.
                OIC_LOG(INFO, TAG, "AC list - address will be added into ACData list");
                CAManagerAddACData(env, jni_address);
                CAManagerSetAutoConnectionFlag(env, jni_address, false);

                // next connection will be requested as JNI_TRUE flag
                // after first connection
                CALEClientSetAutoConnectFlag(env, jni_str_entry, JNI_TRUE);
            }
            else
            {
                OIC_LOG(INFO, TAG, "AC list - device is not matched");
            }

            (*env)->ReleaseStringUTFChars(env, jni_str_entry, foundAddress);
            (*env)->DeleteLocalRef(env, jni_str_entry);
        }

        if (g_connStateCB)
        {
            g_connStateCB(CA_ADAPTER_GATT_BTLE, address, true);
            OIC_LOG(DEBUG, TAG, "LE Connected callback is called");
        }

        (*env)->DeleteLocalRef(env, jni_cls_iterator);
        (*env)->DeleteLocalRef(env, jni_obj_iter);
        (*env)->DeleteLocalRef(env, jni_cls_set);
        (*env)->ReleaseStringUTFChars(env, jni_address, address);
        (*env)->DeleteLocalRef(env, jni_address);
        OIC_LOG(INFO, TAG, "ServicesDiscovery is successful");
    }
    else
    {
        OIC_LOG(ERROR, TAG, "ServicesDiscovery has failed");
    }
}

/*
 * Class:     org_iotivity_ca_jar_caleinterface
 * Method:    caManagerLeRemoteRssiCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaLeClientInterface_caManagerLeRemoteRssiCallback(JNIEnv *env,
                                                                       jobject obj,
                                                                       jobject gatt,
                                                                       jint rssi,
                                                                       jint status)
{
    OIC_LOG_V(DEBUG, TAG, "caManagerLeRemoteRssiCallback - rssi : %d: ", rssi);
    OIC_LOG_V(DEBUG, TAG, "caManagerLeRemoteRssiCallback - status : %d: ", status);
    VERIFY_NON_NULL_VOID(env, TAG, "env");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj");
    VERIFY_NON_NULL_VOID(gatt, TAG, "gatt");
}
