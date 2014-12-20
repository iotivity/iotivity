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




#include "OC_Cm_Notification_NotificationResponse.h"
#include "JNINotificationResponse.h"
#include "JNIGlobal.h"
#include "Serializable.h"

JNINotificationResponse::JNINotificationResponse( jobject jnotificationresponse,
        std::string resourcePath , std::string notificationType , std::string uuid,
        OC::Cm::Serialization::ISerializable *notificationData )
    : OC::Cm::Notification::NotificationResponse(resourcePath, notificationType, uuid, notificationData)
{
    m_notificationResponse = JNIGlobal::getGlobleRef(jnotificationresponse) ;
}

JNINotificationResponse::~JNINotificationResponse()
{
    JNIGlobal::executeVoidMethod("OC/Cm/Notification/NotificationResponse", "onNativeDelete", "()V",
                                 m_notificationResponse);
    JNIGlobal::releaseGlobleRef(m_notificationResponse) ;
}


JNIEXPORT jlong JNICALL Java_OC_Cm_Notification_NotificationResponse_constructNative
(JNIEnv *env, jobject thisObj, jstring jresourcePath, jstring jnotificationType, jstring juuid,
 jlong jnotificationData)
{
    std::string resourcePath ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(jresourcePath, resourcePath)), 0,
                   "Resource Path conversion failed") ;

    std::string notificationType ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(jnotificationType, notificationType)), 0,
                   "Notification Type conversion failed") ;

    std::string uuid ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(juuid, uuid)), 0, "UUID conversion failed") ;

    OC::Cm::Notification::NotificationResponse *notificationresponseObj = new JNINotificationResponse(
        thisObj, resourcePath, notificationType, uuid,
        (OC::Cm::Serialization::ISerializable *)jnotificationData ) ;
    JNI_TryReturn( NULL != notificationresponseObj, 0, "Memory Allocation failed : Out of memory")

    return (jlong)(notificationresponseObj);
}

JNIEXPORT void JNICALL Java_OC_Cm_Notification_NotificationResponse_deleteNative
(JNIEnv *env, jobject thisObj, jlong notificationResponse)
{
    OC::Cm::Notification::NotificationResponse *notification =
        (OC::Cm::Notification::NotificationResponse *) notificationResponse;

    if (NULL != notification)
    {
        delete notification;
    }
}

JNIEXPORT void JNICALL Java_OC_Cm_Notification_NotificationResponse_setResourcePath
(JNIEnv *env, jobject thisObj, jlong notificationResponse, jstring jresPath)
{
    OC::Cm::Notification::NotificationResponse *notification =
        (OC::Cm::Notification::NotificationResponse *) notificationResponse;
    JNI_TryReturnVoid( NULL != notification, "Native device is invalid")

    std::string resPath ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(jresPath, resPath)),
                       "device uuid conversion failed") ;

    notification->setResourcePath(resPath);
}

JNIEXPORT void JNICALL Java_OC_Cm_Notification_NotificationResponse_setNotificationType
(JNIEnv *env, jobject thisObj, jlong notificationResponse, jstring jnotifType)
{
    OC::Cm::Notification::NotificationResponse *notification =
        (OC::Cm::Notification::NotificationResponse *) notificationResponse;
    JNI_TryReturnVoid( NULL != notification, "Native device is invalid")

    std::string notifType ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(jnotifType, notifType)),
                       "device uuid conversion failed") ;

    notification->setNotificationType(notifType);
}

JNIEXPORT void JNICALL Java_OC_Cm_Notification_NotificationResponse_setNotificationData
(JNIEnv *env, jobject thisObj, jlong notificationResponse, jlong jnotifData)
{
    OC::Cm::Notification::NotificationResponse *notification =
        (OC::Cm::Notification::NotificationResponse *) notificationResponse;
    JNI_TryReturnVoid( NULL != notification, "Native device is invalid")

    notification->setNotificationData((OC::Cm::Serialization::ISerializable *)jnotifData);
}

JNIEXPORT void JNICALL Java_OC_Cm_Notification_NotificationResponse_setUUID
(JNIEnv *env, jobject thisObj, jlong notificationResponse, jstring jUUid)
{
    OC::Cm::Notification::NotificationResponse *notification =
        (OC::Cm::Notification::NotificationResponse *) notificationResponse;
    JNI_TryReturnVoid( NULL != notification, "Native device is invalid")

    std::string uuid ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(jUUid, uuid)),
                       "device uuid conversion failed") ;

    notification->setUuid(uuid);
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Notification_NotificationResponse_getResourcePath
(JNIEnv *env, jobject thisObj, jlong notificationResponse)
{
    OC::Cm::Notification::NotificationResponse *notification =
        (OC::Cm::Notification::NotificationResponse *) notificationResponse;
    JNI_TryReturn( NULL != notification, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(notification->getResourcePath()) ;
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Notification_NotificationResponse_getUUID
(JNIEnv *env, jobject thisObj, jlong notificationResponse)
{
    OC::Cm::Notification::NotificationResponse *notification =
        (OC::Cm::Notification::NotificationResponse *) notificationResponse;
    JNI_TryReturn( NULL != notification, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(notification->getUuid()) ;
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Notification_NotificationResponse_getNotificationType
(JNIEnv *env, jobject thisObj, jlong notificationResponse)
{
    OC::Cm::Notification::NotificationResponse *notification =
        (OC::Cm::Notification::NotificationResponse *) notificationResponse;
    JNI_TryReturn( NULL != notification, NULL, "Native device is invalid")

    return JNIGlobal::convertToJString(notification->getNotificationType()) ;
}

JNIEXPORT jobject JNICALL Java_OC_Cm_Notification_NotificationResponse_getNotificationData
(JNIEnv *env, jobject thisObj, jlong notificationResponse)
{
    OC::Cm::Notification::NotificationResponse *notification =
        (OC::Cm::Notification::NotificationResponse *) notificationResponse;
    JNI_TryReturn( NULL != notification, NULL, "Native device is invalid")

    Serializable *data = (Serializable *)( notification->getNotificationData() ) ;
    if ( data == NULL )
    {
        return NULL;
    }
    else
    {
        return data->ser_handler;
    }
}

