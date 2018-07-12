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

#include "RDHelper.h"
using namespace OC;
using namespace boost;
using namespace std;
class RDServerIntegrationTest_stc: public ::testing::Test
{
protected:

    RDHelper* m_rdHelper;

    RDServerIntegrationTest_stc()
    {
        m_rdHelper = nullptr;
    }

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        m_rdHelper = RDHelper::getInstance();
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        free(m_rdHelper);
    }

};

/**
 * @since           2016-02-23
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCRDStart and OCRDStop function positively in a loop
 * @target          OCStackResult OCRDStart(),OCStackResult OCRDStop()
 * @test_data       none
 * @pre_condition   Call OCInit (This function Initializes the OC Stack)
 * @procedure       1. Then call OCRDStart and check return result
 *                  2. Then Call OCRDStop and check return result
 *                  3. Repeat the scenario 1~3 RD_MAX_LOOP_COUNT_VLCC times
 * @post_condition  none
 * @expected        Initialization will succeed and return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__WINDOWS__)
TEST_F(RDServerIntegrationTest_stc, StartStopRDServer_VLCC_P)
{
    try
    {

        for(int i = 0; i < RD_MAX_LOOP_COUNT_VLCC; i++)
        {
            OCStackResult actualResult = m_rdHelper->rdCheckStartServerStatus(RD_START_SERVER_WITH_INIT);
            CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
            ASSERT_EQ(OC_STACK_OK,actualResult)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);

            OCStackResult actualResultStop = m_rdHelper->rdStopServer();
            CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
            ASSERT_EQ(OC_STACK_OK,actualResultStop)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResultStop);
        }
    }
    catch (std::exception &ex)
    {
        FAIL() << "Exception occurred : " << ex.what();
    }

    SUCCEED();
}
#endif

/**
 * @since             2015-08-26
 * @see             none
 * @objective         Test OCRDStart function negatively without calling OCInit
 * @target             OCStackResult OCRDStart();
 * @test_data         none
 * @pre_condition     none
 * @procedure         Call OCRDStart and check return result
 * @post_condition     OCStop
 * @expected         OC_STACK_INVALID_PARAM should return as we did not initialized
 */
#if defined(__LINUX__) || defined(__WINDOWS__)
TEST_F(RDServerIntegrationTest_stc, StartRDServer_DSCC_N)
{
    try
    {
        OCStackResult actualResult = m_rdHelper->rdCheckStartServerStatus(RD_START_SERVER_WITHOUT_INIT);
        CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
        ASSERT_EQ(OC_STACK_INVALID_PARAM,actualResult)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);

        m_rdHelper->rdStopServer();
    }
    catch (std::exception &ex)
    {
        FAIL() << "Exception occurred : " << ex.what();
    }

    SUCCEED();
}
#endif
