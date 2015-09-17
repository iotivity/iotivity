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
 * This file contains the RCSActiveDiscoveryManager class which provide APIs to discover the Resource in the network
 *
 */

#ifndef RCSDISCOVERYMANAGER_IMPL_H
#define RCSDISCOVERYMANAGER_IMPL_H

#include <memory>
#include <functional>
#include <list>
#include <mutex>
#include <unordered_map>
#include <algorithm>

#include "octypes.h"

#include "RCSDiscoveryManager.h"
#include "RCSAddress.h"
#include "ExpiryTimer.h"
#include "PrimitiveResource.h"
#include "RCSRemoteResourceObject.h"

namespace OIC
{
    namespace Service
    {
        class RCSDiscoveryManager;
        class PrimitiveResource;
        class RCSAddress;
        class RCSDiscoveryTask;
        class DiscoverRequestInfo
        {
            public:

                std::string m_address;
                std::string m_relativeUri;
                std::string m_resourceType;
                std::list<std::string> m_receivedIds;
                bool m_isReceivedFindCallback;
                DiscoverCallback m_findCB;
                RCSDiscoveryManager::ResourceDiscoveredCallback m_discoverCB;
        };

        class RCSDiscoveryManagerImpl
        {
            static unsigned int s_uniqueId;

            public:

                typedef std::function<void(OCStackResult, const unsigned int,
                        const std::string&)> PresenceCallback;
                typedef unsigned int ID;
                typedef std::function<void(std::shared_ptr< PrimitiveResource >, ID)> FindCallback;

            public:

                static RCSDiscoveryManagerImpl* getInstance();

                DiscoverRequestInfo m_discoveryItem;
                std::unordered_map<ID,DiscoverRequestInfo> m_discoveryMap;
                PresenceCallback m_presenceCB;
                ExpiryTimer::Callback m_pollingCB;
                ExpiryTimer m_timer;
                ID m_timerHandle;

            private:
                static RCSDiscoveryManagerImpl * s_instance;
                static std::mutex s_mutexForCreation;
                std::mutex m_mutex;

            public:

                std::unique_ptr<RCSDiscoveryTask> startDiscovery(const RCSAddress& address,
                        const std::string& relativeURI,const std::string& resourceType,
                        RCSDiscoveryManager::ResourceDiscoveredCallback cb);

            private:

                void requestMulticastPresence();
                void initializedDiscoveryEnvironment();
                void findCallback(std::shared_ptr< PrimitiveResource > resource, ID discoverID);
                void pollingCallback(unsigned int /*msg*/);
                void presenceCallback(OCStackResult, const unsigned int,const std::string&);
                bool isDuplicatedCallback(std::shared_ptr<PrimitiveResource> resource,ID discoverID);
                ID createId();

            private:

                RCSDiscoveryManagerImpl() = default;;
                ~RCSDiscoveryManagerImpl() = default;
        };
    }
}
#endif // RCSDISCOVERYMANAGER_IMPL_H
