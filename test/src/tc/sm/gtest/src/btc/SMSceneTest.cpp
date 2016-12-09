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

class SMSceneTest_btc: public ::testing::Test {
public:
    SMHelper* m_pSMHelper;
    SceneList* m_pSceneList;
    std::shared_ptr<SceneCollection> m_pSceneCollection;
    std::shared_ptr<Scene> m_pScene1;
    std::shared_ptr<SceneAction> m_pSceneAction1;
    std::shared_ptr<SceneAction> m_pSceneAction2;
    std::shared_ptr<RCSRemoteResourceObject> m_pResource;
    bool m_isException;

    SMSceneTest_btc() {
        m_pSMHelper = nullptr;
        m_pSceneList = nullptr;
        m_pSceneCollection = nullptr;
        m_pScene1 = nullptr;
        m_pSceneAction1 = nullptr;
        m_pSceneAction2 = nullptr;
        m_pResource = nullptr;
        m_isException = false;
    }

    virtual void SetUp() {
        IOTIVITYTEST_LOG(INFO, "SetUp IN");

        CommonUtil::runCommonTCSetUpPart();
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

        ASSERT_NE(m_pSMHelper->g_foundResourceList.size(),INT_ZERO) << "resource is not found";
        m_pResource = m_pSMHelper->g_foundResourceList.at(INT_ZERO);
    }

    virtual void TearDown() {
        IOTIVITYTEST_LOG(INFO, "TearDown IN");

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
 * @objective Test addNewSceneAction function positively for adding scene action using attribute
 * @target SceneAction::Ptr addNewSceneAction(
 *                   const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   RCSResourceAttributes attr);
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. RCSResourceAttributes A attribute set of key and value
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 * @procedure call addNewSceneAction and check if sceneAction pointer is returned
 * @post_condition stop the simulator
 * @expected addNewSceneAction will return sceneAction pointer and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneTest_btc, AddNewSceneActionWithAttribute_SRC_RSV_P)
{
    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    try
    {
        m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,attribute);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside addNewSceneAction: " + std::string(e.what()));
        return;
    }

    if(m_pSceneAction1 == NULL_PTR)
    {
        SET_FAILURE("scene action pointer is not found");
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
 * @objective Test addNewSceneAction function negatively using null resource
 * @target SceneAction::Ptr addNewSceneAction(
 *                   const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   RCSResourceAttributes attr);
 * @test_data         1. RCSRemoteResourceObject::Ptr NULL
 *                     2. RCSResourceAttributes A attribute set of key and value
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 * @procedure         1. call addNewSceneAction negatively using null value in place of remote resource object
 *                     2. check if RCSInvalidParameterException has occured
 *                     3. check if sceneAction pointer is not returned
 * @post_condition stop the simulator
 * @expected addNewSceneAction will not return sceneAction pointer and RCSInvalidParameterException should occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneTest_btc, AddNewSceneActionWithAttributeResource_NV_ETC_N)
{
    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    try
    {
        m_pSceneAction1 = m_pScene1->addNewSceneAction(NULL,attribute);
    }
    catch (const RCSInvalidParameterException& e)
    {
        IOTIVITYTEST_LOG(INFO,"Expected exception occured");
        m_isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexpected Exception occurred inside addNewSceneAction: " + std::string(e.what()));
        return;
    }

    if(m_isException == false)
    {
        SET_FAILURE("Expected exception did not occur");
    }

    if(m_pSceneAction1 != NULL_PTR)
    {
        SET_FAILURE("scene action pointer should not be found");
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
 * @objective Test addNewSceneAction function positively for adding scene action using key and value
 * @target SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. Key = "power"
 *                     3. value = "on"
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 * @procedure call addNewSceneAction and check if sceneAction pointer is returned
 * @post_condition stop the simulator
 * @expected addNewSceneAction will return sceneAction pointer and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneTest_btc, AddNewSceneActionWithKeyValue_SRC_RSV_P)
{
    try
    {
        m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside addNewSceneAction: " + std::string(e.what()));
        return;
    }

    if(m_pSceneAction1 == NULL_PTR)
    {
        SET_FAILURE("scene action pointer is not found");
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
 * @objective Test addNewSceneAction function negatively with null resource
 * @target SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. Key = "power"
 *                     3. value = "on"
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 * @procedure         1. call addNewSceneAction negatively using null value in place of remote resource object
 *                     2. check if RCSInvalidParameterException has occured
 *                     3. check if sceneAction pointer is not returned
 * @post_condition stop the simulator
 * @expected addNewSceneAction will not return sceneAction pointer and RCSInvalidParameterException should occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneTest_btc, AddNewSceneActionWithKeyValueNullResource_NV_ETC_N)
{
    try
    {
        m_pSceneAction1 = m_pScene1->addNewSceneAction(NULL,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));
    }
    catch (const RCSInvalidParameterException& e)
    {
        IOTIVITYTEST_LOG(INFO,"Expected exception occured");
        m_isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexpected Exception occurred inside addNewSceneAction: " + std::string(e.what()));
        return;
    }

    if(m_isException == false)
    {
        SET_FAILURE("Expected exception did not occur");
    }

    if(m_pSceneAction1 != NULL_PTR)
    {
        SET_FAILURE("scene action pointer should not be found");
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
 * @objective Test addNewSceneAction function positively with empty key
 * @target SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. Key = ""
 *                     3. value = "on"
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 * @procedure         1. call addNewSceneAction using empty string as key
 *                     2. check if sceneAction pointer is returned
 * @post_condition stop the simulator
 * @expected addNewSceneAction will return sceneAction pointer and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneTest_btc, AddNewSceneActionWithKeyValueEmptyKey_SRC_ESV_P)
{
    try
    {
        m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_EMPTY,RCSResourceAttributes::Value(VALUE_ON));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside addNewSceneAction: " + std::string(e.what()));
        return;
    }

    if(m_pSceneAction1 == NULL_PTR)
    {
        SET_FAILURE("scene action pointer is not found");
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
 * @objective Test addNewSceneAction function positively with empty value
 * @target SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. Key = "power"
 *                     3. value = ""
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 * @procedure         1. call addNewSceneAction with empty string as value
 *                     2. check if sceneAction pointer is returned
 * @post_condition stop the simulator
 * @expected addNewSceneAction will return sceneAction pointer and exceptions should not occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneTest_btc, AddNewSceneActionWithKeyValueEmptyValue_SRC_ESV_P)
{
    try
    {
        m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_EMPTY));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside addNewSceneAction: " + std::string(e.what()));
        return;
    }

    if(m_pSceneAction1 == NULL_PTR)
    {
        SET_FAILURE("scene action pointer is not found");
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
 * @objective Test getSceneAction function positively
 * @target SceneAction::Ptr getSceneAction(const RCSRemoteResourceObject::Ptr&) const;
 * @test_data RCSRemoteResourceObject::Ptr A pointer of discovered resource
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 *                     6. call addNewSceneAction
 * @procedure call getSceneAction positively and check if sceneAction pointer is returned
 * @post_condition stop the simulator
 * @expected getSceneAction will return sceneAction pointer and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneTest_btc, GetSceneAction_SRC_RSV_P)
{
    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));

    if(m_pSceneAction1 == NULL_PTR)
    {
        SET_FAILURE("could not add scene action");
        return;
    }

    try
    {
        m_pSceneAction2 = m_pScene1->getSceneAction(m_pResource);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getSceneAction: " + std::string(e.what()));
        return;
    }

    if(m_pSceneAction2 == NULL_PTR)
    {
        SET_FAILURE("scene action pointer is not found");
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
 * @objective Test getSceneAction function negatively using NULL resource
 * @target SceneAction::Ptr getSceneAction(const RCSRemoteResourceObject::Ptr&) const;
 * @test_data RCSRemoteResourceObject::Ptr NULL
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 *                     6. call addNewSceneAction
 * @procedure         1. call getSceneAction negatively with null RCSRemoteResourceObject
 *                     2. check if RCSInvalidParameterException has occured
 *                     3. check if sceneAction pointer is not returned
 * @post_condition stop the simulator
 * @expected getSceneAction will not return sceneAction pointer and RCSInvalidParameterException should occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneTest_btc, GetSceneAction_NV_ETC_N)
{
    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));

    if(m_pSceneAction1 == NULL_PTR)
    {
        SET_FAILURE("could not add scene action");
        return;
    }

    try
    {
        m_pSceneAction2 = m_pScene1->getSceneAction(NULL);
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

    if(m_pSceneAction2 != NULL_PTR)
    {
        SET_FAILURE("scene action pointer should not be found");
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
 * @objective Test getSceneActions function positively
 * @target std::vector<SceneAction::Ptr> getSceneActions() const ;
 * @test_data none
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 *                     6. call addNewSceneAction
 * @procedure call getSceneActions and check if list of sceneActions is returned
 * @post_condition stop the simulator
 * @expected getSceneActions will return list of sceneActions and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneTest_btc, GetSceneActions_SRC_P)
{
    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));

    if(m_pSceneAction1 == NULL_PTR)
    {
        SET_FAILURE("could not add scene action");
        return;
    }

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

    if(sceneActions.size() == INT_ZERO)
    {
        SET_FAILURE("no sceneActions found");
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
 * @objective Test getName function positively
 * @target std::string getName() const;
 * @test_data none
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 * @procedure call getName and check if returned name is not empty
 * @post_condition stop the simulator
 * @expected getName will return scene name and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneTest_btc, GetName_SRC_P)
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

    if(receivedName == EMPTY_STRING)
    {
        SET_FAILURE("getName did not return scene name");
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
 * @objective Test execute function positively
 * @target void execute(ExecuteCallback cb);
 * @test_data ExecuteCallback A callback to execute Scene
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 *                     6. call addNewSceneAction
 * @procedure call execute
 * @post_condition stop the simulator
 * @expected execute will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneTest_btc, ExecuteAction_SRC_P)
{
    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));

    try
    {
        m_pScene1->execute(&SMHelper::onExecute);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside execute: " + std::string(e.what()));
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
 *                    std::string key, RCSResourceAttributes::Value value);
 * @objective Test execute function negatively with NULL callback
 * @target void execute(ExecuteCallback cb);
 * @test_data ExecuteCallback A callback to execute Scene
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 *                     6. call addNewSceneAction and get scene action pointer
 * @procedure         1. call execute with null callback
 *                  2. check if RCSInvalidParameterException has occured
 * @post_condition stop the simulator
 * @expected execute will not succeed and RCSInvalidParameterException should occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneTest_btc, ExecuteAction_NV_ETC_N)
{
    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));

    if(m_pSceneAction1 == NULL_PTR)
    {
        SET_FAILURE("could not add scene action");
        return;
    }

    try
    {
        m_pScene1->execute(NULL);
    }

    catch (const RCSInvalidParameterException& e)
    {
        IOTIVITYTEST_LOG(INFO,"Expected exception occured");
        m_isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexpected Exception occurred inside execute: " + std::string(e.what()));
        return;
    }

    if(m_isException == false)
    {
        SET_FAILURE("Expected exception did not occur");
    }
}
#endif
