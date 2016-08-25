/*
 *******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

#include "JniOcAccountManager.h"
#include "JniOcRepresentation.h"
#include "JniUtils.h"

JniOcAccountManager::JniOcAccountManager(std::shared_ptr<OCAccountManager> accountManager)
    : m_sharedAccountManager(accountManager)
{
}

JniOcAccountManager::~JniOcAccountManager()
{
    LOGD("~JniOcAccountManager()");

    m_sharedAccountManager = nullptr;

    jint envRet = JNI_ERR;
    JNIEnv *env = GetJNIEnv(envRet);
    if (nullptr == env)
    {
        return;
    }

    m_onGetManager.removeAllListeners(env);
    m_onPostManager.removeAllListeners(env);
    m_onDeleteManager.removeAllListeners(env);
    m_onObserveManager.removeAllListeners(env);

    if (JNI_EDETACHED == envRet)
    {
        g_jvm->DetachCurrentThread();
    }
}

std::string JniOcAccountManager::host()
{
    return m_sharedAccountManager->host();
}

OCConnectivityType JniOcAccountManager::connectivityType() const
{
    return m_sharedAccountManager->connectivityType();
}

JniOcAccountManager* JniOcAccountManager::getJniOcAccountManagerPtr(JNIEnv *env, jobject thiz)
{
    JniOcAccountManager *accountManager = GetHandle<JniOcAccountManager>(env, thiz);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to get native handle from OcAccountManager");
    }
    if (!accountManager)
    {
        ThrowOcException(JNI_NO_NATIVE_POINTER, "");
    }
    return accountManager;
}

JniOnGetListener* JniOcAccountManager::addOnGetListener(JNIEnv* env, jobject jListener)
{
    return this->m_onGetManager.addListener(env, jListener, this);
}

JniOnPostListener* JniOcAccountManager::addOnPostListener(JNIEnv* env, jobject jListener)
{
    return this->m_onPostManager.addListener(env, jListener, this);
}

JniOnDeleteListener* JniOcAccountManager::addOnDeleteListener(JNIEnv* env, jobject jListener)
{
    return this->m_onDeleteManager.addListener(env, jListener, this);
}

JniOnObserveListener* JniOcAccountManager::addOnObserveListener(JNIEnv* env, jobject jListener)
{
    return this->m_onObserveManager.addListener(env, jListener, this);
}

void JniOcAccountManager::removeOnGetListener(JNIEnv* env, jobject jListener)
{
    this->m_onGetManager.removeListener(env, jListener);
}

void JniOcAccountManager::removeOnPostListener(JNIEnv* env, jobject jListener)
{
    this->m_onPostManager.removeListener(env, jListener);
}

void JniOcAccountManager::removeOnDeleteListener(JNIEnv* env, jobject jListener)
{
    this->m_onDeleteManager.removeListener(env, jListener);
}

void JniOcAccountManager::removeOnObserveListener(JNIEnv* env, jobject jListener)
{
    this->m_onObserveManager.removeListener(env, jListener);
}

OCStackResult JniOcAccountManager::signUp(JNIEnv* env, const std::string& authProvider,
                                          const std::string& authCode, jobject jListener)
{
    JniOnPostListener *onPostListener = addOnPostListener(env, jListener);

    PostCallback postCallback = [onPostListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onPostListener->onPostCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->signUp(authProvider, authCode, postCallback);
}

OCStackResult JniOcAccountManager::signUp(JNIEnv* env, const std::string& authProvider,
                                          const std::string& authCode,
                                          const QueryParamsMap& options, jobject jListener)
{
    JniOnPostListener *onPostListener = addOnPostListener(env, jListener);

    PostCallback postCallback = [onPostListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onPostListener->onPostCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->signUp(authProvider, authCode, options, postCallback);
}

OCStackResult JniOcAccountManager::signIn(JNIEnv* env, const std::string& userUuid,
                                          const std::string& accessToken, jobject jListener)
{
    JniOnPostListener *onPostListener = addOnPostListener(env, jListener);

    PostCallback postCallback = [onPostListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onPostListener->onPostCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->signIn(userUuid, accessToken, postCallback);
}

OCStackResult JniOcAccountManager::signOut(JNIEnv* env, jobject jListener)
{
    JniOnPostListener *onPostListener = addOnPostListener(env, jListener);

    PostCallback postCallback = [onPostListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onPostListener->onPostCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->signOut(postCallback);
}

OCStackResult JniOcAccountManager::refreshAccessToken(JNIEnv* env, const std::string& userUuid,
                                                      const std::string& refreshToken,
                                                      jobject jListener)
{
    JniOnPostListener *onPostListener = addOnPostListener(env, jListener);

    PostCallback postCallback = [onPostListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onPostListener->onPostCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->refreshAccessToken(userUuid, refreshToken, postCallback);
}

OCStackResult JniOcAccountManager::searchUser(JNIEnv* env, const std::string& userUuid,
                                              jobject jListener)
{
    JniOnGetListener *onGetListener = addOnGetListener(env, jListener);

    GetCallback getCallback = [onGetListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onGetListener->onGetCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->searchUser(userUuid, getCallback);
}

OCStackResult JniOcAccountManager::searchUser(JNIEnv* env, const QueryParamsMap& queryMap,
                                              jobject jListener)
{
    JniOnGetListener *onGetListener = addOnGetListener(env, jListener);

    GetCallback getCallback = [onGetListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onGetListener->onGetCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->searchUser(queryMap, getCallback);
}

OCStackResult JniOcAccountManager::deleteDevice(JNIEnv* env, const std::string& deviceId,
                                                jobject jListener)
{
    JniOnDeleteListener *onDeleteListener = addOnDeleteListener(env, jListener);

    DeleteCallback deleteCallback = [onDeleteListener](const HeaderOptions& opts,
        const int eCode)
    {
        onDeleteListener->onDeleteCallback(opts, eCode);
    };

    return m_sharedAccountManager->deleteDevice(deviceId, deleteCallback);
}

OCStackResult JniOcAccountManager::createGroup(JNIEnv* env, AclGroupType groupType,
                                               jobject jListener)
{
    JniOnPostListener *onPostListener = addOnPostListener(env, jListener);

    PostCallback postCallback = [onPostListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onPostListener->onPostCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->createGroup(groupType, postCallback);
}

OCStackResult JniOcAccountManager::getGroupList(JNIEnv* env, jobject jListener)
{
    JniOnGetListener *onGetListener = addOnGetListener(env, jListener);

    GetCallback getCallback = [onGetListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onGetListener->onGetCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->getGroupList(getCallback);
}

OCStackResult JniOcAccountManager::deleteGroup(JNIEnv* env, const std::string& groupId,
                                               jobject jListener)
{
    JniOnDeleteListener *onDeleteListener = addOnDeleteListener(env, jListener);

    DeleteCallback deleteCallback = [onDeleteListener](const HeaderOptions& opts,
        const int eCode)
    {
        onDeleteListener->onDeleteCallback(opts, eCode);
    };

    return m_sharedAccountManager->deleteGroup(groupId, deleteCallback);
}

OCStackResult JniOcAccountManager::joinGroup(JNIEnv* env, const std::string& groupId,
                                             jobject jListener)
{
    JniOnPostListener *onPostListener = addOnPostListener(env, jListener);

    PostCallback postCallback = [onPostListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onPostListener->onPostCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->joinGroup(groupId, postCallback);
}

OCStackResult JniOcAccountManager::addDeviceToGroup(JNIEnv* env, const std::string& groupId,
                                                    const std::vector<std::string>& deviceId,
                                                    jobject jListener)
{
    JniOnPostListener *onPostListener = addOnPostListener(env, jListener);

    PostCallback postCallback = [onPostListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onPostListener->onPostCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->addDeviceToGroup(groupId, deviceId, postCallback);
}

OCStackResult JniOcAccountManager::getGroupInfo(JNIEnv* env, const std::string& groupId,
                                                jobject jListener)
{
    JniOnGetListener *onGetListener = addOnGetListener(env, jListener);

    GetCallback getCallback = [onGetListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onGetListener->onGetCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->getGroupInfo(groupId, getCallback);
}

OCStackResult JniOcAccountManager::leaveGroup(JNIEnv* env, const std::string& groupId,
                                              jobject jListener)
{
    JniOnDeleteListener *onDeleteListener = addOnDeleteListener(env, jListener);

    DeleteCallback deleteCallback = [onDeleteListener](const HeaderOptions& opts,
        const int eCode)
    {
        onDeleteListener->onDeleteCallback(opts, eCode);
    };

    return m_sharedAccountManager->leaveGroup(groupId, deleteCallback);
}

OCStackResult JniOcAccountManager::deleteDeviceFromGroup(JNIEnv* env, const std::string& groupId,
                                                         const std::vector<std::string>& deviceId,
                                                         jobject jListener)
{
    JniOnDeleteListener *onDeleteListener = addOnDeleteListener(env, jListener);

    DeleteCallback deleteCallback = [onDeleteListener](const HeaderOptions& opts,
        const int eCode)
    {
        onDeleteListener->onDeleteCallback(opts, eCode);
    };

    return m_sharedAccountManager->deleteDeviceFromGroup(groupId, deviceId, deleteCallback);
}

OCStackResult JniOcAccountManager::observeGroup(JNIEnv* env, const std::string& groupId,
                                                jobject jListener)
{
    JniOnObserveListener *onObserveListener = addOnObserveListener(env, jListener);

    ObserveCallback observeCallback = [onObserveListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int& eCode, const int& sequenceNumber)
    {
        onObserveListener->onObserveCallback(opts, rep, eCode, sequenceNumber);
    };

    return m_sharedAccountManager->observeGroup(groupId, observeCallback);
}

OCStackResult JniOcAccountManager::cancelObserveGroup(const std::string& groupId)
{
    return m_sharedAccountManager->cancelObserveGroup(groupId);
}

OCStackResult JniOcAccountManager::observeInvitation(JNIEnv* env, jobject jListener)
{
    JniOnObserveListener *onObserveListener = addOnObserveListener(env, jListener);

    ObserveCallback observeCallback = [onObserveListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int& eCode, const int& sequenceNumber)
    {
        onObserveListener->onObserveCallback(opts, rep, eCode, sequenceNumber);
    };

    return m_sharedAccountManager->observeInvitation(observeCallback);
}

OCStackResult JniOcAccountManager::cancelObserveInvitation()
{
    return m_sharedAccountManager->cancelObserveInvitation();
}

OCStackResult JniOcAccountManager::sendInvitation(JNIEnv* env, const std::string& groupId,
                                                  const std::string& userUuid, jobject jListener)
{
    JniOnPostListener *onPostListener = addOnPostListener(env, jListener);

    PostCallback postCallback = [onPostListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int eCode)
    {
        onPostListener->onPostCallback(opts, rep, eCode);
    };

    return m_sharedAccountManager->sendInvitation(groupId, userUuid, postCallback);
}

OCStackResult JniOcAccountManager::cancelInvitation(JNIEnv* env, const std::string& groupId,
                                                    const std::string& userUuid, jobject jListener)
{
    JniOnDeleteListener *onDeleteListener = addOnDeleteListener(env, jListener);

    DeleteCallback deleteCallback = [onDeleteListener](const HeaderOptions& opts,
        const int eCode)
    {
        onDeleteListener->onDeleteCallback(opts, eCode);
    };

    return m_sharedAccountManager->cancelInvitation(groupId, userUuid, deleteCallback);
}

OCStackResult JniOcAccountManager::deleteInvitation(JNIEnv* env, const std::string& groupId,
                                                    jobject jListener)
{
    JniOnDeleteListener *onDeleteListener = addOnDeleteListener(env, jListener);

    DeleteCallback deleteCallback = [onDeleteListener](const HeaderOptions& opts,
        const int eCode)
    {
        onDeleteListener->onDeleteCallback(opts, eCode);
    };

    return m_sharedAccountManager->deleteInvitation(groupId, deleteCallback);
}


/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    getHost
* Signature: ()Ljava/lang/String;
*/
JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcAccountManager_getHost
    (JNIEnv *env, jobject thiz)
{
    LOGD("OcAccountManager_getHost");
    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return nullptr;
    }

    return env->NewStringUTF(accountManager->host().c_str());
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    getConnectivityTypeN
* Signature: ()I
*/
JNIEXPORT jint JNICALL Java_org_iotivity_base_OcAccountManager_getConnectivityTypeN
    (JNIEnv *env, jobject thiz)
{
    LOGD("OcAccountManager_getConnectivityType");
    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return -1;
    }

    OCConnectivityType connectivityType = accountManager->connectivityType();
    return static_cast<jint>(connectivityType);
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    signUp0
* Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/OnPostListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_signUp0
    (JNIEnv *env, jobject thiz, jstring jAuthProvider, jstring jAuthCode, jobject jListener)
{
    LOGD("OcAccountManager_signUp");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPostListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string authProvider;
    std::string authCode;
    if (jAuthProvider)
    {
        authProvider = env->GetStringUTFChars(jAuthProvider, nullptr);
    }
    if (jAuthCode)
    {
        authCode = env->GetStringUTFChars(jAuthCode, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->signUp(env,
                                                      authProvider,
                                                      authCode,
                                                      jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_signUp");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    signUp1
* Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;Lorg/iotivity/base/OcAccountManager/OnPostListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_signUp1
    (JNIEnv *env, jobject thiz, jstring jAuthProvider, jstring jAuthCode, jobject jOptionsMap,
     jobject jListener)
{
    LOGD("OcAccountManager_signUp");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPostListener cannot be null");
        return;
    }
    if (!jOptionsMap)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "options cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string authProvider;
    std::string authCode;
    if (jAuthProvider)
    {
        authProvider = env->GetStringUTFChars(jAuthProvider, nullptr);
    }
    if (jAuthCode)
    {
        authCode = env->GetStringUTFChars(jAuthCode, nullptr);
    }

    QueryParamsMap optionsMap;
    JniUtils::convertJavaMapToQueryParamsMap(env, jOptionsMap, optionsMap);

    try
    {
        OCStackResult result = accountManager->signUp(env,
                                                      authProvider,
                                                      authCode,
                                                      optionsMap,
                                                      jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_signUp");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    signIn0
* Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/OnPostListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_signIn0
    (JNIEnv *env, jobject thiz, jstring jUserUuid, jstring jAccessToken, jobject jListener)
{
    LOGD("OcAccountManager_signIn");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPostListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string userUuid;
    std::string accessToken;
    if (jUserUuid)
    {
        userUuid = env->GetStringUTFChars(jUserUuid, nullptr);
    }
    if (jAccessToken)
    {
        accessToken = env->GetStringUTFChars(jAccessToken, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->signIn(env,
                                                      userUuid,
                                                      accessToken,
                                                      jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_signIn");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    signOut0
* Signature: (Lorg/iotivity/base/OcAccountManager/OnPostListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_signOut0
    (JNIEnv *env, jobject thiz, jobject jListener)
{
    LOGD("OcAccountManager_signOut");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPostListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    try
    {
        OCStackResult result = accountManager->signOut(env,
                                                       jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_signOut");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    refreshAccessToken0
* Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/OnPostListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_refreshAccessToken0
    (JNIEnv *env, jobject thiz, jstring jUserUuid, jstring jRefreshAccessToken, jobject jListener)
{
    LOGD("OcAccountManager_refreshAccessToken");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPostListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string userUuid;
    std::string refreshAccessToken;
    if (jUserUuid)
    {
        userUuid = env->GetStringUTFChars(jUserUuid, nullptr);
    }
    if (jRefreshAccessToken)
    {
        refreshAccessToken = env->GetStringUTFChars(jRefreshAccessToken, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->refreshAccessToken(env,
                                                                  userUuid,
                                                                  refreshAccessToken,
                                                                  jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_refreshAccessToken");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    searchUser0
* Signature: (Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/OnGetListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_searchUser0
    (JNIEnv *env, jobject thiz, jstring jUserUuid, jobject jListener)
{
    LOGD("OcAccountManager_searchUser");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onGetListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string userUuid;
    if (jUserUuid)
    {
        userUuid = env->GetStringUTFChars(jUserUuid, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->searchUser(env,
                                                          userUuid,
                                                          jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_searchUser");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    searchUser1
* Signature: (Ljava/util/Map;Lorg/iotivity/base/OcAccountManager/OnGetListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_searchUser1
    (JNIEnv *env, jobject thiz, jobject jQueryMap, jobject jListener)
{
    LOGD("OcAccountManager_searchUser");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onGetListener cannot be null");
        return;
    }
    if (!jQueryMap)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "queryMap cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    QueryParamsMap queryMap;
    JniUtils::convertJavaMapToQueryParamsMap(env, jQueryMap, queryMap);

    try
    {
        OCStackResult result = accountManager->searchUser(env,
                                                          queryMap,
                                                          jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_searchUser");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    deleteDevice0
* Signature: (Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/onDeleteListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_deleteDevice0
    (JNIEnv *env, jobject thiz, jstring jDeviceId, jobject jListener)
{
    LOGD("OcAccountManager_deleteDevice");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onDeleteListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string deviceId;
    if (jDeviceId)
    {
        deviceId = env->GetStringUTFChars(jDeviceId, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->deleteDevice(env,
                                                            deviceId,
                                                            jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_deleteDevice");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    createGroup0
* Signature: (Lorg/iotivity/base/AclGroupType;Lorg/iotivity/base/OcAccountManager/OnPostListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_createGroup0
    (JNIEnv *env, jobject thiz, jint groupType, jobject jListener)
{
    LOGD("OcAccountManager_createGroup");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPostListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    try
    {
        OCStackResult result = accountManager->createGroup(env,
            JniUtils::getAclGroupType(env, static_cast<int>(groupType)),
            jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_createGroup");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    getGroupList0
* Signature: (Lorg/iotivity/base/OcAccountManager/OnGetListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_getGroupList0
    (JNIEnv *env, jobject thiz, jobject jListener)
{
    LOGD("OcAccountManager_getGroupList");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onGetListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    try
    {
        OCStackResult result = accountManager->getGroupList(env,
                                                            jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_getGroupList");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    deleteGroup0
* Signature: (Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/onDeleteListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_deleteGroup0
    (JNIEnv *env, jobject thiz, jstring jGroupId, jobject jListener)
{
    LOGD("OcAccountManager_deleteGroup");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onDeleteListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string groupId;
    if (jGroupId)
    {
        groupId = env->GetStringUTFChars(jGroupId, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->deleteGroup(env,
                                                           groupId,
                                                           jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_deleteGroup");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    joinGroup0
* Signature: (Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/OnPostListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_joinGroup0
    (JNIEnv *env, jobject thiz, jstring jGroupId, jobject jListener)
{
    LOGD("OcAccountManager_joinGroup");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPostListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string groupId;
    if (jGroupId)
    {
        groupId = env->GetStringUTFChars(jGroupId, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->joinGroup(env,
                                                         groupId,
                                                         jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_joinGroup");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    addDeviceToGroup0
* Signature: (Ljava/lang/String;[Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/OnPostListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_addDeviceToGroup0
    (JNIEnv *env, jobject thiz, jstring jGroupId, jobjectArray jDeviceIdArray, jobject jListener)
{
    LOGD("OcAccountManager_addDeviceToGroup");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPostListener cannot be null");
        return;
    }
    if (!jDeviceIdArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "deviceId cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string groupId;
    if (jGroupId)
    {
        groupId = env->GetStringUTFChars(jGroupId, nullptr);
    }

    std::vector<std::string> deviceIds;
    JniUtils::convertJavaStrArrToStrVector(env, jDeviceIdArray, deviceIds);

    try
    {
        OCStackResult result = accountManager->addDeviceToGroup(env,
                                                                groupId,
                                                                deviceIds,
                                                                jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_addDeviceToGroup");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    getGroupInfo0
* Signature: (Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/OnGetListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_getGroupInfo0
    (JNIEnv *env, jobject thiz, jstring jGroupId, jobject jListener)
{
    LOGD("OcAccountManager_getGroupInfo");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onGetListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string groupId;
    if (jGroupId)
    {
        groupId = env->GetStringUTFChars(jGroupId, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->getGroupInfo(env,
                                                            groupId,
                                                            jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_getGroupInfo");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    leaveGroup0
* Signature: (Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/onDeleteListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_leaveGroup0
    (JNIEnv *env, jobject thiz, jstring jGroupId, jobject jListener)
{
    LOGD("OcAccountManager_leaveGroup");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onDeleteListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string groupId;
    if (jGroupId)
    {
        groupId = env->GetStringUTFChars(jGroupId, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->leaveGroup(env,
                                                          groupId,
                                                          jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_leaveGroup");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    deleteDeviceFromGroup0
* Signature: (Ljava/lang/String;[Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/onDeleteListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_deleteDeviceFromGroup0
    (JNIEnv *env, jobject thiz, jstring jGroupId, jobjectArray jDeviceIdArray, jobject jListener)
{
    LOGD("OcAccountManager_deleteDeviceFromGroup");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onDeleteListener cannot be null");
        return;
    }
    if (!jDeviceIdArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "deviceId cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string groupId;
    if (jGroupId)
    {
        groupId = env->GetStringUTFChars(jGroupId, nullptr);
    }

    std::vector<std::string> deviceIds;
    JniUtils::convertJavaStrArrToStrVector(env, jDeviceIdArray, deviceIds);

    try
    {
        OCStackResult result = accountManager->deleteDeviceFromGroup(env,
                                                                     groupId,
                                                                     deviceIds,
                                                                     jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_deleteDeviceFromGroup");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    observeGroup0
* Signature: (Ljava/lang/String;Lorg/iotivity/base/OcResource/OnObserveListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_observeGroup0
    (JNIEnv *env, jobject thiz, jstring jGroupId, jobject jListener)
{
    LOGD("OcAccountManager_observeGroup");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onObserveListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string groupId;
    if (jGroupId)
    {
        groupId = env->GetStringUTFChars(jGroupId, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->observeGroup(env,
                                                            groupId,
                                                            jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_observeGroup");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    cancelObserveGroup0
* Signature: (Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_cancelObserveGroup0
    (JNIEnv *env, jobject thiz, jstring jGroupId)
{
    LOGD("OcAccountManager_cancelObserveGroup");

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string groupId;
    if (jGroupId)
    {
        groupId = env->GetStringUTFChars(jGroupId, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->cancelObserveGroup(groupId);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_cancelObserveGroup");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    observeInvitation0
* Signature: (Lorg/iotivity/base/OcResource/OnObserveListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_observeInvitation0
    (JNIEnv *env, jobject thiz, jobject jListener)
{
    LOGD("OcAccountManager_observeInvitation");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onObserveListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    try
    {
        OCStackResult result = accountManager->observeInvitation(env,
                                                                 jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_observeInvitation");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    cancelObserveInvitation0
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_cancelObserveInvitation0
    (JNIEnv *env, jobject thiz)
{
    LOGD("OcAccountManager_cancelObserveInvitation");

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    try
    {
        OCStackResult result = accountManager->cancelObserveInvitation();

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_cancelObserveInvitation");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    sendInvitation0
* Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/OnPostListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_sendInvitation0
    (JNIEnv *env, jobject thiz, jstring jGroupId, jstring jUserUuid, jobject jListener)
{
    LOGD("OcAccountManager_sendInvitation");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPostListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string groupId, userUuid;
    if (jGroupId)
    {
        groupId = env->GetStringUTFChars(jGroupId, nullptr);
    }
    if (jUserUuid)
    {
        userUuid = env->GetStringUTFChars(jUserUuid, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->sendInvitation(env,
                                                              groupId,
                                                              userUuid,
                                                              jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_sendInvitation");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    cancelInvitation0
* Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/onDeleteListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_cancelInvitation0
    (JNIEnv *env, jobject thiz, jstring jGroupId, jstring jUserUuid, jobject jListener)
{
    LOGD("OcAccountManager_cancelInvitation");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onDeleteListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string groupId, userUuid;
    if (jGroupId)
    {
        groupId = env->GetStringUTFChars(jGroupId, nullptr);
    }
    if (jUserUuid)
    {
        userUuid = env->GetStringUTFChars(jUserUuid, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->cancelInvitation(env,
                                                                groupId,
                                                                userUuid,
                                                                jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_cancelInvitation");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcAccountManager
* Method:    deleteInvitation0
* Signature: (Ljava/lang/String;Lorg/iotivity/base/OcAccountManager/onDeleteListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcAccountManager_deleteInvitation0
    (JNIEnv *env, jobject thiz, jstring jGroupId, jobject jListener)
{
    LOGD("OcAccountManager_deleteInvitation");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onDeleteListener cannot be null");
        return;
    }

    JniOcAccountManager *accountManager = JniOcAccountManager::getJniOcAccountManagerPtr(env,
                                                                                         thiz);
    if (!accountManager)
    {
        return;
    }

    std::string groupId;
    if (jGroupId)
    {
        groupId = env->GetStringUTFChars(jGroupId, nullptr);
    }

    try
    {
        OCStackResult result = accountManager->deleteInvitation(env,
                                                                groupId,
                                                                jListener);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcAccountManager_deleteInvitation");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}


