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

class TMManagerTest_stc: public ::testing::Test {
protected:
	OCStackResult m_result;
	std::vector<shared_ptr<OCResource> > m_resourceList;
	int m_resourceFoundCallbackCount;
	int m_findCandidateCallCount;
	unsigned int m_expectedResourceCount;
	std::string m_errorMessage;
	TMHelper* m_pTMHelper;
	std::mutex TMManagerTest_stc::s_mutex;

protected:
	virtual void SetUp() {
		CommonUtil::runCommonTCSetUpPart();

#ifdef __LINUX__
		CommonUtil::killApp(RESOURCE_APP);
		CommonUtil::killApp(COLLECTION_APP);
		CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
		IOTIVITYTEST_LOG(INFO, "Previous App closed");

		CommonUtil::launchApp(RESOURCE_APP);
		CommonUtil::launchApp(COLLECTION_APP);
		CommonUtil::waitInSecond(3*CALLBACK_WAIT_MIN);
		IOTIVITYTEST_LOG(INFO, "New App Started");
#endif

		m_errorMessage = "";
		m_pTMHelper = TMHelper::getInstance();
		auto testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
		IOTIVITYTEST_LOG(INFO, "%s.%s New Set Up -- finished",
				testInfo->test_case_name(), testInfo->name());
	}

	virtual void TearDown() {

#ifdef __LINUX__
		CommonUtil::killApp(RESOURCE_APP);
		CommonUtil::killApp(COLLECTION_APP);
		CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
#endif

		auto testInfo = ::testing::UnitTest::GetInstance()->current_test_info();
		IOTIVITYTEST_LOG(INFO, "%s.%s Tear Down -- finished",
				testInfo->test_case_name(), testInfo->name());
		CommonUtil::runCommonTCTearDownPart();
	}

public:
	void onFoundCandidate(vector<std::shared_ptr<OCResource> > resourceList) {
		s_mutex.lock();
		IOTIVITYTEST_LOG(INFO, "....Inside onFoundCandidate....");
		m_resourceList.clear();
		m_resourceFoundCallbackCount++;

		unsigned int resourceCount = resourceList.size();
		m_errorMessage += "Occurrence of onFoundCandidate: "
				+ std::to_string(m_resourceFoundCallbackCount)
				+ " from API call no: "
				+ std::to_string(m_findCandidateCallCount) + "\n";

		if (m_expectedResourceCount > resourceCount) {
			m_errorMessage += "Total resource count mismatch, expected = "
					+ std::to_string(m_expectedResourceCount) + " found = "
					+ std::to_string(resourceCount) + "\n";
		}

		IOTIVITYTEST_LOG(INFO, "Inserting total resource to list: %u",
				resourceCount);
		m_resourceList = resourceList;

		IOTIVITYTEST_LOG(INFO, m_errorMessage.c_str());
		IOTIVITYTEST_LOG(INFO, "....Leaving onFoundCandidate....");
		s_mutex.unlock();
	}
};

/**
 * @since 2014-11-25
 * @see void Configure(const PlatformConfig& config);
 * @objective to test if findCandidateResources API can find at least one resource of a specified type
 * @target OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 * 		CandidateCallback callback, int waitsec = -1);
 * @test_data 		1. resourceTypes list of resource type to search for
 *            		2. callback callback function for what to do when a candidate resource is found
 *            		3. waitsec = -1
 * @pre_condition 3 Light and 2 Fan types resources running in RESOURCE_APP(Total 5)
 * @procedure 		1. Create a resource type list with core.fan type
 *      	   		2. Perform findCandidateResources with this list
 *                  3. Wait till callback is called
 *                  4. Check how many resource is found
 * @post_condition close the app
 * @expected  		1. API returns OC_STACK_OK
 *            		2. At least one fan type resource is found by callback
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_stc, FindOneTypeOfCandidateResources_SQV_P)
{
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_FAN);

	if (!m_pTMHelper->findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK,
					RESOURCE_COUNT_FAN, m_errorMessage))
	{
		SET_FAILURE(m_errorMessage);
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-11-25
 * @see void Configure(const PlatformConfig& config);
 * @objective to check stability of findCandidateResources API by calling it repeatedly(10 times), waiting for callback
 *              each time and check whether the API can always find resources through callback
 * @target OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *                    CandidateCallback callback, int waitsec = -1)
 * @test_data 		1. resourceTypes list of resource type to search for
 *            		2. callback callback function for what to do when a candidate resource is found
 *            		3. waitsec = -1
 * @pre_condition 3 Light and 2 Fan types resources running in RESOURCE_APP(Total 5)
 * @procedure 		1. Create a resource type list with core.light and core.fan type
 *            		2. Perform findCandidateResources with this list
 *            		3. Wait till callback is called
 *            		4. Check how many resource is found
 *           		5.Repeat step 2-4 for nine times
 * @post_condition close the app
 * @expected 		1. API returns OC_STACK_OK each time
 *           		2. At least 2 resources are found by callback each time(at step 4)
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_stc, FindCandidateResourcesTenTimes_VLCC_P)
{
	GroupManager groupManager;
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_FAN);
	resourceTypeList.push_back(RESOURCE_TYPE_LIGHT);

	m_errorMessage = "";
	m_expectedResourceCount = resourceTypeList.size();
	m_findCandidateCallCount = 0;

	for (int i = 0; i < 10; i++)
	{
		m_findCandidateCallCount++;
		m_resourceList.clear();
		m_resourceFoundCallbackCount = 0;

		m_result = groupManager.findCandidateResources(resourceTypeList,
				std::bind(&TMManagerTest_stc::onFoundCandidate, this, std::placeholders::_1), INT_MINUS_ONE);

		CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
		if (m_result != OC_STACK_OK)
		{
			SET_FAILURE("TestCase Failed. Expected = OC_STACK_OK, Actual ="
					+ CommonUtil::s_OCStackResultString.at(m_result));
		}

		if (m_resourceList.size() < m_expectedResourceCount)
		{
			SET_FAILURE("TestCase Failed. Expected = " + std::to_string(m_expectedResourceCount)
					+ ", Actual =" + std::to_string(m_resourceList.size()));
		}

		IOTIVITYTEST_LOG(INFO, "Message Log: %s", m_errorMessage.c_str());
	}

	SUCCEED();
}
#endif

/**
 * @since 2015-01-12
 * @see void Configure(const PlatformConfig& config);
 * @objective to test if findCandidateResources API can find all available resources if enough time is provided
 * @target OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *                    CandidateCallback callback, int waitsec = -1)
 * @test_data 		1. resourceTypes list of resource type to search for
 *            		2. callback callback function for what to do when a candidate resource is found
 *            		3. waitsec = 10
 * @pre_condition 3 Light and 2 Fan types resources running in RESOURCE_APP(Total 5)
 * @procedure  		1. Create a resource type list with core.light and core.fan type
 *             		2. Perform findCandidateResources with this list and waitsec = 10
 *             		3. Wait till callback is called
 *             		4. Check if all 5 resources are found
 * @post_condition close the app
 * @expected 		1. API returns OC_STACK_OK
 *           		2. All 5 resources are found by callback
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_stc, FindCandidateResourcesTillAllResourcesFound_SQV_P)
{
	GroupManager groupManager;
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_FAN);
	resourceTypeList.push_back(RESOURCE_TYPE_LIGHT);

	m_errorMessage = "";
	m_expectedResourceCount = RESOURCE_COUNT_FAN + RESOURCE_COUNT_LIGHT;
	m_findCandidateCallCount = 1;

	m_resourceList.clear();
	m_resourceFoundCallbackCount = 0;

	m_result = groupManager.findCandidateResources(resourceTypeList,
			std::bind(&TMManagerTest_stc::onFoundCandidate, this, std::placeholders::_1),
			CALLBACK_WAIT_MAX);

	CommonUtil::waitInSecond(CALLBACK_WAIT_MAX + CALLBACK_WAIT_MIN);
	if (m_result != OC_STACK_OK)
	{
		SET_FAILURE("TestCase Failed. Expected = OC_STACK_OK, Actual ="
				+ CommonUtil::s_OCStackResultString.at(m_result));
	}

	if (m_resourceList.size() < m_expectedResourceCount)
	{
		SET_FAILURE("TestCase Failed. Expected = " + std::to_string(m_expectedResourceCount)
				+ ", Actual =" + std::to_string(m_resourceList.size()));
	}

	IOTIVITYTEST_LOG(INFO, "Message Log: %s", m_errorMessage.c_str());

	SUCCEED();
}
#endif

/**
 * @since 2015-01-12
 * @see void Configure(const PlatformConfig& config);
 * @objective to test if findCandidateResources API doesn't call the callback when a non-existing type resource is
 *              asked to find
 * @target OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *                    CandidateCallback callback, int waitsec)
 * @test_data 		1. resourceTypes list of resource type to search for
 *            		2. callback callback function for what to do when a candidate resource is found
 *            		3. waitsec = 10
 * @pre_condition 3 Light and 2 Fan types resources running in RESOURCE_APP(Total 5)
 * @procedure  		1. Create resource type list using invalid resource types(core.dummy-1,
 *                  		core.dummy-2, core.dummy-3)
 *             		2. Perform findCandidateResources with this resource type list and waitsec = 10
 *             		3. Wait to test if the callback never gets called
 * @post_condition close the app
 * @expected 		1. API returns OC_STACK_OK
 *           		2. No resources are found after waiting 10 seconds
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_stc, FindNonExistingResources_DSCC_N)
{
	GroupManager groupManager;
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_DUMMY_1);
	resourceTypeList.push_back(RESOURCE_TYPE_DUMMY_2);
	resourceTypeList.push_back(RESOURCE_TYPE_DUMMY_3);

	m_errorMessage = "";
	m_expectedResourceCount = 0;
	m_findCandidateCallCount = 1;

	m_resourceList.clear();
	m_resourceFoundCallbackCount = 0;

	m_result = groupManager.findCandidateResources(resourceTypeList,
			std::bind(&TMManagerTest_stc::onFoundCandidate, this, std::placeholders::_1),
			CALLBACK_WAIT_MAX);

	CommonUtil::waitInSecond(CALLBACK_WAIT_MAX + CALLBACK_WAIT_MIN);

	if (m_result != OC_STACK_OK)
	{
		SET_FAILURE("TestCase Failed. Expected = OC_STACK_OK, Actual ="
				+ CommonUtil::s_OCStackResultString.at(m_result));
	}

	if (m_resourceList.size() != m_expectedResourceCount)
	{
		SET_FAILURE("TestCase Failed. Expected = " + std::to_string(m_expectedResourceCount)
				+ ", Actual =" + std::to_string(m_resourceList.size()));
	}

	IOTIVITYTEST_LOG(INFO, "Message Log: %s", m_errorMessage.c_str());

	SUCCEED();
}
#endif

/**
 * @since 2015-01-12
 * @see void Configure(const PlatformConfig& config);
 * @objective to test if findCandidateResources API can find any resource when waitsec is 0
 * @target OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *                    CandidateCallback callback, int waitsec = -1)
 * @test_data 		1. resourceTypes list of resource type to search for
 *            		2. callback callback function for what to do when a candidate resource is found
 *            		3. waitsec = 0
 * @pre_condition 3 Light and 2 Fan types resources running in RESOURCE_APP(Total 5)
 * @procedure  		1. Create a resource type list with core.light and core.fan type
 *             		2. Perform findCandidateResources with resource type list and 0 waitsec
 *             		3. check if callback is called
 *             		4. Wait 1 second and again check if callback is called
 * @post_condition close the app
 * @expected 		1. API returns OC_STACK_OK
 *           		2. Callback is not called and No resources are found
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_stc, FindCandidateResourcesImmediately_SQV_P)
{
	GroupManager groupManager;
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_FAN);
	resourceTypeList.push_back(RESOURCE_TYPE_LIGHT);

	m_errorMessage = "";
	m_expectedResourceCount = 0;
	m_findCandidateCallCount = 1;

	m_resourceList.clear();
	m_resourceFoundCallbackCount = 0;

	m_result = groupManager.findCandidateResources(resourceTypeList,
			std::bind(&TMManagerTest_stc::onFoundCandidate, this, std::placeholders::_1),
			CALLBACK_WAIT_NONE);

	if (m_result != OC_STACK_OK)
	{
		SET_FAILURE("TestCase Failed. Expected = OC_STACK_OK, Actual ="
				+ CommonUtil::s_OCStackResultString.at(m_result));
	}

	if ((m_resourceFoundCallbackCount == INT_ONE) && (m_resourceList.size() != m_expectedResourceCount))
	{
		SET_FAILURE("TestCase Failed. Expected = " + std::to_string(m_expectedResourceCount)
				+ ", Actual =" + std::to_string(m_resourceList.size()));
		return;
	}

	CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

	if ((m_resourceFoundCallbackCount == INT_ONE) && (m_resourceList.size() != m_expectedResourceCount))
	{
		SET_FAILURE("TestCase Failed. Expected = " + std::to_string(m_expectedResourceCount)
				+ ", Actual =" + std::to_string(m_resourceList.size()));
	}

	IOTIVITYTEST_LOG(INFO, "Message Log: %s", m_errorMessage.c_str());

	SUCCEED();
}
#endif

/**
 * @since 2015-01-12
 * @see void Configure(const PlatformConfig& config);
 * @objective to test if findCandidateResources API can find at least one resource of each type mentioned in the
 *              resource type list
 * @target OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *                    CandidateCallback callback, int waitsec = -1)
 * @test_data 		1. resourceTypes list of resource type to search for
 *            		2. callback callback function for what to do when a candidate resource is found
 *            		3. waitsec = -1
 * @pre_condition 3 Light and 2 Fan types resources running in RESOURCE_APP(Total 5)
 * @procedure  		1. Create a resource type list with core.light and core.fan type
 *             		2. Perform findCandidateResources with resource type list
 *             		3. Wait to check if callback is called
 *             		4. check whether the found resource list contains both core.light &
 *                  			core.fan type resource
 * @post_condition close the app
 * @expected 		1. API returns OC_STACK_OK
 *           		2. Both core.light & core.fan type resources are found
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_stc, FindCandidateResourcesAllTypes_SQV_P)
{
	GroupManager groupManager;
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_FAN);
	resourceTypeList.push_back(RESOURCE_TYPE_LIGHT);

	m_errorMessage = "";
	m_findCandidateCallCount = 1;

	m_resourceList.clear();
	m_resourceFoundCallbackCount = 0;
	m_expectedResourceCount = resourceTypeList.size();

	m_result = groupManager.findCandidateResources(resourceTypeList,
			std::bind(&TMManagerTest_stc::onFoundCandidate, this, std::placeholders::_1), INT_MINUS_ONE);

	CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

	if (m_result != OC_STACK_OK)
	{
		SET_FAILURE("TestCase Failed. Expected = OC_STACK_OK, Actual ="
				+ CommonUtil::s_OCStackResultString.at(m_result));
	}

	if (!m_pTMHelper->containsResource(m_resourceList, resourceTypeList, m_errorMessage))
	{
		SET_FAILURE(m_errorMessage);
	}

	SUCCEED();
}
#endif

/**
 * @since 2015-01-12
 * @see void Configure(const PlatformConfig& config);
 * @objective to test if findCandidateResources API finds resources within the the time limit specified(i.e. whether
 *               the waitsec argument works fine and the API does not consume extra time)
 * @target OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *                    CandidateCallback callback, int waitsec)
 * @test_data 		1. resourceTypes list of resource type to search for
 *            		2. callback callback function for what to do when a candidate resource is found
 *            		3. waitsec = 10
 * @pre_condition 3 Light and 2 Fan types resources running in RESOURCE_APP(Total 5)
 * @procedure  		1. Create a resource type list with core.light and core.fan type
 *             		2. Perform findCandidateResources with resource type list and waitsec = 10
 *             		3. Wait to check if callback is called
 *             		4. Check whether the callback is called within 10 seconds
 * @post_condition close the app
 * @expected 		1. API returns OC_STACK_OK
 *           		2. The callback is called within 10 seconds
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_stc, FindCandidateResourcesCheckWaitsec_SQV_P)
{
	GroupManager groupManager;
	std::vector< std::string > resourceTypeList;
	std::string resourceType = RESOURCE_TYPE_FAN;
	resourceTypeList.push_back(resourceType);
	resourceType = RESOURCE_TYPE_LIGHT;
	resourceTypeList.push_back(resourceType);

	m_errorMessage = "";
	m_findCandidateCallCount = 1;

	m_resourceList.clear();
	m_resourceFoundCallbackCount = 0;
	m_expectedResourceCount = resourceTypeList.size();

	m_result = groupManager.findCandidateResources(resourceTypeList,
			std::bind(&TMManagerTest_stc::onFoundCandidate, this, std::placeholders::_1),
			CALLBACK_WAIT_MAX);

	if (m_result != OC_STACK_OK)
	{
		SET_FAILURE("TestCase Failed. Expected = OC_STACK_OK, Actual ="
				+ CommonUtil::s_OCStackResultString.at(m_result));
	}

	int elapsedTime = 0;
	do
	{
		CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
		elapsedTime += CALLBACK_WAIT_MIN;

		if ((m_resourceFoundCallbackCount == INT_ONE) && (elapsedTime < CALLBACK_WAIT_MAX))
		{
			SET_FAILURE("TestCase Failed. Callback was called at " + to_string(elapsedTime));
		}

		if(elapsedTime > CALLBACK_WAIT_MAX * INT_TWO) {
			SET_FAILURE("TestCase Failed. Callback was not called even after " + to_string(elapsedTime) + " seconds.");
			break;
		}

	}while (m_resourceFoundCallbackCount == INT_ZERO);

	SUCCEED();
}
#endif

/**
 * @since 2015-01-14
 * @see void Configure(const PlatformConfig& config);
 * @objective to test if findCandidateResources API can find collections(groups)
 * @target OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *                    CandidateCallback callback, int waitsec)
 * @test_data 		1. resourceTypes list of resource type to search for
 *            		2. callback callback function for what to do when a candidate resource is found
 *            		3. waitsec = 10
 * @pre_condition 	1. 3 Light and 2 Fan types resources running in RESOURCE_APP(Total 5)
 * 					2. 3 collections are running in COLLECTION_APP(core.room-large,
 *                    		core.room-medium & core.room-small)
 * @procedure  		1. Create resource type list using core.room-large, core.room-medium, core.room-small
 *             		2. Perform findCandidateResources with resource type list and maximum time (10 sec)
 *             		3. Wait to test if it finds any collection
 * @post_condition close the apps
 * @expected 		1. API returns OC_STACK_OK
 *           		2. All 3 collections should be found
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_stc, FindCollenctionsUsingFindCandidateResources_SQV_P)
{
	GroupManager groupManager;
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(COLLECTION_TYPE_ROOM_LARGE);
	resourceTypeList.push_back(COLLECTION_TYPE_ROOM_MEDIUM);
	resourceTypeList.push_back(COLLECTION_TYPE_ROOM_SMALL);

	m_errorMessage = "";
	m_expectedResourceCount = 3;
	m_findCandidateCallCount = 1;

	m_resourceList.clear();
	m_resourceFoundCallbackCount = 0;

	m_result = groupManager.findCandidateResources(resourceTypeList,
			std::bind(&TMManagerTest_stc::onFoundCandidate, this, std::placeholders::_1),
			CALLBACK_WAIT_MAX);

	if (m_result != OC_STACK_OK)
	{
		SET_FAILURE("TestCase Failed. Expected = OC_STACK_OK, Actual ="
				+ CommonUtil::s_OCStackResultString.at(m_result));
	}

	CommonUtil::waitInSecond(CALLBACK_WAIT_MAX + CALLBACK_WAIT_MIN);

	if (m_resourceList.size() < m_expectedResourceCount)
	{
		SET_FAILURE("TestCase Failed. Expected Collection count = "
				+ std::to_string(m_expectedResourceCount) + ", Actual Collection count ="
				+ std::to_string(m_resourceList.size()));
	}

	IOTIVITYTEST_LOG(INFO, "Message Log: %s", m_errorMessage.c_str());

	SUCCEED();
}
#endif

/**
 * @since 2014-12-17
 * @see void Configure(const PlatformConfig& config);
 * @see OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1)
 * @objective to test if subscribeCollectionPresence API returns error when
 *              a resource(not collection) is used to subscribe for collection presence
 * @target OCStackResult subscribeCollectionPresence(std::shared_ptr< OCResource > resource,
 *   		CollectionPresenceCallback);
 * @test_data 		1. resource Pointer of the resource
 *            		2. Callback function to receive updated state
 * @pre_condition 3 Light and 2 Fan types resources running in RESOURCE_APP(Total 5)
 * @procedure 		1. Create a resource type list using core.light type
 *            		2. Perform findCandidateResources using the resource type list created at step 1
 *            		3. Wait for the callback to get called
 *            		4. Perform subscribeCollectionPresence on the found group
 *            		5. Check presence callback
 * @post_condition close the app
 * @expected The callback should return OC_STACK_ERROR (at step 5)
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_stc, SubscribeCollectionPresenceForResource_DSCC_N)
{
	std::vector< std::string > resourceTypeList;
	resourceTypeList.push_back(RESOURCE_TYPE_LIGHT);

	bool result = m_pTMHelper->findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK, INT_ONE,
			m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
		return;
	}

	result = m_pTMHelper->subscribeCollectionPresenceAndVerify(m_pTMHelper->getResourceList().at(INT_ZERO),
			OC_STACK_ERROR, INT_MINUS_ONE, m_errorMessage);

	if (!result)
	{
		SET_FAILURE(m_errorMessage);
	}

	SUCCEED();
}
#endif

/**
 * @since 2014-12-10
 * @see void Configure(const PlatformConfig& config);
 * @see OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
 *          CandidateCallback callback, int waitsec = -1)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *          const std::shared_ptr<OCResource> resource)
 * @objective to test if bindResourceToGroup API can successfully bind the resource to a group
 * @target OCStackResult bindResourceToGroup(OCResourceHandle& childHandle,
 * 		std::shared_ptr< OCResource > resource, OCResourceHandle& collectionHandle);
 * @test_data 		1. childHandle Resource handle for child( The API will supply this, out parameter)
 *            		2. resource Pointer to the resource that will be child
 *            		3. collectionHandle collection handle
 * @pre_condition 	1. 3 Light and 2 Fan types resources running in RESOURCE_APP(Total 5)
 * 					2. 3 collections are running in COLLECTION_APP(core.room-large,
 *                    		core.room-medium & core.room-small)
 * @procedure 		1. Create a resource type list using core.light type and core.fan type
 *            		2. Perform findcandidateResources using the resource type list
 *            		3. Wait for the the callback to get the resource pointer of these two resources
 *            		4. Perform registerResource to the fan type resource to get collection handle
 *            		5. Perform bindResourceToGroup using the resource pointer and collection handle
 * @post_condition close the apps
 * @expected bindResourceToGroup API should return OC_STACK_OK
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(TMManagerTest_stc, BindResourceToGroup_SQV_P)
{
	std::string resourceType = RESOURCE_TYPE_LIGHT;
	OCResourceHandle childHandle;

	bool result = m_pTMHelper->bindResourceToGroupAndVerify(resourceType, childHandle,
			OC_STACK_OK, m_errorMessage);
	if (!result)
	{
		SET_FAILURE(m_errorMessage);
	}

	SUCCEED();
}
#endif
