/*
* ******************************************************************
*
*  Copyright 2015 Intel Corporation.
*
* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
#include "JniOcStack.h"
#include "JniOnResourceFoundListener.h"
#include "JniOnResourcesFoundListener.h"
#include "JniOnDeviceInfoListener.h"
#include "JniOnPlatformInfoListener.h"
#include "JniOnPresenceListener.h"
#include "JniOnObserveListener.h"

#include <mutex>

#ifndef _Included_org_iotivity_base_OcPlatform
#define _Included_org_iotivity_base_OcPlatform

JniOnResourceFoundListener* AddOnResourceFoundListener(JNIEnv* env, jobject jListener);
void RemoveOnResourceFoundListener(JNIEnv* env, jobject jListener);

JniOnResourcesFoundListener* AddOnResourcesFoundListener(JNIEnv* env, jobject jListener);
void RemoveOnResourcesFoundListener(JNIEnv* env, jobject jListener);

JniOnDeviceInfoListener* AddOnDeviceInfoListener(JNIEnv* env, jobject jListener);
void RemoveOnDeviceInfoListener(JNIEnv* env, jobject jListener);

JniOnPlatformInfoListener* AddOnPlatformInfoListener(JNIEnv* env, jobject jListener);
void RemoveOnPlatformInfoListener(JNIEnv* env, jobject jListener);

JniOnPresenceListener* AddOnPresenceListener(JNIEnv* env, jobject jListener);
void RemoveOnPresenceListener(JNIEnv* env, jobject jListener);

JniOnObserveListener* AddOnObserveListener(JNIEnv* env, jobject jListener);
void RemoveOnObserveListener(JNIEnv* env, jobject jListener);

std::map<jobject, std::pair<JniOnResourceFoundListener*, int>> onResourceFoundListenerMap;
std::map<jobject, std::pair<JniOnResourcesFoundListener*, int>> onResourcesFoundListenerMap;
std::map<jobject, std::pair<JniOnDeviceInfoListener*, int>> onDeviceInfoListenerMap;
std::map<jobject, std::pair<JniOnPlatformInfoListener*, int>> onPlatformInfoListenerMap;
std::map<jobject, std::pair<JniOnPresenceListener*, int>> onPresenceListenerMap;
std::map<jobject, std::pair<JniOnObserveListener*, int>> onObserveListenerMap;

std::mutex resourceFoundMapLock;
std::mutex resourcesFoundMapLock;
std::mutex deviceInfoMapLock;
std::mutex platformInfoMapLock;
std::mutex presenceMapLock;
std::mutex observeMapLock;

#ifdef __cplusplus
extern "C" {
#endif
    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    configure
    * Signature: (IILjava/lang/String;IILjava/lang/String;Ljava/lang/String;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_configure
        (JNIEnv *, jclass, jint, jint, jstring, jint, jint, jstring, jstring, jint);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    stop
    * Signature: ()V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_stop
        (JNIEnv *, jclass);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    start
    * Signature: ()V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_start
        (JNIEnv *, jclass);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    notifyAllObservers0
    * Signature: (Lorg/iotivity/base/OcResourceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_notifyAllObservers0
        (JNIEnv *, jclass, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    notifyAllObservers1
    * Signature: (Lorg/iotivity/base/OcResourceHandle;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_notifyAllObservers1
        (JNIEnv *, jclass, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    notifyListOfObservers2
    * Signature: (Lorg/iotivity/base/OcResourceHandle;[BLorg/iotivity/base/OcResourceResponse;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_notifyListOfObservers2
        (JNIEnv *, jclass, jobject, jbyteArray, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    notifyListOfObservers3
    * Signature: (Lorg/iotivity/base/OcResourceHandle;[BLorg/iotivity/base/OcResourceResponse;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_notifyListOfObservers3
        (JNIEnv *, jclass, jobject, jbyteArray, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    findResource0
    * Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnResourceFoundListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findResource0
        (JNIEnv *, jclass, jstring, jstring, jint, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    findResource1
    * Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnResourceFoundListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findResource1
        (JNIEnv *, jclass, jstring, jstring, jint, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    findResources0
    * Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnResourcesFoundListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findResources0
        (JNIEnv *, jclass, jstring, jstring, jint, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    findResources1
    * Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnResourcesFoundListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findResources1
        (JNIEnv *, jclass, jstring, jstring, jint, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    getDeviceInfo0
    * Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnDeviceFoundListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getDeviceInfo0
        (JNIEnv *, jclass, jstring, jstring, jint, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    getDeviceInfo1
    * Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnDeviceFoundListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getDeviceInfo1
        (JNIEnv *, jclass, jstring, jstring, jint, jobject, jint);

    /*
     * Class:     org_iotivity_base_OcPlatform
     * Method:    getPlatformInfo0
     * Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnPlatformFoundListener;)V
     */
     JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getPlatformInfo0
        (JNIEnv *, jclass, jstring, jstring, jint, jobject);

    /*
     * Class:     org_iotivity_base_OcPlatform
     * Method:    getPlatformInfo1
     * Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnPlatformFoundListener;I)V
     */
     JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getPlatformInfo1
        (JNIEnv *, jclass, jstring, jstring, jint, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    registerResource0
    * Signature: (Lorg/iotivity/base/OcResource;)Lorg/iotivity/base/OcResourceHandle;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_registerResource0
        (JNIEnv *, jclass, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    registerResource1
    * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcPlatform/EntityHandler;I)Lorg/iotivity/base/OcResourceHandle;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_registerResource1
        (JNIEnv *, jclass, jstring, jstring, jstring, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    registerDeviceInfo0
    * Signature: (Ljava/lang/String;[Ljava/lang/String;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_registerDeviceInfo0
        (JNIEnv *, jclass, jstring, jobjectArray);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    registerPlatformInfo0
    * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_registerPlatformInfo0
        (JNIEnv *, jclass, jstring, jstring, jstring, jstring, jstring, jstring, jstring, jstring, jstring, jstring, jstring);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    getResourceHandleAtUri0
    * Signature: (Ljava/lang/String;)Lorg/iotivity/base/OcResourceHandle;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_getResourceHandleAtUri0
        (JNIEnv *, jclass, jstring);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    setPropertyValue0
    * Signature: (ILjava/lang/String;[Ljava/lang/String;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_setPropertyValue0
        (JNIEnv *, jclass, jint, jstring, jobjectArray);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    setPropertyValue1
    * Signature: (ILjava/lang/String;Ljava/lang/String;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_setPropertyValue1
        (JNIEnv *, jclass, jint, jstring, jstring);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    getPropertyValue0
    * Signature: (ILjava/lang/String;)Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcPlatform_getPropertyValue0
        (JNIEnv *, jclass, jint, jstring);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    unregisterResource0
    * Signature: (Lorg/iotivity/base/OcResourceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unregisterResource0
        (JNIEnv *, jclass, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    bindResource0
    * Signature: (Lorg/iotivity/base/OcResourceHandle;Lorg/iotivity/base/OcResourceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindResource0
        (JNIEnv *, jclass, jobject, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    bindResources0
    * Signature: (Lorg/iotivity/base/OcResourceHandle;[Lorg/iotivity/base/OcResourceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindResources0
        (JNIEnv *, jclass, jobject, jobjectArray);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    unbindResource0
    * Signature: (Lorg/iotivity/base/OcResourceHandle;Lorg/iotivity/base/OcResourceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unbindResource0
        (JNIEnv *, jclass, jobject, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    unbindResources0
    * Signature: (Lorg/iotivity/base/OcResourceHandle;[Lorg/iotivity/base/OcResourceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unbindResources0
        (JNIEnv *, jclass, jobject, jobjectArray);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    bindTypeToResource0
    * Signature: (Lorg/iotivity/base/OcResourceHandle;Ljava/lang/String;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindTypeToResource0
        (JNIEnv *, jclass, jobject, jstring);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    bindInterfaceToResource0
    * Signature: (Lorg/iotivity/base/OcResourceHandle;Ljava/lang/String;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindInterfaceToResource0
        (JNIEnv *, jclass, jobject, jstring);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    startPresence0
    * Signature: (I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_startPresence0
        (JNIEnv *, jclass, jint);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    stopPresence0
    * Signature: ()V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_stopPresence0
        (JNIEnv *, jclass);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    subscribePresence0
    * Signature: (Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnPresenceListener;)Lorg/iotivity/base/OcPresenceHandle;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_subscribePresence0
        (JNIEnv *, jclass, jstring, jint, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    subscribePresence1
    * Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnPresenceListener;)Lorg/iotivity/base/OcPresenceHandle;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_subscribePresence1
        (JNIEnv *, jclass, jstring, jstring, jint, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    unsubscribePresence0
    * Signature: (Lorg/iotivity/base/OcPresenceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unsubscribePresence0
        (JNIEnv *, jclass, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    subscribeDevicePresence0
    * Signature: (Ljava/lang/String;[Ljava/lang/String;I
    * Lorg/iotivity/base/OcResource/OnObserveListener;)Lorg/iotivity/base/OcPresenceHandle;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_subscribeDevicePresence0
        (JNIEnv *, jclass, jstring, jobjectArray, jint, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    constructResourceObject0
    * Signature: (Ljava/lang/String;Ljava/lang/String;IZ[Ljava/lang/String;[Ljava/lang/String;)Lorg/iotivity/base/OcResource;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_constructResourceObject0
        (JNIEnv *, jclass, jstring, jstring, jint, jboolean, jobjectArray, jobjectArray);

    /*
    * Class:     org_iotivity_base_OcPlatform0
    * Method:    sendResponse0
    * Signature: (Lorg/iotivity/base/OcResourceResponse;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_sendResponse0
        (JNIEnv *, jclass, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    constructAccountManagerObject0
    * Signature: (Ljava/lang/String;I)Lorg/iotivity/base/OcAccountManager;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_constructAccountManagerObject0
        (JNIEnv *, jclass, jstring, jint);

    /*
     * Class:     org_iotivity_base_OcPlatform
     * Method:    getDeviceId
     * Signature: (I)V
     */
    JNIEXPORT jbyteArray JNICALL Java_org_iotivity_base_OcPlatform_getDeviceId
        (JNIEnv *, jobject);

    /*
     * Class:     org_iotivity_base_OcPlatform
     * Method:    setDeviceId
     * Signature: (Ljava/lang/byte;)V
     */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_setDeviceId
        (JNIEnv *, jobject, jbyteArray);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    getIoTivityVersion
    * Signature: ()Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcPlatform_getIoTivityVersion
        (JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif
#endif
