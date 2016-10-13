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

#if defined(__MOT__)

class PMCsdkMotTest_btc: public ::testing::Test
{
protected:

    OCProvisionDev_t *m_OwnList, *m_motEnabledDevList, *m_motOwnedDevList;
    OicSecAcl_t *m_Acl, *m_Acl1, *m_Acl2;
    PMCsdkMotHelper m_PMMotHelper;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(JUSTWORKS_SERVER);
        CommonUtil::killApp(RANDOMPIN_SERVER);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::rmFile(MOT_CLIENT_DATABASE);
        CommonUtil::rmFile(JUSTWORKS_SERVER7_CBOR);
        CommonUtil::rmFile(JUSTWORKS_SERVER2_CBOR);
        CommonUtil::rmFile(MOT_CLIENT_CBOR);
        CommonUtil::waitInSecond(DELAY_LONG);
        CommonUtil::copyFile(JUSTWORKS_SERVER7_CBOR_BACKUP, JUSTWORKS_SERVER7_CBOR);
        CommonUtil::copyFile(JUSTWORKS_SERVER2_CBOR_BACKUP, JUSTWORKS_SERVER2_CBOR);
        CommonUtil::copyFile(MOT_CLIENT_CBOR_BACKUP, MOT_CLIENT_CBOR);
        m_motEnabledDevList = NULL;
        m_motOwnedDevList = NULL;
        m_OwnList = NULL;
        CommonUtil::launchApp(JUSTWORKS_SERVER7);
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(JUSTWORKS_SERVER);
        CommonUtil::killApp(RANDOMPIN_SERVER);
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
    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT_ONE,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
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
    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT_ZERO,
                    &m_motEnabledDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
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
    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT_ONE,
                    NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
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
    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
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
    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT_ONE,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
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
    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT_ZERO,
                    &m_motEnabledDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
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
    if (!m_PMMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT_ONE,
                    NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCAddPreconfigPIN positively with regular data
 * @target          OCStackResult OCAddPreconfigPIN(const OCProvisionDev_t *targetDeviceInfo, const char* preconfPIN, size_t preconfPINLen)
 * @test_data       regular data for the API
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnerEnabledDevices
 *                  5. call OCAddPreconfigPIN
 * @post_condition  none
 * @expected        OCAddPreconfigPIN will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCAddPreconfigPIN_RV_SRC_P)
{
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

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_motEnabledDevList, 1);

    if (!m_PMMotHelper.addPreconfigPIN(targetDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCAddPreconfigPIN(const OCProvisionDev_t *targetDeviceInfo, const char* preconfPIN, size_t preconfPINLen)
 * @objective       Test OCAddPreconfigPIN positively with regular data
 * @target          OCStackResult OCDoMultipleOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @test_data       regular data for the API
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnerEnabledDevices
 *                  5. call OCAddPreconfigPIN
 *                  6. call OCDoMultipleOwnershipTransfer
 * @post_condition  none
 * @expected        OCDoMultipleOwnershipTransfer will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDoMultipleOwnershipTransfer_RV_SRC_P)
{
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

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.addPreconfigPIN(targetDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.doMultipleOwnershipTransfer((void*)MOT_CTX, m_motEnabledDevList,
                    PMCsdkMotHelper::multipleOwnershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCAddPreconfigPIN(const OCProvisionDev_t *targetDeviceInfo, const char* preconfPIN, size_t preconfPINLen)
 * @objective       Test OCAddPreconfigPIN negatively with resultCallback as NULL
 * @target          OCStackResult OCDoMultipleOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnerEnabledDevices
 *                  5. call OCAddPreconfigPIN
 *                  6. call OCDoMultipleOwnershipTransfer
 * @post_condition  none
 * @expected        OCDoMultipleOwnershipTransfer will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDoMultipleOwnershipTransferCB_NV_N)
{
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

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.addPreconfigPIN(targetDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.doMultipleOwnershipTransfer((void*)MOT_CTX, m_motEnabledDevList,
                    NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-09-10
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverMultipleOwnerEnabledDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCAddPreconfigPIN(const OCProvisionDev_t *targetDeviceInfo, const char* preconfPIN, size_t preconfPINLen)
 * @objective       Test OCAddPreconfigPIN negatively with targetDevices as NULL
 * @target          OCStackResult OCDoMultipleOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @test_data       targetDevices = NULL
 * @pre_condition   Start Mot Enabled justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverMultipleOwnerEnabledDevices
 *                  5. call OCAddPreconfigPIN
 *                  6. call OCDoMultipleOwnershipTransfer
 * @post_condition  none
 * @expected        OCDoMultipleOwnershipTransfer will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkMotTest_btc, OCDoMultipleOwnershipTransfer_NV_N)
{
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

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMMotHelper.addPreconfigPIN(targetDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMMotHelper.doMultipleOwnershipTransfer((void*)MOT_CTX, NULL,
                    PMCsdkMotHelper::multipleOwnershipTransferCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMMotHelper.getFailureMessage());
        return;
    }
}
#endif

#endif /*__MOT__*/
