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

class CSCsdkAclDeleteTest_btc: public ::testing::Test
{
protected:

    CSCsdkCloudHelper m_CloudAclHelper;
    string m_hostAddress = COAP_HOST_ADDRESS;
    OCAccountManager::Ptr m_accountMgrControlee = nullptr;
    uint16_t m_credId = 0;
    ByteArray_t m_trustCertChainArray =
    { 0, 0 };
    OCDevAddr m_endPoint =
    { 0, 0 };

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
 * @since           2017-03-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIdDelete negatively with aclId as NULL
 * @target          OCStackResult OCCloudAclIdDelete(void* ctx,const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       aclId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIdDelete
 * @post_condition  none
 * @expected        OCCloudAclIdDelete will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_btc, OCCloudAclIdDeleteAclid_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIdDelete((void*) CTX_DELETE_ACL, NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIdDelete negatively with endPoint as NULL
 * @target          OCStackResult OCCloudAclIdDelete(void* ctx,const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       endPoint as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIdDelete
 * @post_condition  none
 * @expected        OCCloudAclIdDelete will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_btc, OCCloudAclIdDeleteDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIdDelete((void*) CTX_DELETE_ACL, CSCsdkCloudHelper::s_aclId.c_str(), NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclAcesDelete negatively with aclId as NULL
 * @target          OCStackResult OCCloudAclAcesDelete(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       aclId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclAcesDelete
 * @post_condition  none
 * @expected        OCCloudAclAcesDelete will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_btc, OCCloudAclAcesDeleteAclId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclAcesDelete((void*) CTX_ACES_DELETE, NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclAcesDelete negatively with aclId as NULL
 * @target          OCStackResult OCCloudAclAcesDelete(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       aclId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclAcesDelete
 * @post_condition  none
 * @expected        OCCloudAclAcesDelete will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_btc, OCCloudAclAcesDeleteDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclAcesDelete((void*) CTX_ACES_DELETE, CSCsdkCloudHelper::s_aclId.c_str(), NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIndividualAceDelete negatively with aclId as NULL
 * @target          OCStackResult OCCloudAclIndividualAceDelete(void* ctx, const char *aclId, const char *aceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       aclId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIndividualAceDelete
 * @post_condition  none
 * @expected        OCCloudAclIndividualAceDelete will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_btc, OCCloudAclIndividualAceDeleteAclId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIndividualAceDelete((void*) CTX_INDIVIDUAL_ACE_DELETE, NULL, CSCsdkCloudHelper::s_aceid.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIndividualAceDelete negatively with aceId as NULL
 * @target          OCStackResult OCCloudAclIndividualAceDelete(void* ctx, const char *aclId, const char *aceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       aceId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIndividualAceDelete
 * @post_condition  none
 * @expected        OCCloudAclIndividualAceDelete will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_btc, OCCloudAclIndividualAceDeleteAceId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIndividualAceDelete((void*) CTX_INDIVIDUAL_ACE_DELETE, CSCsdkCloudHelper::s_aclId.c_str(), NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIndividualAceDelete negatively with endPoint as NULL
 * @target          OCStackResult OCCloudAclIndividualAceDelete(void* ctx, const char *aclId, const char *aceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       endPoint as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIndividualAceDelete
 * @post_condition  none
 * @expected        OCCloudAclIndividualAceDelete will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_btc, OCCloudAclIndividualAceDeleteDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIndividualAceDelete((void*) CTX_INDIVIDUAL_ACE_DELETE, CSCsdkCloudHelper::s_aclId.c_str(), CSCsdkCloudHelper::s_aceid.c_str(), NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif
