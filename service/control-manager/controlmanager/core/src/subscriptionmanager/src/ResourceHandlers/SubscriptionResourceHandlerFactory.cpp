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



#include "SubscriptionResourceHandlerFactory.h"
#include "xsd/Elements.h"

#include "ResourceHandlers.h"
#include "ResourceTypeEnum.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            SubscriptionResourceHandlerFactory::SubscriptionResourceHandlerFactory()
            {
                registerResourceHandler(OC::Cm::Notification::RT_Notification, "/notifications/{notificationId}");
                registerResourceHandler(OC::Cm::Notification::RT_Notifications, "/notifications");
                registerResourceHandler(OC::Cm::Notification::RT_Subscription, "/subscriptions/{subscriptionId}");
                registerResourceHandler(OC::Cm::Notification::RT_Subscriptions, "/subscriptions");
            }

            SubscriptionResourceHandlerFactory::~SubscriptionResourceHandlerFactory()
            {

            }

            OC::Cm::Server::ResourceHandler::IResourceHandler *
            SubscriptionResourceHandlerFactory::createResourceHandler(int resourceType)
            {
                switch (resourceType)
                {
                    case OC::Cm::Notification::RT_Notification: { return new OC::Cm::Notification::NotificationResourceHandler(p_context) ;}
                    case OC::Cm::Notification::RT_Notifications: { return new OC::Cm::Notification::NotificationsResourceHandler(p_context) ;}
                    case OC::Cm::Notification::RT_Subscription: { return new OC::Cm::Notification::SubscriptionResourceHandler(p_context) ;}
                    case OC::Cm::Notification::RT_Subscriptions: { return new OC::Cm::Notification::SubscriptionsResourceHandler(p_context) ;}
                }

                return NULL ;
            }

        }
    }
}
