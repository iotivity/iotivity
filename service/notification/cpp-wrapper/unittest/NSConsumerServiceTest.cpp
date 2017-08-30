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

/**
 * @file NSConsumerServiceTest.cpp
 *
 * This file contains Notification consumer service test cases that involves network related methods.
 * The test cases are excluded from the build. 
 * They can be executed seperately (local running) from the path <iotivity>/out/linux/<target_os>/<target_arch>/service/notification/cpp-wrapper/unittest
 */

#include <gtest/gtest.h>
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
    std::shared_ptr<OIC::Service::NSProvider> g_provider;

    std::atomic_bool g_isStartedStack(false);

    std::chrono::milliseconds g_waitForResponse(1000);

    std::condition_variable responseCon;
    std::mutex mutexForCondition;

    enum class NSSelector
    {
        NS_SELECTION_CONSUMER = 0,
        NS_SELECTION_PROVIDER = 1
    };

    OIC::Service::NSProviderState g_expectedState = OIC::Service::NSProviderState::DENY;
    OIC::Service::NSProviderState g_revState = OIC::Service::NSProviderState::DENY;

    uint64_t g_messageId = 0;

    OIC::Service::NSSyncInfo::NSSyncType g_expectedSyncType =
        OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;
    OIC::Service::NSSyncInfo::NSSyncType g_syncType =
        OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;

}

class TestWithMock: public testing::Test
{
    protected:
        virtual ~TestWithMock() noexcept(noexcept(std::declval<Test>().~Test()))
        {

        }

        virtual void TearDown()
        {

        }
};

class NotificationServiceConsumerTest : public TestWithMock
{
    public:
        NotificationServiceConsumerTest() = default;
        ~NotificationServiceConsumerTest() = default;

        static void ProviderDiscoveredCallback( std::shared_ptr<OIC::Service::NSProvider> provider)
        {
            std::cout << __func__ << std::endl;
            g_provider = provider;
            responseCon.notify_all();
        }

        static void NotificationReceivedCallback( OIC::Service::NSMessage message)
        {
            std::cout << "Income Notification : " << message.getMessageId() << std::endl;
            g_messageId = message.getMessageId();
            responseCon.notify_all();
        }

        static void SyncCallback(OIC::Service::NSSyncInfo sync)
        {
            std::cout << __func__ << std::endl;
            g_syncType = sync.getState();
            if (g_expectedSyncType == sync.getState())
            {
                std::cout << "[Expected]" << __func__ << std::endl;
                responseCon.notify_all();
            }
        }

        static void FoundResource(std::shared_ptr< OC::OCResource >)
        {
            std::cout << __func__ << std::endl;
        }

        static void ProviderChangedCallback( OIC::Service::NSProviderState state)
        {
            std::cout << __func__ << std::endl;

            if (g_expectedState == state)
            {
                g_revState = state;
                std::cout << "[Expected]" << __func__ << std::endl;
                responseCon.notify_all();
            }
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
    OIC::Service::NSConsumerService::getInstance()->start(ProviderDiscoveredCallback);
}

TEST_F(NotificationServiceConsumerTest, StopConsumerPositive)
{
    OIC::Service::NSConsumerService::getInstance()->stop();
}

TEST_F(NotificationServiceConsumerTest, DiscoverProviderWithNonAccepterWhenStartedConsumerFirst)
{
    OIC::Service::NSConsumerService::getInstance()->start(ProviderDiscoveredCallback);

    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_CONSUMER);
    g_providerSimul.createNotificationResource();

    std::unique_lock< std::mutex > lock { mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    OIC::Service::NSConsumerService::getInstance()->stop();
    g_providerSimul.deleteNotificationResource();
}

TEST_F(NotificationServiceConsumerTest, DiscoverProviderWithNonAccepterWhenStartedConsumerAfter)
{
    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_CONSUMER);
    g_providerSimul.createNotificationResource();
    {
        std::unique_lock< std::mutex > lock { mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    OIC::Service::NSConsumerService::getInstance()->start(ProviderDiscoveredCallback);

    std::unique_lock< std::mutex > lock { mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";

}

TEST_F(NotificationServiceConsumerTest, DiscoverProviderWithNonAccepterWhenRescan)
{
    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_CONSUMER);

    OIC::Service::NSConsumerService::getInstance()->rescanProvider();

    std::unique_lock< std::mutex > lock { mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);
}

TEST_F(NotificationServiceConsumerTest, ExpectSubscribeSuccess)
{
    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";
    g_expectedState = OIC::Service::NSProviderState::ALLOW;

    g_provider->setListener( (OIC::Service::NSProvider::ProviderStateCallback)ProviderChangedCallback,
                             (OIC::Service::NSProvider::MessageReceivedCallback)NotificationReceivedCallback,
                             (OIC::Service::NSProvider::SyncInfoReceivedCallback)SyncCallback);
    if (!g_provider->isSubscribed())
    {
        g_provider->subscribe();
    }
    std::unique_lock< std::mutex > lock { mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);
    EXPECT_EQ(g_expectedState, g_revState);
}

TEST_F(NotificationServiceConsumerTest, ExpectReceiveNotification)
{
    uint64_t id = 10;
    std::string title = "title";
    std::string msg = "msg";

    g_providerSimul.notifyMessage(id, title, msg);

    std::unique_lock< std::mutex > lock { mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);
    EXPECT_EQ(id, g_messageId);
    OIC::Service::NSConsumerService::getInstance()->stop();
}

TEST_F(NotificationServiceConsumerTest, DiscoverProviderWithAccepterisProvider)
{
    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_PROVIDER);

    OIC::Service::NSConsumerService::getInstance()->start(ProviderDiscoveredCallback);


    std::unique_lock< std::mutex > lock { mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";
    g_provider->setListener( (OIC::Service::NSProvider::ProviderStateCallback)
                             ProviderChangedCallback,
                             (OIC::Service::NSProvider::MessageReceivedCallback)NotificationReceivedCallback,
                             (OIC::Service::NSProvider::SyncInfoReceivedCallback)SyncCallback);
    if (!g_provider->isSubscribed())
    {
        g_provider->subscribe();
    }

}

TEST_F(NotificationServiceConsumerTest, ExpectReceiveNotificationWithAccepterisProvider)
{
    uint64_t id = 11;
    std::string title = "title";
    std::string msg = "msg";

    g_providerSimul.notifyMessage(id, title, msg);

    std::unique_lock< std::mutex > lock { mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(id, g_messageId);
}

TEST_F(NotificationServiceConsumerTest, ExpectCallbackReadCheckWhenProviderNotifySync)
{
    uint64_t id = 12;
    std::string title = "title";
    std::string msg = "msg";

    g_expectedSyncType = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;
    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock { mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    g_providerSimul.sendRead(id);
    {
        std::unique_lock< std::mutex > lock { mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    EXPECT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ, g_syncType);
    g_expectedSyncType = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;
}

TEST_F(NotificationServiceConsumerTest, ExpectCallbackDismissCheckWhenProviderNotifySync)
{
    uint64_t id = 13;
    std::string title = "title";
    std::string msg = "msg";
    g_syncType = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;
    g_expectedSyncType = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock { mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    g_providerSimul.sendDismiss(id);
    {
        std::unique_lock< std::mutex > lock { mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    EXPECT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED, g_syncType);
    g_expectedSyncType = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;
}

TEST_F(NotificationServiceConsumerTest, ExpectCallbackReadCheckWhenConsumerPostSync)
{
    uint64_t id = 14;
    std::string title = "title";
    std::string msg = "msg";
    g_syncType = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;
    g_expectedSyncType = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;

    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock { mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    g_provider->sendSyncInfo(g_messageId,
                             OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ);
    std::unique_lock< std::mutex > lock { mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ, g_syncType);
    g_expectedSyncType = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;
}

TEST_F(NotificationServiceConsumerTest, ExpectCallbackDismissCheckWhenConsumerPostSync)
{
    uint64_t id = 15;
    std::string title = "title";
    std::string msg = "msg";
    g_syncType = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;
    g_expectedSyncType = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;

    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock { mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    g_provider->sendSyncInfo(g_messageId,
                             OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED);
    std::unique_lock< std::mutex > lock { mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED, g_syncType);
    g_expectedSyncType = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;
}

TEST_F(NotificationServiceConsumerTest, ExpectGetProviderSuccessWithValidProviderId)
{
    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";

    std::shared_ptr<OIC::Service::NSProvider> provider =
        OIC::Service::NSConsumerService::getInstance()->getProvider(
            g_provider->getProviderId());
    int ret = strcmp(provider->getProviderId().c_str(), g_provider->getProviderId().c_str());
    EXPECT_EQ(0, ret);
}

TEST_F(NotificationServiceConsumerTest, ExpectGetProviderSuccessWithInvalidProviderId)
{
    std::shared_ptr<OIC::Service::NSProvider> provider =
        OIC::Service::NSConsumerService::getInstance()->getProvider(
            "123456789012345678901234567890123457");
    bool res = (provider == nullptr);
    EXPECT_EQ(res, 1);
}

TEST_F(NotificationServiceConsumerTest, ExpectCallbackTopicUpdated)
{
    g_expectedState = OIC::Service::NSProviderState::TOPIC;

    NSProviderSimulator::NS_TopicList topics;
    topics.push_back("1");
    topics.push_back("2");
    topics.push_back("3");

    g_providerSimul.setTopics(topics);
    std::chrono::milliseconds waitForTopicUdateComplete(1000);
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(OIC::Service::NSProviderState::TOPIC, g_revState);
    responseCon.wait_for(lock, waitForTopicUdateComplete);
    g_expectedState = OIC::Service::NSProviderState::STOPPED;
}

TEST_F(NotificationServiceConsumerTest, ExpectEQTopicList)
{
    bool isSame = true;

    NSProviderSimulator::NS_TopicList topics;
    topics.push_back("1");
    topics.push_back("2");
    topics.push_back("3");

    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";

    auto  retTopic = g_provider->getTopicList()->getTopicsList();
    auto it1 = retTopic.begin();
    auto it2 = topics.begin();
    while ( it1 != retTopic.end() || it2 != topics.end() )
    {
        if ((*it1).getTopicName() !=  *it2)
        {
            isSame = false; break;
        }
        it1++; it2++;
    }


    EXPECT_EQ(true, isSame);
}

TEST_F(NotificationServiceConsumerTest, ExpectFailUpdateTopicOnConsumer)
{
    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";

    auto retTopic = g_provider->getTopicList();
    for (auto it : retTopic->getTopicsList())
    {
        std::cout << "Topic Name: " << it.getTopicName() << std::endl;
        std::cout << "state : " << (int) it.getState() << std::endl;
        it.setState(OIC::Service::NSTopic::NSTopicState::SUBSCRIBED);
    }
    OIC::Service::NSResult ret = g_provider->updateTopicList(retTopic);
    std::cout << "ret : " << (int) ret << std::endl;

    EXPECT_EQ(OIC::Service::NSResult::ERROR, ret);
}


TEST_F(NotificationServiceConsumerTest, ExpectCallbackDeletedProvider)
{
    g_expectedState = OIC::Service::NSProviderState::STOPPED;
    g_providerSimul.deleteNotificationResource();

    std::unique_lock< std::mutex > lock { mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(g_revState, OIC::Service::NSProviderState::STOPPED);
    OIC::Service::NSConsumerService::getInstance()->stop();
}
