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

#include <ctime>
#include <stdlib.h>

#include "NotificationProducer.h"
#include "jni_notification_service.h"
#include "jni_notification_service_jvm.h"


using namespace OIC::Service;

constexpr int CORRECT_INPUT = 99;
constexpr int INCORRECT_INPUT = 100;

std::string resourceUri = "/oic/notify";
std::string resourceType = "oic.r.notify";
std::string resourceInterface = "oic.if.";

NotificationProducer notificationProducer(resourceUri, resourceType, resourceInterface);

JNIEXPORT void JNICALL  JNIStartNotificationProducer(JNIEnv *env, jobject thisObj,
        jstring notifyDeviceName)
{
    LOGI("JNI JNIstartNotificationProducer: Enter");
    std::string deviceName = env->GetStringUTFChars(notifyDeviceName, NULL);
    notificationProducer.startNotificationManager(deviceName);

}

JNIEXPORT void JNICALL  JNISendNotification(JNIEnv *env, jobject thisObj, jstring URL,
        jstring Message, jstring Sender, jint MessageType)
{
    LOGI("JNISendNotification %d", MessageType);

   NotificationObject *notificationObjectPtr;//=new NotificationObject();

    time_t now = time(0);
    char *dt = ctime(&now);

    std::string notificationObjectType;
    std::string notificationMessage;
    std::string notificationIconUrl;
    std::string notificationVideoUrl;

    std::string notificationTime = dt;//notificationObjectPtr->mNotificationTime;
    std::string notificationSender;
    int notificationTtl = 9;
    int notificationId=rand();//notificationObjectPtr->mNotificationId;


    NotificationObjectType nText = NotificationObjectType::Text;
    NotificationObjectType nImage = NotificationObjectType::Image;
    NotificationObjectType nVideo = NotificationObjectType::Video;

    if (MessageType == 0)
    {

        TextNotification textNotificationObject;
        notificationMessage = env->GetStringUTFChars(Message, NULL);
        notificationSender = env->GetStringUTFChars(Sender, NULL);
        textNotificationObject.setTextAttributes(notificationMessage, nText ,
                notificationTime, notificationSender,
                notificationId,
                notificationTtl);

        notificationObjectPtr = &textNotificationObject;

        notificationProducer.sendNotification(nText, notificationObjectPtr);

    }

    if (MessageType == 1)
    {

        ImageNotification imageNotificationObject;
        notificationIconUrl = env->GetStringUTFChars(URL, NULL);
        notificationMessage = env->GetStringUTFChars(Message, NULL);
        notificationSender = env->GetStringUTFChars(Sender, NULL);

        imageNotificationObject.setImageAttributes(notificationIconUrl, notificationMessage,
                nImage, //notificationMessageType ,
                notificationTime,
                notificationSender,
                notificationId,
                notificationTtl);

        notificationObjectPtr = &imageNotificationObject;

        notificationProducer.sendNotification(nImage, notificationObjectPtr);

    }

    if (MessageType == 2)
    {

        VideoNotification videoNotificationObject;
        notificationVideoUrl = env->GetStringUTFChars(URL, NULL);
        notificationSender = env->GetStringUTFChars(Sender, NULL);

        videoNotificationObject.setVideoAttributes(notificationVideoUrl, nVideo,
                //notificationMessageType,
                notificationTime, notificationSender,
                notificationId,
                notificationTtl);

        notificationObjectPtr = &videoNotificationObject;

        notificationProducer.sendNotification(nVideo, notificationObjectPtr);
    }
}

