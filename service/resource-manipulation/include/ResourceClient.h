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
 * This file contains the resource client APIs provided to the developers
 */

#ifndef RESOURCE_CLIENT_H_
#define RESOURCE_CLIENT_H_

#include<vector>
#include "ResourceAttributes.h"
#include "PrimitiveResource.h"

namespace OIC
{
    namespace Service
    {

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

        /*
        * Forward Declaration of Classes
        */
        class RCSException;
        class RemoteResourceObject;

        /**
         * @class   BadRequestException
         * @brief   It is used to throw exception to the upper layer if request is invalid.
         *             This class inherited from PrimitiveException class.
         *
         */
        class BadRequestException: public RCSException
        {
            public:
                BadRequestException(const std::string &what) : RCSException{ what } {}
                BadRequestException(std::string &&what) : RCSException{ std::move(what) } {}
        };

        /**
         * @class   InvalidParameterException
         * @brief    It is used to throw exception to the upper layer if parameter is invalid.
         *              This class inherited from PrimitiveException class.
         */
        class InvalidParameterException: public RCSException
        {
            public:
                InvalidParameterException(const std::string &what) : RCSException{ what } {}
                InvalidParameterException(std::string &&what) : RCSException{ std::move(what) } {}
        };

        /**
         * @class   RemoteResourceObject
         * @brief   It is an interaction point between Resource
         *        and the developers.
         *
         */
        class RemoteResourceObject
        {
            public:

                /**
                 * Constructor for RemoteResourceObject
                */
                RemoteResourceObject(std::shared_ptr<PrimitiveResource>  pResource);

                /**
                 *  Typedef for callback of startWatching API
                 */
                typedef std::function< void(ResourceState) > ResourceStateChangedCallback;

                /**
                *  Typedef for callback of startCaching API
                */
                typedef std::function< void(const ResourceAttributes &) > CacheUpdatedCallback;

                /**
                *  Typedef for callback of getRemoteAttributes API
                */
                typedef std::function< void(const ResourceAttributes &) >
                RemoteAttributesReceivedCallback;


                /**
                *  Typedef for callback of setRemoteAttributes API
                */
                typedef std::function< void(const ResourceAttributes &) >
                RemoteAttributesSetCallback;

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
                 * @return bool - observable or not.
                 */
                bool isObservable() const;

                /**
                 * API to start watching the resource.
                 *
                 * @param ResourceStateChangedCallback - callback to get changed resource state.
                 *
                 * @throw InvalidParameterException
                 *
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
                */
                void startCaching();

                /**
                 * API to start caching data of the resource.
                 *
                 * @param CacheUpdatedCallback - callback to get updated resourceAttributes.
                 *
                 * @throw InvalidParameterException
                 *
                 */
                void startCaching(CacheUpdatedCallback);

                /**
                 * API to get the cache State of the resource
                 *
                 * @return CacheState - Current state of the Cache.
                 */
                CacheState getResourceCacheState();

                /**
                 * API to stop caching the data for the resource
                 */
                void stopCaching();

                /**
                 * API to refresh the cache explicitly.
                 */
                void refreshCache() ;

                /**
                 * API to get cached ResourceAttributes data.
                 *
                 * @return ResourceAttributes - cached ResourceAttributes data
                 *
                 * @throw BadRequestException
                 *
                 */
                ResourceAttributes getCachedAttributes() const;

                /**
                 * API to get particular cached ResourceAttribute value
                 *
                 * @return Value - ResourceAttributes::Value class object
                 *
                 * @throw BadRequestException
                 *
                 */
                ResourceAttributes::Value getCachedAttribute( const std::string &) ;

                /**
                 * API to get current resource attributes data.
                 *
                 * @param RemoteAttributesReceivedCallback - callback to get resourceAttributes data.
                 *
                 */
                void getRemoteAttributes(RemoteAttributesReceivedCallback);

                /**
                 * API to set resource attributes data.
                 *
                 * @param ResourceAttributes - resourceAttributes data to set
                 * @param RemoteAttributesSetCallback - callback on setting resourceAttributes data.
                 *
                 */
                void setRemoteAttributes(ResourceAttributes &, RemoteAttributesSetCallback );

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
                 *  Typedef for Cache ID
                 */
                typedef int CacheID;

                /**
                 *  Typedef for Broker ID
                 */
                typedef unsigned int BrokerID;

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
                 *  PrimitiveResource
                 */
                std::shared_ptr<PrimitiveResource> m_primitiveResource;

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
         * @class   DiscoveryManager
         * @brief   It contains the resource discovery method.
         *
         */
        class DiscoveryManager
        {
            public:

                /**
                 *  Typedef for callback of discoverResource API
                 */
                typedef std::function< void( std::shared_ptr< RemoteResourceObject>) >
                OnResourceDiscoveredCallback;

                /**
                * API to get DiscoveryManager instance.
                *
                * @return DiscoveryManager -instance.
                */
                static DiscoveryManager *getInstance();

                /**
                * API for discovey of resource of Interest.
                *
                * @param host - host of the Resource
                * @param resourceURI - uri of resource to be searched
                * @param connectivityType - connection type
                * @param cb - callback to obtain discovered resource.
                *
                * @throw InvalidParameterException
                *
                */
                void discoverResource(std::string host, std::string resourceURI,
                                      OCConnectivityType connectivityType,
                                      OnResourceDiscoveredCallback cb);

            private:

                /**
                 * Constructor for DiscoveryManager.
                */
                DiscoveryManager() = default;

                /**
                * Destructor for DiscoveryManager.
                */
                ~DiscoveryManager() = default;

        };
    }
}
#endif //RESOURCE_CLIENT_H_
