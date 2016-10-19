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

#include "NSConsumerHelper.h"

class NSConsumerTest_btc: public ::testing::Test
{
protected:
    NSConsumerHelper* m_pNSHelper;
    NSCommonHelper* m_pCommonHelper;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pNSHelper = NSConsumerHelper::getInstance();
        m_pCommonHelper = NSCommonHelper::getInstance();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2016-08-16
 * @see None
 * @objective Test 'NSStartConsumer' function with positive basic way
 * @target NSResult NSStartConsumer(NSConsumerConfig config);
 * @test_data 1. changedCb
 *            2. messageCb
 *            3. syncInfoCb
 * @pre_condition Construct config with test_data
 * @procedure Perform NSStartConsumer() API
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerTest_btc, StartConsumer_SRC_P)
{
    EXPECT_EQ(NSStartConsumer(m_pNSHelper->getConsumerConfig()), NS_OK);
}
#endif

/**
 * @since 2016-08-17
 * @see None
 * @objective Test 'NSStopConsumer' function with positive basic way
 * @target NSResult NSStopConsumer();
 * @test_data None
 * @pre_condition None
 * @procedure Perform NSStopConsumer() API
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerTest_btc, StopConsumer_SRC_P)
{
    NSStartConsumer(m_pNSHelper->getConsumerConfig());
    EXPECT_EQ(NSStopConsumer(), NS_OK);
}
#endif

/**
 * @since 2016-08-17
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @objective Test 'NSRescanProvider' function with positive basic way
 * @target NSResult NSStopConsumer();
 * @test_data None
 * @pre_condition Perform NSStartConsumer() API
 * @procedure Perform NSRescanProvider() API
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerTest_btc, RescanProvider_SRC_P)
{
    NSStartConsumer(m_pNSHelper->getConsumerConfig());

    CommonUtil::waitInSecond(WAIT_TIME_MIN);

    EXPECT_EQ(NSRescanProvider(), NS_OK);
}
#endif

/**
 * @since 2016-09-07
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @objective Test 'NSSubscribe' function with positive way
 * @target NSResult NSSubscribe(const char * providerId)
 * @test_data providerId from callback
 * @pre_condition 1. Start Provider simulator
 *                2. Start Consumer
 *                3. Wait and get the providerId from callback
 * @procedure Perform NSSubscribe() API
 * @post_condition Stop Provider Simulator
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerTest_btc, Subscribe_SRC_P)
{
    CommonUtil::launchApp(PROVIDER_SIMULATOR_ALLOW);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    NSProvider *provider = m_pNSHelper->getProvider();

    EXPECT_EQ(NS_OK, NSSubscribe(provider->providerId));

    CommonUtil::killApp(PROVIDER_SIMULATOR);
}
#endif

/**
 * @since 2016-08-18
 * @see None
 * @objective Test 'NSSubscribe' function negatively with null providerId
 * @target NSResult NSSubscribe(const char * providerId)
 * @test_data Null providerId
 * @pre_condition None
 * @procedure Perform NSSubscribe() API
 * @post_condition None
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerTest_btc, Subscribe_NV_N)
{
    NSProvider *provider = nullptr;

    EXPECT_NE(NS_OK, NSSubscribe(provider->providerId));
}
#endif

/**
 * @since 2016-09-07
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @objective Test 'NSConsumerGetProvider' function with positive basic way
 * @target NSProvider * NSConsumerGetProvider(const char * providerId)
 * @test_data providerId
 * @pre_condition 1. Start Provider simulator
 *                2. Start Consumer
 *                3. Wait and get the providerId from callback
 * @procedure Perform NSConsumerGetProvider() API
 * @post_condition Stop Provider Simulator
 * @expected The API should return a valid provider
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerTest_btc, GetProvider_SRC_P)
{
    CommonUtil::launchApp(PROVIDER_SIMULATOR_ALLOW);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    NSProvider *provider = m_pNSHelper->getProvider();

    EXPECT_NE(nullptr, NSConsumerGetProvider(provider->providerId));

    CommonUtil::killApp(PROVIDER_SIMULATOR);
}
#endif

/**
 * @since 2016-08-19
 * @see None
 * @objective Test 'NSConsumerGetProvider' function negatively with null providerId
 * @target NSProvider * NSConsumerGetProvider(const char * providerId)
 * @test_data providerId
 * @pre_condition None
 * @procedure Perform NSConsumerGetProvider() API
 * @post_condition None
 * @expected The API should return null provider
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerTest_btc, GetProvider_USV_N)
{
    NSProvider *provider = nullptr;

    EXPECT_EQ(nullptr, NSConsumerGetProvider(provider->providerId));
}
#endif

/**
 * @since 2016-09-07
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @objective Test 'NSConsumerGetTopicList' function with positive basic way
 * @target NSProvider * NSConsumerGetTopicList(const char * providerId)
 * @test_data providerId
 * @pre_condition 1. Start Provider simulator
 *                2. Start Consumer
 *                3. Wait and get the providerId from callback
 * @procedure Perform NSConsumerGetTopicList() API
 * @post_condition Stop Provider Simulator
 * @expected The API should return topic list
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerTest_btc, GetTopicList_SRC_P)
{
    CommonUtil::launchApp(PROVIDER_SIMULATOR_ALLOW);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    NSProvider *provider = m_pNSHelper->getProvider();

    NSTopicLL *topics = NSConsumerGetTopicList(provider->providerId);

    EXPECT_NE(NULL, topics);

    CommonUtil::killApp(PROVIDER_SIMULATOR);
}
#endif

/**
 * @since 2016-09-08
 * @see None
 * @objective Test 'NSConsumerGetTopicList' function with positive basic way
 * @target NSProvider * NSConsumerGetTopicList(const char * providerId)
 * @test_data null providerId
 * @pre_condition None
 * @procedure Perform NSConsumerGetTopicList() API
 * @post_condition None
 * @expected The API should return NULL
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerTest_btc, GetTopicList_USV_N)
{
    NSProvider *provider = nullptr;

    NSTopicLL *topics = NSConsumerGetTopicList(provider->providerId);

    EXPECT_EQ(NULL, topics);
}
#endif

/**
 * @since 2016-09-08
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @see NSTopicLL * NSConsumerGetTopicList(const char * providerId)
 * @objective Test 'NSConsumerUpdateTopicList' function with positive basic way
 * @target NSResult NSConsumerUpdateTopicList(const char * providerId, NSTopicLL * topics)
 * @test_data providerId, topics
 * @pre_condition 1. Start Provider simulator
 *                2. Start Consumer
 *                3. Wait and get the providerId from callback
 *                4. Get Topic list
 * @procedure Perform NSConsumerUpdateTopicList() API
 * @post_condition Stop Provider Simulator
 * @expected The API should return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerTest_btc, UpdateTopicList_SRC_P)
{
    CommonUtil::launchApp(PROVIDER_SIMULATOR_ALLOW);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    NSProvider *provider = m_pNSHelper->getProvider();

    NSTopicLL *topics = NSConsumerGetTopicList(provider->providerId);

    NSResult result = NSConsumerUpdateTopicList(provider->providerId, topics);

    EXPECT_NE(NS_OK, result) << "Expected: Not " << m_pNSHelper->getResultString(NS_OK)
    << " Actual: " << m_pNSHelper->getResultString(result) << endl;

    CommonUtil::killApp(PROVIDER_SIMULATOR);
}
#endif

/**
 * @since 2016-09-07
 * @see NSResult NSStartProvider(NSProviderConfig config)
 * @see NSResult NSStartConsumer(NSConsumerConfig config)
 * @objective Test 'NSConsumerGetProvider' function with null topic list
 * @target NSResult NSConsumerUpdateTopicList(const char * providerId, NSTopicLL * topics)
 * @test_data providerId, null topics
 * @pre_condition 1. Start Provider
 *                2. Start Consumer
 *                3. Get the provider from callback
 * @procedure Perform NSConsumerUpdateTopicList() API
 * @post_condition Stop Provider Simulator
 * @expected The API should not return NS_OK
 **/
#if defined(__LINUX__)
TEST_F(NSConsumerTest_btc, UpdateTopicList_NV_N)
{
    CommonUtil::launchApp(PROVIDER_SIMULATOR_ALLOW);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    NSProvider *provider = m_pNSHelper->getProvider();

    NSTopicLL *topics = NULL;

    NSResult result = NSConsumerUpdateTopicList(provider->providerId, topics);

    EXPECT_NE(NS_OK, result) << "Expected: Not " << m_pNSHelper->getResultString(NS_OK)
    << " Actual: " << m_pNSHelper->getResultString(result) << endl;

    CommonUtil::killApp(PROVIDER_SIMULATOR);
}
#endif
