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
#include "ReportPolicyProxy.h"

#define CLIENT_W_TAG  PCF("ResourceClient")

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

        OC_LOG(ERROR, CLIENT_W_TAG, "getResourceStateFromBrokerState ERROR");

        //Default return value
        return ResourceState::DESTROYED;
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

    OCStackResult hosting_cb(BROKER_STATE state,
                             PrimitiveClientResource::ResourceStateChangedCallback onResourceStateChanged)
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "hosting_cb entry");

        ResourceState rState = getResourceStateFromBrokerState(state);

        onResourceStateChanged(rState); //passing ResourceState to application

        OC_LOG(DEBUG, CLIENT_W_TAG, "hosting_cb exit");
        return OC_STACK_OK;
    }

    OCStackResult caching_cb(std::shared_ptr<PrimitiveResource> resource,
                             const ResourceAttributes &data,
                             PrimitiveClientResource::CacheUpdatedCallback onCacheUpdated)
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "caching_cb entry");

        onCacheUpdated(data); //passing ResourceAttribute to application

        OC_LOG(DEBUG, CLIENT_W_TAG, "caching_cb exit");
        return OC_STACK_OK;
    }

    void set_cb(const HeaderOptions &header, const ResponseStatement &response, int n)
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "set_cb");

    }

    void get_cb(const HeaderOptions &headerOption, const ResponseStatement &response, int n,
                PrimitiveClientResource::RemoteAttributesReceivedCallback onRemoteAttributesReceived)
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "get_cb entry");

        const ResourceAttributes &attributes = response.getAttributes();
        onRemoteAttributesReceived(attributes); //passing ResourceAttribute to application

        OC_LOG(DEBUG, CLIENT_W_TAG, "get_cb exit");
    }

    void find_cb(std::shared_ptr<PrimitiveResource> primitiveResource,
                 PrimitiveClient::OnResourceDiscoveredCallback OnResourceDiscovered )
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "findcb entry");

        if (nullptr == primitiveResource)
        {
            OC_LOG(ERROR, CLIENT_W_TAG, "find_cb::primitiveResource NULL Parameter");
            return ;
        }

        std::shared_ptr< PrimitiveClientResource>  primitiveClientResource =
            std::shared_ptr< PrimitiveClientResource>(new PrimitiveClientResource(primitiveResource));

        OnResourceDiscovered(primitiveClientResource); //passing PrimitiveClientResource to application

        OC_LOG(DEBUG, CLIENT_W_TAG, "findcb exit");
    }

}

//*******************************Primitive Client Resource*************************************

PrimitiveClientResource:: PrimitiveClientResource(std::shared_ptr<PrimitiveResource>  pResource) :
    m_primitiveResource(pResource), m_uri(pResource->getUri()),
    m_address(pResource->getHost()), m_types(pResource->getTypes()),
    m_interfaces(pResource->getInterfaces()), m_observableFlag(pResource->isObservable()) {}


bool PrimitiveClientResource::isWatching() const
{
    return m_watchingFlag;
}

bool PrimitiveClientResource::isCaching() const
{
    return m_cachingFlag;
}

void PrimitiveClientResource::startWatching(ResourceStateChangedCallback cb)
{
    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::startWatching entry");
    if (true == m_watchingFlag)
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::startWatching : Already started");
    }
    else
    {
        BrokerID brokerId =  ResourceBroker::getInstance()->hostResource(m_primitiveResource,
                             std::bind(hosting_cb, std::placeholders::_1,
                                       cb));
        if (0 == brokerId)
        {
            m_watchingFlag = false;
            throw BadRequestException { "Failed to start watching resource "};
        }
        else
        {
            m_watchingFlag = true;
            m_brokerId = brokerId;
        }
    }

    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::startWatching exit");
}

void PrimitiveClientResource::stopWatching()
{
    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::stopWatching entry");
    if (true == m_watchingFlag)
    {
        BrokerID brokerId = ResourceBroker::getInstance()->cancelHostResource(m_brokerId);
        if (0 == brokerId)
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource:: Failed to terminate hosting");
            throw BadRequestException { "Failed to terminate hosting " };
        }
        else
        {
            m_watchingFlag = false;
        }
    }
    else
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource:: stopWatching : already terminated");
    }

    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::stopWatching exit");
}

ResourceState PrimitiveClientResource::getState() const
{
    OC_LOG(DEBUG, CLIENT_W_TAG, " PrimitiveClientResource::getState entry");

    BROKER_STATE brokerState = ResourceBroker::getInstance()->getResourceState(m_primitiveResource);
    return getResourceStateFromBrokerState(brokerState);
}

void PrimitiveClientResource::startCaching(ReportPolicy reportPolicy, CacheUpdatedCallback cb)
{
    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::startCaching entry");
    if (true == m_cachingFlag)
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::startCaching : already Started");
    }
    else
    {
        CacheID cacheId = reportPolicy.getProxy()->startProxyCaching(m_primitiveResource,
                          std::bind(caching_cb, std::placeholders::_1, std::placeholders::_2, cb));

        OC_LOG_V(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::startCaching CACHE ID %d", cacheId);
        if (0 == cacheId)
        {
            m_cachingFlag = false;
            OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::startCaching FAILED");
            throw BadRequestException { "Failed to generate Cache ID" };
        }
        else
        {
            m_cacheId = cacheId;
            m_cachingFlag = true;
        }
    }

    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::startCaching exit");
}

void PrimitiveClientResource::stopCaching()
{
    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::stopCaching entry");
    OCStackResult result = OC_STACK_ERROR;

    if (true == m_cachingFlag)
    {
        result = ResourceCacheManager::getInstance()->cancelResourceCache(m_primitiveResource,
                 m_cacheId);
        if (result == OC_STACK_OK)
        {
            m_cachingFlag = false;
            OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource:: SUCCESS");
        }
        else
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource:: Failed to terminate Caching");
            throw BadRequestException { "Failed to terminate Caching " };
        }
    }
    else
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource:: Caching already terminated");
    }

    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::stopCaching exit");
}

CacheState  PrimitiveClientResource::getResourceCacheState()
{
    CACHE_STATE cacheState = ResourceCacheManager::getInstance()->getResourceCacheState(
                                 m_primitiveResource);
    return getCacheState(cacheState);
}

void PrimitiveClientResource::refreshCache()
{
    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::refreshCache entry");

    OCStackResult result = ResourceCacheManager::getInstance()->updateResourceCache(
                               m_primitiveResource);
    if (result == OC_STACK_OK)
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::refreshCache Success");
    }
    else
    {
        OC_LOG_V(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::refreshCache FAILED %d", result);
        throw BadRequestException { "Failed to refresh Caching " };
    }
}

ResourceAttributes PrimitiveClientResource:: getCachedAttributes() const
{
    OC_LOG(DEBUG, CLIENT_W_TAG, "ResourceAttributes getCachedAttributes ");
    return  ResourceCacheManager::getInstance()->getCachedData(m_primitiveResource);
}

std::string PrimitiveClientResource::getUri() const
{
    return m_uri;
}

std::string PrimitiveClientResource::getAddress() const
{
    return m_address;
}

bool PrimitiveClientResource::isObservable() const
{
    return m_observableFlag;
}

std::vector < std::string > PrimitiveClientResource::getTypes() const
{
    return m_types;
}

std::vector < std::string > PrimitiveClientResource::getInterfaces() const
{
    return m_interfaces;
}

void PrimitiveClientResource::getRemoteAttributes(RemoteAttributesReceivedCallback cb)
{
    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::getRemoteAttributes entry");

    m_primitiveResource->requestGet(std::bind(get_cb, std::placeholders::_1,
                                    std::placeholders::_2, std::placeholders::_3, cb));

    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::getRemoteAttributes exit");
}

void PrimitiveClientResource::setRemoteAttributes(ResourceAttributes &attribute)
{
    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::setRemoteAttributes entry");

    m_primitiveResource->requestSet(attribute, std::bind(set_cb, std::placeholders::_1,
                                    std::placeholders::_2, std::placeholders::_3));

    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClientResource::setRemoteAttributes exit");
}

//*******************************Report Policy**********************************************

ReportPolicy::ReportPolicy(ReportPolicyProxy &&reportPolicyProxy)
{
    m_proxy = std::shared_ptr< ReportPolicyProxy>(new ReportPolicyProxy(std::forward<ReportPolicyProxy>
              (reportPolicyProxy)));
}

ReportPolicy ReportPolicy::none()
{
    OC_LOG(DEBUG, CLIENT_W_TAG, "ReportPolicy::none entry");

    ReportPolicyProxy::ProxyFunc func = std::bind(&ResourceCacheManager::requestResourceCache,
                                        ResourceCacheManager::getInstance(), std::placeholders::_1, std::placeholders::_2,
                                        REPORT_FREQUENCY::NONE, 0l);
    ReportPolicy reportPolicy = ReportPolicy(ReportPolicyProxy(func));

    OC_LOG(DEBUG, CLIENT_W_TAG, "ReportPolicy::none exit");
    return reportPolicy;
}

ReportPolicy ReportPolicy::upToDate()
{
    OC_LOG(DEBUG, CLIENT_W_TAG, "ReportPolicy::upToDate entry");

    ReportPolicyProxy::ProxyFunc func = std::bind(&ResourceCacheManager::requestResourceCache,
                                        ResourceCacheManager::getInstance(), std::placeholders::_1, std::placeholders::_2,
                                        REPORT_FREQUENCY::UPTODATE, 0l);
    ReportPolicy reportPolicy = ReportPolicy((ReportPolicyProxy(func)));

    OC_LOG(DEBUG, CLIENT_W_TAG, "ReportPolicy::upToDate exit");
    return reportPolicy;
}

ReportPolicy ReportPolicy::periodic(int interval, TimeUnit unit)
{

    OC_LOG(DEBUG, CLIENT_W_TAG, "ReportPolicy::periodic entry");
    if (0 > interval)
    {
        OC_LOG(ERROR, CLIENT_W_TAG, "ReportPolicy::periodic Invalid interval ");
        throw InvalidParameterException { "Invalid interval value " };
    }
    long long timeInMillis;
    if (unit == TimeUnit::MILLISECOND)
    {
        timeInMillis = interval;
    }
    else if (unit == TimeUnit::SECOND)
    {
        timeInMillis = interval * 60;
    }
    else if (unit == TimeUnit::MIN)
    {
        timeInMillis = interval * 60 * 60;
    }
    ReportPolicyProxy::ProxyFunc func = std::bind(&ResourceCacheManager::requestResourceCache,
                                        ResourceCacheManager::getInstance(), std::placeholders::_1, std::placeholders::_2,
                                        REPORT_FREQUENCY::PERIODICTY, timeInMillis);
    ReportPolicy reportPolicy = ReportPolicy((ReportPolicyProxy(func)));

    OC_LOG(DEBUG, CLIENT_W_TAG, "ReportPolicy::periodic exit");
    return reportPolicy;
}

std::shared_ptr<ReportPolicyProxy>  ReportPolicy::getProxy()
{
    return m_proxy;
}

//*******************************primitive client*********************************************

PrimitiveClient *PrimitiveClient:: getInstance()
{
    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClient:: getInstance entry");

    static PrimitiveClient *s_instance;
    static std::mutex s_mutex;
    if (!s_instance)
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        if (!s_instance)
        {
            s_instance = new PrimitiveClient();
        }
    }

    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClient:: getInstance exit");
    return s_instance;
}

void PrimitiveClient::discoverPrimitiveResource(std::string host, std::string resourceURI,
        OCConnectivityType connectivityType,
        OnResourceDiscoveredCallback cb)
{

    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClient::discoverResource entry");

    if ( resourceURI.empty() )
    {
        OC_LOG(ERROR, CLIENT_W_TAG, "discoverPrimitiveResource NULL resourceURI");
        throw InvalidParameterException { "discoverPrimitiveResource NULL resourceURI'" };
    }
    else if ( !cb )
    {
        OC_LOG(ERROR, CLIENT_W_TAG, "discoverPrimitiveResource NULL Callback");
        throw InvalidParameterException { "discoverPrimitiveResource NULL Callback'" };
    }
    discoverResource(host, resourceURI, connectivityType, std::bind(find_cb, std::placeholders::_1,
                     cb));

    OC_LOG(DEBUG, CLIENT_W_TAG, "PrimitiveClient::discoverResource exit");
}
