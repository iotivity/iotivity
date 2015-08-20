/*****************************************************************j
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
#include "canfcinterface.h"

#include "caadapterutils.h"
#include "camutex.h"
#include "oic_malloc.h"
#include "oic_string.h"

/**
 * TAG
 * Logging tag for module name
 */
#define TAG "NFC_SERVER"

static CANFCPacketReceivedCallback g_packetReceivedCallback;

static JavaVM *g_jvm = NULL;
static jobject g_context = NULL;
static jobject g_activity = NULL;
static jobject g_nfcInterface = NULL;
static jmethodID g_sendMethod = NULL;

static void CANfcJniInit();
static void CANfcJNISetContext();
static CAResult_t CANfcCreateJniInterfaceObject();
static CAResult_t CANfcSendDataImpl(const CAEndpoint_t * ep, const char* data, uint32_t dataLen);

static const char CLASS_NFCINTERFACE[] = "org/iotivity/ca/CaNfcInterface";

static void CANfcJniInit()
{
    OIC_LOG(DEBUG, TAG, "CANfcJniInit");
    g_jvm = (JavaVM*) CANativeJNIGetJavaVM();
}

static void CANfcJNISetContext()
{
    OIC_LOG(DEBUG, TAG, "CANfcJNISetContext");
    g_context = (jobject) CANativeJNIGetContext();
    g_activity = (jobject) CANativeGetActivity();
}

void CANFCSetPacketReceiveCallback(CANFCPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, TAG, "IN");

    g_packetReceivedCallback = callback;

    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CANfcCreateJniInterfaceObject()
{
    OIC_LOG(DEBUG, TAG, "CANfcCreateJniInterfaceObject");

    if (!g_context)
    {
        OIC_LOG(ERROR, TAG, "g_context is null");
        return CA_STATUS_FAILED;
    }

    if (!g_jvm)
    {
        OIC_LOG(ERROR, TAG, "g_jvm is null");
        return CA_STATUS_FAILED;
    }

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    jclass jni_NfcInterface = (*env)->FindClass(env, "org/iotivity/ca/CaNfcInterface");
    if (!jni_NfcInterface)
    {
        OIC_LOG(ERROR, TAG, "Could not get CaNfcInterface class");
        goto error_exit;
    }

    jmethodID NfcInterfaceConstructorMethod = (*env)->GetMethodID(env, jni_NfcInterface, "<init>",
                                            "(Landroid/content/Context;Landroid/app/Activity;)V");
    if (!NfcInterfaceConstructorMethod)
    {
        OIC_LOG(ERROR, TAG, "Could not get CaNfcInterface constructor method");
        goto error_exit;
    }

    jobject jni_nfcInstance = (*env)->NewObject(env, jni_NfcInterface,
                                                NfcInterfaceConstructorMethod, g_context,
                                                g_activity);
    if (!jni_nfcInstance)
    {
        OIC_LOG(ERROR, TAG, "Create instance for CaNfcInterface failed");
        goto error_exit;
    }

    g_nfcInterface = (*env)->NewGlobalRef(env, jni_nfcInstance);
    if (!g_nfcInterface)
    {
        OIC_LOG(ERROR, TAG, "NewGlobalRef  for nfcInterface failed");
        goto error_exit;
    }

    OIC_LOG(DEBUG, TAG, "Create instance for CaNfcInterface");

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;

error_exit:
    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_FAILED;
}

CAResult_t CAInitializeNfcServer()
{
    CANfcJniInit();
    CANfcJNISetContext();

    CAResult_t result = CANfcCreateJniInterfaceObject();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CANfcJniInit failed");
    }

    return result;
}

CAResult_t CANFCStartServer()
{
    bool isAttached = false;

    OIC_LOG(INFO, TAG, "CANFCStartServer : IN");

    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    jclass jni_NfcInterface = (*env)->FindClass(env, "org/iotivity/ca/CaNfcInterface");
    if (!jni_NfcInterface)
    {
        OIC_LOG(ERROR, TAG, "Could not get CaNFCClientInterface class");
        goto error_exit;
    }

    jmethodID methodId = (*env)->GetMethodID(env, jni_NfcInterface, "CaNfcInitialize", "()V");
    if (!methodId)
    {
        OIC_LOG(ERROR, TAG, "Could not get methodId");
        goto error_exit;
    }

    if (!g_nfcInterface)
    {
        OIC_LOG(ERROR, TAG, "g_nfcInterface NULL");
        goto error_exit;
    }

    (*env)->CallVoidMethod(env, g_nfcInterface, methodId);
    OIC_LOG(DEBUG, TAG, "CaNfcInitialize");

    jmethodID sendDataMethodId = (*env)->GetMethodID(env, jni_NfcInterface, "processSendRquest",
                                                     "([B)V");
    if (!sendDataMethodId)
    {
        OIC_LOG(ERROR, TAG, "Could not get sendDataMethodId");
        goto error_exit;
    }

    g_sendMethod = sendDataMethodId;
    OIC_LOG(DEBUG, TAG, "processSendRquest");

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;

error_exit:
    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
    return CA_STATUS_FAILED;

}

void CANFCStopServer()
{
    // JNI Call to unregstier nfc adapter
}

void CANativeNfcPacketReceived(JNIEnv *env, jobject obj, jbyteArray data)
{
    OIC_LOG(DEBUG, TAG, "caNfcPacketReceived : IN");
    VERIFY_NON_NULL_VOID(env, TAG, "env is null");
    VERIFY_NON_NULL_VOID(obj, TAG, "obj is null");
    VERIFY_NON_NULL_VOID(data, TAG, "data is null");

    char recvBuffer[COAP_MAX_PDU_SIZE];

    // get Byte Array and covert to char*
    jint length = (*env)->GetArrayLength(env, data);

    if(length >= COAP_MAX_PDU_SIZE)
    {
        OIC_LOG_V(ERROR, TAG, "caNfcPacketReceived - Invalid CoAP length : %d",
                  length);
        return;
    }

    jboolean isCopy;
    jbyte *jni_received_data = (jbyte*) (*env)->GetByteArrayElements(env, data, &isCopy);

    OIC_LOG_V(DEBUG, TAG, "caNfcPacketReceived - raw data received : %s",
              jni_received_data);

    memcpy(recvBuffer, (const char*) jni_received_data, length);
    (*env)->ReleaseByteArrayElements(env, data, jni_received_data, JNI_ABORT);

    const char* address = "AA:BB:CC:DD:EE:FF";

    OIC_LOG_V(DEBUG, TAG, "caNfcPacketReceived - data. : %s, %d",
              recvBuffer, length);

    CAEndpoint_t ep;
    ep.adapter = CA_ADAPTER_NFC;
    OICStrcpy(ep.addr, MAX_ADDR_STR_SIZE_CA, address);
    ep.port = 0;
    ep.interface = CA_DEFAULT_FLAGS;
    ep.interface = 0;

    g_packetReceivedCallback(&ep, recvBuffer, length);
    OIC_LOG(DEBUG, TAG, "caNfcPacketReceived : OUT");

}

/*
 * Class:     org_iotivity_ca_CaNfcInterface
 * Method:    caNfcPacketReceived
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaNfcInterface_caNfcPacketReceived(JNIEnv *env, jobject obj, jbyteArray data)
{
    CANativeNfcPacketReceived(env, obj, data);
}
CAResult_t CANfcSendDataImpl(const CAEndpoint_t * ep, const char* data, uint32_t dataLen)
{
    VERIFY_NON_NULL(ep, TAG, "CANfcSendDataImpl : endpoint is null");
    VERIFY_NON_NULL(data, TAG, "CANfcSendDataImpl : data is null");
    OIC_LOG(INFO, TAG, "CANfcSendDataImpl moved env outside");
    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        OIC_LOG(INFO, TAG, "Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            OIC_LOG(ERROR, TAG, "AttachCurrentThread has failed");
            return CA_STATUS_FAILED;
        }
        isAttached = true;
    }

    OIC_LOG(INFO, TAG, "creating send buffer");
    jbyteArray sendData = (*env)->NewByteArray(env, dataLen);
    if (!sendData)
    {
        OIC_LOG(ERROR, TAG, "Failed to create ByteArray");
        if (isAttached)
        {
            (*g_jvm)->DetachCurrentThread(g_jvm);
        }

        return CA_SEND_FAILED;

    }

    (*env)->SetByteArrayRegion(env, sendData, 0, dataLen, (jbyte*) data);

    (*env)->CallVoidMethod(env, g_nfcInterface, g_sendMethod, sendData);
    OIC_LOG(DEBUG, TAG, "send data through NFC");

    (*env)->DeleteLocalRef(env, sendData);

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    return CA_STATUS_OK;
}

void CANFCSendData(const CAEndpoint_t *endpoint, const void *data, uint32_t dataLength)
{
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint is NULL");
    VERIFY_NON_NULL_VOID(data, TAG, "data is NULL");

    // JNI to Send data
    CANfcSendDataImpl(endpoint, data, dataLength);
    return;
}

