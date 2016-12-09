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
    std::shared_ptr<SceneAction> m_pSceneAction1;
    std::shared_ptr<SceneAction> m_pSceneAction2;
    std::shared_ptr<RCSRemoteResourceObject> m_pResource1;
    std::shared_ptr<RCSRemoteResourceObject> m_pResource2;
    bool m_isException;

    SMSceneIntegrationTest_stc() {
        m_pSMHelper = nullptr;
        m_pSceneList = nullptr;
        m_pSceneCollection = nullptr;
        m_pScene1 = nullptr;
        m_pSceneAction1 = nullptr;
        m_pSceneAction2 = nullptr;
        m_pResource1 = nullptr;
        m_pResource2 = nullptr;
        m_isException = false;
    }

    virtual void SetUp() {
        IOTIVITYTEST_LOG(INFO, "SetUp IN");

        CommonUtil::runCommonTCSetUpPart();

        CommonUtil::launchApp(FAN_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);
        CommonUtil::launchApp(LIGHT_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);

        m_pSMHelper = new SMHelper();
        m_pSMHelper->discoverResource();
        m_pSMHelper->stopDiscovery();

        m_pSceneList = m_pSMHelper->getSceneListInstance();
        ASSERT_NE(m_pSceneList,NULL_PTR) << "scenelist pointer is not found";

        m_pSceneCollection = m_pSceneList->addNewSceneCollection();
        ASSERT_NE(m_pSceneCollection,NULL_PTR) << "scene collection pointer is not found";

        m_pScene1 = m_pSceneCollection->addNewScene(SCENE_NAME_1);
        ASSERT_NE(m_pScene1,NULL_PTR) << "scene pointer is not found";

        ASSERT_GE(m_pSMHelper->g_foundResourceList.size(),INT_TWO) << "Light and Fan Resources are Not Found";

        m_pResource1 = m_pSMHelper->g_foundResourceList.at(INT_ZERO);
        m_pResource2 = m_pSMHelper->g_foundResourceList.at(INT_ONE);
    }

    virtual void TearDown() {
        IOTIVITYTEST_LOG(INFO, "TearDown IN");

        CommonUtil::killApp(FAN_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);
        CommonUtil::killApp(LIGHT_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);

        delete m_pSMHelper;
        m_pSMHelper = nullptr;

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
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource(for adding scene action)
 *                     2. key = "power"
 *                     3. value = "on"
 *                     4. RCSRemoteResourceObject::Ptr A pointer of discovered resource(for getting scene action)
 * @pre_condition     1. create light and fan type resource using LIGHT_SERVER and FAN_SERVER simulator
 *                     2. get SceneList instance
 *                     3. Get SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 * @procedure         1. call addNewSceneAction and check if scene action pointer is returned
 *                     2. call getSceneAction and check if scene action pointer is returned
 *                     3. check if added sceneAction's key and value matches with get sceneAction's key and value
 * @post_condition stop the simulators
 * @expected added sceneAction's key and value matches with get sceneAction's key and value
 */
#if defined(__LINUX__)
TEST_F(SMSceneIntegrationTest_stc, AddAndGetSceneActionWithKeyValue_GSRV_P)
{
    try
    {
        m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource1,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));
        ASSERT_NE(m_pSceneAction1,NULL_PTR) << "addNewSceneAction did not return scene action";
        m_pSceneAction2 = m_pScene1->getSceneAction(m_pResource1);
        ASSERT_NE(m_pSceneAction2,NULL_PTR) << "getSceneAction did not return scene action";
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
}
#endif

/**
 * @since 2016-07-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @objective Test addNewSceneAction and getSceneAction function positively for checking if added sceneAction is get
 * @target SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   RCSResourceAttributes attr)
 * @target SceneAction::Ptr getSceneAction(
 *                  const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject) const;
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource(for adding scene action)
 *                     2. RCSResourceAttributes Attributes to set when the Scene executed
 *                     3. pRCSRemoteResourceObject A pointer of discovered resource(for getting scene action)
 * @pre_condition     1. create light and fan type resource using LIGHT_SERVER and FAN_SERVER simulator
 *                     2. get SceneList instance
 *                     3. Get SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 * @procedure         1. call addNewSceneAction and check if scene action pointer is returned
 *                     2. call getSceneAction and check if scene action pointer is returned
 *                     3. check if added sceneAction's key and value matches with get sceneAction's key and value
 * @post_condition stop the simulators
 * @expected added sceneAction's key and value matches with get sceneAction's key and value
 */
#if defined(__LINUX__)
TEST_F(SMSceneIntegrationTest_stc, AddAndGetSceneActionWithAttribute_GSRV_P)
{
    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    try
    {
        m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource1,attribute);
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
}
#endif

/**
 * @since 2016-07-26
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @objective Test getSceneAction function negatively without adding scene action and
 *         check if no scene action is received
 * @target SceneAction::Ptr getSceneAction(
 *                  const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject) const;
 * @test_data RCSRemoteResourceObject::Ptr A pointer of discovered resource
 * @pre_condition     1. create light and fan type resource using LIGHT_SERVER and FAN_SERVER simulator
 *                     2. get SceneList instance
 *                     3. Get SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 * @procedure call getSceneAction and check if exceptions occured
 * @post_condition stop the simulators
 * @expected RCSInvalidParameterException should occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneIntegrationTest_stc, GetSceneActionWithoutAddingSceneAction_DSCC_N)
{
    try
    {
        m_pSceneAction1 = m_pScene1->getSceneAction(m_pResource1);
    }
    catch (const RCSInvalidParameterException& e)
    {
        IOTIVITYTEST_LOG(INFO,"Expected exception occured");
        m_isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexpected Exception occurred inside getSceneAction: " + std::string(e.what()));
        return;
    }

    if(m_isException == false)
    {
        SET_FAILURE("Expected exception did not occur");
        return;
    }

    ASSERT_EQ(m_pSceneAction1,NULL_PTR) << "getSceneAction should not return any scene action";
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
 *         for checking if added scene action is get
 * @target SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @target std::vector<SceneAction::Ptr> getSceneActions() const ;
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. key = "power"
 *                     3. value = "on"
 *                     4. RCSRemoteResourceObject::Ptr A pointer of another discovered resource(for adding multiple scene actions)
 *                     5. key = "speed"
 *                     6. value = "20"
 * @pre_condition     1. create light and fan type resource using LIGHT_SERVER and FAN_SERVER simulator
 *                     2. get SceneList instance
 *                     3. Get SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 * @procedure         1. call addNewSceneAction two times and check if scene actions are returned
 *                     2. call getSceneActions and check if list of scene actions are returned
 *                     3. check for both scene actions if added sceneAction's key and value matches with get sceneAction's key and value
 * @post_condition stop the simulators
 * @expected added sceneAction's key and value matches with get sceneAction's key and value
 */
#if defined(__LINUX__)
TEST_F(SMSceneIntegrationTest_stc, AddAndGetSceneActionsWithKeyValue_GSRV_P)
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
}
#endif

/**
 * @since 2016-07-28
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @objective Test addNewSceneAction and getSceneActions function positively
 *         for checking if added scene action is get
 * @target SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   RCSResourceAttributes attr)
 * @target std::vector<SceneAction::Ptr> getSceneActions() const ;
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. RCSResourceAttributes Attributes to set when the Scene executed(for first resource)
 *                     3. pRCSRemoteResourceObject A pointer of another discovered resource(for adding multiple scene actions)
 *                     4. RCSResourceAttributes Attributes to set when the Scene executed(for second resource)
 * @pre_condition     1. create light and fan type resource using LIGHT_SERVER and FAN_SERVER simulator
 *                     2. get SceneList instance
 *                     3. Get SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 * @procedure         1. call addNewSceneAction two times and check if scene actions are returned
 *                     2. call getSceneActions and check if list of scene actions are returned
 *                     3. check for both scene actions if added sceneAction's key and value matches with get sceneAction's key and value
 * @post_condition stop the simulators
 * @expected added sceneAction's key and value matches with get sceneAction's key and value
 */
#if defined(__LINUX__)
TEST_F(SMSceneIntegrationTest_stc, AddAndGetSceneActionsWithAttribute_GSRV_P)
{
    std::vector<SceneAction::Ptr> sceneActions;

    std::shared_ptr<SceneAction> m_pSceneAction3;
    std::shared_ptr<SceneAction> m_pSceneAction4;

    const RCSResourceAttributes attribute1;
    attribute1[KEY_POWER] = VALUE_ON;

    const RCSResourceAttributes attribute2;
    attribute2[KEY_SPEED] = VALUE_SPEED;

    try
    {
        m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource1,attribute1);
        ASSERT_NE(m_pSceneAction1,NULL_PTR) << "first addNewSceneAction did not return scene action pointer";
        m_pSceneAction2 = m_pScene1->addNewSceneAction(m_pResource2,attribute2);
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
}
#endif

/**
 * @since 2016-07-26
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @objective Test getSceneActions function negatively without adding scene actions
 *         and checking if returned list of scene actions is empty
 * @target std::vector<SceneAction::Ptr> getSceneActions() const ;
 * @test_data none
 * @pre_condition     1. create light and fan type resource using LIGHT_SERVER and FAN_SERVER simulator
 *                     2. get SceneList instance
 *                     3. Get SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 * @procedure call getSceneActions and check if returned list of scene actions is empty
 * @post_condition stop the simulators
 * @expected returned list of scene actions is empty
 */
#if defined(__LINUX__)
TEST_F(SMSceneIntegrationTest_stc, GetSceneActionsWithoutAddingSceneActions_DSCC_N)
{
    std::vector<SceneAction::Ptr> sceneActions;

    try
    {
        sceneActions = m_pScene1->getSceneActions();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getSceneActions: " + std::string(e.what()));
        return;
    }

    if(sceneActions.size() != INT_ZERO)
    {
        SET_FAILURE("returned list of scene actions should be empty");
    }
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
 * @pre_condition     1. get SceneList instance
 *                     2. Get SceneCollection pointer by adding SceneCollection
 * @procedure         1. Get scene pointer by adding scene
 *                     2. call getName and check if it returns the same name as added
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

    if(receivedName != SCENE_NAME_1)
    {
        SET_FAILURE("received name doesnt match with the added scene's name");
    }
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
 * @target void execute(ExecuteCallback cb);
 * @test_data ExecuteCallback A callback to execute Scene
 * @pre_condition     1. create light and fan type resource using LIGHT_SERVER and FAN_SERVER simulator
 *                     2. get SceneList instance
 *                     3. Get SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 *                     5. call addNewSceneAction
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
        SET_FAILURE("execute callback is not called with proper error code");
    }
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
 * @target void execute(ExecuteCallback cb);
 * @test_data ExecuteCallback A callback to execute Scene
 * @pre_condition create light and fan type resource using LIGHT_SERVER and FAN_SERVER simulator
 * @procedure        1. get SceneList instance
 *                     2. Get SceneCollection pointer by adding SceneCollection
 *                     3. Get scene pointer by adding scene
 *                     4. call addNewSceneAction
 *                     5. call execute and check if callback is called
 *                     6. follow steps 1-5 for multiple times
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

        m_pScene1 = m_pSceneCollection->addNewScene(SCENE_NAME_1);
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
            SET_FAILURE("execute callback is not called with proper error code in "+std::to_string(i)+
                    " no. iteration");
            return;
        }
    }
}
#endif
