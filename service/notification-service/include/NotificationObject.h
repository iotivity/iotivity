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

/**
  * @file
  *
  * This file defines NotificationObject and its attributes.
  */

#ifndef NOTIFICATION_OBJECT_H
#define NOTIFICATION_OBJECT_H

#include <string>

namespace OIC
{
    namespace Service
    {
        enum class NotificationObjectType
        {
            Text = 1,
            Image,
            Video
        };

        enum class NotificationMessageType
        {
            Low = 1,
            Moderate,
            Critical
        };

        /**
         * This class is used to declare a basic NotificationObject
         * which is inherited by TextNotification , ImageNotification and
         * VideoNotification class to furthur define the attributes of the object
         * created.
         *
         *@see TextNotification
         *@see ImageNotification
         *@see VideoNotification
         */
        class NotificationObject
        {
            public:

                /** It is used to define the type of notification object to be created. */
                NotificationObjectType m_NotificationObjectType;

                /** It is used to define the type of notification message to be created. */
                NotificationMessageType m_NotificationMessageType;

                /** It is used to define the timestamp at which the notification was created. */
                std::string m_NotificationTime;

                /** It is used to define the sender of the notification. */
                std::string m_NotificationSender;

                /** It is used to define a unique Id for the notification. */
                unsigned int m_NotificationId;

                /** It is used to define the ttl of the notification after which it will expire. */
                int m_NotificationTtl;

                NotificationObject();

        };

        /**
         * This class is used to declare a Text notification object and define the corresponding
         * attributes.
         *
         * @see NotificationObject
         */
        class TextNotification : public NotificationObject
        {

            public:

                std::string m_NotificationMessage;

                void setTextAttributes(const std::string &notificationMessage,
                                       NotificationObjectType &notificationObjectType,
                                       NotificationMessageType &notificationMessageType,
                                       const std::string &notificationTime,
                                       const std::string &notificationSender,
                                       unsigned int notificationId,
                                       int notificationTtl);
        };

        /**
          * This class is used to declare a Image notification object and define the corresponding
          * attributes.
          *
          *@see NotificationObject
          */
        class ImageNotification : public NotificationObject
        {

            public:

                std::string m_NotificationIconUrl;
                std::string m_NotificationMessage;

                void setImageAttributes(const std::string &notificationIconUrl,
                                        const std::string notificationMessage,
                                        NotificationObjectType &notificationObjectType,
                                        NotificationMessageType &notificationMessageType,
                                        const std::string &notificationTime,
                                        const std::string &notificationSender,
                                        unsigned int notificationId,
                                        int notificationTtl);
        };

        /**
         * This class is used to declare a video notification object and define the corresponding
         * attributes.
         *
         *@see NotificationObject
         */
        class VideoNotification : public NotificationObject
        {
            public:

                std::string m_NotificationVideoUrl;

                void setVideoAttributes(const std::string &notificationVideoUrl,
                                        NotificationObjectType &notificationObjectType,
                                        NotificationMessageType &notificationMessageType,
                                        const std::string &notificationTime,
                                        const std::string &notificationSender,
                                        unsigned int notificationId,
                                        int notificationTtl);
        };
    }
}

#endif  // NOTIFICATION_OBJECT_H