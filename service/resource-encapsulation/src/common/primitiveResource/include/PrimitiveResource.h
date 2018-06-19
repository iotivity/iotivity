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
 * This file contains the declaration of classes and its members related to Primitive resource.
 */

#ifndef COMMON_PRIMITIVERESOURCE_H
#define COMMON_PRIMITIVERESOURCE_H

#include <functional>
#include <string>
#include <vector>

#include "OCResource.h"

#include "ResponseStatement.h"
#include "RCSAddress.h"

namespace OIC
{
    namespace Service
    {

        typedef OC::HeaderOption::OCHeaderOption HeaderOption;
        typedef std::vector<HeaderOption> HeaderOptions;

        class RCSResourceAttributes;
        class RCSRepresentation;

        /**
         * This class represents resource of resource-encapsulation layer.
         * This class is common for both server and client side.
         */
        class PrimitiveResource: public std::enable_shared_from_this< PrimitiveResource >
        {
        public:
            typedef std::shared_ptr< PrimitiveResource > Ptr;
            typedef std::shared_ptr< const PrimitiveResource > ConstPtr;

            /**
             * Callback function for get request.
             */
            typedef std::function<void(const HeaderOptions&, const RCSRepresentation&, int)>
                    GetCallback;

            /**
             * Callback function for set request.
             */
            typedef std::function<void(const HeaderOptions&, const RCSRepresentation&, int)>
                    SetCallback;

            /**
             * Callback function for put request.
             */
            typedef std::function<void(const HeaderOptions&, const RCSRepresentation&, int)>
                    PutCallback;

            /**
             * Callback function for observe request.
             */
            typedef std::function<void(const HeaderOptions&, const RCSRepresentation&, int, int)>
                    ObserveCallback;

        public:

            /**
             * Create primitive resource from given OCResource.
             */
            static PrimitiveResource::Ptr create(const std::shared_ptr<OC::OCResource>&);

            /**
             * Destructor for Primitive Resource.
             */
            virtual ~PrimitiveResource() { }

           /**
            * Sends a get request to primitive resource.
            *
            * @param cb A callback to receive the response.
            *
            * @see GetCallback
            */
            virtual void requestGet(GetCallback cb) = 0;

          /**
            * Sends a get request with resource type, interface and query params
            * to resource.
            *
            * @param resourceType Resource type.
            * @param resourceInterface Resource interface.
            * @param queryParametersMap Query parameters to be sent.
            * @param cb A callback to receive the response.
            *
            * @see GetCallback
            */
            virtual void requestGetWith(const std::string& resourceType,
                    const std::string& resourceInterface,
                    const OC::QueryParamsMap& queryParametersMap, GetCallback cb) = 0;

            /**
             * Sends a set request with resource attributes to resource.
             *
             * @param attributes Resource attributes to be sent.
             * @param cb A callback to receive the response.
             *
             * @see RCSResourceAttributes
             * @see SetCallback
             */
            virtual void requestSet(const RCSResourceAttributes& attributes, SetCallback cb) = 0;

           /**
            * Sends a set request with resource attributes, type, interface and
            * query params to resource.
            *
            * @param resourceType Resource type.
            * @param resourceInterface Resource interface.
            * @param queryParametersMap Query parameters to be sent.
            * @param attributes Resource attributes to be sent.
            * @param cb A callback to receive the response.
            *
            * @see RCSResourceAttributes
            */
            virtual void requestSetWith(const std::string& resourceType,
                    const std::string& resourceInterface,
                    const OC::QueryParamsMap& queryParametersMap,
                    const RCSResourceAttributes& attributes, GetCallback cb) = 0;

           /**
            * Sends a set request with representation, type, interface and
            * query params to resource.
            *
            * @param resourceType Resource type.
            * @param resourceInterface Resource interface.
            * @param queryParametersMap Query parameters to be sent.
            * @param rep Representation to be sent.
            * @param cb A callback to receive the response.
            *
            * @see SetCallback
            * @see RCSRepresentation
            */
            virtual void requestSetWith(const std::string& resourceType,
                    const std::string& resourceInterface,
                    const OC::QueryParamsMap& queryParametersMap,
                    const RCSRepresentation& rep, SetCallback cb) = 0;

            /**
             * Sends a put request with resource attributes to resource.
             *
             * @param attributes Resource attributes to be sent.
             * @param cb A callback to receive the response.
             *
             * @see RCSResourceAttributes
             * @see PutCallback
             */
            virtual void requestPut(const RCSResourceAttributes& attributes, PutCallback cb) = 0;

            /**
             * Sends an observe requet to resource.
             *
             * @param cb A callback to receive the response.
             *
             * @see ObserveCallback
             */
            virtual void requestObserve(ObserveCallback cb) = 0;

            /**
             * Sends an cancel observe requet to resource.
             *
             */
            virtual void cancelObserve() = 0;

            /**
             * Gets sid of resource.
             *
             * @return Resource sid.
             */
            virtual std::string getSid() const = 0;

            /**
             * Gets resource URI.
             *
             * @return Resource URI.
             */
            virtual std::string getUri() const = 0;

            /**
             * Gets resource host address.
             *
             * @return Resource host address.
             */
            virtual std::string getHost() const = 0;

            /**
             * Gets resource types.
             *
             * @return Vector containing resource types.
             */
            virtual std::vector< std::string > getTypes() const = 0;

            /**
             * Gets resource interfaces.
             *
             * @return Vector containing resource interfaces.
             */
            virtual std::vector< std::string > getInterfaces() const = 0;

            /**
             * Gets resource Connectivity type.
             *
             * @return Connectivity type.
             *
             * @see OCConnectivityType
             */
            virtual OCConnectivityType getConnectivityType() const = 0;

            /**
             * Check whether the resource is observable or not.
             *
             * @return True if resource is observable, otherwise false.
             */
            virtual bool isObservable() const = 0;

        protected:
            PrimitiveResource() = default;

            PrimitiveResource(const PrimitiveResource&) = delete;
            PrimitiveResource(PrimitiveResource&&) = delete;

            PrimitiveResource& operator=(const PrimitiveResource&) = delete;
            PrimitiveResource& operator=(PrimitiveResource&&) = delete;
        };

        typedef std::function<void(std::shared_ptr<PrimitiveResource>)> DiscoverCallback;

        void discoverResource(const std::string& host, const std::string& resourceURI,
                OCConnectivityType, DiscoverCallback);

        void discoverResource(const RCSAddress& address, const std::string& resourceURI,
                DiscoverCallback);

    }
}

#endif // COMMON_PRIMITIVERESOURCE_H
