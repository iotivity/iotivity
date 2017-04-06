//******************************************************************
//
// Copyright 2017 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


#include "CHPHelper.h"


class CHPServerTest_stc: public  ::testing::Test
{

    protected:
        CHPHelper *m_pchpHelper;
        CHPServerTest_stc()
        {
            m_pchpHelper = nullptr;
        }

        virtual void SetUp()
        {
            m_pchpHelper = CHPHelper::getInstance();
            CommonTestUtil::runCommonTCSetUpPart();

        }
        virtual void TearDown()
        {
            CommonTestUtil::runCommonTCTearDownPart();
        }

};



/**
 * @since           2016-03-27
 * @see             OCStackResult OCInit(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags)
 * @see             bool CHPIsInitialized()
 * @see             OCStackResult CHPInitialize(bool secure)
 * @see             OCStackResult CHPTerminate()
 * @objective       Test CHPInitialize function State condition with non secure build & Non Secure argument
 * @target          OCStackResult CHPInitialize(bool secure)
 * @test_data       1. CHPInitialize with non secure argument
 * @pre_condition   Call OCInit(NULL, 0, OC_SERVER)
 * @procedure       1. Call CHPInitialize with 0 as argument
 *                  2. Call CHPIsInitialized
 *                  3. Call CHPTerminate
 * @post_condition  NONE
 * @expected        all API  will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_stc, CHPInitializeNonSecure_SCV_P)
{
    m_pchpHelper->initializeOC();
    EXPECT_EQ(OC_STACK_OK, m_pchpHelper->initializeCHPNonSecure());
    CommonUtil::waitInSecond(CHP_SLEEPTIME_MAX);
    EXPECT_EQ(true, m_pchpHelper->isInitializedCHP());
    EXPECT_EQ(OC_STACK_OK, m_pchpHelper->terminateCHP());
}
#endif

/**
 * @since           2016-03-27
 * @see             OCStackResult OCInit(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags);
 * @see             bool CHPIsInitialized()
 * @see             OCStackResult CHPInitialize(bool secure)
 * @see             OCStackResult CHPTerminate()
 * @objective       Test CHPInitialize function State condition with non secure build & Secure argument
 * @target          OCStackResult CHPInitialize(bool secure)
 * @test_data       1. CHPInitialize with non secure argument
 * @pre_condition   Call OCInit(NULL, 0, OC_SERVER)
 * @procedure       1. Call CHPInitialize with 0 as argument
 *                  2. Call CHPIsInitialized
 *                  3. Call CHPTerminate
 * @post_condition  NONE
 * @expected        all API  will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_stc, CHPInitializeSecure_SCV_P)
{
    m_pchpHelper->initializeOC();
    EXPECT_EQ(OC_STACK_OK, m_pchpHelper->initializeCHPSecure());
    CommonUtil::waitInSecond(CHP_SLEEPTIME_MAX);
    EXPECT_EQ(true, m_pchpHelper->isInitializedCHP());
    EXPECT_EQ(OC_STACK_OK, m_pchpHelper->terminateCHP());
}
#endif

/**
 * @since           2016-03-27
 * @see             OCStackResult OCInit(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags);
 * @see             bool CHPIsInitialized()
 * @see             OCStackResult CHPInitialize(bool secure)
 * @see             OCStackResult CHPTerminate()
 * @objective       Test CHPInitialize function State condition with non secure build & Non Secure argument
 * @target          OCStackResult CHPInitialize(bool secure)
 * @test_data       1. CHPInitialize with non secure argument
 * @pre_condition   Call OCInit(NULL, 0, OC_SERVER)
 * @procedure       1. Call CHPInitialize with 0 as argument
 *                  2. Call CHPIsInitialized
 *                  3. Call CHPTerminate
 *                  4. this procedure will continue 5 times
 * @post_condition  NONE
 * @expected        all API  will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_stc, CHPInitializeNonSecure_VLCC_P)
{
    m_pchpHelper->initializeOC();
    for (int i = 0; i < 5; i++)
    {
        EXPECT_EQ(OC_STACK_OK, m_pchpHelper->initializeCHPNonSecure());
        CommonUtil::waitInSecond(CHP_SLEEPTIME_MAX);
        EXPECT_EQ(true, m_pchpHelper->isInitializedCHP());
        EXPECT_EQ(OC_STACK_OK, m_pchpHelper->terminateCHP());
    }
}
#endif

/**
 * @since           2016-03-27
 * @see             OCStackResult OCInit(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags);
 * @see             bool CHPIsInitialized()
 * @see             OCStackResult CHPInitialize(bool secure)
 * @see             OCStackResult CHPTerminate()
 * @objective       Test CHPInitialize function State condition with non secure build & non Secure argument
 * @target          OCStackResult CHPInitialize(bool secure)
 * @test_data       1. CHPInitialize with non secure argument
 * @pre_condition   Call OCInit(NULL, 0, OC_SERVER)
 * @procedure       1. Call CHPInitialize with 0 as argument
 *                  2. Call CHPIsInitialized
 *                  3. Call CHPTerminate
 *                  4. this procedure will continue 5 times
 * @post_condition  NONE
 * @expected        all API  will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_stc, CHPInitializeSecure_VLCC_P)
{
    m_pchpHelper->initializeOC();
    for (int i = 0; i < 5; i++)
    {
        EXPECT_EQ(OC_STACK_OK, m_pchpHelper->initializeCHPSecure());
        CommonUtil::waitInSecond(CHP_SLEEPTIME_MAX);
        EXPECT_EQ(true, m_pchpHelper->isInitializedCHP());
        EXPECT_EQ(OC_STACK_OK, m_pchpHelper->terminateCHP());
    }
}
#endif
