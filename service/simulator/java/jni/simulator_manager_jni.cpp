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

#include "simulator_manager_jni.h"
#include "simulator_resource_jni.h"
#include "simulator_common_jni.h"
#include "simulator_manager.h"

SimulatorClassRefs gSimulatorClassRefs;
std::mutex gEnvMutex;
JavaVM *gvm;

JNIEnv *getEnv()
{
    std::unique_lock<std::mutex> lock(gEnvMutex);
    if (nullptr == gvm)
        return NULL;

    JNIEnv *env = NULL;
    jint ret = gvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    switch (ret)
    {
        case JNI_OK:
            return env;
        case JNI_EDETACHED:
            if (0 == gvm->AttachCurrentThread((void **)&env, NULL))
                return env;
    }

    return NULL;
}

void releaseEnv()
{
    std::unique_lock<std::mutex> lock(gEnvMutex);
    if (nullptr == gvm)
        return;
    gvm->DetachCurrentThread();
}

class JNILogger : public ILogger
{
    public:
        void setJavaLogger(JNIEnv *env, jobject logger)
        {
            m_logger = env->NewWeakGlobalRef(logger);
        }

        void write(std::string time, ILogger::Level level, std::string message)
        {
            JNIEnv *env = getEnv();
            if (nullptr == env)
                return;

            jobject logger = env->NewLocalRef(m_logger);
            if (!logger)
            {
                releaseEnv();
                return;
            }

            jclass loggerCls = env->GetObjectClass(logger);
            if (!loggerCls)
            {
                releaseEnv();
                return;
            }

            jmethodID writeMId = env->GetMethodID(loggerCls, "write",
                                                  "(Ljava/lang/String;ILjava/lang/String;)V");
            if (!writeMId)
            {
                releaseEnv();
                return;
            }

            jstring msg = env->NewStringUTF(message.c_str());
            jstring timeStr = env->NewStringUTF(time.c_str());
            env->CallVoidMethod(logger, writeMId, timeStr, static_cast<jint>(level), msg);
            env->DeleteLocalRef(msg);
            env->DeleteLocalRef(timeStr);
            releaseEnv();
        }

    private:
        jweak m_logger;
};


JNIEXPORT jobject JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_createResource
(JNIEnv *env, jclass object, jstring jConfigPath)
{
    if (!jConfigPath)
        return NULL;

    const char *configPath = env->GetStringUTFChars(jConfigPath, NULL);
    if (!configPath)
        return NULL;

    std::string config(configPath);
    std::shared_ptr<SimulatorResource> resource = SimulatorManager::getInstance()->createResource(
                config, nullptr);
    if (NULL == resource.get())
        return NULL;

    JniSimulatorResource *jniSimResource = new JniSimulatorResource(resource);
    jobject jSimulatorResource = JniSimulatorResource::toJava(env,
                                 reinterpret_cast<jlong>(jniSimResource));

    // Setting the uri and resourceType
    std::string uri = resource->getURI();
    std::string resourceType = resource->getResourceType();
    std::string name = resource->getName();

    JniSimulatorResource::setUri(env, jSimulatorResource, uri);
    JniSimulatorResource::setResourceType(env, jSimulatorResource, resourceType);
    JniSimulatorResource::setResourceName(env, jSimulatorResource, name);

    return jSimulatorResource;
}

JNIEXPORT jobjectArray JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_createResources
(JNIEnv *env, jclass object, jstring jConfigPath, jint count)
{
    //TODO: Need to implement this method
    return nullptr;
}

JNIEXPORT jobjectArray JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_getResources
(JNIEnv *env, jclass object)
{
    //TODO: Need to implement this method
    return nullptr;
}

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_deleteResource
(JNIEnv *env, jclass object, jobject jResource)
{
    if (!jResource)
        return;

    std::shared_ptr<SimulatorResource> resource =
        JniSimulatorResource::getJniSimulatorResourcePtr(env, jResource);
    if (!resource)
        return;

    SimulatorManager::getInstance()->deleteResource(resource);
}


JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_deleteResources
(JNIEnv *env, jclass object, jstring resourceType)
{
    if (!resourceType)
        return;

    const char *type = env->GetStringUTFChars(resourceType, NULL);
    if (!type)
        return;

    SimulatorManager::getInstance()->deleteResources(type);
}


JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_setLogger
(JNIEnv *env, jclass object, jobject logger)
{
    static std::shared_ptr<ILogger> target(new JNILogger());
    dynamic_cast<JNILogger *>(target.get())->setJavaLogger(env, logger);
    SimulatorManager::getInstance()->setLogger(target);
}

static bool getClassRef(JNIEnv *env, const char *className, jclass &classRef)
{
    jclass localClassRef = nullptr;
    localClassRef = env->FindClass(className);
    if (!localClassRef)
        return false;

    classRef = (jclass)env->NewGlobalRef(localClassRef);
    env->DeleteLocalRef(localClassRef);
    return true;
}

#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    if (!vm)
    {
        return JNI_ERR;
    }

    JNIEnv *env = NULL;
    if (JNI_OK != vm->GetEnv((void **) &env, JNI_VERSION_1_6))
    {
        return JNI_ERR;
    }

    // Get the class references
    if (false == getClassRef(env, "java/lang/Integer", gSimulatorClassRefs.classInteger))
    {
        return JNI_ERR;
    }

    if (false == getClassRef(env, "java/lang/Double", gSimulatorClassRefs.classDouble))
    {
        return JNI_ERR;
    }

    if (false == getClassRef(env, "java/lang/String", gSimulatorClassRefs.classString))
    {
        return JNI_ERR;
    }

    if (false == getClassRef(env, "java/util/HashMap", gSimulatorClassRefs.classHashMap))
    {
        return JNI_ERR;
    }

    if (false == getClassRef(env, "java/util/Vector", gSimulatorClassRefs.classVector))
    {
        return JNI_ERR;
    }

    if (false == getClassRef(env, "org/oic/simulator/serviceprovider/SimulatorResourceServer",
                             gSimulatorClassRefs.classSimulatorResource))
    {
        return JNI_ERR;
    }

    if (false == getClassRef(env, "org/oic/simulator/serviceprovider/SimulatorResourceModel",
                             gSimulatorClassRefs.classSimulatorResourceModel))
    {
        return JNI_ERR;
    }

    if (false == getClassRef(env, "org/oic/simulator/SimulatorResourceAttribute",
                             gSimulatorClassRefs.classSimulatorResourceAttribute))
    {
        return JNI_ERR;
    }

    if (false == getClassRef(env, "org/oic/simulator/clientcontroller/SimulatorRemoteResource",
                             gSimulatorClassRefs.classSimulatorRemoteResource))
    {
        return JNI_ERR;
    }


    // Get the reference to methods
    gSimulatorClassRefs.classIntegerCtor = env->GetMethodID(gSimulatorClassRefs.classInteger, "<init>",
                                           "(I)V");
    if (!gSimulatorClassRefs.classIntegerCtor)
        return JNI_ERR;

    gSimulatorClassRefs.classDoubleCtor = env->GetMethodID(gSimulatorClassRefs.classDouble, "<init>",
                                          "(D)V");
    if (!gSimulatorClassRefs.classDoubleCtor)
        return JNI_ERR;

    gSimulatorClassRefs.classHashMapCtor = env->GetMethodID(gSimulatorClassRefs.classHashMap, "<init>",
                                           "()V");
    if (!gSimulatorClassRefs.classHashMapCtor)
        return JNI_ERR;

    gSimulatorClassRefs.classHashMapPut = env->GetMethodID(gSimulatorClassRefs.classHashMap, "put",
                                          "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    if (!gSimulatorClassRefs.classHashMapPut)
        return JNI_ERR;

    gSimulatorClassRefs.classVectorCtor = env->GetMethodID(gSimulatorClassRefs.classVector, "<init>",
                                          "()V");
    if (!gSimulatorClassRefs.classVectorCtor)
        return JNI_ERR;

    gSimulatorClassRefs.classVectorAddElement = env->GetMethodID(gSimulatorClassRefs.classVector,
            "addElement",
            "(Ljava/lang/Object;)V");
    if (!gSimulatorClassRefs.classVectorAddElement)
        return JNI_ERR;

    gSimulatorClassRefs.classSimulatorResourceCtor = env->GetMethodID(
                gSimulatorClassRefs.classSimulatorResource, "<init>", "(J)V");
    if (!gSimulatorClassRefs.classSimulatorResourceCtor)
        return JNI_ERR;

    gSimulatorClassRefs.classSimulatorResourceSetURI = env->GetMethodID(
                gSimulatorClassRefs.classSimulatorResource, "setURI", "(Ljava/lang/String;)V");
    if (!gSimulatorClassRefs.classSimulatorResourceSetURI)
        return JNI_ERR;

    gSimulatorClassRefs.classSimulatorResourceSetResourceType = env->GetMethodID(
                gSimulatorClassRefs.classSimulatorResource, "setResourceType", "(Ljava/lang/String;)V");
    if (!gSimulatorClassRefs.classSimulatorResourceSetResourceType)
        return JNI_ERR;

    gSimulatorClassRefs.classSimulatorResourceSetInterfaceType = env->GetMethodID(
                gSimulatorClassRefs.classSimulatorResource, "setInterfaceType", "(Ljava/lang/String;)V");
    if (!gSimulatorClassRefs.classSimulatorResourceSetInterfaceType)
        return JNI_ERR;

    gSimulatorClassRefs.classSimulatorResourceSetName = env->GetMethodID(
                gSimulatorClassRefs.classSimulatorResource, "setName", "(Ljava/lang/String;)V");
    if (!gSimulatorClassRefs.classSimulatorResourceSetName)
        return JNI_ERR;

    gSimulatorClassRefs.classSimulatorResourceModelCtor = env->GetMethodID(
                gSimulatorClassRefs.classSimulatorResourceModel, "<init>", "(J)V");
    if (!gSimulatorClassRefs.classSimulatorResourceModelCtor)
        return JNI_ERR;

    gSimulatorClassRefs.classSimulatorResourceAttributeCtor = env->GetMethodID(
                gSimulatorClassRefs.classSimulatorResourceAttribute, "<init>", "(J)V");
    if (!gSimulatorClassRefs.classSimulatorResourceAttributeCtor)
        return JNI_ERR;

    gvm = vm;
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
}

#ifdef __cplusplus
}
#endif