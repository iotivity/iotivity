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
#include "PMCsdkHelper.h"
#include "PMCsdkUtilityHelper.h"

class PMCsdkProvTest_btc: public ::testing::Test
{
protected:

    OCProvisionDev_t *m_OwnList, *m_UnownList, *m_motEnabledDevList, *m_targetDevice;
    OicSecAcl_t *m_Acl, *m_Acl1, *m_Acl2;
    PMCsdkHelper m_PMHelper;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        PMCsdkUtilityHelper::removeAllResFile();
        CommonUtil::waitInSecond(DELAY_LONG);
        CommonUtil::copyFile(JUSTWORKS_SERVER1_CBOR_BACKUP, JUSTWORKS_SERVER1_CBOR);
        CommonUtil::copyFile(JUSTWORKS_SERVER2_CBOR_BACKUP, JUSTWORKS_SERVER2_CBOR);
        CommonUtil::copyFile(RANDOMPIN_SERVER_CBOR_BACKUP, RANDOMPIN_SERVER_CBOR);
        CommonUtil::copyFile(CLIENT_CBOR_BACKUP, CLIENT_CBOR);
        CommonUtil::launchApp(JUSTWORKS_SERVER1);
        CommonUtil::launchApp(JUSTWORKS_SERVER2);
        CommonUtil::waitInSecond(DELAY_LONG);
        m_UnownList = NULL;
        m_OwnList = NULL;
        m_motEnabledDevList = NULL;
        m_targetDevice = NULL;
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);
    }
};

/**
 * @since           2015-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials positively.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       Regular data OCProvisionCredentials
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredential_SRC_RV_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkHelper::provisionCredCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials positively with Cred Type with Representative value of SYMMETRIC_PAIR_WISE_KEY.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       type = representative value of SYMMETRIC_PAIR_WISE_KEY
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialCredType_REV_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = (OicSecCredType_t)SYM_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkHelper::provisionCredCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials positively with Upper Boundary Value of Keysize.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       keySize = OWNER_PSK_LENGTH_256
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialKeySize_UBV_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkHelper::provisionCredCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with callback as NULL.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialCB_ECRC_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Device1 ID as NULL.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       pDev1 = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialDev1_NV_N)
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

    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, NULL,
                    device2, PMCsdkHelper::provisionCredCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Device2 ID as NULL.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       pDev2 = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialDev2_NV_N)
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
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    NULL, PMCsdkHelper::provisionCredCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Lower Outer Boundary Value of KeySize.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       keySize is Out of Lower Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialKeysize_LOBV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128 - 1;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkHelper::provisionCredCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively Upper Outer Boundary Value of KeySize
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       keySize is Out of Upper Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialKeysize_UOBV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256 + 1;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkHelper::provisionCredCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Lower Outer Boundary Value of Cred Type.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       OicSecCredType_t is out of Lower Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialCredType_LOBV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = (OicSecCredType_t)CRED_TYPE_LOBV;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkHelper::provisionCredCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Upper Outer Boundary Value of Cred Type
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       OicSecCredType_t is out of Upper Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialCredType_UOBV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = (OicSecCredType_t)CRED_TYPE_UOBV;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkHelper::provisionCredCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL positively.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for OCProvisionACL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionACL
 *                  8. call OCDeleteACLList for m_Acl
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionAcl_SRC_RV_P)
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

    m_Acl = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionACL((void*)g_ctx, m_OwnList, m_Acl,
                    m_PMHelper.provisionAclCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL negatively with Device List NULL.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       selectedDeviceInfo = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionACL
 *                  8. call OCDeleteACLList for m_Acl
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionAclDeviceList_ECRC_NV_N)
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

    m_Acl = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionACL((void*)g_ctx, NULL, m_Acl,
                    m_PMHelper.provisionAclCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL negatively with Access Control List NULL.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       acl = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionACL
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionAclAcl_NV_N)
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

    if (!m_PMHelper.provisionACL((void*)g_ctx, m_OwnList, NULL,
                    m_PMHelper.provisionAclCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL negatively with callback as NULL.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionACL
 *                  8. call OCDeleteACLList for m_Acl
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionAclCB_NV_N)
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

    m_Acl = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionACL((void*)g_ctx, m_OwnList, m_Acl,
                    NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL positively by calling provision acl multiple time.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       Call OCProvisionACL multiple time
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionACL
 *                  8. call OCProvisionACL
 *                  9. call OCDeleteACLList for m_Acl
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ProvisionAclMultipleTime_P)
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

    m_Acl = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionACL((void*)g_ctx, m_OwnList, m_Acl,
                    m_PMHelper.provisionAclCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.provisionACL((void*)g_ctx, m_OwnList, m_Acl,
                    m_PMHelper.provisionAclCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCUnlinkDevices  positively.
 * @target          OCStackResult OCUnlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1, const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for OCUnlinkDevices
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCUnlinkDevices
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, UnlinkDevices_RSV_SRC_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.unlinkDevices((void*)ctxUnlinkDevice, device1, device2,
                    PMCsdkHelper::unlinkDevicesCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCUnlinkDevices  negatively with Device1 as NULL.
 * @target          OCStackResult OCUnlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1, const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback)
 * @test_data       pTargetDev1 = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCUnlinkDevices
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, UnlinkDevicesDev1_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.unlinkDevices((void*)ctxUnlinkDevice, NULL, device2,
                    PMCsdkHelper::unlinkDevicesCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-22
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCUnlinkDevices  negatively with Device2 as NULL.
 * @target          OCStackResult OCUnlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1, const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback)
 * @test_data       pTargetDev2 = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCUnlinkDevices
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, UnlinkDevicesDev2_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.unlinkDevices((void*)ctxUnlinkDevice, device1, NULL,
                    PMCsdkHelper::unlinkDevicesCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-22
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback) * @objective       Test OCUnlinkDevices  negatively with CB as NULL.
 * @objective       Test OCUnlinkDevices  negatively with resultCallback as NULL.
 * @target          OCStackResult OCUnlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1, const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCUnlinkDevices
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, UnlinkDevicesCB_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.unlinkDevices((void*)ctxUnlinkDevice, device1, device2,
                    NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCRemoveDevice positively.
 * @target          OCStackResult OCRemoveDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       Regular Data for OCRemoveDevice
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCRemoveDevice
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, RemoveDevice_SRC_RV_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT, device1,
                    PMCsdkHelper::removeDeviceCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-22
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCRemoveDevice Positively with waitiTimeForownedDeviceDiscovery as Lower Boundary Value.
 * @target          OCStackResult OCRemoveDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCRemoveDevice
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, RemoveDeviceTime_LBV_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT_ONE, device1,
                    PMCsdkHelper::removeDeviceCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-22
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCRemoveDevice negatively with waitiTimeForownedDeviceDiscovery as LOBV (Time = ZERO).
 * @target          OCStackResult OCRemoveDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCRemoveDevice
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, RemoveDevice_LOBV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT_ZERO, device1,
                    PMCsdkHelper::removeDeviceCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-22
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCRemoveDevice negatively with pTargetDev  as NULL.
 * @target          OCStackResult OCRemoveDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       pTargetDev = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCRemoveDevice
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, RemoveDeviceTargetDev_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT, NULL,
                    PMCsdkHelper::removeDeviceCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-22
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCRemoveDevice negatively with callback  as NULL.
 * @target          OCStackResult OCRemoveDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCRemoveDevice
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, RemoveDeviceCB_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT, device1,
                    NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCRemoveDeviceWithUuid positively.
 * @target          OCStackResult OCRemoveDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback)
 * @test_data       Regular Data for OCRemoveDeviceWithUuid
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCRemoveDeviceWithUuid
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, RemoveDeviceWithUuid_SRC_RV_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OicUuid_t deviceUuid = device1->doxm->deviceID;

    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT, &deviceUuid, PMCsdkHelper::removeDeviceCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCRemoveDeviceWithUuid negatively with wrong UuId.
 * @target          OCStackResult OCRemoveDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback)
 * @test_data       Wrong Uuid for OCRemoveDeviceWithUuid
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCRemoveDeviceWithUuid
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, RemoveDeviceWithWrongUuid_RV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OicUuid_t deviceUuid;

    if(!m_PMHelper.convertStrToUuid(DEFAULT_OWNER_ID, &deviceUuid,OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT, &deviceUuid, PMCsdkHelper::removeDeviceCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCRemoveDeviceWithUuid negatively with NULL UuId.
 * @target          OCStackResult OCRemoveDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback)
 * @test_data       NULL Uuid for OCRemoveDeviceWithUuid
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCRemoveDeviceWithUuid
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, RemoveDeviceWithUuidUuid_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT, NULL, PMCsdkHelper::removeDeviceCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCRemoveDeviceWithUuid negatively with waitiTimeForownedDeviceDiscovery as LOBV (Time = ZERO).
 * @target          OCStackResult OCRemoveDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCRemoveDeviceWithUuid
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, RemoveDeviceWithUuid_LOBV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OicUuid_t deviceUuid = device1->doxm->deviceID;

    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT_ZERO, &deviceUuid, PMCsdkHelper::removeDeviceCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCRemoveDeviceWithUuid positively with waitiTimeForownedDeviceDiscovery as Lower Boundary Value
 * @target          OCStackResult OCRemoveDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCRemoveDeviceWithUuid
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, RemoveDeviceWithUuid_LBV_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OicUuid_t deviceUuid = device1->doxm->deviceID;

    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT_ONE, &deviceUuid, PMCsdkHelper::removeDeviceCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCRemoveDeviceWithUuid negatively with callback  as NULL.
 * @target          OCStackResult OCRemoveDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCRemoveDeviceWithUuid
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, RemoveDeviceWithUuidCB_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OicUuid_t deviceUuid = device1->doxm->deviceID;

    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT, &deviceUuid, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-25
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCResetDevice positively.
 * @target          OCStackResult OCResetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       Regular Data for OCResetDevice
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCResetDevice
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ResetDevice_SRC_RV_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.resetDevice((void*)ctxResetDevice, DISCOVERY_TIMEOUT, device1,
                    PMCsdkHelper::syncDeviceCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-25
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCResetDevice positively with waitiTimeForownedDeviceDiscovery as Lower Boundary Value
 * @target          OCStackResult OCResetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCResetDevice
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ResetDevice_LBV_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.resetDevice((void*)ctxResetDevice, DISCOVERY_TIMEOUT_ONE, device1,
                    PMCsdkHelper::syncDeviceCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-25
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCResetDevice negatively with waitiTimeForownedDeviceDiscovery as LOBV (Time = ZERO).
 * @target          OCStackResult OCResetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCResetDevice
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ResetDevice_LOBV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.resetDevice((void*)ctxResetDevice, DISCOVERY_TIMEOUT_ZERO, device1,
                    PMCsdkHelper::syncDeviceCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-25
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCResetDevice negatively with TargetDevice as NULL
 * @target          OCStackResult OCResetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       NULL TargetDevice for OCResetDevice
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCResetDevice
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 *
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ResetDeviceTargetDevice_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.resetDevice((void*)ctxResetDevice, DISCOVERY_TIMEOUT, NULL,
                    PMCsdkHelper::syncDeviceCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-25
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCResetDevice positively with TargetDevice No 2.
 * @target          OCStackResult OCResetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       TargetDevice No 2
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCResetDevice
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ResetDeviceTargetDevice2_SRC_RV_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.resetDevice((void*)ctxResetDevice, DISCOVERY_TIMEOUT, device2,
                    PMCsdkHelper::syncDeviceCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-25
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCResetDevice negatively with callback as NULL
 * @target          OCStackResult OCResetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCResetDevice
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ResetDeviceCB_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.resetDevice((void*)ctxResetDevice, DISCOVERY_TIMEOUT, device2,
                    NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCResetSVRDB positively.
 * @target          OCStackResult OCResetSVRDB(void)
 * @test_data       Regular Data for OCResetSVRDB
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCResetSVRDB
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCResetSVRDB will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, ResetSVRDB_SRC_RV_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.resetSVRDB(OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCGetLinkedStatus positively.
 * @target          OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @test_data       Regular Data for OCGetLinkedStatus
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCGetLinkedStatus
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCGetLinkedStatus will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, OCGetLinkedStatus_RSV_SRC_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCUuidList_t* uuidList = NULL;
    size_t numOfDevices = 0;

    if (!m_PMHelper.getLinkedStatus(&device1->doxm->deviceID, &uuidList, &numOfDevices, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCGetLinkedStatus positively.
 * @target          OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @test_data       uuidOfDevice = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCGetLinkedStatus
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCGetLinkedStatus will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, OCGetLinkedStatusDeviceID_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCUuidList_t* uuidList = NULL;
    size_t numOfDevices = 0;

    if (!m_PMHelper.getLinkedStatus(NULL, &uuidList, &numOfDevices, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCGetLinkedStatus negatively with uuidList as NULL
 * @target          OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @test_data       uuidList = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCGetLinkedStatus
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCGetLinkedStatus will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, OCGetLinkedStatusUuidListNV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    size_t numOfDevices;

    if (!m_PMHelper.getLinkedStatus(&device1->doxm->deviceID, NULL, &numOfDevices, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCGetLinkedStatus negatively with numOfDevices as NULL
 * @target          OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @test_data       numOfDevices = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCGetLinkedStatus
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCGetLinkedStatus will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, OCGetLinkedStatusNumDev_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCUuidList_t* uuidList = NULL;

    if (!m_PMHelper.getLinkedStatus(&device1->doxm->deviceID, &uuidList, NULL ,OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-07-03
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCGetCredResource positively
 * @target          OCStackResult OCGetCredResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       Regular data of  OCGetCredResource API
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCGetCredResource
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCGetCredResource will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, OCGetCredResource_RV_SRC_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.getCredResource(NULL, device1, PMCsdkHelper::getCredCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-07-03
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCGetCredResource negatively with selectedDeviceInfo as NULL
 * @target          OCStackResult OCGetCredResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       selectedDeviceInfo = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCGetCredResource
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCGetCredResource will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, OCGetCredResourceDev_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.getCredResource(NULL, NULL, PMCsdkHelper::getCredCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-07-03
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCGetCredResource negatively with resultCallback as NULL
 * @target          OCStackResult OCGetCredResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCGetCredResource
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCGetCredResource will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, OCGetCredResourceCb_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.getCredResource(NULL, device1, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-07-03
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCGetACLResource positively
 * @target          OCStackResult OCGetACLResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,OCProvisionResultCB resultCallback)
 * @test_data       Regular data of  OCGetACLResource API
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCGetACLResource
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCGetACLResource will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, OCGetACLResource_RV_SRC_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.getACLResource(NULL, device1, PMCsdkHelper::getAclCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-07-03
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCGetACLResource negatively with selectedDeviceInfo as NULL
 * @target          OCStackResult OCGetACLResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,OCProvisionResultCB resultCallback)
 * @test_data       selectedDeviceInfo = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCGetACLResource
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCGetACLResource will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, OCGetACLResource_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.getACLResource(NULL, NULL, PMCsdkHelper::getAclCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-07-03
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCGetACLResource negatively with resultCallback as NULL
 * @target          OCStackResult OCGetACLResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCGetACLResource
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCGetACLResource will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, OCGetACLResourceCb_NV_N)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.getACLResource(NULL, device1, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSelectOwnershipTransferMethod positively
 * @target          OCStackResult OCSelectOwnershipTransferMethod(const OicSecOxm_t *supportedMethods, size_t numberOfMethods, OicSecOxm_t *selectedMethod, OwnerType_t ownerType)
 * @test_data       regular data
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSelectOwnershipTransferMethod
 * @post_condition  none
 * @expected        OCSelectOwnershipTransferMethod will return OC_STACK_OK and selectedMethod will return OIC_JUST_WORKS
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, selectOwnershipTransferMethod_SRC_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OicSecOxm_t supportedMethods[2] =
    {   OIC_JUST_WORKS, OIC_RANDOM_DEVICE_PIN};
    OicSecOxm_t selectedMethod = NULL;

    if(!m_PMHelper.selectOwnershipTransferMethod(supportedMethods, 2, &selectedMethod, SUPER_OWNER, OC_STACK_OK, OIC_RANDOM_DEVICE_PIN))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSelectOwnershipTransferMethod positively
 * @target          OCStackResult OCSelectOwnershipTransferMethod(const OicSecOxm_t *supportedMethods, size_t numberOfMethods, OicSecOxm_t *selectedMethod, OwnerType_t ownerType)
 * @test_data       regular data
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSelectOwnershipTransferMethod
 * @post_condition  none
 * @expected        OCSelectOwnershipTransferMethod will return OC_STACK_OK and selectedMethod will return OIC_JUST_WORKS
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, selectOwnershipTransferMethod_NV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OicSecOxm_t supportedMethods[2] =
    {   OIC_RANDOM_DEVICE_PIN, OIC_JUST_WORKS};
    OicSecOxm_t selectedMethod = NULL;

    if(!m_PMHelper.selectOwnershipTransferMethod(supportedMethods, 2, &selectedMethod, SUPER_OWNER, OC_STACK_OK, OIC_RANDOM_DEVICE_PIN))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-26
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOxmAllowStatus positively with oxm = OIC_JUST_WORKS
 * @target          OCStackResult OCSetOxmAllowStatus(const OicSecOxm_t oxm, const bool allowStatus)
 * @test_data       oxm = OIC_JUST_WORKS
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOxmAllowStatus
 * @post_condition  none
 * @expected        OCSetOxmAllowStatus will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, setOxmAllowStatusJustWork_SRC_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.setOxmAllowStatus(OIC_JUST_WORKS, true, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOxmAllowStatus positively with oxm = OIC_JUST_WORKS and allowStatus=false
 * @target          OCStackResult OCSetOxmAllowStatus(const OicSecOxm_t oxm, const bool allowStatus)
 * @test_data       oxm = OIC_JUST_WORKS and allowStatus=false
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOxmAllowStatus
 * @post_condition  none
 * @expected        OCSetOxmAllowStatus will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, setOxmAllowStatusJustWorkFalse_SRC_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.setOxmAllowStatus(OIC_JUST_WORKS, false, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOxmAllowStatus positively with oxm = OIC_RANDOM_DEVICE_PIN
 * @target          OCStackResult OCSetOxmAllowStatus(const OicSecOxm_t oxm, const bool allowStatus)
 * @test_data       oxm = OIC_RANDOM_DEVICE_PIN
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOxmAllowStatus
 * @post_condition  none
 * @expected        OCSetOxmAllowStatus will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, setOxmAllowStatusRandPin_SRC_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.setOxmAllowStatus(OIC_RANDOM_DEVICE_PIN, true, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOxmAllowStatus positively with oxm = OIC_MANUFACTURER_CERTIFICATE
 * @target          OCStackResult OCSetOxmAllowStatus(const OicSecOxm_t oxm, const bool allowStatus)
 * @test_data       oxm = OIC_MANUFACTURER_CERTIFICATE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOxmAllowStatus
 * @post_condition  none
 * @expected        OCSetOxmAllowStatus will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, setOxmAllowStatusManufacurer_SRC_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.setOxmAllowStatus(OIC_MANUFACTURER_CERTIFICATE, true, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOxmAllowStatus positively with oxm = OIC_DECENTRALIZED_PUBLIC_KEY
 * @target          OCStackResult OCSetOxmAllowStatus(const OicSecOxm_t oxm, const bool allowStatus)
 * @test_data       oxm = OIC_DECENTRALIZED_PUBLIC_KEY
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOxmAllowStatus
 * @post_condition  none
 * @expected        OCSetOxmAllowStatus will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, setOxmAllowStatusDecentralized_SRC_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.setOxmAllowStatus(OIC_DECENTRALIZED_PUBLIC_KEY, true, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOxmAllowStatus negatively with oxm = OIC_OXM_COUNT
 * @target          OCStackResult OCSetOxmAllowStatus(const OicSecOxm_t oxm, const bool allowStatus)
 * @test_data       oxm = OIC_OXM_COUNT
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOxmAllowStatus
 * @post_condition  none
 * @expected        OCSetOxmAllowStatus will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, setOxmAllowStatus_LOBV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.setOxmAllowStatus(OIC_OXM_COUNT, true, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-26
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCSaveACL positively.
 * @target          OCStackResult OCSaveACL(const OicSecAcl_t* acl)
 * @test_data       regular data
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCSaveACL
 *                  8. call OCDeleteACLList for m_Acl
 * @post_condition  None
 * @expected        OCSaveACL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, saveACL_SRC_P)
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
    m_Acl = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList, false);

    if(!m_PMHelper.saveACL(m_Acl, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.deleteACLList(m_Acl))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCSaveACL Negatively with acl NULL value.
 * @target          OCStackResult OCSaveACL(const OicSecAcl_t* acl)
 * @test_data       acl=NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSaveACL
 * @post_condition  None
 * @expected        OCSaveACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, saveACLAcl_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.saveACL(NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCDeleteDiscoveredDevices positively.
 * @target          void OCDeleteDiscoveredDevices(OCProvisionDev_t *pList)
 * @test_data       regular data
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCDeleteDiscoveredDevices
 * @post_condition  None
 * @expected        OCDeleteDiscoveredDevices will not throw Exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, deleteDiscoveredDevices_SRC_P)
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

    if(!m_PMHelper.deleteDiscoveredDevices(m_OwnList))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDeleteDiscoveredDevices positively with unowned device.
 * @target          void OCDeleteDiscoveredDevices(OCProvisionDev_t *pList)
 * @test_data       Unowned device List
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDeleteDiscoveredDevices
 * @post_condition  None
 * @expected        OCDeleteDiscoveredDevices will not throw Exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, deleteDiscoveredDevicesUnowned_SRC_P)
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

    if(!m_PMHelper.deleteDiscoveredDevices(m_UnownList))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDeleteDiscoveredDevices Negatively with pList=NULL.
 * @target          void OCDeleteDiscoveredDevices(OCProvisionDev_t *pList)
 * @test_data       pList=NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDeleteDiscoveredDevices
 * @post_condition  None
 * @expected        OCDeleteDiscoveredDevices will not throw Exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, deleteDiscoveredDevicesPList_NV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.deleteDiscoveredDevices(NULL))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @objective       Test OCDeleteUuidList positively.
 * @target          void OCDeleteUuidList(OCUuidList_t* pList)
 * @test_data       Regular Data for OCDeleteUuidList
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCGetLinkedStatus
 *                  9. call OCDeleteUuidList
 *                 10. call OCDeleteACLList for m_Acl1
 *                 11. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCDeleteUuidList will not throw Exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, deleteUuidList_RSV_SRC_P)
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
    OCProvisionDev_t *device2 = m_OwnList->next;
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCUuidList_t* uuidList = NULL;
    size_t numOfDevices = 0;

    if (!m_PMHelper.getLinkedStatus(&device1->doxm->deviceID, &uuidList, &numOfDevices, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteUuidList(uuidList))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl2))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif
/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @objective       Test OCDeleteUuidList negatively with pList=NULL.
 * @target          void OCDeleteUuidList(OCUuidList_t* pList)
 * @test_data       pList=NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDeleteUuidList
 * @post_condition  None
 * @expected        OCDeleteUuidList will not throw Exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, deleteUuidListPList_NV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.deleteUuidList(NULL))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCDeleteACLList positively.
 * @target          void OCDeleteACLList(OicSecAcl_t* pAcl)
 * @test_data       regular data
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCDeleteACLList
 * @post_condition  None
 * @expected        OCDeleteACLList will not throw Exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, deleteACLList_SRC_P)
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
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCDeleteACLList negatively with pAcl=NULL.
 * @target          void OCDeleteACLList(OicSecAcl_t* pAcl)
 * @test_data       pAcl=NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDeleteACLList
 * @post_condition  None
 * @expected        OCDeleteACLList will not throw Exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkProvTest_btc, deleteACLListPAcl_NV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.deleteACLList(NULL))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

