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
#include "OCResource.h"
#include "JniListenerManager.h"
#include "JniOnGetListener.h"
#include "JniOnPutListener.h"
#include "JniOnPostListener.h"
#include "JniOnDeleteListener.h"
#include "JniOnObserveListener.h"
#include "JniOcResourceIdentifier.h"
#ifdef WITH_MQ
#include "JniOnMQTopicFoundListener.h"
#include "JniOnMQSubscribeListener.h"
#endif

#ifndef _Included_org_iotivity_base_OcResource
#define _Included_org_iotivity_base_OcResource

class JniOcResource
{
public:
    JniOcResource(std::shared_ptr<OC::OCResource> resource);
    ~JniOcResource();

    OCStackResult get(JNIEnv* env, const OC::QueryParamsMap &queryParametersMap, jobject jListener);
    OCStackResult get(JNIEnv* env, const OC::QueryParamsMap &queryParametersMap, jobject jListener,
        OC::QualityOfService QoS);
    OCStackResult get(JNIEnv* env, const std::string &resourceType, const std::string &resourceInterface,
        const OC::QueryParamsMap &queryParametersMap, jobject jListener);
    OCStackResult get(JNIEnv* env, const std::string &resourceType, const std::string &resourceInterface,
        const OC::QueryParamsMap &queryParametersMap, jobject jListener, OC::QualityOfService QoS);

    OCStackResult put(JNIEnv* env, const OC::OCRepresentation &representation, const OC::QueryParamsMap &queryParametersMap,
        jobject jListener);
    OCStackResult put(JNIEnv* env, const OC::OCRepresentation &representation, const OC::QueryParamsMap &queryParametersMap,
        jobject jListener, OC::QualityOfService QoS);
    OCStackResult put(JNIEnv* env, const std::string &resourceType, const std::string &resourceInterface,
        const OC::OCRepresentation &representation, const OC::QueryParamsMap &queryParametersMap, jobject jListener);
    OCStackResult put(JNIEnv* env, const std::string &resourceType, const std::string &resourceInterface,
        const OC::OCRepresentation &representation, const OC::QueryParamsMap &queryParametersMap, jobject jListener, OC::QualityOfService QoS);

    OCStackResult post(JNIEnv* env, const OC::OCRepresentation &representation, const OC::QueryParamsMap &queryParametersMap,
        jobject jListener);
    OCStackResult post(JNIEnv* env, const OC::OCRepresentation &representation, const OC::QueryParamsMap &queryParametersMap,
        jobject jListener, OC::QualityOfService QoS);
    OCStackResult post(JNIEnv* env, const std::string &resourceType, const std::string &resourceInterface,
        const OC::OCRepresentation &representation, const OC::QueryParamsMap &queryParametersMap, jobject jListener);
    OCStackResult post(JNIEnv* env, const std::string &resourceType, const std::string &resourceInterface,
        const OC::OCRepresentation &representation, const OC::QueryParamsMap &queryParametersMap, jobject jListener, OC::QualityOfService QoS);

    OCStackResult deleteResource(JNIEnv* env, jobject jListener);
    OCStackResult deleteResource(JNIEnv* env, jobject jListener, OC::QualityOfService QoS);

    OCStackResult observe(JNIEnv* env, OC::ObserveType observeType, const OC::QueryParamsMap &queryParametersMap,
        jobject jListener);
    OCStackResult observe(JNIEnv* env, OC::ObserveType observeType, const OC::QueryParamsMap &queryParametersMap,
        jobject jListener, OC::QualityOfService qos);

    OCStackResult cancelObserve(JNIEnv* env);
    OCStackResult cancelObserve(JNIEnv* env, OC::QualityOfService qos);

    void setHeaderOptions(const OC::HeaderOptions &headerOptions);
    void unsetHeaderOptions();
    std::string host();
    std::vector< std::string > getAllHosts() const;
    std::string setHost(const std::string& host);
    std::string uri();
    OCConnectivityType connectivityType() const;
    bool isObservable();
    std::vector< std::string >  getResourceTypes() const;
    std::vector< std::string >  getResourceInterfaces(void) const;
    OC::OCResourceIdentifier uniqueIdentifier() const;
    std::string sid() const;

    JniOnGetListener* addOnGetListener(JNIEnv* env, jobject jListener);
    JniOnPutListener* addOnPutListener(JNIEnv* env, jobject jListener);
    JniOnPostListener* addOnPostListener(JNIEnv* env, jobject jListener);
    JniOnDeleteListener* addOnDeleteListener(JNIEnv* env, jobject jListener);
    JniOnObserveListener* addOnObserveListener(JNIEnv* env, jobject jListener);

    void removeOnGetListener(JNIEnv* env, jobject jListener);
    void removeOnPutListener(JNIEnv* env, jobject jListener);
    void removeOnPostListener(JNIEnv* env, jobject jListener);
    void removeOnDeleteListener(JNIEnv* env, jobject jListener);
    void removeOnObserveListener(JNIEnv* env, jobject jListener);

    std::shared_ptr<OC::OCResource> getOCResource();

    static JniOcResource* getJniOcResourcePtr(JNIEnv *env, jobject thiz);

#ifdef WITH_MQ
    JniOnMQTopicFoundListener* addOnTopicFoundListener(JNIEnv* env, jobject jListener);
    void removeOnTopicFoundListener(JNIEnv* env, jobject jListener);

    JniOnMQSubscribeListener* addOnMQTopicSubscribeListener(JNIEnv* env, jobject jListener);
    void removeOnMQTopicSubscribeListener(JNIEnv* env, jobject jListener);

    OCStackResult discoveryMQTopics(JNIEnv* env, const OC::QueryParamsMap &queryParametersMap,
                                    jobject jListener, OC::QualityOfService QoS);
    OCStackResult createMQTopic(JNIEnv* env, const OC::OCRepresentation &representation,
                                const std::string &targetUri,
                                const OC::QueryParamsMap &queryParametersMap,
                                jobject jListener, OC::QualityOfService QoS);
#endif
#ifdef MQ_SUBSCRIBER
    OCStackResult subscribeMQTopic(JNIEnv* env, const OC::QueryParamsMap &queryParametersMap,
                                   jobject jListener, OC::QualityOfService QoS);
    OCStackResult unsubscribeMQTopic(OC::QualityOfService QoS);
    OCStackResult requestMQPublish(JNIEnv* env, const OC::QueryParamsMap &queryParametersMap,
                                   jobject jListener, OC::QualityOfService QoS);
#endif
#ifdef MQ_PUBLISHER
    OCStackResult publishMQTopic(JNIEnv* env, const OC::OCRepresentation &representation,
                                 const OC::QueryParamsMap &queryParametersMap,
                                 jobject jListener, OC::QualityOfService QoS);
#endif

private:
    JniListenerManager<JniOnGetListener> m_onGetManager;
    JniListenerManager<JniOnPutListener> m_onPutManager;
    JniListenerManager<JniOnPostListener> m_onPostManager;
    JniListenerManager<JniOnDeleteListener> m_onDeleteManager;
    JniListenerManager<JniOnObserveListener> m_onObserveManager;
#ifdef WITH_MQ
    JniListenerManager<JniOnMQTopicFoundListener> m_onFoundTopicResourceManager;
    JniListenerManager<JniOnMQSubscribeListener> m_onSubcribeTopicManager;
#endif

    std::shared_ptr<OC::OCResource> m_sharedResource;
};

/* DO NOT EDIT THIS FILE BEYOND THIS LINE - it is machine generated */

#ifdef __cplusplus
extern "C" {
#endif

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    get
    * Signature: (Ljava/util/Map;Lorg/iotivity/base/OcResource/OnGetListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_get
        (JNIEnv *, jobject, jobject, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    get1
    * Signature: (Ljava/util/Map;Lorg/iotivity/base/OcResource/OnGetListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_get1
        (JNIEnv *, jobject, jobject, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    get2
    * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;Lorg/iotivity/base/OcResource/OnGetListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_get2
        (JNIEnv *, jobject, jstring, jstring, jobject, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    get3
    * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;Lorg/iotivity/base/OcResource/OnGetListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_get3
        (JNIEnv *, jobject, jstring, jstring, jobject, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    put
    * Signature: (Lorg/iotivity/base/OcRepresentation;Ljava/util/Map;Lorg/iotivity/base/OcResource/OnPutListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_put
        (JNIEnv *, jobject, jobject, jobject, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    put1
    * Signature: (Lorg/iotivity/base/OcRepresentation;Ljava/util/Map;Lorg/iotivity/base/OcResource/OnPutListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_put1
        (JNIEnv *, jobject, jobject, jobject, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    put2
    * Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcRepresentation;Ljava/util/Map;Lorg/iotivity/base/OcResource/OnPutListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_put2
        (JNIEnv *, jobject, jstring, jstring, jobject, jobject, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    put3
    * Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcRepresentation;Ljava/util/Map;Lorg/iotivity/base/OcResource/OnPutListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_put3
        (JNIEnv *, jobject, jstring, jstring, jobject, jobject, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    post
    * Signature: (Lorg/iotivity/base/OcRepresentation;Ljava/util/Map;Lorg/iotivity/base/OcResource/OnPostListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_post
        (JNIEnv *, jobject, jobject, jobject, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    post1
    * Signature: (Lorg/iotivity/base/OcRepresentation;Ljava/util/Map;Lorg/iotivity/base/OcResource/OnPostListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_post1
        (JNIEnv *, jobject, jobject, jobject, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    post2
    * Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcRepresentation;Ljava/util/Map;Lorg/iotivity/base/OcResource/OnPostListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_post2
        (JNIEnv *, jobject, jstring, jstring, jobject, jobject, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    post3
    * Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcRepresentation;Ljava/util/Map;Lorg/iotivity/base/OcResource/OnPostListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_post3
        (JNIEnv *, jobject, jstring, jstring, jobject, jobject, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    deleteResource
    * Signature: (Lorg/iotivity/base/OcResource/OnDeleteListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_deleteResource
        (JNIEnv *, jobject, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    deleteResource1
    * Signature: (Lorg/iotivity/base/OcResource/OnDeleteListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_deleteResource1
        (JNIEnv *, jobject, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    observe
    * Signature: (Lorg/iotivity/base/ObserveType;Ljava/util/Map;Lorg/iotivity/base/OcResource/OnObserveListener;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_observe
        (JNIEnv *, jobject, jint, jobject, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    observe1
    * Signature: (Lorg/iotivity/base/ObserveType;Ljava/util/Map;Lorg/iotivity/base/OcResource/OnObserveListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_observe1
        (JNIEnv *, jobject, jint, jobject, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    cancelObserve1
    * Signature: (I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_cancelObserve1
        (JNIEnv *, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    setHeaderOptions
    * Signature: ([Lorg/iotivity/OcHeaderOption;)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_setHeaderOptions
        (JNIEnv *, jobject, jobjectArray);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    unsetHeaderOptions
    * Signature: ()V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_unsetHeaderOptions
        (JNIEnv *, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    getHost
    * Signature: ()Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcResource_getHost
        (JNIEnv *, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    getAllHosts
    * Signature: ()Ljava/util/List;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcResource_getAllHosts
        (JNIEnv *, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    setHost
    * Signature: (Ljava/lang/String;)Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcResource_setHost
        (JNIEnv *, jobject, jstring);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    getUri
    * Signature: ()Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcResource_getUri
        (JNIEnv *, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    getConnectivityTypeN
    * Signature: ()I
    */
    JNIEXPORT jint JNICALL Java_org_iotivity_base_OcResource_getConnectivityTypeN
        (JNIEnv *, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    isObservable
    * Signature: ()Z
    */
    JNIEXPORT jboolean JNICALL Java_org_iotivity_base_OcResource_isObservable
        (JNIEnv *, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    getResourceTypes
    * Signature: ()Ljava/util/List;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcResource_getResourceTypes
        (JNIEnv *, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    getResourceInterfaces
    * Signature: ()Ljava/util/List;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcResource_getResourceInterfaces
        (JNIEnv *, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    getUniqueIdentifier
    * Signature: ()Lorg/iotivity/base/OcResourceIdentifier;
    */
    JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcResource_getUniqueIdentifier
        (JNIEnv *, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    getServerId
    * Signature: ()Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcResource_getServerId
        (JNIEnv *, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    dispose
    * Signature: ()V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_dispose
        (JNIEnv *, jobject);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    subscribeMQTopicImpl
    * Signature: (Ljava/util/Map;Lorg/iotivity/base/OcResource/OnObserveListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_subscribeMQTopicImpl
        (JNIEnv *, jobject, jobject, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    unsubscribeMQTopicImpl
    * Signature: (I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_unsubscribeMQTopicImpl
        (JNIEnv *, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    requestMQPublishImpl
    * Signature: (Ljava/util/Map;Lorg/iotivity/base/OcResource/OnPostListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_requestMQPublishImpl
        (JNIEnv *, jobject, jobject, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    publishMQTopicImpl
    * Signature: (Lorg/iotivity/base/OcRepresentation;Ljava/util/Map
    *             ;Lorg/iotivity/base/OcResource/OnPostListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_publishMQTopicImpl
        (JNIEnv *, jobject, jobject, jobject, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    discoveryMQTopicsImpl
    * Signature: (Ljava/util/Map;Lorg/iotivity/base/OcResource/OnMQTopicFoundListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_discoveryMQTopicsImpl
        (JNIEnv *, jobject, jobject, jobject, jint);

    /*
    * Class:     org_iotivity_base_OcResource
    * Method:    createMQTopicImpl
    * Signature: (Lorg/iotivity/base/OcRepresentation;Ljava/util/String;Ljava/util/Map
    *             ;Lorg/iotivity/base/OcPlatform/OnMQTopicCreatedListener;I)V
    */
    JNIEXPORT void JNICALL Java_org_iotivity_base_OcResource_createMQTopicImpl
        (JNIEnv *, jobject, jobject, jstring, jobject, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
