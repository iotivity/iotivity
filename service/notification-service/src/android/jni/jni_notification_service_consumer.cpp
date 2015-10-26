//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "jni_notification_service.h"

#include "jni_notification_service_jvm.h"

#include <iostream>

#include <RCSAddress.h>

#include <OCPlatform.h>

#include <NotificationConsumer.h>
#include <NotificationDiscoveryManager.h>

using namespace OC;
using namespace OIC::Service;

constexpr int CORRECT_INPUT = 99;
constexpr int INCORRECT_INPUT = 100;
std::shared_ptr<NotificationConsumer> resource;
std::vector<std::shared_ptr<NotificationConsumer>> notificationResourceList;
std::vector<std::string> DeviceList;
int resource_index=-1;
jmethodID g_notificationcallback;
jobject g_callback_obj;
JNIEnv *m_env;

void SendCallback(NotificationObject *m_notificationObjectPtr)
{

    LOGE("sending callback to android");
    m_env = NotificationManagerJVM::getEnv();
    if (m_env == NULL)
    {LOGE("jvm is null"); return;}

    jclass NMCallbacks = GetJClass(NM_CALLBACK_NATIVE_API_CLASS_PATH);
    if (NMCallbacks == NULL)
    {LOGE("jclass is null"); return;}

    g_callback_obj = GetJObjectInstance(
                         NM_CALLBACK_NATIVE_API_CLASS_PATH);
    if (g_callback_obj == NULL)
    {LOGE("jobj is null"); return;}
    g_notificationcallback = m_env->GetMethodID(NMCallbacks, "ReceivedNotificationcallback",
                             "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;III)V");
    if (g_notificationcallback == NULL)
        LOGE("couldn't register callback");
    else LOGI("registered callback");

    jint id = m_notificationObjectPtr->mNotificationId;


    if (m_notificationObjectPtr->mNotificationObjectType == NotificationObjectType::Text)
    {
        TextNotification *textNotification = (TextNotification *) m_notificationObjectPtr;
        jstring message = m_env->NewStringUTF(textNotification->mNotificationMessage.c_str());
        jstring time = m_env->NewStringUTF(textNotification->mNotificationTime.c_str());
        jstring sender = m_env->NewStringUTF(textNotification->mNotificationSender.c_str());
        jint ttl = textNotification->mNotificationTtl;
        LOGI("ID and ttl=%d %d", id, ttl);
        m_env->CallVoidMethod(g_callback_obj, g_notificationcallback, NULL, message, sender, time, 0, ttl,
                              id);
    }
    else if (m_notificationObjectPtr->mNotificationObjectType == NotificationObjectType::Image)
    {
        ImageNotification *imageNotificationPtr = (ImageNotification *) m_notificationObjectPtr;
        jstring url = m_env->NewStringUTF(imageNotificationPtr->mNotificationIconUrl.c_str());
        jstring message = m_env->NewStringUTF(imageNotificationPtr->mNotificationMessage.c_str());
        jstring time = m_env->NewStringUTF(imageNotificationPtr->mNotificationTime.c_str());
        jstring sender = m_env->NewStringUTF(imageNotificationPtr->mNotificationSender.c_str());
        int ttl = imageNotificationPtr->mNotificationTtl;
        m_env->CallVoidMethod(g_callback_obj, g_notificationcallback, url, message, sender, time, 1, ttl,
                              id);
    }
    else if (m_notificationObjectPtr->mNotificationObjectType == NotificationObjectType::Video)
    {
        VideoNotification *videoNotificationPtr = (VideoNotification *) m_notificationObjectPtr;
        jstring url = m_env->NewStringUTF(videoNotificationPtr->mNotificationVideoUrl.c_str());
        jstring time = m_env->NewStringUTF(videoNotificationPtr->mNotificationTime.c_str());
        jstring sender = m_env->NewStringUTF(videoNotificationPtr->mNotificationSender.c_str());
        int ttl = videoNotificationPtr->mNotificationTtl;
        m_env->CallVoidMethod(g_callback_obj, g_notificationcallback, url, NULL, sender, time, 2, ttl, id);
    }


    NotificationManagerJVM::releaseEnv();
}

void sendcallbackResourceDiscovered(std::string resourceName,int resource_index)
{
    LOGI("sending callback to android");
    m_env = NotificationManagerJVM::getEnv();
    if (m_env == NULL)
    {
		LOGE("jvm is null"); 
		return;
	}

    jclass NMCallbacks = GetJClass(NM_CALLBACK_NATIVE_API_CLASS_PATH);
    if (NMCallbacks == NULL)
    {LOGE("jclass is null"); return;}

    g_callback_obj = GetJObjectInstance(
                         NM_CALLBACK_NATIVE_API_CLASS_PATH);
    if (g_callback_obj == NULL)
    {LOGE("jobj is null"); return;}
    jmethodID g_discoverycallback = m_env->GetMethodID(NMCallbacks, "onResourceDiscoveredCallback",
                                    "(Ljava/lang/String;I)V");
    if (g_discoverycallback == NULL)
        LOGE("couldn't register callback");
    else LOGI("registered callback");
    jstring jresourceName = m_env->NewStringUTF(resourceName.c_str());

    m_env->CallVoidMethod(g_callback_obj, g_discoverycallback, jresourceName, resource_index);
    NotificationManagerJVM::releaseEnv();
	
}

void onGetDeviceName(std::string deviceName)
{
	resource_index++;
    DeviceList.push_back(deviceName);
    sendcallbackResourceDiscovered(deviceName,resource_index);
}
void notificationAcknowledgement(NotificationObject *m_notificationObjectPtr)
{
    resource->sendNotificationAcknowledgement(m_notificationObjectPtr->mNotificationId);
	LOGI("ACK sent");
}
void onResourceDiscovered(std::shared_ptr<NotificationConsumer> foundResource)
{

    std::string resourceURI = foundResource->getUri();
    std::string hostAddress = foundResource->getAddress();
    LOGI("Resource Discovered %s %s", resourceURI.c_str(), hostAddress.c_str());

	
    foundResource->getDeviceName(&onGetDeviceName);
    notificationResourceList.push_back(foundResource);
}

void onResourceUpdated(NotificationObject *m_notificationObjectPtr)
{
    LOGI("onResourceUpdated callback");
    SendCallback(m_notificationObjectPtr);
	notificationAcknowledgement(m_notificationObjectPtr);

}

JNIEXPORT void JNICALL JNIStartSubscribeNotifications(JNIEnv *env, jobject thisObj,
        jint resourceIndex)
{
    resource = notificationResourceList[resourceIndex];

    LOGI("JNIStartSubscribeNotifications:after index");
    resource->subscribeNotifications(&onResourceUpdated);
    LOGI("Subscribing started...");

}
JNIEXPORT void JNICALL  JNIDiscover(JNIEnv *env, jobject thisObj)
{
    LOGI("JNI JNIDiscover: Enter");
    DeviceList.clear();
    notificationResourceList.clear();
    NotificationDiscoveryManager::getInstance()->discoverNotificationResource(RCSAddress::multicast(),
            &onResourceDiscovered);

}

JNIEXPORT void JNICALL  JNIStopSubscribeNotifications(JNIEnv *env, jobject thisObj)
{
    resource->unSubscribeNotifications();
    LOGI("Subscribing stopped");
}
