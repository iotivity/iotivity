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

#include "JniEasySetup.h"

using namespace OIC::Service;

#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_easysetup_mediator_EasySetupService_nativeCreateEnrolleeDevice
(JNIEnv *env, jobject interfaceClass, jstring ip, jstring ssid, jstring password,
 jint connectivityType, jboolean isSecured)
{

    LOGI("JniEasySetup::nativeCreateEnrolleeDevice enter");

    std::shared_ptr<RemoteEnrollee> nativeRemoteEnrollee;
    jobject jRemoteEnrollee;
    ProvConfig netInfo;
    WiFiOnboadingConnection onboardConn;

    const char *cIp = (env)->GetStringUTFChars(ip, NULL);
    const char *cSssid = (env)->GetStringUTFChars(ssid, NULL);
    const char *cPassword = (env)->GetStringUTFChars(password, NULL);

    std::string sIp(cIp);
    std::string sSssid(cSssid);
    std::string sPassword(cPassword);

    netInfo.connType = getOCConnectivityTypeFromInt(connectivityType);

    onboardConn.isSecured = bool(isSecured);

    OICStrcpy(onboardConn.ipAddress, IPV4_ADDR_SIZE - 1, sIp.c_str());
    OICStrcpy(netInfo.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, sSssid.c_str());
    OICStrcpy(netInfo.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, sPassword.c_str());

    try
    {
        nativeRemoteEnrollee = EasySetup::getInstance()->createEnrolleeDevice(netInfo,onboardConn);
        //create the java object
        jRemoteEnrollee = env->NewObject(g_cls_RemoteEnrollee, g_mid_RemoteEnrollee_ctor);
        if (!jRemoteEnrollee)
        {
            LOGE("JniEasySetup::nativeCreateEnrolleeDevice Unable to create the java object");
            return NULL;
        }

        JniRemoteEnrollee *jniRemoteEnrollee = new JniRemoteEnrollee(nativeRemoteEnrollee);
        ESSetHandle<JniRemoteEnrollee>(env, jRemoteEnrollee, jniRemoteEnrollee);
    }
    catch (ESBadRequestException exception)
    {
        LOGE("JniEasySetup::nativeCreateEnrolleeDevice Unable to create the Native EnrolleeDevice");
        //throw the exception to java layer
        throwESException( env,  exception.what());
    }

    LOGI("JniEasySetup::nativeCreateEnrolleeDevice exit");

    return jRemoteEnrollee;
}
#ifdef __cplusplus
}
#endif

