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
#include "CommonTestUtil.h"

class PMCppPairwiseTest_btc: public ::testing::Test
{
protected:
    PMCppHelper m_PMCppHelper;
    DeviceList_t m_OwnedDevList;
    OicSecAcl_t *m_acl1, *m_acl2;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        PMCppUtilityHelper::removeAllResFile();
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O1_PAIRWISE_BACKUP, JUSTWORKS_SERVER_CBOR_O1);
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O2_PAIRWISE_BACKUP, JUSTWORKS_SERVER_CBOR_O2);
        CommonUtil::copyFile(CLIENT_CBOR_01_PAIRWISE_BACKUP, CLIENT_CBOR_01);
        CommonUtil::copyFile(DEVICE_PROP_CBOR_01_PAIRWISE_BACKUP, DEVICE_PROP_CBOR_01);
        CommonUtil::copyFile(CLIENT_DB_01_PAIRWISE_BACKUP, CLIENT_DB_01);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::launchApp(JUSTWORKS_SERVER1);
        CommonUtil::launchApp(JUSTWORKS_SERVER2);
        CommonUtil::waitInSecond(DELAY_MEDIUM);

        m_OwnedDevList.clear();

        if (!m_PMCppHelper.provisionInit())
        {
            SET_FAILURE(m_PMCppHelper.getFailureMessage());
            return;
        }

        if (!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, m_OwnedDevList, OC_STACK_OK))
        {
            SET_FAILURE(m_PMCppHelper.getFailureMessage());
            return;
        }
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);
    }
};

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test getLinkedDevices positively
 * @target          OCStackResult getLinkedDevices(UuidList_t &uuidList)
 * @test_data       Regular data for getLinkedDevices
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call getLinkedDevices
 * @post_condition  None
 * @expected        getLinkedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppPairwiseTest_btc, GetLinkedDevices_FSV_P)
{
    UuidList_t uuidList1;

    if(!m_PMCppHelper.getLinkedDevices(m_OwnedDevList, uuidList1, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test unlinkDevices positively
 * @target          OCStackResult unlinkDevices(const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       Regular data for unlinkDevices
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call unlinkDevices
 * @post_condition  None
 * @expected        unlinkDevices will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppPairwiseTest_btc, UnlinkDevices_RV_P)
{
    if(!m_PMCppHelper.unlinkDevices(m_OwnedDevList, *m_OwnedDevList[1].get(), PMCppCallbackHelper::provisionDeleteCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test unlinkDevices negatively with resultCallback as NULL
 * @target          OCStackResult unlinkDevices(const OCSecureResource &device2, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call unlinkDevices
 * @post_condition  None
 * @expected        unlinkDevices will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppPairwiseTest_btc, UnlinkDevicesCB_NV_N)
{
    if(!m_PMCppHelper.unlinkDevices(m_OwnedDevList, *m_OwnedDevList[1].get(), NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDevice positively
 * @target          OCStackResult removeDevice(unsigned short waitTimeForOwnedDeviceDiscovery, ResultCallBack resultCallback)
 * @test_data       regular data
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call removeDevice
 * @post_condition  None
 * @expected        removeDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppPairwiseTest_btc, RemoveDeviceTime_RV_SRC_P)
{
    if(!m_PMCppHelper.removeDevice(m_OwnedDevList, DISCOVERY_TIMEOUT, PMCppCallbackHelper::provisionDeleteCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDevice positively with waitTimeForOwnedDeviceDiscovery as DISCOVERY_TIMEOUT_ONE
 * @target          OCStackResult removeDevice(unsigned short waitTimeForOwnedDeviceDiscovery, ResultCallBack resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call removeDevice
 * @post_condition  None
 * @expected        removeDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppPairwiseTest_btc, RemoveDeviceTime_LBV_P)
{
    if(!m_PMCppHelper.removeDevice(m_OwnedDevList, DISCOVERY_TIMEOUT_ONE, PMCppCallbackHelper::provisionDeleteCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDevice negatively with waitTimeForOwnedDeviceDiscovery as DISCOVERY_TIMEOUT_ZERO
 * @target          OCStackResult removeDevice(unsigned short waitTimeForOwnedDeviceDiscovery, ResultCallBack resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call removeDevice
 * @post_condition  None
 * @expected        removeDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppPairwiseTest_btc, RemoveDeviceTime_LOBV_N)
{
    if(!m_PMCppHelper.removeDevice(m_OwnedDevList, DISCOVERY_TIMEOUT_ZERO, PMCppCallbackHelper::provisionDeleteCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDevice negatively with resultCallback as NULL
 * @target          OCStackResult removeDevice(unsigned short waitTimeForOwnedDeviceDiscovery, ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call removeDevice
 * @post_condition  None
 * @expected        removeDevice will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppPairwiseTest_btc, RemoveDeviceCB_NV_N)
{
    if(!m_PMCppHelper.removeDevice(m_OwnedDevList, DISCOVERY_TIMEOUT, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-06
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDeviceWithUuid positively
 * @target          static OCStackResult removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,std::string uuid, ResultCallBack resultCallback)
 * @test_data       regular data
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call removeDeviceWithUuid
 * @post_condition  None
 * @expected        removeDeviceWithUuid will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppPairwiseTest_btc, RemoveDeviceTimeWithUuid_RV_SRC_P)
{
    if(!m_PMCppHelper.removeDeviceWithUuid(DISCOVERY_TIMEOUT, m_OwnedDevList[0]->getDeviceID(), PMCppCallbackHelper::provisionDeleteCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-06
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDeviceWithUuid positively with waitTimeForOwnedDeviceDiscovery as DISCOVERY_TIMEOUT_ONE
 * @target          static OCStackResult removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,std::string uuid, ResultCallBack resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ONE
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call removeDeviceWithUuid
 * @post_condition  None
 * @expected        removeDeviceWithUuid will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppPairwiseTest_btc, RemoveDeviceTimeWithUuid_LBV_P)
{
    if(!m_PMCppHelper.removeDeviceWithUuid(DISCOVERY_TIMEOUT_ONE, m_OwnedDevList[0]->getDeviceID(), PMCppCallbackHelper::provisionDeleteCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-01
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDeviceWithUuid negatively with waitTimeForOwnedDeviceDiscovery as DISCOVERY_TIMEOUT_ZERO
 * @target          static OCStackResult removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,std::string uuid, ResultCallBack resultCallback)
 * @test_data       waitTimeForOwnedDeviceDiscovery = DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call removeDeviceWithUuid
 * @post_condition  None
 * @expected        removeDeviceWithUuid will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppPairwiseTest_btc, RemoveDeviceTimeWithUuid_LOBV_N)
{
    if(!m_PMCppHelper.removeDeviceWithUuid(DISCOVERY_TIMEOUT_ZERO, m_OwnedDevList[0]->getDeviceID(), PMCppCallbackHelper::provisionDeleteCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-01
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDeviceWithUuid negatively with uuid as NULL
 * @target          static OCStackResult removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,std::string uuid, ResultCallBack resultCallback)
 * @test_data       uuid = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call removeDeviceWithUuid
 * @post_condition  None
 * @expected        removeDeviceWithUuid will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppPairwiseTest_btc, RemoveDeviceTimeWithUuid_NV_N)
{
    if(!m_PMCppHelper.removeDeviceWithUuid(DISCOVERY_TIMEOUT,EMPTY_STRING,PMCppCallbackHelper::provisionDeleteCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-02-01
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult provisionPairwiseDevices(const Credential &cred, const OicSecAcl_t* m_acl1, const OCSecureResource &device2, const OicSecAcl_t* m_acl2, ResultCallBack resultCallback)
 * @objective       test removeDeviceWithUuid negatively with Callback as NULL
 * @target          static OCStackResult removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,std::string uuid, ResultCallBack resultCallback)
 * @test_data       Callback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor file from 'pairwise' directory
 * @procedure       1. call provisionInit
 *                  2. call discoverOwnedDevices
 *                  3. call removeDeviceWithUuid
 * @post_condition  None
 * @expected        removeDeviceWithUuid will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCppPairwiseTest_btc, RemoveDeviceTimeWithUuidCB_NV_N)
{
    if(!m_PMCppHelper.removeDeviceWithUuid(DISCOVERY_TIMEOUT, m_OwnedDevList[0]->getDeviceID(), NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif
