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

#ifndef NOTIFICATIONRESPONSE_H_
#define NOTIFICATIONRESPONSE_H_

#include "string.h"
#include "ISerializable.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            /**
             * @class       NotificationResponse
             * @brief       This class handles notification response
             */
            class NotificationResponse
            {

                public:

                    /**
                     *NotificationResponse - Default constructor.
                     */
                    NotificationResponse();

                    /**
                     * NotificationResponse - Parameterized Constructor.
                     *
                     * @param   resourcePath        Represents path of the resource where it is hosted
                     * @param   notificationType    Represents the type of notification
                     * @param   uuid                Represents the UUID of the device
                     * @param   notificationData    Represents the ISerializable object of notification data
                     * @return -none
                     */
                    NotificationResponse(std::string resourcePath, std::string notificationType, std::string uuid, OC::Cm::Serialization::ISerializable *notificationData);

                    /**
                     * setResourcePath - This method sets the resource path.
                     *
                     * @param   resourcePath    Represents the path of the resource where it is hosted
                     * @return -none
                     */
                    void setResourcePath(std::string resourcePath);

                    /**
                     * setNotificationType - This method sets the type of notification.
                     *
                     * @param notificationType  Represents the type of notification.
                     * @return -none
                     */
                    void setNotificationType(std::string notificationType);

                    /**
                     * setNotificationData - This method sets the ISerializable object of notification data
                     *
                     * @param notificationData  Represents the ISerializable object of notification data
                     * @return -none
                     */
                    void setNotificationData(OC::Cm::Serialization::ISerializable *notificationData);

                    /**
                     * getResourcePath - This method returns the resource path.
                     *
                     * @return type:string Resource path.
                     */
                    std::string getResourcePath();

                    /**
                     * getNotificationType -This method returns the notification type.
                     * @param -none
                     *
                     * @return type:string notification type.
                     */
                    std::string getNotificationType();

                    /**
                     * getNotificationData - This method returns the ISerializable object of notification data
                     *@param -none
                     * @return type:ISerializable* ISerializable object of notification data
                     */
                    OC::Cm::Serialization::ISerializable *getNotificationData();

                    /**
                     * getUuid - This method returns the device UUID
                     * @param -none
                     * @return type:string Device UUID
                     */
                    std::string getUuid();

                    /**
                     * setUuid - This method sets the device UUID.
                     *
                     * @param   uuid    Represents the device UUID.
                     * @return -none
                     */
                    void setUuid(const std::string uuid);

                private:

                    std::string resourcePath;   /**< Represents resource path */
                    std::string notificationType;   /**< Represents notification type */
                    std::string uuid;               /**< Represents device UUID */
                    OC::Cm::Serialization::ISerializable *notificationData; /**< Represents ISerializable object of notification data */

            };

        } /* namespace Notification */
    } /* namespace Cm */
} /* namespace OC */

#endif /* NOTIFICATIONRESPONSE_H_ */
