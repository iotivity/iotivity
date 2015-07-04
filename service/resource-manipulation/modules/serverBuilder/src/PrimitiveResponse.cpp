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

#include <PrimitiveResponse.h>

#include <internal/RequestHandler.h>

namespace OIC
{
    namespace Service
    {
        PrimitiveGetResponse PrimitiveGetResponse::defaultAction()
        {
            static PrimitiveGetResponse defaultRes { std::make_shared< RequestHandler >() };

            return defaultRes;
        }

        PrimitiveGetResponse PrimitiveGetResponse::create(const OCEntityHandlerResult& result,
                int errorCode)
        {
            return PrimitiveGetResponse {
                std::make_shared< RequestHandler >( result, errorCode) };
        }

        PrimitiveGetResponse PrimitiveGetResponse::create(const ResourceAttributes& attrs)
        {
            return PrimitiveGetResponse { std::make_shared< RequestHandler >(attrs) };
        }

        PrimitiveGetResponse PrimitiveGetResponse::create(const ResourceAttributes& attrs,
                const OCEntityHandlerResult& result, int errorCode)
        {
            return PrimitiveGetResponse {
                std::make_shared< RequestHandler >(attrs, result, errorCode) };
        }

        PrimitiveGetResponse PrimitiveGetResponse::create(ResourceAttributes&& result)
        {
            return PrimitiveGetResponse {
                std::make_shared< RequestHandler >(std::move(result)) };
        }

        PrimitiveGetResponse PrimitiveGetResponse::create(ResourceAttributes&& attrs,
                const OCEntityHandlerResult& result, int errorCode)
        {
            return PrimitiveGetResponse { std::make_shared< RequestHandler >(
                std::move(attrs), result, errorCode) };
        }

        PrimitiveGetResponse::PrimitiveGetResponse(std::shared_ptr< RequestHandler >&& handler) :
                m_handler{ std::move(handler) }
        {
            assert(m_handler);
        }

        RequestHandler* PrimitiveGetResponse::getHandler() const
        {
            return m_handler.get();
        }


        PrimitiveSetResponse PrimitiveSetResponse::defaultAction()
        {
            return std::make_shared< SetRequestHandler >();
        }

        PrimitiveSetResponse PrimitiveSetResponse::accept()
        {
            return defaultAction().setAcceptanceMethod(AcceptanceMethod::ACCEPT);
        }

        PrimitiveSetResponse PrimitiveSetResponse::accept(const OCEntityHandlerResult& result,
                int errorCode)
        {
            return create(result, errorCode).setAcceptanceMethod(AcceptanceMethod::ACCEPT);
        }

        PrimitiveSetResponse PrimitiveSetResponse::ignore()
        {
            return defaultAction().setAcceptanceMethod(AcceptanceMethod::IGNORE);
        }

        PrimitiveSetResponse PrimitiveSetResponse::ignore(const OCEntityHandlerResult& result,
                int errorCode)
        {
            return create(result, errorCode).setAcceptanceMethod(AcceptanceMethod::IGNORE);
        }

        PrimitiveSetResponse PrimitiveSetResponse::create(const OCEntityHandlerResult& result,
                int errorCode)
        {
            return std::make_shared< SetRequestHandler >(result, errorCode);
        }

        PrimitiveSetResponse PrimitiveSetResponse::create(const ResourceAttributes& attrs)
        {
            return std::make_shared< SetRequestHandler >(attrs);
        }

        PrimitiveSetResponse PrimitiveSetResponse::create(const ResourceAttributes& attrs,
                const OCEntityHandlerResult& result, int errorCode)
        {
            return std::make_shared< SetRequestHandler >(attrs, result, errorCode);
        }

        PrimitiveSetResponse PrimitiveSetResponse::create(ResourceAttributes&& result)
        {
            return std::make_shared< SetRequestHandler >(std::move(result));
        }

        PrimitiveSetResponse PrimitiveSetResponse::create(ResourceAttributes&& attrs,
                const OCEntityHandlerResult& result, int errorCode)
        {
            return std::make_shared< SetRequestHandler >(std::move(attrs), result, errorCode);
        }

        PrimitiveSetResponse::PrimitiveSetResponse(std::shared_ptr< SetRequestHandler >&& handler) :
                PrimitiveSetResponse{ std::move(handler), AcceptanceMethod::DEFAULT }
        {
        }

        PrimitiveSetResponse::PrimitiveSetResponse(std::shared_ptr< SetRequestHandler >&& handler,
                AcceptanceMethod method) :
                m_acceptanceMethod{ method },
                m_handler{ std::move(handler) }
        {
            assert(m_handler);
        }

        SetRequestHandler* PrimitiveSetResponse::getHandler() const
        {
            return m_handler.get();
        }

        auto PrimitiveSetResponse::getAcceptanceMethod() const -> AcceptanceMethod
        {
            return m_acceptanceMethod;
        }

        PrimitiveSetResponse& PrimitiveSetResponse::setAcceptanceMethod(AcceptanceMethod method)
        {
            m_acceptanceMethod = method;
            return *this;
        }
    }
}
