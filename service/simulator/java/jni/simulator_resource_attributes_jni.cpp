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

#include "simulator_resource_attributes_jni.h"
#include "simulator_resource_model.h"
#include "simulator_common_jni.h"
#include "simulator_error_codes.h"

extern SimulatorClassRefs gSimulatorClassRefs;

jobject JniSimulatorResourceAttribute::toJava(JNIEnv *env, jlong resource)
{
    jobject resourceObj = (jobject) env->NewObject(gSimulatorClassRefs.classSimulatorResourceAttribute,
                          gSimulatorClassRefs.classSimulatorResourceAttributeCtor, resource);
    if (NULL == resourceObj)
    {
        return NULL;
    }

    return resourceObj;
}

class attribute_value_visitor : public boost::static_visitor<jobject>
{
    public:
        attribute_value_visitor(JNIEnv *env) : m_Env(env) {}

        jobject operator ()(const int &value) const
        {
            jobject result = m_Env->NewObject(gSimulatorClassRefs.classInteger,
                                              gSimulatorClassRefs.classIntegerCtor, value);
            return result;
        }

        jobject operator ()(const double &value) const
        {
            jobject result = m_Env->NewObject(gSimulatorClassRefs.classDouble,
                                              gSimulatorClassRefs.classDoubleCtor, value);
            return result;
        }

        jobject operator ()(const std::string &value) const
        {
            jstring str = m_Env->NewStringUTF(value.c_str());
            return static_cast<jobject>(str);
        }

    private:
        JNIEnv *m_Env;
};

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_create
(JNIEnv *env, jobject object, jstring attrName)
{
    if (!attrName)
    {
        std::cout << "SimulatorResourceAttribute_create: AttributeName is Empty";
        return;
    }

    const char *attrNamePtr = env->GetStringUTFChars(attrName, NULL);
    if (!attrNamePtr)
        return;

    std::string attrNameStr(attrNamePtr);
    SimulatorResourceModel::Attribute *attribute = new SimulatorResourceModel::Attribute(attrNameStr);
    SetHandle<SimulatorResourceModel::Attribute>(env, object, attribute);
    if (env->ExceptionCheck())
    {
        delete attribute;
    }

    env->ReleaseStringUTFChars(attrName, attrNamePtr);
}

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_dispose
(JNIEnv *env, jobject object)
{
    SimulatorResourceModel::Attribute *attribute = GetHandle<SimulatorResourceModel::Attribute>(env,
            object);
    delete attribute;
}

JNIEXPORT int JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_allowedValuesSize
(JNIEnv *env, jobject object)
{
    SimulatorResourceModel::Attribute *attribute = GetHandle<SimulatorResourceModel::Attribute>(env,
            object);
    if (env->ExceptionCheck() || !attribute)
    {
        return SIMULATOR_ERROR;
    }

    return attribute->getAllowedValuesSize();
}

JNIEXPORT jstring JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_valueToString
(JNIEnv *env, jobject object)
{
    SimulatorResourceModel::Attribute *attribute = GetHandle<SimulatorResourceModel::Attribute>(env,
            object);
    if (env->ExceptionCheck() || !attribute)
    {
        return NULL;
    }

    std::string str = attribute->valueToString();
    return env->NewStringUTF(str.c_str());
}

JNIEXPORT jstring JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_allowedValuesToString
(JNIEnv *env, jobject object)
{
    SimulatorResourceModel::Attribute *attribute = GetHandle<SimulatorResourceModel::Attribute>(env,
            object);
    if (env->ExceptionCheck() || !attribute)
    {
        return NULL;
    }

    std::string str = attribute->allowedValuesToString();
    return env->NewStringUTF(str.c_str());
}

JNIEXPORT jstring JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_getName
(JNIEnv *env, jobject object)
{
    SimulatorResourceModel::Attribute *attribute = GetHandle<SimulatorResourceModel::Attribute>(env,
            object);
    if (env->ExceptionCheck() || !attribute)
    {
        return NULL;
    }

    std::string str = attribute->getName();
    return env->NewStringUTF(str.c_str());
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_getValue
(JNIEnv *env, jobject object)
{
    SimulatorResourceModel::Attribute *attribute = GetHandle<SimulatorResourceModel::Attribute>(env,
            object);
    if (env->ExceptionCheck() || !attribute)
    {
        return NULL;
    }

    return boost::apply_visitor(attribute_value_visitor(env), attribute->getValue());
}