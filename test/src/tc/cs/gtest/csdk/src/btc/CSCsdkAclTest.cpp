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

class CSCsdkAclTest_btc: public ::testing::Test
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
 * @objective       Test OCCloudGetAclIdByDevice negatively with deviceId as NULL
 * @target          OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       deviceId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudGetAclIdByDevice
 * @post_condition  none
 * @expected        OCCloudGetAclIdByDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudGetAclIdByDeviceDevId_NV_N)
{
    if (!m_CloudAclHelper.cloudGetAclIdByDevice((void*) CTX_GET_ACL_ID_BY_DEV, NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, CSCsdkCloudHelper::s_aclId, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudGetAclIdByDevice negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudGetAclIdByDevice
 * @post_condition  none
 * @expected        OCCloudGetAclIdByDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudGetAclIdByDeviceDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudGetAclIdByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, NULL, CSCsdkCloudHelper::cloudResponseCB, CSCsdkCloudHelper::s_aclId, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIdCreate negatively with ownerId as NULL
 * @target          OCStackResult OCCloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       ownerId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIdCreate
 * @post_condition  none
 * @expected        OCCloudAclIdCreate will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIdCreateOwnerId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIdCreate((void*) CTX_INDIVIDUAL_GET_INFO, NULL, DEFAULT_DEV_ID_CS_01, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIdCreate negatively with deviceId as NULL
 * @target          OCStackResult OCCloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       deviceId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIdCreate
 * @post_condition  none
 * @expected        OCCloudAclIdCreate will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIdCreateDevId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIdCreate((void*) CTX_INDIVIDUAL_GET_INFO, DEFAULT_OWNER_ID, NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIdCreate negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIdCreate
 * @post_condition  none
 * @expected        OCCloudAclIdCreate will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIdCreateDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIdCreate((void*) CTX_INDIVIDUAL_GET_INFO, DEFAULT_OWNER_ID, DEFAULT_DEV_ID_CS_01, NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIndividualGetInfo negatively with aclId as NULL
 * @target          OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       aclId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIndividualGetInfo
 * @post_condition  none
 * @expected        OCCloudAclIndividualGetInfo will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualGetInfoAclId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET_INFO, NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIndividualGetInfo negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIndividualGetInfo
 * @post_condition  none
 * @expected        OCCloudAclIndividualGetInfo will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualGetInfoDevAddr_NV_N)
{
    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_aclId.c_str(), NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIndividualUpdateAce negatively with cloudAce_t as NULL
 * @target          OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       cloudAce_t as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIndividualUpdateAce
 * @post_condition  none
 * @expected        OCCloudAclIndividualUpdateAce will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualAclUpdateAclId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIndividualAclUpdate((void*) CTX_INDIVIDUAL_UPDATE_ACE, NULL, m_aces, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIndividualUpdateAce negatively with cloudAce_t as NULL
 * @target          OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *m_aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       cloudAce_t as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIndividualUpdateAce
 * @post_condition  none
 * @expected        OCCloudAclIndividualUpdateAce will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualAclUpdateCloudAce_t_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIndividualAclUpdate((void*) CTX_INDIVIDUAL_UPDATE_ACE, CSCsdkCloudHelper::s_aclId.c_str(), NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIndividualUpdateAce negatively with endPoint as NULL
 * @target          OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *m_aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       endPoint as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIndividualUpdateAce
 * @post_condition  none
 * @expected        OCCloudAclIndividualUpdateAce will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualAclUpdateDevAddr_NV_N)
{
    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualAclUpdate((void*) CTX_INDIVIDUAL_UPDATE_ACE, CSCsdkCloudHelper::s_aclId.c_str(), aces, NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIndividualUpdateAce negatively with aclId as NULL
 * @target          OCStackResult OCCloudAclIndividualAceUpdate(void* ctx, const char *aclId, const char *aceId, const cloudAce_t *m_aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       aclId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIndividualAceUpdate
 * @post_condition  none
 * @expected        OCCloudAclIndividualAceUpdate will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualAceUpdateAclId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIndividualAceUpdate((void*) CTX_INDIVIDUAL_UPDATE_ACE, NULL, CSCsdkCloudHelper::s_aceid.c_str(), m_aces, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIndividualUpdateAce negatively with aceId as NULL
 * @target          OCStackResult OCCloudAclIndividualAceUpdate(void* ctx, const char *aclId, const char *aceId, const cloudAce_t *m_aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       aceId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIndividualAceUpdate
 * @post_condition  none
 * @expected        OCCloudAclIndividualAceUpdate will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualAceUpdateAceId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIndividualAceUpdate((void*) CTX_INDIVIDUAL_UPDATE_ACE, CSCsdkCloudHelper::s_aclId.c_str(), NULL, m_aces, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIndividualUpdateAce negatively with m_aces as NULL
 * @target          OCStackResult OCCloudAclIndividualAceUpdate(void* ctx, const char *aclId, const char *aceId, const cloudAce_t *m_aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       m_aces as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIndividualAceUpdate
 * @post_condition  none
 * @expected        OCCloudAclIndividualAceUpdate will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualAceUpdateAces_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIndividualAceUpdate((void*) CTX_INDIVIDUAL_UPDATE_ACE, CSCsdkCloudHelper::s_aclId.c_str(), CSCsdkCloudHelper::s_aceid.c_str(), NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclIndividualUpdateAce negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclIndividualAceUpdate(void* ctx, const char *aclId, const char *aceId, const cloudAce_t *m_aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclIndividualAceUpdate
 * @post_condition  none
 * @expected        OCCloudAclIndividualAceUpdate will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualAceUpdateDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclIndividualAceUpdate((void*) CTX_INDIVIDUAL_UPDATE_ACE, CSCsdkCloudHelper::s_aclId.c_str(), CSCsdkCloudHelper::s_aceid.c_str(), m_aces, NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif
