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

class SMSceneCollectionIntegrationTest_stc: public ::testing::Test {
public:
	SMHelper* m_pSMHelper;
	SceneList* m_pSceneList;
	std::shared_ptr<SceneCollection> m_pSceneCollection;
	std::shared_ptr<Scene> m_pScene1;
	std::shared_ptr<Scene> m_pScene2;

	SMSceneCollectionIntegrationTest_stc() {
		m_pSMHelper = nullptr;
		m_pSceneList = nullptr;
		m_pSceneCollection = nullptr;
		m_pScene1 = nullptr;
		m_pScene2 = nullptr;
	}

	virtual void SetUp() {
		IOTIVITYTEST_LOG(INFO, "SetUp IN");

		CommonUtil::runCommonTCSetUpPart();
		m_pSMHelper = SMHelper::getInstance();
		m_pSceneList = m_pSMHelper->getSceneListInstance();
		ASSERT_NE(m_pSceneList,NULL_PTR) << "scenelist pointer is not found";

		m_pSceneCollection = m_pSceneList->addNewSceneCollection();
		ASSERT_NE(m_pSceneCollection,NULL_PTR) << "scene collection pointer is not found";
	}

	virtual void TearDown() {
		IOTIVITYTEST_LOG(INFO, "TearDown IN");

		CommonUtil::runCommonTCTearDownPart();
	}
};

/**
 * @since 2016-03-01
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @objective Test addNewScene and getScene function positively to check if added scene is get
 * @target Scene::Ptr addNewScene(const std::string& sceneName);
 * @target Scene::Ptr getScene(const std::string& sceneName) const;
 * @target std::string getName() const;
 * @test_data sceneName = "Scene1"
 * @pre_condition	1. get SceneList instance
 * 				  	2. get SceneCollection pointer by adding SceneCollection
 * @procedure 	  	1. call addNewScene and check if scene pointer is returned
 *    			  	2. call getScene and check if scene pointer is returned
 *                	3. check if added scene's name and get scene's name is same
 * @post_condition none
 * @expected received scene's name will be same as set scene's name
 */
#if defined(__LINUX__)
TEST_F(SMSceneCollectionIntegrationTest_stc, AddAndGetScene_GSRV_P)
{
	std::shared_ptr<Scene> receivedScene = nullptr;

	const std::string sceneName1 = "Scene1";

	try
	{
		m_pScene1 = m_pSceneCollection->addNewScene(sceneName1);
		ASSERT_NE(m_pScene1,NULL_PTR) << "addNewScene did not return scene pointer";

		receivedScene = m_pSceneCollection->getScene(sceneName1);
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred: " + std::string(e.what()));
		return;
	}

	ASSERT_NE(receivedScene,NULL_PTR) << "scene is not received";

	if(m_pScene1->getName() != receivedScene->getName())
	{
		SET_FAILURE("added and get scene doesn't match");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-01
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @objective Test setName and getName function positively for checking if setName is get
 * @target void setName(const std::string& name);
 * @target std::string getName() const;
 * @test_data name = "Kitchen"
 * @pre_condition	1. get SceneList instance
 * 					2. Get SceneCollection pointer by adding SceneCollection
 * @procedure 		1. call setName with valid name
 * 					2. call getName and check if received is same as the set name
 * @post_condition none
 * @expected received name will be same as set name and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneCollectionIntegrationTest_stc, SceneCollectionSetAndGetName_GSRV_P)
{
	const std::string sceneCollectionName = "Kitchen";
	std::string receivedName = "";

	try
	{
		m_pSceneCollection->setName(sceneCollectionName);
		receivedName = m_pSceneCollection->getName();
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred: " + std::string(e.what()));
		return;
	}

	if(receivedName != sceneCollectionName)
	{
		SET_FAILURE("Get name did not receive the set name");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-03-01
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @objective Test addNewScene and getScenes function positively for checking if added scene is get
 * @target Scene::Ptr addNewScene(const std::string& sceneName);
 * @target std::unordered_map< std::string, Scene::Ptr > getScenes() const;
 * @test_data 		1. sceneName1 = "Scene1"
 * 					2. sceneName2 = "Scene2"
 * @pre_condition 	1. get SceneList instance
 *					2. Get SceneCollection pointer by adding SceneCollection
 * @procedure 		1. call addNewScene for two times and check if scene pointers are returned
 * 					2. call getScenes and check if returned list's size is two
 *					3. check if added scene's name and get scene's name matches
 * @post_condition none
 * @expected getScene's name will match with set scene's name
 */
#if defined(__LINUX__)
TEST_F(SMSceneCollectionIntegrationTest_stc, AddAndGetScenes_GSRV_P)
{
	std::shared_ptr<Scene> receivedScene1;
	std::shared_ptr<Scene> receivedScene2;

	const std::string sceneName1 = "Scene1";
	const std::string sceneName2 = "Scene2";

	std::unordered_map< std::string, Scene::Ptr > scenes;

	try
	{
		m_pScene1 = m_pSceneCollection->addNewScene(sceneName1);
		ASSERT_NE(m_pScene1,NULL_PTR) << "first scene is not added";
		m_pScene2 = m_pSceneCollection->addNewScene(sceneName2);
		ASSERT_NE(m_pScene2,NULL_PTR) << "second scene is not added";

		scenes = m_pSceneCollection->getScenes();

		ASSERT_EQ(scenes.size(),INT_TWO) << "Did not receive the added scenes";

		receivedScene1 = scenes.at(sceneName1);
		ASSERT_NE(receivedScene1,NULL_PTR) << "first scene is not received";
		receivedScene2 = scenes.at(sceneName2);
		ASSERT_NE(receivedScene2,NULL_PTR) << "second scene is not received";
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred: " + std::string(e.what()));
		return;
	}

	try
	{
		if(m_pScene1->getName() != receivedScene1->getName())
		{
			SET_FAILURE("first received scene did not match the firstly added scene");
		}

		if(m_pScene2->getName() != receivedScene2->getName())
		{
			SET_FAILURE("second received scene did not match the secondly added scene");
		}
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred while accessing the received scenes: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif
