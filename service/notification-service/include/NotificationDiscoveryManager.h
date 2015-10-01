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
 * This file provides the Notification Discovery Manager APIs to the developers.
 */

#ifndef NOTIFICATION_DISCOVERYMANAGER_H
#define NOTIFICATION_DISCOVERYMANAGER_H

#include "RCSRemoteResourceObject.h"
#include "RCSDiscoveryManager.h"
#include "RCSAddress.h"

#include "NotificationObject.h"
#include "NotificationConsumer.h"

namespace OIC
{
    namespace Service
    {
        const std::string relativeUri = OC_RSRVD_WELL_KNOWN_URI;
        const std::string resourceType = "oic.r.notify";

        class RCSAddress;

        /**
         * This class contains the notification resource discovery methods.
         *
         *@see NotificationObject.
         */
        class NotificationDiscoveryManager
        {
            public:

                /**
                  *Typedef for callback of discoverResource APIs.
                  *
                  *@see discoverNotificationResource.
                  */
                typedef std::function< void(std::shared_ptr<NotificationConsumer>)>
                ResourceDiscoveredCallback;


                /**
                 *@return Returns NotificationDiscoveryManager instance.
                 */
                static  NotificationDiscoveryManager *getInstance();

                /**
                 * API for discovering the notification resource.
                 *
                 *@param address A RCSAddress object.
                 *@param cb A callback to obtain discovered resource.
                 *
                 *@note The callback will be invoked in an internal thread.
                 */
                void discoverNotificationResource(const RCSAddress& address,
                                            ResourceDiscoveredCallback cb);

            private:

                NotificationDiscoveryManager() = default;
                ~NotificationDiscoveryManager() = default;

                std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;


        };
    }
}

#endif  // NOTIFICATION_DISCOVERYMANAGER_H