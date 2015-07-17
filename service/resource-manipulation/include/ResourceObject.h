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

#ifndef SERVERBUILDER_RESOURCEOBJECT_H
#define SERVERBUILDER_RESOURCEOBJECT_H

#include <string>
#include <mutex>
#include <thread>

#include <boost/atomic.hpp>

#include <ResourceAttributes.h>
#include <RCSResponse.h>
#include <RCSRequest.h>

namespace OC
{
    class OCResourceRequest;
}

namespace OIC
{
    namespace Service
    {

        class NoLockException: public RCSException
        {
        public:
            NoLockException(std::string&& what) : RCSException{ std::move(what) } {}
        };

        class ResourceObject
        {
        private:
            class WeakGuard;

        public:
            enum class AutoNotifyPolicy {
                NEVER,
                ALWAYS,
                UPDATED
            };

            enum class SetRequestHandlerPolicy {
                NEVER,
                ACCEPTANCE
            };

            typedef std::shared_ptr< ResourceObject > Ptr;
            typedef std::shared_ptr< const ResourceObject > ConstPtr;

            class Builder
            {
            public:
                Builder(const std::string& uri, const std::string& type,
                        const std::string& interface);

                Builder& setDiscoverable(bool discoverable);
                Builder& setObservable(bool observable);

                Builder& setAttributes(const ResourceAttributes&);
                Builder& setAttributes(ResourceAttributes&&);

                /**
                 * @throw PlatformException
                 */
                ResourceObject::Ptr build();

            private:
                std::string m_uri;
                std::string m_type;
                std::string m_interface;

                uint8_t m_properties;

                ResourceAttributes m_resourceAttributes;
            };

            class LockGuard;

            typedef std::function< RCSGetResponse(const RCSRequest&,
                        ResourceAttributes&) > GetRequestHandler;
            typedef std::function< RCSSetResponse(const RCSRequest&,
                        ResourceAttributes&) > SetRequestHandler;
            typedef std::function< void(const ResourceAttributes::Value&,
                    const ResourceAttributes::Value&) > AttributeUpdatedListener;

        public:
            ResourceObject(ResourceObject&&) = delete;
            ResourceObject(const ResourceObject&) = delete;

            ResourceObject& operator=(ResourceObject&&) = delete;
            ResourceObject& operator=(const ResourceObject&) = delete;

            virtual ~ResourceObject();

            void setAttribute(const std::string& key, const ResourceAttributes::Value&);
            void setAttribute(const std::string& key, ResourceAttributes::Value&&);

            void setAttribute(std::string&& key, const ResourceAttributes::Value&);
            void setAttribute(std::string&& key, ResourceAttributes::Value&&);

            ResourceAttributes::Value getAttributeValue(const std::string& key) const;

            template< typename T >
            T getAttribute(const std::string& key) const
            {
                WeakGuard lock(*this);
                return m_resourceAttributes.at(key).get< T >();
            }

            bool removeAttribute(const std::string& key);

            bool containsAttribute(const std::string& key) const;

            ResourceAttributes& getAttributes();
            const ResourceAttributes& getAttributes() const;

            virtual bool isObservable() const;
            virtual bool isDiscoverable() const;

            virtual void setGetRequestHandler(GetRequestHandler);
            virtual void setSetRequestHandler(SetRequestHandler);

            virtual void addAttributeUpdatedListener(const std::string& key,
                    AttributeUpdatedListener );
            virtual void addAttributeUpdatedListener(std::string&& key, AttributeUpdatedListener);
            virtual bool removeAttributeUpdatedListener(const std::string& key);


            virtual void notify() const;

            void setAutoNotifyPolicy(AutoNotifyPolicy);
            AutoNotifyPolicy getAutoNotifyPolicy() const;

            void setSetRequestHandlerPolicy(SetRequestHandlerPolicy);
            SetRequestHandlerPolicy getSetRequestHandlerPolicy() const;

        private:
            ResourceObject(uint8_t, ResourceAttributes&&);

            OCEntityHandlerResult entityHandler(std::shared_ptr< OC::OCResourceRequest >);

            OCEntityHandlerResult handleRequest(std::shared_ptr< OC::OCResourceRequest >);
            OCEntityHandlerResult handleRequestGet(std::shared_ptr< OC::OCResourceRequest >);
            OCEntityHandlerResult handleRequestSet(std::shared_ptr< OC::OCResourceRequest >);
            OCEntityHandlerResult handleObserve(std::shared_ptr< OC::OCResourceRequest >);

            void expectOwnLock() const;
            void autoNotify(bool, AutoNotifyPolicy) const;
            void autoNotifyIfNeeded(const std::string& , const ResourceAttributes::Value& );

        private:
            const uint8_t m_properties;

            OCResourceHandle m_resourceHandle;
            ResourceAttributes m_resourceAttributes;

            GetRequestHandler m_getRequestHandler;
            SetRequestHandler m_setRequestHandler;
            AutoNotifyPolicy m_autoNotifyPolicy;
            SetRequestHandlerPolicy m_setRequestHandlerPolicy;

            std::unordered_map< std::string, AttributeUpdatedListener >
                    m_keyAttributesUpdatedListeners;

            mutable boost::atomic< std::thread::id > m_lockOwner;
            mutable std::mutex m_mutex;

            std::mutex m_mutexKeyAttributeUpdate;

        };

        class ResourceObject::LockGuard
        {
        public:
            LockGuard(const ResourceObject&);
            LockGuard(const ResourceObject::Ptr);
            LockGuard(const ResourceObject&, AutoNotifyPolicy);
            LockGuard(const ResourceObject::Ptr, AutoNotifyPolicy);
            ~LockGuard();

            LockGuard(const LockGuard&) = delete;
            LockGuard(LockGuard&&) = delete;

            LockGuard& operator=(const LockGuard&) = delete;
            LockGuard& operator=(LockGuard&&) = delete;

        private:
            void init();

        private:
            const ResourceObject& m_resourceObject;

            AutoNotifyPolicy m_autoNotifyPolicy;

            bool m_isOwningLock;

            std::function<void()> m_autoNotifyFunc;
        };

        class ResourceObject::WeakGuard
        {
        public:
            WeakGuard(const ResourceObject&);
            ~WeakGuard();

            WeakGuard(const WeakGuard&) = delete;
            WeakGuard(WeakGuard&&) = delete;

            WeakGuard& operator=(const WeakGuard&) = delete;
            WeakGuard& operator=(WeakGuard&&) = delete;

            bool hasLocked() const;

        private:
            bool m_isOwningLock;
            const ResourceObject& m_resourceObject;
        };
    }
}

#endif // SERVERBUILDER_RESOURCEOBJECT_H
