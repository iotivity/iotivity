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
class ICOCPlatformTest_stc: public ::testing::Test
{

protected:
    std::string m_errorMsg;
    string m_hostAddress = COAP_HOST_ADDRESS;
    string m_resourceUri = "/oic/res?di=oic.wk.d";
    OCStackResult m_result;
    ICHelper m_ICHelper;
    ICHelper* m_ICHelperPtr;
    RDClient m_rdClient;

    virtual void SetUp()
    {
        m_result = OC_STACK_ERROR;
        CommonTestUtil::runCommonTCSetUpPart();
        m_ICHelper.initCloudClient();
        m_ICHelperPtr = ICHelper::getInstance();
        m_rdClient = RDClient::Instance();
        m_hostAddress= CloudCommonUtil::getDefaultHostAddess();
        g_accountMgrControlee = OCPlatform::constructAccountManagerObject(m_hostAddress,
                CT_ADAPTER_TCP);
    }

    virtual void TearDown()
    {
    }

};

/**
 * @since 2017-02-06
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'subscribeDevicePresence' API With valid scenario
 * @target "OCStackResult subscribeDevicePresence(OCPresenceHandle& presenceHandle, const std::string& host, const std::vector<std::string>& di, OCConnectivityType connectivityType, ObserveCallback callback);"
 * @test_data host, presenceHandle, di, connectivityType
 * @pre_condition  constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure 1. Register and Publish Resource To RD
 *            2. Called subscribeDevicePresence API
 * @post_condition SignOut
 * @expected 'subscribeDevicePresence' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCPlatformTest_stc, SubscribeDevicePresence_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";
    ASSERT_TRUE(m_ICHelperPtr->isResourceRegistered()) << "Resource registration is unsuccessful";
    ASSERT_TRUE(!ICHelper::s_ResourceHandles.empty()) << "Resource handler is null";
    ASSERT_TRUE(ICHelper::s_IsServerResponsed) << "server is not responsed.";

    m_result = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,
            ICHelper::s_ResourceHandles, &ICHelper::onPublish, IC_OC_QUALITY_OF_SERVICE);
    ICHelper::waitForServerResponse();
    EXPECT_EQ(OC_STACK_OK, m_result) << "Resource is not published.." << CommonUtil::s_OCStackResultString.at(m_result);

    ASSERT_TRUE(ICHelper::s_IsPublishSuccessful) << "publish Callback is not invoked.";
    m_result = OC_STACK_ERROR;
    m_result = OCPlatform::findResource(m_hostAddress, m_resourceUri,
            static_cast<OCConnectivityType>(CT_ADAPTER_TCP | CT_IP_USE_V4),
            ICHelper::foundDevice, ICHelper::errorFoundDevice);
    EXPECT_EQ(OC_STACK_OK, m_result)<<"SubscribeDevicePresence got failed";
    ICHelper::waitForServerResponse();
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee));
}
#endif

/**
 * @since 2017-02-06
 * @see OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler) API
 * @see OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @see OCStackResult signOut(const std::string& accessToken, PostCallback cloudConnectHandler) API
 * @objective Test 'UnsubscribeDevicePresence' API With valid scenario
 * @target "OCStackResult unsubscribeDevicePresence(OCPresenceHandle& presenceHandle);"
 * @test_data host, presenceHandle, di, connectivityType
 * @pre_condition  constructAccountManagerObject(host, connectivity_type), SignUp(), SignIn() API
 * @procedure 1. Register and Publish Resource To RD
 *            2. Called unsubscribeDevicePresence API
 * @post_condition SignOut
 * @expected 'unsubscribeDevicePresence' API will provides OC_STACK_OK.
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICOCPlatformTest_stc, UnsubscribeDevicePresence_SRC_P)
{
    EXPECT_EQ(true, CloudCommonUtil::signIn(g_accountMgrControlee))<<"signIn filed";
    ASSERT_TRUE(m_ICHelperPtr->isResourceRegistered()) << "Resource registration is unsuccessful";
    ASSERT_TRUE(!ICHelper::s_ResourceHandles.empty()) << "Resource handler is null";
    ASSERT_TRUE(ICHelper::s_IsServerResponsed) << "server is not responsed.";

    m_result = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,
            ICHelper::s_ResourceHandles, &ICHelper::onPublish, IC_OC_QUALITY_OF_SERVICE);
    ICHelper::waitForServerResponse();
    EXPECT_EQ(OC_STACK_OK, m_result) << "Resource is not published.." << CommonUtil::s_OCStackResultString.at(m_result);

    ASSERT_TRUE(ICHelper::s_IsPublishSuccessful) << "publish Callback is not invoked.";
    m_result = OC_STACK_ERROR;
    m_result = OCPlatform::findResource(m_hostAddress, m_resourceUri,
            static_cast<OCConnectivityType>(CT_ADAPTER_TCP | CT_IP_USE_V4),
            &ICHelper::foundDevice, &ICHelper::errorFoundDevice);
    ICHelper::waitForServerResponse();
    EXPECT_EQ(OC_STACK_OK, m_result)<<"SubscribeDevicePresence got failed";
    EXPECT_EQ(true, CloudCommonUtil::signOut(g_accountMgrControlee));
}
#endif
