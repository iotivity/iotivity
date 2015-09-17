/******************************************************************
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
#include "jni_re_jvm.h"
#include "jni_re_utils.h"

JavaVM *g_jvm = NULL;

jclass g_cls_Integer = NULL;
jclass g_cls_Double = NULL;
jclass g_cls_Boolean = NULL;
jclass g_cls_String = NULL;
jclass g_cls_LinkedList = NULL;
jclass g_cls_Iterator = NULL;
jclass g_cls_Vector = NULL;
jclass g_cls_Map = NULL;
jclass g_cls_Set = NULL;
jclass g_cls_MapEntry = NULL;
jclass g_cls_RCSRemoteResourceObject = NULL;
jclass g_cls_RCSRemoteResourceAttributesObject = NULL;
jclass g_cls_RCSException = NULL;

jclass g_cls_RCSResourceObject = NULL;
jclass g_cls_RCSRequest = NULL;
jclass g_cls_RCSBundleInfo = NULL;

jmethodID g_mid_Integer_ctor = NULL;
jmethodID g_mid_Integer_getInt = NULL;
jmethodID g_mid_Double_ctor = NULL;
jmethodID g_mid_Double_getDouble = NULL;
jmethodID g_mid_Boolean_getBoolean = NULL;
jmethodID g_mid_Boolean_ctor = NULL;
jmethodID g_mid_LinkedList_ctor = NULL;
jmethodID g_mid_Set_iterator = NULL;
jmethodID g_mid_LinkedList_add_object = NULL;
jmethodID g_mid_Vector_ctor = NULL;
jmethodID g_mid_Vector_size = NULL;
jmethodID g_mid_Vector_get = NULL;
jmethodID g_mid_Vector_add = NULL;
jmethodID g_mid_Map_entrySet = NULL;
jmethodID g_mid_MapEntry_getKey = NULL;
jmethodID g_mid_MapEntry_getValue = NULL;
jmethodID g_mid_Iterator_hasNext = NULL;
jmethodID g_mid_Iterator_next = NULL;
jmethodID g_mid_RCSRemoteResourceObject_ctor = NULL;
jmethodID g_mid_RCSRemoteResourceAttributesObject_ctor = NULL;
jmethodID g_mid_RCSRemoteResourceAttributesObject_ctor_bool = NULL;
jmethodID g_mid_RCSException_ctor = NULL;

jmethodID g_mid_RCSResourceObject_ctor = NULL;
jmethodID g_mid_RCSRequest_ctor = NULL;
jmethodID g_mid_RCSBundleInfo_ctor = NULL;

// JNI OnLoad
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    LOGI("JNI_OnLoad");
    JNIEnv *env;
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

    g_mid_Integer_getInt = env->GetMethodID(g_cls_Integer, "intValue", "()I");
    if (!g_mid_Integer_getInt) return JNI_ERR;

    //Double
    clazz = env->FindClass("java/lang/Double");
    if (!clazz) return JNI_ERR;
    g_cls_Double = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Double_ctor = env->GetMethodID(g_cls_Double, "<init>", "(D)V");
    if (!g_mid_Double_ctor) return JNI_ERR;

    g_mid_Double_getDouble = env->GetMethodID(g_cls_Double, "doubleValue", "()D");
    if (!g_mid_Double_getDouble) return JNI_ERR;


    //Boolean
    clazz = env->FindClass("java/lang/Boolean");
    if (!clazz) return JNI_ERR;
    g_cls_Boolean = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Boolean_ctor = env->GetMethodID(g_cls_Boolean, "<init>", "(Z)V");
    if (!g_mid_Boolean_ctor) return JNI_ERR;

    g_mid_Boolean_getBoolean = env->GetMethodID(g_cls_Boolean, "booleanValue", "()Z");
    if (!g_mid_Boolean_getBoolean) return JNI_ERR;


    //String
    clazz = env->FindClass("java/lang/String");
    if (!clazz) return JNI_ERR;
    g_cls_String = (jclass)env->NewGlobalRef(clazz);
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

    //Vector
    clazz = env->FindClass("java/util/Vector");
    if (!clazz) return JNI_ERR;
    g_cls_Vector = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Vector_ctor = env->GetMethodID(g_cls_Vector, "<init>", "()V");
    if (!g_mid_Vector_ctor) return JNI_ERR;

    g_mid_Vector_size = env->GetMethodID(g_cls_Vector, "size", "()I");
    if (!g_mid_Vector_size) return JNI_ERR;

    g_mid_Vector_get = env->GetMethodID(g_cls_Vector, "get", "(I)""Ljava/lang/Object;");
    if (!g_mid_Vector_get) return JNI_ERR;

    g_mid_Vector_add =  env->GetMethodID(g_cls_Vector, "add", "(Ljava/lang/Object;)Z");
    if (!g_mid_Vector_add) return JNI_ERR;

    //RCSRemoteResourceObject
    clazz = env->FindClass("org/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject");
    if (!clazz) return JNI_ERR;
    g_cls_RCSRemoteResourceObject = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_RCSRemoteResourceObject_ctor = env->GetMethodID(g_cls_RCSRemoteResourceObject, "<init>",
                                         "(J)V");
    if (!g_mid_RCSRemoteResourceObject_ctor) return JNI_ERR;

    //ResourceState enum
    jclass tmpclazz = reinterpret_cast<jclass>
            (env->FindClass("org/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState"));
    if (!tmpclazz)
    {
        LOGE("Failed to get the class ResourceState");
        return JNI_ERR;
    }
    ResourceStateWrapper::clazz = reinterpret_cast<jclass> (env->NewGlobalRef(tmpclazz));
    env->DeleteLocalRef(tmpclazz);

    //RCS Remote Resource Attributes
    clazz = env->FindClass("org/iotivity/ResourceEncapsulation/common/RCSResourceAttributes");
    if (!clazz) return JNI_ERR;
    g_cls_RCSRemoteResourceAttributesObject = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_RCSRemoteResourceAttributesObject_ctor = env->GetMethodID(
                g_cls_RCSRemoteResourceAttributesObject, "<init>", "(J)V");
    if (!g_mid_RCSRemoteResourceAttributesObject_ctor) return JNI_ERR;

    g_mid_RCSRemoteResourceAttributesObject_ctor_bool = env->GetMethodID(
               g_cls_RCSRemoteResourceAttributesObject, "<init>", "(JZ)V");
    if (!g_mid_RCSRemoteResourceAttributesObject_ctor_bool) return JNI_ERR;

    //RCSException
    clazz = env->FindClass("org/iotivity/ResourceEncapsulation/common/RCSException");
    if (!clazz) return JNI_ERR;
    g_cls_RCSException = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_RCSException_ctor = env->GetMethodID(g_cls_RCSException, "<init>", "(Ljava/lang/String;)V");
    if (!g_mid_RCSException_ctor) return JNI_ERR;

    //RCSResourceObject
    clazz = env->FindClass("org/iotivity/ResourceEncapsulation/server/RCSResourceObject");
    if (!clazz) return JNI_ERR;
    g_cls_RCSResourceObject = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_RCSResourceObject_ctor = env->GetMethodID(g_cls_RCSResourceObject, "<init>",
                                   "(J)V");
    if (!g_mid_RCSResourceObject_ctor) return JNI_ERR;

    //RCSRequest
    clazz = env->FindClass("org/iotivity/ResourceEncapsulation/server/RCSRequest");
    if (!clazz) return JNI_ERR;
    g_cls_RCSRequest = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    //RCSBundleInfo
    clazz = env->FindClass("org/iotivity/ResourceEncapsulation/server/RCSBundleInfo");
    if (!clazz) return JNI_ERR;
    g_cls_RCSBundleInfo = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_RCSBundleInfo_ctor = env->GetMethodID(g_cls_RCSBundleInfo, "<init>",
                               "(J)V");
    if (!g_mid_RCSBundleInfo_ctor) return JNI_ERR;

    return JNI_CURRENT_VERSION;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    LOGI("JNI_OnUnload");
    JNIEnv *env;

    if (g_jvm->GetEnv((void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return;
    }

    env->DeleteGlobalRef(g_cls_Integer);
    env->DeleteGlobalRef(g_cls_Double);
    env->DeleteGlobalRef(g_cls_Boolean);
    env->DeleteGlobalRef(g_cls_String);
    env->DeleteGlobalRef(g_cls_LinkedList);
    env->DeleteGlobalRef(g_cls_Vector);
    env->DeleteGlobalRef(g_cls_Iterator);
    env->DeleteGlobalRef(g_cls_RCSRemoteResourceObject);
    env->DeleteGlobalRef(g_cls_RCSRemoteResourceAttributesObject);
    env->DeleteGlobalRef(g_cls_RCSException);
    env->DeleteGlobalRef(g_cls_RCSResourceObject);
    env->DeleteGlobalRef(g_cls_RCSRequest);
    env->DeleteGlobalRef(g_cls_Map);
    env->DeleteGlobalRef(g_cls_Set);
    env->DeleteGlobalRef(g_cls_MapEntry);
    env->DeleteGlobalRef(g_cls_RCSBundleInfo);

    if (env)
    {
        if (ResourceStateWrapper::clazz != 0)
            env->DeleteGlobalRef(ResourceStateWrapper::clazz);
    }
}
