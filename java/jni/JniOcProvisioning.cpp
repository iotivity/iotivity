/*
* ******************************************************************
*
*  Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "JniOcProvisioning.h"
#include "JniPinCheckListener.h"
#include "JniDisplayPinListener.h"
#include "oic_malloc.h"
#include "aclresource.h"
#include "oxmverifycommon.h"
#include "JniDisplayVerifyNumListener.h"
#include "JniConfirmNumListener.h"
#include <climits>

using namespace OC;
namespace PH = std::placeholders;

static JniPinCheckListener *jniPinListener = nullptr;
static JniDisplayPinListener *jniDisplayPinListener = nullptr;
static JniDisplayVerifyNumListener *jniDisplayMutualVerifyNumListener = nullptr;
static JniConfirmNumListener *jniConfirmMutualVerifyNumListener = nullptr;

void JNICALL Callback(char *buf, size_t size)
{
    if (jniPinListener)
    {
        jniPinListener->PinCallback(buf, size);
    }
    else
    {
        LOGE("jniPinListener is null");
    }
}

void JNICALL displayPinCB(char *pinBuf, size_t pinSize)
{
    if (jniDisplayPinListener)
    {
        jniDisplayPinListener->displayPinCallback(pinBuf, pinSize);
    }
    else
    {
        LOGE("DisplayPinListener is null");
    }
}

OCStackResult displayMutualVerifNumCB(uint8_t *verifyNum)
{
    OCStackResult res;

    if (jniDisplayMutualVerifyNumListener)
    {
        res = jniDisplayMutualVerifyNumListener->displayMutualVerifNumCallback(verifyNum);
    }
    else
    {
        res = OC_STACK_ERROR;
        LOGE("DisplayMutualVerifyNumListener is null");
    }
    return res;
}

OCStackResult confirmMutualVerifNumCB()
{
    OCStackResult res;

    if (jniConfirmMutualVerifyNumListener)
    {
        res = jniConfirmMutualVerifyNumListener->confirmMutualVerifNumCallback();
    }
    else
    {
        res = OC_STACK_ERROR;
        LOGE("ConfirmMutualVerifyNumListener is null");
    }
    return res;
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    ownershipTransferCDdata
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcProvisioning_ownershipTransferCBdata
  (JNIEnv *env, jobject thiz, jint OxmType, jobject jListener)
{
    OC_UNUSED(thiz);
    LOGD("OcProvisioning_ownershipTransferCBdata");
    OCStackResult result = OC_STACK_ERROR;

    try
    {
        if (OIC_JUST_WORKS == (OicSecOxm_t)OxmType)
        {
            /*NO callback required for JUST_WORKS*/
            return;
        }
        if (OIC_RANDOM_DEVICE_PIN == (OicSecOxm_t)OxmType)
        {
            if (jListener)
            {
                delete jniPinListener;
                jniPinListener = new JniPinCheckListener(env, jListener);
                result = OCSecure::setInputPinCallback(Callback);
            }
            else
            {
                result = OC_STACK_INVALID_CALLBACK;
            }
        }

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcProvisioning_ownershipTransferCDdata");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcProvisioning
* Method:    discoverUnownedDevices
* Signature: (I)[Lorg/iotivity/base/OcSecureResource;
*/
JNIEXPORT jobjectArray JNICALL Java_org_iotivity_base_OcProvisioning_discoverUnownedDevices1
  (JNIEnv *env, jclass clazz, jint timeout)
{
    OC_UNUSED(clazz);
    LOGI("OcProvisioning_discoverUnownedDevices");
    DeviceList_t list;

    try
    {
        if (timeout < 0)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "Timeout value cannot be negative");
            return nullptr;
        }
        OCStackResult result = OCSecure::discoverUnownedDevices((unsigned short)timeout, list);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to discover Unowned devices");
            return nullptr;
        }

        return JniSecureUtils::convertDeviceVectorToJavaArray(env, list);
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return nullptr;
    }
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    provisionInit
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcProvisioning_provisionInit
  (JNIEnv *env, jclass clazz, jstring jdbPath)
{
    OC_UNUSED(clazz);
    LOGI("OcProvisioning_provisionInit");
    char *dbpath;

    if (!jdbPath)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "SVR db path cannot be null");
        return;
    }

    try
    {
        dbpath = (char*)env->GetStringUTFChars(jdbPath, NULL);
        OCStackResult result = OCSecure::provisionInit(env->GetStringUTFChars(jdbPath, NULL));

        if (OC_STACK_OK != result)
        {
            env->ReleaseStringUTFChars(jdbPath, (const char*)dbpath);
            ThrowOcException(result, "Failed to Init Provisioning Manager");
            return;
        }
        env->ReleaseStringUTFChars(jdbPath, (const char*)dbpath);
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    provisionClose
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcProvisioning_provisionClose
  (JNIEnv *env, jclass thiz)
{
    OC_UNUSED(thiz);
    LOGD("OcProvisioning_provisionClose");
    try
    {
        OCStackResult result = OCSecure::provisionClose();
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OCSecure::provisionClose Failed");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    discoverOwnedDevices
 * Signature: (I)[Lorg/iotivity/base/OcSecureResource;
 */
JNIEXPORT jobjectArray JNICALL Java_org_iotivity_base_OcProvisioning_discoverOwnedDevices1
  (JNIEnv *env, jclass clazz , jint timeout)
{
    OC_UNUSED(clazz);
    LOGI("OcProvisioning_discoverOwnedDevices");
    DeviceList_t list;

    try
    {
        if (timeout < 0)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "Timeout value cannot be negative");
            return nullptr;
        }
        OCStackResult result = OCSecure::discoverOwnedDevices((unsigned short)timeout, list);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to discover Owned devices");
            return nullptr;
        }

        return JniSecureUtils::convertDeviceVectorToJavaArray(env, list);
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return nullptr;
    }
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    getDevicestatusLists
 * Signature: (I)[Lorg/iotivity/base/OcSecureResource;
 */
JNIEXPORT jobjectArray JNICALL Java_org_iotivity_base_OcProvisioning_getDeviceStatusList1
  (JNIEnv *env, jclass clazz, jint timeout)
{
    OC_UNUSED(clazz);
    LOGI("OcProvisioning_getDeviceStatusList");
    DeviceList_t  ownedDevList, unownedDevList;

    try
    {
        if (timeout < 0)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "Timeout value cannot be negative");
            return nullptr;
        }
        OCStackResult result = OCSecure::getDevInfoFromNetwork((unsigned short)timeout,
                ownedDevList, unownedDevList);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to get Device Status List");
            return nullptr;
        }
        ownedDevList.insert(ownedDevList.end(), unownedDevList.begin(), unownedDevList.end());
        return JniSecureUtils::convertDeviceVectorToJavaArray(env, ownedDevList);
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return nullptr;
    }
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    setDisplayNumListener
 * Signature: (Lorg/iotivity/base/OcProvisioning/DisplayNumListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcProvisioning_setDisplayNumListener
  (JNIEnv *env, jclass clazz, jobject jListener)
{
    OC_UNUSED(clazz);
    LOGI("OcProvisioning_setDisplayNumListener");

    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK, "Listner can't be null");
        return;
    }
    delete jniDisplayMutualVerifyNumListener;
    jniDisplayMutualVerifyNumListener = new JniDisplayVerifyNumListener(env, jListener);

    try
    {
        OC::DisplayNumContext *displayNumContext = new OC::DisplayNumContext(displayMutualVerifNumCB);
        OCStackResult result = OCSecure::registerDisplayNumCallback(displayNumContext);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to set Listner");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return;
    }
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    unsetDisplayNumListener
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_iotivity_base_OcProvisioning_unsetDisplayNumListener
  (JNIEnv * env, jclass clazz)
{
    OC_UNUSED(clazz);
    LOGI("OcProvisioning_unsetDisplayNumListener");

    OCStackResult result = OCSecure::deregisterDisplayNumCallback();

    if (OC_STACK_OK != result)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK, "Failed to unset Listener");
    }

    return result;
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    setPinType0
 * Signature: (II)I
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcProvisioning_setPinType0
  (JNIEnv *env, jclass thiz, jint pinSize, jint pinType)
{
    OC_UNUSED(thiz);
    LOGI("OcProvisioning_setPinType0");

    OCStackResult result = OC_STACK_ERROR;
    try
    {
        result = OCSecure::setRandomPinPolicy((size_t)pinSize, (OicSecPinType_t)pinType);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to set PinType");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
    return;
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    setConfirmNumListener
 * Signature: (Lorg/iotivity/base/OcProvisioning/ConfirmNumListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcProvisioning_setConfirmNumListener
  (JNIEnv *env, jclass clazz, jobject jListener)
{
    OC_UNUSED(clazz);
    LOGI("OcProvisioning_setConfirmNumListener");

    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK, "Listner can't be null");
        return;
    }
    delete jniConfirmMutualVerifyNumListener;
    jniConfirmMutualVerifyNumListener = new JniConfirmNumListener(env, jListener);

    try
    {
        OC::UserConfirmNumContext* userConfirmNumContext = new OC::UserConfirmNumContext(confirmMutualVerifNumCB);
        OCStackResult result = OCSecure::registerUserConfirmCallback(userConfirmNumContext);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to set Listner");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return;
    }
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    unsetConfirmNumListener
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_org_iotivity_base_OcProvisioning_unsetConfirmNumListener
  (JNIEnv *env, jclass clazz)
{
    OC_UNUSED(clazz);
    LOGI("OcProvisioning_unsetConfirmNumListener");

    OCStackResult result = OCSecure::deregisterUserConfirmCallback();

    if (OC_STACK_OK != result)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK, "Failed to unset Listener");
    }

    return result;
}
/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    setMVJustWorksOptions0
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_iotivity_base_OcProvisioning_setMVJustWorksOptions0
  (JNIEnv *env, jclass clazz, jint options)
{
    OC_UNUSED(clazz);
    LOGI("OcProvisioning_setMVJustWorksOptions0");

    OCStackResult result = OCSecure::setVerifyOptionMask((VerifyOptionBitmask_t)options);

    if (OC_STACK_OK != result)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK, "setMVJustWorksOptions Failed");
    }

    return result;
}


/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    setDisplayPinListener
 * Signature: (Lorg/iotivity/base/OcProvisioning/DisplayPinListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcProvisioning_setDisplayPinListener
  (JNIEnv *env, jclass thiz, jobject jListener)
{
    OC_UNUSED(thiz);
    LOGI("OcProvisioning_setDisplayPinListener");

    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_CALLBACK, "displayPinListener can't be null");
        return;
    }
    delete jniDisplayPinListener;
    jniDisplayPinListener = new JniDisplayPinListener(env, jListener);

    try
    {
        OCStackResult result = OCSecure::setDisplayPinCB(displayPinCB);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to set displayPinListener");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    discoverMOTEnabledDevices1
 * Signature: (I)[Lorg/iotivity/base/OcSecureResource;
 */
JNIEXPORT jobjectArray JNICALL Java_org_iotivity_base_OcProvisioning_discoverMOTEnabledDevices1
  (JNIEnv *env, jclass thiz, jint timeout)
{
    OC_UNUSED(thiz);
    LOGI("OcProvisioning_discoverMOTEnabledDevices1");
#if defined(MULTIPLE_OWNER)
    DeviceList_t list;

    try
    {
        if (timeout < 0)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "Timeout value cannot be negative");
            return nullptr;
        }
        OCStackResult result = OCSecure::discoverMultipleOwnerEnabledDevices(
                (unsigned short)timeout, list);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to discover MOT Enabled devices");
            return nullptr;
        }

        return JniSecureUtils::convertDeviceVectorToJavaArray(env, list);
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return nullptr;
    }
#else
    OC_UNUSED(env);
    OC_UNUSED(timeout);
    ThrowOcException(OC_STACK_INVALID_PARAM, "MULTIPLE_OWNER not enabled");
    return nullptr;
#endif
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    discoverMOTEnabledOwnedDevices1
 * Signature: (I)[Lorg/iotivity/base/OcSecureResource;
 */
JNIEXPORT jobjectArray JNICALL Java_org_iotivity_base_OcProvisioning_discoverMOTEnabledOwnedDevices1
  (JNIEnv *env, jclass thiz, jint timeout)
{
    OC_UNUSED(thiz);
    LOGI("OcProvisioning_discoverMOTEnabledOwnedDevices1");
#if defined(MULTIPLE_OWNER)
    DeviceList_t list;

    try
    {
        if (timeout < 0)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "Timeout value cannot be negative");
            return nullptr;
        }
        OCStackResult result = OCSecure::discoverMultipleOwnedDevices(
                (unsigned short)timeout, list);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to discover MOT Enabled Owned devices");
            return nullptr;
        }

        return JniSecureUtils::convertDeviceVectorToJavaArray(env, list);
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return nullptr;
    }
#else
    OC_UNUSED(env);
    OC_UNUSED(timeout);
    ThrowOcException(OC_STACK_INVALID_PARAM, "MULTIPLE_OWNER not enabled");
    return nullptr;
#endif
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    saveTrustCertChain1
 * Signature: (Lorg/iotivity/base/OcProvisioning/provisionTrustCertChain1;)V
 */
    JNIEXPORT jint JNICALL Java_org_iotivity_base_OcProvisioning_saveTrustCertChain1
(JNIEnv *env, jobject thiz, jbyteArray trustCertChain, jint encodingType)
{
    OC_UNUSED(thiz);
    LOGD("OcProvisioning_saveTrustCertChain1");
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
    jbyte* trustCertChainBytes = env->GetByteArrayElements(trustCertChain, 0);
    // TODO figure out why trustCertChainBytes is created an not used.
    OC_UNUSED(trustCertChainBytes);
    jsize arrayLength = env->GetArrayLength(trustCertChain);
    uint16_t credId = USHRT_MAX;
    unsigned char* trustedCertChar = new unsigned char[arrayLength];
    try
    {
        env->GetByteArrayRegion (trustCertChain, 0, arrayLength, reinterpret_cast<jbyte*>(trustedCertChar));
        OCStackResult result = OCSecure::saveTrustCertChain((uint8_t*)trustedCertChar, arrayLength,
                (OicEncodingType_t)encodingType, &credId);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcProvisioning_saveTrustCertChain1");
            return -1;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
    return (jint)credId;
#else
    OC_UNUSED(env);
    OC_UNUSED(trustCertChain);
    OC_UNUSED(encodingType);
    ThrowOcException(OC_STACK_INVALID_PARAM, "WITH_TLS not enabled");
    return -1;
#endif // __WITH_DTLS__ || __WITH_TLS__
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    saveACL
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcProvisioning_saveACL
  (JNIEnv *env , jclass thiz, jobject jacl)
{
    OC_UNUSED(thiz);
    LOGD("OcProvisioning_saveACL");

    if (!jacl)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "acl can't be null");
    }

    OicSecAcl_t *acl = (OicSecAcl_t*)OICCalloc(1, sizeof(OicSecAcl_t));
    if (!acl)
    {
        ThrowOcException(OC_STACK_NO_MEMORY, "acl allocation failed");
        return;
    }

    if (OC_STACK_OK != JniSecureUtils::convertJavaACLToOCAcl(env, jacl, acl))
    {
        DeleteACLList(acl);
        ThrowOcException(OC_STACK_INVALID_PARAM, "Failed to convert Java acl to OC acl");
        return ;
    }

    try
    {
        OCStackResult result = OCSecure::saveACL(acl);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OCSecure::saveACL Failed");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    doSelfOwnershiptransfer
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcProvisioning_doSelfOwnershiptransfer
  (JNIEnv *env, jclass thiz)
{
    OC_UNUSED(thiz);
    LOGD("OcProvisioning_doSelfOwnershiptransfer");
    try
    {
        OCStackResult result = OCSecure::configSelfOwnership();
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OCSecure::configSelfOwnership Failed");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcProvisioning
 * Method:    setDeviceIdSeed1
 * Signature: (Lorg/iotivity/base/OcProvisioning/provisionTrustCertChain1;)V
 */
    JNIEXPORT jint JNICALL Java_org_iotivity_base_OcProvisioning_setDeviceIdSeed1
(JNIEnv *env, jobject thiz, jbyteArray seed)
{
    OC_UNUSED(thiz);
    LOGD("OcProvisioning_setDeviceIdSeed1");
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
    jbyte* byteSeed = env->GetByteArrayElements(seed, 0);
    jsize arrayLength = env->GetArrayLength(seed);
    try
    {
        env->GetByteArrayRegion (seed, 0, arrayLength, byteSeed);
        OCStackResult result = OCSecure::setDeviceIdSeed((uint8_t*)byteSeed, arrayLength);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcProvisioning_setDeviceIdSeed");
            return -1;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
    return 0;
#else
    OC_UNUSED(env);
    OC_UNUSED(seed);
    ThrowOcException(OC_STACK_INVALID_PARAM, "WITH_TLS not enabled");
    return -1;
#endif // __WITH_DTLS__ || __WITH_TLS__
}
