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

#include "jni_re_rcs_bundle_info.h"
#include "jni_re_jvm.h"
#include "jni_re_utils.h"

using namespace OIC::Service;

RCSBundleInfo *JniBundleInfo::getRCSBundleInfoPtr(JNIEnv *env, jobject interfaceObject)
{
    RCSBundleInfo *rep = REGetHandle<RCSBundleInfo>(env, interfaceObject);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to get native handle from RCSBundleInfo");
    }
    if (!rep)
    {
        throwRCSException(env, "JNI no native pointer");
    }
    return rep;
}

JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeGetID
(JNIEnv *env, jobject jObject)
{
    LOGI("RCSBundleInfo_nativeGetID");
    RCSBundleInfo *bundleInfo = JniBundleInfo::getRCSBundleInfoPtr(env, jObject);
    std::string nativeId = bundleInfo->getID();
    return env->NewStringUTF(nativeId.c_str());
}

JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeGetPath
(JNIEnv *env, jobject jObject)
{
    LOGI("RCSBundleInfo_nativeGetPath");
    RCSBundleInfo *bundleInfo = JniBundleInfo::getRCSBundleInfoPtr(env, jObject);
    std::string nativePath = bundleInfo->getPath();
    return env->NewStringUTF(nativePath.c_str());
}

JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeGetActivatorName
(JNIEnv *env, jobject jObject)
{
    LOGI("RCSBundleInfo_nativeGetActivatorName");
    RCSBundleInfo *bundleInfo = JniBundleInfo::getRCSBundleInfoPtr(env, jObject);
    std::string nativeActivatorName = bundleInfo->getActivatorName();
    return env->NewStringUTF(nativeActivatorName.c_str());
}

JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeGetLibraryPath
(JNIEnv *env, jobject jObject)
{
    LOGI("RCSBundleInfo_nativeGetLibraryPath");
    RCSBundleInfo *bundleInfo = JniBundleInfo::getRCSBundleInfoPtr(env, jObject);
    std::string nativeGetLibraryPath = bundleInfo->getLibraryPath();
    return env->NewStringUTF(nativeGetLibraryPath.c_str());
}

JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeGetVersion
(JNIEnv *env, jobject jObject)
{
    LOGI("RCSBundleInfo_nativeGetVersion");
    RCSBundleInfo *bundleInfo = JniBundleInfo::getRCSBundleInfoPtr(env, jObject);
    std::string nativeGetVersion = bundleInfo->getVersion();
    return env->NewStringUTF(nativeGetVersion.c_str());
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeSetID
(JNIEnv *env, jobject jObject, jstring jId)
{
    LOGI("RCSBundleInfo_nativeSetID");
    std::string id = env->GetStringUTFChars(jId, NULL);
    RCSBundleInfo *bundleInfo = JniBundleInfo::getRCSBundleInfoPtr(env, jObject);
    bundleInfo->setID(id);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeSetPath
(JNIEnv *env, jobject jObject, jstring jPath)
{
    LOGI("RCSBundleInfo_nativeSetPath");
    std::string path = env->GetStringUTFChars(jPath, NULL);
    RCSBundleInfo *bundleInfo = JniBundleInfo::getRCSBundleInfoPtr(env, jObject);
    bundleInfo->setPath(path);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeSetActivatorName
(JNIEnv *env, jobject jObject, jstring jActivatorName)
{
    LOGI("RCSBundleInfo_nativeSetActivatorName");
    std::string activatorName = env->GetStringUTFChars(jActivatorName, NULL);
    RCSBundleInfo *bundleInfo = JniBundleInfo::getRCSBundleInfoPtr(env, jObject);
    bundleInfo->setActivatorName(activatorName);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeSetLibraryPath
(JNIEnv *env, jobject jObject, jstring jLibraryPath)
{
    LOGI("RCSBundleInfo_nativeSetLibraryPath");
    std::string libraryPath = env->GetStringUTFChars(jLibraryPath, NULL);
    RCSBundleInfo *bundleInfo = JniBundleInfo::getRCSBundleInfoPtr(env, jObject);
    bundleInfo->setLibraryPath(libraryPath);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeSetVersion
(JNIEnv *env, jobject jObject, jstring jVersion)
{
    LOGI("RCSBundleInfo_nativeSetVersion");
    std::string version = env->GetStringUTFChars(jVersion, NULL);
    RCSBundleInfo *bundleInfo = JniBundleInfo::getRCSBundleInfoPtr(env, jObject);
    bundleInfo->setVersion(version);
}

