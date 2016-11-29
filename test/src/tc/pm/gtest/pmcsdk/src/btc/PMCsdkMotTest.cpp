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
#include "PMCsdkMotHelper.h"
#include "PMCsdkUtilityHelper.h"

#ifdef __MOT__

class PMCsdkMotTest_btc: public ::testing::Test
{
protected:
    OCProvisionDev_t *m_UnownList, *m_OwnList, *m_motEnabledDevList, *m_motOwnedDevList;
    OicSecAcl_t *m_Acl, *m_Acl1, *m_Acl2;
    PMCsdkHelper m_PMHelper;
    PMCsdkMotHelper m_PMMotHelper;

    virtual void SetUp()
    {
        CommonUtil::killApp(KILL_SERVERS);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        PMCsdkUtilityHelper::removeAllResFile();
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::copyFile(PRECONFIG_SERVER1_CBOR_BACKUP, PRECONFIG_SERVER1_CBOR);
        CommonUtil::copyFile(PRECONFIG_SERVER2_CBOR_BACKUP, PRECONFIG_SERVER2_CBOR);
        CommonUtil::copyFile(RANDOMPIN_SERVER_CBOR_BACKUP, RANDOMPIN_SERVER_CBOR);
        CommonUtil::copyFile(CLIENT_CBOR_BACKUP, CLIENT_CBOR);
        CommonUtil::copyFile(JUSTWORKS_SERVER7_CBOR_BACKUP, JUSTWORKS_SERVER7_CBOR);
        CommonUtil::copyFile(PRECONFIG_SERVER2_CBOR_BACKUP, PRECONFIG_SERVER2_CBOR);
        CommonUtil::copyFile(MOT_CLIENT_CBOR_BACKUP, MOT_CLIENT_CBOR);
        m_motEnabledDevList = NULL;
        m_motOwnedDevList = NULL;
        m_UnownList = NULL;
        m_OwnList = NULL;
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);
    }
};

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDiscoverMultipleOwnerEnabledDevices positively with waittime = DISCOVERY_TIMEOUT
 * @target          OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       waittime = DISCOVERY_TIMEOUT
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnerEnabledDevices
 * @post_condition  none
 * @expected        OCDiscoverMultipleOwnerEnabledDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDiscoverMultipleOwnerEnabledDevices_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);

    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDiscoverMultipleOwnerEnabledDevices positively with waittime = 1
 * @target          OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       waittime = 1
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnerEnabledDevices
 * @post_condition  none
 * @expected        OCDiscoverMultipleOwnerEnabledDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDiscoverMultipleOwnerEnabledDevices_LBV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);

    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT_ONE,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDiscoverMultipleOwnerEnabledDevices negatively with waittime = 0
 * @target          OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       waittime = 0
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnerEnabledDevices
 * @post_condition  none
 * @expected        OCDiscoverMultipleOwnerEnabledDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDiscoverMultipleOwnerEnabledDevices_LOBV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);

    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT_ZERO,
                    &m_motEnabledDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDiscoverMultipleOwnerEnabledDevices negatively with ppList = NULL
 * @target          OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       ppList = NULL
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnerEnabledDevices
 * @post_condition  none
 * @expected        OCDiscoverMultipleOwnerEnabledDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDiscoverMultipleOwnerEnabledDevicesDevlist_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);

    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT_ONE,
                    NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDiscoverMultipleOwnedDevices positively with waittime = DISCOVERY_TIMEOUT
 * @target          OCStackResult OCDiscoverMultipleOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       waittime = DISCOVERY_TIMEOUT
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnedDevices
 * @post_condition  none
 * @expected        OCDiscoverMultipleOwnedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDiscoverMultipleOwnedDevices_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);

    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDiscoverMultipleOwnedDevices positively with waittime = 1
 * @target          OCStackResult OCDiscoverMultipleOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       waittime = 1
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnedDevices
 * @post_condition  none
 * @expected        OCDiscoverMultipleOwnedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDiscoverMultipleOwnedDevices_LBV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);

    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT_ONE,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDiscoverMultipleOwnedDevices negatively with waittime = 0
 * @target          OCStackResult OCDiscoverMultipleOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       waittime = 0
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnedDevices
 * @post_condition  none
 * @expected        OCDiscoverMultipleOwnedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDiscoverMultipleOwnedDevices_LOBV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);

    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT_ZERO,
                    &m_motEnabledDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @objective       Test OCDiscoverMultipleOwnedDevices negatively with ppList = NULL
 * @target          OCStackResult OCDiscoverMultipleOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       ppList = NULL
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnedDevices
 * @post_condition  none
 * @expected        OCDiscoverMultipleOwnedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDiscoverMultipleOwnedDevicesDevlist_NV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);

    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT_ONE,
                    NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCAddPreconfigPin positively with regular data
 * @target          OCStackResult OCAddPreconfigPin(const OCProvisionDev_t *targetDeviceInfo, const char* preconfPIN, size_t preconfPINLen)
 * @test_data       regular data for the API
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnerEnabledDevices
 *                  5. call OCAddPreconfigPin
 * @post_condition  none
 * @expected        OCAddPreconfigPin will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCAddPreconfigPin_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);

    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_motEnabledDevList, 1);

    if (!m_PMMotHelper.addPreconfigPIN(targetDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCAddPreconfigPin(const OCProvisionDev_t *targetDeviceInfo, const char* preconfPIN, size_t preconfPINLen)
 * @objective       Test OCAddPreconfigPin positively with regular data
 * @target          OCStackResult OCDoMultipleOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @test_data       regular data for the API
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnerEnabledDevices
 *                  5. call OCAddPreconfigPin
 *                  6. call OCDoMultipleOwnershipTransfer
 * @post_condition  none
 * @expected        OCDoMultipleOwnershipTransfer will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDoMultipleOwnershipTransfer_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);

    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.addPreconfigPIN(targetDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.doMultipleOwnershipTransfer((void*)MOT_CTX, m_motEnabledDevList,
                    PMCsdkMotHelper::multipleOwnershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCAddPreconfigPin(const OCProvisionDev_t *targetDeviceInfo, const char* preconfPIN, size_t preconfPINLen)
 * @objective       Test OCAddPreconfigPin negatively with resultCallback as NULL
 * @target          OCStackResult OCDoMultipleOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnerEnabledDevices
 *                  5. call OCAddPreconfigPin
 *                  6. call OCDoMultipleOwnershipTransfer
 * @post_condition  none
 * @expected        OCDoMultipleOwnershipTransfer will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDoMultipleOwnershipTransferCB_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);

    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.addPreconfigPIN(targetDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.doMultipleOwnershipTransfer((void*)MOT_CTX, m_motEnabledDevList,
                    NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCAddPreconfigPin(const OCProvisionDev_t *targetDeviceInfo, const char* preconfPIN, size_t preconfPINLen)
 * @objective       Test OCAddPreconfigPin negatively with targetDevices as NULL
 * @target          OCStackResult OCDoMultipleOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @test_data       targetDevices = NULL
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnerEnabledDevices
 *                  5. call OCAddPreconfigPin
 *                  6. call OCDoMultipleOwnershipTransfer
 * @post_condition  none
 * @expected        OCDoMultipleOwnershipTransfer will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDoMultipleOwnershipTransfer_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);

    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.addPreconfigPIN(targetDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.doMultipleOwnershipTransfer((void*)MOT_CTX, NULL,
                    PMCsdkMotHelper::multipleOwnershipTransferCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCChangeMOTMode positively with regular data
 * @target          OCStackResult OCChangeMOTMode(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecMomType_t momType, OCProvisionResultCB resultCallback)
 * @test_data       regular data for the target API
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCChangeMOTMode
 * @post_condition  None
 * @expected        OCChangeMOTMode will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCChangeMOTMode_RV_SRC_P)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::launchApp(PRECONFIG_SERVER2);

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

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.changeMOTMode((void*)ctxChangeMOTMode, targetDev, OIC_MULTIPLE_OWNER_ENABLE, PMCsdkMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCChangeMOTMode positively with momType = OIC_MULTIPLE_OWNER_DISABLE
 * @target          OCStackResult OCChangeMOTMode(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecMomType_t momType, OCProvisionResultCB resultCallback)
 * @test_data       momType = OIC_MULTIPLE_OWNER_DISABLE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCChangeMOTMode
 * @post_condition  None
 * @expected        OCChangeMOTMode will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCChangeMOTMode_CLU_P)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::launchApp(PRECONFIG_SERVER2);

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

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.changeMOTMode((void*)ctxChangeMOTMode, targetDev, OIC_MULTIPLE_OWNER_DISABLE, PMCsdkMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCChangeMOTMode negatively with momType greater Upper Boundary Value
 * @target          OCStackResult OCChangeMOTMode(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecMomType_t momType, OCProvisionResultCB resultCallback)
 * @test_data       momType greater Upper Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCChangeMOTMode
 * @post_condition  None
 * @expected        OCChangeMOTMode will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCChangeMOTMode_CLU_UOBV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::launchApp(PRECONFIG_SERVER2);

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

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.changeMOTMode((void*)ctxChangeMOTMode, targetDev, (const OicSecMomType_t) OIC_MULTIPLE_OWNER_UOBV, PMCsdkMotHelper::changeMOTModeCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCChangeMOTMode negatively with resultCallback = NULL
 * @target          OCStackResult OCChangeMOTMode(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecMomType_t momType, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCChangeMOTMode
 * @post_condition  None
 * @expected        OCChangeMOTMode will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCChangeMOTModeCb_NV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::launchApp(PRECONFIG_SERVER2);

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

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.changeMOTMode((void*)ctxChangeMOTMode, targetDev, OIC_MULTIPLE_OWNER_ENABLE, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCChangeMOTMode negatively with targetDeviceInfo = NULL
 * @target          OCStackResult OCChangeMOTMode(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecMomType_t momType, OCProvisionResultCB resultCallback)
 * @test_data       targetDeviceInfo = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCChangeMOTMode
 * @post_condition  None
 * @expected        OCChangeMOTMode will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCChangeMOTModeDev_NV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::launchApp(PRECONFIG_SERVER2);

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

    if (!m_PMMotHelper.changeMOTMode((void*)ctxChangeMOTMode, NULL, OIC_MULTIPLE_OWNER_DISABLE, PMCsdkMotHelper::changeMOTModeCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCChangeMOTMode(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecMomType_t momType, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionPreconfPin positively with regular data
 * @target          OCStackResult OCProvisionPreconfPin(void* ctx, OCProvisionDev_t *targetDeviceInfo, const char * preconfPin, size_t preconfPinLen, OCProvisionResultCB resultCallback)
 * @test_data       regular data for the target API
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCChangeMOTMode
 *                  9. call OCDiscoverMultipleOwnerEnabledDevices
 *                  10. call OCProvisionPreconfPin
 * @post_condition  None
 * @expected        OCProvisionPreconfPin will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCProvisionPreconfPin_RV_SRC_P)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::launchApp(PRECONFIG_SERVER2);

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

    OCProvisionDev_t* targetOwnedDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.changeMOTMode((void*)ctxChangeMOTMode, targetOwnedDev, OIC_MULTIPLE_OWNER_ENABLE, PMCsdkMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t* targetMotDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.provisionPreconfPin((void*)ctxProvisionPreconfPin, targetMotDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, PMCsdkMotHelper::provisionPreconfPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCChangeMOTMode(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecMomType_t momType, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPreconfPin(void* ctx, OCProvisionDev_t *targetDeviceInfo, const char * preconfPin, size_t preconfPinLen, OCProvisionResultCB resultCallback)
 * @objective       Test OCSelectMOTMethod positively with regular data
 * @target          OCStackResult OCSelectMOTMethod(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecOxm_t oxmSelValue, OCProvisionResultCB resultCallback)
 * @test_data       regular data for the target API
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCChangeMOTMode
 *                  9. call OCDiscoverMultipleOwnerEnabledDevices
 *                  10. call OCProvisionPreconfPin
 *                  11. call OCSelectMOTMethod
 * @post_condition  None
 * @expected        OCSelectMOTMethod will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCSelectMOTMethod_RV_SRC_P)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::launchApp(PRECONFIG_SERVER2);
    CommonUtil::waitInSecond(DELAY_LONG);

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

    OCProvisionDev_t* targetOwnedDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.changeMOTMode((void*)ctxChangeMOTMode, targetOwnedDev, OIC_MULTIPLE_OWNER_ENABLE, PMCsdkMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t* targetMotDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.provisionPreconfPin((void*)ctxProvisionPreconfPin, targetMotDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, PMCsdkMotHelper::provisionPreconfPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.selectMOTMethod((void*)ctxSelectMOTMethod, targetMotDev, OIC_PRECONFIG_PIN, PMCsdkMotHelper::selectMOTMethodCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCChangeMOTMode(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecMomType_t momType, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPreconfPin(void* ctx, OCProvisionDev_t *targetDeviceInfo, const char * preconfPin, size_t preconfPinLen, OCProvisionResultCB resultCallback)
 * @objective       Test OCSelectMOTMethod negatively with targetDeviceInfo as NULL
 * @target          OCStackResult OCSelectMOTMethod(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecOxm_t oxmSelValue, OCProvisionResultCB resultCallback)
 * @test_data       targetDeviceInfo = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCChangeMOTMode
 *                  9. call OCDiscoverMultipleOwnerEnabledDevices
 *                  10. call OCProvisionPreconfPin
 *                  11. call OCSelectMOTMethod
 * @post_condition  None
 * @expected        OCSelectMOTMethod will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCSelectMOTMethodDev_NV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::launchApp(PRECONFIG_SERVER2);

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

    OCProvisionDev_t* targetOwnedDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.changeMOTMode((void*)ctxChangeMOTMode, targetOwnedDev, OIC_MULTIPLE_OWNER_ENABLE, PMCsdkMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t* targetMotDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.provisionPreconfPin((void*)ctxProvisionPreconfPin, targetMotDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, PMCsdkMotHelper::provisionPreconfPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.selectMOTMethod((void*)ctxSelectMOTMethod, NULL, OIC_PRECONFIG_PIN, PMCsdkMotHelper::selectMOTMethodCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCChangeMOTMode(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecMomType_t momType, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCProvisionPreconfPin(void* ctx, OCProvisionDev_t *targetDeviceInfo, const char * preconfPin, size_t preconfPinLen, OCProvisionResultCB resultCallback)
 * @objective       Test OCSelectMOTMethod negatively with resultCallback = NULL
 * @target          OCStackResult OCSelectMOTMethod(void *ctx, const OCProvisionDev_t *targetDeviceInfo, const OicSecOxm_t oxmSelValue, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCChangeMOTMode
 *                  9. call OCDiscoverMultipleOwnerEnabledDevices
 *                  10. call OCProvisionPreconfPin
 *                  11. call OCSelectMOTMethod
 * @post_condition  None
 * @expected        OCSelectMOTMethod will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCSelectMOTMethodCb_NV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::launchApp(PRECONFIG_SERVER2);

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

    OCProvisionDev_t* targetOwnedDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.changeMOTMode((void*)ctxChangeMOTMode, targetOwnedDev, OIC_MULTIPLE_OWNER_ENABLE, PMCsdkMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t* targetMotDev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.provisionPreconfPin((void*)ctxProvisionPreconfPin, targetMotDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, PMCsdkMotHelper::provisionPreconfPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.selectMOTMethod((void*)ctxSelectMOTMethod, targetMotDev, OIC_PRECONFIG_PIN, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
    }
}
#endif

#endif /*__MOT__*/
