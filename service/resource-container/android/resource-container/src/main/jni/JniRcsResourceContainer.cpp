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

#include "JniRcsResourceContainer.h"

#include "JavaClasses.h"
#include "JavaLocalRef.h"
#include "JNIEnvWrapper.h"
#include "Log.h"
#include "Verify.h"

#include "ResourceContainerBundleAPI.h"
#include "AndroidResource.h"
#include "RCSResourceContainer.h"



#define LOG_TAG "JNI-RCSResourceContainer"

using namespace OIC::Service;

#define CLS_NAME_BUNDLE_INFO "org/iotivity/service/resourcecontainer/RcsBundleInfo"

std::map< string, BundleResource::Ptr > android_resources;

namespace
{
    jclass g_cls_RCSBundleInfo;
    jfieldID g_field_mNativeHandle;

    jmethodID g_ctor_RCSBundleInfo;

    std::map< std::string, std::string > convertJavaMapToParamsMap(JNIEnvWrapper *env,
            jobject mapObj)
    {
        EXPECT_RET_DEF(mapObj, "map is null");

        auto setObj = invoke_Map_entrySet(env, mapObj);
        auto iterObj = invoke_Set_iterator(env, setObj);

        std::map< std::string, std::string > ret;

        while (invoke_Iterator_hasNext(env, iterObj))
        {
            JavaLocalObject entryObj { env, invoke_Iterator_next(env, iterObj) };

            JavaLocalString keyObj { env,
                static_cast< jstring >(invoke_MapEntry_getKey(env, entryObj)) };

            JavaLocalString valueObj { env,
                static_cast< jstring >(invoke_MapEntry_getValue(env, entryObj)) };

            ret.emplace(toStdString(env, keyObj), toStdString(env, valueObj));
        }

        return ret;
    }

    jobject newBundleInfoObj(JNIEnvWrapper *env, const std::unique_ptr< RCSBundleInfo > &bundleInfo)
    {
        LOGD("new bundle info");
        __android_log_print(ANDROID_LOG_DEBUG, "CONTAINER", "newBundleInfoObj %s",bundleInfo->getActivatorName().c_str());
        JavaLocalString id{env, newStringObject(env, bundleInfo->getID()) };
        JavaLocalString path{env, newStringObject(env, bundleInfo->getPath()) };
        JavaLocalString activatorName{env, newStringObject(env, bundleInfo->getActivatorName()) };
        JavaLocalString libraryPath{env, newStringObject(env, bundleInfo->getLibraryPath()) };
        JavaLocalString version{env, newStringObject(env, bundleInfo->getVersion()) };

        return env->NewObject(g_cls_RCSBundleInfo, g_ctor_RCSBundleInfo,
                id.get(), path.get(), activatorName.get(), libraryPath.get(), version.get());
    }
}

void initRCSResourceContainer(JNIEnvWrapper *env)
{
    g_cls_RCSBundleInfo = env->FindClassAsGlobalRef(CLS_NAME_BUNDLE_INFO);

    g_ctor_RCSBundleInfo = env->GetConstructorID(g_cls_RCSBundleInfo, "("
            AS_SIG(CLS_NAME_STRING)
            AS_SIG(CLS_NAME_STRING)
            AS_SIG(CLS_NAME_STRING)
            AS_SIG(CLS_NAME_STRING)
            AS_SIG(CLS_NAME_STRING)
            ")V");

    auto clsAndroidBundleResource = env->FindClass(PACKAGE_NAME "/AndroidBundleResource");

    g_field_mNativeHandle = env->GetFieldID(clsAndroidBundleResource, "mNativeHandle", "J");
}

void clearRCSResourceContainer(JNIEnvWrapper *env)
{
    env->DeleteGlobalRef(g_cls_RCSBundleInfo);
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeStartContainer
(JNIEnv *env, jobject, jstring configFileObj)
{
    LOGD("nativeStartContainer");

    EXPECT(configFileObj, "ConfigFile is null.");

    auto configFile = toStdString(env, configFileObj);
    //  std::string nativeFilePath = env->GetStringUTFChars(configFile, NULL);
    VERIFY_NO_EXC(env);

    RCSResourceContainer::getInstance()->startContainer(configFile);
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeStopContainer
(JNIEnv *env, jobject)
{
    LOGD("nativeStopContainers");

    RCSResourceContainer::getInstance()->stopContainer();
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeAddBundle
(JNIEnv *env, jobject, jstring idObj, jstring uriObj, jstring pathObj, jstring activatorObj,
 jobject paramsObj)
{
    LOGD("nativeAddBundle");

    EXPECT(idObj, "BundleId is null.");
    EXPECT(pathObj, "BundlePath is null.");
    EXPECT(activatorObj, "Activator is null.");

    JNIEnvWrapper envWrapper(env);

    try
    {
        LOGD("nativeAddBundle before calling native");
        RCSResourceContainer::getInstance()->addBundle(toStdString(&envWrapper, idObj),
                toStdString(&envWrapper, uriObj), toStdString(&envWrapper, pathObj),
                toStdString(&envWrapper, activatorObj),
                convertJavaMapToParamsMap(&envWrapper, paramsObj));

        LOGD("nativeAddBundle after calling native");
    }
    catch (const JavaException &)
    {
        LOGE("Failed to add bundle.");
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeRemoveBundle
(JNIEnv *env, jobject, jstring idObj)
{
    LOGD("nativeRemoveBundle");

    EXPECT(idObj, "BundleId is null.");

    auto id = toStdString(env, idObj);
    VERIFY_NO_EXC(env);

    RCSResourceContainer::getInstance()->removeBundle(id);
}


JNIEXPORT jobject JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeListBundles
(JNIEnv *env, jobject)
{
    LOGD("nativeListBundles");

    JNIEnvWrapper envWrapper(env);

    try
    {
        auto listObj = newArrayList(&envWrapper);

        for (const auto& bundleInfo : RCSResourceContainer::getInstance()->listBundles())
        {
            JavaLocalObject bundleInfoObj{ &envWrapper, newBundleInfoObj(&envWrapper, bundleInfo) };
            invoke_Collection_add(&envWrapper, listObj, bundleInfoObj);
        }
        return listObj;
    }
    catch (const JavaException &)
    {
        LOGE("Failed to convert bundle info list.");
    }
    return nullptr;
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeStartBundle
(JNIEnv *env, jobject, jstring idObj)
{
    LOGD("nativeStartBundle2");

    EXPECT(idObj, "BundleId is null.");

    auto id = env->GetStringUTFChars(idObj, NULL);
    VERIFY_NO_EXC(env);

    RCSResourceContainer::getInstance()->startBundle(id);
}
JNICALL
JNIEXPORT void
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeStopBundle
(JNIEnv *env, jobject, jstring idObj)
{
    LOGD("nativeStopBundle");

    EXPECT(idObj, "BundleId is null.");

    auto id = env->GetStringUTFChars(idObj, NULL);
    VERIFY_NO_EXC(env);

    RCSResourceContainer::getInstance()->stopBundle(id);
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeAddResourceConfig
(JNIEnv *env, jobject, jstring idObj, jstring uriObj, jobject paramsObj)
{
    LOGD("nativeAddResourceConfig");

    EXPECT(idObj, "BundleId is null.");
    EXPECT(uriObj, "BundleUri is null.");
    EXPECT(paramsObj, "Params is null.");

    JNIEnvWrapper envWrapper(env);

    try
    {
        RCSResourceContainer::getInstance()->addResourceConfig(toStdString(&envWrapper, idObj),
                toStdString(&envWrapper, uriObj), convertJavaMapToParamsMap(&envWrapper, paramsObj));
    }
    catch (const JavaException &)
    {
        LOGE("Failed to add bundle.");
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeRemoveResourceConfig
(JNIEnv *env, jobject, jstring idObj, jstring uriObj)
{
    LOGD("nativeRemoveResourceConfig");

    EXPECT(idObj, "BundleId is null.");
    EXPECT(uriObj, "BundleUri is null.");

    auto id = toStdString(env, idObj);
    VERIFY_NO_EXC(env);
    auto uri = toStdString(env, uriObj);
    VERIFY_NO_EXC(env);

    RCSResourceContainer::getInstance()->removeResourceConfig(id, uri);
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeListBundleResources
(JNIEnv *env, jobject, jstring idObj)
{
    LOGD("nativeListBundleResources");

    EXPECT_RET_DEF(idObj, "BundleId is null.");

    JNIEnvWrapper envWrapper(env);

    try
    {
        auto id = toStdString(&envWrapper, idObj);

        auto listObj = newArrayList(&envWrapper);

        for (const auto& s : RCSResourceContainer::getInstance()->listBundleResources(id))
        {
            JavaLocalString strObj{ &envWrapper, newStringObject(&envWrapper, s) };

            invoke_Collection_add(&envWrapper, listObj, strObj);
        }

        return listObj;
    }
    catch (const JavaException &)
    {
        LOGE("Failed to convert bundle info list.");
    }

    return nullptr;
}


JNIEXPORT void JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeRegisterAndroidResource
(JNIEnv *env, jobject obj, jobject bundleResource, jobjectArray attributes, jstring bundleId,
 jstring uri, jstring resourceType, jstring res_name)
{
    LOGD("nativeRegisterAndroidResource");
    const char *str_bundleId = env->GetStringUTFChars(bundleId, 0);
    LOGD("retrieved bundle id.");
    const char *str_uri = env->GetStringUTFChars(uri, 0);
    LOGD("retrieved uri.");
    const char *str_resourceType = env->GetStringUTFChars(resourceType, 0);
    LOGD("retrieved resource type");
    const char *str_res_name = env->GetStringUTFChars(res_name, 0);
    LOGD("retrieved res name.");
    AndroidResource res;

    BundleResource::Ptr androidResource = std::make_shared< AndroidResource >
            (env, obj, bundleResource, str_bundleId, attributes);
    ResourceContainerImpl *container = ResourceContainerImpl::getImplInstance();

    androidResource->m_uri = string(str_uri, strlen(str_uri));
    androidResource->m_resourceType = string(str_resourceType, strlen(str_resourceType));
    androidResource->m_name = string(str_res_name, strlen(str_res_name));

    // link java resource instance to c++ resource instance
    env->SetLongField(bundleResource, g_field_mNativeHandle, reinterpret_cast< jlong >(androidResource.get()));

    container->registerResource(androidResource);

    android_resources[str_uri] = androidResource;
}

/*
 * Class:     org_iotivity_resourcecontainer_bundle_api_BaseActivator
 * Method:    unregisterJavaResource
 * Signature: (Lorg/iotivity/resourcecontainer/bundle/api/BundleResource;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeUnregisterAndroidResource
(JNIEnv *env, jobject obj, jobject bundleResource, jstring uri)
{
    (void)obj;
    (void)bundleResource;
    const char *str_uri = env->GetStringUTFChars(uri, 0);

    if (android_resources[str_uri] != NULL)
    {
        ResourceContainerImpl *container = ResourceContainerImpl::getImplInstance();
        container->unregisterResource(android_resources[str_uri]);
        android_resources.erase(str_uri);
    }
}

/*
 * Class:     org_iotivity_resourcecontainer_bundle_api_BaseActivator
 * Method:    getNumberOfConfiguredResources
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeGetNumberOfConfiguredResources(
    JNIEnv *env, jobject obj, jstring bundleId)
{
    (void)obj;
    const char *str_bundleId = env->GetStringUTFChars(bundleId, 0);

    ResourceContainerImpl *container = ResourceContainerImpl::getImplInstance();
    vector< resourceInfo > resourceConfig;
    container->getResourceConfiguration(str_bundleId, &resourceConfig);

    return resourceConfig.size();
}

/*
 * Class:     org_iotivity_resourcecontainer_bundle_api_BaseActivator
 * Method:    getConfiguredResourceParams
 * Signature: (Ljava/lang/String;I)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL
Java_org_iotivity_service_resourcecontainer_RcsResourceContainer_nativeGetConfiguredResourceParams(
    JNIEnv *env, jobject obj, jstring bundleId, jint resourceId)
{
    (void)obj;
    jobjectArray ret;
    const char *str_bundleId = env->GetStringUTFChars(bundleId, 0);

    ResourceContainerImpl *container = ResourceContainerImpl::getImplInstance();
    vector< resourceInfo > resourceConfig;
    container->getResourceConfiguration(str_bundleId, &resourceConfig);
    resourceInfo conf = resourceConfig[resourceId];
    ret = (jobjectArray) env->NewObjectArray(4, env->FindClass("java/lang/String"),
            env->NewStringUTF(""));

    env->SetObjectArrayElement(ret, 0, env->NewStringUTF(conf.name.c_str()));
    env->SetObjectArrayElement(ret, 1, env->NewStringUTF(conf.uri.c_str()));
    env->SetObjectArrayElement(ret, 2, env->NewStringUTF(conf.resourceType.c_str()));
    env->SetObjectArrayElement(ret, 3, env->NewStringUTF(conf.address.c_str()));
    return ret;
}

