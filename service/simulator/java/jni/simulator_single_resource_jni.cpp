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

#include "simulator_resource_model_jni.h"
#include "simulator_exceptions_jni.h"
#include "simulator_utils_jni.h"
#include "jni_sharedobject_holder.h"
#include "jni_listener_holder.h"
#include "jni_string.h"
#include "jni_vector.h"

#include "simulator_single_resource.h"

#define VALIDATE_OBJECT(ENV, OBJECT) if (!OBJECT){throwBadObjectException(ENV, "No corresponsing native object!"); return;}
#define VALIDATE_OBJECT_RET(ENV, OBJECT, RET) if (!OBJECT){throwBadObjectException(ENV, "No corresponsing native object!"); return RET;}

SimulatorSingleResourceSP simulatorSingleResourceToCpp(JNIEnv *env, jobject object)
{
    JniSharedObjectHolder<SimulatorSingleResource> *jniResource =
        GetHandle<JniSharedObjectHolder<SimulatorSingleResource>>(env, object);
    if (jniResource)
        return jniResource->get();
    return nullptr;
}

static AutomationType AutomationTypeToCpp(JNIEnv *env, jobject jType)
{
    static jmethodID ordinalMID = env->GetMethodID(
                                      gSimulatorClassRefs.automationTypeCls, "ordinal", "()I");

    int ordinal = env->CallIntMethod(jType, ordinalMID);
    return AutomationType(ordinal);
}

static void onAutoUpdationComplete(jobject listener, const std::string &uri, const int id)
{
    JNIEnv *env = getEnv();
    if (!env)
        return;

    jclass listenerCls = env->GetObjectClass(listener);
    jmethodID listenerMethod = env->GetMethodID(listenerCls, "onUpdateComplete",
                               "(Ljava/lang/String;I)V");

    jstring jUri = env->NewStringUTF(uri.c_str());
    env->CallVoidMethod(listener, listenerMethod, jUri, id);
    releaseEnv();
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_server_SimulatorSingleResource_getAttribute
(JNIEnv *env, jobject object, jstring attrName)
{
    VALIDATE_INPUT_RET(env, !attrName, "Attribute name is null!", nullptr)

    SimulatorSingleResourceSP singleResource = simulatorSingleResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, singleResource, nullptr)

    JniString jniAttrName(env, attrName);
    SimulatorResourceModel::Attribute attribute;
    if (singleResource->getAttribute(jniAttrName.get(), attribute))
        return simulatorResourceAttributeToJava(env, attribute);
    return nullptr;
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorSingleResource_addAttribute
(JNIEnv *env, jobject object, jobject resAttribute)
{
    VALIDATE_INPUT(env, !resAttribute, "Resource attribute is null!")

    SimulatorSingleResourceSP singleResource = simulatorSingleResourceToCpp(env, object);
    VALIDATE_OBJECT(env, singleResource)

    try
    {
        SimulatorResourceModel::Attribute attribute;
        if (!simulatorResourceAttributeToCpp(env, resAttribute, attribute))
        {
            throwSimulatorException(env, SIMULATOR_ERROR,
                                    "Failed to covnert SimulatorResourceAttribute java object!");
            return;
        }

        singleResource->addAttribute(attribute);
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorSingleResource_updateAttribute
(JNIEnv *env, jobject object, jstring attrName, jobject attrValue)
{
    VALIDATE_INPUT(env, !attrName, "Attribute name is null!")
    VALIDATE_INPUT(env, !attrValue, "Attribute value is null!")

    SimulatorSingleResourceSP singleResource = simulatorSingleResourceToCpp(env, object);
    VALIDATE_OBJECT(env, singleResource)

    SimulatorResourceModel::ValueVariant value;
    if (!AttributeValueToCpp(env, attrValue, value))
    {
        throwSimulatorException(env, SIMULATOR_ERROR,
                                "Failed to covnert AttributeValue java object!");
        return;
    }

    SimulatorResourceModel::Attribute attribute(JniString(env, attrName).get());
    attribute.setValue(value);
    singleResource->updateAttributeValue(attribute);
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorSingleResource_removeAttribute
(JNIEnv *env, jobject object, jstring attrName)
{
    VALIDATE_INPUT(env, !attrName, "Attribute name is null!")

    SimulatorSingleResourceSP singleResource = simulatorSingleResourceToCpp(env, object);
    VALIDATE_OBJECT(env, singleResource)

    try
    {
        JniString jniAttrName(env, attrName);
        singleResource->removeAttribute(jniAttrName.get());
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
}

JNIEXPORT jint JNICALL
Java_org_oic_simulator_server_SimulatorSingleResource_startResourceUpdation
(JNIEnv *env, jobject object, jint type, jint interval, jobject listener)
{
    VALIDATE_CALLBACK_RET(env, listener, -1)

    SimulatorSingleResourceSP singleResource = simulatorSingleResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, singleResource, -1)

    jobject listenerRef = env->NewGlobalRef(listener);
    updateCompleteCallback callback =  [listenerRef](const std::string & uri, const int id)
    {
        onAutoUpdationComplete(listenerRef, uri, id);
    };

    try
    {
        int id = singleResource->startResourceUpdation((1 == type) ? AutomationType::RECURRENT :
                 AutomationType::NORMAL, interval, callback);
        return id;
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }

    return -1;
}

JNIEXPORT jint JNICALL
Java_org_oic_simulator_server_SimulatorSingleResource_startAttributeUpdation
(JNIEnv *env, jobject object, jstring attrName, jobject type, jint interval, jobject listener)
{
    VALIDATE_INPUT_RET(env, !attrName, "Attribute name is null!", -1)
    VALIDATE_CALLBACK_RET(env, listener, -1)

    SimulatorSingleResourceSP singleResource = simulatorSingleResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, singleResource, -1)

    jobject listenerRef = env->NewGlobalRef(listener);
    updateCompleteCallback callback =  [listenerRef](const std::string & uri, const int id)
    {
        onAutoUpdationComplete(listenerRef, uri, id);
    };

    JniString jniAttrName(env, attrName);
    AutomationType automationType = AutomationTypeToCpp(env, type);

    try
    {
        int id = singleResource->startAttributeUpdation(jniAttrName.get(), automationType,
                 interval, callback);
        return id;
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }

    return -1;
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorSingleResource_stopUpdation
(JNIEnv *env, jobject object, jint id)
{
    SimulatorSingleResourceSP singleResource = simulatorSingleResourceToCpp(env, object);
    VALIDATE_OBJECT(env, singleResource)

    singleResource->stopUpdation(id);
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorSingleResource_dispose
(JNIEnv *env, jobject object)
{
    JniSharedObjectHolder<SimulatorSingleResource> *resource =
        GetHandle<JniSharedObjectHolder<SimulatorSingleResource>>(env, object);
    delete resource;
}

#ifdef __cplusplus
}
#endif
