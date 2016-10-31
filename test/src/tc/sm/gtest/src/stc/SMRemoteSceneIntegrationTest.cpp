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

class SMRemoteSceneIntegrationTest_stc: public ::testing::Test {
public:
    SMRemoteHelper* m_pSMRemoteHelper;
    std::shared_ptr<RemoteSceneList> m_pRemoteSceneList;
    std::shared_ptr<RemoteSceneCollection> m_pRemoteSceneCollection;
    std::shared_ptr<RCSRemoteResourceObject> m_pRemoteResource1;
    std::shared_ptr<RCSRemoteResourceObject> m_pRemoteResource2;
    RemoteSceneAction::Ptr m_pRemoteSceneAction;
    RemoteSceneAction::Ptr m_pReceivedRemoteSceneAction;
    RemoteScene::Ptr m_pRemoteScene;
    bool m_isCallback;

    SMRemoteSceneIntegrationTest_stc() {
        m_pSMRemoteHelper = nullptr;
        m_pRemoteSceneList = nullptr;
        m_pRemoteSceneCollection = nullptr;
        m_pRemoteScene = nullptr;
        m_pRemoteSceneAction = nullptr;
        m_pReceivedRemoteSceneAction = nullptr;
        m_pRemoteResource1 = nullptr;
        m_pRemoteResource2 = nullptr;
        m_isCallback = false;
    }

    virtual void SetUp() {
        IOTIVITYTEST_LOG(INFO, "SetUp IN");

        CommonUtil::runCommonTCSetUpPart();

        CommonUtil::launchApp(FAN_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);

        CommonUtil::launchApp(LIGHT_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);

        CommonUtil::launchApp(SCENE_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);

        m_pSMRemoteHelper = new SMRemoteHelper();
        m_pSMRemoteHelper->discoverResource();
        m_pSMRemoteHelper->stopDiscovery();

        ASSERT_NE(NULL_PTR,m_pSMRemoteHelper->g_pFoundSceneList) << "Local scene List resource was not found in discovery";

        m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
        ASSERT_NE(m_pRemoteSceneList,NULL_PTR)<< "did not find remote scene list instance";

        m_pRemoteSceneList->addNewSceneCollection(
                std::bind(
                        &SMRemoteSceneIntegrationTest_stc::onRemoteSceneCollectionCreated,
                        this, placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        ASSERT_NE(m_pRemoteSceneCollection,NULL_PTR)<< "did not find remote scene collection instance";

        m_pRemoteSceneCollection->addNewScene(SCENE_NAME_1,
                std::bind(
                        &SMRemoteSceneIntegrationTest_stc::onRemoteSceneCreated,
                        this, placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        ASSERT_NE(m_pRemoteScene,NULL_PTR)<< "remote scene is not created";

        ASSERT_GE(m_pSMRemoteHelper->g_vecFoundResourceList.size(),INT_TWO)<< "Light and Fan Resources are Not Found";
        m_pRemoteResource1 = m_pSMRemoteHelper->g_vecFoundResourceList.at(
        INT_ZERO);
        m_pRemoteResource2 = m_pSMRemoteHelper->g_vecFoundResourceList.at(
        INT_ONE);
    }

    virtual void TearDown() {
        IOTIVITYTEST_LOG(INFO, "TearDown IN");

        CommonUtil::killApp(FAN_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);
        CommonUtil::killApp(LIGHT_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);
        CommonUtil::killApp(SCENE_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);

        CommonUtil::runCommonTCTearDownPart();
    }

public:
    void onRemoteSceneCollectionCreated(
            RemoteSceneCollection::Ptr remoteSceneCol, int eCode) {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__,
                eCode);

        if (eCode == SCENE_RESULT_OK) {
            m_pRemoteSceneCollection = remoteSceneCol;
            m_isCallback = true;
        } else {
            IOTIVITYTEST_LOG(INFO, "Create Remote scene collection failed.");
        }
    }

    void onRemoteSceneCreated(RemoteScene::Ptr remoteScene, int eCode) {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__,
                eCode);

        if (eCode == SCENE_RESULT_OK) {
            m_pRemoteScene = remoteScene;
            m_isCallback = true;
        } else {
            IOTIVITYTEST_LOG(INFO, "Create Remote scene failed");
        }
    }

    void onRemoteSceneActionCreated(RemoteSceneAction::Ptr remoteSceneAction,
            int eCode) {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__,
                eCode);

        if (eCode == SCENE_RESULT_OK) {
            m_pRemoteSceneAction = remoteSceneAction;
            m_isCallback = true;
        } else {
            IOTIVITYTEST_LOG(INFO, "Create Remote scene action failed");
        }
    }

    void onRemoteSceneExecuted(const std::string &sceneName, int eCode) {
        IOTIVITYTEST_LOG(INFO,"%s is called with error code: %d, scene name: %s", __func__,
                eCode, sceneName.c_str());
        if (eCode == SCENE_RESULT_OK) {
            m_isCallback = true;
        }
    }
};

/**
 * @since 2016-03-13
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction function on positive way for checking if callback is called
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @test_data none    1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. key = "power"
 *                     3. value = "off"
 *                     4. AddNewSceneActionCallback A callback to receive created RemoteSceneAction instance
 * @pre_condition     1. Start SceneList,light and fan type resource using SCENE_SERVER,LIGHT_SERVER and FAN_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure          1. call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 *                     2. check onRemoteSceneActionCreated callback is invoked with proper error code.
 * @post_condition stop the simulators
 * @expected callback will be called with proper error code and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneIntegrationTest_stc, RemoteAddNewSceneActionWithKeyValue_CV_P)
{
    try
    {
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource1, KEY_POWER, RCSResourceAttributes::Value(VALUE_OFF),
                std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside addNewSceneAction: " + std::string(e.what()));
        return;
    }

    if(!m_isCallback)
    {
        SET_FAILURE("AddNewSceneActionCallback is not invoked with proper error code");
    }
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction and getRemoteSceneAction function on positive basic way
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @target RemoteSceneAction::Ptr getRemoteSceneAction(
 *                  const RCSRemoteResourceObject::Ptr targetResource) const;
 * @test_data none    1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. key = "power"
 *                     3. value = "off"
 *                     4. AddNewSceneActionCallback A callback to receive created RemoteSceneAction instance
 *                     5. targetResource A pointer of discovered resource
 * @pre_condition     1. Start SceneList,light and fan type resource using SCENE_SERVER,LIGHT_SERVER and FAN_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure          1. call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 *                     2. call getRemoteSceneAction to get RemoteSceneAction instance
 *                     3. check new SceneAction is added properly.
 *                     4. check getRemoteSceneAction return a valid remote scene action that
 *                        equal to added scene action
 * @post_condition stop the simulators
 * @expected getRemoteSceneAction will return same as the added sceneAction pointer and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneIntegrationTest_stc, RemoteAddAndGetSceneActionWithKeyValue_GSRV_P)
{
    try
    {
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource1, KEY_POWER, RCSResourceAttributes::Value(VALUE_OFF),
                std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "remote scene action is not created";

        m_pReceivedRemoteSceneAction = m_pRemoteScene->getRemoteSceneAction(m_pRemoteResource1);

        ASSERT_NE(m_pReceivedRemoteSceneAction,NULL_PTR) << "did not receive remote scene action";
        ASSERT_EQ(m_pReceivedRemoteSceneAction,m_pRemoteSceneAction) << "received remote scene action and added action does not match";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred: " + std::string(e.what()));
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
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction function on positive way to check if callback is called
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                   const RCSResourceAttributes &attrs, AddNewSceneActionCallback cb)
 * @test_data none    1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. RCSResourceAttributes AttributeS to set when the Scene executed
 *                     3. AddNewSceneActionCallback A callback to receive created RemoteSceneAction instance
 * @pre_condition     1. Start SceneList,light and fan type resource using SCENE_SERVER,LIGHT_SERVER and FAN_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure          1. call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 *                     2. check onRemoteSceneActionCreated callback is invoked with proper error code
 * @post_condition stop the simulators
 * @expected callback will be called with proper error code and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneIntegrationTest_stc, RemoteAddNewSceneActionWithAttribute_CV_P)
{
    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    try
    {
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource1,attribute,
                std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside addNewSceneAction: " + std::string(e.what()));
        return;
    }

    if(!m_isCallback)
    {
        SET_FAILURE("AddNewSceneActionCallback is not invoked with proper error code");
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
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction and getRemoteSceneAction function on positive way to check if added action is get
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                   const RCSResourceAttributes &attrs, AddNewSceneActionCallback cb)
 * @target RemoteSceneAction::Ptr getRemoteSceneAction(
 *                  const RCSRemoteResourceObject::Ptr targetResource) const;
 * @test_data none    1. RCSRemoteResourceObject::Ptr A pointer of discovered resource(for adding remote scene action)
 *                     2. RCSResourceAttributes AttributeS to set when the Scene executed
 *                     3. AddNewSceneActionCallback A callback to receive created RemoteSceneAction instance
 *                     4. RCSRemoteResourceObject::Ptr A pointer of discovered resource(for getting remote scene action)
 * @pre_condition     1. Start SceneList,light and fan type resource using SCENE_SERVER,LIGHT_SERVER and FAN_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure         1. call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 *                     2. call getRemoteSceneAction to get RemoteSceneAction
 *                     3. check new SceneAction is added properly.
 *                     4. check getRemoteSceneAction return a valid remote scene action that
 *                        equal to added scene action
 * @post_condition stop the simulators
 * @expected getRemoteSceneAction will return the same as added sceneAction pointer and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneIntegrationTest_stc, RemoteAddAndGetSceneActionWithAttribute_GSRV_P)
{
    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    try
    {
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource1,attribute,
                std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "remote scene action was not added";

        m_pReceivedRemoteSceneAction = m_pRemoteScene->getRemoteSceneAction(m_pRemoteResource1);
        ASSERT_NE(m_pReceivedRemoteSceneAction,NULL_PTR) << "could not receive remote scene action";

        ASSERT_EQ(m_pReceivedRemoteSceneAction,m_pRemoteSceneAction) << "added remote scene action and received action does not match";
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
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test getRemoteSceneAction function on negative way without adding remote scene action
 *         and check if no remote scene action is added
 * @target RemoteSceneAction::Ptr getRemoteSceneAction(
 *                  const RCSRemoteResourceObject::Ptr targetResource) const;
 * @test_data RCSRemoteResourceObject::Ptr A pointer of discovered resource
 * @pre_condition     1. Start SceneList,light and fan type resource using SCENE_SERVER,LIGHT_SERVER and FAN_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure call getRemoteSceneAction to get RemoteSceneAction
 * @post_condition stop the simulators
 * @expected getRemoteSceneAction should not return any scene action pointer and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneIntegrationTest_stc, RemoteGetSceneActionWithoutAddingRemoteSceneAction_DSCC_N)
{
    bool isException = false;
    try
    {
        m_pReceivedRemoteSceneAction = m_pRemoteScene->getRemoteSceneAction(m_pRemoteResource1);
    }
    catch (const RCSInvalidParameterException& e)
    {
        IOTIVITYTEST_LOG(INFO,"Expected exception occured");
        isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexpected Exception occurred inside getRemoteSceneAction: " + std::string(e.what()));
        return;
    }

    if(isException == false)
    {
        SET_FAILURE("Expected exception did not occur");
        return;
    }
    ASSERT_EQ(m_pReceivedRemoteSceneAction,NULL_PTR) << "should not receive any remote scene action";
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
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction and getSceneActions functions on positive way to check if added actions are get
 * @target std::vector<SceneAction::Ptr> getSceneActions() const
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                     const std::string &key,
 *                                     const RCSResourceAttributes::Value &value,
 *                                     AddNewSceneActionCallback cb);
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. key = "power"
 *                     3. value = "off"
 *                     4. AddNewSceneActionCallback A callback to receive created RemoteSceneAction instance
 * @pre_condition     1. Start SceneList,light and fan type resource using SCENE_SERVER,LIGHT_SERVER and FAN_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure          1. call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 *                     2. call getRemoteSceneActions to get all RemoteSceneAction instances
 *                        included in the Scene
 *                     3. check new SceneAction is added properly
 *                     4. check getRemoteSceneAction return a valid remote scene action that
 *                        equal to added scene action
 * @post_condition stop the simulators
 * @expected added scene action will be received
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneIntegrationTest_stc, RemoteAddAndGetSceneActionsWithKeyValue_GSRV_P)
{
    std::vector<RemoteSceneAction::Ptr> remoteSceneActions;

    try
    {
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource1, KEY_POWER, RCSResourceAttributes::Value(VALUE_OFF),
                std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        ASSERT_NE(NULL_PTR,m_pRemoteSceneAction) << "remote scene action was not added";

        remoteSceneActions = m_pRemoteScene->getRemoteSceneActions();
        ASSERT_EQ(INT_ONE, remoteSceneActions.size()) << "did not get list of added remote scene actions";

        ASSERT_EQ(m_pRemoteSceneAction,remoteSceneActions.at(INT_ZERO)) << "added and received scene action does not match";
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
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction and getSceneActions functions on positive way to check if added actions are get
 * @target std::vector<SceneAction::Ptr> getSceneActions() const
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                   const RCSResourceAttributes &attrs, AddNewSceneActionCallback cb)
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. RCSResourceAttributes Attributes to set when the Scene executed
 *                     3. AddNewSceneActionCallback A callback to receive created RemoteSceneAction instance
 * @pre_condition     1. Start SceneList,light and fan type resource using SCENE_SERVER,LIGHT_SERVER and FAN_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure          1. call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 *                     2. call getRemoteSceneActions to get all RemoteSceneAction instances
 *                        included in the Scene
 *                     3. check new SceneAction is added properly
 *                     4. check getRemoteSceneAction return a valid remote scene action that
 *                        equal to added scene action
 * @post_condition stop the simulators
 * @expected added scene action will be received
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneIntegrationTest_stc, RemoteAddAndGetSceneActionsWithAttribute_GSRV_P)
{
    std::vector<RemoteSceneAction::Ptr> remoteSceneActions;

    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    try
    {
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource1,attribute,
                std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "remote scene action was not added";

        remoteSceneActions = m_pRemoteScene->getRemoteSceneActions();
        ASSERT_EQ(INT_ONE, remoteSceneActions.size()) << "did not get list of added remote scene actions";

        ASSERT_EQ(m_pRemoteSceneAction,remoteSceneActions.at(INT_ZERO)) << "added and received scene action does not match";
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
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test getSceneActions functions on negative way without adding remote scene actions and
 *         check if empty list of scene actions is returned
 * @target std::vector<SceneAction::Ptr> getSceneActions() const
 * @test_data none
 * @pre_condition     1. Start SceneList,light and fan type resource using SCENE_SERVER,LIGHT_SERVER and FAN_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure call getRemoteSceneActions and check if empty list of remote scene actions is returned
 * @post_condition stop the simulators
 * @expected received list of remote scene actions is empty
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneIntegrationTest_stc, RemoteGetSceneActionsWithoutAddingRemoteSceneAction_DSCC_N)
{
    std::vector<RemoteSceneAction::Ptr> remoteSceneActions;

    try
    {
        remoteSceneActions = m_pRemoteScene->getRemoteSceneActions();
        ASSERT_EQ(INT_ZERO, remoteSceneActions.size()) << "list of remote scene actions should be empty";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getRemoteSceneActions: " + std::string(e.what()));
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
 * @objective Test addNewScene and getName function on positive way to check if added scene's name is get
 * @target void addNewScene(const std::string &name, AddNewSceneCallback cb);
 * @target std::string getName() const
 * @test_data         1. name = "Scene1"
 *                     2. AddNewSceneCallback a callback to receive the added scene
 * @pre_condition     1. Start SceneList,light and fan type resource using SCENE_SERVER,LIGHT_SERVER and FAN_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure          1. call getName to get a name attribute of the Scene
 *                     2. check return attribute name is equal to created scene name
 * @post_condition stop the simulators
 * @expected getName will return the same name as the added scene's name and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneIntegrationTest_stc, RemoteSceneSetAndGetName_GSRV_P)
{
    std::string receivedName;
    try
    {
        receivedName = m_pRemoteScene->getName();
        ASSERT_EQ(receivedName,SCENE_NAME_1) << "received name is not same as the added scene's name";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @objective Test execute function on positive way to check if callback is called
 * @target void execute(RemoteSceneExecuteCallback cb)
 * @test_data RemoteSceneExecuteCallback A callback to receive result of Scene execution
 * @pre_condition     1. Start SceneList,light and fan type resource using SCENE_SERVER,LIGHT_SERVER and FAN_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 *                     6. call addNewSceneAction two times to add new SceneAction to
 *                        the Scene on remote side
 * @procedure          1. call execute to to execute the Scene on remote side
 *                     2. check execute is performed successfully without any
 *                        exception and RemoteSceneExecuteCallback is invoked with proper error code
 * @post_condition stop the simulators
 * @expected RemoteSceneExecuteCallback is called with proper error code and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneIntegrationTest_stc, RemoteExecuteActionWithMultipleResources_SQV_CV_P)
{
    m_pRemoteScene->addNewSceneAction(m_pRemoteResource1, KEY_POWER, RCSResourceAttributes::Value(VALUE_OFF),
            std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "could not create first remote scene action";

    m_pRemoteSceneAction = nullptr;
    m_pRemoteScene->addNewSceneAction(m_pRemoteResource2, KEY_SPEED, RCSResourceAttributes::Value(VALUE_SPEED),
            std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

    ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "could not create second remote scene action";

    try
    {
        m_pRemoteScene->execute(std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneExecuted, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside execute: " + std::string(e.what()));
        return;
    }

    if(!m_isCallback)
    {
        SET_FAILURE("onRemoteSceneExecuted callback not invoked with proper error code");
    }
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @objective Test total procedure of executing a remote scene, for multiple times
 * @target void execute(RemoteSceneExecuteCallback cb)
 * @test_data RemoteSceneExecuteCallback A callback to receive result of Scene execution
 * @pre_condition     1. Start SceneList,light and fan type resource using SCENE_SERVER,LIGHT_SERVER and FAN_SERVER simulators
 *                     2. discover resources
 * @procedure          1. get remote SceneList instance
 *                     2. get remote SceneCollection pointer by adding SceneCollection
 *                     3. Get scene pointer by adding scene
 *                     4. call addNewSceneAction two times to add new SceneAction to
 *                        the Scene on remote side
 *                     5. call execute to to execute the Scene on remote side
 *                     6. check execute is performed successfully without any
 *                        exception and RemoteSceneExecuteCallback is invoked with proper error code
 *                     7. follow steps 1-6 for multiple times
 * @post_condition stop the simulators
 * @expected RemoteSceneExecuteCallback is called each time with proper error code and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneIntegrationTest_stc, RemoteExecuteSameActionMultipleTimes_VLCC_P)
{
    for(int i=1; i<4; i++)
    {
        m_pRemoteSceneList = nullptr;
        m_pRemoteSceneCollection = nullptr;
        m_pRemoteScene = nullptr;
        m_pRemoteSceneAction = nullptr;
        m_isCallback = false;

        m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
        ASSERT_NE(m_pRemoteSceneList,NULL_PTR) << "did not find remote scene list instance in "+std::to_string(i)+" no. iteration";

        m_pRemoteSceneList->addNewSceneCollection(
                std::bind(
                        &SMRemoteSceneIntegrationTest_stc::onRemoteSceneCollectionCreated,
                        this, placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        ASSERT_NE(m_pRemoteSceneCollection,NULL_PTR) << "did not find remote scene collection instance in "+std::to_string(i)+" no. iteration";

        m_pRemoteSceneCollection->addNewScene(SCENE_NAME_1,
                std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneCreated,
                        this, placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        ASSERT_NE(m_pRemoteScene,NULL_PTR)<< "remote scene is not created in "+std::to_string(i)+" no. iteration";

        m_pRemoteScene->addNewSceneAction(m_pRemoteResource1, KEY_POWER, RCSResourceAttributes::Value(VALUE_OFF),
                std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "could not create first remote scene action in "+std::to_string(i)+" no. iteration";

        m_pRemoteSceneAction = nullptr;
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource2, KEY_SPEED, RCSResourceAttributes::Value(VALUE_SPEED),
                std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "could not create second remote scene action in "+std::to_string(i)+" no. iteration";

        try
        {
            m_pRemoteScene->execute(std::bind(&SMRemoteSceneIntegrationTest_stc::onRemoteSceneExecuted, this,placeholders::_1, placeholders::_2));
            CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        }
        catch (exception& e)
        {
            SET_FAILURE("Exception occurred inside execute: " + std::string(e.what()));
            return;
        }

        if(!m_isCallback)
        {
            SET_FAILURE("onRemoteSceneExecuted callback not invoked in "+std::to_string(i)+" no. iteration");
            return;
        }
    }
}
#endif
