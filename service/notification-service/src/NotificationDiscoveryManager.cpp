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

#include <NotificationDiscoveryManager.h>

#include <RCSDiscoveryManager.h>
#include <RCSAddress.h>
#include <RCSRemoteResourceObject.h>
#include <RCSException.h>

#include <OCPlatform.h>
#include <logger.h>

#include <NotificationObject.h>
#include <NotificationConsumer.h>

constexpr char TAG[] {"NotificationDiscoveryManager"};

using namespace OC;

using namespace OIC::Service;

namespace
{
    void onResourceDiscovered(std::shared_ptr<RCSRemoteResourceObject>
                              foundResource, NotificationDiscoveryManager::ResourceDiscoveredCallback cb)
    {
        cb(std::make_shared<NotificationConsumer>(foundResource));
    }
}

namespace OIC
{
    namespace Service
    {
        NotificationDiscoveryManager *NotificationDiscoveryManager::getInstance()
        {
            static NotificationDiscoveryManager instance;

            PlatformConfig config
            {
                OC::ServiceType::InProc, ModeType::Client, "0.0.0.0", 0, OC::QualityOfService::HighQos
            };
            OCPlatform::Configure(config);

            return &instance;
        }

        void NotificationDiscoveryManager::discoverNotificationResource(
            const RCSAddress &address, ResourceDiscoveredCallback cb)
        {


            discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(RCSAddress::multicast(),
                            relativeUri,
                            resourceType,
                            std::bind(onResourceDiscovered, std::placeholders::_1, std::move(cb)));

        }
    }
}