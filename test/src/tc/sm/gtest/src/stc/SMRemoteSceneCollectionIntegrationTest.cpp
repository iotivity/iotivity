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

class SMRemoteSceneCollectionIntegrationTest_stc: public ::testing::Test {
public:
    SMRemoteHelper* m_pSMRemoteHelper;
    std::shared_ptr<RemoteSceneList> m_pRemoteSceneList;
    std::shared_ptr<RemoteSceneCollection> m_pRemoteSceneCollection;
    RemoteScene::Ptr m_pRemoteScene;
    RemoteScene::Ptr m_pReceivedRemoteScene;
    bool m_isCallback;

    SMRemoteSceneCollectionIntegrationTest_stc() {
        m_pSMRemoteHelper = nullptr;
        m_pRemoteSceneList = nullptr;
        m_pRemoteSceneCollection = nullptr;
        m_pRemoteScene = nullptr;
        m_pReceivedRemoteScene = nullptr;
        m_isCallback = false;
    }

    virtual void SetUp() {
        IOTIVITYTEST_LOG(INFO, "SetUp IN");

        CommonUtil::runCommonTCSetUpPart();

        CommonUtil::launchApp(SCENE_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);

        m_pSMRemoteHelper = new SMRemoteHelper();
        m_pSMRemoteHelper->discoverResource();
        m_pSMRemoteHelper->stopDiscovery();

        ASSERT_NE(NULL_PTR,m_pSMRemoteHelper->g_pFoundSceneList) << "Local scene List resource was not found in discovery";

        m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
        ASSERT_NE(m_pRemoteSceneList,NULL_PTR) << "did not find remote scene list instance";

        m_pRemoteSceneList->addNewSceneCollection(
                std::bind(
                        &SMRemoteSceneCollectionIntegrationTest_stc::onRemoteSceneCollectionCreated,
                        this, placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        ASSERT_NE(NULL_PTR,m_pRemoteSceneCollection)<< "remote scene collection is not created";
    }

    virtual void TearDown() {
        IOTIVITYTEST_LOG(INFO, "TearDown IN");

        CommonUtil::runCommonTCTearDownPart();

        CommonUtil::killApp(SCENE_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);

        delete m_pSMRemoteHelper;
        m_pSMRemoteHelper = nullptr;
    }

public:
    void onRemoteSceneCollectionCreated(
            RemoteSceneCollection::Ptr remoteSceneCol, int eCode) {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__, eCode);

        if (eCode == SCENE_RESULT_OK) {
            m_pRemoteSceneCollection = remoteSceneCol;
            m_isCallback = true;
        } else {
            IOTIVITYTEST_LOG(INFO, "Create Remote scene collection failed.");
        }
    }

    void onRemoteSceneCreated(RemoteScene::Ptr remoteScene, int eCode) {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__, eCode);

        if (eCode == SCENE_RESULT_OK) {
            m_pRemoteScene = remoteScene;
            m_isCallback = true;
        } else {
            IOTIVITYTEST_LOG(INFO, "Create Remote scene failed.");
        }
    }

    void onSetName(int eCode) {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__, eCode);
        if (eCode == SCENE_RESULT_OK) {
            m_isCallback = true;
        }
    }
};

/**
 * @since 2016-03-12
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test addNewScene function on positive way to check if callback is called
 * @target void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @test_data        1. name = "Scene1"
 *                     2. AddNewSceneCallback A callback to receive created RemoteScene instance
 * @pre_condition     1. Start SceneList type resource using SCENE_SERVER simulator
 *                     2. discover resource
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure         1. call addNewScene to add a new scene
 *                     2. check onRemoteSceneCreated is invoked with proper error code
 * @post_condition stop the simulator
 * @expected callback will be called with proper error code and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionIntegrationTest_stc, RemoteAddNewScene_CV_P)
{
    try
    {
        m_pRemoteSceneCollection->addNewScene(SCENE_NAME_1,std::bind(&SMRemoteSceneCollectionIntegrationTest_stc::onRemoteSceneCreated, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside addNewScene: " + std::string(e.what()));
        return;
    }

    if(!m_isCallback)
    {
        SET_FAILURE("AddNewSceneCallback is not invoked with proper error code");
    }
}
#endif

/**
 * @since 2016-03-12
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test addNewScene and getRemoteScene function on positive way to check
 *         if added scene is get
 * @target void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @target RemoteScene::Ptr getRemoteScene(const std::string &sceneName) const;
 * @test_data        1. name = "Scene1"(for adding scene)
 *                     2. AddNewSceneCallback A callback to receive created RemoteScene instance
 *                     3. sceneName = "Scene1"(for getting scene)
 * @pre_condition     1. Start SceneList type resource using SCENE_SERVER simulator
 *                     2. discover resource
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure         1. call addNewScene to add a new scene
 *                     2. call getRemoteScene by scene name
 *                     3. check return value is not null and equals to added new scene
 * @post_condition stop the simulator
 * @expected getRemoteScene will return the same as added scene pointer and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionIntegrationTest_stc, RemoteAddAndGetScene_GSRV_P)
{
    try
    {
        m_pRemoteSceneCollection->addNewScene(SCENE_NAME_1,std::bind(&SMRemoteSceneCollectionIntegrationTest_stc::onRemoteSceneCreated, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        ASSERT_NE(m_pRemoteScene,NULL_PTR) << "remote scene was not added";

        m_pReceivedRemoteScene = m_pRemoteSceneCollection->getRemoteScene(SCENE_NAME_1);
        ASSERT_NE(m_pReceivedRemoteScene,NULL_PTR) << "did not receive remote scene";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred: " + std::string(e.what()));
        return;
    }

    ASSERT_EQ(m_pReceivedRemoteScene, m_pRemoteScene) << "added and received scenes do not match";
}
#endif

/**
 * @since 2016-03-12
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test addNewScene and getRemoteScenes function on positive
 *             way to check if added scenes are get
 * @target void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @target std::unordered_map< std::string, RemoteScene::Ptr > getRemoteScenes() const
 * @test_data        1. First scene's name = "Scene1"
 *                     2. AddNewSceneCallback A callback to receive created RemoteScene instance
 *                     3. Second scene's name = "Scene2"
 *                     4. AddNewSceneCallback A callback to receive created RemoteScene instance
 * @pre_condition     1. Start SceneList type resource using SCENE_SERVER simulator
 *                     2. discover resource
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure         1. call addNewScene to add a new scene
 *                     2. call getRemoteScenes to get all RemoteScene instances
 *                        from RemoteSceneCollection instance
 *                     3. check if returned list of scenes contains added scenes
 * @post_condition stop the simulator
 * @expected getRemoteScenes should return all created remote scene and should not occur any exception
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionIntegrationTest_stc, RemoteAddAndGetScenes_GSRV_P)
{
    RemoteScene::Ptr m_pRemoteScene1 = nullptr;
    RemoteScene::Ptr m_pRemoteScene2 = nullptr;
    std::unordered_map< std::string, RemoteScene::Ptr > remoteScenes;

    try
    {
        m_pRemoteSceneCollection->addNewScene(SCENE_NAME_1,std::bind(&SMRemoteSceneCollectionIntegrationTest_stc::onRemoteSceneCreated, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        ASSERT_NE(m_pRemoteScene,NULL_PTR) << "first remote scene is not created";
        m_pRemoteScene1 = m_pRemoteScene;

        m_pRemoteSceneCollection->addNewScene(SCENE_NAME_2,std::bind(&SMRemoteSceneCollectionIntegrationTest_stc::onRemoteSceneCreated, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        ASSERT_NE(m_pRemoteScene,NULL_PTR) << "second remote scene is not created";
        m_pRemoteScene2 = m_pRemoteScene;

        remoteScenes = m_pRemoteSceneCollection->getRemoteScenes();
        EXPECT_EQ(remoteScenes.size(),INT_TWO) << "did not get the added scenes";

        ASSERT_EQ(m_pRemoteScene1,remoteScenes.at(SCENE_NAME_1)) << "first received scene did not match the firstly added scene";
        ASSERT_EQ(m_pRemoteScene2,remoteScenes.at(SCENE_NAME_2)) << "second received scene did not match the secondly added scene";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-07-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test getRemoteScenes function on negative way without adding remote scenes
 *         and check if returned list of remote scenes is empty
 * @target std::unordered_map< std::string, RemoteScene::Ptr > getRemoteScenes() const
 * @test_data none
 * @pre_condition     1. Start SceneList type resource using SCENE_SERVER simulator
 *                     2. discover resource
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure call getRemoteScenes and check if returned list of scenes is empty
 * @post_condition stop the simulator
 * @expected getRemoteScenes should return empty list and should not occur any exception
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionIntegrationTest_stc, RemoteGetScenesWithoutAddingRemoteScenes_DSCC_N)
{
    std::unordered_map< std::string, RemoteScene::Ptr > remoteScenes;

    try
    {
        remoteScenes = m_pRemoteSceneCollection->getRemoteScenes();
        EXPECT_EQ(remoteScenes.size(),INT_ZERO) << "returned list of remote scenes should be empty";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getRemoteScenes: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-13
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test setName function on positive way to check if callback is called
 * @target void setName(const std::string &name, SetNameCallback cb)
 * @test_data        1. name = "Kitchen"
 *                     2. SetNameCallback A callback to receive the response
 * @pre_condition     1. Start SceneList type resource using SCENE_SERVER simulator
 *                     2. discover resource
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure         1. call addNewScene to add a new scene
 *                     2. call setName
 *                     3. check onRemoteSceneCreated callback is invoked with proper error code
 * @post_condition stop the simulator
 * @expected callback should be called with proper error code and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionIntegrationTest_stc, RemoteSceneCollectionSetName_CV_P)
{
    try
    {
        m_pRemoteSceneCollection->setName(SCENE_COLLECTION_NAME,std::bind(&SMRemoteSceneCollectionIntegrationTest_stc::onSetName, this,placeholders::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside setName: " + std::string(e.what()));
        return;
    }

    if(!m_isCallback)
    {
        SET_FAILURE("SetNameCallback is not invoked with proper error code");
    }
}
#endif

/**
 * @since 2016-03-13
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test setName and getName function on positive way to check
 *             if the set name is received
 * @target void setName(const std::string &name, SetNameCallback cb)
 * @target std::string getName() const
 * @test_data        1. name = "Kitchen"
 *                     2. SetNameCallback A callback to receive the response
 * @pre_condition     1. Start SceneList type resource using SCENE_SERVER simulator
 *                     2. discover resource
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure         1. call addNewScene to add a new scene
 *                     2. call setName with empty SceneCollection name
 *                     3. call getName to get a name attribute of the SceneCollection resource
 *                     4. check set and get name is equal
 * @post_condition stop the simulator
 * @expected received name and set name should match
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionIntegrationTest_stc, RemoteSceneCollectionSetAndGetName_GSRV_P)
{
    std::string receivedName = "";

    try
    {
        m_pRemoteSceneCollection->setName(SCENE_COLLECTION_NAME,std::bind(&SMRemoteSceneCollectionIntegrationTest_stc::onSetName, this,placeholders::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        receivedName = m_pRemoteSceneCollection->getName();
        ASSERT_EQ(receivedName,SCENE_COLLECTION_NAME) << "received name and set name did not match";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-07-28
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @objective Test getName function on negative way without calling setName and check if no name
 *          is returned
 * @target std::string getName() const
 * @test_data none
 * @pre_condition     1. Start SceneList type resource using SCENE_SERVER simulator
 *                     2. discover resource
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 * @procedure call getName
 * @post_condition stop the simulator
 * @expected received name should be empty
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneCollectionIntegrationTest_stc, RemoteSceneCollectionGetNameWithoutSettingName_DSCC_N)
{
    std::string receivedName = "";

    try
    {
        receivedName = m_pRemoteSceneCollection->getName();
        ASSERT_EQ(receivedName,EMPTY_STRING) << "received name should be empty";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getName: " + std::string(e.what()));
    }
}
#endif
