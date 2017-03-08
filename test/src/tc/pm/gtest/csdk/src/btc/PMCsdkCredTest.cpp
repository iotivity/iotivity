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

class PMCsdkCredTest_btc: public ::testing::Test
{
protected:

    OCProvisionDev_t *m_OwnList, *m_UnownList, *m_motEnabledDevList;
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
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);
    }
};

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionPairwiseDevices Positively
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for OCProvisionPairwiseDevices
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCDeleteACLList for m_Acl1
 *                  9. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCredTest_btc, ProvisionPairwiseDevices_SRC_RV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

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
 * @since           2015-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionPairwiseDevices Positively with KeySize as Upper Boundary Value
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       keySize = OWNER_PSK_LENGTH_256
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCDeleteACLList for m_Acl1
 *                  9. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCredTest_btc, ProvisionPairwiseDevicesKeySize_UBV_P)
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
    size_t keySize = OWNER_PSK_LENGTH_256;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
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
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Positively with ACL1 NULL
 * @objective       Test OCProvisionPairwiseDevices negatively with pDev1Acl as NULL
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       pDev1Acl = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCredTest_btc, ProvisionPairwiseDevicesAcl1_NV_P)
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
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, NULL, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
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
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Positively with ACL2 NULL
 * @objective       Test OCProvisionPairwiseDevices negatively with pDev2Acl as NULL
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       pDev2Acl = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCDeleteACLList for m_Acl1
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCredTest_btc, ProvisionPairwiseDevicesAcl2_NV_P)
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
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, NULL, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteACLList(m_Acl1))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Positively with callback as NULL
 * @objective       Test OCProvisionPairwiseDevices negatively with resultCallback as NULL
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCDeleteACLList for m_Acl1
 *                  9. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCredTest_btc, ProvisionPairwiseDevicesCB_NV_N)
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
                    device1, m_Acl1, device2, m_Acl2, NULL, OC_STACK_INVALID_CALLBACK))
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
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Negatively with Device1 NULL
 * @objective       Test OCProvisionPairwiseDevices negatively with pDev1 as NULL
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       pDev1 = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCDeleteACLList for m_Acl1
 *                  9. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCredTest_btc, ProvisionPairwiseDevicesDev1_NV_N)
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
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    NULL, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_INVALID_PARAM))
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
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Negatively with Device2 NULL
 * @objective       Test OCProvisionPairwiseDevices negatively with pDev2 as NULL
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       pDev2 = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCDeleteACLList for m_Acl1
 *                  9. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCredTest_btc, ProvisionPairwiseDevicesDev2_NV_N)
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
    m_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &m_OwnList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, NULL, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_INVALID_PARAM))
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
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Negatively with keysize < Min
 * @objective       Test OCProvisionPairwiseDevices negatively with keySize is out of Lower Boundary Value
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       keySize is out of Lower Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCDeleteACLList for m_Acl1
 *                  9. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCredTest_btc, ProvisionPairwiseDevicesDeviceKeySize_LOBV_N)
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
    size_t keySize = OWNER_PSK_LENGTH_128-1;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_INVALID_PARAM))
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
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Negatively with keysize > Max
 * @objective       Test OCProvisionPairwiseDevices negatively with keySize is out of Upper Boundary Value
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       keySize is out of Upper Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCDeleteACLList for m_Acl1
 *                  9. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkCredTest_btc, ProvisionPairwiseDevicesDeviceKeySize_UOBV_N)
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
    size_t keySize = OWNER_PSK_LENGTH_256 + 1;

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, m_Acl1, device2, m_Acl2, PMCsdkHelper::provisionPairwiseCB, OC_STACK_INVALID_PARAM))
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
