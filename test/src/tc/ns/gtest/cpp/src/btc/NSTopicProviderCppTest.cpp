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

#include "NSCommonCppHelper.h"
#include "NSTopic.h"
#include "NSProviderService.h"

class NSTopicProviderCppTest_btc: public ::testing::Test
{
public:
    OIC::Service::NSTopic* m_pNSTopic;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pNSTopic = nullptr;
        m_pNSTopic = new OIC::Service::NSTopic();
        ASSERT_NE(nullptr,m_pNSTopic)<<
                "NSTopic constructor did not return instance";
        IOTIVITYTEST_LOG(INFO, "SetUp called");
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();

        IOTIVITYTEST_LOG(INFO, "TearDown called");
    }
};

/**
 * @since 2016-09-29
 * @objective Test 'NSTopic' constructor with positive basic way
 * @target NSTopic()
 * @test_data None
 * @pre_condition None
 * @procedure Perform NSTopic() API
 * @post_condition None
 * @expected Constructor should return NSTopic instance
 **/
#if defined(__LINUX__)
TEST_F(NSTopicProviderCppTest_btc, TopicConstructor_SRC_P)
{
    OIC::Service::NSTopic* nsTopic = nullptr;

    nsTopic = new OIC::Service::NSTopic();
    ASSERT_NE(nullptr,nsTopic)<< "NSTopic instance is not returned by constructor";
}
#endif

/**
 * @since 2016-09-29
 * @see NSTopic()
 * @objective Test 'setTopicName' API with positive basic way
 * @target void setTopicName(const std::string &topicName)
 * @test_data topicName = "topic1"
 * @pre_condition Perform NSTopic() constructor to get NSTopic instance
 * @procedure Perform setTopicName() API
 * @post_condition None
 * @expected Exceptions should not occur
 **/
#if defined(__LINUX__)
TEST_F(NSTopicProviderCppTest_btc, SetTopicName_SRC_P)
{
    try
    {
        m_pNSTopic->setTopicName(TOPIC_NAME_1);
    }
    catch (exception &e)
    {
        SET_FAILURE(
                "Exception occurs while calling setTopicName. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSTopic()
 * @see void setTopicName(const std::string &topicName)
 * @objective Test 'getTopicName' API with positive basic way
 * @target std::string getTopicName() const
 * @test_data None
 * @pre_condition   1. Perform NSTopic() constructor to get NSTopic instance
 *                  2. Perform setTopicName() API
 * @procedure Perform getTopicName() API
 * @post_condition None
 * @expected Topic name should be returned
 **/
#if defined(__LINUX__)
TEST_F(NSTopicProviderCppTest_btc, GetTopicName_SRC_P)
{
    string topicName = "";
    m_pNSTopic->setTopicName(TOPIC_NAME_1);
    topicName = m_pNSTopic->getTopicName();
    ASSERT_NE(EMPTY_STRING,topicName)<< "getTopicName did not return topic name";
}
#endif

/**
 * @since 2016-09-29
 * @see NSTopic()
 * @objective Test 'setState' API with positive basic way
 * @target void setState(const NSTopicState state)
 * @test_data state = UNSUBSCRIBED
 * @pre_condition Perform NSTopic() constructor to get NSTopic instance
 * @procedure Perform setState() API
 * @post_condition None
 * @expected Exceptions should not occur
 **/
#if defined(__LINUX__)
TEST_F(NSTopicProviderCppTest_btc, SetState_SRC_P)
{
    try
    {
        m_pNSTopic->setState(TOPIC_STATE_UNSUBSCRIBED);
    }
    catch (exception &e)
    {
        SET_FAILURE(
                "Exception occurs while calling setState. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSTopic()
 * @see void setState(const NSTopicState state)
 * @objective Test 'getState' API with positive basic way
 * @target NSTopicState getState() const
 * @test_data None
 * @pre_condition   1. Perform NSTopic() constructor to get NSTopic instance
 *                  2. Perform setState() API
 * @procedure Perform getState() API
 * @post_condition None
 * @expected Set topic state should be returned
 **/
#if defined(__LINUX__)
TEST_F(NSTopicProviderCppTest_btc, GetState_SRC_P)
{
    OIC::Service::NSTopic::NSTopicState topicState;
    m_pNSTopic->setState(TOPIC_STATE_UNSUBSCRIBED);
    topicState = m_pNSTopic->getState();
    ASSERT_EQ(OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED,topicState)<< "getState did not return correct state";
}
#endif
