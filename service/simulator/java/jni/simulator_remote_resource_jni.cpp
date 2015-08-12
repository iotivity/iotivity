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

#include "simulator_remote_resource_jni.h"
#include "simulator_common_jni.h"
#include "simulator_error_codes.h"
#include "simulator_resource_jni_util.h"
#include "simulator_resource_model_jni.h"

extern SimulatorClassRefs gSimulatorClassRefs;

SimulatorRemoteResourcePtr JniSimulatorRemoteResource::getResourceHandle
(JNIEnv *env, jobject object)
{
    JniSimulatorRemoteResource *jniResource = GetHandle<JniSimulatorRemoteResource>(env, object);
    if (env->ExceptionCheck() || !jniResource)
    {
        return nullptr;
    }

    return jniResource->m_resource;
}

class JNIOnObserveListener
{
    public:
        void setJavaOnObserveListener(JNIEnv *env, jobject listener)
        {
            m_listener = env->NewWeakGlobalRef(listener);
        }

        void onObserveCallback(const int eCode,
                               const SimulatorResourceModel &representation,
                               const int sequenceNumber)
        {
            JNIEnv *env = getEnv();
            if (nullptr == env)
                return;

            jobject onObserveListener = env->NewLocalRef(m_listener);
            if (!onObserveListener)
            {
                releaseEnv();
                return;
            }

            jclass onObserveCls = env->GetObjectClass(onObserveListener);
            if (!onObserveCls)
            {
                releaseEnv();
                return;
            }

            if (OC_STACK_OK != eCode && OC_STACK_RESOURCE_CREATED != eCode
                && OC_STACK_RESOURCE_DELETED != eCode)
            {
                jmethodID midL = env->GetMethodID(onObserveCls, "onObserveFailed", "(Ljava/lang/Throwable;)V");
                if (!midL)
                {
                    releaseEnv();
                    return;
                }
                env->CallVoidMethod(onObserveListener, midL);
            }
            else
            {
                SimulatorResourceModel *rep = new SimulatorResourceModel(representation);
                jlong handle = reinterpret_cast<jlong>(rep);
                jobject jRepresentation = env->NewObject(gSimulatorClassRefs.classSimulatorResourceModel,
                                          gSimulatorClassRefs.classSimulatorResourceModelId, handle, true);
                if (!jRepresentation)
                {
                    releaseEnv();
                    return;
                }

                jmethodID midL = env->GetMethodID(onObserveCls, "onObserveCompleted",
                                                  "(Lorg/oic/simulator/SimulatorResourceModel;I)V");
                if (!midL)
                {
                    releaseEnv();
                    return;
                }

                env->CallVoidMethod(onObserveListener, midL, jRepresentation,
                                    static_cast<jint>(sequenceNumber));
                if (env->ExceptionCheck())
                {
                    releaseEnv();
                }
            }
        }

    private:
        jweak m_listener;
};

class JNIOnGetListener
{
    public:
        void setJavaOnGetListener(JNIEnv *env, jobject listener)
        {
            m_listener = env->NewWeakGlobalRef(listener);
        }

        void onGetCallback(const int eCode,
                           const SimulatorResourceModel &representation)
        {
            JNIEnv *env = getEnv();
            if (nullptr == env)
                return;

            jobject onGetListener = env->NewLocalRef(m_listener);
            if (!onGetListener)
            {
                releaseEnv();
                return;
            }

            jclass onGetCls = env->GetObjectClass(onGetListener);
            if (!onGetCls)
            {
                releaseEnv();
                return;
            }

            if (OC_STACK_OK != eCode && OC_STACK_RESOURCE_CREATED != eCode
                && OC_STACK_RESOURCE_DELETED != eCode)
            {
                jmethodID midL = env->GetMethodID(onGetCls, "onGetFailed", "(Ljava/lang/Throwable;)V");
                if (!midL)
                {
                    releaseEnv();
                    return;
                }
                env->CallVoidMethod(onGetListener, midL);
            }
            else
            {
                SimulatorResourceModel *rep = new SimulatorResourceModel(representation);
                jlong handle = reinterpret_cast<jlong>(rep);
                jobject jRepresentation = env->NewObject(gSimulatorClassRefs.classSimulatorResourceModel,
                                          gSimulatorClassRefs.classSimulatorResourceModelId, handle, true);
                if (!jRepresentation)
                {
                    releaseEnv();
                    return;
                }

                jmethodID midL = env->GetMethodID(onGetCls, "onGetCompleted",
                                                  "(Lorg/oic/simulator/SimulatorResourceModel;)V");
                if (!midL)
                {
                    releaseEnv();
                    return;
                }

                env->CallVoidMethod(onGetListener, midL, jRepresentation);
                if (env->ExceptionCheck())
                {
                    releaseEnv();
                }
            }
        }

    private:
        jweak m_listener;
};

class JNIOnPutListener
{
    public:
        void setJavaOnPutListener(JNIEnv *env, jobject listener)
        {
            m_listener = env->NewWeakGlobalRef(listener);
        }

        void onPutCallback(const int eCode,
                           const SimulatorResourceModel &representation)
        {
            JNIEnv *env = getEnv();
            if (nullptr == env)
                return;

            jobject onPutListener = env->NewLocalRef(m_listener);
            if (!onPutListener)
            {
                releaseEnv();
                return;
            }

            jclass onGetCls = env->GetObjectClass(onPutListener);
            if (!onGetCls)
            {
                releaseEnv();
                return;
            }

            if (OC_STACK_OK != eCode && OC_STACK_RESOURCE_CREATED != eCode
                && OC_STACK_RESOURCE_DELETED != eCode)
            {
                jmethodID midL = env->GetMethodID(onGetCls, "onPutFailed", "(Ljava/lang/Throwable;)V");
                if (!midL)
                {
                    releaseEnv();
                    return;
                }
                env->CallVoidMethod(onPutListener, midL);
            }
            else
            {
                SimulatorResourceModel *rep = new SimulatorResourceModel(representation);
                jlong handle = reinterpret_cast<jlong>(rep);
                jobject jRepresentation = env->NewObject(gSimulatorClassRefs.classSimulatorResourceModel,
                                          gSimulatorClassRefs.classSimulatorResourceModelId, handle, true);
                if (!jRepresentation)
                {
                    releaseEnv();
                    return;
                }

                jmethodID midL = env->GetMethodID(onGetCls, "onPutCompleted",
                                                  "(Lorg/oic/simulator/SimulatorResourceModel;)V");
                if (!midL)
                {
                    releaseEnv();
                    return;
                }

                env->CallVoidMethod(onPutListener, midL, jRepresentation);
                if (env->ExceptionCheck())
                {
                    releaseEnv();
                }
            }
        }

    private:
        jweak m_listener;
};

class JNIOnPostListener
{
    public:
        void setJavaOnPostListener(JNIEnv *env, jobject listener)
        {
            m_listener = env->NewWeakGlobalRef(listener);
        }

        void onPostCallback(const int eCode,
                            const SimulatorResourceModel &representation)
        {
            JNIEnv *env = getEnv();
            if (nullptr == env)
                return;

            jobject onPostListener = env->NewLocalRef(m_listener);
            if (!onPostListener)
            {
                releaseEnv();
                return;
            }

            jclass onGetCls = env->GetObjectClass(onPostListener);
            if (!onGetCls)
            {
                releaseEnv();
                return;
            }

            if (OC_STACK_OK != eCode && OC_STACK_RESOURCE_CREATED != eCode
                && OC_STACK_RESOURCE_DELETED != eCode)
            {
                jmethodID midL = env->GetMethodID(onGetCls, "onPostFailed", "(Ljava/lang/Throwable;)V");
                if (!midL)
                {
                    releaseEnv();
                    return;
                }
                env->CallVoidMethod(onPostListener, midL);
            }
            else
            {
                SimulatorResourceModel *rep = new SimulatorResourceModel(representation);
                jlong handle = reinterpret_cast<jlong>(rep);
                jobject jRepresentation = env->NewObject(gSimulatorClassRefs.classSimulatorResourceModel,
                                          gSimulatorClassRefs.classSimulatorResourceModelId, handle, true);
                if (!jRepresentation)
                {
                    releaseEnv();
                    return;
                }

                jmethodID midL = env->GetMethodID(onGetCls, "onPostCompleted",
                                                  "(Lorg/oic/simulator/SimulatorResourceModel;)V");
                if (!midL)
                {
                    releaseEnv();
                    return;
                }

                env->CallVoidMethod(onPostListener, midL, jRepresentation);
                if (env->ExceptionCheck())
                {
                    releaseEnv();
                }
            }
        }

    private:
        jweak m_listener;

};


JNIEXPORT jint JNICALL
Java_org_oic_simulator_SimulatorRemoteResource_observe
(JNIEnv *env, jobject thiz, jint observeType, jobject jQueryParamsMap, jobject jListener)
{
    if (!jListener)
        return SIMULATOR_BAD_INPUT;

    SimulatorRemoteResourcePtr resource = JniSimulatorRemoteResource::getResourceHandle(env,
                                           thiz);
    if (nullptr == resource)
        return SIMULATOR_RESOURCE_NOT_FOUND;

    std::map<std::string, std::string> queryParams;
    if (jQueryParamsMap)
        convertJavaMapToQueryParamsMap(env, jQueryParamsMap, queryParams);

    SimulatorRemoteResource::ObserveType type = SimulatorRemoteResource::OBSERVE;
    if (1 == observeType)
        type = SimulatorRemoteResource::OBSERVE_ALL;

    JNIOnObserveListener *onObserveListener = new JNIOnObserveListener();
    onObserveListener->setJavaOnObserveListener(env, jListener);

    return resource->observe(type, queryParams,
                                                std::bind(&JNIOnObserveListener::onObserveCallback,
                                                onObserveListener, std::placeholders::_1,
                                                std::placeholders::_2, std::placeholders::_3));
}

JNIEXPORT jint JNICALL
Java_org_oic_simulator_SimulatorRemoteResource_cancelObserve
(JNIEnv *env, jobject thiz)
{
    SimulatorRemoteResourcePtr resource = JniSimulatorRemoteResource::getResourceHandle(env,
                                           thiz);
    if (nullptr == resource)
        return SIMULATOR_RESOURCE_NOT_FOUND;

    return resource->cancelObserve();
}

JNIEXPORT jint JNICALL
Java_org_oic_simulator_SimulatorRemoteResource_get
(JNIEnv *env, jobject thiz, jstring jResourceType, jstring jResourceInterface,
 jobject jQueryParamsMap, jobject jListener)
{
    if (!jListener)
        return SIMULATOR_BAD_INPUT;

    SimulatorRemoteResourcePtr resource = JniSimulatorRemoteResource::getResourceHandle(env,
                                           thiz);
    if (nullptr == resource)
        return SIMULATOR_RESOURCE_NOT_FOUND;

    // Resource type
    const char *typeCStr = NULL;
    std::string resourceType;
    if (jResourceType)
    {
        typeCStr = env->GetStringUTFChars(jResourceType, NULL);
        resourceType = typeCStr;
    }

    // Interface type
    const char *interfaceCStr = NULL;
    std::string interfaceType;
    if (jResourceInterface)
    {
        interfaceCStr = env->GetStringUTFChars(jResourceInterface, NULL);
        interfaceType = interfaceCStr;
    }

    // Query parameters
    std::map<std::string, std::string> queryParams;
    if (jQueryParamsMap)
        convertJavaMapToQueryParamsMap(env, jQueryParamsMap, queryParams);

    // Create listener
    JNIOnGetListener *onGetListener = new JNIOnGetListener();
    onGetListener->setJavaOnGetListener(env, jListener);

    SimulatorResult result = resource->get(resourceType, interfaceType,
                                            queryParams,
                                            std::bind(&JNIOnGetListener::onGetCallback,
                                            onGetListener, std::placeholders::_1,
                                            std::placeholders::_2));

    if (typeCStr)
        env->ReleaseStringUTFChars(jResourceType, typeCStr);

    if (interfaceCStr)
        env->ReleaseStringUTFChars(jResourceInterface, interfaceCStr);

    return result;
}

JNIEXPORT jint JNICALL
Java_org_oic_simulator_SimulatorRemoteResource_put
(JNIEnv *env, jobject thiz, jstring jResourceType, jstring jResourceInterface,
 jobject jRepresentation, jobject jQueryParamsMap, jobject jListener)
{
    if (!jListener)
        return SIMULATOR_BAD_INPUT;

    SimulatorRemoteResourcePtr resource = JniSimulatorRemoteResource::getResourceHandle(env,
                                           thiz);
    if (nullptr == resource)
        return SIMULATOR_RESOURCE_NOT_FOUND;

    // Resource type
    const char *typeCStr = NULL;
    std::string resourceType;
    if (jResourceType)
    {
        typeCStr = env->GetStringUTFChars(jResourceType, NULL);
        resourceType = typeCStr;
    }

    // Interface type
    const char *interfaceCStr = NULL;
    std::string interfaceType;
    if (jResourceInterface)
    {
        interfaceCStr = env->GetStringUTFChars(jResourceInterface, NULL);
        interfaceType = interfaceCStr;
    }

    // Query parameters
    std::map<std::string, std::string> queryParams;
    if (jQueryParamsMap)
        convertJavaMapToQueryParamsMap(env, jQueryParamsMap, queryParams);

    SimulatorResourceModel resourceModel;
    JniSimulatorResourceModel::getResourceModel(env, jRepresentation, resourceModel);

    // Create listener
    JNIOnPutListener *onPutListener = new JNIOnPutListener();
    onPutListener->setJavaOnPutListener(env, jListener);

    SimulatorResult result = resource->put(resourceType, interfaceType,
                                            resourceModel, queryParams,
                                            std::bind(&JNIOnPutListener::onPutCallback,
                                            onPutListener, std::placeholders::_1,
                                            std::placeholders::_2));

    if (typeCStr)
        env->ReleaseStringUTFChars(jResourceType, typeCStr);

    if (interfaceCStr)
        env->ReleaseStringUTFChars(jResourceInterface, interfaceCStr);

    return result;
}

JNIEXPORT jint JNICALL
Java_org_oic_simulator_SimulatorRemoteResource_post
(JNIEnv *env, jobject thiz, jstring jResourceType, jstring jResourceInterface,
 jobject jRepresentation, jobject jQueryParamsMap, jobject jListener)
{
    if (!jListener)
        return SIMULATOR_BAD_INPUT;

    SimulatorRemoteResourcePtr resource = JniSimulatorRemoteResource::getResourceHandle(env,
                                           thiz);
    if (nullptr == resource)
        return SIMULATOR_RESOURCE_NOT_FOUND;

    // Resource type
    const char *typeCStr = NULL;
    std::string resourceType;
    if (jResourceType)
    {
        typeCStr = env->GetStringUTFChars(jResourceType, NULL);
        resourceType = typeCStr;
    }

    // Interface type
    const char *interfaceCStr = NULL;
    std::string interfaceType;
    if (jResourceInterface)
    {
        interfaceCStr = env->GetStringUTFChars(jResourceInterface, NULL);
        interfaceType = interfaceCStr;
    }

    // Query parameters
    std::map<std::string, std::string> queryParams;
    if (jQueryParamsMap)
        convertJavaMapToQueryParamsMap(env, jQueryParamsMap, queryParams);

    SimulatorResourceModel resourceModel;
    JniSimulatorResourceModel::getResourceModel(env, jRepresentation, resourceModel);

    // Create listener
    JNIOnPostListener *onPostListener = new JNIOnPostListener();
    onPostListener->setJavaOnPostListener(env, jListener);

    SimulatorResult result = resource->post(resourceType, interfaceType,
                                            resourceModel, queryParams,
                                            std::bind(&JNIOnPostListener::onPostCallback,
                                            onPostListener, std::placeholders::_1,
                                            std::placeholders::_2));

    if (typeCStr)
        env->ReleaseStringUTFChars(jResourceType, typeCStr);

    if (interfaceCStr)
        env->ReleaseStringUTFChars(jResourceInterface, interfaceCStr);

    return result;
}

