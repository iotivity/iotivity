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
#include <vector>
#include <memory>

namespace OIC
{
    namespace Service
    {
        enum class NotificationMessageType
        {
            Text = 1,
            Image,
            Video
        };

        enum class NotificationObjectType
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
            protected:

                /** It is used to define the type of notification object to be created. */
                NotificationObjectType m_Priority;

                /** It is used to define the type of notification message to be created. */
                NotificationMessageType m_Type;

                /** It is used to define the timestamp at which the notification was created. */
                std::string m_Time;

                /** It is used to define the sender of the notification. */
                std::vector<std::string> m_Sender;

                /** It is used to define a unique Id for the notification. */
                unsigned int m_Id;

                /** It is used to define the ttl of the notification after which it will expire. */
                int m_Ttl;



            public:

                /**
                 * Returns the priority of notification.
                 *
                 */
                NotificationObjectType getPriority() const;

                /**
                 * Returns the message type of notification.
                 *
                 */
                NotificationMessageType getType() const;

                /**
                 * Returns the generation time of notification.
                 *
                 */
                std::string getTime() const;

                /**
                 * Returns the sender details attached with notification.
                 *
                 */
                std::vector<std::string> getSender() const;

                /**
                 * Returns the unique id of notification.
                 *
                 */
                unsigned int getId() const;

                /**
                 * Returns the time to live of notification.
                 *
                 */
                int getTtl() const;



        };

        /**
         * This class is used to declare a Text notification object and define the corresponding
         * attributes.
         *
         * @see NotificationObject
         */
        class TextNotification : public NotificationObject
        {

            private:

                std::string m_TextMessage;

            public:


                typedef std::shared_ptr< TextNotification > Ptr;
                /**
                 * This is a builder to create resource with properties and attributes.
                 *
                 * The resource will be observable and discoverable by default, to make them disable
                 * set these properties explicitly with setDiscoverable and setObservable.
                 */
                class Builder
                {
                    public:
                        /**
                         * Constructs a Builder.
                         */
                        Builder();

                        Builder &setPriority(const NotificationObjectType &priority);
                        Builder &setType(const NotificationMessageType &type);

                        Builder &setTime(const std::string &time);
                        Builder &setSender(const std::vector<std::string> &senderDetails);


                        Builder &setId(const unsigned int &id);
                        Builder &setTtl(const int &ttl);

                        Builder &setMessage(const std::string &textMessage);

                        TextNotification::Ptr build();

                    private:
                        NotificationObjectType m_Priority;
                        NotificationMessageType m_Type;
                        std::string m_Time;
                        std::vector<std::string> m_Sender;
                        unsigned int m_Id;
                        int m_Ttl;
                        std::string m_TextMessage;
                };

                /**
                 * Returns the text message in notification.
                 *
                 */
                std::string getMessage() const;
        };

        /**
          * This class is used to declare a Image notification object and define the corresponding
          * attributes.
          *
          *@see NotificationObject
          */
        class ImageNotification : public NotificationObject
        {
                /**
                 * NotificationConsumer is made a friend class
                 *
                 */
                friend class NotificationConsumer;

            private:

                std::string m_ImageTitle;
                std::string m_ImageUrl;

            public:


                typedef std::shared_ptr< ImageNotification > Ptr;

                /**
                * This is a builder to create resource with properties and attributes.
                *
                * The resource will be observable and discoverable by default, to make them disable
                * set these properties explicitly with setDiscoverable and setObservable.
                */
                class Builder
                {
                    public:
                        /**
                         * Constructs a Builder.
                         *
                         * @param uri Resource uri
                         * @param type Resource type
                         * @param interface Resource interface
                         *
                         */
                        Builder();

                        Builder &setPriority(const NotificationObjectType &priority);
                        Builder &setType(const NotificationMessageType &type);

                        Builder &setTime(const std::string &time);
                        Builder &setSender(const std::vector<std::string> &senderDetails);


                        Builder &setId(const unsigned int &id);
                        Builder &setTtl(const int &ttl);

                        Builder &setTitle(const std::string &imageTitle);
                        Builder &setImageUrl(const std::string &imageUrl);

                        ImageNotification::Ptr build();

                    private:
                        NotificationObjectType m_Priority;
                        NotificationMessageType m_Type;
                        std::string m_Time;
                        std::vector<std::string> m_Sender;
                        unsigned int m_Id;
                        int m_Ttl;
                        std::string m_ImageTitle;
                        std::string m_ImageUrl;
                };

                /**
                 * Returns the text message of image.
                 *
                 */
                std::string getTitle() const;

                /**
                 * Returns the image icon url of image.
                 *
                 */
                std::string getImageUrl() const;
        };

        /**
         * This class is used to declare a video notification object and define the corresponding
         * attributes.
         *
         *@see NotificationObject
         */
        class VideoNotification : public NotificationObject
        {
                /**
                 * NotificationConsumer is made a friend class
                 *
                 */
                friend class NotificationConsumer;

            private:

                std::string m_VideoUrl;

            public:

                typedef std::shared_ptr< VideoNotification > Ptr;
                /**
                * This is a builder to create resource with properties and attributes.
                *
                * The resource will be observable and discoverable by default, to make them disable
                * set these properties explicitly with setDiscoverable and setObservable.
                */
                class Builder
                {
                    public:
                        /**
                         * Constructs a Builder.
                         *
                         * @param uri Resource uri
                         * @param type Resource type
                         * @param interface Resource interface
                         *
                         */
                        Builder();

                        Builder &setPriority(const NotificationObjectType &priority);
                        Builder &setType(const NotificationMessageType &type);

                        Builder &setTime(const std::string &time);
                        Builder &setSender(const std::vector<std::string> &senderDetails);


                        Builder &setId(const unsigned int &id);
                        Builder &setTtl(const int &ttl);

                        Builder &setVideoUrl(const std::string &videoUrl);

                        VideoNotification::Ptr build();

                    private:
                        NotificationObjectType m_Priority;
                        NotificationMessageType m_Type;
                        std::string m_Time;
                        std::vector<std::string> m_Sender;
                        unsigned int m_Id;
                        int m_Ttl;
                        std::string m_VideoUrl;
                };

                /**
                 * Returns the video url of notification.
                 *
                 */
                std::string getVideoUrl() const;
        };
    }
}

#endif  // NOTIFICATION_OBJECT_H