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
#include "PMCsdkHelper.h"
#include "PMCsdkCertHelper.h"
#include "PMCsdkUtilityHelper.h"

class PMCsdkCertTest_btc: public ::testing::Test
{
protected:

    OCProvisionDev_t *m_UnownList;
    OicSecAcl_t *m_Acl, *m_Acl1, *m_Acl2;
    PMCsdkHelper m_PMHelper;
    PMCsdkCertHelper m_PMCertHelper;

    char* g_caKeyPem; /* Test CA private key */
    char* g_caCertPem; /* Test CA certificate */
    uint16_t g_caCredId = 0; /* ID of CA's OCF identity cert */

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        PMCsdkUtilityHelper::removeAllResFile();
        CommonUtil::waitInSecond(DELAY_LONG);
        CommonUtil::copyFile(ROOT_CERT_UNOWNED_BACKUP, ROOT_CERT);
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O1_UNOWNED_BACKUP, JUSTWORKS_SERVER_CBOR_O1);
        CommonUtil::copyFile(CLIENT_CBOR_01_UNOWNED_BACKUP, CLIENT_CBOR_01);
        CommonUtil::launchApp(JUSTWORKS_SERVER1);
        CommonUtil::waitInSecond(DELAY_LONG);
        m_UnownList = NULL;
        g_caKeyPem = NULL;
        g_caCertPem = NULL;

        if (!m_PMHelper.initProvisionClient())
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
            return;
        }
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);
    }
};

/**
 * @since           2017-02-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCRegisterTrustCertChainNotifier Positively
 * @target          OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @test_data       Regular data for OCRegisterTrustCertChainNotifier
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCRegisterTrustCertChainNotifier
 * @post_condition  None
 * @expected        OCRegisterTrustCertChainNotifier will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_btc, RegisterTrustCertChainNotifier_SRC_RV_P)
{
    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, PMCsdkCertHelper::trustCertChainChangeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCRegisterTrustCertChainNotifier Negatively with TrustCertChainChangeCB as NULL
 * @target          OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @test_data       TrustCertChainChangeCB as NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCRegisterTrustCertChainNotifier
 * @post_condition  None
 * @expected        OCRegisterTrustCertChainNotifier will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_btc, RegisterTrustCertChainNotifierCb_NV_N)
{
    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @objective       Test OCRemoveTrustCertChainNotifier positively
 * @target          void OCRemoveTrustCertChainNotifier(void)
 * @test_data       Regular data for OCRegisterTrustCertChainNotifier
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCRegisterTrustCertChainNotifier
 *                  5. call OCRemoveTrustCertChainNotifier
 * @post_condition  None
 * @expected        OCRemoveTrustCertChainNotifier will throw no exception
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_btc, RemoveTrustCertChainNotifier_SRC_RV_P)
{
    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, PMCsdkCertHelper::trustCertChainChangeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    if (!m_PMCertHelper.removeTrustCertChainNotifier())
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @objective       Test OCSaveTrustCertChain positively
 * @target          OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @test_data       Regular data for OCRegisterTrustCertChainNotifier
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCRegisterTrustCertChainNotifier
 *                  5. call OCSaveTrustCertChain
 * @post_condition  None
 * @expected        OCSaveTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_btc, SaveTrustCertChain_SRC_RV_P)
{
    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, PMCsdkCertHelper::trustCertChainChangeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray =
    {   0, 0};

    m_PMCertHelper.readFile(ROOT_CERT_FILE, (OCByteString *) &g_trustCertChainArray);

    uint16_t g_credId = 0;
    if (!m_PMCertHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @objective       Test OCSaveTrustCertChain negatively with trustCertChain as NULL
 * @target          OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @test_data       trustCertChain as NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCRegisterTrustCertChainNotifier
 *                  5. call OCSaveTrustCertChain
 * @post_condition  None
 * @expected        OCSaveTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_btc, SaveTrustCertChainCrtChain_NV_N)
{
    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, PMCsdkCertHelper::trustCertChainChangeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray =
    {   0, 0};

    m_PMCertHelper.readFile(ROOT_CERT_FILE, (OCByteString *) &g_trustCertChainArray);

    uint16_t g_credId = 0;
    if (!m_PMCertHelper.saveTrustCertChain(NULL, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @objective       Test OCSaveTrustCertChain negatively with chainSize as -1
 * @target          OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @test_data       chainSize as -1
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCRegisterTrustCertChainNotifier
 *                  5. call OCSaveTrustCertChain
 * @post_condition  None
 * @expected        OCSaveTrustCertChain will return OC_STACK_NO_MEMORY
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_btc, SaveTrustCertChainCrtChainLen_NV_N)
{
    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, PMCsdkCertHelper::trustCertChainChangeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray =
    {   0, 0};

    m_PMCertHelper.readFile(ROOT_CERT_FILE, (OCByteString *) &g_trustCertChainArray);

    uint16_t g_credId = 0;
    if (!m_PMCertHelper.saveTrustCertChain(g_trustCertChainArray.data, -1,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_NO_MEMORY))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCReadTrustCertChain positively with regular data
 * @target          OCStackResult OCReadTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize)
 * @test_data       chainSize as NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCRegisterTrustCertChainNotifier
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCReadTrustCertChain
 * @post_condition  None
 * @expected        OCReadTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_btc, ReadTrustCertChain_SRC_RV_P)
{
    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, PMCsdkCertHelper::trustCertChainChangeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray =
    {   0, 0};

    m_PMCertHelper.readFile(ROOT_CERT_FILE, (OCByteString *) &g_trustCertChainArray);

    uint16_t g_credId = 0;
    if (!m_PMCertHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray1 =
    {   0, 0};

    if (!m_PMCertHelper.readTrustCertChain(g_credId, &g_trustCertChainArray1.data, &g_trustCertChainArray1.len, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCReadTrustCertChain positively with credId as -1
 * @target          OCStackResult OCReadTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize)
 * @test_data       credId as -1
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCRegisterTrustCertChainNotifier
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCReadTrustCertChain
 * @post_condition  None
 * @expected        OCReadTrustCertChain will return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_btc, ReadTrustCertChainCredId_LOBV_N)
{
    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, PMCsdkCertHelper::trustCertChainChangeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray =
    {   0, 0};

    m_PMCertHelper.readFile(ROOT_CERT_FILE, (OCByteString *) &g_trustCertChainArray);

    uint16_t g_credId = 0;
    if (!m_PMCertHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray1 =
    {   0, 0};

    if (!m_PMCertHelper.readTrustCertChain(-1, &g_trustCertChainArray1.data, &g_trustCertChainArray1.len, OC_STACK_ERROR))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCReadTrustCertChain positively with trustCertChain as NULL
 * @target          OCStackResult OCReadTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize)
 * @test_data       trustCertChain as NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCRegisterTrustCertChainNotifier
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCReadTrustCertChain
 * @post_condition  None
 * @expected        OCReadTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_btc, ReadTrustCertChainCertChain_NV_N)
{
    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, PMCsdkCertHelper::trustCertChainChangeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray =
    {   0, 0};

    m_PMCertHelper.readFile(ROOT_CERT_FILE, (OCByteString *) &g_trustCertChainArray);

    uint16_t g_credId = 0;
    if (!m_PMCertHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray1 =
    {   0, 0};

    if (!m_PMCertHelper.readTrustCertChain(g_credId, NULL, &g_trustCertChainArray1.len, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-28
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @objective       Test OCReadTrustCertChain positively with chainSize as NULL
 * @target          OCStackResult OCReadTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize)
 * @test_data       chainSize as NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCRegisterTrustCertChainNotifier
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCReadTrustCertChain
 * @post_condition  None
 * @expected        OCReadTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_btc, ReadTrustCertChainCertChainSize_NV_N)
{
    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, PMCsdkCertHelper::trustCertChainChangeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray =
    {   0, 0};

    m_PMCertHelper.readFile(ROOT_CERT_FILE, (OCByteString *) &g_trustCertChainArray);

    uint16_t g_credId = 0;
    if (!m_PMCertHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray1 =
    {   0, 0};

    if (!m_PMCertHelper.readTrustCertChain(g_credId, &g_trustCertChainArray1.data, NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif
