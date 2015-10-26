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

#include <sstream>

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

    void onCacheUpdated(const RCSResourceAttributes &attributes,
                        NotificationConsumer::SubscribedCallback cb)
    {
        ///TODO: Message Type attribute is to be handled
        //mNotificationMessageType

        RCSResourceAttributes attr = attributes;

        std::string payload = attr["notification-payload"].toString();
        char *nPayload = &payload[0];

        cJSON *putJson = cJSON_Parse(nPayload);

        std::string objectType(cJSON_GetObjectItem(putJson, "OBJECTTYPE")->valuestring);
        std::string objectTime(cJSON_GetObjectItem(putJson, "TIME")->valuestring);
        std::string objectSender(cJSON_GetObjectItem(putJson, "SENDER")->valuestring);
        int objectTtl = cJSON_GetObjectItem(putJson, "TTL")->valueint;
        int objectId = cJSON_GetObjectItem(putJson, "ID")->valueint;

        if (objectType == "text")
        {
            std::shared_ptr<TextNotification> textNotificationPtr = std::make_shared<TextNotification>();
            std::string objectMessage(cJSON_GetObjectItem(putJson, "MESSAGE")->valuestring);

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
            std::string objectMessage(cJSON_GetObjectItem(putJson, "MESSAGE")->valuestring);
            std::string objectImageUrl(cJSON_GetObjectItem(putJson, "ICONURL")->valuestring);

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
            std::string objectVideoUrl(cJSON_GetObjectItem(putJson, "VIDEOURL")->valuestring);

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

    void onRemoteAttributesReceived(const RCSResourceAttributes &attributes, int,
                                    NotificationConsumer::onGetDeviceName cb)
    {
        std::string DeviceName;

        for (const auto &attr : attributes)
        {
            if (attr.key() == "DeviceName")
            {
                DeviceName = attr.value().toString();
                break;
            }
        }
        cb(DeviceName);
    }

    void onSetRemoteAttributesReceived(const RCSResourceAttributes &attributes, int)
    {
        OC_LOG(WARNING, TAG, "Acknowledgement Sent............");
    }
}

namespace OIC
{
    namespace Service
    {
        NotificationConsumer::NotificationConsumer(std::shared_ptr<RCSRemoteResourceObject> remoteResource):
            m_rcsRemoteResource {remoteResource}
        {
        }

        NotificationConsumer::~NotificationConsumer()
        {
        }

        void NotificationConsumer::getDeviceName(NotificationConsumer::onGetDeviceName cb)
        {
            m_rcsRemoteResource->RCSRemoteResourceObject::getRemoteAttributes(std::bind(
                        onRemoteAttributesReceived, std::placeholders::_1,
                        std::placeholders::_2, std::move(cb)));
        }



        void NotificationConsumer::subscribeNotifications(SubscribedCallback cb)
        {

            if (!m_rcsRemoteResource->isCaching())
            {
                m_rcsRemoteResource->startCaching(std::bind(onCacheUpdated, std::placeholders::_1,
                                                  std::move(cb)));
            }
            else
            {
                OC_LOG(WARNING, TAG, "Already started Subscribing......");
            }

        }

        void NotificationConsumer::unSubscribeNotifications()
        {
            if (m_rcsRemoteResource->isCaching())
            {
                m_rcsRemoteResource->stopCaching();
            }
            else
            {
                OC_LOG(WARNING, TAG, "Subscribing not started.......");
            }
        }

        void NotificationConsumer::sendNotificationAcknowledgement(int notificationId)
        {
            RCSResourceAttributes setAttribute;

            setAttribute["notificationId"] = notificationId;
            m_rcsRemoteResource->setRemoteAttributes(setAttribute,
                    &onSetRemoteAttributesReceived);
        }

        std::string NotificationConsumer::getUri() const
        {
            return (m_rcsRemoteResource->getUri());
        }

        std::string NotificationConsumer::getAddress() const
        {
            return (m_rcsRemoteResource->getAddress());
        }
    }
}
