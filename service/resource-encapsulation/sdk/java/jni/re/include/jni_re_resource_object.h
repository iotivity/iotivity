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

/** @file   jni_re_resource_object.h
 *
 *   @brief  This file contains the JniResourceObject class
 *               & declaration of RCSRemoteResourceObject APIs for JNI implementation
 */

#ifndef __JNI_RE_RCS_RESOURCE_OBJECT
#define __JNI_RE_RCS_RESOURCE_OBJECT

#include <jni.h>

#include "jni_re_jvm.h"
#include "RCSResourceObject.h"
#include "jni_re_utils.h"
#include "jni_re_get_request_listener.h"
#include "jni_re_set_request_listener.h"
#include "jni_re_attribute_updated_listener.h"
#include "jni_re_listener_manager_server.h"

using namespace OIC::Service;

/**
 * @class   JniRCSResourceObject
 * @brief   This class contains all the APIs for RCSResourceObject
 *
 * NOTE: JNI APIs internally call the APIs of this class.
 */
class JniRCSResourceObject
{
    public:

        /**
         * @brief constructor
         */
        JniRCSResourceObject(std::shared_ptr<RCSResourceObject> resource);

        /**
         * @brief destructor
        */
        ~JniRCSResourceObject();

        // ***** JNI APIs internally call the APIs of this class ***** //

        void setAttributeInteger(JNIEnv *env, jobject jObject, jstring key, jint value);
        void setAttributeDouble(JNIEnv *env, jobject jObject, jstring key, jdouble value);
        void setAttributeBool(JNIEnv *env, jobject jObject, jstring key, jboolean value);
        void setAttributeString(JNIEnv *env, jobject jObject, jstring key, jstring value);
        jstring getAttributeValue(JNIEnv *env, jstring jkey);
        bool removeAttribute(JNIEnv *env, jstring key);
        bool containsAttribute(JNIEnv *env, jstring key);
        jobject getAttributes(JNIEnv *env, jobject jObject);
        bool isObservable();
        bool isDiscoverable();
        void setGetRequestHandler(JNIEnv *env, jobject getRequestListener);
        void setSetRequestHandler(JNIEnv *env, jobject jSetRequestListener);
        void addAttributeUpdatedListener(JNIEnv *env, jstring key, jobject jAttributeUpdatedListener);
        bool removeAttributeUpdatedListener(JNIEnv *env, jobject jObject, jstring key);
        void notify();
        void setAutoNotifyPolicy(int policy);
        int getAutoNotifyPolicy();
        void setSetRequestHandlerPolicy(int policy);
        int getSetRequestHandlerPolicy();

        JniGetRequestListener *addOnGetRequestListener(JNIEnv *env, jobject jListener);
        JniSetRequestListener *addOnSetRequestListener(JNIEnv *env, jobject jListener);
        JniAttributeUpdatedListener *addOnAttributeUpdatedListener(JNIEnv *env, jobject jListener);

        void removeOnGetRequestListener(JNIEnv *env, jobject jListener);
        void removeOnSetRequestListener(JNIEnv *env, jobject jListener);
        void removeOnAttributeUpdatedListener(JNIEnv *env, jobject jListener);

        static JniRCSResourceObject *getJniRCSResourceObject(JNIEnv *env, jobject thiz);

    private:

        JniReListenerManagerServer<JniGetRequestListener> m_onGetRequest;
        JniReListenerManagerServer<JniSetRequestListener> m_onSetRequest;
        JniReListenerManagerServer<JniAttributeUpdatedListener> m_onAttributeUpdated;

        std::shared_ptr<RCSResourceObject> m_sharedResource;

};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief API for setting a particular attribute value as a integer
 *
 * @param key name of attribute(used to map the attribute value).
 * @param value integer value to be mapped against the key.
 *
 * It is guaranteed thread-safety about attributes.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetAttributeInteger
(JNIEnv *env, jobject jObject, jstring key, jint value);

/**
 * @brief API for setting a particular attribute value as a double
 *
 * @param key name of attribute(used to map the attribute value).
 * @param value Double value to be mapped against the key.
 *
 * It is guaranteed thread-safety about attributes.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetAttributeDouble
(JNIEnv *env, jobject jObject, jstring key, jdouble value);

/**
 * @brief API for setting a particular attribute value as a boolean
 *
 * @param key name of attribute(used to map the attribute value).
 * @param value boolean value to be mapped against the key.
 *
 * It is guaranteed thread-safety about attributes.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetAttributeBool
(JNIEnv *env, jobject jObject, jstring key, jboolean value);

/**
 * @brief API for setting a particular attribute value as a String
 *
 * @param key name of attribute(used to map the attribute value).
 * @param value String value to be mapped against the key.
 *
 * It is guaranteed thread-safety about attributes.
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetAttributeString
(JNIEnv *env, jobject jObject, jstring key, jstring value);

/**
 * @brief API for retrieving the attribute value associated with the supplied key.
 *
 * @param key Name of the attribute
 *
 * @return resource attributes value as a string
 *
 * It is guaranteed thread-safety about attributes.
 */
JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeGetAttributeValue
(JNIEnv *env, jobject jObject, jstring key);

/**
 * @brief API for removing a particular attribute of the resource.
 *
 * @param key Name of the attribute.
 *
 * @return If the key exist and matched attribute is deleted, return true.
 *
 * It is guaranteed thread-safety about attributes.
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeRemoveAttribute
(JNIEnv *env, jobject jObject, jstring key);

/**
 * @brief API for checking whether a particular attribute is there for a resource or not.
 *
 * @param key Name of the attribute.
 *
 * @return If the key exist, return true.
 *
 * It is guaranteed thread-safety about attributes.
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeContainsAttribute
(JNIEnv *env, jobject jObject, jstring key);


/**
 * @brief API for getting all the attributes of the RCSResourceObject.
 *
 * @return  RCSResourceAttributes
 *
 * @see RCSResourceAttributes
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeGetAttributes
(JNIEnv *env, jobject jObject);

/**
* @brief API for checking whether the particular resource is observable or not
*/
JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeIsObservable
(JNIEnv *env, jobject jObject);

/**
* @brief API for checking whether the particular resource is discoverable or not
*/
JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeIsDiscoverable
(JNIEnv *env, jobject jObject);

/**
 * API for setting the resource's get request handler by the developer/application.
 * If developer set this handler then all get request will come to the application &
 * developer can send the response to the client using APIs of RCSGetResponse class.
 *
 * @param jGetRequestListener Request handler for get requests
 *
 * @see RCSGetResponse
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetGetRequestHandler
(JNIEnv *env, jobject jObject, jobject jGetRequestListener);

/**
 * API for setting the resource's set request handler by the developer/application.
 * If developer set this handler then all set request for the resource
 * will come to the application & developer can send the response to the client
 * using APIs of RCSSetResponse class.
 *
 * @param jSetRequestListener Request handler for set requests
 *
 * @see RCSSetResponse
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetSetRequestHandler
(JNIEnv *env, jobject jObject, jobject jSetRequestListener);

/**
 * API for setting the Listener for a particular attribute update.
 *
 * @param key The interested attribute's key
 * @param jAttributeUpdatedListener Listener for updation of the interested attribute
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeAddAttributeUpdatedListener
(JNIEnv *env, jobject jObject, jstring key, jobject jAttributeUpdatedListener);

/**
* API for removing the handler for a particular attribute update.
*
* @param key The interested attribute's key
*
*/
JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeRemoveAttributeUpdatedListener
(JNIEnv *env, jobject jObject, jstring key);

/**
 * API for notifying all observers of the RCSResourceObject
 * with the updated attributes value
 */
JNIEXPORT void JNICALL Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeNotify
(JNIEnv *env, jobject jObject);

/**
* API for setting Auto notify policy
*
* @param policy policy to be set (Its datatype is jint we will convert it to AutoNotifyPolicy enum value using jni_re_utils)
*
* @see AutoNotifyPolicy
* @see jni_re_utils
*
*/
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetAutoNotifyPolicy
(JNIEnv *env, jobject jObject, jint policy);

/**
* API for getting auto notify policy
*
* @returns jint AntoNotify policy as a integer in java it will be converted to AutoNotifyPolicy enum.
*
* @see AutoNotifyPolicy
*
*/
JNIEXPORT jint JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeGetAutoNotifyPolicy
(JNIEnv *env, jobject jObject);

/**
* API for setting the policy for a setRequestHandler.
*
* @param policy policy to be set (Its datatype is jint we will convert it to SetRequestHandlerPolicy enum value using jni_re_utils)
*
* @see SetRequestHandlerPolicy
* @see jni_re_utils
*
*/
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetSetRequestHandlerPolicy
(JNIEnv *env, jobject jObject, jint policy);

/**
* API for getting the SetRequestHandler Policy.
*
* @returns jint SetRequestHandlerPolicy policy as a integer in java it will be converted to SetRequestHandlerPolicy enum.
*
* @see SetRequestHandlerPolicy
*
*/
JNIEXPORT jint JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeGetSetRequestHandlerPolicy
(JNIEnv *env, jobject jObject);

#ifdef __cplusplus
}
#endif
#endif


