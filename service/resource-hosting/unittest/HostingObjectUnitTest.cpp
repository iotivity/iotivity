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
#include "HostingObject.h"

#include "RCSDiscoveryManager.h"

using namespace testing;
using namespace OIC::Service;

namespace
{
    const std::string HOSTING_RESOURCE_TYPE = "oic.r.resourcehosting";
    const std::string TEST_ATT_KEY = "Temperature";

    bool isStarted = false;
    bool isFinished = false;

    ResourceEncapsulationTestSimulator testObject;
    RCSRemoteResourceObject::Ptr remoteObject;

    HostingObject::Ptr instance;
    RCSRemoteResourceObject::Ptr discoveredResource;

    std::condition_variable responseCon;

    void onDestroy() { }
    void onDiscoveryResource(RCSRemoteResourceObject::Ptr){ }
    void onUpdatedCache(const RCSResourceAttributes &) { }
    void onSetAttributes(const RCSResourceAttributes &, int) { }

    void setup()
    {
        if(!isStarted)
        {
            testObject.defaultRunSimulator();
            remoteObject = testObject.getRemoteResource();

            instance = HostingObject::createHostingObject(
                        remoteObject, &onDestroy);

            testObject.getResourceServer()->setAttribute(
                    "Temperature", RCSResourceAttributes::Value((int)0));

            isStarted = true;
        }
    }

    void tearDown()
    {
        if(isFinished)
        {
            testObject.destroy();
            instance.reset();
            isStarted = false;
        }
    }
}

class HostingObjectTest : public TestWithMock
{
public:
    std::mutex mutexForCondition;

protected:

    void SetUp()
    {
        TestWithMock::SetUp();
        setup();
    }

    void TearDown()
    {
        TestWithMock::TearDown();
        tearDown();
    }

public:
    void waitForCondition(int waitingTime = 1000)
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, std::chrono::milliseconds{ waitingTime });
    }

    void notifyCondition()
    {
        responseCon.notify_all();
    }

};

TEST_F(HostingObjectTest, startCachingAtInitialize)
{
    EXPECT_TRUE(remoteObject->isCaching());
}

TEST_F(HostingObjectTest, startMonitoringAtInitialize)
{
    ASSERT_TRUE(remoteObject->isMonitoring());
}

TEST_F(HostingObjectTest, getRemoteResourceisValid)
{
    ASSERT_EQ(remoteObject->getUri(), instance->getRemoteResource()->getUri());
}

TEST_F(HostingObjectTest, createMirroredServer)
{
    int waitForResponse = 1000;
    std::string uri = "";

    std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask = { };

    waitForCondition(waitForResponse);
    mocks.OnCallFunc(onDiscoveryResource).Do(
            [this, &uri, &discoveryTask, &testObject, &discoveredResource]
             (RCSRemoteResourceObject::Ptr ptr)
            {
                if(ptr->getUri() == testObject.getHostedServerUri())
                {
                    uri = ptr->getUri();
                    discoveredResource = ptr;
                    discoveryTask->cancel();
                    notifyCondition();
                }
            });

    discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByType(
            RCSAddress::multicast(), "/oic/res", HOSTING_RESOURCE_TYPE, onDiscoveryResource);
    waitForCondition(waitForResponse);

    EXPECT_EQ(testObject.getHostedServerUri(), uri);
}

TEST_F(HostingObjectTest, UpdateCachedDataWhenChangedOriginResource)
{
    int waitForResponse = 1000;
    RCSResourceAttributes::Value result = { };

    mocks.OnCallFunc(onUpdatedCache).Do(
            [this, &result](const RCSResourceAttributes & att)
            {
                result = att.at(TEST_ATT_KEY);
                notifyCondition();
            });

    discoveredResource->startCaching(onUpdatedCache);
    std::this_thread::sleep_for(std::chrono::milliseconds {waitForResponse});

    RCSResourceAttributes::Value settingValue = 10;
    testObject.getResourceServer()->setAttribute(TEST_ATT_KEY, settingValue);
    waitForCondition(waitForResponse);

    isFinished = true;

    EXPECT_EQ(result.toString(), settingValue.toString());

}

TEST_F(HostingObjectTest, SetDataToMirroredResource)
{
    int waitForResponse = 1000;
    RCSResourceAttributes::Value result = { };

    mocks.ExpectCallFunc(onSetAttributes).Do(
            [this, & result](const RCSResourceAttributes &att, int)
            {
                result = att.at(TEST_ATT_KEY);
                notifyCondition();
            });
    RCSResourceAttributes setAttrs;
    RCSResourceAttributes::Value settingValue = 20;
    setAttrs[TEST_ATT_KEY] = settingValue;
    discoveredResource->setRemoteAttributes(setAttrs, onSetAttributes);
    waitForCondition(waitForResponse);

    EXPECT_EQ(result.toString(), settingValue.toString());
}

TEST_F(HostingObjectTest, ExpectCallOnDestroyWhenStopHostingObject)
{
    int waitForResponse = 1000;

    mocks.ExpectCallFunc(onDestroy).Do(
            [& responseCon]()
            {
                responseCon.notify_all();
            });

    testObject.destroy();
    instance.reset();
    waitForCondition(waitForResponse);
}
