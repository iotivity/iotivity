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

class ICRDClientTest_btc: public ::testing::Test
{
public:
    RDClient m_rdClient;
    ResourceHandles m_resourceHandles;
    OCPresenceHandle m_ocPresenceHandle = nullptr;
    QueryParamsList m_queryParams =
    { };
    const std::vector< std::string > m_vecDeviceIdentifier;bool m_isCallbackInvoked;
    OCStackResult m_actualResult;

    ICHelper* m_ICHelper;

protected:
    string m_hostAddress = COAP_HOST_ADDRESS;
    ICRDClientTest_btc()
    {
        m_isCallbackInvoked = false;
        m_ICHelper = nullptr;
        m_actualResult = OC_STACK_ERROR;
    }

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        m_ICHelper = ICHelper::getInstance();
        m_rdClient = RDClient::Instance();
        m_actualResult = OC_STACK_ERROR;
        ASSERT_TRUE(m_ICHelper->isResourceRegistered())<< "Resource registration is unsuccessful";
    }

    virtual void TearDown()
    {
        if (!ICHelper::isUnResourceRegistered())
        {
            cout << "UnResourceRegistered failed.";
        }
        CommonTestUtil::runCommonTCTearDownPart();

    }

};

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' positively to Publish Resource to RD
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                             PublishResourceCallback callback,QualityOfService QoS)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will Publish Resource to Resource Directory & return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, PublishResourceToRDWithQOS_SRC_FSV_P)
{
    m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,
            &ICHelper::onPublish, IC_OC_QUALITY_OF_SERVICE);
    EXPECT_EQ(OC_STACK_OK,m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' negatively with Unformatted value
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                                      PublishResourceCallback callback,QualityOfService QoS)
 * @test_data       1. host                 invalid host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_URI
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, PublishResourceToRDWithQOSWithInvalidHost_USV_N)
{
    try
    {
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS_INVALID, IC_CONNECTIVITY_TYPE,
                &ICHelper::onPublish, IC_OC_QUALITY_OF_SERVICE);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_URI, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' negatively with Empty IC_HOST_ADDRESS value
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                             PublishResourceCallback callback,QualityOfService QoS)
 * @test_data       1. host                 empty address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, PublishResourceToRDWithQOSWithEmptyHost_ESV_N)
{
    try
    {
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS_EMPTY, IC_CONNECTIVITY_TYPE,
                &ICHelper::onPublish, IC_OC_QUALITY_OF_SERVICE);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_PARAM, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }

    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' negatively with NULL string value but API handled
 *                  null callback properly
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                             PublishResourceCallback callback,QualityOfService QoS)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             null as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will success & return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, PublishResourceToRDWithQOSWithNullCallback_NV_N)
{
    try
    {
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE, IC_CALLBACK_HANDLER_NULL, IC_OC_QUALITY_OF_SERVICE);
        ASSERT_EQ(OC_STACK_OK,m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' positively to Publish Virtual Resource to RD
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                                      ResourceHandles& resourceHandles,PublishResourceCallback callback)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will Publish Virtual Resource to Resource Directory & return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, PublishResourceToRDWithResourceHandle_SRC_FSV_P)
{
    try
    {
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE, m_resourceHandles,
                &ICHelper::onPublish);
        EXPECT_EQ(OC_STACK_OK,m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' negatively with Unformatted value
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                                      ResourceHandles& resourceHandles,PublishResourceCallback callback)
 * @test_data       1. host                 invalid host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_URI
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, PublishResourceToRDWithResourceHandleWithInvalidHost_USV_N)
{
    try
    {
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS_INVALID, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                &ICHelper::onPublish);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_URI, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' negatively with Empty string value
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                                      ResourceHandles& resourceHandles,PublishResourceCallback callback)
 * @test_data       1. host                 empty address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, PublishResourceToRDWithResourceHandleWithEmptyHost_ESV_N)
{
    try
    {
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS_EMPTY, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                &ICHelper::onPublish);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_PARAM, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' negatively with NULL string value but API handled
 *                  null callback properly
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                                      ResourceHandles& resourceHandles,PublishResourceCallback callback)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             null as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will success & return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, PublishResourceToRDWithResourceHandleWithNullCallback_NV_N)
{
    try
    {
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                IC_CALLBACK_HANDLER_NULL);
        ASSERT_EQ(OC_STACK_OK, m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' positively to Publish Resource to RD
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                                      ResourceHandles& resourceHandles,PublishResourceCallback callback,QualityOfService QoS)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will Publish Resource to Resource Directory & return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, PublishResourceToRDWithResourceHandleAndWithQOS_SRC_FSV_P)
{
    try
    {
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                &ICHelper::onPublish,IC_OC_QUALITY_OF_SERVICE);
        EXPECT_EQ(OC_STACK_OK,m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' negatively with Unformatted value
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                                      ResourceHandles& resourceHandles,PublishResourceCallback callback,QualityOfService QoS)
 * @test_data       1. host                 invalid host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_URI
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, PublishResourceToRDWithResourceHandleAndWithQOSWithInvalidHost_USV_N)
{
    try
    {
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS_INVALID, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                &ICHelper::onPublish,IC_OC_QUALITY_OF_SERVICE);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_URI, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' negatively with Empty IC_HOST_ADDRESS value
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                                      ResourceHandles& resourceHandles,PublishResourceCallback callback,QualityOfService QoS)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, PublishResourceToRDWithResourceHandleAndWithQOSWithEmptyHost_ESV_N)
{
    try
    {
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS_EMPTY, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                &ICHelper::onPublish,IC_OC_QUALITY_OF_SERVICE);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_PARAM, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' negatively with NULL string value but API handled
 *                  null callback properly
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                                      ResourceHandles& resourceHandles,PublishResourceCallback callback,QualityOfService QoS)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will success & return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, PublishResourceToRDWithResourceHandleAndWithQOSWithNullCallback_NV_N)
{
    try
    {
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                IC_CALLBACK_HANDLER_NULL,IC_OC_QUALITY_OF_SERVICE);
        ASSERT_EQ(OC_STACK_OK,m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' positively to delete Published Resource from RD
 * @target          deleteResourceFromRD(const std::string& host,OCConnectivityType connectivityType,
 *                                       DeleteResourceCallback callback,QualityOfService QoS)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onDelete as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call deleteResourceFromRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will delete Published Resource from Resource Directory & return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, DeleteResourceFromRDWithQOS_SRC_FSV_P)
{
    try
    {
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,
                &ICHelper::onDelete,IC_OC_QUALITY_OF_SERVICE);
        ASSERT_EQ(OC_STACK_OK,m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' negatively with Unformatted value
 * @target          deleteResourceFromRD(const std::string& host,OCConnectivityType connectivityType,
 *                                       DeleteResourceCallback callback,QualityOfService QoS)
 * @test_data       1. host                 invalid host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onDelete as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call deleteResourceFromRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_URI
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, DeleteResourceFromRDWithQOSWithInvalidHost_USV_N)
{
    try
    {
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS_INVALID, IC_CONNECTIVITY_TYPE,
                &ICHelper::onDelete,IC_OC_QUALITY_OF_SERVICE);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_URI, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' negatively with Empty string value
 * @target          deleteResourceFromRD(const std::string& host,OCConnectivityType connectivityType,
 *                                       DeleteResourceCallback callback,QualityOfService QoS)
 * @test_data       1. host                 epmty host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onDelete as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call deleteResourceFromRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, DeleteResourceFromRDWithQOSWithEmptyHost_ESV_N)
{
    try
    {
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS_EMPTY, IC_CONNECTIVITY_TYPE, &ICHelper::onDelete,IC_OC_QUALITY_OF_SERVICE);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_PARAM, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' negatively with NULL string value
 * @target          deleteResourceFromRD(const std::string& host,OCConnectivityType connectivityType,
 *                                       DeleteResourceCallback callback,QualityOfService QoS)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             null as callback
 *                  4. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call deleteResourceFromRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, DeleteResourceFromRDWithQOSWithNullCallback_NV_N)
{
    try
    {
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS_EMPTY, IC_CONNECTIVITY_TYPE,IC_CALLBACK_HANDLER_NULL,IC_OC_QUALITY_OF_SERVICE);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_PARAM, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' positively to delete Published Resource from RD
 * @target          deleteResourceFromRD(const std::string& host,OCConnectivityType connectivityType,
 *                                       ResourceHandles& resourceHandles,DeleteResourceCallback callback)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. resourceHandles      ResourceHandles
 *                  4. callback             onDelete as callback
 * @pre_condition   none
 * @procedure       1. Call deleteResourceFromRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will delete Published Resource from Resource Directory & return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, DeleteResourceFromRDWithResourceHandler_SRC_FSV_P)
{
    try
    {
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                &ICHelper::onDelete);
        ASSERT_EQ(OC_STACK_OK,m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' negatively with Unformatted value
 * @target          deleteResourceFromRD(const std::string& host,OCConnectivityType connectivityType,
 *                                       ResourceHandles& resourceHandles,DeleteResourceCallback callback)
 * @test_data       1. host                 invalid host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. resourceHandles      ResourceHandles
 *                  4. callback             onDelete as callback
 * @pre_condition   none
 * @procedure       1. Call deleteResourceFromRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_URI
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, DeleteResourceFromRDWithResourceHandlerWithInvalidHost_USV_N)
{
    try
    {
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS_INVALID, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                &ICHelper::onDelete);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_URI, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' negatively with Empty string value
 * @target          deleteResourceFromRD(const std::string& host,OCConnectivityType connectivityType,
 *                                       ResourceHandles& resourceHandles,DeleteResourceCallback callback)
 * @test_data       1. host                 empty host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. resourceHandles      ResourceHandles
 *                  4. callback             onDelete as callback
 * @pre_condition   none
 * @procedure       1. Call deleteResourceFromRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, DeleteResourceFromRDWithResourceHandlerWithEmptyHost_ESV_N)
{
    try
    {
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS_EMPTY, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                &ICHelper::onDelete);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_PARAM, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' negatively with NULL string value
 * @target          deleteResourceFromRD(const std::string& host,OCConnectivityType connectivityType,
 *                                       ResourceHandles& resourceHandles,DeleteResourceCallback callback)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. resourceHandles      ResourceHandles
 *                  4. callback             null as callback
 * @pre_condition   none
 * @procedure       1. Call deleteResourceFromRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, DeleteResourceFromRDWithResourceHandlerWithNullCallback_NV_N)
{
    try
    {
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS_EMPTY, IC_CONNECTIVITY_TYPE,m_resourceHandles,IC_CALLBACK_HANDLER_NULL);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_PARAM, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' positively to delete Published Resource from RD
 * @target          deleteResourceFromRD(const std::string& host,OCConnectivityType connectivityType,
 *                                       ResourceHandles& resourceHandles,DeleteResourceCallback callback, QualityOfService QoS)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. resourceHandles      ResourceHandles
 *                  4. callback             onDelete as callback
 *                  5. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call deleteResourceFromRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will delete Published Resource from Resource Directory & return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, DeleteResourceFromRDWithResourceHanlderAndQOS_SRC_FSV_P)
{
    try
    {
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                &ICHelper::onDelete,IC_OC_QUALITY_OF_SERVICE);
        ASSERT_EQ(OC_STACK_OK,m_actualResult) << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' negatively with Unformatted value
 * @target          deleteResourceFromRD(const std::string& host,OCConnectivityType connectivityType,
 *                                       ResourceHandles& resourceHandles,DeleteResourceCallback callback, QualityOfService QoS)
 * @test_data       1. host                 invalid host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. resourceHandles      ResourceHandles
 *                  4. callback             onDelete as callback
 *                  5. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call deleteResourceFromRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_URI
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, DeleteResourceFromRDWithResourceHanlderAndQOSWithInvalidHost_USV_N)
{
    try
    {
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS_INVALID, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                &ICHelper::onDelete,IC_OC_QUALITY_OF_SERVICE);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_URI, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
}
#endif

/**
 * @since           2016-08-24
 * @see             OCConnectivityType connectivityType
 * @objective       Test 'deleteResourceFromRD' negatively with Empty string value
 * @target          deleteResourceFromRD(const std::string& host,OCConnectivityType connectivityType,
 *                                       ResourceHandles& resourceHandles,DeleteResourceCallback callback, QualityOfService QoS)
 * @test_data       1. host                 empty host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. resourceHandles      ResourceHandles
 *                  4. callback             onDelete as callback
 *                  5. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call deleteResourceFromRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, DeleteResourceFromRDWithResourceHanlderAndQOSWithEmptyHost_ESV_N)
{
    try
    {
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS_EMPTY, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                &ICHelper::onDelete,IC_OC_QUALITY_OF_SERVICE);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_PARAM, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif

/**
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' negatively with NULL string value
 * @target          deleteResourceFromRD(const std::string& host,OCConnectivityType connectivityType,
 *                                       ResourceHandles& resourceHandles,DeleteResourceCallback callback, QualityOfService QoS)
 * @test_data       1. host                 host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. resourceHandles      ResourceHandles
 *                  4. callback             null as callback
 *                  5. QoS                  QualityOfService::HighQos
 * @pre_condition   none
 * @procedure       1. Call deleteResourceFromRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will not success & return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_btc, DeleteResourceFromRDWithResourceHanlderAndQOSWithNullCallback_NV_N)
{

    try
    {
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS_EMPTY, IC_CONNECTIVITY_TYPE,m_resourceHandles,
                IC_CALLBACK_HANDLER_NULL,IC_OC_QUALITY_OF_SERVICE);
        FAIL() << "Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    }
    catch(OCException ex)
    {
        EXPECT_EQ(OC_STACK_INVALID_PARAM, ex.code()) << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }
    SUCCEED();
}
#endif
