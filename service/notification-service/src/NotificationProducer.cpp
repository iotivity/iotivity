//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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
#include <cJSON.h>

#include <NotificationObject.h>
#include <NotificationUtility.h>

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

        int notificationAckId = cJSON_GetObjectItem(putJson, notifyAck)->valueint;
        int notificationId = cJSON_GetObjectItem(putJson, notification_id)->valueint;

        cb(notificationId, notificationAckId);
    }
}
namespace OIC
{
    namespace Service
    {
        std::string Key;
        std::string Value;
        int IdValue;
        int notifyAckIdValue;

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
            if (m_NotificationResource == NULL)
            {
                OC_LOG(INFO, TAG, "Resource Created......");
                m_NotificationResource = RCSResourceObject::Builder(m_uri, m_type,
                                         m_interface).setDiscoverable(true).setObservable(true).build();

                if (m_NotificationResource == NULL)
                {
                    OC_LOG(WARNING, TAG, "Resource not created.....");
                    return NULL;
                }
            }
            else
            {
                OC_LOG(WARNING, TAG, "Resource already created.....");
            }

            /**
                 * @superJson.
                 *  a Json object to set text, image and video payload according to their
                 *   priority i.e. either low, moderate or critical as json key-value pair
                 */

            superJson = cJSON_CreateObject();
            cJSON_AddStringToObject(superJson, t_Critical, "");
            cJSON_AddStringToObject(superJson, t_Moderate, "");
            cJSON_AddStringToObject(superJson, t_Low, "");
            cJSON_AddStringToObject(superJson, i_Critical, "");
            cJSON_AddStringToObject(superJson, i_Moderate, "");
            cJSON_AddStringToObject(superJson, i_Low, "");
            cJSON_AddStringToObject(superJson, v_Critical, "");
            cJSON_AddStringToObject(superJson, v_Moderate, "");
            cJSON_AddStringToObject(superJson, v_Low, "");

            m_NotificationResource->setAutoNotifyPolicy(RCSResourceObject::AutoNotifyPolicy:: UPDATED);

            Key = DeviceName;
            Value = notifyDeviceName;
            m_NotificationResource->setAttribute( Key, Value);

            IdValue = 0;
            notifyAckIdValue = 0;

            cJSON *ackJson = cJSON_CreateObject();
            char *jsonResponse;

            /**
                 * @ackJson.
                 *  a Json object to set notification Id and acknowledgement Id
                 *  as json key-value pair
                 */

            cJSON_AddNumberToObject(ackJson, notification_id, IdValue);
            cJSON_AddNumberToObject(ackJson, notifyAck, notifyAckIdValue);

            jsonResponse = cJSON_Print(ackJson);
            std::string ack(jsonResponse);
            m_NotificationResource->setAttribute(notification_ack, ack);

            m_NotificationResource->addAttributeUpdatedListener(notification_ack,
                    std::bind(IdAttributeUpdatedListener,
                              std::placeholders::_1, std::placeholders::_2, std::move(cb)));

            return NULL;
        }

        void NotificationProducer::stopNotificationManager()
        {
        }

        void NotificationProducer::sendNotification(NotificationObject *notificationObjectPtr)
        {
            OC_LOG(INFO, TAG, "sendNotif API enter......");
            cJSON *json = cJSON_CreateObject();
            char *jsonResponse;
            int nMessageType;

            if (notificationObjectPtr == NULL)
            {
                return;
            }

            /**
                 * @json.
                 *  a Json object to set different attributes of a notification
                 *  as json key-value pair
                 */

            cJSON_AddStringToObject(json, TimeStamp, &notificationObjectPtr->m_NotificationTime[0]);
            cJSON_AddStringToObject(json, Sender, &notificationObjectPtr->m_NotificationSender[0]);
            cJSON_AddNumberToObject(json, Ttl, notificationObjectPtr->m_NotificationTtl);
            cJSON_AddNumberToObject(json, Id, notificationObjectPtr->m_NotificationId);

            if (notificationObjectPtr->m_NotificationObjectType == NotificationObjectType::Text)
            {
                OC_LOG(INFO, TAG, "sendNotif text API enter......");
                TextNotification *textNotificationPtr = (TextNotification *) notificationObjectPtr;

                if (textNotificationPtr != NULL)
                {
                    cJSON_AddStringToObject(json, Message, &textNotificationPtr->m_NotificationMessage[0]);

                    jsonResponse = cJSON_Print(json);
                    std::string payload(jsonResponse);

                    if (notificationObjectPtr->m_NotificationMessageType == NotificationMessageType::Low)
                    {
                        cJSON_ReplaceItemInObject(superJson, t_Low, cJSON_CreateString(&payload[0]));
                    }
                    else if (notificationObjectPtr->m_NotificationMessageType == NotificationMessageType::Moderate)
                    {
                        cJSON_ReplaceItemInObject(superJson, t_Moderate, cJSON_CreateString(&payload[0]));
                    }
                    else if (notificationObjectPtr->m_NotificationMessageType == NotificationMessageType::Critical)
                    {
                        cJSON_ReplaceItemInObject(superJson, t_Critical, cJSON_CreateString(&payload[0]));
                    }

                    jsonResponse = cJSON_Print(superJson);
                    std::string final_payload(jsonResponse);
                    m_NotificationResource->setAttribute(notification_payload, final_payload);
                }
            }

            if (notificationObjectPtr->m_NotificationObjectType == NotificationObjectType::Image)
            {
                ImageNotification *imageNotificationPtr = (ImageNotification *) notificationObjectPtr;

                if (imageNotificationPtr != NULL)
                {
                    cJSON_AddStringToObject(json, IconUrl, &imageNotificationPtr->m_NotificationIconUrl[0]);
                    cJSON_AddStringToObject(json, Message, &imageNotificationPtr->m_NotificationMessage[0]);

                    jsonResponse = cJSON_Print(json);
                    std::string payload(jsonResponse);

                    if (notificationObjectPtr->m_NotificationMessageType == NotificationMessageType::Low)
                    {
                        cJSON_ReplaceItemInObject(superJson, i_Low, cJSON_CreateString(&payload[0]));
                    }
                    else if (notificationObjectPtr->m_NotificationMessageType == NotificationMessageType::Moderate)
                    {
                        cJSON_ReplaceItemInObject(superJson, i_Moderate, cJSON_CreateString(&payload[0]));
                    }
                    else if (notificationObjectPtr->m_NotificationMessageType == NotificationMessageType::Critical)
                    {
                        cJSON_ReplaceItemInObject(superJson, i_Critical, cJSON_CreateString(&payload[0]));
                    }

                    jsonResponse = cJSON_Print(superJson);
                    std::string final_payload(jsonResponse);
                    m_NotificationResource->setAttribute(notification_payload, final_payload);
                }
            }

            if (notificationObjectPtr->m_NotificationObjectType == NotificationObjectType::Video)
            {
                VideoNotification *videoNotificationPtr = (VideoNotification *) notificationObjectPtr;

                if (videoNotificationPtr != NULL)
                {
                    cJSON_AddStringToObject(json, VideoUrl, &videoNotificationPtr->m_NotificationVideoUrl[0]);

                    jsonResponse = cJSON_Print(json);
                    std::string payload(jsonResponse);

                    if (notificationObjectPtr->m_NotificationMessageType == NotificationMessageType::Low)
                    {
                        cJSON_ReplaceItemInObject(superJson, v_Low, cJSON_CreateString(&payload[0]));
                    }
                    else if (notificationObjectPtr->m_NotificationMessageType == NotificationMessageType::Moderate)
                    {
                        cJSON_ReplaceItemInObject(superJson, v_Moderate, cJSON_CreateString(&payload[0]));
                    }
                    else if (notificationObjectPtr->m_NotificationMessageType == NotificationMessageType::Critical)
                    {
                        cJSON_ReplaceItemInObject(superJson, v_Critical, cJSON_CreateString(&payload[0]));
                    }

                    jsonResponse = cJSON_Print(superJson);
                    std::string final_payload(jsonResponse);
                    m_NotificationResource->setAttribute(notification_payload, final_payload);
                }
            }

        }
    }
}
