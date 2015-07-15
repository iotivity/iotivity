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

    typedef void (ResourceObject::* AutoNotifyFunc)(bool, ResourceObject::AutoNotifyPolicy) const;

    std::function <void ()> createAutoNotifyInvoker(AutoNotifyFunc autoNotifyFunc,
            const ResourceObject& resourceObject, const ResourceAttributes& resourceAttributes,
            ResourceObject::AutoNotifyPolicy autoNotifyPolicy)
    {
        if(autoNotifyPolicy == ResourceObject::AutoNotifyPolicy::UPDATED)
        {
            auto&& compareAttributesFunc =
                    std::bind(std::not_equal_to<ResourceAttributes>(),
                                resourceAttributes,
                                std::cref(resourceAttributes));
            return std::bind(autoNotifyFunc,
                    &resourceObject, std::move(compareAttributesFunc), autoNotifyPolicy);
        }
        else if(autoNotifyPolicy == ResourceObject::AutoNotifyPolicy::ALWAYS)
        {
            return std::bind(autoNotifyFunc,
                    &resourceObject, true, autoNotifyPolicy);
        }
        return {};
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
                m_autoNotifyPolicy { AutoNotifyPolicy::UPDATED },
                m_setRequestHandlerPolicy { SetRequestHandlerPolicy::NEVER },
                m_keyAttributesUpdatedHandlers{ },
                m_lockOwner{ },
                m_mutex{ },
                m_mutexKeyAttributeUpdate{ }
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

        void ResourceObject::setAttribute(const std::string& key,
                const ResourceAttributes::Value& value)
        {
            WeakGuard lock(*this);

            if(lock.hasLocked())
            {
                autoNotifyIfNeeded(key, value);
            }

            m_resourceAttributes[key] = value;
        }

        void ResourceObject::setAttribute(const std::string& key, ResourceAttributes::Value&& value)
        {
            WeakGuard lock(*this);

            if(lock.hasLocked())
            {
                autoNotifyIfNeeded(key, value);
            }

            m_resourceAttributes[key] = std::move(value);
        }

        void ResourceObject::setAttribute(std::string&& key, const ResourceAttributes::Value& value)
        {
            WeakGuard lock(*this);

            if(lock.hasLocked())
            {
                autoNotifyIfNeeded(key, value);
            }

            m_resourceAttributes[std::move(key)] = value;
        }

        void ResourceObject::setAttribute(std::string&& key, ResourceAttributes::Value&& value)
        {
            WeakGuard lock(*this);

            if(lock.hasLocked())
            {
                autoNotifyIfNeeded(key, value);
            }

            m_resourceAttributes[std::move(key)] = std::move(value);
        }

        ResourceAttributes::Value ResourceObject::getAttributeValue(const std::string& key) const
        {
            WeakGuard lock(*this);
            return m_resourceAttributes.at(key);
        }

        bool ResourceObject::removeAttribute(const std::string& key)
        {
            WeakGuard lock(*this);
            return m_resourceAttributes.erase(key);
        }

        bool ResourceObject::containsAttribute(const std::string& key) const
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

            invokeOCFuncWithResultExpect(
                    { OC_STACK_OK, OC_STACK_NO_OBSERVERS },
                    static_cast< NotifyAllObservers >(OC::OCPlatform::notifyAllObservers),
                    m_resourceHandle);
        }

        void ResourceObject::setAttributeUpdatedHandler(const std::string& key,
                AttributeUpdatedHandler h)
        {
            std::lock_guard<std::mutex> lock(m_mutexKeyAttributeUpdate);
            m_keyAttributesUpdatedHandlers[key] = std::move(h);
        }

        void ResourceObject::setAttributeUpdatedHandler(std::string&& key,
                AttributeUpdatedHandler h)
        {
           std::lock_guard<std::mutex> lock(m_mutexKeyAttributeUpdate);
           m_keyAttributesUpdatedHandlers[std::move(key)] = std::move(h);
        }

        bool ResourceObject::removeAddAttributeUpdatedHandler(const std::string& key)
        {
           std::lock_guard<std::mutex> lock(m_mutexKeyAttributeUpdate);
           return (bool) m_keyAttributesUpdatedHandlers.erase(key);
        }

        void ResourceObject::autoNotifyIfNeeded(const std::string& key,
                                                const ResourceAttributes::Value& value)
        {
            autoNotify( m_resourceAttributes.contains(key) == false
                        || m_resourceAttributes.at(key) != value
                        , m_autoNotifyPolicy);
        }

        void ResourceObject::setAutoNotifyPolicy(AutoNotifyPolicy policy)
        {
            m_autoNotifyPolicy = policy;
        }

        ResourceObject::AutoNotifyPolicy ResourceObject::getAutoNotifyPolicy() const
        {
            return m_autoNotifyPolicy;
        }

        void ResourceObject::setSetRequestHandlerPolicy(SetRequestHandlerPolicy policy)
        {
            m_setRequestHandlerPolicy = policy;
        }

        ResourceObject::SetRequestHandlerPolicy ResourceObject::getSetRequestHandlerPolicy() const
        {
            return m_setRequestHandlerPolicy;
        }

        void ResourceObject::autoNotify(
                        bool isAttributesChanged, AutoNotifyPolicy autoNotifyPolicy) const
        {
            if(autoNotifyPolicy == AutoNotifyPolicy::NEVER) return;
            if(autoNotifyPolicy == AutoNotifyPolicy::UPDATED && isAttributesChanged == false) return;
            notify();
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

            if (request->getRequestType() == "PUT")
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

            for (const auto& it : replaced)
            {
                std::lock_guard<std::mutex> lock(m_mutexKeyAttributeUpdate);

                auto keyAttribute = m_keyAttributesUpdatedHandlers.find(it.first);
                if(keyAttribute != m_keyAttributesUpdatedHandlers.end())
                {
                    keyAttribute-> second(it.second, attrs[it.first]);
                }
            }

            autoNotify(!replaced.empty(), m_autoNotifyPolicy);
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
                LockGuard{ *ptr , ptr->getAutoNotifyPolicy() }
        {
        }

        ResourceObject::LockGuard::LockGuard(
                const ResourceObject& serverResource) :
                LockGuard{ serverResource, serverResource.getAutoNotifyPolicy() }
        {
        }

        ResourceObject::LockGuard::LockGuard(
                const ResourceObject::Ptr ptr, AutoNotifyPolicy autoNotifyPolicy) :
                LockGuard{ *ptr, autoNotifyPolicy}
        {
        }

        ResourceObject::LockGuard::LockGuard(
                const ResourceObject& resourceObject, AutoNotifyPolicy autoNotifyPolicy) :
                        m_resourceObject(resourceObject),
                        m_autoNotifyPolicy { autoNotifyPolicy },
                        m_isOwningLock{ false }
        {
            if (resourceObject.m_lockOwner != std::this_thread::get_id())
            {
                m_resourceObject.m_mutex.lock();
                m_resourceObject.m_lockOwner = std::this_thread::get_id();
                m_isOwningLock = true;
            }
            m_autoNotifyFunc = ::createAutoNotifyInvoker(&ResourceObject::autoNotify,
                    m_resourceObject, m_resourceObject.m_resourceAttributes, m_autoNotifyPolicy);
        }

        ResourceObject::LockGuard::~LockGuard()
        {
            if (m_autoNotifyFunc) m_autoNotifyFunc();

            if (m_isOwningLock)
            {
                m_resourceObject.m_lockOwner = std::thread::id{ };
                m_resourceObject.m_mutex.unlock();
            }
        }

        ResourceObject::WeakGuard::WeakGuard(
                const ResourceObject& resourceObject) :
                m_isOwningLock{ false },
                m_resourceObject(resourceObject)
        {
            if (resourceObject.m_lockOwner != std::this_thread::get_id())
            {
                m_resourceObject.m_mutex.lock();
                m_isOwningLock = true;
            }
        }

        ResourceObject::WeakGuard::~WeakGuard()
        {
            if (m_isOwningLock)
            {
                m_resourceObject.m_mutex.unlock();
            }
        }

        bool ResourceObject::WeakGuard::hasLocked() const
        {
            return m_isOwningLock;
        }
    }
}
