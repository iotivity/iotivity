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

#include "SMRemoteHelper.h"

class SMRemoteSceneListIntegrationTest_stc: public ::testing::Test {
public:
	SMRemoteHelper* m_pSMRemoteHelper;
	bool isException;
	bool isCallback;
	std::shared_ptr<RemoteSceneList> m_pRemoteSceneList;
	std::shared_ptr<RemoteSceneCollection> m_pRemoteSceneCollection;

	SMRemoteSceneListIntegrationTest_stc() {
		m_pSMRemoteHelper = nullptr;
		m_pRemoteSceneList = nullptr;
		m_pRemoteSceneCollection = nullptr;
		isException = false;
		isCallback = false;
	}

	virtual void SetUp() {
		IOTIVITYTEST_LOG(INFO, "SetUp IN");

		CommonUtil::runCommonTCSetUpPart();

		m_pSMRemoteHelper = SMRemoteHelper::getInstance();

		CommonUtil::launchApp(SCENE_SERVER);
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);

		m_pSMRemoteHelper->discoverResource();
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);
		m_pSMRemoteHelper->stopDiscovery();
	}

	virtual void TearDown() {
		IOTIVITYTEST_LOG(INFO, "TearDown IN");

		CommonUtil::runCommonTCTearDownPart();

		CommonUtil::killApp(SCENE_SERVER);
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);
	}

public:
	void onRemoteSceneListCreated(RemoteSceneList::Ptr remoteSceneList, int) {
		IOTIVITYTEST_LOG(INFO, "%s is called", __func__);
		isCallback = true;

		if (remoteSceneList) {
			m_pRemoteSceneList = std::move(remoteSceneList);
		} else {
			IOTIVITYTEST_LOG(INFO, "Create Remote list failed.");
		}
	}

	void onRemoteSceneCollectionCreated(
			RemoteSceneCollection::Ptr remoteSceneCol, int) {
		IOTIVITYTEST_LOG(INFO, "%s is called", __func__);
		isCallback = true;

		if (remoteSceneCol) {
			m_pRemoteSceneCollection = remoteSceneCol;
		} else {
			IOTIVITYTEST_LOG(INFO, "Create Remote scene collection failed.");
		}
	}

	void onSetName(int) {
		IOTIVITYTEST_LOG(INFO, "%s is called", __func__);
		isCallback = true;
	}
};

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @objective Test createInstance function positively for checking if callback is called
 * @target static void createInstance(RCSRemoteResourceObject::Ptr sceneListResource,
 * 			CreateInstanceCallback cb);
 * @test_data 		1. sceneListResource RCSRemoteResourceObject pointer of SceneList
 * 					2. cb A callback to create instance
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * @procedure call createInstance
 * @post_condition none
 * @expected callback will be called and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListIntegrationTest_stc, RemoteSceneListCreateInstance_CV_P)
{
	try
	{
		RemoteSceneList::createInstance(m_pSMRemoteHelper->g_pFoundSceneList,std::bind(
						&SMRemoteSceneListIntegrationTest_stc::onRemoteSceneListCreated, this, placeholders::_1, placeholders::_2));
		CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
	}
	catch (exception& e)
	{
		SET_FAILURE("Unexpected Exception occurred inside createInstance: " + std::string(e.what()));
		return;
	}

	if(isCallback == false)
	{
		SET_FAILURE("CreateInstanceCallback is not called");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(RCSRemoteResourceObject::Ptr sceneListResource,
 * 			CreateInstanceCallback cb);
 * @objective Test addNewSceneCollection function positively for checking if callback is called
 * @target void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @test_data cb A callback function to create remote scene collection
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. call createInstance method
 * @procedure call addNewSceneCollection
 * @post_condition none
 * @expected callback will be called and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListIntegrationTest_stc, RemoteAddNewSceneCollection_CV_P)
{
	m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();

	if(m_pRemoteSceneList == NULL_PTR)
	{
		SET_FAILURE("did not find remote scene list instance");
		return;
	}

	try
	{
		m_pRemoteSceneList->addNewSceneCollection(std::bind(
						&SMRemoteSceneListIntegrationTest_stc::onRemoteSceneCollectionCreated, this, placeholders::_1, placeholders::_2));
		CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
		ASSERT_NE(m_pRemoteSceneCollection,NULL_PTR) << "remote scene collection is not added";
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside addNewSceneCollection: " + std::string(e.what()));
		return;
	}

	if(isCallback == false)
	{
		SET_FAILURE("AddNewSceneCollectionCallback is not called");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(RCSRemoteResourceObject::Ptr sceneListResource,
 * 			CreateInstanceCallback cb);
 * @objective Test addNewSceneCollection and getSceneCollections functions positively for getting added list of SceneCollections
 * @target void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @target std::vector< RemoteSceneCollection::Ptr > getRemoteSceneCollections() const;
 * @test_data cb A callback function to create remote scene collection
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. call createInstance method
 * @procedure 		1. call addNewSceneCollection
 * 					2. call getSceneCollections
 * @post_condition none
 * @expected getSceneCollections will return added scene collections and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListIntegrationTest_stc, RemoteAddAndGetSceneCollections_GSRV_P)
{
	m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();

	if(m_pRemoteSceneList == NULL_PTR)
	{
		SET_FAILURE("did not find remote scene list instance");
		return;
	}

	std::vector<RemoteSceneCollection::Ptr> remoteSceneCollections;

	try
	{
		m_pRemoteSceneList->addNewSceneCollection(std::bind(
						&SMRemoteSceneListIntegrationTest_stc::onRemoteSceneCollectionCreated, this, placeholders::_1, placeholders::_2));
		CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
		ASSERT_NE(m_pRemoteSceneCollection,NULL_PTR) << "remote scene collection is not added";

		remoteSceneCollections = m_pRemoteSceneList->getRemoteSceneCollections();

		if(remoteSceneCollections.size() == INT_ZERO)
		{
			SET_FAILURE("list of remote scene collections is not found ");
			return;
		}

		if(remoteSceneCollections.at(INT_ZERO) != m_pRemoteSceneCollection)
		{
			SET_FAILURE("set scene collection and received scene collection does not match");
		}
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(RCSRemoteResourceObject::Ptr sceneListResource,
 * 			CreateInstanceCallback cb);
 * @objective Test setName function positively for checking if callback is called
 * @target void setName(const std::string &name, SetNameCallback cb);
 * @test_data 		1. name = "Home"
 * 					2. cb A callback to receive the response
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. call createInstance method
 * @procedure call setName
 * @post_condition none
 * @expected callback will be called and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListIntegrationTest_stc, RemoteSceneListSetName_CV_P)
{
	m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();

	if(m_pRemoteSceneList == NULL_PTR)
	{
		SET_FAILURE("did not find remote scene list instance");
		return;
	}

	std::string remoteSceneListName = "Home";

	try
	{
		m_pRemoteSceneList->setName(remoteSceneListName,std::bind(&SMRemoteSceneListIntegrationTest_stc::onSetName, this, placeholders::_1));
		CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside setName: " + std::string(e.what()));
		return;
	}

	if(isCallback == false)
	{
		SET_FAILURE("SetNameCallback is not called");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(RCSRemoteResourceObject::Ptr sceneListResource,
 * 			CreateInstanceCallback cb);
 * @objective Test setName and getName function positively  to check if set name is received
 * @target void setName(const std::string &name, SetNameCallback cb);
 * @target std::string getName() const;
 * @test_data 		1. name = "Home"
 * 					2. cb A callback to receive the response
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. call createInstance method
 * @procedure		1. call setName
 * 					2. call getName
 * @post_condition none
 * @expected getName will return same as the setName and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneListIntegrationTest_stc, RemoteSceneListSetAndGetName_GSRV_P)
{
	m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();

	if(m_pRemoteSceneList == NULL_PTR)
	{
		SET_FAILURE("did not find remote scene list instance");
		return;
	}

	std::string remoteSceneListName = "Home";
	std::string receivedName="";

	try
	{
		m_pRemoteSceneList->setName(remoteSceneListName,std::bind(&SMRemoteSceneListIntegrationTest_stc::onSetName, this, placeholders::_1));
		CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
		receivedName = m_pRemoteSceneList->getName();
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred: " + std::string(e.what()));
		return;
	}

	if(receivedName != remoteSceneListName)
	{
		SET_FAILURE("setName and getName does not match");
	}

	SUCCEED();
}
#endif
