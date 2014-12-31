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

static const char* TAG = "OCRESOURCE-JNI";

JNIEXPORT jobject JNICALL jniOicGet(JNIEnv *env, jobject jobj, jobject jattributeHandler)
{

    std::shared_ptr<OC::OCResource> *resource = getHandle<std::shared_ptr<OC::OCResource>>(env, jobj);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "get() resource = %p\n", resource);

    JNICallBackContext *callbackContext;
    string key = (*resource)->uri() + "/GET";
    std:map<std::string, JNICallBackContext*>::iterator iter = gJNICallBackContextList.find(key);
    if(iter == gJNICallBackContextList.end()) {
        gJNICallBackContextList[key] = new JNICallBackContext(*resource, env->NewGlobalRef(jattributeHandler));
        callbackContext = gJNICallBackContextList[key];
        __android_log_print(ANDROID_LOG_INFO, TAG, "Adding %s to gJNICallBackContextList", key.c_str());
    }
    else {
        iter->second->m_callBackFunction = env->NewGlobalRef(jattributeHandler);
        callbackContext = iter->second;
    }

	OC::QueryParamsMap test;
    __android_log_print(ANDROID_LOG_ERROR, TAG, "calling oic base get()\n");

    (*resource)->get(test,
        [callbackContext](const OC::HeaderOptions& headerOptions, const OC::OCRepresentation& rep, const int eCode)
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

            // save C++ ocheaderoption ptr now
            jmethodID option_constructor = env->GetMethodID(g_ocheaderoption_clazz, "<init>", "(J)V");
            int size = headerOptions.size();
            jobjectArray joptions = env->NewObjectArray(size, g_ocheaderoption_clazz, 0);
            for (int n=0; n<size;n++) {
                jobject jelem = env->NewObject(g_ocheaderoption_clazz, option_constructor, \
                                            (jlong) reinterpret_cast<jlong>(&headerOptions[n]));

                __android_log_print(ANDROID_LOG_ERROR, TAG, "jelem : %llu",  (jlong) reinterpret_cast<jlong>(&headerOptions[n]));

                env->SetObjectArrayElement(joptions, n, jelem);
                env->DeleteLocalRef(jelem);
            }

            // save C++ ocrepresentation ptr now
            jmethodID representation_constructor = env->GetMethodID(g_ocrepresentation_clazz, "<init>", "(J)V");
            jobject jrepresentation = env->NewObject(g_ocrepresentation_clazz, representation_constructor, \
                                    (jlong) reinterpret_cast<jlong>(&rep));
            if(jrepresentation == NULL) {
                __android_log_print(ANDROID_LOG_ERROR, TAG, "cannot create OCRepresentation class");
                return;
            }

            jclass cb_clazz = env->GetObjectClass(callbackContext->m_callBackFunction);
            if (cb_clazz == NULL) {
                __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to find class");
            }

            // Find Callback function
            jmethodID cb_mid = env->GetMethodID(cb_clazz, "Callback", \
                    "([Lorg/iotivity/base/OCHeaderOption;Lorg/iotivity/base/OCRepresentation;I)V");
            if(cb_mid == NULL) {
                __android_log_print(ANDROID_LOG_ERROR, TAG, "onGet.Callback() is not defined in JAVA");
                return;
            }

            __android_log_print(ANDROID_LOG_INFO, TAG, "calling JAVA GetCallback");
            env->CallVoidMethod(callbackContext->m_callBackFunction, cb_mid, joptions, jrepresentation, eCode);

            __android_log_print(ANDROID_LOG_INFO, TAG, "getCB detach");
            g_JavaVM->DetachCurrentThread();

        }
        );
    return NULL;

}

JNIEXPORT jobject JNICALL jniOicPut(JNIEnv *env, jobject jobj, jobject jocrepresentation, jobject jattributeHandler)
{
    std::shared_ptr<OC::OCResource> *resource = getHandle<std::shared_ptr<OC::OCResource>>(env, jobj);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "put1() resource = %p\n", resource);

    JNICallBackContext *callbackContext;
    string key = (*resource)->uri() + "/PUT";
    std:map<std::string, JNICallBackContext*>::iterator iter = gJNICallBackContextList.find(key);
    if(iter == gJNICallBackContextList.end()) {
        gJNICallBackContextList[key] = new JNICallBackContext(*resource, env->NewGlobalRef(jattributeHandler));
        callbackContext = gJNICallBackContextList[key];
        __android_log_print(ANDROID_LOG_INFO, TAG, "Adding %s to gJNICallBackContextList", key.c_str());
    }
    else {
        iter->second->m_callBackFunction = env->NewGlobalRef(jattributeHandler);
        callbackContext = iter->second;
    }

    OC::OCRepresentation *rep = getHandle<OC::OCRepresentation>(env, jocrepresentation);
	OC::QueryParamsMap test;
    __android_log_print(ANDROID_LOG_ERROR, TAG, "calling oic base put()\n");

    (*resource)->put(*rep, test,
        [callbackContext](const OC::HeaderOptions& headerOptions, const OC::OCRepresentation& rep, const int eCode)
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

            // save C++ ocheaderoption ptr now
            jmethodID option_constructor = env->GetMethodID(g_ocheaderoption_clazz, "<init>", "(J)V");
            int size = headerOptions.size();
            jobjectArray joptions = env->NewObjectArray(size, g_ocheaderoption_clazz, 0);
            for (int n=0; n<size;n++) {
                jobject jelem = env->NewObject(g_ocheaderoption_clazz, option_constructor, \
                                            (jlong) reinterpret_cast<jlong>(&headerOptions[n]));

                __android_log_print(ANDROID_LOG_ERROR, TAG, "jelem : %llu",  (jlong) reinterpret_cast<jlong>(&headerOptions[n]));

                env->SetObjectArrayElement(joptions, n, jelem);
                env->DeleteLocalRef(jelem);
            }

            // save C++ ocrepresentation ptr now
            jmethodID representation_constructor = env->GetMethodID(g_ocrepresentation_clazz, "<init>", "(J)V");
            jobject jrepresentation = env->NewObject(g_ocrepresentation_clazz, representation_constructor, \
                                    (jlong) reinterpret_cast<jlong>(&rep));
            if(jrepresentation == NULL) {
                __android_log_print(ANDROID_LOG_ERROR, TAG, "cannot create OCRepresentation class");
                return;
            }

            jclass cb_clazz = env->GetObjectClass(callbackContext->m_callBackFunction);
            if (cb_clazz == NULL) {
                __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to find class");
            }

            // Find Callback function
            jmethodID cb_mid = env->GetMethodID(cb_clazz, "Callback", \
                    "([Lorg/iotivity/base/OCHeaderOption;Lorg/iotivity/base/OCRepresentation;I)V");
            if(cb_mid == NULL) {
                __android_log_print(ANDROID_LOG_ERROR, TAG, "onPut.Callback() is not defined in JAVA");
                return;
            }

            __android_log_print(ANDROID_LOG_INFO, TAG, "calling JAVA PutCallback");
            env->CallVoidMethod(callbackContext->m_callBackFunction, cb_mid, joptions, jrepresentation, eCode);

            __android_log_print(ANDROID_LOG_INFO, TAG, "putCB detach");
            g_JavaVM->DetachCurrentThread();
        }
        );
    return NULL;
}

JNIEXPORT jobject JNICALL jniOicPost(JNIEnv *env, jobject jobj, jobject jocrepresentation, jobject jattributeHandler)
{
    std::shared_ptr<OC::OCResource> *resource = getHandle<std::shared_ptr<OC::OCResource>>(env, jobj);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "post1() resource = %p\n", resource);

    JNICallBackContext *callbackContext;
    string key = (*resource)->uri() + "/POST";
    std:map<std::string, JNICallBackContext*>::iterator iter = gJNICallBackContextList.find(key);
    if(iter == gJNICallBackContextList.end()) {
        gJNICallBackContextList[key] = new JNICallBackContext(*resource, env->NewGlobalRef(jattributeHandler));
        callbackContext = gJNICallBackContextList[key];
        __android_log_print(ANDROID_LOG_INFO, TAG, "Adding %s to gJNICallBackContextList", key.c_str());
    }
    else {
        iter->second->m_callBackFunction = env->NewGlobalRef(jattributeHandler);
        callbackContext = iter->second;
    }

    OC::OCRepresentation *rep = getHandle<OC::OCRepresentation>(env, jocrepresentation);
	OC::QueryParamsMap test;
    __android_log_print(ANDROID_LOG_ERROR, TAG, "calling oic base post()\n");

    (*resource)->post(*rep, test,
        [callbackContext](const OC::HeaderOptions& headerOptions, const OC::OCRepresentation& rep, const int eCode)
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

            // save C++ ocheaderoption ptr now
            jmethodID option_constructor = env->GetMethodID(g_ocheaderoption_clazz, "<init>", "(J)V");
            int size = headerOptions.size();
            jobjectArray joptions = env->NewObjectArray(size, g_ocheaderoption_clazz, 0);
            for (int n=0; n<size;n++) {
                jobject jelem = env->NewObject(g_ocheaderoption_clazz, option_constructor, \
                                            (jlong) reinterpret_cast<jlong>(&headerOptions[n]));

                __android_log_print(ANDROID_LOG_ERROR, TAG, "jelem : %llu",  (jlong) reinterpret_cast<jlong>(&headerOptions[n]));

                env->SetObjectArrayElement(joptions, n, jelem);
                env->DeleteLocalRef(jelem);
            }

            // save C++ ocrepresentation ptr now
            jmethodID representation_constructor = env->GetMethodID(g_ocrepresentation_clazz, "<init>", "(J)V");
            jobject jrepresentation = env->NewObject(g_ocrepresentation_clazz, representation_constructor, \
                                    (jlong) reinterpret_cast<jlong>(&rep));
            if(jrepresentation == NULL) {
                __android_log_print(ANDROID_LOG_ERROR, TAG, "cannot create OCRepresentation class");
                return;
            }

            jclass cb_clazz = env->GetObjectClass(callbackContext->m_callBackFunction);
            if (cb_clazz == NULL) {
                __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to find class");
            }

            // Find Callback function
            jmethodID cb_mid = env->GetMethodID(cb_clazz, "Callback", \
                    "([Lorg/iotivity/base/OCHeaderOption;Lorg/iotivity/base/OCRepresentation;I)V");
            if(cb_mid == NULL) {
                __android_log_print(ANDROID_LOG_ERROR, TAG, "onPost.Callback() is not defined in JAVA");
                return;
            }

            __android_log_print(ANDROID_LOG_INFO, TAG, "calling JAVA PostCallback");
            env->CallVoidMethod(callbackContext->m_callBackFunction, cb_mid, joptions, jrepresentation, eCode);

            __android_log_print(ANDROID_LOG_INFO, TAG, "postCB detach");
            g_JavaVM->DetachCurrentThread();

            }
    );
    return NULL;
}

JNIEXPORT jobject JNICALL jniOicObserve(JNIEnv *env, jobject jobj, jint jobservetype, jobject jattributeHandler)
{
    std::shared_ptr<OC::OCResource> *resource = getHandle<std::shared_ptr<OC::OCResource>>(env, jobj);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "observe1() resource = %p\n", resource);

    JNICallBackContext *callbackContext;
    string key = (*resource)->uri() + "/OBSERVE";
    std:map<std::string, JNICallBackContext*>::iterator iter = gJNICallBackContextList.find(key);
    if(iter == gJNICallBackContextList.end()) {
        gJNICallBackContextList[key] = new JNICallBackContext(*resource, env->NewGlobalRef(jattributeHandler));
        callbackContext = gJNICallBackContextList[key];
        __android_log_print(ANDROID_LOG_INFO, TAG, "Adding %s to gJNICallBackContextList", key.c_str());
    }
    else {
        iter->second->m_callBackFunction = env->NewGlobalRef(jattributeHandler);
        callbackContext = iter->second;
    }

	OC::QueryParamsMap test;
    __android_log_print(ANDROID_LOG_ERROR, TAG, "calling oic base observe()\n");

    (*resource)->observe((OC::ObserveType) jobservetype, test,
        [callbackContext](const OC::HeaderOptions& headerOptions, const OC::OCRepresentation& rep, const int eCode, const int seqNum)
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

            // save C++ ocheaderoption ptr now
            jmethodID option_constructor = env->GetMethodID(g_ocheaderoption_clazz, "<init>", "(J)V");
            int size = headerOptions.size();
            jobjectArray joptions = env->NewObjectArray(size, g_ocheaderoption_clazz, 0);
            for (int n=0; n<size;n++) {
                jobject jelem = env->NewObject(g_ocheaderoption_clazz, option_constructor, \
                                            (jlong) reinterpret_cast<jlong>(&headerOptions[n]));

                __android_log_print(ANDROID_LOG_ERROR, TAG, "jelem : %llu",  (jlong) reinterpret_cast<jlong>(&headerOptions[n]));

                env->SetObjectArrayElement(joptions, n, jelem);
                env->DeleteLocalRef(jelem);
            }

            // save C++ ocrepresentation ptr now
            jmethodID representation_constructor = env->GetMethodID(g_ocrepresentation_clazz, "<init>", "(J)V");
            jobject jrepresentation = env->NewObject(g_ocrepresentation_clazz, representation_constructor, \
                                    (jlong) reinterpret_cast<jlong>(&rep));
            if(jrepresentation == NULL) {
                __android_log_print(ANDROID_LOG_ERROR, TAG, "cannot create OCRepresentation class");
                return;
            }

            jclass cb_clazz = env->GetObjectClass(callbackContext->m_callBackFunction);
            if (cb_clazz == NULL) {
                __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to find class");
            }

            // Find Callback function
            jmethodID cb_mid = env->GetMethodID(cb_clazz, "Callback", \
                    "([Lorg/iotivity/base/OCHeaderOption;Lorg/iotivity/base/OCRepresentation;II)V");
            if(cb_mid == NULL) {
                __android_log_print(ANDROID_LOG_ERROR, TAG, "onObserve.Callback() is not defined in JAVA");
                return;
            }

            __android_log_print(ANDROID_LOG_INFO, TAG, "calling JAVA ObserveCallback");
            env->CallVoidMethod(callbackContext->m_callBackFunction, cb_mid, joptions, jrepresentation, eCode, seqNum);

            __android_log_print(ANDROID_LOG_INFO, TAG, "observeCB detach");
            g_JavaVM->DetachCurrentThread();

        }
    );
    return NULL;
}

JNIEXPORT jobject JNICALL jniOicCancelObserve(JNIEnv *env, jobject jobj)
{
    std::shared_ptr<OC::OCResource> *resource = getHandle<std::shared_ptr<OC::OCResource>>(env, jobj);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "cancelObserve1() resource = %p\n", resource);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "calling oic base cancelObserve()\n");

    (*resource)->cancelObserve();
    return NULL;
}

JNIEXPORT jstring JNICALL uri(JNIEnv *env, jobject jobj)
{
    __android_log_print(ANDROID_LOG_ERROR, TAG, "uri()");
    std::shared_ptr<OC::OCResource> *resource = getHandle<std::shared_ptr<OC::OCResource>>(env, jobj);
    return env->NewStringUTF((*resource)->uri().c_str());
}

JNIEXPORT jstring JNICALL host(JNIEnv *env, jobject jobj)
{
    __android_log_print(ANDROID_LOG_ERROR, TAG, "host()");
    std::shared_ptr<OC::OCResource> *resource = getHandle<std::shared_ptr<OC::OCResource>>(env, jobj);
    return env->NewStringUTF((*resource)->host().c_str());
}

JNIEXPORT jobjectArray JNICALL getResourceTypes(JNIEnv *env, jobject jobj)
{
    __android_log_print(ANDROID_LOG_ERROR, TAG, "getResourceTypes()");
    std::shared_ptr<OC::OCResource> *resource = getHandle<std::shared_ptr<OC::OCResource>>(env, jobj);

    std::vector<std::string> resourceTypes = (*resource)->getResourceTypes();

    int size = resourceTypes.size();
    jclass clazz = env->FindClass("java/lang/String");
    jobjectArray jresourceTypes = env->NewObjectArray(size, clazz, 0);

    for (int n=0; n<size;n++) {
        jstring jstr = env->NewStringUTF(resourceTypes[n].c_str());
        env->SetObjectArrayElement(jresourceTypes, n, jstr);
    }

    return jresourceTypes;
}

JNIEXPORT jobjectArray JNICALL getResourceInterfaces(JNIEnv *env, jobject jobj)
{
    __android_log_print(ANDROID_LOG_ERROR, TAG, "getResourceInterfaces()");
    std::shared_ptr<OC::OCResource> *resource = getHandle<std::shared_ptr<OC::OCResource>>(env, jobj);

    std::vector<std::string> resourceInterfaces = (*resource)->getResourceInterfaces();

    int size = resourceInterfaces.size();
    jclass clazz = env->FindClass("java/lang/String");
    jobjectArray jresourceInterfaces = env->NewObjectArray(size, clazz, 0);

    for (int n=0; n<size;n++) {
        jstring jstr = env->NewStringUTF(resourceInterfaces[n].c_str());
        env->SetObjectArrayElement(jresourceInterfaces, n, jstr);
    }

    return jresourceInterfaces;
}

