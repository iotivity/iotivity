/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
#include "ICHelper.h"
#include "ICResourceHelper.h"

static OCAccountManager::Ptr g_accountMgrControlee = nullptr;

class ICOCResourceTest_stc: public ::testing::Test
{

public:
    OC::OCResource::Ptr m_mqBrokerResource = nullptr;
    OC::OCResource::Ptr m_mqSelectedTopicResource = nullptr;
    OCRepresentation m_rep;
    int index = 0;

protected:
    std::string m_errorMsg;
    ICHelper m_ICHelper;
    string m_hostAddress = COAP_HOST_ADDRESS;
    QueryParamsMap m_queryParams;
    ICResourceHelper m_ICResourceHelper;

    static FILE *client_open(const char * /*path*/, const char *mode)
    {
        return fopen(MQPUBLISHER, mode);
    }

    virtual void SetUp()
    {
        ICResourceHelper::s_isCreateTopicSuccess = false;
        ICResourceHelper::s_isDiscoverTopicSuccess = false;
        ICResourceHelper::s_isPublishTopicSuccess = false;
        ICResourceHelper::s_isrequestTopicPublishSuccess = false;
        ICResourceHelper::s_isSubscribeSuccess = false;
        CommonTestUtil::runCommonTCSetUpPart();
        m_ICHelper.initCloudClient();
        OCPersistentStorage ps
        { client_open, fread, fwrite, fclose, unlink };
        PlatformConfig cfg
        { ServiceType::InProc, ModeType::Both, CT_DEFAULT, // By setting to "0.0.0.0", it binds to all available interfaces
            CT_DEFAULT, // Uses randomly available port
                QualityOfService::LowQos, &ps };
        OCPlatform::Configure(cfg);

        g_accountMgrControlee = OCPlatform::constructAccountManagerObject(m_hostAddress,
                CT_ADAPTER_TCP);
        // MQ broker resource
        string host = "coap+tcp://";
        host += IC_CLOUD_INTERFACE_HOST_ADDRESS;
        m_mqBrokerResource = OCPlatform::constructResourceObject(host, DEFAULT_MQ_BROKER_URI,
                static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4), false,
                { string("oic.wk.ps") },
                { string(DEFAULT_INTERFACE) });
    }

    virtual void TearDown()
    {
    }

};

/**
 * @since 2017-01-26
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)() API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'createMQTopic' API With valid scenario
 * @target createMQTopic(const OCRepresentation& rep,
                                    const std::string& topicUri,
                                    const QueryParamsMap& queryParametersMap,
                                    MQTopicCallback attributeHandler,
                                    QualityOfService qos)
 * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4. MQTopicCallback 5. QualityOfService
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn
 * @procedure Call createMQTopic API with valid parameter
 * @post_condition SignOut
 * @expected createMQTopic API will return OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCResourceTest_stc, CreateMQTopic_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
    ASSERT_EQ(OC_STACK_OK, m_mqBrokerResource->createMQTopic(m_rep, TOPIC_URI,
                    m_queryParams, ICResourceHelper::createTopicCB, QualityOfService::LowQos))<<"Create MQ does not work.";
    ICHelper::waitForServerResponse();
    ASSERT_EQ(true, ICResourceHelper::s_isCreateTopicSuccess)<<"Topic is not created.";
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee));
    SUCCEED();
}
#endif

/**
 * @since 2017-01-26
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)() API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'createMQTopic' API With valid scenario
 * @target createMQTopic(const OCRepresentation& rep,
                                    const std::string& topicUri,
                                    const QueryParamsMap& queryParametersMap,
                                    MQTopicCallback attributeHandler,
                                    QualityOfService qos)
 * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4. MQTopicCallback 5. QualityOfService
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn
 * @procedure Call createMQTopic API with valid parameter
 * @post_condition SignOut
 * @expected createMQTopic API should return OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCResourceTest_stc, CreateMQTopicByType_SRC_P)
{
    m_queryParams["rt"] = LIGHT;

    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
    ASSERT_EQ(OC_STACK_OK, m_mqBrokerResource->createMQTopic(m_rep, TOPIC_URI,
                    m_queryParams, ICResourceHelper::createTopicCB, QualityOfService::LowQos))<<"Create MQ does not work.";
    ICHelper::waitForServerResponse();
    ASSERT_EQ(true, ICResourceHelper::s_isCreateTopicSuccess)<<"Topic is not created.";
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee));
    SUCCEED();
}
#endif

/**
 * @since 2017-01-26
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)() API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'discoveryMQTopics' API With valid scenario
 * @target OCStackResult discoveryMQTopics(const QueryParamsMap& queryParametersMap,
                                        MQTopicCallback attributeHandler,
                                        QualityOfService qos)
 * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4. MQTopicCallback 5. QualityOfService
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn
 *                4. Create Topic
 * @procedure Call discoveryMQTopics API with valid parameter
 * @post_condition SignOut
 * @expected discoveryMQTopics API should return OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCResourceTest_stc, DiscoveryMQTopics_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
    ASSERT_EQ(OC_STACK_OK, m_mqBrokerResource->createMQTopic(m_rep, TOPIC_URI,
                    m_queryParams, ICResourceHelper::createTopicCB, QualityOfService::LowQos))<<"Create MQ does not work.";
    ICHelper::waitForServerResponse();
    ICResourceHelper::s_TopicList.clear();
    EXPECT_EQ(OC_STACK_OK, m_mqBrokerResource->discoveryMQTopics(m_queryParams,
                    ICResourceHelper::discoverTopicCB, QualityOfService::LowQos))<<"discovery MQ does not work.";
    ICHelper::waitForServerResponse();
    ASSERT_EQ(true, ICResourceHelper::s_isDiscoverTopicSuccess)<<"Discovery MQ got fail.";
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee));
    SUCCEED();

}
#endif

/**
 * @since 2017-01-26
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)() API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'discoveryMQTopics' API With valid scenario
 * @target OCStackResult discoveryMQTopics(const QueryParamsMap& queryParametersMap,
                                        MQTopicCallback attributeHandler,
                                        QualityOfService qos)
 * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4. MQTopicCallback 5. QualityOfService
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn
 * @procedure Call discoveryMQTopics API with valid parameter
 * @post_condition SignOut
 * @expected discoveryMQTopics API should return OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCResourceTest_stc, DiscoveryMQTopicsByType_SRC_P)
{
    m_queryParams["rt"] = LIGHT;

    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
    ASSERT_EQ(OC_STACK_OK, m_mqBrokerResource->createMQTopic(m_rep, TOPIC_URI,
                    m_queryParams, ICResourceHelper::createTopicCB, QualityOfService::LowQos))<<"Create MQ does not work.";
    ICHelper::waitForServerResponse();
    ICResourceHelper::s_TopicList.clear();
    EXPECT_EQ(OC_STACK_OK, m_mqBrokerResource->discoveryMQTopics(m_queryParams,
                    ICResourceHelper::discoverTopicCB, QualityOfService::LowQos))<<"discovery MQ does not work.";
    ICHelper::waitForServerResponse();
    ASSERT_EQ(true, ICResourceHelper::s_isDiscoverTopicSuccess)<<"Discovery MQ got fail.";
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee));
    SUCCEED();

}
#endif

/**
 * @since 2017-01-26
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)() API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'publishMQTopic' API With valid scenario
 * @target OCStackResult publishMQTopic(const QueryParamsMap& queryParametersMap,
                                        MQTopicCallback attributeHandler,
                                        QualityOfService qos)
 * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4. MQTopicCallback 5. QualityOfService
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. Call SignUp
 *                3. Call SignIn
 *                4. Call createMQTopic
 *                5. Call discoveryMQTopics
 * @procedure Call publishMQTopic API with valid parameter
 * @post_condition SignOut
 * @expected publishMQTopic API should return OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCResourceTest_stc, PublishMQTopic_SRC_P)
{
    m_rep["message"] = MESSAGE;
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
    ASSERT_EQ(OC_STACK_OK, m_mqBrokerResource->createMQTopic(m_rep, TOPIC_URI,
                    m_queryParams, ICResourceHelper::createTopicCB, QualityOfService::LowQos))<<"Create MQ does not work.";
    ICHelper::waitForServerResponse();
    ICResourceHelper::s_TopicList.clear();
    EXPECT_EQ(OC_STACK_OK, m_mqBrokerResource->discoveryMQTopics(m_queryParams,
                    ICResourceHelper::discoverTopicCB, QualityOfService::LowQos))<<"discovery MQ does not work.";
    ICHelper::waitForServerResponse();

    m_mqSelectedTopicResource = ICResourceHelper::s_TopicList[index];
    EXPECT_EQ(OC_STACK_OK, m_mqSelectedTopicResource->publishMQTopic(m_rep, m_queryParams,
                    ICResourceHelper::publishMessageCB, QualityOfService::LowQos))<<"publish MQ does not work.";
    ICHelper::waitForServerResponse();
    ASSERT_EQ(true, ICResourceHelper::s_isPublishTopicSuccess)<<"publishMQTopic got fail.";
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee));
    SUCCEED();

}
#endif

/**
 * @since 2017-01-26
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)() API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'requestMQPublish' API With valid scenario
 * @target OCStackResult publishMQTopic(const QueryParamsMap& queryParametersMap,
                                        MQTopicCallback attributeHandler,
                                        QualityOfService qos)
 * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4. MQTopicCallback 5. QualityOfService
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn
 *                4. Call createMQTopic
 *                5. Call discoveryMQTopics
 * @procedure Call requestMQPublish API with valid parameter
 * @post_condition SignOut
 * @expected requestMQPublish API should return OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCResourceTest_stc, RequestpublishMQTopic_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
    ASSERT_EQ(OC_STACK_OK, m_mqBrokerResource->createMQTopic(m_rep, TOPIC_URI,
                    m_queryParams, ICResourceHelper::createTopicCB, QualityOfService::LowQos))<<"Create MQ does not work.";
    ICHelper::waitForServerResponse();
    ICResourceHelper::s_TopicList.clear();
    EXPECT_EQ(OC_STACK_OK, m_mqBrokerResource->discoveryMQTopics(m_queryParams,
                    ICResourceHelper::discoverTopicCB, QualityOfService::LowQos))<<"discovery MQ does not work.";
    ICHelper::waitForServerResponse();

    m_mqSelectedTopicResource = ICResourceHelper::s_TopicList[index];
    EXPECT_EQ(OC_STACK_OK, m_mqBrokerResource->requestMQPublish(m_queryParams,
                    ICResourceHelper::requestMQPublishCB, QualityOfService::LowQos))<<"publish MQ does not work.";
    ICHelper::waitForServerResponse();
    ASSERT_EQ(true, ICResourceHelper::s_isrequestTopicPublishSuccess)<<"publishMQTopic got fail.";
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee));
    SUCCEED();

}
#endif

/**
 * @since 2017-02-03
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)() API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'subscribeMQTopic' API With valid scenario
 * @target OCStackResult subscribeMQTopic(const QueryParamsMap& queryParametersMap,
                                        MQTopicCallback attributeHandler,
                                        QualityOfService qos)
 * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4. MQTopicCallback 5. QualityOfService
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn and CreateGroup
 *                4. discover Topic
 * @procedure Call subscribeMQTopic API with valid parameter
 * @post_condition SignOut
 * @expected subscribeMQTopic API will return OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCResourceTest_stc, SubscribeMQTopic_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
    ASSERT_EQ(OC_STACK_OK, m_mqBrokerResource->createMQTopic(m_rep, TOPIC_URI,
                    m_queryParams, ICResourceHelper::createTopicCB, QualityOfService::LowQos))<<"Create MQ does not work.";
    ICHelper::waitForServerResponse();
    ICResourceHelper::s_TopicList.clear();

    EXPECT_EQ(OC_STACK_OK, m_mqBrokerResource->discoveryMQTopics(m_queryParams,
                    ICResourceHelper::discoverTopicCB, QualityOfService::LowQos))<<"discovery MQ does not work.";

    ICHelper::waitForServerResponse();

    m_mqSelectedTopicResource = ICResourceHelper::s_TopicList[index];
    EXPECT_EQ(OC_STACK_OK, m_mqSelectedTopicResource->subscribeMQTopic(ObserveType::Observe, m_queryParams,
                    ICResourceHelper::subscribeCB, QualityOfService::LowQos))<<"subscribeMQTopic does not work.";
    ICHelper::waitForServerResponse();
    ASSERT_EQ(true, ICResourceHelper::s_isSubscribeSuccess)<<"subscribeMQTopic got fail.";
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee));
    SUCCEED();

}
#endif

/**
 * @since 2017-02-03
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)() API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'unsubscribeMQTopic' API With valid scenario
 * @target OCStackResult unsubscribeMQTopic(QualityOfService qos)
 * @test_data 1. OCRepresentation 2. topicUri 3. queryParametersMap 4. MQTopicCallback 5. QualityOfService
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn
 *                4. discover Topic
 * @procedure Call unsubscribeMQTopic API with valid parameter
 * @post_condition SignOut
 * @expected unsubscribeMQTopic API should return OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCResourceTest_stc, UnsubscribeMQTopic_SRC_P)
{
    try
    {
        m_rep["message"] = MESSAGE;
        EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
        ASSERT_EQ(OC_STACK_OK, m_mqBrokerResource->createMQTopic(m_rep, TOPIC_URI,
                        m_queryParams, ICResourceHelper::createTopicCB, QualityOfService::LowQos))<<"Create MQ does not work.";
        ICHelper::waitForServerResponse();
        ICResourceHelper::s_TopicList.clear();
        EXPECT_EQ(OC_STACK_OK, m_mqBrokerResource->discoveryMQTopics(m_queryParams,
                        ICResourceHelper::discoverTopicCB, QualityOfService::LowQos))<<"discovery MQ does not work.";
        ICHelper::waitForServerResponse();
        m_mqSelectedTopicResource = ICResourceHelper::s_TopicList[index];
        EXPECT_EQ(OC_STACK_OK, m_mqSelectedTopicResource->unsubscribeMQTopic(QualityOfService::LowQos))<<"publish MQ does not work.";
        ICHelper::waitForServerResponse();

        EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee));
        SUCCEED();
    }
    catch (OCException &ex)
    {
        cout << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
        return false;
    }

}
#endif
