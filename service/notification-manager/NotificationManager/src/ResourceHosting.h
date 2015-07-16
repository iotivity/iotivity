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

#ifndef RH_RESOURCEHOSTING_H_
#define RH_RESOURCEHOSTING_H_

#include <cstdbool>
#include <iostream>
#include <list>
#include <memory>
#include <functional>
#include <string>
#include <atomic>

#include "octypes.h"
#include "ResourceClient.h"
#include "PresenceSubscriber.h"
#include "HostingObject.h"
#include "PrimitiveResource.h"

namespace OIC
{
namespace Service
{

class ResourceHosting
{
private:
    using HostingObjectPtr = std::shared_ptr<HostingObject>;
    using RemoteObjectPtr =  std::shared_ptr<RemoteResourceObject>;
    using PrimiteveResourcePtr = std::shared_ptr<PrimitiveResource>;

    using SubscribeCallback
            = std::function<void(OCStackResult, const unsigned int, const std::string&) >;
    using DiscoveryCallback
            = std::function<void(std::shared_ptr<RemoteResourceObject>)>;
    using DestroyedCallback
            = std::function<void()>;

public:
    void startHosting();
    void stopHosting();

    static ResourceHosting * getInstance();

private:
    ResourceHosting() = default;
    ~ResourceHosting() = default;

    ResourceHosting(const ResourceHosting&) = delete;
    ResourceHosting(ResourceHosting&&) = delete;
    ResourceHosting& operator=(const ResourceHosting&) const = delete;
    ResourceHosting& operator=(ResourceHosting&&) const = delete;

    static ResourceHosting * s_instance;
    static std::mutex s_mutexForCreation;

    std::list<HostingObjectPtr> hostingObjectList;

    DiscoveryManager * discoveryManager;
    PresenceSubscriber presenceHandle;

    SubscribeCallback pPresenceCB;
    DiscoveryCallback pDiscoveryCB;

    void initializeResourceHosting();

    void requestMulticastPresence();
    void requestMulticastDiscovery();
    void requestDiscovery(std::string address = std::string());

    void presenceHandler(OCStackResult ret, const unsigned int seq, const std::string & address);
    void discoverHandler(RemoteObjectPtr remoteResource);

    HostingObjectPtr findRemoteResource(RemoteObjectPtr remoteResource);
    bool isSameRemoteResource(RemoteObjectPtr remoteResource_1, RemoteObjectPtr remoteResource_2);

    void destroyedHostingObject(HostingObjectPtr destroyedPtr);

};

} /* namespace Service */
} /* namespace OIC */

#endif /* RH_RESOURCEHOSTING_H_ */
