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
 * @file NSProviderServiceTest.cpp
 *
 * This file contains Notification provider service test cases that involves network related methods.
 * The test cases are excluded from the build. 
 * They can be executed seperately (local running) from the path <iotivity>/out/linux/<target_os>/<target_arch>/service/notification/cpp-wrapper/unittest
 */

#include <gtest/gtest.h>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <chrono>

#include "NSProviderService.h"
#include "NSConsumerServiceSimulator.h"
#include "NSUtils.h"
#include "NSSyncInfo.h"
#include "NSMessage.h"
#include "NSMediaContents.h"

namespace
{
    std::atomic_bool g_isStartedStack(false);

    std::chrono::milliseconds g_waitForResponse(1000);

    std::condition_variable responseProviderSub;
    std::mutex responseProviderSubLock;

    std::condition_variable responseProviderSync;
    std::mutex responseProviderSyncLock;

    std::condition_variable responseConsumerMessage;
    std::mutex responseConsumerMessageLock;

    std::condition_variable responseConsumerSync;
    std::mutex responseConsumerSyncLock;

    NSConsumerSimulator g_consumerSimul;
    std::shared_ptr<OIC::Service::NSConsumer> g_consumer;

    int expectedMsgId;
    int expectedSyncType = NS_SYNC_READ;
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

class NotificationProviderServiceTest : public TestWithMock
{
    public:
        NotificationProviderServiceTest() = default;
        ~NotificationProviderServiceTest() = default;

        static void ConsumerSubscribedCallback(std::shared_ptr<OIC::Service::NSConsumer> consumer)
        {
            std::cout << __func__ << std::endl;
            g_consumer = consumer;

            responseProviderSub.notify_all();
        }

        static void MessageSynchronizedCallback(OIC::Service::NSSyncInfo sync)
        {
            std::cout << __func__ << std::endl;
            expectedSyncType = (int) sync.getState();
            expectedMsgId = sync.getMessageId();
            responseProviderSync.notify_all();
        }

        static void MessageCallbackFromConsumer(
            const int &id, const std::string &, const std::string &, const std::string &)
        {
            std::cout << __func__ << std::endl;
            expectedMsgId = id;
            std::cout << expectedMsgId << std::endl;
            responseConsumerMessage.notify_all();
        }

        static void SyncCallbackFromConsumer(const int type, const int syncId)
        {
            std::cout << __func__ << std::endl;
            expectedSyncType = type;
            expectedMsgId = syncId;
            responseConsumerSync.notify_all();
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
                    OC::QualityOfService::HighQos
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

TEST_F(NotificationProviderServiceTest, StartProviderPositiveWithPolicyTrue)
{
    OIC::Service::NSProviderService::ProviderConfig config;
    config.m_subscribeRequestCb = ConsumerSubscribedCallback;
    config.m_syncInfoCb = MessageSynchronizedCallback;
    config.subControllability = true;
    config.userInfo = "user1";
    config.resourceSecurity = false;

    OIC::Service::NSResult ret =  OIC::Service::NSProviderService::getInstance()->start(config);

    EXPECT_EQ(ret,  OIC::Service::NSResult::OK);
}

TEST_F(NotificationProviderServiceTest, StopProviderPositive)
{
    OIC::Service::NSResult ret =  OIC::Service::NSProviderService::getInstance()->stop();

    EXPECT_EQ(ret,  OIC::Service::NSResult::OK);
}

TEST_F(NotificationProviderServiceTest, StartProviderPositiveWithPolicyFalse)
{
    OIC::Service::NSProviderService::ProviderConfig  config;
    config.m_subscribeRequestCb = ConsumerSubscribedCallback;
    config.m_syncInfoCb = MessageSynchronizedCallback;
    config.subControllability = false;
    config.resourceSecurity = false;

    OIC::Service::NSResult ret =  OIC::Service::NSProviderService::getInstance()->start(config);

    EXPECT_EQ(ret,  OIC::Service::NSResult::OK);
    OIC::Service::NSProviderService::getInstance()->stop();
}

TEST_F(NotificationProviderServiceTest,
       ExpectCallbackWhenReceiveSubscribeRequestWithAccepterProvider)
{
    g_consumer = NULL;

    OIC::Service::NSProviderService::ProviderConfig  config;
    config.m_subscribeRequestCb = ConsumerSubscribedCallback;
    config.m_syncInfoCb = MessageSynchronizedCallback;
    config.subControllability = true;
    config.resourceSecurity = false;

    OIC::Service::NSProviderService::getInstance()->start(config);

    g_consumerSimul.setCallback(MessageCallbackFromConsumer,
                                SyncCallbackFromConsumer);
    g_consumerSimul.findProvider();

    // maximum waiting time for subscription is 1.5 sec.
    // usually maximum time is 1 sec. (g_waitForResponse = 1 sec.)
    // but, in the secured case is need to more wait for processing.
    std::chrono::milliseconds waitForSubscription(1500);
    std::unique_lock< std::mutex > lock{ responseProviderSubLock };
    responseProviderSub.wait_for(lock, waitForSubscription);

    ASSERT_NE(nullptr, g_consumer) << "error: discovery failure";
}

TEST_F(NotificationProviderServiceTest, NeverCallNotifyOnConsumerByAcceptIsFalse)
{
    int msgID = 0;


    ASSERT_NE(nullptr, g_consumer) << "error: discovery failure";

    g_consumer->acceptSubscription(false);
    OIC::Service::NSMessage msg =  OIC::Service::NSProviderService::getInstance()->createMessage();
    msgID = (int)msg.getMessageId();
    msg.setTitle(std::string("Title"));
    msg.setContentText(std::string("ContentText"));
    msg.setSourceName(std::string("OCF"));

    OIC::Service::NSProviderService::getInstance()->sendMessage(msg);
    std::unique_lock< std::mutex > lock{ responseConsumerMessageLock };
    responseConsumerMessage.wait_for(lock, g_waitForResponse);

    EXPECT_NE(expectedMsgId, msgID);
    responseConsumerMessage.wait_for(lock, g_waitForResponse);

}

TEST_F(NotificationProviderServiceTest, ExpectCallNotifyOnConsumerByAcceptIsTrue)
{
    int msgID = 0;

    ASSERT_NE(nullptr, g_consumer) << "error: discovery failure";

    g_consumer->acceptSubscription(true);

    OIC::Service::NSMessage msg =  OIC::Service::NSProviderService::getInstance()->createMessage();
    msgID = (int)msg.getMessageId();
    msg.setTitle(std::string("Title"));
    msg.setContentText(std::string("ContentText"));
    msg.setSourceName(std::string("OCF"));

    OIC::Service::NSProviderService::getInstance()->sendMessage(msg);
    std::unique_lock< std::mutex > lock{ responseConsumerMessageLock };
    responseConsumerMessage.wait_for(lock, g_waitForResponse);
    EXPECT_EQ(expectedMsgId, msgID);

}

TEST_F(NotificationProviderServiceTest, ExpectCallbackSyncOnReadToConsumer)
{
    int id = 0;
    int type = (int)OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;
    expectedSyncType = (int)OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;


    OIC::Service::NSMessage msg =  OIC::Service::NSProviderService::getInstance()->createMessage();
    id = (int)msg.getMessageId();
    msg.setTitle(std::string("Title"));
    msg.setContentText(std::string("ContentText"));
    msg.setSourceName(std::string("OCF"));

    OIC::Service::NSProviderService::getInstance()->sendSyncInfo(msg.getMessageId(),
            OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ);
    std::unique_lock< std::mutex > lock{ responseConsumerSyncLock };
    responseConsumerSync.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(expectedMsgId, id);
    EXPECT_EQ(expectedSyncType, type);
}

TEST_F(NotificationProviderServiceTest, ExpectCallbackSyncOnReadFromConsumer)
{
    int type = (int)OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;
    expectedSyncType = (int)OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED;

    int id = 0;

    OIC::Service::NSMessage msg =  OIC::Service::NSProviderService::getInstance()->createMessage();
    id = (int)msg.getMessageId();
    msg.setTitle(std::string("Title"));
    msg.setContentText(std::string("ContentText"));
    msg.setSourceName(std::string("OCF"));
    g_consumerSimul.syncToProvider(type, id, msg.getProviderId());
    std::unique_lock< std::mutex > lock{ responseProviderSyncLock };
    responseProviderSync.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(expectedMsgId, id);
    EXPECT_EQ(expectedSyncType, type);
}

TEST_F(NotificationProviderServiceTest, ExpectEqualAddedTopicsAndRegisteredTopics)
{
    std::string str1("TEST1");
    std::string str2("TEST2");
    OIC::Service::NSProviderService::getInstance()->registerTopic(str1);
    OIC::Service::NSProviderService::getInstance()->registerTopic(str2);

    bool isSame = false;
    auto topicList =  OIC::Service::NSProviderService::getInstance()->getRegisteredTopicList();

    std::string compString[10];
    int i = 0;
    for (auto itr : topicList->getTopicsList())
    {
        compString[i] = itr.getTopicName(); i++;
    }
    std::cout << compString[0] << std::endl;
    std::cout << compString[1] << std::endl;
    if (str1.compare(compString[0]) == 0 && str2.compare(compString[1]) == 0)
    {
        isSame = true;
    }

    EXPECT_EQ(isSame, true);

    OIC::Service::NSProviderService::getInstance()->unregisterTopic(str1);
    OIC::Service::NSProviderService::getInstance()->unregisterTopic(str2);

}

TEST_F(NotificationProviderServiceTest, ExpectEqualUnregisteredTopicsAndRegisteredTopics)
{
    std::string str1("TEST1");
    std::string str2("TEST2");
    OIC::Service::NSProviderService::getInstance()->registerTopic(str1);
    OIC::Service::NSProviderService::getInstance()->registerTopic(str2);
    OIC::Service::NSProviderService::getInstance()->unregisterTopic(str2);
    bool isSame = false;
    auto topicList =
        OIC::Service::NSProviderService::getInstance()->getRegisteredTopicList();

    std::string compStr[10];
    int i = 0;
    for (auto itr : topicList->getTopicsList())
    {
        compStr[i] = itr.getTopicName(); i++;
    }
    std::cout << compStr << std::endl;
    if (str1.compare(compStr[0]) == 0)
    {
        isSame = true;
    }

    EXPECT_EQ(isSame, true);

    OIC::Service::NSProviderService::getInstance()->unregisterTopic(str1);
}

TEST_F(NotificationProviderServiceTest, ExpectEqualSetConsumerTopicsAndGetConsumerTopics)
{
    std::string str1("TEST1");
    std::string str2("TEST2");
    OIC::Service::NSProviderService::getInstance()->registerTopic(str1);
    OIC::Service::NSProviderService::getInstance()->registerTopic(str2);

    ASSERT_NE(nullptr, g_consumer) << "error: discovery failure";

    g_consumer->setTopic(str1);


    bool isSame = false;
    auto topicList =  g_consumer->getConsumerTopicList();

    std::string compString[10];
    int i = 0, state[10] = {0};
    for (auto itr : topicList->getTopicsList())
    {
        compString[i] = itr.getTopicName();
        state[i++] = (int) itr.getState();
    }
    std::cout << compString[0] << std::endl;
    std::cout << compString[1] << std::endl;
    if (str1.compare(compString[0]) == 0 && str2.compare(compString[1]) == 0
        && state[0] == 1 &&  state[1] == 0)
    {
        isSame = true;
    }


    EXPECT_EQ(isSame, true);

    OIC::Service::NSProviderService::getInstance()->unregisterTopic(str1);
    OIC::Service::NSProviderService::getInstance()->unregisterTopic(str2);

}

TEST_F(NotificationProviderServiceTest, ExpectEqualUnSetConsumerTopicsAndGetConsumerTopics)
{
    std::string str1("TEST1");
    std::string str2("TEST2");
    OIC::Service::NSProviderService::getInstance()->registerTopic(str1);
    OIC::Service::NSProviderService::getInstance()->registerTopic(str2);

    ASSERT_NE(nullptr, g_consumer) << "error: discovery failure";

    g_consumer->setTopic(str1);
    g_consumer->setTopic(str2);
    g_consumer->unsetTopic(str1);

    bool isSame = false;
    auto topicList =  g_consumer->getConsumerTopicList();

    std::string compString[10];
    int i = 0, state[10] = {0};
    for (auto itr : topicList->getTopicsList())
    {
        compString[i] = itr.getTopicName();
        state[i++] = (int) itr.getState();
    }
    std::cout << compString[0] << std::endl;
    std::cout << compString[1] << std::endl;
    if (str1.compare(compString[0]) == 0 && str2.compare(compString[1]) == 0
        && state[0] == 0 &&  state[1] == 1)
    {
        isSame = true;
    }

    EXPECT_EQ(isSame, true);

    OIC::Service::NSProviderService::getInstance()->unregisterTopic(str1);
    OIC::Service::NSProviderService::getInstance()->unregisterTopic(str2);
}


TEST_F(NotificationProviderServiceTest, CancelObserves)
{
    bool ret = g_consumerSimul.cancelObserves();

    std::this_thread::sleep_for(g_waitForResponse);

    EXPECT_EQ(ret, true);
}
