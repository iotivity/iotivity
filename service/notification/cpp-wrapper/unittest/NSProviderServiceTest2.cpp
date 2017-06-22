//******************************************************************
//
// Copyright 2017 Samsung Electronics All Rights Reserved.
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
 * @file NSProviderServiceTest2.cpp
 *
 * This file contains Notification provider service test cases that do not involve network related methods.
 * The test cases are executed during the build by enabling the option 'TEST' in scons build.
 * Eg., scons [options] TEST=1
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
#include "NSAcceptedConsumers.h"

namespace
{
    std::atomic_bool g_isStartedStack(false);
    std::chrono::milliseconds g_waitForResponse(1000);
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

        static void ConsumerSubscribedCallback(std::shared_ptr<OIC::Service::NSConsumer> )
        {
            std::cout << __func__ << std::endl;
        }

        static void MessageSynchronizedCallback(OIC::Service::NSSyncInfo )
        {
            std::cout << __func__ << std::endl;
        }

        static void MessageCallbackFromConsumer(
            const int &, const std::string &, const std::string &, const std::string &)
        {
            std::cout << __func__ << std::endl;
        }

        static void SyncCallbackFromConsumer(const int , const int )
        {
            std::cout << __func__ << std::endl;
        }

    protected:

        void SetUp()
        {

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

TEST_F(NotificationProviderServiceTest, ExpectSuccessCreateMessage)
{
    OIC::Service::NSProviderService::ProviderConfig  config;
    config.m_subscribeRequestCb = ConsumerSubscribedCallback;
    config.m_syncInfoCb = MessageSynchronizedCallback;
    config.subControllability = false;
    config.resourceSecurity = false;

    OIC::Service::NSProviderService::getInstance()->start(config);
    OIC::Service::NSMessage msg =  OIC::Service::NSProviderService::getInstance()->createMessage();
}

TEST_F(NotificationProviderServiceTest, ExpectSuccessSendMessage)
{
    OIC::Service::NSResult res = OIC::Service::NSResult::FAIL;

    OIC::Service::NSMessage msg =  OIC::Service::NSProviderService::getInstance()->createMessage();
    msg.setTitle(std::string("Title"));
    msg.setContentText(std::string("ContentText"));
    msg.setSourceName(std::string("OCF"));

    res = OIC::Service::NSProviderService::getInstance()->sendMessage(msg);
    EXPECT_EQ(OIC::Service::NSResult::OK, res);
}


TEST_F(NotificationProviderServiceTest, ExpectSuccessSendSyncInfo)
{
    uint64_t msgId = 10;
    OIC::Service::NSResult res =
        OIC::Service::NSProviderService::getInstance()->sendSyncInfo(msgId,
                OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ);
    EXPECT_EQ(res,  OIC::Service::NSResult::OK);
}

TEST_F(NotificationProviderServiceTest, ExpectSuccessRegisterTopic)
{
    std::string str1("TEST1");
    std::string str2("TEST2");

    OIC::Service::NSResult res = OIC::Service::NSProviderService::getInstance()->registerTopic(str1);
    EXPECT_EQ(res,  OIC::Service::NSResult::OK);
    res = OIC::Service::NSProviderService::getInstance()->registerTopic(str2);
    EXPECT_EQ(res,  OIC::Service::NSResult::OK);

}

TEST_F(NotificationProviderServiceTest, ExpectSuccessUnRegisterTopic)
{
    std::string str1("TEST1");

    OIC::Service::NSResult res = OIC::Service::NSProviderService::getInstance()->unregisterTopic(str1);
    EXPECT_EQ(res,  OIC::Service::NSResult::OK);
}


TEST_F(NotificationProviderServiceTest, ExpectSuccessGetRegisteredTopicList)
{
    std::string str1("TEST1");

    OIC::Service::NSProviderService::getInstance()->registerTopic(str1);

    auto topicList =  OIC::Service::NSProviderService::getInstance()->getRegisteredTopicList();
    ASSERT_NE(nullptr, topicList) << "error: get TopicList failure";
    OIC::Service::NSProviderService::getInstance()->stop();
}

TEST_F(NotificationProviderServiceTest, ExpectFailAcceptSubscriptionInvalidPolicy)
{
    OIC::Service::NSProviderService::ProviderConfig  config;
    config.m_subscribeRequestCb = ConsumerSubscribedCallback;
    config.m_syncInfoCb = MessageSynchronizedCallback;
    config.subControllability = false;
    config.resourceSecurity = false;

    OIC::Service::NSProviderService::getInstance()->start(config);
    ::NSConsumer *consumer = (::NSConsumer *)malloc(sizeof(::NSConsumer));
    strncpy(consumer->consumerId, "098765432109876543210987654321098765", NS_UTILS_UUID_STRING_SIZE);

    std::shared_ptr<OIC::Service::NSConsumer> consumerTemp = std::make_shared<OIC::Service::NSConsumer>
            (consumer);

    OIC::Service::NSAcceptedConsumers *acceptedConsumers =
        OIC::Service::NSProviderService::getInstance()->getAcceptedConsumers();
    acceptedConsumers->addConsumer(consumerTemp);

    OIC::Service::NSResult res = consumerTemp->acceptSubscription(true);

    EXPECT_NE(res,  OIC::Service::NSResult::OK);
    free(consumer);
}

TEST_F(NotificationProviderServiceTest, ExpectFailSetTopicInvalidPolicy)
{
    std::string str1("TEST1");
    OIC::Service::NSProviderService::getInstance()->registerTopic(str1);
    ::NSConsumer *consumer = (::NSConsumer *)malloc(sizeof(::NSConsumer));
    strncpy(consumer->consumerId, "098765432109876543210987654321098765", NS_UTILS_UUID_STRING_SIZE);

    std::shared_ptr<OIC::Service::NSConsumer> consumerTemp = std::make_shared<OIC::Service::NSConsumer>
            (consumer);

    OIC::Service::NSAcceptedConsumers *acceptedConsumers =
        OIC::Service::NSProviderService::getInstance()->getAcceptedConsumers();
    acceptedConsumers->addConsumer(consumerTemp);

    OIC::Service::NSResult res = consumerTemp->setTopic(str1);

    EXPECT_NE(res,  OIC::Service::NSResult::OK);
    free(consumer);
    OIC::Service::NSProviderService::getInstance()->stop();
}

TEST_F(NotificationProviderServiceTest, ExpectSuccessAcceptSubscription)
{
    OIC::Service::NSProviderService::ProviderConfig  config;
    config.m_subscribeRequestCb = ConsumerSubscribedCallback;
    config.m_syncInfoCb = MessageSynchronizedCallback;
    config.subControllability = true;
    config.resourceSecurity = false;

    OIC::Service::NSProviderService::getInstance()->start(config);
    ::NSConsumer *consumer = (::NSConsumer *)malloc(sizeof(::NSConsumer));
    strncpy(consumer->consumerId, "098765432109876543210987654321098765", NS_UTILS_UUID_STRING_SIZE);

    std::shared_ptr<OIC::Service::NSConsumer> consumerTemp = std::make_shared<OIC::Service::NSConsumer>
            (consumer);

    OIC::Service::NSAcceptedConsumers *acceptedConsumers =
        OIC::Service::NSProviderService::getInstance()->getAcceptedConsumers();
    acceptedConsumers->addConsumer(consumerTemp);

    OIC::Service::NSResult res = consumerTemp->acceptSubscription(true);

    EXPECT_EQ(res,  OIC::Service::NSResult::OK);
    free(consumer);
}

TEST_F(NotificationProviderServiceTest, ExpectSuccessSetTopic)
{
    std::string str1("TEST1");

    OIC::Service::NSProviderService::getInstance()->registerTopic(str1);
    ::NSConsumer *consumer = (::NSConsumer *)malloc(sizeof(::NSConsumer));
    strncpy(consumer->consumerId, "098765432109876543210987654321098765", NS_UTILS_UUID_STRING_SIZE);

    std::shared_ptr<OIC::Service::NSConsumer> consumerTemp = std::make_shared<OIC::Service::NSConsumer>
            (consumer);

    OIC::Service::NSAcceptedConsumers *acceptedConsumers =
        OIC::Service::NSProviderService::getInstance()->getAcceptedConsumers();
    acceptedConsumers->addConsumer(consumerTemp);

    OIC::Service::NSResult res = consumerTemp->setTopic("TEST1");

    EXPECT_EQ(res,  OIC::Service::NSResult::OK);
    free(consumer);
}

TEST_F(NotificationProviderServiceTest, ExpectSuccessUnSetTopic)
{
    ::NSConsumer *consumer = (::NSConsumer *)malloc(sizeof(::NSConsumer));
    strncpy(consumer->consumerId, "098765432109876543210987654321098765", NS_UTILS_UUID_STRING_SIZE);

    std::shared_ptr<OIC::Service::NSConsumer> consumerTemp = std::make_shared<OIC::Service::NSConsumer>
            (consumer);

    OIC::Service::NSAcceptedConsumers *acceptedConsumers =
        OIC::Service::NSProviderService::getInstance()->getAcceptedConsumers();
    acceptedConsumers->addConsumer(consumerTemp);

    consumerTemp->setTopic("TEST1");
    OIC::Service::NSResult res = consumerTemp->unsetTopic("TEST1");
    EXPECT_EQ(res,  OIC::Service::NSResult::OK);
    free(consumer);
    OIC::Service::NSProviderService::getInstance()->stop();
}

TEST_F(NotificationProviderServiceTest, ExpectEqualAddedTopicsAndRegisteredTopics)
{
    OIC::Service::NSProviderService::ProviderConfig  config;
    config.m_subscribeRequestCb = ConsumerSubscribedCallback;
    config.m_syncInfoCb = MessageSynchronizedCallback;
    config.subControllability = true;
    config.resourceSecurity = false;

    OIC::Service::NSProviderService::getInstance()->start(config);
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
}

TEST_F(NotificationProviderServiceTest, ExpectEqualSetConsumerTopicsAndGetConsumerTopics)
{
    std::string str1("TEST1");
    std::string str2("TEST2");
    ::NSConsumer *consumer = (::NSConsumer *)malloc(sizeof(::NSConsumer));
    strncpy(consumer->consumerId, "098765432109876543210987654321098765", NS_UTILS_UUID_STRING_SIZE);

    std::shared_ptr<OIC::Service::NSConsumer> consumerTemp = std::make_shared<OIC::Service::NSConsumer>
            (consumer);

    OIC::Service::NSAcceptedConsumers *acceptedConsumers =
        OIC::Service::NSProviderService::getInstance()->getAcceptedConsumers();
    acceptedConsumers->addConsumer(consumerTemp);

    consumerTemp->setTopic(str1);

    bool isSame = false;
    auto topicList =  consumerTemp->getConsumerTopicList();

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
    free(consumer);
    usleep(2000);
    OIC::Service::NSProviderService::getInstance()->stop();
}

