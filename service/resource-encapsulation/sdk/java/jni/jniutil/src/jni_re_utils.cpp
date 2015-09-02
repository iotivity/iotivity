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
#include "jni_re_utils.h"

int covertResourceStateToInt(ResourceState state)
{
    switch (state)
    {
        case ResourceState::NONE:
            return 0;

        case ResourceState::REQUESTED:
            return 1;

        case ResourceState::ALIVE:
            return 2;

        case ResourceState::LOST_SIGNAL:
            return 3;

        case ResourceState::DESTROYED:
            return 4;
    }
    return 0;
}

int convertCacheStateToInt(CacheState state)
{
    switch (state)
    {
        case CacheState::NONE:
            return 0;

        case CacheState::UNREADY:
            return 1;

        case CacheState::READY:
            return 2;

        case CacheState::LOST_SIGNAL:
            return 3;
    }
    return 0;
}

jobject convertStrVectorToJavaStrList(JNIEnv *env, std::vector<std::string> vector)
{
    jobject jList = env->NewObject(g_cls_LinkedList, g_mid_LinkedList_ctor);
    if (!jList) return nullptr;
    for (size_t i = 0; i < vector.size(); ++i)
    {
        jstring jStr = env->NewStringUTF(vector[i].c_str());
        if (!jStr) return nullptr;
        env->CallBooleanMethod(jList, g_mid_LinkedList_add_object, jStr);
        if (env->ExceptionCheck()) return nullptr;
        env->DeleteLocalRef(jStr);
    }
    return jList;
}

OCEntityHandlerResult convertIntToOCEntityHandlerResult(int result)
{
    switch (result)
    {
        case 1:
            return OCEntityHandlerResult::OC_EH_OK;
        case 2:
            return OCEntityHandlerResult::OC_EH_ERROR;
        case 3:
            return OCEntityHandlerResult::OC_EH_RESOURCE_CREATED;
        case 4:
            return OCEntityHandlerResult::OC_EH_RESOURCE_DELETED;
        case 5:
            return OCEntityHandlerResult::OC_EH_SLOW;
        case 6:
            return OCEntityHandlerResult::OC_EH_FORBIDDEN;
        case 7:
            return OCEntityHandlerResult::OC_EH_RESOURCE_NOT_FOUND;
    }
    return OCEntityHandlerResult::OC_EH_ERROR;
}

void throwRCSException(JNIEnv *env, std::string reason)
{
    jobject ex = env->NewObject(g_cls_RCSException, g_mid_RCSException_ctor,
                                env->NewStringUTF(reason.c_str()));
    if (!ex)
    {
        LOGI("throwRCSException jobject == NULL");
    }
    env->Throw((jthrowable)ex);
}

jfieldID  ResourceStateWrapper::ALIVE_ID = 0;
jfieldID  ResourceStateWrapper::REQUESTED_ID = 0;
jfieldID  ResourceStateWrapper::LOST_SIGNAL_ID = 0;
jfieldID  ResourceStateWrapper::DESTROYED_ID = 0;
jfieldID  ResourceStateWrapper::NONE_ID = 0;

jmethodID ResourceStateWrapper::ordinal_ID = 0;
jmethodID ResourceStateWrapper::toString_ID = 0;
jmethodID ResourceStateWrapper::valueOf_ID = 0;
jclass ResourceStateWrapper::clazz = 0;

jobject ResourceStateWrapper::getALIVE(JNIEnv *env)
{
    return env->GetStaticObjectField(clazz, ALIVE_ID);
}
jobject ResourceStateWrapper::getREQUESTED(JNIEnv *env)
{
    return env->GetStaticObjectField(clazz, REQUESTED_ID);
}
jobject ResourceStateWrapper::getLOST_SIGNAL(JNIEnv *env)
{
    return env->GetStaticObjectField(clazz, LOST_SIGNAL_ID);
}
jobject ResourceStateWrapper::getDESTROYED(JNIEnv *env)
{
    return env->GetStaticObjectField(clazz, DESTROYED_ID);
}
jobject ResourceStateWrapper::getNONE(JNIEnv *env)
{
    return env->GetStaticObjectField(clazz, NONE_ID);
}
jint ResourceStateWrapper::ordinal(JNIEnv *env, jobject enumObj)
{
    return env->CallIntMethod(enumObj, ordinal_ID);
}
jstring ResourceStateWrapper::toString(JNIEnv *env, jobject enumObj)
{
    return reinterpret_cast<jstring>(env->CallObjectMethod(enumObj, toString_ID));
}
jobject ResourceStateWrapper::valueOf(JNIEnv *env, jstring enumName)
{
    return env->CallStaticObjectMethod(clazz, valueOf_ID, (jobject)enumName);
}

jobject convertNativeResourceStateToJavaResourceState(ResourceState nativeResourceState,
        JNIEnv *env)
{
    LOGI("convertNativeResourceStateToJavaResourceState enter");

    ResourceStateWrapper::ALIVE_ID = env->GetStaticFieldID(ResourceStateWrapper::clazz, "ALIVE",
                         "Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;");
    ResourceStateWrapper::REQUESTED_ID = env->GetStaticFieldID(ResourceStateWrapper::clazz, "REQUESTED",
                         "Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;");
    ResourceStateWrapper::LOST_SIGNAL_ID = env->GetStaticFieldID(ResourceStateWrapper::clazz,
                         "LOST_SIGNAL", "Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;");
    ResourceStateWrapper::DESTROYED_ID = env->GetStaticFieldID(ResourceStateWrapper::clazz, "DESTROYED",
                         "Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;");
    ResourceStateWrapper::NONE_ID = env->GetStaticFieldID(ResourceStateWrapper::clazz, "NONE",
                          "Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;");

    ResourceStateWrapper::ordinal_ID = env->GetMethodID(ResourceStateWrapper::clazz, "ordinal", "()I");
    ResourceStateWrapper::toString_ID = env->GetMethodID(ResourceStateWrapper::clazz, "toString",
                                        "()Ljava/lang/String;");
    ResourceStateWrapper::valueOf_ID = env->GetStaticMethodID(ResourceStateWrapper::clazz, "valueOf",
                                       "(Ljava/lang/String;)Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;");

    LOGI("convertNativeResourceStateToJavaResourceState ResourceStateWrapper::clazz == NULL first time");

    switch (nativeResourceState)
    {
        case ResourceState::NONE:
            return ResourceStateWrapper::getNONE(env);

        case ResourceState::REQUESTED:
            return ResourceStateWrapper::getREQUESTED(env);

        case ResourceState::ALIVE:
            return ResourceStateWrapper::getALIVE(env);

        case ResourceState::LOST_SIGNAL:
            return ResourceStateWrapper::getLOST_SIGNAL(env);

        case ResourceState::DESTROYED:
            return ResourceStateWrapper::getDESTROYED(env);
    }
}

RCSResourceObject::AutoNotifyPolicy convertJavaNotifyPolicyToNativePolicy( int policy)
{
    switch (policy)
    {
        case 1:
            return RCSResourceObject::AutoNotifyPolicy::NEVER;

        case 2:
            return RCSResourceObject::AutoNotifyPolicy::ALWAYS;

        case 3:
            return RCSResourceObject::AutoNotifyPolicy::UPDATED;
    }
}

RCSResourceObject::SetRequestHandlerPolicy convertJavasetRequestHandlerPolicyToNativePolicy(
    int policy)
{
    switch (policy)
    {
        case 1:
            return RCSResourceObject::SetRequestHandlerPolicy::NEVER;

        case 2:
            return RCSResourceObject::SetRequestHandlerPolicy::ACCEPTANCE;
    }
}

int convertAutoNotifyPolicyToInt(RCSResourceObject::AutoNotifyPolicy policy)
{
    switch (policy)
    {
        case RCSResourceObject::AutoNotifyPolicy::NEVER:
            return 0;

        case  RCSResourceObject::AutoNotifyPolicy::ALWAYS:
            return 1;

        case  RCSResourceObject::AutoNotifyPolicy::UPDATED:
            return 2;
    }
    return 0;
}

int convertSetRequestHandlerPolicyToInt(RCSResourceObject::SetRequestHandlerPolicy policy)
{
    switch (policy)
    {
        case  RCSResourceObject::SetRequestHandlerPolicy::NEVER:
            return 0;

        case  RCSResourceObject::SetRequestHandlerPolicy::ACCEPTANCE:
            return 1;
    }
    return 0;
}
