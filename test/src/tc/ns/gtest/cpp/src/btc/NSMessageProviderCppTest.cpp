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
#include "NSMessage.h"
#include "NSProviderService.h"

class NSMessageProviderCppTest_btc: public ::testing::Test
{
public:
    OIC::Service::NSMessage* m_pNSMessage;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pNSMessage = nullptr;
        m_pNSMessage = new OIC::Service::NSMessage();
        ASSERT_NE(nullptr,m_pNSMessage)<<
                "NSMessage constructor did not return instance";
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
 * @objective Test 'NSMessage' constructor with positive basic way
 * @target NSMessage()
 * @test_data None
 * @pre_condition None
 * @procedure Perform NSMessage() API
 * @post_condition None
 * @expected Constructor should return NSMessage instance
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, MessageConstructor_SRC_P)
{
    OIC::Service::NSMessage* nsMessage = nullptr;

    nsMessage = new OIC::Service::NSMessage();
    ASSERT_NE(nullptr,nsMessage)<< "NSMessage instance is not returned by constructor";
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @objective Test 'getMessageId' API with positive basic way
 * @target uint64_t getMessageId() const
 * @test_data None
 * @pre_condition Perform NSMessage() constructor to get NSMessage instance
 * @procedure Perform getMessageId() API
 * @post_condition None
 * @expected API should return message ID
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, GetMessageID_SRC_P)
{
    uint64_t id = 100;
    id = m_pNSMessage->getMessageId();
    ASSERT_NE(100,id)<< "getMessageId did not return ID";
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @objective Test 'getProviderId' API with positive basic way
 * @target std::string getProviderId() const
 * @test_data None
 * @pre_condition Perform NSMessage() constructor to get NSMessage instance
 * @procedure Perform getProviderId() API
 * @post_condition None
 * @expected API should return provider ID
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, GetProviderID_SRC_P)
{
    string providerID = "";
    providerID = m_pNSMessage->getProviderId();
    ASSERT_EQ(EMPTY_STRING,providerID)<< "getProviderId did not return provider ID";
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @objective Test 'setSourceName' API with positive basic way
 * @target void setSourceName(const std::string &sourceName)
 * @test_data sourceName "source1"
 * @pre_condition Perform NSMessage() constructor to get NSMessage instance
 * @procedure Perform setSourceName() API
 * @post_condition None
 * @expected Exception should not be thrown
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, SetSourceName_SRC_P)
{
    try
    {
        m_pNSMessage->setSourceName(SOURCE_NAME);
    }
    catch (exception &e)
    {
        SET_FAILURE(
                "Exception occurs while calling setSourceName. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @see void setSourceName(const std::string &sourceName)
 * @objective Test 'setSourceName' API with positive basic way
 * @target std::string getSourceName() const
 * @test_data None
 * @pre_condition   1. Perform NSMessage() constructor to get NSMessage instance
 *                  2. Perform setSourceName() API
 * @procedure Perform getSourceName() API
 * @post_condition None
 * @expected set source name should be returned
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, GetSourceName_SRC_P)
{
    m_pNSMessage->setSourceName(SOURCE_NAME);
    ASSERT_EQ(SOURCE_NAME,m_pNSMessage->getSourceName())<< "getSourceName did not return set source name";
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @objective Test 'setType' API with positive basic way
 * @target void setType(const NSMessageType &type)
 * @test_data NSMessageType INFO
 * @pre_condition Perform NSMessage() constructor to get NSMessage instance
 * @procedure Perform setType() API
 * @post_condition None
 * @expected Exception should not be thrown
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, SetType_SRC_P)
{
    try
    {
        m_pNSMessage->setType(MESSAGE_TYPE);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling setType. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @see void setType(const NSMessageType &type)
 * @objective Test 'getType' API with positive basic way
 * @target NSMessageType getType() const
 * @test_data None
 * @pre_condition   1. Perform NSMessage() constructor to get NSMessage instance
 *                  2. Perform setType() API
 * @procedure Perform getType() API
 * @post_condition None
 * @expected Returned message type should be same as the set type
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, GetType_SRC_P)
{
    m_pNSMessage->setType(MESSAGE_TYPE);
    ASSERT_EQ(MESSAGE_TYPE,m_pNSMessage->getType())<< "getType did not return set type";
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @objective Test 'setTime' API with positive basic way
 * @target void setTime(const std::string &time)
 * @test_data time "time1"
 * @pre_condition Perform NSMessage() constructor to get NSMessage instance
 * @procedure Perform setTime() API
 * @post_condition None
 * @expected Exception should not be thrown
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, SetTime_SRC_P)
{
    try
    {
        m_pNSMessage->setTime(TIME_TO_SET);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling setTime. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @see void setTime(const std::string &time)
 * @objective Test 'getTime' API with positive basic way
 * @target std::string getTime() const
 * @test_data None
 * @pre_condition   1. Perform NSMessage() constructor to get NSMessage instance
 *                  2. Perform setTime() API
 * @procedure Perform getTime() API
 * @post_condition None
 * @expected Time should be returned
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, GetTime_SRC_P)
{
    string time = "";
    m_pNSMessage->setTime(TIME_TO_SET);
    time = m_pNSMessage->getTime();
    ASSERT_NE(EMPTY_STRING,time)<< "getTime did not return time";
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @objective Test 'setTTL' API with positive basic way
 * @target void setTTL(const uint64_t &ttl)
 * @test_data ttl 0
 * @pre_condition Perform NSMessage() constructor to get NSMessage instance
 * @procedure Perform setTTL() API
 * @post_condition None
 * @expected Exception should not be thrown
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, SetTTL_SRC_P)
{
    try
    {
        m_pNSMessage->setTTL(TTL_TO_SET);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling setTTL. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @see void setTTL(const uint64_t &ttl)
 * @objective Test 'getTTL' API with positive basic way
 * @target uint64_t getTTL() const
 * @test_data None
 * @pre_condition   1. Perform NSMessage() constructor to get NSMessage instance
 *                  2. Perform setTTL() API
 * @procedure Perform getTTL() API
 * @post_condition None
 * @expected set ttl is returned
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, GetTTL_SRC_P)
{
    m_pNSMessage->setTTL(TTL_TO_SET);
    ASSERT_EQ(TTL_TO_SET,m_pNSMessage->getTTL())<< "getTTL did not return set TTL";
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @objective Test 'setTitle' API with positive basic way
 * @target void setTitle(const std::string &title)
 * @test_data title "title1"
 * @pre_condition Perform NSMessage() constructor to get NSMessage instance
 * @procedure Perform setTitle() API
 * @post_condition None
 * @expected Exception should not be thrown
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, SetTitle_SRC_P)
{
    try
    {
        m_pNSMessage->setTitle(TITLE_TO_SET);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling setTitle. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @see void setTitle(const std::string &title)
 * @objective Test 'getTitle' API with positive basic way
 * @target std::string getTitle() const
 * @test_data None
 * @pre_condition   1. Perform NSMessage() constructor to get NSMessage instance
 *                  2. Perform setTitle() API
 * @procedure Perform getTitle() API
 * @post_condition None
 * @expected set title should be returned
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, GetTitle_SRC_P)
{
    m_pNSMessage->setTitle(TITLE_TO_SET);
    ASSERT_EQ(TITLE_TO_SET,m_pNSMessage->getTitle())<< "getTitle did not return set title";
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @objective Test 'setContentText' API with positive basic way
 * @target void setContentText(const std::string &contextText)
 * @test_data contextText "contenttext1"
 * @pre_condition Perform NSMessage() constructor to get NSMessage instance
 * @procedure Perform setContentText() API
 * @post_condition None
 * @expected Exception should not be thrown
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, SetContentText_SRC_P)
{
    try
    {
        m_pNSMessage->setContentText(CONTENTTEXT_TO_SET);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling setContentText. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @see void setContentText(const std::string &contextText)
 * @objective Test 'getContentText' API with positive basic way
 * @target std::string getContentText() const;
 * @test_data None
 * @pre_condition   1. Perform NSMessage() constructor to get NSMessage instance
 *                  2. Perform setContentText() API
 * @procedure Perform getContentText() API
 * @post_condition None
 * @expected set Contenttext should be returned
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, GetContentText_SRC_P)
{
    m_pNSMessage->setContentText(CONTENTTEXT_TO_SET);
    ASSERT_EQ(CONTENTTEXT_TO_SET,m_pNSMessage->getContentText())<< "getContentText did not return set contenttext";
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @objective Test 'setMediaContents' API with positive basic way
 * @target void setMediaContents(NSMediaContents *mediaContents)
 * @test_data mediaContents instance of mediaContents
 * @pre_condition Perform NSMessage() constructor to get NSMessage instance
 * @procedure Perform setMediaContents() API
 * @post_condition None
 * @expected Exception should not be thrown
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, SetMediaContents_SRC_P)
{
    OIC::Service::NSMediaContents* mediaContents;

    try
    {
        m_pNSMessage->setMediaContents(mediaContents);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling setMediaContents. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @see void setMediaContents(NSMediaContents *mediaContents)
 * @objective Test 'getMediaContents' API with positive basic way
 * @target NSMediaContents *getMediaContents() const
 * @test_data mediaContents instance of mediaContents
 * @pre_condition   1. Perform NSMessage() constructor to get NSMessage instance
 *                  2. Perform setMediaContents() API
 * @procedure Perform getMediaContents() API
 * @post_condition None
 * @expected set mediacontents should be returned
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, GetMediaContents_SRC_P)
{
    OIC::Service::NSMediaContents* mediaContents;
    m_pNSMessage->setMediaContents(mediaContents);
    ASSERT_EQ(mediaContents,m_pNSMessage->getMediaContents())<< "getMediaContents did not return media contents";
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @objective Test 'setTopic' API with positive basic way
 * @target void setTopic(const std::string &topic)
 * @test_data topic "TEST_TOPIC_1"
 * @pre_condition Perform NSMessage() constructor to get NSMessage instance
 * @procedure Perform setTopic() API
 * @post_condition None
 * @expected Exception should not be thrown
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, SetTopicFromMessage_SRC_P)
{
    try
    {
        m_pNSMessage->setTopic(TEST_TOPIC_1);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs while calling setTopic. Exception is: " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-09-29
 * @see NSMessage()
 * @see void setTopic(const std::string &topic)
 * @objective Test 'getTopic' API with positive basic way
 * @target std::string getTopic() const
 * @test_data None
 * @pre_condition   1. Perform NSMessage() constructor to get NSMessage instance
 *                  2. Perform setTopic() API
 * @procedure Perform getTopic() API
 * @post_condition None
 * @expected set topic should be returned
 **/
#if defined(__LINUX__)
TEST_F(NSMessageProviderCppTest_btc, GetTopicFromMessage_SRC_P)
{
    m_pNSMessage->setTopic(TEST_TOPIC_1);
    ASSERT_EQ(TEST_TOPIC_1,m_pNSMessage->getTopic())<< "getTopic did not return set topic";
}
#endif
