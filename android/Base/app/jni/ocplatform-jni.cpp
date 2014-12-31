//******************************************************************
//
// Copyright 2014 MediaTek All Rights Reserved.
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

#include "ocstack-jni.h"

static const char* TAG = "OCPLATFORM-JNI";

JNIEXPORT void JNICALL configure(JNIEnv *env, jobject obj, jobject jcfg)
{
    OC::PlatformConfig *cfg = getHandle<OC::PlatformConfig>(env, jcfg);

    __android_log_print(ANDROID_LOG_INFO, TAG, "service : %d\n", (int) cfg->serviceType);
    __android_log_print(ANDROID_LOG_INFO, TAG, "mode    : %d\n", (int) cfg->mode);
    __android_log_print(ANDROID_LOG_INFO, TAG, "IP      : %s\n", (cfg->ipAddress).c_str());
    __android_log_print(ANDROID_LOG_INFO, TAG, "port    : %d\n", cfg->port);
    __android_log_print(ANDROID_LOG_INFO, TAG, "QOS     : %d\n", (int) cfg->QoS);

    OC::OCPlatform::Configure(*cfg);
}


JNIEXPORT jint JNICALL findResource(JNIEnv* env, jobject obj, jstring jhost, jstring juri, jobject found)
{
	string host = env->GetStringUTFChars(jhost,0);
	string uri = env->GetStringUTFChars(juri,0);

    JNICallBackContext *callbackContext;
    string key = uri + "/FIND";
    std:map<std::string, JNICallBackContext*>::iterator iter = gJNICallBackContextList.find(key);
    if(iter == gJNICallBackContextList.end()) {
        gJNICallBackContextList[key] = new JNICallBackContext(env->NewGlobalRef(found));
        callbackContext = gJNICallBackContextList[key];
        __android_log_print(ANDROID_LOG_INFO, TAG, "Adding %s to gJNICallBackContextList", key.c_str());
    }
    else
        callbackContext = iter->second;

    __android_log_print(ANDROID_LOG_ERROR, TAG, "Calling oic base findresource()\n");
    try {
        OC::OCPlatform::findResource(host, uri,
            [callbackContext](std::shared_ptr<OC::OCResource> resource)
            {
                 JNIEnv * env;

                // Attach to JavaVM
                // double check it's all ok
                int getEnvStat = g_JavaVM->GetEnv((void **)&env, JNI_VERSION_1_6);
                if (getEnvStat == JNI_EDETACHED) {
                    __android_log_print(ANDROID_LOG_ERROR, TAG, "GetEnv: not attached");
                    if (g_JavaVM->AttachCurrentThread(&env, NULL) != 0) {
                    __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to attach");
                    }
                    else
                        __android_log_print(ANDROID_LOG_INFO, TAG, "Attached OK");
                } else if (getEnvStat == JNI_OK) {
                //
                } else if (getEnvStat == JNI_EVERSION) {
                    __android_log_print(ANDROID_LOG_ERROR, TAG, "GetEnv: version not supported");
                }

                // save the C++ resource ptr now
                jmethodID resource_constructor = env->GetMethodID(g_ocresource_clazz, "<init>", "(J)V");

                callbackContext->m_resource = resource;
                jobject jresource = env->NewObject(g_ocresource_clazz, resource_constructor, \
                                        (jlong) reinterpret_cast<jlong>(&callbackContext->m_resource));

                if(jresource == NULL) {
                    __android_log_print(ANDROID_LOG_ERROR, TAG, "cannot create OCResource class");
                    return;
                }

                jclass clazz = env->GetObjectClass(callbackContext->m_callBackFunction);
                if (clazz == NULL) {
                    __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to find class");
                }

                // Find Callback function
                jmethodID mid = env->GetMethodID(clazz, "Callback", \
                        "(Lorg/iotivity/base/OCResource;)V");
                if(mid == NULL) {
                    __android_log_print(ANDROID_LOG_ERROR, TAG, "FoundResource.Callback() is not defined in JAVA");
                    return;
                }

                __android_log_print(ANDROID_LOG_INFO, TAG, "calling JAVA FindCallback");
                try {
                    env->CallVoidMethod(callbackContext->m_callBackFunction, mid, jresource);
                } catch(OC::OCException& e) {
                    __android_log_print(ANDROID_LOG_ERROR, TAG, "callbackContext() exception : %s", e.reason(e).c_str());
                }

                __android_log_print(ANDROID_LOG_INFO, TAG, "fineResourceCB detach");
                env->DeleteLocalRef(jresource);
                g_JavaVM->DetachCurrentThread();

            }
        );
    }
    catch(OC::OCException& e) {
        __android_log_print(ANDROID_LOG_ERROR, TAG, "FindCB() exception : %s", e.reason(e).c_str());
    }
    __android_log_print(ANDROID_LOG_ERROR, TAG, "Called oic base findresource()\n");

    return 0;
}

