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
#include "RCSDiscoveryManagerImpl.h"

#include "OCPlatform.h"

#include "PresenceSubscriber.h"
#include "RCSAddressDetail.h"
#include "RCSAddress.h"

constexpr unsigned int RESETNUMBER = 0;
constexpr unsigned int LIMITNUMBER = 1000;
constexpr unsigned int INTERVALTIME = 60000;

namespace OIC
{
    namespace Service
    {
        unsigned int RCSDiscoveryManagerImpl::s_uniqueId = RESETNUMBER;
        RCSDiscoveryManagerImpl * RCSDiscoveryManagerImpl::s_instance(nullptr);
        std::mutex RCSDiscoveryManagerImpl::s_mutexForCreation;

        RCSDiscoveryManagerImpl::RCSDiscoveryManagerImpl() : m_timerHandle(0){}

        RCSDiscoveryManagerImpl* RCSDiscoveryManagerImpl::getInstance()
        {
            if (!s_instance)
            {
                s_mutexForCreation.lock();
                if (!s_instance)
                {
                    s_instance = new RCSDiscoveryManagerImpl();
                    srand(time(NULL));
                    s_instance->initializedDiscoveryEnvironment();
                    s_instance->requestMulticastPresence();
                    s_instance->m_timerHandle = s_instance->m_timer.post(INTERVALTIME, s_instance->m_pollingCB);
                }
                s_mutexForCreation.unlock();
            }
            return s_instance;
        }

        void RCSDiscoveryManagerImpl::findCallback(std::shared_ptr< PrimitiveResource > resource,
            RCSDiscoveryManagerImpl::ID discoverID)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

           if(!isDuplicatedCallback(resource,discoverID))
            {
               for(auto it = m_discoveryMap.begin(); it != m_discoveryMap.end(); ++it)
               {
                   if(it->first == discoverID)
                   {
                        it->second.m_isReceivedFindCallback = true;
                        it->second.m_discoverCB(std::make_shared<RCSRemoteResourceObject>(resource));
                   }
                }
            }
        }

        std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> RCSDiscoveryManagerImpl::startDiscovery
        (const RCSAddress& address, const std::string& relativeURI, const std::string& resourceType,
                RCSDiscoveryManager::ResourceDiscoveredCallback cb)
        {
            if (!cb)
            {
                throw RCSInvalidParameterException { "input Parameter(callback) is NULL" };
            }

            DiscoverRequestInfo discoveryItem;
            discoveryItem.m_address = RCSAddressDetail::getDetail(address)->getAddress();
            discoveryItem.m_relativeUri = relativeURI;
            discoveryItem.m_resourceType = resourceType;
            discoveryItem.m_discoverCB = std::move(cb);
            discoveryItem.m_isReceivedFindCallback = false;

            ID discoverID = createId();
            discoveryItem.m_findCB = std::bind(&RCSDiscoveryManagerImpl::findCallback, this,
                    std::placeholders::_1, discoverID);
            m_discoveryMap.insert(std::make_pair(discoverID, discoveryItem));

            OIC::Service::discoverResource(RCSAddressDetail::getDetail(RCSAddress::multicast())->getAddress(),
                    discoveryItem.m_relativeUri + "?rt=" +discoveryItem.m_resourceType,
                    OCConnectivityType::CT_DEFAULT, discoveryItem.m_findCB);

            return std::unique_ptr<RCSDiscoveryManager::DiscoveryTask>(
                    new RCSDiscoveryManager::DiscoveryTask(discoverID));
        }

        void RCSDiscoveryManagerImpl::initializedDiscoveryEnvironment()
        {
            m_presenceCB = std::bind(&RCSDiscoveryManagerImpl::presenceCallback, this,
                    std::placeholders::_1, std::placeholders::_2,std::placeholders::_3);
            m_pollingCB = std::bind(&RCSDiscoveryManagerImpl::pollingCallback, this,
                std::placeholders::_1);
        }

        void RCSDiscoveryManagerImpl::requestMulticastPresence()
        {
            static constexpr char MULTICAST_PRESENCE_ADDRESS[] = "coap://" OC_MULTICAST_PREFIX;
            OCDoHandle presenceHandle;
            subscribePresence(presenceHandle, MULTICAST_PRESENCE_ADDRESS,
                            OCConnectivityType::CT_DEFAULT, std::move(m_presenceCB));
        }

        bool RCSDiscoveryManagerImpl::isDuplicatedCallback(std::shared_ptr< PrimitiveResource > resource,
                ID discoverID)
        {
            std::string retID = resource->getSid()+resource->getUri();
            auto it = m_discoveryMap.find(discoverID);
            std::list<std::string>::iterator itor;
            if(it==m_discoveryMap.end())
            {
                return false;
            }
            itor = std::find(it->second.m_receivedIds.begin(),it->second.m_receivedIds.end(),retID);
            if(itor != it->second.m_receivedIds.end())
            {
              return true;
            }
            it->second.m_receivedIds.push_back(retID);

            return false;
        }

        void RCSDiscoveryManagerImpl::pollingCallback(unsigned int /*msg*/)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for(auto it = m_discoveryMap.begin(); it != m_discoveryMap.end(); ++it)
            {
                OIC::Service::discoverResource(it->second.m_address,it->second.m_relativeUri+ "?rt="
                        +it->second.m_resourceType, OCConnectivityType::CT_DEFAULT, it->second.m_findCB);
            }
            m_timerHandle = m_timer.post(INTERVALTIME, m_pollingCB);
        }

        void RCSDiscoveryManagerImpl::presenceCallback(OCStackResult ret,
                const unsigned int /*seq*/, const std::string& /*address*/)
        {
            if(ret == OC_STACK_OK || ret == OC_STACK_RESOURCE_CREATED || ret == OC_STACK_RESOURCE_DELETED)
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                for(auto it = m_discoveryMap.begin(); it != m_discoveryMap.end(); ++it)
                {
                    if(!it->second.m_isReceivedFindCallback)
                    {
                        OIC::Service::discoverResource(it->second.m_address, it->second.m_relativeUri+ "?rt=" +
                            it->second.m_resourceType, OCConnectivityType::CT_DEFAULT, it->second.m_findCB);
                    }
                }
            }
        }

        RCSDiscoveryManagerImpl::ID RCSDiscoveryManagerImpl::createId()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if(s_uniqueId<LIMITNUMBER)
            {
                 s_uniqueId++;
            }
            else
            {
                s_uniqueId = RESETNUMBER;
            }
            while(m_discoveryMap.size() != LIMITNUMBER)
            {
                if(m_discoveryMap.find(s_uniqueId) == m_discoveryMap.end())
                {
                    return s_uniqueId;
                }
                s_uniqueId++;
            }

            return RESETNUMBER;
        }

        void RCSDiscoveryManagerImpl::cancel(ID id)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_discoveryMap.erase(id);
        }
    }
}
