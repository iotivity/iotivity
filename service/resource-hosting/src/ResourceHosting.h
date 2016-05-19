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

#ifndef RH_RESOURCEHOSTING_H_
#define RH_RESOURCEHOSTING_H_

#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <mutex>

#include "RCSDiscoveryManager.h"
#include "RCSRemoteResourceObject.h"
#include "HostingObject.h"

namespace OIC
{
    namespace Service
    {

        class ResourceHosting
        {
        private:
            typedef RCSRemoteResourceObject::Ptr RemoteObjectPtr;
            typedef std::lock_guard<std::mutex> RHLock;
            typedef std::string HostingObjectKey;

            typedef std::function<void(RemoteObjectPtr)> DiscoveryCallback;
            typedef HostingObject::DestroyedCallback DestroyedCallback;

        public:
            void startHosting();
            void stopHosting();

            static ResourceHosting * getInstance();

        private:
            ResourceHosting();
            ~ResourceHosting() = default;

            ResourceHosting(const ResourceHosting&) = delete;
            ResourceHosting & operator = (const ResourceHosting &) = delete;

            ResourceHosting(ResourceHosting &&) = delete;
            ResourceHosting & operator = (ResourceHosting &&) = delete;

            std::mutex m_mutexForList;
            std::atomic_bool m_isStartedHosting;

            std::unordered_map<HostingObjectKey, HostingObject::Ptr> m_hostingObjects;
            RCSDiscoveryManager::DiscoveryTask::Ptr m_discoveryTask;

            void createDiscoveryListener();
            void discoveryHandler(RemoteObjectPtr remoteResource);

            HostingObjectKey generateHostingObjectKey(RemoteObjectPtr rResource);
            HostingObjectKey generateHostingObjectKey(std::string && address, std::string && uri);
            HostingObjectKey generateHostingObjectKey(
                    const std::string & address, const std::string & uri);

            HostingObject::Ptr findRemoteResource(RemoteObjectPtr remoteResource);

            void destroyedHostingObject(const HostingObjectKey & key);
        };

    } /* namespace Service */
} /* namespace OIC */

#endif /* RH_RESOURCEHOSTING_H_ */
