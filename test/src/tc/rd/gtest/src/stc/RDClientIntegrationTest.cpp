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

class RDClientIntegrationTest_stc: public ::testing::Test
{
protected:

    RDHelper* m_rdHelper;

	RDClientIntegrationTest_stc()
	{
		m_rdHelper = nullptr;
	}
	
    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
		m_rdHelper = RDHelper::getInstance();
        
        PlatformConfig config { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0",0, OC::QualityOfService::LowQos };
		OCPlatform::Configure(config);
	
        m_rdHelper->registerLocalResources();
        CommonUtil::launchApp(SERVER);
        CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
        
    }

    virtual void TearDown()
    {
        m_rdHelper->unregisterLocalResources();
        
        CommonUtil::killApp(SERVER);
        CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
       	
       	free(m_rdHelper);
       	
       	CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 			2015-08-26
 * @see				void Configure(const PlatformConfig& config)
 * @see				OCStackResult registerResource(OCResourceHandle& resourceHandle,std::string& resourceURI,
 *						                           const std::string& resourceTypeName,const std::string& resourceInterface,
 *							                        EntityHandler entityHandler,uint8_t resourceProperty)
 * @see				OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @objective 		Test OCRDDiscover after starting server and check if callback is called
 * @target 			OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor)
 * @test_data 		none
 * @pre_condition 	1. Start Simulator(RD Server) by CommonUtil::launchApp(SERVER)
 * 					2. Call OCPlatform::Configure to overwrite the default configuration of the OCPlatform object
 * 					3. Call registerResource to registers a resource with the server
 * @procedure 		Call OCRDDiscover and check return value
 * @post_condition 	1. Stop RD Simulator by CommonUtil::killApp(SERVER)
 * 					2. Call unregisterResource to unregisters a resource with the server
 * @expected 		OCRDDiscover will succeed(callback will be called) and server address will be found
 */
#if defined(__LINUX__)
TEST_F(RDClientIntegrationTest_stc, RDDiscoverWithServer_CV_P)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdDiscover(RD_DISCOVER_WITH_PARAM,RD_DISCOVER_WITH_CALLBACK);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
		EXPECT_EQ(OC_STACK_OK,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
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
 * @see				void Configure(const PlatformConfig& config)
 * @see				OCStackResult registerResource(OCResourceHandle& resourceHandle,std::string& resourceURI,
 *						                           const std::string& resourceTypeName,const std::string& resourceInterface,
 *							                        EntityHandler entityHandler,uint8_t resourceProperty)
 * @see				OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @objective 		Test OCRDDiscover without starting server and check if server is found
 * @target 			OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor)
 * @test_data 		none
 * @pre_condition 	1. Start Simulator(RD Server) by CommonUtil::launchApp(SERVER)
 * 					2. Call OCPlatform::Configure to overwrite the default configuration of the OCPlatform object
 * 					3. Call registerResource to registers a resource with the server
 * @procedure 		Call OCRDDiscover
 * @post_condition 	1. Stop RD Simulator by CommonUtil::killApp(SERVER)
 * 					2. Call unregisterResource to unregisters a resource with the server
 * @expected 		OCRDDiscover will succeed but server address will not be found
 */
#if defined(__LINUX__)
TEST_F(RDClientIntegrationTest_stc, RDDiscoverWithoutServer_N)
{
	try
	{
		CommonUtil::killApp(SERVER);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		OCStackResult actualResult = m_rdHelper->rdDiscover(RD_DISCOVER_WITH_PARAM,RD_DISCOVER_WITH_CALLBACK);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
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
 * @see				void Configure(const PlatformConfig& config)
 * @see				OCStackResult registerResource(OCResourceHandle& resourceHandle,std::string& resourceURI,
 *						                           const std::string& resourceTypeName,const std::string& resourceInterface,
 *							                        EntityHandler entityHandler,uint8_t resourceProperty)
 * @see				OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see 			OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor)
 * @objective 		Test publishing resource after starting server
 * @target 			OCStackResult OCRDPublish(char *addr, uint16_t port, int num, ...)
 * @test_data 		none
 * @pre_condition 	1. Start Simulator(RD Server) by CommonUtil::launchApp(SERVER)
 * 					2. Call OCPlatform::Configure to overwrite the default configuration of the OCPlatform object
 * 					3. Call registerResource to registers a resource with the server
 * 					4. Call OCRDDiscover to find the RD server
 * @procedure 		Call OCRDPublish and check return result
 * @post_condition 	1. Stop RD Simulator by CommonUtil::killApp(SERVER)
 * 					2. Call unregisterResource to unregisters a resource with the server
 * @expected 		Publish will succeed and will return return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(RDClientIntegrationTest_stc, RDPublishWithServer_FSV_P)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdDiscover(RD_DISCOVER_WITH_PARAM,RD_DISCOVER_WITH_CALLBACK);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		EXPECT_EQ(OC_STACK_OK,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
		
		OCStackResult actualResultPublish = m_rdHelper->rdPublish();
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
		EXPECT_EQ(OC_STACK_OK,actualResultPublish) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResultPublish);
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
 * @see				void Configure(const PlatformConfig& config)
 * @see				OCStackResult registerResource(OCResourceHandle& resourceHandle,std::string& resourceURI,
 *						                           const std::string& resourceTypeName,const std::string& resourceInterface,
 *							                        EntityHandler entityHandler,uint8_t resourceProperty)
 * @see				OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see 			OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor)
 * @objective 		Test publishing resource multiple times
 * @target 			OCRDPublish();
 * @test_data 		none
 * @pre_condition 	1. Start Simulator(RD Server) by CommonUtil::launchApp(SERVER)
 * 					2. Call OCPlatform::Configure to overwrite the default configuration of the OCPlatform object
 * 					3. Call registerResource to registers a resource with the server
 * 					4. Call OCRDDiscover to find the RD server
 * @procedure 		Call OCRDPublish multiple times
 * @post_condition 	1. Stop RD Simulator by CommonUtil::killApp(SERVER)
 * 					2. Call unregisterResource to unregisters a resource with the server
 * @expected 		Publish will succeed and will return return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(RDClientIntegrationTest_stc, RDPublishMultipleTimes_FSV_P)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdDiscover(RD_DISCOVER_WITH_PARAM,RD_DISCOVER_WITH_CALLBACK);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		EXPECT_EQ(OC_STACK_OK,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);

		OCStackResult actualResultPublish = m_rdHelper->rdPublish();
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
		EXPECT_EQ(OC_STACK_OK,actualResultPublish) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResultPublish);
		
		OCStackResult actualResultPublish1 = m_rdHelper->rdPublish();
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
		EXPECT_EQ(OC_STACK_OK,actualResultPublish1) << CommonUtil::s_OCStackResultString.at(actualResultPublish1);
	}
	catch (std::exception &ex)
	{
		FAIL() << "Exception occurred : " << ex.what();
	}
	
    SUCCEED();
}
#endif

/**
 * @since 			2016-05-10
 * @see				void Configure(const PlatformConfig& config)
 * @see				OCStackResult registerResource(OCResourceHandle& resourceHandle,std::string& resourceURI,
 *						                           const std::string& resourceTypeName,const std::string& resourceInterface,
 *							                        EntityHandler entityHandler,uint8_t resourceProperty)
 * @see				OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see 			OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor)
 * @see				OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   						   OCConnectivityType connectivityType, FindCallback resourceHandler);
 * @objective 		Test publishing resource by findResource from OCPlatform
 * @target 			OCRDPublish();
 * @test_data 		none
 * @pre_condition 	1. Start Simulator(RD Server) by CommonUtil::launchApp(SERVER)
 * 					2. Call OCPlatform::Configure to overwrite the default configuration of the OCPlatform object
 * 					3. Call registerResource to registers a resource with the server
 * 					4. Call OCRDDiscover to find the RD server
 * @procedure 		1. Call OCRDPublish and checked return result.
 * 					2. Now call findResource for light type resource and check return result and callback values
 * 					3. Again call findResource for thermostat type resource and check return result and callback values
 * @post_condition 	1. Stop RD Simulator by CommonUtil::killApp(SERVER)
 * 					2. Call unregisterResource to unregisters a resource with the server
 * @expected 		Publish will succeed and will return return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(RDClientIntegrationTest_stc, RDPublishCheckedByFindResource_SQV_P)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdDiscover(RD_DISCOVER_WITH_PARAM,RD_DISCOVER_WITH_CALLBACK);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		EXPECT_EQ(OC_STACK_OK,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
		
		OCStackResult actualResultPublish = m_rdHelper->rdPublish();
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
		EXPECT_EQ(OC_STACK_OK,actualResultPublish) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResultPublish);

		g_foundRDResource = false;
		OCStackResult checkPublishedResourceLight = m_rdHelper->rdFindResourcesChecked(RD_RESOURCE_HOST_EMPTY,RD_RESOURCE_QUERY_URL_LIGHT,CT_DEFAULT);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
		EXPECT_EQ(OC_STACK_OK,checkPublishedResourceLight) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(checkPublishedResourceLight);
		ASSERT_TRUE(g_foundRDResource) << "Published resource light is not found by foundResource API.";

		g_foundRDResource = false;
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		OCStackResult checkPublishedResourceTharmo = m_rdHelper->rdFindResourcesChecked(RD_RESOURCE_HOST_EMPTY,RD_RESOURCE_QUERY_URL_THERMOSTAT,CT_DEFAULT);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
		EXPECT_EQ(OC_STACK_OK,checkPublishedResourceTharmo) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(checkPublishedResourceTharmo);
		ASSERT_TRUE(g_foundRDResource) << "Published resource thermostat is not found by foundResource API.";
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
 * @see				void Configure(const PlatformConfig& config)
 * @see				OCStackResult registerResource(OCResourceHandle& resourceHandle,std::string& resourceURI,
 *						                           const std::string& resourceTypeName,const std::string& resourceInterface,
 *							                        EntityHandler entityHandler,uint8_t resourceProperty)
 * @see				OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @objective 		Test OCRDDiscover multiples time's by which API can update searched data
 * 					list properly without any memory dump
 * @target 			OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor)
 * @test_data 		none
 * @pre_condition 	1. Start Simulator(RD Server) by CommonUtil::launchApp(SERVER)
 * 					2. Call OCPlatform::Configure to overwrite the default configuration of the OCPlatform object
 * 					3. Call registerResource to registers a resource with the server
 * @procedure 		Call OCRDDiscover multiple time's and check return value
 * @post_condition 	1. Stop RD Simulator by CommonUtil::killApp(SERVER)
 * 					2. Call unregisterResource to unregisters a resource with the server
 * @expected 		OCRDDiscover will succeed(callback will be called) and server address will be found multiple times
 */
#if defined(__LINUX__)
TEST_F(RDClientIntegrationTest_stc, RDDiscoverWithServer_VLCC_P)
{
	int  loopIndex = 1;
	try
	{
		for(loopIndex = 1; loopIndex <= RD_MAX_LOOP_COUNT_VLCC; loopIndex++)
		{
			OCStackResult actualResult = m_rdHelper->rdDiscover(RD_DISCOVER_WITH_PARAM,RD_DISCOVER_WITH_CALLBACK);
			CommonUtil::waitInSecond(RD_SLEEPTIME_MIN);
			EXPECT_EQ(OC_STACK_OK,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
		}
	}
	catch (std::exception &ex)
	{
		 IOTIVITYTEST_LOG(INFO, "Exception occurr's: %s ",ex.what());
		 FAIL() << "Exception occurred due to call rdDiscover " << loopIndex << " times ";
	}

	SUCCEED();
}
#endif

/**
 * @since 			2015-08-26
 * @see				void Configure(const PlatformConfig& config)
 * @see				OCStackResult registerResource(OCResourceHandle& resourceHandle,std::string& resourceURI,
 *						                           const std::string& resourceTypeName,const std::string& resourceInterface,
 *							                        EntityHandler entityHandler,uint8_t resourceProperty)
 * @see				OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @objective 		Test OCRDDiscover function by positive basic way
 * @target 			OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor)
 * @test_data 		OCRDBiasFactorCB (It's a callback function that invoked when bias factor
 * 									  is returned by the Resource Directory Server)
 * @pre_condition 	1. Call OCPlatform::Configure to overwrite the default configuration of the OCPlatform object
 * 					2. Call registerResource to registers a resource with the server
 * @procedure 		Call OCRDDiscover
 * @post_condition 	Call unregisterResource to unregisters a resource with the server
 * @expected 		OCRDDiscover will succeed and return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(RDClientIntegrationTest_stc, RDDiscover_SRC_P)
{	
	try
	{
		OCStackResult actualResult = m_rdHelper->rdDiscover(RD_DISCOVER_WITH_PARAM,RD_DISCOVER_WITHOUT_CALLBACK);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		EXPECT_EQ(OC_STACK_OK,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
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
 * @see				void Configure(const PlatformConfig& config)
 * @see				OCStackResult registerResource(OCResourceHandle& resourceHandle,std::string& resourceURI,
 *						                           const std::string& resourceTypeName,const std::string& resourceInterface,
 *							                        EntityHandler entityHandler,uint8_t resourceProperty)
 * @see				OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @objective 		Test OCRDPublish function positively
 * @target 			OCStackResult OCRDPublish(char *addr, uint16_t port, int num, ...);
 * @test_data 		1. addr (The IP address of the RD)
 * 					2. port (The port of the RD)
 * 					3. num  (This denotes the number of registered resource handles being passed for the remaining arguments)
 * @pre_condition 	1. Call OCPlatform::Configure to overwrite the default configuration of the OCPlatform object
 * 					2. Call registerResource to registers a resource with the server
 * 					3. rdDiscover
 * @procedure 		Call OCRDPublish
 * @post_condition 	Call unregisterResource to unregisters a resource with the server
 * @expected 		OCRDPublish will succeed and return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(RDClientIntegrationTest_stc, RDPublish_SRC_P)
{
	try
	{
		char *ipAddressOfRD="default";
		
		OCStackResult actualResultDis = m_rdHelper->rdDiscover(RD_DISCOVER_WITH_PARAM,RD_DISCOVER_WITHOUT_CALLBACK);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		EXPECT_EQ(OC_STACK_OK,actualResultDis) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResultDis);
		
		OCStackResult actualResult = m_rdHelper->rdPublish(ipAddressOfRD,RD_UNIT16_PORT,RD_NUMBER_OF_REGISTER_RESOURCE);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		EXPECT_EQ(OC_STACK_OK,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
	}
	catch (std::exception &ex)
	{
		FAIL() << "Exception occurred : " << ex.what();
	}

  	SUCCEED();
}
#endif
