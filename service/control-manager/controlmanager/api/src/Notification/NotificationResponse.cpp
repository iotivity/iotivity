//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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



#include "Notification/NotificationResponse.h"


namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            NotificationResponse::NotificationResponse()
            {
                this->resourcePath = "";
                this->notificationType = "";
                this->notificationData = NULL;
                this->uuid = "";
            }

            NotificationResponse::NotificationResponse(std::string resourcePath, std::string notificationType,
                    std::string uuid, OC::Cm::Serialization::ISerializable *notificationData)
            {
                this->resourcePath = resourcePath;
                this->notificationType = notificationType;
                this->notificationData = notificationData;
                this->uuid = uuid;
            }


            void
            NotificationResponse::setResourcePath(std::string resourcePath)
            {
                this->resourcePath = resourcePath;
            }

            void
            NotificationResponse::setNotificationType(std::string notificationType)
            {
                this->notificationType = notificationType;
            }

            void
            NotificationResponse::setNotificationData(OC::Cm::Serialization::ISerializable *notificationData)
            {
                this->notificationData = notificationData;
            }

            std::string
            NotificationResponse::getResourcePath()
            {
                return this->resourcePath;
            }

            std::string
            NotificationResponse::getNotificationType()
            {
                return this->notificationType;
            }

            OC::Cm::Serialization::ISerializable *
            NotificationResponse::getNotificationData()
            {
                return this->notificationData;
            }

            std::string
            NotificationResponse::getUuid()
            {
                return uuid;
            }

            void
            NotificationResponse::setUuid(std::string uuid)
            {
                this->uuid = uuid;
            }

        } /* namespace Notification */
    } /* namespace Cm */
} /* namespace OC */
