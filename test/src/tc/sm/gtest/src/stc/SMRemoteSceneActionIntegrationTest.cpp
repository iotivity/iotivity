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

class SMRemoteSceneActionIntregrationTest_stc: public ::testing::Test {
public:
    SMRemoteHelper* m_pSMRemoteHelper;
    std::shared_ptr<RCSRemoteResourceObject> m_pRemoteResource;
    std::shared_ptr<RemoteSceneList> m_pRemoteSceneList;
    std::shared_ptr<RemoteSceneCollection> m_pRemoteSceneCollection;
    RemoteScene::Ptr m_pRemoteScene;
    RemoteSceneAction::Ptr m_pRemoteSceneAction;
    bool m_isCallback;

    SMRemoteSceneActionIntregrationTest_stc() {
        m_pSMRemoteHelper = nullptr;
        m_pRemoteResource = nullptr;
        m_pRemoteSceneCollection = nullptr;
        m_pRemoteSceneList = nullptr;
        m_pRemoteScene = nullptr;
        m_pRemoteSceneAction = nullptr;
        m_isCallback = false;
    }

    virtual void SetUp() {
        IOTIVITYTEST_LOG(INFO, "SetUp IN");

        CommonUtil::runCommonTCSetUpPart();

        CommonUtil::launchApp(LIGHT_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);
        CommonUtil::launchApp(SCENE_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);

        m_pSMRemoteHelper = new SMRemoteHelper();
        m_pSMRemoteHelper->discoverResource();
        m_pSMRemoteHelper->stopDiscovery();

        m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
        ASSERT_NE(m_pRemoteSceneList,NULL_PTR) << "did not find remote scene list instance";

        m_pRemoteSceneList->addNewSceneCollection(
                std::bind(
                        &SMRemoteSceneActionIntregrationTest_stc::onRemoteSceneCollectionCreated,
                        this, placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        ASSERT_NE(m_pRemoteSceneCollection,NULL_PTR)<< "remote scene collection was not created";

        m_pRemoteSceneCollection->addNewScene(SCENE_NAME_1,
                std::bind(
                        &SMRemoteSceneActionIntregrationTest_stc::onRemoteSceneCreated,
                        this, placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        ASSERT_NE(m_pRemoteScene,NULL_PTR)<< "remote scene was not created";

        ASSERT_NE(m_pSMRemoteHelper->g_vecFoundResourceList.size(),INT_ZERO) << "light resource not found";
        m_pRemoteResource = m_pSMRemoteHelper->g_vecFoundResourceList.at(
                                    INT_ZERO);

        m_pRemoteScene->addNewSceneAction(m_pRemoteResource, KEY_POWER,
                RCSResourceAttributes::Value(VALUE_OFF),
                std::bind(
                        &SMRemoteSceneActionIntregrationTest_stc::onRemoteSceneActionCreated,
                        this, placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        ASSERT_NE(m_pRemoteSceneAction,NULL_PTR)<< "remote scene action was not created";

    }

    virtual void TearDown()
    {
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
    void onRemoteSceneCollectionCreated(RemoteSceneCollection::Ptr remoteSceneCol, int eCode)
    {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__, eCode);

        if(eCode == SCENE_RESULT_OK) {
            m_pRemoteSceneCollection = remoteSceneCol;
            m_isCallback = true;
        } else {
            IOTIVITYTEST_LOG(INFO,"Create Remote scene collection failed.");
        }
    }

    void onRemoteSceneCreated(RemoteScene::Ptr remoteScene, int eCode)
    {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__, eCode);

        if(eCode == SCENE_RESULT_OK) {
            m_pRemoteScene = remoteScene;
            m_isCallback = true;
        } else {
            IOTIVITYTEST_LOG(INFO,"Create Remote scene failed");
        }
    }

    void onRemoteSceneActionCreated(RemoteSceneAction::Ptr remoteSceneAction, int eCode)
    {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__, eCode);

        if(eCode == SCENE_RESULT_OK) {
            m_pRemoteSceneAction = remoteSceneAction;
            m_isCallback = true;
        } else {
            IOTIVITYTEST_LOG(INFO,"Create Remote scene action failed");
        }
    }

    void onActionUpdated(int eCode)
    {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__, eCode);
        if(eCode == SCENE_RESULT_OK) {
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
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @objective Test resetExecutionParameter function on positive way to check if callback is called
 * @target void resetExecutionParameter(const std::string &key,
 *                   const RCSResourceAttributes::Value &value, ResetExecutionParameterCallback cb)
 * @test_data         1. key = "power"
 *                       2. value = "on"
 *                       3. ResetExecutionParameterCallback A callback to receive the response
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. get remote SceneList instance
 *                     3. get remote SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 *                     5. Add scene action
 * @procedure         1. call resetExecutionParameter positively
 *                     2. Check if onActionUpdated is invoked with proper error code
 * @post_condition stop the simulators
 * @expected callback will be called with proper error code and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneActionIntregrationTest_stc, RemoteResetExecutionParameterWithKeyValue_CV_P)
{
    try
    {
        m_pRemoteSceneAction->resetExecutionParameter(KEY_POWER, RCSResourceAttributes::Value(VALUE_ON),std::bind(
                        &SMRemoteSceneActionIntregrationTest_stc::onActionUpdated, this, placeholders::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside resetExecutionParameter: " + std::string(e.what()));
        return;
    }

    if(!m_isCallback)
    {
        SET_FAILURE("ResetExecutionParameterCallback is not invoked with proper error code");
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
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @objective Test resetExecutionParameter and getExecutionParameter on positive way to check
 *         if reset value is get
 * @target void resetExecutionParameter(const std::string &key,
 *                   const RCSResourceAttributes::Value &value, ResetExecutionParameterCallback cb)
 * @target RCSResourceAttributes getExecutionParameter() const;
 * @test_data         1. key = "power"
 *                       2. value = "on"
 *                       3. ResetExecutionParameterCallback A callback to receive the response
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. get remote SceneList instance
 *                     3. get remote SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 *                     5. add scene action(power,off) and check if key value matched
 * @procedure         1. call resetExecutionParameter
 *                     2. call getExecutionParameter
 *                     3. Check if received execution parameter matches with reset parameters
 * @post_condition stop the simulators
 * @expected getExecutionParameter will return the setExecutionParameter and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneActionIntregrationTest_stc, RemoteResetWithKeyValueAndGetExecutionParameter_GSRV_P)
{
    for(const auto &attribute : m_pRemoteSceneAction->getExecutionParameter())
    {
        if(attribute.key() != KEY_POWER || attribute.value() != VALUE_OFF)
        {
            SET_FAILURE("added scene action's key and value does not match");
            return;
        }
    }

    try
    {
        m_pRemoteSceneAction->resetExecutionParameter(KEY_POWER, RCSResourceAttributes::Value(VALUE_ON),std::bind(
                        &SMRemoteSceneActionIntregrationTest_stc::onActionUpdated, this, placeholders::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        for(const auto &attribute : m_pRemoteSceneAction->getExecutionParameter())
        {
            if(attribute.key() != KEY_POWER || attribute.value() != VALUE_ON)
            {
                SET_FAILURE("received key or value doesn't match with set key or value");
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
 * @since 2016-03-13
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
 * @objective Test resetExecutionParameter function on positive way to check if callback is called
 * @target void resetExecutionParameter(
 *                   const RCSResourceAttributes &attr, ResetExecutionParameterCallback cb)
 * @test_data         1. RCSResourceAttributes attribute to be set
 *                       2. ResetExecutionParameterCallback A callback to receive the response
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. get remote SceneList instance
 *                     3. get remote SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 *                     5. Add scene action
 * @procedure         1. call resetExecutionParameter positively
 *                     2. Check if onActionUpdated is invoked with proper error code
 * @post_condition stop the simulators
 * @expected callback will be called with proper error code and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneActionIntregrationTest_stc, RemoteResetExecutionParameterWithAttribute_CV_P)
{
    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    try
    {
        m_pRemoteSceneAction->resetExecutionParameter(attribute,std::bind(
                        &SMRemoteSceneActionIntregrationTest_stc::onActionUpdated, this, placeholders::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside resetExecutionParameter: " + std::string(e.what()));
        return;
    }

    if(!m_isCallback)
    {
        SET_FAILURE("ResetExecutionParameterCallback is not invoked with proper error code");
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
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @objective Test resetExecutionParameter and getExecutionParameter functions on positive way to
 *             check if reset parameters are get
 * @target void resetExecutionParameter(
 *                   const RCSResourceAttributes &attr, ResetExecutionParameterCallback cb)
 * @target RCSResourceAttributes getExecutionParameter() const;
 * @test_data         1. RCSResourceAttributes attributes to be set
 *                       2. ResetExecutionParameterCallback A callback to receive the response
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. get remote SceneList instance
 *                     3. get remote SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 *                     5. add scene action(power,off) and check if key value matched
 * @procedure         1. call resetExecutionParameter
 *                     2. call getExecutionParameter
 *                     3. Check if get value matches with set value
 * @post_condition stop the simulators
 * @expected resetExecutionParameter will return the same as the reset valkues no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneActionIntregrationTest_stc, RemoteResetWithAttributeAndGetExecutionParameter_GSRV_P)
{
    for(const auto &attribute : m_pRemoteSceneAction->getExecutionParameter())
    {
        if(attribute.key() != KEY_POWER || attribute.value() != VALUE_OFF)
        {
            SET_FAILURE("added scene action's key and value does not match");
            return;
        }
    }

    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    try
    {
        m_pRemoteSceneAction->resetExecutionParameter(attribute,std::bind(
                        &SMRemoteSceneActionIntregrationTest_stc::onActionUpdated, this, placeholders::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

        for(const auto &attribute : m_pRemoteSceneAction->getExecutionParameter())
        {
            if(attribute.key() != KEY_POWER || attribute.value() != VALUE_ON)
            {
                SET_FAILURE("received key or value doesn't match with set key or value");
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
 * @since 2016-03-13
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                  const std::vector< std::string >& resourceTypes,
 *                  ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test getRemoteResourceObject function positively to check if get RCSRemoteResourceObject
 *                     and set RCSRemoteResourceObject has matched
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @target RCSRemoteResourceObject::Ptr getRemoteResourceObject() const;
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of the discovered resource
 *                     2. key = "power"
 *                     3. value = "off"
 *                     4. AddNewSceneActionCallback callback to be called when remote scene action is created
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. get remote SceneList instance
 *                     3. get remote SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 * @procedure        1. Add scene action
 *                     2. call getRemoteResourceObject positively
 *                     3. Check if resource used while creating remote sceneaction matches with received
 *                     remote resource object
 * @post_condition stop the simulators
 * @expected getRemoteResourceObject will return the same resource object
 *         which was used while creating remote scene action
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneActionIntregrationTest_stc, RemoteAddActionAndGetRemoteResourceObject_GSRV_P)
{
    std::shared_ptr<RCSRemoteResourceObject> m_pReceivedRemoteResource = nullptr;

    try
    {
        m_pReceivedRemoteResource = m_pRemoteSceneAction->getRemoteResourceObject();
        EXPECT_NE(m_pReceivedRemoteResource,NULL_PTR) << "received resource object is null";
        EXPECT_EQ(m_pReceivedRemoteResource,m_pRemoteResource) << "received resource does not match";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred: " + std::string(e.what()));
    }
}
#endif
