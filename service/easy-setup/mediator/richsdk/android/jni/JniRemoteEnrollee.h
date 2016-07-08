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

/** @file   JniRemoteEnrollee.h
 *
 *   @brief  This file contains the JniRemoteEnrollee class
 *               & declaration of RemoteEnrollee APIs for JNI implementation
 */

#ifndef __JNI_ES_REMOTEENROLLEE_H
#define __JNI_ES_REMOTEENROLLEE_H

#include "RemoteEnrollee.h"
#include "ESRichCommon.h"
#include "ESException.h"

#include "JniJvm.h"
#include "JniEsUtils.h"
#include "JniRequestPropertyDataStatusListener.h"
#include "JniSecurityStatusListener.h"
#include "JniDataProvisioningStatusListener.h"
#include "JniCloudProvisioningStatusListener.h"
#include "JniEsListenerManager.h"

using namespace OIC::Service;

/**
 * @class   JniRemoteEnrollee
 * @brief   This class contains all the APIs for RemoteEnrollee
 *
 * NOTE: JNI APIs internally call the APIs of this class.
 */
class JniRemoteEnrollee
{
    public:

        /**
        *@brief constructor
        */
        JniRemoteEnrollee(std::shared_ptr< RemoteEnrollee> remoteEnrollee);

        /**
         *@brief destructor
        */
        ~JniRemoteEnrollee();

        // ***** JNI APIs internally call the APIs of this class ***** //
        void initRemoteEnrollee(JNIEnv *env);
        void requestPropertyData(JNIEnv *env, jobject jListener);
        void startSecurityProvisioning(JNIEnv *env, jobject jListener);
        void startDataProvisioning(JNIEnv *env, jobject jListener);
        void startCloudProvisioning(JNIEnv *env, jobject jListener);
        void setDataProvInfo(JNIEnv *env, jstring jssid, jstring jpwd, jint jauthType,
                                            jint jencType, jstring jlanguage, jstring jcountry);
        void setCloudProvInfo(JNIEnv *env, jstring authCode, jstring authProvider, jstring ciServer);

        static JniRemoteEnrollee *getJniRemoteEnrollee(JNIEnv *env, jobject thiz);

        template <class T>
        T *addStatusListener(JNIEnv *env, jobject jListener)
        {
            return JniEsListenerManager<T>().addListener(env, jListener, this);
        };

        template <class T>
        void removeStatusListener(JNIEnv *env, jobject jListener)
        {
            JniEsListenerManager<T>().removeListener(env, jListener);
        };

    private:
        std::shared_ptr<RemoteEnrollee> m_sharedResource;

};


#ifdef __cplusplus
extern "C" {
#endif

/**
 * API for starting the initRemoteEnrollee process.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeInitRemoteEnrollee
(JNIEnv *env, jobject jClass);

/**
 * API for starting the Request PropertyData process.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeRequestPropertyData
(JNIEnv *env, jobject jClass, jobject jListener);

/**
 * API for starting the Sequrity provisioning process.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeStartSecurityProvision
(JNIEnv *env, jobject jClass, jobject jListener);

/**
 * API for starting the Data provisioning process.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeStartDataProvision
(JNIEnv *env, jobject jClass, jobject jListener);

/**
 * API for setting data provisioning information.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeSetDataProvInfo
(JNIEnv *env, jobject jClass, jstring jssid, jstring jpwd, jint jauthType, jint jencType,
    jstring jlanguage, jstring jcountry);

/**
 * API for setting cloud provisioning information.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeSetCloudProvInfo
(JNIEnv *env, jobject jClass, jstring jauthCode, jstring jauthProvider, jstring jciServer);

/**
 * API for starting the cloud provisioning process.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeStartCloudProvisioning
(JNIEnv *env, jobject jClass, jobject jListener);

#ifdef __cplusplus
}
#endif
#endif // __JNI_ES_REMOTEENROLLEE_H
