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
 * JNIResourceHandler.cpp
 *
 *  Created on: Jul 4, 2013
 *      Author: kirtigupta
 */
#include <jni.h>
#include "Server/ResourceHandler/IResourceHandler.h"
#include "OC_Cm_Server_ResourceHandler_IResourceHandler.h"
#include "JNIGlobal.h"

/*
 * Class:     OC_Cm_Server_ResourceHandler_IResourceHandler
 * Method:    getNativeGlobleReference
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_OC_Cm_Server_ResourceHandler_IResourceHandler_getNativeGlobleReference
(JNIEnv *pEnv, jobject thisObject)
{
    return (jlong)(JNIGlobal::getGlobleRef(thisObject)) ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Server_ResourceHandler_IResourceHandler_onInit( JNIEnv *env,
        jobject thisObj, jlong resHandler )
{
    OC::Cm::Server::ResourceHandler::IResourceHandler *resourceHandler =
        ( OC::Cm::Server::ResourceHandler::IResourceHandler *) resHandler ;
    return resourceHandler->onInit();
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Server_ResourceHandler_IResourceHandler_onDeinit( JNIEnv *env,
        jobject thisObj, jlong resHandler )
{
    OC::Cm::Server::ResourceHandler::IResourceHandler *resourceHandler =
        ( OC::Cm::Server::ResourceHandler::IResourceHandler *) resHandler ;
    return resourceHandler->onDeinit();
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Server_ResourceHandler_IResourceHandler_handleResource(
    JNIEnv *env, jobject thisObj, jlong resHandler, jlong serHandler, jobject serverSession,
    jstring mthdId, jobject req, jobject resp )
{
    //CHECK?????

    /*Sec::Shp::Server::ResourceHandler::IResourceHandler* resourceHandler = ( Sec::Shp::Server::ResourceHandler::IResourceHandler* ) resHandler ;

    Sec::Shp::Connector::Server::ServerSession* serverS = new Sec::Shp::Connector::Server::ServerSession*;

    Sec::Shp::Serialization::ISerializable* request  = ( Sec::Shp::Serialization::ISerializable* )serHandler ;

    Sec::Shp::Serialization::ISerializable* response = ( Sec::Shp::Serialization::ISerializable* )serHandler ;

    const char * methodId = env->GetStringUTFChars(mthdId,0);

    return resourceHandler->handleResource( (char*)methodId);*/

    return 0 ;

}

