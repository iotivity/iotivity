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

class CSCsdkCloudTest_stc: public ::testing::Test
{
protected:
    CSCsdkCloudHelper m_CloudAclHelper;
    string m_hostAddress = COAP_HOST_ADDRESS;
    string m_uidControlee = "";
    string m_authTokenControlee = "";
    OCAccountManager::Ptr m_accountMgrControlee = nullptr;
    uint16_t m_credId = 0;
    ByteArray_t m_trustCertChainArray =
    { 0, 0 };

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

#ifdef __TLS_ON__
        setCoapPrefix(true);
        CASelectCipherSuite(MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256, CA_ADAPTER_TCP);
#endif

        CSCsdkCloudHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &m_trustCertChainArray);
        m_accountMgrControlee = OCPlatform::constructAccountManagerObject(m_hostAddress,
                CT_ADAPTER_TCP);
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since           2016-10-23
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkCloudTest_stc, OCSaveTrustCertChain_SRC_RV_P)
{
    if (!m_CloudAclHelper.saveTrustCertChain(m_trustCertChainArray.data, m_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &m_credId, OC_STACK_OK))
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
 *                  5. call OCSaveTrustCertChain
 *                  6. call signIn
 *                  7. call OCCloudGetCRL
 * @post_condition  none
 * @expected        OCCloudGetCRL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkCloudTest_stc, OCCloudGetCRL_SRC_RV_P)
{
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
 *                  5. call OCSaveTrustCertChain
 *                  6. call signIn
 *                  7. call OCCloudGetCRL
 * @post_condition  none
 * @expected        OCCloudGetCRL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkCloudTest_stc, OCCloudGetCRLCb_NV_P)
{
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
 *                  5. call signIn
 *                  6. call OCCloudCertificateIssueRequest
 * @post_condition  none
 * @expected        OCCloudCertificateIssueRequest will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkCloudTest_stc, OCCloudCertificateIssueRequest_SRC_RV_P)
{
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
 *                  5. call OCSaveTrustCertChain
 *                  6. call signIn
 *                  7. call OCCloudCertificateIssueRequest
 * @post_condition  none
 * @expected        OCCloudCertificateIssueRequest will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkCloudTest_stc, OCCloudCertificateIssueRequest_CB_NV_P)
{
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

    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudCertificateIssueRequest((void*) CTX_CERT_REQ_ISSUE, &endPoint, NULL, OC_STACK_OK, false))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif
