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
 *
 ******************************************************************/
#include "CSCsdkCloudHelper.h"
#include "CSCsdkUtilityHelper.h"

class CSCsdkGetInvitationTest_stc: public ::testing::Test
{
protected:

    CSCsdkCloudHelper m_CloudAclHelper;
    string m_hostAddress = COAP_HOST_ADDRESS;
    string m_authProvider = "github";
    string m_authCode = "";
    string m_groupId = "";
    string m_aclId = "";
    string m_uidControlee = "";
    string m_authTokenControlee = "";

    OCAccountManager::Ptr m_accountMgrControlee = nullptr;

    std::vector< std::string > m_deviceIdAddded;
    std::vector< std::string > m_deviceIdRemoving;
    uint16_t m_credId = 0;
    ByteArray_t m_trustCertChainArray =
    { 0, 0 };

    OCDevAddr m_endPoint = {0, 0};
    cloudAce_t *m_aces = NULL;

    virtual void SetUp()
    {
        CommonUtil::copyFile(CLOUD_ACL_CONTROLLER_DAT_BACKUP, CLOUD_ACL_CONTROLLER_DAT);
        CommonUtil::copyFile(ROOT_CERT_FILE_BACKUP, ROOT_CERT_FILE);
        CommonTestUtil::runCommonTCSetUpPart();

        if (!m_CloudAclHelper.initCloudACLClient())
        {
            SET_FAILURE(m_CloudAclHelper.getFailureMessage());
            return;
        }

        m_hostAddress = CloudCommonUtil::getDefaultHostAddess();
        m_endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();
        m_aces = CSCsdkUtilityHelper::createCloudAces();

#ifdef __TLS_ON__
        setCoapPrefix(true);
        CASelectCipherSuite(MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256, CA_ADAPTER_TCP);
#endif

        CSCsdkCloudHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &m_trustCertChainArray);
        m_accountMgrControlee = OCPlatform::constructAccountManagerObject(m_hostAddress,
                CT_ADAPTER_TCP);

        if (!m_CloudAclHelper.saveTrustCertChain(m_trustCertChainArray.data, m_trustCertChainArray.len,
                        OIC_ENCODING_PEM, &m_credId, OC_STACK_OK))
        {
            SET_FAILURE(m_CloudAclHelper.getFailureMessage());
            return;
        }

        if (!CloudCommonUtil::signIn(m_accountMgrControlee))
        {
            SET_FAILURE(ERROR_SIGN_IN);
            return;
        }
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since           2017-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see            OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclGetInvitation positively with regular data
 * @target          OCStackResult OCCloudAclGetInvitation(void* ctx, const char *userId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix wtih true param
 *                  4. call CASelectCipherSuite with MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA and CA_ADAPTER_TCP
 *                  5. call signIn
 *                  6. call OCCloudAclGetInvitation
 * @post_condition  none
 * @expected        OCCloudAclGetInvitation will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGetInvitationTest_stc, OCCloudAclGetInvitation_SRC_P)
{
    if (!m_CloudAclHelper.cloudAclGetInvitation((void*)CTX_GET_GROUP_INVITATION, NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclGetInvitation(void* ctx, const char *userId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclGetInvitation positively with regular data
 * @target          OCStackResult OCCloudAclDeleteInvitation(void* ctx, const char *userId, const char *groupId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix wtih true param
 *                  4. call CASelectCipherSuite with MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA and CA_ADAPTER_TCP
 *                  5. call signIn
 *                  6. call OCCloudAclGetInvitation
 *                  7. call OCCloudAclDeleteInvitation
 * @post_condition  none
 * @expected        OCCloudAclDeleteInvitation will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGetInvitationTest_stc, OCCloudAclDeleteInvitation_SRC_P)
{
    if (!m_CloudAclHelper.cloudAclGetInvitation((void*)CTX_GET_GROUP_INVITATION, NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclDeleteInvitation((void*)CTX_DELETE_INVITATION, NULL, m_groupId.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclGetInvitation(void* ctx, const char *userId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclJoinToInvitedGroup positively with regular data
 * @target          OCStackResult OCCloudAclJoinToInvitedGroup(void* ctx, const char *groupId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix wtih true param
 *                  4. call CASelectCipherSuite with MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA and CA_ADAPTER_TCP
 *                  5. call signIn
 *                  6. call OCCloudAclGetInvitation
 *                  7. call OCCloudAclJoinToInvitedGroup
 * @post_condition  none
 * @expected        OCCloudAclJoinToInvitedGroup will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGetInvitationTest_stc, OCCloudAclJoinToInvitedGroup_SRC_P)
{
    if (!m_CloudAclHelper.cloudAclGetInvitation((void*)CTX_GET_GROUP_INVITATION, NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclJoinToInvitedGroup((void*)CTX_JOIN_GROUP, m_groupId.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclGetInvitation(void* ctx, const char *userId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclJoinToInvitedGroup(void* ctx, const char *groupId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclObserveGroup positively with regular data
 * @target          OCStackResult OCCloudAclObserveGroup(void* ctx,const char *groupId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix wtih true param
 *                  4. call CASelectCipherSuite with MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA and CA_ADAPTER_TCP
 *                  5. call signIn
 *                  6. call OCCloudAclGetInvitation
 *                  7. call OCCloudAclJoinToInvitedGroup
 *                  8. call OCCloudAclObserveGroup
 * @post_condition  none
 * @expected        OCCloudAclObserveGroup will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGetInvitationTest_stc, OCCloudAclObserveGroup_SRC_P)
{
    if (!m_CloudAclHelper.cloudAclGetInvitation((void*)CTX_GET_GROUP_INVITATION, NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclJoinToInvitedGroup((void*)CTX_JOIN_GROUP, m_groupId.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclObserveGroup((void*)CTX_OBSERVER_GROUP, m_groupId.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif
