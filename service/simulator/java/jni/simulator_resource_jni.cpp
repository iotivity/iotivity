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
#include "simulator_exceptions_jni.h"
#include "simulator_utils_jni.h"
#include "jni_sharedobject_holder.h"
#include "jni_listener_holder.h"
#include "jni_string.h"
#include "jni_vector.h"

#include "simulator_resource.h"

#define VALIDATE_OBJECT(ENV, OBJECT) if (!OBJECT){throwBadObjectException(ENV, "No corresponsing native object!"); return;}
#define VALIDATE_OBJECT_RET(ENV, OBJECT, RET) if (!OBJECT){throwBadObjectException(ENV, "No corresponsing native object!"); return RET;}

SimulatorResourceSP SimulatorResourceToCpp(JNIEnv *env, jobject object)
{
    JniSharedObjectHolder<SimulatorResource> *jniResource =
        GetHandle<JniSharedObjectHolder<SimulatorResource>>(env, object);
    if (jniResource)
        return jniResource->get();
    return nullptr;
}

static jobject resourceTypeToJava(JNIEnv *env, SimulatorResource::Type type)
{
    static jfieldID single = env->GetStaticFieldID(gSimulatorClassRefs.simulatorResourceTypeCls,
                             "SINGLE", "Lorg/oic/simulator/server/SimulatorResource$Type;");
    static jfieldID collection = env->GetStaticFieldID(gSimulatorClassRefs.simulatorResourceTypeCls,
                                 "COLLECTION", "Lorg/oic/simulator/server/SimulatorResource$Type;");

    switch (type)
    {
        case SimulatorResource::Type::SINGLE_RESOURCE:
            return env->GetStaticObjectField(gSimulatorClassRefs.simulatorResourceTypeCls, single);
        case SimulatorResource::Type::COLLECTION_RESOURCE:
            return env->GetStaticObjectField(gSimulatorClassRefs.simulatorResourceTypeCls, collection);
    }

    return nullptr;
}

static jobject createObserverInfo(JNIEnv *env, const ObserverInfo &observerInfo)
{
    static jmethodID observerInfoCtor = env->GetMethodID(gSimulatorClassRefs.observerCls,
                                        "<init>", "(ILjava/lang/String;I)V");

    jstring address = env->NewStringUTF(observerInfo.address.c_str());
    jobject observer = (jobject) env->NewObject(gSimulatorClassRefs.observerCls,
                       observerInfoCtor, observerInfo.id, address, observerInfo.port);
    env->DeleteLocalRef(address);

    return observer;
}

static jobject createObserverInfoVector(JNIEnv *env,
                                        const std::vector<ObserverInfo> observersList)
{
    jobject vectorObject = JniVector(env).toJava();
    if (!vectorObject)
        return nullptr;

    static jmethodID addMethod = env->GetMethodID(gSimulatorClassRefs.vectorCls,
                                 "add", "(Ljava/lang/Object;)Z");

    for (auto &observer : observersList)
        env->CallBooleanMethod(vectorObject, addMethod, createObserverInfo(env, observer));

    return vectorObject;
}

static void onObserverChange(jobject listener, const std::string &uri,
                             ObservationStatus state, const ObserverInfo &observerInfo)
{
    JNIEnv *env = getEnv();
    if (!env)
        return;

    jclass listenerCls = env->GetObjectClass(listener);
    jmethodID listenerMethod = nullptr;
    if (ObservationStatus::REGISTER == state)
    {
        listenerMethod = env->GetMethodID(listenerCls, "onObserverAdded",
                                          "(Ljava/lang/String;Lorg/oic/simulator/server/Observer;)V");
    }
    else
    {
        listenerMethod = env->GetMethodID(listenerCls, "onObserverRemoved",
                                          "(Ljava/lang/String;Lorg/oic/simulator/server/Observer;)V");
    }

    jstring jUri = env->NewStringUTF(uri.c_str());
    jobject jobserver = createObserverInfo(env, observerInfo);

    env->CallVoidMethod(listener, listenerMethod, jUri, jobserver);
    releaseEnv();
}

static void onResourceModelChange(jobject listener, const std::string &uri,
                                  SimulatorResourceModel &resModel)
{
    JNIEnv *env = getEnv();
    if (!env)
        return;

    jclass listenerCls = env->GetObjectClass(listener);
    jmethodID listenerMethod = env->GetMethodID(listenerCls, "onResourceModelChanged",
                               "(Ljava/lang/String;Lorg/oic/simulator/SimulatorResourceModel;)V");

    jobject jResModel = simulatorResourceModelToJava(env, resModel);
    jstring jUri = env->NewStringUTF(uri.c_str());
    env->CallVoidMethod(listener, listenerMethod, jUri, jResModel);
    releaseEnv();
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL
Java_org_oic_simulator_server_SimulatorResource_getName
(JNIEnv *env, jobject object)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, resource, nullptr)

    return env->NewStringUTF(resource->getName().c_str());
}

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_server_SimulatorResource_getType
(JNIEnv *env, jobject object)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, resource, nullptr)

    return resourceTypeToJava(env, resource->getType());
}

JNIEXPORT jstring JNICALL
Java_org_oic_simulator_server_SimulatorResource_getURI
(JNIEnv *env, jobject object)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, resource, nullptr)

    return env->NewStringUTF(resource->getURI().c_str());
}

JNIEXPORT jstring JNICALL
Java_org_oic_simulator_server_SimulatorResource_getResourceType
(JNIEnv *env, jobject object)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, resource, nullptr)

    return env->NewStringUTF(resource->getResourceType().c_str());
}

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_server_SimulatorResource_getInterface
(JNIEnv *env, jobject object)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, resource, nullptr)

    std::vector<std::string> interfaces = resource->getInterface();
    return JniVector(env).toJava(interfaces);
}

JNIEXPORT jboolean JNICALL
Java_org_oic_simulator_server_SimulatorResource_isObservable
(JNIEnv *env, jobject object)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, resource, false)

    return resource->isObservable();
}

JNIEXPORT jboolean JNICALL
Java_org_oic_simulator_server_SimulatorResource_isStarted
(JNIEnv *env, jobject object)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, resource, false)

    return resource->isStarted();
}

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_server_SimulatorResource_getResourceModel
(JNIEnv *env, jobject object)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, resource, nullptr)

    SimulatorResourceModel resModel = resource->getResourceModel();
    return simulatorResourceModelToJava(env, resModel);
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorResource_setName
(JNIEnv *env, jobject object, jstring name)
{
    VALIDATE_INPUT(env, !name, "Name is null!")

    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        JniString jniName(env, name);
        resource->setName(jniName.get());
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorResource_setURI
(JNIEnv *env, jobject object, jstring uri)
{
    VALIDATE_INPUT(env, !uri, "Uri is null!")

    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        JniString jniUri(env, uri);
        resource->setURI(jniUri.get());
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorResource_setResourceType
(JNIEnv *env, jobject object, jstring resourceType)
{
    VALIDATE_INPUT(env, !resourceType, "Resource type is null!")

    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        JniString jniType(env, resourceType);
        resource->setResourceType(jniType.get());
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (SimulatorException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorResource_addInterface
(JNIEnv *env, jobject object, jstring interfaceType)
{
    VALIDATE_INPUT(env, !interfaceType, "Interface type is null!")

    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        JniString jniInterfaceType(env, interfaceType);
        resource->addInterface(jniInterfaceType.get());
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (NoSupportException &e)
    {
        throwNoSupportException(env, e.what());
    }
    catch (SimulatorException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorResource_setObservable
(JNIEnv *env, jobject object, jboolean state)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        resource->setObservable(static_cast<bool>(state));
    }
    catch (SimulatorException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorResource_setObserverListener
(JNIEnv *env, jobject object, jobject listener)
{
    VALIDATE_CALLBACK(env, listener)

    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    SimulatorResource::ObserverCallback callback =  std::bind([](const std::string & uri,
            ObservationStatus state, const ObserverInfo & observerInfo,
            const std::shared_ptr<JniListenerHolder> &listenerRef)
    {
        onObserverChange(listenerRef->get(), uri, state, observerInfo);
    }, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
    JniListenerHolder::create(env, listener));

    try
    {
        resource->setObserverCallback(callback);
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorResource_setResourceModelChangeListener
(JNIEnv *env, jobject object, jobject listener)
{
    VALIDATE_CALLBACK(env, listener)

    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    SimulatorResource::ResourceModelChangedCallback callback =  std::bind(
                [](const std::string & uri, SimulatorResourceModel & resModel,
                   const std::shared_ptr<JniListenerHolder> &listenerRef)
    {
        onResourceModelChange(listenerRef->get(), uri, resModel);
    }, std::placeholders::_1, std::placeholders::_2, JniListenerHolder::create(env, listener));

    try
    {
        resource->setModelChangeCallback(callback);
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorResource_start
(JNIEnv *env, jobject object)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        resource->start();
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorResource_stop
(JNIEnv *env, jobject object)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        resource->stop();
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_server_SimulatorResource_getObservers
(JNIEnv *env, jobject object)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, resource, nullptr)

    return createObserverInfoVector(env, resource->getObserversList());
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorResource_notifyObserver
(JNIEnv *env, jobject object, jint id)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        resource->notify(id);
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorResource_notifyAllObservers
(JNIEnv *env, jobject object)
{
    SimulatorResourceSP resource = SimulatorResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        resource->notifyAll();
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }
}

#ifdef __cplusplus
}
#endif