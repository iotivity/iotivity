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

#include "PMCppHelper.h"
#include "PMCppUtilityHelper.h"

class PMCppTest_btc: public ::testing::Test
{
protected:
    PMCppHelper m_PMCppHelper;
    DeviceList_t m_UnownedDevList, m_OwnedDevList;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(JUSTWORKS_SERVER);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::rmFile(DATABASE_PDM);
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
        m_UnownedDevList.clear();
        m_OwnedDevList.clear();

        m_PMCppHelper.provisionInit();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(JUSTWORKS_SERVER);
    }
};

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test discoverUnownedDevices positively
 * @target          static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @test_data       timeout = DISCOVERY_TIMEOUT
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 * @post_condition  None
 * @expected        discoverUnownedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, DiscoverUnownedDevices_RV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test discoverUnownedDevices positively with Lower Boundary Value of Time
 * @target          static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @test_data       timeout = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 * @post_condition  None
 * @expected        discoverOwnedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, DiscoverUnownedDevicesTime_LBV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT_ONE, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test discoverUnownedDevices negatively with Out of Lower Boundary value of Time
 * @target          static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @test_data       timeout = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 * @post_condition  None
 * @expected        discoverUnownedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, DiscoverUnownedDevicesTime_LOBV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT_ZERO, m_UnownedDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test discoverOwnedDevices positively
 * @target          static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @test_data       timeout = DISCOVERY_TIMEOUT
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 * @post_condition  None
 * @expected        discoverOwnedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, DiscoverOwnedDevices_RV_P)
{
    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test discoverOwnedDevices positively with Lower Boundary Value of Time
 * @target          static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @test_data       timeout = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 * @post_condition  None
 * @expected        discoverOwnedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, DiscoverOwnedDevicesTime_LBV_P)
{
    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT_ONE, m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test discoverOwnedDevices negatively with Outer Lower Boundary Value of Time
 * @target          static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @test_data       timeout = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 * @post_condition  None
 * @expected        discoverOwnedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, DiscoverOwnedDevicesTime_LOBV_N)
{
    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT_ZERO, m_OwnedDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test getDevInfoFromNetwork positively
 * @target          static OCStackResult getDevInfoFromNetwork(unsigned short timeout, DeviceList_t &ownedDevList, DeviceList_t &unownedDevList)
 * @test_data       timeout = DISCOVERY_TIMEOUT
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call getDevInfoFromNetwork
 * @post_condition  None
 * @expected        getDevInfoFromNetwork will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, GetDevInfoFromNetwork_RV_P)
{
    if(!m_PMCppHelper.getDevInfoFromNetwork(DISCOVERY_TIMEOUT, m_OwnedDevList, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test getDevInfoFromNetwork positively with Lower Boundary Value of Time
 * @target          static OCStackResult getDevInfoFromNetwork(unsigned short timeout, DeviceList_t &ownedDevList, DeviceList_t &unownedDevList)
 * @test_data       timeout = DISCOVERY_TIMEOUT_TWO
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call getDevInfoFromNetwork
 * @post_condition  None
 * @expected        getDevInfoFromNetwork will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, GetDevInfoFromNetworkTime_LBV_P)
{
    if(!m_PMCppHelper.getDevInfoFromNetwork(DISCOVERY_TIMEOUT_TWO, m_OwnedDevList, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test getDevInfoFromNetwork negatively with Outer Lower Boundary Value of Time
 * @target          static OCStackResult getDevInfoFromNetwork(unsigned short timeout, DeviceList_t &ownedDevList, DeviceList_t &unownedDevList)
 * @test_data       timeout = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call getDevInfoFromNetwork
 * @post_condition  None
 * @expected        getDevInfoFromNetwork will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, GetDevInfoFromNetworkTime_LOBV_N)
{
    if(!m_PMCppHelper.getDevInfoFromNetwork(DISCOVERY_TIMEOUT_ONE, m_OwnedDevList, m_UnownedDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setOwnerTransferCallbackData positively
 * @target          static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @test_data       oxm = OIC_JUST_WORKS & callbackData = justWorksCBData
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call setOwnerTransferCallbackData
 * @post_condition  None
 * @expected        setOwnerTransferCallbackData will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, SetOwnerTransferCBDataJustWork_RV_P)
{
    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setOwnerTransferCallbackData positively
 * @target          static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @test_data       oxm = Representative value of OIC_JUST_WORKS & callbackData = justWorksCBData
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call setOwnerTransferCallbackData
 * @post_condition  None
 * @expected        setOwnerTransferCallbackData will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, SetOwnerTransferCBDataJustWork_REV_P)
{
    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OTM_JUSTWORK, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setOwnerTransferCallbackData negatively with inputCB as NULL
 * @target          static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @test_data       oxm = OIC_JUST_WORKS & inputPIN = NULL
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call setOwnerTransferCallbackData
 * @post_condition  None
 * @expected        setOwnerTransferCallbackData will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, SetOwnerTransferCBDataJustWorkInvalidCB_N)
{
    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_OXM_COUNT, justWorksCBData, NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setOwnerTransferCallbackData negatively with callbackData as NULL
 * @target          static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @test_data       oxm = OIC_JUST_WORKS & callbackData = NULL
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call setOwnerTransferCallbackData
 * @post_condition  None
 * @expected        setOwnerTransferCallbackData will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, SetOwnerTransferCBDataJustWorkInvalidParam_N)
{
    OCStackResult expetedResult = OC_STACK_INVALID_PARAM;
    OCStackResult actualResult = OCSecure::setOwnerTransferCallbackData(OIC_JUST_WORKS, NULL, NULL);

    ASSERT_EQ(expetedResult, actualResult);
}
#endif

/**
 * @since           2015-11-30
 * @objective       test setOwnerTransferCallbackData negatively with Outer Upper Boundary Value of oxm
 * @target          static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @test_data       oxm = Outer Upper Boundary Value of oxm
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call setOwnerTransferCallbackData
 * @post_condition  None
 * @expected        setOwnerTransferCallbackData will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, SetOwnerTransferCBDataJustWorkOxm_UOBV_P)
{
    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    OCStackResult expetedResult = OC_STACK_INVALID_PARAM;
    OCStackResult actualResult = OCSecure::setOwnerTransferCallbackData((OicSecOxm_t) OTM_INVALID_UOBV, &justWorksCBData, NULL);

    ASSERT_EQ(expetedResult, actualResult);
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setOwnerTransferCallbackData positively with OIC_RANDOM_DEVICE_PIN
 * @target          static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @test_data       oxm = OIC_RANDOM_DEVICE_PIN & callbackData = pinBasedCBData
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call setOwnerTransferCallbackData
 * @post_condition  None
 * @expected        setOwnerTransferCallbackData will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, SetOwnerTransferCBDataRanDomPin_P)
{
    OTMCallbackData_t pinBasedCBData;
    pinBasedCBData.loadSecretCB = InputPinCodeCallback;
    pinBasedCBData.createSecureSessionCB =
    CreateSecureSessionRandomPinCallback;
    pinBasedCBData.createSelectOxmPayloadCB =
    CreatePinBasedSelectOxmPayload;
    pinBasedCBData.createOwnerTransferPayloadCB =
    CreatePinBasedOwnerTransferPayload;
    OCSecure::setOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN,
            &pinBasedCBData, InputPinCB);

    OCStackResult expetedResult = OC_STACK_OK;
    OCStackResult actualResult = OCSecure::setOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN,
            &pinBasedCBData, InputPinCB);

    ASSERT_EQ(expetedResult, actualResult);
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setOwnerTransferCallbackData positively with OIC_RANDOM_DEVICE_PIN and inputPin as NULL
 * @target          static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @test_data       oxm = OIC_RANDOM_DEVICE_PIN & inputPin = NULL
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call setOwnerTransferCallbackData
 * @post_condition  None
 * @expected        setOwnerTransferCallbackData will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, SetOwnerTransferCBDataRanDomPinCB_NV_N)
{
    OTMCallbackData_t pinBasedCBData;
    pinBasedCBData.loadSecretCB = InputPinCodeCallback;
    pinBasedCBData.createSecureSessionCB =
    CreateSecureSessionRandomPinCallback;
    pinBasedCBData.createSelectOxmPayloadCB =
    CreatePinBasedSelectOxmPayload;
    pinBasedCBData.createOwnerTransferPayloadCB =
    CreatePinBasedOwnerTransferPayload;
    OCSecure::setOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN,
            &pinBasedCBData, InputPinCB);

    OCStackResult expetedResult = OC_STACK_INVALID_PARAM;
    OCStackResult actualResult = OCSecure::setOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN,
            &pinBasedCBData, NULL);

    ASSERT_EQ(expetedResult, actualResult);
}
#endif

/**
 * @since           2015-11-30
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setOwnerTransferCallbackData positively with OIC_RANDOM_DEVICE_PIN and callbackData as NULL
 * @target          static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @test_data       oxm = OIC_RANDOM_DEVICE_PIN & callbackData = NULL
 * @pre_condition   None
 * @procedure       1. call provisionInit
 *                  2. call setOwnerTransferCallbackData
 * @post_condition  None
 * @expected        setOwnerTransferCallbackData will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, SetOwnerTransferCBDataRanDomPinData_NV_N)
{
    OCStackResult expetedResult = OC_STACK_INVALID_PARAM;
    OCStackResult actualResult = OCSecure::setOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN,
            NULL, InputPinCB);

    ASSERT_EQ(expetedResult, actualResult);
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test doOwnershipTransfer negatively for OIC_JUST_WORKS
 * @target          OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 * @post_condition  None
 * @expected        doOwnershipTransfer will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, DoOwnershipTransferJustWorkCB_NV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionACL positively
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionACL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionACL
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionAcl_RV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, acl1, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionACL negatively with acl as NULL
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       acl = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionACL
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionAcl_NV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, NULL, PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionACL negatively with resultCallback as NULL
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionACL
 *                  7. call provisionACL
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionAclCB_NV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, acl1, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, acl1, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials positively
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionCredentials
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionCredentialsKeysize_FSV_LBV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials positively with Upper Boundary Value of keysize for Credential
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Credentail with keySize = OWNER_PSK_LENGTH_256
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionCredentialsKeysize_UBV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionACL(const OicSecAcl_t* acl, ResultCallBack resultCallback)
 * @objective       test provisionCredentials positively with resultCallback as NULL
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionACL
 *                  7. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionCredentialsCB_NV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, acl1, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively with Outer Lower Boundary Value of keySize
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Outer Lower Boundary Value of keySize
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionCredentialsKeysize_LOBV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128-1;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively with Outer Upper Boundary Value of keySize
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Outer Upper Boundary Value of keySize
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionCredentialsKeysize_UOBV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256 + 1;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively with Outer Lower Boundary Value of credType
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Outer Lower Boundary Value of credType
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionCredentialsCredType_LOBV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecCredType_t type = (OicSecCredType_t) CRED_TYPE_LOBV;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively with Outer Upper Boundary Value of credType
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Outer Upper Boundary Value of credType
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionCredentialsCredType_UOBV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecCredType_t type = (OicSecCredType_t) CRED_TYPE_UOBV;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionCredentials negatively for same device
 * @target          OCStackResult provisionCredentials(const Credential &cred, const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       credential between same devices
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionCredentials
 * @post_condition  None
 * @expected        provisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionCredentialsSameDev_EG_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecCredType_t type = (OicSecCredType_t) 1;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionCredentials(m_OwnedDevList, cred, *m_OwnedDevList[0].get(), PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices positively
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @test_data       Regular data for provisionPairwiseDevices
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionPairwiseDevicesKeySize_LBV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type , keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices positively with keySize as OWNER_PSK_LENGTH_256
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @test_data       keySize = OWNER_PSK_LENGTH_256
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionPairwiseDevicesKeySize_FSV_UBV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices negatively with Outer Lower Boundary Value of keySize
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @test_data       Outer Lower Boundary Value of keySize
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionPairwiseDevicesKeySize_LOBV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128-1;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices negatively with Outer Upper Boundary Value of keySize
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @test_data       Outer Upper Boundary Value of keySize
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionPairwiseDevicesKeySize_UOBV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256+1;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices negatively with resultCallback as null
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionPairwiseDevicesCB_NV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices positively with acl1 = NULL
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @test_data       acl1 = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionPairwiseDevicesAcl1_NV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, NULL, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionPairwiseDevices positively with acl2 = NULL
 * @target          OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @test_data       acl2 = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 * @post_condition  None
 * @expected        provisionPairwiseDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionPairwiseDevicesAcl2_NV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), NULL, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @objective       test getLinkedDevices positively
 * @target          OCStackResult getLinkedDevices(UuidList_t &uuidList)
 * @test_data       Regular data for getLinkedDevices
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 *                  7. call getLinkedDevices
 * @post_condition  None
 * @expected        getLinkedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, GetLinkedDevices_FSV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    UuidList_t uuidList1;

    if(!m_PMCppHelper.getLinkedDevices(m_OwnedDevList, uuidList1, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    SUCCEED();
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @objective       test unlinkDevices positively
 * @target          OCStackResult unlinkDevices(const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Regular data for unlinkDevices
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 *                  7. call unlinkDevices
 * @post_condition  None
 * @expected        unlinkDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, UnlinkDevices_RV_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.unlinkDevices(m_OwnedDevList, *m_OwnedDevList[1].get(), PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @objective       test unlinkDevices negatively with resultCallback as NULL
 * @target          OCStackResult unlinkDevices(const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 *                  7. call unlinkDevices
 * @post_condition  None
 * @expected        unlinkDevices will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, UnlinkDevicesCB_NV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.unlinkDevices(m_OwnedDevList, *m_OwnedDevList[1].get(), NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @objective       test removeDevice negatively with waitTimeForOwnedDeviceDiscovery as DISCOVERY_TIMEOUT_ZERO
 * @target          OCStackResult removeDevice(unsigned short waitTimeForOwnedDeviceDiscovery, ResultCallBack resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 *                  7. call removeDevice
 * @post_condition  None
 * @expected        removeDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, RemoveDeviceTime_LOBV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDevice(m_OwnedDevList, DISCOVERY_TIMEOUT_ZERO, PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2, ResultCallBack resultCallback)
 * @objective       test removeDevice negatively with resultCallback as NULL
 * @target          OCStackResult removeDevice(unsigned short waitTimeForOwnedDeviceDiscovery, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionPairwiseDevices
 *                  7. call removeDevice
 * @post_condition  None
 * @expected        removeDevice will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, RemoveDeviceCB_NV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);
    OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, m_OwnedDevList);
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if(!m_PMCppHelper.provisionPairwiseDevices(m_OwnedDevList, cred, acl1, *m_OwnedDevList[1].get(), acl2, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.removeDevice(m_OwnedDevList, DISCOVERY_TIMEOUT, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test getDeviceID positively
 * @target          std::string getDeviceID()
 * @test_data       None
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3, call getDeviceID
 * @post_condition  None
 * @expected        getDeviceID will call without throwing exception
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, GetDeviceID_SRC_P)
{

    if (!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    try
    {
        m_UnownedDevList[0]->getDeviceID();
    }
    catch (...)
    {
        SET_FAILURE("getDeviceID Failed");
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test getDevAddr positively
 * @target          std::string getDevAddr()
 * @test_data       None
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3, call getDevAddr
 * @post_condition  None
 * @expected        getDevAddr will call without throwing exception
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, GetDevAddr_SRC_P)
{
    if (!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    try
    {
        m_UnownedDevList[0]->getDevAddr();
    }
    catch (exception e)
    {
        SET_FAILURE("getDeviceID Failed");
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test getDeviceStatus positively
 * @target          int getDeviceStatus()
 * @test_data       None
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3, call getDeviceStatus
 * @post_condition  None
 * @expected        getDeviceStatus will retunr ON
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, GetDeviceStatusOn_SRC_P)
{
    if (!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if (ON != m_UnownedDevList[0]->getDeviceStatus())
    {
        SET_FAILURE("getDeviceStatus Failed");
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test getOwnedStatus positively
 * @target          int getOwnedStatus()
 * @test_data       None
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3, call getOwnedStatus
 * @post_condition  None
 * @expected        getOwnedStatus will return DEVICE_UNOWNED
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, GetOwnedStatusUnowned_SRC_P)
{
    if (!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if (DEVICE_UNOWNED != m_UnownedDevList[0]->getOwnedStatus())
    {
        SET_FAILURE("getOwnedStatus Failed");
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test getOwnedStatus positively to Check if Owned devices return Owned Status as DEVICE_OWNED
 * @target          bool getOwnedStatus()
 * @test_data       Outer Upper Boundary Value of keySize
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call getOwnedStatus
 * @post_condition  None
 * @expected        getOwnedStatus will return DEVICE_OWNED
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, GetOwnedStatusOwned_SRCC_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT,m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, acl1, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if (DEVICE_OWNED != m_OwnedDevList[0]->getOwnedStatus())
    {
        SET_FAILURE("getOwnedStatus Failed");
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test getDevPtr positively
 * @target          OCProvisionDev_t* getDevPtr()const
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call getDevPtr
 * @post_condition  None
 * @expected        getDevPtr will return no Exception
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, GetDevPtr_SRC_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    try
    {
        m_UnownedDevList[0]->getDevPtr();
    }
    catch (...)
    {
        SET_FAILURE("getDevPtr throws exception");
    }
}
#endif

/**
 * @since 2016-02-29
 * @objective Test getCredentialType
 * @target OicSecCredType_t getCredentialType() const
 * @test_data getCredentialType
 * @pre_condition None
 * @procedure call getCredentialType
 * @post_condition None
 * @expected getCredentialType will return SYMMETRIC_PAIR_WISE_KEY
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, GetCredentialType_SRC_P)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if (SYMMETRIC_PAIR_WISE_KEY != cred.getCredentialType())
    {
        SET_FAILURE("getCredentialType Failed");
    }
}
#endif

/**
 * @since 2016-02-29
 * @see 1. getCredentialKeySize()
 * @objective Test getCredentialKeySize
 * @target size_t getCredentialKeySize() const
 * @test_data getCredentialKeySize
 * @pre_condition None
 * @procedure call getCredentialKeySize
 * @post_condition None
 * @expected getCredentialKeySize will return OWNER_PSK_LENGTH_128
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, GetCredentialKeySize_SRC_P)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    if (OWNER_PSK_LENGTH_128 != cred.getCredentialKeySize())
    {
        SET_FAILURE("getCredentialKeySize Failed");
    }
}
#endif

/**
 * @since 2016-02-29
 * @see 1. setCredentialType()
 * @objective Test setCredentialType
 * @target void setCredentialType(OicSecCredType_t type)
 * @test_data setCredentialType
 * @pre_condition None
 * @procedure call setCredentialType
 * @post_condition None
 * @expected setCredentialType will set SYMMETRIC_GROUP_KEY
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, SetCredentialType_SRC_P)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    cred.setCredentialType(SYMMETRIC_GROUP_KEY);

    if (SYMMETRIC_GROUP_KEY != cred.getCredentialType())
    {
        SET_FAILURE("getCredentialType Failed");
    }

}
#endif

/**
 * @since 2016-02-29
 * @see 1. setCredentialKeySize()
 * @objective Test setCredentialKeySize
 * @target void setCredentialKeySize(size_t keySize)
 * @test_data setCredentialKeySize
 * @pre_condition None
 * @procedure call setCredentialKeySize
 * @post_condition None
 * @expected setCredentialKeySize will set OWNER_PSK_LENGTH_256
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, SetCredentialKeySize_SRC_P)
{
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;
    Credential cred(type, keySize);

    cred.setCredentialKeySize(OWNER_PSK_LENGTH_256);

    if (OWNER_PSK_LENGTH_256 != cred.getCredentialKeySize())
    {
        SET_FAILURE("getCredentialKeySize Failed");
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setDisplayPinCB negatively with displayPin as nullptr
 * @target          static OCStackResult setDisplayPinCB(GeneratePinCallback displayPin)
 * @test_data       displayPin == nullptr
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  3. call setDisplayPinCB
 * @post_condition  None
 * @expected        setDisplayPinCB will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, SetDisplayPinCB_N)
{
    OCStackResult expetedResult = OC_STACK_INVALID_PARAM;
    OCStackResult actualResult = OCSecure::setDisplayPinCB(nullptr);

    if (expetedResult != actualResult)
    {
        SET_FAILURE("setDisplayPinCB Failed");
    }
}
#endif
#if defined(__CLOUD__)

/**
 * @since           2016-10-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionTrustCertChain positively
 * @target          OCStackResult provisionTrustCertChain(OicSecCredType_t type, uint16_t credId, ResultCallBack resultCallback)
 * @test_data       Regular data for getLinkedDevices
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionTrustCertChain
 * @post_condition  None
 * @expected        provisionTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionTrustCertChain_RV_SRC_P)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.provisionTrustCertChain(m_OwnedDevList, SIGNED_ASYMMETRIC_KEY, CRED_ID_ONE, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setOwnerTransferCallbackData(OicSecOxm_t oxm, OTMCallbackData_t* callbackData, InputPinCallback inputPin)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test provisionTrustCertChain negatively with OicSecCredType_t as SYMMETRIC_PAIR_WISE_KEY
 * @target          OCStackResult provisionTrustCertChain(OicSecCredType_t type, uint16_t credId, ResultCallBack resultCallback)
 * @test_data       Regular data for getLinkedDevices
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call setOwnerTransferCallbackData
 *                  4. call doOwnershipTransfer
 *                  5. call discoverOwnedDevices
 *                  6. call provisionTrustCertChain
 * @post_condition  None
 * @expected        provisionTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionTrustCertChainCredType_EG_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.provisionTrustCertChain(m_OwnedDevList, SYMMETRIC_PAIR_WISE_KEY, CRED_ID_ONE, PMCppHelper::provisionCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

#if defined(__LINUX__)
TEST_F(PMCppTest_btc, ProvisionTrustCertChainCb_NV_N)
{
    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t justWorksCBData;
    justWorksCBData.loadSecretCB = LoadSecretJustWorksCallback;
    justWorksCBData.createSecureSessionCB =
    CreateSecureSessionJustWorksCallback;
    justWorksCBData.createSelectOxmPayloadCB =
    CreateJustWorksSelectOxmPayload;
    justWorksCBData.createOwnerTransferPayloadCB =
    CreateJustWorksOwnerTransferPayload;

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, PMCppHelper::ownershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, m_OwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OicSecCredType_t type = SIGNED_ASYMMETRIC_KEY;

    if(!m_PMCppHelper.provisionTrustCertChain(m_OwnedDevList, type, 1, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif
#endif
