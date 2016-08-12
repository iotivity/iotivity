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

#include "TMHelper.h"

namespace PH = std::placeholders;

class TMThingsConfigurationAPITest_stc: public ::testing::Test {
protected:
	OCStackResult m_result;
	std::string m_errorMessage;
	TMHelper* m_pTMHelper;

protected:
	virtual void SetUp() {
		CommonUtil::runCommonTCSetUpPart();

#ifdef __LINUX__
		CommonUtil::killApp(RESOURCE_APP);
		CommonUtil::killApp(COLLECTION_APP);

		CommonUtil::launchApp(RESOURCE_APP);
		CommonUtil::launchApp(COLLECTION_APP);
#endif

		m_errorMessage = "";
		m_pTMHelper = TMHelper::getInstance();

		CommonUtil::waitInSecond(INT_TWO);
		auto testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
		IOTIVITYTEST_LOG(INFO, "%s.%s Set Up -- finished",
				testInfo->test_case_name(), testInfo->name());
	}

	virtual void TearDown() {

#ifdef __LINUX__
		CommonUtil::killApp(RESOURCE_APP);
		CommonUtil::killApp(COLLECTION_APP);
#endif

		auto testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
		IOTIVITYTEST_LOG(INFO, "%s.%s Tear Down -- finished",
				testInfo->test_case_name(), testInfo->name());
		CommonUtil::runCommonTCTearDownPart();
	}

};

/**
 * @since 2014-12-11
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsConfiguration* getInstance();
 * @objective to test if getConfigurations API can successfully return the configuration of
 * 		single resource
 * @target OCStackResult getConfigurations(std::shared_ptr< OCResource > resource,
 *              std::vector<ConfigurationName> configurations, ConfigurationCallback callback)
 * @test_data 		1. resource single resource or collection resource
 *            		2. configurations List of configurations key
 *            		3. callback Callback function to be called
 * @pre_condition several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * @procedure 		1. Create a resource type list using core.light
 *            		2. Perform findCandidateResources using that resource type list
 *            		3. Wait for the callback to get the found resource list
 *            		4. Create configuration name list
 *            		5. Get instance of ThingsConfiguration
 *            		6. Perform getConfigurations on that resource, using the configuration name list
 *            		7. Check whether callback was called
 *            		8. Check whether callback returns desired error code
 *            		9. Check the returned representation whether the configuration is actually
 *               		valid value or null
 * @post_condition Close the apps
 * @expected 		1. getConfigurations API returns OC_STACK_OK
 *           		2. The configuration found from callback has valid values
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationAPITest_stc, GetConfigurationsOfSingleResource_SQV_P)
{
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_LIGHT);

	bool result = m_pTMHelper->findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK,
			RESOURCE_COUNT_LIGHT, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
		return;
	}

	string expectedValue = "off";
	result = m_pTMHelper->getConfigurationAndVerify(m_pTMHelper->getResourceList().at(INT_ZERO),
			OC_STACK_OK, true, INT_ZERO, expectedValue, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
	}

	SUCCEED();
}
#endif

/**
 * @since 2015-01-23
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsConfiguration* getInstance();
 * @objective to test if getConfigurations API can successfully get the configuration of an
 * 		unavailable resource
 * @target OCStackResult getConfigurations(std::shared_ptr< OCResource > resource,
 *              std::vector<ConfigurationName> configurations, ConfigurationCallback callback)
 * @test_data 		1. resource single resource or collection resource
 *            		2. configurations List of configurations key
 *            		3. callback Callback function to be called
 * @pre_condition several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * @procedure 		1. Create a resource type list using a core.light
 *            		2. Perform findCandidateResources() using the resource type list
 *            		3. Wait for callback to get found resource list
 *            		4. Create a configuration name list using 'power' & 'intensity'
 *            		5. Stop the process that was running the resources
 *            		6. Get instance of ThingsConfiguration
 *            		7. Perform getConfigurations() using a resource from the found resource list
 *               		& configuration name list
 *            		8. Wait for the configuration callback to get error code
 * @post_condition Close the apps
 * @expected 		1. getConfigurations API should return OC_STACK_OK
 *           		2. The configuration callback should return error code -1
 **/
#if defined(__LINUX__)
TEST_F(TMThingsConfigurationAPITest_stc, GetConfigurationsOfUnavailableResource_DSCC_N)
{
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_LIGHT);

	bool result = m_pTMHelper->findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK,
			RESOURCE_COUNT_LIGHT, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
		return;
	}

	CommonUtil::killApp(RESOURCE_APP);
	CommonUtil::waitInSecond(INT_THREE);

	string expectedValue = "";
	result = m_pTMHelper->getConfigurationAndVerify(m_pTMHelper->getResourceList().at(INT_ZERO),
			OC_STACK_OK, false, INT_MINUS_ONE, expectedValue, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-17
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsConfiguration* getInstance();
 * @see OCStackResult getConfigurations(std::shared_ptr< OCResource > resource,
 *              std::vector<ConfigurationName> configurations, ConfigurationCallback callback)
 * @objective to test if updateConfigurations API can successfully update the configuration of a resource
 * @target OCStackResult updateConfigurations(std::shared_ptr< OCResource > resource,
 *               std::map< ConfigurationName, ConfigurationValue > configurations,
 *               ConfigurationCallback callback);
 * @test_data 		1. resource single resource or collection resource
 *            		2. configurations map of configuration key and value
 *            		3. callback Callback function to be called
 * @pre_condition several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * @procedure 		1. Create a resource type list using core.light
 *            		2. Perform findCandidateResources using that resource type list
 *            		3. Wait for the callback to get found resource list
 *            		4. Get instance of ThingsConfiguration
 *					5. Create a configuration using 'power' = 'on'
 *            		6. Perform updateConfigurations on the found resource
 *            		7. Check whether callback was called
 *            		8. Check whether callback returns desired error code
 *            		9. Check the returned representation whether the configuration is actually
 *               		updated (using getValue())
 * @post_condition Close the apps
 * @expected 		1. updateConfigurations API should return OC_STACK_OK
 *           		2. The callback should return with error code 0
 *           		3. The configuration should be updated as 'power' = 'on'
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationAPITest_stc, UpdateConfigurationsOfSingleResource_SQV_P)
{
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_LIGHT);

	bool result = m_pTMHelper->findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK,
			RESOURCE_COUNT_LIGHT, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
		return;
	}

	string toUpdateValue = "on";
	result = m_pTMHelper->updateConfigurationAndVerify(m_pTMHelper->getResourceList().at(INT_ZERO),
			OC_STACK_OK, true, INT_ZERO, toUpdateValue, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
	}

	SUCCEED();
}
#endif

/**
 * @since 2015-01-21
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsConfiguration* getInstance();
 * @objective to test if updateConfigurations API can successfully update the configuration of an unavailable
 *              resource
 * @target OCStackResult updateConfigurations(std::shared_ptr< OCResource > resource,
 *               std::map< ConfigurationName, ConfigurationValue > configurations,
 *               ConfigurationCallback callback);
 * @test_data 		1. resource single resource or collection resource
 *            		2. configurations map of configuration key and value
 *            		3. callback Callback function to be called
 * @pre_condition several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * @procedure 		1. Create a resource list using a resource type(core.light)
 *            		2. Find the resource using findCandidateResources()
 *            		3. Wait for callback to get found resource list
 *            		4. Get instance of ThingsConfiguration
 *            		5. Create a configuration to update
 *            		6. Kill the process that is running the resources
 *            		7. Perform updateConfigurations() using a resource from the resource list & configuration
 * @post_condition Close the apps
 * @expected 		1. updateConfigurations API should return OC_STACK_OK
 *           		2. The callback should return error code -1
 **/
#if defined(__LINUX__)
TEST_F(TMThingsConfigurationAPITest_stc, UpdateConfigurationsOfUnavailableResource_DSCC_N)
{
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_LIGHT);

	bool result = m_pTMHelper->findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK,
			RESOURCE_COUNT_LIGHT, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
		return;
	}

	CommonUtil::killApp(RESOURCE_APP);
	CommonUtil::waitInSecond(INT_THREE);

	string toUpdateValue = "";
	result = m_pTMHelper->updateConfigurationAndVerify(m_pTMHelper->getResourceList().at(INT_ZERO),
			OC_STACK_OK, false, INT_MINUS_ONE, toUpdateValue, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-17
 * @see void Configure(const PlatformConfig& config);
 * @objective to test if doBootstrap API can successfully do bootstrapping from bootstrap server
 * @target OCStackResult doBootstrap(ConfigurationCallback callback)
 * @test_data None
 * @pre_condition Start the bootstrap server app
 * @procedure		1. Get instance of ThingsConfiguration
 * 					2. Perform doBootstrap()
 *            		3. Wait for bootstrapping to be finished
 *            		4. Verify from callback function
 * @post_condition Stop the bootstrap server app
 * @expected 		1. doBootstrap API should return OC_STACK_OK
 *           		2. The callback should be called with error code = 0
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationAPITest_stc, DoBootstrap_SQV_P)
{

#ifdef __LINUX__
	CommonUtil::launchApp(BOOTSTRAP_APP);
	CommonUtil::waitInSecond(INT_TWO);
#endif

	OCStackResult expectedResult = OC_STACK_OK;
	int expectedErrrorCode = SUCCESS_RESPONSE;
	bool result = m_pTMHelper->doBootstrapAndVerify(expectedResult, expectedErrrorCode,
			m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
		return;
	}

#ifdef __LINUX__
	CommonUtil::killApp(BOOTSTRAP_APP);
#endif

	SUCCEED();
}
#endif

/**
 * @since 2015-01-26
 * @see void Configure(const PlatformConfig& config);
 * @objective to test if doBootstrap API can successfully bootstrap without bootstrap server
 * @target OCStackResult doBootstrap(ConfigurationCallback callback)
 * @test_data None
 * @pre_condition several types of resources running in RESOURCE_APP and
 * 		COLLECTION_APP and no bootstrapserver is running
 * @procedure 		1. Get instance of ThingsConfiguration
 * 					2. Perform doBootstrap()
 *            		3. Wait for bootstrapping to be finished
 *            		4. Verify from callback function
 * @post_condition Close the apps
 * @expected 		1. doBootstrap API should return OC_STACK_OK
 *           		2. The callback should be returned with error code = -1
 **/
#if defined(__LINUX__)
TEST_F(TMThingsConfigurationAPITest_stc, DoBootstrapToUnsupportedResource_DSCC_N)
{
	OCStackResult expectedResult = OC_STACK_OK;
	int expectedErrrorCode = -1;

	bool result = m_pTMHelper->doBootstrapAndVerify(expectedResult, expectedErrrorCode,
			m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
		return;
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-17
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @objective to test if reboot API can successfully reboot a resource
 * @target OCStackResult reboot(std::shared_ptr< OCResource > resource, ConfigurationCallback callback)
 * @test_data 		1. resource Pointer of the resource to factory reset
 *            		2. callback Callback function to be called
 * @pre_condition several types of resources running in RESOURCE_APP and
 * 		COLLECTION_APP
 * @procedure 		1. Create a resource type list using core.light
 *            		2. Perform findCandidateResources() API using the list
 *            		3. Wait for the callback to get the found resource list
 *            		4. Get instance of ThingsMaintenance
 *            		5. Perform reboot() API on one of the found resources
 *            		6. Check whether callback is called
 *            		7. Check the error code from the callback
 *            		8. check representation for reboot state
 * @post_condition Close the apps
 * @expected 		1. reboot API should return OC_STACK_OK
 *           		2. From the callback, representation reboot state should be true
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationAPITest_stc, RebootResource_SQV_P)
{
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_LIGHT);

	bool result = m_pTMHelper->findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK,
			RESOURCE_COUNT_LIGHT, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
		return;
	}

	int expectedErrorCode = SUCCESS_RESPONSE;
	string expectedValue = "true";

	result = m_pTMHelper->rebootAndVerify(m_pTMHelper->getResourceList().at(INT_ZERO), OC_STACK_OK,
			expectedErrorCode, expectedValue, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
	}

	SUCCEED();
}
#endif

/**
 * @since 2015-01-28
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @objective to test if reboot API can successfully reboot with unsupported resource
 * @target OCStackResult reboot(std::shared_ptr< OCResource > resource, ConfigurationCallback callback)
 * @test_data 		1. resource Pointer of the resource to factory reset
 *            		2. callback Callback function to be called
 * @pre_condition several types of resources running in RESOURCE_APP and
 * 			COLLECTION_APP
 * @procedure 		1. Create a resource type list using core.dummy
 *            		2. Start a server of unsupported type(core.dummy)
 *            		3. Perform findCandidateResources() API using the list
 *            		4. Get instance of ThingsMaintenance
 *            		5. Perform reboot() API on one of the found resources
 *            		6. Check whether callback is called
 *            		7. Check the error code from the callback
 *            		8. check representation for reboot state
 * @post_condition Close the apps
 * @expected		1. reboot API should return OC_STACK_OK
 *           		2. From the callback, the reboot state should be empty
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationAPITest_stc, RebootUnsupportedResource_DSCC_N)
{
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_DUMMY);

#ifdef __LINUX__
	CommonUtil::killApp(UNSUPPORTIVE_RESOURCE_APP);
	CommonUtil::waitInSecond(INT_TWO);
	CommonUtil::launchApp(UNSUPPORTIVE_RESOURCE_APP);
	CommonUtil::waitInSecond(INT_TWO);
#endif

	bool result = m_pTMHelper->findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK,
			RESOURCE_COUNT_LIGHT, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
		return;
	}

	int expectedErrorCode = SUCCESS_RESPONSE;
	string expectedValue = "";

	result = m_pTMHelper->rebootAndVerify(m_pTMHelper->getResourceList().at(INT_ZERO), OC_STACK_OK,
			expectedErrorCode, expectedValue, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
	}

#ifdef __LINUX__
	CommonUtil::killApp(UNSUPPORTIVE_RESOURCE_APP);
#endif

	SUCCEED();
}
#endif

/**
 * @since 2014-12-17
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see OCStackResult updateConfigurations(std::shared_ptr< OCResource > resource,
 *               std::map< ConfigurationName, ConfigurationValue > configurations,
 *               ConfigurationCallback callback);
 * @objective to test if factoryReset API can factoryReset successfully
 * @target OCStackResult factoryReset(std::shared_ptr< OCResource > resource, ConfigurationCallback callback)
 * @test_data 		1. resource Pointer of the resource to factory reset
 *            		2. callback Callback function to receive updated state
 * @pre_condition several types of resources running in RESOURCE_APP and
 * 			COLLECTION_APP
 * @procedure 		1. Create a list using core.light type resource
 *            		2. Perform findCandidateResources() API using the list
 * 		           	3. Get instance of ThingsConfiguration
 *            		4. Create a configuration using 'power' = 'on'
 *            		5. Perform updateConfiguration() on one of the found resources
 *            		6. Check whether callback is called
 *            		7. Check the error code from the callback
 *            		8. check representation for all attribute state has changed
 *            		9. Get instance of ThingsMaintenance
 *            		10. Perform factoryReset() API on  the same resource
 *            		11. Check whether callback is called
 *            		12.	Check the error code from the callback
 *            		13. check representation for all attribute state has reverted to default value
 * @post_condition Close the apps
 * @expected 		1. factoryReset API should return OC_STACK_OK
 *           		2. From the callback, all attribute value will be equal to default value
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationAPITest_stc, FactoryReset_SQV_P)
{
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_LIGHT);

	bool result = m_pTMHelper->findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK,
			RESOURCE_COUNT_LIGHT, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
		return;
	}

	string toUpdateValue = "on";

	result = m_pTMHelper->updateConfigurationAndVerify(m_pTMHelper->getResourceList().at(INT_ZERO),
			OC_STACK_OK, true, INT_ZERO, toUpdateValue, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
	}

	int expectedErrorCode = -0;

	result = m_pTMHelper->factoryResetAndVerify(m_pTMHelper->getResourceList().at(INT_ZERO), OC_STACK_OK,
			expectedErrorCode, true, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-01-28
 * @see void Configure(const PlatformConfig& config);
 * @see OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1)
 * @objective to test if factoryReset API can successfully factory reset on unsupported resource
 * @target OCStackResult factoryReset(std::shared_ptr< OCResource > resource, ConfigurationCallback callback)
 * @test_data 		1. resource Pointer of the resource to factory reset
 *            		2. callback Callback function to receive updated state
 * @pre_condition several types of resources running in RESOURCE_APP and
 * 			COLLECTION_APP
 * @procedure 		1. Create a resource type list using core.dummy
 *            		2. Start a server of unsupported type
 *            		3. Perform findCandidateResources() API using the list
 *            		4. Get instance of ThingsMaintenance
 *            		5. Perform factoryReset() API on one of the found resources
 *            		6. Check whether callback is called
 *            		7. Check the error code from the callback
 *            		8. check representation for all attribute
 * @post_condition Close the apps
 * @expected 		1. factoryReset API should return OC_STACK_OK
 *           		2. From the callback, all attribute value will be equal to default value
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationAPITest_stc, FactoryResetToUnsupportedResource_DSCC_N)
{
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_DUMMY);

#ifdef __LINUX__
	CommonUtil::killApp(UNSUPPORTIVE_RESOURCE_APP);
	CommonUtil::waitInSecond(INT_TWO);
	CommonUtil::launchApp(UNSUPPORTIVE_RESOURCE_APP);
	CommonUtil::waitInSecond(INT_TWO);
#endif

	bool result = m_pTMHelper->findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK,
			RESOURCE_COUNT_LIGHT, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
		return;
	}

	result = m_pTMHelper->factoryResetAndVerify(m_pTMHelper->getResourceList().at(INT_ZERO), OC_STACK_OK,
			INT_ZERO, false, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
	}

#ifdef __LINUX__
	CommonUtil::killApp(UNSUPPORTIVE_RESOURCE_APP);
#endif

	SUCCEED();
}
#endif
