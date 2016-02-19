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
#include "jni_queryparam.h"
#include "jni_string.h"
#include "jni_vector.h"

#include "simulator_remote_resource.h"

#define VALIDATE_OBJECT(ENV, OBJECT) if (!OBJECT){throwBadObjectException(ENV, "No corresponsing native object!"); return;}
#define VALIDATE_OBJECT_RET(ENV, OBJECT, RET) if (!OBJECT){throwBadObjectException(ENV, "No corresponsing native object!"); return RET;}

SimulatorRemoteResourceSP SimulatorRemoteResourceToCpp(JNIEnv *env, jobject object)
{
    JniSharedObjectHolder<SimulatorRemoteResource> *jniResource =
        GetHandle<JniSharedObjectHolder<SimulatorRemoteResource>>(env, object);
    if (jniResource)
        return jniResource->get();
    return nullptr;
}

void onObserveCallback(jobject listener, const std::string &uid, const int errorCode,
                       SimulatorResourceModelSP representation, const int seq)
{
    JNIEnv *env = getEnv();
    if (!env)
        return;

    jclass listenerCls = env->GetObjectClass(listener);
    jmethodID listenerMethodId = env->GetMethodID(listenerCls, "onObserveNotification",
                                 "(Ljava/lang/String;Lorg/oic/simulator/SimulatorResourceModel;I)V");

    SimulatorResourceModel *resModel = representation.get();
    jobject jResModel = simulatorResourceModelToJava(env, *resModel);
    jstring jUid = env->NewStringUTF(uid.c_str());
    env->CallVoidMethod(listener, listenerMethodId, jUid, jResModel, seq);
    releaseEnv();
}

void onGetCallback(jobject listener, const std::string &uid, int errorCode,
                   SimulatorResourceModelSP representation)
{
    JNIEnv *env = getEnv();
    if (!env)
        return;

    jclass listenerCls = env->GetObjectClass(listener);
    jmethodID listenerMethodId = env->GetMethodID(listenerCls, "onGetResponse",
                                 "(Ljava/lang/String;Lorg/oic/simulator/SimulatorResult;Lorg/oic/simulator/SimulatorResourceModel;)V");

    SimulatorResourceModel *resModel = representation.get();
    jobject jResModel = simulatorResourceModelToJava(env, *resModel);
    jstring jUid = env->NewStringUTF(uid.c_str());
    env->CallVoidMethod(listener, listenerMethodId, jUid,
                        simulatorResultToJava(env, static_cast<SimulatorResult>(errorCode)), jResModel);

    releaseEnv();
}

void onPutCallback(jobject listener, const std::string &uid, int errorCode,
                   SimulatorResourceModelSP representation)
{
    JNIEnv *env = getEnv();
    if (!env)
        return;

    jclass listenerCls = env->GetObjectClass(listener);
    jmethodID listenerMethodId = env->GetMethodID(listenerCls, "onPutResponse",
                                 "(Ljava/lang/String;Lorg/oic/simulator/SimulatorResult;Lorg/oic/simulator/SimulatorResourceModel;)V");

    SimulatorResourceModel *resModel = representation.get();
    jobject jResModel = simulatorResourceModelToJava(env, *resModel);
    jstring jUid = env->NewStringUTF(uid.c_str());
    env->CallVoidMethod(listener, listenerMethodId, jUid,
                        simulatorResultToJava(env, static_cast<SimulatorResult>(errorCode)), jResModel);

    releaseEnv();
}

void onPostCallback(jobject listener, const std::string &uid, int errorCode,
                    SimulatorResourceModelSP representation)
{
    JNIEnv *env = getEnv();
    if (!env)
        return;

    jclass listenerCls = env->GetObjectClass(listener);
    jmethodID listenerMethodId = env->GetMethodID(listenerCls, "onPostResponse",
                                 "(Ljava/lang/String;Lorg/oic/simulator/SimulatorResult;Lorg/oic/simulator/SimulatorResourceModel;)V");

    SimulatorResourceModel *resModel = representation.get();
    jobject jResModel = simulatorResourceModelToJava(env, *resModel);
    jstring jUid = env->NewStringUTF(uid.c_str());
    env->CallVoidMethod(listener, listenerMethodId, jUid,
                        simulatorResultToJava(env, static_cast<SimulatorResult>(errorCode)), jResModel);

    releaseEnv();
}

void onVerificationCallback(jobject listener, const std::string &uid, int id,
                            OperationState opState)
{
    JNIEnv *env = getEnv();
    if (!env)
        return;

    jclass listenerCls = env->GetObjectClass(listener);
    jmethodID listenerMethodId;
    if (OP_START == opState)
    {
        listenerMethodId = env->GetMethodID(listenerCls, "onVerificationStarted", "(Ljava/lang/String;I)V");
    }
    else if (OP_COMPLETE == opState)
    {
        listenerMethodId = env->GetMethodID(listenerCls, "onVerificationCompleted",
                                            "(Ljava/lang/String;I)V");
    }
    else if (OP_ABORT == opState)
    {
        listenerMethodId = env->GetMethodID(listenerCls, "onVerificationAborted", "(Ljava/lang/String;I)V");
    }

    jstring jUid = env->NewStringUTF(uid.c_str());
    env->CallVoidMethod(listener, listenerMethodId, jUid, id);

    if (OP_COMPLETE == opState || OP_ABORT == opState)
        env->DeleteGlobalRef(listener);
    releaseEnv();
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_org_oic_simulator_client_SimulatorRemoteResource_nativeGet
(JNIEnv *env, jobject object, jstring resInterface, jobject queryParamsMap, jobject listener)
{
    VALIDATE_CALLBACK(env, listener)

    SimulatorRemoteResourceSP resource = SimulatorRemoteResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        JniString jniInterface(env, resInterface);
        std::map<std::string, std::string> queryParams =
            JniQueryParameter(env).toCpp(queryParamsMap);

        SimulatorRemoteResource::ResponseCallback callback =  std::bind([](
                    const std::string & uid, int errorCode, SimulatorResourceModelSP representation,
                    const std::shared_ptr<JniListenerHolder> &listenerRef)
        {
            onGetCallback(listenerRef->get(), uid, errorCode, representation);
        }, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
        JniListenerHolder::create(env, listener));

        resource->get(jniInterface.get(), queryParams, callback);
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
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_client_SimulatorRemoteResource_nativePut
(JNIEnv *env, jobject object, jstring resInterface, jobject queryParamsMap,
 jobject representation, jobject listener)
{
    VALIDATE_CALLBACK(env, listener)

    SimulatorRemoteResourceSP resource = SimulatorRemoteResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        JniString jniInterface(env, resInterface);
        std::map<std::string, std::string> queryParams =
            JniQueryParameter(env).toCpp(queryParamsMap);

        SimulatorResourceModel resModel;
        simulatorResourceModelToCpp(env, representation, resModel);
        SimulatorResourceModelSP resModelSP(new SimulatorResourceModel(resModel));

        SimulatorRemoteResource::ResponseCallback callback =  std::bind([](
                    const std::string & uid, int errorCode, SimulatorResourceModelSP representation,
                    const std::shared_ptr<JniListenerHolder> &listenerRef)
        {
            onPutCallback(listenerRef->get(), uid, errorCode, representation);
        }, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
        JniListenerHolder::create(env, listener));

        resource->put(jniInterface.get(), queryParams, resModelSP, callback);
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
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_client_SimulatorRemoteResource_nativePost
(JNIEnv *env, jobject object, jstring resInterface, jobject queryParamsMap,
 jobject representation, jobject listener)
{
    VALIDATE_CALLBACK(env, listener)

    SimulatorRemoteResourceSP resource = SimulatorRemoteResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        JniString jniInterface(env, resInterface);
        std::map<std::string, std::string> queryParams =
            JniQueryParameter(env).toCpp(queryParamsMap);

        SimulatorResourceModel resModel;
        simulatorResourceModelToCpp(env, representation, resModel);
        SimulatorResourceModelSP resModelSP(new SimulatorResourceModel(resModel));

        SimulatorRemoteResource::ResponseCallback callback =  std::bind([](
                    const std::string & uid, int errorCode, SimulatorResourceModelSP representation,
                    const std::shared_ptr<JniListenerHolder> &listenerRef)
        {
            onPostCallback(listenerRef->get(), uid, errorCode, representation);
        }, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
        JniListenerHolder::create(env, listener));

        resource->post(jniInterface.get(), queryParams, resModelSP, callback);
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
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_client_SimulatorRemoteResource_setConfigInfo
(JNIEnv *env, jobject object, jstring configPath)
{
    VALIDATE_INPUT_RET(env, !configPath, "Path is null!", nullptr)

    SimulatorRemoteResourceSP resource = SimulatorRemoteResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, resource, nullptr)

    try
    {
        JniString jniConfigPath(env, configPath);
        SimulatorResourceModelSP repSchema = resource->configure(jniConfigPath.get());
        return simulatorResourceModelToJava(env, *(repSchema.get()));
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }

    return nullptr;
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_client_SimulatorRemoteResource_startObserve
(JNIEnv *env, jobject object, jobject queryParamsMap, jobject listener)
{
    VALIDATE_CALLBACK(env, listener)

    SimulatorRemoteResourceSP resource = SimulatorRemoteResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        std::map<std::string, std::string> queryParams =
            JniQueryParameter(env).toCpp(queryParamsMap);

        SimulatorRemoteResource::ObserveNotificationCallback callback =  std::bind([](
                    const std::string & uid, const int errorCode,
                    SimulatorResourceModelSP representation, const int seq,
                    const std::shared_ptr<JniListenerHolder> &listenerRef)
        {
            onObserveCallback(listenerRef->get(), uid, errorCode, representation, seq);
        }, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
        std::placeholders::_4, JniListenerHolder::create(env, listener));

        resource->observe(ObserveType::OBSERVE, callback);
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
Java_org_oic_simulator_client_SimulatorRemoteResource_stopObserve
(JNIEnv *env, jobject object)
{
    SimulatorRemoteResourceSP resource = SimulatorRemoteResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        resource->cancelObserve();
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }
}

JNIEXPORT jint JNICALL
Java_org_oic_simulator_client_SimulatorRemoteResource_startVerification
(JNIEnv *env, jobject object, jint reqType, jobject listener)
{
    VALIDATE_CALLBACK_RET(env, listener, -1)

    SimulatorRemoteResourceSP resource = SimulatorRemoteResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, resource, -1)

    // Convert RequestType
    RequestType type;
    switch (reqType)
    {
        case 0: type = RequestType::RQ_TYPE_GET; break;
        case 1: type = RequestType::RQ_TYPE_PUT; break;
        case 2: type = RequestType::RQ_TYPE_POST; break;
        case 3: type = RequestType::RQ_TYPE_DELETE; break;
        default: return -1;
    }

    try
    {
        SimulatorRemoteResource::StateCallback callback =  std::bind([](
                    const std::string & uid, int id, OperationState opState,
                    const std::shared_ptr<JniListenerHolder> &listenerRef)
        {
            onVerificationCallback(listenerRef->get(), uid, id, opState);
        }, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
        JniListenerHolder::create(env, listener));

        return resource->startVerification(type, callback);
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (NoSupportException &e)
    {
        throwNoSupportException(env, e.what());
    }
    catch (OperationInProgressException &e)
    {
        throwOperationInProgressException(env, e.what());
    }
    catch (SimulatorException &e)
    {
        throwSimulatorException(env, e.code(), e.what());
    }

    return -1;
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_client_SimulatorRemoteResource_stopVerification
(JNIEnv *env, jobject object, jint id)
{
    SimulatorRemoteResourceSP resource = SimulatorRemoteResourceToCpp(env, object);
    VALIDATE_OBJECT(env, resource)

    try
    {
        resource->stopVerification(id);
    }
    catch (InvalidArgsException &e)
    {
        throwInvalidArgsException(env, e.code(), e.what());
    }
    catch (NoSupportException &e)
    {
        throwNoSupportException(env, e.what());
    }
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_client_SimulatorRemoteResource_dispose
(JNIEnv *env, jobject object)
{
    JniSharedObjectHolder<SimulatorRemoteResource> *resource =
        GetHandle<JniSharedObjectHolder<SimulatorRemoteResource>>(env, object);
    delete resource;
}

#ifdef __cplusplus
}
#endif
