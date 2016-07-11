/*
* //******************************************************************
* //
* // Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "JniOcDirectPairDevice.h"
#include "OCDirectPairing.h"
#include "OCDirectPairing.h"
using namespace OC;
namespace PH = std::placeholders;

JniOcDirectPairDevice::JniOcDirectPairDevice(std::shared_ptr<OC::OCDirectPairing> directPairingDevice)
    : m_sharedDirectPairDevice(directPairingDevice)
{
}

JniOcDirectPairDevice::~JniOcDirectPairDevice()
{
    LOGD("~JniOcDirectPairDevice()");
    m_sharedDirectPairDevice = nullptr;
}

std::string JniOcDirectPairDevice::getHost()
{
   return m_sharedDirectPairDevice->getHost();
}

std::vector<OCPrm_t> JniOcDirectPairDevice::getPairingMethods()
{

   return m_sharedDirectPairDevice->getPairingMethods();
}

OCConnectivityType JniOcDirectPairDevice::connectivityType()
{
    return m_sharedDirectPairDevice->getConnType();
}

JniOcDirectPairDevice* JniOcDirectPairDevice::getJniOcDirectPairDevicePtr(JNIEnv *env, jobject thiz)
{
    JniOcDirectPairDevice *dpDev = GetHandle<JniOcDirectPairDevice>(env, thiz);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to get native handle from OcDirectPairingDevice");
    }
    if (!dpDev)
    {
        ThrowOcException(JNI_NO_NATIVE_POINTER, "");
    }
    return dpDev;
}
std::shared_ptr<OC::OCDirectPairing> JniOcDirectPairDevice::getPtr()
{
    return m_sharedDirectPairDevice;
}

JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcDirectPairDevice_getHost
(JNIEnv *env, jobject thiz)
{

    LOGD("OcDirectPairing_getHost");

    JniOcDirectPairDevice* device = JniOcDirectPairDevice::getJniOcDirectPairDevicePtr(env,thiz);
    if (!device)
    {
        return nullptr;
    }

    std::string dev  = device->getHost();
    return env->NewStringUTF(dev.c_str());
}

JNIEXPORT jintArray JNICALL Java_org_iotivity_base_OcDirectPairDevice_getPairingMethods
        (JNIEnv *env, jobject thiz)
{

    LOGD("OcDirectPairing_getPairingMethods");

    std::vector<jint> pairingMethodList;
    JniOcDirectPairDevice* device = JniOcDirectPairDevice::getJniOcDirectPairDevicePtr(env,thiz);
    if (!device)
    {
        return nullptr;
    }

    std::vector<OCPrm_t> pairingMethods = device->getPairingMethods();
    return JniOcDirectPairDevice::JconvertIntVectorToJavaList(env,pairingMethods);
}


jintArray JniOcDirectPairDevice::JconvertIntVectorToJavaList(JNIEnv *env, std::vector<OCPrm_t> &vector)
{

    jsize len = static_cast<jsize>(vector.size());

    jintArray intArray = env->NewIntArray(len);
    if (!intArray)
    {
        return nullptr;
    }

    env->SetIntArrayRegion(intArray, (jsize)0, len, (const jint*)&vector[0]);

    if (env->ExceptionCheck())
    {
        LOGE("ArrayIndexOutOfBoundsException in  JconvertIntVectorToJavaList");
    }
    return intArray;
}

/*
* Class:     org_iotivity_base_OcDirectPairDevice
* Method:    getConnectivityTypeN
* Signature: ()I
*/
JNIEXPORT jint JNICALL Java_org_iotivity_base_OcDirectPairDevice_getConnectivityTypeN
(JNIEnv *env, jobject thiz)
{
    LOGD("OcDirectPairDevice_getConnectivityType");
    JniOcDirectPairDevice* device = JniOcDirectPairDevice::getJniOcDirectPairDevicePtr(env,thiz);
    if (!device)
    {
        return -1;
    }

    OCConnectivityType connectivityType = device->connectivityType();
    return static_cast<jint>(connectivityType);
}
