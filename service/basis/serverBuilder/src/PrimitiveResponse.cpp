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
            static PrimitiveGetResponse defaultRes { new SimpleRequestHandler };

            return defaultRes;
        }

        PrimitiveGetResponse PrimitiveGetResponse::create(const OCEntityHandlerResult& result,
                int errorCode)
        {
            return PrimitiveGetResponse { new SimpleRequestHandler { result, errorCode } };
        }

        PrimitiveGetResponse PrimitiveGetResponse::create(const ResourceAttributes& attrs)
        {
            return PrimitiveGetResponse { new CustomAttrRequestHandler { attrs } };
        }

        PrimitiveGetResponse PrimitiveGetResponse::create(const ResourceAttributes& attrs,
                const OCEntityHandlerResult& result, int errorCode)
        {
            return PrimitiveGetResponse {
                new CustomAttrRequestHandler { attrs, result, errorCode } };
        }

        PrimitiveGetResponse PrimitiveGetResponse::create(ResourceAttributes&& result)
        {
            return PrimitiveGetResponse { new CustomAttrRequestHandler { std::move(result) } };
        }

        PrimitiveGetResponse PrimitiveGetResponse::create(ResourceAttributes&& attrs,
                const OCEntityHandlerResult& result, int errorCode)
        {
            return PrimitiveGetResponse { new CustomAttrRequestHandler {
                std::move(attrs), result, errorCode } };
        }

        PrimitiveGetResponse::PrimitiveGetResponse(RequestHandler* handler) :
                m_handler{ handler }
        {
            assert(m_handler);
        }

        RequestHandler* PrimitiveGetResponse::getHandler() const
        {
            return m_handler.get();
        }


        PrimitiveSetResponse PrimitiveSetResponse::defaultAction()
        {
            static PrimitiveSetResponse defaultRes { withProxy(new SimpleRequestHandler) };

            return defaultRes;
        }

        PrimitiveSetResponse PrimitiveSetResponse::create(const OCEntityHandlerResult& result,
                int errorCode)
        {
            return withProxy(new SimpleRequestHandler { result, errorCode });
        }

        PrimitiveSetResponse PrimitiveSetResponse::create(const ResourceAttributes& attrs)
        {
            return withProxy(new CustomAttrRequestHandler { attrs });
        }

        PrimitiveSetResponse PrimitiveSetResponse::create(const ResourceAttributes& attrs,
                const OCEntityHandlerResult& result, int errorCode)
        {
            return withProxy(new CustomAttrRequestHandler { attrs, result, errorCode });
        }

        PrimitiveSetResponse PrimitiveSetResponse::create(ResourceAttributes&& result)
        {
            return withProxy(new CustomAttrRequestHandler { std::move(result) });
        }

        PrimitiveSetResponse PrimitiveSetResponse::create(ResourceAttributes&& attrs,
                const OCEntityHandlerResult& result, int errorCode)
        {
            return withProxy(
                new CustomAttrRequestHandler { std::move(attrs), result, errorCode });
        }

        PrimitiveSetResponse::PrimitiveSetResponse(RequestHandler* handler) :
                m_handler{ handler }
        {
            assert(m_handler);
        }

        PrimitiveSetResponse PrimitiveSetResponse::withProxy(RequestHandler* handler)
        {
            assert(handler);
            return PrimitiveSetResponse{ new SetRequestProxyHandler {
                RequestHandler::Ptr{ handler } } };
        }

        RequestHandler* PrimitiveSetResponse::getHandler() const
        {
            return m_handler.get();
        }
    }
}
