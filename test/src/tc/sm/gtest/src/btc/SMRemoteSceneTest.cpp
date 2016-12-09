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

class SMRemoteSceneTest_btc: public ::testing::Test {
public:
    SMRemoteHelper* m_pSMRemoteHelper;
    std::shared_ptr<RemoteSceneList> m_pRemoteSceneList;
    std::shared_ptr<RemoteSceneCollection> m_pRemoteSceneCollection;
    std::shared_ptr<RCSRemoteResourceObject> m_pRemoteResource;
    RemoteSceneAction::Ptr m_pRemoteSceneAction;
    RemoteSceneAction::Ptr m_pReceivedRemoteSceneAction;
    RemoteScene::Ptr m_pRemoteScene;
    bool m_isException;

    SMRemoteSceneTest_btc() {
        m_pSMRemoteHelper = nullptr;
        m_pRemoteSceneList = nullptr;
        m_pRemoteSceneCollection = nullptr;
        m_pRemoteResource = nullptr;
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
                std::bind(&SMRemoteSceneTest_btc::onRemoteSceneCollectionCreated,
                        this, placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

        ASSERT_NE(m_pRemoteSceneCollection,NULL_PTR) << "did not find remote scene collection instance";

        m_pRemoteSceneCollection->addNewScene(SCENE_NAME_1,
                std::bind(&SMRemoteSceneTest_btc::onRemoteSceneCreated, this,
                        placeholders::_1, placeholders::_2));
        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        ASSERT_NE(m_pRemoteScene,NULL_PTR) << "Remote Scene is not found";

        ASSERT_NE(m_pSMRemoteHelper->g_vecFoundResourceList.size(),INT_ZERO) << "light resource not found";
        m_pRemoteResource = m_pSMRemoteHelper->g_vecFoundResourceList.at(
                    INT_ZERO);
    }

    virtual void TearDown() {
        IOTIVITYTEST_LOG(INFO, "TearDown IN");

        CommonUtil::runCommonTCTearDownPart();

        CommonUtil::killApp(LIGHT_SERVER);
        CommonUtil::waitInSecond(MAX_SLEEP_TIME);
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

    void onRemoteSceneExecuted(const std::string &sceneName, int eCode) {
        IOTIVITYTEST_LOG(INFO, "%s is called with error code: %d, scene name: %s", __func__, eCode,sceneName.c_str());
    }
};

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction function on positive basic way with key and value
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @test_data none    1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. key = "power"
 *                     3. value = "value"
 *                     4. AddNewSceneActionCallback A callback to receive created RemoteSceneAction instance
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 * @post_condition stop the simulators
 * @expected addNewSceneAction will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteAddNewSceneActionWithKeyValue_SRC_RSV_P)
{
    try
    {
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource, KEY_POWER, RCSResourceAttributes::Value(VALUE_OFF),
                std::bind(&SMRemoteSceneTest_btc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside addNewSceneAction: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction function on negative way using null resource
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @test_data none    1. RCSRemoteResourceObject::Ptr NULL
 *                     2. key = "power"
 *                     3. value = "off"
 *                     4. AddNewSceneActionCallback A callback to receive created RemoteSceneAction instance
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure call addNewSceneAction using null resource pointer
 *             and check if RCSInvalidParameterException has occured
 * @post_condition stop the simulators
 * @expected RCSInvalidParameterException should occur due to Invalid parameter
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteAddNewSceneActionWithKeyValueResource_NV_ETC_N)
{
    try
    {
        m_pRemoteScene->addNewSceneAction(NULL, KEY_POWER, RCSResourceAttributes::Value(VALUE_OFF),
                std::bind(&SMRemoteSceneTest_btc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    }
    catch(const RCSInvalidParameterException& e)
    {
        m_isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexcpected Exception occurred inside addNewSceneAction: " + std::string(e.what()));
        return;
    }

    if(!m_isException)
    {
        SET_FAILURE("RCSInvalidParameterException should be occurred due to Null Resource attribute");
    }
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction function on positive way using empty string as key
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @test_data none    1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. key = ""
 *                     3. value = "off"
 *                     4. AddNewSceneActionCallback A callback to receive created RemoteSceneAction instance
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure call addNewSceneAction with empty key
 * @post_condition stop the simulators
 * @expected addNewSceneAction will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteAddNewSceneActionWithKeyValueEmptyKey_ESV_P)
{
    try
    {
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource, EMPTY_STRING, RCSResourceAttributes::Value(VALUE_OFF),
                std::bind(&SMRemoteSceneTest_btc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside addNewSceneAction: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction function on positive way using empty value
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @test_data         1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. key = "power"
 *                     3. value = ""
 *                     4. AddNewSceneActionCallback A callback to receive created RemoteSceneAction instance
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure call addNewSceneAction with empty value
 * @post_condition stop the simulators
 * @expected addNewSceneAction should succeed and should not occur any exception
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteAddNewSceneActionWithKeyValueEmptyValue_ESV_P)
{
    try
    {
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource, KEY_POWER, VALUE_EMPTY,
                std::bind(&SMRemoteSceneTest_btc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside addNewSceneAction: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction function on negative way using null callback
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                                    const std::string &key,
 *                                  const RCSResourceAttributes::Value &value,
 *                                  AddNewSceneActionCallback cb)
 * @test_data none    1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. key = "power"
 *                     3. value = "off"
 *                     4. AddNewSceneActionCallback NULL
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure call addNewSceneAction with null callback and check if RCSInvalidParameterException
 *             has occured
 * @post_condition stop the simulators
 * @expected RCSInvalidParameterException should occur due to Invalid parameter
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteAddNewSceneActionWithKeyValueCallback_NV_ETC_N)
{
    try
    {
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource, KEY_POWER, RCSResourceAttributes::Value(VALUE_OFF),NULL);
    }
    catch(const RCSInvalidParameterException& e)
    {
        m_isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexpected Exception occurred inside addNewSceneAction: " + std::string(e.what()));
        return;
    }

    if(!m_isException)
    {
        SET_FAILURE("RCSInvalidParameterException should be occurred due to Null Callback");
    }
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction function on positive way with attribute
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                   const RCSResourceAttributes &attrs, AddNewSceneActionCallback cb)
 * @test_data none    1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. RCSResourceAttributes Attributes to set when the Scene executed
 *                     3. AddNewSceneActionCallback A callback to receive created RemoteSceneAction instance
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 * @post_condition stop the simulators
 * @expected addNewSceneAction will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteAddNewSceneActionWithAttribute_SRC_RSV_P)
{
    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    try
    {
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource,attribute,
                std::bind(&SMRemoteSceneTest_btc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside addNewSceneAction: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction function on negative way using null resource
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                   const RCSResourceAttributes &attrs, AddNewSceneActionCallback cb)
 * @test_data none    1. RCSRemoteResourceObject::Ptr Null
 *                     2. RCSResourceAttributes Attributes to set when the Scene executed
 *                     3. AddNewSceneActionCallback A callback to receive created RemoteSceneAction instance
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure call addNewSceneAction with null resource and check if RCSInvalidParameterException
 *                 has occured
 * @post_condition stop the simulators
 * @expected RCSInvalidParameterException exception should occur due to Invalid parameter
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteAddNewSceneActionWithAttributeResource_NV_ETC_N)
{
    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    try
    {
        m_pRemoteScene->addNewSceneAction(NULL,attribute,
                std::bind(&SMRemoteSceneTest_btc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    }
    catch(const RCSInvalidParameterException& e)
    {
        m_isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexpected Exception occurred : " + std::string(e.what()));
        return;
    }

    if(!m_isException)
    {
        SET_FAILURE("RCSInvalidParameterException should be occurred due to Null Parameter");
    }
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test addNewSceneAction function on negative way using null callback
 * @target void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                   const RCSResourceAttributes &attrs, AddNewSceneActionCallback cb)
 * @test_data none    1. RCSRemoteResourceObject::Ptr A pointer of discovered resource
 *                     2. RCSResourceAttributes Attributes to set when the Scene executed
 *                     3. AddNewSceneActionCallback Null
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure call addNewSceneAction with null callback and check if
 *             RCSInvalidParameterException has occured
 * @post_condition stop the simulators
 * @expected RCSInvalidParameterException should occur due to Invalid parameter
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteAddNewSceneActionWithAttributeCallback_NV_N)
{
    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    try
    {
        m_pRemoteScene->addNewSceneAction(m_pRemoteResource,attribute,NULL);
    }
    catch(const RCSInvalidParameterException& e)
    {
        m_isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexpected Exception occurred inside addNewSceneAction: " + std::string(e.what()));
        return;
    }

    if(!m_isException)
    {
        SET_FAILURE("RCSInvalidParameterException exception should be occurred due to Null Parameter");
    }
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                   const RCSResourceAttributes &attrs, AddNewSceneActionCallback cb)
 * @objective Test getSceneAction function on positive way
 * @target RemoteSceneAction::Ptr getRemoteSceneAction(
 *                   const RCSRemoteResourceObject::Ptr targetResource) const
 * @test_data RCSRemoteResourceObject::Ptr A pointer of discovered resource
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 *                     6. call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 * @procedure         1. call getRemoteSceneAction to get RemoteSceneAction instance by
 *                        using a certain discovered resource
 *                     2. check if scene action is returned
 * @post_condition stop the simulators
 * @expected getSceneAction will return sceneAction pointer and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteGetRemoteSceneAction_SRC_RSV_P)
{
    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    m_pRemoteScene->addNewSceneAction(m_pRemoteResource,attribute,
            std::bind(&SMRemoteSceneTest_btc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "remote scene action is not added";

    try
    {
        m_pReceivedRemoteSceneAction = m_pRemoteScene->getRemoteSceneAction(m_pRemoteResource);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getRemoteSceneAction: " + std::string(e.what()));
        return;
    }

    ASSERT_NE(m_pReceivedRemoteSceneAction,NULL_PTR) << "did not return scene action pointer";
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @see void addNewSceneAction(RCSRemoteResourceObject::Ptr targetResource,
 *                   const RCSResourceAttributes &attrs, AddNewSceneActionCallback cb)
 * @objective Test getSceneAction function on negative way using null resource
 * @target RemoteSceneAction::Ptr getRemoteSceneAction(
 *                   const RCSRemoteResourceObject::Ptr targetResource) const
 * @test_data RCSRemoteResourceObject::Ptr null
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 *                     6. call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 * @procedure call getRemoteSceneAction with null resource and check
 *             if RCSInvalidParameterException
 * @post_condition stop the simulators
 * @expected RCSInvalidParameterException should occur due to Invalid parameter
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteGetRemoteSceneAction_NV_ETC_N)
{
    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    m_pRemoteScene->addNewSceneAction(m_pRemoteResource,attribute,
            std::bind(&SMRemoteSceneTest_btc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "remote scene action is not added";

    try
    {
        m_pReceivedRemoteSceneAction = m_pRemoteScene->getRemoteSceneAction(NULL);
    }
    catch(const RCSInvalidParameterException& e)
    {
        m_isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexcpected Exception occurred inside getRemoteSceneAction: " + std::string(e.what()));
        return;
    }

    if(!m_isException)
    {
        SET_FAILURE("RCSInvalidParameterException should be occurred due to Null Parameter");
    }
}
#endif

/**
 * @since 2016-03-09
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
 * @objective Test getSceneActions function on positive basic way
 * @target std::vector< RemoteSceneAction::Ptr > getRemoteSceneActions() const
 * @test_data none
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 *                     6. call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 * @procedure call getRemoteSceneActions to get all RemoteSceneAction instances
 *                        included in the Scene
 * @post_condition stop the simulators
 * @expected getSceneActions will return list of sceneActions and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteGetRemoteSceneActions_SRC_P)
{
    std::vector<RemoteSceneAction::Ptr> remoteSceneActions;

    const RCSResourceAttributes attribute;
    attribute[KEY_POWER] = VALUE_ON;

    m_pRemoteScene->addNewSceneAction(m_pRemoteResource, KEY_POWER, RCSResourceAttributes::Value(VALUE_OFF),
            std::bind(&SMRemoteSceneTest_btc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
    ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "remote scene action is not added";

    try
    {
        remoteSceneActions = m_pRemoteScene->getRemoteSceneActions();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getRemoteSceneActions: " + std::string(e.what()));
        return;
    }

    ASSERT_NE(INT_ZERO, remoteSceneActions.size()) << "did not get list of remote scene actions";
}
#endif

/**
 * @since 2016-03-09
 * @see void Configure(const PlatformConfig& config);
 * @see DiscoveryTask::Ptr discoverResourceByTypes(const RCSAddress& address,
 *                   const std::vector< std::string >& resourceTypes,
 *                   ResourceDiscoveredCallback cb);
 * @see static void createInstance(
 *         RCSRemoteResourceObject::Ptr sceneListResource, CreateInstanceCallback cb);
 * @see void addNewSceneCollection(AddNewSceneCollectionCallback cb);
 * @see void addNewScene(const std::string &name, AddNewSceneCallback cb)
 * @objective Test getName function on positive basic way
 * @target std::string getName() const
 * @test_data none
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 * @procedure call getName and check if name is returned
 * @post_condition stop the simulators
 * @expected getName will return name of scene and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteSceneGetName_SRC_P)
{
    std::string receivedName = "";
    try
    {
        receivedName = m_pRemoteScene->getName();
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside getName: " + std::string(e.what()));
        return;
    }

    if(receivedName == EMPTY_STRING)
    {
        SET_FAILURE("Get name did not receive the scene name");
    }
}
#endif

/**
 * @since 2016-03-09
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
 * @objective Test execute function on positive basic way
 * @target void execute(RemoteSceneExecuteCallback cb)
 * @test_data RemoteSceneExecuteCallback A callback to receive result of Scene execution
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 *                     6. call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 * @procedure call execute to to execute the Scene on remote side
 * @post_condition stop the simulators
 * @expected execute will succeed and no exceptions occur
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteExecuteAction_RSV_P)
{
    m_pRemoteScene->addNewSceneAction(m_pRemoteResource, KEY_POWER, RCSResourceAttributes::Value(VALUE_OFF),
            std::bind(&SMRemoteSceneTest_btc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "remote scene action is not added";

    try
    {
        m_pRemoteScene->execute(std::bind(&SMRemoteSceneTest_btc::onRemoteSceneExecuted, this,placeholders::_1, placeholders::_2));
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred inside execute: " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-03-09
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
 * @objective Test execute function on negative way using null callback
 * @target void execute(RemoteSceneExecuteCallback cb)
 * @test_data RemoteSceneExecuteCallback Null
 * @pre_condition     1. Start SceneList and light type resource using SCENE_SERVER and LIGHT_SERVER simulators
 *                     2. discover resources
 *                     3. get remote SceneList instance
 *                     4. get remote SceneCollection pointer by adding SceneCollection
 *                     5. Get scene pointer by adding scene
 *                     6. call addNewSceneAction to add new SceneAction to
 *                        the Scene on remote side
 * @procedure call execute with null callback and check if
 *             RCSInvalidParameterException has occured
 * @post_condition stop the simulators
 * @expected RCSInvalidParameterException exception should occur due to Invalid parameter
 */
#if defined(__LINUX__)
TEST_F(SMRemoteSceneTest_btc, RemoteExecuteAction_NV_N)
{
    m_pRemoteScene->addNewSceneAction(m_pRemoteResource, KEY_POWER, RCSResourceAttributes::Value(VALUE_OFF),
            std::bind(&SMRemoteSceneTest_btc::onRemoteSceneActionCreated, this,placeholders::_1, placeholders::_2));
    CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
    ASSERT_NE(m_pRemoteSceneAction,NULL_PTR) << "remote scene action is not added";

    try
    {
        m_pRemoteScene->execute(NULL);
    }
    catch(const RCSInvalidParameterException& e)
    {
        m_isException = true;
    }
    catch (exception& e)
    {
        SET_FAILURE("Unexpected Exception occurred inside execute: " + std::string(e.what()));
        return;
    }

    if(!m_isException)
    {
        SET_FAILURE("RCSInvalidParameterException should be occurred due to Null Parameter");
    }
}
#endif
