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

static const char* TAG = "OCHEADEROPTION-JNI";

JNIEXPORT jint JNICALL getOptionID(JNIEnv *env, jobject jobj)
{
    OC::HeaderOption::OCHeaderOption *option = getHandle<OC::HeaderOption::OCHeaderOption>(env, jobj);
    return option->getOptionID();
}

JNIEXPORT jstring JNICALL getOptionData(JNIEnv *env, jobject jobj)
{
    OC::HeaderOption::OCHeaderOption *option = getHandle<OC::HeaderOption::OCHeaderOption>(env, jobj);
    std::string data = option->getOptionData();
    return env->NewStringUTF(data.c_str());
}

JNIEXPORT jlong JNICALL OCHeaderOptionConstructor(JNIEnv *env, jobject jobj, jint joption, jstring jdata)
{
    OC::HeaderOption::OCHeaderOption *option = new OC::HeaderOption::OCHeaderOption(joption, env->GetStringUTFChars(jdata, 0));
    jlong instptr = reinterpret_cast<jlong>(option);
    return instptr;
}

