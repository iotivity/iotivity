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
#include "simulator_common_jni.h"
#include "simulator_resource_attributes_jni.h"
#include "simulator_error_codes.h"

using namespace std;

extern SimulatorClassRefs gSimulatorClassRefs;

JniSimulatorResourceModel::JniSimulatorResourceModel(SimulatorResourceModel resourceModel)
    : m_resourceModel(resourceModel)
{}

bool JniSimulatorResourceModel::getResourceModel(JNIEnv *env, jobject thiz,
        SimulatorResourceModel &resourceModel)
{
    JniSimulatorResourceModel *resource = GetHandle<JniSimulatorResourceModel>(env, thiz);
    if (env->ExceptionCheck())
    {
        cout << "Exception while converting the nativeHandle to JniSimulatorResourceModel" << endl;
        return false;
    }
    resourceModel = resource->m_resourceModel;
    return true;
}

jobject JniSimulatorResourceModel::toJava(JNIEnv *env, jlong resource)
{
    jobject resourceObj = (jobject) env->NewObject(gSimulatorClassRefs.classSimulatorResourceModel,
                          gSimulatorClassRefs.classSimulatorResourceModelCtor, resource);
    if (!resourceObj)
    {
        return NULL;
    }
    return resourceObj;
}

static jobject createHashMap(JNIEnv *env)
{
    jobject mapobj = env->NewObject(gSimulatorClassRefs.classHashMap,
                                    gSimulatorClassRefs.classHashMapCtor);
    return mapobj;
}

static void addEntryToHashMap(JNIEnv *env, jobject mapobj, jobject key, jobject value)
{
    if (!mapobj || !key || !value)
    {
        return;
    }

    env->CallObjectMethod(mapobj, gSimulatorClassRefs.classHashMapPut, key, value);
}

JNIEXPORT jint JNICALL
Java_org_iotivity_simulator_SimulatorResourceModel_size
(JNIEnv *env, jobject thiz)
{
    SimulatorResourceModel resourceModel;
    bool result = JniSimulatorResourceModel::getResourceModel(env, thiz, resourceModel);
    if (!result)
    {
        return SIMULATOR_ERROR;
    }

    return resourceModel.size();
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_simulator_SimulatorResourceModel_getAttributes
(JNIEnv *env, jobject thiz)
{
    SimulatorResourceModel resourceModel;
    bool result = JniSimulatorResourceModel::getResourceModel(env, thiz, resourceModel);
    if (!result)
    {
        return NULL;
    }

    map<string, SimulatorResourceModel::Attribute> attributesMap = resourceModel.getAttributes();

    // Create Java HashMap object
    jobject jHashMap = NULL;
    jHashMap = createHashMap(env);
    if (!jHashMap)
    {
        return NULL;
    }

    for (auto & attributeEntry : attributesMap)
    {

        // Create JniSimulatorResourceAttribute object and put the attribute.second into it
        SimulatorResourceModel::Attribute *attribute = new SimulatorResourceModel::Attribute(
            attributeEntry.second);

        // Create a java object for SimulatorResourceAttribute
        jobject jAttribute = JniSimulatorResourceAttribute::toJava(env, reinterpret_cast<jlong>(attribute));

        // Add an entry with attribute.first and javaSimualatorResourceAttribute to the HashMap
        jstring jAttrName = env->NewStringUTF((attributeEntry.first).c_str());
        addEntryToHashMap(env, jHashMap, static_cast<jobject>(jAttrName), jAttribute);
        env->DeleteLocalRef(jAttrName);
    }
    return jHashMap;
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_simulator_SimulatorResourceModel_getAttribute
(JNIEnv *env, jobject thiz, jstring jAttrName)
{
    if (!jAttrName)
    {
        std::cout << "getAttribute: AttributeName is Empty";
        return NULL;
    }

    const char *attrName = env->GetStringUTFChars(jAttrName, NULL);
    if (!attrName)
    {
        std::cout << "getAttribute: Failed to convert jstring to char string!";
        return NULL;
    }

    SimulatorResourceModel resourceModel;
    bool result = JniSimulatorResourceModel::getResourceModel(env, thiz, resourceModel);
    if (!result)
    {
        std::cout << "getAttribute: getResourceModel failed!";
        env->ReleaseStringUTFChars(jAttrName, attrName);
        return NULL;
    }

    SimulatorResourceModel::Attribute *attribute = new SimulatorResourceModel::Attribute();
    bool found = resourceModel.getAttribute(attrName, *attribute);
    if (!found)
    {
        std::cout << "getAttribute: Attribute not found in ResourceModel!";
        env->ReleaseStringUTFChars(jAttrName, attrName);
        delete attribute;
        return NULL;
    }

    env->ReleaseStringUTFChars(jAttrName, attrName);

    // Create a java object for SimulatorResourceAttribute
    jobject jsimulatorResourceAttribute = JniSimulatorResourceAttribute::toJava(env,
                                          reinterpret_cast<jlong>(attribute));
    return jsimulatorResourceAttribute;
}

JNIEXPORT jobjectArray JNICALL
Java_org_iotivity_simulator_SimulatorResourceModel_getAllowedValues
(JNIEnv *env, jobject thiz, jstring jAttrName)
{
    if (!jAttrName)
    {
        std::cout << "getAllowedValues: AttributeName is Empty";
        return NULL;
    }

    const char *attrName = env->GetStringUTFChars(jAttrName, NULL);
    if (!attrName)
    {
        std::cout << "getAllowedValues: Failed to convert jstring to char string!";
        env->ReleaseStringUTFChars(jAttrName, attrName);
        return NULL;
    }

    SimulatorResourceModel resourceModel;
    bool result = JniSimulatorResourceModel::getResourceModel(env, thiz, resourceModel);
    if (!result)
    {
        std::cout << "getAllowedValues: getResourceModel failed!";
        env->ReleaseStringUTFChars(jAttrName, attrName);
        return NULL;
    }

    SimulatorResourceModel::Attribute *attribute = new SimulatorResourceModel::Attribute();
    bool found = resourceModel.getAttribute(attrName, *attribute);
    if (!found)
    {
        std::cout << "getAllowedValues: Attribute not found in ResourceModel!";
        env->ReleaseStringUTFChars(jAttrName, attrName);
        delete attribute;
        return NULL;
    }

    env->ReleaseStringUTFChars(jAttrName, attrName);

    std::vector<std::string> values = attribute->allowedValuesToVectorString();

    int size = attribute->getAllowedValuesSize();

    // Create a jObjectArray for AllowedValues vector.
    jobjectArray allowedValuesArr = env->NewObjectArray(size, env->FindClass("java/lang/String"),
                                    env->NewStringUTF(""));

    int i = 0;
    for (std::vector<std::string>::iterator it = values.begin(); it != values.end(); ++it, i++)
    {
        env->SetObjectArrayElement(allowedValuesArr, i, env->NewStringUTF((*it).c_str()));
    }
    return allowedValuesArr;
}

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceModel_dispose
(JNIEnv *env, jobject thiz)
{
    JniSimulatorResourceModel *resourceModel = GetHandle<JniSimulatorResourceModel>(env, thiz);
    delete resourceModel;
}

