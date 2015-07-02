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

#ifndef __OIC_RESOURCEOBJECT_H
#define __OIC_RESOURCEOBJECT_H

#include <string>
#include <mutex>
#include <atomic>
#include <thread>

#include <ResourceAttributes.h>
#include <PrimitiveResponse.h>
#include <PrimitiveRequest.h>

namespace OC
{
    class OCResourceRequest;
}

namespace OIC
{
    namespace Service
    {

        class NoLockException: public PrimitiveException
        {
        public:
            NoLockException(std::string&& what) : PrimitiveException{ std::move(what) } {}
        };

        class DeadLockException: public PrimitiveException
        {
        public:
            DeadLockException(std::string&& what) : PrimitiveException{ std::move(what) } {}
        };

        class ResourceObject
        {
        private:
            class WeakGuard;

        public:
        //    enum class AutoNotifyPolicy {
        //        NEVER,
        //        ALWAYS,
        //        UPDATED
        //    };

            using Ptr = std::shared_ptr< ResourceObject >;
            using ConstPtr = std::shared_ptr< const ResourceObject >;

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

            using GetRequestHandler = std::function< PrimitiveGetResponse(const PrimitiveRequest&,
                    ResourceAttributes&) >;
            using SetRequestHandler = std::function< PrimitiveSetResponse(const PrimitiveRequest&,
                    ResourceAttributes&) >;

        public:
            ResourceObject(ResourceObject&&) = delete;
            ResourceObject(const ResourceObject&) = delete;

            ResourceObject& operator=(ResourceObject&&) = delete;
            ResourceObject& operator=(const ResourceObject&) = delete;

            virtual ~ResourceObject();

            template< typename T >
            void setAttribute(const std::string& key, T&& value)
            {
                WeakGuard lock(*this);
                m_resourceAttributes[key] = std::forward<T>(value);
            }

            template< typename T >
            void setAttribute(std::string&& key, T&& value)
            {
                WeakGuard lock(*this);
                m_resourceAttributes[std::move(key)] = std::forward<T>(value);
            }

            template< typename T >
            T getAttribute(const std::string& key) const
            {
                WeakGuard lock(*this);
                return m_resourceAttributes.at(key).get< T >();
            }

            bool removeAttribute(const std::string& key);

            bool hasAttribute(const std::string& key) const;

            ResourceAttributes& getAttributes();
            const ResourceAttributes& getAttributes() const;

            virtual bool isObservable() const;
            virtual bool isDiscoverable() const;

            virtual void setGetRequestHandler(GetRequestHandler);
            virtual void setSetRequestHandler(SetRequestHandler);

            virtual void notify() const;

        //    void setAutoNotifyPolicy(AutoNotifyPolicy);

        private:
            ResourceObject(uint8_t, ResourceAttributes&&);

            OCEntityHandlerResult entityHandler(std::shared_ptr< OC::OCResourceRequest >);

            OCEntityHandlerResult handleRequest(std::shared_ptr< OC::OCResourceRequest >);
            OCEntityHandlerResult handleRequestGet(std::shared_ptr< OC::OCResourceRequest >);
            OCEntityHandlerResult handleRequestSet(std::shared_ptr< OC::OCResourceRequest >);
            OCEntityHandlerResult handleObserve(std::shared_ptr< OC::OCResourceRequest >);

            void expectOwnLock() const;

        private:
            const uint8_t m_properties;

            OCResourceHandle m_resourceHandle;
            ResourceAttributes m_resourceAttributes;

            GetRequestHandler m_getRequestHandler;
            SetRequestHandler m_setRequestHandler;

            mutable std::atomic< std::thread::id > m_lockOwner;
            mutable std::mutex m_mutex;
        };

        class ResourceObject::LockGuard
        {
        public:
            LockGuard(const ResourceObject&);
            LockGuard(const ResourceObject::Ptr);
            ~LockGuard();

            LockGuard(const LockGuard&) = delete;
            LockGuard(LockGuard&&) = delete;

            LockGuard& operator=(const LockGuard&) = delete;
            LockGuard& operator=(LockGuard&&) = delete;

        private:
            const ResourceObject& m_resourceObject;
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

        private:
            const ResourceObject& m_serverResource;
            bool m_hasLocked;
        };
    }
}

#endif // __OIC_RESOURCEOBJECT_H
