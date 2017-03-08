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

class PMCsdkOtmTest_btc: public ::testing::Test
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
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDiscoverUnownedDevices positively with waittime = DISCOVERY_TIMEOUT
 * @target          OCStackResult OCDiscoverUnownedDevices(unsigned short waittime, OCProvisionDev_t **ppList)
 * @test_data       waittime = DISCOVERY_TIMEOUT
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 * @post_condition  none
 * @expected        OCDiscoverUnownedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCDiscoverUnownedDevices_RV_SRC_P)
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
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDiscoverUnownedDevices positively with Lower Boundary value of Time.
 * @target          OCStackResult OCDiscoverUnownedDevices(unsigned short waittime, OCProvisionDev_t **ppList)
 * @test_data       waittime = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 * @post_condition  None
 * @expected        OCDiscoverUnownedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCDiscoverUnownedDevicesTime_LBV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT_ONE,
                    &m_UnownList, OC_STACK_OK))
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
 * @objective       Test OCDiscoverUnownedDevices negatively with  Out of Lower Boundary value of Time.
 * @target          OCStackResult OCDiscoverUnownedDevices(unsigned short waittime, OCProvisionDev_t **ppList)
 * @test_data       waittime = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 * @post_condition  None
 * @expected        OCDiscoverUnownedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCDiscoverUnownedDevices_LOBV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT_ZERO,
                    &m_UnownList, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCDiscoverUnownedDevices Negatively with ppList as NULL
 * @target          OCStackResult OCDiscoverUnownedDevices(unsigned short waittime, OCProvisionDev_t **ppList)
 * @test_data       ppList = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 * @post_condition  None
 * @expected        OCDiscoverUnownedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCDiscoverUnownedDevices_ECRC_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,
                    NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDiscoverOwnedDevices positively.
 * @target          OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       timeout = DISCOVERY_TIMEOUT
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 * @post_condition  none
 * @expected        OCDiscoverOwnedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCDiscoverOwnedDevices_RV_SRC_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
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
 * @objective       Test OCDiscoverOwnedDevices positively with Lower Boundary Value of Time
 * @target          OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       timeout = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 * @post_condition  None
 * @expected        discoverOwnedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCDiscoverOwnedDevicesTime_LBV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT_ONE, &m_OwnList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDiscoverOwnedDevices negatively with Lower Outer Boundary Value of Time.
 * @target          OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data        timeout = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 * @post_condition  None
 * @expected        OCDiscoverOwnedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCDiscoverOwnedDevicesTime_ECRC_LOBV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT_ZERO, &m_OwnList, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCDiscoverOwnedDevices negatively with ppList NULL.
 * @target          OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       OCProvisionDev_t **ppList = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 * @post_condition  None
 * @expected        OCDiscoverOwnedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCDiscoverOwnedDevices_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT_ZERO, NULL, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCDiscoverSingleDevice positively with waittime = DISCOVERY_TIMEOUT
 * @target          OCStackResult OCDiscoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID,OCProvisionDev_t **ppFoundDevice)
 * @test_data       waittime = DISCOVERY_TIMEOUT
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCDiscoverSingleDevice
 * @post_condition  none
 * @expected        OCDiscoverSingleDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, discoverSingleDevice_SRC_P)
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
    OicUuid_t deviceUuid = device1->doxm->deviceID;

    if (!m_PMHelper.discoverSingleDevice(DISCOVERY_TIMEOUT,&deviceUuid,
                    &m_targetDevice, OC_STACK_OK))
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
 * @objective       Test OCDiscoverSingleDevice Negatively with ppFoundDevice = NULL
 * @target          OCStackResult OCDiscoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID,OCProvisionDev_t **ppFoundDevice)
 * @test_data       ppFoundDevice = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCDiscoverSingleDevice
 * @post_condition  none
 * @expected        OCDiscoverSingleDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, discoverSingleDeviceFoundDevice_NV_N)
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
    OicUuid_t deviceUuid = device1->doxm->deviceID;

    if (!m_PMHelper.discoverSingleDevice(DISCOVERY_TIMEOUT,&deviceUuid,
                    NULL, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCDiscoverSingleDevice positively with waittime = DISCOVERY_TIMEOUT_ONE
 * @target          OCStackResult OCDiscoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID,OCProvisionDev_t **ppFoundDevice)
 * @test_data       waittime = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCDiscoverSingleDevice
 * @post_condition  none
 * @expected        OCDiscoverSingleDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, discoverSingleDevice_LBV_P)
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
    OicUuid_t deviceUuid = device1->doxm->deviceID;

    if (!m_PMHelper.discoverSingleDevice(DISCOVERY_TIMEOUT_ONE,&deviceUuid,
                    &m_targetDevice, OC_STACK_OK))
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
 * @objective       Test OCDiscoverSingleDevice Negatively with waittime = DISCOVERY_TIMEOUT_ZERO
 * @target          OCStackResult OCDiscoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID,OCProvisionDev_t **ppFoundDevice)
 * @test_data       waittime = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCDiscoverSingleDevice
 * @post_condition  none
 * @expected        OCDiscoverSingleDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, discoverSingleDevice_LOBV_N)
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
    OicUuid_t deviceUuid = device1->doxm->deviceID;

    if (!m_PMHelper.discoverSingleDevice(DISCOVERY_TIMEOUT_ZERO,&deviceUuid,
                    &m_targetDevice, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCDiscoverSingleDevice Positively with Unowned device ID
 * @target          OCStackResult OCDiscoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID,OCProvisionDev_t **ppFoundDevice)
 * @test_data       deviceID = Unowned device ID
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDiscoverSingleDevice
 * @post_condition  none
 * @expected        OCDiscoverSingleDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, discoverSingleDeviceUnowned_SRC_P)
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

    OCProvisionDev_t *device1 = m_UnownList;
    OicUuid_t deviceUuid = device1->doxm->deviceID;

    if (!m_PMHelper.discoverSingleDevice(DISCOVERY_TIMEOUT,&deviceUuid,
                    &m_targetDevice, OC_STACK_OK))
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
 * @objective       Test OCDiscoverSingleDevice Negatively with deviceID=NULL
 * @target          OCStackResult OCDiscoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID,OCProvisionDev_t **ppFoundDevice)
 * @test_data       deviceID = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverSingleDevice
 * @post_condition  none
 * @expected        OCDiscoverSingleDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, discoverSingleDeviceDeviceID_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverSingleDevice(DISCOVERY_TIMEOUT,NULL,
                    &m_targetDevice, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-07
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCDiscoverSingleDeviceInUnicast positively with waittime = DISCOVERY_TIMEOUT
 * @target          OOCStackResult OCDiscoverSingleDeviceInUnicast(unsigned short timeout, const OicUuid_t* deviceID,const char* hostAddress, OCConnectivityType connType,OCProvisionDev_t **ppFoundDevice)
 * @test_data       waittime = DISCOVERY_TIMEOUT
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCDiscoverSingleDeviceInUnicast
 * @post_condition  none
 * @expected        OCDiscoverSingleDeviceInUnicast will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, discoverSingleDeviceInUnicast_SRC_P)
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
    OicUuid_t deviceUuid = device1->doxm->deviceID;
    char* hostAddr = device1->endpoint.routeData;

    if (!m_PMHelper.discoverSingleDeviceInUnicast(DISCOVERY_TIMEOUT,&deviceUuid,hostAddr,CT_DEFAULT,
                    &m_targetDevice, OC_STACK_OK))
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
 * @objective       Test OCDiscoverSingleDeviceInUnicast positively with waittime = DISCOVERY_TIMEOUT_ONE
 * @target          OOCStackResult OCDiscoverSingleDeviceInUnicast(unsigned short timeout, const OicUuid_t* deviceID,const char* hostAddress, OCConnectivityType connType,OCProvisionDev_t **ppFoundDevice)
 * @test_data       waittime = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCDiscoverSingleDeviceInUnicast
 * @post_condition  none
 * @expected        OCDiscoverSingleDeviceInUnicast will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, discoverSingleDeviceInUnicast_LBV_P)
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
    OicUuid_t deviceUuid = device1->doxm->deviceID;
    char* hostAddr = device1->endpoint.routeData;

    if (!m_PMHelper.discoverSingleDeviceInUnicast(DISCOVERY_TIMEOUT_ONE,&deviceUuid,hostAddr,CT_DEFAULT,
                    &m_targetDevice, OC_STACK_OK))
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
 * @objective       Test OCDiscoverSingleDeviceInUnicast negatively with waittime = DISCOVERY_TIMEOUT_ZERO
 * @target          OOCStackResult OCDiscoverSingleDeviceInUnicast(unsigned short timeout, const OicUuid_t* deviceID,const char* hostAddress, OCConnectivityType connType,OCProvisionDev_t **ppFoundDevice)
 * @test_data       waittime = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCDiscoverSingleDeviceInUnicast
 * @post_condition  none
 * @expected        OCDiscoverSingleDeviceInUnicast will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, discoverSingleDeviceInUnicast_LOBV_N)
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
    OicUuid_t deviceUuid = device1->doxm->deviceID;
    char* hostAddr = device1->endpoint.routeData;

    if (!m_PMHelper.discoverSingleDeviceInUnicast(DISCOVERY_TIMEOUT_ZERO,&deviceUuid,hostAddr,CT_DEFAULT,
                    &m_targetDevice, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCDiscoverSingleDeviceInUnicast negatively with deviceID = NULL
 * @target          OOCStackResult OCDiscoverSingleDeviceInUnicast(unsigned short timeout, const OicUuid_t* deviceID,const char* hostAddress, OCConnectivityType connType,OCProvisionDev_t **ppFoundDevice)
 * @test_data       deviceID = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCDiscoverSingleDeviceInUnicast
 * @post_condition  none
 * @expected        OCDiscoverSingleDeviceInUnicast will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, discoverSingleDeviceInUnicastdeviceID_NV_N)
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
    char* hostAddr = device1->endpoint.routeData;

    if (!m_PMHelper.discoverSingleDeviceInUnicast(DISCOVERY_TIMEOUT,NULL,hostAddr,CT_DEFAULT,
                    &m_targetDevice, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCDiscoverSingleDeviceInUnicast negatively with hostAddress = NULL
 * @target          OOCStackResult OCDiscoverSingleDeviceInUnicast(unsigned short timeout, const OicUuid_t* deviceID,const char* hostAddress, OCConnectivityType connType,OCProvisionDev_t **ppFoundDevice)
 * @test_data       hostAddress = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCDiscoverSingleDeviceInUnicast
 * @post_condition  none
 * @expected        OCDiscoverSingleDeviceInUnicast will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, discoverSingleDeviceInUnicasthostAddress_NV_N)
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
    OicUuid_t deviceUuid = device1->doxm->deviceID;

    if (!m_PMHelper.discoverSingleDeviceInUnicast(DISCOVERY_TIMEOUT,&deviceUuid,NULL,CT_DEFAULT,
                    &m_targetDevice, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCDiscoverSingleDeviceInUnicast negatively with ppFoundDevice = NULL
 * @target          OOCStackResult OCDiscoverSingleDeviceInUnicast(unsigned short timeout, const OicUuid_t* deviceID,const char* hostAddress, OCConnectivityType connType,OCProvisionDev_t **ppFoundDevice)
 * @test_data       ppFoundDevice = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 *                  6. call OCDiscoverOwnedDevices
 *                  7. call OCDiscoverSingleDeviceInUnicast
 * @post_condition  none
 * @expected        OCDiscoverSingleDeviceInUnicast will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, discoverSingleDeviceInUnicastppFoundDevice_NV_N)
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
    OicUuid_t deviceUuid = device1->doxm->deviceID;
    char* hostAddr = device1->endpoint.routeData;

    if (!m_PMHelper.discoverSingleDeviceInUnicast(DISCOVERY_TIMEOUT,&deviceUuid,hostAddr,CT_DEFAULT,
                    NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @target          OCStackResult OCGetDevInfoFromNetwork(unsigned short waittime, OCProvisionDev_t** pOwnedDevList, OCProvisionDev_t** pUnownedDevList)
 * @objective       Test OCGetDevInfoFromNetwork positively with regular values.
 * @test_data       waittime = DISCOVERY_TIMEOUT
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCGetDevInfoFromNetwork
 * @post_condition  None
 * @expected        OCGetDevInfoFromNetwork will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCGetDevInfoFromNetwork_SRC_RV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverAllDevices(DISCOVERY_TIMEOUT, &m_OwnList, &m_UnownList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCGetDevInfoFromNetwork positively with Lower Boundary Value of Time
 * @target          OCStackResult OCGetDevInfoFromNetwork(unsigned short waittime, OCProvisionDev_t** pOwnedDevList, OCProvisionDev_t** pUnownedDevList)
 * @test_data       waittime = DISCOVERY_TIMEOUT_TWO
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCGetDevInfoFromNetwork
 * @post_condition  None
 * @expected        OCGetDevInfoFromNetwork will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCGetDevInfoFromNetwork_LBV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverAllDevices(DISCOVERY_TIMEOUT_TWO, &m_OwnList, &m_UnownList, OC_STACK_OK))
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
 * @objective       Test OCGetDevInfoFromNetwork negatively with Lower Outer Boundary Value of Time
 * @target          OCStackResult OCGetDevInfoFromNetwork(unsigned short waittime, OCProvisionDev_t** pOwnedDevList, OCProvisionDev_t** pUnownedDevList)
 * @test_data       waittime = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCGetDevInfoFromNetwork
 * @post_condition  None
 * @expected        OCGetDevInfoFromNetwork will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCGetDevInfoFromNetwork_LOBV_ECRC_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverAllDevices(DISCOVERY_TIMEOUT_ONE, &m_OwnList, &m_UnownList, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCGetDevInfoFromNetwork negatively with NULL Unowned Device List
 * @target          OCStackResult OCGetDevInfoFromNetwork(unsigned short waittime, OCProvisionDev_t** pOwnedDevList, OCProvisionDev_t** pUnownedDevList)
 * @test_data       pUnownedDevList = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCGetDevInfoFromNetwork
 * @post_condition  None
 * @expected        OCGetDevInfoFromNetwork will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCGetDevInfoFromNetworkOwned_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverAllDevices(DISCOVERY_TIMEOUT_ONE, &m_OwnList, NULL, OC_STACK_INVALID_PARAM))
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
 * @objective       Test OCGetDevInfoFromNetwork negatively with NULL Owned Device List
 * @target          OCStackResult OCGetDevInfoFromNetwork(unsigned short waittime, OCProvisionDev_t** pOwnedDevList, OCProvisionDev_t** pUnownedDevList)
 * @test_data       pOwnedDevList = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCGetDevInfoFromNetwork
 * @post_condition  None
 * @expected        OCGetDevInfoFromNetwork will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCGetDevInfoFromNetworkUnowned_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverAllDevices(DISCOVERY_TIMEOUT_ONE, NULL, &m_UnownList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOwnerTransferCallbackData positively for Just Work
 * @target          OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @test_data       oxm = OIC_JUST_WORKS & otmcb = OIC_JUST_WORKS related callbacks
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 * @procedure       call  OCSetOwnerTransferCallbackData
 * @post_condition  None
 * @expected        OCSetOwnerTransferCallbackData will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCSetOwnerTransferCallbackDataJustWork_SRC_RV_P)
{
    if (!m_PMHelper.initProvisionClient(OTM_NONE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t otmcb = otmCbRegister(OTM_JUSTWORK);

    OCStackResult expectedResult = OC_STACK_OK;
    OCStackResult actualResult = OCSetOwnerTransferCallbackData(OIC_JUST_WORKS, &otmcb);

    ASSERT_EQ(expectedResult, actualResult);
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOwnerTransferCallbackData positively with Representative Sample Value 0 for OIC_JUST_WORK
 * @target          OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @test_data       oxm = representative value OIC_JUST_WORKS & otmcb = OIC_JUST_WORKS related callbacks
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 * @procedure       call  OCSetOwnerTransferCallbackData
 * @post_condition  None
 * @expected        OCSetOwnerTransferCallbackData will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCSetOwnerTransferCallbackDataJustWork_REV_P)
{
    if (!m_PMHelper.initProvisionClient(OTM_NONE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t otmcb = otmCbRegister(OTM_JUSTWORK);

    OCStackResult expectedResult = OC_STACK_OK;
    OCStackResult actualResult = OCSetOwnerTransferCallbackData((OicSecOxm_t)OTM_JUSTWORK, &otmcb);

    ASSERT_EQ(expectedResult, actualResult);
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOwnerTransferCallbackData negatively for OIC_JUST_WORKS with Null CB
 * @target          OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @test_data       oxm = OIC_JUST_WORKS & otmcb = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 * @post_condition  None
 * @expected        OCSetOwnerTransferCallbackData will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCSetOwnerTransferCallbackDataJustWorkCB_NV_N)
{
    if (!m_PMHelper.initProvisionClient(OTM_NONE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCStackResult expectedResult = OC_STACK_INVALID_CALLBACK;
    OCStackResult actualResult = OCSetOwnerTransferCallbackData(OIC_JUST_WORKS, NULL);
    ASSERT_EQ(expectedResult, actualResult);
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOwnerTransferCallbackData positively for OIC_RANDOM_DEVICE_PIN
 * @target          OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @test_data       oxm = OIC_RANDOM_DEVICE_PIN & otmcb = OIC_RANDOM_DEVICE_PIN related callbacks
 * @pre_condition   None
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 * @post_condition  None
 * @expected        OCSetOwnerTransferCallbackData will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCSetOwnerTransferCallbackDataRandomPin_RV_P)
{
    if (!m_PMHelper.initProvisionClient(OTM_NONE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t otmcb = otmCbRegister(OTM_RANDOMPIN);

    OCStackResult expectedResult = OC_STACK_OK;
    OCStackResult actualResult = OCSetOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN, &otmcb);

    ASSERT_EQ(expectedResult, actualResult);
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOwnerTransferCallbackData with Lower Outer Boundary Value of oxmType
 * @target          OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @test_data       oxm = OTM_INVALID_LOBV & otmcb = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       call  OCSetOwnerTransferCallbackData
 * @post_condition  None
 * @expected        OCSetOwnerTransferCallbackData will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCSetOwnerTransferCallbackData_LOBV_N)
{
    if (!m_PMHelper.initProvisionClient(OTM_NONE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCStackResult expectedResult = OC_STACK_INVALID_CALLBACK;
    OCStackResult actualResult = OCSetOwnerTransferCallbackData((OicSecOxm_t)OTM_INVALID_LOBV, NULL);

    ASSERT_EQ(expectedResult, actualResult);
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOwnerTransferCallbackData with oxmType as Upper Outer Boundary Value of oxmType
 * @target          OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @test_data       oxm = OTM_INVALID_LOBV
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 * @post_condition  None
 * @expected        OCSetOwnerTransferCallbackData will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCSetOwnerTransferCallbackData_UOBV_N)
{
    if (!m_PMHelper.initProvisionClient(OTM_NONE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t otmcb;
    otmcb.loadSecretCB = InputPinCodeCallback;
    otmcb.createSecureSessionCB = CreateSecureSessionRandomPinCallback;
    otmcb.createSelectOxmPayloadCB = CreatePinBasedSelectOxmPayload;
    otmcb.createOwnerTransferPayloadCB = CreatePinBasedOwnerTransferPayload;

    OCStackResult expectedResult = OC_STACK_INVALID_PARAM;
    OCStackResult actualResult = OCSetOwnerTransferCallbackData((OicSecOxm_t)OTM_INVALID_UOBV, &otmcb);

    ASSERT_EQ(expectedResult, actualResult);
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOwnerTransferCallbackData positively for Representative Value 1 for OIC_RANDOM_DEVICE_PIN
 * @target          OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @test_data       oxm = representative value of OIC_RANDOM_DEVICE_PIN & otmcb = OIC_RANDOM_DEVICE_PIN related callbacks
 * @pre_condition   None
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 * @post_condition  None
 * @expected        OCSetOwnerTransferCallbackData will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCSetOwnerTransferCallbackDataRandomPin_RSV_P)
{
    if (!m_PMHelper.initProvisionClient(OTM_NONE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t otmcb;
    otmcb.loadSecretCB = InputPinCodeCallback;
    otmcb.createSecureSessionCB = CreateSecureSessionRandomPinCallback;
    otmcb.createSelectOxmPayloadCB = CreatePinBasedSelectOxmPayload;
    otmcb.createOwnerTransferPayloadCB = CreatePinBasedOwnerTransferPayload;

    OCStackResult expectedResult = OC_STACK_OK;
    OCStackResult actualResult = OCSetOwnerTransferCallbackData((OicSecOxm_t)OTM_RANDOMPIN, &otmcb);

    ASSERT_EQ(expectedResult, actualResult);
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCSetOwnerTransferCallbackData negatively for OIC_RANDOM_DEVICE_PIN
 * @target          OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @test_data       oxm = OIC_RANDOM_DEVICE_PIN & otmcb = NULL
 * @pre_condition   None
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 * @post_condition  None
 * @expected        OCSetOwnerTransferCallbackData will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCSetOwnerTransferCallbackDataRandomPinNullCB_N)
{
    if (!m_PMHelper.initProvisionClient(OTM_NONE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCStackResult expectedResult = OC_STACK_INVALID_CALLBACK;
    OCStackResult actualResult = OCSetOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN, NULL);

    ASSERT_EQ(CommonUtil::getOCStackResult(expectedResult), CommonUtil::getOCStackResult(actualResult));
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDoOwnershipTransfer positively.
 * @target          OCDoOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices,OCProvisionResultCB resultCallback)
 * @test_data       OIC_JUSTWORKS ownership transfer
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 * @post_condition  None
 * @expected        OCDoOwnershipTransfer will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCDoOwnershipTransferJustWork_RSV_SRC_P)
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
    }
}
#endif

/**
 * @since           2015-02-29
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDoOwnershipTransfer positively for Random Pin
 * @target          OCDoOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices,OCProvisionResultCB resultCallback)
 * @test_data       OIC_RANDOM_DEVICE_PIN ownership transfer
 * @pre_condition   Start randompin simulator
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 * @post_condition  None
 * @expected        OCDoOwnershipTransfer will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCDoOwnershipTransferRandomPin_SRC_P)
{

    CommonUtil::killApp(KILL_SERVERS);
    CommonUtil::launchApp(RANDOMPIN_SERVER);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMHelper.initProvisionClient(OTM_ALL))
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
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDoOwnershipTransfer negatively with callbackData as NULL.
 * @target          OCDoOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices,OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverUnownedDevices
 *                  5. call OCDoOwnershipTransfer
 * @post_condition  None
 * @expected        OCDoOwnershipTransfer will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCsdkOtmTest_btc, OCDoOwnershipTransferCB_NV_N)
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
                    NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif
