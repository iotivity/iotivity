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
#include "PMCppMotHelper.h"
#include "PMCppUtilityHelper.h"
#if defined(__MOT__)
class PMCppMotTest_btc: public ::testing::Test
{
protected:
    PMCppHelper m_PMCppHelper;
    PMCppMotHelper m_PMCppMotHelper;
    DeviceList_t m_UnownedDevList, m_OwnedDevList, m_motEnabledDevList, m_motOwnedDevList;
    OicSecAcl_t *m_acl1, *m_acl2;
    std::shared_ptr< OCSecureResource > m_SingleDevList;
    OicUuid_t devUuid;
    bool subowner;

    virtual void SetUp()
    {
        CommonUtil::killApp(KILL_SERVERS);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        PMCppUtilityHelper::removeAllResFile();
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::copyFile(PRECONFIG_SERVER1_CBOR_BACKUP, PRECONFIG_SERVER1_CBOR);
        CommonUtil::copyFile(RANDOMPIN_SERVER_CBOR_BACKUP, RANDOMPIN_SERVER_CBOR);
        CommonUtil::copyFile(CLIENT_CBOR_BACKUP, CLIENT_CBOR);
        CommonUtil::copyFile(JUSTWORKS_SERVER7_CBOR_BACKUP, JUSTWORKS_SERVER7_CBOR);
        CommonUtil::copyFile(PRECONFIG_SERVER2_CBOR_BACKUP, PRECONFIG_SERVER2_CBOR);
        CommonUtil::copyFile(MOT_CLIENT_CBOR_BACKUP, MOT_CLIENT_CBOR);
        m_UnownedDevList.clear();
        m_OwnedDevList.clear();
        m_motEnabledDevList.clear();
        m_motOwnedDevList.clear();
        m_acl1 = NULL;
        m_acl2 = NULL;
        devUuid =
        {   0,0};
        m_SingleDevList = nullptr;
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);
    }
};

/**
 * @since           2017-02-23
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       Test changeMOTMode positively with regular data
 * @target          OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @test_data       regular data for the target API
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 * @post_condition  None
 * @expected        changeMOTMode will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, changeMOTMode_RV_SRC_P)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-23
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       Test changeMOTMode positively with momType=OIC_MULTIPLE_OWNER_DISABLE
 * @target          OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback);
 * @test_data       momType=OIC_MULTIPLE_OWNER_DISABLE
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 * @post_condition  None
 * @expected        changeMOTMode will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, changeMOTModeDisable_RV_SRC_P)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_DISABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-23
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       Test changeMOTMode Negatively resultCallback=NULL
 * @target          OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback);
 * @test_data       resultCallback=NULL
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 * @post_condition  None
 * @expected        changeMOTMode will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, changeMOTModeResultCallback_NV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-23
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @objective       Test discoverMultipleOwnerEnabledDevice positively with regular data
 * @target          static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @test_data       regular data for the target API
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 * @post_condition  None
 * @expected        discoverMultipleOwnerEnabledDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, DiscoverMultipleOwnerEnabledDevices_RV_SRC_P)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-23
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @objective       Test discoverMultipleOwnerEnabledDevice positively with timeout=DISCOVERY_TIMEOUT_ONE
 * @target          static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @test_data       timeout=DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 * @post_condition  None
 * @expected        discoverMultipleOwnerEnabledDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, DiscoverMultipleOwnerEnabledDevices_LBV_P)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT_ONE,m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-23
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @objective       Test discoverMultipleOwnerEnabledDevice negatively with timeout=DISCOVERY_TIMEOUT_ZERO
 * @target          static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @test_data       timeout=DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 * @post_condition  None
 * @expected        discoverMultipleOwnerEnabledDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, DiscoverMultipleOwnerEnabledDevices_LOBV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT_ZERO,m_motEnabledDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-03
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @objective       Test discoverMultipleOwnerEnabledDevice positively with regular data
 * @target          static OCStackResult discoverMultipleOwnerEnabledDevice(unsigned short timeout,const OicUuid_t* deviceID,std::shared_ptr<OCSecureResource> &foundDevice)
 * @test_data       regular data for the target API
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 * @post_condition  None
 * @expected        discoverMultipleOwnerEnabledDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, DiscoverMultipleOwnerEnabledDevicesWithDeviceID_RV_SRC_P)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.convertStrToUuid(m_OwnedDevList[0]->getDeviceID(), &devUuid, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevice(DISCOVERY_TIMEOUT, &devUuid, m_SingleDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-03
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @objective       Test discoverMultipleOwnerEnabledDevice positively with timeout=DISCOVERY_TIMEOUT_ONE
 * @target          static OCStackResult discoverMultipleOwnerEnabledDevice(unsigned short timeout,const OicUuid_t* deviceID,std::shared_ptr<OCSecureResource> &foundDevice)
 * @test_data       timeout=DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 * @post_condition  None
 * @expected        discoverMultipleOwnerEnabledDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, DiscoverMultipleOwnerEnabledDevicesWithDeviceID_LBV_P)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.convertStrToUuid(m_OwnedDevList[0]->getDeviceID(), &devUuid, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevice(DISCOVERY_TIMEOUT_ONE, &devUuid, m_SingleDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-03
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @objective       Test discoverMultipleOwnerEnabledDevice negatively with timeout=DISCOVERY_TIMEOUT_ZERO
 * @target          static OCStackResult discoverMultipleOwnerEnabledDevice(unsigned short timeout,const OicUuid_t* deviceID,std::shared_ptr<OCSecureResource> &foundDevice)
 * @test_data       timeout=DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 * @post_condition  None
 * @expected        discoverMultipleOwnerEnabledDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, DiscoverMultipleOwnerEnabledDevicesWithDeviceID_LOBV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.convertStrToUuid(m_OwnedDevList[0]->getDeviceID(), &devUuid, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevice(DISCOVERY_TIMEOUT_ZERO, &devUuid, m_SingleDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-03
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @objective       Test discoverMultipleOwnerEnabledDevice negatively with deviceID=NULL
 * @target          static OCStackResult discoverMultipleOwnerEnabledDevice(unsigned short timeout,const OicUuid_t* deviceID,std::shared_ptr<OCSecureResource> &foundDevice)
 * @test_data       deviceID=NULL
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 * @post_condition  None
 * @expected        discoverMultipleOwnerEnabledDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, DiscoverMultipleOwnerEnabledDevicesWithDeviceID_NV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevice(DISCOVERY_TIMEOUT, NULL, m_SingleDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test provisionPreconfPin positively with regular data
 * @target          OCStackResult provisionPreconfPin(const char * preconfPin,size_t preconfPinLength, ResultCallBack resultCallback)
 * @test_data       regular data for the target API
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 *                  7. call provisionPreconfPin
 * @post_condition  None
 * @expected        provisionPreconfPin will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, ProvisionPreconfPin_RV_SRC_P)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.provisionPreconfPin(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, strlen(MOT_DEFAULT_PRE_CONFIG_PIN), PMCppMotHelper::provisionPreconfPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test provisionPreconfPin Negatively with preconfPin=NULL
 * @target          OCStackResult provisionPreconfPin(const char * preconfPin,size_t preconfPinLength, ResultCallBack resultCallback)
 * @test_data       preconfPin=NULL
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 *                  7. call provisionPreconfPin
 * @post_condition  None
 * @expected        provisionPreconfPin will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, ProvisionPreconfPinPreconfPin_NV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.provisionPreconfPin(m_motEnabledDevList, NULL, strlen(MOT_DEFAULT_PRE_CONFIG_PIN), PMCppMotHelper::provisionPreconfPinCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test provisionPreconfPin Negatively with preconfPinLength=MOT_PRECONFIG_PIN_ZERO_SIZE
 * @target          OCStackResult provisionPreconfPin(const char * preconfPin,size_t preconfPinLength, ResultCallBack resultCallback)
 * @test_data       preconfPinLength=MOT_PRECONFIG_PIN_ZERO_SIZE
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 *                  7. call provisionPreconfPin
 * @post_condition  None
 * @expected        provisionPreconfPin will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, ProvisionPreconfPinpreconfPinLength_NV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.provisionPreconfPin(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, MOT_PRECONFIG_PIN_ZERO_SIZE, PMCppMotHelper::provisionPreconfPinCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test provisionPreconfPin Negatively with resultCallback=NULL
 * @target          OCStackResult provisionPreconfPin(const char * preconfPin,size_t preconfPinLength, ResultCallBack resultCallback)
 * @test_data       resultCallback=NULL
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 *                  7. call provisionPreconfPin
 * @post_condition  None
 * @expected        provisionPreconfPin will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, ProvisionPreconfPinResultCallback_NV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.provisionPreconfPin(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, strlen(MOT_DEFAULT_PRE_CONFIG_PIN), NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-03
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @see             OCStackResult provisionPreconfPin(const char * preconfPin,size_t preconfPinLength, ResultCallBack resultCallback)
 * @objective       Test selectMOTMethod positively with regular data
 * @target          OCStackResult selectMOTMethod( const OicSecOxm_t oxmSelVal,ResultCallBack resultCallback)
 * @test_data       regular data
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 *                  7. call provisionPreconfPin
 *                  8. call selectMOTMethod
 * @post_condition  None
 * @expected        selectMOTMethod will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, SelectMOTMethod_RV_SRC_P)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.provisionPreconfPin(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, strlen(MOT_DEFAULT_PRE_CONFIG_PIN), PMCppMotHelper::provisionPreconfPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.selectMOTMethod(m_motEnabledDevList, OIC_PRECONFIG_PIN, PMCppMotHelper::selectMOTMethodCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult changeMOTMode( const OicSecMomType_t momType,ResultCallBack resultCallback)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @see             OCStackResult provisionPreconfPin(const char * preconfPin,size_t preconfPinLength, ResultCallBack resultCallback)
 * @objective       Test selectMOTMethod negatively with resultCallback=NULL
 * @target          OCStackResult selectMOTMethod( const OicSecOxm_t oxmSelVal,ResultCallBack resultCallback)
 * @test_data       resultCallback=NULL
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call changeMOTMode
 *                  6. call discoverMultipleOwnerEnabledDevice
 *                  7. call provisionPreconfPin
 *                  8. call selectMOTMethod
 * @post_condition  None
 * @expected        selectMOTMethod will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, SelectMOTMethodResultCallback_NV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.changeMOTMode(m_OwnedDevList, OIC_MULTIPLE_OWNER_ENABLE, PMCppMotHelper::changeMOTModeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.provisionPreconfPin(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, strlen(MOT_DEFAULT_PRE_CONFIG_PIN), PMCppMotHelper::provisionPreconfPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.selectMOTMethod(m_motEnabledDevList, OIC_PRECONFIG_PIN, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       Test discoverMultipleOwnerEnabledDevices positively with regular data
 * @target          static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @test_data       resultCallback=NULL
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 * @post_condition  None
 * @expected        discoverMultipleOwnerEnabledDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, SelectDiscoverMultipleOwnerEnabledDevices_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       Test discoverMultipleOwnerEnabledDevices positively with timeout=DISCOVERY_TIMEOUT_ONE
 * @target          static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @test_data       timeout=DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 * @post_condition  None
 * @expected        discoverMultipleOwnerEnabledDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, SelectDiscoverMultipleOwnerEnabledDevices_LBV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT_ONE,m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       Test discoverMultipleOwnerEnabledDevices positively with timeout=DISCOVERY_TIMEOUT_ZERO
 * @target          static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @test_data       timeout=DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 * @post_condition  None
 * @expected        discoverMultipleOwnerEnabledDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, SelectDiscoverMultipleOwnerEnabledDevices_LOBV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT_ZERO,m_motEnabledDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test addPreconfigPIN positively with regular data
 * @target          OCStackResult addPreconfigPIN(const char* preconfPIN,size_t preconfPINLength)
 * @test_data       regular data for addPreconfigPIN
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call addPreconfigPIN
 * @post_condition  None
 * @expected        addPreconfigPIN will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, AddPreconfigPIN_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.addPreconfigPIN(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test addPreconfigPIN negatively with preconfPIN=NULL
 * @target          OCStackResult addPreconfigPIN(const char* preconfPIN,size_t preconfPINLength)
 * @test_data       preconfPIN=NULL
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call addPreconfigPIN
 * @post_condition  None
 * @expected        addPreconfigPIN will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, AddPreconfigPINPreconfPIN_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.addPreconfigPIN(m_motEnabledDevList, NULL, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test addPreconfigPIN negatively with preconfPINLength=MOT_PRECONFIG_PIN_ZERO_SIZE
 * @target          OCStackResult addPreconfigPIN(const char* preconfPIN,size_t preconfPINLength)
 * @test_data       preconfPINLength=MOT_PRECONFIG_PIN_ZERO_SIZE
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call addPreconfigPIN
 * @post_condition  None
 * @expected        addPreconfigPIN will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, AddPreconfigPINPreconfPINLength_LOBV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.addPreconfigPIN(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, MOT_PRECONFIG_PIN_ZERO_SIZE, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @see             OCStackResult addPreconfigPIN(const char* preconfPIN,size_t preconfPINLength)
 * @objective       Test doMultipleOwnershipTransfer positively with regular data
 * @target          OCStackResult doMultipleOwnershipTransfer(ResultCallBack resultCallback)
 * @test_data       regular data for doMultipleOwnershipTransfer
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call addPreconfigPIN
 *                  4. call doMultipleOwnershipTransfer
 * @post_condition  None
 * @expected        doMultipleOwnershipTransfer will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, DoMultipleOwnershipTransfer_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.addPreconfigPIN(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.doMultipleOwnershipTransfer(m_motEnabledDevList,PMCppMotHelper::multipleOwnershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @see             OCStackResult addPreconfigPIN(const char* preconfPIN,size_t preconfPINLength)
 * @objective       Test doMultipleOwnershipTransfer negatively with resultCallback=NULL
 * @target          OCStackResult doMultipleOwnershipTransfer(ResultCallBack resultCallback)
 * @test_data       resultCallback=NULL
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call addPreconfigPIN
 *                  4. call doMultipleOwnershipTransfer
 * @post_condition  None
 * @expected        doMultipleOwnershipTransfer will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, DoMultipleOwnershipTransferResultCallback_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.addPreconfigPIN(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.doMultipleOwnershipTransfer(m_motEnabledDevList,NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @see             OCStackResult addPreconfigPIN(const char* preconfPIN,size_t preconfPINLength)
 * @see             OCStackResult doMultipleOwnershipTransfer(ResultCallBack resultCallback)
 * @objective       Test discoverMultipleOwnedDevices positively with regular data
 * @target          static OCStackResult discoverMultipleOwnedDevices(unsigned short timeout,DeviceList_t &list)
 * @test_data       regular data for discoverMultipleOwnedDevices
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call addPreconfigPIN
 *                  4. call doMultipleOwnershipTransfer
 *                  5. call discoverMultipleOwnedDevices
 * @post_condition  None
 * @expected        discoverMultipleOwnedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, DiscoverMultipleOwnedDevices_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.addPreconfigPIN(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.doMultipleOwnershipTransfer(m_motEnabledDevList,PMCppMotHelper::multipleOwnershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT,m_motOwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @see             OCStackResult addPreconfigPIN(const char* preconfPIN,size_t preconfPINLength)
 * @see             OCStackResult doMultipleOwnershipTransfer(ResultCallBack resultCallback)
 * @objective       Test discoverMultipleOwnedDevices positively with timeout=DISCOVERY_TIMEOUT_ONE
 * @target          static OCStackResult discoverMultipleOwnedDevices(unsigned short timeout,DeviceList_t &list)
 * @test_data       timeout=DISCOVERY_TIMEOUT_ONE
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call addPreconfigPIN
 *                  4. call doMultipleOwnershipTransfer
 *                  5. call discoverMultipleOwnedDevices
 * @post_condition  None
 * @expected        discoverMultipleOwnedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, DiscoverMultipleOwnedDevices_LBV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.addPreconfigPIN(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.doMultipleOwnershipTransfer(m_motEnabledDevList,PMCppMotHelper::multipleOwnershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT_ONE,m_motOwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-02-24
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @see             OCStackResult addPreconfigPIN(const char* preconfPIN,size_t preconfPINLength)
 * @see             OCStackResult doMultipleOwnershipTransfer(ResultCallBack resultCallback)
 * @objective       Test discoverMultipleOwnedDevices negatively with timeout=DISCOVERY_TIMEOUT_ZERO
 * @target          static OCStackResult discoverMultipleOwnedDevices(unsigned short timeout,DeviceList_t &list)
 * @test_data       timeout=DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call addPreconfigPIN
 *                  4. call doMultipleOwnershipTransfer
 *                  5. call discoverMultipleOwnedDevices
 * @post_condition  None
 * @expected        discoverMultipleOwnedDevices will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, DiscoverMultipleOwnedDevices_LOBV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.addPreconfigPIN(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.doMultipleOwnershipTransfer(m_motEnabledDevList,PMCppMotHelper::multipleOwnershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT_ZERO,m_motOwnedDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-02
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test getMOTMethod positively with regular data
 * @target          OCStackResult getMOTMethod(OicSecOxm_t* oxm)
 * @test_data       regular data
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call getMOTMethod
 * @post_condition  None
 * @expected        getMOTMethod will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, GetMOTMethod_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    OicSecOxm_t oxm = NULL;
    if (!m_PMCppMotHelper.getMOTMethod(m_motEnabledDevList, &oxm, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-02
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test getMOTMethod negatively with oxm=NULL
 * @target          OCStackResult getMOTMethod(OicSecOxm_t* oxm)
 * @test_data       oxm=NULL
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call getMOTMethod
 * @post_condition  None
 * @expected        getMOTMethod will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, GetMOTMethodOxm_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    OicSecOxm_t oxm = NULL;
    if (!m_PMCppMotHelper.getMOTMethod(m_motEnabledDevList, NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-02
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test isMOTSupported positively with regular data
 * @target          bool isMOTSupported()
 * @test_data       regular data
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call isMOTSupported
 * @post_condition  None
 * @expected        isMOTSupported will return True
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, IsMOTSupported_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.isMOTSupported(m_motEnabledDevList,true))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-02
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       Test isMOTSupported without enable MOT mode
 * @target          bool isMOTSupported()
 * @test_data       without enable MOT mode
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call isMOTSupported
 * @post_condition  None
 * @expected        isMOTSupported will return False
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, IsMOTSupported_NV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.isMOTSupported(m_OwnedDevList,false))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-02
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test isMOTEnabled positively with regular data
 * @target          bool isMOTEnabled()
 * @test_data       regular data
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call isMOTEnabled
 * @post_condition  None
 * @expected        isMOTEnabled will return True
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, IsMOTEnabled_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.isMOTEnabled(m_motEnabledDevList,true))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-02
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       Test isMOTEnabled without enable MOT mode
 * @target          bool isMOTEnabled()
 * @test_data       without enable MOT mode
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call isMOTEnabled
 * @post_condition  None
 * @expected        isMOTEnabled will return False
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, IsMOTEnabled_NV_N)
{
    CommonUtil::launchApp(PRECONFIG_SERVER1);
    CommonUtil::waitInSecond(DELAY_LONG);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
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

    if (!m_PMCppMotHelper.isMOTEnabled(m_OwnedDevList,false))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-03
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @see             OCStackResult addPreconfigPIN(const char* preconfPIN,size_t preconfPINLength)
 * @see             OCStackResult doMultipleOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverMultipleOwnedDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test isSubownerOfDevice positively with regular data
 * @target          OCStackResult isSubownerOfDevice(bool* subowner)
 * @test_data       regular data
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call addPreconfigPIN
 *                  4. call doMultipleOwnershipTransfer
 *                  5. call discoverMultipleOwnedDevices
 *                  6. call isSubownerOfDevice
 * @post_condition  None
 * @expected        isSubownerOfDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, IsSubownerOfDevice_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.addPreconfigPIN(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.doMultipleOwnershipTransfer(m_motEnabledDevList,PMCppMotHelper::multipleOwnershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT,m_motOwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.isSubownerOfDevice(m_motOwnedDevList,&subowner,OC_STACK_OK,true))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-03
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test isSubownerOfDevice without multipleOwnerShip transfer
 * @target          OCStackResult isSubownerOfDevice(bool* subowner)
 * @test_data       without multipleOwnerShip transfer
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call isSubownerOfDevice
 * @post_condition  None
 * @expected        isSubownerOfDevice will return OC_STACK_OK and subowner return false
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, IsSubownerOfDeviceWithoutOwnership_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.isSubownerOfDevice(m_motEnabledDevList,&subowner,OC_STACK_OK,false))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-03
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test isSubownerOfDevice negatively with subowner=NULL
 * @target          OCStackResult isSubownerOfDevice(bool* subowner)
 * @test_data       subowner=NULL
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call isSubownerOfDevice
 * @post_condition  None
 * @expected        isSubownerOfDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, IsSubownerOfDeviceSubowner_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.isSubownerOfDevice(m_motEnabledDevList,NULL,OC_STACK_INVALID_PARAM,false))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-03
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverMultipleOwnerEnabledDevices(unsigned short timeout,DeviceList_t &list)
 * @see             OCStackResult addPreconfigPIN(const char* preconfPIN,size_t preconfPINLength)
 * @see             OCStackResult doMultipleOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverMultipleOwnedDevices(unsigned short timeout,DeviceList_t &list)
 * @objective       Test provisionACL positively with regular data
 * @target          OCStackResult provisionACL(const OicSecAcl_t* acl,ResultCallBack resultCallback)
 * @test_data       regular data for provisionACL
 * @pre_condition   Start One PreConfig simulator
 * @procedure       1. call provisionInit
 *                  2. call discoverMultipleOwnerEnabledDevices
 *                  3. call addPreconfigPIN
 *                  4. call doMultipleOwnershipTransfer
 *                  5. call discoverMultipleOwnedDevices
 *                  6. call provisionACL
 * @post_condition  None
 * @expected        provisionACL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppMotTest_btc, ProvisionACLForMultipleOwnedDevices_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER7);
    CommonUtil::waitInSecond(DELAY_LONG);

    if (!m_PMCppMotHelper.initMotClient())
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
                    m_motEnabledDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.addPreconfigPIN(m_motEnabledDevList, MOT_DEFAULT_PRE_CONFIG_PIN, OXM_PRECONFIG_PIN_MAX_SIZE, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.doMultipleOwnershipTransfer(m_motEnabledDevList,PMCppMotHelper::multipleOwnershipTransferCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    if (!m_PMCppMotHelper.discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT,m_motOwnedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppMotHelper.getFailureMessage());
        return;
    }

    m_acl1 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl1, DEVICE_INDEX_ONE, FULL_PERMISSION, m_motOwnedDevList);

    if(!m_PMCppHelper.provisionACL(m_motOwnedDevList, m_acl1, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    m_PMCppHelper.deleteACLList(m_acl1);
}
#endif

#endif //end of __MOT__
