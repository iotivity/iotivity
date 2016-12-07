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
#include "CSCsdkCloudHelper.h"
#include "CSCsdkUtilityHelper.h"

const string COAP_HOST_ADDRESS = "coap+tcp://127.0.0.1:5683";
const string COAPS_HOST_ADDRESS = "coaps+tcp://127.0.0.1:5683";
string g_hostAddress = "coap+tcp://127.0.0.1:5683";
static string g_authProvider = "github";
static string g_authCode = "";
static string g_groupId = "";
static string g_aclId = "";
static string g_uidControlee = "";
static string g_authTokenControlee = "";
static string g_refreshAuthTokenControlee = "";
static string g_devIDcontroller = "";
static string g_authTokenController = "";
static string g_refreshAuthTokenController = "";
static OCAccountManager::Ptr g_accountMgrControlee = nullptr;
static OCAccountManager::Ptr g_accountMgrController = nullptr;
static std::vector< std::string > g_deviceIdAddded;
static std::vector< std::string > g_deviceIdRemoving;
static uint16_t g_credId = 0;
static ByteArray_t g_trustCertChainArray =
{ 0, 0 };

class CSCsdkCloudTest_btc: public ::testing::Test
{
protected:

    OCProvisionDev_t *m_OwnList, *m_UnownList, *m_motEnabledDevList;
    OicSecAcl_t *m_Acl, *m_Acl1, *m_Acl2;
    CSCsdkCloudHelper m_CloudAclHelper;

    virtual void SetUp()
    {
        CommonUtil::rmFile(CLOUD_ACL_CONTROLLER_DAT);
        CommonUtil::copyFile(CLOUD_ACL_CONTROLLER_DAT_BACKUP, CLOUD_ACL_CONTROLLER_DAT);
        CommonUtil::rmFile(ROOT_CERT_FILE);
        CommonUtil::copyFile(ROOT_CERT_FILE_BACKUP, ROOT_CERT_FILE);

        CommonUtil::runCommonTCSetUpPart();

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
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test signUp positively with regular data
 * @target          OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix wtih true param
 *                  4. call CASelectCipherSuite with MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA and CA_ADAPTER_TCP
 *                  5. call OCSaveTrustCertChain
 *                  6. call signUp
 * @post_condition  none
 * @expected        signUp will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, SignUp_P)
{
    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCSaveTrustCertChain positively with regular data
 * @target          OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix wtih true param
 *                  4. call CASelectCipherSuite with MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA and CA_ADAPTER_TCP
 *                  5. call OCSaveTrustCertChain
 * @post_condition  none
 * @expected        OCSaveTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCSaveTrustCertChain_SRC_RV_P)
{
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
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudGetCRL positively with regular data
 * @target          OCStackResult OCCloudGetCRL(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudGetCRL
 * @post_condition  none
 * @expected        OCCloudGetCRL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudGetCRL_SRC_RV_P)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudGetCRL((void*) CTX_GET_CRL, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
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
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudGetCRL negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudGetCRL(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudGetCRL
 * @post_condition  none
 * @expected        OCCloudGetCRL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudGetCRLDev_NV_N)
{

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudGetCRL((void*) CTX_GET_CRL, NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudGetCRL positively with OCCloudResponseCB as NULL
 * @target          OCStackResult OCCloudGetCRL(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCCloudResponseCB as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudGetCRL
 * @post_condition  none
 * @expected        OCCloudGetCRL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudGetCRLCb_NV_P)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudGetCRL((void*) CTX_GET_CRL, &endPoint, NULL, OC_STACK_OK, false))
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
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudCertificateIssueRequest positively with regular data
 * @target          OCStackResult OCCloudCertificateIssueRequest(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudCertificateIssueRequest
 * @post_condition  none
 * @expected        OCCloudCertificateIssueRequest will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudCertificateIssueRequest_SRC_RV_P)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudCertificateIssueRequest((void*) CTX_CERT_REQ_ISSUE, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
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
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudCertificateIssueRequest negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudCertificateIssueRequest(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudCertificateIssueRequest
 * @post_condition  none
 * @expected        OCCloudCertificateIssueRequest will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudCertificateIssueRequestDev_NV_N)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudCertificateIssueRequest((void*) CTX_CERT_REQ_ISSUE, NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudCertificateIssueRequest positively with OCCloudResponseCB as NULL
 * @target          OCStackResult OCCloudCertificateIssueRequest(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCCloudResponseCB as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudCertificateIssueRequest
 * @post_condition  none
 * @expected        OCCloudCertificateIssueRequest will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudCertificateIssueRequest_CB_NV_P)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudCertificateIssueRequest((void*) CTX_CERT_REQ_ISSUE, &endPoint, NULL, OC_STACK_OK, false))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclIdCreate positively with regular data
 * @target          OCStackResult OCCloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call OCCloudAclIdCreate
 * @post_condition  none
 * @expected        OCCloudAclIdCreate will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudAclIdCreate_SRC_RV_P)
{
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdCreate((void*) CTX_INDIVIDUAL_GET__INFO, DEFAULT_OWNER_ID, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
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
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudGetAclIdByDevice positively with regualr data
 * @target          OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regualr data
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudGetAclIdByDevice
 * @post_condition  none
 * @expected        OCCloudGetAclIdByDevice will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudAclIdGetByDevice_SRC_RV_P)
{
    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, g_aclId, OC_STACK_OK))
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
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudGetAclIdByDevice negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudGetAclIdByDevice
 * @post_condition  none
 * @expected        OCCloudGetAclIdByDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudAclIdGetByDeviceOCDevAddr_NV_N)
{
    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, NULL, CSCsdkCloudHelper::cloudResponseCB, g_aclId, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudGetAclIdByDevice positively with OCCloudResponseCB as NULL
 * @target          OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCCloudResponseCB as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudGetAclIdByDevice
 * @post_condition  none
 * @expected        OCCloudGetAclIdByDevice will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudAclIdGetByDeviceCb_NV_P)
{
    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, NULL, g_aclId, OC_STACK_OK, false))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclIndividualGetInfo positively with regular data
 * @target          OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudAclIndividualUpdateAce
 *                  6. call OCCloudAclIndividualGetInfo
 * @post_condition  none
 * @expected        OCCloudAclIndividualGetInfo will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudAclIndividualGetInfo_SRC_RV_P)
{
    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, g_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, g_aclId.c_str(), aces, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET__INFO, g_aclId.c_str(), &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclIndividualGetInfo positively with regular data
 * @target          OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudAclIndividualUpdateAce
 *                  6. call OCCloudAclIndividualGetInfo
 * @post_condition  none
 * @expected        OCCloudAclIndividualGetInfo will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudAclIndividualGetInfoOCDevAddr_NV_N)
{
    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, g_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, g_aclId.c_str(), aces, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET__INFO, g_aclId.c_str(), NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclIndividualGetInfo positively with callback as NULL
 * @target          OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       callback as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudAclIndividualGetInfo
 * @post_condition  none
 * @expected        OCCloudAclIndividualGetInfo will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudAclIndividualGetInfoCb_NV_P)
{
    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, g_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, g_aclId.c_str(), aces, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET__INFO, g_aclId.c_str(), &endPoint, NULL, OC_STACK_OK, false))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclIndividualUpdateAce positively with regular data
 * @target          OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudAclIndividualUpdateAce
 * @post_condition  none
 * @expected        OCCloudAclIndividualUpdateAce will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudAclIndividualUpdateAce_SRC_RV_P)
{
    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, g_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, g_aclId.c_str(), aces, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclIndividualUpdateAce negatively with endPoint as NULL
 * @target          OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       endPoint as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudAclIndividualUpdateAce
 * @post_condition  none
 * @expected        OCCloudAclIndividualUpdateAce will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudAclIndividualUpdateAceOCDevAddr_NV_N)
{
    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, g_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, g_aclId.c_str(), aces, NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclIndividualUpdateAce positively with callback as NULL
 * @target          OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       callback as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudAclIndividualUpdateAce
 * @post_condition  none
 * @expected        OCCloudAclIndividualUpdateAce will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudAclIndividualUpdateAceCb_NV_P)
{
    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, g_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, g_aclId.c_str(), aces, &endPoint, NULL, OC_STACK_OK, false))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclIndividualUpdateAce negatively with cloudAce_t as NULL
 * @target          OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       cloudAce_t as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudAclIndividualUpdateAce
 * @post_condition  none
 * @expected        OCCloudAclIndividualUpdateAce will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(CSCsdkCloudTest_btc, OCCloudAclIndividualUpdateAceCloudAce_t_NV_N)
{
    if (!m_CloudAclHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
            CT_ADAPTER_TCP);

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, CSCsdkCloudHelper::handleLoginoutCB, g_uidControlee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_uidControlee, g_authTokenControlee, CSCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, g_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, g_aclId.c_str(), NULL, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif
