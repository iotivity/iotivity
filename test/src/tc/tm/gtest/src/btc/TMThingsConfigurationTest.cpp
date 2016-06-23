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

class TMThingsConfigurationTest_btc: public ::testing::Test {
protected:
	TMHelper* m_pTMHelper;
	std::string m_errorMsg;
	std::vector<std::string> resourceTypes;
	std::vector<std::shared_ptr<OCResource> > foundCollectionResourceList;

protected:
	virtual void SetUp() {
		CommonUtil::runCommonTCSetUpPart();

#ifdef __LINUX__
		CommonUtil::killApp(RESOURCE_APP);
		CommonUtil::killApp(COLLECTION_APP);
		CommonUtil::waitInSecond(APP_WAIT);

		CommonUtil::launchApp(RESOURCE_APP);
		CommonUtil::launchApp(COLLECTION_APP);
		CommonUtil::waitInSecond(APP_WAIT);
#endif

		m_pTMHelper = TMHelper::getInstance();
		resourceTypes.clear();
		resourceTypes.push_back(RESOURCE_TYPE_FAN);
		foundCollectionResourceList.clear();
		m_errorMsg = "";
	}

	virtual void TearDown() {

#ifdef __LINUX__
		CommonUtil::killApp(RESOURCE_APP);
		CommonUtil::killApp(COLLECTION_APP);
#endif

		CommonUtil::runCommonTCTearDownPart();
	}

public:
	void onConfiguration(const HeaderOptions& headerOptions,
			const OCRepresentation& rep, const int eCode) {
	}

};

/**
 * @since 2014-12-11
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsMaintenance* getInstance();
 * @objective Test 'factoryReset' function with positive basic way
 * @target OCStackResult factoryReset(std::shared_ptr< OCResource > resource,
 *              MaintenanceCallback callback);
 * @test_data 		1. resource Pointer of the resource to factory reset
 *            		2. callback Callback function to be called
 * @pre_condition	1. several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * 					2. find resource
 *                	3. Get instance of ThingsMaintenance
 * @procedure Perform factoryReset and check if OC_STACK_OK is returned
 * @post_condition Close the apps
 * @expected OC_STACK_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationTest_btc, FactoryReset_SRC_P)
{
	if (!m_pTMHelper->findCandidateResourcesAndVerify(resourceTypes, OC_STACK_OK, RESOURCE_COUNT_FAN, m_errorMsg))
	{
		SET_FAILURE("Precondition Failed And " + m_errorMsg);
		return;
	}

	OCStackResult result = m_pTMHelper->m_pThingsMaintenance->factoryReset(m_pTMHelper->getResourceList().at(INT_ZERO),
			std::bind(&TMThingsConfigurationTest_btc::onConfiguration, this, std::placeholders::_1,
					std::placeholders::_2, std::placeholders::_3));

	if (result != OC_STACK_OK)
	{
		SET_FAILURE("Failed to do factory reset. Result is: " + std::to_string(result));
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-11
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsMaintenance* getInstance();
 * @objective Test 'factoryReset' function with negative basic way using null resource
 * @target OCStackResult factoryReset(std::shared_ptr< OCResource > resource,
 *              MaintenanceCallback callback);
 * @test_data 		1. resource Null
 *            		2. callback Callback function to be called
 * @pre_condition 	1. several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 *                	2. Get instance of ThingsMaintenance
 * @procedure Perform factoryReset with null resource and check if
 * 		OC_STACK_OK is not returned
 * @post_condition Close the apps
 * @expected OC_STACK_OK is not returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationTest_btc, FactoryReset_NV_N)
{
	OCStackResult result = m_pTMHelper->m_pThingsMaintenance->factoryReset(NULL,
			std::bind(&TMThingsConfigurationTest_btc::onConfiguration, this, std::placeholders::_1,
					std::placeholders::_2, std::placeholders::_3));

	if (result == OC_STACK_OK)
	{
		SET_FAILURE("Failed to test negative case. Done factory reset. Result is: "
				+ std::to_string(result));
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-11
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsMaintenance* getInstance();
 * @objective Test 'reboot' function with positive basic way
 * @target OCStackResult reboot(std::shared_ptr< OCResource > resource, MaintenanceCallback callback);
 * @test_data 		1. resource Pointer of the resource to factory reset
 *            		2. callback Callback function to be called
 * @pre_condition 	1. several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * 					2. find resource
 *                	3. Get instance of ThingsMaintenance
 * @procedure Perform reboot and check if OC_STACK_OK is returned
 * @post_condition Close the apps
 * @expected OC_STACK_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationTest_btc, Reboot_SRC_P)
{
	if (!m_pTMHelper->findCandidateResourcesAndVerify(resourceTypes, OC_STACK_OK, RESOURCE_COUNT_FAN, m_errorMsg))
	{
		SET_FAILURE("Precondition Failed And " + m_errorMsg);
		return;
	}

	OCStackResult result = m_pTMHelper->m_pThingsMaintenance->reboot(m_pTMHelper->getResourceList().at(INT_ZERO),
			std::bind(&TMThingsConfigurationTest_btc::onConfiguration, this, std::placeholders::_1,
					std::placeholders::_2, std::placeholders::_3));

	if (result != OC_STACK_OK)
	{
		SET_FAILURE("Failed to reboot. Result is: " + std::to_string(result));
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-11
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsMaintenance* getInstance();
 * @objective Test 'reboot' function with negative basic way using null resource
 * @target OCStackResult reboot(std::shared_ptr< OCResource > resource, MaintenanceCallback callback);
 * @test_data 		1. resource Null
 *            		2. callback Callback function to be called
 * @pre_condition 	1. several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 *                	2. Get instance of ThingsMaintenance
 * @procedure Perform reboot with null resource and check if OC_STACK_OK is not returned
 * @post_condition Close the apps
 * @expected OC_STACK_OK is not returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationTest_btc, Reboot_NV_N)
{
	OCStackResult result = m_pTMHelper->m_pThingsMaintenance->reboot(NULL,
			std::bind(&TMThingsConfigurationTest_btc::onConfiguration, this, std::placeholders::_1,
					std::placeholders::_2, std::placeholders::_3));

	if (result == OC_STACK_OK)
	{
		SET_FAILURE("Failed to test negative case. Rebooted. Result is: " + std::to_string(result));
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-11
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsConfiguration* getInstance();
 * @objective Test 'updateConfigurations' function with positive basic way
 * @target OCStackResult updateConfigurations(std::shared_ptr< OCResource > resource,
 *               std::map< ConfigurationName, ConfigurationValue > configurations,
 *               ConfigurationCallback callback);
 * @test_data 		1. resource single resource or collection resource
 *            		2. configurations map of configuration key and value
 *            		3. callback Callback function to be called
 * @pre_condition 	1. several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * 					2. find resource
 *                	3. Get instance of ThingsConfiguration
 * @procedure Perform updateConfigurations and check if OC_STACK_OK is returned
 * @post_condition Close the apps
 * @expected OC_STACK_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationTest_btc, UpdateConfigurations_SRC_P)
{
	if (!m_pTMHelper->findCandidateResourcesAndVerify(resourceTypes, OC_STACK_OK, RESOURCE_COUNT_FAN, m_errorMsg))
	{
		SET_FAILURE("Precondition Failed And " + m_errorMsg);
		return;
	}

	std::map < std::string, std::string > configurations;
	configurations["brightness"] = "80";

	OCStackResult result = m_pTMHelper->m_pThingsConfiguration->updateConfigurations(m_pTMHelper->getResourceList().at(INT_ZERO),
			configurations,
			std::bind(&TMThingsConfigurationTest_btc::onConfiguration, this, std::placeholders::_1,
					std::placeholders::_2, std::placeholders::_3));

	if (result != OC_STACK_OK)
	{
		SET_FAILURE("Failed to update configurations. Result is: " + std::to_string(result));
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-11
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsConfiguration* getInstance();
 * @objective Test 'updateConfigurations' function with negative basic way using null resource
 * @target OCStackResult updateConfigurations(std::shared_ptr< OCResource > resource,
 *               std::map< ConfigurationName, ConfigurationValue > configurations,
 *               ConfigurationCallback callback);
 * @test_data 		1. resource Null
 *            		2. configurations map of configuration key and value
 *            		3. callback Callback function to be called
 * @pre_condition 	1. several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 *                	2. Get instance of ThingsConfiguration
 * @procedure Perform updateConfigurations with null resource and check if OC_STACK_OK is not returned
 * @post_condition Close the apps
 * @expected OC_STACK_OK is not returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationTest_btc, UpdateConfigurations_NV_N)
{
	std::map < std::string, std::string > configurations;
	configurations["brightness"] = "80";

	OCStackResult result = m_pTMHelper->m_pThingsConfiguration->updateConfigurations(NULL, configurations,
			std::bind(&TMThingsConfigurationTest_btc::onConfiguration, this, std::placeholders::_1,
					std::placeholders::_2, std::placeholders::_3));

	if (result == OC_STACK_OK)
	{
		SET_FAILURE("Failed to test nagative case. Updated configurations. Result is: "
				+ std::to_string(result));
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-11
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsConfiguration* getInstance();
 * @objective Test 'getConfigurations' function with positive basic way
 * @target OCStackResult getConfigurations(std::shared_ptr< OCResource > resource,
 *              std::vector< ConfigurationName > configurations, ConfigurationCallback callback);
 * @test_data 		1. resource single resource or collection resource
 *            		2. configurations List of configurations key
 *            		3. callback Callback function to be called
 * @pre_condition 	1. several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * 					2. find resource
 *                	3. Get instance of ThingsConfiguration
 * @procedure Perform getConfigurations and check if OC_STACK_OK is returned
 * @post_condition Close the apps
 * @expected OC_STACK_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationTest_btc, GetConfigurations_SRC_P)
{
	if (!m_pTMHelper->findCandidateResourcesAndVerify(resourceTypes, OC_STACK_OK, RESOURCE_COUNT_FAN, m_errorMsg))
	{
		SET_FAILURE("Precondition Failed And " + m_errorMsg);
		return;
	}

	std::vector < std::string > configurations;
	std::string configuration = "brightness";
	configurations.push_back(configuration);

	OCStackResult result = m_pTMHelper->m_pThingsConfiguration->getConfigurations(m_pTMHelper->getResourceList().at(INT_ZERO),
			configurations,
			std::bind(&TMThingsConfigurationTest_btc::onConfiguration, this, std::placeholders::_1,
					std::placeholders::_2, std::placeholders::_3));

	if (result != OC_STACK_OK)
	{
		SET_FAILURE("Failed to get configurations. Result is: " + std::to_string(result));
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-11
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsConfiguration* getInstance();
 * @objective Test 'getConfigurations' function with negative basic way using null resource
 * @target OCStackResult getConfigurations(std::shared_ptr< OCResource > resource,
 *              std::vector< ConfigurationName > configurations, ConfigurationCallback callback);
 * @test_data 		1. resource Null
 *            		2. configurations List of configurations key
 *            		3. callback Callback function to be called
 * @pre_condition 	1. several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 *                	2. Get instance of ThingsConfiguration
 * @procedure Perform reboot with null resource and check if OC_STACK_OK is not returned
 * @post_condition Close the apps
 * @expected OC_STACK_OK is not returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationTest_btc, GetConfigurations_NV_N)
{
	std::vector < std::string > configurations;

	OCStackResult result = m_pTMHelper->m_pThingsConfiguration->getConfigurations(NULL, configurations,
			std::bind(&TMThingsConfigurationTest_btc::onConfiguration, this, std::placeholders::_1,
					std::placeholders::_2, std::placeholders::_3));

	if (result == OC_STACK_OK)
	{
		SET_FAILURE("Failed to test negative case. Gotten configurations. Result is: "
				+ std::to_string(result));
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-11
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsConfiguration* getInstance();
 * @objective Test 'getListOfSupportedConfigurationUnits' function with positive basic way
 * @target std::string getListOfSupportedConfigurationUnits();
 * @test_data None
 * @pre_condition 	1. several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * 					2. find resource
 *                	3. Get instance of ThingsConfiguration
 * @procedure Perform getListOfSupportedConfigurationUnits and check if list of supported configuration units is returned
 * @post_condition Close the apps
 * @expected list of supported configuration units is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationTest_btc, GetListOfSupportedConfigurationUnits_SRC_P)
{
	std::string result = "";
	result = m_pTMHelper->m_pThingsConfiguration->getListOfSupportedConfigurationUnits();

	if(result == EMPTY_STRING)
	{
		SET_FAILURE("getListOfSupportedConfigurationUnits did not return list of supported configuration units");
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-18
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsConfiguration* getInstance();
 * @objective Test 'doBootstrap' function with positive basic way
 * @target OCStackResult doBootstrap(ConfigurationCallback callback);
 * @test_data callback Callback function to be called
 * @pre_condition 	1. several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * 					2. find resource
 *                	3. Get instance of ThingsConfiguration
 * @procedure Perform doBootstrap and check if OC_STACK_OK is returned
 * @post_condition Close the apps
 * @expected OC_STACK_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationTest_btc, DoBootstrap_SRC_P)
{
	OCStackResult result = m_pTMHelper->m_pThingsConfiguration->doBootstrap(
			std::bind(&TMThingsConfigurationTest_btc::onConfiguration, this, std::placeholders::_1,
					std::placeholders::_2, std::placeholders::_3));

	if (result != OC_STACK_OK)
	{
		SET_FAILURE("Failed to do bootstrap. Result is: " + std::to_string(result));
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-18
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @see static ThingsConfiguration* getInstance();
 * @objective Test 'doBootstrap' function with negative basic way using null callback
 * @target OCStackResult doBootstrap(ConfigurationCallback callback);
 * @test_data callback Callback function to be called
 * @pre_condition 	1. several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * 					2. find resource
 *                	3. Get instance of ThingsConfiguration
 * @procedure Perform doBootstrap using null callback and check if OC_STACK_OK is not returned
 * @post_condition Close the apps
 * @expected OC_STACK_OK is not returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMThingsConfigurationTest_btc, DoBootstrap_NV_N)
{
	OCStackResult result = m_pTMHelper->m_pThingsConfiguration->doBootstrap(NULL);

	if (result == OC_STACK_OK)
	{
		SET_FAILURE("Failed to test negative case. Done bootstrap. Result is: "
				+ std::to_string(result));
	}

	SUCCEED();
}
#endif
