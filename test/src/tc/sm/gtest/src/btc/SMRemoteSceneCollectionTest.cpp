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

class SMRemoteSceneCollectionTest_btc: public ::testing::Test {
public:
	SMRemoteHelper* m_pSMRemoteHelper;
	std::shared_ptr<RemoteSceneList> m_pRemoteSceneList;
	std::shared_ptr<RemoteSceneCollection> m_pRemoteSceneCollection;
	RemoteScene::Ptr m_pRemoteScene;
	RemoteScene::Ptr m_pReceivedRemoteScene;
	bool bExcpOccurred;
	const std::string sceneCollectionName;
	const std::string sceneName1;

	SMRemoteSceneCollectionTest_btc() {
		m_pSMRemoteHelper = nullptr;
		m_pRemoteSceneList = nullptr;
		m_pRemoteSceneCollection = nullptr;
		m_pReceivedRemoteScene = nullptr;
		m_pRemoteScene = nullptr;
		bExcpOccurred = false;
		sceneCollectionName = "Kitchen";
		sceneName1 = "Scene1";
	}

	virtual void SetUp() {
		IOTIVITYTEST_LOG(INFO, "SetUp IN");

		CommonUtil::runCommonTCSetUpPart();

		CommonUtil::launchApp(SCENE_SERVER);
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);

		m_pSMRemoteHelper = SMRemoteHelper::getInstance();
		m_pSMRemoteHelper->discoverResource();
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);
		m_pSMRemoteHelper->stopDiscovery();

		m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
		ASSERT_NE(m_pRemoteSceneList,NULL_PTR) << "did not find remote scene list instance";

		m_pRemoteSceneList->addNewSceneCollection(
				std::bind(
						&SMRemoteSceneCollectionTest_btc::onRemoteSceneCollectionCreated,
						this, placeholders::_1, placeholders::_2));
		CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
		ASSERT_NE(m_pRemoteSceneCollection,NULL_PTR) << "did not find remote scene collection instance";
	}

	virtual void TearDown() {
		IOTIVITYTEST_LOG(INFO, "TearDown IN");

		CommonUtil::runCommonTCTearDownPart();

		CommonUtil::killApp(SCENE_SERVER);
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);
	}

public:
	void onRemoteSceneCollectionCreated(
			RemoteSceneCollection::Ptr remoteSceneCol, int) {
		IOTIVITYTEST_LOG(INFO, "%s is called", __func__);

		if (remoteSceneCol) {
			m_pRemoteSceneCollection = remoteSceneCol;
		} else {
			IOTIVITYTEST_LOG(INFO, "Create Remote scene collection failed.");
		}
	}

	void onRemoteSceneCreated(RemoteScene::Ptr remoteScene, int) {
		IOTIVITYTEST_LOG(INFO, "%s is called", __func__);

		if (remoteScene) {
			m_pRemoteScene = remoteScene;
		} else {
			IOTIVITYTEST_LOG(INFO, "Create Remote scene collection failed.");
		}
	}

	void onSetName(int) {
		IOTIVITYTEST_LOG(INFO, "%s is called", __func__);
	}

};

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test addNewScene function on positive basic way
 * @target void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @test_data		1. name = "Scene1"
 *	 				2. cb A callback to receive created RemoteScene instance
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. get remote SceneList instance
 * 					4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure call addNewScene positively
 * @post_condition stop the simulator
 * @expected addNewScene will succeed no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionTest_btc, RemoteAddNewScene_SRC_P)
{
	try
	{
		m_pRemoteSceneCollection->addNewScene(sceneName1,std::bind(
						&SMRemoteSceneCollectionTest_btc::onRemoteSceneCreated, this,placeholders::_1, placeholders::_2));
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside addNewScene: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test addNewScene function on negative way using empty string as name
 * @target void addNewScene(const std::string &name, AddNewSceneCallback cb);
 * @test_data		1. name = ""
 * 					2. cb A callback to receive created RemoteScene instance
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. get remote SceneList instance
 * 					4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure 		1. call addNewScene with empty scene name
 * 					2. check if RCSInvalidParameterException has occured
 * @post_condition stop the simulator
 * @expected RCSInvalidParameterException exception should occur due to Invalid parameter
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionTest_btc, RemoteAddNewScene_ESV_ETC_N)
{
	try
	{
		m_pRemoteSceneCollection->addNewScene(EMPTY_STRING,std::bind(
						&SMRemoteSceneCollectionTest_btc::onRemoteSceneCreated, this,placeholders::_1, placeholders::_2));
	}
	catch(const RCSInvalidParameterException& e)
	{
		bExcpOccurred = true;
	}
	catch (exception& e)
	{
		SET_FAILURE("Unexpected Exception occurred: " + std::string(e.what()));
		return;
	}

	if(!bExcpOccurred)
	{
		SET_FAILURE("RCSInvalidParameterException should be occurred due to empty string parameter");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test addNewScene function on negative way using null callback
 * @target void addNewScene(const std::string &name, AddNewSceneCallback cb);
 * @test_data		1. name = "Scene1"
 * 					2. cb = NULL
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. get remote SceneList instance
 * 					4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure 		1. call addNewScene with Null function pointer(callback)
 * 					2. check if RCSInvalidParameterException has occured
 * @post_condition stop the simulator
 * @expected RCSInvalidParameterException exception should occur due to Invalid parameter
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionTest_btc, RemoteAddNewSceneCallback_NV_N)
{
	try
	{
		m_pRemoteSceneCollection->addNewScene(sceneName1,NULL);
	}
	catch(const RCSInvalidParameterException& e)
	{
		bExcpOccurred = true;
	}
	catch (exception& e)
	{
		SET_FAILURE("Unexpected Exception occurred: " + std::string(e.what()));
		return;
	}

	if(!bExcpOccurred)
	{
		SET_FAILURE("RCSInvalidParameterException should be occurred due to null callback");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb);
 * @objective Test getRemoteScenes function on positive basic way
 * @target std::unordered_map< std::string, RemoteScene::Ptr > getRemoteScenes() const
 * @test_data none
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. get remote SceneList instance
 * 					4. get remote SceneCollection pointer by adding SceneCollection
 * 					5. call addNewScene to add a new scene
 * @procedure call getRemoteScenes and check if it returns list of created scenes
 * @post_condition stop the simulator
 * @expected getRemoteScenes should return return list of created scenes and should not occur any exception
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionTest_btc, RemoteGetRemoteScenes_SRC_P)
{
	std::unordered_map< std::string, RemoteScene::Ptr > receivedRemoteScenes;

	m_pRemoteSceneCollection->addNewScene(sceneName1,std::bind(&SMRemoteSceneCollectionTest_btc::onRemoteSceneCreated, this,placeholders::_1, placeholders::_2));
	CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
	ASSERT_NE(m_pRemoteScene,NULL_PTR) << "remote scene is not created";

	try
	{
		receivedRemoteScenes = m_pRemoteSceneCollection->getRemoteScenes();
		ASSERT_NE(receivedRemoteScenes.size(),INT_ZERO) << "list of created remote scenes is not found";
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside getRemoteScenes: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb);
 * @objective Test getRemoteScene function on positive basic way
 * @target RemoteScene::Ptr getRemoteScene(const std::string &sceneName) const
 * @test_data sceneName = "Scene1"
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. get remote SceneList instance
 * 					4. get remote SceneCollection pointer by adding SceneCollection
 * 					5. call addNewScene to add a new scene
 * @procedure call getRemoteScene by sceneName and check if remote scene pointer is returned
 * @post_condition stop the simulator
 * @expected getRemoteScene should return created remote scene and should not occur any exception
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionTest_btc, RemoteGetRemoteScene_SRC_P)
{
	m_pRemoteSceneCollection->addNewScene(sceneName1,std::bind(&SMRemoteSceneCollectionTest_btc::onRemoteSceneCreated, this,placeholders::_1, placeholders::_2));
	CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
	ASSERT_NE(m_pRemoteScene,NULL_PTR) << "remote scene is not added";

	try
	{
		m_pReceivedRemoteScene = m_pRemoteSceneCollection->getRemoteScene(sceneName1);
		ASSERT_NE(m_pReceivedRemoteScene,NULL_PTR) << "did not receive created remote scene";
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside getRemoteScene: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test setName function on positive basic way
 * @target void setName(const std::string &name, SetNameCallback cb)
 * @test_data		1. name = "Kitchen"
 * 					2. cb A callback to receive the response
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. get remote SceneList instance
 * 					4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure call setName
 * @post_condition stop the simulator
 * @expected setName should work properly and should not occur any exception
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionTest_btc, RemoteSceneCollectionSetName_SRC_P)
{
	try
	{
		m_pRemoteSceneCollection->setName(sceneCollectionName,std::bind(&SMRemoteSceneCollectionTest_btc::onSetName, this,placeholders::_1));
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside setName: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test setName function on positive way using empty string as name
 * @target void setName(const std::string &name, SetNameCallback cb)
 * @test_data		1. name = ""
 * 					2. cb A callback to receive the response
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. get remote SceneList instance
 * 					4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure call setName using empty string as name
 * @post_condition stop the simulator
 * @expected setName should work properly and should not occur any exception
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionTest_btc, RemoteSceneCollectionSetName_ESV_P)
{
	try
	{
		m_pRemoteSceneCollection->setName(EMPTY_STRING,std::bind(&SMRemoteSceneCollectionTest_btc::onSetName, this,placeholders::_1));
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside setName: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test setName function on negative way using null callback
 * @target void setName(const std::string &name, SetNameCallback cb)
 * @test_data		1. name = "Kitchen"
 * 					2. cb = NULL
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. get remote SceneList instance
 * 					4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure call setName with null function pointer(callback) and check if
 * 			RCSInvalidParameterException has occured
 * @post_condition stop the simulator
 * @expected RCSInvalidParameterException exception should occur due to Invalid parameter
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionTest_btc, RemoteSceneCollectionSetNameCallback_NV_ETC_N)
{
	try
	{
		m_pRemoteSceneCollection->setName(sceneCollectionName,NULL);
	}
	catch(const RCSInvalidParameterException& e)
	{
		bExcpOccurred = true;
	}
	catch (exception& e)
	{
		SET_FAILURE("Unexpected Exception occurred: " + std::string(e.what()));
		return;
	}

	if(!bExcpOccurred)
	{
		SET_FAILURE("RCSInvalidParameterException should be occurred due to null function pointer.");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void setName(const std::string &name, SetNameCallback cb)
 * @objective Test getName function by positive basic way
 * @target std::string getName() const
 * @test_data none
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. get remote SceneList instance
 * 					4. get remote SceneCollection pointer by adding SceneCollection
 * 					5. call setName to set name of the SceneCollection
 * @procedure 		1. call getName to get name of the SceneCollection
 * 					2. check if returned name is not empty
 * @post_condition stop the simulator
 * @expected getName should return name and should not occur any exception
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionTest_btc, RemoteSceneCollectionGetName_SRC_P)
{
	const std::string receivedName = "";

	m_pRemoteSceneCollection->setName(sceneCollectionName,std::bind(&SMRemoteSceneCollectionTest_btc::onSetName, this,placeholders::_1));
	CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

	try
	{
		receivedName = m_pRemoteSceneCollection->getName();
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside getName:" + std::string(e.what()));
		return;
	}

	if(receivedName == EMPTY_STRING)
	{
		SET_FAILURE("received name is empty");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-01
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 * 		RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test getId function positively
 * @target std::string getId() const;
 * @test_data none
 * @pre_condition 	1. Start SceneList type resource using SCENE_SERVER simulator
 * 					2. discover resource
 * 					3. get remote SceneList instance
 * 					4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure call getId and check if returned id is not empty
 * @post_condition stop the simulator
 * @expected getId will return id and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionTest_btc, RemoteSceneCollectionGetId_SRC_P)
{
	string collectionId = "";

	try
	{
		collectionId = m_pRemoteSceneCollection->getId();
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside getId:" + std::string(e.what()));
		return;
	}

	if(collectionId == EMPTY_STRING)
	{
		SET_FAILURE("received id is empty");
	}

	SUCCEED();
}
#endif
