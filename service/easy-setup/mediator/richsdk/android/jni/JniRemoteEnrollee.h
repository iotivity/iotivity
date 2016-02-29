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
#include "JniProvisioningStatusListener.h"
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

        void startProvisioning(JNIEnv *env);
        void stopProvisioning(JNIEnv *env);
        void registerProvisioningHandler(JNIEnv *env, jobject jListener);

        JniProvisioningStatusListener *addProvisioningStatusListener(JNIEnv *env, jobject jListener);
        void removeProvisioningStatusListener(JNIEnv *env, jobject jListener);

        static JniRemoteEnrollee *getJniRemoteEnrollee(JNIEnv *env, jobject thiz);

    private:
        JniEsListenerManager<JniProvisioningStatusListener> m_provisioingStatus;
        std::shared_ptr<RemoteEnrollee> m_sharedResource;

};


#ifdef __cplusplus
extern "C" {
#endif

/**
 * API for starting the provisioning process.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeStartProvision
(JNIEnv *env, jobject jClass);

/**
 * API for stopping the provisioning process.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeStopProvision
(JNIEnv *env, jobject jClass);

/**
 * API for setting the lisener for recieiving provisioning status.
 *
 * @param provisiongListener - Provisioning listener [callback from native layer will be passing to this listener]
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeRegisterProvisioningHandler
(JNIEnv *env, jobject jClass, jobject provisiongListener);

#ifdef __cplusplus
}
#endif
#endif // __JNI_ES_REMOTEENROLLEE_H
