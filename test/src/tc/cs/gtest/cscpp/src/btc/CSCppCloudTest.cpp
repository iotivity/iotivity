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
#include "CSCppCloudHelper.h"
#include "CSCppUtilityHelper.h"

const string COAP_HOST_ADDRESS = "coap+tcp://127.0.0.1:5683";
const string COAPS_HOST_ADDRESS = "coaps+tcp://127.0.0.1:5683";
string g_hostAddress = "coap+tcp://127.0.0.1:5683";
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
OCPlatform::OCPresenceHandle g_presenceHandle = nullptr;
std::vector< std::string > g_deviceIdAddded;
std::vector< std::string > g_deviceIdRemoving;
uint16_t g_credId = 0;
static ByteArray_t g_trustCertChainArray =
{ 0, 0 };

class CSCppCloudTest_btc: public ::testing::Test
{
protected:

    CSCppCloudHelper m_CloudAclHelper;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        CommonUtil::rmFile(CLOUD_ACL_CONTROLLER_DAT);
        CommonUtil::copyFile(CLOUD_ACL_CONTROLLER_DAT_BACKUP, CLOUD_ACL_CONTROLLER_DAT);

        if (!m_CloudAclHelper.initCloudACLClient())
        {
            SET_FAILURE(m_CloudAclHelper.getFailureMessage());
            return;
        }

#ifdef __TLS_ON__
        g_hostAddress = COAPS_HOST_ADDRESS;
        setCoapPrefix(true);
        CASelectCipherSuite(MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA, CA_ADAPTER_TCP);
#endif
        g_trustCertChainArray =
        {   0, 0};
        readFile(ROOT_CERT_FILE, (OCByteString *) &g_trustCertChainArray);
        IOTIVITYTEST_LOG(INFO, "ADDRESS : %s", g_hostAddress.c_str());
        IOTIVITYTEST_LOG(INFO, "Provide Auth Code: ");
        cin >> g_authCode;
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test saveTrustCertChain positively with regular data
 * @target          static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix wtih true param
 *                  4. call CASelectCipherSuite with MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA and CA_ADAPTER_TCP
 *                  5. call saveTrustCertChain
 * @post_condition  none
 * @expected        saveTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCppCloudTest_btc, SaveTrustCertChain_SRC_RV_P)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test requestCertificate positively with regular data
 * @target          OCStackResult requestCertificate(ResponseCallBack callback)
 * @test_data       regular data
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  3. call saveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call requestCertificate
 * @post_condition  none
 * @expected        requestCertificate will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCppCloudTest_btc, RequestCertificate_SRC_RV_P)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, CSCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);

    if (!m_CloudAclHelper.requestCertificate(cloudProv, CSCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test requestCertificate negatively with ResponseCallBack as NULL
 * @target          OCStackResult requestCertificate(ResponseCallBack callback)
 * @test_data       ResponseCallBack as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  3. call saveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call requestCertificate
 * @post_condition  none
 * @expected        requestCertificate will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(CSCppCloudTest_btc, RequestCertificateCb_NV_N)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, CSCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
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
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult requestCertificate(ResponseCallBack callback)
 * @objective       Test getCRL positively with regular data
 * @target          OCStackResult getCRL(ResponseCallBack callback)
 * @test_data       regular data
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  5. call saveTrustCertChain
 *                  6. call signUp
 *                  7. call signIn
 *                  8. call requestCertificate
 *                  9. call getCRL
 * @post_condition  none
 * @expected        getCRL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCppCloudTest_btc, GetCRL_SRC_RV_P)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, CSCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);

    if (!m_CloudAclHelper.requestCertificate(cloudProv, CSCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.getCRL(cloudProv, CSCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult requestCertificate(ResponseCallBack callback)
 * @objective       Test getCRL negatively with ResponseCallBack as NULL
 * @target          OCStackResult getCRL(ResponseCallBack callback)
 * @test_data       ResponseCallBack as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  5. call saveTrustCertChain
 *                  6. call signUp
 *                  7. call signIn
 *                  8. call requestCertificate
 *                  9. call getCRL
 * @post_condition  none
 * @expected        getCRL will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(CSCppCloudTest_btc, GetCRLCb_NV_N)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, CSCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
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
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult requestCertificate(ResponseCallBack callback)
 * @see             OCStackResult getCRL(ResponseCallBack callback)
 * @objective       Test postCRL positively with regular data
 * @target          OCStackResult postCRL(const std::string& thisUpdate, const std::string& nextUpdate, const OCByteString *crl, const stringArray_t *serialNumbers, ResponseCallBack callback)
 * @test_data       Regular data
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  5. call saveTrustCertChain
 *                  6. call signUp
 *                  7. call signIn
 *                  8. call requestCertificate
 *                  9. call getCRL
 *                  10. call postCRL
 * @post_condition  none
 * @expected        postCRL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCppCloudTest_btc, PostCRL_SRC_RV_P)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, CSCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);

    if (!m_CloudAclHelper.requestCertificate(cloudProv, CSCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.getCRL(cloudProv, CSCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
    OCByteString crlData =
    {   0, 0};

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

    if (!m_CloudAclHelper.postCRL(cloudProv, CRL_THIS_UPDATE, CRL_NEXT_DATE, crl, rcsn, CSCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test getAclIdByDevice positively with regular data
 * @target          OCStackResult getAclIdByDevice(const std::string& deviceId, AclIdResponseCallBack callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  5. call saveTrustCertChain
 *                  6. call signUp
 *                  7. call signIn
 *                  7. call getAclIdByDevice
 * @post_condition  none
 * @expected        getAclIdByDevice will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCppCloudTest_btc, GetAclIdByDevice_SRC_RV_P)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, CSCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);
    const std::string deviceID = DEFAULT_DEV_ID;
    std::string aclID = "";

    if (!m_CloudAclHelper.getAclIdByDevice(cloudProv, deviceID, CSCppCloudHelper::cloudAclResponseCB, aclID, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test getAclIdByDevice negatively with AclIdResponseCallBack as NULL
 * @target          OCStackResult getAclIdByDevice(const std::string& deviceId, AclIdResponseCallBack callback)
 * @test_data       AclIdResponseCallBack as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  5. call saveTrustCertChain
 *                  6. call signUp
 *                  7. call signIn
 *                  7. call getAclIdByDevice
 * @post_condition  none
 * @expected        getAclIdByDevice will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(CSCppCloudTest_btc, GetAclIdByDeviceCb_NV_N)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, CSCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
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
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult getAclIdByDevice(const std::string& deviceId, AclIdResponseCallBack callback)
 * @objective       Test getIndividualAclInfo positively with regular data
 * @target          OCStackResult getIndividualAclInfo(const std::string& aclId, ResponseCallBack callback)
 * @test_data       Regular data
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  5. call saveTrustCertChain
 *                  6. call signUp
 *                  7. call signIn
 *                  8. call getAclIdByDevice
 *                  9. call getIndividualAclInfo
 * @post_condition  none
 * @expected        getIndividualAclInfo will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCppCloudTest_btc, GetIndividualAclInfo_RV_SRC_P)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, CSCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);

    const std::string deviceID = DEFAULT_DEV_ID;
    std::string aclID = "";
    if (!m_CloudAclHelper.getAclIdByDevice(cloudProv, deviceID, CSCppCloudHelper::cloudAclResponseCB, aclID, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.getIndividualAclInfo(cloudProv, aclID, CSCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult getAclIdByDevice(const std::string& deviceId, AclIdResponseCallBack callback)
 * @objective       Test getIndividualAclInfo negatively with ResponseCallBack as NULL
 * @target          OCStackResult getIndividualAclInfo(const std::string& aclId, ResponseCallBack callback)
 * @test_data       ResponseCallBack as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  5. call saveTrustCertChain
 *                  6. call signUp
 *                  7. call signIn
 *                  8. call getAclIdByDevice
 *                  9. call getIndividualAclInfo
 * @post_condition  none
 * @expected        getIndividualAclInfo will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(CSCppCloudTest_btc, GetIndividualAclInfoCb_NV_N)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCppCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, CSCppCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);

    if (!m_CloudAclHelper.requestCertificate(cloudProv, CSCppCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    const std::string deviceID = DEFAULT_DEV_ID;
    std::string aclID = "";
    if (!m_CloudAclHelper.getAclIdByDevice(cloudProv, deviceID, CSCppCloudHelper::cloudAclResponseCB, aclID, OC_STACK_OK))
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
