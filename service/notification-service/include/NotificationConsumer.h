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
 * This file provides the Notification Consumer APIs to the developers.
 */

#ifndef NOTIFICATION_CONSUMER_H
#define NOTIFICATION_CONSUMER_H

#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"

#include "NotificationObject.h"

namespace OIC
{
    namespace Service
    {
        /**
         * This class is an interaction point between applications and Notification Manager Service
         * on client side.
         *
         * @see NotificationDiscoveryManager, NotificationObject
         */
        class NotificationConsumer
        {
            public:
                typedef std::function< void(std::string)> onGetDeviceName;

                typedef std::shared_ptr<NotificationConsumer> notificationConsumerPtr;
                /**
                 * typedef for callback of startSubscribing API
                 *
                 *@see RCSResourceAttributes
                 */
                typedef std::function<void(NotificationObject *)> SubscribedCallback;

                NotificationConsumer(std::shared_ptr<RCSRemoteResourceObject>);

                ~NotificationConsumer();

                /**
                 * Starts subscribing attributes for the resource.
                 *
                 * This will start subscribing for the resource.
                 * Once subscribing is started it will look for any attribute changes on the resource,
                 * and will provide notifications accordingly.
                 *
                 * @param cb If non-empty function, it will be invoked whenever the data is updated.
                 *
                 * @throws BadRequestException If subscribing is already started.
                 *
                 * @note The callback will be invoked in an internal thread.
                 *
                 * @see SubscribedCallback
                 */
                void subscribeNotifications(SubscribedCallback cb);

                /**
                 * UnSubscribe Notifications
                 *
                 * It does nothing if subscribing is not started.
                 *
                 * @see subscribeNotifications(SubscribedCallback)
                 */
                void unSubscribeNotifications();
                /**
                  * @return Returns the uri of the resource.
                  *
                  */
                std::string getUri() const;

                /**
                  * @return Returns the address of the resource .
                  *
                  */
                std::string getAddress() const;

                void getDeviceName(onGetDeviceName cb);

                void sendNotificationAcknowledgement(int notificationId);

            private:


                std::shared_ptr<RCSRemoteResourceObject> m_rcsRemoteResource;

        };
    }
}

#endif // NOTIFICATION_CONSUMER_H
