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

class SMSceneActionTest_btc: public ::testing::Test {
public:
    SMHelper* m_pSMHelper;
    SceneList* m_pSceneList;
    std::shared_ptr<SceneCollection> m_pSceneCollection;
    std::shared_ptr<Scene> m_pScene1;
    std::shared_ptr<SceneAction> m_pSceneAction1;
    std::shared_ptr<SceneAction> m_pSceneAction2;
    std::shared_ptr<RCSRemoteResourceObject> m_pResource;

    SMSceneActionTest_btc() {
        m_pSMHelper = nullptr;
        m_pSceneList = nullptr;
        m_pSceneCollection = nullptr;
        m_pScene1 = nullptr;
        m_pSceneAction1 = nullptr;
        m_pSceneAction2 = nullptr;
        m_pResource = nullptr;
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

        delete m_pSMRemoteHelper;
        m_pSMRemoteHelper = nullptr;

        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @see SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   RCSResourceAttributes attr);
 * @objective Test resetExecutionParameter function positively with attribute
 * @target void resetExecutionParameter(const RCSResourceAttributes& attr);
 * @test_data RCSResourceAttributes Attributes to set
 * @pre_condition    1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 *                     6. call addNewSceneAction using one attribute
 * @procedure call resetExecutionParameter positively using another attribute
 * @post_condition stop the simulator
 * @expected resetExecutionParameter will succeed and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneActionTest_btc, ResetExecutionParameterWithAttribute_RSV_P)
{
    const RCSResourceAttributes attribute1;
    attribute1[KEY_POWER] = VALUE_ON;

    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,attribute1);
    ASSERT_NE(m_pSceneAction1,NULL_PTR) << "could not add scene action";

    const RCSResourceAttributes attribute2;
    attribute2[KEY_POWER] = VALUE_OFF;

    try
    {
        m_pSceneAction1->resetExecutionParameter(attribute2);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside resetExecutionParameter: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @see SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @objective Test resetExecutionParameter function positively with key and value
 * @target void resetExecutionParameter(const std::string& key, RCSResourceAttributes::Value value);
 * @test_data         1. key = "power"
 *                     2. value = "on"
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 *                     6. call addNewSceneAction with value = "on"
 * @procedure call resetExecutionParameter with value = "off"
 * @post_condition stop the simulator
 * @expected resetExecutionParameter will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneActionTest_btc, ResetExecutionParameterWithKeyValue_SRC_P)
{
    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));

    ASSERT_NE(m_pSceneAction1,NULL_PTR) << "could not add scene action";

    try
    {
        m_pSceneAction1->resetExecutionParameter(KEY_POWER,RCSResourceAttributes::Value(VALUE_OFF));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside resetExecutionParameter: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @see SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @objective Test resetExecutionParameter function positively with empty key
 * @target void resetExecutionParameter(const std::string& key, RCSResourceAttributes::Value value);
 * @test_data         1. key = ""
 *                     2. value = "off"
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 *                     6. call addNewSceneAction
 * @procedure call resetExecutionParameter positively using empty string as key
 * @post_condition stop the simulator
 * @expected resetExecutionParameter will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneActionTest_btc, ResetExecutionParameterWithKeyValueEmptyKey_ESV_P)
{
    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));

    ASSERT_NE(m_pSceneAction1,NULL_PTR) << "could not add scene action";

    try
    {
        m_pSceneAction1->resetExecutionParameter(KEY_EMPTY,RCSResourceAttributes::Value(VALUE_OFF));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside resetExecutionParameter: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @see SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @objective Test resetExecutionParameter function positively with empty value
 * @target void resetExecutionParameter(const std::string& key, RCSResourceAttributes::Value value);
 * @test_data         1. key = "power"
 *                     2. value = ""
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 *                     6. call addNewSceneAction
 * @procedure call resetExecutionParameter positively using empty string as value
 * @post_condition stop the simulator
 * @expected resetExecutionParameter will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneActionTest_btc, ResetExecutionParameterWithKeyValueEmptyValue_ESV_P)
{
    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));

    ASSERT_NE(m_pSceneAction1,NULL_PTR) << "could not add scene action";

    try
    {
        m_pSceneAction1->resetExecutionParameter(KEY_POWER,RCSResourceAttributes::Value(VALUE_EMPTY));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside resetExecutionParameter: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @see SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @see void resetExecutionParameter(const std::string& key, RCSResourceAttributes::Value value);
 * @objective Test getExecutionParameter function positively
 * @target const RCSResourceAttributes getExecutionParameter();
 * @test_data none
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 *                     6. call addNewSceneAction with value = "on"
 *                     7. call resetExecutionParameter with value = "off"
 * @procedure         1. call getExecutionParameter and check if attribute is get
 *                     2. check if attributes key or value is not empty
 * @post_condition stop the simulator
 * @expected getExecutionParameter should get attribute and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneActionTest_btc, GetExecutionParameter_SRC_P)
{
    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));

    ASSERT_NE(m_pSceneAction1,NULL_PTR) << "could not add scene action";

    m_pSceneAction1->resetExecutionParameter(KEY_POWER,RCSResourceAttributes::Value(VALUE_OFF));

    try
    {
        for(const auto &attribute : m_pSceneAction1->getExecutionParameter())
        {
            if(attribute.key() == EMPTY_STRING || attribute.value() == EMPTY_STRING)
            {
                SET_FAILURE("received key or value is empty");
            }
        }

    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getExecutionParameter: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @see SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @objective Test getRemoteResourceObject function positively
 * @target RCSRemoteResourceObject::Ptr getRemoteResourceObject() const;
 * @test_data none
 * @pre_condition    1. create light type resource using LIGHT_SERVER simulator
 *                     2. Discover resource
 *                     3. get SceneList instance
 *                     4. get SceneCollection pointer by adding SceneCollection
 *                     5. get Scene pointer by adding scene
 *                     6. call addNewSceneAction
 * @procedure call getRemoteResourceObject and check if remote resource pointer is returned
 * @post_condition stop the simulator
 * @expected getRemoteResourceObject will return resource and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneActionTest_btc, GetRemoteResourceObject_SRC_P)
{
    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));

    ASSERT_NE(m_pSceneAction1,NULL_PTR) << "could not add scene action";

    std::shared_ptr<RCSRemoteResourceObject> m_pReceivedResource = nullptr;

    try
    {
        m_pReceivedResource = m_pSceneAction1->getRemoteResourceObject();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getRemoteResourceObject: " + std::string(e.what()));
        return;
    }

    if(m_pReceivedResource == NULL_PTR)
    {
        SET_FAILURE("received resource is null");
    }
}
#endif
