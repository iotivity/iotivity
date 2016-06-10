//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <jni.h>
#include "NSProviderInterface.h"
#include "NSCommon.h"

#ifndef NOTIFICATION_JNI_H
#define NOTIFICATION_JNI_H
#ifdef __cplusplus
extern "C"
{
#endif

    JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSStartProvider(
            JNIEnv *, jobject, jboolean, jobject, jobject);

    JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSStopProvider(
            JNIEnv *, jobject);

    JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSSendNotification(
            JNIEnv *, jobject, jobject);

    JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSProviderReadCheck(
            JNIEnv *, jobject, jobject);

    JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSAccept(JNIEnv *,
            jobject, jobject, jboolean);

    void NSSubscribeRequestCb(NSConsumer*);

    void NSSyncCb(NSSyncInfo*);

    NSMessage * NSGetMessage(JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
