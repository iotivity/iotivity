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

class SMRemoteSceneActionTest_btc: public ::testing::Test {
public:
    SMRemoteHelper* m_pSMRemoteHelper;
    std::shared_ptr<RemoteSceneCollection> m_pRemoteSceneCollection;
    std::shared_ptr<RCSRemoteResourceObject> m_pRemoteResource;
    std::shared_ptr<RemoteSceneList> m_pRemoteSceneList;
    RemoteSceneAction::Ptr m_pRemoteSceneAction;
    RemoteSceneAction::Ptr m_pReceivedRemoteSceneAction;
    RemoteScene::Ptr m_pRemoteScene;
    bool m_isException;

    SMRemoteSceneActionTest_btc() {
        m_pSMRemoteHelper = nullptr;
        m_pRemoteResource = nullptr;
        m_pRemoteSceneCollection = nullptr;
        m_pRemoteSceneList = nullptr;
        m_pRemoteSceneAction = nullptr;
        m_pReceivedRemoteSceneAction = nullptr;
        m_pRemoteScene = nullptr;
        m_isException = false;
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

        ASSERT_NE(NULL_PTR,m_pSMRemoteHelper->g_pFoundSceneList) << "Local scene List resource was not found in discovery";

        m_pRemoteSceneList = m_pSMRemoteHelper->createRemoteSceneListInstance();
        ASSERT_NE(m_pRemoteSceneList,NULL_PTR) << "did not find remote scene list instance";

        m_pRemoteSceneList->addNewSceneCollection(
                std::bind(
                        &SMRemoteSceneActionTest_btc::onRemoteSceneCollectionCreated,
                        this, placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

        ASSERT_NE(m_pRemoteSceneCollection,NULL_PTR) << "did not find remote scene collection instance";

        m_pRemoteScene = nullptr;
        m_pRemoteSceneCollection->addNewScene(SCENE_NAME_1,
                std::bind(&SMRemoteSceneActionTest_btc::onRemoteSceneCreated, this,
                        placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        ASSERT_NE(m_pRemoteScene,NULL_PTR) << "Remote Scene is not found";

        ASSERT_NE(m_pSMRemoteHelper->g_vecFoundResourceList.size(),INT_ZERO) << "light resource not found";
        m_pRemoteResource = m_pSMRemoteHelper->g_vecFoundResourceList.at(
                            INT_ZERO);

        m_pRemoteSceneAction = nullptr;
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource, KEY_POWER,
                RCSResourceAttributes::Value(VALUE_OFF),
                std::bind(&SMRemoteSceneActionTest_btc::onRemoteSceneActionCreated,
                        this, placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

        ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "Did not find remote scene action";
    }

    virtual void TearDown() {
        IOTIVITYTEST_LOG(INFO, "TearDown IN");

        CommonUtil::killApp(LIGHT_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);

        CommonUtil::killApp(SCENE_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);

        CommonUtil::runCommonTCTearDownPart();
    }

public:
    void onRemoteSceneCollectionCreated(
            RemoteSceneCollection::Ptr remoteSceneCol, int eCode) {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__, eCode);

        m_pRemoteSceneCollection = remoteSceneCol;

        if (eCode == SCENE_RESULT_OK) {
            m_pRemoteSceneCollection = remoteSceneCol;
        } else {
            IOTIVITYTEST_LOG(INFO, "Create Remote scene collection failed.");
        }
    }

    void onRemoteSceneCreated(RemoteScene::Ptr remoteScene, int eCode) {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__, eCode);

        if (eCode == SCENE_RESULT_OK) {
            m_pRemoteScene = remoteScene;
        } else {
            IOTIVITYTEST_LOG(INFO, "Create Remote scene failed");
        }
    }

    void onRemoteSceneActionCreated(RemoteSceneAction::Ptr remoteSceneAction,
            int eCode) {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__, eCode);

        if (eCode == SCENE_RESULT_OK) {
            m_pRemoteSceneAction = remoteSceneAction;
        } else {
            IOTIVITYTEST_LOG(INFO, "Create Remote scene action failed");
        }
    }

    void onActionUpdated(int eCode) {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d", __func__, eCode);
    }
};

/**
 * @since 2016-03-12
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @objective Test resetExecutionParameter function on positive basic way with attribute
 * @target void resetExecutionParameter(
 *                   const RCSResourceAttributes &attr, ResetExecutionParameterCallback cb)
 * @test_data         1. RCSResourceAttributes Attributes to set
 *                       2. ResetExecutionParameterCallback A callback to receive the response
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. get remote SceneList instance
 *                     3. get remote SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 *                     5. Add scene action
 * @procedure call resetExecutionParameter positively
 * @post_condition stop the simulators
 * @expected resetExecutionParameter will succeed and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneActionTest_btc, RemoteResetExecutionParameterWithAttribute_RSV_P)
{
    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    try
    {
        m_pRemoteSceneAction->resetExecutionParameter(attribute,std::bind(
                        &SMRemoteSceneActionTest_btc::onActionUpdated, this, placeholders::_1));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside resetExecutionParameter: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-12
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @objective Test resetExecutionParameter function on positive basic way with key and value
 * @target void resetExecutionParameter(const std::string &key,
 *                     const RCSResourceAttributes::Value &value, ResetExecutionParameterCallback cb);
 * @test_data         1. key = "power"
 *                       2. value = "on"
 *                       3. ResetExecutionParameterCallback A callback to receive the response
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. get remote SceneList instance
 *                     3. get remote SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 *                     5. Add scene action
 * @procedure call resetExecutionParameter positively
 * @post_condition stop the simulators
 * @expected resetExecutionParameter will succeed and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneActionTest_btc, RemoteResetExecutionParameterWithKeyValue_RSV_P)
{
    try
    {
        m_pRemoteSceneAction->resetExecutionParameter(KEY_POWER, RCSResourceAttributes::Value(VALUE_ON),std::bind(
                        &SMRemoteSceneActionTest_btc::onActionUpdated, this, placeholders::_1));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside resetExecutionParameter: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-12
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @objective Test resetExecutionParameter function on positive way using empty key
 * @target void resetExecutionParameter(const std::string &key,
 *                     const RCSResourceAttributes::Value &value, ResetExecutionParameterCallback cb);
 * @test_data         1. key = ""
 *                       2. value = "on"
 *                       3. ResetExecutionParameterCallback A callback to receive the response
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. get remote SceneList instance
 *                     3. get remote SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 *                     5. Add scene action
 * @procedure call resetExecutionParameter using empty string as key
 * @post_condition stop the simulators
 * @expected resetExecutionParameter will succeed and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneActionTest_btc, RemoteResetExecutionParameterWithKeyValueEmptyKey_ESV_P)
{
    try
    {
        m_pRemoteSceneAction->resetExecutionParameter(KEY_EMPTY, RCSResourceAttributes::Value(VALUE_ON),std::bind(
                        &SMRemoteSceneActionTest_btc::onActionUpdated, this, placeholders::_1));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside resetExecutionParameter: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-12
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @objective Test resetExecutionParameter function on positive way using empty value
 * @target void resetExecutionParameter(const std::string &key,
 *                     const RCSResourceAttributes::Value &value, ResetExecutionParameterCallback cb);
 * @test_data         1. key = "power"
 *                       2. value = ""
 *                       3. ResetExecutionParameterCallback A callback to receive the response
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. get remote SceneList instance
 *                     3. get remote SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 *                     5. Add scene action
 * @procedure call resetExecutionParameter with empty value
 * @post_condition stop the simulators
 * @expected resetExecutionParameter should succeed and should not occur any exception
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneActionTest_btc, RemoteResetExecutionParameterWithKeyValueEmptyValue_ESV_P)
{
    try
    {
        m_pRemoteSceneAction->resetExecutionParameter(KEY_POWER,RCSResourceAttributes::Value(VALUE_EMPTY),std::bind(
                        &SMRemoteSceneActionTest_btc::onActionUpdated, this, placeholders::_1));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside resetExecutionParameter: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-12
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @objective Test resetExecutionParameter function on negative way using null callback
 * @target void resetExecutionParameter(const std::string &key,
 *                     const RCSResourceAttributes::Value &value, ResetExecutionParameterCallback cb);
 * @test_data         1. key = "power"
 *                       2. value = "on"
 *                       3. ResetExecutionParameterCallback Null
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. get remote SceneList instance
 *                     3. get remote SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 *                     5. Add scene action
 * @procedure call resetExecutionParameter using null callback and check if
 *             RCSInvalidParameterException has occured
 * @post_condition stop the simulators
 * @expected RCSInvalidParameterException exception should occur due to Invalid parameter
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneActionTest_btc, RemoteResetExecutionParameterWithKeyValueCallback_NV_N)
{
    try
    {
        m_pRemoteSceneAction->resetExecutionParameter(KEY_POWER, RCSResourceAttributes::Value(VALUE_ON),NULL);
    }
    catch(const RCSInvalidParameterException& e)
    {
        m_isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexpected Exception occurred inside resetExecutionParameter: " + std::string(e.what()));
        return;
    }

    if(!m_isException)
    {
        SET_FAILURE("RCSInvalidParameterException should be occurred due to Invalid Parameter");
    }
}
#endif

/**
 * @since 2016-03-12
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @see void resetExecutionParameter(const std::string &key,
 *                     const RCSResourceAttributes::Value &value, ResetExecutionParameterCallback cb);
 * @objective Test getExecutionParameter function on positive basic way
 * @target const RCSResourceAttributes getExecutionParameter();
 * @test_data none
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. get remote SceneList instance
 *                     3. get remote SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 *                     5. Add scene action
 *                     6. call resetExecutionParameter
 * @procedure call getExecutionParameter and check if execution parameter is returned
 * @post_condition stop the simulators
 * @expected getExecutionParameter will return execution parameter and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneActionTest_btc, RemoteGetExecutionParameter_SRC_P)
{
    std::shared_ptr<RemoteSceneAction> m_pReceivedRemoteSceneAction = nullptr;

    m_pRemoteSceneAction->resetExecutionParameter(KEY_POWER, RCSResourceAttributes::Value(VALUE_ON),std::bind(
                    &SMRemoteSceneActionTest_btc::onActionUpdated, this, placeholders::_1));
    CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);

    try
    {
        for(const auto &attribute : m_pRemoteSceneAction->getExecutionParameter())
        {
            if(attribute.key() == EMPTY_STRING || attribute.value() == EMPTY_STRING)
            {
                SET_FAILURE("Did not receive key or value");
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
 * @since 2016-03-12
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @objective Test getRemoteResourceObject function on positive basic way
 * @target RCSRemoteResourceObject::Ptr getRemoteResourceObject() const;
 * @test_data none
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. get remote SceneList instance
 *                     3. get remote SceneCollection pointer by adding SceneCollection
 *                     4. Get scene pointer by adding scene
 *                     5. Add scene action
 * @procedure call getRemoteResourceObject and check if resource object is returned
 * @post_condition stop the simulators
 * @expected getRemoteResourceObject will return resource object and no exceptions should occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneActionTest_btc, RemoteGetRemoteResourceObject_SRC_P)
{
    std::shared_ptr<RCSRemoteResourceObject> m_pReceivedRemoteResource = nullptr;

    try
    {
        m_pReceivedRemoteResource = m_pRemoteSceneAction->getRemoteResourceObject();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside resetExecutionParameter: " + std::string(e.what()));
        return;
    }

    if(m_pReceivedRemoteResource == NULL_PTR)
    {
        SET_FAILURE("did not receive resource pointer");
    }
}
#endif
