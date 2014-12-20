//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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




#include "OC_Cm_Device.h"
#include "DeviceType.h"
#include "DeviceDomain.h"
#include "Serializable.h"
#include "Device.h"
#include "CMSerialization.h"
#include "JNICMDev.h"
#include "Serializable.h"
#include "iostream"

#ifdef REMOTE_ACCESS_SUPPORT
//#include "DeviceToken.h"
#endif

JNICMDevice::JNICMDevice( jobject jdevice, jint type, jint domain)
    : OC::Cm::Device((OC::Cm::DeviceType)type, (OC::Cm::DeviceDomain)domain)
{
    m_device = JNIGlobal::getGlobleRef(jdevice) ;
}

JNICMDevice::~JNICMDevice()
{
    JNIGlobal::executeVoidMethod("OC/Cm/Device", "onNativeDelete", "()V", m_device);
    JNIGlobal::releaseGlobleRef(m_device) ;
}

bool
JNICMDevice::loadDeviceDescription(OC::Cm::Serialization::ISerializable &serializable)
{
    Serializable *data = dynamic_cast<Serializable *> (&serializable) ;
    if (data != NULL)
    {
        return JNIGlobal::executeBoolMethod("OC/Cm/Device", "loadDeviceDescription",
                                            "(LOC/Cm/Serialization/ISerializable;)Z", m_device, data->ser_handler) ;
    }

    return false;
}

bool
JNICMDevice::getDeviceDescription(OC::Cm::Serialization::ISerializable **ppSerializable)
{
    jobject obj = JNIGlobal::executeObjectMethod ("OC/Cm/Device", "getDeviceDescription",
                  "()LOC/Cm/Serialization/ISerializable;", m_device) ;
    if (NULL == obj)
    {
        return false ;
    }

    *ppSerializable = new Serializable(obj) ;
    JNIGlobal::releaseLocalRef(obj);

    return ( NULL == *ppSerializable ) ? false : true ;
}

std::string
JNICMDevice::getSubscriptionsLink()
{
    jstring strLink = (jstring)JNIGlobal::executeObjectMethod("OC/Cm/Device", "getSubscriptionsLink",
                      "()Ljava/lang/String;", m_device) ;
    if (NULL == strLink)
    {
        return ""; // TODO: Check return value
    }

    std::string subsLink ;
    if (false == JNIGlobal::convertToStdString(strLink, subsLink))
        return "";

    JNIGlobal::releaseLocalRef(strLink);

    return subsLink;
}

JNIEXPORT jlong JNICALL Java_OC_Cm_Device_constructNative
(JNIEnv *env, jobject thisObj, jint devType, jint devDomain)
{
    std::cout << "in Java_OC_Cm_Device_constructNative " << std::endl ;
    OC::Cm::Device *deviceObj = new JNICMDevice(thisObj, devType, devDomain) ;
    JNI_TryReturn( NULL != deviceObj, 0, "Memory Allocation failed : Out of memory")

    return (jlong)(deviceObj);
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_deleteNative
(JNIEnv *, jobject, jlong dev)
{
    std::cout << "in Java_OC_Cm_Device_deleteNative " << std::endl ;
    OC::Cm::Device *device = (OC::Cm::Device *) dev;

    if (NULL != device)
    {
        delete device;
    }
}

JNIEXPORT jlong JNICALL Java_OC_Cm_Device_getContext
(JNIEnv *, jobject, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev;
    if (NULL != device)
    {
        return (jlong) device->getContext() ;
    }

    return 0 ;
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Device_getAddress
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(device->getAddress()) ;
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Device_getProtocol
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(device->getProtocol()) ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setAddress
(JNIEnv *env, jobject thisObj, jlong dev, jstring address )
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    std::string dAddress ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(address, dAddress)),
                       "device address conversion failed") ;

    CM_LOG_DEBUG_A(CM_API, "IP Address = %s", dAddress.c_str());

    device->setAddress(dAddress.c_str());
}


JNIEXPORT void JNICALL Java_OC_Cm_Device_setProtocol
(JNIEnv *env, jobject thisObj, jlong dev, jstring address )
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    std::string dAddress ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(address, dAddress)),
                       "device address conversion failed") ;

    device->setProtocol(dAddress.c_str());
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setContext
(JNIEnv *env, jobject thisObj, jlong dev, jlong context )
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    JNI_TryReturnVoid( ( 0 != context), "context is invalid") ;

    device->setContext((OC::Cm::IContext *)context) ;
}



JNIEXPORT jstring JNICALL Java_OC_Cm_Device_getDeviceDescriptionUri
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(device->getDeviceDescriptionUri()) ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setDeviceDescriptionUri
(JNIEnv *env, jobject thisObj, jlong dev, jstring devUri)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    std::string dAddress ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(devUri, dAddress)),
                       "devUri conversion failed") ;

    device->setDeviceDescriptionUri(dAddress.c_str());
}


JNIEXPORT jobject JNICALL Java_OC_Cm_Device_getDeviceType
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, NULL, "Native device is invalid")

    jobject jdeviceType = JNIGlobal::getEnum("OC/Cm/DeviceType", "getDeviceType",
                          "(I)LOC/Cm/DeviceType;" , (int)device->getDeviceType());
    JNI_TryReturn( NULL != jdeviceType, NULL, "Failed to get device type")

    return (jobject)jdeviceType;
}


JNIEXPORT void JNICALL Java_OC_Cm_Device_setDeviceType
(JNIEnv *env, jobject thisObj, jlong dev, jint devType)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    device->setDeviceType((OC::Cm::DeviceType)devType);
}

JNIEXPORT jobject JNICALL Java_OC_Cm_Device_getDomain
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, NULL, "Native device is invalid")

    jobject jdomain = JNIGlobal::getEnum("OC/Cm/DeviceDomain", "getDeviceDomain",
                                         "(I)LOC/Cm/DeviceDomain;" , (int)device->getDomain());
    JNI_TryReturn( NULL != jdomain, NULL, "Failed to get domain")

    return (jobject)jdomain;
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setDomain
(JNIEnv *env, jobject thisObj, jlong dev, jint domain)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    device->setDomain((OC::Cm::DeviceDomain)domain);
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Device_getUUID
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(device->getUUID()) ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setUUID
(JNIEnv *env, jobject thisObj, jlong dev, jstring uuid)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    std::string dAddress ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(uuid, dAddress)),
                       "device address conversion failed") ;

    device->setUUID(dAddress.c_str());
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Device_isDeviceDescriptionLoaded
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, JNI_FALSE, "Native device is invalid")

    return device->isDeviceDescriptionLoaded() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Device_setDeviceDescriptionLoaded
(JNIEnv *env, jobject thisObj, jlong dev, jboolean flag)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, JNI_FALSE, "Native device is invalid")

    if (device)
    {
        device->setDeviceDescriptionLoaded(flag == JNI_TRUE) ;
    }

    return JNI_TRUE ;
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Device_getDescription
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(device->getDescription()) ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setDescription
(JNIEnv *env, jobject thisObj, jlong dev, jstring description)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    std::string desc ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(description, desc)),
                       "device description conversion failed") ;

    device->setDescription(desc.c_str());
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Device_getManufacturer
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(device->getManufacturer()) ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setManufacturer
(JNIEnv *env, jobject thisObj, jlong dev, jstring manufacturer)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    std::string manf ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(manufacturer, manf)),
                       "device manufacturer conversion failed") ;

    device->setManufacturer(manf.c_str());
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Device_getDeviceSubType
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *)dev;
    JNI_TryReturn(NULL != device, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(device->getDeviceSubType());
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setDeviceSubType
(JNIEnv *env, jobject thisObj, jlong dev, jstring deviceSubType)
{
    OC::Cm::Device *device = (OC::Cm::Device *)dev;
    JNI_TryReturnVoid(NULL != device, "Native device is invalid")

    std::string devSubTyp;
    JNI_TryReturnVoid((true == JNIGlobal::convertToStdString(deviceSubType, devSubTyp)),
                      "device deviceSubType conversion failed");

    device->setDeviceSubType(devSubTyp.c_str());
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Device_getModelID
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(device->getModelID()) ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setModelID
(JNIEnv *env, jobject thisObj, jlong dev, jstring id)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    std::string mID ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(id, mID)),
                       "device ModelID conversion failed") ;

    device->setModelID(mID.c_str());
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Device_getSerialNumber
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(device->getSerialNumber()) ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setSerialNumber
(JNIEnv *env, jobject thisObj, jlong dev, jstring num)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    std::string sNum ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(num, sNum)),
                       "device SerialNumber conversion failed") ;

    device->setSerialNumber(sNum.c_str());
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Device_getSalesLocation
(JNIEnv *env, jobject thisObj, jlong dev)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturn( NULL != device, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(device->getSalesLocation()) ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setSalesLocation
(JNIEnv *env, jobject thisObj, jlong dev, jstring jLoc)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    std::string location ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(jLoc, location)),
                       "device SalesLocation conversion failed") ;

    device->setSalesLocation(location.c_str());
}

JNIEXPORT jobject JNICALL Java_OC_Cm_Device_getSupportedResources
(JNIEnv *, jobject, jlong)
{
    // Needs to implement
    return NULL;
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setSupportedResource
(JNIEnv *env, jobject thisObj, jlong dev, jstring jresource)
{
    OC::Cm::Device *device = (OC::Cm::Device *) dev ;
    JNI_TryReturnVoid( NULL != device, "Native device is invalid")

    std::string resource ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(jresource, resource)),
                       "ResourceName conversion failed") ;

    device->setSupportedResource(resource.c_str());
}

/* SRIB - NOT Supported

JNIEXPORT jboolean JNICALL Java_OC_Cm_Device_deRegisterFromSHS
(JNIEnv *env, jobject thisObj, jlong dev)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Device* device = (OC::Cm::Device*) dev ;
    JNI_TryReturn( NULL != device, JNI_FALSE, "Native device is invalid")
    return device->deRegisterFromSHS() ? JNI_TRUE: JNI_FALSE;
#else
    return JNI_TRUE;
#endif
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Device_getDeviceOnlineStatus
(JNIEnv *env, jobject thisObj, jlong dev)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Device* device = (OC::Cm::Device*) dev ;
    JNI_TryReturn( NULL != device, JNI_FALSE, "Native device is invalid")

    return device->getDeviceOnlineStatus() ? JNI_TRUE: JNI_FALSE;
#else
    return JNI_TRUE;
#endif
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setTokenGrantStatus
(JNIEnv *env, jobject thisObj, jlong dev, jboolean status)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Device* device = (OC::Cm::Device*)dev;
    JNI_TryReturnVoid(NULL != device, "Native device is invalid");

    device->setTokenGrantStatus(status == JNI_TRUE);
#endif
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_updateDeviceTokenList
  (JNIEnv *env, jobject thisObj, jlong dev, jlongArray jdevtokenlist)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Device* device = (OC::Cm::Device*)dev;
    JNI_TryReturnVoid(NULL != device, "Native device is invalid");

    if (NULL == jdevtokenlist) {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource Path array is NULL") ;
    }

    jboolean var = false;

    if (NULL != jdevtokenlist) {
        int count = env->GetArrayLength(jdevtokenlist);
        jlong* nativeList = env->GetLongArrayElements(jdevtokenlist, &var);
        for (int i=0; i< count; i++) {
            OC::Cm::DeviceToken* devToken = (OC::Cm::DeviceToken*) nativeList[i];
            if (devToken != NULL) {
                device->setPeerTokenIntoUUIDMap(devToken->getUUID(), devToken->getDeviceToken());
            }
        }
    }
#endif
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Device_initiateTokenRequest
(JNIEnv *env, jobject thisObj, jlong dev, jstring jIP, jstring jPort)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Device* device = (OC::Cm::Device*)dev;
    JNI_TryReturn( NULL != device, JNI_FALSE, "Native device is invalid")

    std::string mIP ;
    JNI_TryReturn((true == JNIGlobal::convertToStdString(jIP, mIP)), JNI_FALSE, "device IP conversion failed") ;
    return device->initiateTokenRequestUsingUUID(mIP, false);
#else
    return JNI_TRUE;
#endif
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Device_initiateTokenRequestUsingUUID
(JNIEnv *env, jobject thisObj, jlong dev, jstring jUUID, jboolean fromSimulator)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Device* device = (OC::Cm::Device*)dev;
    JNI_TryReturn( NULL != device, JNI_FALSE, "Native device is invalid")

    std::string mUUID;
    JNI_TryReturn((true == JNIGlobal::convertToStdString(jUUID, mUUID)), JNI_FALSE, "device UUID conversion failed") ;

    return device->initiateTokenRequestUsingUUID(mUUID, fromSimulator == JNI_TRUE);
#else
    return JNI_TRUE;
#endif
}


JNIEXPORT jstring JNICALL Java_OC_Cm_Device_getMyDeviceToken
  (JNIEnv *env, jobject thisObj, jlong dev)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Device* device = (OC::Cm::Device*)dev;
    JNI_TryReturn(NULL != device, NULL, "Native device is invalid");

    return JNIGlobal::convertToJString(device->getMyDeviceToken());
#else
    return NULL;
#endif
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setMyDeviceToken
  (JNIEnv *env, jobject thisObj, jlong dev, jstring jToken)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Device* device = (OC::Cm::Device*)dev;
    JNI_TryReturnVoid(NULL != device, "Native device is invalid");

    std::string mToken ;
    JNI_TryReturnVoid((true == JNIGlobal::convertToStdString(jToken, mToken)), "device Token conversion failed") ;

    device->setMyDeviceToken(mToken) ;
#endif
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Device_getPeerTokenFromUUIDMap
  (JNIEnv *env, jobject thisObj, jlong dev, jstring jUUID)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Device* device = (OC::Cm::Device*)dev;
    JNI_TryReturn(NULL != device, NULL, "Native device is invalid");

    std::string mUUID ;
    JNI_TryReturn((true == JNIGlobal::convertToStdString(jUUID, mUUID)), NULL, "device UUID conversion failed") ;

    return JNIGlobal::convertToJString(device->getPeerTokenFromUUIDMap(mUUID)) ;
#else
    return NULL;
#endif
}

JNIEXPORT void JNICALL Java_OC_Cm_Device_setPeerTokenIntoUUIDMap
  (JNIEnv *env, jobject thisObj, jlong dev, jstring jUUID, jstring jToken)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Device* device = (OC::Cm::Device*)dev;
    JNI_TryReturnVoid(NULL != device, "Native device is invalid");

    std::string mUUID ;
    JNI_TryReturnVoid((true == JNIGlobal::convertToStdString(jUUID, mUUID)), "device UUID conversion failed") ;

    std::string mToken ;
    JNI_TryReturnVoid((true == JNIGlobal::convertToStdString(jToken, mToken)), "device Token conversion failed") ;

    device->setPeerTokenIntoUUIDMap(mUUID, mToken);
#endif
}

 SRIB - NOT Supported */
