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
 * MyResourceResponseListener.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: kirtigupta
 */

#include "MyResourceResponseListener.h"
//#include "Log.h"
#include <jni.h>
#include "Serializable.h"


using namespace std ;

MyResourceResponseListener::MyResourceResponseListener(jobject myListener)
{
    listener = JNIGlobal::getGlobleRef(myListener) ;
    m_reqCount = 1 ;
    forceDeleteOnError = false ;
}

MyResourceResponseListener::~MyResourceResponseListener()
{
    JNIGlobal::releaseGlobleRef(listener) ;
}

void
MyResourceResponseListener::OnGetResponseReceived(int clientSessionId, int status,
        OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response)
{
    /*jclass        clsSer  = NULL;
    jmethodID   m_constructor   = NULL;
    jobject     jheaderList = NULL  ;

    //Obtaining Classes
    clsSer = env->FindClass("OC/Cm/Connector/HeadersList");

    //Obtaining Method IDs
    if (clsSer != NULL)
    {
        m_constructor = env->GetMethodID(clsSer, "<init>", "(J)V");
        jheaderList = env->NewObject(clsSer, m_constructor, (jlong)(&headers));
    }*/
    jobject     jheaderList = NULL  ;

    jheaderList = JNIGlobal::constructObject("OC/Cm/Serialization/ISerializer", "(J)V",
                  (jlong)(&headers)) ;

    Serializable *jniSer = (Serializable *) (response) ;

    jobject javaSerObject = NULL ;

    if (jniSer != NULL)
    {
        javaSerObject = jniSer->ser_handler ;
    }


    JNIGlobal::executeVoidMethod("OC/Cm/Client/Resource/IResourceResponseListener",
                                 "OnGetResponseReceived", "(IILOC/Cm/Connector/HeadersList;LOC/Cm/Serialization/ISerializable;)V",
                                 listener, clientSessionId, status, jheaderList, javaSerObject);

    JNIGlobal::releaseLocalRef(jheaderList);
}

void
MyResourceResponseListener::OnPutResponseReceived(int clientSessionId, int status,
        OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response)
{
    /*jclass        clsSer  = NULL;
    jmethodID   m_constructor   = NULL;
    jobject     jheaderList = NULL  ;

    //Obtaining Classes
    clsSer = env->FindClass("OC/Cm/Connector/HeadersList");

    //Obtaining Method IDs
    if (clsSer != NULL)
    {
        m_constructor = env->GetMethodID(clsSer, "<init>", "(J)V");
        jheaderList = env->NewObject(clsSer, m_constructor, (jlong)(&headers));
    }
    */
    jobject     jheaderList = NULL  ;

    jheaderList = JNIGlobal::constructObject("OC/Cm/Serialization/ISerializer", "(J)V",
                  (jlong)(&headers)) ;

    Serializable *jniSer = (Serializable *) (response) ;
    jobject javaSerObject = NULL ;

    if (jniSer != NULL)
    {
        javaSerObject = jniSer->ser_handler ;
    }

    JNIGlobal::executeVoidMethod("OC/Cm/Client/Resource/IResourceResponseListener",
                                 "OnPutResponseReceived", "(IILOC/Cm/Connector/HeadersList;LOC/Cm/Serialization/ISerializable;)V",
                                 listener, clientSessionId, status, jheaderList, javaSerObject);

    JNIGlobal::releaseLocalRef(jheaderList);
}

void
MyResourceResponseListener::OnPostResponseReceived(int clientSessionId, int status,
        OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response)
{
    /*jclass        clsSer  = NULL;
    jmethodID   m_constructor   = NULL;
    jobject     jheaderList = NULL  ;

    //Obtaining Classes
    clsSer = env->FindClass("OC/Cm/Connector/HeadersList");

    //Obtaining Method IDs
    if (clsSer != NULL)
    {
        m_constructor = env->GetMethodID(clsSer, "<init>", "(J)V");
        jheaderList = env->NewObject(clsSer, m_constructor, (jlong)(&headers));
    }*/

    jobject     jheaderList = NULL  ;

    jheaderList = JNIGlobal::constructObject("OC/Cm/Serialization/ISerializer", "(J)V",
                  (jlong)(&headers)) ;

    Serializable *jniSer = (Serializable *) (response) ;
    jobject javaSerObject = NULL ;

    if (jniSer != NULL)
    {
        javaSerObject = jniSer->ser_handler ;
    }

    JNIGlobal::executeVoidMethod("OC/Cm/Client/Resource/IResourceResponseListener",
                                 "OnPostResponseReceived", "(IILOC/Cm/Connector/HeadersList;LOC/Cm/Serialization/ISerializable;)V",
                                 listener, clientSessionId, status, jheaderList, javaSerObject);

    JNIGlobal::releaseLocalRef(jheaderList);
}

void
MyResourceResponseListener::onDeleteResponseReceived(int clientSessionId, int status,
        OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response)
{
    /*jclass        clsSer  = NULL;
    jmethodID   m_constructor   = NULL;
    jobject     jheaderList = NULL  ;

    //Obtaining Classes
    clsSer = env->FindClass("OC/Cm/Connector/HeadersList");

    //Obtaining Method IDs
    if (clsSer != NULL)
    {
        m_constructor = env->GetMethodID(clsSer, "<init>", "(J)V");
        jheaderList = env->NewObject(clsSer, m_constructor, (jlong)(&headers));
    }*/
    jobject     jheaderList = NULL  ;

    jheaderList = JNIGlobal::constructObject("OC/Cm/Serialization/ISerializer", "(J)V",
                  (jlong)(&headers)) ;

    Serializable *jniSer = (Serializable *) (response) ;

    jobject javaSerObject = NULL ;

    if (jniSer != NULL)
    {
        javaSerObject = jniSer->ser_handler ;
    }

    JNIGlobal::executeVoidMethod("OC/Cm/Client/Resource/IResourceResponseListener",
                                 "onDeleteResponseReceived", "(IILOC/Cm/Connector/HeadersList;LOC/Cm/Serialization/ISerializable;)V",
                                 listener, clientSessionId, status, jheaderList, javaSerObject);

    JNIGlobal::releaseLocalRef(jheaderList);
}

void
MyResourceResponseListener::onSubscribeResponseReceived( int clientSessionId, int status,
        std::string &subscriptionUri )
{
    jstring jsubsriptionUri = JNIGlobal::convertToJString(subscriptionUri);
    JNIGlobal::executeVoidMethod("OC/Cm/Client/Resource/IResourceResponseListener",
                                 "onSubscribeResponseReceived", "(IILjava/lang/String;)V", listener, clientSessionId, status,
                                 jsubsriptionUri);

    JNIGlobal::releaseLocalRef(jsubsriptionUri);
}

void
MyResourceResponseListener::onUnSubscribeResponseReceived(int clientSessionId, int status )
{
    JNIGlobal::executeVoidMethod("OC/Cm/Client/Resource/IResourceResponseListener",
                                 "onUnSubscribeResponseReceived", "(II)V", listener, clientSessionId, status);
}

void
MyResourceResponseListener::onErrorReceived( int clientSessionId, int status, OC::Cm::CMError error,
        OC::Cm::Serialization::ISerializable *errorMessage )
{
    //JNIEnv* pEnv = JNIGlobal::getEnv(true, isAttached) ;

    jobject jErrorObject = JNIGlobal::getEnum("OC/Cm/CMErrorCode", "getErrorCode",
                           "(I)LOC/Cm/CMErrorCode;" , (int)error.getErrorCode());

    // find class(I)LOC/Cm/CMErrorCode;
//  ClsErrorCode = pEnv->FindClass("OC/Cm/CMErrorCode");

    // find static method
//  if (ClsErrorCode != NULL)
//  {
//      m_getErrorCode = pEnv->GetStaticMethodID(ClsErrorCode,"getErrorCode", "(I)LOC/Cm/CMErrorCode;");
//  }

    // invoke method
    //jobject jErrorObject = pEnv->CallStaticObjectMethod(ClsErrorCode,m_getErrorCode,jdomain);

    if (forceDeleteOnError)
    {
        m_reqCount = 1 ;
    }

    Serializable *jniSer = (Serializable *) (errorMessage) ;

    jobject javaSerObject = NULL ;

    if (jniSer != NULL)
    {
        javaSerObject = jniSer->ser_handler ;
    }

    jstring errMsg = JNIGlobal::convertToJString(error.getErrorMessage()) ;
    JNIGlobal::executeVoidMethod("OC/Cm/Client/Resource/IResourceResponseListener", "onErrorReceived",
                                 "(IILOC/Cm/CMErrorCode;Ljava/lang/String;LOC/Cm/Serialization/ISerializable;)V", listener,
                                 clientSessionId, status, jErrorObject, errMsg, javaSerObject );

    JNIGlobal::releaseLocalRef(errMsg);
    JNIGlobal::releaseLocalRef(jErrorObject);
}

void
MyResourceResponseListener::onRequestComplete(int requestId, std::string requestPayload,
        std::string responsePayload)
{
    bool    isAttached      = false ;
    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return ;
    }

    const char *stringRequestPayload = requestPayload.c_str();
    jstring jrequestPayload = pEnv->NewStringUTF(stringRequestPayload);

    const char *stringResponsePayload = responsePayload.c_str();
    jstring jresponsePayload = pEnv->NewStringUTF(stringResponsePayload);

    JNIGlobal::executeVoidMethod("OC/Cm/Client/Resource/IResourceResponseListener", "onRequestComplete",
                                 "(ILjava/lang/String;Ljava/lang/String;)V", listener, requestId, jrequestPayload, jresponsePayload);
    m_reqCount = m_reqCount - 1 ;

    if (m_reqCount <= 0)
    {
        delete this ;
    }

    JNIGlobal::releaseLocalRef(jrequestPayload);
    JNIGlobal::releaseLocalRef(jresponsePayload);
}

void
MyResourceResponseListener::setRequestCount(int reqCount)
{
    m_reqCount = reqCount ;
}


