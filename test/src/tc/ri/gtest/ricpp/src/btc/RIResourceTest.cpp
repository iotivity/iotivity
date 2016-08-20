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
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#include "RIHelper.h"

class RIResourceTest_btc: public ::testing::Test
{
protected:
    RIHelper *m_RIHelper;
    string m_HostAddress = "coap://192.168.1.2:5000";
    string m_ResAddress = "/resource";
    OCRepresentation m_Representation;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_RIHelper = RIHelper::getInstance();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }

public:
    void onGetPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
    {
    }

    void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
            const int &sequenceNumber)
    {
    }

    void deleteResource(const HeaderOptions &headerOptions, const int eCode)
    {
    }
};

/**
 * @since 2016-07-12
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test get() positively
 * @target OCStackResult get(const QueryParamsMap& queryParametersMap, GetCallback attributeHandler)
 * @test_data 1. QueryParamsMap
 *            2. attributeHandler
 * @pre_condition Constuct resource
 * @procedure Perform get() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, GetResource_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->get(OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3)));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test get() positively
 * @target OCStackResult get(const QueryParamsMap& queryParametersMap, GetCallback attributeHandler)
 * @test_data 1. QueryParamsMap
 *            2. attributeHandler
 * @pre_condition Construct resource
 * @procedure Perform get() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, GetResourceWithLowQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->get(OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::LowQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test get() positively
 * @target OCStackResult get(const QueryParamsMap& queryParametersMap, GetCallback attributeHandler,
 *                       QualityOfService QoS)
 * @test_data 1. QueryParamsMap
 *            2. attributeHandler
 *            3. Qos = HighQos
 * @pre_condition Construct resource
 * @procedure Perform get() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, GetResourceWithHighQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->get(OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::HighQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test get() positively
 * @target OCStackResult get(const QueryParamsMap& queryParametersMap, GetCallback attributeHandler,
 *                       QualityOfService QoS)
 * @test_data 1. QueryParamsMap
 *            2. attributeHandler
 *            3. Qos = MidQos
 * @pre_condition Construct resource
 * @procedure Perform get() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, GetResourceWithMidQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->get(OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::MidQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test get() positively
 * @target OCStackResult get(const QueryParamsMap& queryParametersMap, GetCallback attributeHandler,
 *                       QualityOfService QoS)
 * @test_data 1. QueryParamsMap
 *            2. attributeHandler
 *            3. Qos = NaQos
 * @pre_condition Construct resource
 * @procedure Perform get() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, GetResourceWithNaQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->get(OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::NaQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test get() with resourceType and Interface positively
 * @target OCStackResult get(const std::string& resourceType, const std::string& resourceInterface,
 *                       const QueryParamsMap& queryParametersMap, GetCallback attributeHandler)
 * @test_data 1. resourceType = ""
 *            2. resourceInterface = DEFAULT_INTERFACE
 *            3. QueryParamsMap
 *            4. attributeHandler
 * @pre_condition Construct resource
 * @procedure Perform get() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, GetResourceWithRTIF_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->get("", DEFAULT_INTERFACE, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3)));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test get() with resourceType and Interface positively
 * @target OCStackResult get(const std::string& resourceType, const std::string& resourceInterface,
 *                       const QueryParamsMap& queryParametersMap, GetCallback attributeHandler)
 * @test_data 1. resourceType = nullptr
 *            2. resourceInterface = DEFAULT_INTERFACE
 *            3. QueryParamsMap
 *            4. attributeHandler
 * @pre_condition Construct resource
 * @procedure Perform get() using resource
 * @post_condition None
 * @expected Should throw exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, GetResourceWithRT_NV_N)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_ANY_THROW(
            resource->get(nullptr, DEFAULT_INTERFACE, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3)));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test get() with resourceType and Interface positively
 * @target OCStackResult get(const std::string& resourceType, const std::string& resourceInterface,
 *                       const QueryParamsMap& queryParametersMap, GetCallback attributeHandler)
 * @test_data 1. resourceType = ""
 *            2. resourceInterface = nullptr
 *            3. QueryParamsMap
 *            4. attributeHandler
 * @pre_condition Construct resource
 * @procedure Perform get() using resource
 * @post_condition None
 * @expected Should throw exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, GetResourceWithIF_NV_N)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_ANY_THROW(
            resource->get("", nullptr, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3)));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test get() with resourceType and Interface positively
 * @target OCStackResult get(const std::string& resourceType, const std::string& resourceInterface,
 *                       const QueryParamsMap& queryParametersMap, GetCallback attributeHandler,
 *                       QualityOfService QoS)
 * @test_data 1. resourceType = ""
 *            2. resourceInterface = DEFAULT_INTERFACE
 *            3. QueryParamsMap
 *            4. attributeHandler
 *            5. Qos = LowQos
 * @pre_condition Construct resource
 * @procedure Perform get() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, GetResourceWithRTIFQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->get("", DEFAULT_INTERFACE, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::LowQos));
}
#endif

/**
 * @since 2016-07-12
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test put() with Qos positively
 * @target OCStackResult put(const OCRepresentation& representation,
 *                       const QueryParamsMap& queryParametersMap, PutCallback attributeHandler)
 * @test_data 1. QueryParamsMap
 *            2. attributeHandler
 * @pre_condition Construct resource
 * @procedure Perform put() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PutResource_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->put(m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3)));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test put() with Qos positively
 * @target OCStackResult put(const OCRepresentation& representation,
 *                       const QueryParamsMap& queryParametersMap, PutCallback attributeHandler,
 *                       QualityOfService QoS)
 * @test_data 1. QueryParamsMap
 *            2. attributeHandler
 *            3. Qos = LowQos
 * @pre_condition Construct resource
 * @procedure Perform put() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PutResourceWithLowQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->put(m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::LowQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test put() with Qos positively
 * @target OCStackResult put(const OCRepresentation& representation,
 *                       const QueryParamsMap& queryParametersMap, PutCallback attributeHandler,
 *                       QualityOfService QoS)
 * @test_data 1. QueryParamsMap
 *            2. attributeHandler
 *            3. Qos = HighQos
 * @pre_condition Construct resource
 * @procedure Perform put() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PutResourceWithHighQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->put(m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::HighQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test put() with Qos positively
 * @target OCStackResult put(const OCRepresentation& representation,
 *                       const QueryParamsMap& queryParametersMap, PutCallback attributeHandler,
 *                       QualityOfService QoS)
 * @test_data 1. QueryParamsMap
 *            2. attributeHandler
 *            3. Qos = MidQos
 * @pre_condition Construct resource
 * @procedure Perform put() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PutResourceWithMidQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->put(m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::MidQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test put() with Qos positively
 * @target OCStackResult put(const OCRepresentation& representation,
 *                       const QueryParamsMap& queryParametersMap, PutCallback attributeHandler,
 *                       QualityOfService QoS)
 * @test_data 1. QueryParamsMap
 *            2. attributeHandler
 *            3. Qos = NaQos
 * @pre_condition Construct resource
 * @procedure Perform put() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PutResourceWithNaQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->put(m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::NaQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test put() with resourceType and Interface positively
 * @target OCStackResult put(const std::string& resourceType, const std::string& resourceInterface,
 *                       const OCRepresentation& representation, const QueryParamsMap& queryParametersMap,
 *                       PutCallback attributeHandler)
 * @test_data 1. resourceType = ""
 *            2. resourceInterface = DEFAULT_INTERFACE
 *            3. attributeHandler
 * @pre_condition Construct resource
 * @procedure Perform put() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PutResourceWithRTIF_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->put("", DEFAULT_INTERFACE, m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3)));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test put() with resourceType and Interface positively
 * @target OCStackResult put(const std::string& resourceType, const std::string& resourceInterface,
 *                       const OCRepresentation& representation, const QueryParamsMap& queryParametersMap,
 *                       PutCallback attributeHandler, QualityOfService QoS)
 * @test_data 1. resourceType = ""
 *            2. resourceInterface = DEFAULT_INTERFACE
 *            3. attributeHandler
 *            4. Qos = LowQos
 * @pre_condition Construct resource
 * @procedure Perform put() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PutResourceWithRTIFQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->put("", DEFAULT_INTERFACE, m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::LowQos));
}
#endif

/**
 * @since 2016-07-12
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test post() positively
 * @target OCStackResult post(const OCRepresentation& representation,
 *                       const QueryParamsMap& queryParametersMap, PostCallback attributeHandler)
 * @test_data attributeHandler
 * @pre_condition Construct resource
 * @procedure Perform post() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PostResource_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->post(m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3)));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test post() with Qos positively
 * @target OCStackResult post(const OCRepresentation& representation,
 *                       const QueryParamsMap& queryParametersMap, PostCallback attributeHandler)
 *                       QualityOfService QoS)
 * @test_data 1. attributeHandler
 *            2. Qos = LowQos
 * @pre_condition Construct resource
 * @procedure Perform post() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PostResourceWithLowQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->post(m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::LowQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test post() with Qos positively
 * @target OCStackResult post(const OCRepresentation& representation,
 *                       const QueryParamsMap& queryParametersMap, PostCallback attributeHandler)
 *                       QualityOfService QoS)
 * @test_data 1. attributeHandler
 *            2. Qos = HighQos
 * @pre_condition Construct resource
 * @procedure Perform post() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PostResourceWithHighQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->post(m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::HighQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test post() with Qos positively
 * @target OCStackResult post(const OCRepresentation& representation,
 *                       const QueryParamsMap& queryParametersMap, PostCallback attributeHandler)
 *                       QualityOfService QoS)
 * @test_data 1. attributeHandler
 *            2. Qos = MidQos
 * @pre_condition Construct resource
 * @procedure Perform post() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PostResourceWithMidQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->post(m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::MidQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test post() with Qos positively
 * @target OCStackResult post(const OCRepresentation& representation,
 *                       const QueryParamsMap& queryParametersMap, PostCallback attributeHandler)
 *                       QualityOfService QoS)
 * @test_data 1. attributeHandler
 *            2. Qos = NaQos
 * @pre_condition Construct resource
 * @procedure Perform post() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PostResourceWithNaQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->post(m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::NaQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test post() with resourceType and resource interface positively
 * @target OCStackResult post(const std::string& resourceType, const std::string& resourceInterface,
 const OCRepresentation& representation, const QueryParamsMap& queryParametersMap,
 PostCallback attributeHandler)
 * @test_data 1. resourceType = "'
 *            2. resourceInterface = DEFAULT_INTERFACE
 *            3. attributeHandler
 * @pre_condition Construct resource
 * @procedure Perform post() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PostResourceWithRTIF_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->post("", DEFAULT_INTERFACE, m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3)));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test post() with resourceType, resource interface and Qos positively
 * @target OCStackResult post(const std::string& resourceType, const std::string& resourceInterface,
 const OCRepresentation& representation, const QueryParamsMap& queryParametersMap,
 PostCallback attributeHandler, QualityOfService QoS)
 * @test_data 1. resourceType = "'
 *            2. resourceInterface = DEFAULT_INTERFACE
 *            3. attributeHandler
 *            4. Qos = LowQos
 * @pre_condition Construct resource
 * @procedure Perform post() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, PostResourceWithRTIFQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->post("", DEFAULT_INTERFACE, m_Representation, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onGetPut, this, PH::_1, PH::_2, PH::_3),
                    OC::QualityOfService::LowQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test observe() positively
 * @target OCStackResult observe(ObserveType observeType, const QueryParamsMap& queryParametersMap,
 ObserveCallback observeHandler)
 * @test_data 1. observeType = ObserveAll
 *            2. observeHandler
 * @pre_condition Construct resource
 * @procedure Perform observe() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, ObserveResource_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->observe(ObserveType::ObserveAll, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onObserve, this, PH::_1, PH::_2, PH::_3, PH::_4)));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test observe() with Qos positively
 * @target OCStackResult observe(ObserveType observeType, const QueryParamsMap& queryParametersMap,
 ObserveCallback observeHandler)
 * @test_data 1. observeType = ObserveAll
 *            2. observeHandler
 *            3. Qos = LowQos
 * @pre_condition Construct resource
 * @procedure Perform observe() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, ObserveResourceWithLowQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->observe(ObserveType::ObserveAll, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onObserve, this, PH::_1, PH::_2, PH::_3, PH::_4),
                    QualityOfService::LowQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test observe() with Qos positively
 * @target OCStackResult observe(ObserveType observeType, const QueryParamsMap& queryParametersMap,
 ObserveCallback observeHandler)
 * @test_data 1. observeType = ObserveAll
 *            2. observeHandler
 *            3. Qos = HighQos
 * @pre_condition Construct resource
 * @procedure Perform observe() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, ObserveResourceWithHighQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->observe(ObserveType::ObserveAll, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onObserve, this, PH::_1, PH::_2, PH::_3, PH::_4),
                    QualityOfService::HighQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test observe() with Qos positively
 * @target OCStackResult observe(ObserveType observeType, const QueryParamsMap& queryParametersMap,
 ObserveCallback observeHandler)
 * @test_data 1. observeType = ObserveAll
 *            2. observeHandler
 *            3. Qos = MidQos
 * @pre_condition Construct resource
 * @procedure Perform observe() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, ObserveResourceWithMidQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->observe(ObserveType::ObserveAll, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onObserve, this, PH::_1, PH::_2, PH::_3, PH::_4),
                    QualityOfService::MidQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test observe() with Qos positively
 * @target OCStackResult observe(ObserveType observeType, const QueryParamsMap& queryParametersMap,
 ObserveCallback observeHandler)
 * @test_data 1. observeType = ObserveAll
 *            2. observeHandler
 *            3. Qos = NaQos
 * @pre_condition Construct resource
 * @procedure Perform observe() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, ObserveResourceWithNaQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->observe(ObserveType::ObserveAll, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onObserve, this, PH::_1, PH::_2, PH::_3, PH::_4),
                    QualityOfService::NaQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @see OCStackResult observe(ObserveType observeType, const QueryParamsMap& queryParametersMap,
 *                    ObserveCallback observeHandler)
 * @objective Test cancelObserve() positively
 * @target  OCStackResult cancelObserve()
 * @test_data None
 * @pre_condition 1. Construct resource
 *                2. observer request to the resource
 * @procedure Perform cancelObserve() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, CancelObserveResource_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);

    resource->observe(ObserveType::ObserveAll, OC::QueryParamsMap(),
            bind(&RIResourceTest_btc::onObserve, this, PH::_1, PH::_2, PH::_3, PH::_4));

    EXPECT_EQ(OC_STACK_OK, resource->cancelObserve());
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @see OCStackResult observe(ObserveType observeType, const QueryParamsMap& queryParametersMap,
 *                    ObserveCallback observeHandler)
 * @objective Test cancelObserve() with Qos positively
 * @target  OCStackResult cancelObserve()
 * @test_data Qos = LowQos
 * @pre_condition 1. Construct resource
 *                2. observer request to the resource
 * @procedure Perform cancelObserve() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, CancelObserveResourceWithLowQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);

    resource->observe(ObserveType::ObserveAll, OC::QueryParamsMap(),
            bind(&RIResourceTest_btc::onObserve, this, PH::_1, PH::_2, PH::_3, PH::_4),
            QualityOfService::LowQos);

    EXPECT_EQ(OC_STACK_OK, resource->cancelObserve(QualityOfService::LowQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @see OCStackResult observe(ObserveType observeType, const QueryParamsMap& queryParametersMap,
 *                    ObserveCallback observeHandler)
 * @objective Test cancelObserve() with Qos positively
 * @target  OCStackResult cancelObserve()
 * @test_data Qos = HighQos
 * @pre_condition 1. Construct resource
 *                2. observer request to the resource
 * @procedure Perform cancelObserve() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, CancelObserveResourceWithHighQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->observe(ObserveType::ObserveAll, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onObserve, this, PH::_1, PH::_2, PH::_3, PH::_4),
                    QualityOfService::HighQos));

    EXPECT_EQ(OC_STACK_OK, resource->cancelObserve(QualityOfService::HighQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @see OCStackResult observe(ObserveType observeType, const QueryParamsMap& queryParametersMap,
 *                    ObserveCallback observeHandler)
 * @objective Test cancelObserve() with Qos positively
 * @target  OCStackResult cancelObserve()
 * @test_data Qos = MidQos
 * @pre_condition 1. Construct resource
 *                2. observer request to the resource
 * @procedure Perform cancelObserve() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, CancelObserveResourceWithMidQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->observe(ObserveType::ObserveAll, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onObserve, this, PH::_1, PH::_2, PH::_3, PH::_4),
                    QualityOfService::MidQos));

    EXPECT_EQ(OC_STACK_OK, resource->cancelObserve(QualityOfService::MidQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @see OCStackResult observe(ObserveType observeType, const QueryParamsMap& queryParametersMap,
 *                    ObserveCallback observeHandler)
 * @objective Test cancelObserve() with Qos positively
 * @target  OCStackResult cancelObserve()
 * @test_data Qos = NaQos
 * @pre_condition 1. Construct resource
 *                2. observer request to the resource
 * @procedure Perform cancelObserve() using resource
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, CancelObserveResourceWithNaQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->observe(ObserveType::ObserveAll, OC::QueryParamsMap(),
                    bind(&RIResourceTest_btc::onObserve, this, PH::_1, PH::_2, PH::_3, PH::_4),
                    QualityOfService::NaQos));

    EXPECT_EQ(OC_STACK_OK, resource->cancelObserve(QualityOfService::NaQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test deleteResource() positively
 * @target OCStackResult deleteResource(DeleteCallback deleteHandler)
 * @test_data deleteHandler
 * @pre_condition Construct resource
 * @procedure Perform deleteResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, DeleteResource_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->deleteResource(
                    bind(&RIResourceTest_btc::deleteResource, this, PH::_1, PH::_2)));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test deleteResource() positively
 * @target OCStackResult deleteResource(DeleteCallback deleteHandler, QualityOfService QoS)
 * @test_data 1. deleteHandler
 *            2. Qos = LowQos
 * @pre_condition Construct resource
 * @procedure Perform deleteResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, DeleteResourceWithLowQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->deleteResource(
                    bind(&RIResourceTest_btc::deleteResource, this, PH::_1, PH::_2),
                    QualityOfService::LowQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test deleteResource() positively
 * @target OCStackResult deleteResource(DeleteCallback deleteHandler, QualityOfService QoS)
 * @test_data 1. deleteHandler
 *            2. Qos = HighQos
 * @pre_condition Construct resource
 * @procedure Perform deleteResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, DeleteResourceWithHighQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->deleteResource(
                    bind(&RIResourceTest_btc::deleteResource, this, PH::_1, PH::_2),
                    QualityOfService::HighQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test deleteResource() positively
 * @target OCStackResult deleteResource(DeleteCallback deleteHandler, QualityOfService QoS)
 * @test_data 1. deleteHandler
 *            2. Qos = MidQos
 * @pre_condition Construct resource
 * @procedure Perform deleteResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, DeleteResourceWithMidQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->deleteResource(
                    bind(&RIResourceTest_btc::deleteResource, this, PH::_1, PH::_2),
                    QualityOfService::MidQos));
}
#endif

/**
 * @since 2016-07-13
 * @see OCResource::Ptr constructResourceObject(const std::string& host, const std::string& uri,
 *                       OCConnectivityType connectivityType, bool isObservable,
 *                       const std::vector<std::string>& resourceTypes,
 *                       const std::vector<std::string>& interfaces)
 * @objective Test deleteResource() positively
 * @target OCStackResult deleteResource(DeleteCallback deleteHandler, QualityOfService QoS)
 * @test_data 1. deleteHandler
 *            2. Qos = NaQos
 * @pre_condition Construct resource
 * @procedure Perform deleteResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIResourceTest_btc, DeleteResourceWithNaQos_SRC_P)
{
    OCResource::Ptr resource = m_RIHelper->constructResourceObject(m_HostAddress, m_ResAddress);

    EXPECT_TRUE(resource != NULL);
    EXPECT_EQ(OC_STACK_OK,
            resource->deleteResource(
                    bind(&RIResourceTest_btc::deleteResource, this, PH::_1, PH::_2),
                    QualityOfService::NaQos));
}
#endif
