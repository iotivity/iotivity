/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#include "NSTopicsList.h"
#include "NSProviderService.h"
#include "NSCppUtility.h"

class NSTopicsListProviderCppTest_btc: public ::testing::Test
{
public:
    OIC::Service::NSTopicsList* m_pNSTopicsList;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();

        m_pNSTopicsList = nullptr;
        m_pNSTopicsList = new OIC::Service::NSTopicsList();
        ASSERT_NE(nullptr,m_pNSTopicsList)<<
        "NSTopicsList constructor did not return instance";
        IOTIVITYTEST_LOG(INFO, "SetUp called");
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();

        IOTIVITYTEST_LOG(INFO, "TearDown called");
    }
};

/**
 * @since 2016-09-29
 * @objective Test 'NSTopicsList' constructor with positive basic way
 * @target NSTopicsList()
 * @test_data None
 * @pre_condition None
 * @procedure Perform NSTopicsList() API
 * @post_condition None
 * @expected Constructor should return NSTopicsList instance
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSTopicsListProviderCppTest_btc, TopicsListConstructor_SRC_P)
{
    OIC::Service::NSTopicsList* nsTopicsList = nullptr;

    nsTopicsList = new OIC::Service::NSTopicsList();
    ASSERT_NE(nullptr,nsTopicsList)<< "NSTopicsList instance is not returned by constructor";
}
#endif

/**
 * @since 2016-09-29
 * @see NSTopicsList()
 * @objective Test 'addTopic' API with positive basic way
 * @target void addTopic(const std::string &topicName, NSTopic::NSTopicState state)
 * @test_data       1. topicName = "topic1"
 *                  2. state = UNSUBSCRIBED
 * @pre_condition Perform NSTopicsList() constructor to get NSTopicsList instance
 * @procedure Perform addTopic() API
 * @post_condition None
 * @expected Exceptions should not occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSTopicsListProviderCppTest_btc, AddTopic_SRC_P)
{
    try
    {
        m_pNSTopicsList->addTopic(TOPIC_NAME_1, OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED);
    }
    catch (exception &e)
    {
        FAIL() << "Exception occurred while calling addTopic. Exception is: " << e.what();
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSTopicsList()
 * @see void addTopic(const std::string &topicName, NSTopic::NSTopicState state)
 * @objective Test 'removeTopic' API with positive basic way
 * @target void removeTopic(const std::string &topicName)
 * @test_data topicName = "topic1"
 * @pre_condition   1. Perform NSTopicsList() constructor to get NSTopicsList instance
 *                  2. Perform addTopic() API
 * @procedure Perform removeTopic() API
 * @post_condition None
 * @expected Exceptions should not occur
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSTopicsListProviderCppTest_btc, RemoveTopic_SRC_P)
{
    string topicName = TOPIC_NAME_1;
    try
    {
        m_pNSTopicsList->addTopic(topicName, OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED);
    }
    catch (exception &e)
    {
        IOTIVITYTEST_LOG(WARNING, "Exception occurs while calling removeTopic. Exception is: %s",
                e.what());
    }
    IOTIVITYTEST_LOG(DEBUG, "Topic Added");
    try
    {
        m_pNSTopicsList->removeTopic(topicName);
    }
    catch (exception &e)
    {
        FAIL() << "Exception occurred while calling removeTopic. Exception is: " << e.what();
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSTopicsList()
 * @see void addTopic(const std::string &topicName, NSTopic::NSTopicState state)
 * @objective Test 'getTopicsList' API with positive basic way
 * @target std::list<NSTopic *> getTopicsList() const
 * @test_data None
 * @pre_condition   1. Perform NSTopicsList() constructor to get NSTopicsList instance
 *                  2. Perform addTopic() API
 * @procedure Perform getTopicsList() API
 * @post_condition None
 * @expected Returned list's size should not be zero
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(NSTopicsListProviderCppTest_btc, GetTopicsList_SRC_P)
{
    m_pNSTopicsList->addTopic(TOPIC_NAME_1, OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED);
    ASSERT_NE( (size_t) 0, m_pNSTopicsList->getTopicsList().size() )<< "getTopicsList returned an empty list";
}
#endif
