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

#include "NSProviderInterface.h"
#include "NSConsumerSimulator.h"
#include "NSCommon.h"

namespace
{
    std::atomic_bool g_isStartedStack(false);

    std::chrono::milliseconds g_waitForResponse(500);

    std::condition_variable responseCon;
    std::mutex mutexForCondition;

    NSConsumerSimulator g_consumerSimul;
    char * g_consumerID;
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

class NotificationProviderTest : public TestWithMock
{
public:
    NotificationProviderTest() = default;
    ~NotificationProviderTest() = default;

    static void NSRequestedSubscribeCallbackEmpty(NSConsumer *)
    {
        std::cout << __func__ << std::endl;
    }

    static void NSSyncCallbackEmpty(NSSyncInfo *)
    {
        std::cout << __func__ << std::endl;
    }

    static void NSMessageCallbackFromConsumerEmpty(
            const int &, const std::string &, const std::string &, const std::string &)
    {
        std::cout << __func__ << std::endl;
    }

    static void NSSyncCallbackFromConsumerEmpty(int, int)
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

TEST_F(NotificationProviderTest, StartProviderPositiveWithNSPolicyTrue)
{
    NSProviderConfig config;
    config.subRequestCallback = NSRequestedSubscribeCallbackEmpty;
    config.syncInfoCallback = NSSyncCallbackEmpty;
    config.policy = true;
    config.userInfo = NULL;

    NSResult ret = NSStartProvider(config);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(ret, NS_OK);
}

TEST_F(NotificationProviderTest, StopProviderPositive)
{
    NSResult ret = NSStopProvider();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    EXPECT_EQ(ret, NS_OK);
}

TEST_F(NotificationProviderTest, StartProviderPositiveWithNSPolicyFalse)
{
    NSProviderConfig config;
    config.subRequestCallback = NSRequestedSubscribeCallbackEmpty;
    config.syncInfoCallback = NSSyncCallbackEmpty;
    config.policy = false;
    config.userInfo = NULL;

    NSResult ret = NSStartProvider(config);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(3000));
    g_consumerSimul.findProvider();

    responseCon.wait_for(lock, std::chrono::milliseconds(3000));
    NSStopProvider();
    EXPECT_EQ(ret, NS_OK);
}

TEST_F(NotificationProviderTest, ExpectCallbackWhenReceiveSubscribeRequestWithAccepterProvider)
{
    g_consumerID = NULL;
    mocks.OnCallFunc(NSRequestedSubscribeCallbackEmpty).Do(
            [](NSConsumer * consumer)
            {
                std::cout << "NSRequestedSubscribeCallback" << std::endl;
                g_consumerID = strdup(consumer->consumerId);
                responseCon.notify_all();
            });

    NSProviderConfig config;
    config.subRequestCallback = NSRequestedSubscribeCallbackEmpty;
    config.syncInfoCallback = NSSyncCallbackEmpty;
    config.policy = true;
    config.userInfo = NULL;

    NSStartProvider(config);

    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    g_consumerSimul.setCallback(NSMessageCallbackFromConsumerEmpty,
            NSSyncCallbackFromConsumerEmpty);
    g_consumerSimul.findProvider();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(1000));

    EXPECT_NE((void*)g_consumerID, (void*)NULL);
}

TEST_F(NotificationProviderTest, NeverCallNotifyOnConsumerByAcceptIsFalse)
{
    bool expectTrue = true;
    int msgID;

    mocks.OnCallFunc(NSMessageCallbackFromConsumerEmpty).Do(
            [& expectTrue, &msgID](const int &id, const std::string&, const std::string&, const std::string&)
            {
                if (id == msgID)
                {
                    std::cout << "This function never call" << std::endl;
                    expectTrue = false;
                }
            });

    NSAcceptSubscription(g_consumerID, false);

    NSMessage * msg = NSCreateMessage();
    msgID = (int)msg->messageId;
    msg->title = strdup(std::string("Title").c_str());
    msg->contentText = strdup(std::string("ContentText").c_str());
    msg->sourceName = strdup(std::string("OCF").c_str());

    NSSendMessage(msg);
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, g_waitForResponse);
    }

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(1000));

    EXPECT_EQ(expectTrue, true);

    NSAcceptSubscription(g_consumerID, true);
}

TEST_F(NotificationProviderTest, ExpectCallNotifyOnConsumerByAcceptIsTrue)
{
    int msgID;

    mocks.ExpectCallFunc(NSMessageCallbackFromConsumerEmpty).Do(
            [&msgID](const int &id, const std::string&, const std::string&, const std::string&)
            {
                std::cout << "id : " << id << std::endl;
                if (id == msgID)
                {
                    std::cout << "ExpectCallNotifyOnConsumerByAcceptIsTrue" << std::endl;
                    responseCon.notify_all();
                }
            });

    NSAcceptSubscription(g_consumerID, true);

    NSMessage * msg = NSCreateMessage();
    msgID = (int)msg->messageId;
    msg->title = strdup(std::string("Title").c_str());
    msg->contentText = strdup(std::string("ContentText").c_str());
    msg->sourceName = strdup(std::string("OCF").c_str());
    NSSendMessage(msg);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait(lock);
}

TEST_F(NotificationProviderTest, ExpectCallbackSyncOnReadToConsumer)
{
    int id;

    mocks.ExpectCallFunc(NSSyncCallbackFromConsumerEmpty).Do(
            [& id](int & type, int &syncId)
            {
        std::cout << "NSSyncCallbackEmpty" << std::endl;
                if (syncId == id &&
                        type == NS_SYNC_READ)
                {
                    std::cout << "ExpectCallbackSyncOnReadFromConsumer" << std::endl;
                    responseCon.notify_all();
                }
            });

    NSMessage * msg = NSCreateMessage();
    id = (int)msg->messageId;
    msg->title = strdup(std::string("Title").c_str());
    msg->contentText = strdup(std::string("ContentText").c_str());
    msg->sourceName = strdup(std::string("OCF").c_str());

    NSProviderSendSyncInfo(msg->messageId, NS_SYNC_READ);
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(5000));
}

TEST_F(NotificationProviderTest, ExpectCallbackSyncOnReadFromConsumer)
{
    int type = NS_SYNC_READ;
    int id;
    mocks.ExpectCallFunc(NSSyncCallbackEmpty).Do(
            [& id](NSSyncInfo * sync)
            {
                std::cout << "NSSyncCallbackEmpty" << std::endl;
                if ((int)sync->messageId == id && sync->state == NS_SYNC_READ)
                {
                    std::cout << "ExpectCallbackSyncOnReadFromConsumer" << std::endl;
                    responseCon.notify_all();
                }
            });

    NSMessage * msg = NSCreateMessage();
    id = (int)msg->messageId;
    msg->title = strdup(std::string("Title").c_str());
    msg->contentText = strdup(std::string("ContentText").c_str());
    msg->sourceName = strdup(std::string("OCF").c_str());

    g_consumerSimul.syncToProvider(type, id, msg->providerId);
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(5000));
}

TEST_F(NotificationProviderTest, ExpectEqualAddedTopicsAndRegisteredTopics)
{
    std::string str("TEST1");
    std::string str2("TEST2");
    NSProviderRegisterTopic(str.c_str());
    NSProviderRegisterTopic(str2.c_str());

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(1000));

    bool isSame = true;
    NSTopicLL * topics = NSProviderGetTopics();

    if(!topics)
    {
        printf("topic is NULL\n");
        isSame = false;
    }
    else
    {
        NSTopicLL * iter = topics;
        std::string compStr(iter->topicName);
        std::string compStr2(iter->next->topicName);

        printf("str = %s, compStr = %s\n", str.c_str(), iter->topicName);
        printf("str2 = %s, compStr2 = %s\n", str2.c_str(), iter->next->topicName);

        if(str.compare(compStr) == 0 && str2.compare(compStr2) == 0)
        {
            isSame = true;
        }
    }

    EXPECT_EQ(isSame, true);
}

TEST_F(NotificationProviderTest, ExpectEqualUnregisteredTopicsAndRegisteredTopics)
{
    std::string str("TEST1");
    std::string str2("TEST2");
    NSProviderRegisterTopic(str.c_str());
    NSProviderRegisterTopic(str2.c_str());
    NSProviderUnregisterTopic(str2.c_str());

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(1000));

    bool isSame = true;
    NSTopicLL * topics = NSProviderGetTopics();

    if(!topics)
    {
        printf("topic is NULL\n");
        isSame = false;
    }
    else
    {
        NSTopicLL * iter = topics;
        std::string compStr(iter->topicName);

        printf("str = %s, compStr = %s\n", str.c_str(), iter->topicName);

        if(str.compare(compStr) == 0)
        {
            isSame = true;
        }
    }

    EXPECT_EQ(isSame, true);
}

TEST_F(NotificationProviderTest, CancelObserves)
{
    bool ret = g_consumerSimul.cancelObserves();

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(5000));

    EXPECT_EQ(ret, true);
}
