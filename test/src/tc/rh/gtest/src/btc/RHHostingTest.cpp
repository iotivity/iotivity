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


#include <signal.h>
#include <unistd.h>

#include "CommonUtil.h"
#include "RHCommon.h"

extern "C" {
    #include "Hosting.h"
}

class RHHostingTest_btc: public ::testing::Test
{
protected:

    RHCommon m_RHCommon;
    OCStackResult m_OCStackResult;

    virtual void SetUp()
    {
        m_OCStackResult = OC_STACK_ERROR;

    }

    virtual void TearDown()
    {

    }
};

/**
 * @since 			2015-04-15
 * @see 			none
 * @objective 		test if OICStartCoordinate API can start coordinate successfully
 * @target 			OCStackResult OICStartCoordinate();
 * @test_data 		none
 * @pre_condition 	none
 * @procedure 		Call OICStartCoordinate API
 * @post_condition  clear resource
 * @expected 		OICStartCoordinate API will start coordinate successfully
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_btc, StartCoordinate_SRC_P)
{
    ASSERT_EQ(OICStartCoordinate(), OC_STACK_OK) << START_ERR_MSG;

  OICStopCoordinate();


}

#endif


/**
 * @since 			2015-04-15
 * @see 			OCStackResult OICStartCoordinate();
 * @objective 		test OICStopCoordinate API can function successfully
 * @target 			OCStackResult OICStopCoordinate();
 * @test_data 		none
 * @pre_condition 	none
 * @procedure 		Call OICStopCoordinate API
 * @post_condition 	none
 * @expected 		OICStopCoordinate API will stop coordinate successfully
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RHHostingTest_btc, StopCoordinate_SRC_P)
{
	ASSERT_EQ(OICStopCoordinate(), OC_STACK_OK) << STOP_ERR_MSG;
}
#endif
