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
#include "PMCsdkCloudHelper.h"
#include "PMCsdkUtilityHelper.h"

const string HOST_ADDRESS = "coap+tcp://127.0.0.1:5683";
string g_authProvider = "github";
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

class PMCsdkCloudTest_btc: public ::testing::Test
{
protected:

    OCProvisionDev_t *m_OwnList, *m_UnownList, *m_motEnabledDevList;
    OicSecAcl_t *m_Acl, *m_Acl1, *m_Acl2;
    PMCsdkCloudHelper m_CloudAclHelper;

    virtual void SetUp()
    {
        CommonUtil::rmFile(CLOUD_ACL_CONTROLLER_DAT);
        CommonUtil::copyFile(CLOUD_ACL_CONTROLLER_DAT_BACKUP, CLOUD_ACL_CONTROLLER_DAT);
        CommonUtil::rmFile(ROOT_CERT_FILE);
        CommonUtil::copyFile(ROOT_CERT_FILE_BACKUP, ROOT_CERT_FILE);
        CommonUtil::runCommonTCSetUpPart();
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
 * @objective       Test OCSaveTrustCertChain positively with regular data
 * @target          OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  5. call OCSaveTrustCertChain
 * @post_condition  none
 * @expected        OCSaveTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCloudTest_btc, OCSaveTrustCertChain_SRC_RV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
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
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudGetCRL positively with regular data
 * @target          OCStackResult OCCloudGetCRL(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCCloudGetCRL
 * @post_condition  none
 * @expected        OCCloudGetCRL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCloudTest_btc, OCCloudGetCRL_SRC_RV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudGetCRL negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudGetCRL(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCCloudGetCRL
 * @post_condition  none
 * @expected        OCCloudGetCRL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCloudTest_btc, OCCloudGetCRLDev_NV_N)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudGetCRL((void*) CTX_GET_CRL, NULL, PMCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudGetCRL positively with callback as NULL
 * @target          OCStackResult OCCloudGetCRL(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       callback as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCCloudGetCRL
 * @post_condition  none
 * @expected        OCCloudGetCRL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCloudTest_btc, OCCloudGetCRLCb_NV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudGetCRL((void*) CTX_GET_CRL, &endPoint, NULL, OC_STACK_OK, false))
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
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudCertificateIssueRequest positively with regular data
 * @target          OCStackResult OCCloudCertificateIssueRequest(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCCloudCertificateIssueRequest
 * @post_condition  none
 * @expected        OCCloudCertificateIssueRequest will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCloudTest_btc, OCCloudCertificateIssueRequest_SRC_RV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudCertificateIssueRequest((void*) CTX_CERT_REQ_ISSUE, &endPoint, PMCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
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
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudCertificateIssueRequest negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudCertificateIssueRequest(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCCloudCertificateIssueRequest
 * @post_condition  none
 * @expected        OCCloudCertificateIssueRequest will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCloudTest_btc, OCCloudCertificateIssueRequestDev_NV_N)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudCertificateIssueRequest((void*) CTX_CERT_REQ_ISSUE, NULL, PMCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult signUp(const std::string& authProvider, const std::string& authCode, PostCallback cloudConnectHandler)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCCloudCertificateIssueRequest positively with callback as NULL
 * @target          OCStackResult OCCloudCertificateIssueRequest(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       callback as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call signUp
 *                  4. call signIn
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCCloudCertificateIssueRequest
 * @post_condition  none
 * @expected        OCCloudCertificateIssueRequest will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCloudTest_btc, OCCloudCertificateIssueRequest_CB_NV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudCertificateIssueRequest((void*) CTX_CERT_REQ_ISSUE, &endPoint, NULL, OC_STACK_OK, false))
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
TEST_F(PMCsdkCloudTest_btc, OCCloudAclIdCreate_SRC_RV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdCreate((void*) CTX_INDIVIDUAL_GET__INFO, DEFAULT_OWNER_ID, DEFAULT_DEV_ID, &endPoint, PMCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
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
 * @objective       Test OCCloudAclIdGetByDevice positively with regular data
 * @target          OCStackResult OCCloudAclIdGetByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudAclIdGetByDevice
 * @post_condition  none
 * @expected        OCCloudAclIdGetByDevice will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCloudTest_btc, OCCloudAclIdGetByDevice_SRC_RV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, PMCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
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
 * @objective       Test OCCloudAclIdGetByDevice negatively with endPoint as NULL
 * @target          OCStackResult OCCloudAclIdGetByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       endPoint = NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudAclIdGetByDevice
 * @post_condition  none
 * @expected        OCCloudAclIdGetByDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCloudTest_btc, OCCloudAclIdGetByDeviceOCDevAddr_NV_N)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, NULL, PMCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCCloudAclIdGetByDevice positively  with callback as NULL
 * @target          OCStackResult OCCloudAclIdGetByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       callback as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudAclIdGetByDevice
 * @post_condition  none
 * @expected        OCCloudAclIdGetByDevice will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCloudTest_btc, OCCloudAclIdGetByDeviceCb_NV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, NULL, OC_STACK_OK, false))
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
 * @objective       Test OCCloudAclIndividualGetInfo positively with regular data
 * @target          OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
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
TEST_F(PMCsdkCloudTest_btc, OCCloudAclIndividualGetInfo_SRC_RV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET__INFO, DEFAULT_ACL_ID, &endPoint, PMCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
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
 * @objective       Test OCCloudAclIndividualGetInfo negatively with endPoint as NULL
 * @target          OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       endPoint as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signUp
 *                  5. call signIn
 *                  6. call OCCloudAclIndividualGetInfo
 * @post_condition  none
 * @expected        OCCloudAclIndividualGetInfo will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCloudTest_btc, OCCloudAclIndividualGetInfoOCDevAddr_NV_N)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET__INFO, DEFAULT_ACL_ID, NULL, PMCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
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
TEST_F(PMCsdkCloudTest_btc, OCCloudAclIndividualGetInfoCb_NV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET__INFO, DEFAULT_ACL_ID, &endPoint, NULL, OC_STACK_OK, false))
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
TEST_F(PMCsdkCloudTest_btc, OCCloudAclIndividualUpdateAce_SRC_RV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET__INFO, DEFAULT_ACL_ID, &endPoint, PMCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    cloudAce_t *aces = PMCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, DEFAULT_ACL_ID, aces, &endPoint, PMCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
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
TEST_F(PMCsdkCloudTest_btc, OCCloudAclIndividualUpdateAceOCDevAddr_NV_N)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    cloudAce_t *aces = PMCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, DEFAULT_ACL_ID, aces, NULL, PMCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
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
TEST_F(PMCsdkCloudTest_btc, OCCloudAclIndividualUpdateAceCb_NV_P)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    cloudAce_t *aces = PMCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, DEFAULT_ACL_ID, aces, &endPoint, NULL, OC_STACK_OK, false))
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
TEST_F(PMCsdkCloudTest_btc, OCCloudAclIndividualUpdateAceCloudAce_t_NV_N)
{
    if (!m_CloudAclHelper.initCloudACLClient())
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    ByteArray trustCertChainArray = PMCsdkCloudHelper::getTrustCertChainArray();

    if (!m_CloudAclHelper.saveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
            CT_ADAPTER_TCP);

    cout << "Provide Auth Code: ";
    cin >> g_authCode;

    if (!m_CloudAclHelper.signUp(g_accountMgrControlee, g_authProvider, g_authCode, PMCsdkCloudHelper::handleLoginoutCB, g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, PMCsdkCloudHelper::handleLoginoutCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    OCDevAddr endPoint = PMCsdkUtilityHelper::getOCDevAddrEndPoint();

    cloudAce_t *aces = PMCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, DEFAULT_ACL_ID, NULL, &endPoint, PMCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
}
#endif
#endif // __CLOUD__
