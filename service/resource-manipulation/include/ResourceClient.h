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
 * This file contains the resource client APIs provided to the developer
 */

#ifndef RESOURCE_CLIENT_H_
#define RESOURCE_CLIENT_H_

#include<iostream>
#include<string>
#include<vector>
#include<mutex>
#include<functional>

#include "ResourceAttributes.h"

using namespace OIC::Service;

/*
* Forward Declaration of Classes
*/

namespace OIC
{
    namespace Service
    {
        class PrimitiveResource;
    }
}
class ReportPolicyProxy;
class PrimitiveClientResource;

/**
* Cache State enum specify the state of the Cache.
*/
enum class CacheState
{
    READY = 0,
    READY_YET,
    LOST_SIGNAL,
    DESTROYED,
    UPDATING,
    NONE
};

/**
* Resource State enum specify the state of the resource.
*/
enum class ResourceState
{
    NOT_WATCHING,
    ALIVE, REQUESTED,
    LOST_SIGNAL,
    DESTROYED
};

/**
* Time unit enum to indicate time duration in report cache policy.
*/
enum class TimeUnit
{
    MILLISECOND, SECOND, MIN
};

/**
 * @class   InvalidParameterException
 * @brief   This class extends from PrimitiveException class. It is used to
 *       throw exception to the upper layer if parameter is invalid.
 *
 */
class BadRequestException: public OIC::Service::PrimitiveException
{
    public:
        BadRequestException(const std::string &what) : PrimitiveException { what } {}
        BadRequestException(std::string &&what) : PrimitiveException { std::move(what) } {}
};

/**
 * @class   InvalidParameterException
 * @brief   This class extends from PrimitiveException class. It is used to
 *       throw exception to the upper layer if parameter is invalid.
 *
 */
class InvalidParameterException: public OIC::Service::PrimitiveException
{
    public:
        InvalidParameterException(const std::string &what) : PrimitiveException { what } {}
        InvalidParameterException(std::string &&what) : PrimitiveException { std::move(what) } {}
};

/**
 * @class   ReportPolicy
 * @brief   This class provides a set of method for various caching policies.
 *        Each method corresponds to REPORT_FREQUENCY values.
 *
 * NOTE: REPORT_FREQUENCY is enum class having values as the following:
 *          NONE(default), UPDATE and PERIODIC.
 *
 */
class ReportPolicy
{
    public:

        /**
         * Destructor for ReportPolicy
         */
        ~ReportPolicy(void) {}

        /**
         * API for setting the caching policy to default.
         *
         * @return ReportPolicy - reportPolicy object corresponding to default caching method.
         *
         */
        static ReportPolicy none();

        /**
          * API for setting the caching policy to updated.
          *
          * @return ReportPolicy - reportPolicy object corresponding to updated caching method.
          *
          */
        static ReportPolicy upToDate();

        /**
         * API for setting the caching policy to periodic with tive interval given.
         *
         * @param interval - Duration for the periodic caching of data.
         * @param unit - unit of the time interval.
         *
         * @return ReportPolicy - reportPolicy object corresponding to periodic caching method.
         *
         */
        static ReportPolicy periodic(int interval, TimeUnit unit);

        /**
         * API for getting the m_proxy data member for the called object.
         *
         * @return ReportPolicyProxy - ReportPolicyProxy data member.
         *
         */
        std::shared_ptr<ReportPolicyProxy> getProxy();

    private:

        /**
         *  constructor for ReportPolicy
        *
        * @param reportPolicyProxy - object binded to startCaching method.
        *
        */
        explicit ReportPolicy(ReportPolicyProxy &&reportPolicyProxy);

        /**
         *  ReportPolicyProxy object to invoke startCaching method.
         */
        std::shared_ptr<ReportPolicyProxy> m_proxy;

};

/**
 * @class   PrimitiveClientResource
 * @brief   PrimitiveClientResource is an interaction point between Resource
 *        and the developer.
 *
 */
class PrimitiveClientResource
{
    public:

        /**
         * Constructor for PrimitiveClientResource.
        */
        PrimitiveClientResource(std::shared_ptr<OIC::Service::PrimitiveResource>  pResource);

        /**
         *  Typedef for callbacks
         */
        typedef std::function< void(ResourceState) > ResourceStateChangedCallback;
        typedef std::function< void(const OIC::Service::ResourceAttributes &) > CacheUpdatedCallback;
        typedef std::function< void(const OIC::Service::ResourceAttributes &) >
        RemoteAttributesReceivedCallback;

        /**
         *  Typedef for Broker and Cache ID
         */
        typedef int CacheID;
        typedef unsigned int BrokerID;

        /**
         * API to get watching state.
         *
         * @return bool - watching or not.
         */
        bool isWatching() const;

        /**
         * API to get Caching state.
         *
         * @return bool - caching or not.
         */
        bool isCaching() const;

        /**
         * API to get observable state.
         *
         * @return bool - observing or not.
         */
        bool isObservable() const;

        /**
         * API to start watching the resource.
         *
         * @param ResourceStateChangedCallback - callback to get changed resource state.
         */
        void startWatching(ResourceStateChangedCallback);

        /**
         * API to stop watching the resource.
         */
        void stopWatching();

        /**
         * API to get resource state.
         *
         * @return ResourceState - current state of the resource.
         */
        ResourceState getState() const ;

        /**
         * API to start caching for the resource.
         *
         * @param ReportPolicy - caching policy.
         *
         * @param CacheUpdatedCallback - callback to get updated resourceAttributes.
         */
        void startCaching(ReportPolicy , CacheUpdatedCallback);

        /**
         * API to get the cache State of the resource
         *
         *@return CacheState - current state of the Cache.
         */
        CacheState getResourceCacheState();

        /**
         * API to stop caching for the resource.
         */
        void stopCaching();

        /**
         * API to refresh the cache explicitly.
         */
        void refreshCache() ;

        /**
         * API to obtain cached ResourceAttributes data.
         *
         *@return ResourceAttributes - cached ResourceAttributes data
         */
        ResourceAttributes getCachedAttributes() const;

        /**
         * API to obtain current resource attributes data.
         *
         * @param RemoteAttributesReceivedCallback - callback to get resourceAttributes data.
         */
        void getRemoteAttributes(RemoteAttributesReceivedCallback);

        /**
         * API to Set resource attributes data.
         *
         * @param ResourceAttributes - resourceAttributes data to set for the resource.
         */
        void setRemoteAttributes(ResourceAttributes &);

        /**
         * API to get resource uri.
         *
         * @return string - uri of the Resource
         */
        std::string getUri() const;

        /**
         * API to get resource address.
         *
         * @return string - address of the Resource
         */
        std::string getAddress() const;

        /**
         * API to get resource types.
         *
         * @return vector - resource types
         */
        std::vector< std::string > getTypes() const;

        /**
         * API to get resource interfaces.
         *
         * @return vector - resource interfaces
         */
        std::vector< std::string > getInterfaces() const;

    private:

        /**
         *  Flag to check watching state.
         */
        bool m_watchingFlag;

        /**
         *  Flag to check caching state.
         */
        bool m_cachingFlag;

        /**
         *  Flag to check observing state.
         */
        bool m_observableFlag;

        /**
         *  resource uri.
         */
        std::string m_uri;

        /**
         *  resource address.
         */
        std::string m_address;

        /**
         *  Resource type(s).
         */
        std::vector< std::string > m_types;

        /**
         *  Resource interface(s).
         */
        std::vector< std::string > m_interfaces;

        /**
         *  PrimitiveResource.
         */
        std::shared_ptr<OIC::Service::PrimitiveResource> m_primitiveResource;

        /**
         *  caching identification number.
         */
        CacheID m_cacheId;

        /**
        *  Broker  identification number.
        */
        BrokerID m_brokerId;

};

/**
 * @class   PrimitiveClient
 * @brief   It contains the resource discovery method.
 *
 */
class PrimitiveClient
{
    public:

        /**
         *  Typedef for callback
         */
        typedef std::function< void( std::shared_ptr< PrimitiveClientResource>) >
        OnResourceDiscoveredCallback;

        /**
        * API to get Primitive Client instance.
        *
        * @return PrimitiveClient -instance.
        */
        static PrimitiveClient *getInstance();

        /**
        * API for discovey of resource of Interrest.
        *
        * @param host - host to search for
        * @param resourceURI - uri of resource to be searched
        * @param connectivityType - connection type
        * @param cb - callback to obtain discovered resource.
        *
        */
        void discoverPrimitiveResource(std::string host, std::string resourceURI,
                                       OCConnectivityType connectivityType,
                                       OnResourceDiscoveredCallback cb);

    private:

        /**
         * Constructor for PrimitiveClient.
        */
        PrimitiveClient() = default;

        /**
         * Destructor for PrimitiveClient.
        */
        ~PrimitiveClient();

};
#endif //RESOURCE_CLIENT_H_
