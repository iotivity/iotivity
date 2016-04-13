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
#include <NotificationObject.h>

#include <stdlib.h>
#include <ctime>
#include <limits.h>

namespace OIC
{
    namespace Service
    {
        TextNotification::Builder::Builder() {}

        TextNotification::Ptr TextNotification::Builder::build()
        {


            TextNotification::Ptr server = std::make_shared<TextNotification>();

            server->m_Priority = m_Priority;
            server->m_Type = m_Type;
            server->m_Time = m_Time;
            server->m_Id = m_Id;
            server->m_Ttl = m_Ttl;
            server->m_Sender = m_Sender;
            server->m_TextMessage = m_TextMessage;

            return server;
        }

        TextNotification::Builder &TextNotification::Builder::setMessage(const std::string &textMessage)
        {
            m_TextMessage = textMessage;
            return *this;
        }

        TextNotification::Builder &TextNotification::Builder::setPriority(const NotificationObjectType
                &priority)
        {
            m_Priority = priority;
            return *this;
        }

        TextNotification::Builder &TextNotification::Builder::setType(const NotificationMessageType &type)
        {
            m_Type = type;
            return *this;
        }

        TextNotification::Builder &TextNotification::Builder::setTime(const std::string &time)
        {
            m_Time = time;
            return *this;
        }

        TextNotification::Builder &TextNotification::Builder::setSender(const std::vector<std::string>
                &senderDetails)
        {
            m_Sender = senderDetails;
            return *this;
        }

        TextNotification::Builder &TextNotification::Builder::setId(const unsigned int &id)
        {
            m_Id = id;
            return *this;
        }

        TextNotification::Builder &TextNotification::Builder::setTtl(const int &ttl)
        {
            m_Ttl = ttl;
            return *this;
        }


        ImageNotification::Builder::Builder() {}

        ImageNotification::Ptr ImageNotification::Builder::build()
        {

            ImageNotification::Ptr server = std::make_shared<ImageNotification>();

            server->m_Priority = m_Priority;
            server->m_Type = m_Type;
            server->m_Time = m_Time;
            server->m_Id = m_Id;
            server->m_Ttl = m_Ttl;
            server->m_Sender = m_Sender;
            server->m_ImageTitle = m_ImageTitle;
            server->m_ImageUrl = m_ImageUrl;

            return server;
        }

        ImageNotification::Builder &ImageNotification::Builder::setTitle(const std::string &imageTitle)
        {
            m_ImageTitle = imageTitle;
            return *this;
        }

        ImageNotification::Builder &ImageNotification::Builder::setImageUrl(const std::string &imageUrl)
        {
            m_ImageUrl = imageUrl;
            return *this;
        }

        ImageNotification::Builder &ImageNotification::Builder::setPriority(const NotificationObjectType
                &priority)
        {
            m_Priority = priority;
            return *this;
        }

        ImageNotification::Builder &ImageNotification::Builder::setType(const NotificationMessageType &type)
        {
            m_Type = type;
            return *this;
        }

        ImageNotification::Builder &ImageNotification::Builder::setTime(const std::string &time)
        {
            m_Time = time;
            return *this;
        }

        ImageNotification::Builder &ImageNotification::Builder::setSender(const std::vector<std::string>
                &senderDetails)
        {
            m_Sender = senderDetails;
            return *this;
        }

        ImageNotification::Builder &ImageNotification::Builder::setId(const unsigned int &id)
        {
            m_Id = id;
            return *this;
        }

        ImageNotification::Builder &ImageNotification::Builder::setTtl(const int &ttl)
        {
            m_Ttl = ttl;
            return *this;
        }


        VideoNotification::Builder::Builder() {}

        VideoNotification::Ptr VideoNotification::Builder::build()
        {

            VideoNotification::Ptr server  = std::make_shared<VideoNotification>();

            server->m_Priority = m_Priority;
            server->m_Type = m_Type;
            server->m_Time = m_Time;
            server->m_Id = m_Id;
            server->m_Ttl = m_Ttl;
            server->m_Sender = m_Sender;
            server->m_VideoUrl = m_VideoUrl;

            return server;
        }

        VideoNotification::Builder &VideoNotification::Builder::setVideoUrl(const std::string &videoUrl)
        {
            m_VideoUrl = videoUrl;
            return *this;
        }

        VideoNotification::Builder &VideoNotification::Builder::setPriority(const NotificationObjectType
                &priority)
        {
            m_Priority = priority;
            return *this;
        }

        VideoNotification::Builder &VideoNotification::Builder::setType(const NotificationMessageType &type)
        {
            m_Type = type;
            return *this;
        }

        VideoNotification::Builder &VideoNotification::Builder::setTime(const std::string &time)
        {
            m_Time = time;
            return *this;
        }

        VideoNotification::Builder &VideoNotification::Builder::setSender(const std::vector<std::string>
                &senderDetails)
        {
            m_Sender = senderDetails;
            return *this;
        }

        VideoNotification::Builder &VideoNotification::Builder::setId(const unsigned int &id)
        {
            m_Id = id;
            return *this;
        }

        VideoNotification::Builder &VideoNotification::Builder::setTtl(const int &ttl)
        {
            m_Ttl = ttl;
            return *this;
        }



        NotificationObjectType NotificationObject::getPriority() const
        {
            return m_Priority;
        }

        NotificationMessageType NotificationObject::getType() const
        {
            return m_Type;
        }

        std::string NotificationObject::getTime() const
        {
            return m_Time;
        }

        std::vector<std::string> NotificationObject::getSender() const
        {
            return m_Sender;
        }

        unsigned int NotificationObject::getId() const
        {
            return m_Id;
        }

        int NotificationObject::getTtl() const
        {
            return m_Ttl;
        }

        std::string TextNotification::getMessage() const
        {
            return m_TextMessage;
        }

        std::string ImageNotification::getTitle() const
        {
            return m_ImageTitle;
        }

        std::string ImageNotification::getImageUrl() const
        {
            return m_ImageUrl;
        }

        std::string VideoNotification::getVideoUrl() const
        {
            return m_VideoUrl;
        }

    }
}