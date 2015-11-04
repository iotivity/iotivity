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

#include <jni.h>
#include <string.h>
#include <iostream>
#include "Log.h"

#define LOG_TAG "JNI-AndroidResource"

using namespace OIC::Service;
using namespace std;


AndroidResource::AndroidResource()
{

}

void AndroidResource::initAttributes()
{

}

AndroidResource::AndroidResource(JNIEnv *env, jobject obj, jobject bundleResource,
                                       string bundleId, jobjectArray attributes)
{
    LOGD("Creating android resource");
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
            BundleResource::setAttributes(attrs);
        }

        m_jvm->DetachCurrentThread();
    }
    LOGD("BundleResource::getAttributes().size() %d", BundleResource::getAttributes().size());
    return BundleResource::getAttributes();
}
