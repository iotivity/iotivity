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

#include "simulator_exceptions_jni.h"
#include "simulator_resource_utils_jni.h"
#include "simulator_utils_jni.h"
#include "jni_sharedobject_holder.h"
#include "jni_string.h"
#include "jni_vector.h"

#include "simulator_collection_resource.h"

#define VALIDATE_OBJECT(ENV, OBJECT) if (!OBJECT){throwBadObjectException(ENV, "No corresponsing native object!"); return;}
#define VALIDATE_OBJECT_RET(ENV, OBJECT, RET) if (!OBJECT){throwBadObjectException(ENV, "No corresponsing native object!"); return RET;}

extern SimulatorResourceSP SimulatorResourceToCpp(JNIEnv *env, jobject object);

SimulatorCollectionResourceSP SimulatorCollectionResourceToCpp(JNIEnv *env, jobject object)
{
    JniSharedObjectHolder<SimulatorCollectionResource> *jniResource =
        GetHandle<JniSharedObjectHolder<SimulatorCollectionResource>>(env, object);
    if (jniResource)
        return jniResource->get();
    return nullptr;
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_server_SimulatorCollectionResource_getSupportedResources
(JNIEnv *env, jobject object)
{
    SimulatorCollectionResourceSP collectionResource = SimulatorCollectionResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, collectionResource, nullptr)

    std::vector<std::string> supportedTypes = collectionResource->getSupportedResources();
    return JniVector(env).toJava(supportedTypes);
}

JNIEXPORT void JNICALL
Java_org_oic_simulator_server_SimulatorCollectionResource_addChildResource
(JNIEnv *env, jobject object, jobject resource)
{
    VALIDATE_INPUT(env, !resource, "Child resource is null!")

    SimulatorResourceSP childResource = SimulatorResourceToCpp(env, resource);
    VALIDATE_INPUT(env, !childResource, "No corresponding native object of child resource!")

    SimulatorCollectionResourceSP collectionResource = SimulatorCollectionResourceToCpp(env, object);
    VALIDATE_OBJECT(env, collectionResource)

    try
    {
        collectionResource->addChildResource(childResource);
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
Java_org_oic_simulator_server_SimulatorCollectionResource_removeChildResource
(JNIEnv *env, jobject object, jobject resource)
{
    VALIDATE_INPUT(env, !resource, "Child resource is null!")

    SimulatorResourceSP childResource = SimulatorResourceToCpp(env, resource);
    VALIDATE_INPUT(env, !childResource, "No corresponding native object of child resource!")

    SimulatorCollectionResourceSP collectionResource = SimulatorCollectionResourceToCpp(env, object);
    VALIDATE_OBJECT(env, collectionResource)

    try
    {
        collectionResource->removeChildResource(childResource);
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
Java_org_oic_simulator_server_SimulatorCollectionResource_removeChildResourceByUri
(JNIEnv *env, jobject object, jstring uri)
{
    SimulatorCollectionResourceSP collectionResource = SimulatorCollectionResourceToCpp(env, object);
    VALIDATE_OBJECT(env, collectionResource)

    try
    {
        JniString jniUri(env, uri);
        collectionResource->removeChildResource(jniUri.get());
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

JNIEXPORT jobject JNICALL
Java_org_oic_simulator_server_SimulatorCollectionResource_getChildResource
(JNIEnv *env, jobject object)
{
    SimulatorCollectionResourceSP collectionResource = SimulatorCollectionResourceToCpp(env, object);
    VALIDATE_OBJECT_RET(env, collectionResource, nullptr)

    std::vector<SimulatorResourceSP> childResources = collectionResource->getChildResources();
    return createSimulatorResourceVector(env,  childResources);
}

JNIEXPORT void JNICALL Java_org_oic_simulator_server_SimulatorCollectionResource_dispose
(JNIEnv *env, jobject object)
{
    JniSharedObjectHolder<SimulatorCollectionResource> *resource =
        GetHandle<JniSharedObjectHolder<SimulatorCollectionResource>>(env, object);
    delete resource;
}

#ifdef __cplusplus
}
#endif
