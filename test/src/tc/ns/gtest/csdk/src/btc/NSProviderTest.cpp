/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#include "NSProviderHelper.h"

class NSProviderTest_btc: public ::testing::Test
{
protected:
    NSProviderHelper* m_pNSHelper;
    const string m_MsgTitle = "Test Title";
    const string m_MsgBody = "Test Message Body";
    const string m_SourceName = "OCF";
    const string m_TopicName = "Test Topic";

    uint64_t m_MessageId = 10;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pNSHelper = NSProviderHelper::getInstance();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2016-08-11
 * @see None
 * @objective Test 'NSStartProvider' function with positive basic way
 * @target NSResult NSStartProvider(NSProviderConfig config);
 * @test_data 1. policy = true
 *            2. NSSubscribeRequestCallback
 *            3. NSProviderSyncInfoCallback
 * @pre_condition Construct config with test_data
 * @procedure Perform NSStartProvider() API
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, StartProviderWithNSPolicyTrue_SRC_P)
{
    NSProviderConfig config = m_pNSHelper->getProviderConfig(true);
    NSResult result = NSStartProvider(config);

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    EXPECT_EQ(NS_OK, result);
}
#endif

/**
 * @since 2016-08-11
 * @see None
 * @objective Test 'NSStartProvider' function with positive basic way
 * @target NSResult NSStartProvider(NSProviderConfig config);
 * @test_data 1. policy = false
 *            2. NSSubscribeRequestCallback
 *            3. NSProviderSyncInfoCallback
 * @pre_condition None
 * @procedure Perform NSStartProvider() API
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, StartProviderWithNSPolicyFalse_SRC_P)
{
    EXPECT_EQ(NS_OK, NSStartProvider(m_pNSHelper->getProviderConfig(false)));
}
#endif

/**
 * @since 2016-08-11
 * @see None
 * @objective Test 'NSStopProvider' function with positive basic way
 * @target NSResult NSStopProvider();
 * @test_data None
 * @pre_condition None
 * @procedure Perform NSStopProvider() API
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, StopProvider_SRC_P)
{
    EXPECT_EQ(NS_OK, NSStopProvider());
}
#endif

/**
 * @since 2016-08-13
 * @see None
 * @objective Test 'NSStartProvider' function with positive basic way
 * @target NSResult NSStartProvider(NSProviderConfig config);
 * @test_data empty NSProviderConfig
 * @pre_condition None
 * @procedure Perform NSStartProvider() API
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, StartProviderWithEmptyConfig_SRC_P)
{
    NSProviderConfig config;

    EXPECT_EQ(NS_OK, NSStartProvider(config));
}
#endif

/**
 * @since 2016-08-13
 * @see None
 * @objective Test 'NSCreateMessage()' function with positive basic way
 * @target NSMessage* NSCreateMessage();
 * @test_data None
 * @pre_condition None
 * @procedure Perform NSCreateMessage() API
 * @post_condition None
 * @expected The API should not return nullptr
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, CreateMessage_SRC_P)
{
    NSMessage *msg = NSCreateMessage();

    EXPECT_NE(nullptr, msg);
}
#endif

/**
 * @since 2016-08-13
 * @see NSMessage* NSCreateMessage();
 * @objective Test 'NSSendMessage' function with positive basic way
 * @target NSResult NSSendMessage(NSMessage *msg);
 * @test_data msg = NSMessage
 * @pre_condition Construct NSMessage
 * @procedure Perform NSSendMessage() API with msg
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, SendMessage_SRC_P)
{
    NSMessage *msg = NSCreateMessage();

    ASSERT_NE(nullptr, msg);

    msg->messageId = m_MessageId;
    msg->title = OICStrdup(m_MsgTitle.c_str());
    msg->contentText = OICStrdup(m_MsgBody.c_str());
    msg->sourceName = OICStrdup(m_SourceName.c_str());

    NSStartProvider(m_pNSHelper->getProviderConfig(true));
    EXPECT_EQ(NS_OK, NSSendMessage(msg));
}
#endif

/**
 * @since 2016-08-13
 * @see None
 * @objective Test 'NSSendMessage' function with positive basic way
 * @target NSResult NSSendMessage(NSMessage *msg);
 * @test_data msg = nullptr
 * @pre_condition None
 * @procedure Perform NSSendMessage() API
 * @post_condition None
 * @expected The API should not return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, SendMessage_NV_N)
{
    NSMessage *msg = nullptr;

    EXPECT_NE(NS_OK, NSSendMessage(msg));
}
#endif

/**
 * @since 2016-08-13
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSSubscribe(const char * providerId)
 * @objective Test 'NSAcceptSubscription' function with positive basic way
 * @target NSResult NSAcceptSubscription(const char * consumerId, bool accepted)
 * @test_data consumerId from callback, isAccept = true
 * @pre_condition 1. Start Provider
 *                2. Start Consumer
 *                3. Subscribe consumer
 * @procedure Perform NSAcceptSubscription() API
 * @post_condition Stop Consumer
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, AcceptSubscriptionAllow_SRC_P)
{
    NSStartProvider(m_pNSHelper->getProviderConfig(true));

    CommonUtil::launchApp(CONSUMER_SIMULATOR_OPTION);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    bool isAccept = true;
    string consumerID = m_pNSHelper->getConsumerID();

    CommonUtil::killApp(CONSUMER_SIMULATOR);

    EXPECT_EQ(NS_OK, NSAcceptSubscription(consumerID.c_str(), isAccept));
}
#endif

/**
 * @since 2016-08-13
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSSubscribe(const char * providerId)
 * @objective Test 'NSAcceptSubscription' function with positive basic way
 * @target NSResult NSAcceptSubscription(const char * consumerId, bool accepted)
 * @test_data consumerId from callback, isAccept = false
 * @pre_condition 1. Start Provider
 *                2. Start Consumer
 *                3. Subscribe consumer
 * @procedure Perform NSAcceptSubscription() API
 * @post_condition Stop Consumer
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, AcceptSubscriptionDeny_SRC_P)
{
    NSStartProvider(m_pNSHelper->getProviderConfig(true));

    CommonUtil::launchApp(CONSUMER_SIMULATOR_OPTION);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    bool isAccept = false;
    string consumerID = m_pNSHelper->getConsumerID();

    CommonUtil::killApp(CONSUMER_SIMULATOR);

    EXPECT_EQ(NS_OK, NSAcceptSubscription(consumerID.c_str(), isAccept));
}
#endif

/**
 * @since 2016-08-17
 * @see None
 * @objective Test 'NSAcceptSubscription' function with null consumerId
 * @target NSResult NSAcceptSubscription(const char * consumerId, bool accepted)
 * @test_data consumerId = NULL
 * @pre_condition None
 * @procedure Perform NSAcceptSubscription() API
 * @post_condition None
 * @expected The API should not return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, AcceptSubscription_NV_N)
{
    bool isAccept = true;
    NSConsumer *consumer = nullptr;

    EXPECT_NE(NS_OK, NSAcceptSubscription(consumer->consumerId, isAccept));
}
#endif

/**
 * @since 2016-08-13
 * @see None
 * @objective Test 'NSProviderSendSyncInfo' function with positive basic way
 * @target NSResult NSProviderSendSyncInfo(uint64_t messageId, NSSyncType type)
 * @test_data m_MessageId = 10, type = NS_SYNC_UNREAD
 * @pre_condition None
 * @procedure Perform NSProviderSendSyncInfo() API
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, SendSyncInfoAsUnread_SRC_P)
{
    NSStartProvider(m_pNSHelper->getProviderConfig(true));
    EXPECT_EQ(NS_OK, NSProviderSendSyncInfo(m_MessageId, NSSyncType::NS_SYNC_UNREAD));
}
#endif

/**
 * @since 2016-08-13
 * @see None
 * @objective Test 'NSProviderSendSyncInfo' function with positive basic way
 * @target NSResult NSProviderSendSyncInfo(uint64_t messageId, NSSyncType type)
 * @test_data m_MessageId = 10, type = NS_SYNC_READ
 * @pre_condition None
 * @procedure Perform NSProviderSendSyncInfo() API
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, SendSyncInfoAsRead_SRC_P)
{
    NSStartProvider(m_pNSHelper->getProviderConfig(true));
    EXPECT_EQ(NS_OK, NSProviderSendSyncInfo(m_MessageId, NSSyncType::NS_SYNC_READ));
}
#endif

/**
 * @since 2016-08-13
 * @see None
 * @objective Test 'NSProviderSendSyncInfo' function with positive basic way
 * @target NSResult NSProviderSendSyncInfo(uint64_t messageId, NSSyncType type)
 * @test_data m_MessageId = 10, type = NS_SYNC_DELETED
 * @pre_condition None
 * @procedure Perform NSProviderSendSyncInfo() API
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, SendSyncInfoAsDeleted_SRC_P)
{
    NSStartProvider(m_pNSHelper->getProviderConfig(true));
    EXPECT_EQ(NS_OK, NSProviderSendSyncInfo(m_MessageId, NSSyncType::NS_SYNC_DELETED));
}
#endif

/**
 * @since 2016-08-13
 * @see None
 * @objective Test 'NSProviderRegisterTopic' function with positive basic way
 * @target NSResult NSProviderRegisterTopic(const char * topicName)
 * @test_data TopicName = "Test Topic"
 * @pre_condition None
 * @procedure Perform NSProviderRegisterTopic() API
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, NSRegisterTopic_SRC_P)
{
    NSStartProvider(m_pNSHelper->getProviderConfig(true));
    EXPECT_EQ(NS_OK, NSProviderRegisterTopic(m_TopicName.c_str()));
}
#endif

/**
 * @since 2016-08-13
 * @see NSResult NSProviderRegisterTopic(const char * topicName)
 * @objective Test 'NSProviderUnregisterTopic' function with positive basic way
 * @target NSResult NSProviderUnregisterTopic(const char * topicName)
 * @test_data TopicName = "Test Topic"
 * @pre_condition Perform NSProviderRegisterTopic() with TopicName
 * @procedure Perform NSProviderUnregisterTopic() with TopicName
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, NSUnregisterTopic_SRC_P)
{
    NSStartProvider(m_pNSHelper->getProviderConfig(true));
    NSProviderRegisterTopic(m_TopicName.c_str());

    EXPECT_EQ(NS_OK, NSProviderUnregisterTopic(m_TopicName.c_str()));
}
#endif

/**
 * @since 2016-08-13
 * @see None
 * @objective Test 'NSProviderUnregisterTopic' function without registering
 * @target NSResult NSProviderUnregisterTopic(const char * topicName)
 * @test_data TopicName = "Test Topic"
 * @pre_condition None
 * @procedure Perform NSProviderUnregisterTopic() with TopicName
 * @post_condition None
 * @expected The API should not return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, NSUnregisterTopic_USV_N)
{
    NSStartProvider(m_pNSHelper->getProviderConfig(true));
    EXPECT_NE(NS_OK, NSProviderUnregisterTopic(m_TopicName.c_str()))
    << "Unregister topic success without registering.";
}
#endif

/**
 * @since 2016-09-08
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSSubscribe(const char * providerId)
 * @objective Test 'NSProviderSetConsumerTopic' function with positive basic way
 * @target NSResult NSProviderSetConsumerTopic(const char * consumerId, const char * topicName)
 * @test_data consumerId from callback, topic = "Test Topic"
 * @pre_condition 1. Start Provider
 *                2. Register topic
 *                3. Start Consumer
 *                4. Subscribe consumer
 *                5. Get consumer from callback
 * @procedure Perform NSProviderSetConsumerTopic() API
 * @post_condition Stop Consumer
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, NSSetConsumerTopic_SRC_P)
{
    NSStartProvider(m_pNSHelper->getProviderConfig(true));
    NSProviderRegisterTopic(m_TopicName.c_str());

    CommonUtil::launchApp(CONSUMER_SIMULATOR_OPTION);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    string consumerID = m_pNSHelper->getConsumerID();

    CommonUtil::killApp(CONSUMER_SIMULATOR);

    EXPECT_EQ(NS_OK, NSProviderSetConsumerTopic(consumerID.c_str(), m_TopicName.c_str()));
}
#endif

/**
 * @since 2016-10-06
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSSubscribe(const char * providerId)
 * @objective Test 'NSProviderSetConsumerTopic' function with positive basic way
 * @target NSResult NSProviderSetConsumerTopic(const char * consumerId, const char * topicName)
 * @test_data consumerId from callback, topic = "Test Topic"
 * @pre_condition 1. Start Provider
 *                2. Start Consumer
 *                3. Subscribe consumer
 *                4. Get consumer from callback
 * @procedure Perform NSProviderSetConsumerTopic() API
 * @post_condition Stop Consumer
 * @expected The API should not return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, NSSetConsumerTopic_USV_N)
{
    NSStartProvider(m_pNSHelper->getProviderConfig(true));

    CommonUtil::launchApp(CONSUMER_SIMULATOR_OPTION);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    string consumerID = m_pNSHelper->getConsumerID();

    CommonUtil::killApp(CONSUMER_SIMULATOR);

    EXPECT_NE(NS_OK, NSProviderSetConsumerTopic(consumerID.c_str(), m_TopicName.c_str()));
}
#endif

/**
 * @since 2016-08-13
 * @see None
 * @objective Test 'NSProviderSetConsumerTopic' function with positive basic way
 * @target NSResult NSProviderSetConsumerTopic(const char * consumerId, const char * topicName)
 * @test_data null consumerId, topic = "Test Topic"
 * @pre_condition None
 * @procedure Perform NSProviderSetConsumerTopic() with null consumerId
 * @post_condition Stop Consumer
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, NSSetConsumerTopic_NV_N)
{
    NSConsumer *consumer = nullptr;

    NSResult result = NSProviderSetConsumerTopic(consumer->consumerId, m_TopicName.c_str());

    EXPECT_NE(NS_OK, result);
}
#endif

/**
 * @since 2016-09-08
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSSubscribe(const char * providerId)
 * @see NSResult NSProviderSetConsumerTopic(const char * consumerId, const char * topicName)
 * @objective Test 'NSProviderUnsetConsumerTopic' function with positive basic way
 * @target NSResult NSProviderUnsetConsumerTopic(const char * consumerId, const char * topicName)
 * @test_data consumerId from callback, topic = "Test Topic"
 * @pre_condition 1. Start Provider
 *                2. Register topic
 *                3. Start Consumer
 *                4. Subscribe consumer
 *                5. Get consumer from callback
 *                6. Perform NSProviderSetConsumerTopic() API
 * @procedure Perform NSProviderUnsetConsumerTopic() API
 * @post_condition Stop Consumer
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, NSUnsetConsumerTopic_SRC_P)
{
    NSStartProvider(m_pNSHelper->getProviderConfig(true));
    NSProviderRegisterTopic(m_TopicName.c_str());

    CommonUtil::launchApp(CONSUMER_SIMULATOR_OPTION);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    string consumerID = m_pNSHelper->getConsumerID();

    CommonUtil::killApp(CONSUMER_SIMULATOR);

    NSProviderSetConsumerTopic(consumerID.c_str(), m_TopicName.c_str());

    EXPECT_EQ(NS_OK, NSProviderUnsetConsumerTopic(consumerID.c_str(), m_TopicName.c_str()));
}
#endif

/**
 * @since 2016-09-08
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSSubscribe(const char * providerId)
 * @see NSResult NSProviderSetConsumerTopic(const char * consumerId, const char * topicName)
 * @objective Test 'NSProviderUnsetConsumerTopic' function with positive basic way
 * @target NSResult NSProviderUnsetConsumerTopic(const char * consumerId, const char * topicName)
 * @test_data consumerId from callback, topic = "Test Topic"
 * @pre_condition 1. Start Provider as Provider Acceptor
 *                2. Register topic
 *                3. Start Consumer
 *                4. Subscribe consumer
 *                5. Get consumer from callback
 *                6. Perform NSProviderSetConsumerTopic() API
 * @procedure Perform NSProviderUnsetConsumerTopic() API
 * @post_condition Stop Consumer
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, NSUnsetConsumerTopic_USV_N)
{
    NSStartProvider(m_pNSHelper->getProviderConfig(true));
    NSProviderRegisterTopic(m_TopicName.c_str());

    CommonUtil::launchApp(CONSUMER_SIMULATOR_OPTION);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    string consumerID = m_pNSHelper->getConsumerID();

    CommonUtil::killApp(CONSUMER_SIMULATOR);

    EXPECT_NE(NS_OK, NSProviderUnsetConsumerTopic(consumerID.c_str(), m_TopicName.c_str()));
}
#endif

/**
 * @since 2016-08-13
 * @see None
 * @objective Test 'NSProviderUnsetConsumerTopic' function with positive basic way
 * @target NSResult NSProviderUnsetConsumerTopic(const char * consumerId, const char * topicName)
 * @test_data null consumerId, topic = "Test Topic"
 * @pre_condition None
 * @procedure Perform NSProviderUnsetConsumerTopic() with null consumerId
 * @post_condition Stop Consumer
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, NSUnsetConsumerTopic_NV_N)
{
    NSConsumer *consumer = nullptr;

    EXPECT_NE(NS_OK, NSProviderUnsetConsumerTopic(consumer->consumerId, m_TopicName.c_str()));
}
#endif

/**
 * @since 2016-09-07
 * @see None
 * @objective Test 'NSProviderGetConsumerTopics' function with positive basic way
 * @target NSTopicLL * NSProviderGetConsumerTopics(const char * consumerId)
 * @test_data null consumerId
 * @pre_condition None
 * @procedure Perform NSProviderGetConsumerTopics() with null consumerId
 * @post_condition None
 * @expected The API should not return Topic list
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, NSGetConsumerTopics_NV_N)
{
    NSConsumer *consumer = nullptr;

    EXPECT_EQ(NULL, NSProviderGetConsumerTopics(consumer->consumerId));
}
#endif

/**
 * @since 2016-09-07
 * @see NSResult NSProviderRegisterTopic(const char * topicName)
 * @objective Test 'NSProviderGetTopics' function with positive basic way
 * @target NSTopicLL * NSProviderGetTopics()
 * @test_data msg = nullptr
 * @pre_condition Register topic
 * @procedure Perform NSSendMessage() API
 * @post_condition None
 * @expected The topic list should not be null
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, NSGetTopics_SRC_P)
{
    NSProviderConfig config = m_pNSHelper->getProviderConfig(true);
    NSResult result = NSStartProvider(config);

    NSProviderRegisterTopic(m_TopicName.c_str());

    EXPECT_NE(NULL, NSProviderGetTopics());
}
#endif

/**
 * @since 2016-09-07
 * @see NSResult NSProviderRegisterTopic(const char * topicName)
 * @objective Test 'NSProviderGetTopics' function with positive basic way
 * @target NSTopicLL * NSProviderGetTopics()
 * @test_data msg = nullptr
 * @pre_condition Register topic
 * @procedure Perform NSSendMessage() API
 * @post_condition None
 * @expected The topic list should be null
 **/
#if defined(__LINUX__)
TEST_F(NSProviderTest_btc, NSGetTopics_USV_N)
{
    NSProviderConfig config = m_pNSHelper->getProviderConfig(true);
    NSResult result = NSStartProvider(config);

    EXPECT_EQ(NULL, NSProviderGetTopics());
}
#endif
