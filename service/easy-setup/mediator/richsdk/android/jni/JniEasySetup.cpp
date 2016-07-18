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

#include "JniEasySetup.h"

using namespace OC;
using namespace OIC::Service;

#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_easysetup_mediator_EasySetup_nativeCreateRemoteEnrollee
(JNIEnv *env, jobject thiz , jstring jhost, jstring juri, jstring jdID, jboolean jisObservable, jint jconType)
{
    LOGI("JniEasySetup::nativeCreateRemoteEnrollee enter");

    std::shared_ptr<RemoteEnrollee> nativeRemoteEnrollee;
    std::shared_ptr< OC::OCResource > resource;
    jobject jRemoteEnrollee;
    std::string host;
    std::string uri;
    std::string devID;

    host = env->GetStringUTFChars(jhost, NULL);
    uri = env->GetStringUTFChars(juri, NULL);
    devID = env->GetStringUTFChars(jdID, NULL);
    bool isObservable = jisObservable;
    OCConnectivityType conType = static_cast<OCConnectivityType>(jconType);
    std::vector<std::string> resourceTypes = {OC_RSRVD_ES_RES_TYPE_PROV};
    std::vector<std::string> interfaces = {OC_RSRVD_INTERFACE_BATCH};

    LOGI("host : %s", host.c_str());
    LOGI("uri : %s", uri.c_str());
    try
    {
        resource = OC::OCPlatform::constructResourceObject(host,
                                uri, conType, isObservable, resourceTypes, interfaces);
    }
    catch(ResourceInitException e)
    {
        LOGI("constructResourceObject error : %s", e.what());
        return NULL;
    }

    try
    {
        nativeRemoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(resource);
        nativeRemoteEnrollee->setDevID(devID);
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

