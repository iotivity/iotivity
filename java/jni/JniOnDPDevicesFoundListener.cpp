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
#include "JniOnDPDevicesFoundListener.h"
#include "JniOcDirectPairDevice.h"
#include "OCApi.h"
using namespace OC;

JniOnDPDevicesFoundListener::JniOnDPDevicesFoundListener(JNIEnv *env, jobject jListener,
    RemoveListenerCallback removeListenerCallback)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
    m_removeListenerCallback = removeListenerCallback;
}

jobject JniOnDPDevicesFoundListener::convertdpDevVectorToJavaList(JNIEnv *env, PairedDevices DPdevList)
{
    jobject jResultList = env->NewObject(g_cls_LinkedList, g_mid_LinkedList_ctor);
    if (!jResultList)
    {
        return nullptr;
    }

    for (size_t i = 0; i < DPdevList.size(); ++i)
    {
        JniOcDirectPairDevice *device = new JniOcDirectPairDevice(DPdevList.at(i));
        if (!device)
        {
            return nullptr;
        }

        jstring jStr = env->NewStringUTF(((DPdevList.at(i))->getDeviceID()).c_str());
        if (!jStr)
        {
            return nullptr;
        }
        jobject jresult = env->NewObject(g_cls_OcDirectPairDevice,
                g_mid_OcDirectPairDevice_dev_ctor,jStr);
        if (!jresult)
        {
            return nullptr;
        }

        SetHandle<JniOcDirectPairDevice>(env, jresult, device);

        env->CallBooleanMethod(jResultList, g_mid_LinkedList_add_object, jresult);
        if (env->ExceptionCheck())
        {
            return nullptr;
        }
        env->DeleteLocalRef(jresult);
        env->DeleteLocalRef(jStr);
    }
    return jResultList;
}

JniOnDPDevicesFoundListener::~JniOnDPDevicesFoundListener()
{
    LOGI("~JniOnDPDevicesFoundListener()");
    if (m_jwListener)
    {
        jint ret = JNI_ERR;
        JNIEnv *env = GetJNIEnv(ret);
        if (nullptr == env)
        {
            return;
        }
        env->DeleteWeakGlobalRef(m_jwListener);
        m_jwListener = nullptr;
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
    }
}

void JniOnDPDevicesFoundListener::directPairingDevicesCallback(PairedDevices paringDevicesList, DPFunc func)
{
    jint ret = JNI_ERR;
    JNIEnv *env = GetJNIEnv(ret);
    if (nullptr == env)
    {
        return;
    }

    jobject jListener = env->NewLocalRef(m_jwListener);
    if (!jListener)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    jclass clsL = env->GetObjectClass(jListener);

    if (!clsL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    jobject jpairingDevicesList = convertdpDevVectorToJavaList(env, paringDevicesList);
    std::string calledFunc;
    switch (func)
    {
        case DPFunc::FIND_DIRECT_PAIRED_DEV_LIST:
            {
                calledFunc = "onFindDirectPairingListener";
            }
            break;
        case DPFunc::GET_PAIRED_DEV_LIST:
            {
                calledFunc = "onGetDirectPairedListener";
            }
            break;
        default:
            {
                checkExAndRemoveListener(env);
                if (JNI_EDETACHED == ret)
                {
                    g_jvm->DetachCurrentThread();
                }
            }
            return;
    }

    jmethodID midL = env->GetMethodID(clsL, calledFunc.c_str(), "(Ljava/util/List;)V");

    if (!midL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    env->CallVoidMethod(jListener, midL, jpairingDevicesList);
    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
    }

    checkExAndRemoveListener(env);
    if (JNI_EDETACHED == ret)
    {
        g_jvm->DetachCurrentThread();
    }
}

void JniOnDPDevicesFoundListener::checkExAndRemoveListener(JNIEnv* env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_removeListenerCallback(env, m_jwListener);
        env->Throw((jthrowable)ex);
    }
    else
    {
        m_removeListenerCallback(env, m_jwListener);
    }
}
