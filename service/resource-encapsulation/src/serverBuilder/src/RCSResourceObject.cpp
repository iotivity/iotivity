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

#include <RCSResourceObject.h>

#include <string>
#include <functional>
#include <vector>

#include <RequestHandler.h>
#include <AssertUtils.h>
#include <ResourceAttributesConverter.h>

#include <logger.h>
#include <OCPlatform.h>

namespace
{
    using namespace OIC::Service;

    constexpr char LOG_TAG[]{ "RCSResourceObject" };

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
    OCEntityHandlerResult sendResponse(RCSResourceObject& resource,
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
            return handler(RCSRequest{ ocRequest->getResourceUri() }, attrs);
        }

        return RESPONSE::defaultAction();
    }

    typedef void (RCSResourceObject::* AutoNotifyFunc)(bool, RCSResourceObject::AutoNotifyPolicy) const;

    std::function <void ()> createAutoNotifyInvoker(AutoNotifyFunc autoNotifyFunc,
            const RCSResourceObject& resourceObject, const ResourceAttributes& resourceAttributes,
            RCSResourceObject::AutoNotifyPolicy autoNotifyPolicy)
    {
        if(autoNotifyPolicy == RCSResourceObject::AutoNotifyPolicy::UPDATED)
        {
            auto&& compareAttributesFunc =
                    std::bind(std::not_equal_to<ResourceAttributes>(),
                                resourceAttributes,
                                std::cref(resourceAttributes));
            return std::bind(autoNotifyFunc,
                    &resourceObject, std::move(compareAttributesFunc), autoNotifyPolicy);
        }
        else if(autoNotifyPolicy == RCSResourceObject::AutoNotifyPolicy::ALWAYS)
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

        RCSResourceObject::Builder::Builder(const std::string& uri, const std::string& type,
                const std::string& interface) :
                m_uri{ uri },
                m_type{ type },
                m_interface{ interface },
                m_properties{ OC_DISCOVERABLE | OC_OBSERVABLE },
                m_resourceAttributes{ }
        {
        }

        RCSResourceObject::Builder& RCSResourceObject::Builder::setDiscoverable(
                bool discoverable)
        {
            m_properties = ::makePropertyFlags(m_properties, OC_DISCOVERABLE, discoverable);
            return *this;
        }

        RCSResourceObject::Builder& RCSResourceObject::Builder::setObservable(
                bool observable)
        {
            m_properties = ::makePropertyFlags(m_properties, OC_OBSERVABLE, observable);
            return *this;
        }

        RCSResourceObject::Builder& RCSResourceObject::Builder::setAttributes(
                const ResourceAttributes& attrs)
        {
            m_resourceAttributes = attrs;
            return *this;
        }

        RCSResourceObject::Builder& RCSResourceObject::Builder::setAttributes(
                ResourceAttributes&& attrs)
        {
            m_resourceAttributes = std::move(attrs);
            return *this;
        }

        RCSResourceObject::Ptr RCSResourceObject::Builder::build()
        {
            OCResourceHandle handle{ nullptr };

            RCSResourceObject::Ptr server {
                new RCSResourceObject{ m_properties, std::move(m_resourceAttributes) } };

            OC::EntityHandler entityHandler{ std::bind(&RCSResourceObject::entityHandler,
                    server.get(), std::placeholders::_1) };

            try
            {
                typedef OCStackResult (*RegisterResource)(OCResourceHandle&, std::string&,
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


        RCSResourceObject::RCSResourceObject(uint8_t properties, ResourceAttributes&& attrs) :
                m_properties { properties },
                m_resourceHandle{ },
                m_resourceAttributes{ std::move(attrs) },
                m_getRequestHandler{ },
                m_setRequestHandler{ },
                m_autoNotifyPolicy { AutoNotifyPolicy::UPDATED },
                m_setRequestHandlerPolicy { SetRequestHandlerPolicy::NEVER },
                m_keyAttributesUpdatedListeners{ },
                m_lockOwner{ },
                m_mutex{ },
                m_mutexKeyAttributeUpdate{ }
        {
        }

        RCSResourceObject::~RCSResourceObject()
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

        void RCSResourceObject::setAttribute(const std::string& key,
                const ResourceAttributes::Value& value)
        {
            WeakGuard lock(*this);

            if(lock.hasLocked())
            {
                autoNotifyIfNeeded(key, value);
            }

            m_resourceAttributes[key] = value;
        }

        void RCSResourceObject::setAttribute(const std::string& key, ResourceAttributes::Value&& value)
        {
            WeakGuard lock(*this);

            if(lock.hasLocked())
            {
                autoNotifyIfNeeded(key, value);
            }

            m_resourceAttributes[key] = std::move(value);
        }

        void RCSResourceObject::setAttribute(std::string&& key, const ResourceAttributes::Value& value)
        {
            WeakGuard lock(*this);

            if(lock.hasLocked())
            {
                autoNotifyIfNeeded(key, value);
            }

            m_resourceAttributes[std::move(key)] = value;
        }

        void RCSResourceObject::setAttribute(std::string&& key, ResourceAttributes::Value&& value)
        {
            WeakGuard lock(*this);

            if(lock.hasLocked())
            {
                autoNotifyIfNeeded(key, value);
            }

            m_resourceAttributes[std::move(key)] = std::move(value);
        }

        ResourceAttributes::Value RCSResourceObject::getAttributeValue(const std::string& key) const
        {
            WeakGuard lock(*this);
            return m_resourceAttributes.at(key);
        }

        bool RCSResourceObject::removeAttribute(const std::string& key)
        {
            WeakGuard lock(*this);
            if (m_resourceAttributes.erase(key))
            {
                autoNotify(true, getAutoNotifyPolicy());
                return true;
            }
            return false;
        }

        bool RCSResourceObject::containsAttribute(const std::string& key) const
        {
            WeakGuard lock(*this);
            return m_resourceAttributes.contains(key);
        }

        ResourceAttributes& RCSResourceObject::getAttributes()
        {
            expectOwnLock();
            return m_resourceAttributes;
        }

        const ResourceAttributes& RCSResourceObject::getAttributes() const
        {
            expectOwnLock();
            return m_resourceAttributes;
        }

        void RCSResourceObject::expectOwnLock() const
        {
            if (m_lockOwner != std::this_thread::get_id())
            {
                throw NoLockException{ "Must acquire the lock first using LockGuard." };
            }
        }

        bool RCSResourceObject::isObservable() const
        {
            return ::hasProperty(m_properties, OC_OBSERVABLE);
        }

        bool RCSResourceObject::isDiscoverable() const
        {
            return ::hasProperty(m_properties, OC_DISCOVERABLE);
        }

        void RCSResourceObject::setGetRequestHandler(GetRequestHandler h)
        {
            m_getRequestHandler = std::move(h);
        }

        void RCSResourceObject::setSetRequestHandler(SetRequestHandler h)
        {
            m_setRequestHandler = std::move(h);
        }

        void RCSResourceObject::notify() const
        {
            typedef OCStackResult (*NotifyAllObservers)(OCResourceHandle);

            invokeOCFuncWithResultExpect(
                    { OC_STACK_OK, OC_STACK_NO_OBSERVERS },
                    static_cast< NotifyAllObservers >(OC::OCPlatform::notifyAllObservers),
                    m_resourceHandle);
        }

        void RCSResourceObject::addAttributeUpdatedListener(const std::string& key,
                AttributeUpdatedListener h)
        {
            std::lock_guard<std::mutex> lock(m_mutexKeyAttributeUpdate);
            m_keyAttributesUpdatedListeners[key] = std::move(h);
        }

        void RCSResourceObject::addAttributeUpdatedListener(std::string&& key,
                AttributeUpdatedListener h)
        {
           std::lock_guard<std::mutex> lock(m_mutexKeyAttributeUpdate);
           m_keyAttributesUpdatedListeners[std::move(key)] = std::move(h);
        }

        bool RCSResourceObject::removeAttributeUpdatedListener(const std::string& key)
        {
           std::lock_guard<std::mutex> lock(m_mutexKeyAttributeUpdate);
           return (bool) m_keyAttributesUpdatedListeners.erase(key);
        }

        void RCSResourceObject::autoNotifyIfNeeded(const std::string& key,
                                                const ResourceAttributes::Value& value)
        {
            autoNotify( m_resourceAttributes.contains(key) == false
                        || m_resourceAttributes.at(key) != value
                        , m_autoNotifyPolicy);
        }

        void RCSResourceObject::setAutoNotifyPolicy(AutoNotifyPolicy policy)
        {
            m_autoNotifyPolicy = policy;
        }

        RCSResourceObject::AutoNotifyPolicy RCSResourceObject::getAutoNotifyPolicy() const
        {
            return m_autoNotifyPolicy;
        }

        void RCSResourceObject::setSetRequestHandlerPolicy(SetRequestHandlerPolicy policy)
        {
            m_setRequestHandlerPolicy = policy;
        }

        RCSResourceObject::SetRequestHandlerPolicy RCSResourceObject::getSetRequestHandlerPolicy() const
        {
            return m_setRequestHandlerPolicy;
        }

        void RCSResourceObject::autoNotify(
                        bool isAttributesChanged, AutoNotifyPolicy autoNotifyPolicy) const
        {
            if(autoNotifyPolicy == AutoNotifyPolicy::NEVER) return;
            if(autoNotifyPolicy == AutoNotifyPolicy::UPDATED && isAttributesChanged == false) return;
            notify();
        }

        OCEntityHandlerResult RCSResourceObject::entityHandler(
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

        OCEntityHandlerResult RCSResourceObject::handleRequest(
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

        OCEntityHandlerResult RCSResourceObject::handleRequestGet(
                std::shared_ptr< OC::OCResourceRequest > request)
        {
            assert(request != nullptr);

            auto attrs = getAttributesFromOCRequest(request);

            return sendResponse(*this, request, invokeHandler(attrs, request, m_getRequestHandler));
        }

        OCEntityHandlerResult RCSResourceObject::handleRequestSet(
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

                auto keyAttribute = m_keyAttributesUpdatedListeners.find(it.first);
                if(keyAttribute != m_keyAttributesUpdatedListeners.end())
                {
                    keyAttribute-> second(it.second, attrs[it.first]);
                }
            }

            autoNotify(!replaced.empty(), m_autoNotifyPolicy);
            return sendResponse(*this, request, response);
        }

        OCEntityHandlerResult RCSResourceObject::handleObserve(
                std::shared_ptr< OC::OCResourceRequest > request)
        {
            assert(request != nullptr);

            if (!isObservable())
            {
                return OC_EH_ERROR;
            }

            return OC_EH_OK;
        }

        RCSResourceObject::LockGuard::LockGuard(const RCSResourceObject::Ptr ptr) :
                m_resourceObject(*ptr),
                m_autoNotifyPolicy{ ptr->getAutoNotifyPolicy() },
                m_isOwningLock{ false }
        {
            init();
        }

        RCSResourceObject::LockGuard::LockGuard(
                const RCSResourceObject& serverResource) :
                m_resourceObject(serverResource),
                m_autoNotifyPolicy{ serverResource.getAutoNotifyPolicy() },
                m_isOwningLock{ false }
        {
            init();
        }

        RCSResourceObject::LockGuard::LockGuard(
                const RCSResourceObject::Ptr ptr, AutoNotifyPolicy autoNotifyPolicy) :
                m_resourceObject(*ptr),
                m_autoNotifyPolicy { autoNotifyPolicy },
                m_isOwningLock{ false }
        {
            init();
        }

        RCSResourceObject::LockGuard::LockGuard(
                const RCSResourceObject& resourceObject, AutoNotifyPolicy autoNotifyPolicy) :
                        m_resourceObject(resourceObject),
                        m_autoNotifyPolicy { autoNotifyPolicy },
                        m_isOwningLock{ false }
        {
            init();
        }

        RCSResourceObject::LockGuard::~LockGuard()
        {
            if (m_autoNotifyFunc) m_autoNotifyFunc();

            if (m_isOwningLock)
            {
                m_resourceObject.m_lockOwner = std::thread::id{ };
                m_resourceObject.m_mutex.unlock();
            }
        }

        void RCSResourceObject::LockGuard::init()
        {
            if (m_resourceObject.m_lockOwner != std::this_thread::get_id())
            {
                m_resourceObject.m_mutex.lock();
                m_resourceObject.m_lockOwner = std::this_thread::get_id();
                m_isOwningLock = true;
            }
            m_autoNotifyFunc = ::createAutoNotifyInvoker(&RCSResourceObject::autoNotify,
                    m_resourceObject, m_resourceObject.m_resourceAttributes, m_autoNotifyPolicy);
        }

        RCSResourceObject::WeakGuard::WeakGuard(
                const RCSResourceObject& resourceObject) :
                m_isOwningLock{ false },
                m_resourceObject(resourceObject)
        {
            if (resourceObject.m_lockOwner != std::this_thread::get_id())
            {
                m_resourceObject.m_mutex.lock();
                m_isOwningLock = true;
            }
        }

        RCSResourceObject::WeakGuard::~WeakGuard()
        {
            if (m_isOwningLock)
            {
                m_resourceObject.m_mutex.unlock();
            }
        }

        bool RCSResourceObject::WeakGuard::hasLocked() const
        {
            return m_isOwningLock;
        }
    }
}
