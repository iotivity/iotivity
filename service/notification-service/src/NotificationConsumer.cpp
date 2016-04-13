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

#include<iostream>
#include <vector>

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


    void onCacheUpdated(const RCSResourceAttributes &attributes,
                        NotificationConsumer::SubscribedCallback cb,
                        RCSRemoteResourceObject::Ptr notificationResource)
    {
        OC_LOG(INFO, TAG, "onCacheUpdated enter......");

        RCSResourceAttributes attr = attributes;

        std::shared_ptr<NotificationObject> notificationObjectPtr;

        if (!attr.contains(PAYLOAD))
        {
            OC_LOG(WARNING, TAG, "Notification Payload Null......");
            return;
        }

        //parse Notification unique ID
        unsigned int notifyId = std::atoi(attr[NOTIFICATION_ID].toString().c_str());

        //parse details of notification
        std:: string details = attr[DETAILS].toString();
        char *detailsChar = &details[0];
        cJSON *detailsJson = cJSON_Parse(detailsChar);

        NotificationMessageType notifyType = (NotificationMessageType) cJSON_GetObjectItem(detailsJson,
                                             TYPE)->valueint;
        NotificationObjectType notifyPriority = (NotificationObjectType) cJSON_GetObjectItem(detailsJson,
                                                PRIORITY)->valueint;
        std::string notifyTime = cJSON_GetObjectItem(detailsJson, TIMESTAMP)->valuestring;
        int notifyTtl =  cJSON_GetObjectItem(detailsJson, TTL)->valueint;

        //parse extra details sent by sender with notification
        int item_size = 0;
        std::vector<std::string> senderDetails;
        cJSON *senderJson = cJSON_GetObjectItem(detailsJson, SENDER);

        if (senderJson)
        {
            item_size = cJSON_GetArraySize(senderJson);
        }
        for (int item_no = 0; item_no < item_size; item_no++)
        {
            std::string value = cJSON_GetArrayItem(senderJson, item_no)->valuestring;
            senderDetails.push_back(value);
        }

        //parse payload as a json
        std:: string payload = attr[PAYLOAD].toString();
        char *payloadChar = &payload[0];
        cJSON *payloadJson = cJSON_Parse(payloadChar);

        if (notifyType == NotificationMessageType::Text)
        {
            std::shared_ptr<TextNotification> textNotificationPtr = std::make_shared<TextNotification>();

            std::string textMessage = cJSON_GetObjectItem(payloadJson, MESSAGE)->valuestring;

            textNotificationPtr = TextNotification::Builder().setType(notifyType).setPriority(
                                      notifyPriority).setTime(
                                      notifyTime).setTtl(notifyTtl).setId(notifyId).setSender(senderDetails).setMessage(
                                      textMessage).build();

            notificationObjectPtr = textNotificationPtr;
        }
        else if (notifyType == NotificationMessageType::Image)
        {
            std::shared_ptr<ImageNotification> imageNotificationPtr = std::make_shared<ImageNotification>();

            std::string imageTitle = cJSON_GetObjectItem(payloadJson, TITLE)->valuestring;
            std::string imageUrl = cJSON_GetObjectItem(payloadJson, IMAGE_URL)->valuestring;

            imageNotificationPtr = ImageNotification::Builder().setType(notifyType).setPriority(
                                       notifyPriority).setTime(
                                       notifyTime).setTtl(notifyTtl).setId(notifyId).setSender(senderDetails).setTitle(
                                       imageTitle).setImageUrl(imageUrl).build();


            notificationObjectPtr = imageNotificationPtr;
        }
        else if (notifyType == NotificationMessageType::Video)
        {
            std::shared_ptr<VideoNotification> videoNotificationPtr = std::make_shared<VideoNotification>();

            std::string videoUrl = cJSON_GetObjectItem(payloadJson, VIDEO_URL)->valuestring;

            videoNotificationPtr = VideoNotification::Builder().setType(notifyType).setPriority(
                                       notifyPriority).setTime(
                                       notifyTime).setTtl(notifyTtl).setId(notifyId).setSender(senderDetails).setVideoUrl(
                                       videoUrl).build();

            notificationObjectPtr = videoNotificationPtr;
        }

        cb(notificationObjectPtr.get(), notificationResource->getAddress(),
           std::make_shared<NotificationConsumer>(notificationResource));

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

            srand(time(NULL));
            int notifyAckIdValue = rand();

            setAttribute[ACK_ID] = notifyAckIdValue;

            if (m_NotificationResource != NULL)
            {
                m_NotificationResource->setRemoteAttributes(setAttribute,
                        &notificationConsumer::onSetRemoteAttributesReceived);
            }
        }
    }
}
