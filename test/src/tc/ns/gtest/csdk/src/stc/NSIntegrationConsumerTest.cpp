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

#include "../../include/NSConsumerHelper.h"

class NSIntegrationConsumerTest_stc: public ::testing::Test
{
protected:
    NSConsumerHelper* m_pNSHelper;
    NSTopicLL* m_pTopicLL;
    NSCommonHelper* m_pCommonHelper;
    string m_testAppLog;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pNSHelper = NSConsumerHelper::getInstance();
        NSConsumerHelper::s_isDiscovered = false;
        NSConsumerHelper::s_isTopicChanged = false;
        NSConsumerHelper::s_isConsumerAllowed = false;
        NSConsumerHelper::s_isNotificationPosted = false;
        NSConsumerHelper::s_pProvider = nullptr;
        m_pTopicLL = nullptr;

        m_testAppLog = "";
        m_pCommonHelper = NSCommonHelper::getInstance();
        CommonUtil::launchApp(PROVIDER_APP_OPTION);
        CommonUtil::waitInSecond(WAIT_TIME_MIN);
        m_pCommonHelper->initPipe(true);
    }

    virtual void TearDown()
    {
        m_pCommonHelper->closePipe();
        CommonUtil::killApp(PROVIDER_APP);
        CommonUtil::waitInSecond(WAIT_TIME_MIN);

        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @objective Test if provider is discovered from consumer side
 * @target NSResult NSStartConsumer(NSConsumerConfig config)
 * @test_data config consumer configuration
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability false
 * @procedure       1. Perform NSStartConsumer() API with config
 *                  2. Check if onProviderChanged Callback is called
 * @post_condition None
 * @expected Callback should be called and provider should be found in callback
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationConsumerTest_stc, StartConsumerAndDiscoverProvider_SQV_CV_P)
{
    m_pCommonHelper->inputMenu("2");
    ASSERT_EQ(NSStartConsumer(m_pNSHelper->getConsumerConfig()), NS_OK)<< "NSStartConsumer did not return success";
    IOTIVITYTEST_LOG(INFO, "Waiting for provider");
    CommonUtil::waitInSecond(WAIT_TIME_MAX);
    m_pCommonHelper->closePipe();
    m_testAppLog = m_pCommonHelper->getAppLog();
    IOTIVITYTEST_LOG(INFO, "Provider Log: %s", m_testAppLog.c_str());

    ASSERT_EQ(NSConsumerHelper::s_isDiscovered,true)<< "onProviderChanged callback was not called";
    ASSERT_NE(NSConsumerHelper::s_pProvider,nullptr)<< "Provider object was not found in callback";
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @objective Test if provider is discovered using 'NSRescanProvider()' API from consumer side
 * @target NSResult NSStartConsumer(NSConsumerConfig config)
 * @target NSResult NSRescanProvider()
 * @test_data config consumer configuration
 * @pre_condition Perform Configure() and stopPresence() API
 * @procedure       1. Perform NSStartConsumer() API with config
 *                  2. Check if onProviderChanged Callback is not called
 *                  3. Start Provider with subcontrollability false
 *                  4. Perform NSRescanProvider() API to discover provider now
 *                  5. Check if onProviderChanged Callback is called
 * @post_condition None
 * @expected        1. At step 2, callback should not be called
 *                  2. At step 5, callback should be called and provider should be found
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationConsumerTest_stc, StartConsumerAndRescanProvider_SQV_CV_P)
{
    ASSERT_EQ(NSStartConsumer(m_pNSHelper->getConsumerConfig()), NS_OK)<< "NSStartConsumer did not return success";
    IOTIVITYTEST_LOG(INFO, "Waiting for provider when there is no provider");
    CommonUtil::waitInSecond(WAIT_TIME_MAX);
    ASSERT_EQ(NSConsumerHelper::s_isDiscovered,false) << "onProviderChanged callback should not be called as there is no provider";
    ASSERT_EQ(NSConsumerHelper::s_pProvider,nullptr) << "Provider object should not be found as there is no provider";

    m_pCommonHelper->inputMenu("2");
    ASSERT_EQ(NSRescanProvider(), NS_OK) << "NSRescanProvider did not return success";
    IOTIVITYTEST_LOG(INFO, "Rescanning provider");
    CommonUtil::waitInSecond(WAIT_TIME_MAX);
    m_pCommonHelper->closePipe();
    m_testAppLog = m_pCommonHelper->getAppLog();
    IOTIVITYTEST_LOG(INFO, "TESTAPP LOG: %s",m_testAppLog.c_str());

    ASSERT_EQ(NSConsumerHelper::s_isDiscovered,true) << "onProviderChanged callback was not called";
    ASSERT_NE(NSConsumerHelper::s_pProvider,nullptr) << "Provider object was not found in callback";
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @objective Test if consumer can subscribe to provider properly
 * @target NSResult NSSubscribe(const char * providerId)
 * @test_data providerId ID of the discovered provider
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability false
 *                  3. Perform NSStartConsumer() API with config
 *                  4. Check if provider is found
 * @procedure       1. Perform NSSubscribe() API with found provider's id
 *                  2. Check if onProviderChanged Callback is called with NSProviderState NS_ALLOW
 * @post_condition None
 * @expected onProviderChanged Callback should be called with NSProviderState NS_ALLOW
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationConsumerTest_stc, SubscribeProvider_SQV_CV_P)
{
    m_pCommonHelper->inputMenu("2");
    NSProvider *provider = m_pNSHelper->getProvider();

    ASSERT_NE(provider,nullptr)<< "Provider object was not found in callback";

    ASSERT_EQ(NS_OK, NSSubscribe(provider->providerId))<< "NSSubscribe did not return success";

    IOTIVITYTEST_LOG(INFO, "Waiting for subscribe");

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    m_pCommonHelper->closePipe();
    m_testAppLog = m_pCommonHelper->getAppLog();
    IOTIVITYTEST_LOG(INFO, "Provider Log: %s", m_testAppLog.c_str());

    ASSERT_EQ(NSConsumerHelper::s_isConsumerAllowed,true)<< "Subscribe was not allowed by provider";
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSAcceptSubscription(const char * consumerId, bool accepted)
 * @see NSResult NSProviderRegisterTopic(const char * topicName)
 * @see NSResult NSProviderSetConsumerTopic(const char * consumerId, const char * topicName)
 * @see NSResult NSProviderSendSyncInfo(uint64_t messageId, NSSyncType type)
 * @objective Test if consumer can send sync info to provider
 * @target NSResult NSConsumerSendSyncInfo(
 *       const char * providerId, uint64_t messageId, NSSyncType type);
 * @test_data providerId ID of the discovered provider
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability true
 *                  3. Perform NSStartConsumer() API with config
 *                  4. Check if provider is found and consumer is allowed
 *                  5. Register one topic from provider side
 *                  6. Set that topic from provider side
 *                  7. Send notification from provider side
 *                  8. Check if consumer onNotificationPosted callback is called
 * @procedure       1. Call NSConsumerSendSyncInfo() API using NSMessage object found in callback
 *                      and check if API returns OK
 *                  2. Check if syncInfoCallback is called in provider side
 * @post_condition None
 * @expected NSConsumerSendSyncInfo() API will return OK and provider's syncInfoCallback callback is called
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationConsumerTest_stc, SendSyncInfo_SQV_CV_P)
{
    m_pCommonHelper->inputMenu("1");

    NSConsumerConfig config = m_pNSHelper->getConsumerConfig();

    NSStartConsumer(config);

    IOTIVITYTEST_LOG(INFO, "Waiting for discovering provider");

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    IOTIVITYTEST_LOG(INFO, "Waiting for allowing subscribe");

    m_pCommonHelper->inputMenu("4");

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    NSProvider *provider = NSConsumerHelper::s_pProvider;

    ASSERT_NE(provider,nullptr)<< "Provider object was not found in callback";

    ASSERT_EQ(NSConsumerHelper::s_isConsumerAllowed,true)<< "Subscribe was not allowed by provider";

    m_pCommonHelper->inputMenu("8");
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);
    m_pCommonHelper->inputMenu("10");
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);
    m_pCommonHelper->inputMenu("6");
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    ASSERT_EQ(true,NSConsumerHelper::s_isNotificationPosted)<< "messageCb was not called";

    uint64_t messageId = 1;
    NSSyncType type = NS_SYNC_READ;
    NSMessage* notification = m_pNSHelper->getNotificationMessage();
    m_pCommonHelper->loggerReader();
    ASSERT_EQ(NS_OK,NSConsumerSendSyncInfo(notification->providerId, notification->messageId,
                    type))<< "NSConsumerSendSyncInfo did not return success";
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);
    m_pCommonHelper->loggerReader();

    IOTIVITYTEST_LOG(INFO, "Checking if callback called in provider");
    m_pCommonHelper->closePipe();
    m_testAppLog = m_pCommonHelper->getAppLog();
    IOTIVITYTEST_LOG(INFO, "Provider Log: %s", m_testAppLog.c_str());
    int findResult = m_testAppLog.find("syncInfoCallback Called");
    ASSERT_NE(-1 , findResult)<< "syncInfoCallback was not called in provider app";
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @objective Test if NSConsumerGetProvider() API returns the correct provider
 * @target NSProvider * NSConsumerGetProvider(const char * providerId)
 * @test_data providerId ID of the discovered provider
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability false
 *                  3. Perform NSStartConsumer() API with config
 *                  4. Check if provider is found
 * @procedure       1. Perform NSConsumerGetProvider() API with found provider's id
 *                  2. Check if returned provider's ID and previously received provider's ID matches
 * @post_condition None
 * @expected The two IDs match
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationConsumerTest_stc, StartConsumerAndGetProvider_GSRV_P)
{
    m_pCommonHelper->inputMenu("2");

    NSProvider *provider = m_pNSHelper->getProvider();

    ASSERT_NE(provider,nullptr)<< "Provider object was not found in callback";

    NSProvider *receivedProvider = nullptr;
    receivedProvider = NSConsumerGetProvider(provider->providerId);

    ASSERT_STREQ(receivedProvider->providerId,provider->providerId)<< "NSConsumerGetProvider did not return the same provider";

    m_pCommonHelper->closePipe();
    m_testAppLog = m_pCommonHelper->getAppLog();
    IOTIVITYTEST_LOG(INFO, "Provider Log: %s", m_testAppLog.c_str());
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSSubscribe(const char * providerId)
 * @see NSResult NSProviderRegisterTopic(const char * topicName)
 * @objective Test if consumer can get topic list properly after adding topics in provider
 * @target NSTopicLL * NSConsumerGetTopicList(const char * providerId)
 * @test_data providerId ID of the discovered provider
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability false
 *                  3. Perform NSStartConsumer() API with config
 *                  4. Check if provider is found
 *                  5. Perform NSSubscribe() API and check if consumer is allowed
 * @procedure       1. Register two topics in provider side
 *                  2. Check if consumer's callback is called
 *                  3. Perform NSConsumerGetTopicList() API to get topiclist
 *                  4. Check if added topic exists in the received topic list
 * @post_condition None
 * @expected Provider's added topic is found
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationConsumerTest_stc, AddTopicAndGetTopicList_GSRV_CV_P)
{
    m_pCommonHelper->inputMenu("2");

    NSProvider *provider = m_pNSHelper->getProvider();

    ASSERT_NE(provider,nullptr)<< "Provider object was not found in callback";

    ASSERT_EQ(NS_OK, NSSubscribe(provider->providerId))<< "NSSubscribe did not return success";

    IOTIVITYTEST_LOG(INFO, "Waiting for subscribe");

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    ASSERT_EQ(NSConsumerHelper::s_isConsumerAllowed,true)<< "Subscribe was not allowed by provider";

    IOTIVITYTEST_LOG(INFO, "Waiting for adding topic in provider");

    m_pCommonHelper->inputMenu("8");
    CommonUtil::waitInSecond(WAIT_TIME_MIN);
    m_pCommonHelper->inputMenu("8");
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    ASSERT_EQ(NSConsumerHelper::s_isTopicChanged,true)<< "Callback was not called after adding topic in provider";

    m_pTopicLL = NSConsumerGetTopicList(provider->providerId);

    ASSERT_NE(m_pTopicLL,nullptr)<< "Topic list was not found by consumer";

    ASSERT_EQ(m_pNSHelper->printTopicList(m_pTopicLL),true)<< "Added and received topic did not match";

    m_pCommonHelper->closePipe();
    m_testAppLog = m_pCommonHelper->getAppLog();
    IOTIVITYTEST_LOG(INFO, "Provider Log: %s", m_testAppLog.c_str());
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSSubscribe(const char * providerId)
 * @see NSResult NSProviderRegisterTopic(const char * topicName)
 * @see NSResult NSProviderUnregisterTopic(const char * topicName)
 * @objective Test if consumer can update and get topic list properly after deleting previously added
 *      topics in provider
 * @target NSResult NSConsumerUpdateTopicList(const char * providerId, NSTopicLL * topics)
 * @test_data       1. providerId ID of the discovered provider
 *                  2. NSTopicLL topic list to be updated
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability false
 *                  3. Perform NSStartConsumer() API with config
 *                  4. Check if provider is found
 *                  5. Perform NSSubscribe() API and check if consumer is allowed
 * @procedure       1. Register two topics in provider side
 *                  2. Check if consumer's callback is called
 *                  3. Perform NSConsumerGetTopicList() API to get topiclist
 *                  4. Check if added topic exists in the received topic list
 *                  5. Unregister one topic in provider side
 *                  6. Perform NSConsumerGetTopicList() API to get topiclist and check if list is not null
 *                  7. Perform NSConsumerUpdateTopicList() API using the received topic list and check if it returns OK
 *                  8. Perform NSConsumerGetTopicList() API to get topiclist again
 *                  9. Check if the unregistered topic is does not exist in the received topic list
 * @post_condition None
 * @expected Provider's unregistered topic is not found in the list
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationConsumerTest_stc, DeleteTopicAndGetTopicListAfterUpdate_GSRV_CV_P)
{
    m_pCommonHelper->inputMenu("2");

    NSProvider *provider = m_pNSHelper->getProvider();
    const char* providerID = provider->providerId;

    ASSERT_NE(provider,nullptr)<< "Provider object was not found in callback";

    ASSERT_EQ(NS_OK, NSSubscribe(providerID))<< "NSSubscribe did not return success";

    IOTIVITYTEST_LOG(INFO, "Waiting for subscribe");

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    ASSERT_EQ(NSConsumerHelper::s_isConsumerAllowed,true)<< "Subscribe was not allowed by provider";

    IOTIVITYTEST_LOG(INFO, "Waiting for adding topic in provider");

    m_pCommonHelper->inputMenu("8");
    CommonUtil::waitInSecond(WAIT_TIME_MIN);
    m_pCommonHelper->inputMenu("8");

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    ASSERT_EQ(NSConsumerHelper::s_isTopicChanged,true)<< "Callback was not called after adding topic in provider";

    m_pTopicLL = NSConsumerGetTopicList(providerID);

    ASSERT_NE(m_pTopicLL,nullptr)<< "Topic list was not found by consumer";

    ASSERT_EQ(m_pNSHelper->printTopicList(m_pTopicLL),true)<< "Added and received topic did not match";

    IOTIVITYTEST_LOG(INFO, "Waiting for deleting topic in provider");

    m_pCommonHelper->inputMenu("9");
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    m_pTopicLL = NSConsumerGetTopicList(providerID);
    ASSERT_NE(m_pTopicLL,nullptr)<< "Topic list was not found by consumer";

    ASSERT_EQ(NS_OK, NSConsumerUpdateTopicList(providerID, m_pTopicLL))<< "NSConsumerUpdateTopicList did not return success";
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    m_pTopicLL = NSConsumerGetTopicList(providerID);

    ASSERT_NE(m_pTopicLL,nullptr)<< "Topic list was not found by consumer";

    ASSERT_EQ(m_pNSHelper->printTopicList(m_pTopicLL),false) << "Deleted topic should not be found";

    m_pCommonHelper->closePipe();
    m_testAppLog = m_pCommonHelper->getAppLog();
    IOTIVITYTEST_LOG(INFO, "Provider Log: %s", m_testAppLog.c_str());
}
#endif

/**
 * @since 2016-08-16
 * @see void Configure(const PlatformConfig& config)
 * @see OCStackResult stopPresence()
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSResult NSProviderRegisterTopic(const char * topicName)
 * @see NSResult NSProviderUnregisterTopic(const char * topicName)
 * @objective Test if update returns ERROR when provider is started in True Mode
 * @target NSResult NSConsumerUpdateTopicList(const char * providerId, NSTopicLL * topics)
 * @test_data       1. providerId ID of the discovered provider
 *                  2. NSTopicLL topic list to be updated
 * @pre_condition   1. Perform Configure() and stopPresence() API
 *                  2. Start Provider with subcontrollability true
 *                  3. Perform NSStartConsumer() API with config
 *                  4. Check if provider is found and consumer is allowed
 * @procedure       1. Register two topics in provider side
 *                  2. Check if consumer's callback is called
 *                  3. Perform NSConsumerGetTopicList() API to get topiclist
 *                  4. Check if added topic exists in the received topic list
 *                  5. Unregister one topic in provider side
 *                  6. Perform NSConsumerGetTopicList() API to get topiclist and check if list is not null
 *                  7. Perform NSConsumerUpdateTopicList() API using the received topic list and check if it return NS_ERROR
 * @post_condition None
 * @expected NSConsumerUpdateTopicList() returns NS_ERROR
 **/
#if defined(__LINUX__)
TEST_F(NSIntegrationConsumerTest_stc, DeleteTopicAndUpdateInTrueMode_DSCC_N)
{
    m_pCommonHelper->inputMenu("1");

    NSConsumerConfig config = m_pNSHelper->getConsumerConfig();

    NSStartConsumer(config);

    IOTIVITYTEST_LOG(INFO, "Waiting for discovering provider");

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    IOTIVITYTEST_LOG(INFO, "Waiting for allowing subscribe");

    m_pCommonHelper->inputMenu("4");

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    NSProvider *provider = NSConsumerHelper::s_pProvider;

    ASSERT_NE(provider,nullptr)<< "Provider object was not found in callback";

    const char* providerID = provider->providerId;

    IOTIVITYTEST_LOG(INFO, "Waiting for subscribe");

    ASSERT_EQ(NSConsumerHelper::s_isConsumerAllowed,true)<< "Subscribe was not allowed by provider";

    IOTIVITYTEST_LOG(INFO, "Waiting for adding topic in provider");

    m_pCommonHelper->inputMenu("8");
    CommonUtil::waitInSecond(WAIT_TIME_MIN);
    m_pCommonHelper->inputMenu("8");

    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    ASSERT_EQ(NSConsumerHelper::s_isTopicChanged,true)<< "Callback was not called after adding topic in provider";

    m_pTopicLL = NSConsumerGetTopicList(providerID);

    ASSERT_NE(m_pTopicLL,nullptr)<< "Topic list was not found by consumer";

    ASSERT_EQ(m_pNSHelper->printTopicList(m_pTopicLL),true)<< "Added and received topic did not match";

    IOTIVITYTEST_LOG(INFO, "Waiting for deleting topic in provider");

    m_pCommonHelper->inputMenu("9");
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    m_pTopicLL = NSConsumerGetTopicList(providerID);
    ASSERT_NE(m_pTopicLL,nullptr)<< "Topic list was not found by consumer";

    ASSERT_EQ(NS_ERROR, NSConsumerUpdateTopicList(providerID, m_pTopicLL))<< "NSConsumerUpdateTopicList did not return error";

    m_pCommonHelper->closePipe();
    m_testAppLog = m_pCommonHelper->getAppLog();
    IOTIVITYTEST_LOG(INFO, "Provider Log: %s", m_testAppLog.c_str());
}
#endif
