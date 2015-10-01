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
            Information, Emergency, Warning
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

                NotificationObjectType mNotificationObjectType;
                NotificationMessageType mNotificationMessageType;
                std::string mNotificationTime;
                std::string mNotificationSender;
                int mNotificationId;
                int mNotificationTtl;

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

                std::string mNotificationMessage;

                void setTextAttributes(const std::string &nNotificationMessage,
                                       NotificationObjectType &nNotificationObjectType,
                                       ///TODO: Message Type attribute is to be handled
                                       //NotificationMessageType &nNotificationMessageType,
                                       const std::string &nNotificationTime,
                                       const std::string &nNotificationSender,
                                       int nNotificationId,
                                       int nNotificationTtl);
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

                std::string mNotificationIconUrl;
                std::string mNotificationMessage;

                void setImageAttributes(const std::string &nNotificationIconUrl,
                                        const std::string nNotificationMessage,
                                        NotificationObjectType &nNotificationObjectType,
                                        ///TODO: Message Type attribute is to be handled
                                        // NotificationMessageType &nNotificationMessageType,
                                        const std::string &nNotificationTime,
                                        const std::string &nNotificationSender,
                                        int nNotificationId,
                                        int nNotificationTtl);
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

                std::string mNotificationVideoUrl;

                void setVideoAttributes(const std::string &nNotificationVideoUrl,
                                        NotificationObjectType &nNotificationObjectType,
                                        ///TODO: Message Type attribute is to be handled
                                        // NotificationMessageType &nNotificationMessageType,
                                        const std::string &nNotificationTime,
                                        const std::string &nNotificationSender,
                                        int nNotificationId,
                                        int nNotificationTtl);
        };
    }
}

#endif  // NOTIFICATION_OBJECT_H