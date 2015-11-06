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

#include <ctime>
#include <limits.h>

#include <NotificationObject.h>

namespace OIC
{
    namespace Service
    {
        static int sNotificationId = 0;

        NotificationObject::NotificationObject()
        {
            time_t now = time(0);
            char *dt = ctime(&now);
            mNotificationTime = dt;
            if (mNotificationId < INT_MAX)
            {
                mNotificationId = ++sNotificationId;
            }
            else
            {
                mNotificationId = 0;
            }
        }

        void TextNotification::setTextAttributes(const std::string &nNotificationMessage,
                NotificationObjectType &nNotificationObjectType,
                const std::string &nNotificationTime,
                const std::string &nNotificationSender,
                int nNotificationId,
                int nNotificationTtl)
        {
            mNotificationMessage = nNotificationMessage;
            mNotificationObjectType = nNotificationObjectType;
            mNotificationTime = nNotificationTime;
            mNotificationSender = nNotificationSender;
            mNotificationId = nNotificationId;
            mNotificationTtl = nNotificationTtl;
        }

        void ImageNotification::setImageAttributes(const std::string &nNotificationIconUrl,
                const std::string nNotificationMessage,
                NotificationObjectType &nNotificationObjectType,
                const std::string &nNotificationTime,
                const std::string &nNotificationSender,
                int nNotificationId,
                int nNotificationTtl)
        {
            mNotificationIconUrl = nNotificationIconUrl;
            mNotificationMessage = nNotificationMessage;
            mNotificationObjectType = nNotificationObjectType;
            mNotificationTime = nNotificationTime;
            mNotificationSender = nNotificationSender;
            mNotificationId = nNotificationId;
            mNotificationTtl = nNotificationTtl;
        }

        void VideoNotification::setVideoAttributes(const std::string &nNotificationVideoUrl,
                NotificationObjectType &nNotificationObjectType,
                const std::string &nNotificationTime,
                const std::string &nNotificationSender,
                int nNotificationId,
                int nNotificationTtl)
        {
            mNotificationVideoUrl = nNotificationVideoUrl;
            mNotificationObjectType = nNotificationObjectType;
            mNotificationTime = nNotificationTime;
            mNotificationSender = nNotificationSender;
            mNotificationId = nNotificationId;
            mNotificationTtl = nNotificationTtl;
        }
    }
}