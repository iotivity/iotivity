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

#include "CMConfig.h"
#include "string.h"
#include "Serialization/ISerializable.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            /**
            *  @addtogroup Notification
            *  @{
            */

            /**
             * @class       NotificationResponse
             * @brief       This class handles notification response
             * @since       1.0
             */
            class CMAPI NotificationResponse
            {

                public:

                    /**
                     * Default constructor.
                     */
                    NotificationResponse();

                    /**
                     * Parameterized Constructor.
                     *
                     * @param[in]   resourcePath        Represents path of the resource where it is hosted
                     * @param[in]   notificationType    Represents the type of notification
                     * @param[in]   uuid                Represents the UUID of the device
                     * @param[in]   notificationData    Represents the ISerializable object of notification data
                     */
                    NotificationResponse(std::string resourcePath, std::string notificationType, std::string uuid,
                                         OC::Cm::Serialization::ISerializable *notificationData);

                    /**
                     * This method sets the resource path.
                     *
                     * @param[in]   resourcePath    Represents the path of the resource where it is hosted
                     */
                    void setResourcePath(std::string resourcePath);

                    /**
                     * This method sets the type of notification.
                     *
                     * @param[in] notificationType  Represents the type of notification.
                     */
                    void setNotificationType(std::string notificationType);

                    /**
                     * This method sets the ISerializable object of notification data
                     *
                     * @param[in] notificationData  Represents the ISerializable object of notification data
                     */
                    void setNotificationData(OC::Cm::Serialization::ISerializable *notificationData);

                    /**
                     * This method returns the resource path.
                     *
                     * @return Resource path.
                     */
                    std::string getResourcePath();

                    /**
                     * This method returns the notification type.
                     *
                     * @return notification type.
                     */
                    std::string getNotificationType();

                    /**
                     * This method returns the ISerializable object of notification data
                     *
                     * @return ISerializable object of notification data
                     */
                    OC::Cm::Serialization::ISerializable *getNotificationData();

                    /**
                     * This method returns the device UUID
                     *
                     * @return Device UUID
                     */
                    std::string getUuid();

                    /**
                     * This method sets the device UUID.
                     *
                     * @param[in]   uuid    Represents the device UUID.
                     */
                    void setUuid(const std::string uuid);

                private:

                    std::string resourcePath;   /**< Represents resource path */

                    std::string notificationType;   /**< Represents notification type */

                    std::string uuid;               /**< Represents device UUID */

                    OC::Cm::Serialization::ISerializable
                    *notificationData; /**< Represents ISerializable object of notification data */

            };
            /** @} */
        } /* namespace Notification */
    } /* namespace Cm */
} /* namespace OC */

#endif /* NOTIFICATIONRESPONSE_H_ */
