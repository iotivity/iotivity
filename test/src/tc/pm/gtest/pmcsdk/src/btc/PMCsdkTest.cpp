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
#include "PMCsdkUtilityHelper.h"

class PMCsdkTest_btc: public ::testing::Test
{
protected:

    OCProvisionDev_t *m_OwnList, *m_UnownList, *m_motEnabledDevList;
    OicSecAcl_t *m_Acl, *m_Acl1, *m_Acl2;
    PMCsdkHelper m_PMHelper;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(JUSTWORKS_SERVER);
        CommonUtil::killApp(RANDOMPIN_SERVER);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::rmFile(CLIENT_DATABASE);
        CommonUtil::rmFile(JUSTWORKS_SERVER1_CBOR);
        CommonUtil::rmFile(JUSTWORKS_SERVER2_CBOR);
        CommonUtil::rmFile(CLIENT_CBOR);
        CommonUtil::waitInSecond(DELAY_LONG);
        CommonUtil::copyFile(JUSTWORKS_SERVER1_CBOR_BACKUP, JUSTWORKS_SERVER1_CBOR);
        CommonUtil::copyFile(JUSTWORKS_SERVER2_CBOR_BACKUP, JUSTWORKS_SERVER2_CBOR);
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
        CommonUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(JUSTWORKS_SERVER);
        CommonUtil::killApp(RANDOMPIN_SERVER);
    }
};

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCDiscoverUnownedDevices positively with waittime = DISCOVERY_TIMEOUT
 * @target          OCStackResult OCDiscoverUnownedDevices(unsigned short waittime, OCProvisionDev_t **ppList)
 * @test_data       waittime = DISCOVERY_TIMEOUT
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 * @post_condition  none
 * @expected               OCDiscoverUnownedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCDiscoverUnownedDevices_RV_SRC_P)
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
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCDiscoverUnownedDevices positively with Lower Boundary value of Time.
 * @target          OCStackResult OCDiscoverUnownedDevices(unsigned short waittime, OCProvisionDev_t **ppList)
 * @test_data       waittime = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 * @post_condition  None
 * @expected        OCDiscoverUnownedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCDiscoverUnownedDevicesTime_LBV_P)
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
        return;
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCDiscoverUnownedDevices negatively with  Out of Lower Boundary value of Time.
 * @target          OCStackResult OCDiscoverUnownedDevices(unsigned short waittime, OCProvisionDev_t **ppList);
 * @test_data       waittime = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 * @post_condition  None
 * @expected        OCDiscoverUnownedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCDiscoverUnownedDevices_LOBV_N)
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
        return;
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCDiscoverUnownedDevices Negatively with ppList as NULL
 * @target          OCStackResult OCDiscoverUnownedDevices(unsigned short waittime, OCProvisionDev_t **ppList)
 * @test_data       ppList = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 * @post_condition  None
 * @expected        OCDiscoverUnownedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCDiscoverUnownedDevices_ECRC_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCDiscoverOwnedDevices positively.
 * @target          OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       timeout = DISCOVERY_TIMEOUT
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverOwnedDevices
 * @post_condition  none
 * @expected        OCDiscoverOwnedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCDiscoverOwnedDevices_RV_SRC_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCDiscoverOwnedDevices positively with Lower Boundary Value of Time
 * @target          OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       timeout = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverOwnedDevices
 * @post_condition  None
 * @expected        discoverOwnedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCDiscoverOwnedDevicesTime_LBV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT_ONE, &m_OwnList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCDiscoverOwnedDevices negatively with Lower Outer Boundary Value of Time.
 * @target          OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data        timeout = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverOwnedDevices
 * @post_condition  None
 * @expected        OCDiscoverOwnedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCDiscoverOwnedDevicesTime_ECRC_LOBV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT_ZERO, &m_OwnList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCDiscoverOwnedDevices negatively with ppList NULL.
 * @target          OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @test_data       OCProvisionDev_t **ppList = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverOwnedDevices
 * @post_condition  None
 * @expected        OCDiscoverOwnedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCDiscoverOwnedDevices_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT_ZERO, NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)* @objective Test OCGetDevInfoFromNetwork positively.
 * @target          OCStackResult OCGetDevInfoFromNetwork(unsigned short waittime, OCProvisionDev_t** pOwnedDevList, OCProvisionDev_t** pUnownedDevList)
 * @objective       Test OCGetDevInfoFromNetwork positively with regular values.
 * @test_data       waittime = DISCOVERY_TIMEOUT
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCGetDevInfoFromNetwork
 * @post_condition  None
 * @expected        OCGetDevInfoFromNetwork will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetDevInfoFromNetwork_SRC_RV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverAllDevices(DISCOVERY_TIMEOUT, &m_OwnList, &m_UnownList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2015-12-30
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCGetDevInfoFromNetwork positively with Lower Boundary Value of Time
 * @target          OCStackResult OCGetDevInfoFromNetwork(unsigned short waittime, OCProvisionDev_t** pOwnedDevList, OCProvisionDev_t** pUnownedDevList)
 * @test_data       waittime = DISCOVERY_TIMEOUT_TWO
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCGetDevInfoFromNetwork
 * @post_condition  None
 * @expected        OCGetDevInfoFromNetwork will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetDevInfoFromNetwork_LBV_P)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverAllDevices(DISCOVERY_TIMEOUT_TWO, &m_OwnList, &m_UnownList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCGetDevInfoFromNetwork negatively with Lower Outer Boundary Value of Time
 * @target          OCStackResult OCGetDevInfoFromNetwork(unsigned short waittime, OCProvisionDev_t** pOwnedDevList, OCProvisionDev_t** pUnownedDevList)
 * @test_data       waittime = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCGetDevInfoFromNetwork
 * @post_condition  None
 * @expected        OCGetDevInfoFromNetwork will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetDevInfoFromNetwork_LOBV_ECRC_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverAllDevices(DISCOVERY_TIMEOUT_ONE, &m_OwnList, &m_UnownList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCGetDevInfoFromNetwork negatively with NULL Unowned Device List
 * @target          OCStackResult OCGetDevInfoFromNetwork(unsigned short waittime, OCProvisionDev_t** pOwnedDevList, OCProvisionDev_t** pUnownedDevList)
 * @test_data       pUnownedDevList = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCGetDevInfoFromNetwork
 * @post_condition  None
 * @expected        OCGetDevInfoFromNetwork will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetDevInfoFromNetworkOwned_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverAllDevices(DISCOVERY_TIMEOUT_ONE, &m_OwnList, NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCGetDevInfoFromNetwork negatively with NULL Owned Device List
 * @target          OCStackResult OCGetDevInfoFromNetwork(unsigned short waittime, OCProvisionDev_t** pOwnedDevList, OCProvisionDev_t** pUnownedDevList)
 * @test_data       pOwnedDevList = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCGetDevInfoFromNetwork
 * @post_condition  None
 * @expected        OCGetDevInfoFromNetwork will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetDevInfoFromNetworkUnowned_NV_N)
{
    if (!m_PMHelper.initProvisionClient())
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMHelper.discoverAllDevices(DISCOVERY_TIMEOUT_ONE, NULL, &m_UnownList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
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
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCSetOwnerTransferCallbackDataJustWork_SRC_RV_P)
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
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCSetOwnerTransferCallbackDataJustWork_REV_P)
{
    if (!m_PMHelper.initProvisionClient(OTM_NONE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t otmcb = otmCbRegister(OTM_JUSTWORK);

    OCStackResult expectedResult = OC_STACK_OK;
    OCStackResult actualResult = OCSetOwnerTransferCallbackData(OTM_JUSTWORK, &otmcb);

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
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCSetOwnerTransferCallbackDataJustWorkCB_NV_N)
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
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCSetOwnerTransferCallbackDataRandomPin_RV_P)
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
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCSetOwnerTransferCallbackData_LOBV_N)
{
    if (!m_PMHelper.initProvisionClient(OTM_NONE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCStackResult expectedResult = OC_STACK_INVALID_CALLBACK;
    OCStackResult actualResult = OCSetOwnerTransferCallbackData(OTM_INVALID_LOBV, NULL);

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
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCSetOwnerTransferCallbackData_UOBV_N)
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
    OCStackResult actualResult = OCSetOwnerTransferCallbackData(OTM_INVALID_UOBV, &otmcb);

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
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCSetOwnerTransferCallbackDataRandomPin_RSV_P)
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
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCSetOwnerTransferCallbackDataRandomPinNullCB_N)
{
    if (!m_PMHelper.initProvisionClient(OTM_NONE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCStackResult expectedResult = OC_STACK_INVALID_CALLBACK;
    OCStackResult actualResult = OCSetOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN, NULL);

    ASSERT_EQ(PMCsdkUtilityHelper::getOCStackResult(expectedResult), PMCsdkUtilityHelper::getOCStackResult(actualResult));
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCDoOwnershipTransfer positively.
 * @target          OCDoOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices,OCProvisionResultCB resultCallback)
 * @test_data       OIC_JUSTWORKS ownership transfer
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 * @post_condition  None
 * @expected        OCDoOwnershipTransfer will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCDoOwnershipTransferJustWork_RSV_SRC_P)
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
}
#endif

/**
 * @since           2015-02-29
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCDoOwnershipTransfer positively for Random Pin
 * @target          OCDoOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices,OCProvisionResultCB resultCallback)
 * @test_data       OIC_RANDOM_DEVICE_PIN ownership transfer
 * @pre_condition   Start randompin simulator
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 * @post_condition  None
 * @expected        OCDoOwnershipTransfer will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCDoOwnershipTransferRandomPin_SRC_P)
{

    CommonUtil::killApp(JUSTWORKS_SERVER);
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
        return;
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @objective       Test OCDoOwnershipTransfer negatively with callbackData as NULL.
 * @target          OCDoOwnershipTransfer(void* ctx,OCProvisionDev_t *targetDevices,OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 * @post_condition  None
 * @expected        OCDoOwnershipTransfer will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCDoOwnershipTransferCB_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-03-12
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)*
 * @objective       Test OCProvisionDirectPairing positively.
 * @target          OCStackResult OCProvisionDirectPairing(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecPconf_t *pconf, OCProvisionResultCB resultCallback)
 * @objective       Test OCProvisionDirectPairing positively.
 * @test_data       Regular data for OCProvisionDirectPairing
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionDirectPairing
 * @post_condition  None
 * @expected        OCProvisionDirectPairing will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCProvisionDirectPairing_SRC_RV_P)
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
    OicSecPconf_t pconf;
    memset(&pconf, 0, sizeof(OicSecPconf_t));
    pconf.edp = true;

    // set default supported PRM types
    pconf.prmLen = sizeof(SUPPORTED_PRMS)/sizeof(OicSecPrm_t);
    pconf.prm = (OicSecPrm_t *)OICCalloc(pconf.prmLen, sizeof(OicSecPrm_t));

    if(pconf.prm)
    {
        for (size_t i=0; i<pconf.prmLen; i++)
        {
            pconf.prm[i] = SUPPORTED_PRMS[i];
        }
    }

    memcpy(pconf.pin.val, DEFAULT_DP_PROVSIONING_PIN, DP_PIN_LENGTH);

    // set default pdacl
    pconf.pdacls = createPdAcl(FULL_PERMISSION);

    if (!m_PMHelper.proivisioningDirectPairing((void*)ctxProvDirectPairing, device1,
                    &pconf, m_PMHelper.provisionDPCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2015-03-12
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionDirectPairing negatively with Callback NULL.
 * @target          OCStackResult OCProvisionDirectPairing(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecPconf_t *pconf, OCProvisionResultCB resultCallback)
 * @objective       Test OCProvisionDirectPairing negatively with resultCallback as NULL
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionDirectPairing
 * @post_condition  None
 * @expected        OCProvisionDirectPairing will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCProvisionDirectPairingCB_NV_N)
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

    OicSecPconf_t pconf;
    memset(&pconf, 0, sizeof(OicSecPconf_t));
    pconf.edp = true;

    // set default supported PRM types
    memcpy(pconf.pin.val, DEFAULT_DP_PROVSIONING_PIN, DP_PIN_LENGTH);

    if(pconf.prm)
    {
        for (size_t i=0; i<pconf.prmLen; i++)
        {
            pconf.prm[i] = SUPPORTED_PRMS[i];
        }
    }

    // set default pin
    const char DP_DEFAULT_PIN[] = DEFAULT_DP_PROVSIONING_PIN;
    memcpy(pconf.pin.val, DP_DEFAULT_PIN, DP_PIN_LENGTH);

    // set default pdacl
    pconf.pdacls = createPdAcl(FULL_PERMISSION);

    if (!m_PMHelper.proivisioningDirectPairing((void*)ctxProvDirectPairing, device1,
                    &pconf, NULL, OC_STACK_INVALID_CALLBACK))
    {

        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2015-03-12
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionDirectPairing negatively with DeviceInfo as NULL.
 * @target          OCStackResult OCProvisionDirectPairing(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecPconf_t *pconf, OCProvisionResultCB resultCallback);
 * @objective       Test OCProvisionDirectPairing negatively with selectedDeviceInfo as NULL
 * @test_data       selectedDeviceInfo = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionDirectPairing
 * @post_condition  None
 * @expected        OCProvisionDirectPairing will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCProvisionDirectPairingDeviceInfo_NV_N)
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

    OicSecPconf_t pconf;
    memset(&pconf, 0, sizeof(OicSecPconf_t));
    pconf.edp = true;

    // set default supported PRM types
    pconf.prmLen = sizeof(SUPPORTED_PRMS)/sizeof(OicSecPrm_t);
    pconf.prm = (OicSecPrm_t *)OICCalloc(pconf.prmLen, sizeof(OicSecPrm_t));

    if(pconf.prm)
    {
        for (size_t i=0; i<pconf.prmLen; i++)
        {
            pconf.prm[i] = SUPPORTED_PRMS[i];
        }
    }

    // set default pin
    memcpy(pconf.pin.val, DEFAULT_DP_PROVSIONING_PIN, DP_PIN_LENGTH);

    // set default pdacl
    pconf.pdacls = createPdAcl(FULL_PERMISSION);

    if (!m_PMHelper.proivisioningDirectPairing((void*)ctxProvDirectPairing, NULL,
                    &pconf, m_PMHelper.provisionDPCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2015-03-12
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionDirectPairing negatively with acl as NULL.
 * @target          OCStackResult OCProvisionDirectPairing(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecPconf_t *pconf, OCProvisionResultCB resultCallback);
 * @objective       Test OCProvisionDirectPairing negatively with pconf as NULL
 * @test_data       pconf = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionDirectPairing
 * @post_condition  None
 * @expected        OCProvisionDirectPairing will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCProvisionDirectPairingPAcl_NV_N)
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

    if (!m_PMHelper.proivisioningDirectPairing((void*)ctxProvDirectPairing, device1,
                    NULL, m_PMHelper.provisionDPCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2015-03-12
 * @objective       Test OCDeletePdAclList positively
 * @target          void OCDeletePdAclList(OicSecPdAcl_t* pPdAcl)
 * @test_data       regular data for pPdAcl
 * @pre_condition   Start two justworks simulators
 * @procedure       call  OCDeletePdAclList
 * @post_condition  None
 * @expected        OCDeletePdAclList will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCDeletePdAclList_SRC_P)
{
    OicSecPconf_t pconf;
    memset(&pconf, 0, sizeof(OicSecPconf_t));
    pconf.edp = true;

    // set default supported PRM types
    pconf.prmLen = sizeof(SUPPORTED_PRMS)/sizeof(OicSecPrm_t);
    pconf.prm = (OicSecPrm_t *)OICCalloc(pconf.prmLen, sizeof(OicSecPrm_t));

    if(pconf.prm)
    {
        for (size_t i=0; i<pconf.prmLen; i++)
        {
            pconf.prm[i] = SUPPORTED_PRMS[i];
        }
    }

    // set default pin
    memcpy(pconf.pin.val, DEFAULT_DP_PROVSIONING_PIN, DP_PIN_LENGTH);

    // set default pdacl
    pconf.pdacls = createPdAcl(FULL_PERMISSION);

    m_PMHelper.deletePdAclList(pconf.pdacls);

}
#endif

/**
 * @since           2015-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials positively.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       Regular data OCProvisionCredentials
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionCredential_SRC_RV_P)
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
}
#endif

/**
 * @since           2015-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials positively with Cred Type with Representative value of SYMMETRIC_PAIR_WISE_KEY.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       type = representative value of SYMMETRIC_PAIR_WISE_KEY
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionCredentialCredType_REV_P)
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
        return;
    }
}
#endif

/**
 * @since           2015-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials positively with Upper Boundary Value of Keysize.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       keySize = OWNER_PSK_LENGTH_256
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionCredentialKeySize_UBV_P)
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
        return;
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with callback as NULL.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionCredentialCB_ECRC_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Device1 ID as NULL.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       pDev1 = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionCredentialDev1_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Device2 ID as NULL.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       pDev2 = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionCredentialDev2_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Lower Outer Boundary Value of KeySize.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       keySize is Out of Lower Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionCredentialKeysize_LOBV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively Upper Outer Boundary Value of KeySize
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       keySize is Out of Upper Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionCredentialKeysize_UOBV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Lower Outer Boundary Value of Cred Type.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       OicSecCredType_t is out of Lower Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionCredentialCredType_LOBV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Upper Outer Boundary Value of Cred Type
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       OicSecCredType_t is out of Upper Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionCredentialCredType_UOBV_N)
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
        return;
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL positively.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for OCProvisionACL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionACL
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionAcl_SRC_RV_P)
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
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL negatively with Device List NULL.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       selectedDeviceInfo = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionACL
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionAclDeviceList_ECRC_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL negatively with Access Control List NULL.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       acl = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionACL
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionAclAcl_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL negatively with callback as NULL.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionACL
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionAclCB_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL positively by calling provision acl multiple time.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       Call OCProvisionACL multiple time
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionACL
 *                  9. call OCProvisionACL
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionAclMultipleTime_P)
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
        return;
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionPairwiseDevices Positively
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for OCProvisionPairwiseDevices
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionPairwiseDevices_SRC_RV_P)
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
        return;
    }
}
#endif

/**
 * @since           2015-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionPairwiseDevices Positively with KeySize as Upper Boundary Value
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       keySize = OWNER_PSK_LENGTH_256
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionPairwiseDevicesKeySize_UBV_P)
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
        return;
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Positively with ACL1 NULL
 * @objective       Test OCProvisionPairwiseDevices negatively with pDev1Acl as NULL
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       pDev1Acl = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionPairwiseDevicesAcl1_NV_P)
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
        return;
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Positively with ACL2 NULL
 * @objective       Test OCProvisionPairwiseDevices negatively with pDev2Acl as NULL
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       pDev2Acl = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionPairwiseDevicesAcl2_NV_P)
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
        return;
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Positively with callback as NULL
 * @objective       Test OCProvisionPairwiseDevices negatively with resultCallback as NULL
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionPairwiseDevicesCB_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Negatively with Device1 NULL
 * @objective       Test OCProvisionPairwiseDevices negatively with pDev1 as NULL
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       pDev1 = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionPairwiseDevicesDev1_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Negatively with Device2 NULL
 * @objective       Test OCProvisionPairwiseDevices negatively with pDev2 as NULL
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       pDev2 = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionPairwiseDevicesDev2_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Negatively with keysize < Min
 * @objective       Test OCProvisionPairwiseDevices negatively with keySize is out of Lower Boundary Value
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       keySize is out of Lower Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionPairwiseDevicesDeviceKeySize_LOBV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)* @objective Test OCProvisionPairwiseDevices Negatively with keysize > Max
 * @objective       Test OCProvisionPairwiseDevices negatively with keySize is out of Upper Boundary Value
 * @target          OCStackResult OCProvisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2, OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback)
 * @test_data       keySize is out of Upper Boundary Value
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 * @post_condition  None
 * @expected        OCProvisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionPairwiseDevicesDeviceKeySize_UOBV_N)
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
        return;
    }
}
#endif

/**
 * @since           2015-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCUnlinkDevices
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, UnlinkDevices_RSV_SRC_P)
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
}
#endif

/**
 * @since           2015-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCUnlinkDevices
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, UnlinkDevicesDev1_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2016-02-22
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCUnlinkDevices
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, UnlinkDevicesDev2_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2016-02-22
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCUnlinkDevices
 * @post_condition  None
 * @expected        OCUnlinkDevices will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, UnlinkDevicesCB_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2016-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCRemoveDevice
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, RemoveDevice_SRC_RV_P)
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
        return;
    }
}
#endif

/**
 * @since           2016-02-22
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCRemoveDevice
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, RemoveDeviceTime_LBV_P)
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
}
#endif

/**
 * @since           2016-02-22
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCRemoveDevice
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, RemoveDevice_LOBV_N)
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
        return;
    }
}
#endif

/**
 * @since           2016-02-22
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCRemoveDevice
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, RemoveDeviceTargetDev_NV_N)
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

    if (!m_PMHelper.removeDevice((void*)ctxRemoveDevice, DISCOVERY_TIMEOUT, NULL,
                    PMCsdkHelper::removeDeviceCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-02-22
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCRemoveDevice
 * @post_condition  None
 * @expected        OCRemoveDevice will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, RemoveDeviceCB_NV_N)
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
                    NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCGetLinkedStatus
 * @post_condition  None
 * @expected        OCGetLinkedStatus will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetLinkedStatus_RSV_SRC_P)
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
}
#endif

/**
 * @since           2016-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCGetLinkedStatus
 * @post_condition  None
 * @expected        OCGetLinkedStatus will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetLinkedStatusDeviceID_NV_N)
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
        return;
    }
}
#endif

/**
 * @since           2016-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCGetLinkedStatus
 * @post_condition  None
 * @expected        OCGetLinkedStatus will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetLinkedStatusUuidListNV_N)
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

    size_t numOfDevices;

    if (!m_PMHelper.getLinkedStatus(&device1->doxm->deviceID, NULL, &numOfDevices, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-02-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCGetLinkedStatus
 * @post_condition  None
 * @expected        OCGetLinkedStatus will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetLinkedStatusNumDev_NV_N)
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

    if (!m_PMHelper.getLinkedStatus(&device1->doxm->deviceID, &uuidList, NULL ,OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-03
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCGetCredResource
 * @post_condition  None
 * @expected        OCGetCredResource will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetCredResource_RV_SRC_P)
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

    if (!m_PMHelper.getCredResource(NULL, device1, PMCsdkHelper::getCredCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-03
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCGetCredResource
 * @post_condition  None
 * @expected        OCGetCredResource will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetCredResourceDev_NV_N)
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

    if (!m_PMHelper.getCredResource(NULL, NULL, PMCsdkHelper::getCredCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-03
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCGetCredResource
 * @post_condition  None
 * @expected        OCGetCredResource will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetCredResourceCb_NV_N)
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

    if (!m_PMHelper.getCredResource(NULL, device1, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-03
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCGetACLResource
 * @post_condition  None
 * @expected        OCGetACLResource will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetACLResource_RV_SRC_P)
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

    if (!m_PMHelper.getACLResource(NULL, device1, PMCsdkHelper::getAclCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-03
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCGetACLResource
 * @post_condition  None
 * @expected        OCGetACLResource will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetACLResource_NV_N)
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

    if (!m_PMHelper.getACLResource(NULL, NULL, PMCsdkHelper::getAclCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-03
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
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
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionPairwiseDevices
 *                  9. call OCGetACLResource
 * @post_condition  None
 * @expected        OCGetACLResource will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, OCGetACLResourceCb_NV_N)
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

    if (!m_PMHelper.getACLResource(NULL, device1, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

#if defined(__MOT__)
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
TEST_F(PMCsdkTest_btc, OCChangeMOTMode_RV_SRC_P)
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

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMHelper.changeMOTMode((void*)ctxChangeMOTMode, targetDev, OIC_MULTIPLE_OWNER_ENABLE, PMCsdkHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
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
TEST_F(PMCsdkTest_btc, OCChangeMOTMode_CLU_P)
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

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMHelper.changeMOTMode((void*)ctxChangeMOTMode, targetDev, OIC_MULTIPLE_OWNER_DISABLE, PMCsdkHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
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
TEST_F(PMCsdkTest_btc, OCChangeMOTMode_CLU_UOBV_N)
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

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMHelper.changeMOTMode((void*)ctxChangeMOTMode, targetDev, (const OicSecMomType_t) OIC_MULTIPLE_OWNER_UOBV, PMCsdkHelper::changeMOTModeCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
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
TEST_F(PMCsdkTest_btc, OCChangeMOTModeCb_NV_N)
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

    OCProvisionDev_t* targetDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMHelper.changeMOTMode((void*)ctxChangeMOTMode, targetDev, OIC_MULTIPLE_OWNER_ENABLE, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
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
TEST_F(PMCsdkTest_btc, OCChangeMOTModeDev_NV_N)
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

    if (!m_PMHelper.changeMOTMode((void*)ctxChangeMOTMode, NULL, OIC_MULTIPLE_OWNER_DISABLE, PMCsdkHelper::changeMOTModeCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
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
TEST_F(PMCsdkTest_btc, OCProvisionPreconfPin_RV_SRC_P)
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

    OCProvisionDev_t* targetOwnedDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMHelper.changeMOTMode((void*)ctxChangeMOTMode, targetOwnedDev, OIC_MULTIPLE_OWNER_ENABLE, PMCsdkHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t* targetMotDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMHelper.provisionPreconfPin((void*)ctxProvisionPreconfPin, targetMotDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_SIZE, PMCsdkHelper::provisionPreconfPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
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
TEST_F(PMCsdkTest_btc, OCSelectMOTMethod_RV_SRC_P)
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

    OCProvisionDev_t* targetOwnedDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMHelper.changeMOTMode((void*)ctxChangeMOTMode, targetOwnedDev, OIC_MULTIPLE_OWNER_ENABLE, PMCsdkHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t* targetMotDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMHelper.provisionPreconfPin((void*)ctxProvisionPreconfPin, targetMotDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_SIZE, PMCsdkHelper::provisionPreconfPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.selectMOTMethod((void*)ctxSelectMOTMethod, targetMotDev, OIC_PRECONFIG_PIN, PMCsdkHelper::selectMOTMethodCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
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
TEST_F(PMCsdkTest_btc, OCSelectMOTMethodDev_NV_N)
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

    OCProvisionDev_t* targetOwnedDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMHelper.changeMOTMode((void*)ctxChangeMOTMode, targetOwnedDev, OIC_MULTIPLE_OWNER_ENABLE, PMCsdkHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t* targetMotDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMHelper.provisionPreconfPin((void*)ctxProvisionPreconfPin, targetMotDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_SIZE, PMCsdkHelper::provisionPreconfPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.selectMOTMethod((void*)ctxSelectMOTMethod, NULL, OIC_PRECONFIG_PIN, PMCsdkHelper::selectMOTMethodCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
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
TEST_F(PMCsdkTest_btc, OCSelectMOTMethodCb_NV_N)
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

    OCProvisionDev_t* targetOwnedDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_OwnList, DEVICE_INDEX_ONE);

    if (!m_PMHelper.changeMOTMode((void*)ctxChangeMOTMode, targetOwnedDev, OIC_MULTIPLE_OWNER_ENABLE, PMCsdkHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    &m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    OCProvisionDev_t* targetMotDev = PMCsdkUtilityHelper::getDevInst((const OCProvisionDev_t*) m_motEnabledDevList, DEVICE_INDEX_ONE);

    if (!m_PMHelper.provisionPreconfPin((void*)ctxProvisionPreconfPin, targetMotDev, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_SIZE, PMCsdkHelper::provisionPreconfPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.selectMOTMethod((void*)ctxSelectMOTMethod, targetMotDev, OIC_PRECONFIG_PIN, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif
#endif /*MOT*/

#if defined(__CLOUD__)
#if defined(__LINUX__)
/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionTrustCertChain positively with regular data
 * @target          OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for the API
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionTrustCertChain
 * @post_condition  None
 * @expected        OCProvisionTrustCertChain will return OC_STACK_OK
 */
TEST_F(PMCsdkTest_btc, ProvisionTrustCertChain_SRC_RV_P)
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

    ByteArray trustCertChainArray = PMCsdkHelper::getTrustCertChainArray();

    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, 1, device1, PMCsdkHelper::provisionPairwiseCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionTrustCertChain negatively with regular data
 * @target          OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       OCProvisionDev_t = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionTrustCertChain
 * @post_condition  None
 * @expected        OCProvisionTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionTrustCertChainDev_NV_N)
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

    ByteArray trustCertChainArray = PMCsdkHelper::getTrustCertChainArray();

    if (!m_PMHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, 1, NULL, PMCsdkHelper::provisionPairwiseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionTrustCertChain negatively with credId as -1
 * @target          OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       credId = -1
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionTrustCertChain
 * @post_condition  None
 * @expected        OCProvisionTrustCertChain will return OC_STACK_NO_RESOURCE
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionTrustCertChainCredId_LOBV_NV_N)
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

    ByteArray trustCertChainArray = PMCsdkHelper::getTrustCertChainArray();
    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, CRED_ID_NEGATIVE, device1, PMCsdkHelper::provisionPairwiseCB, OC_STACK_NO_RESOURCE))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCSetOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionTrustCertChain negatively with resultCallback as NULL
 * @target          OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCSetOwnerTransferCallbackData
 *                  5. call OCDiscoverUnownedDevices
 *                  6. call OCDoOwnershipTransfer
 *                  7. call OCDiscoverOwnedDevices
 *                  8. call OCProvisionTrustCertChain
 * @post_condition  None
 * @expected        OCProvisionTrustCertChain will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkTest_btc, ProvisionTrustCertChainCb_NV_N)
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

    if (!m_PMHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, 1, device1, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif
#endif /*__CLOUD__*/
