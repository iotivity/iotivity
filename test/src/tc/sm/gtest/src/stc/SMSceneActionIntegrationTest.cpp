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

class SMSceneActionIntegrationTest_stc: public ::testing::Test {
public:
    SMHelper* m_pSMHelper;
    SceneList* m_pSceneList;
    std::shared_ptr<SceneCollection> m_pSceneCollection;
    std::shared_ptr<Scene> m_pScene1;
    std::shared_ptr<Scene> m_pScene2;
    std::shared_ptr<SceneAction> m_pSceneAction1;
    std::shared_ptr<SceneAction> m_pSceneAction2;
    std::shared_ptr<RCSRemoteResourceObject> m_pResource;

    SMSceneActionIntegrationTest_stc() {
        m_pSMHelper = nullptr;
        m_pSceneList = nullptr;
        m_pSceneCollection = nullptr;
        m_pScene1 = nullptr;
        m_pScene2 = nullptr;
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
        ASSERT_NE(m_pScene1,NULL_PTR) << "first scene pointer is not found";

        m_pScene2 = m_pSceneCollection->addNewScene(SCENE_NAME_2);
        ASSERT_NE(m_pScene2,NULL_PTR) << "second scene pointer is not found";

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
 * @since 2016-02-29
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @see SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @objective Test resetExecutionParameter and getExecutionParameter functions positively
 * @target void resetExecutionParameter(const std::string& key, RCSResourceAttributes::Value value);
 * @target const RCSResourceAttributes getExecutionParameter();
 * @test_data         1. key = "power"
 *                     2. value = "off"
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. get SceneList instance
 *                     3. get SceneCollection pointer by adding SceneCollection
 *                     4. get scene pointer by adding scene
 *                     5. add scene action and check if key value matched
 * @procedure         1. call resetExecutionParameter
 *                     2. call getExecutionParameter and check if set value is received
 * @post_condition stop the simulator
 * @expected set value will be received and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneActionIntegrationTest_stc, ResetWithKeyValueAndGetExecutionParameter_GSRV_P)
{
    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));

    ASSERT_NE(m_pSceneAction1,NULL_PTR) << "added scene action pointer is not found";

    for(const auto &attribute : m_pSceneAction1->getExecutionParameter())
    {
        if(attribute.key() != KEY_POWER || attribute.value() != VALUE_ON)
        {
            SET_FAILURE("added scene action's key and value does not match");
            return;
        }
    }

    try
    {
        m_pSceneAction1->resetExecutionParameter(KEY_POWER,RCSResourceAttributes::Value(VALUE_OFF));

        for(const auto &attribute : m_pSceneAction1->getExecutionParameter())
        {
            if(attribute.key() != KEY_POWER || attribute.value() != VALUE_OFF)
            {
                SET_FAILURE("received key or value is not matching with reset key or value");
            }
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred: " + std::string(e.what()));
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
 *                   RCSResourceAttributes attr);
 * @objective Test resetExecutionParameter and getExecutionParameter functions positively
 * @target void resetExecutionParameter(const RCSResourceAttributes& attr);
 * @target const RCSResourceAttributes getExecutionParameter();
 * @test_data RCSResourceAttributes Attributes to set
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. get SceneList instance
 *                     3. get SceneCollection pointer by adding SceneCollection
 *                     4. get scene pointer by adding scene
 *                     5. add scene action and check if key value matched
 * @procedure         1. call resetExecutionParameter
 *                     2. call getExecutionParameter and check if set value is received
 * @post_condition stop the simulator
 * @expected set value will be received and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneActionIntegrationTest_stc, ResetWithAttributeAndGetExecutionParameter_GSRV_P)
{
    const RCSResourceAttributes attribute1;
    attribute1[KEY_POWER] = VALUE_ON;

    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,attribute1);

    ASSERT_NE(m_pSceneAction1,NULL_PTR) << "could not add scene action";

    for(const auto &attribute : m_pSceneAction1->getExecutionParameter())
    {
        if(attribute.key() != KEY_POWER || attribute.value() != VALUE_ON)
        {
            SET_FAILURE("added scene action's key and value does not match");
            return;
        }
    }

    const RCSResourceAttributes attribute2;
    attribute2[KEY_POWER] = VALUE_OFF;
    try
    {
        m_pSceneAction1->resetExecutionParameter(attribute2);

        for(const auto &attribute : m_pSceneAction1->getExecutionParameter())
        {
            if(attribute.key() != KEY_POWER || attribute.value() != VALUE_OFF)
            {
                SET_FAILURE("received key or value is not matching with reset key or value");
            }
        }
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
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static SceneList* getInstance();
 * @see SceneCollection::Ptr addNewSceneCollection();
 * @see Scene::Ptr addNewScene(const std::string& sceneName);
 * @objective Test getRemoteResourceObject function positively for checking if added action's resource
 *             and getRemoteResourceObject's returned resource is same
 * @target SceneAction::Ptr addNewSceneAction(const RCSRemoteResourceObject::Ptr& pRCSRemoteResourceObject,
 *                   std::string key, RCSResourceAttributes::Value value);
 * @target RCSRemoteResourceObject::Ptr getRemoteResourceObject() const;
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. key = "power"
 *                     3. value = "on"
 * @pre_condition     1. create light type resource using LIGHT_SERVER simulator
 *                     2. get SceneList instance
 *                     3. get SceneCollection pointer by adding SceneCollection
 *                     4. get scene pointer by adding scene
 *                     5. add scene action
 * @procedure call getRemoteResourceObject and check if returned resource is same as the resource
 *             used while creating scene action
 * @post_condition stop the simulator
 * @expected getRemoteResourceObject will return the same resource and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMSceneActionIntegrationTest_stc, AddActionAndGetRemoteResourceObject_GSRV_P)
{
    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource,KEY_POWER,RCSResourceAttributes::Value(VALUE_ON));

    ASSERT_NE(m_pSceneAction1,NULL_PTR) << "added scene action pointer is not found";

    std::shared_ptr<RCSRemoteResourceObject> m_pReceivedResource = nullptr;

    try
    {
        m_pReceivedResource = m_pSceneAction1->getRemoteResourceObject();
        ASSERT_NE(m_pReceivedResource,NULL_PTR) << "remote resource object is not found";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getRemoteResourceObject: " + std::string(e.what()));
        return;
    }

    if(m_pReceivedResource != m_pResource)
    {
        SET_FAILURE("received resource does not match with the set resource");
    }
}
#endif
