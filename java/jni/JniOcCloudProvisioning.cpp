/*
* ******************************************************************
*
*  Copyright 2016 Samsung Electronics All Rights Reserved.
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
#include "JniOcCloudProvisioning.h"
#include "srmutility.h"
#include "oic_malloc.h"

namespace PH = std::placeholders;
using namespace OC;

JniOcCloudProvisioning::JniOcCloudProvisioning(std::shared_ptr<OCCloudProvisioning> p)
    : m_sharedCloudObject(p)
{}

JniOcCloudProvisioning::~JniOcCloudProvisioning()
{
    LOGD("~JniOcCloudProvisioning");
    m_sharedCloudObject = nullptr;
}

JniOcCloudResultListener* JniOcCloudProvisioning::AddCloudResultListener(JNIEnv* env,
        jobject jListener)
{
    JniOcCloudResultListener *resultListener = NULL;
    resultMapLock.lock();

    for (auto it = resultMap.begin(); it != resultMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            resultListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            resultMap.insert(*it);
            LOGD("Cloud Provision resultListener: ref. count incremented");
            break;
        }
    }
    if (!resultListener)
    {
        resultListener = new JniOcCloudResultListener(env, jListener,
                RemoveCallback(std::bind(&JniOcCloudProvisioning::RemoveCloudResultListener,
                        this, PH::_1, PH::_2)));
        jobject jgListener = env->NewGlobalRef(jListener);

        resultMap.insert(std::pair < jobject, std::pair < JniOcCloudResultListener*,
                int >> (jgListener, std::pair<JniOcCloudResultListener*, int>(resultListener, 1)));
        LOGD("Cloud Provision resultListener: new listener");
    }
    resultMapLock.unlock();
    return resultListener;
}

void JniOcCloudProvisioning::RemoveCloudResultListener(JNIEnv* env, jobject jListener)
{
    resultMapLock.lock();

    for (auto it = resultMap.begin(); it != resultMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                resultMap.insert(*it);
                LOGI("Cloud Provision resultListener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniOcCloudResultListener* listener = refPair.first;
                delete listener;
                resultMap.erase(it);
                LOGI("Cloud Provision resultListener removed");
            }
            break;
        }
    }
    resultMapLock.unlock();
}

JniGetAclIdByDeviceListener* JniOcCloudProvisioning::AddGetAclByDeviceListener(JNIEnv* env,
        jobject jListener)
{
    JniGetAclIdByDeviceListener *resultListener = NULL;
    aclresultMapLock.lock();

    for (auto it = aclresultMap.begin(); it != aclresultMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            resultListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            aclresultMap.insert(*it);
            LOGD("GetACLByDeviceID Listener: ref. count incremented");
            break;
        }
    }
    if (!resultListener)
    {
        resultListener = new JniGetAclIdByDeviceListener(env, jListener,
                RemoveCallback(std::bind(&JniOcCloudProvisioning::RemoveGetAclByDeviceIdListener,
                        this, PH::_1, PH::_2)));
        jobject jgListener = env->NewGlobalRef(jListener);

        aclresultMap.insert(std::pair < jobject, std::pair < JniGetAclIdByDeviceListener*,
                int >> (jgListener, std::pair<JniGetAclIdByDeviceListener*, int>(resultListener, 1)));
        LOGD("GetACLByDeviceID Listener: new listener");
    }
    aclresultMapLock.unlock();
    return resultListener;
}

JniCreateAclIdListener* JniOcCloudProvisioning::CreateAclListener(JNIEnv* env,
        jobject jListener)
{
    JniCreateAclIdListener *resultListener = NULL;
    createresultMapLock.lock();

    for (auto it = createresultMap.begin(); it != createresultMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            resultListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            createresultMap.insert(*it);
            LOGD("CreateACLID Listener: ref. count incremented");
            break;
        }
    }
    if (!resultListener)
    {
        resultListener = new JniCreateAclIdListener(env, jListener,
                RemoveCallback(std::bind(&JniOcCloudProvisioning::RemoveCreateAclIdListener,
                        this, PH::_1, PH::_2)));
        jobject jgListener = env->NewGlobalRef(jListener);

        createresultMap.insert(std::pair < jobject, std::pair < JniCreateAclIdListener*,
                int >> (jgListener, std::pair<JniCreateAclIdListener*, int>(resultListener, 1)));
        LOGD("CreateACLID Listener: new listener");
    }
    createresultMapLock.unlock();
    return resultListener;
}

void JniOcCloudProvisioning::RemoveGetAclByDeviceIdListener(JNIEnv* env, jobject jListener)
{
    aclresultMapLock.lock();

    for (auto it = aclresultMap.begin(); it != aclresultMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                aclresultMap.insert(*it);
                LOGI("GetACLByDeviceID Listener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniGetAclIdByDeviceListener* listener = refPair.first;
                delete listener;
                aclresultMap.erase(it);
                LOGI("GetACLByDeviceID Listener removed");
            }
            break;
        }
    }
    aclresultMapLock.unlock();
}

void JniOcCloudProvisioning::RemoveCreateAclIdListener(JNIEnv* env, jobject jListener)
{
    createresultMapLock.lock();

    for (auto it = createresultMap.begin(); it != createresultMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                createresultMap.insert(*it);
                LOGI("CreateACLID Listener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniCreateAclIdListener* listener = refPair.first;
                delete listener;
                createresultMap.erase(it);
                LOGI("CreateACLID Listener removed");
            }
            break;
        }
    }
    createresultMapLock.unlock();
}

JniOcCloudProvisioning * Create_native_object(JNIEnv *env, jobject thiz)
{
    jstring jip = (jstring)env->CallObjectMethod(thiz, g_mid_OcCloudProvisioning_getIP);
    if (!jip || env->ExceptionCheck())
    {
        return nullptr;
    }
    const char *str = env->GetStringUTFChars(jip, NULL);
    std::string ipaddress(str);
    env->ReleaseStringUTFChars(jip, str);

    jint port = env->CallIntMethod(thiz, g_mid_OcCloudProvisioning_getPort);
    if (env->ExceptionCheck())
    {
        return nullptr;
    }
    JniOcCloudProvisioning *cloud = new JniOcCloudProvisioning(
            std::shared_ptr<OCCloudProvisioning>(new OCCloudProvisioning(ipaddress, (uint16_t)port)));
    SetHandle<JniOcCloudProvisioning>(env, thiz, cloud);
    return cloud;
}

JniOcCloudProvisioning* JniOcCloudProvisioning::getJniOcCloudProvisioningPtr(JNIEnv *env, jobject thiz)
{
    JniOcCloudProvisioning *cloud = GetHandle<JniOcCloudProvisioning>(env, thiz);

    if (env->ExceptionCheck())
    {
        LOGE("Failed to get native handle from OcCloudProvisioning class");
    }

    return cloud;
}

OCStackResult JniOcCloudProvisioning::requestCertificate(JNIEnv* env, jobject jListener)
{
    JniOcCloudResultListener *resultListener = AddCloudResultListener(env, jListener);

    ResponseCallBack responseCallBack = [resultListener](OCStackResult result, void *data)
    {
        resultListener->CloudResultListenerCB(result, data, ListenerFunc::REQUEST_CERTIFICATE);
    };

    return m_sharedCloudObject->requestCertificate(responseCallBack);
}

OCStackResult JniOcCloudProvisioning::getIndividualAclInfo(JNIEnv* env, jobject jListener, std::string &aclID)
{
    JniOcCloudResultListener *resultListener = AddCloudResultListener(env, jListener);

    ResponseCallBack responseCallBack = [resultListener](OCStackResult result, void *data)
    {
        resultListener->CloudResultListenerCB(result, data, ListenerFunc::GET_ACLINFO);
    };

    return m_sharedCloudObject->getIndividualAclInfo(aclID, responseCallBack);
}

OCStackResult JniOcCloudProvisioning::getCRL(JNIEnv* env, jobject jListener)
{
    JniOcCloudResultListener *resultListener = AddCloudResultListener(env, jListener);

    ResponseCallBack responseCallBack = [resultListener](OCStackResult result, void *data)
    {
        resultListener->CloudResultListenerCB(result, data, ListenerFunc::GET_CRL);
    };

    return m_sharedCloudObject->getCRL(responseCallBack);
}

OCStackResult JniOcCloudProvisioning::postCRL(JNIEnv* env, const std::string& thisUpdate,
                                  const std::string& nextUpdate, const OCByteString *crl,
                                   const stringArray_t *serialNumbers, jobject jListener)
{
    JniOcCloudResultListener *resultListener = AddCloudResultListener(env, jListener);

    ResponseCallBack responseCallBack = [resultListener](OCStackResult result, void *data)
    {
        resultListener->CloudResultListenerCB(result, data, ListenerFunc::POST_CRL);
    };

    return m_sharedCloudObject->postCRL(thisUpdate, nextUpdate, crl, serialNumbers,
            responseCallBack);
}

OCStackResult JniOcCloudProvisioning::getAclIdByDevice(JNIEnv* env, std::string deviceId,
        jobject jListener)
{
    JniGetAclIdByDeviceListener *resultListener = AddGetAclByDeviceListener(env, jListener);

    AclIdResponseCallBack aclIdResponseCallBack = [resultListener](OCStackResult result,
            std::string aclId)
    {
        resultListener->GetAclIdByDeviceListenerCB(result, aclId);
    };

    return m_sharedCloudObject->getAclIdByDevice(deviceId, aclIdResponseCallBack);
}

OCStackResult JniOcCloudProvisioning::createAclId(JNIEnv* env, std::string ownerId,
              std::string deviceID, jobject jListener)
{
    JniCreateAclIdListener *resultListener = CreateAclListener(env, jListener);

    AclIdResponseCallBack aclIdResponseCallBack = [resultListener](OCStackResult result,
            std::string aclId)
    {
        resultListener->CreateAclIdListenerCB(result, aclId);
    };

    return m_sharedCloudObject->createAclId(ownerId, deviceID, aclIdResponseCallBack);
}

static OicSecValidity_t* getValiditiesList(JNIEnv *env, jobject validityObject)
{
    jstring jData;
    jobjectArray  valList = (jobjectArray)env->CallObjectMethod(validityObject,
            g_mid_OcOicSecCloudAcl_ace_get_validities);
    if (!valList || env->ExceptionCheck())
    {
        return nullptr;
    }
    int nr_validities = env->GetArrayLength(valList);

    OicSecValidity_t *valHead = NULL;

    for (int i = 0 ; i < nr_validities; i++)
    {
        OicSecValidity_t *tmp = (OicSecValidity_t*)OICCalloc(1, sizeof(OicSecValidity_t));
        jobject element = env->GetObjectArrayElement(valList, i);
        if (!element || env->ExceptionCheck())
        {
            return nullptr;
        }

        jData = (jstring)env->CallObjectMethod(element, g_mid_OcOicSecAcl_validity_get_getPeriod);
        if (!jData || env->ExceptionCheck())
        {
            return nullptr;
        }
        tmp->period = (char*)env->GetStringUTFChars(jData, 0);

        jint jrecurrenceLen = (jint) env->CallIntMethod(element,
                g_mid_OcOicSecAcl_validity_get_recurrenceLen);
        tmp->recurrenceLen = (int)jrecurrenceLen;

        if (jrecurrenceLen > 0)
        {
            jvalue argv[1];
            tmp->recurrences = (char**)OICCalloc(jrecurrenceLen, sizeof(char*));

            for (int i = 0 ; i < jrecurrenceLen; i++)
            {
                argv[0].i = i;
                jData = (jstring)env->CallObjectMethodA(element,
                        g_mid_OcOicSecAcl_validity_get_recurrences, argv);
                if (!jData || env->ExceptionCheck())
                {
                    return nullptr;
                }
                tmp->recurrences[i] = (char*)env->GetStringUTFChars(jData, 0);
            }
        }
        if (NULL == valHead)
        {
            valHead = tmp;
        }
        else
        {
            OicSecValidity_t *ptr = valHead;
            while(ptr->next != NULL) ptr = ptr->next;
            ptr->next = tmp;
            tmp->next = NULL;
        }
        env->DeleteLocalRef(element);
    }
    return valHead;
}

static OicSecRsrc_t * getResourcesList(JNIEnv *env, jobject resourceObject)
{
    jstring jData;

    jobjectArray rescList = (jobjectArray)env->CallObjectMethod(resourceObject,
            g_mid_OcOicSecCloudAcl_ace_get_resources);
    if (!rescList || env->ExceptionCheck())
    {
        return nullptr;
    }

    int nr_resc = env->GetArrayLength(rescList);
    OicSecRsrc_t *rescHead = NULL;

    for (int i = 0 ; i < nr_resc; i++)
    {
        OicSecRsrc_t *tmp = (OicSecRsrc_t*)OICCalloc(1, sizeof(OicSecRsrc_t));
        jobject element = env->GetObjectArrayElement(rescList, i);
        if (!element || env->ExceptionCheck())
        {
            return nullptr;
        }
        jData = (jstring)env->CallObjectMethod(element, g_mid_OcOicSecAcl_resr_get_href);
        if (!jData || env->ExceptionCheck())
        {
            return nullptr;
        }
        tmp->href = (char*)env->GetStringUTFChars(jData, 0);

        jData = (jstring)env->CallObjectMethod(element, g_mid_OcOicSecAcl_resr_get_rel);
        if (!jData || env->ExceptionCheck())
        {
            return nullptr;
        }
        tmp->rel = (char*)env->GetStringUTFChars(jData, 0);

        jint len = (jint) env->CallIntMethod(element, g_mid_OcOicSecAcl_resr_get_typeLen);
        tmp->typeLen = (int)len;
        if (len > 0)
        {
            jvalue argv[1];
            tmp->types = (char**)OICCalloc(len, sizeof(char*));

            for (int i = 0 ; i < len; i++)
            {
                argv[0].i = i;
                jData = (jstring)env->CallObjectMethodA(element,
                        g_mid_OcOicSecAcl_resr_get_types, argv);
                if (!jData || env->ExceptionCheck())
                {
                    return nullptr;
                }
                tmp->types[i] = (char*)env->GetStringUTFChars(jData, 0);
            }
        }

        len = (jint) env->CallIntMethod(element, g_mid_OcOicSecAcl_resr_get_interfaceLen);
        tmp->interfaceLen = len;
        if (len > 0)
        {
            jvalue argv[1];
            tmp->interfaces = (char**)OICCalloc(len, sizeof(char*));

            for (int i = 0 ; i < len; i++)
            {
                argv[0].i = i;
                jData = (jstring)env->CallObjectMethodA(element,
                        g_mid_OcOicSecAcl_resr_get_interfaces, argv);
                if (!jData || env->ExceptionCheck())
                {
                    return nullptr;
                }
                tmp->interfaces[i] = (char*)env->GetStringUTFChars(jData, 0);
            }
        }

        if (NULL == rescHead)
        {
            rescHead = tmp;
        }
        else
        {
            OicSecRsrc_t *ptr = rescHead;
            while(ptr->next != NULL) ptr = ptr->next;
            ptr->next = tmp;
            tmp->next = NULL;
        }
        env->DeleteLocalRef(element);
    }
    return rescHead;
}

cloudAce_t * ConvertJavaCloudAcestoOCAces(JNIEnv* env, jobjectArray jcloudAces)
{
    char *str;
    jstring jData;
    cloudAce_t *acesHead = NULL;
    const jsize arrayLen = env->GetArrayLength(jcloudAces);

    for (int i = 0 ; i < arrayLen; i++)
    {
        cloudAce_t *aces = (cloudAce_t*)OICCalloc(1, sizeof(cloudAce_t));
        jobject element = env->GetObjectArrayElement(jcloudAces, i);
        if (!element || env->ExceptionCheck())
        {
            return nullptr;
        }
        //get aclID
        jData = (jstring) env->CallObjectMethod(element, g_mid_OcOicSecCloudAcl_ace_get_aclId);
        if (!jData || env->ExceptionCheck())
        {
            return nullptr;
        }
        str = (char*) env->GetStringUTFChars(jData, 0);
        aces->aceId = strdup(str);
        env->ReleaseStringUTFChars(jData, str);

        //get subjectID
        jData = (jstring) env->CallObjectMethod(element, g_mid_OcOicSecCloudAcl_ace_get_subjectID);
        if (!jData || env->ExceptionCheck())
        {
            return nullptr;
        }
        str = (char*) env->GetStringUTFChars(jData, 0);
        if (OC_STACK_OK == ConvertStrToUuid(str, &aces->subjectuuid))
        {
            env->ReleaseStringUTFChars(jData, str);
        }
        else
        {
            return nullptr;
        }
        //get permission
        jint permType = (jint)env->CallIntMethod(element, g_mid_OcOicSecCloudAcl_ace_get_permission);
        aces->permission = (uint16_t)permType;
        //get stype
        permType = (jint)env->CallIntMethod(element, g_mid_OcOicSecCloudAcl_ace_get_stype);
        aces->stype = (uint16_t)permType;
        //get resources List
        if (nullptr == (aces->resources = getResourcesList(env, element)))
        {
            return nullptr;
        }
        aces->validities = NULL;
        //get validities
        if (nullptr == (aces->validities = getValiditiesList(env, element)))
        {
            return nullptr;
        }
        // create list
        if (NULL == acesHead)
        {
            acesHead = aces;
        }
        else
        {
            cloudAce_t *ptr = acesHead;
            while(ptr->next != NULL) ptr = ptr->next;
            ptr->next = aces;
            aces->next = NULL;
        }
    }
    return acesHead;
}

OCStackResult JniOcCloudProvisioning::updateIndividualACL(JNIEnv* env, jobject jListener,
        std::string aclID, jobjectArray jcloudAces)
{
    JniOcCloudResultListener *resultListener = AddCloudResultListener(env, jListener);

    cloudAce_t *aces = NULL;
    aces = ConvertJavaCloudAcestoOCAces(env, jcloudAces);

    ResponseCallBack responseCallBack = [resultListener](OCStackResult result, void *data)
    {
        resultListener->CloudResultListenerCB(result, data, ListenerFunc::UPDATE_IND_ACL);

    };

    return m_sharedCloudObject->updateIndividualACL(aces, aclID, responseCallBack);
}

/*
 * Class:     org_iotivity_base_OcCloudProvisioning
 * Method:    requestCertificate
 * Signature: (Lorg/iotivity/base/OcCloudProvisioning/RequestCertificateListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCloudProvisioning_requestCertificate
  (JNIEnv* env, jobject thiz, jobject jListener)
{
    LOGD("OcCloudProvisioning_requestCertificate");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK, "Listener cannot be null");
        return;
    }

    JniOcCloudProvisioning *cloud = JniOcCloudProvisioning::getJniOcCloudProvisioningPtr(env, thiz);
    if (!cloud)
    {
        LOGD("OcCloudProvisioning_requestCertificate, No native object, creating now");
        cloud = Create_native_object(env, thiz);
        if (!cloud)
        {
            ThrowOcException(OC_STACK_ERROR, "OcCloudProvisioning_requestCertificate,"
                    "Can not Create Native object");
            return;
        }
    }

    try
    {
        OCStackResult result = cloud->requestCertificate(env, jListener);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcCloudProvisioning_requestCertificate");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
    return;
}

/*
 * Class:     org_iotivity_base_OcCloudProvisioning
 * Method:    getAclIdByDevice
 * Signature: (Ljava/lang/String;Lorg/iotivity/base/OcCloudProvisioning/GetAclIdByDeviceListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCloudProvisioning_getAclIdByDevice
  (JNIEnv *env, jobject thiz, jstring jdeviceId, jobject jListener)
{
    LOGD("OcCloudProvisioning_getAclIdByDevice");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK, "Listener cannot be null");
        return;
    }

    if (!jdeviceId)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "deviceID can not be null");
        return;
    }
    JniOcCloudProvisioning *cloud = JniOcCloudProvisioning::getJniOcCloudProvisioningPtr(env, thiz);
    if (!cloud)
    {
        LOGD("OcCloudProvisioning_getAclIdByDevice, No native object, creating now");
        cloud = Create_native_object(env, thiz);
        if (!cloud)
        {
            ThrowOcException(OC_STACK_ERROR, "OcCloudProvisioning_getAclIdByDevice,"
                    "Can not Create Native object");
            return;
        }
    }

    const char *str = env->GetStringUTFChars(jdeviceId, NULL);
    if (!str || env->ExceptionCheck())
    {
        ThrowOcException(OC_STACK_ERROR,"OcCloudProvisioning_getAclIdByDevice");
        return;
    }
    std::string deviceId(str);
    env->ReleaseStringUTFChars(jdeviceId, str);

    try
    {
        OCStackResult result = cloud->getAclIdByDevice(env, deviceId, jListener);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcCloudProvisioning_getAclIdByDevice");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
    return;
}

/*
 * Class:     org_iotivity_base_OcCloudProvisioning
 * Method:    createAclId
 * Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcCloudProvisioning/CreateAclIdListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCloudProvisioning_createAclId
  (JNIEnv *env, jobject thiz, jstring jownerId, jstring jdeviceId, jobject jListener)
{
    LOGD("OcCloudProvisioning_createAclId");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK, "Listener cannot be null");
        return;
    }

    if (!jownerId)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "ownerId can not be null");
        return;
    }

    if (!jdeviceId)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "deviceID can not be null");
        return;
    }

    JniOcCloudProvisioning *cloud = JniOcCloudProvisioning::getJniOcCloudProvisioningPtr(env, thiz);
    if (!cloud)
    {
        LOGD("OcCloudProvisioning_createAclId, No native object, creating now");
        cloud = Create_native_object(env, thiz);
        if (!cloud)
        {
            ThrowOcException(OC_STACK_ERROR, "OcCloudProvisioning_createAclId,"
                    "Can not Create Native object");
            return;
        }
    }

    const char *ownerstr = env->GetStringUTFChars(jownerId, NULL);
    if (!ownerstr || env->ExceptionCheck())
    {
        ThrowOcException(OC_STACK_ERROR,"OcCloudProvisioning_createAclId");
        return;
    }
    std::string ownerId(ownerstr);
    env->ReleaseStringUTFChars(jownerId, ownerstr);

    const char *str = env->GetStringUTFChars(jdeviceId, NULL);
    if (!str || env->ExceptionCheck())
    {
        ThrowOcException(OC_STACK_ERROR,"OcCloudProvisioning_createAclId");
        return;
    }
    std::string deviceId(str);
    env->ReleaseStringUTFChars(jdeviceId, str);

    try
    {
        OCStackResult result = cloud->createAclId(env, ownerId, deviceId, jListener);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcCloudProvisioning_createAclId");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
    return;
}

/*
 * Class:     org_iotivity_base_OcCloudProvisioning
 * Method:    getIndividualAclInfo
 * Signature: (Ljava/lang/String;Lorg/iotivity/base/OcCloudProvisioning/GetIndividualAclInfoListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCloudProvisioning_getIndividualAclInfo
  (JNIEnv *env, jobject thiz, jstring jaclID, jobject jListener)
{
    LOGD("OcCloudProvisioning_getIndividualAclInfo");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK, "Listener cannot be null");
        return;
    }
    if (!jaclID)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "aclID cannot be null");
        return;
    }

    JniOcCloudProvisioning *cloud = JniOcCloudProvisioning::getJniOcCloudProvisioningPtr(env, thiz);
    if (!cloud)
    {
        LOGD("OcCloudProvisioning_getIndividualAclInfo, No native object, creating now");
        cloud = Create_native_object(env, thiz);
        if (!cloud)
        {
            ThrowOcException(OC_STACK_ERROR, "OcCloudProvisioning_getIndividualAclInfo,"
                    "Can not Create Native object");
            return;
        }
    }

    const char *str = env->GetStringUTFChars(jaclID, NULL);
    std::string aclID(str);
    env->ReleaseStringUTFChars(jaclID, str);

    try
    {
        OCStackResult result = cloud->getIndividualAclInfo(env, jListener, aclID);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcCloudProvisioning_getIndividualAclInf");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
    return;
}
/*
 * Class:     org_iotivity_base_OcCloudProvisioning
 * Method:    updateIndividualACL
 * Signature: (Ljava/lang/String;Ljava/lang/Object;Lorg/iotivity/base/OcCloudProvisioning/UpdateIndividualACLListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCloudProvisioning_updateIndividualACL0
  (JNIEnv *env, jobject thiz, jstring jaclID, jobjectArray jcloudAces, jobject jListener)
{
    LOGD("OcCloudProvisioning_updateIndividualACL");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK, "Listener cannot be null");
        return;
    }
    if (!jaclID)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "aclID cannot be null");
        return;
    }

    JniOcCloudProvisioning *cloud = JniOcCloudProvisioning::getJniOcCloudProvisioningPtr(env, thiz);
    if (!cloud)
    {
        LOGD("OcCloudProvisioning_updateIndividualACL, No native object, creating now");
        cloud = Create_native_object(env, thiz);
        if (!cloud)
        {
            ThrowOcException(OC_STACK_ERROR, "OcCloudProvisioning_updateIndividualACL,"
                    "Can not Create Native object");
            return;
        }
    }

    const char *str = env->GetStringUTFChars(jaclID, NULL);
    std::string aclID(str);
    env->ReleaseStringUTFChars(jaclID, str);

    try
    {
        OCStackResult result = cloud->updateIndividualACL(env, jListener, aclID, jcloudAces);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcCloudProvisioning_updateIndividualACL");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
    return;
}


/*
 * Class:     org_iotivity_base_OcCloudProvisioning
 * Method:    getCRL
 * Signature: (Lorg/iotivity/base/OcCloudProvisioning/GetCRLListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCloudProvisioning_getCRL
  (JNIEnv *env, jobject thiz, jobject jListener)
{
    LOGD("OcCloudProvisioning_getCRL");
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK, "Listener cannot be null");
        return;
    }

    JniOcCloudProvisioning *cloud = JniOcCloudProvisioning::getJniOcCloudProvisioningPtr(env, thiz);
    if (!cloud)
    {
        LOGD("OcCloudProvisioning_getCRL, No native object, creating now");
        cloud = Create_native_object(env, thiz);
        if (!cloud)
        {
            ThrowOcException(OC_STACK_ERROR, "OcCloudProvisioning_getCRL,"
                    "Can not Create Native object");
            return;
        }
    }

    try
    {
        OCStackResult result = cloud->getCRL(env, jListener);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcCloudProvisioning_requestCertificate");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
    return;
}

/*
 * Class:     org_iotivity_base_OcCloudProvisioning
 * Method:    postCRL0
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/util/ArrayList;Lorg/iotivity/base/OcCloudProvisioning/PostCRLListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCloudProvisioning_postCRL0
  (JNIEnv *env , jobject thiz, jstring jthisUpdate, jstring jnextUpdate, jstring jcrl,
   jobjectArray jserialnumArray, jobject jListener)
{
    LOGD("OcCloudProvisioning_postCRL0");
     if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK,"Listener can not be null");
        return;
    }
    if (!jthisUpdate || !jnextUpdate || !jserialnumArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, " Invalid parameter (NULL param)");
        return;
    }

    JniOcCloudProvisioning *cloud = JniOcCloudProvisioning::getJniOcCloudProvisioningPtr(env, thiz);
    if (!cloud)
    {
        LOGD("OcCloudProvisioning_getCRL, No native object, creating now");
        cloud = Create_native_object(env, thiz);
        if (!cloud)
        {
            ThrowOcException(OC_STACK_ERROR, "OcCloudProvisioning_PostCRL0,"
                    "Can not Create Native object");
            return;
        }
    }

    const char *str = env->GetStringUTFChars(jthisUpdate, NULL);
    if (!str || env->ExceptionCheck())
    {
        ThrowOcException(OC_STACK_ERROR,"OcCloudProvisioning_PostCRL0");
        return;
    }
    std::string thisUpdate(str);
    env->ReleaseStringUTFChars(jthisUpdate, str);

    str = env->GetStringUTFChars(jnextUpdate, NULL);
    if (!str || env->ExceptionCheck())
    {
        ThrowOcException(OC_STACK_ERROR,"OcCloudProvisioning_PostCRL0");
        return;
    }
    std::string nextUpdate(str);
    env->ReleaseStringUTFChars(jnextUpdate, str);

    OCByteString *crl = NULL;
    if (jcrl)
    {
        str = env->GetStringUTFChars(jcrl, NULL);
        if (!str || env->ExceptionCheck())
        {
            ThrowOcException(OC_STACK_ERROR,"OcCloudProvisioning_PostCRL0");
            return;
        }
        crl = (OCByteString*)OICCalloc(1, sizeof(OCByteString));
        crl->len = (size_t)(strlen(str));
        crl->bytes = (uint8_t*)OICCalloc(crl->len, sizeof(uint8_t));

        for (size_t i = 0 ; i < crl->len; i++)
        {
            crl->bytes[i] = (uint8_t)str[i];
        }
    }

    jsize len = env->GetArrayLength(jserialnumArray);

    stringArray_t *serialNumArray = (stringArray_t*)OICCalloc(1, sizeof(stringArray_t));
    serialNumArray->array = (char**)OICCalloc(len, sizeof(char*));
    serialNumArray->length = len;

    for (jsize i = 0; i < len; ++i)
    {
        jstring jStr = (jstring)env->GetObjectArrayElement(jserialnumArray, i);
        if (!jStr)
        {
            ThrowOcException(OC_STACK_ERROR,"OcCloudProvisioning_PostCRL0");
            return;
        }
        serialNumArray->array[i] = (char*)env->GetStringUTFChars(jStr, nullptr);
        if (env->ExceptionCheck())
        {
            ThrowOcException(OC_STACK_ERROR,"OcCloudProvisioning_PostCRL0");
            return;
        }
        env->DeleteLocalRef(jStr);
    }

    try
    {
        OCStackResult result = cloud->postCRL(env, thisUpdate, nextUpdate, crl,
                serialNumArray, jListener);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcCloudProvisioning_PostCRL0");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
    return;
}
