/*
* ******************************************************************
*
*  Copyright 2015 Intel Corporation.
*
* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
#include "JniOcStack.h"
#include "OCApi.h"
#include "OCPlatform.h"
#include "OCRepresentation.h"
#include "JniUtils.h"

/**
 * Macro to verify the validity of input argument.
 *
 * @param  arg  log level
 */
#define VERIFY_VARIABLE_NULL(arg) \
    if (nullptr == (arg)) \
    { \
        LOGE("invalid input"); \
        return JNI_ERR; \
    } \

JavaVM* g_jvm = nullptr;

jclass g_cls_byte1DArray = nullptr;
jclass g_cls_byte2DArray = nullptr;
jclass g_cls_byte3DArray = nullptr;
jclass g_cls_Integer = nullptr;
jclass g_cls_int1DArray = nullptr;
jclass g_cls_int2DArray = nullptr;
jclass g_cls_Double = nullptr;
jclass g_cls_double1DArray = nullptr;
jclass g_cls_double2DArray = nullptr;
jclass g_cls_Boolean = nullptr;
jclass g_cls_boolean1DArray = nullptr;
jclass g_cls_boolean2DArray = nullptr;
jclass g_cls_String = nullptr;
jclass g_cls_String1DArray = nullptr;
jclass g_cls_String2DArray = nullptr;
jclass g_cls_LinkedList = nullptr;
jclass g_cls_Map = nullptr;
jclass g_cls_MapEntry = nullptr;
jclass g_cls_Set = nullptr;
jclass g_cls_Iterator = nullptr;
jclass g_cls_HashMap = nullptr;
jclass g_cls_OcException = nullptr;
jclass g_cls_OcResource = nullptr;
jclass g_cls_OcRepresentation = nullptr;
jclass g_cls_OcRepresentation1DArray = nullptr;
jclass g_cls_OcRepresentation2DArray = nullptr;
jclass g_cls_OcResourceRequest = nullptr;
jclass g_cls_OcResourceResponse = nullptr;
jclass g_cls_OcResourceHandle = nullptr;
jclass g_cls_OcPresenceHandle = nullptr;
jclass g_cls_OcRequestHandle = nullptr;
jclass g_cls_OcPresenceStatus = nullptr;
jclass g_cls_OcHeaderOption = nullptr;
jclass g_cls_ObservationInfo = nullptr;
jclass g_cls_OcResourceIdentifier = nullptr;
jclass g_cls_OcProvisionResult = nullptr;
jclass g_cls_OcSecureResource = nullptr;
jclass g_cls_OcOicSecAcl = nullptr;
jclass g_cls_OcOicSecAcl_ace = nullptr;
jclass g_cls_OcOicSecAcl_resr = nullptr;
jclass g_cls_OcOicSecAcl_validity = nullptr;
jclass g_cls_OcOicSecPdAcl = nullptr;

#ifdef WITH_CLOUD
jclass g_cls_OcAccountManager = nullptr;
#ifdef __WITH_TLS__
jclass g_cls_OcCloudProvisioning = nullptr;
jclass g_cls_OcOicSecCloudAcl_ace = nullptr;
#endif
#endif

jmethodID g_mid_Integer_ctor = nullptr;
jmethodID g_mid_Double_ctor = nullptr;
jmethodID g_mid_Boolean_ctor = nullptr;
jmethodID g_mid_LinkedList_ctor = nullptr;
jmethodID g_mid_LinkedList_add_object = nullptr;
jmethodID g_mid_Map_entrySet = nullptr;
jmethodID g_mid_MapEntry_getKey = nullptr;
jmethodID g_mid_MapEntry_getValue = nullptr;
jmethodID g_mid_Set_iterator = nullptr;
jmethodID g_mid_Iterator_hasNext = nullptr;
jmethodID g_mid_Iterator_next = nullptr;
jmethodID g_mid_HashMap_ctor = nullptr;
jmethodID g_mid_HashMap_put = nullptr;
jmethodID g_mid_OcException_ctor = nullptr;
jmethodID g_mid_OcException_setNativeExceptionLocation = nullptr;
jmethodID g_mid_OcResource_ctor = nullptr;
jmethodID g_mid_OcRepresentation_N_ctor = nullptr;
jmethodID g_mid_OcRepresentation_N_ctor_bool = nullptr;
jmethodID g_mid_OcResourceRequest_N_ctor = nullptr;
jmethodID g_mid_OcResourceResponse_N_ctor = nullptr;
jmethodID g_mid_OcResourceHandle_N_ctor = nullptr;
jmethodID g_mid_OcPresenceHandle_N_ctor = nullptr;

jmethodID g_mid_OcRequestHandle_N_ctor = nullptr;
jmethodID g_mid_OcHeaderOption_ctor = nullptr;
jmethodID g_mid_OcHeaderOption_get_id = nullptr;
jmethodID g_mid_OcHeaderOption_get_data = nullptr;
jmethodID g_mid_ObservationInfo_N_ctor = nullptr;
jmethodID g_mid_OcPresenceStatus_get = nullptr;
jmethodID g_mid_OcResourceIdentifier_N_ctor = nullptr;
jmethodID g_mid_OcProvisionResult_ctor = nullptr;
jmethodID g_mid_OcSecureResource_ctor = nullptr;
#ifdef WITH_CLOUD
jmethodID g_mid_OcAccountManager_ctor = nullptr;
#endif

jmethodID g_mid_OcOicSecPdAcl_get_resources_cnt = nullptr;
jmethodID g_mid_OcOicSecPdAcl_get_resources = nullptr;
jmethodID g_mid_OcOicSecPdAcl_get_permission = nullptr;
jmethodID g_mid_OcOicSecPdAcl_get_periods_cnt = nullptr;
jmethodID g_mid_OcOicSecPdAcl_get_periods = nullptr;
jmethodID g_mid_OcOicSecPdAcl_get_recurrences = nullptr;

jmethodID g_mid_OcOicSecAcl_get_aces = nullptr;
jmethodID g_mid_OcOicSecAcl_ace_get_subjectID = nullptr;
jmethodID g_mid_OcOicSecAcl_ace_get_permissions = nullptr;
jmethodID g_mid_OcOicSecAcl_ace_get_resources = nullptr;
jmethodID g_mid_OcOicSecAcl_ace_get_validities = nullptr;
jmethodID g_mid_OcOicSecAcl_resr_get_href = nullptr;
jmethodID g_mid_OcOicSecAcl_resr_get_rel = nullptr;
jmethodID g_mid_OcOicSecAcl_resr_get_types = nullptr;
jmethodID g_mid_OcOicSecAcl_resr_get_typeLen = nullptr;
jmethodID g_mid_OcOicSecAcl_resr_get_interfaces = nullptr;
jmethodID g_mid_OcOicSecAcl_validity_get_getPeriod = nullptr;
jmethodID g_mid_OcOicSecAcl_validity_get_recurrences = nullptr;
jmethodID g_mid_OcOicSecAcl_validity_get_recurrenceLen = nullptr;
jmethodID g_mid_OcOicSecAcl_resr_get_interfaceLen = nullptr;
jmethodID g_mid_OcOicSecAcl_get_rownerID = nullptr;

#ifdef WITH_CLOUD
#ifdef __WITH_TLS__
jmethodID g_mid_OcOicSecCloudAcl_ace_get_aclId = nullptr;
jmethodID g_mid_OcOicSecCloudAcl_ace_get_subjectID = nullptr;
jmethodID g_mid_OcOicSecCloudAcl_ace_get_stype = nullptr;
jmethodID g_mid_OcOicSecCloudAcl_ace_get_permission = nullptr;
jmethodID g_mid_OcOicSecCloudAcl_ace_get_resources = nullptr;
jmethodID g_mid_OcOicSecCloudAcl_ace_get_validities = nullptr;

jmethodID g_mid_OcCloudProvisioning_getIP = nullptr;
jmethodID g_mid_OcCloudProvisioning_getPort = nullptr;
#endif
#endif

jobject getOcException(JNIEnv* env, const char* file, const char* functionName,
    const int line, const int code, const char* message)
{
    std::string codeStr = JniUtils::stackResultToStr(code);
    if (codeStr.empty())
    {
        codeStr = JniUtils::stackResultToStr(JNI_INVALID_VALUE);
    }
    jobject ex = env->NewObject(g_cls_OcException,
        g_mid_OcException_ctor,
        env->NewStringUTF(codeStr.c_str()),
        env->NewStringUTF(message));
    if (!ex)
    {
        return nullptr;
    }
    env->CallVoidMethod(ex,
        g_mid_OcException_setNativeExceptionLocation,
        env->NewStringUTF(file),
        env->NewStringUTF(functionName),
        line);
    if (env->ExceptionCheck())
    {
        return nullptr;
    }
    return ex;
}

void throwOcException(JNIEnv* env, jobject ex)
{
    env->Throw((jthrowable)ex);
}

// JNI OnLoad
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved)
{
    OC_UNUSED(reserved);
    LOGI("JNI_OnLoad");
    JNIEnv* env = nullptr;
    g_jvm = vm;

    if (g_jvm->GetEnv((void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return JNI_ERR;
    }
    VERIFY_VARIABLE_NULL(env);

    jclass clazz = nullptr;

    //byte
    clazz = env->FindClass("[B");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_byte1DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[[B");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_byte2DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[[[B");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_byte3DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    //Integer
    clazz = env->FindClass("java/lang/Integer");
    VERIFY_VARIABLE_NULL(clazz);

    g_cls_Integer = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Integer_ctor = env->GetMethodID(g_cls_Integer, "<init>", "(I)V");
    VERIFY_VARIABLE_NULL(g_mid_Integer_ctor);

    clazz = env->FindClass("[I");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_int1DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[[I");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_int2DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    //Double
    clazz = env->FindClass("java/lang/Double");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_Double = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Double_ctor = env->GetMethodID(g_cls_Double, "<init>", "(D)V");
    VERIFY_VARIABLE_NULL(g_mid_Double_ctor);

    clazz = env->FindClass("[D");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_double1DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[[D");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_double2DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    //Boolean
    clazz = env->FindClass("java/lang/Boolean");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_Boolean = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Boolean_ctor = env->GetMethodID(g_cls_Boolean, "<init>", "(Z)V");
    VERIFY_VARIABLE_NULL(g_mid_Boolean_ctor);

    clazz = env->FindClass("[Z");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_boolean1DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[[Z");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_boolean2DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    //String
    clazz = env->FindClass("java/lang/String");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_String = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_String1DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[[Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_String2DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    //LinkedList
    clazz = env->FindClass("java/util/LinkedList");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_LinkedList = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_LinkedList_ctor = env->GetMethodID(g_cls_LinkedList, "<init>", "()V");
    VERIFY_VARIABLE_NULL(g_mid_LinkedList_ctor);

    g_mid_LinkedList_add_object = env->GetMethodID(g_cls_LinkedList, "add", "(Ljava/lang/Object;)Z");
    VERIFY_VARIABLE_NULL(g_mid_LinkedList_add_object);

    //Map
    clazz = env->FindClass("java/util/Map");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_Map = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Map_entrySet = env->GetMethodID(g_cls_Map, "entrySet", "()Ljava/util/Set;");
    VERIFY_VARIABLE_NULL(g_mid_Map_entrySet);

    //MapEntry
    clazz = env->FindClass("java/util/Map$Entry");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_MapEntry = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_MapEntry_getKey = env->GetMethodID(g_cls_MapEntry, "getKey", "()Ljava/lang/Object;");
    VERIFY_VARIABLE_NULL(g_mid_MapEntry_getKey);
    g_mid_MapEntry_getValue = env->GetMethodID(g_cls_MapEntry, "getValue", "()Ljava/lang/Object;");
    VERIFY_VARIABLE_NULL(g_mid_MapEntry_getValue);

    //Set
    clazz = env->FindClass("java/util/Set");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_Set = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Set_iterator = env->GetMethodID(g_cls_Set, "iterator", "()Ljava/util/Iterator;");
    VERIFY_VARIABLE_NULL(g_mid_Set_iterator);

    //Iterator
    clazz = env->FindClass("java/util/Iterator");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_Iterator = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Iterator_hasNext = env->GetMethodID(g_cls_Iterator, "hasNext", "()Z");
    VERIFY_VARIABLE_NULL(g_mid_Iterator_hasNext);

    g_mid_Iterator_next = env->GetMethodID(g_cls_Iterator, "next", "()Ljava/lang/Object;");
    VERIFY_VARIABLE_NULL(g_mid_Iterator_next);

    //HashMap
    clazz = env->FindClass("java/util/HashMap");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_HashMap = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_HashMap_ctor = env->GetMethodID(g_cls_HashMap, "<init>", "()V");
    VERIFY_VARIABLE_NULL(g_mid_HashMap_ctor);

    g_mid_HashMap_put = env->GetMethodID(g_cls_HashMap, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    VERIFY_VARIABLE_NULL(g_mid_HashMap_put);

    //OcException
    clazz = env->FindClass("org/iotivity/base/OcException");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcException = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcException_ctor = env->GetMethodID(g_cls_OcException, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    VERIFY_VARIABLE_NULL(g_mid_OcException_ctor);

    g_mid_OcException_setNativeExceptionLocation = env->GetMethodID(g_cls_OcException, "setNativeExceptionLocation",
        "(Ljava/lang/String;""Ljava/lang/String;""I)V");
    VERIFY_VARIABLE_NULL(g_mid_OcException_setNativeExceptionLocation);

    //OcResource
    clazz = env->FindClass("org/iotivity/base/OcResource");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcResource = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcResource_ctor = env->GetMethodID(g_cls_OcResource, "<init>", "(J)V");
    VERIFY_VARIABLE_NULL(g_mid_OcResource_ctor);

    //OcRepresentation
    clazz = env->FindClass("org/iotivity/base/OcRepresentation");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcRepresentation = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcRepresentation_N_ctor = env->GetMethodID(g_cls_OcRepresentation, "<init>", "(J)V");
    VERIFY_VARIABLE_NULL(g_mid_OcRepresentation_N_ctor);

    g_mid_OcRepresentation_N_ctor_bool = env->GetMethodID(g_cls_OcRepresentation, "<init>", "(JZ)V");
    VERIFY_VARIABLE_NULL(g_mid_OcRepresentation_N_ctor_bool);

    clazz = env->FindClass("[Lorg/iotivity/base/OcRepresentation;");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcRepresentation1DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[[Lorg/iotivity/base/OcRepresentation;");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcRepresentation2DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    //HeaderOptions
    clazz = env->FindClass("org/iotivity/base/OcHeaderOption");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcHeaderOption = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcHeaderOption_ctor = env->GetMethodID(g_cls_OcHeaderOption, "<init>", "(ILjava/lang/String;)V");
    VERIFY_VARIABLE_NULL(g_mid_OcHeaderOption_ctor);

    g_mid_OcHeaderOption_get_id = env->GetMethodID(g_cls_OcHeaderOption, "getOptionId", "()I");
    VERIFY_VARIABLE_NULL(g_mid_OcHeaderOption_get_id);

    g_mid_OcHeaderOption_get_data = env->GetMethodID(g_cls_OcHeaderOption, "getOptionData", "()Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcHeaderOption_get_data);

    //OcResourceRequest
    clazz = env->FindClass("org/iotivity/base/OcResourceRequest");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcResourceRequest = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcResourceRequest_N_ctor = env->GetMethodID(g_cls_OcResourceRequest, "<init>", "(J)V");
    VERIFY_VARIABLE_NULL(g_mid_OcResourceRequest_N_ctor);

    //OcResourceResponse
    clazz = env->FindClass("org/iotivity/base/OcResourceResponse");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcResourceResponse = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcResourceResponse_N_ctor = env->GetMethodID(g_cls_OcResourceResponse, "<init>", "(J)V");
    VERIFY_VARIABLE_NULL(g_mid_OcResourceResponse_N_ctor);

    //OcResourceHandle
    clazz = env->FindClass("org/iotivity/base/OcResourceHandle");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcResourceHandle = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcResourceHandle_N_ctor = env->GetMethodID(g_cls_OcResourceHandle, "<init>", "(J)V");
    VERIFY_VARIABLE_NULL(g_mid_OcResourceHandle_N_ctor);

    //OcPresenceHandle
    clazz = env->FindClass("org/iotivity/base/OcPresenceHandle");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcPresenceHandle = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcPresenceHandle_N_ctor = env->GetMethodID(g_cls_OcPresenceHandle, "<init>", "(J)V");
    VERIFY_VARIABLE_NULL(g_mid_OcPresenceHandle_N_ctor);

    //OcRequestHandle
    clazz = env->FindClass("org/iotivity/base/OcRequestHandle");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcRequestHandle = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcRequestHandle_N_ctor = env->GetMethodID(g_cls_OcRequestHandle, "<init>", "(J)V");
    VERIFY_VARIABLE_NULL(g_mid_OcRequestHandle_N_ctor);

    //OcPresenceStatus
    clazz = env->FindClass("org/iotivity/base/OcPresenceStatus");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcPresenceStatus = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcPresenceStatus_get = env->GetStaticMethodID(g_cls_OcPresenceStatus, "get",
        "(Ljava/lang/String;)Lorg/iotivity/base/OcPresenceStatus;");
    VERIFY_VARIABLE_NULL(g_mid_OcPresenceStatus_get);

    //ObservationInfo
    clazz = env->FindClass("org/iotivity/base/ObservationInfo");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_ObservationInfo = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_ObservationInfo_N_ctor = env->GetMethodID(g_cls_ObservationInfo, "<init>", "(IB)V");
    VERIFY_VARIABLE_NULL(g_mid_ObservationInfo_N_ctor);

    clazz = env->FindClass("org/iotivity/base/OcResourceIdentifier");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcResourceIdentifier = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcResourceIdentifier_N_ctor = env->GetMethodID(g_cls_OcResourceIdentifier, "<init>", "(J)V");
    VERIFY_VARIABLE_NULL(g_mid_OcResourceIdentifier_N_ctor);

    //OcSecureResource
    clazz = env->FindClass("org/iotivity/base/OcSecureResource");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcSecureResource =  (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcSecureResource_ctor = env->GetMethodID(g_cls_OcSecureResource, "<init>", "(J)V");
    VERIFY_VARIABLE_NULL(g_mid_OcSecureResource_ctor);

    //ProvisionResult
    clazz = env->FindClass("org/iotivity/base/ProvisionResult");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcProvisionResult =  (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcProvisionResult_ctor = env->GetMethodID(g_cls_OcProvisionResult, "<init>", "(Ljava/lang/String;I)V");
    VERIFY_VARIABLE_NULL(g_mid_OcProvisionResult_ctor);

#ifdef WITH_CLOUD
    //OcAccountManager
    clazz = env->FindClass("org/iotivity/base/OcAccountManager");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcAccountManager = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcAccountManager_ctor = env->GetMethodID(g_cls_OcAccountManager, "<init>", "(J)V");
    VERIFY_VARIABLE_NULL(g_mid_OcAccountManager_ctor);
#endif

    //OicSecAcl
    clazz = env->FindClass("org/iotivity/base/OicSecAcl");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcOicSecAcl =  (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcOicSecAcl_get_rownerID = env->GetMethodID(g_cls_OcOicSecAcl, "getRownerID", "()Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_get_rownerID);

    g_mid_OcOicSecAcl_get_aces = env->GetMethodID(g_cls_OcOicSecAcl, "getOicSecAces", "()[Lorg/iotivity/base/OicSecAce;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_get_aces);

    //OicSecAce
    clazz = env->FindClass("org/iotivity/base/OicSecAce");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcOicSecAcl_ace =  (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcOicSecAcl_ace_get_subjectID = env->GetMethodID(g_cls_OcOicSecAcl_ace, "getSubjectID","()Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_ace_get_subjectID);

    g_mid_OcOicSecAcl_ace_get_permissions = env->GetMethodID(g_cls_OcOicSecAcl_ace, "getPermission","()I");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_ace_get_permissions);

    g_mid_OcOicSecAcl_ace_get_resources = env->GetMethodID(g_cls_OcOicSecAcl_ace, "getResources","()[Lorg/iotivity/base/OicSecResr;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_ace_get_resources);

    g_mid_OcOicSecAcl_ace_get_validities = env->GetMethodID(g_cls_OcOicSecAcl_ace, "getValidities","()[Lorg/iotivity/base/OicSecValidity;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_ace_get_validities);

    //OicSecResr
    clazz = env->FindClass("org/iotivity/base/OicSecResr");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcOicSecAcl_resr =  (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcOicSecAcl_resr_get_href = env->GetMethodID(g_cls_OcOicSecAcl_resr, "getHref","()Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_resr_get_href);

    g_mid_OcOicSecAcl_resr_get_rel = env->GetMethodID(g_cls_OcOicSecAcl_resr, "getRel","()Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_resr_get_rel);

    g_mid_OcOicSecAcl_resr_get_types = env->GetMethodID(g_cls_OcOicSecAcl_resr, "getTypes","(I)Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_resr_get_types);

    g_mid_OcOicSecAcl_resr_get_typeLen = env->GetMethodID(g_cls_OcOicSecAcl_resr, "getTypeLen","()I");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_resr_get_typeLen);

    g_mid_OcOicSecAcl_resr_get_interfaces = env->GetMethodID(g_cls_OcOicSecAcl_resr, "getInterfaces","(I)Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_resr_get_interfaces);

    g_mid_OcOicSecAcl_resr_get_interfaceLen = env->GetMethodID(g_cls_OcOicSecAcl_resr, "getInterfaceLen","()I");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_resr_get_interfaceLen);

    //OicSecAcl$OicSecValidity
    clazz = env->FindClass("org/iotivity/base/OicSecValidity");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcOicSecAcl_validity =  (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcOicSecAcl_validity_get_getPeriod = env->GetMethodID(g_cls_OcOicSecAcl_validity, "getPeriod","()Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_validity_get_getPeriod);

    g_mid_OcOicSecAcl_validity_get_recurrences = env->GetMethodID(g_cls_OcOicSecAcl_validity, "getRecurrences","(I)Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_validity_get_recurrences);

    g_mid_OcOicSecAcl_validity_get_recurrenceLen = env->GetMethodID(g_cls_OcOicSecAcl_validity, "getRecurrenceLen","()I");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecAcl_validity_get_recurrenceLen);

    //OicSecPdAcl
    clazz = env->FindClass("org/iotivity/base/OicSecPdAcl");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcOicSecPdAcl =  (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcOicSecPdAcl_get_resources_cnt = env->GetMethodID(g_cls_OcOicSecPdAcl, "getResourcesCount", "()I");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecPdAcl_get_resources_cnt);

    g_mid_OcOicSecPdAcl_get_resources = env->GetMethodID(g_cls_OcOicSecPdAcl, "getResources", "(I)Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecPdAcl_get_resources);

    g_mid_OcOicSecPdAcl_get_permission = env->GetMethodID(g_cls_OcOicSecPdAcl, "getPermission", "()I");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecPdAcl_get_permission);

    g_mid_OcOicSecPdAcl_get_periods_cnt = env->GetMethodID(g_cls_OcOicSecPdAcl, "getPeriodsCount", "()I");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecPdAcl_get_periods_cnt);

    g_mid_OcOicSecPdAcl_get_periods = env->GetMethodID(g_cls_OcOicSecPdAcl, "getPeriods", "(I)Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecPdAcl_get_periods);

    g_mid_OcOicSecPdAcl_get_recurrences = env->GetMethodID(g_cls_OcOicSecPdAcl, "getRecurrences", "(I)Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecPdAcl_get_recurrences);
#ifdef SECURED
#ifdef WITH_CLOUD
#ifdef __WITH_TLS__

    //OicSecCloudAce
    clazz = env->FindClass("org/iotivity/base/OicSecCloudAce");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcOicSecCloudAcl_ace =  (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcOicSecCloudAcl_ace_get_aclId = env->GetMethodID(g_cls_OcOicSecCloudAcl_ace, "getSubjectID","()Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecCloudAcl_ace_get_aclId);

    g_mid_OcOicSecCloudAcl_ace_get_subjectID = env->GetMethodID(g_cls_OcOicSecCloudAcl_ace, "getSubjectID","()Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecCloudAcl_ace_get_subjectID);

    g_mid_OcOicSecCloudAcl_ace_get_stype = env->GetMethodID(g_cls_OcOicSecCloudAcl_ace, "getStype","()I");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecCloudAcl_ace_get_stype);

    g_mid_OcOicSecCloudAcl_ace_get_permission = env->GetMethodID(g_cls_OcOicSecCloudAcl_ace, "getPermission","()I");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecCloudAcl_ace_get_permission);

    g_mid_OcOicSecCloudAcl_ace_get_resources = env->GetMethodID(g_cls_OcOicSecCloudAcl_ace,
            "getResources","()[Lorg/iotivity/base/OicSecResr;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecCloudAcl_ace_get_resources);

    g_mid_OcOicSecCloudAcl_ace_get_validities = env->GetMethodID(g_cls_OcOicSecCloudAcl_ace,
            "getValidities","()[Lorg/iotivity/base/OicSecValidity;");
    VERIFY_VARIABLE_NULL(g_mid_OcOicSecCloudAcl_ace_get_validities);


    //OcCloudProvisioning
    clazz = env->FindClass("org/iotivity/base/OcCloudProvisioning");
    VERIFY_VARIABLE_NULL(clazz);
    g_cls_OcCloudProvisioning =  (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcCloudProvisioning_getIP = env->GetMethodID(g_cls_OcCloudProvisioning,
            "getIP", "()Ljava/lang/String;");
    VERIFY_VARIABLE_NULL(g_mid_OcCloudProvisioning_getIP);

    g_mid_OcCloudProvisioning_getPort = env->GetMethodID(g_cls_OcCloudProvisioning,
            "getPort", "()I");
    VERIFY_VARIABLE_NULL(g_mid_OcCloudProvisioning_getPort);
#endif
#endif
#endif
    return JNI_CURRENT_VERSION;
}

JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM *vm, void *reserved)
{
    OC_UNUSED(reserved);
    LOGI("JNI_OnUnload");
    JNIEnv* env = nullptr;

    if (vm->GetEnv((void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return;
    }

    if (env)
    {
        env->DeleteGlobalRef(g_cls_Integer);
        env->DeleteGlobalRef(g_cls_int1DArray);
        env->DeleteGlobalRef(g_cls_int2DArray);
        env->DeleteGlobalRef(g_cls_Double);
        env->DeleteGlobalRef(g_cls_double1DArray);
        env->DeleteGlobalRef(g_cls_double2DArray);
        env->DeleteGlobalRef(g_cls_Boolean);
        env->DeleteGlobalRef(g_cls_boolean1DArray);
        env->DeleteGlobalRef(g_cls_boolean2DArray);
        env->DeleteGlobalRef(g_cls_String);
        env->DeleteGlobalRef(g_cls_String1DArray);
        env->DeleteGlobalRef(g_cls_String2DArray);
        env->DeleteGlobalRef(g_cls_LinkedList);
        env->DeleteGlobalRef(g_cls_Map);
        env->DeleteGlobalRef(g_cls_MapEntry);
        env->DeleteGlobalRef(g_cls_Set);
        env->DeleteGlobalRef(g_cls_Iterator);
        env->DeleteGlobalRef(g_cls_HashMap);
        env->DeleteGlobalRef(g_cls_OcResource);
        env->DeleteGlobalRef(g_cls_OcException);
        env->DeleteGlobalRef(g_cls_OcRepresentation);
        env->DeleteGlobalRef(g_cls_OcRepresentation1DArray);
        env->DeleteGlobalRef(g_cls_OcRepresentation2DArray);
        env->DeleteGlobalRef(g_cls_OcResourceRequest);
        env->DeleteGlobalRef(g_cls_OcResourceResponse);
        env->DeleteGlobalRef(g_cls_OcResourceHandle);
        env->DeleteGlobalRef(g_cls_OcPresenceHandle);
        env->DeleteGlobalRef(g_cls_OcRequestHandle);
        env->DeleteGlobalRef(g_cls_OcPresenceStatus);
        env->DeleteGlobalRef(g_cls_OcHeaderOption);
        env->DeleteGlobalRef(g_cls_ObservationInfo);
        env->DeleteGlobalRef(g_cls_OcResourceIdentifier);
        env->DeleteGlobalRef(g_cls_OcSecureResource);
        env->DeleteGlobalRef(g_cls_OcProvisionResult);
        env->DeleteGlobalRef(g_cls_byte1DArray);
        env->DeleteGlobalRef(g_cls_byte2DArray);
        env->DeleteGlobalRef(g_cls_byte3DArray);
#ifdef WITH_CLOUD
        env->DeleteGlobalRef(g_cls_OcAccountManager);
#ifdef __WITH_TLS__
        env->DeleteGlobalRef(g_cls_OcCloudProvisioning);
				env->DeleteGlobalRef(g_cls_OcOicSecCloudAcl_ace);
#endif
#endif
        env->DeleteGlobalRef(g_cls_OcOicSecAcl);
        env->DeleteGlobalRef(g_cls_OcOicSecAcl_ace);
        env->DeleteGlobalRef(g_cls_OcOicSecAcl_resr);
        env->DeleteGlobalRef(g_cls_OcOicSecAcl_validity);
    }
}
