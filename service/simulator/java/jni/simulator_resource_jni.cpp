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

#include "simulator_resource_jni.h"
#include "simulator_resource_jni_util.h"
#include "simulator_common_jni.h"
#include "simulator_resource_model_jni.h"

extern SimulatorClassRefs gSimulatorClassRefs;

JniSimulatorResource::JniSimulatorResource(SimulatorResourcePtr &resource)
    : m_sharedResource(resource) {}

SimulatorResourcePtr JniSimulatorResource::getJniSimulatorResourcePtr(JNIEnv *env, jobject thiz)
{
    JniSimulatorResource *resource = GetHandle<JniSimulatorResource>(env, thiz);
    if (env->ExceptionCheck())
    {
        return NULL;
    }
    return resource->m_sharedResource;
}

jobject JniSimulatorResource::toJava(JNIEnv *env, jlong resource)
{
    jobject resourceObj = (jobject) env->NewObject(gSimulatorClassRefs.classSimulatorResource,
                          gSimulatorClassRefs.classSimulatorResourceCtor, resource);
    if (NULL == resourceObj)
    {
        return NULL;
    }
    return resourceObj;
}

void JniSimulatorResource::setUri(JNIEnv *env, jobject jobj, const std::string &uri)
{
    if (!env || !jobj)
        return;

    jstring jURI = env->NewStringUTF(uri.c_str());
    if (!jURI)
        return;

    env->CallVoidMethod(jobj, gSimulatorClassRefs.classSimulatorResourceSetURI, jURI);
    env->DeleteLocalRef(jURI);
}

void JniSimulatorResource::setResourceType(JNIEnv *env, jobject jobj,
        const std::string &resourceType)
{
    if (!env || !jobj)
        return;

    jstring jResourceType = env->NewStringUTF(resourceType.c_str());
    if (!jResourceType)
        return;

    env->CallVoidMethod(jobj, gSimulatorClassRefs.classSimulatorResourceSetResourceType, jResourceType);
    env->DeleteLocalRef(jResourceType);
}

void JniSimulatorResource::setResourceName(JNIEnv *env, jobject jobj, const std::string &name)
{
    if (!env || !jobj)
        return;

    jstring jName = env->NewStringUTF(name.c_str());
    if (!jName)
        return;

    env->CallVoidMethod(jobj, gSimulatorClassRefs.classSimulatorResourceSetName, jName);
    env->DeleteLocalRef(jName);
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_getModel
(JNIEnv *env, jobject object)
{
    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, object);
    if (nullptr == resource.get())
    {
        std::cout << "getModel: Resource is NULL";
        return nullptr;
    }

    SimulatorResourceModel resModel = resource->getModel();
    JniSimulatorResourceModel *model = new JniSimulatorResourceModel(resModel);
    jobject jModel = JniSimulatorResourceModel::toJava(env, reinterpret_cast<jlong>(model));
    return jModel;
}

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_updateAttributeFromAllowedValues
(JNIEnv *env, jobject object, jstring attrName, jint index)
{
    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, object);
    if (nullptr == resource.get())
    {
        std::cout << "updateAttributeFromAllowedValues: Resource is NULL";
        return;
    }

    const char *attrNamePtr = env->GetStringUTFChars(attrName, NULL);
    if (!attrNamePtr)
    {
        std::cout << "updateAttributeFromAllowedValues: Failed to convert jstring to char string!";
        return;
    }

    resource->updateAttributeFromAllowedValues(attrNamePtr, static_cast<int>(index));
    env->ReleaseStringUTFChars(attrName, attrNamePtr);
}

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_setRange
(JNIEnv *env, jobject object, jstring attrName, jint min, jint max)
{
    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, object);
    if (nullptr == resource.get())
    {
        std::cout << "setRange: Resource is NULL";
        return;
    }

    const char *attrNamePtr = env->GetStringUTFChars(attrName, NULL);
    if (!attrNamePtr)
    {
        std::cout << "setRange: Failed to convert jstring to char string!";
        return;
    }

    resource->setRange(attrNamePtr, static_cast<int>(min), static_cast<int>(max));
    env->ReleaseStringUTFChars(attrName, attrNamePtr);
}

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_setInterfaceType
(JNIEnv *env, jobject jobject, const std::string &interfaceType)
{
    jstring jInterfaceType = env->NewStringUTF(interfaceType.c_str());
    if (!jInterfaceType)
    {
        std::cout << "setInterfaceType: InterfaceType is NULL";
        return;
    }

    env->CallVoidMethod(jobject, gSimulatorClassRefs.classSimulatorResourceSetInterfaceType,
                        jInterfaceType);
    env->DeleteLocalRef(jInterfaceType);
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_addAttributeInteger
(JNIEnv *env, jobject jobject, jstring jKey, jint jValue)
{
    if (!jKey)
    {
        std::cout << "addAttributeInteger: AttributeName is Empty";
        return;
    }

    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, jobject);
    if (nullptr == resource.get())
    {
        std::cout << "addAttributeInteger: Resource is NULL";
        return;
    }

    std::string str = env->GetStringUTFChars(jKey, NULL);
    resource->addAttribute(str, static_cast<int>(jValue));
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_addAttributeDouble
(JNIEnv *env, jobject jobject, jstring jKey, jdouble jValue)
{
    if (!jKey)
    {
        std::cout << "addAttributeDouble: AttributeName is Empty";
        return;
    }

    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, jobject);
    if (nullptr == resource.get())
    {
        std::cout << "addAttributeDouble: Resource is NULL";
        return;
    }

    std::string str = env->GetStringUTFChars(jKey, NULL);
    resource->addAttribute(str, static_cast<double>(jValue));
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_addAttributeBoolean
(JNIEnv *env, jobject jobject, jstring jKey, jboolean jValue)
{
    if (!jKey)
    {
        std::cout << "addAttributeBoolean: AttributeName is Empty";
        return;
    }

    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, jobject);
    if (nullptr == resource.get())
    {
        std::cout << "addAttributeBoolean: Resource is NULL";
        return;
    }

    std::string str = env->GetStringUTFChars(jKey, NULL);
    resource->addAttribute(str, static_cast<bool>(jValue));
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_addAttributeStringN
(JNIEnv *env, jobject jobject, jstring jKey, jstring jValue)
{
    if (!jKey)
    {
        std::cout << "addAttributeStringN: AttributeName is Empty";
        return;
    }

    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, jobject);
    if (nullptr == resource.get())
    {
        std::cout << "addAttributeStringN: Resource is NULL";
        return;
    }

    std::string key = env->GetStringUTFChars(jKey, NULL);
    std::string value = env->GetStringUTFChars(jValue, NULL);

    resource->addAttribute(key, value);
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_updateAttributeInteger
(JNIEnv *env, jobject jobject, jstring jKey, jint jValue)
{
    if (!jKey)
    {
        std::cout << "updateAttributeInteger: AttributeName is Empty";
        return;
    }

    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, jobject);
    if (nullptr == resource.get())
    {
        std::cout << "updateAttributeInteger: Resource is NULL";
        return;
    }

    std::string str = env->GetStringUTFChars(jKey, NULL);
    resource->updateAttribute(str, static_cast<int>(jValue));
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_updateAttributeDouble
(JNIEnv *env, jobject jobject, jstring jKey, jdouble jValue)
{
    if (!jKey)
    {
        std::cout << "updateAttributeDouble: AttributeName is Empty";
        return;
    }

    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, jobject);
    if (nullptr == resource.get())
    {
        std::cout << "updateAttributeDouble: Resource is NULL";
        return;
    }

    std::string str = env->GetStringUTFChars(jKey, NULL);
    resource->updateAttribute(str, static_cast<double>(jValue));
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_updateAttributeBoolean
(JNIEnv *env, jobject jobject, jstring jKey, jboolean jValue)
{
    if (!jKey)
    {
        std::cout << "updateAttributeBoolean: AttributeName is Empty";
        return;
    }

    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, jobject);
    if (nullptr == resource.get())
    {
        std::cout << "updateAttributeBoolean: Resource is NULL";
        return;
    }

    std::string str = env->GetStringUTFChars(jKey, NULL);
    resource->updateAttribute(str, static_cast<bool>(jValue));
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_updateAttributeStringN
(JNIEnv *env, jobject jobject, jstring jKey, jstring jValue)
{
    if (!jKey)
    {
        std::cout << "updateAttributeStringN: AttributeName is Empty";
        return;
    }

    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, jobject);
    if (nullptr == resource.get())
    {
        std::cout << "updateAttributeStringN: Resource is NULL";
        return;
    }

    std::string key = env->GetStringUTFChars(jKey, NULL);
    std::string value = env->GetStringUTFChars(jValue, NULL);

    resource->updateAttribute(key, value);
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_setAllowedValuesInteger
(JNIEnv *env, jobject object, jstring jKey, jobject jAllowedValues)
{
    if (!jKey)
    {
        std::cout << "setAllowedValuesInteger: AttributeName is Empty";
        return;
    }

    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, object);
    if (nullptr == resource.get())
    {
        std::cout << "setAllowedValuesInteger: Resource is NULL";
        return;
    }

    std::string str = env->GetStringUTFChars(jKey, NULL);
    resource->setAllowedValues(str, convertIntegerVector(env, jAllowedValues));
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_setAllowedValuesDouble
(JNIEnv *env, jobject object, jstring jKey, jobject jAllowedValues)
{
    if (!jKey)
    {
        std::cout << "setAllowedValuesDouble: AttributeName is Empty";
        return;
    }

    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, object);
    if (nullptr == resource.get())
    {
        std::cout << "setAllowedValuesDouble: Resource is NULL";
        return;
    }

    std::string str = env->GetStringUTFChars(jKey, NULL);
    resource->setAllowedValues(str, convertDoubleVector(env, jAllowedValues));
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_setAllowedValuesStringN
(JNIEnv *env, jobject object, jstring jKey, jobject jAllowedValues)
{
    if (!jKey)
    {
        std::cout << "setAllowedValuesStringN: AttributeName is Empty";
        return;
    }

    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, object);
    if (nullptr == resource.get())
    {
        std::cout << "setAllowedValuesStringN: Resource is NULL";
        return;
    }

    std::string str = env->GetStringUTFChars(jKey, NULL);
    resource->setAllowedValues(str, convertStringVector(env, jAllowedValues));
}

JNIEXPORT jint JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_startResourceAutomation
(JNIEnv *env, jobject object)
{
    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, object);
    if (nullptr == resource.get())
    {
        return -1;
    }

    int automationId;
    if (SIMULATOR_SUCCESS != resource->startUpdateAutomation(AutomationType::NORMAL, automationId))
        return -1;

    return automationId;
}

JNIEXPORT jint JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_startAttributeAutomation
(JNIEnv *env, jobject object, jstring attrName)
{
    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, object);
    if (nullptr == resource.get())
    {
        return -1;
    }

    const char *attrNamePtr = env->GetStringUTFChars(attrName, NULL);

    int automationId = -1;
    resource->startUpdateAutomation(AutomationType::NORMAL, automationId);

    env->ReleaseStringUTFChars(attrName, attrNamePtr);
    return automationId;
}

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_startAutomation
(JNIEnv *env, jobject object, jint automationId)
{
    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, object);
    if (nullptr == resource.get())
    {
        return;
    }

    resource->stopUpdateAutomation(automationId);
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_removeAttribute
(JNIEnv *env, jobject jobject, jstring jKey)
{
    if (!jKey)
    {
        std::cout << "removeAttribute: AttributeName is Empty";
        return;
    }

    SimulatorResourcePtr resource = JniSimulatorResource::getJniSimulatorResourcePtr(env, jobject);
    if (nullptr == resource.get())
    {
        std::cout << "removeAttribute: Resource is NULL";
        return;
    }

    std::string str = env->GetStringUTFChars(jKey, NULL);
    resource->removeAttribute(str);
}

JNIEXPORT void JNICALL Java_org_iotivity_simulator_SimulatorResourceServer_dispose
(JNIEnv *env, jobject thiz)
{
    JniSimulatorResource *resource = GetHandle<JniSimulatorResource>(env, thiz);
    delete resource;
}

