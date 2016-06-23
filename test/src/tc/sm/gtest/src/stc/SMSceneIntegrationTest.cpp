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

#include "SMHelper.h"

class SMSceneIntegrationTest_stc: public ::testing::Test {
public:
	SMHelper* m_pSMHelper;
	SceneList* m_pSceneList;
	std::shared_ptr<SceneCollection> m_pSceneCollection;
	std::shared_ptr<Scene> m_pScene1;
	const std::string sceneName1;
	std::shared_ptr<SceneAction> m_pSceneAction1;
	std::shared_ptr<SceneAction> m_pSceneAction2;
	std::shared_ptr<RCSRemoteResourceObject> m_pResource1;
	std::shared_ptr<RCSRemoteResourceObject> m_pResource2;
	bool isException;

	SMSceneIntegrationTest_stc() {
		m_pSMHelper = nullptr;
		m_pSceneList = nullptr;
		m_pSceneCollection = nullptr;
		m_pScene1 = nullptr;
		m_pSceneAction1 = nullptr;
		m_pSceneAction2 = nullptr;
		m_pResource1 = nullptr;
		m_pResource2 = nullptr;
		isException = false;
		sceneName1 = "Scene1";
	}

	virtual void SetUp() {
		IOTIVITYTEST_LOG(INFO, "SetUp IN");

		CommonUtil::runCommonTCSetUpPart();

		CommonUtil::launchApp(FAN_SERVER);
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);
		CommonUtil::launchApp(LIGHT_SERVER);
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);

		m_pSMHelper = SMHelper::getInstance();
		m_pSMHelper->discoverResource();
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);
		m_pSMHelper->stopDiscovery();

		m_pSceneList = m_pSMHelper->getSceneListInstance();
		ASSERT_NE(m_pSceneList,NULL_PTR) << "scenelist pointer is not found";

		m_pSceneCollection = m_pSceneList->addNewSceneCollection();
		ASSERT_NE(m_pSceneCollection,NULL_PTR) << "scene collection pointer is not found";

		m_pScene1 = m_pSceneCollection->addNewScene(sceneName1);
		ASSERT_NE(m_pScene1,NULL_PTR) << "scene pointer is not found";

		ASSERT_EQ(m_pSMHelper->g_foundResourceList.size(),INT_TWO) << "Light and Fan Resources are Not Found";

		m_pResource1 = m_pSMHelper->g_foundResourceList.at(INT_ZERO);
		m_pResource2 = m_pSMHelper->g_foundResourceList.at(INT_ONE);
	}

	virtual void TearDown() {
		IOTIVITYTEST_LOG(INFO, "TearDown IN");

		CommonUtil::killApp(FAN_SERVER);
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);
		CommonUtil::killApp(LIGHT_SERVER);
		CommonUtil::waitInSecond(MAX_SLEEP_TIME);

		CommonUtil::runCommonTCTearDownPart();
	}

};

/**
 * @since 2016-03-02
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @objective Test addNewSceneAction and getSceneAction function positively for checking if added sceneAction is get
 * @target SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @target SceneAction::Ptr getSceneAction(
 *                  const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject) const;
 * @test_data 		1. pRCSRemoteResourceObject A pointer of discovered resource
 * 					2. key = "power"
 * 					3. value = "on"
 * 					4. pRCSRemoteResourceObject A pointer of discovered resource
 * @pre_condition 	1. create light and fan type resource using LIGHT_SERVER and FAN_SERVER simulator
 * 					2. get SceneList instance
 * 					3. Get SceneCollection pointer by adding SceneCollection
 * 					4. Get scene pointer by adding scene
 * @procedure 		1. call addNewSceneAction and check if scene action pointer is returned
 * 					2. call getSceneAction and check if scene action pointer is returned
 * 					3. check if added sceneAction's key and value matches with get sceneAction's key and value
 * @post_condition stop the simulators
 * @expected added sceneAction's key and value matches with get sceneAction's key and value
 */
#if defined(__LINUX__)
TEST_F(SMSceneIntegrationTest_stc, AddAndGetSceneAction_GSRV_P)
{
	try
	{
		m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource1,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));
		ASSERT_NE(m_pSceneAction1,NULL_PTR) << "addNewSceneAction did not return scene aciton";
		m_pSceneAction2 = m_pScene1->getSceneAction(m_pResource1);
		ASSERT_NE(m_pSceneAction2,NULL_PTR) << "getSceneAction did not return scene aciton";
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred: " + std::string(e.what()));
	}

	for(const auto &attribute : m_pSceneAction2->getExecutionParameter())
	{
		if(attribute.key() != KEY_POWER || attribute.value() != VALUE_ON)
		{
			SET_FAILURE("received key or value is not matching with set key or value");
		}
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-02
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @objective Test addNewSceneAction and getSceneActions function positively
 * 		for checking if added scene action is get
 * @target SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @target std::vector<SceneAction::Ptr> getSceneActions() const ;
 * @test_data 		1. pRCSRemoteResourceObject A pointer of discovered resource
 * 					2. key = "power"
 * 					3. value = "on"
 * 					4. pRCSRemoteResourceObject A pointer of another discovered resource
 * 					5. key = "speed"
 * 					6. value = "20"
 * @pre_condition 	1. create light and fan type resource using LIGHT_SERVER and FAN_SERVER simulator
 * 					2. get SceneList instance
 * 					3. Get SceneCollection pointer by adding SceneCollection
 * 					4. Get scene pointer by adding scene
 * @procedure 		1. call addNewSceneAction two times and check if scene actions are returned
 * 					2. call getSceneActions and check if list of scene actions are returned
 * 					3. check for both scene actions if added sceneAction's key and value matches with get sceneAction's key and value
 * @post_condition stop the simulators
 * @expected added sceneAction's key and value matches with get sceneAction's key and value
 */
#if defined(__LINUX__)
TEST_F(SMSceneIntegrationTest_stc, AddAndGetSceneActions_GSRV_P)
{
	std::vector<SceneAction::Ptr> sceneActions;

	std::shared_ptr<SceneAction> m_pSceneAction3;
	std::shared_ptr<SceneAction> m_pSceneAction4;

	try
	{
		m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource1,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));
		ASSERT_NE(m_pSceneAction1,NULL_PTR) << "first addNewSceneAction did not return scene action pointer";
		m_pSceneAction2 = m_pScene1->addNewSceneAction(m_pResource2,KEY_SPEED,RCSResourceAttributes::Value(VALUE_SPEED));
		ASSERT_NE(m_pSceneAction2,NULL_PTR) << "second addNewSceneAction did not return scene action pointer";
		sceneActions = m_pScene1->getSceneActions();
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred: " + std::string(e.what()));
		return;
	}

	if(sceneActions.size() != INT_TWO)
	{
		SET_FAILURE("added two scene actions, but didn't get two scene actions");
		return;
	}

	m_pSceneAction3 = sceneActions.at(INT_ZERO);
	ASSERT_NE(m_pSceneAction3,NULL_PTR) << "did not receive first scene action pointer from getSceneActions";
	m_pSceneAction4 = sceneActions.at(INT_ONE);
	ASSERT_NE(m_pSceneAction4,NULL_PTR) << "did not receive second scene action pointer from getSceneActions";

	for(const auto &attribute : m_pSceneAction3->getExecutionParameter())
	{
		if(attribute.key() != KEY_POWER || attribute.value() != VALUE_ON)
		{
			SET_FAILURE("received key or value is not matching with set key or value for the first scene action");
		}
	}

	for(const auto &attribute : m_pSceneAction4->getExecutionParameter())
	{
		if(attribute.key() != KEY_SPEED || attribute.value() != VALUE_SPEED)
		{
			SET_FAILURE("received key or value is not matching with set key or value for the second scene action");
		}
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-02
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @objective Test addNewScene and getName function positively for checking if added scene's name is returned
 * @target Scene::Ptr addNewScene(const std::string& sceneName);
 * @target std::string getName() const;
 * @test_data sceneName = "Scene1"
 * @pre_condition 	1. get SceneList instance
 * 					2. Get SceneCollection pointer by adding SceneCollection
 * @procedure 		1. Get scene pointer by adding scene
 * 					2. call getName and check if it returns the same name as added
 * @post_condition none
 * @expected getName will return added scene's name and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneIntegrationTest_stc, AddSceneAndGetName_GSRV_P)
{
	std::string receivedName = "";
	try
	{
		receivedName = m_pScene1->getName();
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside getName: " + std::string(e.what()));
		return;
	}

	if(receivedName != sceneName1)
	{
		SET_FAILURE("received name doesnt match with the added scene's name");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-02
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @see SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @objective Test execute function positively for checking if callback is called
 * @target void execute(ExecuteCallback);
 * @test_data ExecuteCallback A callback to execute Scene
 * @pre_condition 	1. create light and fan type resource using LIGHT_SERVER and FAN_SERVER simulator
 * 					2. get SceneList instance
 * 					3. Get SceneCollection pointer by adding SceneCollection
 * 					4. Get scene pointer by adding scene
 * 					5. call addNewSceneAction
 * @procedure call execute and check if callback is called
 * @post_condition stop the simulators
 * @expected onExecute callback is called and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneIntegrationTest_stc, ExecuteActionOnMultipleResources_SQV_CV_P)
{
	SMHelper::s_checkExecuteCallback = false;

	m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource1,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));
	ASSERT_NE(m_pSceneAction1,NULL_PTR) << "first addNewSceneAction did not return scene action pointer";
	m_pSceneAction2 = m_pScene1->addNewSceneAction(m_pResource2,KEY_SPEED,RCSResourceAttributes::Value(VALUE_SPEED));
	ASSERT_NE(m_pSceneAction2,NULL_PTR) << "second addNewSceneAction did not return scene action pointer";

	try
	{
		m_pScene1->execute(&SMHelper::onExecute);
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside execute: " + std::string(e.what()));
		return;
	}

	CommonUtil::waitInSecond(MAX_SLEEP_TIME);

	if(SMHelper::s_checkExecuteCallback != true)
	{
		SET_FAILURE("execute callback is not called");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-02
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @see SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @objective Test total procedure of executing a scene, for multiple times
 * @target void execute(ExecuteCallback);
 * @test_data ExecuteCallback A callback to execute Scene
 * @pre_condition create light and fan type resource using LIGHT_SERVER and FAN_SERVER simulator
 * @procedure		1. get SceneList instance
 * 					2. Get SceneCollection pointer by adding SceneCollection
 * 					3. Get scene pointer by adding scene
 * 					4. call addNewSceneAction
 * 					5. call execute and check if callback is called
 * 					6. follow steps 1-5 for multiple times
 * @post_condition stop the simulators
 * @expected onExecute callback is called each time and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneIntegrationTest_stc, ExecuteSameActionMultipleTimes_VLCC_P)
{
	for(int i=1; i<4; i++)
	{
		SMHelper::s_checkExecuteCallback = false;
		m_pSceneList = nullptr;
		m_pSceneCollection = nullptr;
		m_pScene1 = nullptr;
		m_pSceneAction1 = nullptr;
		m_pSceneAction2 = nullptr;

		m_pSceneList = m_pSMHelper->getSceneListInstance();
		ASSERT_NE(m_pSceneList,NULL_PTR) << "scenelist pointer is not found in "+std::to_string(i)+" no. iteration";

		m_pSceneCollection = m_pSceneList->addNewSceneCollection();
		ASSERT_NE(m_pSceneCollection,NULL_PTR) << "scene collection pointer is not found in "+std::to_string(i)+" no. iteration";

		m_pScene1 = m_pSceneCollection->addNewScene(sceneName1);
		ASSERT_NE(m_pScene1,NULL_PTR) << "scene pointer is not found in "+std::to_string(i)+" no. iteration";

		m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource1,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));
		ASSERT_NE(m_pSceneAction1,NULL_PTR) << "first addNewSceneAction did not return scene action pointer in "+std::to_string(i)+" no. iteration";

		m_pSceneAction2 = m_pScene1->addNewSceneAction(m_pResource2,KEY_SPEED,RCSResourceAttributes::Value(VALUE_SPEED));
		ASSERT_NE(m_pSceneAction2,NULL_PTR) << "second addNewSceneAction did not return scene action pointer in "+std::to_string(i)+" no. iteration";

		try
		{
			m_pScene1->execute(&SMHelper::onExecute);
		}
		catch (exception& e)
		{
			SET_FAILURE("Exception occurred inside execute: " + std::string(e.what()));
			return;
		}

		CommonUtil::waitInSecond(MAX_SLEEP_TIME);

		if(SMHelper::s_checkExecuteCallback != true)
		{
			SET_FAILURE("execute callback is not called in "+std::to_string(i)+" no. iteration");
			return;
		}
	}

	SUCCEED();
}
#endif
