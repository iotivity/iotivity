/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef COMMON_INTERNAL_PRIMITIVERESOURCEIMPL_H
#define COMMON_INTERNAL_PRIMITIVERESOURCEIMPL_H

#include "PrimitiveResource.h"
#include "AssertUtils.h"

#include "ResourceAttributesConverter.h"

/** OIC namespace */
namespace OIC
{
    /** service namespace */
    namespace Service
    {

        template< typename BaseResource >
        class PrimitiveResourceImpl: public PrimitiveResource
        {
        private:
            typedef std::shared_ptr< BaseResource > BaseResourcePtr;

        private:
            static RCSRepresentation convertRepresentation(
                    const OC::OCRepresentation& rep)
            {
                return RCSRepresentation::fromOCRepresentation(rep);
            }

            template< typename CALLBACK, typename ...ARGS >
            static inline void checkedCall(const std::weak_ptr< const PrimitiveResource >& resource,
                    const CALLBACK& cb, ARGS&&... args)
            {
                auto checkedRes = resource.lock();

                if (!checkedRes)
                {
                    return;
                }

                cb(std::forward< ARGS >(args)...);
            }

            template< typename CALLBACK >
            static void safeCallback(const std::weak_ptr< const PrimitiveResource >& resource,
                    const CALLBACK& cb, const HeaderOptions& headerOptions,
                    const OC::OCRepresentation& rep, int errorCode)
            {
                checkedCall(resource, cb, headerOptions, convertRepresentation(rep), errorCode);
            }

            static void safeObserveCallback(const std::weak_ptr< const PrimitiveResource >& res,
                    const PrimitiveResource::ObserveCallback& cb,
                    const HeaderOptions& headerOptions, const OC::OCRepresentation& rep,
                    int errorCode, int sequenceNumber)
            {
                checkedCall(res, cb, headerOptions, convertRepresentation(rep), errorCode,
                        sequenceNumber);
            }

            std::weak_ptr< PrimitiveResource > WeakFromThis()
            {
                return shared_from_this();
            }

            std::weak_ptr< const PrimitiveResource > WeakFromThis() const
            {
                return shared_from_this();
            }

        public:
            PrimitiveResourceImpl(const BaseResourcePtr& baseResource) :
                    m_baseResource{ baseResource }
            {
            }

            /**
             * This API is for the get request
             * @param callback  get callback
             */
            void requestGet(GetCallback callback)
            {
                requestGetWith("", "", {}, std::move(callback));
            }

            void requestGetWith(const std::string& resourceType,
                    const std::string& resourceInterface,
                    const OC::QueryParamsMap& queryParametersMap, GetCallback callback)
            {
                using namespace std::placeholders;

                typedef OCStackResult(BaseResource::*GetFunc)(
                        const std::string&, const std::string&,
                        const OC::QueryParamsMap&, OC::GetCallback);

                invokeOC(m_baseResource, static_cast< GetFunc >(&BaseResource::get),
                        resourceType, resourceInterface, queryParametersMap,
                        std::bind(safeCallback< GetCallback >, WeakFromThis(),
                                std::move(callback), _1, _2, _3));
            }

            /**
             * This API is for the set request
             * @param attrs     resource attributes
             * @param callback  set callback
             */
            void requestSet(const RCSResourceAttributes& attrs, SetCallback callback)
            {
                requestSetWith("", "", {}, attrs, std::move(callback));
            }

            void requestSetWith(const std::string& resourceType,
                          const std::string& resourceInterface,
                          const OC::QueryParamsMap& queryParametersMap,
                          const RCSResourceAttributes& attrs, GetCallback callback)
            {
                using namespace std::placeholders;

                typedef OCStackResult (BaseResource::*PostFunc)(const std::string&,
                        const std::string&, const OC::OCRepresentation&, const OC::QueryParamsMap&,
                        OC::GetCallback);

                invokeOC(m_baseResource, static_cast< PostFunc >(&BaseResource::post),
                        resourceType, resourceInterface,
                        ResourceAttributesConverter::toOCRepresentation(attrs), queryParametersMap,
                        std::bind(safeCallback< SetCallback >, WeakFromThis(), std::move(callback),
                                _1, _2, _3));
            }

            void requestSetWith(const std::string& resourceType,
                          const std::string& resourceInterface,
                          const OC::QueryParamsMap& queryParametersMap,
                          const RCSRepresentation & rep, SetCallback callback)
            {
                using namespace std::placeholders;

                typedef OCStackResult (BaseResource::*PostFunc)(const std::string&,
                        const std::string&, const OC::OCRepresentation&, const OC::QueryParamsMap&,
                        OC::GetCallback);

                invokeOC(m_baseResource, static_cast< PostFunc >(&BaseResource::post),
                        resourceType, resourceInterface,
                        RCSRepresentation::toOCRepresentation(rep), queryParametersMap,
                        std::bind(safeCallback< SetCallback >, WeakFromThis(), std::move(callback),
                                _1, _2, _3));
            }

            /**
             * This API is for the put request
             * @param attrs     resource attributes
             * @param callback  put callback
             */
            void requestPut(const RCSResourceAttributes& attrs, PutCallback callback)
            {
                using namespace std::placeholders;

                typedef OCStackResult(BaseResource::*PutFunc)(
                        const OC::OCRepresentation&,
                        const OC::QueryParamsMap&, OC::PutCallback);

                invokeOC(m_baseResource, static_cast< PutFunc >(&BaseResource::put),
                        ResourceAttributesConverter::toOCRepresentation(attrs),
                        OC::QueryParamsMap{ },
                        std::bind(safeCallback< PutCallback >, WeakFromThis(),
                                std::move(callback), _1, _2, _3));
            }

            void requestObserve(ObserveCallback callback)
            {
                using namespace std::placeholders;

                typedef OCStackResult (BaseResource::*ObserveFunc)(OC::ObserveType,
                        const OC::QueryParamsMap&, OC::ObserveCallback);

                invokeOC(m_baseResource, static_cast< ObserveFunc >(&BaseResource::observe),
                        OC::ObserveType::ObserveAll, OC::QueryParamsMap{ },
                        std::bind(safeObserveCallback, WeakFromThis(),
                                std::move(callback), _1, _2, _3, _4));
            }

            /**
             * This API is to cancle the resource observe
             */
            void cancelObserve()
            {
                typedef OCStackResult (BaseResource::*CancelObserveFunc)();

                invokeOC(m_baseResource,
                        static_cast< CancelObserveFunc >(&BaseResource::cancelObserve));
            }

            /**
             * This API is to get the sid value of resource
             * @return resource sid value
             */
            std::string getSid() const
            {
                return invokeOC(m_baseResource, &BaseResource::sid);
            }

            /**
             * API to get the resource URI
             * @return resource uri
             */
            std::string getUri() const
            {
                return invokeOC(m_baseResource, &BaseResource::uri);
            }

            /**
             * API to get the host information
             * @return host information
             */
            std::string getHost() const
            {
                return invokeOC(m_baseResource, &BaseResource::host);
            }

            /**
             * API to get the resource type
             * @return resource type
             */
            std::vector< std::string > getTypes() const
            {
                return invokeOC(m_baseResource, &BaseResource::getResourceTypes);
            }

            /**
             * API is to get the resource interface details
             * @return resource interface detail
             */
            std::vector< std::string > getInterfaces() const
            {
                return invokeOC(m_baseResource, &BaseResource::getResourceInterfaces);
            }

            /**
             * API is to get the resource connectivity type
             * @return resource connectivity type
             */
            OCConnectivityType getConnectivityType() const
            {
                return invokeOC(m_baseResource, &BaseResource::connectivityType);
            }

            /**
             * API to get the observable state of resource
             * @return true or false
             */
            bool isObservable() const
            {
                return invokeOC(m_baseResource, &BaseResource::isObservable);
            }

        private:
            BaseResourcePtr m_baseResource;
        };

    }
}

#endif // COMMON_INTERNAL_PRIMITIVERESOURCEIMPL_H
