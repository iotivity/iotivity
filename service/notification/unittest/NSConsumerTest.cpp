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

    enum class NSSelector
    {
        NS_SELECTION_CONSUMER = 0,
        NS_SELECTION_PROVIDER = 1
    };

    NSConsumerConfig cfg;
}

class TestWithMock: public testing::Test
{
public:
    MockRepository mocks;

protected:
    virtual ~TestWithMock() noexcept(noexcept(std::declval<Test>().~Test()))
    {

    }

    virtual void TearDown()
    {
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

    static void NSNotificationReceivedCallbackEmpty(NSMessage *)
    {
        std::cout << __func__ << std::endl;
    }

    static void NSSyncCallbackEmpty(NSSyncInfo *)
    {
        std::cout << __func__ << std::endl;
    }

    static void NSFoundResourceEmpty(std::shared_ptr< OC::OCResource >)
    {
        std::cout << __func__ << std::endl;
    }

    static void NSProviderChangedCallback(NSProvider *,  NSProviderState)
    {
        std::cout << __func__ << std::endl;
    }

protected:

    void SetUp()
    {
        TestWithMock::SetUp();

        if (g_isStartedStack == false)
        {
            OC::PlatformConfig occfg
            {
                OC::ServiceType::InProc,
                OC::ModeType::Both,
                "0.0.0.0",
                0,
                OC::QualityOfService::LowQos
            };
            OC::OCPlatform::Configure(occfg);

            try
            {
                OC::OCPlatform::stopPresence();
            }
            catch (...)
            {

            }

            g_isStartedStack = true;

            cfg.changedCb = NSProviderChangedCallback;
            cfg.messageCb = NSNotificationReceivedCallbackEmpty;
            cfg.syncInfoCb = NSSyncCallbackEmpty;
        }

    }

    void TearDown()
    {
        TestWithMock::TearDown();
    }

};

TEST_F(NotificationConsumerTest, StartConsumerPositive)
{
    EXPECT_EQ(NS_OK, NSStartConsumer(cfg));
}

TEST_F(NotificationConsumerTest, StopConsumerPositive)
{
    EXPECT_EQ(NSStopConsumer(), NS_OK);
}

TEST_F(NotificationConsumerTest, DiscoverProviderWithNonAccepterWhenStartedConsumerFirst)
{
    mocks.ExpectCallFunc(NSProviderChangedCallback).Do(
            [this](NSProvider *, NSProviderState)
            {
                std::cout << "Call Discovered" << std::endl;
                responseCon.notify_all();
            });

    NSStartConsumer(cfg);

    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_CONSUMER);
    g_providerSimul.createNotificationResource();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    NSStopConsumer();
    g_providerSimul.deleteNotificationResource();
}

TEST_F(NotificationConsumerTest, DiscoverProviderWithNonAccepterWhenStartedConsumerAfter)
{
    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_CONSUMER);
    g_providerSimul.createNotificationResource();
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    mocks.ExpectCallFunc(NSProviderChangedCallback).Do(
            [this](NSProvider * provider, NSProviderState)
            {
                std::cout << "Call Discovered" << std::endl;
                g_provider = provider;
                responseCon.notify_all();
            });

    NSStartConsumer(cfg);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

}

TEST_F(NotificationConsumerTest, DiscoverProviderWithNonAccepterWhenRescan)
{
    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_CONSUMER);
    mocks.ExpectCallFunc(NSProviderChangedCallback).Do(
            [this](NSProvider * provider, NSProviderState)
            {
                std::cout << "Call Discovered" << std::endl;
                g_provider = provider;
                std::cout << g_provider->providerId << std::endl;
                responseCon.notify_all();
            });

    NSRescanProvider();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

//    NSStopConsumer();
}

TEST_F(NotificationConsumerTest, ExpectSubscribeSuccess)
{
//    mocks.ExpectCallFunc(NSSubscriptionAcceptedCallback).Do(
//            [](NSProvider * )
//            {
//                std::cout << "Income Accepted subscription : " << std::endl;
//            });

    NSResult ret = NSSubscribe(g_provider);
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(NS_OK, ret);
}

TEST_F(NotificationConsumerTest, ExpectReceiveNotification)
{
    uint64_t id = 10;
    std::string title = "title";
    std::string msg = "msg";

    mocks.ExpectCallFunc(NSNotificationReceivedCallbackEmpty).Do(
            [](NSMessage * message)
            {
                std::cout << "Income Notification : " << message->messageId << std::endl;
            });

    g_providerSimul.notifyMessage(id, title, msg);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    NSStopConsumer();
}

TEST_F(NotificationConsumerTest, ExpectReceiveNotificationWithAccepterisProvider)
{
    uint64_t id = 11;
    std::string title = "title";
    std::string msg = "msg";

    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_PROVIDER);

    NSStartConsumer(cfg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    mocks.ExpectCallFunc(NSNotificationReceivedCallbackEmpty).Do(
            [](NSMessage * message)
            {
                std::cout << "Income Notification : " << message->messageId << std::endl;
            });

    g_providerSimul.notifyMessage(id, title, msg);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

//    g_providerSimul.deleteNotificationResource();
//    NSStopConsumer();
}

TEST_F(NotificationConsumerTest, ExpectCallbackReadCheckWhenProviderNotifySync)
{
    uint64_t id = 12;
    std::string title = "title";
    std::string msg = "msg";

    NSSyncType type = NS_SYNC_DELETED;

    mocks.OnCallFunc(NSNotificationReceivedCallbackEmpty).Do(
            [](NSMessage * message)
            {
                std::cout << "Income Notification : " << message->messageId << std::endl;
            });

    mocks.ExpectCallFunc(NSSyncCallbackEmpty).Do(
            [& type](NSSyncInfo * sync)
            {
                std::cout << "Income SyncInfo : " << sync->messageId
                        << ", State : " << sync->state << std::endl;
                type = sync->state;

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

    EXPECT_EQ(NS_SYNC_READ, type);
}

TEST_F(NotificationConsumerTest, ExpectCallbackDismissCheckWhenProviderNotifySync)
{
    uint64_t id = 13;
    std::string title = "title";
    std::string msg = "msg";

    NSSyncType type = NS_SYNC_READ;

    mocks.OnCallFunc(NSNotificationReceivedCallbackEmpty).Do(
            [](NSMessage * message)
            {
                std::cout << "Income Notification : " << message->messageId << std::endl;
            });

    mocks.ExpectCallFunc(NSSyncCallbackEmpty).Do(
            [& type](NSSyncInfo * sync)
            {
                std::cout << "Income Notification : " << sync->messageId
                        << ", State : " << sync->state << std::endl;
                type = sync->state;

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

    EXPECT_EQ(NS_SYNC_DELETED, type);
}

TEST_F(NotificationConsumerTest, ExpectCallbackReadCheckWhenConsumerPostSync)
{
    uint64_t id = 14;
    std::string title = "title";
    std::string msg = "msg";

    NSSyncType type = NS_SYNC_DELETED;

    mocks.OnCallFunc(NSNotificationReceivedCallbackEmpty).Do(
            [](NSMessage * message)
            {
                std::cout << "Income Notification : " << message->messageId << std::endl;
                NSConsumerSendSyncInfo(message->providerId, message->messageId, NS_SYNC_READ);
                std::unique_lock< std::mutex > lock{ mutexForCondition };
                responseCon.wait_for(lock, g_waitForResponse);
            });

    mocks.ExpectCallFunc(NSSyncCallbackEmpty).Do(
            [& type](NSSyncInfo * sync)
            {
                std::cout << "Income Notification : " << sync->messageId
                        << ", State : " << sync->state << std::endl;
                type = sync->state;

            });

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

//    g_providerSimul.deleteNotificationResource();
//    NSStopConsumer();

    EXPECT_EQ(NS_SYNC_READ, type);
}

TEST_F(NotificationConsumerTest, ExpectCallbackDismissCheckWhenConsumerPostSync)
{
    uint64_t id = 15;
    std::string title = "title";
    std::string msg = "msg";

    NSSyncType type = NS_SYNC_READ;

    mocks.OnCallFunc(NSNotificationReceivedCallbackEmpty).Do(
            [](NSMessage * message)
            {
                std::cout << "Income Notification : " << message->messageId << std::endl;
                NSConsumerSendSyncInfo(message->providerId, message->messageId, NS_SYNC_DELETED);
                std::unique_lock< std::mutex > lock{ mutexForCondition };
                responseCon.wait_for(lock, g_waitForResponse);
            });

    mocks.ExpectCallFunc(NSSyncCallbackEmpty).Do(
            [& type](NSSyncInfo * sync)
            {
                std::cout << "Income Notification : " << sync->messageId
                        << ", State : " << sync->state << std::endl;
                type = sync->state;

            });

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    EXPECT_EQ(NS_SYNC_DELETED, type);
}

TEST_F(NotificationConsumerTest, ExpectUnsubscribeSuccess)
{
    NSResult ret = NSUnsubscribe(g_provider);
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    g_providerSimul.deleteNotificationResource();
    NSStopConsumer();

    EXPECT_EQ(NS_OK, ret);
}
