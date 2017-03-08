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
#include "PMCsdkHelper.h"
#include "PMCsdkCertHelper.h"
#include "PMCsdkUtilityHelper.h"

class PMCsdkCertTest_btc: public ::testing::Test
{
protected:

    OCProvisionDev_t *m_OwnList, *m_UnownList;
    OicSecAcl_t *m_Acl, *m_Acl1, *m_Acl2;
    PMCsdkHelper m_PMHelper;
    PMCsdkCertHelper m_PMCertHelper;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        PMCsdkUtilityHelper::removeAllResFile();
        CommonUtil::waitInSecond(DELAY_LONG);
        CommonUtil::copyFile(ROOT_CERT_FILE_BACKUP, ROOT_CERT_FILE);
        CommonUtil::copyFile(JUSTWORKS_SERVER1_CBOR_BACKUP, JUSTWORKS_SERVER1_CBOR);
        CommonUtil::copyFile(CLIENT_CBOR_BACKUP, CLIENT_CBOR);
        CommonUtil::launchApp(JUSTWORKS_SERVER1);
        CommonUtil::waitInSecond(DELAY_LONG);
        m_UnownList = NULL;
        m_OwnList = NULL;
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, RegisterTrustCertChainNotifier_SRC_RV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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
 * @expected        OCRegisterTrustCertChainNotifier will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, RegisterTrustCertChainNotifierCb_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, NULL, OC_STACK_INVALID_CALLBACK))
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, RemoveTrustCertChainNotifier_SRC_RV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, SaveTrustCertChain_SRC_RV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, SaveTrustCertChainCrtChain_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, PMCsdkCertHelper::trustCertChainChangeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray =
    {   0, 0};

    m_PMCertHelper.readFile(ROOT_CERT_FILE, (OCByteString *) &g_trustCertChainArray);

    uint16_t g_credId = 0;
    if (!m_PMCertHelper.saveTrustCertChain(NULL, g_trustCertChainArray.len,
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
 * @objective       Test OCSaveTrustCertChain negatively with chainSize as NULL
 * @target          OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @test_data       chainSize as NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCRegisterTrustCertChainNotifier
 *                  5. call OCSaveTrustCertChain
 * @post_condition  None
 * @expected        OCSaveTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, SaveTrustCertChainCrtChainLen_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, PMCsdkCertHelper::trustCertChainChangeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray =
    {   0, 0};

    m_PMCertHelper.readFile(ROOT_CERT_FILE, (OCByteString *) &g_trustCertChainArray);

    uint16_t g_credId = 0;
    if (!m_PMCertHelper.saveTrustCertChain(g_trustCertChainArray.data, -1,
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, ReadTrustCertChain_SRC_RV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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
 * @expected        OCReadTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, ReadTrustCertChainCredId_LOBV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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

    if (!m_PMCertHelper.readTrustCertChain(-1, &g_trustCertChainArray1.data, &g_trustCertChainArray1.len, OC_STACK_INVALID_PARAM))
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, ReadTrustCertChainCertChain_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, ReadTrustCertChainCertChainSize_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionTrustCertChain positively with regular data
 * @target          OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for the API
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionTrustCertChain
 * @post_condition  None
 * @expected        OCProvisionTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, ProvisionTrustCertChain_SRC_RV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,
                    &m_UnownList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.doOwnerShipTransfer((void*)g_ctx, &m_UnownList,
                    PMCsdkHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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

    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, g_credId, device1, PMCsdkCertHelper::provisionCertCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionTrustCertChain negatively with regular data
 * @target          OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       OCProvisionDev_t = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionTrustCertChain
 * @post_condition  None
 * @expected        OCProvisionTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, ProvisionTrustCertChainDev_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,
                    &m_UnownList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.doOwnerShipTransfer((void*)g_ctx, &m_UnownList,
                    PMCsdkHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, 1, NULL, PMCsdkCertHelper::provisionCertCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionTrustCertChain negatively with credId as -1
 * @target          OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       credId = -1
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionTrustCertChain
 * @post_condition  None
 * @expected        OCProvisionTrustCertChain will return OC_STACK_NO_RESOURCE
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, ProvisionTrustCertChainCredId_LOBV_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,
                    &m_UnownList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.doOwnerShipTransfer((void*)g_ctx, &m_UnownList,
                    PMCsdkHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, CRED_ID_NEGATIVE, device1, PMCsdkCertHelper::provisionCertCB, OC_STACK_NO_RESOURCE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionTrustCertChain negatively with resultCallback as NULL
 * @target          OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionTrustCertChain
 * @post_condition  None
 * @expected        OCProvisionTrustCertChain will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, ProvisionTrustCertChainCb_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,
                    &m_UnownList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.doOwnerShipTransfer((void*)g_ctx, &m_UnownList,
                    PMCsdkHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, 1, device1, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCReadTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize)
 * @objective       Test OCProvisionTrustCertChain positively with regular data
 * @target          OCStackResult OCProvisionCRL(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecCrl_t *crl, OCProvisionResultCB resultCallback)
 * @test_data       OCProvisionDev_t = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCRegisterTrustCertChainNotifier
 *                  8. call OCSaveTrustCertChain
 *                  9. call OCProvisionTrustCertChain
 *                  10. call OCReadTrustCertChain
 *                  11. call OCProvisionCRL
 * @post_condition  None
 * @expected        OCProvisionCRL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, ProvisionCRL_SRC_RV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,
                    &m_UnownList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.doOwnerShipTransfer((void*)g_ctx, &m_UnownList,
                    PMCsdkHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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

    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, g_credId, device1, PMCsdkCertHelper::provisionCertCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray1 =
    {   0, 0};

    if (!m_PMCertHelper.readTrustCertChain(g_credId, &g_trustCertChainArray1.data, &g_trustCertChainArray1.len, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    OicSecCrl_t *crl = GetCRLResource();

    if (!m_PMCertHelper.provisionCRL((void*)CTX_PROV_TRUST_CERT, device1,
                    crl, PMCsdkCertHelper::provisionCertCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCReadTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize)
 * @objective       Test OCProvisionTrustCertChain negatively with OCProvisionResultCB as NULL
 * @target          OCStackResult OCProvisionCRL(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecCrl_t *crl, OCProvisionResultCB resultCallback)
 * @test_data       OCProvisionResultCB = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCRegisterTrustCertChainNotifier
 *                  8. call OCSaveTrustCertChain
 *                  9. call OCProvisionTrustCertChain
 *                  10. call OCReadTrustCertChain
 *                  11. call OCProvisionCRL
 * @post_condition  None
 * @expected        OCProvisionCRL will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, ProvisionCRLCb_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,
                    &m_UnownList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.doOwnerShipTransfer((void*)g_ctx, &m_UnownList,
                    PMCsdkHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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

    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, g_credId, device1, PMCsdkCertHelper::provisionCertCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray1 =
    {   0, 0};

    if (!m_PMCertHelper.readTrustCertChain(g_credId, &g_trustCertChainArray1.data, &g_trustCertChainArray1.len, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    OicSecCrl_t *crl = GetCRLResource();

    if (!m_PMCertHelper.provisionCRL((void*)CTX_PROV_TRUST_CERT, device1,
                    crl, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCReadTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize)
 * @objective       Test OCProvisionTrustCertChain negatively with OCProvisionDev_t as NULL
 * @target          OCStackResult OCProvisionCRL(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecCrl_t *crl, OCProvisionResultCB resultCallback)
 * @test_data       OCProvisionDev_t = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCRegisterTrustCertChainNotifier
 *                  8. call OCSaveTrustCertChain
 *                  9. call OCProvisionTrustCertChain
 *                  10. call OCReadTrustCertChain
 *                  11. call OCProvisionCRL
 * @post_condition  None
 * @expected        OCProvisionCRL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, ProvisionCRLDev_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,
                    &m_UnownList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.doOwnerShipTransfer((void*)g_ctx, &m_UnownList,
                    PMCsdkHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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

    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, g_credId, device1, PMCsdkCertHelper::provisionCertCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray1 =
    {   0, 0};

    if (!m_PMCertHelper.readTrustCertChain(g_credId, &g_trustCertChainArray1.data, &g_trustCertChainArray1.len, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    OicSecCrl_t *crl = GetCRLResource();

    if (!m_PMCertHelper.provisionCRL((void*)CTX_PROV_TRUST_CERT, NULL,
                    crl, PMCsdkCertHelper::provisionCertCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCRegisterTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB)
 * @see             OCStackResult OCSaveTrustCertChain(uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCReadTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize)
 * @objective       Test OCProvisionTrustCertChain negatively with OicSecCrl_t as NULL
 * @target          OCStackResult OCProvisionCRL(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecCrl_t *crl, OCProvisionResultCB resultCallback)
 * @test_data       OicSecCrl_t = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCRegisterTrustCertChainNotifier
 *                  8. call OCSaveTrustCertChain
 *                  9. call OCProvisionTrustCertChain
 *                  10. call OCReadTrustCertChain
 *                  11. call OCProvisionCRL
 * @post_condition  None
 * @expected        OCProvisionCRL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCertTest_btc, ProvisionCRLCrl_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,
                    &m_UnownList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.doOwnerShipTransfer((void*)g_ctx, &m_UnownList,
                    PMCsdkHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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

    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, g_credId, device1, PMCsdkCertHelper::provisionCertCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t trustCertChainArray =
    {   0, 0};

    if (!m_PMCertHelper.readTrustCertChain(g_credId, &trustCertChainArray.data, &trustCertChainArray.len, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    if (!m_PMCertHelper.provisionCRL((void*)CTX_PROV_TRUST_CERT, device1,
                    NULL, PMCsdkCertHelper::provisionCertCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif
