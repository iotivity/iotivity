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

#include <internal/RequestHandler.h>

#include <internal/ResourceAttributesConverter.h>
#include <internal/ResourceAttributesUtils.h>

namespace OIC
{
    namespace Service
    {

        constexpr OCEntityHandlerResult RequestHandler::DEFAULT_RESULT;

        SimpleRequestHandler::SimpleRequestHandler(const OCEntityHandlerResult& result,
                int errorCode) :
                m_result{ result }, m_errorCode{ errorCode }
        {
        }

        std::shared_ptr< OC::OCResourceResponse > SimpleRequestHandler::buildResponse(
                ResourceObject& resource, const ResourceAttributes&)
        {
            auto response = std::make_shared< OC::OCResourceResponse >();

            response->setErrorCode(getErrorCode(resource));
            response->setResponseResult(getResponseResult(resource));
            response->setResourceRepresentation(getOCRepresentation(resource));

            return response;
        }

        int SimpleRequestHandler::getErrorCode(ResourceObject&)
        {
            return m_errorCode;
        }

        OCEntityHandlerResult SimpleRequestHandler::getResponseResult(ResourceObject&)
        {
            return m_result;
        }

        OC::OCRepresentation SimpleRequestHandler::getOCRepresentation(
                ResourceObject& resource)
        {
            ResourceObject::LockGuard lock{ resource };
            return ResourceAttributesConverter::toOCRepresentation(resource.getAttributes());
        }


        OC::OCRepresentation CustomAttrRequestHandler::getOCRepresentation(
                ResourceObject& resource)
        {
            return ResourceAttributesConverter::toOCRepresentation(m_attrs);
        }


        SetRequestProxyHandler::SetRequestProxyHandler(RequestHandler::Ptr requestHandler) :
                m_requestHandler{ requestHandler }
        {
            assert(m_requestHandler);
        }

        std::shared_ptr< OC::OCResourceResponse > SetRequestProxyHandler::buildResponse(
                ResourceObject& resource, const ResourceAttributes& requestAttrs)
        {
            {
                ResourceObject::LockGuard lock(resource);

                if (!acceptableAttributes(resource.getAttributes(), requestAttrs))
                {
                    throw PrimitiveException("Resource can't accept request Attributes!");
                }

                replaceAttributesRecursively(resource.getAttributes(), requestAttrs);
            }
            return m_requestHandler->buildResponse(resource, requestAttrs);
        }

    }
}
