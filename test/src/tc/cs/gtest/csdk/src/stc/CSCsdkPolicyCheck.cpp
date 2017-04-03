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

class CSCsdkPolicyCheckTest_stc: public ::testing::Test
{
protected:

    CSCsdkCloudHelper m_CloudAclHelper;
    string m_hostAddress = COAP_HOST_ADDRESS;
    OCAccountManager::Ptr m_accountMgrControlee = nullptr;
    uint16_t m_credId = 0;
    ByteArray_t m_trustCertChainArray =
    { 0, 0 };

    OCDevAddr m_endPoint = {0, 0};
    cloudAce_t *m_aces = NULL;

    virtual void SetUp()
    {
        CommonUtil::copyFile(CLOUD_ACL_CONTROLLER_DAT_BACKUP,
        CLOUD_ACL_CONTROLLER_DAT);

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

        CloudCommonUtil::signUp(m_accountMgrControlee);

        if (!CloudCommonUtil::signIn(m_accountMgrControlee))
        {
            SET_FAILURE(ERROR_SIGN_IN);
            return;
        }

        if (!m_CloudAclHelper.cloudGetAclIdByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &m_endPoint, CSCsdkCloudHelper::aclResponseCB, CSCsdkCloudHelper::s_aclId, OC_STACK_OK))
        {
            SET_FAILURE(m_CloudAclHelper.getFailureMessage());
            return;
        }

        if (!m_CloudAclHelper.cloudAclIndividualAclUpdate((void*) CTX_INDIVIDUAL_UPDATE_ACE, CSCsdkCloudHelper::s_aclId.c_str(), m_aces, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
        {
            SET_FAILURE(m_CloudAclHelper.getFailureMessage());
            return;
        }

        if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_aclId.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
        {
            SET_FAILURE(m_CloudAclHelper.getFailureMessage());
            return;
        }
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclPolicyCheck positively with regular data
 * @target          OCStackResult OCCloudAclPolicyCheck(void* ctx, const char *subjectId, const char *deviceId, const char *method, const char *uri, const OCDevAddr *endPoint, OCCloudResponseCB callback);
 * @test_data       method = "GET"
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIndividualUpdateAce
 *                  6. call OCCloudAclIndividualGetInfo
 *                  7. call OCCloudAclPolicyCheck
 * @post_condition  none
 * @expected        OCCloudAclPolicyCheck will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkPolicyCheckTest_stc, OCCloudAclPolicyCheckGetAllowed_SRC_RV_P)
{
    if (!m_CloudAclHelper.cloudAclPolicyCheck((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_subjectuuid.c_str(),
            CSCsdkCloudHelper::s_deviceId.c_str(), m_CloudAclHelper.GET_REQUEST.c_str(),
            CSCsdkCloudHelper::s_href.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclPolicyCheck positively with regular data
 * @target          OCStackResult OCCloudAclPolicyCheck(void* ctx, const char *subjectId, const char *deviceId, const char *method, const char *uri, const OCDevAddr *endPoint, OCCloudResponseCB callback);
 * @test_data       method = "PUT"
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIndividualUpdateAce
 *                  6. call OCCloudAclIndividualGetInfo
 *                  7. call OCCloudAclPolicyCheck
 * @post_condition  none
 * @expected        OCCloudAclPolicyCheck will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkPolicyCheckTest_stc, OCCloudAclPolicyCheckPutAllowed_SRC_RV_P)
{
    if (!m_CloudAclHelper.cloudAclPolicyCheck((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_subjectuuid.c_str(),
            CSCsdkCloudHelper::s_deviceId.c_str(), m_CloudAclHelper.PUT_REQUEST.c_str(),
            CSCsdkCloudHelper::s_href.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclPolicyCheck positively with regular data
 * @target          OCStackResult OCCloudAclPolicyCheck(void* ctx, const char *subjectId, const char *deviceId, const char *method, const char *uri, const OCDevAddr *endPoint, OCCloudResponseCB callback);
 * @test_data       method = "POST"
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIndividualUpdateAce
 *                  6. call OCCloudAclIndividualGetInfo
 *                  7. call OCCloudAclPolicyCheck
 * @post_condition  none
 * @expected        OCCloudAclPolicyCheck will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkPolicyCheckTest_stc, OCCloudAclPolicyCheckPostAllowed_SRC_RV_P)
{
    if (!m_CloudAclHelper.cloudAclPolicyCheck((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_subjectuuid.c_str(),
            CSCsdkCloudHelper::s_deviceId.c_str(), m_CloudAclHelper.POST_REQUEST.c_str(),
            CSCsdkCloudHelper::s_href.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclPolicyCheck positively with regular data
 * @target          OCStackResult OCCloudAclPolicyCheck(void* ctx, const char *subjectId, const char *deviceId, const char *method, const char *uri, const OCDevAddr *endPoint, OCCloudResponseCB callback);
 * @test_data       method = "DELETE"
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIndividualUpdateAce
 *                  6. call OCCloudAclIndividualGetInfo
 *                  7. call OCCloudAclPolicyCheck
 * @post_condition  none
 * @expected        OCCloudAclPolicyCheck will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkPolicyCheckTest_stc, OCCloudAclPolicyCheckDeleteAllowed_SRC_RV_P)
{
    if (!m_CloudAclHelper.cloudAclPolicyCheck((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_subjectuuid.c_str(),
            CSCsdkCloudHelper::s_deviceId.c_str(), m_CloudAclHelper.DELETE_REQUEST.c_str(),
            CSCsdkCloudHelper::s_href.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif
