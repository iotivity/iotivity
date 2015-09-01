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

#include "jni_re_rcs_resource_container.h"
#include "jni_re_utils.h"
#include "RCSResourceContainer.h"
#include "RCSBundleInfo.h"

using namespace OIC::Service;

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeStartContainer
(JNIEnv *env, jobject jObject, jstring configFile)
{
    LOGD("RCSResourceContainer_nativeStartContainer");
    RCSResourceContainer *container = RCSResourceContainer::getInstance();
    std::string nativeFilePath = env->GetStringUTFChars(configFile, NULL);
    container->startContainer(nativeFilePath);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeStopContainer
(JNIEnv *env, jobject jObject)
{
    LOGD("RCSResourceContainer_nativeStopContainers");
    RCSResourceContainer *container = RCSResourceContainer::getInstance();
    container->stopContainer();
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeAddBundle
(JNIEnv *env, jobject jObject, jstring jBundleId, jstring jBundleUri, jstring jBundlePath,
 jobject jParams)
{
    LOGD("RCSResourceContainer_nativeAddBundle");
    std::string bundleId = env->GetStringUTFChars(jBundleId, NULL);
    std::string bundleUri = env->GetStringUTFChars(jBundleUri, NULL);
    std::string bundlePath = env->GetStringUTFChars(jBundlePath, NULL);

    std::map<std::string, std::string> params;
    //convert java map to native param map
    convertJavaMapToParamsMap(env, jParams, params);

    RCSResourceContainer *container = RCSResourceContainer::getInstance();
    container->addBundle(bundleId, bundleUri, bundlePath, params);
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeListBundles
(JNIEnv *env, jobject jObject)
{
    LOGD("RCSResourceContainer_nativeListBundles");

    RCSResourceContainer *container = RCSResourceContainer::getInstance();
    std::list<RCSBundleInfo *> nativeBundleInfoList = container->listBundles();

    if (nativeBundleInfoList.size() == 0)
    {
        LOGD("RCSResourceContainer_nativeListBundles size of bundle list is zerooooo");
    }
    else
    {
        LOGD("RCSResourceContainer_nativeListBundles size of bundle list is not zerooooo");
    }
    //conversion from native list bundle to java list bundle...
    jobject jList = env->NewObject(g_cls_LinkedList, g_mid_LinkedList_ctor);
    if (!jList) return nullptr;

    std::list<RCSBundleInfo *>::iterator bundleIt;

    for (bundleIt = nativeBundleInfoList.begin(); bundleIt != nativeBundleInfoList.end(); bundleIt++)
    {
        jlong handle = reinterpret_cast<jlong>(*bundleIt);

        //set native handle
        jclass clazz = env->FindClass("org/iotivity/ResourceEncapsulation/server/RCSBundleInfo");
        jfieldID fid =  env->GetFieldID(clazz, "m_nativeHandle", "J");

        //create java RCSBundleInfo Object
        jobject resourceObj = (jobject) env->NewObject(g_cls_RCSBundleInfo, g_mid_RCSBundleInfo_ctor,
                              handle);

        env->CallBooleanMethod(jList, g_mid_LinkedList_add_object, resourceObj);

        if (env->ExceptionCheck()) return nullptr;
        env->DeleteLocalRef(resourceObj);
    }
    return jList ;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeRemoveBundle
(JNIEnv *env, jobject jObject, jstring jBundleId)
{
    LOGD("RCSResourceContainer_nativeRemoveBundle");
    std::string bundleId = env->GetStringUTFChars(jBundleId, NULL);
    RCSResourceContainer *container = RCSResourceContainer::getInstance();
    container->removeBundle(bundleId);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeStartBundle
(JNIEnv *env, jobject jObject, jstring jBundleId)
{
    LOGD("RCSResourceContainer_nativeStartBundle");
    std::string bundleId = env->GetStringUTFChars(jBundleId, NULL);
    RCSResourceContainer *container = RCSResourceContainer::getInstance();
    container->startBundle(bundleId);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeStopBundle
(JNIEnv *env, jobject jObject, jstring jBundleId)
{
    LOGD("RCSResourceContainer_nativeStopBundle");
    std::string bundleId = env->GetStringUTFChars(jBundleId, NULL);
    RCSResourceContainer *container = RCSResourceContainer::getInstance();
    container->stopBundle(bundleId);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeAddResourceConfig
(JNIEnv *env, jobject jObject, jstring jBundleId, jstring jResourceUri, jobject jParams)
{
    LOGD("RCSResourceContainer_nativeAddResourceConfig");
    std::string bundleId = env->GetStringUTFChars(jBundleId, NULL);
    std::string resourceUri = env->GetStringUTFChars(jResourceUri, NULL);

    std::map<std::string, std::string> params;
    //convert java map to native param map
    convertJavaMapToParamsMap(env, jParams, params);

    RCSResourceContainer *container = RCSResourceContainer::getInstance();
    container->addResourceConfig(bundleId, resourceUri, params);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeRemoveResourceConfig
(JNIEnv *env, jobject jObject, jstring jBundleId, jstring jResourceUri)
{
    LOGD("RCSResourceContainer_nativeRemoveResourceConfig");
    std::string bundleId = env->GetStringUTFChars(jBundleId, NULL);
    std::string resourceUri = env->GetStringUTFChars(jResourceUri, NULL);
    RCSResourceContainer *container = RCSResourceContainer::getInstance();
    container->removeResourceConfig(bundleId, resourceUri);
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeListBundleResources
(JNIEnv *env, jobject jObject, jstring jBundleId)
{
    LOGD("RCSResourceContainer_nativeListBundleResources");
    std::string bundleId = env->GetStringUTFChars(jBundleId, NULL);
    RCSResourceContainer *container = RCSResourceContainer::getInstance();
    std::list<std::string> nativeList = container->listBundleResources(bundleId);
    return convertStrListToJavaStrList(env, nativeList);
}

