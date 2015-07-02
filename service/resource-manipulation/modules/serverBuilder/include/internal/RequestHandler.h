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

#ifndef __REQUESTHANDLER_H
#define __REQUESTHANDLER_H

#include <ResourceObject.h>
#include <internal/ResourceAtrributesConverter.h>

#include <OCResourceResponse.h>

namespace OIC
{
    namespace Service
    {
        class RequestHandler
        {
        public:
            using Ptr = std::shared_ptr< RequestHandler >;

            static constexpr int DEFAULT_ERROR_CODE = 200;
            static constexpr OCEntityHandlerResult DEFAULT_RESULT = OC_EH_OK;

            virtual ~RequestHandler()
            {
            }

            virtual std::shared_ptr< OC::OCResourceResponse > buildResponse(
                    ResourceObject&, const ResourceAttributes& requestAttrs) = 0;
        };

        class SimpleRequestHandler: public RequestHandler
        {
        public:
            SimpleRequestHandler(const OCEntityHandlerResult& result = DEFAULT_RESULT,
                    int errorCode = DEFAULT_ERROR_CODE);

            std::shared_ptr< OC::OCResourceResponse > buildResponse(
                    ResourceObject&, const ResourceAttributes&) override;

        protected:
            virtual int getErrorCode(ResourceObject&);
            virtual OCEntityHandlerResult getResponseResult(ResourceObject&);
            virtual OC::OCRepresentation getOCRepresentation(ResourceObject& resource);

        private:
            OCEntityHandlerResult m_result;
            int m_errorCode;
        };


        class CustomAttrRequestHandler: public SimpleRequestHandler
        {
        public:
            template <typename T>
            CustomAttrRequestHandler(T&& attrs,
                    const OCEntityHandlerResult& result = DEFAULT_RESULT,
                    int errorCode = DEFAULT_ERROR_CODE) :
                SimpleRequestHandler{ result, errorCode }, m_attrs{ std::forward<T>(attrs) }
            {
            }

        protected:
            OC::OCRepresentation getOCRepresentation(ResourceObject& resource) override;

        private:
            ResourceAttributes m_attrs;
        };

        class SetRequestProxyHandler: public RequestHandler
        {
        public:
            SetRequestProxyHandler(RequestHandler::Ptr requestHandler);

            std::shared_ptr< OC::OCResourceResponse > buildResponse(
                    ResourceObject& resource,
                    const ResourceAttributes& requestAttrs) override;

        private:
            RequestHandler::Ptr m_requestHandler;
        };
    }
}

#endif // __REQUESTHANDLER_H
