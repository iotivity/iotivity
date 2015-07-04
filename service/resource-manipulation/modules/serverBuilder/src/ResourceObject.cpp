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

#include <ResourceObject.h>

#include <string>
#include <functional>
#include <vector>

#include <internal/RequestHandler.h>
#include <internal/AssertUtils.h>
#include <internal/ResourceAttributesConverter.h>

#include <logger.h>
#include <OCPlatform.h>

namespace
{
    using namespace OIC::Service;

    constexpr const char LOG_TAG[]{ "PrimitiveServerResource" };


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

    template <typename RESPONSE>
    OCEntityHandlerResult sendResponse(ResourceObject& resource,
            std::shared_ptr< OC::OCResourceRequest > ocRequest, RESPONSE&& response)
    {
        auto ocResponse = response.getHandler()->buildResponse(resource);

        ocResponse->setRequestHandle(ocRequest->getRequestHandle());
        ocResponse->setResourceHandle(ocRequest->getResourceHandle());

        try
        {
            if (OC::OCPlatform::sendResponse(ocResponse) == OC_STACK_OK)
            {
                return OC_EH_OK;
            }
        }
        catch (const OC::OCException& e)
        {
            OC_LOG(WARNING, LOG_TAG, e.what());
        }

        return OC_EH_ERROR;
    }

    ResourceAttributes getAttributesFromOCRequest(std::shared_ptr< OC::OCResourceRequest > request)
    {
        return ResourceAttributesConverter::fromOCRepresentation(
                request->getResourceRepresentation());
    }

    template< typename HANDLER, typename RESPONSE = typename std::decay<HANDLER>::type::result_type >
    RESPONSE invokeHandler(ResourceAttributes& attrs,
            std::shared_ptr< OC::OCResourceRequest > ocRequest, HANDLER&& handler)
    {
        if (handler)
        {
            return handler(PrimitiveRequest{ ocRequest->getResourceUri() }, attrs);
        }

        return RESPONSE::defaultAction();
    }

} // unnamed namespace


namespace OIC
{
    namespace Service
    {

        ResourceObject::Builder::Builder(const std::string& uri, const std::string& type,
                const std::string& interface) :
                m_uri{ uri },
                m_type{ type },
                m_interface{ interface },
                m_properties{ OC_DISCOVERABLE | OC_OBSERVABLE },
                m_resourceAttributes{ }
        {
        }

        ResourceObject::Builder& ResourceObject::Builder::setDiscoverable(
                bool discoverable)
        {
            m_properties = ::makePropertyFlags(m_properties, OC_DISCOVERABLE, discoverable);
            return *this;
        }

        ResourceObject::Builder& ResourceObject::Builder::setObservable(
                bool observable)
        {
            m_properties = ::makePropertyFlags(m_properties, OC_OBSERVABLE, observable);
            return *this;
        }

        ResourceObject::Builder& ResourceObject::Builder::setAttributes(
                const ResourceAttributes& attrs)
        {
            m_resourceAttributes = attrs;
            return *this;
        }

        ResourceObject::Builder& ResourceObject::Builder::setAttributes(
                ResourceAttributes&& attrs)
        {
            m_resourceAttributes = std::move(attrs);
            return *this;
        }

        ResourceObject::Ptr ResourceObject::Builder::build()
        {
            OCResourceHandle handle{ nullptr };

            ResourceObject::Ptr server {
                new ResourceObject{ m_properties, std::move(m_resourceAttributes) } };

            OC::EntityHandler entityHandler{ std::bind(&ResourceObject::entityHandler,
                    server.get(), std::placeholders::_1) };

            try
            {
                using RegisterResource = OCStackResult (*)(OCResourceHandle&, std::string&,
                        const std::string&, const std::string&, OC::EntityHandler, uint8_t);

                invokeOCFunc(static_cast<RegisterResource>(OC::OCPlatform::registerResource),
                        handle, m_uri, m_type, m_interface, entityHandler, m_properties);
            }
            catch (OC::OCException& e)
            {
                throw PlatformException(e.code());
            }

            server->m_resourceHandle = handle;

            return server;
        }


        ResourceObject::ResourceObject(uint8_t properties, ResourceAttributes&& attrs) :
                m_properties { properties },
                m_resourceHandle{ },
                m_resourceAttributes{ std::move(attrs) },
                m_getRequestHandler{ },
                m_setRequestHandler{ },
                m_lockOwner{ },
                m_mutex{ }
        {
        }

        ResourceObject::~ResourceObject()
        {
            if (m_resourceHandle)
            {
                try
                {
                    OC::OCPlatform::unregisterResource(m_resourceHandle);
                }
                catch (...)
                {
                    OC_LOG(WARNING, LOG_TAG, "Failed to unregister resource.");
                }
            }
        }

        bool ResourceObject::removeAttribute(const std::string& key)
        {
            WeakGuard lock(*this);
            return m_resourceAttributes.erase(key);
        }

        bool ResourceObject::hasAttribute(const std::string& key) const
        {
            WeakGuard lock(*this);
            return m_resourceAttributes.contains(key);
        }

        ResourceAttributes& ResourceObject::getAttributes()
        {
            expectOwnLock();
            return m_resourceAttributes;
        }

        const ResourceAttributes& ResourceObject::getAttributes() const
        {
            expectOwnLock();
            return m_resourceAttributes;
        }

        void ResourceObject::expectOwnLock() const
        {
            if (m_lockOwner != std::this_thread::get_id())
            {
                throw NoLockException{ "Must acquire the lock first using LockGuard." };
            }
        }

        bool ResourceObject::isObservable() const
        {
            return ::hasProperty(m_properties, OC_OBSERVABLE);
        }

        bool ResourceObject::isDiscoverable() const
        {
            return ::hasProperty(m_properties, OC_DISCOVERABLE);
        }

        void ResourceObject::setGetRequestHandler(GetRequestHandler h)
        {
            m_getRequestHandler = std::move(h);
        }

        void ResourceObject::setSetRequestHandler(SetRequestHandler h)
        {
            m_setRequestHandler = std::move(h);
        }

        void ResourceObject::notify() const
        {
            using NotifyAllObservers = OCStackResult (*)(OCResourceHandle);

            invokeOCFunc(static_cast<NotifyAllObservers>(OC::OCPlatform::notifyAllObservers),
                    m_resourceHandle);
        }

        OCEntityHandlerResult ResourceObject::entityHandler(
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
            catch (const std::exception& e)
            {
                OC_LOG_V(WARNING, LOG_TAG, "Failed to handle request : %s", e.what());
                throw;
            }
            catch (...)
            {
                OC_LOG(WARNING, LOG_TAG, "Failed to handle request.");
                throw;
            }

            return OC_EH_ERROR;
        }

        OCEntityHandlerResult ResourceObject::handleRequest(
                std::shared_ptr< OC::OCResourceRequest > request)
        {
            assert(request != nullptr);

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

        OCEntityHandlerResult ResourceObject::handleRequestGet(
                std::shared_ptr< OC::OCResourceRequest > request)
        {
            assert(request != nullptr);

            auto attrs = getAttributesFromOCRequest(request);

            return sendResponse(*this, request, invokeHandler(attrs, request, m_getRequestHandler));
        }

        OCEntityHandlerResult ResourceObject::handleRequestSet(
                std::shared_ptr< OC::OCResourceRequest > request)
        {
            assert(request != nullptr);

            auto attrs = getAttributesFromOCRequest(request);
            auto response = invokeHandler(attrs, request, m_setRequestHandler);
            auto requestHandler = response.getHandler();

            assert(requestHandler != nullptr);

            AttrKeyValuePairs replaced = requestHandler->applyAcceptanceMethod(
                    response.getAcceptanceMethod(), *this, attrs);

            return sendResponse(*this, request, response);
        }

        OCEntityHandlerResult ResourceObject::handleObserve(
                std::shared_ptr< OC::OCResourceRequest > request)
        {
            assert(request != nullptr);

            if (!isObservable())
            {
                return OC_EH_ERROR;
            }

            return OC_EH_OK;
        }

        ResourceObject::LockGuard::LockGuard(const ResourceObject::Ptr ptr) :
                LockGuard{ *ptr }
        {
        }

        ResourceObject::LockGuard::LockGuard(
                const ResourceObject& serverResource) :
                m_resourceObject(serverResource)
        {
            if (m_resourceObject.m_lockOwner == std::this_thread::get_id())
            {
                throw DeadLockException{ "Can't lock recursively in same thread." };
            }

            m_resourceObject.m_mutex.lock();
            m_resourceObject.m_lockOwner = std::this_thread::get_id();
        }

        ResourceObject::LockGuard::~LockGuard()
        {
            m_resourceObject.m_lockOwner = std::thread::id();
            m_resourceObject.m_mutex.unlock();
        }

        ResourceObject::WeakGuard::WeakGuard(
                const ResourceObject& serverResource) :
                m_serverResource(serverResource), m_hasLocked{ false }
        {
            if (m_serverResource.m_lockOwner != std::this_thread::get_id())
            {
                m_serverResource.m_mutex.lock();
                m_hasLocked = true;
            }
        }

        ResourceObject::WeakGuard::~WeakGuard()
        {
            if (m_hasLocked)
            {
                m_serverResource.m_mutex.unlock();
            }
        }
    }
}
