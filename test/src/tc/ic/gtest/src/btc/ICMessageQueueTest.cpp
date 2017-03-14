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

using namespace OC;

class ICMessageQueueTest_btc: public ::testing::Test{

public:
    std::vector< std::string > m_types = { "oic.ps" };
    std::vector< std::string > m_iFaces = { DEFAULT_INTERFACE };

    OC::OCResource::Ptr m_OCResource ;

    QueryParamsMap m_queryParametersMap;
    QualityOfService m_qos;
    OCRepresentation m_rep;
    ObserveType m_observe;
    QueryParamsMap m_query;

    static void foundResource(const int ecode, const string &originUri,
            shared_ptr< OC::OCResource > topic)
    {

    }

    static void observeHandler(const HeaderOptions&, const OCRepresentation&, const int, const int)
    {

    }

    static void attributeHandler(const HeaderOptions&, const OCRepresentation&, const int)
    {

    }

protected:
    std::string m_errorMsg;

    virtual void SetUp()
    {
        m_OCResource = OCPlatform::constructResourceObject(HOST, URI, CT_DEFAULT, true, m_types, m_iFaces);
        m_query = {};
        m_qos = QualityOfService::LowQos;
    }

    virtual void TearDown()
    {
    }
};

/**
 * @since 2016-08-25
 * @see None
 * @objective Test createMQTopic function with valid OCRepresentation, topicURI QueryMap, createCallback and QOS
 * @target createMQTopic(const OCRepresentation& rep, const std::string& topicUri, const QueryParamsMap& queryParametersMap, &createCallback)
 * @test_data OCRepresentation, topic uri, query params, createCallback and qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform createMQTopic(const OCRepresentation& rep, const std::string& topicUri, const QueryParamsMap& queryParametersMap, &createCallback) API
 * @post_condition None
 * @expected createMQTopic should return OC_STACK_OK
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, CreateMQTopic_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK, m_OCResource->createMQTopic(m_rep, TOPIC_URI, m_queryParametersMap, &foundResource, m_qos))<<"CreateMQTopic is not creating Topic into MQ Broker";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test createMQTopic function with NULL callback
 * @target createMQTopic(const OCRepresentation& rep, const std::string& topicUri, const QueryParamsMap& queryParametersMap, &createCallback)
 * @test_data OCRepresentation, topic uri, query params, NULL createCallback and qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform createMQTopic(const OCRepresentation& rep, const std::string& topicUri, const QueryParamsMap& queryParametersMap, &createCallback) API
 * @post_condition None
 * @expected createMQTopic will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, CreateMQTopicNULLCallback_NV_N)
{
    EXPECT_ANY_THROW(m_OCResource->createMQTopic(m_rep, TOPIC_URI, m_queryParametersMap, nullptr, m_qos))<<"CreateMQTopic is creating Topic into MQ Broker with NULL callback";
}
#endif

/**
 * @since 2016-08-26
 * @see None
 * @objective Test createMQTopic function with empt topicURI
 * @target createMQTopic(const OCRepresentation& rep, const std::string& topicUri, const QueryParamsMap& queryParametersMap, &createCallback)
 * @test_data OCRepresentation, empty topic uri, query params, createCallback and qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform createMQTopic(const OCRepresentation& rep, const std::string& topicUri, const QueryParamsMap& queryParametersMap, &createCallback) API
 * @post_condition None
 * @expected createMQTopic will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, CreateMQTopic_ESV_N)
{
    EXPECT_ANY_THROW(m_OCResource->createMQTopic(m_rep, EMPTY_STRING, m_queryParametersMap, &foundResource, m_qos))<<"CreateMQTopic is creating Topic into MQ Broker with empty string as topic uri";
}
#endif

/**
 * @since 2016-08-26
 * @see None
 * @objective Test createMQTopic function with invalid topicURI
 * @target createMQTopic(const OCRepresentation& rep, const std::string& topicUri, const QueryParamsMap& queryParametersMap, &createCallback)
 * @test_data OCRepresentation, invalid topic uri, query params, createCallback and qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform createMQTopic(const OCRepresentation& rep, const std::string& topicUri, const QueryParamsMap& queryParametersMap, &createCallback) API
 * @post_condition None
 * @expected createMQTopic will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, CreateMQTopic_USV_N)
{
    EXPECT_ANY_THROW(m_OCResource->createMQTopic(m_rep, INVALID, m_queryParametersMap, &foundResource, m_qos))<<"CreateMQTopic is creating Topic into MQ Broker with invalid string as topic uri";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test 'discoveryMQTopics' function with valid QueryParameterMap, createCallback and QOS
 * @target discoveryMQTopics(const QueryParamsMap& queryParametersMap, &findCallback)
 * @test_data queryParameterMap, findCallback and qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform discoveryMQTopics(const QueryParamsMap& queryParametersMap, &findCallback) API
 * @post_condition None
 * @expected discoveryMQTopics that provide OC_STACK_OK
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, DiscoveryMQTopics_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,m_OCResource->discoveryMQTopics(m_queryParametersMap, &foundResource, m_qos))<<"DiscoveryMQTopics is not to discovery Topics from MQ Broker.";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test 'discoveryMQTopics' function with NULL Callback
 * @target discoveryMQTopics(const QueryParamsMap& queryParametersMap, &findCallback)
 * @test_data queryParameterMap, NULL findCallback and qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform discoveryMQTopics(const QueryParamsMap& queryParametersMap, &findCallback) API
 * @post_condition None
 * @expected discoveryMQTopics will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, DiscoveryMQTopicsCallback_NV_N)
{
    EXPECT_ANY_THROW(m_OCResource->discoveryMQTopics(m_queryParametersMap, nullptr, m_qos))<<"DiscoveryMQTopics is to discovery Topics from MQ Broker with NULL callback.";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test requestMQPublish API with valid QueryParameterMap, publishCallback and QOS
 * @target requestMQPublish(const QueryParamsMap& queryParametersMap, &publishCallback)
 * @test_data queryParameterMap, publishCallback and qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform requestMQPublish(const QueryParamsMap& queryParametersMap, &publishCallback) API
 * @post_condition None
 * @expected requestMQPublish should return OC_STACK_OK
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, RequestMQPublish_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK, m_OCResource->requestMQPublish(m_queryParametersMap, &attributeHandler, m_qos))<<"RequestMQPublish is not to request publish to MQ publisher.";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test requestMQPublish API with NULL Callback
 * @target requestMQPublish(const QueryParamsMap& queryParametersMap, &publishCallback)
 * @test_data queryParameterMap, NULL publishCallback and qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform requestMQPublish(const QueryParamsMap& queryParametersMap, &publishCallback) API
 * @post_condition None
 * @expected requestMQPublish will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, RequestMQPublishNULLCallback_NV_N)
{
    EXPECT_ANY_THROW(m_OCResource->requestMQPublish(m_queryParametersMap, nullptr, m_qos))<<"RequestMQPublish is to request publish to MQ publisher with NULL callback.";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test subscribeMQTopic API with valid observer, QueryParameterMap, observeCallback and QOS
 * @target subscribeMQTopic(ObserveType observeType, const QueryParamsMap& queryParametersMap, &observeCallback)
 * @test_data observeType, queryParameterMap, observeCallback and qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform subscribeMQTopic(ObserveType observeType, const QueryParamsMap& queryParametersMap, &observeCallback) API
 * @post_condition None
 * @expected subscribeMQTopic should return OC_STACK_OK
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, SubscribeMQTopic_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK, m_OCResource->subscribeMQTopic(m_observe, m_queryParametersMap, &observeHandler, m_qos))<<"SubscribeMQTopic is not to subscribe Topic to MQ Broker.";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test subscribeMQTopic API with NULL Callback
 * @target subscribeMQTopic(ObserveType observeType, const QueryParamsMap& queryParametersMap, &observeCallback)
 * @test_data observeType, queryParameterMap, NULL observeCallback and qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform subscribeMQTopic(ObserveType observeType, const QueryParamsMap& queryParametersMap, &observeCallback) API
 * @post_condition None
 * @expected subscribeMQTopic will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, SubscribeMQTopicNULLCallback_NV_N)
{
    EXPECT_ANY_THROW(m_OCResource->subscribeMQTopic(m_observe, m_queryParametersMap, nullptr, m_qos))<<"SubscribeMQTopic is to subscribe Topic to MQ Broker with NULL callback.";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test unsubscribeMQTopic API with valid QOS
 * @target unsubscribeMQTopic()
 * @test_data qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform unsubscribeMQTopic() API
 * @post_condition None
 * @expected unsubscribeMQTopic should return OC_STACK_OK
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, UnsubscribeMQTopic_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,m_OCResource->subscribeMQTopic(m_observe, m_queryParametersMap, observeHandler, m_qos))<<"SubscribeMQTopic is NOT to subscribe Topic to MQ Broker with NULL callback.";
    EXPECT_EQ(OC_STACK_OK, m_OCResource->unsubscribeMQTopic(m_qos))<<"UnsubscribeMQTopic is not to unsubscribe Topic to MQ Broker.";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test publishMQTopic API with valid OCRepresentation, QueryParameterMap, publishCallback and QOS
 * @target publishMQTopic(const OCRepresentation& rep, const QueryParamsMap& queryParametersMap, &publishCallback)
 * @test_data OCRepresentation, queryParameterMap, publishCallback and qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform publishMQTopic(const OCRepresentation& rep, const QueryParamsMap& queryParametersMap, &publishCallback) API
 * @post_condition None
 * @expected publishMQTopic should return OC_STACK_OK
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, PublishMQTopic_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK, m_OCResource->publishMQTopic(m_rep, m_queryParametersMap, &attributeHandler, m_qos))<<"PublishMQTopic is not to publish Topic information into MQ Broker.";
}
#endif

/**
 * @since 2016-08-25
 * @see None
 * @objective Test publishMQTopic API with NULL Callback
 * @target publishMQTopic(const OCRepresentation& rep, const QueryParamsMap& queryParametersMap, &publishCallback)
 * @test_data OCRepresentation, queryParameterMap, NULL publishCallback and qos
 * @pre_condition constructResourceObject(host, uri, connectivity type, true, types, iFaces) API
 * @procedure Perform publishMQTopic(const OCRepresentation& rep, const QueryParamsMap& queryParametersMap, &publishCallback) API
 * @post_condition None
 * @expected publishMQTopic will throw Exception.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICMessageQueueTest_btc, PublishMQTopicNULLCall_NV_N)
{
    EXPECT_ANY_THROW(m_OCResource->publishMQTopic(m_rep, m_queryParametersMap, nullptr, m_qos))<<"PublishMQTopic is to publish Topic information into MQ Broker with NULL callback.";
}
#endif
