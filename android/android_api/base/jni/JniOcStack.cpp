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
#include "JniOcStack.h"
#include "OCApi.h"
#include "OCPlatform.h"
#include "OCRepresentation.h"
#include "JniUtils.h"

JavaVM* g_jvm = NULL;

jclass g_cls_Integer = NULL;
jclass g_cls_int1DArray = NULL;
jclass g_cls_int2DArray = NULL;
jclass g_cls_Double = NULL;
jclass g_cls_double1DArray = NULL;
jclass g_cls_double2DArray = NULL;
jclass g_cls_Boolean = NULL;
jclass g_cls_boolean1DArray = NULL;
jclass g_cls_boolean2DArray = NULL;
jclass g_cls_String = NULL;
jclass g_cls_String1DArray = NULL;
jclass g_cls_String2DArray = NULL;
jclass g_cls_LinkedList = NULL;
jclass g_cls_Map = NULL;
jclass g_cls_MapEntry = NULL;
jclass g_cls_Set = NULL;
jclass g_cls_Iterator = NULL;
jclass g_cls_HashMap = NULL;
jclass g_cls_OcException = NULL;
jclass g_cls_OcResource = NULL;
jclass g_cls_OcRepresentation = NULL;
jclass g_cls_OcRepresentation1DArray = NULL;
jclass g_cls_OcRepresentation2DArray = NULL;
jclass g_cls_OcResourceRequest = NULL;
jclass g_cls_OcResourceResponse = NULL;
jclass g_cls_OcResourceHandle = NULL;
jclass g_cls_OcPresenceHandle = NULL;
jclass g_cls_OcRequestHandle = NULL;
jclass g_cls_OcPresenceStatus = NULL;
jclass g_cls_OcHeaderOption = NULL;
jclass g_cls_ObservationInfo = NULL;
jclass g_cls_OcResourceIdentifier = NULL;

jmethodID g_mid_Integer_ctor = NULL;
jmethodID g_mid_Double_ctor = NULL;
jmethodID g_mid_Boolean_ctor = NULL;
jmethodID g_mid_LinkedList_ctor = NULL;
jmethodID g_mid_LinkedList_add_object = NULL;
jmethodID g_mid_Map_entrySet = NULL;
jmethodID g_mid_MapEntry_getKey = NULL;
jmethodID g_mid_MapEntry_getValue = NULL;
jmethodID g_mid_Set_iterator = NULL;
jmethodID g_mid_Iterator_hasNext = NULL;
jmethodID g_mid_Iterator_next = NULL;
jmethodID g_mid_HashMap_ctor = NULL;
jmethodID g_mid_HashMap_put = NULL;
jmethodID g_mid_OcException_ctor = NULL;
jmethodID g_mid_OcException_setNativeExceptionLocation = NULL;
jmethodID g_mid_OcResource_ctor = NULL;
jmethodID g_mid_OcRepresentation_N_ctor = NULL;
jmethodID g_mid_OcRepresentation_N_ctor_bool = NULL;
jmethodID g_mid_OcResourceRequest_N_ctor = NULL;
jmethodID g_mid_OcResourceResponse_N_ctor = NULL;
jmethodID g_mid_OcResourceHandle_N_ctor = NULL;
jmethodID g_mid_OcPresenceHandle_N_ctor = NULL;
jmethodID g_mid_OcRequestHandle_N_ctor = NULL;
jmethodID g_mid_OcHeaderOption_ctor = NULL;
jmethodID g_mid_OcHeaderOption_get_id = NULL;
jmethodID g_mid_OcHeaderOption_get_data = NULL;
jmethodID g_mid_ObservationInfo_N_ctor = NULL;
jmethodID g_mid_OcPresenceStatus_get = NULL;
jmethodID g_mid_OcResourceIdentifier_N_ctor = NULL;

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
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    LOGI("JNI_OnLoad");
    JNIEnv* env;
    g_jvm = vm;

    if (g_jvm->GetEnv((void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return JNI_ERR;
    }

    jclass clazz = nullptr;

    //Integer
    clazz = env->FindClass("java/lang/Integer");
    if (!clazz) return JNI_ERR;
    g_cls_Integer = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Integer_ctor = env->GetMethodID(g_cls_Integer, "<init>", "(I)V");
    if (!g_mid_Integer_ctor) return JNI_ERR;

    clazz = env->FindClass("[I");
    if (!clazz) return JNI_ERR;
    g_cls_int1DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[[I");
    if (!clazz) return JNI_ERR;
    g_cls_int2DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    //Double
    clazz = env->FindClass("java/lang/Double");
    if (!clazz) return JNI_ERR;
    g_cls_Double = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Double_ctor = env->GetMethodID(g_cls_Double, "<init>", "(D)V");
    if (!g_mid_Double_ctor) return JNI_ERR;

    clazz = env->FindClass("[D");
    if (!clazz) return JNI_ERR;
    g_cls_double1DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[[D");
    if (!clazz) return JNI_ERR;
    g_cls_double2DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    //Boolean
    clazz = env->FindClass("java/lang/Boolean");
    if (!clazz) return JNI_ERR;
    g_cls_Boolean = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Boolean_ctor = env->GetMethodID(g_cls_Boolean, "<init>", "(Z)V");
    if (!g_mid_Boolean_ctor) return JNI_ERR;

    clazz = env->FindClass("[Z");
    if (!clazz) return JNI_ERR;
    g_cls_boolean1DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[[Z");
    if (!clazz) return JNI_ERR;
    g_cls_boolean2DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    //String
    clazz = env->FindClass("java/lang/String");
    if (!clazz) return JNI_ERR;
    g_cls_String = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[Ljava/lang/String;");
    if (!clazz) return JNI_ERR;
    g_cls_String1DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[[Ljava/lang/String;");
    if (!clazz) return JNI_ERR;
    g_cls_String2DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    //LinkedList
    clazz = env->FindClass("java/util/LinkedList");
    if (!clazz) return JNI_ERR;
    g_cls_LinkedList = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_LinkedList_ctor = env->GetMethodID(g_cls_LinkedList, "<init>", "()V");
    if (!g_mid_LinkedList_ctor) return JNI_ERR;

    g_mid_LinkedList_add_object = env->GetMethodID(g_cls_LinkedList, "add", "(Ljava/lang/Object;)Z");
    if (!g_mid_LinkedList_add_object) return JNI_ERR;

    //Map
    clazz = env->FindClass("java/util/Map");
    if (!clazz) return JNI_ERR;
    g_cls_Map = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Map_entrySet = env->GetMethodID(g_cls_Map, "entrySet", "()Ljava/util/Set;");
    if (!g_mid_Map_entrySet) return JNI_ERR;

    //MapEntry
    clazz = env->FindClass("java/util/Map$Entry");
    if (!clazz) return JNI_ERR;
    g_cls_MapEntry = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_MapEntry_getKey = env->GetMethodID(g_cls_MapEntry, "getKey", "()Ljava/lang/Object;");
    if (!g_mid_MapEntry_getKey) return JNI_ERR;
    g_mid_MapEntry_getValue = env->GetMethodID(g_cls_MapEntry, "getValue", "()Ljava/lang/Object;");
    if (!g_mid_MapEntry_getValue) return JNI_ERR;

    //Set
    clazz = env->FindClass("java/util/Set");
    if (!clazz) return JNI_ERR;
    g_cls_Set = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Set_iterator = env->GetMethodID(g_cls_Set, "iterator", "()Ljava/util/Iterator;");
    if (!g_mid_Set_iterator) return JNI_ERR;

    //Iterator
    clazz = env->FindClass("java/util/Iterator");
    if (!clazz) return JNI_ERR;
    g_cls_Iterator = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Iterator_hasNext = env->GetMethodID(g_cls_Iterator, "hasNext", "()Z");
    if (!g_mid_Iterator_hasNext) return JNI_ERR;

    g_mid_Iterator_next = env->GetMethodID(g_cls_Iterator, "next", "()Ljava/lang/Object;");
    if (!g_mid_Iterator_next) return JNI_ERR;

    //HashMap
    clazz = env->FindClass("java/util/HashMap");
    if (!clazz) return JNI_ERR;
    g_cls_HashMap = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_HashMap_ctor = env->GetMethodID(g_cls_HashMap, "<init>", "()V");
    if (!g_mid_HashMap_ctor) return JNI_ERR;

    g_mid_HashMap_put = env->GetMethodID(g_cls_HashMap, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    if (!g_mid_HashMap_put) return JNI_ERR;

    //OcException
    clazz = env->FindClass("org/iotivity/base/OcException");
    if (!clazz) return JNI_ERR;
    g_cls_OcException = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcException_ctor = env->GetMethodID(g_cls_OcException, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    if (!g_mid_OcException_ctor) return JNI_ERR;

    g_mid_OcException_setNativeExceptionLocation = env->GetMethodID(g_cls_OcException, "setNativeExceptionLocation",
        "(Ljava/lang/String;""Ljava/lang/String;""I)V");
    if (!g_mid_OcException_setNativeExceptionLocation) return JNI_ERR;

    //OcResource
    clazz = env->FindClass("org/iotivity/base/OcResource");
    if (!clazz) return JNI_ERR;
    g_cls_OcResource = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcResource_ctor = env->GetMethodID(g_cls_OcResource, "<init>", "(J)V");
    if (!g_mid_OcResource_ctor) return JNI_ERR;

    //OcRepresentation
    clazz = env->FindClass("org/iotivity/base/OcRepresentation");
    if (!clazz) return JNI_ERR;
    g_cls_OcRepresentation = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcRepresentation_N_ctor = env->GetMethodID(g_cls_OcRepresentation, "<init>", "(J)V");
    if (!g_mid_OcRepresentation_N_ctor) return JNI_ERR;

    g_mid_OcRepresentation_N_ctor_bool = env->GetMethodID(g_cls_OcRepresentation, "<init>", "(JZ)V");
    if (!g_mid_OcRepresentation_N_ctor_bool) return JNI_ERR;

    clazz = env->FindClass("[Lorg/iotivity/base/OcRepresentation;");
    if (!clazz) return JNI_ERR;
    g_cls_OcRepresentation1DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    clazz = env->FindClass("[[Lorg/iotivity/base/OcRepresentation;");
    if (!clazz) return JNI_ERR;
    g_cls_OcRepresentation2DArray = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    //HeaderOptions
    clazz = env->FindClass("org/iotivity/base/OcHeaderOption");
    if (!clazz) return JNI_ERR;
    g_cls_OcHeaderOption = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcHeaderOption_ctor = env->GetMethodID(g_cls_OcHeaderOption, "<init>", "(ILjava/lang/String;)V");
    if (!g_mid_OcHeaderOption_ctor) return JNI_ERR;

    g_mid_OcHeaderOption_get_id = env->GetMethodID(g_cls_OcHeaderOption, "getOptionId", "()I");
    if (!g_mid_OcHeaderOption_get_id) return JNI_ERR;

    g_mid_OcHeaderOption_get_data = env->GetMethodID(g_cls_OcHeaderOption, "getOptionData", "()Ljava/lang/String;");
    if (!g_mid_OcHeaderOption_get_data) return JNI_ERR;

    //OcResourceRequest
    clazz = env->FindClass("org/iotivity/base/OcResourceRequest");
    if (!clazz) return JNI_ERR;
    g_cls_OcResourceRequest = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcResourceRequest_N_ctor = env->GetMethodID(g_cls_OcResourceRequest, "<init>", "(J)V");
    if (!g_mid_OcResourceRequest_N_ctor) return JNI_ERR;

    //OcResourceResponse
    clazz = env->FindClass("org/iotivity/base/OcResourceResponse");
    if (!clazz) return JNI_ERR;
    g_cls_OcResourceResponse = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_OcResourceResponse_N_ctor = env->GetMethodID(g_cls_OcResourceResponse, "<init>", "(J)V");
    if (!g_mid_OcResourceResponse_N_ctor) return JNI_ERR;

    //OcResourceHandle
    clazz = env->FindClass("org/iotivity/base/OcResourceHandle");
    if (!clazz) return JNI_ERR;
    g_cls_OcResourceHandle = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcResourceHandle_N_ctor = env->GetMethodID(g_cls_OcResourceHandle, "<init>", "(J)V");
    if (!g_mid_OcResourceHandle_N_ctor) return JNI_ERR;

    //OcPresenceHandle
    clazz = env->FindClass("org/iotivity/base/OcPresenceHandle");
    if (!clazz) return JNI_ERR;
    g_cls_OcPresenceHandle = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcPresenceHandle_N_ctor = env->GetMethodID(g_cls_OcPresenceHandle, "<init>", "(J)V");
    if (!g_mid_OcPresenceHandle_N_ctor) return JNI_ERR;

    //OcRequestHandle
    clazz = env->FindClass("org/iotivity/base/OcRequestHandle");
    if (!clazz) return JNI_ERR;
    g_cls_OcRequestHandle = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcRequestHandle_N_ctor = env->GetMethodID(g_cls_OcRequestHandle, "<init>", "(J)V");
    if (!g_mid_OcRequestHandle_N_ctor) return JNI_ERR;

    //OcPresenceStatus
    clazz = env->FindClass("org/iotivity/base/OcPresenceStatus");
    if (!clazz) return JNI_ERR;
    g_cls_OcPresenceStatus = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcPresenceStatus_get = env->GetStaticMethodID(g_cls_OcPresenceStatus, "get",
        "(Ljava/lang/String;)Lorg/iotivity/base/OcPresenceStatus;");
    if (!g_mid_OcPresenceStatus_get) return JNI_ERR;

    //ObservationInfo
    clazz = env->FindClass("org/iotivity/base/ObservationInfo");
    if (!clazz) return JNI_ERR;
    g_cls_ObservationInfo = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_ObservationInfo_N_ctor = env->GetMethodID(g_cls_ObservationInfo, "<init>", "(IB)V");
    if (!g_mid_ObservationInfo_N_ctor) return JNI_ERR;

    clazz = env->FindClass("org/iotivity/base/OcResourceIdentifier");
    if (!clazz) return JNI_ERR;
    g_cls_OcResourceIdentifier = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    g_mid_OcResourceIdentifier_N_ctor = env->GetMethodID(g_cls_OcResourceIdentifier, "<init>", "(J)V");
    if (!g_mid_OcResourceIdentifier_N_ctor) return JNI_ERR;

    return JNI_CURRENT_VERSION;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    LOGI("JNI_OnUnload");
    JNIEnv* env;

    if (g_jvm->GetEnv((void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return;
    }

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
}