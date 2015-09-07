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

#include "jni_re_resource_attributes.h"
#include "jni_re_jvm.h"

RCSResourceAttributes *JniResourceAttributes::getResourceAttributePtr(JNIEnv *env,
        jobject interfaceObject)
{
    RCSResourceAttributes *rep = REGetHandle<RCSResourceAttributes>(env, interfaceObject);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to get native handle from RCSResourceAttributes");
    }
    if (!rep)
    {
        throwRCSException(env, "JNI no native pointer");
    }
    return rep;
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeGetValueN
(JNIEnv *env, jobject interfaceObject, jstring jKey)
{
    LOGD("RCSResourceAttributes_getValueN");
    if (!jKey)
    {
        throwRCSException(env, "attributeKey cannot be null");
        return nullptr;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return nullptr;

    std::string key = env->GetStringUTFChars(jKey, NULL);

    if (! rep->contains(key)) return nullptr;

    RCSResourceAttributes::Value attrValue = rep->at(key);
    RCSResourceAttributes::TypeId typeId = attrValue.getType().getId();

    if (typeId == RCSResourceAttributes::TypeId::INT)
    {
        int val = attrValue.get<int>();
        jobject jobj = env->NewObject(
                           g_cls_Integer,
                           g_mid_Integer_ctor,
                           static_cast<jint>(val));
        return jobj;
    }
    else if (typeId == RCSResourceAttributes::TypeId::DOUBLE)
    {
        double val = attrValue.get<double>();
        jobject jobj = env->NewObject(
                           g_cls_Double,
                           g_mid_Double_ctor,
                           static_cast<jdouble>(val));
        return jobj;
    }
    else if (typeId == RCSResourceAttributes::TypeId::BOOL)
    {
        bool val = attrValue.get<bool>();
        jobject jobj = env->NewObject(
                           g_cls_Boolean,
                           g_mid_Boolean_ctor,
                           static_cast<jboolean>(val));
        return jobj;
    }
    else if (typeId == RCSResourceAttributes::TypeId::STRING)
    {
        std::string val = attrValue.get<std::string>();
        jstring jstr = env->NewStringUTF(val.c_str());
        return static_cast<jobject>(jstr);
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueInteger
(JNIEnv *env, jobject interfaceObject, jstring jKey, jint jValue)
{
    LOGD("RCSResourceAttributes_setValueInteger");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);

    RCSResourceAttributes setAttribute;
    setAttribute[key] = static_cast<int>(jValue);
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueDouble
(JNIEnv *env, jobject interfaceObject, jstring jKey, jdouble jValue)
{
    LOGD("RCSResourceAttributes_setValueDouble");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);

    RCSResourceAttributes setAttribute;
    setAttribute[key] = static_cast<double>(jValue);
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueBoolean
(JNIEnv *env, jobject interfaceObject, jstring jKey, jboolean jValue)
{
    LOGD("RCSResourceAttributes_setValueBoolean");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);

    RCSResourceAttributes setAttribute;
    setAttribute[key] = static_cast<bool>(jValue);
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueStringN
(JNIEnv *env, jobject interfaceObject, jstring jKey, jstring jValue)
{
    LOGD("RCSResourceAttributes_setValueStringN");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    std::string value = env->GetStringUTFChars(jValue, NULL);

    RCSResourceAttributes setAttribute;
    setAttribute[key] = value;
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueIntegerArray
(JNIEnv *env, jobject interfaceObject, jstring jKey, jintArray jValue)
{
    LOGD("RCSResourceAttributes_setValueIntegerArray");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    const jsize len = env->GetArrayLength(jValue);
    jint *ints = env->GetIntArrayElements(jValue, NULL);

    std::vector<int> value;
    for (jsize i = 0; i < len; ++i)
    {
        value.push_back(static_cast<int>(ints[i]));
    }
    env->ReleaseIntArrayElements(jValue, ints, JNI_ABORT);

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeIsEmpty
(JNIEnv *env, jobject interfaceObject)
{
    LOGD("RCSResourceAttributes_isEmpty");
    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return false;

    return static_cast<jboolean>(rep->empty());
}

JNIEXPORT jint JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSize
(JNIEnv *env, jobject interfaceObject)
{
    LOGD("RCSResourceAttributes_size");
    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return -1;

    return static_cast<jint>(rep->size());
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeRemove
(JNIEnv *env, jobject interfaceObject, jstring jAttributeKey)
{
    LOGD("RCSResourceAttributes_remove");
    if (!jAttributeKey)
    {
        throwRCSException(env, "attributeKey cannot be null");
        return false;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return false;

    std::string attributeKey = env->GetStringUTFChars(jAttributeKey, NULL);
    return static_cast<jboolean>(rep->erase(attributeKey));
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeHasAttribute
(JNIEnv *env, jobject interfaceObject, jstring jstr)
{
    LOGD("RCSResourceAttributes_hasAttribute");
    if (!jstr)
    {
        throwRCSException(env, "attributeKey cannot be null");
        return false;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return false;

    std::string attributeKey = env->GetStringUTFChars(jstr, NULL);
    return static_cast<jboolean>(rep->contains(attributeKey));
}

JNIEXPORT void JNICALL Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_create
(JNIEnv *env, jobject interfaceObject)
{
    LOGD("RCSResourceAttributes_create");
    RCSResourceAttributes *rep = new RCSResourceAttributes();
    RESetHandle<RCSResourceAttributes>(env, interfaceObject, rep);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to set native handle for RCSResourceAttributes");
        delete rep;
    }
}

JNIEXPORT void JNICALL Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_dispose
(JNIEnv *env, jobject interfaceObject, jboolean jNeedsDelete)
{
    LOGD("RCSResourceAttributes_dispose");
    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);

    if (jNeedsDelete)
    {
        delete rep;
    }
}
