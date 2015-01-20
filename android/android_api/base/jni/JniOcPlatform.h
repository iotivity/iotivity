/*
* //******************************************************************
* //
* // Copyright 2015 Intel Corporation.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
* //
* // Licensed under the Apache License, Version 2.0 (the "License");
* // you may not use this file except in compliance with the License.
* // You may obtain a copy of the License at
* //
* //      http://www.apache.org/licenses/LICENSE-2.0
* //
* // Unless required by applicable law or agreed to in writing, software
* // distributed under the License is distributed on an "AS IS" BASIS,
* // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* // See the License for the specific language governing permissions and
* // limitations under the License.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
#include <jni.h>
#include "JniOcStack.h"
#include "JniOnResourceFoundListener.h"
#include "JniOnDeviceInfoListener.h"
#include "JniOnPresenceListener.h"
#include <mutex>

#ifndef _Included_org_iotivity_base_OcPlatform
#define _Included_org_iotivity_base_OcPlatform

using namespace OC;

JniOnResourceFoundListener* AddOnResourceFoundListener(JNIEnv* env, jobject jListener);

JniOnDeviceInfoListener* AddOnDeviceInfoListener(JNIEnv* env, jobject jListener);
void RemoveOnDeviceInfoListener(JNIEnv* env, jobject jListener);

JniOnPresenceListener* AddOnPresenceListener(JNIEnv* env, jobject jListener);
void RemoveOnPresenceListener(JNIEnv* env, jobject jListener);

std::map<jobject, std::pair<JniOnResourceFoundListener*, int>> onResourceFoundListenerMap;
std::map<jobject, std::pair<JniOnDeviceInfoListener*, int>> onDeviceInfoListenerMap;
std::map<jobject, std::pair<JniOnPresenceListener*, int>> onPresenceListenerMap;

std::mutex resourceFoundMapMutex;
std::mutex deviceInfoMapLock;
std::mutex onPresenceMapLock;


#ifdef __cplusplus
extern "C" {
#endif
    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    configure
    * Signature: (IILjava/lang/String;II)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_configure
        (JNIEnv *, jclass, jint, jint, jstring, jint, jint);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    notifyAllObservers
    * Signature: (Lorg/iotivity/base/OcResourceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_notifyAllObservers
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
    * Method:    findResource
    * Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcPlatform/OnResourceFoundListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findResource
        (JNIEnv *, jclass, jstring, jstring, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    findResource1
    * Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcPlatform/OnResourceFoundListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findResource1
        (JNIEnv *, jclass, jstring, jstring, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    getDeviceInfo
    * Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcPlatform/OnDeviceFoundListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getDeviceInfo
        (JNIEnv *, jclass, jstring, jstring, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    getDeviceInfo1
    * Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcPlatform/OnDeviceFoundListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getDeviceInfo1
        (JNIEnv *, jclass, jstring, jstring, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    registerResource
    * Signature: (Lorg/iotivity/base/OcResource;)Lorg/iotivity/base/OcResourceHandle;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_registerResource
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
    * Method:    registerDeviceInfo
    * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_registerDeviceInfo
        (JNIEnv *, jclass, jstring, jstring, jstring, jstring, jstring, jstring, jstring, jstring, jstring, jstring, jstring, jstring);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    unregisterResource
    * Signature: (Lorg/iotivity/base/OcResourceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unregisterResource
        (JNIEnv *, jclass, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    bindResource
    * Signature: (Lorg/iotivity/base/OcResourceHandle;Lorg/iotivity/base/OcResourceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindResource
        (JNIEnv *, jclass, jobject, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    bindResources
    * Signature: (Lorg/iotivity/base/OcResourceHandle;[Lorg/iotivity/base/OcResourceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindResources
        (JNIEnv *, jclass, jobject, jobjectArray);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    unbindResource
    * Signature: (Lorg/iotivity/base/OcResourceHandle;Lorg/iotivity/base/OcResourceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unbindResource
        (JNIEnv *, jclass, jobject, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    unbindResources
    * Signature: (Lorg/iotivity/base/OcResourceHandle;[Lorg/iotivity/base/OcResourceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unbindResources
        (JNIEnv *, jclass, jobject, jobjectArray);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    bindTypeToResource
    * Signature: (Lorg/iotivity/base/OcResourceHandle;Ljava/lang/String;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindTypeToResource
        (JNIEnv *, jclass, jobject, jstring);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    bindInterfaceToResource
    * Signature: (Lorg/iotivity/base/OcResourceHandle;Ljava/lang/String;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindInterfaceToResource
        (JNIEnv *, jclass, jobject, jstring);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    startPresence
    * Signature: (I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_startPresence
        (JNIEnv *, jclass, jint);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    stopPresence
    * Signature: ()V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_stopPresence
        (JNIEnv *, jclass);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    subscribePresence
    * Signature: (Ljava/lang/String;Lorg/iotivity/base/OcPlatform/OnPresenceListener;)Lorg/iotivity/base/OcPresenceHandle;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_subscribePresence
        (JNIEnv *, jclass, jstring, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    subscribePresence1
    * Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcPlatform/OnPresenceListener;)Lorg/iotivity/base/OcPresenceHandle;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_subscribePresence1
        (JNIEnv *, jclass, jstring, jstring, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    unsubscribePresence
    * Signature: (Lorg/iotivity/base/OcPresenceHandle;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unsubscribePresence
        (JNIEnv *, jclass, jobject);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    constructResourceObject
    * Signature: (Ljava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;)Lorg/iotivity/base/OcResource;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_constructResourceObject
        (JNIEnv *, jclass, jstring, jstring, jboolean, jobjectArray, jobjectArray);

    /*
    * Class:     org_iotivity_base_OcPlatform
    * Method:    sendResponse
    * Signature: (Lorg/iotivity/base/OcResourceResponse;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_sendResponse
        (JNIEnv *, jclass, jobject);

#ifdef __cplusplus
}
#endif
#endif
