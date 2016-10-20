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

#include "NSConsumerCppHelper.h"

class NSConsumerCppTest_btc: public ::testing::Test
{
public:
    NSConsumerCppHelper* m_pNSConsumerHelper;
    NSConsumerService* m_pNSConsumerServiceInstance;
    OIC::Service::NSProvider* m_pNSProvider;
    OIC::Service::NSResult m_result;
    string m_providerID = "";

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pNSConsumerHelper = NSConsumerCppHelper::getInstance();

        m_pNSConsumerServiceInstance = nullptr;
        m_pNSConsumerServiceInstance = NSConsumerService::getInstance();
        ASSERT_NE(nullptr,m_pNSConsumerServiceInstance)<< "NSConsumerService instance could not be found";
        m_pNSProvider = nullptr;
        m_providerID = "";

        IOTIVITYTEST_LOG(INFO, "SetUp called");
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();

        m_pNSConsumerServiceInstance->stop();

        CommonUtil::killApp(PROVIDER_SIMULATOR);
        CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

        IOTIVITYTEST_LOG(INFO, "TearDown called");
    }

    static void onProviderDiscovered(OIC::Service::NSProvider* provider)
    {
        IOTIVITYTEST_LOG(INFO, "%s is called", __func__);
    }

    static void onProviderStateChangedCb(OIC::Service::NSProviderState state)
    {
        IOTIVITYTEST_LOG(INFO, "%s is called", __func__);
    }

    static void onNotificationPostedCb(OIC::Service::NSMessage *notification)
    {
        IOTIVITYTEST_LOG(INFO, "%s is called", __func__);
    }

    static void onNotificationSyncCb(OIC::Service::NSSyncInfo *sync)
    {
        IOTIVITYTEST_LOG(INFO, "%s is called", __func__);
    }
};

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @objective Test 'getInstance' function with positive basic way
 * @target NSConsumerService *getInstance()
 * @test_data None
 * @pre_condition Perform Configure() and stopPresence() API
 * @procedure Perform getInstance() API
 * @post_condition None
 * @expected The API should return NSConsumerService instance
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, ConsumerServiceGetInstance_SRC_P)
{
    ASSERT_NE(nullptr,NSConsumerService::getInstance())<< "getInstance does not return success";
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSConsumerService *getInstance()
 * @objective Test 'start' function with positive basic way
 * @target void start(ProviderDiscoveredCallback providerDiscovered)
 * @test_data providerDiscovered callback to be called when provider is discovered
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSConsumerService instance using getInstance() method
 * @procedure Perform start() API
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected The API should not throw any exceptions
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, ConsumerServiceStart_SRC_P)
{
    try
    {
        m_pNSConsumerServiceInstance->start(onProviderDiscovered);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling start. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSConsumerService *getInstance()
 * @see void start(ProviderDiscoveredCallback providerDiscovered)
 * @objective Test 'stop' function with positive basic way
 * @target void stop()
 * @test_data None
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSConsumerService instance using getInstance() method
 *                  3. Perform start() API
 * @procedure Perform stop() API using NSConsumerService instance
 * @post_condition None
 * @expected The API should not throw any exceptions
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, ConsumerServiceStop_SRC_P)
{
    try
    {
        m_pNSConsumerServiceInstance->start(onProviderDiscovered);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling start. Exception is: " + string(e.what()));
    }

    try
    {
        m_pNSConsumerServiceInstance->stop();
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling stop. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSConsumerService *getInstance()
 * @see void start(ProviderDiscoveredCallback providerDiscovered)
 * @objective Test 'rescanProvider' function with positive basic way
 * @target void rescanProvider()
 * @test_data None
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Get NSConsumerService instance using getInstance() method
 *                  3. Perform start() API
 * @procedure Perform rescanProvider() API
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected The API should not throw any exceptions
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, RescanProvider_SRC_P)
{
    try
    {
        m_pNSConsumerServiceInstance->start(onProviderDiscovered);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling start. Exception is: " + string(e.what()));
    }

    try
    {
        m_pNSConsumerServiceInstance->rescanProvider();
    }
    catch (exception &e)
    {
        SET_FAILURE(
                "Exception occurs while calling rescanProvider. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-28
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @objective Test 'NSProvider()' constructor with positive basic way
 * @target NSProvider()
 * @test_data None
 * @pre_condition Perform Configure() and stopPresence() API
 * @procedure Perform NSProvider() API
 * @post_condition None
 * @expected The API should return NSProvider instance
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, ProviderConstructor_SRC_P)
{
    m_pNSProvider = new OIC::Service::NSProvider();
    ASSERT_NE(nullptr,m_pNSProvider)<< "NSProvider instance could not be created";
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
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSProvider *getProvider(const std::string &id)
 * @objective Test 'getProviderId' function with positive basic way
 * @target std::string getProviderId()
 * @test_data None
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start provider with subcontrollability true
 *                  3. Get NSConsumerService instance using getInstance() method
 *                  4. Perform start() API
 *                  5. Accept subscription from Provider side and register one topic in provider
 *                  6. Get NSProvider() instance using getProvider() API of NSConsumerService class
 * @procedure Perform getProviderId() API using NSProvider instance
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected Provider ID should be returned
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, GetProviderID_SRC_P)
{
    m_pNSProvider = m_pNSConsumerHelper->getProvider(true);
    ASSERT_NE(nullptr,m_pNSProvider)<< "NSProvider instance could not be found";

    m_providerID = m_pNSProvider->getProviderId();
    IOTIVITYTEST_LOG(INFO, "Provider ID: %s", m_providerID.c_str());
    ASSERT_NE("",m_providerID)<< "getProviderId did not return provider ID";
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
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSProvider *getProvider(const std::string &id)
 * @objective Test 'getTopicList' function with positive basic way
 * @target NSTopicsList *getTopicList()
 * @test_data None
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start provider with subcontrollability true
 *                  3. Get NSConsumerService instance using getInstance() method
 *                  4. Perform start() API
 *                  5. Accept subscription from Provider side and register one topic in provider
 *                  6. Get NSProvider() instance using getProvider() API of NSConsumerService class
 * @procedure Perform getTopicList() API using NSProvider instance
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected Returned list should not be null
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, GetTopicList_SRC_P)
{
    m_pNSProvider = m_pNSConsumerHelper->getProvider(true);
    ASSERT_NE(nullptr,m_pNSProvider)<< "NSProvider instance could not be found";

    ASSERT_NE(NULL,m_pNSProvider->getTopicList())<< "getTopicList did not return list of topics";
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
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSProvider *getProvider(const std::string &id)
 * @see NSTopicsList *getTopicList()
 * @objective Test 'updateTopicList' function with negative basic way
 * @target NSResult updateTopicList(NSTopicsList *topicList)
 * @test_data topicList list to use when updating
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start provider with subcontrollability true
 *                  3. Get NSConsumerService instance using getInstance() method
 *                  4. Perform start() API
 *                  5. Accept subscription from Provider side and register one topic in provider
 *                  6. Get NSProvider() instance using getProvider() API of NSConsumerService class
 *                  7. Perform getTopicList() API using NSProvider instance
 * @procedure Perform updateTopicList() API using the found topic list
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected API should not return OK
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, UpdateTopicList_USV_N)
{
    m_pNSProvider = m_pNSConsumerHelper->getProvider(true);
    ASSERT_NE(nullptr,m_pNSProvider)<< "NSProvider instance could not be found";

    NSTopicsList* nsTopicList = m_pNSProvider->getTopicList();
    ASSERT_NE(NULL,nsTopicList)<< "getTopicList did not return list of topics";

    m_result = m_pNSProvider->updateTopicList(nsTopicList);

    ASSERT_NE(OIC::Service::NSResult::OK,m_result)<< "updateTopicList did not return success. Expected: Not OK. Actual: "
    << m_pNSConsumerHelper->getResultString(m_result);
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
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSProvider *getProvider(const std::string &id)
 * @see NSTopicsList *getTopicList()
 * @objective Test 'updateTopicList' function with negative basic way
 * @target NSResult updateTopicList(NSTopicsList *topicList)
 * @test_data topicList null
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start provider with subcontrollability true
 *                  3. Get NSConsumerService instance using getInstance() method
 *                  4. Perform start() API
 *                  5. Accept subscription from Provider side and register one topic in provider
 *                  6. Get NSProvider() instance using getProvider() API of NSConsumerService class
 *                  7. Perform getTopicList() API using NSProvider instance
 * @procedure Perform updateTopicList() API using null value in place of topic list
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected API should not return OK
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, UpdateTopicList_NV_N)
{
    m_pNSProvider = m_pNSConsumerHelper->getProvider(true);
    ASSERT_NE(nullptr,m_pNSProvider)<< "NSProvider instance could not be found";

    NSTopicsList* nsTopicList = m_pNSProvider->getTopicList();
    ASSERT_NE(NULL,nsTopicList)<< "getTopicList did not return list of topics";

    m_result = m_pNSProvider->updateTopicList(NULL);

    ASSERT_NE(OIC::Service::NSResult::OK,m_result)<< "updateTopicList did not return success. Expected: Not OK. Actual: "
    << m_pNSConsumerHelper->getResultString(m_result);
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
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSProvider *getProvider(const std::string &id)
 * @objective Test 'getProviderState' function with positive basic way
 * @target NSProviderState getProviderState() const
 * @test_data None
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start provider with subcontrollability true
 *                  3. Get NSConsumerService instance using getInstance() method
 *                  4. Perform start() API
 *                  5. Accept subscription from Provider side and register one topic in provider
 *                  6. Get NSProvider() instance using getProvider() API of NSConsumerService class
 * @procedure Perform getProviderState() API
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected Returned state should not be STOPPED
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, GetProviderState_SRC_P)
{
    m_pNSProvider = m_pNSConsumerHelper->getProvider(true);
    ASSERT_NE(nullptr,m_pNSProvider)<< "NSProvider instance could not be found";

    OIC::Service::NSProviderState state;

    state = m_pNSProvider->getProviderState();

    ASSERT_NE(OIC::Service::NSProviderState::STOPPED,state)<< "getProviderState did not return correct state";
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
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSProvider *getProvider(const std::string &id)
 * @objective Test 'getProviderSubscribedState' function with positive basic way
 * @target NSProviderSubscribedState getProviderSubscribedState() const
 * @test_data None
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start provider with subcontrollability true
 *                  3. Get NSConsumerService instance using getInstance() method
 *                  4. Perform start() API
 *                  5. Accept subscription from Provider side and register one topic in provider
 *                  6. Get NSProvider() instance using getProvider() API of NSConsumerService class
 * @procedure Perform getProviderSubscribedState() API
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected Returned subscribed state should be SUBSCRIBED
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, GetProviderSubscribedState_SRC_P)
{
    m_pNSProvider = m_pNSConsumerHelper->getProvider(true);
    ASSERT_NE(nullptr,m_pNSProvider)<< "NSProvider instance could not be found";

    OIC::Service::NSProviderSubscribedState subscribedState;

    subscribedState = m_pNSProvider->getProviderSubscribedState();

    ASSERT_EQ(OIC::Service::NSProviderSubscribedState::SUBSCRIBED,subscribedState)<< "getProviderSubscribedState did not return correct state";
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
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSProvider *getProvider(const std::string &id)
 * @objective Test 'subscribe' function with positive basic way
 * @target void subscribe()
 * @test_data None
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start provider with subcontrollability false
 *                  3. Get NSConsumerService instance using getInstance() method
 *                  4. Perform start() API
 *                  5. Get NSProvider() instance using getProvider() API of NSConsumerService class
 * @procedure Perform subscribe() API
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected Exception should not be thrown
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, Subscribe_SRC_P)
{
    m_pNSProvider = m_pNSConsumerHelper->getProvider(false);
    ASSERT_NE(nullptr,m_pNSProvider)<< "NSProvider instance could not be found";

    try
    {
        m_pNSProvider->subscribe();
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling subscribe. Exception is: " + string(e.what()));
    }
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
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSProvider *getProvider(const std::string &id)
 * @objective Test 'isSubscribed' function with positive basic way
 * @target bool isSubscribed()
 * @test_data None
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start provider with subcontrollability true
 *                  3. Get NSConsumerService instance using getInstance() method
 *                  4. Perform start() API
 *                  5. Accept subscription from Provider side and register one topic in provider
 *                  6. Get NSProvider() instance using getProvider() API of NSConsumerService class
 * @procedure Perform isSubscribed() API
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected True should be returned
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, IsSubscribed_SRC_P)
{
    m_pNSProvider = m_pNSConsumerHelper->getProvider(true);
    ASSERT_NE(nullptr,m_pNSProvider)<< "NSProvider instance could not be found";

    ASSERT_EQ(true,m_pNSProvider->isSubscribed())<< "correct subscribed state is not found";
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
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSProvider *getProvider(const std::string &id)
 * @objective Test 'sendSyncInfo' function with positive basic way
 * @target void sendSyncInfo(uint64_t messageId, NSSyncInfo::NSSyncType type)
 * @test_data       1. messageId 1
 *                  2. NSSyncType NS_SYNC_READ
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start provider with subcontrollability true
 *                  3. Get NSConsumerService instance using getInstance() method
 *                  4. Perform start() API
 *                  5. Accept subscription from Provider side and register one topic in provider
 *                  6. Get NSProvider() instance using getProvider() API of NSConsumerService class
 * @procedure Perform sendSyncInfo() API
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected Exception should not be thrown
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, SendSyncInfo_SRC_P)
{
    m_pNSProvider = m_pNSConsumerHelper->getProvider(true);
    ASSERT_NE(nullptr,m_pNSProvider)<< "NSProvider instance could not be found";

    uint64_t messageId = 1;
    OIC::Service::NSSyncInfo::NSSyncType type = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ;

    try
    {
        m_pNSProvider->sendSyncInfo(messageId, type);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling sendSyncInfo. Exception is: " + string(e.what()));
    }
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
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSProvider *getProvider(const std::string &id)
 * @objective Test 'setListener' function with positive basic way
 * @target void setListener(ProviderStateCallback stateHandle,
 *                                MessageReceivedCallback messageHandle,
 *                                SyncInfoReceivedCallback syncHandle)
 * @test_data       1. stateHandle ProviderStateCallback callback.
 *                  2. messageHandle MessageReceivedCallback callback.
 *                  3. syncHandle SyncInfoReceivedCallback callback.
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start provider with subcontrollability true
 *                  3. Get NSConsumerService instance using getInstance() method
 *                  4. Perform start() API
 *                  5. Accept subscription from Provider side and register one topic in provider
 *                  6. Get NSProvider() instance using getProvider() API of NSConsumerService class
 * @procedure Perform setListener() API
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected Exception should not be thrown
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, SetListener_SRC_P)
{
    m_pNSProvider = m_pNSConsumerHelper->getProvider(true);
    ASSERT_NE(nullptr,m_pNSProvider)<< "NSProvider instance could not be found";

    try
    {
        m_pNSProvider->setListener(onProviderStateChangedCb, onNotificationPostedCb, onNotificationSyncCb);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling sendSyncInfo. Exception is: " + string(e.what()));
    }
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
 * @see NSProviderService *getInstance()
 * @see NSResult start(ProviderConfig config)
 * @see int acceptSubscription(bool accepted)
 * @see NSResult registerTopic(const std::string &topicName)
 * @see NSProvider *getProvider(const std::string &id)
 * @objective Test 'setListener' function with positive basic way
 * @target void setListener(ProviderStateCallback stateHandle,
 *                                MessageReceivedCallback messageHandle,
 *                                SyncInfoReceivedCallback syncHandle)
 * @test_data       1. stateHandle null
 *                  2. messageHandle null
 *                  3. syncHandle null
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start provider with subcontrollability true
 *                  3. Get NSConsumerService instance using getInstance() method
 *                  4. Perform start() API
 *                  5. Accept subscription from Provider side and register one topic in provider
 *                  6. Get NSProvider() instance using getProvider() API of NSConsumerService class
 * @procedure Perform setListener() API using null value as parameters
 * @post_condition Perform stop() API using NSConsumerService instance
 * @expected Exception should not be thrown
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerCppTest_btc, SetListener_NV_P)
{
    m_pNSProvider = m_pNSConsumerHelper->getProvider(true);
    ASSERT_NE(nullptr,m_pNSProvider)<< "NSProvider instance could not be found";

    try
    {
        m_pNSProvider->setListener(NULL,NULL,NULL);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling sendSyncInfo. Exception is: " + string(e.what()));
    }
}
#endif
