//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include <gtest/gtest.h>
#include <HippoMocks/hippomocks.h>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <chrono>

#include "ocstack.h"

#include "NSCommon.h"
#include "NSConsumerInterface.h"

#include "NSProviderSimulator.h"

namespace
{
    NSProviderSimulator g_providerSimul;
    NSProvider * g_provider;

    std::atomic_bool g_isStartedStack(false);

    std::chrono::milliseconds g_waitForResponse(500);

    std::condition_variable responseCon;
    std::mutex mutexForCondition;

}

class TestWithMock: public testing::Test
{
public:
    MockRepository mocks;

protected:
    virtual ~TestWithMock() noexcept(noexcept(std::declval<Test>().~Test())) {}

    virtual void TearDown() {
        try
        {
            mocks.VerifyAll();
        }
        catch (...)
        {
            mocks.reset();
            throw;
        }
    }
};

class NotificationConsumerTest : public TestWithMock
{
public:
    NotificationConsumerTest() = default;
    ~NotificationConsumerTest() = default;

    static void NSProviderDiscoveredCallbackEmpty(NSProvider *)
    {
        std::cout << __func__ << std::endl;
    }

    static void NSNotificationReceivedCallbackEmpty(NSProvider *, NSMessage *) { }

    static void NSSyncCallbackEmpty(NSProvider *, NSSync *) { }

    static void foundResourceEmpty(std::shared_ptr< OC::OCResource >) { }

protected:

    void SetUp()
    {
        TestWithMock::SetUp();

        if (g_isStartedStack == false)
        {
            OC::PlatformConfig cfg
            {
                OC::ServiceType::InProc,
                OC::ModeType::Both,
                "0.0.0.0",
                0,
                OC::QualityOfService::LowQos
            };
            OC::OCPlatform::Configure(cfg);

            try
            {
                OC::OCPlatform::stopPresence();
            }
            catch (...)
            {

            }

            g_isStartedStack = true;
        }

    }

    void TearDown()
    {
        TestWithMock::TearDown();
    }

};

TEST_F(NotificationConsumerTest, StartConsumerPositive)
{
    EXPECT_EQ(NS_OK,
              NSStartConsumer(
                    NSProviderDiscoveredCallbackEmpty,
                    NSNotificationReceivedCallbackEmpty,
                    NSSyncCallbackEmpty));
}

TEST_F(NotificationConsumerTest, StopConsumerPositive)
{
    EXPECT_EQ(NSStopConsumer(), NS_OK);
}

TEST_F(NotificationConsumerTest, DiscoverProviderWithNonAccepterWhenStartedConsumerFirst)
{
    mocks.ExpectCallFunc(NSProviderDiscoveredCallbackEmpty).Do(
            [this, & responseCon](NSProvider *)
            {
                std::cout << "Call Discovered" << std::endl;
                responseCon.notify_all();
            });

    NSStartConsumer(
            NSProviderDiscoveredCallbackEmpty,
            NSNotificationReceivedCallbackEmpty,
            NSSyncCallbackEmpty);

    g_providerSimul.setAccepter(1);
    g_providerSimul.createNotificationResource();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    NSStopConsumer();
    g_providerSimul.deleteNotificationResource();
}

TEST_F(NotificationConsumerTest, DiscoverProviderWithNonAccepterWhenStartedConsumerAfter)
{
    g_providerSimul.setAccepter(1);
    g_providerSimul.createNotificationResource();
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    mocks.ExpectCallFunc(NSProviderDiscoveredCallbackEmpty).Do(
            [this, & responseCon](NSProvider *)
            {
                std::cout << "Call Discovered" << std::endl;
                responseCon.notify_all();
            });

    NSStartConsumer(
            NSProviderDiscoveredCallbackEmpty,
            NSNotificationReceivedCallbackEmpty,
            NSSyncCallbackEmpty);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

}

TEST_F(NotificationConsumerTest, DiscoverProviderWithNonAccepterWhenRescan)
{
    mocks.ExpectCallFunc(NSProviderDiscoveredCallbackEmpty)
            .Do(
            [this, & responseCon](NSProvider * provider)
            {
                std::cout << "Call Discovered" << std::endl;
                g_provider = provider;
                responseCon.notify_all();
            });

    NSRescanProvider();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

//    NSStopConsumer();
}

TEST_F(NotificationConsumerTest, ExpectSubscribeSuccess)
{
    NSResult ret = NSSubscribe(g_provider);
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(NS_OK, ret);
}

TEST_F(NotificationConsumerTest, ExpectReceiveNotification)
{
    std::string id = "id";
    std::string title = "title";
    std::string msg = "msg";

    mocks.ExpectCallFunc(NSNotificationReceivedCallbackEmpty).Do(
            [](NSProvider *, NSMessage * message)
            {
                std::cout << "Income Notification : " << message->mId << std::endl;
                NSDropNSObject(message);
            });

    g_providerSimul.notifyMessage(id, title, msg);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

}

TEST_F(NotificationConsumerTest, ExpectUnsubscribeSuccess)
{
    NSResult ret = NSUnsubscribe(g_provider);
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(NS_OK, ret);
}

TEST_F(NotificationConsumerTest, ExpectReceiveNotificationWithAccepterisProvider)
{
    std::string id = "ExpectReceiveNotificationWithAccepterisProvider";
    std::string title = "title";
    std::string msg = "msg";

    g_providerSimul.setAccepter(0);

    NSRescanProvider();
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    mocks.ExpectCallFunc(NSNotificationReceivedCallbackEmpty).Do(
            [](NSProvider *, NSMessage * message)
            {
                std::cout << "Income Notification : " << message->mId << std::endl;
                NSDropNSObject(message);
            });

    g_providerSimul.notifyMessage(id, title, msg);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

//    g_providerSimul.deleteNotificationResource();
//    NSStopConsumer();
}

TEST_F(NotificationConsumerTest, ExpectCallbackReadCheckWhenProviderNotifySync)
{
    std::string id = "ExpectCallbackReadCheckWhenProviderNotifySync";
    std::string title = "title";
    std::string msg = "msg";

    NSSyncTypes type = Notification_Dismiss;

    mocks.OnCallFunc(NSNotificationReceivedCallbackEmpty).Do(
            [](NSProvider *, NSMessage * message)
            {
                std::cout << "Income Notification : " << message->mId << std::endl;
                NSDropNSObject(message);
            });

    mocks.ExpectCallFunc(NSSyncCallbackEmpty).Do(
            [& type](NSProvider *, NSSync * sync)
            {
                std::cout << "Income Notification : " << sync->mMessageId
                        << ", State : " << sync->mState << std::endl;
                type = sync->mState;

            });

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    g_providerSimul.sendRead(id);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

//    g_providerSimul.deleteNotificationResource();
//    NSStopConsumer();

    EXPECT_EQ(Notification_Read, type);
}

TEST_F(NotificationConsumerTest, ExpectCallbackDismissCheckWhenProviderNotifySync)
{
    std::string id = "ExpectCallbackDismissCheckWhenProviderNotifySync";
    std::string title = "title";
    std::string msg = "msg";

    NSSyncTypes type = Notification_Read;

    mocks.OnCallFunc(NSNotificationReceivedCallbackEmpty).Do(
            [](NSProvider *, NSMessage * message)
            {
                std::cout << "Income Notification : " << message->mId << std::endl;
                NSDropNSObject(message);
            });

    mocks.ExpectCallFunc(NSSyncCallbackEmpty).Do(
            [& type](NSProvider *, NSSync * sync)
            {
                std::cout << "Income Notification : " << sync->mMessageId
                        << ", State : " << sync->mState << std::endl;
                type = sync->mState;

            });

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    g_providerSimul.sendDismiss(id);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

//    g_providerSimul.deleteNotificationResource();
//    NSStopConsumer();

    EXPECT_EQ(Notification_Dismiss, type);
}

TEST_F(NotificationConsumerTest, ExpectCallbackReadCheckWhenConsumerPostSync)
{
    std::string id = "ExpectCallbackReadCheckWhenConsumerPostSync";
    std::string title = "title";
    std::string msg = "msg";

    NSSyncTypes type = Notification_Dismiss;

    mocks.OnCallFunc(NSNotificationReceivedCallbackEmpty).Do(
            [](NSProvider *, NSMessage * message)
            {
                std::cout << "Income Notification : " << message->mId << std::endl;
                NSConsumerReadCheck(message);
                std::unique_lock< std::mutex > lock{ mutexForCondition };
                responseCon.wait_for(lock, g_waitForResponse);
            });

    mocks.ExpectCallFunc(NSSyncCallbackEmpty).Do(
            [& type](NSProvider *, NSSync * sync)
            {
                std::cout << "Income Notification : " << sync->mMessageId
                        << ", State : " << sync->mState << std::endl;
                type = sync->mState;

            });

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

//    g_providerSimul.deleteNotificationResource();
//    NSStopConsumer();

    EXPECT_EQ(Notification_Read, type);
}

TEST_F(NotificationConsumerTest, ExpectCallbackDismissCheckWhenConsumerPostSync)
{
    std::string id = "ExpectCallbackDismissCheckWhenConsumerPostSync";
    std::string title = "title";
    std::string msg = "msg";

    NSSyncTypes type = Notification_Read;

    mocks.OnCallFunc(NSNotificationReceivedCallbackEmpty).Do(
            [](NSProvider *, NSMessage * message)
            {
                std::cout << "Income Notification : " << message->mId << std::endl;
                NSConsumerDismissCheck(message);
                std::unique_lock< std::mutex > lock{ mutexForCondition };
                responseCon.wait_for(lock, g_waitForResponse);
            });

    mocks.ExpectCallFunc(NSSyncCallbackEmpty).Do(
            [& type](NSProvider *, NSSync * sync)
            {
                std::cout << "Income Notification : " << sync->mMessageId
                        << ", State : " << sync->mState << std::endl;
                type = sync->mState;

            });

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    g_providerSimul.deleteNotificationResource();
    NSStopConsumer();

    EXPECT_EQ(Notification_Dismiss, type);
}
