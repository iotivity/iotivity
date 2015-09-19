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

#include "ResourceEncapsulationTestSimulator.h"

#include "ResourceHosting.h"

using namespace testing;
using namespace OIC::Service;

namespace
{
    void onDiscoveryResource(RCSRemoteResourceObject::Ptr) { }
}

class ResourceHostingTest : public TestWithMock
{
public:
    std::mutex mutexForCondition;
    std::condition_variable responseCon;
    std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;

protected:

    void SetUp()
    {
        TestWithMock::SetUp();
    }

    void TearDown()
    {
        TestWithMock::TearDown();

        if (ResourceHosting::getInstance())
        {
            ResourceHosting::getInstance()->stopHosting();
        }

    }

public:
    void waitForCondition(int waitingTime = 1000)
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, std::chrono::milliseconds{ waitingTime });
        std::cout << "condition return back" << std::endl;
    }

    void notifyCondition()
    {
        std::cout << "notify condition to all" << std::endl;
        responseCon.notify_all();
    }

};

TEST(ResourceHostingSTATICMethodTest, getInstanceAllwaysSameReturnInstance)
{
    EXPECT_EQ(ResourceHosting::getInstance(), ResourceHosting::getInstance());
}

TEST_F(ResourceHostingTest, HostingFoundBeforeMakeOriginServer)
{
    ResourceEncapsulationTestSimulator::Ptr testObject
        = std::make_shared<ResourceEncapsulationTestSimulator>();
    testObject->createResource();

    ResourceHosting::getInstance()->startHosting();
    std::this_thread::sleep_for(std::chrono::milliseconds{1000});

    std::string uri = "";
    mocks.OnCallFunc(onDiscoveryResource).Do(
            [this, &uri, &testObject](RCSRemoteResourceObject::Ptr ptr)
            {
                if(testObject.use_count() <= 0)
                {
                    return;
                }
                if(ptr->getUri() == testObject->getServerUri())
                {
                    uri = ptr->getUri();
                    notifyCondition();
                }
            });

    discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(
            RCSAddress::multicast(), "Resource.Hosting", onDiscoveryResource);
    waitForCondition(2000);

    std::string mirroredUri = { testObject->getServerUri() };

    testObject->destroy();

    ASSERT_EQ(uri, mirroredUri);
}

TEST_F(ResourceHostingTest, startHosting)
{
    ResourceEncapsulationTestSimulator::Ptr testObject
        = std::make_shared<ResourceEncapsulationTestSimulator>();
    testObject->createResource();

    ResourceHosting::getInstance()->startHosting();
    std::this_thread::sleep_for(std::chrono::milliseconds{1000});

    testObject->destroy();
}

TEST_F(ResourceHostingTest, stopHosting)
{
    ResourceEncapsulationTestSimulator::Ptr testObject
        = std::make_shared<ResourceEncapsulationTestSimulator>();
    testObject->createResource();

    ResourceHosting::getInstance()->startHosting();
    std::this_thread::sleep_for(std::chrono::milliseconds{1000});
    
    testObject->destroy();

    ResourceHosting::getInstance()->stopHosting();
}