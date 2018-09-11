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
#include "PMCsdkHelper.h"
#include "PMCsdkUtilityHelper.h"

class PMCsdkPairwiseTest_btc: public ::testing::Test
{
protected:

    OCProvisionDev_t *m_OwnList;
    OicSecAcl_t *m_Acl, *m_Acl1, *m_Acl2;
    PMCsdkHelper m_PMHelper;
    OCProvisionDev_t *m_Device1 = NULL;
    OCProvisionDev_t *m_Device2 = NULL;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        PMCsdkUtilityHelper::removeAllResFile();
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O1_PAIRWISE_BACKUP, JUSTWORKS_SERVER_CBOR_O1);
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O2_PAIRWISE_BACKUP, JUSTWORKS_SERVER_CBOR_O2);
        CommonUtil::copyFile(CLIENT_CBOR_01_PAIRWISE_BACKUP, CLIENT_CBOR_01);
        CommonUtil::copyFile(DEVICE_PROP_CBOR_01_PAIRWISE_BACKUP, DEVICE_PROP_CBOR_01);
        CommonUtil::copyFile(CLIENT_DB_01_PAIRWISE_BACKUP, CLIENT_DB_01);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::launchApp(JUSTWORKS_SERVER1);
        CommonUtil::launchApp(JUSTWORKS_SERVER2);
        CommonUtil::waitInSecond(DELAY_LONG);
        m_OwnList = NULL;

        if (!m_PMHelper.initProvisionClient(OTM_JUSTWORK, (char*) CLIENT_DB_01))
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
            return;
        }

        if (!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
            return;
        }

        m_Device1 = m_OwnList;
        m_Device2 = m_OwnList->next;
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);

        if(!m_PMHelper.deleteDiscoveredDevices(m_OwnList))
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
            return;
        }
    }
};

/**
 * @since           2015-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCUnlinkDevices  positively.
 * @target          OCStackResult OCUnlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1, const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for OCUnlinkDevices
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCUnlinkDevices
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, UnlinkDevices_RSV_SRC_P)
{
    if (!m_PMHelper.unlinkDevices((void*)ctxUnlinkDevice, m_Device1, m_Device2,
                    PMCsdkCallbackHelper::provisionDeleteCB, OC_STACK_OK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCUnlinkDevices  negatively with m_Device1 as NULL.
 * @target          OCStackResult OCUnlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1, const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback)
 * @test_data       pTargetDev1 = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCUnlinkDevices
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, UnlinkDevicesDev1_NV_N)
{
    if (!m_PMHelper.unlinkDevices((void*)ctxUnlinkDevice, NULL, m_Device2,
                    PMCsdkCallbackHelper::provisionDeleteCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCUnlinkDevices  negatively with m_Device2 as NULL.
 * @target          OCStackResult OCUnlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1, const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback)
 * @test_data       pTargetDev2 = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCUnlinkDevices
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, UnlinkDevicesDev2_NV_N)
{
    if (!m_PMHelper.unlinkDevices((void*)ctxUnlinkDevice, m_Device1, NULL,
                    PMCsdkCallbackHelper::provisionDeleteCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCUnlinkDevices  negatively with resultCallback as NULL.
 * @target          OCStackResult OCUnlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1, const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCUnlinkDevices
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, UnlinkDevicesCB_NV_N)
{
    if (!m_PMHelper.unlinkDevices((void*)ctxUnlinkDevice, m_Device1, m_Device2,
                    NULL, OC_STACK_INVALID_CALLBACK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCRemoveDevice positively.
 * @target          OCStackResult OCRemoveDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       Regular Data for OCRemoveDevice
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCRemoveDevice
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, RemoveDevice_SRC_RV_P)
{
    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT, m_Device1,
                    PMCsdkCallbackHelper::provisionDeleteCB, OC_STACK_OK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCRemoveDevice Positively with waitiTimeForownedDeviceDiscovery as Lower Boundary Value.
 * @target          OCStackResult OCRemoveDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCRemoveDevice
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, RemoveDeviceTime_LBV_P)
{
    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT_ONE, m_Device1,
                    PMCsdkCallbackHelper::provisionDeleteCB, OC_STACK_OK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCRemoveDevice negatively with waitiTimeForownedDeviceDiscovery as LOBV (Time = ZERO).
 * @target          OCStackResult OCRemoveDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCRemoveDevice
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, RemoveDevice_LOBV_N)
{
    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT_ZERO, m_Device1,
                    PMCsdkCallbackHelper::provisionDeleteCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCRemoveDevice negatively with pTargetDev  as NULL.
 * @target          OCStackResult OCRemoveDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       pTargetDev = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCRemoveDevice
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, RemoveDeviceTargetDev_NV_N)
{
    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT, NULL,
                    PMCsdkCallbackHelper::provisionDeleteCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCRemoveDevice negatively with callback  as NULL.
 * @target          OCStackResult OCRemoveDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCRemoveDevice
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, RemoveDeviceCB_NV_N)
{
    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT, m_Device1,
                    NULL, OC_STACK_INVALID_CALLBACK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCRemoveDeviceWithUuid positively.
 * @target          OCStackResult OCRemoveDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback)
 * @test_data       Regular Data for OCRemoveDeviceWithUuid
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCRemoveDeviceWithUuid
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, RemoveDeviceWithUuid_SRC_RV_P)
{
    OicUuid_t deviceUuid = m_Device1->doxm->deviceID;

    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT, &deviceUuid, PMCsdkCallbackHelper::provisionDeleteCB, OC_STACK_OK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCRemoveDeviceWithUuid negatively with wrong UuId.
 * @target          OCStackResult OCRemoveDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback)
 * @test_data       Wrong Uuid for OCRemoveDeviceWithUuid
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCRemoveDeviceWithUuid
 *                  6. call OCDeleteACLList for m_Acl1
 *                  7. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, RemoveDeviceWithWrongUuid_RV_N)
{
    OicUuid_t deviceUuid;

    if(!m_PMHelper.convertStrToUuid(DEFAULT_OWNER_ID, &deviceUuid,OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT, &deviceUuid, PMCsdkCallbackHelper::provisionDeleteCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCRemoveDeviceWithUuid negatively with NULL UuId.
 * @target          OCStackResult OCRemoveDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback)
 * @test_data       NULL Uuid for OCRemoveDeviceWithUuid
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCRemoveDeviceWithUuid
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, RemoveDeviceWithUuidUuid_NV_N)
{
    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT, NULL, PMCsdkCallbackHelper::provisionDeleteCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCRemoveDeviceWithUuid negatively with waitiTimeForownedDeviceDiscovery as LOBV (Time = ZERO).
 * @target          OCStackResult OCRemoveDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCRemoveDeviceWithUuid
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, RemoveDeviceWithUuid_LOBV_N)
{
    OicUuid_t deviceUuid = m_Device1->doxm->deviceID;

    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT_ZERO, &deviceUuid, PMCsdkCallbackHelper::provisionDeleteCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCRemoveDeviceWithUuid positively with waitiTimeForownedDeviceDiscovery as Lower Boundary Value
 * @target          OCStackResult OCRemoveDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCRemoveDeviceWithUuid
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, RemoveDeviceWithUuid_LBV_P)
{
    OicUuid_t deviceUuid = m_Device1->doxm->deviceID;

    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT_ONE, &deviceUuid, PMCsdkCallbackHelper::provisionDeleteCB, OC_STACK_OK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCRemoveDeviceWithUuid negatively with callback  as NULL.
 * @target          OCStackResult OCRemoveDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCRemoveDeviceWithUuid
 * @post_condition  None
 * @expected        OCRemoveDeviceWithUuid will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, RemoveDeviceWithUuidCB_NV_N)
{
    OicUuid_t deviceUuid = m_Device1->doxm->deviceID;

    if (!m_PMHelper.removeDeviceWithUuid((void*)ctxRemoveDeviceWithUuid, DISCOVERY_TIMEOUT, &deviceUuid, NULL, OC_STACK_INVALID_CALLBACK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCResetDevice positively.
 * @target          OCStackResult OCResetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       Regular Data for OCResetDevice
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCResetDevice
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, ResetDevice_SRC_RV_P)
{
    if (!m_PMHelper.resetDevice((void*)ctxResetDevice, DISCOVERY_TIMEOUT, m_Device1,
                    PMCsdkHelper::syncDeviceCB, OC_STACK_OK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCResetDevice positively with waitiTimeForownedDeviceDiscovery as Lower Boundary Value
 * @target          OCStackResult OCResetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCResetDevice
 *                  9. call OCDeleteACLList for m_Acl1
 *                 10. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, ResetDevice_LBV_P)
{
    if (!m_PMHelper.resetDevice((void*)ctxResetDevice, DISCOVERY_TIMEOUT_ONE, m_Device1,
                    PMCsdkHelper::syncDeviceCB, OC_STACK_OK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCResetDevice negatively with waitiTimeForownedDeviceDiscovery as LOBV (Time = ZERO).
 * @target          OCStackResult OCResetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCResetDevice
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, ResetDevice_LOBV_N)
{
    if (!m_PMHelper.resetDevice((void*)ctxResetDevice, DISCOVERY_TIMEOUT_ZERO, m_Device1,
                    PMCsdkHelper::syncDeviceCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCResetDevice negatively with TargetDevice as NULL
 * @target          OCStackResult OCResetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       NULL TargetDevice for OCResetDevice
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCResetDevice
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, ResetDeviceTargetDevice_NV_N)
{
    if (!m_PMHelper.resetDevice((void*)ctxResetDevice, DISCOVERY_TIMEOUT, NULL,
                    PMCsdkHelper::syncDeviceCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCResetDevice positively with TargetDevice No 2.
 * @target          OCStackResult OCResetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       TargetDevice No 2
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCResetDevice
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, ResetDeviceTargetm_Device2_SRC_RV_P)
{
    if (!m_PMHelper.resetDevice((void*)ctxResetDevice, DISCOVERY_TIMEOUT, m_Device2,
                    PMCsdkHelper::syncDeviceCB, OC_STACK_OK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCResetDevice negatively with callback as NULL
 * @target          OCStackResult OCResetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery, const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCResetDevice
 *                  6. call OCDeleteACLList for m_Acl1
 *                  7. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCResetDevice will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, ResetDeviceCB_NV_N)
{
    if (!m_PMHelper.resetDevice((void*)ctxResetDevice, DISCOVERY_TIMEOUT, m_Device2,
                    NULL, OC_STACK_INVALID_CALLBACK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCResetSVRDB positively.
 * @target          OCStackResult OCResetSVRDB(void)
 * @test_data       Regular Data for OCResetSVRDB
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCResetSVRDB
 *                  6. call OCDeleteACLList for m_Acl1
 *                  7. call OCDeleteACLList for m_Acl2
 * @post_condition  None
 * @expected        OCResetSVRDB will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, ResetSVRDB_SRC_RV_P)
{
    if (!m_PMHelper.resetSVRDB(OC_STACK_OK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetLinkedStatus positively.
 * @target          OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @test_data       Regular Data for OCGetLinkedStatus
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetLinkedStatus
 * @post_condition  None
 * @expected        OCGetLinkedStatus will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCGetLinkedStatus_RSV_SRC_P)
{
    OCUuidList_t* uuidList = NULL;
    size_t numOfDevices = 0;

    if (!m_PMHelper.getLinkedStatus(&m_Device1->doxm->deviceID, &uuidList, &numOfDevices, OC_STACK_OK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetLinkedStatus positively.
 * @target          OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @test_data       uuidOfDevice = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetLinkedStatus
 * @post_condition  None
 * @expected        OCGetLinkedStatus will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCGetLinkedStatusDeviceID_NV_N)
{
    OCUuidList_t* uuidList = NULL;
    size_t numOfDevices = 0;

    if (!m_PMHelper.getLinkedStatus(NULL, &uuidList, &numOfDevices, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetLinkedStatus negatively with uuidList as NULL
 * @target          OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @test_data       uuidList = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetLinkedStatus
 * @post_condition  None
 * @expected        OCGetLinkedStatus will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCGetLinkedStatusUuidListNV_N)
{
    size_t numOfDevices = 0;

    if (!m_PMHelper.getLinkedStatus(&m_Device1->doxm->deviceID, NULL, &numOfDevices, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetLinkedStatus negatively with numOfDevices as NULL
 * @target          OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @test_data       numOfDevices = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetLinkedStatus
 * @post_condition  None
 * @expected        OCGetLinkedStatus will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCGetLinkedStatusNumDev_NV_N)
{
    OCUuidList_t* uuidList = NULL;

    if (!m_PMHelper.getLinkedStatus(&m_Device1->doxm->deviceID, &uuidList, NULL ,OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetCredResource positively
 * @target          OCStackResult OCGetCredResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       Regular data of  OCGetCredResource API
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetCredResource
 * @post_condition  None
 * @expected        OCGetCredResource will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCGetCredResource_RV_SRC_P)
{
    if (!m_PMHelper.getCredResource(NULL, m_Device1, PMCsdkCallbackHelper::provisionGetCB, OC_STACK_OK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetCredResource negatively with selectedDeviceInfo as NULL
 * @target          OCStackResult OCGetCredResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       selectedDeviceInfo = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetCredResource
 * @post_condition  None
 * @expected        OCGetCredResource will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCGetCredResourceDev_NV_N)
{
    if (!m_PMHelper.getCredResource(NULL, NULL, PMCsdkCallbackHelper::provisionGetCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetCredResource negatively with resultCallback as NULL
 * @target          OCStackResult OCGetCredResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetCredResource
 * @post_condition  None
 * @expected        OCGetCredResource will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCGetCredResourceCb_NV_N)
{
    if (!m_PMHelper.getCredResource(NULL, m_Device1, NULL, OC_STACK_INVALID_CALLBACK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetACLResource positively
 * @target          OCStackResult OCGetACLResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,OCProvisionResultCB resultCallback)
 * @test_data       Regular data of  OCGetACLResource API
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetACLResource
 * @post_condition  None
 * @expected        OCGetACLResource will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCGetACLResource_RV_SRC_P)
{
    if (!m_PMHelper.getACLResource(NULL, m_Device1, PMCsdkCallbackHelper::provisionGetCB, OC_STACK_OK))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetACLResource negatively with selectedDeviceInfo as NULL
 * @target          OCStackResult OCGetACLResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,OCProvisionResultCB resultCallback)
 * @test_data       selectedDeviceInfo = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetACLResource
 * @post_condition  None
 * @expected        OCGetACLResource will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCGetACLResource_NV_N)
{
    if (!m_PMHelper.getACLResource(NULL, NULL, PMCsdkCallbackHelper::provisionGetCB, OC_STACK_INVALID_PARAM))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetACLResource negatively with resultCallback as NULL
 * @target          OCStackResult OCGetACLResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetACLResource
 * @post_condition  None
 * @expected        OCGetACLResource will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCGetACLResourceCb_NV_N)
{
    if (!m_PMHelper.getACLResource(NULL, m_Device1, NULL, OC_STACK_INVALID_CALLBACK))
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
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOxmAllowStatus
 * @post_condition  none
 * @expected        OCSetOxmAllowStatus will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, setOxmAllowStatus_LOBV_N)
{
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCSaveACL positively.
 * @target          OCStackResult OCSaveACL(const OicSecAcl_t* acl)
 * @test_data       regular data
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  5. call OCDiscoverOwnedDevices
 *                  6. call OCSaveACL
 *                  7. call OCDeleteACLList for m_Acl
 * @post_condition  None
 * @expected        OCSaveACL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, saveACL_SRC_P)
{
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCSaveACL Negatively with acl NULL value.
 * @target          OCStackResult OCSaveACL(const OicSecAcl_t* acl)
 * @test_data       acl=NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSaveACL
 * @post_condition  None
 * @expected        OCSaveACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, saveACLAcl_NV_N)
{
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @objective       Test OCDeleteUuidList positively.
 * @target          void OCDeleteUuidList(OCUuidList_t* pList)
 * @test_data       Regular Data for OCDeleteUuidList
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetLinkedStatus
 *                  6. call OCDeleteUuidList
 * @post_condition  None
 * @expected        OCDeleteUuidList will not throw Exception
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCDeleteUuidList_RSV_SRC_P)
{
    OCUuidList_t* uuidList = NULL;
    size_t numOfDevices = 0;

    if (!m_PMHelper.getLinkedStatus(&m_Device1->doxm->deviceID, &uuidList, &numOfDevices, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }

    if (!m_PMHelper.deleteUuidList(uuidList))
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCGetLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList, size_t* numOfDevices)
 * @objective       Test OCDeleteUuidList negatively with pList=NULL.
 * @target          void OCDeleteUuidList(OCUuidList_t* pList)
 * @test_data       pList=NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDeleteUuidList
 * @post_condition  None
 * @expected        OCDeleteUuidList will not throw Exception
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCDeleteUuidListpList_NV_P)
{
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
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCDeleteACLList positively.
 * @target          void OCDeleteACLList(OicSecAcl_t* pAcl)
 * @test_data       regular data
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCDeleteACLList
 * @post_condition  None
 * @expected        OCDeleteACLList will not throw Exception
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCDeleteACLList_SRC_P)
{
    m_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.deleteACLList(m_Acl1))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-12
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetCSRResource positively
 * @target          OCStackResult OCGetCSRResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OCGetCSRResultCB resultCallback)
 * @test_data       Regular data of  OCGetCSRResource API
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetCSRResource
 * @post_condition  None
 * @expected        OCGetCSRResource will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCGetCSRResource_RV_SRC_P)
{
    if (!m_PMHelper.getCSRResource(NULL, m_Device1, (OCGetCSRResultCB)PMCsdkCallbackHelper::provisionGetCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-18
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetACL2Resource positively
 * @target          OCStackResult OCGetACL2Resource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,OCProvisionResultCB resultCallback)
 * @test_data       Regular data of  OCGetACL2Resource API
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetACL2Resource
 * @post_condition  None
 * @expected        OCGetACL2Resource will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkPairwiseTest_btc, OCGetACL2Resource_RV_SRC_P)
{
    if (!m_PMHelper.getACL2Resource(NULL, m_Device1, PMCsdkCallbackHelper::provisionGetCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif
