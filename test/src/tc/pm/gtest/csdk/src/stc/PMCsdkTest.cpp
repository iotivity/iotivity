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

class PMCsdkTest_stc: public ::testing::Test
{
protected:
    OCProvisionDev_t *m_OwnList, *m_UnownList;
    OicSecAcl_t *m_Acl, *m_Acl1, *m_Acl2;
    PMCsdkHelper m_PMHelper;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        PMCsdkUtilityHelper::removeAllResFile();
        CommonUtil::waitInSecond(DELAY_LONG);
        CommonUtil::rmFile(CLIENT_DATABASE);
        CommonUtil::rmFile(JUSTWORKS_SERVER1_CBOR);
        CommonUtil::rmFile(JUSTWORKS_SERVER2_CBOR);
        CommonUtil::rmFile(RANDOMPIN_SERVER_CBOR);
        CommonUtil::rmFile(CLIENT_CBOR);
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Negatively with Same 2 Device
 * @objective       Test OCProvisionPairwiseDevices negatively calling it multiple time
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       call OCProvisionPairwiseDevices second time
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 *                  8. call OCProvisionPairwiseDevices
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_INVALID_PARAM while calling for the second time.
 */
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, ProvisionPairwiseDevicesMultipleTime_EG_N)
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

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, NULL, device2, NULL, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device1, NULL, device2, NULL, PMCsdkHelper::provisionPairwiseCB, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCUnlinkDevices  negatively trying to remove client device.
 * @target          OCStackResult OCUnlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1, const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback)
 * @test_data       pTargetDev1 = client device
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCUnlinkDevices
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, UnlinkDevicesClientDev_EG_N)
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

    OCProvisionDev_t *device1 = m_OwnList->next->next;
    OCProvisionDev_t *device2 = m_OwnList;

    if (!m_PMHelper.unlinkDevices((void*)ctxUnlinkDevice, device1, device2,
                    PMCsdkHelper::unlinkDevicesCB, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCUnlinkDevices  negatively trying to unlink same device.
 * @target          OCStackResult OCUnlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1, const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback)
 * @test_data       pTargetDev1 = pTargetDev2
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCUnlinkDevices
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, UnlinkDevicesSameDevicesDev_EG_N)
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

    if (!m_PMHelper.unlinkDevices((void*)ctxUnlinkDevice, device1, device1,
                    PMCsdkHelper::unlinkDevicesCB, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCRemoveDevice negatively with client dev as OCRemoveDevice.
 * @target          OCStackResult OCRemoveDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       pTargetDev = client device
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCRemoveDevice
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, RemoveDeviceClientDev_EG_N)
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

    OCProvisionDev_t *device1 = m_OwnList->next->next;

    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT, device1,
                    PMCsdkHelper::removeDeviceCB, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCGetLinkedStatus positively with  unformatted uuidOfDevice
 * @target          OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @test_data       OCGetLinkedStatus with a reference of unformatted Device ID
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
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, OCGetLinkedStatusUuid_URSV_N)
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

    OicUuid_t uuidOfDevice;

    for(int i=0; i <= 15; i++)
    {
        uuidOfDevice.id[i]= 1;
    }

    OCUuidList_t* uuidList = NULL;
    size_t numOfDevices;

    if (!m_PMHelper.getLinkedStatus(&uuidOfDevice, &uuidList, &numOfDevices, OC_STACK_INVALID_PARAM))
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
 * @since           2016-07-25
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDoOwnershipTransfer negatively calling the API multiple times.
 * @target          OCDoOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices,OCProvisionResultCB resultCallback)
 * @test_data       OIC_JUSTWORKS ownership transfer
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDoOwnershipTransfer
 * @post_condition  None
 * @expected        OCDoOwnershipTransfer will return OC_STACK_ERROR while calling it second time
 */
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, OCDoOwnershipTransferJustWorksMultipleTimes_EG_N)
{
    if (!m_PMHelper.initProvisionClient(OTM_JUSTWORK))
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

    if (!m_PMHelper.doOwnerShipTransfer((void*)g_ctx, &m_UnownList,
                    PMCsdkHelper::ownershipTransferCB, OC_STACK_ERROR, false))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Negatively with Same 2 Device
 * @objective       Test OCProvisionPairwiseDevices negatively with pDev1 = pDev2
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       pDev1 = pDev2
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCProvisionPairwiseDevices
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, ProvisionPairwiseDevicesSameDevices_EG_N)
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

    if (!m_PMHelper.provisionPairwiseDevices((void*)ctxProvPairwise, type, keySize,
                    device2, NULL, device2, NULL, PMCsdkHelper::provisionPairwiseCB, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCProvisionCredentials negatively calling it multiple times.
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
 *                  8. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM while calling it second time
 */
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, ProvisionCredentialMultipleTimes_EG_N)
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
        return;
    }

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkHelper::provisionCredCB, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCUnlinkDevices  negatively calling the API multiple times.
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
 *                  9. call OCUnlinkDevices
 *                 10. call OCDeleteACLList for m_Acl1
 *                 11. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_INVALID_PARAM while calling it second time
 */
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, UnlinkDevicesMultipleTimes_EG_N)
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
        return;
    }

    if (!m_PMHelper.unlinkDevices((void*)ctxUnlinkDevice, device1, device2,
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
 * @objective       Test OCRemoveDevice negatively calling the API multiple times.
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
 *                  9. call OCRemoveDevice
 *                 10. call OCDeleteACLList for m_Acl1
 *                 11. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_INVALID_PARAM while calling it second time
 */
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, RemoveDeviceTimeMultipleTimes_EG_N)
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
        return;
    }

    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT_ONE, device1,
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
 * @since           2016-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCUnlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1, const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @objective       Test OCUnlinkDevices positively verifying it with OCGetLinkedStatus.
 * @target          OCStackResult OCUnlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1, const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback)
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
 *                  9. call OCUnlinkDevices
 *                 10. call OCGetLinkedStatus
 *                 11. call OCDeleteACLList for m_Acl1
 *                 12. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        numOfDevices provided by OCGetLinkedStatus will be different
 */
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, OCGetLinkedStatusBeforenAfterUnlinkDev_RSV_SRC_P)
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

    if (!m_PMHelper.getLinkedStatus(&device1->doxm->deviceID, &uuidList, &numOfDevices, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(0 == numOfDevices)
    {
        SET_FAILURE("No Device has been linked.");
        return;
    }

    size_t tempNumOfDevices = numOfDevices;

    if (!m_PMHelper.unlinkDevices((void*)ctxUnlinkDevice, device1, device2,
                    NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    uuidList = NULL;
    numOfDevices = 0;

    if (!m_PMHelper.getLinkedStatus(&device1->doxm->deviceID, &uuidList, &numOfDevices, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(tempNumOfDevices > numOfDevices)
    {
        SET_FAILURE("No Device has been unlinked.");
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
 * @since           2016-07-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL positively with no permission
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       ACL Permission CRUDN = No Permission
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
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, ProvisionAclPermission_LBV_P)
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

    m_Acl = createAcl(DEVICE_INDEX_ONE, NO_PERMISSION, &m_OwnList);

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
 * @since           2016-07-27
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL positively with no permission
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       ACL Permission CRUDN = 65535
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
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, ProvisionAclPermission_UBV_P)
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

    m_Acl = createAcl(DEVICE_INDEX_ONE, MAX_PERMISSION_RANGE, &m_OwnList);

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
 * @since           2017-01-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @objective       Test OCRemoveDeviceWithUuid negatively calling the API multiple times.
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
 *                  9. call OCRemoveDeviceWithUuid
 *                 10. call OCDeleteACLList for m_Acl1
 *                 11. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_INVALID_PARAM while calling it second time
 */
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, RemoveDeviceWithUuidMultipleTimes_EG_N)
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

    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT_ONE, &deviceUuid, PMCsdkHelper::removeDeviceCB, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCResetDevice negatively calling the API multiple times.
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
 *                  9. call OCResetDevice
 *                 10. call OCDeleteACLList for m_Acl1
 *                 11. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_OK while calling it second time
 */
#if defined(__LINUX__) || defined(_TIZEN_)
TEST_F(PMCsdkTest_stc, ResetDeviceMutipleTimes_SRC_P)
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
