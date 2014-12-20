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


/*
 * MyNotificationListener.cpp
 *
 *  Created on: Jul 4, 2013
 *      Author: kirtigupta
 */

#include "Serializable.h"
#include "MyNotificationListener.h"
#include "JNICMDev.h"
#include "Log.h"

using namespace std ;

MyNotificationListener::MyNotificationListener(jobject myListener)
{

    listener = JNIGlobal::getGlobleRef(myListener) ;

}

MyNotificationListener::~MyNotificationListener()
{
    JNIGlobal::releaseGlobleRef(listener) ;
}

void
MyNotificationListener::onNotificationReceived( std::string &uuid, std::string &resource,
        std::string &elementType,
        OC::Cm::Serialization::ISerializable *notification,
        std::string &subscriptionURI,  OC::Cm::Serialization::Xsd::DateTimeType *eventTime)
{

    CM_LOG_DEBUG_A(CM_API, "%s", "MyNotificationListener::onNotificationReceived START ");
    bool isAttached = false ;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return;
    }

    jobject javaSerObject = NULL ;
    if (notification != NULL)
    {
        Serializable *jniSer = (Serializable *) (notification) ;
        javaSerObject = jniSer->ser_handler ;
    }
    jstring javaEventTime = NULL;
    if (eventTime != NULL)
    {
        javaEventTime = JNIGlobal::convertToJString(eventTime->value);
    }

//  const char* cAddress = resource.c_str();
    jstring javaResourceStr = JNIGlobal::convertToJString(resource);
    jstring javaElementStr = JNIGlobal::convertToJString(elementType);
    jstring juuid = JNIGlobal::convertToJString(uuid);
    jstring javasubscriptionURI = JNIGlobal::convertToJString(subscriptionURI);


    JNIGlobal::executeVoidMethod("OC/Cm/Notification/INotificationListener", "onNotificationReceived",
                                 "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;LOC/Cm/Serialization/ISerializable;Ljava/lang/String;Ljava/lang/String;)V",
                                 listener, juuid, javaResourceStr, javaElementStr, javaSerObject, javasubscriptionURI,
                                 javaEventTime);
    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }

    JNIGlobal::releaseLocalRef(javaResourceStr) ;
    JNIGlobal::releaseLocalRef(javaElementStr) ;
    JNIGlobal::releaseLocalRef(juuid) ;
    JNIGlobal::releaseLocalRef(javasubscriptionURI) ;
    JNIGlobal::releaseLocalRef(javaEventTime) ;

    CM_LOG_DEBUG_A(CM_API, "%s", "MyNotificationListener::onNotificationReceived END ");
}

void
MyNotificationListener::onMulticastedNotifcationReceived( const OC::Cm::Device &device,
        const std::string &elementType,
        const OC::Cm::Serialization::ISerializable *notification )
{
    bool isAttached = false ;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return ;
    }

    JNICMDevice *jniDevice = (JNICMDevice *) (&device) ;
    jobject javaDeviceObject = jniDevice->m_device ;

    Serializable *jniSer = (Serializable *) (&notification) ;
    jobject javaSerObject = jniSer->ser_handler ;

    const char *cAddress = elementType.c_str();
    jstring javaElementStr = pEnv->NewStringUTF(cAddress);
    if (NULL == NULL)
    {
        return ;
    }

    JNIGlobal::executeVoidMethod("OC/Cm/Notification/INotificationListener",
                                 "onMulticastedNotifcationReceived",
                                 "(LOC/Cm/Device;Ljava/lang/String;LOC/Cm/Serialization/ISerializable;)V", listener,
                                 javaDeviceObject, javaElementStr, javaSerObject);
    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }
}
