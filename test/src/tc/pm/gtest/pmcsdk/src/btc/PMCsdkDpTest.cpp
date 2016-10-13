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
#if defined(__DIRECTPAIRING__)
#include "PMCsdkDpHelper.h"
#include "PMCsdkUtilityHelper.h"

class PMCsdkDpTest_btc: public ::testing::Test
{

protected:

    PMCsdkDpHelper m_DPHelper;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_JUSTWORKS_SERVER_DP);
        CommonUtil::rmFile(DIRECT_PAIRING_CLIENT_C_DAT);
        CommonUtil::rmFile(JUSTWORKS_SERVER_DP_C_DAT);
        CommonUtil::copyFile(JUSTWORKS_SERVER_DP_DAT_C_BACKUP, JUSTWORKS_SERVER_DP_C_DAT);
        CommonUtil::copyFile(DIRECT_PAIRING_CLIENT_DAT_C_BACKUP, DIRECT_PAIRING_CLIENT_C_DAT);
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_JUSTWORKS_SERVER_DP);
    }
};

/**
 * @since           2016-07-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCDiscoverDirectPairingDevices positively with waittime = DP_DISCOVERY_TIMEOUT
 * @target          const OCDPDev_t* OCDiscoverDirectPairingDevices(unsigned short waittime)
 * @test_data       waittime = DISCOVERY_TIMEOUT
 * @pre_condition   start a direct pairing provisioned Justworks server
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCDiscoverDirectPairingDevices
 * @post_condition  none
 * @expected        OCDiscoverDirectPairingDevices will not return NULL
 */
#if defined(__LINUX__)
TEST_F(PMCsdkDpTest_btc, OCDiscoverDirectPairingDevices_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps =
    {   clientFopenDP, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE(OC_INIT_FAIL);
        return;
    }

    if (!m_DPHelper.discoverDirectPairingDevices(DP_DISCOVERY_TIMEOUT))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCDiscoverDirectPairingDevices positively with waittime = DP_DISCOVERY_TIMEOUT_ZERO
 * @target          const OCDPDev_t* OCDiscoverDirectPairingDevices(unsigned short waittime)
 * @test_data       waittime = DP_DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   start a direct pairing provisioned Justworks server
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCDiscoverDirectPairingDevices
 * @post_condition  none
 * @expected        OCDiscoverDirectPairingDevices will not return NULL
 */
#if defined(__LINUX__)
TEST_F(PMCsdkDpTest_btc, OCDiscoverDirectPairingDevices_LBV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps =
    {   clientFopenDP, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE(OC_INIT_FAIL);
        return;
    }

    if (!m_DPHelper.discoverDirectPairingDevices(DP_DISCOVERY_TIMEOUT_ZERO))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             const OCDPDev_t* OCDiscoverDirectPairingDevices(unsigned short waittime)
 * @objective       Test OCDoDirectPairing positively
 * @target          OCStackResult OCDoDirectPairing(void *ctx, OCDPDev_t* peer, OCPrm_t pmSel, char *pinNumber, OCDirectPairingCB resultCallback)
 * @test_data       Regular data for OCDoDirectPairing
 * @pre_condition   start a direct pairing provisioned Justworks server
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCDiscoverDirectPairingDevices
 *                  4. call OCDoDirectPairing
 * @post_condition  none
 * @expected        OCDoDirectPairing will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkDpTest_btc, OCDoDirectPairing_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps =
    {   clientFopenDP, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE(OC_INIT_FAIL);
        return;
    }

    if (!m_DPHelper.discoverDirectPairingDevices(DP_DISCOVERY_TIMEOUT_ZERO))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }

    if (!m_DPHelper.doDirectPairing(NULL, DP_DEV_INSTANCE_ONE, DP_PRE_CONFIGURED, DP_DEFAULT_PIN,
                    PMCsdkDpHelper::pairingReqCBF, OC_STACK_OK, OC_STACK_OK))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             const OCDPDev_t* OCDiscoverDirectPairingDevices(unsigned short waittime)
 * @objective       Test OCDoDirectPairing negatively with wrong pincode
 * @target          OCStackResult OCDoDirectPairing(void *ctx, OCDPDev_t* peer, OCPrm_t pmSel, char *pinNumber, OCDirectPairingCB resultCallback)
 * @test_data       Wrong pinNumber
 * @pre_condition   start a direct pairing provisioned Justworks server
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCDiscoverDirectPairingDevices
 *                  4. call OCDoDirectPairing
 * @post_condition  none
 * @expected        OCDoDirectPairing's callback will return OC_STACK_AUTHENTICATION_FAILURE
 */
#if defined(__LINUX__)
TEST_F(PMCsdkDpTest_btc, OCDoDirectPairingPinNumber_EG_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps =
    {   clientFopenDP, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE(OC_INIT_FAIL);
        return;
    }

    if (!m_DPHelper.discoverDirectPairingDevices(DP_DISCOVERY_TIMEOUT_ZERO))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }

    if (!m_DPHelper.doDirectPairing(NULL, DP_DEV_INSTANCE_ONE, DP_PRE_CONFIGURED, DP_WRONG_PIN,
                    PMCsdkDpHelper::pairingReqCBF, OC_STACK_OK, OC_STACK_AUTHENTICATION_FAILURE))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             const OCDPDev_t* OCDiscoverDirectPairingDevices(unsigned short waittime)
 * @objective       Test OCDoDirectPairing negatively with NULL pincode
 * @target          OCStackResult OCDoDirectPairing(void *ctx, OCDPDev_t* peer, OCPrm_t pmSel, char *pinNumber, OCDirectPairingCB resultCallback)
 * @test_data       pinNumber = NULL
 * @pre_condition   start a direct pairing provisioned Justworks server
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCDiscoverDirectPairingDevices
 *                  4. call OCDoDirectPairing
 * @post_condition  none
 * @expected        OCDoDirectPairing  will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCsdkDpTest_btc, OCDoDirectPairingPinNumber_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps =
    {   clientFopenDP, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE(OC_INIT_FAIL);
        return;
    }

    if (!m_DPHelper.discoverDirectPairingDevices(DP_DISCOVERY_TIMEOUT_ZERO))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }

    if (!m_DPHelper.doDirectPairing(NULL, DP_DEV_INSTANCE_ONE, DP_PRE_CONFIGURED, NULL,
                    PMCsdkDpHelper::pairingReqCBF, OC_STACK_INVALID_PARAM, OC_STACK_OK))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             const OCDPDev_t* OCDiscoverDirectPairingDevices(unsigned short waittime)
 * @objective       Test OCDoDirectPairing negatively with NULL pincode
 * @target          OCStackResult OCDoDirectPairing(void *ctx, OCDPDev_t* peer, OCPrm_t pmSel, char *pinNumber, OCDirectPairingCB resultCallback)
 * @test_data       pinNumber = NULL
 * @pre_condition   start a direct pairing provisioned Justworks server
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCDiscoverDirectPairingDevices
 *                  4. call OCDoDirectPairing
 * @post_condition  none
 * @expected        OCDoDirectPairing's resultCallback OC_STACK_ERROR
 */
#if defined(__LINUX__)
TEST_F(PMCsdkDpTest_btc, OCDoDirectPairingPmSel_UOBV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps =
    {   clientFopenDP, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE(OC_INIT_FAIL);
        return;
    }

    if (!m_DPHelper.discoverDirectPairingDevices(DP_DISCOVERY_TIMEOUT_ZERO))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }

    if (!m_DPHelper.doDirectPairing(NULL, DP_DEV_INSTANCE_ONE, PMSEL_UOBV, DP_DEFAULT_PIN,
                    PMCsdkDpHelper::pairingReqCBF, OC_STACK_OK, OC_STACK_ERROR))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             const OCDPDev_t* OCDiscoverDirectPairingDevices(unsigned short waittime)
 * @objective       Test OCDoDirectPairing negatively with wrong pmSel
 * @target          OCStackResult OCDoDirectPairing(void *ctx, OCDPDev_t* peer, OCPrm_t pmSel, char *pinNumber, OCDirectPairingCB resultCallback)
 * @test_data       Wrong pmSel
 * @pre_condition   start a direct pairing provisioned Justworks server
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCDiscoverDirectPairingDevices
 *                  4. call OCDoDirectPairing
 * @post_condition  none
 * @expected        OCDoDirectPairing's callback will return OC_STACK_AUTHENTICATION_FAILURE
 */
#if defined(__LINUX__)
TEST_F(PMCsdkDpTest_btc, OCDoDirectPairingPmSel_EG_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps =
    {   clientFopenDP, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE(OC_INIT_FAIL);
        return;
    }

    if (!m_DPHelper.discoverDirectPairingDevices(DP_DISCOVERY_TIMEOUT_ZERO))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }

    if (!m_DPHelper.doDirectPairing(NULL, DP_DEV_INSTANCE_ONE, DP_NOT_ALLOWED, DP_DEFAULT_PIN,
                    PMCsdkDpHelper::pairingReqCBF, OC_STACK_OK, OC_STACK_AUTHENTICATION_FAILURE))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             const OCDPDev_t* OCDiscoverDirectPairingDevices(unsigned short waittime)
 * @objective       Test OCDoDirectPairing negatively with resultCallback as null
 * @target          OCStackResult OCDoDirectPairing(void *ctx, OCDPDev_t* peer, OCPrm_t pmSel, char *pinNumber, OCDirectPairingCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start a direct pairing provisioned Justworks server
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCDiscoverDirectPairingDevices
 *                  4. call OCDoDirectPairing
 * @post_condition  none
 * @expected        OCDoDirectPairing will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkDpTest_btc, OCDoDirectPairingCb_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps =
    {   clientFopenDP, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE(OC_INIT_FAIL);
        return;
    }

    if (!m_DPHelper.discoverDirectPairingDevices(DP_DISCOVERY_TIMEOUT_ZERO))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }

    if (!m_DPHelper.doDirectPairing(NULL, DP_DEV_INSTANCE_ONE, DP_PRE_CONFIGURED, DP_DEFAULT_PIN,
                    NULL, OC_STACK_INVALID_CALLBACK, OC_STACK_OK))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2016-07-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             const OCDPDev_t* OCDiscoverDirectPairingDevices(unsigned short waittime)
 * @see             OCStackResult OCDoDirectPairing(void *ctx, OCDPDev_t* peer, OCPrm_t pmSel, char *pinNumber, OCDirectPairingCB resultCallback)
 * @objective       Test OCgetDirectPairedDevices positively
 * @target          const OCDPDev_t* OCgetDirectPairedDevices()
 * @test_data       None
 * @pre_condition   start a direct pairing provisioned Justworks server
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCDiscoverDirectPairingDevices
 *                  4. call OCDoDirectPairing
 *                  5. call OCgetDirectPairedDevices
 * @post_condition  none
 * @expected        OCgetDirectPairedDevices will return device ID
 */
#if defined(__LINUX__)
TEST_F(PMCsdkDpTest_btc, OCgetDirectPairedDevices_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps =
    {   clientFopenDP, fread, fwrite, fclose, unlink};
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE(OC_INIT_FAIL);
        return;
    }

    if (!m_DPHelper.discoverDirectPairingDevices(DP_DISCOVERY_TIMEOUT_ZERO))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }

    if (!m_DPHelper.doDirectPairing(NULL, DP_DEV_INSTANCE_ONE, DP_PRE_CONFIGURED, DP_DEFAULT_PIN,
                    PMCsdkDpHelper::pairingReqCBF, OC_STACK_OK, OC_STACK_OK))
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }

    if (!m_DPHelper.getDirectPairedDevices())
    {
        SET_FAILURE(m_DPHelper.getFailureMessage());
        return;
    }
}
#endif
#endif /*#if defined(__DIRECTPAIRING__)*/
