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
 * This file contains the declaration of classes and its members related to RCSRemoteResourceObject
 */

#ifndef RCSREMOTERESOURCEOBJECT_H
#define RCSREMOTERESOURCEOBJECT_H

#include <vector>

#include "RCSResourceAttributes.h"
#include "RCSRepresentation.h"

namespace OC
{
    class OCResource;

    namespace HeaderOption
    {
        class OCHeaderOption;
    }
}

namespace OIC
{
    namespace Service
    {

        class RCSRepresentation;

        typedef std::vector< OC::HeaderOption::OCHeaderOption > HeaderOpts;

        /**
         * The states of caching.
         *
         * @see startCaching
         * @see getCacheState
         */
        enum class CacheState
        {
            NONE, /**< Caching is not started.*/
            UNREADY, /**< Caching is started, but the data is not ready yet.
                          This is the default state after startCaching. */
            READY, /**< The data is ready.*/
            LOST_SIGNAL, /**< Failed to reach the resource. */
        };

        enum class CacheMode
        {
            OBSERVE_ONLY, /**< Caching with observe.*/
            OBSERVE_WITH_POLLING /**< Caching with periodic polling.*/
        };

        /**
         * The states of monitoring.
         *
         * @see startMonitoring
         * @see getState
         */
        enum class ResourceState
        {
            NONE, /**< Monitoring is not started.*/
            REQUESTED, /**< Monitoring is started and checking state is in progress.
                            This is the default state after startMonitoring. */
            ALIVE, /**< The resource is alive. */
            LOST_SIGNAL, /**< Failed to reach the resource. */
            DESTROYED /**< The resource is deleted. */
        };

        class PrimitiveResource;

        /**
         * This is to specify query parameters for requests to the server.
         *
         * @see RCSRemoteResourceObject
         */
        class RCSQueryParams
        {
        public:
            typedef std::unordered_map< std::string, std::string > Map;

        public:

            /**
             * Sets an interface of the resource to operate on.
             *
             * @param interface resource interface to be set.
             *
             * @return Updated RCSQueryParams instance.
             */
            RCSQueryParams& setResourceInterface(std::string interface);

            /**
             * Sets a resource type of the resource to operate on.
             *
             * @param type resource type to be set.
             *
             * @return Updated RCSQueryParams instance.
             */
            RCSQueryParams& setResourceType(std::string type);

            /**
             * Puts given key and value in query params.
             *
             * @param key key to be inserted.
             * @param value value to be inserted.
             *
             * @return Updated RCSQueryParams instance.
             *
             * @note "rt" and "if" are reserved, so you should avoid them as a key.
             *
             */
            RCSQueryParams& put(std::string key, std::string value);

            /**
             * Gets the resource interface.
             *
             * @return Resource interface.
             */
            std::string getResourceInterface() const;

            /**
             * Gets the resource type.
             *
             * @return Resource type.
             */
            std::string getResourceType() const;

            /**
             * Gets query value for the given key.
             *
             * @param key key of the element whose mapped value is accessed.
             *
             * @return Value associated with given key.
             *
             * @throws InvalidKeyException If @a key doesn't match the key of any value.
             */
            std::string get(const std::string& key) const;

            /**
             * Get all the query params.
             *
             * @return Map containing all the queries [Key-Value pair].
             */
            const Map& getAll() const;

        private:
            std::string m_resourceInterface;
            std::string m_resourceType;

            std::unordered_map< std::string, std::string > m_map;
        };

        /**
         *
         * This represents a remote resource and provides simple ways to interact with it.
         * Basically this is a client of a remote resource that runs on other device.
         *
         * The class supports features to help get information of a remote resource
         * such as monitoring and caching.
         *
         * @see RCSDiscoveryManager
         *
         */
        class RCSRemoteResourceObject
        {
        public:
            typedef std::shared_ptr< RCSRemoteResourceObject > Ptr;

            /**
             * Callback definition to be invoked when monitoring state is changed.
             *
             * @see startMonitioring
             * @see ResourceState
             */
            typedef std::function< void(ResourceState) > StateChangedCallback;

            /**
             * Callback definition to be invoked when cache is updated.
             *
             * @param attrs the updated attributes
             */
            typedef std::function< void(const RCSResourceAttributes& attrs, int eCode) >
                CacheUpdatedCallback;

            /**
             * Callback definition to be invoked when the response of getRemoteAttributes is
             * received.
             *
             * @param attrs the result attributes
             * @param eCode the error code received from the resource
             *
             * @see getRemoteAttributes
             */
            typedef std::function< void(const RCSResourceAttributes& attrs, int eCode) >
                RemoteAttributesGetCallback;

            /**
             * Callback definition to be invoked when the response of get is received.
             *
             * @param HeaderOpts
             * @param rep the result representation
             * @param eCode the error code received from the resource
             *
             * @see get
             */
            typedef std::function< void(const HeaderOpts& headerOpts,
                    const RCSRepresentation& rep, int eCode) > GetCallback;

            /**
             * Callback definition to be invoked when the response of setRemoteAttributes is
             * received.
             *
             * @param attrs the result attributes
             * @param eCode the error code received from the resource
             *
             * @see setRemoteAttributes
             */
            typedef std::function< void(const RCSResourceAttributes& attrs, int eCode) >
                RemoteAttributesSetCallback;

            /**
             * Callback definition to be invoked when the response of set is received.
             *
             * @param HeaderOpts
             * @param rep the result representation
             * @param eCode the error code received from the resource
             *
             * @see set
             */
            typedef std::function< void(const HeaderOpts& headerOpts,
                    const RCSRepresentation& rep, int eCode) > SetCallback;

        private:
            typedef int CacheID;
            typedef unsigned int BrokerID;

        public:
            //! @cond
            RCSRemoteResourceObject(std::shared_ptr< PrimitiveResource >);
            //! @endcond

            ~RCSRemoteResourceObject();

            /**
             * Creates an RCSRemoteResourceObject instance from an OCResource instance.
             *
             * @return RCSRemoteResourceObject instance.
             *
             * @throw RCSInvalidParameterException If ocResource is nullptr.
             */
            static RCSRemoteResourceObject::Ptr fromOCResource(
                    std::shared_ptr< OC::OCResource > ocResource);

            /**
             * Converts RCSRemoteResourceObject instance to an equivalent OCResource.
             *
             * @return OCResource instance.
             *
             * @throw RCSInvalidParameterException If rcsResource is nullptr.
             */
            static std::shared_ptr< OC::OCResource > toOCResource(
                    RCSRemoteResourceObject::Ptr rcsResource);

            /**
             * Checks whether monitoring is started or not.
             *
             * @return True if monitoring started, otherwise false.
             *
             * @see startMonitoring()
             */
            bool isMonitoring() const;

            /**
             * Checks whether caching is started or not.
             *
             * @return True if caching started, otherwise false.
             *
             * @see startCaching()
             */

            bool isCaching() const;

            /**
             * Checks whether resource is observable or not.
             *
             * @return True if resource is observable, otherwise false.
             */
            bool isObservable() const;

            /**
             * Starts monitoring the resource.
             *
             * Monitoring provides a feature to check the presence of a resource,
             * even when the server is not announcing Presence using startPresnece.
             *
             * @param cb A Callback to get changed resource state.
             *
             * @throws InvalidParameterException If cb is an empty function or null.
             * @throws BadRequestException If monitoring is already started.
             *
             * @note The callback will be invoked in an internal thread.
             *
             * @see StateChangedCallback
             * @see ResourceState
             * @see isMonitoring()
             * @see stopMonitoring()
             *
             */
            void startMonitoring(StateChangedCallback cb);

            /**
             * Stops monitoring the resource.
             *
             * It does nothing if monitoring is not started.
             *
             * @see startMonitoring()
             *
             */
            void stopMonitoring();

            /**
             * Gets the current state of the resource.
             *
             * @return Current resource state.
             *
             * @see startMonitoring
             * @see ResourceState
             */
            ResourceState getState() const;

            /**
             * Starts caching attributes of the resource.
             *
             * This will start caching for the resource.
             * Once caching started it will look for the data updation on the resource
             * and updates the cache data accordingly.
             *
             * It is equivalent to calling startCaching(CacheUpdatedCallback) with an empty function.
             *
             * @see getCacheState()
             * @see getCachedAttributes()
             * @see getCachedAttribute(const std::string&) const
             *
             * @throws BadRequestException
             *
             */
            void startCaching();

            /**
             * Starts caching attributes for the resource.
             *
             * This will start data caching for the resource.
             * Once caching started it will look for the data updation on the resource and
             * updates the cached data accordingly.
             *
             * @param cb If non-empty function, it will be invoked whenever the cache updated.
             * @param mode if CacheMode is OBSERVE_ONLY, it will be invoked when receive observe response only.
             *
             * @throws BadRequestException If caching is already started.
             *
             * @note The callback will be invoked in an internal thread.
             *
             * @see CacheUpdatedCallback
             * @see getCacheState()
             * @see isCachedAvailable()
             * @see getCachedAttributes()
             * @see getCachedAttribute(const std::string&) const
             *
             */
            void startCaching(CacheUpdatedCallback cb, CacheMode mode = CacheMode::OBSERVE_WITH_POLLING);

            /**
             * Stops caching.
             *
             * It does nothing if caching is not started.
             *
             * @see startCaching()
             * @see startCaching(CacheUpdatedCallback)
             */
            void stopCaching();

            /**
             * Gets the current cache state.
             *
             * @return Current cache state.
             *
             * @see startCaching
             * @see CacheState
             *
             */
            CacheState getCacheState() const;

            /**
             * Checks whether cached data is available.
             *
             * Cache will be available always once cache state had been CacheState::READY
             * even if current state is CacheState::LOST_SIGNAL.
             *
             * @return True if caching data available, otherwise false.
             *
             * @see getCacheState()
             */
            bool isCachedAvailable() const;

            /**
             * Gets the cached RCSResourceAttributes data.
             *
             * @pre Cache should be available.
             *
             * @return The cached attributes.
             *
             * @throws BadRequestException If the precondition is not fulfilled.
             *
             * @see RCSResourceAttributes
             * @see isCachedAvailable()
             * @see startCaching()
             * @see startCaching(CacheUpdatedCallback)
             *
             */
            RCSResourceAttributes getCachedAttributes() const;

            /**
             * Gets a particular cached a ResourceAttribute Value.
             *
             * @pre Cache should be available.
             *
             * @return A requested attribute value.
             *
             * @throws BadRequestException If the precondition is not fulfilled.
             * @throws InvalidKeyException If @a key doesn't match the key of any value.
             *
             * @see RCSResourceAttributes::Value
             * @see isCachedAvailable()
             * @see startCaching()
             * @see startCaching(CacheUpdatedCallback)
             *
             */
            RCSResourceAttributes::Value getCachedAttribute(const std::string& key) const;

            /**
             * Gets resource attributes directly from the server.
             *
             * This API send a get request to the resource of interest and provides
             * the attributes to the caller in the RemoteAttributesGetCallback.
             *
             * @throws PlatformException If the operation failed
             * @throws InvalidParameterException If cb is an empty function or null.
             *
             * @note The callback will be invoked in an internal thread.
             */
            void getRemoteAttributes(RemoteAttributesGetCallback cb);

            /**
             * Gets resource representation with empty query parameters directly from the server.
             *
             * @param cb A callback to receive the response.
             *
             * @throws PlatformException If the operation failed
             * @throws InvalidParameterException If cb is an empty function or null.
             *
             * @note The callback will be invoked in an internal thread.
             */
            void get(GetCallback cb);

            /**
             * Gets resource representation directly from the server.
             *
             * The response could be different by the query parameters, it depends on server.
             *
             * @param queryParams Query parameters
             * @param cb A callback to receive the response.
             *
             * @throws PlatformException If the operation failed
             * @throws InvalidParameterException If cb is an empty function or null.
             *
             * @note The callback will be invoked in an internal thread.
             */
            void get(const RCSQueryParams& queryParams, GetCallback cb);

            /**
             * Sends a set request with resource attributes to the server.
             *
             * The SetRequest behavior depends on the server, whether updating its attributes or not.
             *
             * @param attributes Attributes to set
             * @param cb A callback to receive the response.
             *
             * @throws PlatformException If the operation failed
             * @throws InvalidParameterException If cb is an empty function or null.
             *
             * @see RCSResourceObject
             * @see RCSResourceObject::SetRequestHandlerPolicy
             *
             * @note The callback will be invoked in an internal thread.
             */
            void setRemoteAttributes(const RCSResourceAttributes& attributes,
                    RemoteAttributesSetCallback cb);

            /**
             * Sends a set request with resource attributes to the server.
             *
             * The SetRequest behavior depends on query parameters and the server.
             *
             * @param attributes Attributes to set
             * @param cb A callback to receive the response.
             *
             * @throws PlatformException If the operation failed
             * @throws InvalidParameterException If cb is an empty function or null.
             *
             * @see RCSResourceObject
             * @see RCSResourceObject::SetRequestHandlerPolicy
             *
             * @note The callback will be invoked in an internal thread.
             */
            void set(const RCSResourceAttributes& attributes, SetCallback cb);

            /**
             * Sends a set request with resource attributes to the server.
             *
             * The SetRequest behavior depends on query parameters and the server.
             *
             * @param queryParams Query parameters
             * @param attributes Attributes to set
             * @param cb A callback to receive the response.
             *
             * @throws PlatformException If the operation failed
             * @throws InvalidParameterException If cb is an empty function or null.
             *
             * @see RCSResourceObject
             * @see RCSResourceObject::SetRequestHandlerPolicy
             *
             * @note The callback will be invoked in an internal thread.
             */
            void set(const RCSQueryParams& queryParams, const RCSResourceAttributes& attributes,
                    SetCallback cb);

            /**
             * Sends a set request with resource representation to the server.
             *
             * The SetRequest behavior depends on query parameters and the server.
             *
             * @param queryParams Query parameters
             * @param rep Representation to set
             * @param cb A callback to receive the response.
             *
             * @throws PlatformException If the operation failed
             * @throws InvalidParameterException If cb is an empty function or null.
             *
             * @see RCSResourceObject
             * @see RCSResourceObject::SetRequestHandlerPolicy
             *
             * @note The callback will be invoked in an internal thread.
             */
            void set(const RCSQueryParams& queryParams, const RCSRepresentation &rep,
                    SetCallback cb);

            /**
             * Gets the resource URI.
             *
             * @return Resource URI.
             */
            std::string getUri() const;

            /**
             * Gets the resource address.
             *
             * @return Resource address.
             */
            std::string getAddress() const;

            /**
             * Gets the resource types.
             *
             * @return Vector containing resource types.
             */
            std::vector< std::string > getTypes() const;

            /**
             * Gets the resource interfaces.
             *
             * @return Vector containing resource interfaces.
             */
            std::vector< std::string > getInterfaces() const;

        private:
            std::shared_ptr< PrimitiveResource > m_primitiveResource;
            CacheID m_cacheId;
            BrokerID m_brokerId;
        };
    }
}
#endif // RCSREMOTERESOURCEOBJECT_H
