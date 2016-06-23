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

class SMSceneListTest_btc: public ::testing::Test {
public:
	SMHelper* m_pSMHelper;
	SceneList* m_pSceneList;
	std::shared_ptr<SceneCollection> m_pSceneCollection;
	std::string sceneListName;

	SMSceneListTest_btc() {
		m_pSMHelper = nullptr;
		m_pSceneList = nullptr;
		m_pSceneCollection = nullptr;
		sceneListName = "Home";
	}

	virtual void SetUp() {
		IOTIVITYTEST_LOG(INFO, "SetUp IN");

		CommonUtil::runCommonTCSetUpPart();
		m_pSMHelper = SMHelper::getInstance();
	}

	virtual void TearDown() {
		IOTIVITYTEST_LOG(INFO, "TearDown IN");

		CommonUtil::runCommonTCTearDownPart();
	}

};

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @objective Test getInstance function positively for getting SceneList instance
 * @target static SceneList* getInstance();
 * @test_data none
 * @pre_condition none
 * @procedure call getInstance and check if SceneList instance is returned
 * @post_condition none
 * @expected getInstance will succeed and return SceneList instance
 */
#if defined(__LINUX__)
TEST_F(SMSceneListTest_btc, GetInstance_SRC_P)
{
	m_pSceneList = m_pSMHelper->getSceneListInstance();

	if(m_pSceneList == NULL_PTR)
	{
		SET_FAILURE("scenelist pointer is not found");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @objective Test addNewSceneCollection function positively for adding SceneCollection
 * @target SceneCollection::Ptr addNewSceneCollection();
 * @test_data none
 * @pre_condition call getInstance method
 * @procedure call addNewSceneCollection and check if scene collection pointer is returned
 * @post_condition none
 * @expected addNewSceneCollection will return scene collection pointer and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneListTest_btc, AddNewSceneCollection_SRC_P)
{
	m_pSceneList = m_pSMHelper->getSceneListInstance();

	if(m_pSceneList == NULL_PTR)
	{
		SET_FAILURE("scenelist pointer is not found");
		return;
	}

	try
	{
		m_pSceneCollection = m_pSceneList->addNewSceneCollection();
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside addNewSceneCollection: " + std::string(e.what()));
		return;
	}

	if(m_pSceneCollection == NULL_PTR)
	{
		SET_FAILURE("scene collection pointer is not found");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @objective Test getSceneCollections function positively for getting list of SceneCollections
 * @target std::vector<SceneCollection::Ptr> getSceneCollections() const;
 * @test_data none
 * @pre_condition	1. call getInstance method
 * 					2. call addNewSceneCollection
 * @procedure call getSceneCollections and check if list of scene collections is returned
 * @post_condition none
 * @expected getSceneCollections will return list of scene collections and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneListTest_btc, GetSceneCollections_SRC_P)
{
	m_pSceneList = m_pSMHelper->getSceneListInstance();

	if(m_pSceneList == NULL_PTR)
	{
		SET_FAILURE("scenelist pointer is not found");
		return;
	}

	m_pSceneCollection = m_pSceneList->addNewSceneCollection();

	std::vector<SceneCollection::Ptr> sceneCollections;

	try
	{
		sceneCollections = m_pSceneList->getSceneCollections();
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside getSceneCollections: " + std::string(e.what()));
		return;
	}

	if(sceneCollections.size() == INT_ZERO)
	{
		SET_FAILURE("list of scene collections is not found");
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @objective Test setName function positively
 * @target void setName(const std::string& sceneListName);
 * @test_data sceneListName = "Home"
 * @pre_condition call getInstance method
 * @procedure call setName
 * @post_condition none
 * @expected setName will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneListTest_btc, SceneListSetName_SRC_P)
{
	m_pSceneList = m_pSMHelper->getSceneListInstance();

	if(m_pSceneList == NULL_PTR)
	{
		SET_FAILURE("scenelist pointer is not found");
		return;
	}

	try
	{
		m_pSceneList->setName(sceneListName);
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside setName: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @objective Test setName function positively by setting empty string as name
 * @target void setName(const std::string& sceneListName);
 * @test_data sceneListName = ""
 * @pre_condition call getInstance method
 * @procedure call setName
 * @post_condition none
 * @expected setName will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneListTest_btc, SceneListSetName_ESV_P)
{
	m_pSceneList = m_pSMHelper->getSceneListInstance();

	if(m_pSceneList == NULL_PTR)
	{
		SET_FAILURE("scenelist pointer is not found");
		return;
	}

	try
	{
		m_pSceneList->setName(EMPTY_STRING);
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside setName: " + std::string(e.what()));
	}

	SUCCEED();
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see void setName(const std::string& sceneListName);
 * @objective Test getName function positively
 * @target std::string getName() const;
 * @test_data none
 * @pre_condition	1. call getInstance method
 *    			 	2. call setName
 * @procedure call getName and check if name is returned
 * @post_condition none
 * @expected getName will return name and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneListTest_btc, SceneListGetName_SRC_P)
{
	m_pSceneList = m_pSMHelper->getSceneListInstance();

	if(m_pSceneList == NULL_PTR)
	{
		SET_FAILURE("scenelist pointer is not found");
		return;
	}

	m_pSceneList->setName(sceneListName);

	std::string receivedName = "";

	try
	{
		receivedName = m_pSceneList->getName();
	}
	catch (exception& e)
	{
		SET_FAILURE("Exception occurred inside getName: " + std::string(e.what()));
	}

	if(receivedName == EMPTY_STRING)
	{
		SET_FAILURE("getName did not return scene list name");
	}

	SUCCEED();
}
#endif
