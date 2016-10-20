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
#include "NSSyncInfo.h"
#include "NSProviderService.h"

class NSSyncInfoProviderCppTest_btc: public ::testing::Test
{
public:
    OIC::Service::NSSyncInfo* m_pNSSyncInfo;
    uint64_t m_messageID = 10;
    string m_providerID = "provider1";
    OIC::Service::NSSyncInfo::NSSyncType m_state = OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_UNREAD;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_pNSSyncInfo = nullptr;
        m_pNSSyncInfo = new OIC::Service::NSSyncInfo(m_messageID,m_providerID,m_state);
        ASSERT_NE(nullptr,m_pNSSyncInfo)<<
                "NSSyncInfo constructor did not return instance";
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
 * @objective Test 'NSSyncInfo' constructor with positive basic way
 * @target NSSyncInfo()
 * @test_data None
 * @pre_condition None
 * @procedure Perform NSSyncInfo() API
 * @post_condition None
 * @expected Constructor should return NSSyncInfo instance
 **/
#if defined(__LINUX__)
TEST_F(NSSyncInfoProviderCppTest_btc, SyncInfoConstructor_SRC_P)
{
    OIC::Service::NSSyncInfo* nsSyncInfo = nullptr;

    nsSyncInfo = new OIC::Service::NSSyncInfo();
    ASSERT_NE(nullptr,nsSyncInfo)<< "NSSyncInfo instance is not returned by constructor";
}
#endif

/**
 * @since 2016-09-29
 * @objective Test 'NSSyncInfo' constructor with positive basic way
 * @target NSSyncInfo(const uint64_t &messageId, const std::string &providerId,
 *                          const NSSyncType &state)
 * @test_data       1. messageId=10
 *                  2. providerId="provider1"
 *                  3. state=NS_SYNC_UNREAD
 * @pre_condition None
 * @procedure Perform NSSyncInfo() API
 * @post_condition None
 * @expected Constructor should return NSSyncInfo instance
 **/
#if defined(__LINUX__)
TEST_F(NSSyncInfoProviderCppTest_btc, SyncInfoConstructorWithParameters_SRC_P)
{
    OIC::Service::NSSyncInfo* nsSyncInfo = nullptr;

    nsSyncInfo = new OIC::Service::NSSyncInfo(m_messageID,m_providerID,m_state);
    ASSERT_NE(nullptr,nsSyncInfo)<< "NSSyncInfo instance is not returned by constructor";
}
#endif

/**
 * @since 2016-09-29
 * @see NSSyncInfo(const uint64_t &messageId, const std::string &providerId,
 *                          const NSSyncType &state)
 * @objective Test 'getMessageId' API with positive basic way
 * @target uint64_t getMessageId() const
 * @test_data None
 * @pre_condition Perform NSSyncInfo() constructor with parameters to get NSSyncInfo instance
 * @procedure Perform getMessageId() API
 * @post_condition None
 * @expected API should return correct message ID
 **/
#if defined(__LINUX__)
TEST_F(NSSyncInfoProviderCppTest_btc, GetMessageID_SRC_P)
{
    ASSERT_EQ(m_messageID,m_pNSSyncInfo->getMessageId())
            << "getMessageId did not return correct message ID";
}
#endif

/**
 * @since 2016-09-29
 * @see NSSyncInfo(const uint64_t &messageId, const std::string &providerId,
 *                          const NSSyncType &state)
 * @objective Test 'getProviderId' API with positive basic way
 * @target std::string getProviderId() const
 * @test_data None
 * @pre_condition Perform NSSyncInfo() constructor with parameters to get NSSyncInfo instance
 * @procedure Perform getProviderId() API
 * @post_condition None
 * @expected API should return correct provider ID
 **/
#if defined(__LINUX__)
TEST_F(NSSyncInfoProviderCppTest_btc, GetProviderID_SRC_P)
{
    ASSERT_EQ(m_providerID,m_pNSSyncInfo->getProviderId())
            << "getProviderId did not return correct provider ID";
}
#endif

/**
 * @since 2016-09-29
 * @see NSSyncInfo(const uint64_t &messageId, const std::string &providerId,
 *                          const NSSyncType &state)
 * @objective Test 'getState' API with positive basic way
 * @target NSSyncType getState() const
 * @test_data None
 * @pre_condition Perform NSSyncInfo() constructor with parameters to get NSSyncInfo instance
 * @procedure Perform getState() API
 * @post_condition None
 * @expected API should return correct state
 **/
#if defined(__LINUX__)
TEST_F(NSSyncInfoProviderCppTest_btc, GetState_SRC_P)
{
    ASSERT_EQ(m_state,m_pNSSyncInfo->getState())
            << "getState did not return correct state";
}
#endif
