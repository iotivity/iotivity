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
 * This file contains the Resource Client APIs provided to the developers.
 * It is a common API layer for the Resource Broker and Resource Cache module of Resource
 * Encapsulation layer.
 */

#ifndef RESOURCE_CLIENT_H_
#define RESOURCE_CLIENT_H_

#include<vector>
#include "RCSResourceAttributes.h"

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
            NOT_MONITORING,
            ALIVE, REQUESTED,
            LOST_SIGNAL,
            DESTROYED
        };

        /*
        * Forward Declaration of Classes
        */
        class RCSException;
        class RemoteResourceObject;
        class PrimitiveResource;
        class RCSAddress;

        /**
         * @class  BadRequestException
         * @brief  This class is used to throw exception to the upper layer if request is invalid.
         *             It is inherited from RCSException class.
         *
         */
        class BadRequestException: public RCSException
        {
            public:
                BadRequestException(const std::string &what) : RCSException { what } {}
                BadRequestException(std::string &&what) : RCSException { std::move(what) } {}
        };

        /**
         * @class   InvalidParameterException
         * @brief   This class is used to throw exception to the upper layer if parameter is invalid.
         *              It is  inherited from RCSException class.
         */
        class InvalidParameterException: public RCSException
        {
            public:
                InvalidParameterException(const std::string &what) : RCSException { what } {}
                InvalidParameterException(std::string &&what) : RCSException{ std::move(what) } {}
        };

        /**
         * @class   RemoteResourceObject
         * @brief   This class is an interaction point between Resource
         *              and the developers. Developer will get the RemoteResourceObject by calling the
         *              discoverResource() API of "DiscoveryManager" class.
         *
         * @see DiscoveryManager
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
                 *  Typedef for callback of startMonitoring API
                 *
                 * @see ResourceState
                 */
                typedef std::function< void(ResourceState) > ResourceStateChangedCallback;

                /**
                *  Typedef for callback of startCaching API
                *
                * @see RCSResourceAttributes
                */
                typedef std::function< void(const RCSResourceAttributes &) > CacheUpdatedCallback;

                /**
                *  Typedef for callback of getRemoteAttributes API
                *
                *  @see RCSResourceAttributes
                */
                typedef std::function< void(const RCSResourceAttributes &) >
                RemoteAttributesReceivedCallback;


                /**
                *  Typedef for callback of setRemoteAttributes API
                *
                *  @see RCSResourceAttributes
                */
                typedef std::function< void(const RCSResourceAttributes &) >
                RemoteAttributesSetCallback;

                /**
                 * Check monitoring state.
                 *
                 * @details This API checks the current monitoring state for the resource of interest.
                 *
                 * @return bool - true if monitoring the resource otherwise false.
                 */
                bool isMonitoring() const;

                /**
                 * Check current Caching state.
                 *
                 * @details This API checks the current caching state for the resource of interest.
                 *
                 * @return bool - true if Caching started otherwise false.
                 */

                bool isCaching() const;

                /**
                 * Check whether reosurce is observable or not.
                 *
                 * @details This API checks  the observable property of the resource.
                 *
                 * @return bool - true if observable otherwise false.
                 */
                bool isObservable() const;

                /**
                 * Start Monitoring the resource.
                 *
                 * @details This API will start monitoring the resource of interest.
                 *               Once this API is called it will check whether the particular resource
                 *               is available or not. It will provide the changed resource state in the callback.
                 *
                 * @param cb - callback to get changed resource state.
                 *
                 * @throw InvalidParameterException
                 *
                 * @see ResourceStateChangedCallback
                 * @see ResourceState
                 *
                 * NOTE: Developer can call this API any number of time. Developer should take care
                 *            of Synchronization as ResourceStateChangedCallback is asynchronous.
                 *            This function throws the InvalidParameterException if the callback is NULL or not valid.
                 */
                void startMonitoring(ResourceStateChangedCallback cb);

                /**
                 * Stop monitoring the resource.
                 *
                 * @details This API will stop monitoring the resource of interest it means it will stop to look
                 *               for the resource presence in the network.
                 *
                 * NOTE: If startMonitoring() is not being called & directly this API is called it will do nothing.
                 *           Developer can call this API any number of time. It will not results in any kind of warning.
                 *
                 */
                void stopMonitoring();

                /**
                 * Provides the current resource state. Resource state is an enum class.
                 *
                 * @return ResourceState - current state of the resource.
                 *
                 * @throw BadRequestException
                 *
                 * @see ResourceState
                 */
                ResourceState getState() const ;

                /**
                 * Start caching data for the resource of interest.
                 *
                 * @details This API will start data caching for the resource of interest.
                 *               Once caching started it will look for the data updation on the resource of interest
                 *                & updates the cache data accordingly. It provides the cached data on demand.
                 *
                 * @see getCachedAttributes()
                 * @see getCachedAttribute( const std::string &)
                 *
                 * NOTE: developer can get the cached data by calling getCachedAttributes()
                 *            or getCachedAttribute() API
                 */
                void startCaching();

                /**
                 * Start caching data for the resource of interest.
                 *
                 * @details This API will start data caching for the resource of interest.
                 *              Once caching started it look for the data updation on the resource of interest &
                 *              updates the cached data accordingly Whenever data is updated in the cache, it
                 *              provides the updated data to the application/caller.
                 *
                 * @param cb - callback to get updated resourceAttributes.
                 *
                 * @throw InvalidParameterException
                 *
                 * @see CacheUpdatedCallback
                 *
                 * NOTE: Developer can call this API any number of time. Developer should
                 *           take care of Synchronization as CacheUpdatedCallback is asynchronous.
                 *           This function throws the InvalidParameterException if the callback is NULL or not valid.
                 *
                 */
                void startCaching(CacheUpdatedCallback cb);

                /**
                 * Provides the current cache state for the resource of interest. CacheState is the enum class.
                 *
                 * @return CacheState - Current state of the Cache.
                 *
                 * @throw BadRequestException
                 *
                 * @see CacheState
                 *
                 */
                CacheState getResourceCacheState();

                /**
                * Stop data caching for the resource of interest.
                *
                * @details This API will stop caching the data for the resource of interest.
                *
                * NOTE: If startCaching() or startCaching(CacheUpdatedCallback) is not being called &
                *            directly this API is called it will do nothing.
                *            Developer can call this API any number of time, it will not results in any warning.
                *
                */
                void stopCaching();

                /**
                * Refresh the cache.
                *
                * @details This API will refresh the cache, i.e. it will get the latest data from the server.
                *
                */
                void refreshCache() ;

                /**
                 * Get the cached RCSResourceAttributes data.
                 *
                 * @pre startCaching() or startCaching(CacheUpdatedCallback) API should be called.
                 *
                 * @return RCSResourceAttributes - cached resourceAttribute
                 *
                 * @throw BadRequestException
                 *
                 * @see startCaching()
                 * @see startCaching(CacheUpdatedCallback)
                 * @see RCSResourceAttributes
                 *
                 * NOTE: If startCaching() or startCaching(CacheUpdatedCallback) is not being called &
                 *           directly this API is called it will throw the
                 *           BadRequestException.
                 */
                RCSResourceAttributes getCachedAttributes() const;

                /**
                * Get a particular cached ResourceAttribute value.
                *
                * @pre startCaching() or startCaching(CacheUpdatedCallback) API should be called.
                *
                * @return RCSResourceAttributes::Value - requested attribute Value
                *
                * @throw BadRequestException
                *
                * @see startCaching()
                * @see startCaching(CacheUpdatedCallback)
                * @see RCSResourceAttributes::Value
                *
                * NOTE: If startCaching() or startCaching(CacheUpdatedCallback) is not being called &
                *           directly this API is called it will throw the BadRequestException.
                *
                */
                RCSResourceAttributes::Value getCachedAttribute( const std::string &) ;

                /**
                * Get resource attributes.
                *
                * @details This API send a get request to the resource of interest and provides the attributes
                *               to the caller in the RemoteAttributesReceivedCallback.
                *
                * @see RCSResourceAttributes::Value
                */
                void getRemoteAttributes(RemoteAttributesReceivedCallback cb);

                /**
                * Set resource attributes.
                *
                * @details This API send a set request to the resource of interest and provides the updated
                *              attributes to the caller in the RemoteAttributesSetCallback.
                *
                * @param attributes - resourceAttributes data to set
                * @param cb - callback on setting resourceAttributes data.
                *
                * @throw InvalidParameterException
                *
                */
                void setRemoteAttributes(const RCSResourceAttributes &attributes, RemoteAttributesSetCallback cb);

                /**
                 * Get resource uri.
                 *
                 * @return string - uri of the Resource
                 */
                std::string getUri() const;

                /**
                 * Get resource address.
                 *
                 * @return string - address of the Resource
                 */
                std::string getAddress() const;

                /**
                 * Get resource types.
                 *
                 * @return vector - resource types
                 */
                std::vector< std::string > getTypes() const;

                /**
                 * Get resource interfaces.
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
                 *  Flag to check monitoring state.
                 */
                bool m_monitoringFlag;

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
         * @brief   This class contains the resource discovery method.
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
                * API for getting DiscoveryManager instance.
                *
                * @return DiscoveryManager - Instance of DiscoveryManager class
                */
                static DiscoveryManager *getInstance();

                /**
                * API for discovering the resource of Interest.
                *
                * @param address - RCSAddress object
                * @param resourceURI - uri of resource to be searched
                * @param cb - callback to obtain discovered resource
                *
                * @throw InvalidParameterException : This API throws the InvalidParameterException if any of
                *                                                         the parameter is invalid.
                * @RCSAddress
                */
                void discoverResource(const RCSAddress &address, const std::string &resourceURI,
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
