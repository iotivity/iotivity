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


class CHPServerTest_btc: public  ::testing::Test
{

    protected:
        CHPHelper *m_pchpHelper;
        CHPServerTest_btc()
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
 * @since           2017-03-27
 * @see             OCStackResult CHPInitialize(bool secure)
 * @objective       Test CHPInitialize function without calling ocinit non secure build & Non Secure argument
 * @target          OCStackResult CHPInitialize(bool secure)
 * @test_data       1. CHPInitialize with non secure argument
 * @pre_condition   none
 * @procedure       Call CHPInitialize with 0 as argument
 * @post_condition  none
 * @expected        expected no crash & OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_btc, CHPInitializeNonSecurewithoutOCInit_ECRC_N)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, m_pchpHelper->initializeCHPNonSecure());
}
#endif

/**
 * @since           2017-03-27
 * @see             OCStackResult CHPInitialize(bool secure);
 * @objective       Test CHPInitialize function without calling ocinit non secure build & Secure argument
 * @target          OCStackResult CHPInitialize(bool secure)
 * @test_data       1. CHPInitialize with secure argument
 * @pre_condition   none
 * @procedure       Call CHPInitialize with 1 as argument
 * @post_condition  none
 * @expected        expected no crash and OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_btc, CHPInitializeSecurewithoutOCInit_ECRC_N)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, m_pchpHelper->initializeCHPSecure());
}
#endif

/**
 * @since           2016-03-27
 * @see             OCStackResult CHPTerminate();
 * @objective       Test CHPTerminate function without CHPInitialize
 * @target          OCStackResult CHPTerminate();
 * @test_data       none
 * @pre_condition   none
 * @procedure       Call CHPTerminate
 * @post_condition  none
 * @expected        CHPTerminate not crash and will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_btc, CHPTerminate_ECRC_N)
{
    EXPECT_EQ(OC_STACK_OK,(m_pchpHelper->terminateCHP()));
}
#endif

/**
 * @since           2017-03-27
 * @see             OCStackResult OCInit(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags);
 * @see             OCStackResult CHPInitialize(bool secure)
 * @see             OCStackResult CHPTerminate()
 * @objective       Test CHPInitialize function with non secure build & Non Secure argument
 * @target          OCStackResult CHPInitialize(bool secure)
 * @test_data       1. CHPInitialize with non secure argument
 * @pre_condition   1. Call OCInit(NULL, 0, OC_SERVER)
 * @procedure       Call CHPInitialize with 0 as argument
 * @post_condition  Call CHPTerminate
 * @expected        CHPInitialize will succeed and will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_btc, CHPInitializeNonSecure_SRC_P)
{
    m_pchpHelper->initializeOC();
    EXPECT_EQ(OC_STACK_OK, m_pchpHelper->initializeCHPNonSecure());
    CommonUtil::waitInSecond(CHP_SLEEPTIME_MAX);
    m_pchpHelper->terminateCHP();
}
#endif

/**
 * @since           2017-03-27
 * @see             OCStackResult OCInit(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags);
 * @see             OCStackResult CHPInitialize(bool secure)
 * @see             OCStackResult CHPTerminate()
 * @objective       Test CHPInitialize function with non secure build & Secure argument
 * @target          OCStackResult CHPInitialize(bool secure)
 * @test_data       1. CHPInitialize with secure argument
 * @pre_condition   1. Call OCInit(NULL, 0, OC_SERVER)
 * @procedure       Call CHPInitialize with 1 as argument
 * @post_condition  Call CHPTerminate
 * @expected        CHPInitialize will succeed and will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_btc, CHPInitializedSecure_SRC_N)
{
    m_pchpHelper->initializeOC();
    EXPECT_EQ(OC_STACK_OK, m_pchpHelper->initializeCHPSecure());
    CommonUtil::waitInSecond(CHP_SLEEPTIME_MAX);
    m_pchpHelper->terminateCHP();
}
#endif

/**
 * @since           2017-03-27
 * @see             OCStackResult OCInit(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags);
 * @see             OCStackResult CHPInitialize(bool secure)
 * @see             OCStackResult CHPTerminate()
 * @objective       Test CHPTerminate function with non secure build & Non Secure Initialization
 * @target          OCStackResult CHPTerminate()
 * @test_data       none
 * @pre_condition   1. Call OCInit(NULL, 0, OC_SERVER)
 *                  2. CHPInitialize with non secure argument
 * @procedure       Call CHPTerminate
 * @post_condition  none
 * @expected        CHPTerminate will succeed and will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_btc, CHPTerminateNonSecure_SRC_P)
{
    m_pchpHelper->initializeOC();
    m_pchpHelper->initializeCHPNonSecure();
    CommonUtil::waitInSecond(CHP_SLEEPTIME_MAX);
    EXPECT_EQ(OC_STACK_OK, m_pchpHelper->terminateCHP());
}
#endif

/**
 * @since           2017-03-27
 * @see             OCStackResult OCInit(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags);
 * @see             OCStackResult CHPInitialize(bool secure)
 * @see             OCStackResult CHPTerminate()
 * @objective       Test CHPTerminate function with non secure build & Secure Initialization
 * @target          OCStackResult CHPTerminate()
 * @test_data       none
 * @pre_condition   1. Call OCInit(NULL, 0, OC_SERVER)
 *                  2. CHPInitialize with secure argument
 * @procedure       Call CHPTerminate
 * @post_condition  none
 * @expected        CHPTerminate will succeed and will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_btc, CHPTerminateSecure_SRC_P)
{
    m_pchpHelper->initializeOC();
    m_pchpHelper->initializeCHPSecure();
    CommonUtil::waitInSecond(CHP_SLEEPTIME_MAX);
    EXPECT_EQ(OC_STACK_OK, m_pchpHelper->terminateCHP());
}
#endif

/**
 * @since           2017-03-27
 * @see             bool CHPIsInitialized()
 * @objective       Test CHPIsInitialized function with out Initialization
 * @target          bool CHPIsInitialized()
 * @test_data       none
 * @pre_condition   none
 * @procedure       Call bool CHPIsInitialized()
 * @post_condition  none
 * @expected        CHPIsInitialized() will return false
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_btc, CHPIsInitialized_SRC_N)
{
    EXPECT_EQ(false, m_pchpHelper->isInitializedCHP());
}
#endif

/**
 * @since           2017-03-27
 * @see             OCStackResult OCInit(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags);
 * @see             bool CHPIsInitialized()
 * @see             OCStackResult CHPInitialize(bool secure)
 * @see             OCStackResult CHPTerminate()
 * @objective       Test CHPIsInitialized function with secure build & Secure Initialization
 * @target          bool CHPInitialize()
 * @test_data       none
 * @pre_condition   1. Call OCInit(NULL, 0, OC_SERVER)
 *                  2. CHPInitialize with secure argument
 * @procedure       Call CHPIsInitialized
 * @post_condition  Call CHPTerminate
 * @expected        CHPIsInitialized will succeed and will return true
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_btc, CHPIsInitializedSecure_SRC_P)
{
    m_pchpHelper->initializeOC();
    m_pchpHelper->initializeCHPSecure();
    CommonUtil::waitInSecond(CHP_SLEEPTIME_MAX);
    EXPECT_EQ(true, m_pchpHelper->isInitializedCHP());
    m_pchpHelper->terminateCHP();
}
#endif

/**
 * @since           2017-03-27
 * @see             OCStackResult OCInit(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags);
 * @see             bool CHPIsInitialized()
 * @see             OCStackResult CHPInitialize(bool secure)
 * @see             OCStackResult CHPTerminate()
 * @objective       Test CHPIsInitialized function with secure build & Non Secure Initialization
 * @target          bool CHPInitialize()
 * @test_data       none
 * @pre_condition   1. Call OCInit(NULL, 0, OC_SERVER)
 *                  2. CHPInitialize with non secure argument
 * @procedure       Call CHPIsInitialized
 * @post_condition  Call CHPTerminate
 * @expected        CHPIsInitialized will succeed and will return true
 */
#if defined(__LINUX__)
TEST_F(CHPServerTest_btc, CHPIsInitializedNonSecure_SRC_P)
{
    m_pchpHelper->initializeOC();
    m_pchpHelper->initializeCHPNonSecure();
    CommonUtil::waitInSecond(CHP_SLEEPTIME_MAX);
    EXPECT_EQ(true, m_pchpHelper->isInitializedCHP());
    m_pchpHelper->terminateCHP();
}
#endif
