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

#include "simulator_resource_model_jni.h"
#include "simulator_device_info_jni.h"
#include "simulator_platform_info_jni.h"
#include "simulator_exceptions_jni.h"
#include "simulator_resource_utils_jni.h"
#include "simulator_utils_jni.h"
#include "jni_sharedobject_holder.h"
#include "jni_listener_holder.h"
#include "jni_string.h"

#include "simulator_manager.h"

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

void onResourceFound(jobject listener, std::shared_ptr<SimulatorRemoteResource> remoteResource)
{
    JNIEnv *env = getEnv();
    if (!env)
        return;

    jclass listenerCls = env->GetObjectClass(listener);
    jmethodID callbackMethod = env->GetMethodID(listenerCls, "onResourceFound",
                               "(Lorg/oic/simulator/client/SimulatorRemoteResource;)V");

    jobject resource = createSimulatorRemoteResource(env, remoteResource);
    if (!resource)
        return;

    env->CallVoidMethod(listener, callbackMethod, resource);
    releaseEnv();
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_SimulatorManager_createResource
(JNIEnv *env, jclass object, jstring configPath)
{
    VALIDATE_INPUT_RET(env, !configPath, "Path is null!", nullptr)

    try
    {
        JniString jniPath(env, configPath);
        SimulatorResourceSP resource = SimulatorManager::getInstance()->createResource(
                                           jniPath.get());
        return createSimulatorResource(env, resource);
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }

    return nullptr;
}

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_SimulatorManager_createResources
(JNIEnv *env, jclass object, jstring configPath, jint count)
{
    VALIDATE_INPUT_RET(env, !configPath, "Path is null!", nullptr)
    VALIDATE_INPUT_RET(env, !count || count < 0, "Invalid count value!", nullptr)

    try
    {
        JniString jniPath(env, configPath);
        std::vector<SimulatorResourceSP> resources =
            SimulatorManager::getInstance()->createResource(jniPath.get(), count);
        return createSimulatorResourceVector(env, resources);
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }

    return nullptr;
}

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_SimulatorManager_createSingleResource
(JNIEnv *env, jclass object, jstring name, jstring uri, jstring resourceType)
{
    VALIDATE_INPUT_RET(env, !name, "Name is null!", nullptr)
    VALIDATE_INPUT_RET(env, !uri, "URI is null!", nullptr)
    VALIDATE_INPUT_RET(env, !resourceType, "Resource type is null!", nullptr)

    try
    {
        JniString jniName(env, name);
        JniString jniUri(env, uri);
        JniString jniResourceType(env, resourceType);

        SimulatorSingleResourceSP resource = SimulatorManager::getInstance()->createSingleResource(
                jniName.get(), jniUri.get(), jniResourceType.get());
        return createSimulatorResource(env, std::dynamic_pointer_cast<SimulatorResource>(resource));
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }

    return nullptr;
}

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_SimulatorManager_createCollectionResource
(JNIEnv *env, jclass object, jstring name, jstring uri, jstring resourceType)
{
    VALIDATE_INPUT_RET(env, !name, "Name is null!", nullptr)
    VALIDATE_INPUT_RET(env, !uri, "URI is null!", nullptr)
    VALIDATE_INPUT_RET(env, !resourceType, "Resource type is null!", nullptr)

    try
    {
        JniString jniName(env, name);
        JniString jniUri(env, uri);
        JniString jniResourceType(env, resourceType);

        SimulatorCollectionResourceSP resource = SimulatorManager::getInstance()->createCollectionResource(
                    jniName.get(), jniUri.get(), jniResourceType.get());
        return createSimulatorResource(env, std::dynamic_pointer_cast<SimulatorResource>(resource));
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }

    return nullptr;
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_SimulatorManager_searchResource
(JNIEnv *env, jobject object, jstring resourceType, jobject listener)
{
    VALIDATE_CALLBACK(env, listener)

    ResourceFindCallback callback =  std::bind([](
                                         std::shared_ptr<SimulatorRemoteResource> resource,
                                         const std::shared_ptr<JniListenerHolder> &listenerRef)
    {
        onResourceFound(listenerRef->get(), resource);
    }, std::placeholders::_1, JniListenerHolder::create(env, listener));

    try
    {
        if (!resourceType)
        {
            SimulatorManager::getInstance()->findResource(callback);
        }
        else
        {
            JniString type(env, resourceType);
            SimulatorManager::getInstance()->findResource(type.get(), callback);
        }

    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_SimulatorManager_setDeviceInfo
(JNIEnv *env, jobject object, jstring deviceName)
{
    VALIDATE_INPUT(env, !deviceName, "Device name is null!")

    try
    {
        JniString jniDeviceName(env, deviceName);
        SimulatorManager::getInstance()->setDeviceInfo(jniDeviceName.get());
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_SimulatorManager_findDevices
(JNIEnv *env, jobject object, jobject listener)
{
    VALIDATE_CALLBACK(env, listener)

    DeviceInfoCallback callback =  std::bind([](DeviceInfo & deviceInfo,
                                   const std::shared_ptr<JniListenerHolder> &listenerRef)
    {
        onDeviceInfoReceived(listenerRef->get(), deviceInfo);
    }, std::placeholders::_1, JniListenerHolder::create(env, listener));

    try
    {
        SimulatorManager::getInstance()->getDeviceInfo(callback);
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_SimulatorManager_setPlatformInfo
(JNIEnv *env, jobject object, jobject platformInfo)
{
    VALIDATE_INPUT(env, !platformInfo, "Platform info is null!")

    try
    {
        JniPlatformInfo jniPlatformInfo(env);
        PlatformInfo info = jniPlatformInfo.toCpp(platformInfo);
        SimulatorManager::getInstance()->setPlatformInfo(info);
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_SimulatorManager_getPlatformInformation
(JNIEnv *env, jobject object, jobject listener)
{
    VALIDATE_CALLBACK(env, listener)

    PlatformInfoCallback callback =  std::bind([](PlatformInfo & platformInfo,
                                     const std::shared_ptr<JniListenerHolder> &listenerRef)
    {
        onPlatformInfoReceived(listenerRef->get(), platformInfo);
    }, std::placeholders::_1, JniListenerHolder::create(env, listener));

    try
    {
        SimulatorManager::getInstance()->getPlatformInfo(callback);
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_SimulatorManager_setLogger
(JNIEnv *env, jobject object, jobject logger)
{
    static std::shared_ptr<JNILogger> target(new JNILogger());
    target->setJavaLogger(env, logger);
    SimulatorManager::getInstance()->setLogger(target);
}

#ifdef __cplusplus
}
#endif