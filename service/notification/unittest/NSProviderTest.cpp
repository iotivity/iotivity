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

#include "NSProviderInterface.h"
#include "NSConsumerSimulator.h"
#include "NSUnittestUtil.h"
#include "NSCommon.h"

#define DEFAULT_G_TIMEOUT 1000

namespace
{
    std::atomic_bool g_isStartedStack(false);

    /// Reasonable timeout is set to 1000 ms in unsecured mode
#ifdef SECURED
    unsigned int g_timeout = 2 * DEFAULT_G_TIMEOUT;
#else
    unsigned int g_timeout = DEFAULT_G_TIMEOUT;
#endif
    std::chrono::milliseconds g_waitForResponse(g_timeout);

    std::condition_variable responseProviderSub;
    std::mutex responseProviderSubLock;

    std::condition_variable responseProviderSync;
    std::mutex responseProviderSyncLock;

    std::condition_variable responseConsumerMessage;
    std::mutex responseConsumerMessageLock;

    std::condition_variable responseConsumerSync;
    std::mutex responseConsumerSyncLock;

    NSConsumerSimulator g_consumerSimul;
    char * g_consumerID = NULL;
    char g_title[100];
    char g_body[100];
    char g_sourceName[100];

    int expectedMsgId;
    int expectedSyncType = NS_SYNC_READ;

    static FILE* server_open(const char * path, const char * mode)
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

class NotificationProviderTest : public TestWithMock
{
public:
    NotificationProviderTest() = default;
    ~NotificationProviderTest() = default;

    static void NSRequestedSubscribeCallback(NSConsumer * consumer)
    {
        if (g_consumerID)
        {
            free(g_consumerID);
        }
        g_consumerID = strdup(consumer->consumerId);

        responseProviderSub.notify_all();
    }

    static void NSSyncCallback(NSSyncInfo * sync)
    {
        expectedSyncType = sync->state;
        expectedMsgId = sync->messageId;
        free(sync);
        responseProviderSync.notify_all();
    }

    static void NSMessageCallbackFromConsumer(
            const int & id, const std::string &, const std::string &, const std::string &)
    {
        expectedMsgId = id;
        responseConsumerMessage.notify_all();
    }

    static void NSSyncCallbackFromConsumer(const int type, const int syncId)
    {
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
            static OCPersistentStorage gps {server_open, fread, fwrite, fclose, unlink };

            OC::PlatformConfig cfg
            {
                OC::ServiceType::InProc,
                OC::ModeType::Both,
                "0.0.0.0",
                0,
                OC::QualityOfService::HighQos,
                &gps
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

            strcpy(g_title, "Title");
            strcpy(g_body, "ContentText");
            strcpy(g_sourceName, "OIC");
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
    config.subRequestCallback = NSRequestedSubscribeCallback;
    config.syncInfoCallback = NSSyncCallback;
    config.subControllability = true;
    config.userInfo = strdup("user1");
    config.resourceSecurity = false;

    NSResult ret = NSStartProvider(config);

    EXPECT_EQ(ret, NS_OK);
    free(config.userInfo);
    config.userInfo = NULL;
}

TEST_F(NotificationProviderTest, StopProviderPositive)
{
    NSResult ret = NSStopProvider();

    EXPECT_EQ(ret, NS_OK);
}

TEST_F(NotificationProviderTest, StartProviderPositiveWithNSPolicyFalse)
{
    NSProviderConfig config;
    config.subRequestCallback = NSRequestedSubscribeCallback;
    config.syncInfoCallback = NSSyncCallback;
    config.subControllability = false;
    config.userInfo = NULL;
    config.resourceSecurity = false;

    NSResult ret = NSStartProvider(config);

    g_consumerSimul.findProvider();

    NSStopProvider();
    EXPECT_EQ(ret, NS_OK);
}

TEST_F(NotificationProviderTest, ExpectCallbackWhenReceiveSubscribeRequestWithAccepterProvider)
{
    NSProviderConfig config;
    config.subRequestCallback = NSRequestedSubscribeCallback;
    config.syncInfoCallback = NSSyncCallback;
    config.subControllability = true;
    config.userInfo = NULL;
    config.resourceSecurity = false;

    NSStartProvider(config);

    g_consumerSimul.setCallback(NSMessageCallbackFromConsumer,
            NSSyncCallbackFromConsumer);
    g_consumerSimul.findProvider();

    // maximum waiting time for subscription is 1.5 sec.
    // usually maximum time is 1 sec. (g_waitForResponse = 1 sec.)
#ifdef SECURED
    unsigned int timeout = g_timeout * 3;
#else
    unsigned int timeout = g_timeout * 1.5;
#endif
    std::chrono::milliseconds waitForSubscription(timeout);
    std::unique_lock< std::mutex > lock{ responseProviderSubLock };
    responseProviderSub.wait_for(lock, waitForSubscription);

    ASSERT_NE(nullptr, g_consumerID) << "error: discovery failure";
}

TEST_F(NotificationProviderTest, NeverCallNotifyOnConsumerByAcceptIsFalse)
{
    int msgID = 0;

    ASSERT_NE(nullptr, g_consumerID) << "error: discovery failure";

    NSAcceptSubscription(g_consumerID, false);

    NSMessage * msg = NSCreateMessage();
    if(msg)
    {
        msgID = (int)msg->messageId;
        msg->title = g_title;
        msg->contentText = g_body;
        msg->sourceName = g_sourceName;
        NSSendMessage(msg);

        std::unique_lock< std::mutex > lock{ responseConsumerMessageLock };
        responseConsumerMessage.wait_for(lock, g_waitForResponse);

        EXPECT_NE(expectedMsgId, msgID);

        NSAcceptSubscription(g_consumerID, true);
        responseConsumerMessage.wait_for(lock, g_waitForResponse);
    }
    else
    {
        EXPECT_EQ(expectedMsgId, msgID);
    }

    free(msg);
}

TEST_F(NotificationProviderTest, ExpectCallNotifyOnConsumerByAcceptIsTrue)
{
    int msgID = 0;

    ASSERT_NE(nullptr, g_consumerID) << "error: discovery failure";

    NSMessage * msg = NSCreateMessage();
    if(msg)
    {
        msgID = (int)msg->messageId;
        msg->title = g_title;
        msg->contentText = g_body;
        msg->sourceName = g_sourceName;
        NSSendMessage(msg);

        std::unique_lock< std::mutex > lock{ responseConsumerMessageLock };
        responseConsumerMessage.wait_for(lock, g_waitForResponse);
        EXPECT_EQ(expectedMsgId, msgID);
    }

    free(msg);
}

TEST_F(NotificationProviderTest, ExpectCallbackSyncOnReadToConsumer)
{
    int id = 0;
    int type = NS_SYNC_READ;

    ASSERT_NE(nullptr, g_consumerID) << "error: discovery failure";

    NSMessage * msg = NSCreateMessage();
    if (msg)
    {
        id = (int)msg->messageId;
        msg->title = g_title;
        msg->contentText = g_body;
        msg->sourceName = g_sourceName;

        NSProviderSendSyncInfo(msg->messageId, NS_SYNC_READ);
        std::unique_lock< std::mutex > lock{ responseConsumerSyncLock };
        responseConsumerSync.wait_for(lock, g_waitForResponse);

        EXPECT_EQ(expectedMsgId, id);
        EXPECT_EQ(expectedSyncType, type);
    }

    free(msg);
}

TEST_F(NotificationProviderTest, ExpectCallbackSyncOnReadFromConsumer)
{
    int id = 0;
    int type = NS_SYNC_READ;

    ASSERT_NE(nullptr, g_consumerID) << "error: discovery failure";

    NSMessage * msg = NSCreateMessage();
    if(msg)
    {
        id = (int)msg->messageId;
        msg->title = g_title;
        msg->contentText = g_body;
        msg->sourceName = g_sourceName;

        g_consumerSimul.syncToProvider(type, id, msg->providerId);
        std::unique_lock< std::mutex > lock{ responseProviderSyncLock };
        responseProviderSync.wait_for(lock, g_waitForResponse);

        EXPECT_EQ(expectedMsgId, id);
        EXPECT_EQ(expectedSyncType, type);
    }

    free(msg);
}

TEST_F(NotificationProviderTest, ExpectEqualAddedTopicsAndRegisteredTopics)
{
    std::string str("TEST1");
    std::string str2("TEST2");
    NSProviderRegisterTopic(str.c_str());
    NSProviderRegisterTopic(str2.c_str());

    bool isSame = true;
    NSTopicLL * topics = NSProviderGetTopics();

    if(!topics)
    {
        isSame = false;
    }
    else
    {
        NSTopicLL * iter = topics;
        std::string compStr(iter->topicName);
        std::string compStr2(iter->next->topicName);

        if(str.compare(compStr) == 0 && str2.compare(compStr2) == 0)
        {
            isSame = true;
        }
    }

    removeTopics(topics);

    NSProviderUnregisterTopic(str.c_str());
    NSProviderUnregisterTopic(str2.c_str());
    EXPECT_EQ(isSame, true);
}

TEST_F(NotificationProviderTest, ExpectEqualUnregisteredTopicsAndRegisteredTopics)
{
    std::string str("TEST1");
    std::string str2("TEST2");
    NSProviderRegisterTopic(str.c_str());
    NSProviderRegisterTopic(str2.c_str());
    NSProviderUnregisterTopic(str2.c_str());

    bool isSame = true;
    NSTopicLL * topics = NSProviderGetTopics();

    if(!topics)
    {
        isSame = false;
    }
    else
    {
        NSTopicLL * iter = topics;
        std::string compStr(iter->topicName);

        if(str.compare(compStr) == 0)
        {
            isSame = true;
        }
    }

    removeTopics(topics);

    NSProviderUnregisterTopic(str.c_str());
    EXPECT_EQ(isSame, true);
}

TEST_F(NotificationProviderTest, ExpectEqualSetConsumerTopicsAndGetConsumerTopics)
{
    std::string str("TEST1");
    std::string str2("TEST2");
    NSProviderRegisterTopic(str.c_str());
    NSProviderRegisterTopic(str2.c_str());
    NSProviderSetConsumerTopic(g_consumerID, str.c_str());

    bool isSame = false;
    NSTopicLL * topics = NSProviderGetConsumerTopics(g_consumerID);

    if(!topics)
    {
        isSame = false;
    }
    else
    {
        NSTopicLL * firstData = topics;
        NSTopicLL * secondData = firstData->next;

        if(str.compare(firstData->topicName) == 0 && str2.compare(secondData->topicName) == 0
                && ((int)firstData->state) == 1 && ((int)secondData->state) == 0)
        {
            isSame = true;
        }
    }

    removeTopics(topics);

    NSProviderUnregisterTopic(str.c_str());
    NSProviderUnregisterTopic(str2.c_str());
    EXPECT_EQ(isSame, true);
}

TEST_F(NotificationProviderTest, ExpectEqualUnSetConsumerTopicsAndGetConsumerTopics)
{
    std::string str("TEST1");
    std::string str2("TEST2");
    NSProviderRegisterTopic(str.c_str());
    NSProviderRegisterTopic(str2.c_str());
    NSProviderSetConsumerTopic(g_consumerID, str.c_str());
    NSProviderSetConsumerTopic(g_consumerID, str2.c_str());
    NSProviderUnsetConsumerTopic(g_consumerID, str.c_str());

    bool isSame = false;

    ASSERT_NE(nullptr, g_consumerID) << "error: discovery failure";

    NSTopicLL * topics = NSProviderGetConsumerTopics(g_consumerID);

    if(!topics)
    {
        isSame = false;
    }
    else
    {
        NSTopicLL * firstData = topics;
        NSTopicLL * secondData = firstData->next;

        if(str.compare(firstData->topicName) == 0 && str2.compare(secondData->topicName) == 0
                && ((int)firstData->state) == 0 && ((int)secondData->state) == 1)
        {
            isSame = true;
        }
    }

    removeTopics(topics);

    NSProviderUnregisterTopic(str.c_str());
    NSProviderUnregisterTopic(str2.c_str());
    EXPECT_EQ(isSame, true);
}

TEST_F(NotificationProviderTest, ExpectFailAcceptSubscription)
{
    NSResult result = NS_SUCCESS;
    result = NSAcceptSubscription(NULL, true);
    result = NSAcceptSubscription("\0", true);

    EXPECT_EQ(result, NS_FAIL);
}

TEST_F(NotificationProviderTest, ExpectFailSendMessage)
{
    NSResult result = NS_SUCCESS;
    result = NSSendMessage(NULL);

    EXPECT_EQ(result, NS_FAIL);
}

TEST_F(NotificationProviderTest, ExpectFailRegisterTopic)
{
    NSResult result = NS_SUCCESS;
    result = NSProviderRegisterTopic(NULL);
    result = NSProviderRegisterTopic("\0");

    EXPECT_EQ(result, NS_FAIL);
}

TEST_F(NotificationProviderTest, ExpectFailUnregisterTopic)
{
    NSResult result = NS_SUCCESS;
    result = NSProviderUnregisterTopic(NULL);
    result = NSProviderUnregisterTopic("\0");

    EXPECT_EQ(result, NS_FAIL);
}

TEST_F(NotificationProviderTest, ExpectFailGetConsumerTopics)
{
    NSTopicLL topic;
    NSTopicLL * topicLL = &topic;

    topicLL = NSProviderGetConsumerTopics(NULL);
    topicLL = NSProviderGetConsumerTopics("\0");

    EXPECT_EQ(topicLL, (NSTopicLL *)NULL);
}

TEST_F(NotificationProviderTest, ExpectFailSetConsumerTopics)
{
    NSResult result = NS_SUCCESS;
    result = NSProviderSetConsumerTopic(NULL, NULL);
    result = NSProviderSetConsumerTopic(NULL, "\0");
    result = NSProviderSetConsumerTopic("\0", NULL);
    result = NSProviderSetConsumerTopic("\0", "\0");
    result = NSProviderSetConsumerTopic("abc", NULL);
    result = NSProviderSetConsumerTopic(NULL, "abc");
    result = NSProviderSetConsumerTopic("abc", "\0");
    result = NSProviderSetConsumerTopic("\0", "abc");

    EXPECT_EQ(result, NS_FAIL);
}

TEST_F(NotificationProviderTest, ExpectFailUnsetConsumerTopics)
{
    NSResult result = NS_SUCCESS;
    result = NSProviderUnsetConsumerTopic(NULL, NULL);
    result = NSProviderUnsetConsumerTopic(NULL, "\0");
    result = NSProviderUnsetConsumerTopic("\0", NULL);
    result = NSProviderUnsetConsumerTopic("\0", "\0");
    result = NSProviderUnsetConsumerTopic("abc", NULL);
    result = NSProviderUnsetConsumerTopic(NULL, "abc");
    result = NSProviderUnsetConsumerTopic("abc", "\0");
    result = NSProviderUnsetConsumerTopic("\0", "abc");

    EXPECT_EQ(result, NS_FAIL);
}

TEST_F(NotificationProviderTest, CancelObserves)
{
    bool ret = false;
    g_consumerSimul.findProvider();
    ret = g_consumerSimul.cancelObserves();

    std::chrono::milliseconds waitForTerminate(g_timeout);
    std::this_thread::sleep_for(waitForTerminate);

    EXPECT_EQ(ret, true);
}
