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

#ifndef __PRIMITIVERESOURCEIMPL_H
#define __PRIMITIVERESOURCEIMPL_H

#include <PrimitiveResource.h>
#include <ResponseStatement.h>

#include <internal/ResourceAtrributesConverter.h>

namespace OIC
{
    namespace Service
    {

        template< typename BaseResource >
        class PrimitiveResourceImpl: public PrimitiveResource
        {
        private:
            using BaseResourcePtr = std::shared_ptr< BaseResource >;

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

            void requestGet(GetCallback callback) override
            {
                using namespace std::placeholders;

                m_baseResource->get(OC::QueryParamsMap(),
                        std::bind(callback, _1, std::bind(createResponseStatement, _2), _3));
            }

            void requestSet(const ResourceAttributes& attrs, SetCallback callback) override
            {
                using namespace std::placeholders;

                m_baseResource->put(ResourceAttributesConverter::toOCRepresentation(attrs),
                        OC::QueryParamsMap{},
                        std::bind(callback, _1, std::bind(createResponseStatement, _2), _3));
            }

            void requestObserve(ObserveCallback callback) override
            {
                using namespace std::placeholders;

                m_baseResource->observe(OC::ObserveType::ObserveAll, OC::QueryParamsMap{ },
                        bind(callback, _1, bind(createResponseStatement, _2), _3, _4));
            }

            void cancelObserve() override
            {
                m_baseResource->cancelObserve();
            }

            std::string getUri() const override
            {
                return m_baseResource->uri();
            }

            std::string getHost() const override
            {
                return m_baseResource->host();
            }

            std::vector< std::string > getTypes() const override
            {
                return m_baseResource->getResourceTypes();
            }

            std::vector< std::string > getInterfaces() const override
            {
                return m_baseResource->getResourceInterfaces();
            }

            bool isObservable() const override
            {
                return m_baseResource->isObservable();
            }

        private:
            BaseResourcePtr m_baseResource;
        };

    }
}

#endif // __PRIMITIVERESOURCEIMPL_H
