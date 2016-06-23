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

class TMManagerTest_btc: public ::testing::Test {
protected:
	TMHelper* m_pTMHelper;
	std::string m_errorMsg;
	std::vector<std::shared_ptr<OCResource> > foundChildResourceList;
	clock_t startTime;

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
		foundChildResourceList.clear();
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
	void onFoundCandidate(
			std::vector<std::shared_ptr<OCResource> > resourceList) {
		foundChildResourceList = resourceList;
	}

	void onCollectionPresence(std::string, OCStackResult result) {
	}
};

/**
 * @since 2014-12-04
 * @see void Configure(const PlatformConfig& config);
 * @objective Test 'findCandidateResources' function with positive basic way
 * @target OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @test_data 		1. resourceTypes list of resource type to search for
 *            		2. callback callback function for what to do when a candidate resource is found
 *            		3. waitsec = -1
 * @pre_condition Several types of resources running in RESOURCE_APP and
 * 			COLLECTION_APP
 * @procedure Perform findCandidateResources and check if OC_STACK_OK
 * 			is returned
 * @post_condition close the apps
 * @expected OC_STACK_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_btc, FindCandidateResources_SRC_P)
{
	std::vector < std::string > resourceTypeList;
	std::string resourceType = RESOURCE_TYPE_LIGHT;
	resourceTypeList.push_back(resourceType);

	OCStackResult result = OC_STACK_OK;

	result = m_pTMHelper->m_groupManager.findCandidateResources(resourceTypeList,
			std::bind(&TMManagerTest_btc::onFoundCandidate, this, std::placeholders::_1), INT_MINUS_ONE);

	CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

	if (result != OC_STACK_OK)
	{
		SET_FAILURE("Failed to find resources. result is: " + std::to_string(result));
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-03
 * @see void Configure(const PlatformConfig& config);
 * @objective to test if findCandidateResources API can return error when improper argument is provided
 * @target OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @test_data 		1. resourceTypes Null list
 *            		2. Callback function for what to do when a candidate resource is found
 *            		3. waitsec = -1
 * @pre_condition Several types of resources running in RESOURCE_APP and
 * 			COLLECTION_APP
 * @procedure Perform findCandidateResources with invalid resource type list
 * 			and check if OC_STACK_OK is not returned
 * @post_condition close the apps
 * @expected OC_STACK_OK is not returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_btc, FindCandidateResources_NV_N)
{
	std::vector < std::string > resourceTypeList;

	OCStackResult result = OC_STACK_ERROR;

	result = m_pTMHelper->m_groupManager.findCandidateResources(resourceTypeList, NULL, INT_MINUS_ONE);

	if (result == OC_STACK_OK)
	{
		SET_FAILURE("Failed to test negative case. Found child resources. result is: "
				+ std::to_string(result));
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-18
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @objective Test 'subscribeCollectionPresence' function with positive basic way
 * @target OCStackResult subscribeCollectionPresence(std::shared_ptr< OCResource > resource,
 *          CollectionPresenceCallback);
 * @test_data 		1. resource resource pointer
 *            		2. Callback callback function to be called
 * @pre_condition 	1. several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * 					2. find resource
 * @procedure Perform subscribeCollectionPresence and check if OC_STACK_OK
 * 			is returned
 * @post_condition close the apps
 * @expected OC_STACK_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_btc, SubscribeCollectionPresence_SRC_P)
{
	string groupType = COLLECTION_TYPE_ROOM_LARGE;
	vector<string> groupTypeList;
	groupTypeList.push_back(groupType);

	if (!m_pTMHelper->findCandidateResourcesAndVerify(groupTypeList, OC_STACK_OK, groupTypeList.size(), m_errorMsg))
	{
		SET_FAILURE("Precondition Failed And " + m_errorMsg);
		return;
	}

	OCStackResult result = m_pTMHelper->m_groupManager.subscribeCollectionPresence(
			m_pTMHelper->getResourceList().at(INT_ZERO),
			std::bind(&TMManagerTest_btc::onCollectionPresence, this, std::placeholders::_1,
					std::placeholders::_2));

	if (result != OC_STACK_OK)
	{
		SET_FAILURE("Failed to subscribe collection presence. Result is: " + std::to_string(result));
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-18
 * @see void Configure(const PlatformConfig& config);
 * @see	OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1);
 * @objective Test 'subscribeCollectionPresence' function with negative basic way using null parameters
 * @target OCStackResult subscribeCollectionPresence(std::shared_ptr< OCResource > resource,
 *          CollectionPresenceCallback);
 * @test_data 		1. resource Null
 *            		2. Callback Null
 * @pre_condition several types of resources running in RESOURCE_APP and
 * 						COLLECTION_APP
 * @procedure Perform subscribeCollectionPresence with null parameters and check if
 * 			OC_STACK_OK is not returned
 * @post_condition close the apps
 * @expected OC_STACK_OK is not returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_btc, SubscribeCollectionPresence_NV_N)
{
	OCStackResult result = m_pTMHelper->m_groupManager.subscribeCollectionPresence(NULL, NULL);

	if (result == OC_STACK_OK)
	{
		SET_FAILURE("Failed to test negative case. Subscribed collection presence. Result is: "
				+ std::to_string(result));
	}

	SUCCEED();
}
#endif
