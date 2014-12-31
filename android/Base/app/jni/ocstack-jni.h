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

#ifndef _Included_org_iotivity_base_ocstack
#define _Included_org_iotivity_base_ocstack

#include <jni.h>
#include <android/log.h>
#include "OCApi.h"
#include "OCPlatform.h"

class JNICallBackContext {
    public:

    std::shared_ptr<OC::OCResource> m_resource;
    jobject m_callBackFunction;

    JNICallBackContext(jobject callback) {
        m_resource = NULL;
        m_callBackFunction = callback;
    }

    JNICallBackContext(std::shared_ptr<OC::OCResource> rsrc,
        jobject callback) {
        m_resource = rsrc;
        m_callBackFunction = callback;
    }
};

extern std::map<string, JNICallBackContext*> gJNICallBackContextList;

extern JavaVM* g_JavaVM;

extern jclass g_ocplatform_clazz;
extern jclass g_ocresource_clazz;
extern jclass g_ocheaderoption_clazz;
extern jclass g_ocrepresentation_clazz;
extern jclass g_platformcfg_clazz;

template <typename T>
T *getHandle(JNIEnv *env, jobject obj)
{
    jclass cls = env->GetObjectClass(obj);
    jfieldID fid = env->GetFieldID(cls, "nativeHandle", "J");
    jlong handle = env->GetLongField(obj, fid);
    return reinterpret_cast<T *>(handle);
}

// ocplatform-jni
extern JNIEXPORT void JNICALL configure(JNIEnv *env, jobject obj, jobject jcfg);
extern JNIEXPORT jint JNICALL findResource(JNIEnv* env, jobject obj, jstring jhost, jstring juri, jobject found);

// platformcfg-jni
extern JNIEXPORT jlong JNICALL createNativeInstance
  (JNIEnv *env, jobject obj, jint jServiceType, jint jModeType, jstring jIpAddress, jint port, jint jQOS);

// ocheaderoption-jni
extern JNIEXPORT jint JNICALL getOptionID(JNIEnv *env, jobject jobj);
extern JNIEXPORT jstring JNICALL getOptionData(JNIEnv *env, jobject jobj);
extern JNIEXPORT jlong JNICALL OCHeaderOptionConstructor(JNIEnv *env, jobject jobj, jint joption, jstring jdata);

// ocrepresentation-jni
extern JNIEXPORT jlong JNICALL OCRepresentationConstructor(JNIEnv *env, jobject jobj);
extern JNIEXPORT jstring JNICALL getUri(JNIEnv *env, jobject jobj);
extern JNIEXPORT jint JNICALL getValueInt(JNIEnv *env, jobject jobj, jstring jstr);
extern JNIEXPORT jboolean JNICALL getValueBool(JNIEnv *env, jobject jobj, jstring jstr);
extern JNIEXPORT jstring JNICALL getValueString(JNIEnv *env, jobject jobj, jstring jstr);
extern JNIEXPORT void JNICALL setValueInt(JNIEnv *env, jobject jobj, jstring jstr, jint jval);
extern JNIEXPORT void JNICALL setValueBool(JNIEnv *env, jobject jobj, jstring jstr, jboolean jval);
extern JNIEXPORT void JNICALL setValueString(JNIEnv *env, jobject jobj, jstring jstr, jstring jval);
extern JNIEXPORT jboolean JNICALL hasAttribute(JNIEnv *env, jobject jobj, jstring jstr);

// ocresource-jni
extern JNIEXPORT jobject JNICALL jniOicGet(JNIEnv *env, jobject jobj, jobject jattributeHandler);
extern JNIEXPORT jobject JNICALL jniOicPut(JNIEnv *env, jobject jobj, jobject jocrepresentation, jobject jattributeHandler);
extern JNIEXPORT jobject JNICALL jniOicPost(JNIEnv *env, jobject jobj, jobject jocrepresentation, jobject jattributeHandler);
extern JNIEXPORT jobject JNICALL jniOicObserve(JNIEnv *env, jobject jobj, jint jobservetype, jobject jattributeHandler);
extern JNIEXPORT jobject JNICALL jniOicCancelObserve(JNIEnv *env, jobject jobj);
extern JNIEXPORT jstring JNICALL uri(JNIEnv *env, jobject jobj);
extern JNIEXPORT jstring JNICALL host(JNIEnv *env, jobject jobj);
extern JNIEXPORT jobjectArray JNICALL getResourceTypes(JNIEnv *env, jobject jobj);
extern JNIEXPORT jobjectArray JNICALL getResourceInterfaces(JNIEnv *env, jobject jobj);

#endif
