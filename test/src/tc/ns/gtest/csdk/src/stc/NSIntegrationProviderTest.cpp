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

class NSIntegrationProviderTest_stc: public ::testing::Test
{
protected:
    NSProviderHelper* m_pNSProviderHelper;
    NSConsumer* m_pNSConsumer;
    NSTopicLL* m_pNSTopicLL;
    const string m_MsgTitle = "Test Title";
    const string m_MsgBody = "Test Message Body";
    const string m_SourceName = "OCF";
    const string m_TopicName = "Test_Topic_2";
    uint64_t m_MessageId = 10;
    NSCommonHelper* m_pCommonHelper;
    string m_testAppLog;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pNSProviderHelper = NSProviderHelper::getInstance();

        m_pNSConsumer = nullptr;
        m_pNSTopicLL = nullptr;
        m_pNSProviderHelper->s_isOnSubscribeRequest = false;
        m_pNSProviderHelper->s_isOnProviderSyncInfo = false;

        m_pCommonHelper = NSCommonHelper::getInstance();
        CommonUtil::launchApp(CONSUMER_APP_OPTION);
        CommonUtil::waitInSecond(WAIT_TIME_MIN);

        m_testAppLog = "";
    }

    virtual void TearDown()
    {
        m_pCommonHelper->closePipe();
        CommonUtil::killApp(CONSUMER_TESTAPP);
        CommonUtil::waitInSecond(WAIT_TIME_MIN);

        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @objective Test if provider receives consumers subscribe request
 * @target NSResult NSStartProvider(NSProviderConfig config)
 * @test_data config provider configuration
 * @pre_condition   1. Perform Configure() and stopPresence() API
 * @procedure       1. Start Provider with subcontrollability true
 *                  2. Perform NSStartConsumer() API with config
 *                  3. Check if onSubscribeRequest Callback is called in provider side
 * @post_condition None
 * @expected Callback should be called and consumer should be found in callback
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationProviderTest_stc, StartProviderAndStartConsumer_SQV_CV_P)
{
    NSProviderConfig config = m_pNSProviderHelper->getProviderConfig(true);
    NSResult result = NSStartProvider(config);

    ASSERT_EQ(NS_OK, result) << "Provider was not started successfully";

    IOTIVITYTEST_LOG(INFO, "Starting consumer app");
    m_pCommonHelper->initPipe(false);
    m_pCommonHelper->inputMenu("1");
    m_pNSConsumer = m_pNSProviderHelper->getConsumer();

    m_pCommonHelper->closePipe();
    m_testAppLog = m_pCommonHelper->getAppLog();
    IOTIVITYTEST_LOG(INFO, "Consumer Log: %s", m_testAppLog.c_str());

    ASSERT_EQ(m_pNSProviderHelper->s_isOnSubscribeRequest,true)<< "onSubscribeRequest callback was not called";
    ASSERT_NE(m_pNSConsumer,nullptr)<< "consumer was not found";
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @objective Test if provider can accept subscription successfully
 * @target NSResult NSAcceptSubscription(const char * consumerId, bool accepted)
 * @test_data       1. consumerId ID of the found consuemer
 *                  2. accepted true
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability true
 *                  3. Perform NSStartConsumer() API with config
 *                  4. Check if onSubscribeRequest Callback is called in provider side
 * @procedure       1. Perform NSAcceptSubscription() API in provider side
 *                  2. Check if callback is called in consumer side
 * @post_condition None
 * @expected Callback should be called in consumer side
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationProviderTest_stc, AcceptSubscription_SQV_P)
{
    NSProviderConfig config = m_pNSProviderHelper->getProviderConfig(true);
    NSResult result = NSStartProvider(config);

    ASSERT_EQ(NS_OK, result) << "Provider was not started successfully";

    IOTIVITYTEST_LOG(INFO, "Starting consumer app");
    m_pCommonHelper->initPipe(false);
    m_pCommonHelper->inputMenu("1");
    m_pNSConsumer = m_pNSProviderHelper->getConsumer();

    ASSERT_EQ(m_pNSProviderHelper->s_isOnSubscribeRequest,true)<< "onSubscribeRequest callback was not called";

    ASSERT_NE(m_pNSConsumer,nullptr)<<"consumer was not found";

    bool isAccept = true;

    ASSERT_EQ(NS_OK, NSAcceptSubscription(NSProviderHelper::s_consumerID.c_str(), isAccept))<< "NSAcceptSubscription did not return success";
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);
    m_pCommonHelper->loggerReader();

    m_pCommonHelper->closePipe();
    m_testAppLog = m_pCommonHelper->getAppLog();
    IOTIVITYTEST_LOG(INFO, "Consumer Log: %s", m_testAppLog.c_str());
    int findResult = m_testAppLog.find("Provider changed to NS_ALLOW State");
    ASSERT_NE(-1 , findResult)<< "Callback was not called in consumer app";
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSAcceptSubscription(const char * consumerId, bool accepted)
 * @objective Test if provider can accept subscription successfully
 * @target NSResult NSStopProvider()
 * @test_data none
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability true
 *                  3. Perform NSStartConsumer() API with config
 *                  4. Check if onSubscribeRequest Callback is called in provider side
 *                  5. Perform NSAcceptSubscription() API in provider side
 *                  6. Check if callback is called in consumer side
 * @procedure       1. Perform NSStopProvider() API and check if it returns OK
 *                  2. Check if callback is called in consumer side
 * @post_condition None
 * @expected Callback should be called in consumer side
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationProviderTest_stc, StopProvider_SQV_P)
{
    NSProviderConfig config = m_pNSProviderHelper->getProviderConfig(true);
    NSResult result = NSStartProvider(config);

    ASSERT_EQ(NS_OK, result) << "Provider was not started successfully";

    IOTIVITYTEST_LOG(INFO, "Starting consumer app");
    m_pCommonHelper->initPipe(false);
    m_pCommonHelper->inputMenu("1");
    m_pNSConsumer = m_pNSProviderHelper->getConsumer();

    ASSERT_EQ(m_pNSProviderHelper->s_isOnSubscribeRequest,true)<< "onSubscribeRequest callback was not called";

    ASSERT_NE(m_pNSConsumer,nullptr)<<"consumer was not found";

    bool isAccept = true;

    ASSERT_EQ(NS_OK, NSAcceptSubscription(NSProviderHelper::s_consumerID.c_str(), isAccept))<< "NSAcceptSubscription did not return success";
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);
    ASSERT_EQ(NS_OK, NSStopProvider())<< "NSStopProvider did not return success";

    IOTIVITYTEST_LOG(INFO, "Checking if callback is called in consumer");
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);
    m_pCommonHelper->loggerReader();
    m_pCommonHelper->closePipe();
    m_testAppLog = m_pCommonHelper->getAppLog();
    IOTIVITYTEST_LOG(INFO, "Consumer Log: %s", m_testAppLog.c_str());

    int findResult = m_testAppLog.find("Provider changed to NS_STOPPED State");
    ASSERT_NE(-1 , findResult)<< "Callback was not called in consumer app";
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSAcceptSubscription(const char * consumerId, bool accepted)
 * @see NSResult NSProviderRegisterTopic(const char * topicName)
 * @see NSResult NSProviderSetConsumerTopic(const char * consumerId, const char * topicName)
 * @see NSMessage * NSCreateMessage()
 * @see NSResult NSSendMessage(NSMessage * msg)
 * @objective Test if provider can send message successfully
 * @target NSResult NSSendMessage(NSMessage * msg)
 * @test_data msg message to send
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability true
 *                  3. Perform NSStartConsumer() API with config
 *                  4. Check if onSubscribeRequest Callback is called in provider side
 *                  5. Perform NSAcceptSubscription() API in provider side
 *                  6. Check if callback is called in consumer side
 *                  7. Register one topic in provider
 *                  8. Set consumer topic using NSProviderSetConsumerTopic() API
 *                  9. Perform NSCreateMessage() API to get NSMessage instance
 * @procedure       1. Bind necessary values with the NSMessage instance
 *                  2. Perform NSSendMessage() API using the NSMessage instance
 *                  3. Check if callback is called in consumer side
 * @post_condition None
 * @expected NSMessageReceivedCallback should be called in consumer side
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationProviderTest_stc, ProviderSendMessage_SQV_P)
{
    NSProviderConfig config = m_pNSProviderHelper->getProviderConfig(true);
    NSResult result = NSStartProvider(config);

    ASSERT_EQ(NS_OK, result) << "Provider was not started successfully";

    IOTIVITYTEST_LOG(INFO, "Starting consumer app");
    m_pCommonHelper->initPipe(false);
    m_pCommonHelper->inputMenu("1");
    m_pNSConsumer = m_pNSProviderHelper->getConsumer();

    ASSERT_EQ(m_pNSProviderHelper->s_isOnSubscribeRequest,true)<< "onSubscribeRequest callback was not called";

    ASSERT_NE(m_pNSConsumer,nullptr)<<"consumer was not found";

    bool isAccept = true;

    ASSERT_EQ(NS_OK, NSAcceptSubscription(NSProviderHelper::s_consumerID.c_str(), isAccept))<< "NSAcceptSubscription did not return success";

    ASSERT_EQ(NS_OK, NSProviderRegisterTopic(TOPIC_NAME_PROVIDER))<< "NSProviderRegisterTopic did not return success";

    ASSERT_EQ(NS_OK, NSProviderSetConsumerTopic(NSProviderHelper::s_consumerID.c_str(),TOPIC_NAME_PROVIDER))<< "NSProviderSetConsumerTopic did not return success";

    NSMessage* msg = nullptr;
    msg = NSCreateMessage();
    ASSERT_NE(msg,nullptr)<<"NSMessage was not created";

    string title = "TESTAPP_Title_1";
    msg->title = title.c_str();

    string body = "TESTAPP_Body_1";
    msg->contentText = body.c_str();

    msg->sourceName = OICStrdup("TEST");

    msg->topic = TOPIC_NAME_PROVIDER;

    ASSERT_EQ(NS_OK, NSSendMessage(msg))<< "NSSendMessage did not return success";

    m_pCommonHelper->loggerReader();
    IOTIVITYTEST_LOG(INFO, "Checking if callback is called in consumer");
    CommonUtil::waitInSecond(WAIT_TIME_MAX);

    m_pCommonHelper->loggerReader();
    m_pCommonHelper->closePipe();
    m_testAppLog = m_pCommonHelper->getAppLog();
    IOTIVITYTEST_LOG(INFO, "Consumer Log: %s", m_testAppLog.c_str());

    int findResult = m_testAppLog.find("NSMessageReceivedCallback called");
    ASSERT_NE(-1 , findResult)<< "NSMessage was not received in the consumer app";
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSTopicLL * NSProviderGetTopics()
 * @objective Test if provider can register topic successfully
 * @target NSResult NSProviderRegisterTopic(const char * topicName)
 * @test_data topicName name of the topic
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability true
 * @procedure       1. Perform NSProviderRegisterTopic() API and check if it returns OK
 *                  2. Perform NSProviderGetTopics() API and check if received list is not null
 *                  3. Check if the registered topic exists in the received list
 * @post_condition None
 * @expected Topic exists in the received list
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationProviderTest_stc, RegisterTopic_SQV_P)
{
    NSProviderConfig config = m_pNSProviderHelper->getProviderConfig(false);
    NSResult result = NSStartProvider(config);

    ASSERT_EQ(NS_OK, result) << "Provider was not started successfully";

    ASSERT_EQ(NS_OK, NSProviderRegisterTopic(TOPIC_NAME_PROVIDER))<< "NSProviderRegisterTopic did not return success";

    m_pNSTopicLL = NSProviderGetTopics();

    ASSERT_NE(m_pNSTopicLL,nullptr)<<"registered topic list was not found";

    ASSERT_EQ(m_pNSProviderHelper->printProviderTopicList(m_pNSTopicLL),true)<< "registered topic name was not found in the received topic list";
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSProviderRegisterTopic(const char * topicName)
 * @see NSTopicLL * NSProviderGetTopics()
 * @objective Test if provider can unregister topic successfully
 * @target NSResult NSProviderUnregisterTopic(const char * topicName)
 * @test_data topicName name of the topic
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability true
 *                  3. Perform NSProviderRegisterTopic() API to register a topic
 *                  4. Perform NSProviderGetTopics() API and check if received list is not null
 *                  5. Check if the registered topic exists in the received list
 * @procedure       1. Perform NSProviderUnregisterTopic() API and check if it returns OK
 *                  2. Perform NSProviderGetTopics() API and check if received list is not null
 *                  3. Check if the unregistered topic does not exist in the received list
 * @post_condition None
 * @expected Topic does not exist in the received list
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationProviderTest_stc, UnregisterTopic_SQV_P)
{
    NSProviderConfig config = m_pNSProviderHelper->getProviderConfig(true);
    NSResult result = NSStartProvider(config);

    ASSERT_EQ(NS_OK, result) << "Provider was not started successfully";

    ASSERT_EQ(NS_OK, NSProviderRegisterTopic(TOPIC_NAME_PROVIDER))<< "NSProviderRegisterTopic did not return success for the first topic";

    EXPECT_EQ(NS_OK, NSProviderRegisterTopic(m_TopicName.c_str()))
            << "NSProviderRegisterTopic did not return success for the second topic";

    m_pNSTopicLL = NSProviderGetTopics();

    ASSERT_NE(m_pNSTopicLL,nullptr)<<"registered topic list was not found";

    ASSERT_EQ(m_pNSProviderHelper->printProviderTopicList(m_pNSTopicLL),true)<< "registered topic name was not found in the received topic list";

    ASSERT_EQ(NS_OK, NSProviderUnregisterTopic(TOPIC_NAME_PROVIDER))<< "NSProviderUnregisterTopic did not return success";

    m_pNSTopicLL = nullptr;

    m_pNSTopicLL = NSProviderGetTopics();

    ASSERT_NE(m_pNSTopicLL,nullptr)<<"topic list was not found";

    ASSERT_EQ(m_pNSProviderHelper->printProviderTopicList(m_pNSTopicLL),false)<< "unregistered topic should not be found";
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSAcceptSubscription(const char * consumerId, bool accepted)
 * @objective Test if provider can get consumer topics successfully
 * @target NSTopicLL * NSProviderGetConsumerTopics(const char * consumerId)
 * @test_data none
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability true
 *                  3. Perform NSStartConsumer() API with config
 *                  4. Check if onSubscribeRequest Callback is called in provider side
 *                  5. Perform NSAcceptSubscription() API in provider side
 *                  6. Check if callback is called in consumer side
 *                  7. Perform NSProviderRegisterTopic() API to register a topic
 * @procedure       1. Perform NSProviderGetConsumerTopics() API
 *                  2. Check if returned list contains the registered topic
 * @post_condition None
 * @expected Registered topic should exist in the returned list
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationProviderTest_stc, GetConsumerTopics_SQV_P)
{
    NSProviderConfig config = m_pNSProviderHelper->getProviderConfig(true);
    NSResult result = NSStartProvider(config);

    ASSERT_EQ(NS_OK, result) << "Provider was not started successfully";

    IOTIVITYTEST_LOG(INFO, "Starting consumer app");
    m_pCommonHelper->initPipe(false);
    m_pCommonHelper->inputMenu("1");
    m_pNSConsumer = m_pNSProviderHelper->getConsumer();

    ASSERT_EQ(m_pNSProviderHelper->s_isOnSubscribeRequest,true)<< "onSubscribeRequest callback was not called";

    ASSERT_NE(m_pNSConsumer,nullptr)<<"consumer was not found";

    bool isAccept = true;

    ASSERT_EQ(NS_OK, NSAcceptSubscription(NSProviderHelper::s_consumerID.c_str(), isAccept))<< "NSAcceptSubscription did not return success";

    ASSERT_EQ(NS_OK, NSProviderRegisterTopic(TOPIC_NAME_PROVIDER))<< "NSProviderRegisterTopic did not return success";

    IOTIVITYTEST_LOG(INFO, "Waiting for consumer to get the registered topic");

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    m_pNSTopicLL = NSProviderGetConsumerTopics(NSProviderHelper::s_consumerID.c_str());

    ASSERT_NE(m_pNSTopicLL,nullptr)<<"consumer's topic list was not found";

    ASSERT_EQ(m_pNSProviderHelper->printProviderTopicList(m_pNSTopicLL),true)<< "registered topic name was not found in the received topic list"
    "from consumer";
}
#endif
