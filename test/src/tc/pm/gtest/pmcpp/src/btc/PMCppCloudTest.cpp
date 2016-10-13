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
#if defined(__CLOUD__)
#include "PMCppCloudHelper.h"
#include "PMCppUtilityHelper.h"
#include "OCCloudProvisioning.h"

string g_authCode = "";
string g_groupId = "";
int g_userInput = -1;
string g_devIDcontrolee = "";
string g_authTokenControlee = "";
string g_refreshAuthTokenControlee = "";
string g_devIDcontroller = "";
string g_authTokenController = "";
string g_refreshAuthTokenController = "";
OCAccountManager::Ptr g_accountMgrControlee = nullptr;
OCAccountManager::Ptr g_accountMgrController = nullptr;
OCPlatform::OCPresenceHandle g_presenceHandle = nullptr;
std::vector< std::string > g_deviceIdAddded;
std::vector< std::string > g_deviceIdRemoving;
uint16_t g_credId = 0;

class PMCppCloudTest_btc: public ::testing::Test
{
protected:

    PMCppCloudHelper m_CloudAclHelper;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        CommonUtil::rmFile(CLOUD_ACL_CONTROLLER_DAT);
        CommonUtil::copyFile(CLOUD_ACL_CONTROLLER_DAT_BACKUP, CLOUD_ACL_CONTROLLER_DAT);
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test saveTrustCertChain positively with regular data
 * @target          static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId);
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call saveTrustCertChain
 * @post_condition  none
 * @expected        saveTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppCloudTest_btc, SaveTrustCertChain_SRC_RV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    ByteArray trustCertChainArray = PMCppCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test saveTrustCertChain positively with regular data
 * @target          OCStackResult requestCertificate(ResponseCallBack callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  3. call saveTrustCertChain
 *                  4. call requestCertificate
 * @post_condition  none
 * @expected        requestCertificate will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppCloudTest_btc, RequestCertificate_SRC_RV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCppCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    IOTIVITYTEST_LOG(INFO, "Provide Auth Code: ");
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);

    if (!m_CloudAclHelper.requestCertificate(cloudProv, PMCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test saveTrustCertChain negatively with callback as NULL
 * @target          OCStackResult requestCertificate(ResponseCallBack callback)
 * @test_data       callback as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  3. call saveTrustCertChain
 *                  4. call requestCertificate
 * @post_condition  none
 * @expected        requestCertificate will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCppCloudTest_btc, RequestCertificateCb_NV_N)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCppCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    IOTIVITYTEST_LOG(INFO, "Provide Auth Code: ");
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);

    if (!m_CloudAclHelper.requestCertificate(cloudProv, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult requestCertificate(ResponseCallBack callback)
 * @objective       Test getCRL postively with regular data
 * @target          OCStackResult getCRL(ResponseCallBack callback)
 * @test_data       regular data
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call saveTrustCertChain
 *                  6. call requestCertificate
 *                  7. call getCRL
 * @post_condition  none
 * @expected        getCRL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppCloudTest_btc, GetCRL_SRC_RV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCppCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    IOTIVITYTEST_LOG(INFO, "Provide Auth Code: ");
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);

    if (!m_CloudAclHelper.requestCertificate(cloudProv, PMCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.getCRL(cloudProv, PMCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult requestCertificate(ResponseCallBack callback)
 * @objective       Test getCRL negatively with callback as null
 * @target          OCStackResult getCRL(ResponseCallBack callback)
 * @test_data       callback as null
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call saveTrustCertChain
 *                  6. call requestCertificate
 *                  7. call getCRL
 * @post_condition  none
 * @expected        getCRL will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCppCloudTest_btc, GetCRLCb_NV_N)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCppCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    IOTIVITYTEST_LOG(INFO, "Provide Auth Code: ");
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);

    if (!m_CloudAclHelper.getCRL(cloudProv, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult requestCertificate(ResponseCallBack callback)
 * @objective       Test getCRL postively with regular data
 * @target          OCStackResult getCRL(ResponseCallBack callback)
 * @test_data       regular data
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call saveTrustCertChain
 *                  6. call requestCertificate
 *                  7. call getCRL
 * @post_condition  none
 * @expected        getCRL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppCloudTest_btc, PostCRL_SRC_RV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCppCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    IOTIVITYTEST_LOG(INFO, "Provide Auth Code: ");
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);

    if (!m_CloudAclHelper.requestCertificate(cloudProv, PMCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.getCRL(cloudProv, PMCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
    OCByteString crlData =
    {   0, 0};
    char thisUpdate[16] =
    {   0};
    char nextUpdate[16] =
    {   0};
    stringArray_t serialNumbers =
    {   0, 0};
    stringArray_t *rcsn = 0;
    OCByteString *crl = 0;
    int cnt = 1;
    int len = 1;

    char **item = (char**)OICCalloc(cnt, sizeof(char*));

    for (int i = 0; i < cnt; i++)
    {
        item[i] = (char*)OICCalloc(len, sizeof(char));
        strcpy(item[i], "1");
    }

    serialNumbers.array = item;
    serialNumbers.length = (size_t)cnt;

    rcsn = serialNumbers.array? &serialNumbers : NULL;
    crl = crlData.bytes? &crlData : NULL;

    if (!m_CloudAclHelper.postCRL(cloudProv, CRL_THIS_UPDATE, CRL_NEXT_DATE, crl, rcsn, PMCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult requestCertificate(ResponseCallBack callback)
 * @objective       Test getAclIdByDevice positively with regular data
 * @target          OCStackResult getAclIdByDevice(const std::string& deviceId, AclIdResponseCallBack callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call saveTrustCertChain
 *                  6. call requestCertificate
 *                  7. call getAclIdByDevice
 * @post_condition  none
 * @expected        getAclIdByDevice will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppCloudTest_btc, GetAclIdByDevice_SRC_RV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCppCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    IOTIVITYTEST_LOG(INFO, "Provide Auth Code: ");
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);
    const std::string deviceID = DEFAULT_DEV_ID;
    std::string aclID = "";

    if (!m_CloudAclHelper.getAclIdByDevice(cloudProv, deviceID, PMCppCloudHelper::cloudAclResponseCB, aclID, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult requestCertificate(ResponseCallBack callback)
 * @objective       Test getAclIdByDevice negatively with callback as null
 * @target          OCStackResult getAclIdByDevice(const std::string& deviceId, AclIdResponseCallBack callback)
 * @test_data       callback as null
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call saveTrustCertChain
 *                  6. call requestCertificate
 *                  7. call getAclIdByDevice
 * @post_condition  none
 * @expected        getAclIdByDevice will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCppCloudTest_btc, GetAclIdByDeviceCb_NV_N)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCppCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    IOTIVITYTEST_LOG(INFO, "Provide Auth Code: ");
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);
    const std::string deviceID = DEFAULT_DEV_ID;
    std::string aclID = "";

    if (!m_CloudAclHelper.getAclIdByDevice(cloudProv, deviceID, NULL, aclID, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult requestCertificate(ResponseCallBack callback)
 * @see             OCStackResult getAclIdByDevice(const std::string& deviceId, AclIdResponseCallBack callback)
 * @objective       Test getAclIdByDevice positively with regular data
 * @target          OCStackResult getIndividualAclInfo(const std::string& aclId, ResponseCallBack callback)
 * @test_data       deviceId as null
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call saveTrustCertChain
 *                  6. call requestCertificate
 *                  7. call getAclIdByDevice
 *                  8. call getIndividualAclInfo
 * @post_condition  none
 * @expected        getIndividualAclInfo will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppCloudTest_btc, GetIndividualAclInfo_RV_SRC_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCppCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    IOTIVITYTEST_LOG(INFO, "Provide Auth Code: ");
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);

    if (!m_CloudAclHelper.requestCertificate(cloudProv, PMCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    const std::string deviceID = DEFAULT_DEV_ID;
    std::string aclID = "";
    if (!m_CloudAclHelper.getAclIdByDevice(cloudProv, deviceID, PMCppCloudHelper::cloudAclResponseCB, aclID, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.getIndividualAclInfo(cloudProv, aclID, PMCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult requestCertificate(ResponseCallBack callback)
 * @see             OCStackResult getAclIdByDevice(const std::string& deviceId, AclIdResponseCallBack callback)
 * @objective       Test getIndividualAclInfo negatively with callback as NULL
 * @target          OCStackResult getIndividualAclInfo(const std::string& aclId, ResponseCallBack callback)
 * @test_data       callback as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call saveTrustCertChain
 *                  6. call requestCertificate
 *                  7. call getAclIdByDevice
 *                  8. call getIndividualAclInfo
 * @post_condition  none
 * @expected        getIndividualAclInfo will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCppCloudTest_btc, GetIndividualAclInfoCb_NV_N)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCppCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    IOTIVITYTEST_LOG(INFO, "Provide Auth Code: ");
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);

    if (!m_CloudAclHelper.requestCertificate(cloudProv, PMCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    const std::string deviceID = DEFAULT_DEV_ID;
    std::string aclID = "";
    if (!m_CloudAclHelper.getAclIdByDevice(cloudProv, deviceID, PMCppCloudHelper::cloudAclResponseCB, aclID, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.getIndividualAclInfo(cloudProv, aclID, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif
#endif /*__CLOUD__*/
