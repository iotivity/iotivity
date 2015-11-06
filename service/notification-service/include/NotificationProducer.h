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
 * This file provides Notification Producer APIs to the developers.
 */

#ifndef NOTIFICATION_PRODUCER_H
#define NOTIFICATION_PRODUCER_H

#include "RCSResourceAttributes.h"
#include "RCSResourceObject.h"

#include "NotificationObject.h"

#define TimeStamp "TIME"
#define Sender "SENDER"
#define Ttl "TTL"
#define Id "ID"
#define Message "MESSAGE"
#define IconUrl "ICONURL"
#define VideoUrl "VIDEOURL"
#define ObjectType "OBJECTTYPE"
#define notification_payload "notification-payload"
#define DeviceName "DeviceName"
#define notification_id "notificationId"
#define hostAddress "hostAddress"
#define notification_ack "notificationAck"

namespace OIC
{
    namespace Service
    {
        /**
         * This class is used to initalise the NotificationManager
         * and send the desired Notification Object.
         *
         *@see startNotificationManager()
         *@see sendNotification()
         */
        class NotificationProducer
        {
            private:
                std::string m_uri;
                std::string m_type;
                std::string m_interface;
                RCSResourceObject::Ptr mNotificationResource;

            public:

                typedef std::shared_ptr<NotificationProducer> NotificationProducerPtr;

                typedef std::function < void(int, std::string) > notificationIdListener;

                /**
                 * @brief Constructor.
                 *  Sets the resource property values using initializers list.
                 *
                 * @param uri Resource URI value to be set.
                 * @param type Resource type value to be set.
                 * @param interface Interface value to be set.
                 */
                NotificationProducer(const std::string& n_uri, const std::string& n_type,
                                     const std::string& n_interface);

                /**
                 * API to start the Notification Manager.
                 *
                 * This API creates a new Notification Resource.
                 *
                 *@return Pointer to NotificationProvider instance created.
                 *@throw PlatformException
                 *
                 * It catches exception from registerResource API of OCPlatform and throws it to
                 * the caller.
                 */

                NotificationProducerPtr startNotificationManager(std::string& notifyDeviceName,
                                                                           NotificationProducer::notificationIdListener cb);

                /**
                 * API to stops the Notification Manager.
                 *
                 * This API destroys the Notification Resource created.
                 */
                void stopNotificationManager();

                /**
                 * API is used to send the Notification Object created.
                 *
                 * @param object NotificationObject containing attributes to be notified.
                 */
                void sendNotification(NotificationObjectType &type,
                                           NotificationObject *notificationObjectPtr);
        };
    }
}

#endif  // NOTIFICATION_PROVIDER_H