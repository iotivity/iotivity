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
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "CAHelper.h"

class CACommonTest_stc: public ::testing::Test
{
protected:
    CAHelper m_caHelper;
    CAResult_t m_result;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2015-02-03
 * @see none
 * @objective Test 'CAInitialize' and 'CATerminate' in loop
 * @target  CAResult_t CAInitialize()
 * @target  void CATerminate()
 * @test_data none
 * @pre_condition none
 * @procedure 1. Call CAInitialize() API
 *            2. Check it's return value
 *            3. Call sleep to halt the program
 *            4. Call CATerminate() API
 *            5. Repeat steps 1-4 for multiple times
 * @post_condition none
 * @expected each time CAInitialize() will return CA_STATUS_OK & CATerminate() will terminate successfully
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CACommonTest_stc , CAInitializeAndCATerminateLoop_P)
{
    for (int count = 0; count < TRY_COUNT; count++)
    {
        if (!m_caHelper.initialize())
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            return;
        }

        CommonUtil::waitInSecond(SLEEP_TIME);

        CATerminate();
    }
}
#endif

/**
 * @since 2014-12-15
 * @see none
 * @objective Test multiple times 'CAInitialize' and multiple times 'CATerminate' calling effect
 * @target  CAResult_t CAInitialize()
 * @target  void CATerminate()
 * @test_data none
 * @pre_condition none
 * @procedure 1. Call CAInitialize() API
 *            2. Check it's return value
 *            3. Call sleep to halt the program
 *            4. Repeat steps 1-3 for multiple times
 *            5. Call CATerminate() API
 *            6. Call sleep to halt the program
 *            7. Repeat steps 5-6 for multiple times
 * @post_condition none
 * @expected 1. Each time CAInitialize() will return CA_STATUS_OK
 *           2. Each time CATerminate() will terminate successfully
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CACommonTest_stc , CAInitializeAndCATerminateLoop_N)
{
    for (int count = 0; count < TRY_COUNT; count++)
    {
        if (!m_caHelper.initialize())
        {
            SET_FAILURE(m_caHelper.getFailureMessage());
            return;
        }
        CommonUtil::waitInSecond(SLEEP_TIME);
    }

    for (int count = 0; count < TRY_COUNT; count++)
    {
        IOTIVITYTEST_LOG(DEBUG, "Calling CATerminate %d", count);
        CATerminate();
        IOTIVITYTEST_LOG(DEBUG, "Called CATerminate %d", count);
        CommonUtil::waitInSecond(SLEEP_TIME);
    }
}
#endif

/**
 * @since 2016-02-19
 * @see none
 * @objective Test CAGenerateToken() and CADestroyToken() with checking token length [GSRV]
 * @target  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @target  void CADestroyToken(CAToken_t token)
 * @test_data none
 * @pre_condition none
 * @procedure 1. Call CAGenerateToken() API
 *            2. Check if token is generated and API returns CA_STATUS_OK
 *            3. Call CADestroyToken() API
 * @post_condition none
 * @expected 1. CAGenerateToken() will generate the token & will return CA_STATUS_OK
 *           2. CADestroyToken() will destroy the generated token successfully
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CACommonTest_stc , CAGenerateTokenAndCADestroyToken_GSRV_P)
{
    m_caHelper.m_token = NULL;
    CAResult_t m_result = CAGenerateToken(&m_caHelper.m_token, TOKEN_MAX_LENGTH );
    if (m_caHelper.m_token == NULL)
    {
        m_caHelper.m_failureMessage = "Failed to generate token.";
        SET_FAILURE(m_caHelper.getFailureMessage());
    }

    if (m_result != CA_STATUS_OK)
    {
        SET_FAILURE(m_caHelper.getFailureMessage("CAGenerateToken", m_result, CA_STATUS_OK ));
        return;
    }

    m_caHelper.destroyToken();
}
#endif

/**
 * @since 2016-02-19
 * @see none
 * @objective Test CAGenerateToken() and CADestroyToken() by calling multiple times in a loop [VLCC]
 * @target  CAResult_t CAGenerateToken(CAToken_t *token, uint8_t tokenLength)
 * @target  void CADestroyToken(CAToken_t token)
 * @test_data none
 * @pre_condition none
 * @procedure 1. Call CAGenerateToken() API
 *            2. Check if token is generated and API returns CA_STATUS_OK
 *            3. Call CADestroyToken() API
 *            4. Repeat steps 1-3 for multiple times
 * @post_condition none
 * @expected 1. Each time CAGenerateToken() will generate the token & will return CA_STATUS_OK
 *           2. Each time CADestroyToken() will destroy the generated token successfully
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__ANDROID__)
TEST_F(CACommonTest_stc , CAGenerateTokenAndCADestroyTokenLoop_VLCC_P)
{
    for (int count = 0; count < TRY_COUNT; count++)
    {
        m_caHelper.m_token = NULL;
        CAResult_t m_result = CAGenerateToken(&m_caHelper.m_token, TOKEN_MAX_LENGTH );
        if (m_caHelper.m_token == NULL)
        {
            m_caHelper.m_failureMessage = "Failed to generate token.";
            SET_FAILURE(m_caHelper.getFailureMessage());
        }

        if (m_result != CA_STATUS_OK)
        {
            SET_FAILURE(m_caHelper.getFailureMessage("CAGenerateToken", m_result, CA_STATUS_OK ));
            return;
        }

        m_caHelper.destroyToken();
    }
}
#endif
