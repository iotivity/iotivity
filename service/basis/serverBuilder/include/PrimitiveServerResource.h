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

#ifndef __PRIMITIVESERVERRESOURCE_H
#define __PRIMITIVESERVERRESOURCE_H

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

class NoLockException : public PrimitiveException
{
};

class DeadLockException : public PrimitiveException
{
};

class PrimitiveServerResource
{
private:
    class WeakGuard;

public:
//    enum class AutoNotifyPolicy {
//        NEVER,
//        ALWAYS,
//        UPDATED
//    };

    using Ptr = std::shared_ptr<PrimitiveServerResource>;
    using ConstPtr = std::shared_ptr<const PrimitiveServerResource>;

    class Builder
    {
    public:
        Builder(const std::string& uri, const std::string& type, const std::string& interface);

        Builder& setDiscoverable(bool discoverable);
        Builder& setObservable(bool observable);

        Builder& setAttributes(const ResourceAttributes&);
        Builder& setAttributes(ResourceAttributes&&);

        /**
         * @throw PlatformException
         */
        PrimitiveServerResource::Ptr create();

    private:
        std::string m_uri;
        std::string m_type;
        std::string m_interface;

        uint8_t m_properties;

        ResourceAttributes m_resourceAttributes;
    };

    class LockGuard;

    typedef std::function< PrimitiveGetResponse(const PrimitiveRequest&, const ResourceAttributes&) > GetRequestHandler;
    typedef std::function< PrimitiveSetResponse(const PrimitiveRequest&, const ResourceAttributes&) > SetRequestHandler;

public:
    PrimitiveServerResource(PrimitiveServerResource&&) = delete;
    PrimitiveServerResource(const PrimitiveServerResource&) = delete;

    PrimitiveServerResource& operator=(PrimitiveServerResource&&) = delete;
    PrimitiveServerResource& operator=(const PrimitiveServerResource&) = delete;

    template< typename T >
    void setAttribute(const std::string& key, const T &value)
    {
        WeakGuard lock(*this);
        m_resourceAttributes[key] = value;
    }

    template< typename T >
    T getAttribute(const std::string& key) const
    {
        WeakGuard lock(*this);
        return m_resourceAttributes.at(key).get< T >();
    }

    bool hasAttribute(const std::string& key) const;

    ResourceAttributes& getAttributes();
    const ResourceAttributes& getAttributes() const;

//    bool isObservable() const;
//    bool isDiscoverable() const;

    void setGetRequestHandler(GetRequestHandler);
    void setSetRequestHandler(SetRequestHandler);

 //   void notify();

//    void setAutoNotifyPolicy(AutoNotifyPolicy);

private:
    PrimitiveServerResource(ResourceAttributes&&);

    OCEntityHandlerResult entityHandler(std::shared_ptr< OC::OCResourceRequest >);

    OCEntityHandlerResult handleRequest(std::shared_ptr< OC::OCResourceRequest >);
    OCEntityHandlerResult handleRequestGet(std::shared_ptr< OC::OCResourceRequest >);
    OCEntityHandlerResult handleRequestSet(std::shared_ptr< OC::OCResourceRequest >);
    OCEntityHandlerResult handleObserve(std::shared_ptr< OC::OCResourceRequest >);

    void assertOwnLock() const;

private:
    OCResourceHandle m_resourceHandle;
    ResourceAttributes m_resourceAttributes;

    GetRequestHandler m_getRequestHandler;
    SetRequestHandler m_setRequestHandler;

    mutable std::atomic<std::thread::id> m_lockOwner;
    mutable std::mutex m_mutex;
};

class PrimitiveServerResource::LockGuard
{
public:
    LockGuard(const PrimitiveServerResource&);
    ~LockGuard();

    LockGuard(const LockGuard&) = delete;
    LockGuard(LockGuard&&) = delete;

    LockGuard& operator=(const LockGuard&) = delete;
    LockGuard& operator=(LockGuard&&) = delete;

private:
    const PrimitiveServerResource& m_serverResource;
};

class PrimitiveServerResource::WeakGuard
{
public:
    WeakGuard(const PrimitiveServerResource&);
    ~WeakGuard();

    WeakGuard(const WeakGuard&) = delete;
    WeakGuard(WeakGuard&&) = delete;

    WeakGuard& operator=(const WeakGuard&) = delete;
    WeakGuard& operator=(WeakGuard&&) = delete;

private:
    const PrimitiveServerResource& m_serverResource;
    bool m_hasLocked;
};

#endif
