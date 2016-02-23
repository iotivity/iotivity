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

#include <memory>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "UnitTestHelper.h"

#include "OCPlatform.h"
#include "RCSDiscoveryManager.h"
#include "RCSRemoteResourceObject.h"
#include "RCSResourceObject.h"
#include "RCSResourceAttributes.h"
#include "RCSAddress.h"

#include "RequestObject.h"

using namespace testing;
using namespace OIC::Service;

class ResourceEncapsulationTestSimulator
{
public:
    RCSResourceObject::Ptr server;
    RCSRemoteResourceObject::Ptr remoteResource;

private:
    std::mutex mutexForDiscovery;
    std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;

    std::string MULTICASTURI;
    std::string HOSTED_RESOURCEURI;
    std::string RESOURCEURI;
    std::string RESOURCETYPE;
    std::string RESOURCEINTERFACE;
    std::string ATTR_KEY;
    int ATTR_VALUE;

public:
    ResourceEncapsulationTestSimulator()
    :server(nullptr), remoteResource(nullptr),
     mutexForDiscovery(),
     MULTICASTURI("/oic/res"),
     HOSTED_RESOURCEURI("/a/TempHumSensor"),
     RESOURCEURI("/a/TempHumSensor/hosting"),
     RESOURCETYPE("oic.r.resourcehosting"),
     RESOURCEINTERFACE("oic.if.baseline"),
     ATTR_KEY("Temperature"),
     ATTR_VALUE(0)
    { }
    ~ResourceEncapsulationTestSimulator()
    { }

private:
    void onDiscoveryResource(RCSRemoteResourceObject::Ptr resourceObject)
    {
        if (remoteResource != nullptr)
        {
            return;
        }

        if (RESOURCEURI.compare(resourceObject->getUri()) != 0)
        {
            return;
        }

        remoteResource = resourceObject;
        discoveryTask->cancel();
        mutexForDiscovery.unlock();
    }

    void waitForDiscovery()
    {
        std::chrono::milliseconds interval(100);
        while(true)
        {
            if(mutexForDiscovery.try_lock())
            {
                mutexForDiscovery.unlock();
                return;
            }
            std::this_thread::sleep_for(interval);
        }
    }
    void WaitForPtrBeingUnique()
    {
        while((remoteResource && !remoteResource.unique()) || (server && !server.unique()))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
        }
    }

public:
    void destroy()
    {
        if(server.use_count()) server.reset();
        if(remoteResource.use_count()) remoteResource.reset();
        WaitForPtrBeingUnique();
    }
    void defaultRunSimulator()
    {
        createResource();
        discoveryResource();
        waitForDiscovery();
    }

    void createResource(std::string postUri = "")
    {
        HOSTED_RESOURCEURI = HOSTED_RESOURCEURI + postUri;
        server = RCSResourceObject::Builder(HOSTED_RESOURCEURI +  "/hosting",
                RESOURCETYPE, RESOURCEINTERFACE).
                setDiscoverable(true).setObservable(true).build();
        server->setAttribute(ATTR_KEY, RCSResourceAttributes::Value(ATTR_VALUE));
    }

    void discoveryResource()
    {
        discoveryResource(RESOURCETYPE);
    }

    void discoveryResource(std::string & resourceType)
    {
        try
        {
            discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(
                    RCSAddress::multicast(), MULTICASTURI, resourceType,
                    std::bind(
                            &ResourceEncapsulationTestSimulator::onDiscoveryResource,
                            this, std::placeholders::_1));
            mutexForDiscovery.lock();
        }
        catch(std::exception & e)
        {
            std::cout << "exception : " << e.what() << std::endl;
        }
    }

    std::string getServerUri() const
    {
        return RESOURCEURI;
    }

    std::string getHostedServerUri() const
    {
        return HOSTED_RESOURCEURI;
    }

    RCSResourceObject::Ptr getResourceServer() const
    {
        return server;
    }
 
    RCSRemoteResourceObject::Ptr getRemoteResource() const
    {
        return remoteResource;
    }
};
