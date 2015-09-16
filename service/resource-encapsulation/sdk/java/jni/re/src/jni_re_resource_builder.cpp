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
#include "jni_re_jvm.h"
#include "jni_re_resource_builder.h"
#include "jni_re_resource_object.h"
#include "RCSResourceObject.h"
#include "jni_re_resource_attributes.h"

using namespace OIC::Service;

JNIEXPORT jobject JNICALL Java_org_iotivity_ResourceEncapsulation_server_Builder_nativeBuild
(JNIEnv *env, jobject interfaceObject, jstring jResourceURI, jstring jResourceType,
 jstring jResourceInterface,
 jboolean jObservableFlag, jboolean jDiscovervableFlag, jobject jAttributes)
{
    LOGI("nativeBuild called");

    std::string resourceURI;
    if (jResourceURI)
    {
        resourceURI = env->GetStringUTFChars(jResourceURI, NULL);
    }
    std::string resourceType;
    if (jResourceType)
    {
        resourceType = env->GetStringUTFChars(jResourceType, NULL);
    }
    std::string resourceInterface;
    if (jResourceInterface)
    {
        resourceInterface = env->GetStringUTFChars(jResourceInterface, NULL);
    }

    std::shared_ptr< RCSResourceObject > NativeRCSResourceObject;
    try
    {
        if (jAttributes)
        {
            RCSResourceAttributes *attributesObj =   JniResourceAttributes::getResourceAttributePtr(env,
                    jAttributes);
            NativeRCSResourceObject = RCSResourceObject::Builder(resourceURI, resourceType,
                                      resourceInterface).setDiscoverable((bool)jObservableFlag).setObservable((
                                                  bool)jObservableFlag).setAttributes(*attributesObj).build();

            jobject jResource = env->NewObject(g_cls_RCSResourceObject, g_mid_RCSResourceObject_ctor);
            JniRCSResourceObject *jniRCSResourceObject = new JniRCSResourceObject(NativeRCSResourceObject);
            RESetHandle<JniRCSResourceObject>(env, jResource, jniRCSResourceObject);
            return jResource;
        }
        else
        {
            NativeRCSResourceObject = RCSResourceObject::Builder(resourceURI, resourceType,
                                      resourceInterface).setDiscoverable((bool)jObservableFlag).setObservable((
                                                  bool)jObservableFlag).build();

            jobject jResource = env->NewObject(g_cls_RCSResourceObject, g_mid_RCSResourceObject_ctor);
            JniRCSResourceObject *jniRCSResourceObject = new JniRCSResourceObject(NativeRCSResourceObject);
            RESetHandle<JniRCSResourceObject>(env, jResource, jniRCSResourceObject);
            return jResource;
        }
    }
    catch (OIC::Service::PlatformException exception)
    {
        LOGE("nativeBuild : platform Exception");
        throwRCSException(env,  exception.what());
    }
    catch (OIC::Service::RCSException exception)
    {
        LOGE("nativeBuild : RCSException");
        throwRCSException(env,  exception.what());
    }
}
