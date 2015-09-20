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

/** @file   jni_re_rcs_remoteresourceobject.h
 *
 *   @brief  This file contains the JniRCSRemoteResourceObject class
 *               & declaration of RCSRemoteResourceObject APIs for JNI implementation
 */

#ifndef _JNI_RE_RCS_REMOTE_RESOURCE_OBJECT_H_
#define _JNI_RE_RCS_REMOTE_RESOURCE_OBJECT_H_

#include "RCSRemoteResourceObject.h"

#include "jni_re_jvm.h"
#include "jni_re_utils.h"
#include "jni_re_listener_manager.h"
#include "jni_re_resource_statechange_listener.h"
#include "jni_re_cache_update_listener.h"
#include "jni_re_getattributes_listener.h"
#include "jni_re_setattributes_listener.h"

using namespace OIC::Service;

/**
 * @class   JniRCSRemoteResourceObject
 * @brief   This class contains all the APIs for RCSRemoteResourceObject
 *
 * NOTE: JNI APIs internally call the APIs of this class.
 */
class JniRCSRemoteResourceObject
{
    public:

        /**
         * @brief constructor
         */
        JniRCSRemoteResourceObject(std::shared_ptr< RCSRemoteResourceObject> resource);

        /**
         * @brief destructor
         */
        ~JniRCSRemoteResourceObject();

        // ***** JNI APIs internally call the APIs of this class ***** //

        bool isMonitoring();
        bool isCaching();
        bool isObservable();
        void startMonitoring(JNIEnv *env, jobject jListener) ;
        void stopMonitoring();
        int getState();
        void startCaching(JNIEnv *env, jobject jListener);
        void startCaching(JNIEnv *env);
        void stopCaching();
        int getCacheState();
        bool isCachedAvailable();
        std::string getUri();
        std::string getAddress();
        jobject getInterfaces(JNIEnv *env);
        jobject getTypes(JNIEnv *env);
        void getRemoteAttributes(JNIEnv *env, jobject jListener);
        void setRemoteAttributes(JNIEnv *env, jobject attribute, jobject jListener);
        jobject getCachedAttributes(JNIEnv *env);
        jstring getCachedAttribute(JNIEnv *env, jstring key, jobject object);

        JniResourceStateChangeListener *addOnStateChangeListener(JNIEnv *env, jobject jListener);
        JniCacheUpdateListener *addOnCacheUpdateListener(JNIEnv *env, jobject jListener);
        JniGetAttributesListener *addOngetAttributesListener(JNIEnv *env, jobject jListener);
        JniSetAttributesListener *addOnsetAttributesListener(JNIEnv *env, jobject jListener);

        void removeOnStateChangeListener(JNIEnv *env, jobject jListener);
        void removeCacheUpdateListener(JNIEnv *env, jobject jListener);
        void removeOngetAttributesListener(JNIEnv *env, jobject jListener);
        void removeOnsetAttributesListener(JNIEnv *env, jobject jListener);

        std::shared_ptr< RCSRemoteResourceObject> getRCSResource();

        static JniRCSRemoteResourceObject *getJniRCSRemoteResourceObject(JNIEnv *env, jobject thiz);

    private:
        JniReListenerManager<JniResourceStateChangeListener> m_onStateChange;
        JniReListenerManager<JniCacheUpdateListener> m_onCacheUpdateChange;
        JniReListenerManager<JniGetAttributesListener> m_ongetAttributes;
        JniReListenerManager<JniSetAttributesListener> m_onsetAttributes;

        std::shared_ptr< RCSRemoteResourceObject> m_sharedResource;

};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * API for checking whether monitoring is enabled or not.
 *
 * @return jboolean - boolean value (true/false)
 *
 * NOTE: If monitoring is started returns true otherwise false
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeIsMonitoring
(JNIEnv *env, jobject interfaceClass);

/**
 * API for checking whether Caching is enabled or not.
 *
 * @return jboolean - boolean value (true/false)
 *
 * NOTE: If Caching is started returns true otherwise false
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeIsCaching
(JNIEnv *env, jobject interfaceClass);

/**
 * API for checking whether resource is observable or not.
 *
 * @return jboolean - boolean value (true/false)
 *
 * NOTE: If resource is observable returns true otherwise false
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeIsObservable
(JNIEnv *env, jobject interfaceClass);

/**
 * Starts monitoring the resource.
 *
 * Monitoring provides a feature to check the presence of a resource,
 * even when the server is not announcing Presence using startPresnece.
 *
 * @param stateChangedListener - Listener to obtain the changed resource state
 *
 * @throws RCSException If monitoring is already started.
 *
 * @note The callback will be invoked in an internal thread.
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeStartMonitoring
(JNIEnv *env, jobject interfaceClass, jobject stateChangedListener);

/**
 * Stops monitoring the resource.
 *
 * It does nothing if monitoring is not started.
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeStopMonitoring
(JNIEnv *env, jobject interfaceClass);

/**
 * Returns the current state of the resource.
 *
 */
JNIEXPORT jint JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetState
(JNIEnv *env, jobject interfaceClass);

/**
 * Starts caching attributes of the resource.
 *
 * This will start data caching for the resource.
 * Once caching started it will look for the data updation on the resource.
 * If this method is called with null listener it will not notify the updated value to caller.
 * It it is called with a valid listener updated cached values will be notified to caller.
 *
 * @param cacheUpdateListener - Listener to obtain the updated cached data
 *
 * @throws RCSException If Caching is already started
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeStartCaching
(JNIEnv *env, jobject interfaceClass, jobject cacheUpdateListener);

/**
 * Stops caching.
 *
 * It does nothing if caching is not started.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeStopCaching
(JNIEnv *env, jobject interfaceClass);

/**
 * Returns the current cache state.
 *
 */
JNIEXPORT jint JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetCacheState
(JNIEnv *env, jobject interfaceClass);

/**
 * Returns whether cached data is available.
 *
 * Cache will be available always after CacheState::READY even if current state is
 * LOST_SIGNAL.
 *
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeIsCachedAvailable
(JNIEnv *env, jobject interfaceClass);

/**
 * Gets the cached RCSResourceAttributes data.
 *
 * @pre Cache should be available.
 *
 * @return The cached attributes.
 *
 * @throws RCSException If the precondition is not fulfilled.
 *
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetCachedAttributes
(JNIEnv *env, jobject interfaceClass);

/**
 * Gets a particular cached a ResourceAttribute Value.
 *
 * @pre Cache should be available.
 *
 * @param key - Attribute Key
 *
 * @return Requested attribute value.
 *
 * @throws RCSException If the precondition is not fulfilled or key doesn't match the key of any value.
 *
 */
JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetCachedAttribute
(JNIEnv *env, jobject interfaceClass, jstring key);

/**
 * Gets resource attributes directly from the server.
 *
 * This API send a get request to the resource of interest and provides
 * the attributes to the caller in the RemoteAttributesReceivedCallback.
 *
 * @param resourceAttributesListener - Listener to get the resource attributes
 *
 * @note The callback will be invoked in an internal thread.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetRemoteAttributes
(JNIEnv *env, jobject interfaceClass, jobject resourceAttributesListener);

/**
 * Sends a set request with resource attributes to the server.
 *
 * The SetRequest behavior depends on the server, whether updating its attributes or not.
 *
 * @param resourceAttributes -  Attributes to set
 * @param resourceAttributesListener - Listener to obtain the updated cached data
 *
 * @note The callback will be invoked in an internal thread.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeSetRemoteAttributes
(JNIEnv *env, jobject interfaceClass, jobject resourceAttributes,
 jobject resourceAttributesListener);

/**
 * Returns the uri of the resource.
 *
 */
JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetUri
(JNIEnv *env, jobject interfaceClass);

/**
 * Returns the address of the resource .
 *
 */
JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetAddress
(JNIEnv *env, jobject interfaceClass);

/**
 * Returns the resource types of the resource.
 *
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetTypes
(JNIEnv *env, jobject interfaceClass);

/**
 * Returns the resource interfaces of the resource.
 *
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetInterFaces
(JNIEnv *env, jobject interfaceClass);

#ifdef __cplusplus
}
#endif
#endif // _JNI_RE_RCS_REMOTE_RESOURCE_OBJECT_H_
