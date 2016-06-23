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
 * @since 			2016-02-23
 * @see 			OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective 		Test OCRDStart and OCRDStop function positively in a loop
 * @target 			OCStackResult OCRDStart(),OCStackResult OCRDStop()
 * @test_data 		none
 * @pre_condition 	Call OCInit (This function Initializes the OC Stack)
 * @procedure 		1. Then call OCRDStart and check return result
 * 					2. Then Call OCRDStop and check return result
 * 					3. Repeat the scenario 1~3 RD_MAX_LOOP_COUNT_VLCC times
 * @post_condition 	none
 * @expected 		Initialization will succeed and return OC_STACK_OK
 */
#if defined(__LINUX__)
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
 * @since 			2015-08-26
 * @see				OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see 			OCStackResult OCRDStart()
 * @see 			OCStackResult OCRDStop()
 * @see				OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor)
 * @see				OCStackResult OCRDPublish(char *addr, uint16_t port, int num, ...)
 * @objective 		Test OCRDCheckPublishedResource function positively for checking RD resource(light)
 * @target 			OCStackResult OCRDCheckPublishedResource(const char *interfaceType, const char *resourceType,
 *       													 OCResourceCollectionPayload **payload)
 * @test_data 		1. interfaceType (A interface type that is being queried)
 * 					2. resourceType  (A resource type that is being queried)
 * 					3. payload 		 (A payload of the matching resource type or interface type or NULL)
 * 					4. addr 		 (A device address)
 * @pre_condition 	1. Call OCRDStart initializing by OCInit
 * 					2. Call OCRDDiscover
 * 					3. Publish Resource to RD by OCRDPublish
 * @procedure 		call OCRDCheckPublishedResource
 * @post_condition 	OCRDStop
 * @expected 		OCRDCheckPublishedResource will succeed and return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(RDServerIntegrationTest_stc, RDCheckPublishedResource_SQV_P)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdCheckStartServerStatus(RD_START_SERVER_WITH_INIT);
		ASSERT_EQ(OC_STACK_OK,actualResult)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);

		m_rdHelper->startClient();
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
		
		while (!g_quitFlag)
		{
			CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
			IOTIVITYTEST_LOG(DEBUG, "g_quitFlag = %d",g_quitFlag);
		}

		OCStackResult resultCheckPublished = m_rdHelper->rdCheckPublishedResource(RD_RESOURCE_INTERFACE_TYPE_EMPTY,RD_RESOURCE_TYPE_LIGHT);
		EXPECT_EQ(OC_STACK_OK,resultCheckPublished)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(resultCheckPublished);

		m_rdHelper->joinThread();
		
		m_rdHelper->rdStopServer();
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
 * @see				OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see 			OCStackResult OCRDStart()
 * @see 			OCStackResult OCRDStop()
 * @objective 		Test OCRDCheckPublishedResource function without publishing resource to RD
 * @target 			OCStackResult OCStackResult OCRDCheckPublishedResource(const char *interfaceType, const char *resourceType,
        																   OCResourceCollectionPayload **payload);
 * @test_data 		1. interfaceType (A interface type that is being queried)
 * 					2. resourceType  (A resource type that is being queried)
 * 					3. payload 		 (A payload of the matching resource type or interface type or NULL)
 * 					4. addr 		 (A device address)
 * @pre_condition 	Call OCRDStart initializing by OCInit
 * @procedure 		Call OCRDCheckPublishedResource without publishing resource from client side
 * @post_condition 	OCRDStop
 * @expected 		OCRDCheckPublishedResource will not succeed and will return OC_STACK_ERROR
 * 					as we didn't published resource from client side
 */
#if defined(__LINUX__)
TEST_F(RDServerIntegrationTest_stc, RDCheckPublishedResourceWithoutPublish_DSCC_N)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdCheckStartServerStatus(RD_START_SERVER_WITH_INIT);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
		EXPECT_EQ(OC_STACK_OK,actualResult)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);

		OCStackResult resultCheckPublished = m_rdHelper->rdCheckPublishedResource(RD_RESOURCE_INTERFACE_TYPE_EMPTY,RD_RESOURCE_TYPE_THERMOSTAT);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
		EXPECT_EQ(OC_STACK_ERROR,resultCheckPublished)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(resultCheckPublished);

		m_rdHelper->rdStopServer();
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
 * @see				OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see 			OCStackResult OCRDStart()
 * @see 			OCStackResult OCRDStop()
 * @see				OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor)
 * @see				OCStackResult OCRDPublish(char *addr, uint16_t port, int num, ...)
 * @objective 		Test OCRDCheckPublishedResource function by calling multiple times with different resource
 * @target 			OCStackResult OCRDCheckPublishedResource(const char *interfaceType, const char *resourceType,
 *       													 OCResourceCollectionPayload **payload);
 * @test_data 		1. interfaceType (A interface type that is being queried)
 * 					2. resourceType  (A resource type that is being queried)
 * 					3. payload 		 (A payload of the matching resource type or interface type or NULL)
 * 					4. addr 		 (A device address)
 * @pre_condition 	1. Call OCRDStart initializing by OCInit
 * 					2. Call OCRDDiscover
 * 					3. Publish Resource to RD by OCRDPublish
 * @procedure 		Call OCRDCheckPublishedResource two times with different resource and check return values
 * @post_condition 	OCRDStop
 * @expected 		OCRDCheckPublishedResource will succeed and return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(RDServerIntegrationTest_stc, RDCheckPublishedResourceDifferentResourceMultiple_SQV_P)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdCheckStartServerStatus(RD_START_SERVER_WITH_INIT);
		EXPECT_EQ(OC_STACK_OK,actualResult)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);

		m_rdHelper->startClient();
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
		
		while (!g_quitFlag)
		{
			CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
			IOTIVITYTEST_LOG(DEBUG, "g_quitFlag = %d",g_quitFlag);
		}

		OCStackResult resultCheckPublished = m_rdHelper->rdCheckPublishedResource(RD_RESOURCE_INTERFACE_TYPE_EMPTY,RD_RESOURCE_TYPE_THERMOSTAT);
		EXPECT_EQ(OC_STACK_OK,resultCheckPublished)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(resultCheckPublished);
		
		OCStackResult resultCheckPublished1 = m_rdHelper->rdCheckPublishedResource(RD_RESOURCE_INTERFACE_TYPE_EMPTY,RD_RESOURCE_TYPE_LIGHT);
		EXPECT_EQ(OC_STACK_OK,resultCheckPublished1)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(resultCheckPublished1);

		m_rdHelper->joinThread();
		m_rdHelper->rdStopServer();
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
 * @objective 		Test OCRDStart function negatively without calling OCInit
 * @target 			OCStackResult OCRDStart();
 * @test_data 		none
 * @pre_condition 	none
 * @procedure 		Call OCRDStart and check return result
 * @post_condition 	OCStop
 * @expected 		OC_STACK_INVALID_PARAM should return as we did not initialized
 */
#if defined(__LINUX__)
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

/**
 * @since 			2015-08-26
 * @see				OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see 			OCStackResult OCRDStart()
 * @see 			OCStackResult OCRDStop()
 * @see				OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor)
 * @see				OCStackResult OCRDPublish(char *addr, uint16_t port, int num, ...)
 * @objective 		Test OCRDCheckPublishedResource function negatively for checking abcdefg(instead of valid resource)
 * @target 			OCStackResult OCStackResult OCRDCheckPublishedResource(const char *interfaceType, const char *resourceType,
 *       																   OCResourceCollectionPayload **payload);
 * @test_data 		1. interfaceType (A interface type that is being queried)
 * 					2. resourceType  (A resource type that is being queried)
 * 					3. payload 		 (A payload of the matching resource type or interface type or NULL)
 * 					4. addr 		 (A device address)
 * @pre_condition 	1. Call OCRDStart initializing by OCInit
 * 					2. Call OCRDDiscover
 * 					3. Publish Resource to RD by OCRDPublish
 * @procedure 		Call OCRDCheckPublishedResource with a invalid resources
 * @post_condition 	OCRDStop
 * @expected 		OCRDCheckPublishedResource will not succeed and will return OC_STACK_ERROR for invalid resource
 */
#if defined(__LINUX__)
TEST_F(RDServerIntegrationTest_stc, RDCheckPublishedWithInvalidResource_DICC_N)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdCheckStartServerStatus(RD_START_SERVER_WITH_INIT);
		EXPECT_EQ(OC_STACK_OK,actualResult)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);

		m_rdHelper->startClient();
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
		
		while (!g_quitFlag)
		{
			CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
			IOTIVITYTEST_LOG(DEBUG, "g_quitFlag = %d",g_quitFlag);
		}

		OCStackResult resultCheckPublished = m_rdHelper->rdCheckPublishedResource(RD_RESOURCE_INTERFACE_TYPE_EMPTY,RD_RESOURCE_TYPE_INVALID);
		EXPECT_EQ(OC_STACK_ERROR,resultCheckPublished)<< "Actual result string : " << CommonUtil::s_OCStackResultString.at(resultCheckPublished);

		m_rdHelper->joinThread();
		m_rdHelper->rdStopServer();
	}
	catch (std::exception &ex)
	{
		FAIL() << "Exception occurred : " << ex.what();
	}
	
    SUCCEED();
}
#endif
