//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

/// @file PluginManager.cpp

/// @brief

#include <stdio.h>
#include <string>

#include "PluginManager.h"

/*
 * Class:     org_iotivity_service_ppm_PluginManager
 * Method:    startPlugins
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_iotivity_service_ppm_PluginManager_jniStartPlugins(JNIEnv *env, jobject jobj,
        jstring jkey, jstring jvalue)
{
    LOGD("jniStartPlugins() Called.");

    if((!jkey)||(!jvalue))
        return 0;
    std::string ckey = env->GetStringUTFChars(jkey, 0);
    std::string cvalue = env->GetStringUTFChars(jvalue, 0);

    return (jint)pluginManagerImpl->startPlugins(ckey, cvalue);
}

/*
 * Class:     org_iotivity_service_ppm_PluginManager
 * Method:    stopPlugins
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_iotivity_service_ppm_PluginManager_jniStopPlugins(JNIEnv *env, jobject jobj,
        jstring jkey, jstring jvalue)
{
    LOGD("jniStopPlugins() Called.");

    if((!jkey)||(!jvalue))
        return 0;
    std::string ckey = env->GetStringUTFChars(jkey, 0);
    std::string cvalue = env->GetStringUTFChars(jvalue, 0);

    return (jint)pluginManagerImpl->stopPlugins(ckey, cvalue);
}

/*
 * Class:     org_iotivity_service_ppm_PluginManager
 * Method:    jniRescanPlugin
 * Signature: (V)I
 */
JNIEXPORT jint JNICALL Java_org_iotivity_service_ppm_PluginManager_jniRescanPlugin(JNIEnv *env, jobject jobj)
{
    LOGD("jniRescanPlugin() Called.");

    return (jint)pluginManagerImpl->rescanPlugin();
}


/*
 * Class:     org_iotivity_service_ppm_PluginManager
 * Method:    getPlugins
 * Signature: (V)[Lorg/iotivity/service/ppm/Plugin;
 */
JNIEXPORT jobjectArray JNICALL Java_org_iotivity_service_ppm_PluginManager_jniGetPlugins(JNIEnv *env,
        jobject jobj)
{
    LOGD("jniGetPlugins() Called.");

    std::vector<OIC::Plugin> result = pluginManagerImpl->getAllPlugins();

    jclass cls = env->FindClass("org/iotivity/service/ppm/Plugin");
    jmethodID mid = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    jobjectArray resultArray = (jobjectArray)env->NewObjectArray(result.size(), cls, NULL);

    for (int i = 0; i < result.size(); i++)
    {
        jstring jname = env->NewStringUTF(result[i].getName().c_str());
        LOGD("jniGetPlugins() [%d] Name: %s", i + 1, result[i].getName().c_str());
        jstring jplugID = env->NewStringUTF(result[i].getID().c_str());
        LOGD("jniGetPlugins() [%d] Version: %s", i + 1, result[i].getID().c_str());
        env->SetObjectArrayElement(resultArray, i, env->NewObject(cls, mid, jname, jplugID));
    }

    return resultArray;
}


/*
 * Class:     org_iotivity_service_ppm_PluginManager
 * Method:    getState
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_org_iotivity_service_ppm_PluginManager_jniGetState(JNIEnv *env, jobject jobj,
        jstring jplugID)
{
    LOGD("jniGetState() Called.");

    if (!jplugID)
        return env->NewStringUTF("NULL_INPUT");
    std::string cplugID = env->GetStringUTFChars(jplugID, 0);
    std::string result = pluginManagerImpl->getState(cplugID);

    return env->NewStringUTF(result.c_str());
}

/*
 * Class:     org_iotivity_service_ppm_PluginManager
 * Method:    JNI_OnLoad
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jjvm, void *reserved)
{
    LOGD("JNI_OnLoad() Called.");

    JNIEnv *env;

    if (jjvm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK)
        return JNI_ERR;

    jvm = jjvm;
    pluginManagerImpl = new OIC::PluginManagerImpl((void *)jvm);

    return JNI_VERSION_1_6;
}

/*
 * Class:     org_iotivity_service_ppm_PluginManager
 * Method:    JNI_OnUnload
 */
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *jjvm, void *reserved)
{
    LOGD("JNI_OnUnload() Called.");

    JNIEnv *env;

    if (jjvm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK)
        return ;

    jvm = NULL;

    if (pluginManagerImpl)
        delete pluginManagerImpl;
}
