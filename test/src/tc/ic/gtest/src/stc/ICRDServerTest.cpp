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
#include "ICHelper.h"

using namespace OC;
using namespace std;

class ICRDServerTest_stc: public ::testing::Test
{
public:
	RDClient m_rdClient;
    ResourceHandles m_resourceHandles;
    OCPresenceHandle m_ocPresenceHandle = nullptr;
    QueryParamsList m_queryParams =
    { };
    const std::vector< std::string > m_vecDeviceIdentifier;
    bool m_isCallbackInvoked;
    OCStackResult m_actualResult;

    OCAccountManager::Ptr m_accountManager;
    bool flag = true;
    ICHelper* m_ICHelper;

protected:
    ICRDServerTest_stc()
    {
        m_isCallbackInvoked = false;
        m_ICHelper = nullptr;
        m_actualResult = OC_STACK_ERROR;
    }

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_ICHelper = ICHelper::getInstance();
        m_rdClient = RDClient::Instance();
        m_actualResult = OC_STACK_ERROR;
        m_isCallbackInvoked = false;

        if (!ICHelper::isSignedUp)
        {
            ICHelper::isSignedUp = true;

            OCPersistentStorage ps
            { ICHelper::readResourceDATFile, fread, fwrite, fclose, unlink };

            PlatformConfig cfg
            { ServiceType::InProc, ModeType::Both, "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
                    0, // Uses randomly available port
                    IC_OC_QUALITY_OF_SERVICE, &ps};

            OCPlatform::Configure(cfg);

            cout << "Platform configured successfully..." << endl;

            string host = "coap+tcp://";
            host += IC_CLOUD_INTERFACE_HOST_ADDRESS;

            m_accountManager = OCPlatform::constructAccountManagerObject(IC_HOST_ADDRESS,
                    CT_ADAPTER_TCP);
            cout << "OCPlatform::constructAccountManagerObject successull..." << endl;

			char* authCode = "";

			authCode = m_ICHelper->getGitLoginAuthCodeMain();

            m_actualResult = m_accountManager->signUp(AUTH_PROVIDER, authCode, onCloudConnect);

            ICHelper::waitForServerResponse();

            EXPECT_EQ(OC_STACK_OK, m_actualResult) << "Actual result string : "
                    << CommonUtil::s_OCStackResultString.at(m_actualResult);

            cout << "ACCESS_TOKEN: " << ICHelper::ACCESS_TOKEN << endl;
            cout << "UID : " << ICHelper::UID << endl;
        }
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        flag = false;
    }

public:
    static void onPublish(const OCRepresentation& rep, const int& eCode)
    {
        ICHelper::isServerResponsed = true;
        if (eCode == 4)
        {
			cout << "onPublish callback is invoked with received code : " << eCode << endl;
		}

    }

    static void onDelete(const int& eCode)
    {
        ICHelper::isServerResponsed = true;
        cout << "onDelete callback is invoked with received code : " << eCode << endl;
    }

    static void onCloudConnect(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
    {
        cout << "onCloudConnect callback is invoked with received code : " << ecode << endl;

        ICHelper::isServerResponsed = true;

        if (rep.getPayload() != NULL)
        {
            ICHelper::icPrintRepresentation(rep);
        }

        if (ecode == 4)
        {
            ICHelper::ACCESS_TOKEN = rep.getValueToString("accesstoken");

            ICHelper::UID = rep.getValueToString("uid");
        }
    }
};

/*
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' positively to Publish Virtual Resource to RD
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                             PublishResourceCallback callback)
 * @test_data       1. host                 Host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will Publish Virtual Resource to Resource Directory & return OC_STACK_OK
 */

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDServerTest_stc, PublishResourcesToRD_SRC_FSV_P)
{
    try
    {
        string host = "coap+tcp://";
        host += IC_CLOUD_INTERFACE_HOST_ADDRESS;

        m_actualResult = OC_STACK_ERROR;

        m_actualResult = m_accountManager->signIn(ICHelper::UID, ICHelper::ACCESS_TOKEN,onCloudConnect);

        ASSERT_EQ(OC_STACK_OK, m_actualResult) << "SignIn API does not sign-in to account server!";

        ASSERT_TRUE(m_ICHelper->isResourceRegistered()) << "Resource registration is unsuccessful";
        ASSERT_TRUE(!ICHelper::g_ResourceHandles.empty()) << "Resource handler is null";

        ICHelper::isServerResponsed = false;
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,
                ICHelper::g_ResourceHandles,&onPublish,IC_OC_QUALITY_OF_SERVICE);

        ICHelper::waitForServerResponse();

        EXPECT_EQ(OC_STACK_OK, m_actualResult) << "Resource is not published.." << CommonUtil::s_OCStackResultString.at(m_actualResult);
        ASSERT_TRUE(ICHelper::isServerResponsed) << "Callback is not invoked.";

    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }

    SUCCEED();
}
#endif

/*
 * @since           2016-08-24
 * @see             none
 * @objective       Test 'publishResourceToRD' positively to Publish Virtual Resource to RD
 * @target          publishResourceToRD(const std::string& host,OCConnectivityType connectivityType,
 *                             PublishResourceCallback callback)
 * @test_data       1. host                 Host address
 *                  2. connectivityType     CT_DEFAULT as connectivity
 *                  3. callback             onPublish as callback
 * @pre_condition   none
 * @procedure       1. Call publishResourceToRD API
 *                  2. Check it's return value
 * @post_condition  none
 * @expected        It will Publish Virtual Resource to Resource Directory & return OC_STACK_OK
 */

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ICRDServerTest_stc, DeleteResourcesToRD_SRC_FSV_P)
{
    try
    {
        string host = "coap+tcp://";
        host += IC_ACCOUNT_MANAGER_HOST_ADDRESS;

        m_actualResult = OC_STACK_ERROR;
        m_actualResult = m_accountManager->signIn(ICHelper::UID, ICHelper::ACCESS_TOKEN,onCloudConnect);

        ASSERT_EQ(OC_STACK_OK, m_actualResult) << "SignIn API does not sign-in to account server!";

        ASSERT_TRUE(m_ICHelper->isResourceRegistered()) << "Resource registration is unsuccessful";
        ASSERT_TRUE(!ICHelper::g_ResourceHandles.empty()) << "Resource handler is null";

        ICHelper::isServerResponsed = false;
        m_actualResult = OC_STACK_ERROR;
        m_actualResult = m_rdClient.publishResourceToRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,
                ICHelper::g_ResourceHandles,&onPublish,IC_OC_QUALITY_OF_SERVICE);

        ICHelper::waitForServerResponse();

        ASSERT_EQ(OC_STACK_OK, m_actualResult) << "Resource is not published.." << CommonUtil::s_OCStackResultString.at(m_actualResult);
        ASSERT_TRUE(ICHelper::isServerResponsed) << "onPublish Callback is not invoked.";

        ICHelper::isServerResponsed = false;
        m_actualResult = OC_STACK_ERROR;
        m_actualResult = m_rdClient.deleteResourceFromRD(IC_HOST_ADDRESS, IC_CONNECTIVITY_TYPE,ICHelper::g_ResourceHandles,
                &onDelete,IC_OC_QUALITY_OF_SERVICE);

        ICHelper::waitForServerResponse();

        ASSERT_EQ(OC_STACK_OK,m_actualResult) << "Delete resource operation is unsuccessful. Actual result string : " << CommonUtil::s_OCStackResultString.at(m_actualResult);
        ASSERT_TRUE(ICHelper::isServerResponsed) << "onDelete Callback is not invoked.";
    }
    catch(OCException ex)
    {
        FAIL() << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
    }

    SUCCEED();
}
#endif
