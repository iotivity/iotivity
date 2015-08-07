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
#include "simulator_resource_server_jni.h"
#include "simulator_common_jni.h"
#include "simulator_manager.h"
#include "simulator_remote_resource_jni.h"
#include "simulator_resource_model_jni.h"

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


jobject SimulatorRemoteResourceToJava(JNIEnv *env, jlong resource)
{
    jmethodID constructor = env->GetMethodID(gSimulatorClassRefs.classSimulatorRemoteResource, "<init>",
                            "(J)V");
    if (NULL == constructor)
    {
        return NULL;
    }

    jobject resourceObj = (jobject) env->NewObject(gSimulatorClassRefs.classSimulatorRemoteResource,
                          constructor, resource);
    if (NULL == resourceObj)
    {
        return NULL;
    }

    return resourceObj;
}

class JNIFoundResourceListener
{
    public:
        void setJavaFoundResourceListener(JNIEnv *env, jobject listener)
        {
            m_listener = env->NewWeakGlobalRef(listener);
        }

        void onFoundResource(std::shared_ptr<SimulatorRemoteResource> resource)
        {
            JNIEnv *env = getEnv();
            if (nullptr == env)
                return;

            jobject foundResourceListener = env->NewLocalRef(m_listener);
            if (!foundResourceListener)
            {
                releaseEnv();
                return;
            }

            jclass foundResourceCls = env->GetObjectClass(foundResourceListener);
            if (!foundResourceCls)
            {
                releaseEnv();
                return;
            }

            jmethodID foundResourceMId = env->GetMethodID(foundResourceCls, "onResourceCallback",
                                         "(Lorg/oic/simulator/clientcontroller/SimulatorRemoteResource;)V");
            if (!foundResourceMId)
            {
                releaseEnv();
                return;
            }

            JniSimulatorRemoteResource *jniSimulatorResource = new JniSimulatorRemoteResource(resource);

            if (!jniSimulatorResource)
            {
                releaseEnv();
                return;
            }

            jobject simulatorResource = SimulatorRemoteResourceToJava(env,
                                        reinterpret_cast<jlong>(jniSimulatorResource));

            env->CallVoidMethod(foundResourceListener, foundResourceMId, simulatorResource);
            if ((env)->ExceptionCheck())
            {
                releaseEnv();
                return;
            }

            releaseEnv();
        }

    private:
        jweak m_listener;

};

void onResourceModelChange(jweak jlistenerRef, const std::string &uri,
                           const SimulatorResourceModel &resModel)
{
    JNIEnv *env = getEnv();
    if (nullptr == env)
        return;

    jobject modelChangeListener = env->NewLocalRef(jlistenerRef);
    if (!modelChangeListener)
    {
        releaseEnv();
        return;
    }

    jclass modelChangeCls = env->GetObjectClass(modelChangeListener);
    if (!modelChangeCls)
    {
        releaseEnv();
        return;
    }

    jmethodID foundModelChangeMId = env->GetMethodID(modelChangeCls, "onResourceModelChanged",
                                    "(Ljava/lang/String;Lorg/oic/simulator/serviceprovider/SimulatorResourceModel;)V");
    if (!foundModelChangeMId)
    {
        releaseEnv();
        return;
    }

    JniSimulatorResourceModel *jniModel = new JniSimulatorResourceModel(resModel);
    if (!jniModel)
    {
        releaseEnv();
        return;
    }

    jobject jModel = JniSimulatorResourceModel::toJava(env, reinterpret_cast<jlong>(jniModel));

    jstring jUri = env->NewStringUTF(uri.c_str());

    env->CallVoidMethod(modelChangeListener, foundModelChangeMId, jUri, jModel);
    if ((env)->ExceptionCheck())
    {
        releaseEnv();
        return;
    }

    env->DeleteLocalRef(jUri);

    releaseEnv();
}


JNIEXPORT jobject JNICALL
Java_org_oic_simulator_SimulatorManagerNativeInterface_createResource
(JNIEnv *env, jclass object, jstring configPath, jobject listener)
{
    if (!configPath)
        return NULL;

    if (!listener)
        return NULL;

    jweak jlistenerRef = env->NewWeakGlobalRef(listener);
    SimulatorResourceServer::ResourceModelChangedCB callback =  [jlistenerRef](const std::string & uri,
            const SimulatorResourceModel & resModel)
    {
        onResourceModelChange(jlistenerRef, uri, resModel);
    };

    const char *configPathCStr = env->GetStringUTFChars(configPath, NULL);
    SimulatorResourceServerPtr resource = SimulatorManager::getInstance()->createResource(
            configPathCStr, callback);
    if (nullptr == resource)
    {
        if (configPathCStr)
            env->ReleaseStringUTFChars(configPath, configPathCStr);
        return NULL;
    }

    JniSimulatorResource *jniSimResource = new JniSimulatorResource(resource);
    jobject jSimulatorResource = JniSimulatorResource::toJava(env,
                                 reinterpret_cast<jlong>(jniSimResource));

    jniSimResource->setResourceInfo(env, jSimulatorResource);

    if (configPathCStr)
        env->ReleaseStringUTFChars(configPath, configPathCStr);
    return jSimulatorResource;
}

JNIEXPORT jobjectArray JNICALL
Java_org_oic_simulator_SimulatorManagerNativeInterface_createResources
(JNIEnv *env, jclass object, jstring configPath, jint count, jobject listener)
{
    if (!configPath)
        return NULL;

    if (!listener)
        return NULL;

    jweak jlistenerRef = env->NewWeakGlobalRef(listener);
    SimulatorResourceServer::ResourceModelChangedCB callback =  [jlistenerRef](const std::string & uri,
            const SimulatorResourceModel & resModel)
    {
        onResourceModelChange(jlistenerRef, uri, resModel);
    };


    const char *configPathCStr = env->GetStringUTFChars(configPath, NULL);
    std::vector<SimulatorResourceServerPtr> resources =
        SimulatorManager::getInstance()->createResource(configPathCStr, count, callback);

    // Construct the object array and send it java layer
    jobjectArray resourceArray = env->NewObjectArray(resources.size(),
                                 gSimulatorClassRefs.classSimulatorResource, NULL);
    if (resourceArray)
    {
        for (size_t i = 0; i < resources.size(); i++)
        {
            JniSimulatorResource *jniSimResource = new JniSimulatorResource(resources[i]);
            jobject jSimulatorResource = JniSimulatorResource::toJava(env,
                                         reinterpret_cast<jlong>(jniSimResource));
            jniSimResource->setResourceInfo(env, jSimulatorResource);
            env->SetObjectArrayElement(resourceArray, i, jSimulatorResource);
        }
    }

    if (configPathCStr)
        env->ReleaseStringUTFChars(configPath, configPathCStr);
    return resourceArray;
}

JNIEXPORT jobjectArray JNICALL
Java_org_oic_simulator_SimulatorManagerNativeInterface_getResources
(JNIEnv *env, jclass object)
{
    //TODO: Need to implement this method
    return nullptr;
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_SimulatorManagerNativeInterface_deleteResource
(JNIEnv *env, jclass object, jobject jResource)
{
    if (!jResource)
        return;

    SimulatorResourceServerPtr resource =
        JniSimulatorResource::getJniSimulatorResourcePtr(env, jResource);
    if (!resource)
        return;

    SimulatorManager::getInstance()->deleteResource(resource);
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_SimulatorManagerNativeInterface_deleteResources
(JNIEnv *env, jclass object, jstring resourceType)
{
    std::string type;
    const char *typeCStr = NULL;
    if (resourceType)
    {
        typeCStr = env->GetStringUTFChars(resourceType, NULL);
        type = typeCStr;
    }

    SimulatorManager::getInstance()->deleteResources(type);
    if (typeCStr)
        env->ReleaseStringUTFChars(resourceType, typeCStr);
}

JNIEXPORT jint JNICALL
Java_org_oic_simulator_SimulatorManagerNativeInterface_findResource
(JNIEnv *env, jobject object, jstring resourceType, jobject listener)
{
    const char *typeCStr = NULL;

    if (resourceType)
    {
        typeCStr = env->GetStringUTFChars(resourceType, NULL);
    }

    JNIFoundResourceListener *resourceListener = new JNIFoundResourceListener();
    resourceListener->setJavaFoundResourceListener(env, listener);

    SimulatorResult result = SimulatorManager::getInstance()->findResource(typeCStr,
                             std::bind(&JNIFoundResourceListener::onFoundResource, resourceListener, std::placeholders::_1));

    if (typeCStr)
        env->ReleaseStringUTFChars(resourceType, typeCStr);
    return result;
}

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_SimulatorManagerNativeInterface_getFoundResources
(JNIEnv *env, jobject object, jstring resourceType)
{
    const char *typeCStr = NULL;
    if (resourceType)
    {
        typeCStr = env->GetStringUTFChars(resourceType, NULL);
    }

    std::vector<SimulatorRemoteResourcePtr> resourceList;
    resourceList = SimulatorManager::getInstance()->getFoundResources(typeCStr);
    if (resourceList.empty())
    {
        if (typeCStr)
            env->ReleaseStringUTFChars(resourceType, typeCStr);
        return NULL;
    }

    jobject vectorObj = env->NewObject(gSimulatorClassRefs.classVector,
                                       gSimulatorClassRefs.classVectorCtor);
    if (!vectorObj)
    {
        if (typeCStr)
            env->ReleaseStringUTFChars(resourceType, typeCStr);
        return NULL;
    }

    // Convert to java SimulatorRemoteResource object
    for (unsigned int i = 0; i < resourceList.size(); i++)
    {
        JniSimulatorRemoteResource *jniSimulatorResource = new JniSimulatorRemoteResource(resourceList[i]);
        if (!jniSimulatorResource)
        {
            if (typeCStr)
                env->ReleaseStringUTFChars(resourceType, typeCStr);
            return NULL;
        }

        jobject resource = SimulatorRemoteResourceToJava(env,
                           reinterpret_cast<jlong>(jniSimulatorResource));
        env->CallVoidMethod(vectorObj, gSimulatorClassRefs.classVectorAddElement, resource);
    }

    if (typeCStr)
        env->ReleaseStringUTFChars(resourceType, typeCStr);
    return vectorObj;
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_SimulatorManagerNativeInterface_setLogger
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

    if (false == getClassRef(env, "java/util/Map", gSimulatorClassRefs.classMap))
    {
        return JNI_ERR;
    }

    if (false == getClassRef(env, "java/util/Map$Entry", gSimulatorClassRefs.classMapEntry))
    {
        return JNI_ERR;
    }

    if (false == getClassRef(env, "java/util/Set", gSimulatorClassRefs.classSet))
    {
        return JNI_ERR;
    }

    if (false == getClassRef(env, "java/util/Iterator", gSimulatorClassRefs.classIterator))
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

    gSimulatorClassRefs.classMapEntrySet = env->GetMethodID(
            gSimulatorClassRefs.classMap, "entrySet", "()Ljava/util/Set;");
    if (!gSimulatorClassRefs.classMapEntrySet)
        return JNI_ERR;

    gSimulatorClassRefs.classMapGetKey = env->GetMethodID(
            gSimulatorClassRefs.classMapEntry, "getKey", "()Ljava/lang/Object;");
    if (!gSimulatorClassRefs.classMapGetKey)
        return JNI_ERR;

    gSimulatorClassRefs.classMapGetValue = env->GetMethodID(
            gSimulatorClassRefs.classMapEntry, "getValue", "()Ljava/lang/Object;");
    if (!gSimulatorClassRefs.classMapGetValue)
        return JNI_ERR;

    gSimulatorClassRefs.classIteratorId = env->GetMethodID(
            gSimulatorClassRefs.classSet, "iterator", "()Ljava/util/Iterator;");
    if (!gSimulatorClassRefs.classIteratorId)
        return JNI_ERR;

    gSimulatorClassRefs.classHasNextId = env->GetMethodID(
            gSimulatorClassRefs.classIterator, "hasNext", "()Z");
    if (!gSimulatorClassRefs.classHasNextId)
        return JNI_ERR;

    gSimulatorClassRefs.classNextId = env->GetMethodID(
                                          gSimulatorClassRefs.classIterator, "next", "()Ljava/lang/Object;");
    if (!gSimulatorClassRefs.classNextId)
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

    gSimulatorClassRefs.classSimulatorResourceModelId = env->GetMethodID(
                gSimulatorClassRefs.classSimulatorResourceModel, "<init>", "(J)V");
    if (!gSimulatorClassRefs.classSimulatorResourceModelId)
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