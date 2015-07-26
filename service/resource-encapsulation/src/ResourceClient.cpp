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

#include "ResourceClient.h"
#include "ResourceBroker.h"
#include "ResourceCacheManager.h"

#define CLIENT_W_TAG  PCF("ResourceClient")

using namespace OIC::Service;

namespace
{
    ResourceState getResourceStateFromBrokerState(BROKER_STATE state)
    {

        OC_LOG(DEBUG, CLIENT_W_TAG, "getResourceStateFromBrokerState entry");

        if (state == BROKER_STATE::ALIVE)
        {
            return ResourceState::ALIVE;
        }
        else if (state == BROKER_STATE::REQUESTED)
        {
            return ResourceState::REQUESTED;
        }
        else if (state == BROKER_STATE::LOST_SIGNAL)
        {
            return ResourceState::LOST_SIGNAL;
        }
        else if (state == BROKER_STATE::DESTROYED)
        {
            return ResourceState::DESTROYED;
        }
        else if (state == BROKER_STATE::NONE)
        {
            return ResourceState::NOT_MONITORING;
        }
        OC_LOG(ERROR, CLIENT_W_TAG, "getResourceStateFromBrokerState ERROR");

        //Default return value
        return ResourceState::NOT_MONITORING;
    }

    CacheState getCacheState(CACHE_STATE state)
    {

        OC_LOG(DEBUG, CLIENT_W_TAG, "getCacheState (from CACHE_STATE) entry");

        if (state == CACHE_STATE::READY)
        {
            return CacheState::READY;
        }
        else if (state == CACHE_STATE::READY_YET)
        {
            return CacheState::READY_YET;
        }
        else if (state == CACHE_STATE::LOST_SIGNAL)
        {
            return CacheState::LOST_SIGNAL;
        }
        else if (state == CACHE_STATE::DESTROYED)
        {
            return CacheState::DESTROYED;
        }
        else if (state == CACHE_STATE::UPDATING)
        {
            return CacheState::UPDATING;
        }
        else if (state == CACHE_STATE::NONE)
        {
            return CacheState::NONE;
        }
        OC_LOG(ERROR, CLIENT_W_TAG, "getCacheState (from CACHE_STATE) ERROR");

        //Default return value
        return CacheState::NONE;
    }

    OCStackResult hostingCallback(BROKER_STATE state,
                                  RemoteResourceObject::ResourceStateChangedCallback onResourceStateChanged)
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "hostingCallback entry");

        ResourceState resourceState = getResourceStateFromBrokerState(state);
        onResourceStateChanged(resourceState); //passing ResourceState to application

        OC_LOG(DEBUG, CLIENT_W_TAG, "hostingCallback exit");
        return OC_STACK_OK;
    }

    OCStackResult cachingCallback(std::shared_ptr<PrimitiveResource> resource,
                                  const ResourceAttributes &data,
                                  RemoteResourceObject::CacheUpdatedCallback onCacheUpdated)
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "cachingCallback entry");

        onCacheUpdated(data); //passing ResourceAttributes to application

        OC_LOG(DEBUG, CLIENT_W_TAG, "cachingCallback exit");
        return OC_STACK_OK;
    }

    void setCallback(const HeaderOptions &header, const ResponseStatement &response, int n,
                     RemoteResourceObject::RemoteAttributesSetCallback onRemoteAttributesSet)
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "setCallback entry");

        const ResourceAttributes &attributes = response.getAttributes();
        onRemoteAttributesSet(attributes); //passing ResourceAttributes to application

        OC_LOG(DEBUG, CLIENT_W_TAG, "setCallback exit");
    }

    void getCallback(const HeaderOptions &headerOption, const ResponseStatement &response, int n,
                     RemoteResourceObject::RemoteAttributesReceivedCallback onRemoteAttributesReceived)
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "getCallback entry");

        const ResourceAttributes &attributes = response.getAttributes();
        onRemoteAttributesReceived(attributes); //passing ResourceAttributes to application

        OC_LOG(DEBUG, CLIENT_W_TAG, "getCallback exit");
    }

    void findCallback(std::shared_ptr<PrimitiveResource> primitiveResource,
                      DiscoveryManager::OnResourceDiscoveredCallback OnResourceDiscovered )
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "findCallback entry");

        if (nullptr == primitiveResource)
        {
            OC_LOG(ERROR, CLIENT_W_TAG, "findCallback::primitiveResource NULL Parameter");
            return ;
        }

        std::shared_ptr< RemoteResourceObject>  primitiveClientResource =
            std::shared_ptr< RemoteResourceObject>(new RemoteResourceObject(primitiveResource));

        OnResourceDiscovered(primitiveClientResource); //passing PrimitiveClientResource to application

        OC_LOG(DEBUG, CLIENT_W_TAG, "findcb exit");
    }

}

//*******************************Primitive Client Resource*************************************

namespace OIC
{
    namespace Service
    {

        RemoteResourceObject:: RemoteResourceObject(std::shared_ptr<PrimitiveResource>  pResource) :
            m_monitoringFlag(false), m_cachingFlag(false),  m_observableFlag(pResource->isObservable()),
            m_primitiveResource(pResource), m_cacheId(0), m_brokerId(0) {}

        bool RemoteResourceObject::isMonitoring() const
        {
            return m_monitoringFlag;
        }

        bool RemoteResourceObject::isCaching() const
        {
            return m_cachingFlag;
        }

        void RemoteResourceObject::startMonitoring(ResourceStateChangedCallback cb)
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::startMonitoring entry");
            if (true == m_monitoringFlag)
            {
                OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::startMonitoring : Already started");
            }
            else
            {
                try
                {
                    BrokerID brokerId =  ResourceBroker::getInstance()->hostResource(m_primitiveResource,
                                         std::bind(hostingCallback, std::placeholders::_1,
                                                   cb));
                    m_monitoringFlag = true;
                    m_brokerId = brokerId;
                }
                catch (std::exception &exception)
                {
                    throw InvalidParameterException {exception.what()};
                }
            }
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::startMonitoring exit");
        }

        void RemoteResourceObject::stopMonitoring()
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::stopMonitoring entry");
            if (true == m_monitoringFlag)
            {
                try
                {
                    ResourceBroker::getInstance()->cancelHostResource(m_brokerId);
                    m_monitoringFlag = false;
                }
                catch (std::exception &exception)
                {
                    OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::stopMonitoring InvalidParameterException");
                }
            }
            else
            {
                OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject:: stopMonitoring : already terminated");
            }

            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::stopMonitoring exit");
        }

        ResourceState RemoteResourceObject::getState() const
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, " RemoteResourceObject::getState entry");
            try
            {
                BROKER_STATE brokerState = ResourceBroker::getInstance()->getResourceState(m_primitiveResource);
                OC_LOG(DEBUG, CLIENT_W_TAG, " RemoteResourceObject::getState exit");
                return getResourceStateFromBrokerState(brokerState);
            }
            catch (std::exception &exception)
            {
                OC_LOG(DEBUG, CLIENT_W_TAG, " RemoteResourceObject::getState InvalidParameterException");
                throw BadRequestException { "[getState] Get Resource Source State from Broker Error " };
            }
        }

        void RemoteResourceObject::startCaching()
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::startCaching entry");
            if (true == m_cachingFlag)
            {
                OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::startCaching : already Started");
            }
            else
            {
                try
                {
                  CacheID cacheId = ResourceCacheManager::getInstance()->requestResourceCache(m_primitiveResource,
                                      NULL, REPORT_FREQUENCY::NONE,  0);

                    m_cacheId = cacheId;
                    m_cachingFlag = true;
                    OC_LOG_V(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::startCaching CACHE ID %d", cacheId);
                }
                catch (std::exception &exception)
                {
                    OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::startCaching InvalidParameterException");
                }
            }
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::startCaching exit");
        }

        void RemoteResourceObject::startCaching(CacheUpdatedCallback cb)
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::startCaching entry");
            if (!cb)
            {
                throw InvalidParameterException {"startCaching : Callback is NULL" };
            }
            else
            {
                if (true == m_cachingFlag)
                {
                    OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::startCaching : already Started");
                }
                else
                {
                    try
                    {
                        CacheID cacheId = ResourceCacheManager::getInstance()->requestResourceCache(m_primitiveResource,
                                          std::bind(cachingCallback, std::placeholders::_1, std::placeholders::_2, cb),
                                          REPORT_FREQUENCY::UPTODATE,  0);

                        m_cacheId = cacheId;
                        m_cachingFlag = true;
                        OC_LOG_V(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::startCaching CACHE ID %d", cacheId);
                    }
                    catch (std::exception &exception)
                    {
                        throw InvalidParameterException {"startCaching : error" };
                    }
                }
            }
        }

        void RemoteResourceObject::stopCaching()
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::stopCaching entry");

            if (true == m_cachingFlag)
            {
                try
                {
                    ResourceCacheManager::getInstance()->cancelResourceCache(m_cacheId);
                    m_cachingFlag = false;
                }
                catch (std::exception &exception)
                {
                    OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::stopCaching InvalidParameterException");
                }
            }
            else
            {
                OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject:: Caching already terminated");
            }

            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::stopCaching exit");
        }

        CacheState  RemoteResourceObject::getResourceCacheState()
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::getResourceCacheState entry");
            try
            {
                CACHE_STATE cacheState = ResourceCacheManager::getInstance()->getResourceCacheState(
                                             m_primitiveResource);
                OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::getResourceCacheState exit");
                return getCacheState(cacheState);
            }
            catch (std::exception &exception)
            {
                OC_LOG(DEBUG, CLIENT_W_TAG,
                       "RemoteResourceObject::getResourceCacheState InvalidParameterException");
                throw BadRequestException { "[getResourceCacheState] Caching not started" };
            }
        }

        void RemoteResourceObject::refreshCache()
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::refreshCache entry");
            try
            {

                ResourceCacheManager::getInstance()->updateResourceCache(
                    m_primitiveResource);
            }
            catch (std::exception &exception)
            {
                OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::refreshCache InvalidParameterException");
            }
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::refreshCache exit");
        }

        ResourceAttributes RemoteResourceObject:: getCachedAttributes() const
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject :: getCachedAttributes ");
            try
            {
                return ResourceCacheManager::getInstance()->getCachedData(m_primitiveResource);
            }
            catch (std::exception &exception)
            {
                throw BadRequestException { "[getCachedAttributes]  Caching not started" };
            }
        }

        ResourceAttributes::Value  RemoteResourceObject:: getCachedAttribute( const std::string &key)
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject :: getCachedAttribute entry");
            try
            {
                ResourceAttributes  Cachedattributes = ResourceCacheManager::getInstance()->getCachedData(
                        m_primitiveResource);
                return Cachedattributes[key];
            }
            catch (std::exception &exception)
            {
                throw BadRequestException { "[getCachedAttribute]  Caching not started" };
            }
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject :: getCachedAttribute exit");
        }

        std::string RemoteResourceObject::getUri() const
        {
            return m_primitiveResource->getUri();
        }

        std::string RemoteResourceObject::getAddress() const
        {
            return m_primitiveResource->getHost();
        }

        std::vector < std::string > RemoteResourceObject::getTypes() const
        {
            return m_primitiveResource->getTypes();
        }

        std::vector < std::string > RemoteResourceObject::getInterfaces() const
        {
            return m_primitiveResource->getInterfaces();
        }

        void RemoteResourceObject::getRemoteAttributes(RemoteAttributesReceivedCallback cb)
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::getRemoteAttributes entry");
            if (!cb)
            {
                throw InvalidParameterException {"getRemoteAttributes : Callback is NULL" };
            }
            else
            {
                m_primitiveResource->requestGet(std::bind(getCallback, std::placeholders::_1,
                                                std::placeholders::_2, std::placeholders::_3, cb));
            }
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::getRemoteAttributes exit");
        }

        void RemoteResourceObject::setRemoteAttributes(ResourceAttributes &attribute,
                RemoteAttributesSetCallback cb)
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::setRemoteAttributes entry");
            if (!cb)
            {
                throw InvalidParameterException {"setRemoteAttributes : Callback is NULL" };
            }
            else
            {
                m_primitiveResource->requestSet(attribute, std::bind(setCallback, std::placeholders::_1,
                                                std::placeholders::_2, std::placeholders::_3, cb));
            }
            OC_LOG(DEBUG, CLIENT_W_TAG, "RemoteResourceObject::setRemoteAttributes exit");
        }

        bool RemoteResourceObject::isObservable() const
        {
            return m_observableFlag;
        }

//*******************************Discovery Manager****************************************

        DiscoveryManager *DiscoveryManager:: getInstance()
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "DiscoveryManager:: getInstance entry");
            static DiscoveryManager *s_instance;
            static std::mutex s_mutex;
            if (!s_instance)
            {
                std::lock_guard<std::mutex> lock(s_mutex);
                if (!s_instance)
                {
                    s_instance = new DiscoveryManager();
                }
            }
            OC_LOG(DEBUG, CLIENT_W_TAG, "DiscoveryManager:: getInstance exit");
            return s_instance;
        }

        void DiscoveryManager::discoverResource(const RCSAddress &address, const std::string &resourceURI,
                                                OnResourceDiscoveredCallback cb)
        {

            OC_LOG(DEBUG, CLIENT_W_TAG, "DiscoveryManager::discoverResource entry");

            if ( resourceURI.empty() )
            {
                OC_LOG(ERROR, CLIENT_W_TAG, "discoverResource NULL resourceURI");
                throw InvalidParameterException { "discoverResource NULL resourceURI'" };
            }
            else if ( !cb )
            {
                OC_LOG(ERROR, CLIENT_W_TAG, "discoverResource NULL Callback");
                throw InvalidParameterException { "discoverResource NULL Callback'" };
            }
            OIC::Service::discoverResource(address, resourceURI, std::bind(findCallback,
                                           std::placeholders::_1,
                                           cb));

            OC_LOG(DEBUG, CLIENT_W_TAG, "DiscoveryManager::discoverResource exit");
        }
    }
}
