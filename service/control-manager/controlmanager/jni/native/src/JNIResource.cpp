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
 * JNIResource.cpp
 *
 *  Created on: Jul 3, 2013
 *      Author: kirtigupta
 */

#include <jni.h>
#include "Client/Resource/Resource.h"
#include "OC_Cm_Client_Resource_Resource.h"
#include "MyResourceResponseListener.h"
#include "IContext.h"

bool JNIConvertToMap( JNIEnv *env, jobjectArray javaMapEntries,
                      std::map< std::string, std::string > &cppMapEntries )
{
    if ( NULL == javaMapEntries )
    {
        return false ;
    }

    int count = env->GetArrayLength( javaMapEntries );
    for (int i = 0; i < count; i++)
    {
        jobject entry = (jobject)env->GetObjectArrayElement(javaMapEntries, i);
        if (NULL != entry)
        {
            std::string key;
            std::string value;

            jstring jKey = (jstring)(JNIGlobal::executeObjectMethod("java/util/Map/Entry", "getKey",
                                     "()Ljava/lang/Object;", entry)) ;
            jstring jValue = (jstring)(JNIGlobal::executeObjectMethod("java/util/Map/Entry", "getValue",
                                       "()Ljava/lang/Object;", entry)) ;

            if ( false == JNIGlobal::convertToStdString(jKey, key))
            {
                continue ;
            }

            if ( false == JNIGlobal::convertToStdString(jValue, value))
            {
                continue ;
            }

            if (key.empty() == false)
            {
                cppMapEntries[key] = value;
            }
        }
    }

    return true;
}

JNIEXPORT jlong JNICALL Java_OC_Cm_Client_Resource_Resource_constructNativeClass
(JNIEnv *env, jobject thisObj, jlong context, jstring protocol, jstring deviceAddress,
 jstring resourcePath)
{
    const char *devAdd = env->GetStringUTFChars(deviceAddress, 0);
    const char *resPath = env->GetStringUTFChars(resourcePath, 0);
    const char *cproto = env->GetStringUTFChars(protocol, 0);

    OC::Cm::IContext *pContext =  (OC::Cm::IContext *)context ;

    std::string dAdd(devAdd);

    std::string rPath(resPath);

    std::string proto(cproto);

    OC::Cm::Client::Resource::Resource *resourceObj = new OC::Cm::Client::Resource::Resource(pContext,
            proto, dAdd, rPath ) ;

    resourceObj->forceApplicationSerializables = true ;

    env->ReleaseStringUTFChars(deviceAddress, devAdd);

    env->ReleaseStringUTFChars(resourcePath, resPath);

    env->ReleaseStringUTFChars(resourcePath, cproto);

    jlong nativeResource = (jlong)(resourceObj);

    return nativeResource;
}

JNIEXPORT void JNICALL Java_OC_Cm_Client_Resource_Resource_deleteNativeResource
(JNIEnv *env, jobject thisObj, jlong resource)
{
    OC::Cm::Client::Resource::Resource *resourceObj = (OC::Cm::Client::Resource::Resource *) resource ;
    delete resourceObj;

}
JNIEXPORT jint JNICALL Java_OC_Cm_Client_Resource_Resource_get
(JNIEnv *env, jobject thisObj, jlong resource, jobjectArray map, jobject listener)
{
    int requestId;
    OC::Cm::CMError error ;

    OC::Cm::Client::Resource::Resource *resourceObj = (OC::Cm::Client::Resource::Resource *) resource ;
    if ( NULL == resourceObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource object is NULL") ;
        return -1 ;
    }

    MyResourceResponseListener *responseListener = new MyResourceResponseListener( listener );
    if ( NULL == responseListener)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource Listener object is NULL") ;
        return -1 ;
    }

    std::map< std::string, std::string > queryMap ;
    JNIConvertToMap( env, map, queryMap );
    if (queryMap.size() > 0)
    {
        error = resourceObj->get(requestId, &queryMap, *responseListener);
    }
    else
    {
        error = resourceObj->get(requestId, NULL, *responseListener);
    }
    if (error.getErrorCode() != OC::Cm::CM_SUCCESS )
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, error.getErrorMessage().c_str()) ;
        return -1 ;
    }
    return requestId;
}

JNIEXPORT jint JNICALL Java_OC_Cm_Client_Resource_Resource_put
(JNIEnv *env, jobject thisObj, jlong resource, jobjectArray map, jlong serializable,
 jobject listener)

{
    int requestId;
    OC::Cm::CMError error ;

    OC::Cm::Client::Resource::Resource *resourceObj = (OC::Cm::Client::Resource::Resource *) resource ;

    if ( NULL == resourceObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource object is NULL") ;
        return -1 ;
    }

    MyResourceResponseListener *responseListener = new MyResourceResponseListener( listener );
    if ( NULL == responseListener)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource Listener object is NULL") ;
        return -1 ;
    }

    std::map< std::string, std::string > queryMap ;
    JNIConvertToMap( env, map, queryMap );
    if (queryMap.size() > 0)
    {
        error = resourceObj->put(requestId, &queryMap, (OC::Cm::Serialization::ISerializable *)serializable,
                                 *responseListener) ;
    }
    else
    {
        error = resourceObj->put(requestId, NULL, (OC::Cm::Serialization::ISerializable *)serializable,
                                 *responseListener) ;
    }
    if (error.getErrorCode() != OC::Cm::CM_SUCCESS )
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, error.getErrorMessage().c_str()) ;
        return -1 ;
    }

    return requestId ;
}

JNIEXPORT jint JNICALL Java_OC_Cm_Client_Resource_Resource_post
(JNIEnv *env, jobject thisObj, jlong resource, jobjectArray map, jlong serializable,
 jobject listener)
{
    int requestId;
    OC::Cm::CMError error ;

    OC::Cm::Client::Resource::Resource *resourceObj = (OC::Cm::Client::Resource::Resource *) resource ;
    if ( NULL == resourceObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource object is NULL") ;
        return -1 ;
    }

    MyResourceResponseListener *responseListener = new MyResourceResponseListener( listener );
    if ( NULL == responseListener)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource Listener object is NULL") ;
        return -1 ;
    }

    std::map< std::string, std::string > queryMap ;
    JNIConvertToMap( env, map, queryMap );
    if (queryMap.size() > 0)
    {
        error = resourceObj->post(requestId, &queryMap,
                                  (OC::Cm::Serialization::ISerializable *)serializable, *responseListener);
    }
    else
    {
        error = resourceObj->post(requestId, NULL, (OC::Cm::Serialization::ISerializable *)serializable,
                                  *responseListener);
    }
    if (error.getErrorCode() != OC::Cm::CM_SUCCESS )
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, error.getErrorMessage().c_str()) ;
        return -1 ;
    }
    return requestId ;
}

JNIEXPORT jint JNICALL Java_OC_Cm_Client_Resource_Resource_delete
(JNIEnv *env,  jobject thisObj, jlong resource, jobjectArray map, jobject listener)
{
    int requestId;
    OC::Cm::CMError error ;

    OC::Cm::Client::Resource::Resource *resourceObj = (OC::Cm::Client::Resource::Resource *) resource ;

    if ( NULL == resourceObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource object is NULL") ;
        return -1 ;
    }

    MyResourceResponseListener *responseListener = new MyResourceResponseListener( listener );
    if ( NULL == responseListener)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource Listener object is NULL") ;
        return -1 ;
    }

    std::map< std::string, std::string > queryMap ;
    JNIConvertToMap( env, map, queryMap );
    if (queryMap.size() > 0)
    {
        error = resourceObj->deleteResource(requestId, &queryMap, *responseListener) ;
    }
    else
    {
        error = resourceObj->deleteResource(requestId , NULL, *responseListener) ;
    }
    if (error.getErrorCode() != OC::Cm::CM_SUCCESS )
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, error.getErrorMessage().c_str()) ;
        return -1 ;
    }
    return requestId ;
}

JNIEXPORT jint JNICALL Java_OC_Cm_Client_Resource_Resource_subscribe
(JNIEnv *env, jobject thisObj, jlong resource, jstring jDUuid, jstring jSUuid, jobject listener,
 jstring jdescription )
{
    int requestId;

    OC::Cm::Client::Resource::Resource *resourceObj = (OC::Cm::Client::Resource::Resource *) resource ;

    if ( NULL == resourceObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource object is NULL") ;
        return -1 ;
    }

    MyResourceResponseListener *responseListener = new MyResourceResponseListener( listener );

    if ( NULL == responseListener)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource Listener object is NULL") ;
        return -1 ;
    }

    std::string dUuid ;
    if ( false == JNIGlobal::convertToStdString(jDUuid, dUuid) )
    {
        if (responseListener != NULL)
        {
            delete responseListener; responseListener = NULL;
        }
        return -1;
    }

    std::string sUuid ;
    if ( false == JNIGlobal::convertToStdString(jSUuid, sUuid) )
    {
        if (responseListener != NULL)
        {
            delete responseListener; responseListener = NULL;
        }
        return -1;
    }

    std::string sDescription ;
    if ( false == JNIGlobal::convertToStdString(jdescription, sDescription) )
    {
        if (responseListener != NULL)
        {
            delete responseListener; responseListener = NULL;
        }
        return -1;
    }

    resourceObj->subscribe(requestId, dUuid, sUuid, *responseListener, sDescription) ;

    return requestId ;

}

JNIEXPORT jint JNICALL Java_OC_Cm_Client_Resource_Resource_unSubscribe
(JNIEnv *env, jobject thisObj, jlong resource, jstring juuid, jobject listener)
{
    int requestId;

    OC::Cm::Client::Resource::Resource *resourceObj = (OC::Cm::Client::Resource::Resource *) resource ;

    if ( NULL == resourceObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource object is NULL") ;
        return -1 ;
    }
    MyResourceResponseListener *responseListener = new MyResourceResponseListener( listener );

    if ( NULL == responseListener)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource Listener object is NULL") ;
        return -1 ;
    }

    std::string uuid ;
    if (false == JNIGlobal::convertToStdString(juuid, uuid))
    {
        if (responseListener != NULL)
        {
            delete responseListener; responseListener = NULL;
        }
        return -1;
    }

    resourceObj->unSubscribe(requestId, uuid, *responseListener) ;

    return requestId ;

}

JNIEXPORT void JNICALL Java_OC_Cm_Client_Resource_Resource_setFilter
(JNIEnv *env, jobject thisObj, jlong resource, jstring filter)
{
    OC::Cm::Client::Resource::Resource *resourceObj = (OC::Cm::Client::Resource::Resource *) resource ;

    if ( NULL != resourceObj)
    {
        std::string cfilter ;
        JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(filter, cfilter)),
                           "string conversion failed") ;
        resourceObj->setFilter(cfilter);
    }
}

JNIEXPORT void JNICALL Java_OC_Cm_Client_Resource_Resource_setRecursive
(JNIEnv *env, jobject thisObj, jlong resource, jboolean recursive)
{
    OC::Cm::Client::Resource::Resource *resourceObj = (OC::Cm::Client::Resource::Resource *) resource ;

    if ( NULL != resourceObj)
    {
        if (recursive == JNI_TRUE)
        {
            resourceObj->setRecursive(true);
        }
        else
        {
            resourceObj->setRecursive(false);
        }
    }
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Client_Resource_Resource_setResourcePath
(JNIEnv *env, jobject thisObj, jlong resource, jstring filter)
{
    OC::Cm::Client::Resource::Resource *resourceObj = (OC::Cm::Client::Resource::Resource *) resource ;

    if ( NULL != resourceObj)
    {
        std::string cfilter ;

        if (false == JNIGlobal::convertToStdString(filter, cfilter))
        {
            return JNI_FALSE ;
        }

        resourceObj->setResourcePath(cfilter) ;
        return JNI_TRUE;
    }

    return JNI_FALSE ;
}
