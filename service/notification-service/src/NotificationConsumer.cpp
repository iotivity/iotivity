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

#include <NotificationConsumer.h>

#include <string.h>
#include <list>
#include<map>

#include <RCSRemoteResourceObject.h>
#include <RCSResourceAttributes.h>

#include <logger.h>
#include <cJSON.h>

#include <NotificationObject.h>
#include <NotificationUtility.h>

constexpr char TAG[] {"NotificationConsumer"};

namespace notificationConsumer
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
                        RCSRemoteResourceObject::Ptr notificationResource)
    {
        OC_LOG(INFO, TAG, "onCacheUpdated enter......");

        RCSResourceAttributes attr = attributes;

        std::shared_ptr<NotificationObject> notificationObjectPtr;

        if (!attr.contains(notification_payload))
        {
            OC_LOG(WARNING, TAG, "Notification Payload Null......");
            return;
        }

        std::string final_payload = attr[notification_payload].toString();
        char *nPayload = &final_payload[0];
        cJSON *putJson = cJSON_Parse(nPayload);
        NotificationMessageType mMessageType;

        std::string textCritical(cJSON_GetObjectItem(putJson, t_Critical)->valuestring);
        if (textCritical != "")
        {
            char *n_Payload = &textCritical[0];
            cJSON *putJson = cJSON_Parse(n_Payload);

            mMessageType = NotificationMessageType::Critical;
            std::string objectTime(cJSON_GetObjectItem(putJson, TimeStamp)->valuestring);
            std::string objectSender(cJSON_GetObjectItem(putJson, Sender)->valuestring);
            int objectTtl = cJSON_GetObjectItem(putJson, Ttl)->valueint;
            unsigned int objectId = cJSON_GetObjectItem(putJson, Id)->valueint;

            std::shared_ptr<TextNotification> textNotificationPtr = std::make_shared<TextNotification>();
            std::string objectMessage(cJSON_GetObjectItem(putJson, Message)->valuestring);

            textNotificationPtr->m_NotificationObjectType = NotificationObjectType::Text;
            textNotificationPtr->m_NotificationMessageType = mMessageType;
            textNotificationPtr->m_NotificationTime = objectTime;
            textNotificationPtr->m_NotificationSender = objectSender;
            textNotificationPtr->m_NotificationTtl = objectTtl;
            textNotificationPtr->m_NotificationMessage = objectMessage;
            textNotificationPtr->m_NotificationId = objectId;

            notificationObjectPtr = textNotificationPtr;

            cb(notificationObjectPtr.get(), notificationResource->getAddress(),
               std::make_shared<NotificationConsumer>(notificationResource));
        }

        std::string textModerate(cJSON_GetObjectItem(putJson, t_Moderate)->valuestring);
        if (textModerate != "")
        {
            char *n_Payload = &textModerate[0];
            cJSON *putJson = cJSON_Parse(n_Payload);

            mMessageType = NotificationMessageType::Moderate;
            std::string objectTime(cJSON_GetObjectItem(putJson, TimeStamp)->valuestring);
            std::string objectSender(cJSON_GetObjectItem(putJson, Sender)->valuestring);
            int objectTtl = cJSON_GetObjectItem(putJson, Ttl)->valueint;
            unsigned int objectId = cJSON_GetObjectItem(putJson, Id)->valueint;

            std::shared_ptr<TextNotification> textNotificationPtr = std::make_shared<TextNotification>();
            std::string objectMessage(cJSON_GetObjectItem(putJson, Message)->valuestring);

            textNotificationPtr->m_NotificationObjectType = NotificationObjectType::Text;
            textNotificationPtr->m_NotificationMessageType = mMessageType;
            textNotificationPtr->m_NotificationTime = objectTime;
            textNotificationPtr->m_NotificationSender = objectSender;
            textNotificationPtr->m_NotificationTtl = objectTtl;
            textNotificationPtr->m_NotificationMessage = objectMessage;
            textNotificationPtr->m_NotificationId = objectId;

            notificationObjectPtr = textNotificationPtr;

            cb(notificationObjectPtr.get(), notificationResource->getAddress(),
               std::make_shared<NotificationConsumer>(notificationResource));
        }

        std::string textLow(cJSON_GetObjectItem(putJson, t_Low)->valuestring);
        if (textLow != "")
        {
            OC_LOG(INFO, TAG, "onCacheUpdated enter......");

            char *n_Payload = &textLow[0];
            cJSON *putJson = cJSON_Parse(n_Payload);

            mMessageType = NotificationMessageType::Low;
            std::string objectTime(cJSON_GetObjectItem(putJson, TimeStamp)->valuestring);
            std::string objectSender(cJSON_GetObjectItem(putJson, Sender)->valuestring);
            int objectTtl = cJSON_GetObjectItem(putJson, Ttl)->valueint;
            unsigned int objectId = cJSON_GetObjectItem(putJson, Id)->valueint;

            std::shared_ptr<TextNotification> textNotificationPtr = std::make_shared<TextNotification>();
            std::string objectMessage(cJSON_GetObjectItem(putJson, Message)->valuestring);

            textNotificationPtr->m_NotificationObjectType = NotificationObjectType::Text;
            textNotificationPtr->m_NotificationMessageType = mMessageType;
            textNotificationPtr->m_NotificationTime = objectTime;
            textNotificationPtr->m_NotificationSender = objectSender;
            textNotificationPtr->m_NotificationTtl = objectTtl;
            textNotificationPtr->m_NotificationMessage = objectMessage;
            textNotificationPtr->m_NotificationId = objectId;

            notificationObjectPtr = textNotificationPtr;

            cb(notificationObjectPtr.get(), notificationResource->getAddress(),
               std::make_shared<NotificationConsumer>(notificationResource));
        }

        std::string imageCritical(cJSON_GetObjectItem(putJson, i_Critical)->valuestring);
        if (imageCritical != "")
        {
            char *n_Payload = &imageCritical[0];
            cJSON *putJson = cJSON_Parse(n_Payload);

            mMessageType = NotificationMessageType::Critical;
            std::string objectTime(cJSON_GetObjectItem(putJson, TimeStamp)->valuestring);
            std::string objectSender(cJSON_GetObjectItem(putJson, Sender)->valuestring);
            int objectTtl = cJSON_GetObjectItem(putJson, Ttl)->valueint;
            unsigned int objectId = cJSON_GetObjectItem(putJson, Id)->valueint;

            std::shared_ptr<ImageNotification> imageNotificationPtr = std::make_shared<ImageNotification>();
            std::string objectMessage(cJSON_GetObjectItem(putJson, Message)->valuestring);
            std::string objectImageUrl(cJSON_GetObjectItem(putJson, IconUrl)->valuestring);

            imageNotificationPtr->m_NotificationObjectType = NotificationObjectType::Image;
            imageNotificationPtr->m_NotificationMessageType = mMessageType;
            imageNotificationPtr->m_NotificationTime = objectTime;
            imageNotificationPtr->m_NotificationSender = objectSender;
            imageNotificationPtr->m_NotificationTtl = objectTtl;
            imageNotificationPtr->m_NotificationIconUrl = objectImageUrl;
            imageNotificationPtr->m_NotificationMessage = objectMessage;
            imageNotificationPtr->m_NotificationId = objectId;

            notificationObjectPtr = imageNotificationPtr;

            cb(notificationObjectPtr.get(), notificationResource->getAddress(),
               std::make_shared<NotificationConsumer>(notificationResource));
        }

        std::string imageModerate(cJSON_GetObjectItem(putJson, i_Moderate)->valuestring);
        if (imageModerate != "")
        {
            char *n_Payload = &imageModerate[0];
            cJSON *putJson = cJSON_Parse(n_Payload);

            mMessageType = NotificationMessageType::Moderate;
            std::string objectTime(cJSON_GetObjectItem(putJson, TimeStamp)->valuestring);
            std::string objectSender(cJSON_GetObjectItem(putJson, Sender)->valuestring);
            int objectTtl = cJSON_GetObjectItem(putJson, Ttl)->valueint;
            unsigned int objectId = cJSON_GetObjectItem(putJson, Id)->valueint;

            std::shared_ptr<ImageNotification> imageNotificationPtr = std::make_shared<ImageNotification>();
            std::string objectMessage(cJSON_GetObjectItem(putJson, Message)->valuestring);
            std::string objectImageUrl(cJSON_GetObjectItem(putJson, IconUrl)->valuestring);

            imageNotificationPtr->m_NotificationObjectType = NotificationObjectType::Image;
            imageNotificationPtr->m_NotificationMessageType = mMessageType;
            imageNotificationPtr->m_NotificationTime = objectTime;
            imageNotificationPtr->m_NotificationSender = objectSender;
            imageNotificationPtr->m_NotificationTtl = objectTtl;
            imageNotificationPtr->m_NotificationIconUrl = objectImageUrl;
            imageNotificationPtr->m_NotificationMessage = objectMessage;
            imageNotificationPtr->m_NotificationId = objectId;

            notificationObjectPtr = imageNotificationPtr;

            cb(notificationObjectPtr.get(), notificationResource->getAddress(),
               std::make_shared<NotificationConsumer>(notificationResource));
        }

        std::string imageLow(cJSON_GetObjectItem(putJson, i_Low)->valuestring);
        if (imageLow != "")
        {
            char *n_Payload = &imageLow[0];
            cJSON *putJson = cJSON_Parse(n_Payload);

            mMessageType = NotificationMessageType::Low;
            std::string objectTime(cJSON_GetObjectItem(putJson, TimeStamp)->valuestring);
            std::string objectSender(cJSON_GetObjectItem(putJson, Sender)->valuestring);
            int objectTtl = cJSON_GetObjectItem(putJson, Ttl)->valueint;
            unsigned int objectId = cJSON_GetObjectItem(putJson, Id)->valueint;

            std::shared_ptr<ImageNotification> imageNotificationPtr = std::make_shared<ImageNotification>();
            std::string objectMessage(cJSON_GetObjectItem(putJson, Message)->valuestring);
            std::string objectImageUrl(cJSON_GetObjectItem(putJson, IconUrl)->valuestring);

            imageNotificationPtr->m_NotificationObjectType = NotificationObjectType::Image;
            imageNotificationPtr->m_NotificationMessageType = mMessageType;
            imageNotificationPtr->m_NotificationTime = objectTime;
            imageNotificationPtr->m_NotificationSender = objectSender;
            imageNotificationPtr->m_NotificationTtl = objectTtl;
            imageNotificationPtr->m_NotificationIconUrl = objectImageUrl;
            imageNotificationPtr->m_NotificationMessage = objectMessage;
            imageNotificationPtr->m_NotificationId = objectId;

            notificationObjectPtr = imageNotificationPtr;

            cb(notificationObjectPtr.get(), notificationResource->getAddress(),
               std::make_shared<NotificationConsumer>(notificationResource));
        }

        std::string videoCritical(cJSON_GetObjectItem(putJson, v_Critical)->valuestring);
        if (videoCritical != "")
        {
            char *n_Payload = &videoCritical[0];
            cJSON *putJson = cJSON_Parse(n_Payload);

            mMessageType = NotificationMessageType::Critical;
            std::string objectTime(cJSON_GetObjectItem(putJson, TimeStamp)->valuestring);
            std::string objectSender(cJSON_GetObjectItem(putJson, Sender)->valuestring);
            int objectTtl = cJSON_GetObjectItem(putJson, Ttl)->valueint;
            unsigned int objectId = cJSON_GetObjectItem(putJson, Id)->valueint;


            std::shared_ptr<VideoNotification> videoNotificationPtr = std::make_shared<VideoNotification>();
            std::string objectVideoUrl(cJSON_GetObjectItem(putJson, VideoUrl)->valuestring);

            videoNotificationPtr->m_NotificationObjectType = NotificationObjectType::Video;
            videoNotificationPtr->m_NotificationMessageType = mMessageType;
            videoNotificationPtr->m_NotificationTime = objectTime;
            videoNotificationPtr->m_NotificationSender = objectSender;
            videoNotificationPtr->m_NotificationTtl = objectTtl;
            videoNotificationPtr->m_NotificationVideoUrl = objectVideoUrl;
            videoNotificationPtr->m_NotificationId = objectId;

            notificationObjectPtr = videoNotificationPtr;

            cb(notificationObjectPtr.get(), notificationResource->getAddress(),
               std::make_shared<NotificationConsumer>(notificationResource));
        }

        std::string videoModerate(cJSON_GetObjectItem(putJson, v_Moderate)->valuestring);
        if (videoModerate != "")
        {
            char *n_Payload = &videoModerate[0];
            cJSON *putJson = cJSON_Parse(n_Payload);

            mMessageType = NotificationMessageType::Moderate;
            std::string objectTime(cJSON_GetObjectItem(putJson, TimeStamp)->valuestring);
            std::string objectSender(cJSON_GetObjectItem(putJson, Sender)->valuestring);
            int objectTtl = cJSON_GetObjectItem(putJson, Ttl)->valueint;
            unsigned int objectId = cJSON_GetObjectItem(putJson, Id)->valueint;


            std::shared_ptr<VideoNotification> videoNotificationPtr = std::make_shared<VideoNotification>();
            std::string objectVideoUrl(cJSON_GetObjectItem(putJson, VideoUrl)->valuestring);

            videoNotificationPtr->m_NotificationObjectType = NotificationObjectType::Video;
            videoNotificationPtr->m_NotificationMessageType = mMessageType;
            videoNotificationPtr->m_NotificationTime = objectTime;
            videoNotificationPtr->m_NotificationSender = objectSender;
            videoNotificationPtr->m_NotificationTtl = objectTtl;
            videoNotificationPtr->m_NotificationVideoUrl = objectVideoUrl;
            videoNotificationPtr->m_NotificationId = objectId;

            notificationObjectPtr = videoNotificationPtr;

            cb(notificationObjectPtr.get(), notificationResource->getAddress(),
               std::make_shared<NotificationConsumer>(notificationResource));
        }

        std::string videoLow(cJSON_GetObjectItem(putJson, v_Low)->valuestring);
        if (videoLow != "")
        {
            char *n_Payload = &videoLow[0];
            cJSON *putJson = cJSON_Parse(n_Payload);

            mMessageType = NotificationMessageType::Low;
            std::string objectTime(cJSON_GetObjectItem(putJson, TimeStamp)->valuestring);
            std::string objectSender(cJSON_GetObjectItem(putJson, Sender)->valuestring);
            int objectTtl = cJSON_GetObjectItem(putJson, Ttl)->valueint;
            unsigned int objectId = cJSON_GetObjectItem(putJson, Id)->valueint;


            std::shared_ptr<VideoNotification> videoNotificationPtr = std::make_shared<VideoNotification>();
            std::string objectVideoUrl(cJSON_GetObjectItem(putJson, VideoUrl)->valuestring);

            videoNotificationPtr->m_NotificationObjectType = NotificationObjectType::Video;
            videoNotificationPtr->m_NotificationMessageType = mMessageType;
            videoNotificationPtr->m_NotificationTime = objectTime;
            videoNotificationPtr->m_NotificationSender = objectSender;
            videoNotificationPtr->m_NotificationTtl = objectTtl;
            videoNotificationPtr->m_NotificationVideoUrl = objectVideoUrl;
            videoNotificationPtr->m_NotificationId = objectId;

            notificationObjectPtr = videoNotificationPtr;

            cb(notificationObjectPtr.get(), notificationResource->getAddress(),
               std::make_shared<NotificationConsumer>(notificationResource));
        }

        OC_LOG(INFO, TAG, "onCacheUpdated exit......");
    }
}

namespace OIC
{
    namespace Service
    {
        NotificationConsumer::NotificationConsumer(RCSRemoteResourceObject::Ptr remoteResource):
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
                        notificationConsumer::onRemoteAttributesReceived, std::placeholders::_1,
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
                m_NotificationResource->startCaching(std::bind(notificationConsumer::onCacheUpdated,
                                                     std::placeholders::_1,
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
                OC_LOG(INFO, TAG, "Subscribing not started.......");
            }
        }

        bool NotificationConsumer::isSubscribing()
        {
            if (m_NotificationResource->isCaching())
            {
                OC_LOG(INFO, TAG, "Already started Subscribing......");
                return true;
            }
            else
            {
                return false;
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

        void NotificationConsumer::sendAcknowledgement(int notificationId)
        {
            RCSResourceAttributes setAttribute;

            cJSON *ackJson = cJSON_CreateObject();
            char *jsonResponse;

            srand(time(NULL));
            unsigned int notifyAckIdValue = rand();


            cJSON_AddNumberToObject(ackJson, notification_id, notificationId);
            cJSON_AddNumberToObject(ackJson, notifyAck, notifyAckIdValue);

            jsonResponse = cJSON_Print(ackJson);
            std::string ack(jsonResponse);
            setAttribute[notification_ack] =  ack;

            if (m_NotificationResource != NULL)
            {
                m_NotificationResource->setRemoteAttributes(setAttribute,
                        &notificationConsumer::onSetRemoteAttributesReceived);
            }
        }
    }
}
