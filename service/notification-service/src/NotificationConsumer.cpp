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

#include <NotificationConsumer.h>

#include <RCSRemoteResourceObject.h>
#include <RCSResourceAttributes.h>

#include <logger.h>
#include <cJSON.h>

#include <NotificationObject.h>
#include <NotificationProducer.h>

constexpr char TAG[] {"NotificationConsumer"};

namespace
{
    using namespace OIC::Service;

    void onSetRemoteAttributesReceived(const RCSResourceAttributes &attributes, int)
    {
        OC_LOG(WARNING, TAG, "Acknowledgement Sent............");
    }

    void onRemoteAttributesReceived(const RCSResourceAttributes &attributes, int,
                                    NotificationConsumer::onGetDeviceName cb)
    {
        std::string deviceName;

        for (const auto &attr : attributes)
        {
            if (attr.key() == DeviceName)
            {
                deviceName = attr.value().toString();
                break;
            }
        }
        cb(deviceName);
    }
    void onCacheUpdated(const RCSResourceAttributes &attributes,
                        NotificationConsumer::SubscribedCallback cb,
                        std::shared_ptr<RCSRemoteResourceObject> m_NotificationResource)
    {
        RCSResourceAttributes attr = attributes;

        if (!attr.contains(notification_payload))
        {
            OC_LOG(WARNING, TAG, "Notification Payload Null......");
            return;
        }

        std::string payload = attr[notification_payload].toString();
        char *nPayload = &payload[0];
        cJSON *putJson = cJSON_Parse(nPayload);

        std::string objectType(cJSON_GetObjectItem(putJson, ObjectType)->valuestring);
        std::string objectTime(cJSON_GetObjectItem(putJson, TimeStamp)->valuestring);
        std::string objectSender(cJSON_GetObjectItem(putJson, Sender)->valuestring);
        int objectTtl = cJSON_GetObjectItem(putJson, Ttl)->valueint;
        int objectId = cJSON_GetObjectItem(putJson, Id)->valueint;

        if (objectType == "text")
        {
            std::shared_ptr<TextNotification> textNotificationPtr = std::make_shared<TextNotification>();
            std::string objectMessage(cJSON_GetObjectItem(putJson, Message)->valuestring);

            textNotificationPtr->mNotificationObjectType = NotificationObjectType::Text;
            textNotificationPtr->mNotificationTime = objectTime;
            textNotificationPtr->mNotificationSender = objectSender;
            textNotificationPtr->mNotificationTtl = objectTtl;
            textNotificationPtr->mNotificationMessage = objectMessage;
            textNotificationPtr->mNotificationId = objectId;

            std::shared_ptr<NotificationObject> notificationObjectPtr = textNotificationPtr;

            cb(notificationObjectPtr.get());
        }

        else if (objectType == "image")
        {
            std::shared_ptr<ImageNotification> imageNotificationPtr = std::make_shared<ImageNotification>();
            std::string objectMessage(cJSON_GetObjectItem(putJson, Message)->valuestring);
            std::string objectImageUrl(cJSON_GetObjectItem(putJson, IconUrl)->valuestring);

            imageNotificationPtr->mNotificationObjectType = NotificationObjectType::Image;
            imageNotificationPtr->mNotificationTime = objectTime;
            imageNotificationPtr->mNotificationSender = objectSender;
            imageNotificationPtr->mNotificationTtl = objectTtl;
            imageNotificationPtr->mNotificationIconUrl = objectImageUrl;
            imageNotificationPtr->mNotificationMessage = objectMessage;
            imageNotificationPtr->mNotificationId = objectId;

            std::shared_ptr<NotificationObject> notificationObjectPtr = imageNotificationPtr;

            cb(notificationObjectPtr.get());
        }

        else if (objectType == "video")
        {
            std::shared_ptr<VideoNotification> videoNotificationPtr = std::make_shared<VideoNotification>();
            std::string objectVideoUrl(cJSON_GetObjectItem(putJson, VideoUrl)->valuestring);

            videoNotificationPtr->mNotificationObjectType = NotificationObjectType::Video;
            videoNotificationPtr->mNotificationTime = objectTime;
            videoNotificationPtr->mNotificationSender = objectSender;
            videoNotificationPtr->mNotificationTtl = objectTtl;
            videoNotificationPtr->mNotificationVideoUrl = objectVideoUrl;
            videoNotificationPtr->mNotificationId = objectId;

            std::shared_ptr<NotificationObject> notificationObjectPtr = videoNotificationPtr;

            cb(notificationObjectPtr.get());
        }
    }
}

namespace OIC
{
    namespace Service
    {
        NotificationConsumer::NotificationConsumer(std::shared_ptr<RCSRemoteResourceObject> remoteResource):
            m_NotificationResource {remoteResource}
        {
        }

        NotificationConsumer::~NotificationConsumer()
        {
        }

        void NotificationConsumer::getDeviceName(NotificationConsumer::onGetDeviceName cb)
        {
            if (m_NotificationResource == NULL)
            {
                return;
            }
            m_NotificationResource->RCSRemoteResourceObject::getRemoteAttributes(std::bind(
                        onRemoteAttributesReceived, std::placeholders::_1,
                        std::placeholders::_2, std::move(cb)));
        }



        void NotificationConsumer::subscribeNotifications(SubscribedCallback cb)
        {
            if (m_NotificationResource == NULL)
            {
                return;
            }

            if (!m_NotificationResource->isCaching())
            {
                m_NotificationResource->startCaching(std::bind(onCacheUpdated, std::placeholders::_1,
                                                     std::move(cb), m_NotificationResource));
            }
            else
            {
                OC_LOG(INFO, TAG, "Already started Subscribing......");
            }
        }

        void NotificationConsumer::unSubscribeNotifications()
        {
            if (m_NotificationResource == NULL)
            {
                return;
            }

            if (m_NotificationResource->isCaching())
            {
                m_NotificationResource->stopCaching();
            }
            else
            {
                OC_LOG(ERROR, TAG, "Subscribing not started.......");
            }
        }

        std::string NotificationConsumer::getUri() const
        {
            if (m_NotificationResource != NULL)
                return (m_NotificationResource->getUri());
        }

        std::string NotificationConsumer::getAddress() const
        {
            if (m_NotificationResource != NULL)
                return (m_NotificationResource->getAddress());
        }

        void NotificationConsumer::sendAcknowledgement(int notificationId, std::string hostAddressValue)
        {
            RCSResourceAttributes setAttribute;

            cJSON *ackJson = cJSON_CreateObject();
            char *jsonResponse;

            cJSON_AddNumberToObject(ackJson, notification_id, notificationId);
            cJSON_AddStringToObject(ackJson, hostAddress, &hostAddressValue[0]);

            jsonResponse = cJSON_Print(ackJson);
            std::string ack(jsonResponse);
            setAttribute[notification_ack] =  ack;

            if (m_NotificationResource != NULL)
            {
                m_NotificationResource->setRemoteAttributes(setAttribute,
                        &onSetRemoteAttributesReceived);
            }
        }
    }
}
