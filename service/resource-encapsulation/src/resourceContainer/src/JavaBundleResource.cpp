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


#if(JAVA_SUPPORT)
    #include "JavaBundleResource.h"
    #include <jni.h>
    #include <string.h>

    #include <iostream>

    using namespace OIC::Service;
    using namespace std;

    #include "oc_logger.hpp"

    using OC::oc_log_stream;
    using namespace OIC::Service;

    auto info_logger = []() -> boost::iostreams::stream<OC::oc_log_stream> &
    {
        static OC::oc_log_stream ols(oc_make_ostream_logger);
        static boost::iostreams::stream<OC::oc_log_stream> os(ols);
        os->set_level(OC_LOG_INFO);
        os->set_module("JavaBundleResource");
        return os;
    };

    auto error_logger = []() -> boost::iostreams::stream<OC::oc_log_stream> &
    {
        static OC::oc_log_stream ols(oc_make_ostream_logger);
        static boost::iostreams::stream<OC::oc_log_stream> os(ols);
        os->set_level(OC_LOG_ERROR);
        os->set_module("JavaBundleResource");
        return os;
    };

    JavaBundleResource::JavaBundleResource()
    {

    }

    void JavaBundleResource::initAttributes()
    {

    }

    JavaBundleResource::JavaBundleResource(JNIEnv *env, jobject obj, jobject bundleResource,
            string bundleId, jobjectArray attributes)
    {
        int stringCount = env->GetArrayLength(attributes);

        for (int i = 0; i < stringCount; i++)
        {
            jstring str = (jstring) env->GetObjectArrayElement(attributes, i);
            const char *rawString = env->GetStringUTFChars(str, 0);
            string s(rawString, strlen(rawString));
            BundleResource::setAttribute(s, "");
        }

        m_bundleId = bundleId;

        this->bundleResource = env->NewGlobalRef(bundleResource);

        bundleResourceClass = env->GetObjectClass(bundleResource);

        attributeSetter = env->GetMethodID(bundleResourceClass, "setAttribute",
                "(Ljava/lang/String;Ljava/lang/String;)V");

        attributeGetter = env->GetMethodID(bundleResourceClass, "getAttribute",
                "(Ljava/lang/String;)Ljava/lang/String;");

    }

    JavaBundleResource::~JavaBundleResource()
    {

    }

    RCSResourceAttributes& JavaBundleResource::getAttributes(){
        return BundleResource::getAttributes();
    }

    RCSResourceAttributes::Value JavaBundleResource::getAttribute(const std::string& attributeName)
    {
        JavaVM* vm = ResourceContainerImpl::getImplInstance()->getJavaVM(m_bundleId);

        JNIEnv * env;
        int envStat = vm->GetEnv((void **) &env, JNI_VERSION_1_4);

        if (envStat == JNI_EDETACHED)
        {
            if (vm->AttachCurrentThread((void**) &env, NULL) != 0)
            {
                error_logger() << "Failed to attach " << endl;
            }
        }
        else if (envStat == JNI_EVERSION)
        {
            error_logger() << "Env: version not supported " << endl;
        }

        jstring attrName = env->NewStringUTF(attributeName.c_str());

        jstring returnString = (jstring) env->CallObjectMethod(bundleResource, attributeGetter,
                attrName);

        const char *js = env->GetStringUTFChars(returnString, NULL);
        std::string val(js);
        RCSResourceAttributes::Value newVal = val;
        env->ReleaseStringUTFChars(returnString, js);
        BundleResource::setAttribute(attributeName, newVal.toString());
        return BundleResource::getAttribute(attributeName);
    }

    void JavaBundleResource::setAttribute(std::string attributeName, RCSResourceAttributes::Value&& value)
    {
        JavaVM* vm = ResourceContainerImpl::getImplInstance()->getJavaVM(m_bundleId);

        JNIEnv * env;
        int envStat = vm->GetEnv((void **) &env, JNI_VERSION_1_4);

        if (envStat == JNI_EDETACHED)
        {
            if (vm->AttachCurrentThread((void**) &env, NULL) != 0)
            {
                error_logger() << "Failed to attach " << endl;
            }
        }
        else if (envStat == JNI_EVERSION)
        {
            error_logger() << "Env: version not supported " << endl;
        }

        jstring attrName = env->NewStringUTF(attributeName.c_str());
        jstring val = env->NewStringUTF(value.toString().c_str());


        env->CallObjectMethod(bundleResource, attributeSetter, attrName, val);
        BundleResource::setAttribute(attributeName, std::move(value));
    }
#endif
