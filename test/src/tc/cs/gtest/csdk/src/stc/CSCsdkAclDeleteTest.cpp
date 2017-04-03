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

class CSCsdkAclDeleteTest_stc: public ::testing::Test
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

        if (!m_CloudAclHelper.cloudAclIdCreate((void*) CTX_CREATE_ACL, DEFAULT_OWNER_ID, DEFAULT_DEV_ID_INDIVIDUAL_ACE_DELETE, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
        {
            SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        }

        if (!m_CloudAclHelper.cloudGetAclIdByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID_INDIVIDUAL_ACE_DELETE, &m_endPoint, CSCsdkCloudHelper::aclResponseCB, CSCsdkCloudHelper::s_aclId, OC_STACK_OK))
        {
            SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        }

        if (!m_CloudAclHelper.cloudAclIndividualAclUpdate((void*) CTX_INDIVIDUAL_ACL_UPDATE, CSCsdkCloudHelper::s_aclId.c_str(), m_aces, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
        {
            SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        }
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
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclIdDelete positively with regular data
 * @target          OCStackResult OCCloudAclIdDelete(void* ctx,const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIdCreate
 *                  6. call OCCloudGetAclIdByDevice
 *                  7. call OCCloudAclIdDelete
 * @post_condition  none
 * @expected        OCCloudAclIdDelete will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_stc, OCCloudAclIdDelete_SRC_RV_P)
{
    if (!m_CloudAclHelper.cloudAclIdCreate((void*) CTX_CREATE_ACL, DEFAULT_OWNER_ID, DEFAULT_DEV_ID_ACL_DELETE, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudGetAclIdByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID_ACL_DELETE, &m_endPoint, CSCsdkCloudHelper::aclResponseCB, CSCsdkCloudHelper::s_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclIdDelete((void*) CTX_DELETE_ACL, CSCsdkCloudHelper::s_aclId.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclIdDelete positively with OCCloudResponseCB as NULL
 * @target          OCStackResult OCCloudAclIdDelete(void* ctx,const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCCloudResponseCB as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIdCreate
 *                  6. call OCCloudGetAclIdByDevice
 *                  7. call OCCloudAclIdDelete
 * @post_condition  none
 * @expected        OCCloudAclIdDelete will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_stc, OCCloudAclIdDeleteCb_NV_P)
{
    if (!m_CloudAclHelper.cloudAclIdCreate((void*) CTX_CREATE_ACL, DEFAULT_OWNER_ID, DEFAULT_DEV_ID_ACL_DELETE, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudGetAclIdByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID_ACL_DELETE, &m_endPoint, CSCsdkCloudHelper::aclResponseCB, CSCsdkCloudHelper::s_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclIdDelete((void*) CTX_DELETE_ACL, CSCsdkCloudHelper::s_aclId.c_str(), &m_endPoint, NULL, OC_STACK_OK, false))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclIndividualAclUpdate(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclAcesDelete positively with regular data
 * @target          OCStackResult OCCloudAclAcesDelete(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIdCreate
 *                  6. call OCCloudGetAclIdByDevice
 *                  7. call OCCloudAclIndividualAclUpdate
 *                  8. call OCCloudAclIndividualGetInfo
 *                  9. call OCCloudAclAcesDelete
 * @post_condition  none
 * @expected        OCCloudAclAcesDelete will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_stc, OCCloudAclAcesDelete_SRC_RV_P)
{
    if (!m_CloudAclHelper.cloudAclIdCreate((void*) CTX_CREATE_ACL, DEFAULT_OWNER_ID, DEFAULT_DEV_ID_ACES_DELETE, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudGetAclIdByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID_ACES_DELETE, &m_endPoint, CSCsdkCloudHelper::aclResponseCB, CSCsdkCloudHelper::s_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclIndividualAclUpdate((void*) CTX_INDIVIDUAL_ACL_UPDATE, CSCsdkCloudHelper::s_aclId.c_str(), m_aces, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_aclId.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclAcesDelete((void*) CTX_ACES_DELETE, CSCsdkCloudHelper::s_aclId.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclIndividualAclUpdate(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclAcesDelete positively with OCCloudResponseCB as NULL
 * @target          OCStackResult OCCloudAclAcesDelete(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCCloudResponseCB as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIdCreate
 *                  6. call OCCloudGetAclIdByDevice
 *                  7. call OCCloudAclIndividualAclUpdate
 *                  8. call OCCloudAclIndividualGetInfo
 *                  9. call OCCloudAclAcesDelete
 * @post_condition  none
 * @expected        OCCloudAclAcesDelete will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_stc, OCCloudAclAcesDeleteCb_NV_P)
{
    if (!m_CloudAclHelper.cloudAclIdCreate((void*) CTX_CREATE_ACL, DEFAULT_OWNER_ID, DEFAULT_DEV_ID_ACES_DELETE, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudGetAclIdByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID_ACES_DELETE, &m_endPoint, CSCsdkCloudHelper::aclResponseCB, CSCsdkCloudHelper::s_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclIndividualAclUpdate((void*) CTX_INDIVIDUAL_ACL_UPDATE, CSCsdkCloudHelper::s_aclId.c_str(), m_aces, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_aclId.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclAcesDelete((void*) CTX_ACES_DELETE, CSCsdkCloudHelper::s_aclId.c_str(), &m_endPoint, NULL, OC_STACK_OK, false))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclIndividualAclUpdate(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclIndividualAceDelete positively with regular data
 * @target          OCStackResult OCCloudAclIndividualAceDelete(void* ctx, const char *aclId, const char *aceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIdCreate
 *                  6. call OCCloudGetAclIdByDevice
 *                  7. call OCCloudAclIndividualAclUpdate
 *                  8. call OCCloudAclIndividualGetInfo
 *                  9. call OCCloudAclIndividualAceDelete
 * @post_condition  none
 * @expected        OCCloudAclIndividualAceDelete will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_stc, OCCloudAclIndividualAceDelete_SRC_RV_P)
{
    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_aclId.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclIndividualAceDelete((void*) CTX_INDIVIDUAL_ACE_DELETE, CSCsdkCloudHelper::s_aclId.c_str(), CSCsdkCloudHelper::s_aceid.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclIndividualAclUpdate(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @see             OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclIndividualAceDelete positively with OCCloudResponseCB as NULL
 * @target          OCStackResult OCCloudAclIndividualAceDelete(void* ctx, const char *aclId, const char *aceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   OCCloudResponseCB as NULL
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIdCreate
 *                  6. call OCCloudGetAclIdByDevice
 *                  7. call OCCloudAclIndividualAclUpdate
 *                  8. call OCCloudAclIndividualGetInfo
 *                  9. call OCCloudAclIndividualAceDelete
 * @post_condition  none
 * @expected        OCCloudAclIndividualAceDelete will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclDeleteTest_stc, OCCloudAclIndividualAceDeleteCb_NV_P)
{
    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_aclId.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }

    if (!m_CloudAclHelper.cloudAclIndividualAceDelete((void*) CTX_INDIVIDUAL_ACE_DELETE, CSCsdkCloudHelper::s_aclId.c_str(), CSCsdkCloudHelper::s_aceid.c_str(), &m_endPoint, NULL, OC_STACK_OK, false))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif
