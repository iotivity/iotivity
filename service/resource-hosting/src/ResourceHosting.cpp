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

#include "ResourceHosting.h"

#include "RCSDiscoveryManager.h"
#include "RCSAddress.h"

namespace OIC
{
    namespace Service
    {

        namespace
        {
            const std::string HOSTING_TAG = "/hosting";
            const auto HOSTING_TAG_SIZE = HOSTING_TAG.size();
            const std::string HOSTING_RESOURSE_TYPE = "oic.r.resourcehosting";
        }

        ResourceHosting::ResourceHosting()
        : m_mutexForList(),
          m_isStartedHosting(false),
          m_hostingObjects(),
          m_discoveryTask()
        {
        }

        ResourceHosting * ResourceHosting::getInstance()
        {
            static ResourceHosting instance;
            return & instance;
        }

        void ResourceHosting::startHosting()
        {
            if (m_isStartedHosting) return;
            m_isStartedHosting = true;
            createDiscoveryListener();
        }

        void ResourceHosting::stopHosting()
        {
            if (!m_isStartedHosting) return;

            if (!m_discoveryTask->isCanceled())
            {
                m_discoveryTask->cancel();
            }

            m_isStartedHosting = false;

            RHLock lock(m_mutexForList);
            m_hostingObjects.clear();
        }

        void ResourceHosting::createDiscoveryListener()
        {
            m_discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(
                    RCSAddress::multicast(), OC_RSRVD_WELL_KNOWN_URI, HOSTING_RESOURSE_TYPE,
                    std::bind(&ResourceHosting::discoveryHandler, this,
                                std::placeholders::_1));
        }

        void ResourceHosting::discoveryHandler(RemoteObjectPtr remoteResource)
        {
            auto discoverdUri = remoteResource->getUri();
            if (discoverdUri.compare(
                    discoverdUri.size()-HOSTING_TAG_SIZE, HOSTING_TAG_SIZE, HOSTING_TAG) != 0)
            {
                return;
            }

            auto foundHostingObject = findRemoteResource(remoteResource);
            if (foundHostingObject != nullptr) return;

            try
            {
                HostingObjectKey key = generateHostingObjectKey(remoteResource);
                foundHostingObject = HostingObject::createHostingObject(remoteResource,
                        std::bind(&ResourceHosting::destroyedHostingObject, this, key));

                RHLock lock(m_mutexForList);
                m_hostingObjects.insert(std::make_pair(key, foundHostingObject));

            } catch (const RCSException & e)
            {
                OIC_HOSTING_LOG(DEBUG,
                        "[ResourceHosting::discoverHandler]InvalidParameterException:%s", e.what());
            }
        }

        HostingObject::Ptr ResourceHosting::findRemoteResource(RemoteObjectPtr remoteResource)
        {
            RHLock lock(m_mutexForList);

            auto iter = m_hostingObjects.find(generateHostingObjectKey(remoteResource));
            if (iter != m_hostingObjects.end()) return iter->second;

            return nullptr;
        }

        void ResourceHosting::destroyedHostingObject(const HostingObjectKey & key)
        {
            RHLock lock(m_mutexForList);
            m_hostingObjects.erase(key);
        }

        ResourceHosting::HostingObjectKey ResourceHosting::generateHostingObjectKey(
                std::string && address, std::string && uri)
        {
            return HostingObjectKey(address + uri);
        }

        ResourceHosting::HostingObjectKey ResourceHosting::generateHostingObjectKey(
                const std::string & address, const std::string & uri)
        {
            return generateHostingObjectKey(std::string(address), std::string(uri));
        }
        ResourceHosting::HostingObjectKey ResourceHosting::generateHostingObjectKey(
                RemoteObjectPtr rResource)
        {
            return generateHostingObjectKey(rResource->getAddress(), rResource->getUri());
        }

    } /* namespace Service */
} /* namespace OIC */
