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

#include "NSProviderCppHelper.h"

class NSProviderCppTest_btc: public ::testing::Test
{
public:
    NSProviderCppHelper* m_pNSProviderHelper;
    NSProviderService* m_pNSProviderServiceInstance;
    OIC::Service::NSConsumer* m_pNSConsumer;
    OIC::Service::NSResult m_result;
    string m_consumerID = "";
    NSProviderService::ProviderConfig m_providerCfgTrue;
    NSProviderService::ProviderConfig m_providerCfgFalse;
    bool m_accepted = false;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pNSConsumer = nullptr;
        m_consumerID = "";

        m_pNSProviderHelper = NSProviderCppHelper::getInstance();

        m_pNSProviderServiceInstance = nullptr;
        m_pNSProviderServiceInstance = NSProviderService::getInstance();
        ASSERT_NE(nullptr,m_pNSProviderServiceInstance) << "NSProviderService instance could not be found";

        m_providerCfgTrue.m_subscribeRequestCb = subscribeRequestCallback;
        m_providerCfgTrue.m_syncInfoCb = syncCallback;
        m_providerCfgTrue.subControllability = true;

        m_providerCfgFalse.m_subscribeRequestCb = subscribeRequestCallback;
        m_providerCfgFalse.m_syncInfoCb = syncCallback;
        m_providerCfgFalse.subControllability = false;

        OIC::Service::NSResult m_result;

        IOTIVITYTEST_LOG(INFO, "SetUp called");
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();

        CommonUtil::killApp(CONSUMER_SIMULATOR);
        CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

        m_pNSProviderServiceInstance->stop();

        IOTIVITYTEST_LOG(INFO, "TearDown called");
    }

    static void syncCallback(OIC::Service::NSSyncInfo *sync)
    {
        IOTIVITYTEST_LOG(INFO, "NSProviderSyncInfoCallback() called !!");
    }

    static void subscribeRequestCallback(OIC::Service::NSConsumer *consumer)
    {
        IOTIVITYTEST_LOG(INFO, "subscribeRequestCallback() called !!");
        IOTIVITYTEST_LOG(INFO, "Consumer ID: %s", consumer->getConsumerId().c_str());
    }
};

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @objective Test 'getInstance' function with positive basic way
 * @target NSProviderService *getInstance()
 * @test_data None
 * @pre_condition Perform Configure() and stopPresence() API
 * @procedure Perform getInstance() API
 * @post_condition None
 * @expected The API should return NSProviderService instance
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, ProviderServiceGetInstance_SRC_P)
{
    ASSERT_NE(nullptr,NSProviderService::getInstance())<< "getInstance does not return instance";
}
#endif

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @objective Test 'start' function with positive basic way
 * @target NSResult start(ProviderConfig config)
 * @test_data ProviderConfig ProviderConfig Callback function pointers to onConsumerSubscribed,
 *                    and onMessageSynchronized function listeners
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 * @procedure Perform start() API with subcontrollability true
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, ProviderServiceStartSubcontrollabilityTrue_SRC_P)
{
    m_result = m_pNSProviderServiceInstance->start(m_providerCfgTrue);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "start did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @objective Test 'start' function with positive basic way
 * @target NSResult start(ProviderConfig config)
 * @test_data ProviderConfig ProviderConfig Callback function pointers to onConsumerSubscribed,
 *                    and onMessageSynchronized function listeners
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 * @procedure Perform start() API with subcontrollability false
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, ProviderServiceStartSubcontrollabilityFalse_SRC_P)
{
    m_result = m_pNSProviderServiceInstance->start(m_providerCfgFalse);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "start did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @objective Test 'start' function with positive basic way
 * @target NSResult start(ProviderConfig config)
 * @test_data ProviderConfig ProviderConfig Callback function pointers to onConsumerSubscribed,
 *                    and onMessageSynchronized function listeners
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 * @procedure Perform start() API without configuring
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, ProviderServiceStart_USV_P)
{
    NSProviderService::ProviderConfig providerCfg;
    m_result = m_pNSProviderServiceInstance->start(providerCfg);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "start did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @objective Test 'stop' function with positive basic way
 * @target NSResult stop()
 * @test_data None
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 * @procedure Perform stop() API
 * @post_condition None
 * @expected The API should return OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, ProviderServiceStop_SRC_P)
{
    m_result = m_pNSProviderServiceInstance->start(m_providerCfgTrue);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "start did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    m_result = m_pNSProviderServiceInstance->stop();
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "stop did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @objective Test 'createMessage' function with positive basic way
 * @target NSMessage *createMessage()
 * @test_data None
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 * @procedure Perform createMessage() API
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return NSMessage instance
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, CreateMessage_SRC_P)
{
    m_result = m_pNSProviderServiceInstance->start(m_providerCfgTrue);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "start did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    OIC::Service::NSMessage* msg = nullptr;
    msg = m_pNSProviderServiceInstance->createMessage();
    ASSERT_NE(nullptr,msg)<< "createMessage did not return message";
}
#endif

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see NSMessage *createMessage()
 * @objective Test 'sendMessage' function with positive basic way
 * @target NSResult sendMessage(NSMessage *msg)
 * @test_data msg NSMessage instance
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 *                  4. Perform createMessage() API to get NSMessage instance
 * @procedure Perform sendMessage() API with that NSMessage instance
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, SendMessage_SRC_P)
{
    m_result = m_pNSProviderServiceInstance->start(m_providerCfgTrue);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "start did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    OIC::Service::NSMessage* msg = nullptr;
    msg = m_pNSProviderServiceInstance->createMessage();
    ASSERT_NE(nullptr,msg)<< "createMessage did not return message";

    m_result = m_pNSProviderServiceInstance->sendMessage(msg);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "sendMessage did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @objective Test 'sendMessage' function with negative basic way
 * @target NSResult sendMessage(NSMessage *msg)
 * @test_data msg NSMessage instance
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 * @procedure Perform sendMessage() API with null pointer
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, SendMessage_NV_N)
{
    m_result = m_pNSProviderServiceInstance->start(m_providerCfgTrue);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "start did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    m_result = m_pNSProviderServiceInstance->sendMessage(nullptr);
    ASSERT_EQ(OIC::Service::NSResult::ERROR,m_result)<< "sendMessage did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @objective Test 'sendSyncInfo' function with positive basic way
 * @target void sendSyncInfo(uint64_t messageId, NSSyncInfo::NSSyncType type)
 * @test_data       1. messageId 1
 *                  2. NSSyncType NS_SYNC_READ
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 * @procedure Perform sendSyncInfo() API
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should not throw any exceptions
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, SendSyncInfo_SRC_P)
{
    m_result = m_pNSProviderServiceInstance->start(m_providerCfgTrue);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "start did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    uint64_t messageId = 1;
    OIC::Service::NSSyncInfo::NSSyncType type = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;

    try
    {
        m_pNSProviderServiceInstance->sendSyncInfo(messageId, type);
    }
    catch (exception &e)
    {
        SET_FAILURE(
                "Exception occurs while calling sendSyncInfo. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @objective Test 'registerTopic' function with positive basic way
 * @target NSResult registerTopic(const std::string &topicName)
 * @test_data topicName = "TEST_TOPIC_1"
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 * @procedure Perform registerTopic() API
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, RegisterTopic_SRC_P)
{
    m_result = m_pNSProviderServiceInstance->start(m_providerCfgTrue);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "start did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    m_result = m_pNSProviderServiceInstance->registerTopic(TEST_TOPIC_1);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "registerTopic did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @objective Test 'registerTopic' function with negative basic way
 * @target NSResult registerTopic(const std::string &topicName)
 * @test_data topicName = ""
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 * @procedure Perform registerTopic() API with empty string as parameter
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return FAIL
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, RegisterTopic_ESV_N)
{
    m_result = m_pNSProviderServiceInstance->start(m_providerCfgTrue);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "start did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    m_result = m_pNSProviderServiceInstance->registerTopic(EMPTY_STRING);
    ASSERT_EQ(OIC::Service::NSResult::FAIL,m_result)<< "registerTopic did not "
    "return success. Expected: FAIL. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see NSResult registerTopic(const std::string &topicName)
 * @objective Test 'unregisterTopic' function with positive basic way
 * @target NSResult unregisterTopic(const std::string &topicName)
 * @test_data topicName = "TEST_TOPIC_1"
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 *                  4. Perform registerTopic() API
 * @procedure Perform unregisterTopic() API
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, UnregisterTopic_SRC_P)
{
    m_result = m_pNSProviderServiceInstance->start(m_providerCfgTrue);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "start did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    m_result = m_pNSProviderServiceInstance->registerTopic(TEST_TOPIC_1);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "registerTopic did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    m_result = m_pNSProviderServiceInstance->unregisterTopic(TEST_TOPIC_1);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "unregisterTopic did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-29
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see NSResult registerTopic(const std::string &topicName)
 * @objective Test 'getRegisteredTopicList' function with positive basic way
 * @target NSTopicsList *getRegisteredTopicList()
 * @test_data None
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 *                  4. Perform registerTopic() API
 * @procedure Perform getRegisteredTopicList() API
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return NSTopicsList
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, GetRegisteredTopicList_SRC_P)
{
    m_result = m_pNSProviderServiceInstance->start(m_providerCfgTrue);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "start did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    m_result = m_pNSProviderServiceInstance->registerTopic(TEST_TOPIC_1);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "registerTopic did not "
    "return success. Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    NSTopicsList* topicList = nullptr;
    topicList = m_pNSProviderServiceInstance->getRegisteredTopicList();
    ASSERT_NE(nullptr,topicList)<< "getRegisteredTopicList does not return topic list";
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @objective Test 'NSConsumer()' constructor with positive basic way
 * @target NSConsumer()
 * @test_data None
 * @pre_condition Perform Configure() and stopPresence() API
 * @procedure Perform NSConsumer() API
 * @post_condition None
 * @expected The API should return NSConsumer instance
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, ConsumerConstructor_SRC_P)
{
    m_pNSConsumer = new OIC::Service::NSConsumer();
    ASSERT_NE(nullptr,m_pNSConsumer)<< "NSConsumer instance could not be created";
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see NSConsumerService *getInstance()
 * @see void start(ProviderDiscoveredCallback providerDiscovered)
 * @see NSConsumer *getConsumer(const std::string &id)
 * @objective Test 'getConsumerId' function with positive basic way
 * @target std::string getConsumerId() const
 * @test_data None
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 *                  4. Start consumer service
 *                  5. Get NSConsumer instance from getConsumer() API of NSProviderService class
 * @procedure Perform getConsumerId() API with using that NSConsumer instance
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return consumer ID
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, GetConsumerID_SRC_P)
{
    m_pNSConsumer = m_pNSProviderHelper->getConsumer(true);
    ASSERT_NE(nullptr,m_pNSConsumer)<< "NSConsumer instance could not be created";

    m_consumerID = m_pNSConsumer->getConsumerId();
    IOTIVITYTEST_LOG(INFO, "Consumer ID: %s", m_consumerID.c_str());
    ASSERT_NE("",m_consumerID)<< "getConsumerId did not return Consumer ID";
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see NSConsumerService *getInstance()
 * @see void start(ProviderDiscoveredCallback providerDiscovered)
 * @see NSConsumer *getConsumer(const std::string &id)
 * @objective Test 'acceptSubscription' function with positive basic way with accepted status true
 * @target int acceptSubscription(bool accepted)
 * @test_data accepted true
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 *                  4. Start consumer service
 *                  5. Get NSConsumer instance from getConsumer() API of NSProviderService class
 * @procedure Perform acceptSubscription() API with using that NSConsumer instance
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return 100
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, AcceptSubscriptionAllow_SRC_P)
{
    m_pNSConsumer = m_pNSProviderHelper->getConsumer(true);
    ASSERT_NE(nullptr,m_pNSConsumer)<< "NSConsumer instance could not be created";

    m_accepted = true;
    ASSERT_EQ(PROVIDER_ACCEPT_SUCCESS,m_pNSConsumer->acceptSubscription(m_accepted))<< "acceptSubscription did not return success";
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see NSConsumerService *getInstance()
 * @see void start(ProviderDiscoveredCallback providerDiscovered)
 * @see NSConsumer *getConsumer(const std::string &id);
 * @objective Test 'acceptSubscription' function with positive basic way with accepted status false
 * @target int acceptSubscription(bool accepted)
 * @test_data accepted false
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 *                  4. Start consumer service
 *                  5. Get NSConsumer instance from getConsumer() API of NSProviderService class
 * @procedure Perform acceptSubscription() API with using that NSConsumer instance
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return 100
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, AcceptSubscriptionDeny_SRC_P)
{
    m_pNSConsumer = m_pNSProviderHelper->getConsumer(true);
    ASSERT_NE(nullptr,m_pNSConsumer)<< "NSConsumer instance could not be created";

    m_accepted = false;
    ASSERT_EQ(PROVIDER_ACCEPT_SUCCESS,m_pNSConsumer->acceptSubscription(m_accepted))<< "acceptSubscription did not return success";
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see NSConsumerService *getInstance()
 * @see void start(ProviderDiscoveredCallback providerDiscovered)
 * @see NSConsumer *getConsumer(const std::string &id)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @objective Test 'setTopic' function with positive basic way
 * @target NSResult setTopic(const std::string &topicName)
 * @test_data topicName = "TEST_TOPIC_1"
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 *                  4. Start consumer service
 *                  5. Get NSConsumer instance from getConsumer() API of NSProviderService class
 *                  6. Perform getConsumerId() API with using that NSConsumer instance
 *                  7. Perform acceptSubscription() API
 *                  8. Register one topic using registerTopic() API
 * @procedure Perform setTopic() API for that registered topic
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, SetTopic_SRC_P)
{
    m_pNSConsumer = m_pNSProviderHelper->getConsumer(true);
    ASSERT_NE(nullptr,m_pNSConsumer)<< "NSConsumer instance could not be created";

    m_accepted = true;
    ASSERT_EQ(PROVIDER_ACCEPT_SUCCESS,m_pNSConsumer->acceptSubscription(m_accepted))<< "acceptSubscription did not return success";
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    m_result = NSProviderCppHelper::s_pNSProviderService->registerTopic(TEST_TOPIC_1);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "registerTopic did "
    "not return success. Expected: OK. Actual: " <<m_pNSProviderHelper->getResultString(m_result);

    m_result = m_pNSConsumer->setTopic(TEST_TOPIC_1);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "setTopic did not return success."
    " Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see NSConsumerService *getInstance()
 * @see void start(ProviderDiscoveredCallback providerDiscovered)
 * @see NSConsumer *getConsumer(const std::string &id)
 * @see int acceptSubscription(bool accepted)
 * @objective Test 'setTopic' function with negative basic way
 * @target NSResult setTopic(const std::string &topicName)
 * @test_data topicName = "TEST_TOPIC_1"
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 *                  4. Start consumer service
 *                  5. Get NSConsumer instance from getConsumer() API of NSProviderService class
 *                  6. Perform getConsumerId() API with using that NSConsumer instance
 *                  7. Perform acceptSubscription() API
 * @procedure Perform setTopic() API without registering the topic
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should not return OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, SetTopic_USV_N)
{
    m_pNSConsumer = m_pNSProviderHelper->getConsumer(true);
    ASSERT_NE(nullptr,m_pNSConsumer)<< "NSConsumer instance could not be created";

    m_consumerID = m_pNSConsumer->getConsumerId();
    IOTIVITYTEST_LOG(INFO, "Consumer ID: %s", m_consumerID.c_str());
    ASSERT_NE("",m_consumerID)<< "getConsumerId did not return Consumer ID";

    m_accepted = true;
    ASSERT_EQ(PROVIDER_ACCEPT_SUCCESS,m_pNSConsumer->acceptSubscription(m_accepted))<< "acceptSubscription did not return success";
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    m_result = m_pNSConsumer->setTopic(TEST_TOPIC_1);
    ASSERT_NE(OIC::Service::NSResult::OK,m_result)<< "setTopic did not return success."
    " Expected: Not OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see NSConsumerService *getInstance()
 * @see void start(ProviderDiscoveredCallback providerDiscovered)
 * @see NSConsumer *getConsumer(const std::string &id)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSResult setTopic(const std::string &topicName)
 * @objective Test 'unsetTopic' function with positive basic way
 * @target NSResult unsetTopic(const std::string &topicName)
 * @test_data topicName = "TEST_TOPIC_1"
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 *                  4. Start consumer service
 *                  5. Get NSConsumer instance from getConsumer() API of NSProviderService class
 *                  6. Perform getConsumerId() API with using that NSConsumer instance
 *                  7. Perform acceptSubscription() API
 *                  8. Register one topic using registerTopic() API
 *                  9. Perform setTopic() API for that registered topic
 * @procedure Perform unsetTopic() API for that set topic
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, UnsetTopic_SRC_P)
{
    m_pNSConsumer = m_pNSProviderHelper->getConsumer(true);
    ASSERT_NE(nullptr,m_pNSConsumer)<< "NSConsumer instance could not be created";

    m_consumerID = m_pNSConsumer->getConsumerId();
    IOTIVITYTEST_LOG(INFO, "Consumer ID: %s", m_consumerID.c_str());
    ASSERT_NE("",m_consumerID)<< "getConsumerId did not return Consumer ID";

    m_accepted = true;
    ASSERT_EQ(PROVIDER_ACCEPT_SUCCESS,m_pNSConsumer->acceptSubscription(m_accepted))<< "acceptSubscription did not return success";
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    m_result = NSProviderCppHelper::s_pNSProviderService->registerTopic(TEST_TOPIC_1);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "registerTopic did "
    "not return success. Expected: OK. Actual: " <<m_pNSProviderHelper->getResultString(m_result);

    m_result = m_pNSConsumer->setTopic(TEST_TOPIC_1);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "setTopic did not return success."
    " Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    m_result = m_pNSConsumer->unsetTopic(TEST_TOPIC_1);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "unsetTopic did not return success."
    " Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see NSConsumerService *getInstance()
 * @see void start(ProviderDiscoveredCallback providerDiscovered)
 * @see NSConsumer *getConsumer(const std::string &id)
 * @see int acceptSubscription(bool accepted)
 * @objective Test 'unsetTopic' function with negative basic way
 * @target NSResult unsetTopic(const std::string &topicName)
 * @test_data topicName = "TEST_TOPIC_1"
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 *                  4. Start consumer service
 *                  5. Get NSConsumer instance from getConsumer() API of NSProviderService class
 *                  6. Perform getConsumerId() API with using that NSConsumer instance
 *                  7. Perform acceptSubscription() API
 * @procedure Perform unsetTopic() API without registering and setting topic
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should not return OK
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, UnsetTopic_USV_N)
{
    m_pNSConsumer = m_pNSProviderHelper->getConsumer(true);
    ASSERT_NE(nullptr,m_pNSConsumer)<< "NSConsumer instance could not be created";

    m_consumerID = m_pNSConsumer->getConsumerId();
    IOTIVITYTEST_LOG(INFO, "Consumer ID: %s", m_consumerID.c_str());
    ASSERT_NE("",m_consumerID)<< "getConsumerId did not return Consumer ID";

    m_accepted = true;
    ASSERT_EQ(PROVIDER_ACCEPT_SUCCESS,m_pNSConsumer->acceptSubscription(m_accepted))<< "acceptSubscription did not return success";
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    m_result = m_pNSConsumer->unsetTopic(TEST_TOPIC_1);
    ASSERT_NE(OIC::Service::NSResult::OK,m_result)<< "unsetTopic did not return success."
    " Expected: Not OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see NSConsumerService *getInstance()
 * @see void start(ProviderDiscoveredCallback providerDiscovered)
 * @see NSConsumer *getConsumer(const std::string &id)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSResult setTopic(const std::string &topicName)
 * @objective Test 'getConsumerTopicList' function with positive basic way
 * @target NSTopicsList *getConsumerTopicList()
 * @test_data topicName = "TEST_TOPIC_1"
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSProviderService instance using getInstance() method
 *                  3. Perform start() API with subcontrollability true
 *                  4. Start consumer service
 *                  5. Get NSConsumer instance from getConsumer() API of NSProviderService class
 *                  6. Perform getConsumerId() API with using that NSConsumer instance
 *                  7. Perform acceptSubscription() API
 *                  8. Register one topic using registerTopic() API
 *                  9. Perform setTopic() API for that registered topic
 * @procedure Perform getConsumerTopicList() API
 * @post_condition Perform stop() API using NSProviderService instance
 * @expected The API should return NSTopicsList instance
 **/
#if defined(__LINUX__)
TEST_F(NSProviderCppTest_btc, GetConsumerTopicList_SRC_P)
{
    m_pNSConsumer = m_pNSProviderHelper->getConsumer(true);
    ASSERT_NE(nullptr,m_pNSConsumer)<< "NSConsumer instance could not be created";

    m_consumerID = m_pNSConsumer->getConsumerId();
    IOTIVITYTEST_LOG(INFO, "Consumer ID: %s", m_consumerID.c_str());
    ASSERT_NE("",m_consumerID)<< "getConsumerId did not return Consumer ID";

    m_accepted = true;
    ASSERT_EQ(PROVIDER_ACCEPT_SUCCESS,m_pNSConsumer->acceptSubscription(m_accepted))<< "acceptSubscription did not return success";
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    m_result = NSProviderCppHelper::s_pNSProviderService->registerTopic(TEST_TOPIC_1);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "registerTopic did "
    "not return success. Expected: OK. Actual: " <<m_pNSProviderHelper->getResultString(m_result);

    m_result = m_pNSConsumer->setTopic(TEST_TOPIC_1);
    ASSERT_EQ(OIC::Service::NSResult::OK,m_result)<< "setTopic did not return success."
    " Expected: OK. Actual: " << m_pNSProviderHelper->getResultString(m_result);

    OIC::Service::NSTopicsList* topicList = nullptr;
    topicList = m_pNSConsumer->getConsumerTopicList();
    ASSERT_NE(nullptr,topicList)<< "Consumer topic list was not found";
}
#endif
