/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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
#include <memory>
#include <vector>

#include "OCPlatform.h"
#include "OCResource.h"
#include "octypes.h"
#include "ESRichCommon.h"

#include "JniOcPlatform.h"
#include "JniOcResource.h"
#include "JniEasySetup.h"

using namespace OC;
using namespace OIC::Service;

#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_easysetup_mediator_EasySetup_nativeCreateRemoteEnrollee
(JNIEnv *env, jobject thiz, jobject jResource)
{
    LOGI("JniEasySetup::nativeCreateRemoteEnrollee enter");

    std::shared_ptr<RemoteEnrollee> nativeRemoteEnrollee;
    jobject jRemoteEnrollee;

    JniOcResource* jniOcResource = JniOcResource::getJniOcResourcePtr(env, jResource);

    if (!jniOcResource)
    {
        LOGE("JniEasySetup::nativeCreateRemoteEnrollee getJniOcResourcePtr returns nullptr.");
        return nullptr;
    }

    try
    {
        nativeRemoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(jniOcResource->getOCResource());
        //create the java object
        jRemoteEnrollee = env->NewObject(g_cls_RemoteEnrollee, g_mid_RemoteEnrollee_ctor);
        if (!jRemoteEnrollee)
        {
            LOGE("JniEasySetup::nativeCreateRemoteEnrollee Unable to create the java object");
            return NULL;
        }
        JniRemoteEnrollee *jniRemoteEnrollee = new JniRemoteEnrollee(nativeRemoteEnrollee);
        ESSetHandle<JniRemoteEnrollee>(env, jRemoteEnrollee, jniRemoteEnrollee);
    }
    catch (ESBadRequestException exception)
    {
        LOGE("JniEasySetup::nativeCreateRemoteEnrollee Unable to create the Native EnrolleeDevice");
        //throw the exception to java layer
        throwESException( env,  exception.what());
    }

    LOGI("JniEasySetup::nativeCreateRemoteEnrollee exit");

    return jRemoteEnrollee;
}
#ifdef __cplusplus
}
#endif

