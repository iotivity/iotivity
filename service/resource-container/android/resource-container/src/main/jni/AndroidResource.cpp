//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


#include "AndroidResource.h"
#include "JniRcsResourceAttributes.h"
#include "JavaClasses.h"
#include "JavaLocalRef.h"
#include "JNIEnvWrapper.h"
#include "JniRcsValue.h"

#include <jni.h>
#include <string.h>
#include <iostream>
#include "Log.h"

#define LOG_TAG "JNI-AndroidResource"

using namespace OIC::Service;
using namespace std;

namespace
{
    jclass g_cls_RCSBundleInfo;
    jfieldID g_field_mNativeHandle;
}

void initRCSAndroidResource(JNIEnvWrapper *env)
{
    auto clsAndroidBundleResource = env->FindClass(PACKAGE_NAME "/AndroidBundleResource");

    g_field_mNativeHandle = env->GetFieldID(clsAndroidBundleResource, "mNativeHandle", "J");
}

AndroidResource::AndroidResource()
{

}

void AndroidResource::initAttributes()
{

}

AndroidResource::AndroidResource(JNIEnv *env, jobject obj, jobject bundleResource,
                                       string bundleId, jobjectArray attributes)
{
    LOGD("Creating android resource, bundleId: %s", bundleId.c_str());
    (void) obj;
    m_env = env;
    int stringCount = m_env->GetArrayLength(attributes);
    LOGD("string count is %d", stringCount);

    for (int i = 0; i < stringCount; i++)
    {
        jstring str = (jstring) m_env->GetObjectArrayElement(attributes, i);
        const char *rawString = m_env->GetStringUTFChars(str, 0);
        string s(rawString, strlen(rawString));
        BundleResource::setAttribute(s, "");
    }

    m_bundleId = bundleId;

    this->m_bundleResource = m_env->NewGlobalRef(bundleResource);

    m_bundleResourceClass = m_env->GetObjectClass(bundleResource);
    LOGD("Looking for setter.");
    m_attributeSetRequestHandler = m_env->GetMethodID(m_bundleResourceClass,
            "handleSetAttributesRequest", "(Lorg/iotivity/service/resourcecontainer/RcsResourceAttributes;)V");
    LOGD("Looking for getter.");
    m_attributeGetRequestHandler = m_env->GetMethodID(m_bundleResourceClass,
            "handleGetAttributesRequest", "()Lorg/iotivity/service/resourcecontainer/RcsResourceAttributes;");
    //LOGD("Looking for softSensorResource class.");
    //jclass androidBundleSoftSensorResourceClass = m_env->FindClass("org/iotivity/service/resourcecontainer/AndroidBundleSoftSensorResource");
    LOGD("Looking for onUpdatedInputResource.");
    jclass superclass = m_env->GetSuperclass(m_bundleResourceClass);

    jclass classClass = m_env->FindClass("java/lang/Class");

    // Find the getName() method on the class object
    jmethodID mid = env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");

    // Call the getName() to get a jstring object back
    jstring strObj = (jstring)env->CallObjectMethod(superclass, mid);

    // Now get the c string from the java jstring object
    const char* str = env->GetStringUTFChars(strObj, NULL);

    LOGD("Name of super class is %s", str);

    //check for softsensor resource
    if(strcmp("org.iotivity.service.resourcecontainer.AndroidBundleSoftSensorResource", str) == 0){
       m_onUpdatedInputResource = m_env->GetMethodID(m_bundleResourceClass, "onUpdatedInputResource", "(Ljava/lang/String;Ljava/util/Vector;)V");
       if (env->ExceptionCheck()) {
           env->ExceptionDescribe();
       }
       LOGD("Looking up vector.");
       m_vectorClazz = m_env->FindClass("java/util/Vector");
       if (env->ExceptionCheck()) {
               env->ExceptionDescribe();
       }
       LOGD("Looking up vector add method.");
       m_vectorAddMethod =  m_env->GetMethodID(m_vectorClazz, "add", "(Ljava/lang/Object;)Z");
       if (env->ExceptionCheck()) {
              env->ExceptionDescribe();
       }
    }


    LOGD("Get java vm.");
    int jvmAccess = m_env->GetJavaVM(&m_jvm);
    LOGD("JVM: %s", (jvmAccess ? "false" : "true") );


}

AndroidResource::~AndroidResource()
{

}

RCSResourceAttributes::Value AndroidResource::handleGetAttributeRequest(
        const std::string &attributeName)
{
    LOGD("handleGetAttributeRequest called2");
    LOGD("Attaching thread now");
    int attached = m_jvm->AttachCurrentThread(&m_env, NULL);
    if(attached>0)
    {
        LOGE("Failed to attach thread to JavaVM");
    }
    else{
        jstring attrName = m_env->NewStringUTF(attributeName.c_str());
        auto responseObj =  m_env->CallObjectMethod(m_bundleResource,
                m_attributeGetRequestHandler, attrName);

        if (responseObj)
        {
            LOGD("parsing attributes");
            RCSResourceAttributes attrs = toNativeAttributes(m_env, responseObj);
            LOGD("Received attributes %d", attrs.size());
        }
        /*const char *js = m_env->GetStringUTFChars(returnString, NULL);
        std::string val(js);
        RCSResourceAttributes::Value newVal = val;
        m_env->ReleaseStringUTFChars(returnString, js);
        m_jvm->DetachCurrentThread();*/
        //BundleResource::setAttribute(attributeName, newVal.toString());
    }
    return BundleResource::getAttribute(attributeName);
}

void AndroidResource::handleSetAttributeRequest(const std::string &attributeName,
                                      RCSResourceAttributes::Value &&value)
{
    jstring attrName = m_env->NewStringUTF(attributeName.c_str());
    jstring val = m_env->NewStringUTF(value.toString().c_str());

    //LOGD("handleSetAttributeRequest calling object method %d", &m_attributeSetRequestHandler);
    m_env->CallObjectMethod(m_bundleResource, m_attributeSetRequestHandler, attrName, val);
    BundleResource::setAttribute(attributeName, std::move(value));
}


void AndroidResource::handleSetAttributesRequest(RCSResourceAttributes &attrs){
    LOGD("handleSetAttributesRequest called %d", attrs.size());

    //m_env->CallObjectMethod(m_bundleResource, m_attributeSetRequestHandler, attrName, val);
    //BundleResource::setAttribute(attributeName, std::move(value));

    int attached = m_jvm->AttachCurrentThread(&m_env, NULL);
    if(attached>0)
    {
        LOGE("Failed to attach thread to JavaVM");
    }
    else{
        LOGD("Creating resource attributes for JNI.");
        auto jniRcsAttributes = newAttributesObject(m_env, attrs);
        LOGD("jobject created. calling");
        m_env->CallVoidMethod(m_bundleResource,
                m_attributeSetRequestHandler, jniRcsAttributes);
        BundleResource::setAttributes(attrs);
        m_jvm->DetachCurrentThread();
    }
}

RCSResourceAttributes & AndroidResource::handleGetAttributesRequest()
{
    LOGD("handleGetAttributesRequest");
    /*std::list<string> attrsNames = getAttributeNames();
    for(std::list<string>::iterator iterator = attrsNames.begin();
            iterator != attrsNames.end(); ++iterator )
    {
        handleGetAttributeRequest(*iterator);
    }*/
    int attached = m_jvm->AttachCurrentThread(&m_env, NULL);
    if(attached>0)
    {
        LOGE("Failed to attach thread to JavaVM");
    }
    else{
        auto responseObj =  m_env->CallObjectMethod(m_bundleResource,
                m_attributeGetRequestHandler);

        if (responseObj)
        {
            LOGD("parsing attributes");

            RCSResourceAttributes attrs = toNativeAttributes(m_env, responseObj);
            LOGD("Received attributes %d", attrs.size());
            BundleResource::setAttributes(attrs, false);
        }

        m_jvm->DetachCurrentThread();
    }
    LOGD("BundleResource::getAttributes().size() %d", BundleResource::getAttributes().size());
    return BundleResource::getAttributes();
}

void AndroidResource::executeLogic(){
    LOGD("executeLogic");

}

void AndroidResource::onUpdatedInputResource(const std::string attributeName,
        std::vector<RCSResourceAttributes::Value> values){
    LOGD("onUpdatedInputResource");

    int attached = m_jvm->AttachCurrentThread(&m_env, NULL);
    jobject valueObj;
    if(attached>0)
    {
        LOGE("Failed to attach thread to JavaVM");
    }
    else{
        jobject obj = m_env->NewObject(m_vectorClazz, m_env->GetMethodID(m_vectorClazz, "<init>", "()V"));

        for (int n=0;n<values.size();n++)
        {
           valueObj  = newRCSValueObject(m_env, values[n]);
           m_env->CallVoidMethod(obj, m_vectorAddMethod, valueObj);
        }
        m_env->CallObjectMethod(m_bundleResource,
                m_onUpdatedInputResource, obj);
        m_jvm->DetachCurrentThread();
    }
    LOGD("BundleResource::getAttributes().size() %d", BundleResource::getAttributes().size());
}

JNIEXPORT void JNICALL Java_org_iotivity_service_resourcecontainer_AndroidBundleResource_updateNativeInstance
(JNIEnv* env, jobject obj, jobject updates)
{
    LOGD("updateNativeInstance");
    BundleResource* androidResource = reinterpret_cast<BundleResource*>(env->GetLongField(obj, g_field_mNativeHandle));
    RCSResourceAttributes attrs = toNativeAttributes(env, updates);
    LOGD("Received attributes %d", attrs.size());
    androidResource->setAttributes(attrs, true);
}
