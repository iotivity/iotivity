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

#include "NSUtils.h"
#include "NSSyncInfo.h"
#include "NSMessage.h"
#include "NSMediaContents.h"
#include "NSConsumerService.h"

#include "NSProviderServiceSimulator.h"

namespace
{
    NSProviderSimulator g_providerSimul;
     OIC::Service::NSProvider * g_provider;

    std::atomic_bool g_isStartedStack(false);

    std::chrono::milliseconds g_waitForResponse(2000);

    std::condition_variable responseCon;
    std::mutex mutexForCondition;

    enum class NSSelector
    {
        NS_SELECTION_CONSUMER = 0,
        NS_SELECTION_PROVIDER = 1
    };

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

class NotificationServiceConsumerTest : public TestWithMock
{
public:
    NotificationServiceConsumerTest() = default;
    ~NotificationServiceConsumerTest() = default;

    static void ProviderDiscoveredCallbackEmpty( OIC::Service::NSProvider *)
    {
        std::cout << __func__ << std::endl;
    }

    static void NotificationReceivedCallbackEmpty( OIC::Service::NSMessage *)
    {
        std::cout << __func__ << std::endl;
    }

    static void SyncCallbackEmpty(OIC::Service::NSSyncInfo *)
    {
        std::cout << __func__ << std::endl;
    }

    static void FoundResourceEmpty(std::shared_ptr< OC::OCResource >)
    {
        std::cout << __func__ << std::endl;
    }

    static void ProviderChangedCallbackEmpty( OIC::Service::NSProvider * , OIC::Service::NSResponse )
    {
        std::cout << __func__ << std::endl;
    }

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

TEST_F(NotificationServiceConsumerTest, StartConsumerPositive)
{
    OIC::Service::NSConsumerService::ConsumerConfig cfg;
    cfg.m_discoverCb = ProviderDiscoveredCallbackEmpty;
    cfg.m_changedCb = ProviderChangedCallbackEmpty;
    OIC::Service::NSConsumerService::getInstance()->Start(cfg);
}

TEST_F(NotificationServiceConsumerTest, StopConsumerPositive)
{
    OIC::Service::NSConsumerService::getInstance()->Stop();
}

TEST_F(NotificationServiceConsumerTest, DiscoverProviderWithNonAccepterWhenStartedConsumerFirst)
{
    mocks.ExpectCallFunc(ProviderDiscoveredCallbackEmpty).Do(
            [this]( OIC::Service::NSProvider * provider)
            {
                std::cout << "Call Discovered" << std::endl;
                g_provider = provider;
                g_provider->subscribe();
                g_provider->setListener((OIC::Service::NSProvider::MessageReceivedCallback)NotificationReceivedCallbackEmpty,
                                                   (OIC::Service::NSProvider::SyncInfoReceivedCallback)SyncCallbackEmpty);
                responseCon.notify_all();
            });

    OIC::Service::NSConsumerService::ConsumerConfig cfg;
    cfg.m_discoverCb = ProviderDiscoveredCallbackEmpty;
    cfg.m_changedCb = ProviderChangedCallbackEmpty;
    OIC::Service::NSConsumerService::getInstance()->Start(cfg);

    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_CONSUMER);
    g_providerSimul.createNotificationResource();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

     OIC::Service::NSConsumerService::getInstance()->Stop();
    g_providerSimul.deleteNotificationResource();
}

TEST_F(NotificationServiceConsumerTest, DiscoverProviderWithNonAccepterWhenStartedConsumerAfter)
{
    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_CONSUMER);
    g_providerSimul.createNotificationResource();
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    mocks.ExpectCallFunc(ProviderDiscoveredCallbackEmpty).Do(
            [this]( OIC::Service::NSProvider * provider)
            {
                std::cout << "Call Discovered" << std::endl;
                 g_provider->subscribe();
                g_provider->setListener((OIC::Service::NSProvider::MessageReceivedCallback)NotificationReceivedCallbackEmpty,
                                                   (OIC::Service::NSProvider::SyncInfoReceivedCallback)SyncCallbackEmpty);
                responseCon.notify_all();
            });

    OIC::Service::NSConsumerService::ConsumerConfig cfg;
    cfg.m_discoverCb = ProviderDiscoveredCallbackEmpty;
    cfg.m_changedCb = ProviderChangedCallbackEmpty;
    OIC::Service::NSConsumerService::getInstance()->Start(cfg);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

}

TEST_F(NotificationServiceConsumerTest, DiscoverProviderWithNonAccepterWhenRescan)
{
    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_CONSUMER);
    mocks.ExpectCallFunc(ProviderDiscoveredCallbackEmpty).Do(
            [this]( OIC::Service::NSProvider * provider)
            {
                std::cout << "Call Discovered" << std::endl;
                g_provider = provider;
                 g_provider->subscribe();
                g_provider->setListener((OIC::Service::NSProvider::MessageReceivedCallback)NotificationReceivedCallbackEmpty,
                                                   (OIC::Service::NSProvider::SyncInfoReceivedCallback)SyncCallbackEmpty);
                std::cout << g_provider->getProviderId() << std::endl;
                responseCon.notify_all();
            });

     OIC::Service::NSConsumerService::getInstance()->RescanProvider();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

//     OIC::Service::NSConsumerService::getInstance()->Stop();
}

TEST_F(NotificationServiceConsumerTest, ExpectSubscribeSuccess)
{
//    mocks.ExpectCallFunc(ProviderChangedCallbackEmpty).Do(
//            []( OIC::Service::NSProvider * , OIC::Service::NSResponse)
//            {
//                std::cout << "Income Accepted subscription : " << std::endl;
//            });

    g_provider->subscribe();
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);
}

TEST_F(NotificationServiceConsumerTest, ExpectReceiveNotification)
{
    uint64_t id = 10;
    std::string title = "title";
    std::string msg = "msg";

    mocks.ExpectCallFunc(NotificationReceivedCallbackEmpty).Do(
            []( OIC::Service::NSMessage * message)
            {
                std::cout << "Income Notification : " << message->getMessageId() << std::endl;
            });

    g_providerSimul.notifyMessage(id, title, msg);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

     OIC::Service::NSConsumerService::getInstance()->Stop();
}

TEST_F(NotificationServiceConsumerTest, ExpectReceiveNotificationWithAccepterisProvider)
{
    uint64_t id = 11;
    std::string title = "title";
    std::string msg = "msg";

    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_PROVIDER);

    OIC::Service::NSConsumerService::ConsumerConfig cfg;
    cfg.m_discoverCb = ProviderDiscoveredCallbackEmpty;
    cfg.m_changedCb = ProviderChangedCallbackEmpty;
    OIC::Service::NSConsumerService::getInstance()->Start(cfg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    mocks.ExpectCallFunc(NotificationReceivedCallbackEmpty).Do(
            []( OIC::Service::NSMessage * message)
            {
                std::cout << "Income Notification : " << message->getMessageId() << std::endl;
            });

    g_providerSimul.notifyMessage(id, title, msg);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

//    g_providerSimul.deleteNotificationResource();
//     OIC::Service::NSConsumerService::getInstance()->Stop();
}

TEST_F(NotificationServiceConsumerTest, ExpectCallbackReadCheckWhenProviderNotifySync)
{
    uint64_t id = 12;
    std::string title = "title";
    std::string msg = "msg";

    OIC::Service::NSSyncInfo::NSSyncType type = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;

    mocks.OnCallFunc(NotificationReceivedCallbackEmpty).Do(
            []( OIC::Service::NSMessage * message)
            {
                std::cout << "Income Notification : " << message->getMessageId() << std::endl;
            });

    mocks.ExpectCallFunc(SyncCallbackEmpty).Do(
            [& type](OIC::Service::NSSyncInfo * sync)
            {
                std::cout << "Income SyncInfo : " << sync->getMessageId()
                        << ", State : " << (int) sync->getState() << std::endl;
                type = sync->getState();

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
//     OIC::Service::NSConsumerService::getInstance()->Stop();

    EXPECT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ, type);
}

TEST_F(NotificationServiceConsumerTest, ExpectCallbackDismissCheckWhenProviderNotifySync)
{
    uint64_t id = 13;
    std::string title = "title";
    std::string msg = "msg";

    OIC::Service::NSSyncInfo::NSSyncType type = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;

    mocks.OnCallFunc(NotificationReceivedCallbackEmpty).Do(
            []( OIC::Service::NSMessage * message)
            {
                std::cout << "Income Notification : " << message->getMessageId() << std::endl;
            });

    mocks.ExpectCallFunc(SyncCallbackEmpty).Do(
            [& type](OIC::Service::NSSyncInfo * sync)
            {
                std::cout << "Income Notification : " << sync->getMessageId()
                        << ", State : " << (int) sync->getState() << std::endl;
                type = sync->getState();

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
//     OIC::Service::NSConsumerService::getInstance()->Stop();

    EXPECT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED, type);
}

TEST_F(NotificationServiceConsumerTest, ExpectCallbackReadCheckWhenConsumerPostSync)
{
    uint64_t id = 14;
    std::string title = "title";
    std::string msg = "msg";

    OIC::Service::NSSyncInfo::NSSyncType type = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;

    mocks.OnCallFunc(NotificationReceivedCallbackEmpty).Do(
            []( OIC::Service::NSMessage * message)
            {
                std::cout << "Income Notification : " << message->getMessageId() << std::endl;
                g_provider->SendSyncInfo(message->getMessageId(), OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ);
                std::unique_lock< std::mutex > lock{ mutexForCondition };
                responseCon.wait_for(lock, g_waitForResponse);
            });

    mocks.ExpectCallFunc(SyncCallbackEmpty).Do(
            [& type](OIC::Service::NSSyncInfo * sync)
            {
                std::cout << "Income Notification : " << sync->getMessageId()
                        << ", State : " << (int) sync->getState() << std::endl;
                type = sync->getState();

            });

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

//    g_providerSimul.deleteNotificationResource();
//     OIC::Service::NSConsumerService::getInstance()->Stop();

    EXPECT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ, type);
}

TEST_F(NotificationServiceConsumerTest, ExpectCallbackDismissCheckWhenConsumerPostSync)
{
    uint64_t id = 15;
    std::string title = "title";
    std::string msg = "msg";

    OIC::Service::NSSyncInfo::NSSyncType type = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;

    mocks.OnCallFunc(NotificationReceivedCallbackEmpty).Do(
            []( OIC::Service::NSMessage * message)
            {
                std::cout << "Income Notification : " << message->getMessageId() << std::endl;
                g_provider->SendSyncInfo(message->getMessageId(), OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED);
                std::unique_lock< std::mutex > lock{ mutexForCondition };
                responseCon.wait_for(lock, g_waitForResponse);
            });

    mocks.ExpectCallFunc(SyncCallbackEmpty).Do(
            [& type](OIC::Service::NSSyncInfo * sync)
            {
                std::cout << "Income Notification : " << sync->getMessageId()
                        << ", State : " << (int) sync->getState() << std::endl;
                type = sync->getState();

            });

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    EXPECT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED, type);
}

TEST_F(NotificationServiceConsumerTest, ExpectUnsubscribeSuccess)
{
    g_provider->unSubscribe();
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    g_providerSimul.deleteNotificationResource();
     OIC::Service::NSConsumerService::getInstance()->Stop();

}
