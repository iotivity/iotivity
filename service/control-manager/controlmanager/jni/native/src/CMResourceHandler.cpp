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
 * CMResourceHandler.cpp
 *
 *  Created on: Jul 8, 2013
 *      Author: kirtigupta
 */

#include "CMResourceHandler.h"
#include "JNIGlobal.h"
#include "Serializable.h"
#include "iostream"
#include "Log.h"

using namespace std ;

CMResourceHandler::CMResourceHandler(jlong context, jobject handler)
    : OC::Cm::Server::ResourceHandler::SyncResourceHandler((OC::Cm::IContext *) context)
{
    //res_handler = JNIGlobal::getGlobleRef(handler) ;
    res_handler = (handler) ;
}

CMResourceHandler::~CMResourceHandler()
{
    JNIGlobal::releaseGlobleRef(res_handler) ;
}

bool
CMResourceHandler::handleSyncResource( OC::Cm::Connector::Server::ServerSession &session,
                                       std::string methodId,
                                       int &statusCode,
                                       OC::Cm::Serialization::ISerializable  *request,
                                       OC::Cm::Serialization::ISerializable  *response)
{
    CM_LOG_DEBUG_A(CM_JNI, "%s", "Enter");

    bool isAttached = false ;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return false;
    }

    jobject     jserverSesison = NULL  ;

    jserverSesison = JNIGlobal::constructObject("OC/Cm/Connector/Server/ServerSession", "(JJ)V",
                     (jlong)(&session), (jlong)p_context) ;

    //Serializable *jniReq = (Serializable*) (&request) ;
//  Serializable *jniResp = (Serializable*) (&response) ;
    jobject reqObj = NULL ;
    jobject respObj = NULL ;

    if (request != NULL)
    {
        CM_LOG_DEBUG_A(CM_JNI, "%s", "Request object is not NULL");

        reqObj = ((Serializable *) (request))->ser_handler ;

        if (reqObj == NULL)
            CM_LOG_DEBUG_A(CM_JNI, "%s", "Java request object is NULL");
    }

    if (response != NULL)
    {
        CM_LOG_DEBUG_A(CM_JNI, "%s", "Response object is not NULL");

        respObj = ((Serializable *) (response))->ser_handler ;

        if (respObj == NULL)
            CM_LOG_DEBUG_A(CM_JNI, "%s", "Java response object is NULL");
    }

    const char *stringGetAdd = methodId.c_str();

    jstring jmethod = pEnv->NewStringUTF(stringGetAdd);

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }

    CM_LOG_DEBUG_A(CM_JNI, "%s", "Invoking");

    statusCode = JNIGlobal::executeIntMethod("OC/Cm/Server/ResourceHandler/IResourceHandler",
                 "handleSyncResource",
                 "(LOC/Cm/Connector/Server/ServerSession;Ljava/lang/String;LOC/Cm/Serialization/ISerializable;LOC/Cm/Serialization/ISerializable;)I",
                 res_handler,
                 jserverSesison, jmethod, reqObj, respObj);

    JNIGlobal::releaseLocalRef(jserverSesison);

    return true ; // TODO: handle Exceptions
}



//
//bool
//CMResourceHandler::onInit()
//{
//  return JNIGlobal::executeBoolMethod("Sec/Shp/Server/ResourceHandler/IResourceHandler","onInit","(J)Z", res_handler);
//}
//
//bool
//CMResourceHandler::onDeinit()
//{
//  return JNIGlobal::executeBoolMethod("Sec/Shp/Server/ResourceHandler/IResourceHandler","onDeinit", "(J)Z", res_handler);
//}
//
//bool
//CMResourceHandler::handleResource( Sec::Shp::Connector::Server::ServerSession& session,std::string methodId,Sec::Shp::Serialization::ISerializable* request,
//                              Sec::Shp::Serialization::ISerializable* response)
//{
//  /*jclass        clsSer  = NULL;
//  jmethodID   m_constructor   = NULL;
//  jobject     jserverSesison = NULL  ;
//
//  //Obtaining Classes
//  clsSer = env->FindClass("Sec/Shp/Connector/Server/ServerSession");
//
//  //Obtaining Method IDs
//  if (clsSer != NULL)
//  {
//      m_constructor = env->GetMethodID(clsSer, "<init>", "(J)V");
//      jserverSesison = env->NewObject(clsSer, m_constructor, (jlong)(&session));
//  }
//*/
//  bool isAttached = false ;
//
//  JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
//
//  jobject     jserverSesison = NULL  ;
//
//  jserverSesison = JNIGlobal::constructObject("Sec/Shp/Connector/Server/ServerSession","(J)V",(jlong)(&session)) ;
//
//  Serializable *jniReq = (Serializable*) (&request) ;
//  Serializable *jniResp = (Serializable*) (&response) ;
//
//  const char* stringGetAdd = methodId.c_str();
//
//  jstring jmethod = pEnv->NewStringUTF(stringGetAdd);
//
//  if ( isAttached )
//  {
//      JNIGlobal::detachCurrentThread() ;
//  }
//
//  return JNIGlobal::executeBoolMethod("Sec/Shp/Server/ResourceHandler/IResourceHandler","handleResource","(JJLSec/Shp/Connector/Server/ServerSession;Ljava/lang/String;LSec/Shp/Serialization/ISerializable;LSec/Shp/Serialization/ISerializable;)Z",res_handler,
//                              jserverSesison, jmethod, jniReq->ser_handler, jniResp->ser_handler);
//}
