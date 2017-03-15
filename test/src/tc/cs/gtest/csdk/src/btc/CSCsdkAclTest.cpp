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

class CSCsdkAclTest_btc: public ::testing::Test
{
protected:

    CSCsdkCloudHelper m_CloudAclHelper;
    string m_hostAddress = COAP_HOST_ADDRESS;
    OCAccountManager::Ptr m_accountMgrControlee = nullptr;
    string m_aclId = "";
    uint16_t m_credId = 0;
    ByteArray_t m_trustCertChainArray =
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
 * @since           2016-09-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             void setCoapPrefix(bool secured)
 * @see             CAResult_t CASelectCipherSuite(const uint16_t cipher, CATransportAdapter_t adapter)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclIdCreate positively with regular data
 * @target          OCStackResult OCCloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix for TLS MODE enabled
 *                  4. call CASelectCipherSuite(true)for TLS MODE enabled
 *                  5. call OCSaveTrustCertChain
 *                  6. call signIn
 *                  7. call OCCloudAclIdCreate
 * @post_condition  none
 * @expected        OCCloudAclIdCreate will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIdCreate_SRC_RV_P)
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

    if (!m_CloudAclHelper.cloudAclIdCreate((void*) CTX_INDIVIDUAL_GET_INFO, DEFAULT_OWNER_ID, DEFAULT_DEV_ID_CS_01, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
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
 * @objective       Test OCCloudGetAclIdByDevice positively with regualr data
 * @target          OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regualr data
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix for TLS MODE enabled
 *                  4. call CASelectCipherSuite(true)for TLS MODE enabled
 *                  5. call OCSaveTrustCertChain
 *                  6. call signIn
 *                  7. call OCCloudGetAclIdByDevice
 * @post_condition  none
 * @expected        OCCloudGetAclIdByDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIdGetByDevice_SRC_RV_P)
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

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, m_aclId, OC_STACK_OK))
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
 * @objective       Test OCCloudGetAclIdByDevice negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix for TLS MODE enabled
 *                  4. call CASelectCipherSuite(true)for TLS MODE enabled
 *                  5. call OCSaveTrustCertChain
 *                  6. call signIn
 *                  7. call OCCloudGetAclIdByDevice
 * @post_condition  none
 * @expected        OCCloudGetAclIdByDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIdGetByDeviceOCDevAddr_NV_N)
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

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, NULL, CSCsdkCloudHelper::cloudResponseCB, m_aclId, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCCloudGetAclIdByDevice positively with OCCloudResponseCB as NULL
 * @target          OCStackResult OCCloudGetAclIdByDevice(void* ctx, const char *deviceId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCCloudResponseCB as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix for TLS MODE enabled
 *                  4. call CASelectCipherSuite(true)for TLS MODE enabled
 *                  5. call OCSaveTrustCertChain
 *                  6. call signIn
 *                  7. call OCCloudGetAclIdByDevice
 * @post_condition  none
 * @expected        OCCloudGetAclIdByDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIdGetByDeviceCb_NV_P)
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

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, NULL, m_aclId, OC_STACK_OK, false))
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
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclIndividualGetInfo positively with regular data
 * @target          OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIndividualUpdateAce
 *                  6. call OCCloudAclIndividualGetInfo
 * @post_condition  none
 * @expected        OCCloudAclIndividualGetInfo will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualGetInfo_SRC_RV_P)
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

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, m_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, m_aclId.c_str(), aces, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET_INFO, m_aclId.c_str(), &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
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
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @see             OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclIndividualGetInfo positively with regular data
 * @target          OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIndividualUpdateAce
 *                  6. call OCCloudAclIndividualGetInfo
 * @post_condition  none
 * @expected        OCCloudAclIndividualGetInfo will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualGetInfoOCDevAddr_NV_N)
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

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, m_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, m_aclId.c_str(), aces, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET_INFO, m_aclId.c_str(), NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclIndividualGetInfo positively with callback as NULL
 * @target          OCStackResult OCCloudAclIndividualGetInfo(void* ctx, const char *aclId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       callback as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIndividualGetInfo
 * @post_condition  none
 * @expected        OCCloudAclIndividualGetInfo will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualGetInfoCb_NV_P)
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

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, m_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, m_aclId.c_str(), aces, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    if (!m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET_INFO, m_aclId.c_str(), &endPoint, NULL, OC_STACK_OK, false))
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
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclIndividualUpdateAce positively with regular data
 * @target          OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       regular data for target API
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIndividualUpdateAce
 * @post_condition  none
 * @expected        OCCloudAclIndividualUpdateAce will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualUpdateAce_SRC_RV_P)
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

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, m_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, m_aclId.c_str(), aces, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK))
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
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclIndividualUpdateAce negatively with endPoint as NULL
 * @target          OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       endPoint as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIndividualUpdateAce
 * @post_condition  none
 * @expected        OCCloudAclIndividualUpdateAce will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualUpdateAceOCDevAddr_NV_N)
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

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, m_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, m_aclId.c_str(), aces, NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclIndividualUpdateAce positively with callback as NULL
 * @target          OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       callback as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIndividualUpdateAce
 * @post_condition  none
 * @expected        OCCloudAclIndividualUpdateAce will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualUpdateAceCb_NV_P)
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

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, m_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    cloudAce_t *aces = CSCsdkUtilityHelper::createCloudAces();

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, m_aclId.c_str(), aces, &endPoint, NULL, OC_STACK_OK, false))
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
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test OCCloudAclIndividualUpdateAce negatively with cloudAce_t as NULL
 * @target          OCStackResult OCCloudAclIndividualUpdateAce(void* ctx, const char *aclId, const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       cloudAce_t as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCSaveTrustCertChain
 *                  4. call signIn
 *                  5. call OCCloudAclIndividualUpdateAce
 * @post_condition  none
 * @expected        OCCloudAclIndividualUpdateAce will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkAclTest_btc, OCCloudAclIndividualUpdateAceCloudAce_t_NV_N)
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

    if (!m_CloudAclHelper.cloudAclIdGetByDevice((void*) CTX_GET_ACL_ID_BY_DEV, DEFAULT_DEV_ID, &endPoint, CSCsdkCloudHelper::aclResponseCB, m_aclId, OC_STACK_OK))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }

    if (!m_CloudAclHelper.cloudAclIndividualUpdateAce((void*) CTX_INDIVIDUAL_UPDATE_ACE, m_aclId.c_str(), NULL, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif
