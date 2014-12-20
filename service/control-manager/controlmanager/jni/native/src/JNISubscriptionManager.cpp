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
#include "string"
#include "CMClientResource.h"
#include "Notification/ISubscriptionManager.h"
#include "OC_Cm_Notification_ISubscriptionManager.h"
#include "CMNotification.h"
#include "MyResourceResponseListener.h"
#include "MyNotificationListener.h"
#include "CMJNISubscriptionProviderImpl.h"

JNIEXPORT jlong JNICALL Java_OC_Cm_Notification_ISubscriptionManager_addSubscriptionProvider(
    JNIEnv *env, jobject thisObj, jlong subscriptionObj, jobject notificationListener)
{
    CMJNISubscriptionProviderImpl *notificationLstnr = new CMJNISubscriptionProviderImpl(
        notificationListener) ;
    if ( NULL == notificationLstnr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Notification Listener object is NULL") ;
        return -1;
    }

    OC::Cm::Notification::ISubscriptionManager *subsMgr = ( OC::Cm::Notification::ISubscriptionManager
            *) subscriptionObj ;

    if ( NULL == subsMgr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "SubscriptionManager object is NULL") ;
        delete notificationLstnr;
        return -1;
    }

    subsMgr->addSubscriptionProvider(*notificationLstnr) ;
    return (jlong)notificationLstnr ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Notification_ISubscriptionManager_removeSubscriptionProvider(
    JNIEnv *env, jobject thisObj, jlong subscriptionObj, jlong notificationListener)
{

    CMJNISubscriptionProviderImpl *notificationLstnr =  (CMJNISubscriptionProviderImpl *)(
                notificationListener) ;
    if ( NULL == notificationLstnr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Notification Listener object is NULL") ;
        return;
    }

    OC::Cm::Notification::ISubscriptionManager *subsMgr = ( OC::Cm::Notification::ISubscriptionManager
            *) subscriptionObj ;
    if ( NULL == subsMgr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "SubscriptionManager object is NULL") ;
        return;
    }

    subsMgr->removeSubscriptionProvider(*notificationLstnr) ;
    delete notificationLstnr;
}


JNIEXPORT jlong JNICALL Java_OC_Cm_Notification_ISubscriptionManager_addNotificationListener(
    JNIEnv *env, jobject thisObj, jlong subscriptionObj, jobject notificationListener)
{
    MyNotificationListener *notificationLstnr = new MyNotificationListener(notificationListener) ;
    if ( NULL == notificationLstnr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Notification Listener object is NULL") ;
        return -1;
    }

    OC::Cm::Notification::ISubscriptionManager *subsMgr = ( OC::Cm::Notification::ISubscriptionManager
            *) subscriptionObj ;

    if ( NULL == subsMgr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "SubscriptionManager object is NULL") ;
        delete notificationLstnr;
        return -1;
    }

    subsMgr->addNotificationListener(*notificationLstnr) ;
    return (jlong)notificationLstnr ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Notification_ISubscriptionManager_removeNotificationListener(
    JNIEnv *env, jobject thisObj, jlong subscriptionObj, jlong notificationListener)
{

    MyNotificationListener *notificationLstnr =  (MyNotificationListener *)(notificationListener) ;
    if ( NULL == notificationLstnr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Notification Listener object is NULL") ;
        return ;
    }

    OC::Cm::Notification::ISubscriptionManager *subsMgr = ( OC::Cm::Notification::ISubscriptionManager
            *) subscriptionObj ;
    if ( NULL == subsMgr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "SubscriptionManager object is NULL") ;
        return;
    }

    subsMgr->removeNotificationListener(*notificationLstnr) ;
    delete notificationLstnr;
}

JNIEXPORT jint JNICALL
Java_OC_Cm_Notification_ISubscriptionManager_addSubscription( JNIEnv *env, jobject thisObj,
        jlong subscriptionObj,
        jstring jprotocol, jstring jduuid, jstring deviceAddress, jstring jsuuid,
        jobjectArray resourcePaths, jobject listener, jstring jdescription )
{
    jint reqID;
    int requestID;

    if (NULL == resourcePaths)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource Path array is NULL") ;
        return -1;
    }

    OC::Cm::Notification::ISubscriptionManager *subsMgr = ( OC::Cm::Notification::ISubscriptionManager
            *) subscriptionObj ;
    if ( NULL == subsMgr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "SubscriptionManager object is NULL") ;
        return -1;
    }

    MyResourceResponseListener *resRespListener = new MyResourceResponseListener(listener);
    if ( NULL == resRespListener)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ResourceResponse Listener object is NULL") ;
        return -1;
    }

    const char *devAddress;
    devAddress = env->GetStringUTFChars(deviceAddress, 0) ;
    std::string devAdd(devAddress);
    env->ReleaseStringUTFChars(deviceAddress, devAddress);

    std::list<std::string> resourcePathList ;

    if (NULL != resourcePaths)
    {
        int count = env->GetArrayLength(resourcePaths);

        for (int i = 0; i < count; i++)
        {
            std::string resP;
            jstring path = (jstring)env->GetObjectArrayElement(resourcePaths, i);

            if ( true == JNIGlobal::convertToStdString(path, resP))
            {
                resourcePathList.push_back(resP);
            }
        }
    }

    std::string duuid ;
    if (true != JNIGlobal::convertToStdString(jduuid, duuid))
    {
        if (resRespListener) { delete resRespListener; resRespListener = NULL; }
        return -1;
    }

    std::string suuid ;
    if (true != JNIGlobal::convertToStdString(jsuuid, suuid))
    {
        if (resRespListener) { delete resRespListener; resRespListener = NULL; }
        return -1;
    }

    std::string protocol ;
    if (true != JNIGlobal::convertToStdString(jprotocol, protocol))
    {
        if (resRespListener) { delete resRespListener; resRespListener = NULL; }
        return -1;
    }

    std::string description;
    if (true != JNIGlobal::convertToStdString(jdescription, description))
    {
        if (resRespListener) { delete resRespListener; resRespListener = NULL; }
        return -1;
    }

    if (false == subsMgr->addSubscription( requestID, protocol, duuid, devAdd, suuid, resourcePathList,
                                           "", resRespListener, description))
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Native method execution failed") ;
    }

    reqID = requestID;
    return reqID;
}

JNIEXPORT jint JNICALL
Java_OC_Cm_Notification_ISubscriptionManager_removeSubscription__JLjava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2LOC_Cm_Client_Resource_IResourceResponseListener_2
(JNIEnv *env, jobject thisObj, jlong subscriptionObj, jstring juuid, jstring deviceAddress,
 jstring subscriptionUri, jstring jprotocol, jobject listener)
{
    jint reqID;
    int requestID;

    if (NULL == subscriptionUri)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "subscriptionUri is NULL") ;
        return -1;
    }

    OC::Cm::Notification::ISubscriptionManager *subsMgr = ( OC::Cm::Notification::ISubscriptionManager
            *) subscriptionObj ;
    if ( NULL == subsMgr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "SubscriptionManager object is NULL") ;
        return -1;
    }

    MyResourceResponseListener *resRespListener = new MyResourceResponseListener(listener);
    if ( NULL == resRespListener)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ResourceResponse Listener object is NULL") ;
        return -1;
    }

    std::string jDeviceAddress ;
    if (true != JNIGlobal::convertToStdString(deviceAddress, jDeviceAddress))
    {
        if (resRespListener) { delete resRespListener; resRespListener = NULL; }
        return -1;
    }

    std::string jSubUri ;
    if (true != JNIGlobal::convertToStdString(subscriptionUri, jSubUri))
    {
        if (resRespListener) { delete resRespListener; resRespListener = NULL; }
        return -1;
    }

    std::string uuid ;
    if (true != JNIGlobal::convertToStdString(juuid, uuid))
    {
        if (resRespListener) { delete resRespListener; resRespListener = NULL; }
        return -1;
    }

    std::string protocol ;
    if (true != JNIGlobal::convertToStdString(jprotocol, protocol))
    {
        if (resRespListener) { delete resRespListener; resRespListener = NULL; }
        return -1;
    }

    if (false == subsMgr->removeSubscription( uuid, jDeviceAddress, jSubUri, protocol,  resRespListener,
            requestID))
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Native method execution failed") ;
    }

    reqID = requestID;
    return reqID;
}

JNIEXPORT jobject JNICALL
Java_OC_Cm_Notification_ISubscriptionManager_removeSubscription__JLjava_lang_String_2Ljava_lang_String_2Ljava_lang_String_2_3Ljava_lang_String_2LOC_Cm_Client_Resource_IResourceResponseListener_2
(JNIEnv *env, jobject thisObj, jlong subscriptionObj, jstring juuid, jstring deviceAddress,
 jstring jprotocol, jobjectArray resourcePaths, jobject listener)
{
    OC::Cm::Notification::ISubscriptionManager *subsMgr = ( OC::Cm::Notification::ISubscriptionManager
            *) subscriptionObj ;
    if ( NULL == subsMgr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "SubscriptionManager object is NULL") ;
        return NULL;
    }

    MyResourceResponseListener *resRespListener = new MyResourceResponseListener(listener);
    if ( NULL == resRespListener)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ResourceResponse Listener object is NULL") ;
        return NULL;
    }

    std::string uuid ;
    if (true != JNIGlobal::convertToStdString(juuid, uuid))
    {
        if (resRespListener) { delete resRespListener; resRespListener = NULL; }
        return NULL;
    }

    std::string protocol ;
    if (true != JNIGlobal::convertToStdString(jprotocol, protocol))
    {
        if (resRespListener) { delete resRespListener; resRespListener = NULL; }
        return NULL;
    }

    const char *devAddress;
    devAddress = env->GetStringUTFChars(deviceAddress, 0) ;
    std::string devAdd(devAddress);
    env->ReleaseStringUTFChars(deviceAddress, devAddress);

    std::list<std::string> resourcePathList ;

    if (NULL != resourcePaths)
    {
        int count = env->GetArrayLength(resourcePaths);

        for (int i = 0; i < count; i++)
        {
            std::string resP;
            jstring path = (jstring)env->GetObjectArrayElement(resourcePaths, i);

            if ( true == JNIGlobal::convertToStdString(path, resP))
            {
                resourcePathList.push_back(resP);
            }
        }
    }

    std::map< int, std::list< std::string > > requestIDMap ;
    if ( true == subsMgr->removeSubscription(uuid, devAdd, protocol, resourcePathList, resRespListener,
            requestIDMap) )
    {
        jobject retValue = JNIGlobal::constructObject("java/util/HashMap", "(I)V", requestIDMap.size()) ;

        std::map< int, std::list< std::string > >::iterator iter ;

        for ( iter = requestIDMap.begin(); iter != requestIDMap.end(); iter++ )
        {
            if (iter->second.size() != 0)
            {
                jsize size = iter->second.size();
                int index = 0 ;
                jobjectArray resultList = NULL;

                for ( std::list< std::string >::const_iterator stritr = iter->second.begin();
                      stritr != iter->second.end(); stritr++, index++ )
                {
                    jstring jstringPath = env->NewStringUTF((*stritr).c_str());

                    if (NULL == resultList)
                    {
                        jclass javaDeviceClass = env->GetObjectClass(jstringPath) ;
                        resultList = env->NewObjectArray(size, javaDeviceClass, NULL) ;
                    }

                    env->SetObjectArrayElement(resultList, index , jstringPath) ;
                }

                if (NULL != resultList)
                {
                    jobject id = JNIGlobal::constructObject("java/lang/Integer", "(I)V", iter->first) ;
                    jobject prevObj = JNIGlobal::executeObjectMethod("java/util/HashMap", "put",
                                      "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;", retValue, id, resultList);

                    if (prevObj != NULL)
                    {
                        JNIGlobal::releaseLocalRef(prevObj);
                    }

                    JNIGlobal::releaseLocalRef(id);
                }
            }
        }

        resRespListener->setRequestCount(requestIDMap.size());
        return retValue ;
    }
    else
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Native method execution failed") ;
    }

    return NULL ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Notification_ISubscriptionManager_notify
(JNIEnv *env, jobject thisObj, jlong subscriptionObj, jlong notificationData)
{

    OC::Cm::Notification::ISubscriptionManager *subsMgr = ( OC::Cm::Notification::ISubscriptionManager
            *) subscriptionObj ;
    if ( NULL == subsMgr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "SubscriptionManager object is NULL") ;
        return JNI_FALSE ;
    }

    OC::Cm::Notification::NotificationResponse *notifResponse =
        (OC::Cm::Notification::NotificationResponse *) (notificationData);

    if (false == subsMgr->notify(notifResponse))
    {
        return JNI_FALSE ;
    }

    return JNI_TRUE ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Notification_ISubscriptionManager_notifymultiple
(JNIEnv *env, jobject thisObj, jlong subscriptionObj, jlongArray jnotiflist)
{
    if (NULL == jnotiflist)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Resource Path array is NULL") ;
    }
    OC::Cm::Notification::ISubscriptionManager *subsMgr = ( OC::Cm::Notification::ISubscriptionManager
            *) subscriptionObj ;
    if ( NULL == subsMgr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "SubscriptionManager object is NULL") ;
        return JNI_FALSE ;
    }

    jboolean var = false;

    std::list<OC::Cm::Notification::NotificationResponse *> notificationList;

    if (NULL != jnotiflist)
    {
        int count = env->GetArrayLength(jnotiflist);
        jlong *nativeList = env->GetLongArrayElements(jnotiflist, &var);
        for (int i = 0; i < count; i++)
        {

            OC::Cm::Notification::NotificationResponse *notifResponse =
                (OC::Cm::Notification::NotificationResponse *) (nativeList[i]);
            if (notifResponse != NULL)
                notificationList.push_back(notifResponse);
        }

        if (false == subsMgr->notify( notificationList ))
        {
            return JNI_FALSE ;
        }

        return JNI_TRUE ;

    }
    return JNI_FALSE ;
}


JNIEXPORT jobjectArray JNICALL Java_OC_Cm_Notification_ISubscriptionManager_getMySubscription
(JNIEnv *env, jobject thisObj, jlong subscriptionObj, jstring juuid)
{
    jobjectArray resultList = NULL ;

    std::string uuid ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(juuid, uuid)), NULL,
                   "string conversion failed") ;

    OC::Cm::Notification::ISubscriptionManager *subsMgr = ( OC::Cm::Notification::ISubscriptionManager
            *) subscriptionObj ;
    if ( NULL == subsMgr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "SubscriptionManager object is NULL") ;
        return resultList ;
    }

    std::list<std::string> resList = subsMgr->getMySubscriptions(uuid);
    if (resList.size() != 0)
    {
        jsize size = resList.size();
        int index = 0 ;

        for ( std::list<std::string>::const_iterator itr = resList.begin(); itr != resList.end();
              itr++, index++ )
        {
            std::string resource =  (*itr) ;
            jstring jResource = JNIGlobal::convertToJString(resource) ;

            if (NULL == resultList)
            {
                jclass javaDeviceClass = env->GetObjectClass(jResource) ;
                resultList = env->NewObjectArray(size, javaDeviceClass, NULL) ;
            }

            env->SetObjectArrayElement(resultList, index , jResource) ;

            JNIGlobal::releaseLocalRef(jResource);
        }
    }

    return resultList;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Notification_ISubscriptionManager_notifyAll(JNIEnv *env,
        jobject thisObj, long subscriptionObj, jstring resourcePath, jstring notificationType,
        jobject notificationData)
{

    /*const char* resPath = env->GetStringUTFChars(resourcePath,0);

    const char* notType = env->GetStringUTFChars(notificationType,0);

    OC::Cm::Notification::ISubscriptionManager* subsMgr = ( OC::Cm::Notification::ISubscriptionManager* ) subscriptionObj ;
    if ( NULL == subsMgr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "SubscriptionManager object is NULL") ;
    }

    OC::Cm::Serialization::ISerializable* serializableObj = new OC::Cm::Serialization::ISerializable();

    serializableObj = notificationData;

    return subsMgr->notifyAll((char*)resPath,(char*)notType,serializableObj);*/

    return true ;

}
