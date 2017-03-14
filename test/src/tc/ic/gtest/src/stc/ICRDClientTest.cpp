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

static OCAccountManager::Ptr g_accountMgrControlee = nullptr;

class ICRDClientTest_stc: public ::testing::Test
{
public:
    RDClient m_rdClient;
    ResourceHandles m_resourceHandles;
    OCPresenceHandle m_ocPresenceHandle = nullptr;
    QueryParamsList m_queryParams =
    { };
    const std::vector< std::string > m_vecDeviceIdentifier;bool m_isCallbackInvoked;
    OCStackResult m_actualResult;

    OCAccountManager::Ptr m_accountManager;
    ICHelper* m_ICHelper;
    ICHelper m_ICHelperForCloud;

protected:
    string m_hostAddress = COAP_HOST_ADDRESS;
    ICRDClientTest_stc()
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
        m_ICHelperForCloud.initCloudClient();
        m_actualResult = OC_STACK_ERROR;
        m_isCallbackInvoked = false;
        ICHelper::s_IsDeleteSuccessful = false;
        ICHelper::s_IsServerResponsed = false;

        g_accountMgrControlee = OCPlatform::constructAccountManagerObject(m_hostAddress,
                CT_ADAPTER_TCP);
    }

    virtual void TearDown()
    {
        if (!ICHelper::isUnResourceRegistered())
        {
            cout << "UnResourceRegistered failed.";
        }
        EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee));
        CommonTestUtil::runCommonTCTearDownPart();

    }

};

/*
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' positively to Publish Virtual Resource to RD
 * @target          OCStackResult publishResourceToRD(const std::string& host, OCConnectivityType connectivityType, OC::ResourceHandles& resourceHandles, PublishResourceCallback callback, QualityOfService qos))
 * @test_data       1. host                 Host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  signOut
 * @expected        It will Publish Virtual Resource to Resource Directory & return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_stc, PublishResourcesToRDWithResourceHandelerAndQOS_SRC_FSV_P)
{
    m_actualResult = OC_STACK_ERROR;
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));

    ASSERT_TRUE(m_ICHelper->isResourceRegistered()) << "Resource registration is unsuccessful";
    ASSERT_TRUE(!ICHelper::s_ResourceHandles.empty()) << "Resource handler is null";
    ASSERT_TRUE(ICHelper::s_IsServerResponsed) << "server is not responsed.";
    m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,
            ICHelper::s_ResourceHandles, &ICHelper::onPublish, IC_OC_QUALITY_OF_SERVICE);
    ICHelper::waitForServerResponse();
    EXPECT_EQ(OC_STACK_OK, m_actualResult) << "Resource is not published.." << CommonUtil::s_OCStackResultString.at(m_actualResult);

    ASSERT_TRUE(ICHelper::s_IsPublishSuccessful) << "publish Callback is not invoked.";
    SUCCEED();
}
#endif

/*
 * @since           2017-01-24
 * @see             none
 * @objective       Test 'publishResourceToRD' positively to Publish Virtual Resource to RD
 * @target          OCStackResult publishResourceToRD(const std::string& host, OCConnectivityType connectivityType, PublishResourceCallback callback, QualityOfService qos))
 * @test_data       1. host                 Host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  signOut
 * @expected        It will Publish Virtual Resource to Resource Directory & return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_stc, PublishResourcesToRDWithQOS_SRC_FSV_P)
{
    m_actualResult = OC_STACK_ERROR;
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
    ASSERT_TRUE(m_ICHelper->isResourceRegistered()) << "Resource registration is unsuccessful";
    ASSERT_TRUE(!ICHelper::s_ResourceHandles.empty()) << "Resource handler is null";
    ASSERT_TRUE(ICHelper::s_IsServerResponsed) << "server is not responsed.";
    m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,
            &ICHelper::onPublish, IC_OC_QUALITY_OF_SERVICE);
    ICHelper::waitForServerResponse();
    EXPECT_EQ(OC_STACK_OK, m_actualResult) << "Resource is not published.." << CommonUtil::s_OCStackResultString.at(m_actualResult);
    ASSERT_TRUE(ICHelper::s_IsPublishSuccessful) << "Callback is not invoked.";
    SUCCEED();
}
#endif

/*
 * @since           2017-01-24
 * @see             none
 * @objective       Test 'publishResourceToRD' positively to Publish Virtual Resource to RD
 * @target          OCStackResult publishResourceToRD(const std::string& host, OCConnectivityType connectivityType, OC::ResourceHandles& resourceHandles, PublishResourceCallback callback))
 * @test_data       1. host                 Host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  signOut
 * @expected        It will Publish Virtual Resource to Resource Directory & return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_stc, PublishResourcesToRDWithRESHandle_SRC_FSV_P)
{
    m_actualResult = OC_STACK_ERROR;
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
    ASSERT_TRUE(m_ICHelper->isResourceRegistered()) << "Resource registration is unsuccessful";
    ASSERT_TRUE(!ICHelper::s_ResourceHandles.empty()) << "Resource handler is null";
    ASSERT_TRUE(ICHelper::s_IsServerResponsed) << "server is not responsed.";
    m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,ICHelper::s_ResourceHandles,
            &ICHelper::onPublish);
    ICHelper::waitForServerResponse();
    EXPECT_EQ(OC_STACK_OK, m_actualResult) << "Resource is not published.." << CommonUtil::s_OCStackResultString.at(m_actualResult);
    ASSERT_TRUE(ICHelper::s_IsPublishSuccessful) << "Callback is not invoked.";
    SUCCEED();

    SUCCEED();
}
#endif

/*
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' positively to Publish Virtual Resource to RD
 * @target          OCStackResult deleteResourceFromRD(const std::string& host, OCConnectivityType connectivityType, OC::ResourceHandles& resourceHandles, PublishResourceCallback callback, QualityOfService qos))
 * @test_data       1. host                 Host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn
 * @procedure       1. Call publishResourceToRD API
 *                  2. Call deleteResourceFromRD and check it's return value
 * @post_condition  signOut
 * @expected        deleteResourceFromRD should delete resource and return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_stc, DeleteResourcesToRDWithRESHandleAndQOS_SRC_FSV_P)
{
    m_actualResult = OC_STACK_ERROR;
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
    ASSERT_TRUE(m_ICHelper->isResourceRegistered()) << "Resource registration is unsuccessful";
    ASSERT_TRUE(!ICHelper::s_ResourceHandles.empty()) << "Resource handler is null";
    m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,
            ICHelper::s_ResourceHandles, &ICHelper::onPublish, IC_OC_QUALITY_OF_SERVICE);
    ICHelper::waitForServerResponse();
    EXPECT_EQ(OC_STACK_OK, m_actualResult) << "Resource is not published.." << CommonUtil::s_OCStackResultString.at(m_actualResult);
    ASSERT_TRUE(ICHelper::s_IsPublishSuccessful) << "publish Callback is not invoked.";

    m_actualResult = OC_STACK_ERROR;
    m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE, ICHelper::s_ResourceHandles,
            &ICHelper::onDelete, IC_OC_QUALITY_OF_SERVICE);

    ICHelper::waitForServerResponse();

    ASSERT_EQ(OC_STACK_OK,m_actualResult) << "Delete resource operation is unsuccessful. Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    ASSERT_TRUE(ICHelper::s_IsDeleteSuccessful) << "onDelete Callback is not invoked.";

    SUCCEED();
}
#endif

/*
 * @since           2017-01-24
 * @see             none
 * @objective       Test 'deleteResourceFromRD' positively
 * @target          OCStackResult deleteResourceFromRD(const std::string& host, OCConnectivityType connectivityType, DeleteResourceCallback callback, QualityOfService qos)
 * @test_data     1. host                 Host address
 *                2. connectivityType     CT_DEFAULT as connectivity
 *                3. callback             onPublish as callback
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn
 * @procedure     1. Call publishResourceToRD API
 *                2. Call deleteResourceFromRD and check it's return value
 * @post_condition  signOut
 * @expected      deleteResourceFromRD should delete resource and return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_stc, DeleteResourcesToRDWithQOS_SRC_FSV_P)
{
    m_actualResult = OC_STACK_ERROR;
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
    ASSERT_TRUE(m_ICHelper->isResourceRegistered()) << "Resource registration is unsuccessful";
    ICHelper::waitForServerResponse();
    ASSERT_TRUE(!ICHelper::s_ResourceHandles.empty()) << "Resource handler is null";
    m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,
            &ICHelper::onPublish, IC_OC_QUALITY_OF_SERVICE);
    ICHelper::waitForServerResponse();
    EXPECT_EQ(OC_STACK_OK, m_actualResult) << "Resource is not published.." << CommonUtil::s_OCStackResultString.at(m_actualResult);
    ASSERT_TRUE(ICHelper::s_IsPublishSuccessful) << "publish Callback is not invoked.";

    m_actualResult = OC_STACK_ERROR;
    m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,
            &ICHelper::onDelete, IC_OC_QUALITY_OF_SERVICE);

    ICHelper::waitForServerResponse();

    ASSERT_EQ(OC_STACK_OK,m_actualResult) << "Delete resource operation is unsuccessful. Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    ASSERT_TRUE(ICHelper::s_IsDeleteSuccessful) << "onDelete Callback is not invoked.";

    SUCCEED();
}
#endif

/*
 * @since           2017-01-24
 * @see             none
 * @objective       Test 'publishResourceToRD' positively to Publish Virtual Resource to RD
 * @target          OCStackResult deleteResourceFromRD(const std::string& host, OCConnectivityType connectivityType,OC::ResourceHandles& resourceHandles, DeleteResourceCallback callback)
 * @test_data     1. host                 Host address
 *                2. connectivityType     CT_DEFAULT as connectivity
 *                3. callback             onPublish as callback
 * @pre_condition 1. constructAccountManagerObject(host, connectivity_type) API
 *                2. SignUp
 *                3. SignIn
 * @procedure     1. Call publishResourceToRD API
 *                2. Call deleteResourceFromRD and check it's return value
 * @post_condition  signOut
 * @expected        deleteResourceFromRD should delete resource and return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDClientTest_stc, DeleteResourcesToRDWithRESHandle_SRC_FSV_P)
{
    m_actualResult = OC_STACK_ERROR;
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee));
    ASSERT_TRUE(m_ICHelper->isResourceRegistered()) << "Resource registration is unsuccessful";
    ICHelper::waitForServerResponse();
    ASSERT_TRUE(!ICHelper::s_ResourceHandles.empty()) << "Resource handler is null";
    m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,
            ICHelper::s_ResourceHandles, &ICHelper::onPublish);
    ICHelper::waitForServerResponse();
    EXPECT_EQ(OC_STACK_OK, m_actualResult) << "Resource is not published.." << CommonUtil::s_OCStackResultString.at(m_actualResult);
    ASSERT_TRUE(ICHelper::s_IsPublishSuccessful) << "publish Callback is not invoked.";

    m_actualResult = OC_STACK_ERROR;
    m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,ICHelper::s_ResourceHandles,
            &ICHelper::onDelete);

    ICHelper::waitForServerResponse();

    ASSERT_EQ(OC_STACK_OK,m_actualResult) << "Delete resource operation is unsuccessful. Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
    ASSERT_TRUE(ICHelper::s_IsDeleteSuccessful) << "onDelete Callback is not invoked.";

    SUCCEED();
}
#endif
