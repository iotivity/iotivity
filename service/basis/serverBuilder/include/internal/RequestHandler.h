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

#include <PrimitiveServerResource.h>

#include <internal/ResourceAtrributesConverter.h>

#include <OCResourceResponse.h>

class RequestHandler
{
public:
    virtual ~RequestHandler()
    {
    }

    virtual std::shared_ptr< OC::OCResourceResponse > buildResponse(PrimitiveServerResource&) = 0;
};

class SimpleRequestHandler: public RequestHandler
{
public:
    SimpleRequestHandler(const OCEntityHandlerResult& result = OC_EH_OK, int errorCode = 200) :
        m_result{ result }, m_errorCode{ errorCode }
    {
    }

    std::shared_ptr< OC::OCResourceResponse > buildResponse(PrimitiveServerResource& resource) override
    {
        auto response = std::make_shared< OC::OCResourceResponse >();

        response->setErrorCode(getErrorCode(resource));
        response->setResponseResult(getResponseResult(resource));
        response->setResourceRepresentation(getOCRepresentation(resource));

        return response;
    }

protected:
    virtual int getErrorCode(PrimitiveServerResource&)
    {
        return m_errorCode;
    }

    virtual OCEntityHandlerResult getResponseResult(PrimitiveServerResource&)
    {
        return m_result;
    }

    virtual OC::OCRepresentation getOCRepresentation(PrimitiveServerResource& resource)
    {
        PrimitiveServerResource::LockGuard lock{ resource };
        return ResourceAttributesConverter::toOCRepresentation(resource.getAttributes());
    }

private:
    OCEntityHandlerResult m_result;
    int m_errorCode;
};


class CustomAttrRequestHandler: public SimpleRequestHandler
{
public:
    template <typename T>
    CustomAttrRequestHandler(T&& attrs,
            const OCEntityHandlerResult& result = OC_EH_OK, int errorCode = 200) :
        SimpleRequestHandler{ result, errorCode }, m_attrs{ std::forward<T>(attrs) }
    {
    }

protected:
    virtual OC::OCRepresentation getOCRepresentation(PrimitiveServerResource& resource) override
    {
        return ResourceAttributesConverter::toOCRepresentation(m_attrs);
    }

private:
    ResourceAttributes m_attrs;
};

#endif // __REQUESTHANDLER_H
