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

#ifndef COMMON_INTERNAL_PRIMITIVERESOURCEIMPL_H
#define COMMON_INTERNAL_PRIMITIVERESOURCEIMPL_H

#include <PrimitiveResource.h>
#include <ResponseStatement.h>
#include <internal/AssertUtils.h>

#include <internal/ResourceAttributesConverter.h>

namespace OIC
{
    namespace Service
    {

        template< typename BaseResource >
        class PrimitiveResourceImpl: public PrimitiveResource
        {
        private:
            typedef std::shared_ptr< BaseResource > BaseResourcePtr;

        private:
            static ResponseStatement createResponseStatement(
                    const OC::OCRepresentation& ocRepresentation)
            {
                return ResponseStatement::create(
                        ResourceAttributesConverter::fromOCRepresentation(ocRepresentation));
            }

        public:
            PrimitiveResourceImpl(const BaseResourcePtr& baseResource) :
                    m_baseResource{ baseResource }
            {
            }

            void requestGet(GetCallback callback)
            {
                using namespace std::placeholders;

                typedef OCStackResult(BaseResource::*GetFunc)(
                        const OC::QueryParamsMap&, OC::GetCallback);

                invokeOC(m_baseResource, static_cast< GetFunc >(&BaseResource::get),
                        OC::QueryParamsMap(), std::bind(std::move(callback), _1,
                                std::bind(createResponseStatement, _2), _3));
            }

            void requestSet(const ResourceAttributes& attrs, SetCallback callback)
            {
                using namespace std::placeholders;

                typedef OCStackResult(BaseResource::*PutFunc)(
                        const OC::OCRepresentation&,
                        const OC::QueryParamsMap&, OC::PutCallback);

                invokeOC(m_baseResource, static_cast< PutFunc >(&BaseResource::put),
                        ResourceAttributesConverter::toOCRepresentation(attrs),
                        OC::QueryParamsMap{ }, std::bind(std::move(callback), _1,
                                std::bind(createResponseStatement, _2), _3));
            }

            void requestObserve(ObserveCallback callback)
            {
                using namespace std::placeholders;

                typedef OCStackResult (BaseResource::*ObserveFunc)(OC::ObserveType,
                        const OC::QueryParamsMap&, OC::ObserveCallback);

                invokeOC(m_baseResource, static_cast< ObserveFunc >(&BaseResource::observe),
                        OC::ObserveType::ObserveAll, OC::QueryParamsMap{ },
                        bind(std::move(callback), _1, bind(createResponseStatement, _2), _3, _4));
            }

            void cancelObserve()
            {
                typedef OCStackResult (BaseResource::*CancelObserveFunc)();

                invokeOC(m_baseResource,
                        static_cast< CancelObserveFunc >(&BaseResource::cancelObserve));
            }

            std::string getSid() const
            {
                return invokeOC(m_baseResource, &BaseResource::sid);
            }

            std::string getUri() const
            {
                return invokeOC(m_baseResource, &BaseResource::uri);
            }

            std::string getHost() const
            {
                return invokeOC(m_baseResource, &BaseResource::host);
            }

            std::vector< std::string > getTypes() const
            {
                return invokeOC(m_baseResource, &BaseResource::getResourceTypes);
            }

            std::vector< std::string > getInterfaces() const
            {
                return invokeOC(m_baseResource, &BaseResource::getResourceInterfaces);
            }

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
