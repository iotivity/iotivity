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

class SMSceneCollectionTest_btc: public ::testing::Test {
public:
    SMHelper* m_pSMHelper;
    SceneList* m_pSceneList;
    std::shared_ptr<SceneCollection> m_pSceneCollection;
    std::shared_ptr<Scene> m_pScene1;
    std::shared_ptr<Scene> m_pScene2;
    bool m_isException;

    SMSceneCollectionTest_btc() {
        m_pSMHelper = nullptr;
        m_pSceneList = nullptr;
        m_pSceneCollection = nullptr;
        m_pScene1 = nullptr;
        m_pScene2 = nullptr;
        m_isException = false;
    }

    virtual void SetUp() {
        IOTIVITYTEST_LOG(INFO, "SetUp IN");

        CommonUtil::runCommonTCSetUpPart();
        m_pSMHelper = new SMHelper();
        m_pSceneList = m_pSMHelper->getSceneListInstance();
        ASSERT_NE(m_pSceneList,NULL_PTR) << "scenelist pointer is not found";

        m_pSceneCollection = m_pSceneList->addNewSceneCollection();
        ASSERT_NE(m_pSceneCollection,NULL_PTR) << "scene collection pointer is not found";
    }

    virtual void TearDown() {
        IOTIVITYTEST_LOG(INFO, "TearDown IN");

        delete m_pSMHelper;
        m_pSMHelper = nullptr;

        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2016-03-01
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @objective Test addNewScene function positively
 * @target Scene::Ptr addNewScene(const std::string& sceneName);
 * @test_data sceneName = "Scene1"
 * @pre_condition    1. get SceneList instance
 *                      2. Get SceneCollection pointer by adding SceneCollection
 * @procedure call addNewScene and check if scene pointer is returned
 * @post_condition none
 * @expected addNewScene will return scene pointer and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneCollectionTest_btc, AddNewScene_SRC_P)
{
    try
    {
        m_pScene1 = m_pSceneCollection->addNewScene(SCENE_NAME_1);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside addNewScene:" + std::string(e.what()));
        return;
    }

    if(m_pScene1 == NULL_PTR)
    {
        SET_FAILURE("scene pointer is not found");
    }
}
#endif

/**
 * @since 2016-03-01
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @objective Test addNewScene function negatively with empty string
 * @target Scene::Ptr addNewScene(const std::string& sceneName);
 * @test_data sceneName = ""
 * @pre_condition     1. get SceneList instance
 *                       2. get SceneCollection pointer by adding SceneCollection
 * @procedure           1. call addNewScene with empty string
 *                      2. check if RCSInvalidParameterException has occured
 *                      3. check if scene pointer is not returned
 * @post_condition none
 * @expected addNewScene will not return scene pointer and throw RCSInvalidParameterException exception
 */
#if defined(__LINUX__)
TEST_F(SMSceneCollectionTest_btc, AddNewScene_ESV_ETC_N)
{
    try
    {
        m_pScene1 = m_pSceneCollection->addNewScene(EMPTY_STRING);
    }

    catch (const RCSInvalidParameterException& e)
    {
        IOTIVITYTEST_LOG(INFO,"Expected exception occured");
        m_isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexpected Exception occurred inside AddNewScene: " + std::string(e.what()));
        return;
    }

    if(m_isException == false)
    {
        SET_FAILURE("Expected exception did not occur");
    }
}
#endif

/**
 * @since 2016-03-01
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @objective Test getScene function positively
 * @target Scene::Ptr getScene(const std::string& sceneName) const;
 * @test_data sceneName = "Scene1"
 * @pre_condition    1. get SceneList instance
 *                       2. get SceneCollection pointer by adding SceneCollection
 *                    3. call addNewScene and check if scene is added
 * @procedure call getScene and check if scene pointer is returned
 * @post_condition none
 * @expected getScene will return scene pointer and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneCollectionTest_btc, GetScene_SRC_P)
{
    m_pScene1 = m_pSceneCollection->addNewScene(SCENE_NAME_1);
    ASSERT_NE(m_pScene1,NULL_PTR) << "scene is not added";

    try
    {
        m_pScene2 = m_pSceneCollection->getScene(SCENE_NAME_1);
        ASSERT_NE(m_pScene2,NULL_PTR) << "scene pointer is not found";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getScene: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-01
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @objective Test getScenes function positively
 * @target std::unordered_map< std::string, Scene::Ptr > getScenes() const;
 * @test_data none
 * @pre_condition    1. get SceneList instance
 *                      2. Get SceneCollection pointer by adding SceneCollection
 *                   3. call addNewScene for two times and check if scenes are added
 * @procedure call getScenes and check if list of scenes is returned
 * @post_condition none
 * @expected getScenes will succeed and return list of scenes
 */
#if defined(__LINUX__)
TEST_F(SMSceneCollectionTest_btc, GetScenes_SRC_P)
{
    m_pScene1 = m_pSceneCollection->addNewScene(SCENE_NAME_1);
    ASSERT_NE(m_pScene1,NULL_PTR) << "first scene is not added";
    m_pScene2 = m_pSceneCollection->addNewScene(SCENE_NAME_2);
    ASSERT_NE(m_pScene2,NULL_PTR) << "second scene is not added";

    std::unordered_map< std::string, Scene::Ptr > scenes;

    try
    {
        scenes = m_pSceneCollection->getScenes();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getScenes" + std::string(e.what()));
        return;
    }

    if(scenes.size() == INT_ZERO)
    {
        SET_FAILURE("list of scenes is not found");
    }
}
#endif

/**
 * @since 2016-03-01
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @objective Test setName function positively
 * @target void setName(const std::string& name);
 * @test_data name = "Kitchen"
 * @pre_condition    1. get SceneList instance
 *                       2. Get SceneCollection pointer by adding SceneCollection
 * @procedure call setName
 * @post_condition none
 * @expected setName will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneCollectionTest_btc, SceneCollectionSetName_SRC_P)
{
    try
    {
        m_pSceneCollection->setName(SCENE_COLLECTION_NAME);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside setName: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-01
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @objective Test setName function positively with empty string
 * @target void setName(const std::string& name);
 * @test_data name = ""
 * @pre_condition    1. get SceneList instance
 *                      2. Get SceneCollection pointer by adding SceneCollection
 * @procedure call setName with empty string
 * @post_condition none
 * @expected setName will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneCollectionTest_btc, SceneCollectionSetName_ESV_P)
{
    try
    {
        m_pSceneCollection->setName(EMPTY_STRING);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside setName: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-01
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see void setName(const std::string& name);
 * @objective Test getName function positively
 * @target std::string getName() const;
 * @test_data none
 * @pre_condition    1. get SceneList instance
 *                       2. Get SceneCollection pointer by adding SceneCollection
 *                    3. call setName with valid name
 * @procedure call getName and check if scene collection's name is returned
 * @post_condition none
 * @expected getName will return scene collection's name and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneCollectionTest_btc, SceneCollectionGetName_SRC_P)
{
    m_pSceneCollection->setName(SCENE_COLLECTION_NAME);

    std::string receivedName = "";

    try
    {
        receivedName = m_pSceneCollection->getName();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getName: " + std::string(e.what()));
        return;
    }

    if(receivedName == EMPTY_STRING)
    {
        SET_FAILURE("getName did not return scene collection name");
    }
}
#endif

/**
 * @since 2016-03-01
 * @see void Configure(const PlatformConfig& config);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @objective Test getId function positively
 * @target std::string getId() const;
 * @test_data none
 * @pre_condition    1. get SceneList instance
 *                      2. Get SceneCollection pointer by adding SceneCollection
 * @procedure call getId and check if id is returned
 * @post_condition none
 * @expected getId will return id and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneCollectionTest_btc, SceneCollectionGetId_SRC_P)
{
    string collectionId = "";

    try
    {
        collectionId = m_pSceneCollection->getId();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getId: " + std::string(e.what()));
        return;
    }

    if(collectionId == EMPTY_STRING)
    {
        SET_FAILURE("getId did not return collection id");
    }
}
#endif
