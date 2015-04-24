/*
* //******************************************************************
* //
* // Copyright 2015 Intel Corporation.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
* //
* // Licensed under the Apache License, Version 2.0 (the "License");
* // you may not use this file except in compliance with the License.
* // You may obtain a copy of the License at
* //
* //      http://www.apache.org/licenses/LICENSE-2.0
* //
* // Unless required by applicable law or agreed to in writing, software
* // distributed under the License is distributed on an "AS IS" BASIS,
* // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* // See the License for the specific language governing permissions and
* // limitations under the License.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include "cainterface.h"
#include "cacommon.h"
#include "JniCaInterface.h"

#define  LOG_TAG   "JNI_CA_INTERFACE"
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static CAToken_t g_lastRequestToken;

JavaVM* g_jvm = NULL;

#ifdef __WITH_DTLS__
static CADtlsPskCredsBlob_t *pskCredsBlob = NULL;

void clearDtlsCredentialInfo()
{
    LOGI("clearDtlsCredentialInfo IN");
    if (pskCredsBlob)
    {
        // Initialize sensitive data to zeroes before freeing.
        if (NULL != pskCredsBlob->creds)
        {
            memset(pskCredsBlob->creds, 0, sizeof(OCDtlsPskCreds)*(pskCredsBlob->num));
            free(pskCredsBlob->creds);
        }

        memset(pskCredsBlob, 0, sizeof(CADtlsPskCredsBlob_t));
        free(pskCredsBlob);
        pskCredsBlob = NULL;
    }
    LOGI("clearDtlsCredentialInfo OUT");
}

// Internal API. Invoked by OC stack to retrieve credentials from this module
void CAGetDtlsPskCredentials(CADtlsPskCredsBlob_t **credInfo)
{
    LOGI("CAGetDtlsPskCredentials IN");
    *credInfo = (CADtlsPskCredsBlob_t *) malloc(sizeof(CADtlsPskCredsBlob_t));
    if (NULL == *credInfo)
    {
        LOGE("Failed to allocate credential blob.");
        return;
    }

    int16_t credLen = sizeof(OCDtlsPskCreds) * (pskCredsBlob->num);
    (*credInfo)->creds = (OCDtlsPskCreds *) malloc(credLen);
    if (NULL == (*credInfo)->creds)
    {
        LOGE("Failed to allocate crentials.");
        free(*credInfo);
        *credInfo = NULL;
        return;
    }

    memcpy((*credInfo)->identity, pskCredsBlob->identity, DTLS_PSK_ID_LEN);
    (*credInfo)->num = pskCredsBlob->num;
    memcpy((*credInfo)->creds, pskCredsBlob->creds, credLen);

    LOGI("CAGetDtlsPskCredentials OUT");
}

CAResult_t SetCredentials()
{
    LOGI("SetCredentials IN");
    pskCredsBlob = (CADtlsPskCredsBlob_t *)malloc(sizeof(CADtlsPskCredsBlob_t));
    if (NULL == pskCredsBlob)
    {
        LOGE("Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memcpy(pskCredsBlob->identity, IDENTITY, DTLS_PSK_ID_LEN);

    pskCredsBlob->num = 1;

    pskCredsBlob->creds = (OCDtlsPskCreds *)malloc(sizeof(OCDtlsPskCreds) *(pskCredsBlob->num));
    if (NULL == pskCredsBlob->creds)
    {
        LOGE("Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memcpy(pskCredsBlob->creds[0].id, IDENTITY, DTLS_PSK_ID_LEN);
    memcpy(pskCredsBlob->creds[0].psk, RS_CLIENT_PSK, DTLS_PSK_PSK_LEN);

    LOGI("SetCredentials OUT");
    return CA_STATUS_OK;
}
#endif

JNIEXPORT jint JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    LOGI("JNI_OnLoad");

    JNIEnv* env;
    if (JNI_OK != (*jvm)->GetEnv(jvm, (void**) &env, JNI_VERSION_1_6))
    {
        return -1;
    }
    g_jvm = jvm; /* cache the JavaVM pointer */

    CANativeJNISetJavaVM(g_jvm);

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *jvm, void *reserved)
{
    LOGI("JNI_OnUnload");

    JNIEnv* env;
    if (JNI_OK != (*jvm)->GetEnv(jvm, (void**) &env, JNI_VERSION_1_6))
    {
        return;
    }
    g_jvm = 0;
    return;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_initialize
(JNIEnv *env, jclass clazz, jobject context)
{
    LOGI("initialize");

    CANativeJNISetContext(env, context);

    CAResult_t res = CAInitialize();

    if (CA_STATUS_OK != res)
    {
        LOGE("Could not Initialize");
    }

#ifdef __WITH_DTLS__
    if (CA_STATUS_OK != SetCredentials())
    {
        LOGE("SetCredentials failed");
        return;
    }

    res = CARegisterDTLSCredentialsHandler(CAGetDtlsPskCredentials);
    if(CA_STATUS_OK != res)
    {
        LOGE("Set credential handler fail");
        return;
    }
#endif
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_terminate(JNIEnv *env, jclass clazz)
{
    LOGI("terminate");
    CADestroyToken(g_lastRequestToken);
    CATerminate();
}