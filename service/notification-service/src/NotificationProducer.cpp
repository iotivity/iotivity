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

#include <NotificationProducer.h>

#include <string.h>

#include <RCSResourceAttributes.h>
#include <RCSResourceObject.h>

#include <logger.h>
#include <OCPlatform.h>
#include "cJSON.h"

#include <NotificationObject.h>

constexpr char TAG[] { "NotificationProducer" };

namespace
{
    using namespace OIC::Service;

    void IdAttributeUpdatedListener(const RCSResourceAttributes::Value &oldValue,
                                    const RCSResourceAttributes::Value &newValue, NotificationProducer::notificationIdListener cb)
    {
        int notificationId = std::stoi (newValue.toString());

        cb(notificationId);
    }
}
namespace OIC
{
    namespace Service
    {
        std::string Key;
        std::string Value;
        int IdValue;

        NotificationProducer::NotificationProducer(const std::string &uri,
                const std::string &type,
                const std::string &interface) :
            m_uri {uri},
            m_type {type},
            m_interface {interface}
        {
        }

        NotificationProducer::NotificationProducerPtr NotificationProducer::startNotificationManager(
            std::string &notifyDeviceName, NotificationProducer::notificationIdListener cb)
        {
            if (mRCSResource == NULL)
            {
                mRCSResource = RCSResourceObject::Builder(m_uri, m_type,
                               m_interface).setDiscoverable(true).setObservable(true).build();
            }
            else
            {
                OC_LOG(WARNING, TAG, "Resource already created.....");
            }

            mRCSResource->setAutoNotifyPolicy(RCSResourceObject::AutoNotifyPolicy:: UPDATED);

            Key = "DeviceName";
            Value = notifyDeviceName;
            mRCSResource->setAttribute( Key, Value);

            std::string nIdKey = "notificationId";
            IdValue = 0;
            mRCSResource->setAttribute(nIdKey, IdValue);

            mRCSResource->addAttributeUpdatedListener(nIdKey, std::bind(IdAttributeUpdatedListener,
                    std::placeholders::_1, std::placeholders::_2, std::move(cb)));
            return NULL;
        }

        void NotificationProducer::stopNotificationManager()
        {
        }

        void NotificationProducer::sendNotification(NotificationObjectType &type,
                NotificationObject *notificationObjectPtr)
        {
            cJSON *json = cJSON_CreateObject();
            char *jsonResponse;
            std::string payload;

            cJSON_AddStringToObject(json, "TIME", &notificationObjectPtr->mNotificationTime[0]);
            cJSON_AddStringToObject(json, "SENDER", &notificationObjectPtr->mNotificationSender[0]);
            cJSON_AddNumberToObject(json, "TTL", notificationObjectPtr->mNotificationTtl);
            cJSON_AddNumberToObject(json, "ID", notificationObjectPtr->mNotificationId);

            if ( type == NotificationObjectType::Text)
            {
                TextNotification *textNotificationPtr = (TextNotification *) notificationObjectPtr;

                ///TODO: Message Type attribute is to be handled
                //mRCSResource->setAttribute(MESSAGETYPE, textNotificationPtr->mNotificationMessageType);
                cJSON_AddStringToObject(json, "MESSAGE", &textNotificationPtr->mNotificationMessage[0]);
                cJSON_AddStringToObject(json, "OBJECTTYPE", "text");

                jsonResponse = cJSON_Print(json);
                std::string payload(jsonResponse);
                mRCSResource->setAttribute("notification-payload", payload);
            }

            if ( type == NotificationObjectType::Image)
            {

                ImageNotification *imageNotificationPtr = (ImageNotification *) notificationObjectPtr;

                ///TODO: Message Type attribute is to be handled
                //mRCSResource->setAttribute(MESSAGETYPE, imageNotificationPtr->mNotificationMessageType);
                cJSON_AddStringToObject(json, "ICONURL", &imageNotificationPtr->mNotificationIconUrl[0]);
                cJSON_AddStringToObject(json, "MESSAGE", &imageNotificationPtr->mNotificationMessage[0]);
                cJSON_AddStringToObject(json, "OBJECTTYPE", "image");

                jsonResponse = cJSON_Print(json);
                std::string payload(jsonResponse);
                mRCSResource->setAttribute("notification-payload", payload);
            }

            if ( type == NotificationObjectType::Video)
            {

                VideoNotification *videoNotificationPtr = (VideoNotification *) notificationObjectPtr;

                ///TODO: Message Type attribute is to be handled
                //mRCSResource->setAttribute(MESSAGETYPE, videoNotificationPtr->mNotificationMessageType);
                cJSON_AddStringToObject(json, "VIDEOURL", &videoNotificationPtr->mNotificationVideoUrl[0]);
                cJSON_AddStringToObject(json, "OBJECTTYPE", "video");

                jsonResponse = cJSON_Print(json);
                std::string payload(jsonResponse);
                mRCSResource->setAttribute("notification-payload", payload);
            }
        }
    }
}
