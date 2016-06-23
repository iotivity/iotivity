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


class RDServerTest_btc: public ::testing::Test
{
protected:

    RDHelper* m_rdHelper;

	RDServerTest_btc()
	{
		m_rdHelper = nullptr;
	}
	
	virtual void SetUp()
    {        
		CommonUtil::runCommonTCSetUpPart();
		m_rdHelper = RDHelper::getInstance();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        free(m_rdHelper);
    }
};

/**
 * @since 			2015-08-26
 * @see 			OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see				OCStackResult OCRDStop()
 * @objective 		Test OCRDStart function positively
 * @target 			OCStackResult OCRDStart();
 * @test_data 		none
 * @pre_condition 	Call OCInit (This function Initializes the OC Stack)
 * @procedure 		Call start RD and check return result
 * @post_condition 	OCRDStop
 * @expected 		Initialization will succeed and return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(RDServerTest_btc, StartRDServer_SRC_P)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdCheckStartServerStatus(RD_START_SERVER_WITH_INIT);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		ASSERT_EQ(OC_STACK_OK,actualResult)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
		
		m_rdHelper->rdStopServer();
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
	}
	catch (std::exception &ex)
	{
		FAIL() << "Exception occurred : " << ex.what();
	}
	
	SUCCEED();
}
#endif

/**
 * @since 			2015-08-26
 * @see				OCStackResult OCRDStop()
 * @objective 		Test RD stop without calling OCRDStart and observed that
 * 					API will not crashed and will be return OC_STACK_ERROR
 * @target 			OCStackResult OCRDStop();
 * @test_data 		none
 * @pre_condition 	none
 * @procedure 		Call stop RD.
 * @post_condition 	none
 * @expected 		OCRDStop will be executed and will return OC_STACK_ERROR
 */
#if defined(__LINUX__)
TEST_F(RDServerTest_btc, StopRDServer_ETC_P)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdStopServer();
		ASSERT_EQ(OC_STACK_ERROR,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
	}
	catch (std::exception &ex)
	{
		FAIL() << "Exception occurred : " << ex.what();
	}
	
    SUCCEED();
}
#endif


/**
 * @since 			2015-08-26
 * @see none
 * @objective 		Test OCRDCheckPublishedResource without calling other precondition's
 * 					and observed that API will not crashed and will be return OC_STACK_ERROR
 * @target 			OCStackResult OCRDCheckPublishedResource(const char *interfaceType, const char *resourceType,
        													 OCResourceCollectionPayload **payload);
 * @test_data 		none
 * @pre_condition 	none
 * @procedure 		Call OCRDCheckPublishedResource
 * @post_condition 	none
 * @expected 		OCRDStop will be executed and will return OC_STACK_ERROR
 */

#if defined(__LINUX__)
TEST_F(RDServerTest_btc, RDCheckPublishedResource_ETC_P)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdCheckPublishedResource(RD_RESOURCE_INTERFACE_TYPE_EMPTY,RD_RESOURCE_TYPE_LIGHT);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		ASSERT_EQ(OC_STACK_ERROR,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
	}
	catch (std::exception &ex)
	{
		FAIL() << "Exception occurred : " << ex.what();
	}
	
    SUCCEED();
}
#endif

/**
 * @since 			2015-08-26
 * @see none
 * @objective 		Test OCRDCheckPublishedResource function negatively for empty Resource Type and empty Interface Type
 * @target 			OCStackResult OCRDCheckPublishedResource(const char *interfaceType, const char *resourceType,
        																   OCResourceCollectionPayload **payload);
 * @test_data 		1. interfaceType as empty (a interface type that is being queried)
 * 					2. resourceType as empty  (a resource type that is being queried)
 * 					3. payload as NULL        (A payload of the matching resource type or interface type or NULL)
 * 					4. addr as {0} 			  (A device address)
 * @pre_condition 	none
 * @procedure 		Call OCRDCheckPublishedResource and check return value
 * @post_condition 	none
 * @expected 		OCRDCheckPublishedResource will not succeed and will not return return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(RDServerTest_btc, RDCheckPublishedResource_USV_N)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdCheckPublishedResource(RD_RESOURCE_INTERFACE_TYPE_EMPTY,RD_RESOURCE_TYPE_EMPTY);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		ASSERT_NE(OC_STACK_OK,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
	}
	catch (std::exception &ex)
	{
		FAIL() << "Exception occurred : " << ex.what();
	}
	
    SUCCEED();
}
#endif

/**
 * @since 			2015-08-26
 * @see 			none
 * @objective 		Test OCRDCheckPublishedResource function with null resourcetype and null interfacetype
 * @target 			OCStackResult OCRDCheckPublishedResource(const char *interfaceType, const char *resourceType,
        																   OCResourceCollectionPayload **payload);
 * @test_data 		1. interfaceType as NULL (a interface type that is being queried)
 * 					2. resourceType as NULL  (a resource type that is being queried)
 * 					3. payload as NULL        (A payload of the matching resource type or interface type or NULL)
 * 					4. addr as {0} 			  (A device address)
 * @pre_condition	none
 * @procedure 		Call OCRDCheckPublishedResource
 * @post_condition 	none
 * @expected 		OCRDCheckPublishedResource will not succeed and will not return return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(RDServerTest_btc, RDCheckPublishedResource_NV_N)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdCheckPublishedResource(RD_RESOURCE_TYPE_NULL,RD_RESOURCE_INTERFACE_TYPE_NULL);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		ASSERT_NE(OC_STACK_OK,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
	}
	catch (std::exception &ex)
	{
		FAIL() << "Exception occurred : " << ex.what();
	}
	
    SUCCEED();
}
#endif
