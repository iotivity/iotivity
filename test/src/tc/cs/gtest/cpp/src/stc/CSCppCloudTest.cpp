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
#include "CSCppCloudHelper.h"
#include "CSCppUtilityHelper.h"

class CSCppCloudTest_stc: public ::testing::Test
{
protected:

    CSCppCloudHelper m_CloudAclHelper;

    string m_hostAddress = "coap+tcp://127.0.0.1:5683";
    string m_authCode = "";
    string m_groupId = "";
    string m_devIDcontrolee = "";
    string m_authTokenControlee = "";
    string m_refreshAuthTokenControlee = "";
    string m_devIDcontroller = "";
    string m_authTokenController = "";
    string m_refreshAuthTokenController = "";
    OCAccountManager::Ptr m_accountMgrControlee = nullptr;
    uint16_t m_credId = 0;
    ByteArray_t m_trustCertChainArray =
    { 0, 0 };

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();

        CommonUtil::copyFile(CLOUD_ACL_CONTROLLER_DAT_BACKUP,
        CLOUD_ACL_CONTROLLER_DAT);
        CommonUtil::copyFile(ROOT_CERT_FILE_BACKUP, ROOT_CERT_FILE);

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

        CSCppCloudHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &m_trustCertChainArray);
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCppCloudTest_stc, SaveTrustCertChain_SRC_RV_P)
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
 * @see             static OCStackResult saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test requestCertificate positively with regular data
 * @target          OCStackResult requestCertificate(ResponseCallBack callback)
 * @test_data       regular data
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  5. call saveTrustCertChain
 *                  6. call signIn
 *                  7. call requestCertificate
 * @post_condition  none
 * @expected        requestCertificate will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCppCloudTest_stc, RequestCertificate_SRC_RV_P)
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
 * @see             OCStackResult signIn(const std::string& userUuid, const std::string& accessToken, PostCallback cloudConnectHandler)
 * @objective       Test requestCertificate negatively with ResponseCallBack as NULL
 * @target          OCStackResult requestCertificate(ResponseCallBack callback)
 * @test_data       ResponseCallBack as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call setCoapPrefix
 *                  4. call CASelectCipherSuite
 *                  5. call saveTrustCertChain
 *                  6. call signIn
 *                  7. call requestCertificate
 * @post_condition  none
 * @expected        requestCertificate will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCppCloudTest_stc, RequestCertificateCb_NV_N)
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
 *                  6. call signIm
 *                  7. call requestCertificate
 *                  8. call getCRL
 * @post_condition  none
 * @expected        getCRL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCppCloudTest_stc, GetCRL_SRC_RV_P)
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
 *                  6. call signIn
 *                  7. call requestCertificate
 *                  8. call getCRL
 * @post_condition  none
 * @expected        getCRL will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCppCloudTest_stc, GetCRLCb_NV_N)
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
 *                  6. call signIn
 *                  7. call requestCertificate
 *                  8. call getCRL
 *                  9. call postCRL
 * @post_condition  none
 * @expected        postCRL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCppCloudTest_stc, PostCRL_SRC_RV_P)
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
 *                  6. call signIn
 *                  7. call getAclIdByDevice
 * @post_condition  none
 * @expected        getAclIdByDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCppCloudTest_stc, GetAclIdByDevice_SRC_RV_P)
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

    std::string ip(DEFAULT_HOST);
    OCCloudProvisioning cloudProv(ip, DEFAULT_PORT);
    std::string aclID = "";

    if (!m_CloudAclHelper.getAclIdByDevice(cloudProv, DEFAULT_DEV_ID, CSCppCloudHelper::cloudAclResponseCB, aclID, OC_STACK_OK))
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
 *                  6. call signIn
 *                  7. call getAclIdByDevice
 * @post_condition  none
 * @expected        getAclIdByDevice will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCppCloudTest_stc, GetAclIdByDeviceCb_NV_N)
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
 *                  7. call signIn
 *                  8. call getAclIdByDevice
 *                  9. call getIndividualAclInfo
 * @post_condition  none
 * @expected        getIndividualAclInfo will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCppCloudTest_stc, GetIndividualAclInfo_RV_SRC_P)
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
 *                  6. call signIn
 *                  7. call getAclIdByDevice
 *                  8. call getIndividualAclInfo
 * @post_condition  none
 * @expected        getIndividualAclInfo will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCppCloudTest_stc, GetIndividualAclInfoCb_NV_N)
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
