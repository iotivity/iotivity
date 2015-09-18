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
#include <unistd.h>
#include "OCPlatform.h"
#include "UnitTestHelper.h"
#include "RCSRemoteResourceObject.h"
#include "RCSDiscoveryManager.h"
#include "RCSResourceObject.h"
#include "PrimitiveResource.h"

using namespace OIC::Service;
using namespace OC::OCPlatform;

constexpr char RESOURCEURI[]{ "/a/TemperatureSensor" };
constexpr char RESOURCETYPE[]{ "Resource.Hosting" };
constexpr char RESOURCEINTERFACE[]{ "oic.if.baseline" };
constexpr int DiscoveryTaskDELAYTIME = 7;

class DiscoveryManagerTest: public TestWithMock
{
public:

    RCSResourceObject::Ptr server;
    RCSRemoteResourceObject::Ptr object;
    std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;

public:

    void startDiscovery()
    {
        const std::string uri  = "/oic/res";
        const std::string type = "Resource.Hosting";
        discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(RCSAddress::multicast(),
                 uri, type, &resourceDiscovered);
    }

    void cancelDiscovery()
    {
        discoveryTask->cancel();
    }

    void createResource()
    {
        server = RCSResourceObject::Builder(RESOURCEURI, RESOURCETYPE, RESOURCEINTERFACE).build();
    }

    void waitForDiscoveryTask()
    {
        sleep(DiscoveryTaskDELAYTIME);
    }

    static void resourceDiscovered(std::shared_ptr< RCSRemoteResourceObject >) {}

};

TEST_F(DiscoveryManagerTest, resourceIsNotSupportedPresenceBeforeDiscovering)
{
    createResource();

    mocks.ExpectCallFunc(resourceDiscovered);

    startDiscovery();
    waitForDiscoveryTask();
}

TEST_F(DiscoveryManagerTest, resourceIsSupportedPresenceBeforeDiscovering)
{
    startPresence(10);
    createResource();

    mocks.ExpectCallFunc(resourceDiscovered);

    startDiscovery();
    waitForDiscoveryTask();
}

TEST_F(DiscoveryManagerTest, resourceIsNotSupportedPresenceAfterDiscovering)
{
    mocks.ExpectCallFunc(resourceDiscovered);

    startDiscovery();
    createResource();
    waitForDiscoveryTask();
}

TEST_F(DiscoveryManagerTest, resourceIsSupportedPresenceAndAfterDiscovering)
{
    mocks.ExpectCallFunc(resourceDiscovered);

    startPresence(10);
    startDiscovery();
    createResource();
    waitForDiscoveryTask();
}

TEST_F(DiscoveryManagerTest, cancelDiscoveryTaskAfterDiscoveryResource)
{
    startDiscovery();
    cancelDiscovery();

    mocks.NeverCallFunc(resourceDiscovered);

    sleep(3);
    createResource();

}

TEST_F(DiscoveryManagerTest, cancelDiscoveryTaskNotStartDiscoveryResource)
{
    startDiscovery();
    cancelDiscovery();
    cancelDiscovery();
}
