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
        NotificationObject::NotificationObject()
        {
            time_t now = time(0);
            char *dt = ctime(&now);
            m_NotificationTime = dt;
            srand(time(NULL));
            m_NotificationId = rand();
        }

        void TextNotification::setTextAttributes(const std::string &notificationMessage,
                NotificationObjectType &notificationObjectType,
                NotificationMessageType &notificationMessageType,
                const std::string &notificationTime,
                const std::string &notificationSender,
                unsigned int notificationId,
                int notificationTtl)
        {
            m_NotificationMessage = notificationMessage;
            m_NotificationObjectType = notificationObjectType;
            m_NotificationMessageType = notificationMessageType;
            m_NotificationTime = notificationTime;
            m_NotificationSender = notificationSender;
            m_NotificationId = notificationId;
            m_NotificationTtl = notificationTtl;
        }

        void ImageNotification::setImageAttributes(const std::string &notificationIconUrl,
                const std::string notificationMessage,
                NotificationObjectType &notificationObjectType,
                NotificationMessageType &notificationMessageType,
                const std::string &notificationTime,
                const std::string &notificationSender,
                unsigned int notificationId,
                int notificationTtl)
        {
            m_NotificationIconUrl = notificationIconUrl;
            m_NotificationMessage = notificationMessage;
            m_NotificationObjectType = notificationObjectType;
            m_NotificationMessageType = notificationMessageType;
            m_NotificationTime = notificationTime;
            m_NotificationSender = notificationSender;
            m_NotificationId = notificationId;
            m_NotificationTtl = notificationTtl;
        }

        void VideoNotification::setVideoAttributes(const std::string &notificationVideoUrl,
                NotificationObjectType &notificationObjectType,
                NotificationMessageType &notificationMessageType,
                const std::string &notificationTime,
                const std::string &notificationSender,
                unsigned int notificationId,
                int notificationTtl)
        {
            m_NotificationVideoUrl = notificationVideoUrl;
            m_NotificationObjectType = notificationObjectType;
            m_NotificationMessageType = notificationMessageType;
            m_NotificationTime = notificationTime;
            m_NotificationSender = notificationSender;
            m_NotificationId = notificationId;
            m_NotificationTtl = notificationTtl;
        }
    }
}