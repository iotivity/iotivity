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
#include <atomic>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <string.h>

#include "octypes.h"
#include "ocstack.h"

#include "NSCommon.h"
#include "NSConsumerInterface.h"

#include "NSProviderSimulator.h"
#include "NSUnittestUtil.h"

#define DEFAULT_G_TIMEOUT 1000

namespace
{
    NSProviderSimulator g_providerSimul;
    NSProvider * g_provider = NULL;

    std::atomic_bool g_isStartedStack(false);

    /// Reasonable timeout is set to 1000 ms in unsecured mode.
#ifdef SECURED
    unsigned int g_timeout = 2 * DEFAULT_G_TIMEOUT;
#else
    unsigned int g_timeout = DEFAULT_G_TIMEOUT;
#endif
    std::chrono::milliseconds g_waitForResponse(g_timeout);
 

    std::condition_variable messageReceive;
    std::mutex messageReceiveLock;

    std::condition_variable syncReceive;
    std::mutex syncReceiveLock;

    std::condition_variable providerChanged;
    std::mutex providerChangedLock;

    enum class NSSelector
    {
        NS_SELECTION_CONSUMER = 0,
        NS_SELECTION_PROVIDER = 1
    };

    NSConsumerConfig cfg;

    NSProviderSimulator::NS_TopicStateList g_topicStateList;

    NSProviderState revState = NS_STOPPED;
    NSProviderState expectedState = NS_STOPPED;
    uint64_t revId = 0;
    NSSyncType type = NS_SYNC_DELETED;

    const std::string testProviderID = "123456789012345678901234567890123456";

    static FILE* client_open(const char * path, const char * mode)
    {
        if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
        {
            std::string file_name = "./oic_svr_db_ns.dat";
#ifndef LOCAL_RUNNING
            file_name = "./service/notification/unittest/oic_svr_db_ns.dat";
#endif
            return fopen(file_name.c_str(), mode);
        }
        else
        {
            return fopen(path, mode);
        }
    }
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

class NotificationConsumerTest : public TestWithMock
{
public:
    NotificationConsumerTest() = default;
    ~NotificationConsumerTest() = default;

    static void NSNotificationReceivedCallback(NSMessage * message)
    {
        if (0 != testProviderID.compare(message->providerId))
        {
            removeMessage(message);
            return;
        }

        std::cout << __func__ << std::endl;
        std::cout << "Income Notification : " << message->messageId << std::endl;
        revId = message->messageId;

        removeMessage(message);
        messageReceive.notify_all();
    }

    static void NSSyncCallback(NSSyncInfo * sync)
    {
        if (0 != testProviderID.compare(sync->providerId))
        {
            free(sync);
            return;
        }

        std::cout << __func__ << std::endl;
        std::cout << "Income SyncInfo : " << sync->messageId
                << ", State : " << sync->state << std::endl;
        type = sync->state;

        free(sync);
        syncReceive.notify_all();
    }

    static void NSProviderChangedCallback(NSProvider * provider,  NSProviderState state)
    {
        if (0 != testProviderID.compare(provider->providerId))
        {
            free(provider);
            return;
        }

        std::cout << __func__ << " " << state << std::endl;
        if (g_provider)
        {
            free(g_provider);
            g_provider = NULL;
        }
        g_provider = provider;

        if (expectedState == state)
        {
            revState = state;
            providerChanged.notify_all();
        }
    }

protected:

    void SetUp()
    {
        TestWithMock::SetUp();

        if (g_isStartedStack == false)
        {
            static OCPersistentStorage gps {client_open, fread, fwrite, fclose, unlink };
            OC::PlatformConfig occfg
            {
                OC::ServiceType::InProc,
                OC::ModeType::Both,
                "0.0.0.0",
                0,
                OC::QualityOfService::LowQos,
                &gps
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
            cfg.messageCb = NSNotificationReceivedCallback;
            cfg.syncInfoCb = NSSyncCallback;
        }

    }

    void TearDown()
    {
        TestWithMock::TearDown();
    }

};

TEST_F(NotificationConsumerTest, StartConsumerNegativeNonSetChangedCB)
{
    cfg.changedCb = NULL;
    cfg.messageCb = NSNotificationReceivedCallback;
    cfg.syncInfoCb = NSSyncCallback;

    EXPECT_EQ(NS_ERROR, NSStartConsumer(cfg));
}

TEST_F(NotificationConsumerTest, StartConsumerNegativeNonSetNotiReceiveCB)
{
    cfg.changedCb = NSProviderChangedCallback;
    cfg.messageCb = NULL;
    cfg.syncInfoCb = NSSyncCallback;

    EXPECT_EQ(NS_ERROR, NSStartConsumer(cfg));
}

TEST_F(NotificationConsumerTest, StartConsumerNegativeNonSetSyncCB)
{
    cfg.changedCb = NSProviderChangedCallback;
    cfg.messageCb = NSNotificationReceivedCallback;
    cfg.syncInfoCb = NULL;

    EXPECT_EQ(NS_ERROR, NSStartConsumer(cfg));
}

TEST_F(NotificationConsumerTest, StartConsumerPositive)
{
    cfg.changedCb = NSProviderChangedCallback;
    cfg.messageCb = NSNotificationReceivedCallback;
    cfg.syncInfoCb = NSSyncCallback;

    EXPECT_EQ(NS_OK, NSStartConsumer(cfg));
}

TEST_F(NotificationConsumerTest, StopConsumerPositive)
{
    EXPECT_EQ(NSStopConsumer(), NS_OK);
}

TEST_F(NotificationConsumerTest, StopConsumerNegative)
{
    EXPECT_EQ(NSStopConsumer(), NS_ERROR);
}

TEST_F(NotificationConsumerTest, DiscoverProviderWithNonAccepterWhenStartedConsumerFirst)
{
    revState = NS_STOPPED;

    cfg.changedCb = NSProviderChangedCallback;
    cfg.messageCb = NSNotificationReceivedCallback;
    cfg.syncInfoCb = NSSyncCallback;

    expectedState = NS_DISCOVERED;
    NSStartConsumer(cfg);

    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_CONSUMER);
    g_providerSimul.createNotificationResource();

    std::unique_lock< std::mutex > lock{ providerChangedLock };
    providerChanged.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(NS_DISCOVERED, revState);

    NSStopConsumer();
    g_providerSimul.deleteNotificationResource();
}

TEST_F(NotificationConsumerTest, DiscoverProviderWithNonAccepterWhenStartedConsumerAfter)
{
    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_CONSUMER);
    g_providerSimul.createNotificationResource();

    revState = NS_STOPPED;

    NSStartConsumer(cfg);

    std::unique_lock< std::mutex > lock{ providerChangedLock };
    providerChanged.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(NS_DISCOVERED, revState);

    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";
}

TEST_F(NotificationConsumerTest, DiscoverProviderWithNonAccepterWhenRescan)
{
    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_CONSUMER);
    revState = NS_STOPPED;

    NSRescanProvider();

    std::unique_lock< std::mutex > lock{ providerChangedLock };
    providerChanged.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(NS_DISCOVERED, revState);
}

TEST_F(NotificationConsumerTest, ExpectSubscribeSuccess)
{
    revState = NS_DENY;

    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";

    NSResult ret = NSSubscribe(g_provider->providerId);

    expectedState = NS_ALLOW;
    std::unique_lock< std::mutex > lock{ providerChangedLock };
    providerChanged.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(NS_ALLOW, revState);
    EXPECT_EQ(NS_OK, ret);
}

TEST_F(NotificationConsumerTest, ExpectReceiveNotification)
{
    uint64_t id = 10;
    std::string title = "title";
    std::string msg = "msg";
    revId = 0;

    g_providerSimul.notifyMessage(id, title, msg);

    std::unique_lock< std::mutex > lock{ messageReceiveLock };
    messageReceive.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(id, revId);
    NSStopConsumer();
    g_providerSimul.deleteNotificationResource();
}

TEST_F(NotificationConsumerTest, ExpectReceiveSubAllowWithAccepterisProvider)
{
    g_providerSimul.setAccepter((int)NSSelector::NS_SELECTION_PROVIDER);
    revState = NS_DENY;
    g_providerSimul.createNotificationResource();

    expectedState = NS_DISCOVERED;
    NSStartConsumer(cfg);
    {
        std::unique_lock< std::mutex > lock{ providerChangedLock };
        providerChanged.wait_for(lock, g_waitForResponse);
    }

    expectedState = NS_ALLOW;
    {
        std::unique_lock< std::mutex > lock{ providerChangedLock };
        providerChanged.wait_for(lock, g_waitForResponse);
    }

    EXPECT_EQ(NS_ALLOW, revState);
    expectedState = NS_STOPPED;
}

TEST_F(NotificationConsumerTest, ExpectReceiveNotificationWithAccepterisProvider)
{
    uint64_t id = 11;
    std::string title = "title";
    std::string msg = "msg";
    revId = 1;

    g_providerSimul.notifyMessage(id, title, msg);

    std::unique_lock< std::mutex > lock{ messageReceiveLock };
    messageReceive.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(id, revId);
}

TEST_F(NotificationConsumerTest, ExpectCallbackReadCheckWhenProviderNotifySync)
{
    uint64_t id = 12;
    std::string title = "title";
    std::string msg = "msg";
    type = NS_SYNC_DELETED;

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock{ messageReceiveLock };
        messageReceive.wait_for(lock, g_waitForResponse);
    }

    g_providerSimul.sendRead(id);
    {
        std::unique_lock< std::mutex > lock{ syncReceiveLock };
        syncReceive.wait_for(lock, g_waitForResponse);
    }

    EXPECT_EQ(NS_SYNC_READ, type);
}

TEST_F(NotificationConsumerTest, ExpectCallbackDismissCheckWhenProviderNotifySync)
{
    uint64_t id = 13;
    std::string title = "title";
    std::string msg = "msg";
    type = NS_SYNC_READ;

    g_providerSimul.notifyMessage(id, title, msg);
    {
        std::unique_lock< std::mutex > lock{ messageReceiveLock };
        messageReceive.wait_for(lock, g_waitForResponse);
    }

    g_providerSimul.sendDismiss(id);
    {
        std::unique_lock< std::mutex > lock{ syncReceiveLock };
        syncReceive.wait_for(lock, g_waitForResponse);
    }

    EXPECT_EQ(NS_SYNC_DELETED, type);
}

TEST_F(NotificationConsumerTest, ExpectCallbackReadCheckWhenConsumerPostSync)
{
    uint64_t id = 14;
    std::string title = "title";
    std::string msg = "msg";
    type = NS_SYNC_DELETED;

    g_providerSimul.notifyMessage(id, title, msg);
    std::unique_lock< std::mutex > lock{ messageReceiveLock };
    messageReceive.wait_for(lock, g_waitForResponse);
    {
        NSConsumerSendSyncInfo(testProviderID.c_str(), revId, NS_SYNC_READ);
        std::unique_lock< std::mutex > lock{ syncReceiveLock };
        syncReceive.wait_for(lock, g_waitForResponse);
    }

    EXPECT_EQ(NS_SYNC_READ, type);
}

TEST_F(NotificationConsumerTest, ExpectCallbackDismissCheckWhenConsumerPostSync)
{
    uint64_t id = 15;
    std::string title = "title";
    std::string msg = "msg";
    type = NS_SYNC_READ;

    g_providerSimul.notifyMessage(id, title, msg);
    std::unique_lock< std::mutex > lock{ messageReceiveLock };
    messageReceive.wait_for(lock, g_waitForResponse);
    {
        NSConsumerSendSyncInfo(testProviderID.c_str(), revId, NS_SYNC_DELETED);
        std::unique_lock< std::mutex > lock{ syncReceiveLock };
        syncReceive.wait_for(lock, g_waitForResponse);
    }

    EXPECT_EQ(NS_SYNC_DELETED, type);
}

TEST_F(NotificationConsumerTest, ExpectGetProviderSuccessWithValidProviderId)
{
    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";

    NSProvider * provider = NSConsumerGetProvider(g_provider->providerId);
    int ret = strcmp(provider->providerId, g_provider->providerId);
    EXPECT_EQ(0, ret);
    free(provider);
}

TEST_F(NotificationConsumerTest, ExpectGetProviderSuccessWithInvalidProviderId)
{
    NSProvider * provider = NSConsumerGetProvider("123456789012345678901234567890123457");
    EXPECT_EQ(provider, (void*)NULL);
}

TEST_F(NotificationConsumerTest, ExpectGetProviderSuccessWithNULL)
{
    NSProvider * provider = NSConsumerGetProvider(NULL);
    EXPECT_EQ(provider, (void*)NULL);
}

TEST_F(NotificationConsumerTest, ExpectGetTopicListIsNULL)
{
    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";

    NSTopicLL * currentTopics = NSConsumerGetTopicList(g_provider->providerId);
    EXPECT_EQ(NULL, currentTopics);

    if (currentTopics)
    {
        removeTopics(currentTopics);
    }
}

TEST_F(NotificationConsumerTest, ExpectCallbackTopicUpdated)
{
    revState = NS_STOPPED;

    NSProviderSimulator::NS_TopicList topics;
    topics.push_back("1");
    topics.push_back("2");
    topics.push_back("3");

    g_providerSimul.setTopics(topics);

    expectedState = NS_TOPIC;
    std::unique_lock< std::mutex > lock{ providerChangedLock };
    providerChanged.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(NS_TOPIC, revState);

    expectedState = NS_STOPPED;
}

TEST_F(NotificationConsumerTest, ExpectEQTopicList)
{
    bool isSame = false;

    NSProviderSimulator::NS_TopicList topics;
    topics.push_back("1");
    topics.push_back("2");
    topics.push_back("3");

    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";

    NSTopicLL * retTopic = NSConsumerGetTopicList(g_provider->providerId);

    NSTopicLL * iter = retTopic;
    std::for_each (topics.begin(), topics.end(),
            [this, & iter, & isSame](const std::string & str)
            {
                isSame = (str == std::string(iter->topicName));
                iter = iter->next;
            });

    removeTopics(retTopic);

    EXPECT_EQ(true, isSame);
}

TEST_F(NotificationConsumerTest, ExpectFailUpdateTopicOnConsumer)
{
    ASSERT_NE(nullptr, g_provider) << "error: discovery failure";

    NSTopicLL * retTopic = NSConsumerGetTopicList(g_provider->providerId);

    NSTopicLL * iter = retTopic;
    for (; iter; iter = iter->next)
    {
        iter->state = NS_TOPIC_SUBSCRIBED;
    }
    NSResult ret = NSConsumerUpdateTopicList(g_provider->providerId, retTopic);

    removeTopics(retTopic);

    EXPECT_EQ(NS_ERROR, ret);
}

TEST_F(NotificationConsumerTest, ExpectCallbackDeletedProvider)
{
    revState = NS_ALLOW;

    expectedState = NS_STOPPED;
    g_providerSimul.deleteNotificationResource();

    std::unique_lock< std::mutex > lock{ providerChangedLock };
    providerChanged.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(revState, NS_STOPPED);
    NSStopConsumer();

    if (g_provider)
    {
        free(g_provider);
    }
}
