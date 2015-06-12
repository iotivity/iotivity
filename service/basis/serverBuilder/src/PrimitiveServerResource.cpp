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

#include <PrimitiveServerResource.h>

#include <string>
#include <functional>
#include <vector>

#include <internal/RequestHandler.h>
#include <internal/AssertUtils.h>

#include <OCPlatform.h>

namespace
{
    using namespace OIC::Service;

    namespace Detail
    {
        template <typename RESPONSE>
        OCEntityHandlerResult sendResponse(PrimitiveServerResource& resource,
                std::shared_ptr< OC::OCResourceRequest > ocRequest,
                const ResourceAttributes& requestAttrs, RESPONSE&& response)
        {
            auto ocResponse = response.getHandler()->buildResponse(resource, requestAttrs);

            ocResponse->setRequestHandle(ocRequest->getRequestHandle());
            ocResponse->setResourceHandle(ocRequest->getResourceHandle());

            if (OC::OCPlatform::sendResponse(ocResponse) == OC_STACK_OK)
            {
                return OC_EH_OK;
            }
            return OC_EH_ERROR;
        }
    }

    inline bool hasProperty(uint8_t base, uint8_t target)
    {
        return (base & target) == target;
    }

    inline uint8_t makePropertyFlags(uint8_t base, uint8_t target, bool add)
    {
        if (add)
        {
            return base | target;
        }

        return base & ~target;
    }

    template< typename HANDLER, typename RESPONSE = typename std::decay<HANDLER>::type::result_type>
    OCEntityHandlerResult handleRequest(PrimitiveServerResource& resource,
            std::shared_ptr< OC::OCResourceRequest > ocRequest, HANDLER&& handler)
    {
        ResourceAttributes attrs{ ResourceAttributesConverter::fromOCRepresentation(
                ocRequest->getResourceRepresentation()) };

        if (handler)
        {
            return Detail::sendResponse(resource, ocRequest, attrs, handler(
                    PrimitiveRequest{ ocRequest->getResourceUri() }, attrs));
        }

        return Detail::sendResponse(resource, ocRequest, attrs, RESPONSE::defaultAction());
    }
} // unnamed namespace


namespace OIC
{
    namespace Service
    {
        PrimitiveServerResource::PrimitiveServerResource(uint8_t properties,
                ResourceAttributes&& attrs) :
                m_properties { properties }, m_resourceHandle{},
                m_resourceAttributes{ std::move(attrs) }, m_getRequestHandler{ },
                m_setRequestHandler{ }, m_mutex { }
        {
        }

        PrimitiveServerResource::Builder::Builder(const std::string& uri, const std::string& type,
                const std::string& interface) :
                m_uri{ uri }, m_type{ type }, m_interface{ interface }, m_properties{ 0 }
        {
        }

        PrimitiveServerResource::Builder& PrimitiveServerResource::Builder::setDiscoverable(
                bool discoverable)
        {
            m_properties = ::makePropertyFlags(m_properties, OC_DISCOVERABLE, discoverable);
            return *this;
        }

        PrimitiveServerResource::Builder& PrimitiveServerResource::Builder::setObservable(
                bool observable)
        {
            m_properties = ::makePropertyFlags(m_properties, OC_OBSERVABLE, observable);
            return *this;
        }

        PrimitiveServerResource::Builder& PrimitiveServerResource::Builder::setAttributes(
                const ResourceAttributes& attrs)
        {
            m_resourceAttributes = attrs;
            return *this;
        }

        PrimitiveServerResource::Builder& PrimitiveServerResource::Builder::setAttributes(
                ResourceAttributes&& attrs)
        {
            m_resourceAttributes = std::move(attrs);
            return *this;
        }

        PrimitiveServerResource::Ptr PrimitiveServerResource::Builder::create()
        {
            OCResourceHandle handle{ nullptr };

            PrimitiveServerResource::Ptr server {
                new PrimitiveServerResource{ m_properties, std::move(m_resourceAttributes) } };

            OC::EntityHandler entityHandler{ std::bind(&PrimitiveServerResource::entityHandler,
                    server.get(), std::placeholders::_1) };

            try
            {
                expectOCStackResultOK(
                        OC::OCPlatform::registerResource(handle, m_uri, m_type, m_interface,
                                entityHandler, m_properties));
            }
            catch (OC::OCException& e)
            {
                throw PlatformException(e.code());
            }

            server->m_resourceHandle = handle;

            return server;
        }

        bool PrimitiveServerResource::hasAttribute(const std::string& key) const
        {
            WeakGuard lock(*this);
            return m_resourceAttributes.contains(key);
        }

        ResourceAttributes& PrimitiveServerResource::getAttributes()
        {
            expectOwnLock();
            return m_resourceAttributes;
        }

        const ResourceAttributes& PrimitiveServerResource::getAttributes() const
        {
            expectOwnLock();
            return m_resourceAttributes;
        }

        void PrimitiveServerResource::expectOwnLock() const
        {
            if (m_lockOwner != std::this_thread::get_id())
            {
                throw NoLockException{ };
            }
        }

        bool PrimitiveServerResource::isObservable() const
        {
            return ::hasProperty(m_properties, OC_OBSERVABLE);
        }

        bool PrimitiveServerResource::isDiscoverable() const
        {
            return ::hasProperty(m_properties, OC_DISCOVERABLE);
        }

        void PrimitiveServerResource::setGetRequestHandler(GetRequestHandler h)
        {
            m_getRequestHandler = h;
        }

        void PrimitiveServerResource::setSetRequestHandler(SetRequestHandler h)
        {
            m_setRequestHandler = h;
        }

        void PrimitiveServerResource::notify() const
        {
            expectOCStackResultOK(OC::OCPlatform::notifyAllObservers(m_resourceHandle));
        }

        OCEntityHandlerResult PrimitiveServerResource::entityHandler(
                std::shared_ptr< OC::OCResourceRequest > request)
        {
            if (!request)
            {
                return OC_EH_ERROR;
            }

            try
            {
                if (request->getRequestHandlerFlag() & OC::RequestHandlerFlag::RequestFlag)
                {
                    return handleRequest(request);
                }

                if (request->getRequestHandlerFlag() & OC::RequestHandlerFlag::ObserverFlag)
                {
                    return handleObserve(request);
                }
            }
            catch (...)
            {
                // TODO : how to notify the error?
            }

            return OC_EH_ERROR;
        }

        OCEntityHandlerResult PrimitiveServerResource::handleRequest(
                std::shared_ptr< OC::OCResourceRequest > request)
        {
            if (request->getRequestType() == "GET")
            {
                return handleRequestGet(request);
            }

            if (request->getRequestType() == "PUT" || request->getRequestType() == "POST")
            {
                return handleRequestSet(request);
            }

            return OC_EH_ERROR;
        }

        OCEntityHandlerResult PrimitiveServerResource::handleRequestGet(
                std::shared_ptr< OC::OCResourceRequest > request)
        {
            return ::handleRequest(*this, request, m_getRequestHandler);
        }

        OCEntityHandlerResult PrimitiveServerResource::handleRequestSet(
                std::shared_ptr< OC::OCResourceRequest > request)
        {
            return ::handleRequest(*this, request, m_setRequestHandler);
        }

        OCEntityHandlerResult PrimitiveServerResource::handleObserve(
                std::shared_ptr< OC::OCResourceRequest > request)
        {
            if (!isObservable())
            {
                return OC_EH_ERROR;
            }

            return OC_EH_OK;
        }

        PrimitiveServerResource::LockGuard::LockGuard(const PrimitiveServerResource::Ptr ptr) :
                LockGuard{ *ptr }
        {
        }

        PrimitiveServerResource::LockGuard::LockGuard(
                const PrimitiveServerResource& serverResource) :
                m_serverResource(serverResource)
        {
            if (m_serverResource.m_lockOwner == std::this_thread::get_id())
            {
                throw DeadLockException{ };
            }

            m_serverResource.m_mutex.lock();
            m_serverResource.m_lockOwner = std::this_thread::get_id();
        }

        PrimitiveServerResource::LockGuard::~LockGuard()
        {
            m_serverResource.m_lockOwner = std::thread::id();
            m_serverResource.m_mutex.unlock();
        }

        PrimitiveServerResource::WeakGuard::WeakGuard(
                const PrimitiveServerResource& serverResource) :
                m_serverResource(serverResource), m_hasLocked{ false }
        {
            if (m_serverResource.m_lockOwner != std::this_thread::get_id())
            {
                m_serverResource.m_mutex.lock();
                m_hasLocked = true;
            }
        }

        PrimitiveServerResource::WeakGuard::~WeakGuard()
        {
            if (m_hasLocked)
            {
                m_serverResource.m_mutex.unlock();
            }
        }
    }
}
