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
        std::string ack = newValue.toString();
        char *nAck = &ack[0];
        cJSON *putJson = cJSON_Parse(nAck);

        std::string notificationHostAddress(cJSON_GetObjectItem(putJson, hostAddress)->valuestring);
        int notificationId = cJSON_GetObjectItem(putJson, notification_id)->valueint;

        cb(notificationId, notificationHostAddress);
    }
}
namespace OIC
{
    namespace Service
    {
        std::string Key;
        std::string Value;
        int IdValue;
        std::string hostAddressValue;

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
            if (mNotificationResource == NULL)
            {
                mNotificationResource = RCSResourceObject::Builder(m_uri, m_type,
                                        m_interface).setDiscoverable(true).setObservable(true).build();

                if (mNotificationResource == NULL)
                {
                    OC_LOG(WARNING, TAG, "Resource not created.....");
                    return NULL;
                }
            }
            else
            {
                OC_LOG(WARNING, TAG, "Resource already created.....");
            }

            mNotificationResource->setAutoNotifyPolicy(RCSResourceObject::AutoNotifyPolicy:: UPDATED);

            Key = DeviceName;
            Value = notifyDeviceName;
            mNotificationResource->setAttribute( Key, Value);

            IdValue = 0;
            hostAddressValue = " ";

            cJSON *ackJson = cJSON_CreateObject();
            char *jsonResponse;

            cJSON_AddNumberToObject(ackJson, notification_id, IdValue);
            cJSON_AddStringToObject(ackJson, hostAddress, &hostAddressValue[0]);

            jsonResponse = cJSON_Print(ackJson);
            std::string ack(jsonResponse);
            mNotificationResource->setAttribute(notification_ack, ack);

            mNotificationResource->addAttributeUpdatedListener(notification_ack,
                    std::bind(IdAttributeUpdatedListener,
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

            if (notificationObjectPtr == NULL)
            {
                return;
            }

            cJSON_AddStringToObject(json, TimeStamp, &notificationObjectPtr->mNotificationTime[0]);
            cJSON_AddStringToObject(json, Sender, &notificationObjectPtr->mNotificationSender[0]);
            cJSON_AddNumberToObject(json, Ttl, notificationObjectPtr->mNotificationTtl);
            cJSON_AddNumberToObject(json, Id, notificationObjectPtr->mNotificationId);

            if ( type == NotificationObjectType::Text)
            {
                TextNotification *textNotificationPtr = (TextNotification *) notificationObjectPtr;

                if (textNotificationPtr != NULL)
                {
                    cJSON_AddStringToObject(json, Message, &textNotificationPtr->mNotificationMessage[0]);
                    cJSON_AddStringToObject(json, ObjectType, "text");

                    jsonResponse = cJSON_Print(json);
                    std::string payload(jsonResponse);
                    mNotificationResource->setAttribute(notification_payload, payload);
                }
            }

            if ( type == NotificationObjectType::Image)
            {
                ImageNotification *imageNotificationPtr = (ImageNotification *) notificationObjectPtr;

                if (imageNotificationPtr != NULL)
                {
                    cJSON_AddStringToObject(json, IconUrl, &imageNotificationPtr->mNotificationIconUrl[0]);
                    cJSON_AddStringToObject(json, Message, &imageNotificationPtr->mNotificationMessage[0]);
                    cJSON_AddStringToObject(json, ObjectType, "image");

                    jsonResponse = cJSON_Print(json);
                    std::string payload(jsonResponse);
                    mNotificationResource->setAttribute(notification_payload, payload);
                }
            }

            if ( type == NotificationObjectType::Video)
            {
                VideoNotification *videoNotificationPtr = (VideoNotification *) notificationObjectPtr;

                if (videoNotificationPtr != NULL)
                {
                    cJSON_AddStringToObject(json, VideoUrl, &videoNotificationPtr->mNotificationVideoUrl[0]);
                    cJSON_AddStringToObject(json, ObjectType, "video");

                    jsonResponse = cJSON_Print(json);
                    std::string payload(jsonResponse);
                    mNotificationResource->setAttribute(notification_payload, payload);
                }
            }

        }
    }
}
