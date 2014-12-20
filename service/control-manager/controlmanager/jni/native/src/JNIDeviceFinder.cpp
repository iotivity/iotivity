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




#include <jni.h>
#include "OC_Cm_DeviceFinder_IDeviceFinder.h"
#include "CMDeviceFinder.h"
#include "MyDeviceFinderListener.h"
#include "MyResourceResponseListener.h"
#include "list"
#include "JNICMDev.h"

JNIEXPORT jboolean JNICALL Java_OC_Cm_DeviceFinder_IDeviceFinder_refresh( JNIEnv *env,
        jobject thisObj, jlong devFinder )
{
    OC::Cm::DeviceFinder::IDeviceFinder *devFindObj = (OC::Cm::DeviceFinder::IDeviceFinder *) devFinder
            ;
    JNI_TryReturn( NULL != devFindObj, JNI_FALSE,  "Native instance is invalid")

    return devFindObj->refresh() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jlong JNICALL
Java_OC_Cm_DeviceFinder_IDeviceFinder_setDeviceFinderListener__JLOC_Cm_DeviceFinder_IDeviceFinderListener_2(
    JNIEnv *env, jobject thisObj, jlong devFinder, jobject listener )
{
    OC::Cm::DeviceFinder::IDeviceFinder *devFindObj = (OC::Cm::DeviceFinder::IDeviceFinder *) devFinder
            ;
    JNI_TryReturn( NULL != devFindObj, -1,  "Native instance is invalid")

    MyDeviceFinderListener *devFindListener = new MyDeviceFinderListener( listener );
    JNI_TryReturn( NULL != devFindListener, -1,  "Failed to allocate memory")

    devFindObj->setDeviceFinderListener( *devFindListener );
    return (jlong)devFindListener ;
}

JNIEXPORT jlong JNICALL
Java_OC_Cm_DeviceFinder_IDeviceFinder_setDeviceFinderListener__JLOC_Cm_DeviceType_2LOC_Cm_DeviceFinder_IDeviceFinderListener_2
( JNIEnv *env, jobject thisObj, jlong devFinder, jint devType, jobject listener )
{
    OC::Cm::DeviceFinder::IDeviceFinder *devFindObj = (OC::Cm::DeviceFinder::IDeviceFinder *) devFinder
            ;
    JNI_TryReturn( NULL != devFindObj, -1,  "Native instance is invalid")

    MyDeviceFinderListener *devFindListener = new MyDeviceFinderListener( listener );
    JNI_TryReturn( NULL != devFindListener, -1,  "Failed to allocate memory")

    devFindObj->setDeviceFinderListener((OC::Cm::DeviceType) devType, *devFindListener);
    return (jlong)devFindListener ;
}

JNIEXPORT jlong JNICALL
Java_OC_Cm_DeviceFinder_IDeviceFinder_setDeviceFinderListener__JLOC_Cm_DeviceDomain_2LOC_Cm_DeviceType_2LOC_Cm_DeviceFinder_IDeviceFinderListener_2
( JNIEnv *env, jobject thisObj, jlong devFinder, jint domain, jint devType, jobject listener )
{
    OC::Cm::DeviceFinder::IDeviceFinder *devFindObj = (OC::Cm::DeviceFinder::IDeviceFinder *) devFinder
            ;
    JNI_TryReturn( NULL != devFindObj, -1,  "Native instance is invalid")

    MyDeviceFinderListener *devFindListener = new MyDeviceFinderListener( listener );
    JNI_TryReturn( NULL != devFindListener, -1,  "Failed to allocate memory")

    devFindObj->setDeviceFinderListener((OC::Cm::DeviceDomain)domain, (OC::Cm::DeviceType)devType,
                                        *devFindListener);
    return (jlong)devFindListener ;
}

JNIEXPORT jobjectArray JNICALL Java_OC_Cm_DeviceFinder_IDeviceFinder_getDevices__J
( JNIEnv *env, jobject thisObj, jlong devFinder )
{
    jobjectArray resultList = NULL ;

    OC::Cm::DeviceFinder::IDeviceFinder *devFindObj = (OC::Cm::DeviceFinder::IDeviceFinder *) devFinder
            ;
    JNI_TryReturn( NULL != devFindObj, NULL,  "Native instance is invalid")

    std::list<OC::Cm::Device *> deviceList = devFindObj->getDevices();
    if (deviceList.size() != 0)
    {
        jsize size = devFindObj->getDevices().size();
        int index = 0 ;

        for ( std::list<OC::Cm::Device *>::const_iterator itr = deviceList.begin(); itr != deviceList.end();
              itr++, index++ )
        {
            JNICMDevice *jniDevice = (JNICMDevice *) (*itr) ;
            jobject javaDeviceObject = jniDevice->m_device ;

            if (NULL == resultList)
            {
                jclass javaDeviceClass = env->GetObjectClass(javaDeviceObject) ;
                resultList = env->NewObjectArray(size, javaDeviceClass, NULL) ;
            }

            env->SetObjectArrayElement(resultList, index , javaDeviceObject) ;
        }
    }

    return resultList;
}

JNIEXPORT jobjectArray JNICALL Java_OC_Cm_DeviceFinder_IDeviceFinder_getDevices__JI
( JNIEnv *env, jobject thisObj, jlong devFinder, jint deviceType )
{
    jobjectArray resultList = NULL ;

    OC::Cm::DeviceFinder::IDeviceFinder *devFindObj = (OC::Cm::DeviceFinder::IDeviceFinder *) devFinder
            ;
    JNI_TryReturn( NULL != devFindObj, NULL,  "Native instance is invalid")

    OC::Cm::DeviceType dType = (OC::Cm::DeviceType)deviceType;
    std::list<OC::Cm::Device *> deviceList = devFindObj->getDevices(dType);
    if (deviceList.size() != 0)
    {
        jsize size = devFindObj->getDevices().size();
        int index = 0 ;

        for ( std::list<OC::Cm::Device *>::const_iterator itr = deviceList.begin(); itr != deviceList.end();
              itr++, index++ )
        {
            JNICMDevice *jniDevice = (JNICMDevice *) (*itr) ;
            jobject javaDeviceObject = jniDevice->m_device ;

            if (NULL == resultList)
            {
                jclass javaDeviceClass = env->GetObjectClass(javaDeviceObject) ;
                resultList = env->NewObjectArray(size, javaDeviceClass, NULL) ;
            }

            env->SetObjectArrayElement(resultList, index , javaDeviceObject) ;
        }
    }

    return resultList;
}

JNIEXPORT jobjectArray JNICALL Java_OC_Cm_DeviceFinder_IDeviceFinder_getDevices__JII
( JNIEnv *env, jobject thisObj, jlong devFinder, jint devDomain, jint deviceType )
{
    jobjectArray resultList = NULL ;

    OC::Cm::DeviceFinder::IDeviceFinder *devFindObj = (OC::Cm::DeviceFinder::IDeviceFinder *) devFinder
            ;
    JNI_TryReturn( NULL != devFindObj, NULL,  "Native instance is invalid")

    OC::Cm::DeviceDomain dom = (OC::Cm::DeviceDomain)devDomain;
    OC::Cm::DeviceType dType = (OC::Cm::DeviceType)deviceType;
    std::list<OC::Cm::Device *> deviceList = devFindObj->getDevices(dom, dType);
    if (deviceList.size() != 0)
    {
        jsize size = devFindObj->getDevices().size();
        int index = 0 ;

        for ( std::list<OC::Cm::Device *>::const_iterator itr = deviceList.begin(); itr != deviceList.end();
              itr++, index++ )
        {
            JNICMDevice *jniDevice = (JNICMDevice *) (*itr) ;
            jobject javaDeviceObject = jniDevice->m_device ;

            if (NULL == resultList)
            {
                jclass javaDeviceClass = env->GetObjectClass(javaDeviceObject) ;
                resultList = env->NewObjectArray(size, javaDeviceClass, NULL) ;
            }

            env->SetObjectArrayElement(resultList, index , javaDeviceObject) ;
        }
    }

    return resultList;
}

JNIEXPORT void JNICALL Java_OC_Cm_DeviceFinder_IDeviceFinder_removeDeviceFinderListener(
    JNIEnv *env, jobject thisObj, jlong devFinder, jlong listener )
{
    OC::Cm::DeviceFinder::IDeviceFinder *devFindObj = (OC::Cm::DeviceFinder::IDeviceFinder *) devFinder
            ;
    JNI_TryReturnVoid( NULL != devFindObj,  "Native instance is invalid")

    MyDeviceFinderListener *devFindListener = (MyDeviceFinderListener *) listener ;
    JNI_TryReturnVoid( NULL != devFindListener,  "Failed to allocate memory")

    devFindObj->removeDeviceFinderListener(*devFindListener) ;
    delete devFindListener ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_DeviceFinder_IDeviceFinder_addDevice( JNIEnv *env,
        jobject thisObj, jlong devFinder, jint type, jstring juuid, jstring jAddress, jobject  listener)
{
    OC::Cm::DeviceFinder::IDeviceFinder *devFindObj = (OC::Cm::DeviceFinder::IDeviceFinder *) devFinder
            ;
    JNI_TryReturn( NULL != devFindObj, JNI_FALSE, "Native instance is invalid")

    MyResourceResponseListener *responseListener = new MyResourceResponseListener( listener );
    if ( NULL == responseListener)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource Listener object is NULL") ;
        return JNI_FALSE;
    }

    responseListener->forceDeleteOnError = true ;
    responseListener->setRequestCount(2) ;

    std::string dAddress ;
//  JNI_TryReturn( ( true == JNIGlobal::convertToStdString(jAddress, dAddress)), JNI_FALSE, "device address conversion failed") ;
    if ( true != JNIGlobal::convertToStdString(jAddress, dAddress))
    {
        if (responseListener != NULL)
        {
            delete responseListener; responseListener = NULL;
        }
        return JNI_FALSE;
    }

    std::string dUUID ;
//  JNI_TryReturn( ( true == JNIGlobal::convertToStdString(juuid, dUUID)), JNI_FALSE,"uuid conversion failed") ;
    if ( true != JNIGlobal::convertToStdString(juuid, dUUID))
    {
        if (responseListener != NULL)
        {
            delete responseListener; responseListener = NULL;
        }
        return JNI_FALSE;
    }

    devFindObj->addDevice((OC::Cm::DeviceType)type, dUUID, dAddress, *responseListener) ;

    return JNI_TRUE;
}


