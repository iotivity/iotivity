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

class RDClientTest_btc: public ::testing::Test
{
protected:

    RDHelper* m_rdHelper;

	RDClientTest_btc()
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
   	    CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        m_rdHelper->unregisterLocalResources();
        free(m_rdHelper);
    }
};


/**
 * @since 			2015-08-26
 * @see				void Configure(const PlatformConfig& config)
 * @see				OCStackResult registerResource(OCResourceHandle& resourceHandle,std::string& resourceURI,
 *						                           const std::string& resourceTypeName,const std::string& resourceInterface,
 *							                        EntityHandler entityHandler,uint8_t resourceProperty)
 * @see				OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @objective 		Test OCRDPublish function with empty ip address that will return OC_STACK_NO_MEMORY
 * @target 			OCStackResult OCRDPublish(char *addr, uint16_t port, int num, ...);
 * @test_data 		1. addr (The IP address of the RD) as empty
 * 					2. port (The port of the RD) as NULL
 * 					3. num as 0 (This denotes the number of registered resource handles being passed for the remaining arguments)
 * @pre_condition 	1. Call OCPlatform::Configure to overwrite the default configuration of the OCPlatform object
 * 					2. Call registerResource to registers a resource with the server
 * @procedure 		Call OCRDPublish with empty address,null port and number of 0 resource
 * @post_condition 	Call unregisterResource to unregisters a resource with the server
 * @expected 		OCRDPublish will not succeed and will return OC_STACK_NO_MEMORY
 */
#if defined(__LINUX__)
TEST_F(RDClientTest_btc, RDPublishPort_NV_N)
{
	try
	{
		char *ipAddressOfRD="";

		OCStackResult actualResult = m_rdHelper->rdPublish(ipAddressOfRD,RD_UNIT16_PORT_NULL,RD_NUMBER_OF_REGISTER_RESOURCE_ZERO);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		
		EXPECT_EQ(OC_STACK_NO_MEMORY,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
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
 * @objective 		Test OCRDPublish function negatively with null value
 * @target 			OCStackResult OCRDPublish(char *addr, uint16_t port, int num, ...);
 * @test_data 		1. addr (The IP address of the RD) as NULL
 * 					2. port The port of the RD
 * 					3. num This denotes the number of registered resource handles being passed for the remaining arguments
 * @pre_condition 	1. Call OCPlatform::Configure to overwrite the default configuration of the OCPlatform object
 * 					2. Call registerResource to registers a resource with the server
 * @procedure 		Call OCRDPublish with null address
 * @post_condition 	Call unregisterResource to unregisters a resource with the server
 * @expected 		OCRDPublish will not succeed and not return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(RDClientTest_btc, RDPublishAddress_NV_N)
{
	try
	{
		char *ipAddressOfRD=NULL;

		OCStackResult actualResult = m_rdHelper->rdPublish(ipAddressOfRD,RD_UNIT16_PORT,RD_NUMBER_OF_REGISTER_RESOURCE_ZERO);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		
		EXPECT_EQ(OC_STACK_INVALID_PARAM,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
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
 * @objective 		Test publishing resource without registerResource that will return OC_STACK_NO_MEMORY
 * @target 			OCRDPublish();
 * @test_data 		1. addr (addr The IP address of the RD, it could be either retrieved via OCRDDiscover())
 * 					2. port The port of the RD
 * 					3. num This denotes the number of registered resource handles being passed for the remaining arguments
 * @pre_condition 	1. Call OCPlatform::Configure to overwrite the default configuration of the OCPlatform object
 * 					3. Call unregisterResource to unregisters a resource with the server
 * 					4. Call OCRDDiscover to find the RD server
 * @procedure 		Call OCRDPublish
 * @post_condition 	Call registerResource to registers a resource with the server that will be unregister in tear down
 * @expected 		Publish will not succeed and will return OC_STACK_NO_MEMORY
 */
#if defined(__LINUX__)
TEST_F(RDClientTest_btc, RDPublishWithoutRegisterRD_NV_N)
{
	try
	{
		m_rdHelper->unregisterLocalResources();
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		
		OCStackResult actualResultPublish = m_rdHelper->rdPublish();
		EXPECT_EQ(OC_STACK_NO_MEMORY,actualResultPublish) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResultPublish);

		m_rdHelper->registerLocalResources();
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
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
 * @objective 		Test OCRDDiscover function with NULL callback function
 * @target 			OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor)
 * @test_data 		OCRDBiasFactorCB (It's a callback function that invoked when bias factor
 * 									  is returned by the Resource Directory Server)
 * @pre_condition 	1. Call OCPlatform::Configure to overwrite the default configuration of the OCPlatform object
 * 					2. Call registerResource to registers a resource with the server
 * @procedure 		Call OCRDDiscover with NULL argument
 * @post_condition 	Call unregisterResource to unregisters a resource with the server
 * @expected 		OCRDDiscover will not succeed and will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(RDClientTest_btc, RDDiscover_USV_N)
{
	try
	{
		OCStackResult actualResult = m_rdHelper->rdDiscover(RD_DISCOVER_WITH_INVALID_PARAM,RD_DISCOVER_WITHOUT_CALLBACK);
		CommonUtil::waitInSecond(RD_SLEEPTIME_MAX);
		EXPECT_EQ(OC_STACK_INVALID_CALLBACK,actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(actualResult);
	}
	catch (std::exception &ex)
	{
		FAIL() << "Exception occurred : " << ex.what();
	}
	
	SUCCEED();
}
#endif
