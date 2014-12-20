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
#include "Configuration.h"
#include "DeviceFinder/DeviceFactory.h"
#include "Server/ResourceHandler/ResourceHandlerFactory.h"
#include "OC_Cm_Configuration.h"
#include "CMDeviceFactory.h"
#include "CMResourceHandlerFactory.h"
#include "string"
#include "CMSerializableDataFactory.h"
#include "stdlib.h"
#include "CMConfiguration.h"
#include <list>




JNIEXPORT jlong JNICALL Java_OC_Cm_Configuration_constructNative
(JNIEnv *env, jobject thisObj)
{
    OC::Cm::Configuration *cObject = new CMConfiguration(thisObj);

    if (NULL == cObject)
    {
        JNIGlobal::throwException("Failed to allocate CMConfiguration : OutOfMemory");
    }

    return (jlong) cObject ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Configuration_deleteNative
(JNIEnv *env, jobject thisObj, jlong config)
{
    OC::Cm::Configuration *cObject =  ( CMConfiguration *) config;

    if (NULL != cObject)
    {
        delete cObject;
    }
}

JNIEXPORT void JNICALL Java_OC_Cm_Configuration_reset
(JNIEnv *env, jobject thisObj, jlong config)
{
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;

    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return ;
    }

    OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *pResourceHandlerFactory =
        con->getResourceHandlerFactory() ;
    OC::Cm::DeviceFinder::DeviceFactory *pDeviceFactory = con->getDeviceFactory();
    OC::Cm::Serialization::SerializableDataFactory *pSerializableFactory =
        con->getSerializableDataFactory();

    con->reset();

    if ( NULL != pResourceHandlerFactory )
    {
        delete pResourceHandlerFactory;
    }

    if ( NULL != pDeviceFactory )
    {
        delete pDeviceFactory;
    }

    if ( NULL != pSerializableFactory )
    {
        delete pSerializableFactory;
    }
}

JNIEXPORT void JNICALL Java_OC_Cm_Configuration_setDeviceFactory
(JNIEnv *env, jobject thisObj, jlong config, jlong deviceFactory)
{
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return ;
    }

    CMDeviceFactory *devFactory = ( CMDeviceFactory *)(deviceFactory);
    if ( NULL == devFactory )
    {
        JNIGlobal::throwException("Native DeviceFactory object is NULL") ;
        return ;
    }

    con->setDeviceFactory(devFactory);
}

JNIEXPORT void JNICALL Java_OC_Cm_Configuration_setResourceHandlerFactory
(JNIEnv *env, jobject thisObj, jlong config, jlong resourceHandler)
{
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return ;
    }

    MyCMResourceHandlerFactory *resHandlerFactory = (MyCMResourceHandlerFactory *)resourceHandler ;
    if ( NULL == resHandlerFactory )
    {
        JNIGlobal::throwException("Native resHandlerFactory object is NULL") ;
        return ;
    }

    con->setResourceHandlerFactory(resHandlerFactory);
}

JNIEXPORT void JNICALL Java_OC_Cm_Configuration_setSerializableDataFactory
(JNIEnv *env, jobject thisObj, jlong config, jlong serFactory)
{
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return ;
    }

    CMSerializableDataFactory *serDataFactory = ( CMSerializableDataFactory *) (serFactory) ;
    if ( NULL == serDataFactory )
    {
        JNIGlobal::throwException("Native serialization DataFactory object is NULL") ;
        return ;
    }

    con->setSerializableDataFactory(serDataFactory);
}

JNIEXPORT void JNICALL Java_OC_Cm_Configuration_addDataFormat
(JNIEnv *env, jobject thisObj, jlong config, jstring contentType, jobject serializer,
 jobject deserializer)
{
//  OC::Cm::Configuration* con = (OC::Cm::Configuration*) config;
//
//  const char* conType = env->GetStringUTFChars(contentType,0);
//
//  OC::Cm::Serialization::IDeserializer& ideserializer = new OC::Cm::Serialization::IDeserializer&;
//  ideserializer = deserializer;
//
//  if ( NULL == con)
//  {
//      jclass mapClass = env->FindClass("java/lang/Exception");
//      (env)->ThrowNew(mapClass, "Configuration object is NULL") ;
//  }
//  con->addDataFormat((char*)conType,serDataFac,ideserializer);

}

JNIEXPORT void JNICALL Java_OC_Cm_Configuration_setServerConnector
(JNIEnv *env, jobject thisObj, jlong config, jlong serverConnector)
{
    /* Not Supported
    OC::Cm::Configuration* con = (OC::Cm::Configuration*) config;
    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return ;
    }

    OC::Cm::Connector::Server::IServerConnector* serConnector = (OC::Cm::Connector::Server::IServerConnector*) serverConnector;
    if ( NULL == serConnector )
    {
        JNIGlobal::throwException("Native server Connector object is NULL") ;
        return ;
    }

    con->setServerConnector(*serConnector);
    */
}

JNIEXPORT void JNICALL Java_OC_Cm_Configuration_setClientConnector
(JNIEnv *env, jobject thisObj, jlong config, jlong clientConnector)
{
    /* Not Supported
    OC::Cm::Configuration* con = (OC::Cm::Configuration*) config;

    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return;
    }

    OC::Cm::Connector::Client::IClientConnector* cConnector = (OC::Cm::Connector::Client::IClientConnector*) clientConnector;
    if ( NULL == cConnector )
    {
        JNIGlobal::throwException("Native client Connector object is NULL") ;
        return ;
    }

    con->setClientConnector(*cConnector);
    */
}

JNIEXPORT jobject JNICALL Java_OC_Cm_Configuration_getClientConnectors
(JNIEnv *env, jobject thisObj, jlong config)
{
    /*OC::Cm::Configuration* con = (OC::Cm::Configuration*) config;

    if ( NULL == con)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Configuration object is NULL") ;
    }

    return (jobject)con->getClientConnectors();*/
    return NULL ;
}

JNIEXPORT jobject JNICALL Java_OC_Cm_Configuration_getDeSerializers
(JNIEnv *env, jobject thisObj, jlong config)
{
    /*OC::Cm::Configuration* con = (OC::Cm::Configuration*) config;

    if ( NULL == con)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Configuration object is NULL") ;
    }

    return (jobject)con->getDeSerializers();*/
    return NULL ;
}

JNIEXPORT jobject JNICALL Java_OC_Cm_Configuration_getDeviceFactory
(JNIEnv *env, jobject thisObj, jlong config)
{
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return NULL;
    }

    return (jobject)con->getDeviceFactory();
}

JNIEXPORT jobject JNICALL Java_OC_Cm_Configuration_getResourceHandlerFactory
(JNIEnv *env, jobject thisObj, jlong config)
{
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return NULL;
    }

    return (jobject)con->getResourceHandlerFactory();
}

JNIEXPORT jobject JNICALL Java_OC_Cm_Configuration_getSerializableDataFactory
(JNIEnv *env, jobject thisObj, jlong config)
{
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return NULL;
    }

    return (jobject)con->getSerializableDataFactory();
}

JNIEXPORT jobject JNICALL Java_OC_Cm_Configuration_getSerializers
(JNIEnv *env, jobject thisObj, jlong config)
{
    /*OC::Cm::Configuration* con = (OC::Cm::Configuration*) config;

    if ( NULL == con)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Configuration object is NULL") ;
    }

    return (jobject)con->getSerializers();*/
    return NULL ;
}


JNIEXPORT jobject JNICALL Java_OC_Cm_Configuration_getServerConnectors
(JNIEnv *env, jobject thisObj, jlong config)
{
    /*OC::Cm::Configuration* con = (OC::Cm::Configuration*) config;

    if ( NULL == con)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Configuration object is NULL") ;
    }

    return (jobject)con->getServerConnectors();*/
    return NULL ;
}

JNIEXPORT jobject JNICALL Java_OC_Cm_Configuration_getAppType
(JNIEnv *env, jobject thisObj, jlong config)
{
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return NULL;
    }
    jobject apptype =  JNIGlobal::getEnum("OC/Cm/ApplicationType", "getAppType",
                                          "(I)LOC/Cm/ApplicationType;", (int)con->getAppType()) ;
    return apptype ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Configuration_setAppType
(JNIEnv *env, jobject thisObj, jlong config, jint applicationType)
{
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return;
    }
    OC::Cm::ApplicationType appType = (OC::Cm::ApplicationType)applicationType ;
    //con->setAppType(appType); // this is being used internally now.
}


JNIEXPORT jstring JNICALL Java_OC_Cm_Configuration_getSubscriptionDbPath
(JNIEnv *env, jobject thisObj, jlong config)
{
    /*OC::Cm::Configuration* con = (OC::Cm::Configuration*) config;

    if ( NULL == con)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Configuration object is NULL") ;
    }

    return (jstring)(con->getSubscriptionDbPath());*/
    return NULL ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Configuration_setSubscriptionDbPath
(JNIEnv *env, jobject thisObj, jlong config, jstring subsPath)
{
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return;
    }

    std::string subPath ;
    if (false == JNIGlobal::convertToStdString(subsPath, subPath))
    {
        JNIGlobal::throwException("Failed to convert jstring to std::string (subscripton db path)") ;
        return;
    }

    con->setSubscriptionDbPath(subPath.c_str());
}


JNIEXPORT void JNICALL Java_OC_Cm_Configuration_setRAConfigPath__JLjava_lang_String_2
(JNIEnv *env, jobject thisObj, jlong config, jstring conPath)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    JNI_TryReturnVoid( NULL != con, "Native Configuration is invalid");

    std::string raPath ;
    bool bret = JNIGlobal::convertToStdString(conPath, raPath);
    JNI_TryReturnVoid( bret != false,
                       "Failed to convert jstring to std::string (Remote Access Configuration Path)");

    //con->setRAConfigPath(raPath);
#endif
}

JNIEXPORT void JNICALL
Java_OC_Cm_Configuration_setRAConfigPath__JLjava_lang_String_2Ljava_lang_String_2
(JNIEnv *env, jobject thisObj, jlong config, jstring conPath, jstring jKey)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    JNI_TryReturnVoid( NULL != con, "Native Configuration is invalid");

    std::string raPath ;
    bool bret = JNIGlobal::convertToStdString(conPath, raPath);
    JNI_TryReturnVoid( bret != false,
                       "Failed to convert jstring to std::string (Remote Access Configuration Path)");

    std::string privateKey ;
    bret = JNIGlobal::convertToStdString(jKey, privateKey);
    JNI_TryReturnVoid( bret != false, "Failed to convert jstring to std::string (Private Key)");

    //con->setRAConfigPath(raPath, privateKey);
#endif
}


JNIEXPORT jstring JNICALL Java_OC_Cm_Configuration_getRAConfigPath
(JNIEnv *env, jobject thisObj, jlong config )
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    JNI_TryReturn( NULL != con, NULL, "Native Configuration is invalid");

    //return JNIGlobal::convertToJString(con->getRAConfigPath()) ;
#endif
    return NULL;
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Configuration_getRAPrivatetKey
(JNIEnv *env, jobject thisObj, jlong config )
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    JNI_TryReturn( NULL != con, NULL, "Native Configuration is invalid");

//  return JNIGlobal::convertToJString(con->getRAPrivateKey()) ;
#endif
    return NULL;
}

JNIEXPORT void JNICALL Java_OC_Cm_Configuration_setRemoteAccessEnable
(JNIEnv *env, jobject thisObj, jlong config, jboolean enable)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    JNI_TryReturnVoid( NULL != con, "Native Configuration is invalid");

//  con->setRemoteAccessEnable(enable == JNI_TRUE);
#endif
}

JNIEXPORT jlong JNICALL Java_OC_Cm_Configuration_getRemoteAccessConfig
(JNIEnv *env, jobject thisObj, jlong config)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config;
    if ( NULL == con)
    {
        JNIGlobal::throwException("Native Configuration object is NULL") ;
        return 0;
    }

//  OC::Cm::RemoteAccessConfig *rConfig = &(con->getRemoteAccessConfig());

//  return (jlong)rConfig;
#endif
    return 0;
}

JNIEXPORT void JNICALL Java_OC_Cm_Configuration_setWifiDetails
(JNIEnv *env, jobject thisObj, jlong config, jlong serializable)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::Configuration *con = (OC::Cm::Configuration *) config ;
    JNI_TryReturnVoid( NULL != con, "Native Configuration is invalid");

    OC::Cm::Serialization::ISerializable  *iserializable = (OC::Cm::Serialization::ISerializable *)
            serializable;

//  con->setWifiDetails(iserializable);
#endif
}
