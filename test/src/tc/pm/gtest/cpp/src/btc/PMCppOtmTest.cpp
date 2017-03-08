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

class PMCppOtmTest_btc: public ::testing::Test
{
protected:
    PMCppHelper m_PMCppHelper;
    DeviceList_t m_UnownedDevList, m_OwnedDevList;
    std::shared_ptr< OCSecureResource > m_TargetList;
    OicUuid_t deviceUuid;
    OicSecAcl_t *m_acl1, *m_acl2;
    InputPinCallbackHandle callbackHandle;
    DisplayPinCallbackHandle displayPinCallbackHandle;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        PMCppUtilityHelper::removeAllResFile();
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::rmFile(DATABASE_PDM);
        CommonUtil::rmFile(JUSTWORKS_SERVER1_CBOR);
        CommonUtil::rmFile(JUSTWORKS_SERVER2_CBOR);
        CommonUtil::rmFile(MV_JUSTWORKS_SERVER_CBOR);
        CommonUtil::rmFile(CLIENT_CBOR);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::copyFile(JUSTWORKS_SERVER1_CBOR_BACKUP, JUSTWORKS_SERVER1_CBOR);
        CommonUtil::copyFile(JUSTWORKS_SERVER2_CBOR_BACKUP, JUSTWORKS_SERVER2_CBOR);
        CommonUtil::copyFile(MV_JUSTWORKS_SERVER_CBOR_BACKUP, MV_JUSTWORKS_SERVER_CBOR);
        CommonUtil::copyFile(RANDOMPIN_SERVER_CBOR_BACKUP, RANDOMPIN_SERVER_CBOR);
        CommonUtil::copyFile(CLIENT_CBOR_BACKUP, CLIENT_CBOR);
        m_UnownedDevList.clear();
        m_OwnedDevList.clear();
        m_acl1 = NULL;
        m_acl2 = NULL;
        m_TargetList = NULL;

        callbackHandle = nullptr;
        displayPinCallbackHandle = nullptr;
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverUnownedDevices_RV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverUnownedDevicesTime_LBV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverUnownedDevicesTime_LOBV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverOwnedDevices_RV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverOwnedDevicesTime_LBV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverOwnedDevicesTime_LOBV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT_ZERO, m_OwnedDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-31
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test discoverUnownedDevices positively
 * @target          static OCStackResult discoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID, std::shared_ptr<OCSecureResource> foundDevice)
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call discoverSingleDevice
 * @post_condition  None
 * @expected        discoverSingleDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverSingleDevice_RV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

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

    if(!m_PMCppHelper.convertStrToUuid(m_UnownedDevList[0]->getDeviceID(), &deviceUuid,OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverSingleDevice(DISCOVERY_TIMEOUT,&deviceUuid,m_TargetList,OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-31
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test discoverUnownedDevices positively with timeout=DISCOVERY_TIMEOUT_ONE
 * @target          static OCStackResult discoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID, std::shared_ptr<OCSecureResource> foundDevice)
 * @test_data       timeout=DISCOVERY_TIMEOUT_ONE
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call discoverSingleDevice
 * @post_condition  None
 * @expected        discoverSingleDevice will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverSingleDevice_LBV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

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

    if(!m_PMCppHelper.convertStrToUuid(m_UnownedDevList[0]->getDeviceID(), &deviceUuid,OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverSingleDevice(DISCOVERY_TIMEOUT_ONE,&deviceUuid,m_TargetList,OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-31
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test discoverUnownedDevices negatively with timeout=DISCOVERY_TIMEOUT_ZERO
 * @target          static OCStackResult discoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID, std::shared_ptr<OCSecureResource> foundDevice)
 * @test_data       timeout=DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call discoverSingleDevice
 * @post_condition  None
 * @expected        discoverSingleDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverSingleDevice_LOBV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

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

    if(!m_PMCppHelper.convertStrToUuid(m_UnownedDevList[0]->getDeviceID(), &deviceUuid,OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverSingleDevice(DISCOVERY_TIMEOUT_ZERO,&deviceUuid,m_TargetList,OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-01-31
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test discoverUnownedDevices negatively with deviceID=NULL
 * @target          static OCStackResult discoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID, std::shared_ptr<OCSecureResource> foundDevice)
 * @test_data       deviceID=NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call discoverSingleDevice
 * @post_condition  None
 * @expected        discoverSingleDevice will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverSingleDeviceDeviceID_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

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

    if(!m_PMCppHelper.discoverSingleDevice(DISCOVERY_TIMEOUT,NULL,m_TargetList,OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-06
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test discoverSingleDeviceInUnicast positively
 * @target          static OCStackResult discoverSingleDeviceInUnicast(unsigned short timeout,const OicUuid_t* deviceID,const std::string& hostAddress,OCConnectivityType connType,std::shared_ptr<OCSecureResource> &foundDevice)
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call discoverSingleDeviceInUnicast
 * @post_condition  None
 * @expected        discoverSingleDeviceInUnicast will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverSingleDeviceInUnicast_RV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.convertStrToUuid(m_UnownedDevList[0]->getDeviceID(), &deviceUuid,OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    std::string hostAddr = m_UnownedDevList[0]->getDevPtr()->endpoint.routeData;

    if(!m_PMCppHelper.discoverSingleDeviceInUnicast(DISCOVERY_TIMEOUT,&deviceUuid,hostAddr,CT_DEFAULT,m_TargetList,OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-06
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test discoverSingleDeviceInUnicast positively with timeout=DISCOVERY_TIMEOUT_ONE
 * @target          static OCStackResult discoverSingleDeviceInUnicast(unsigned short timeout,const OicUuid_t* deviceID,const std::string& hostAddress,OCConnectivityType connType,std::shared_ptr<OCSecureResource> &foundDevice)
 * @test_data       timeout=DISCOVERY_TIMEOUT_ONE
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call discoverSingleDeviceInUnicast
 * @post_condition  None
 * @expected        discoverSingleDeviceInUnicast will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverSingleDeviceInUnicast_LBV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.convertStrToUuid(m_UnownedDevList[0]->getDeviceID(), &deviceUuid,OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    std::string hostAddr = m_UnownedDevList[0]->getDevPtr()->endpoint.routeData;

    if(!m_PMCppHelper.discoverSingleDeviceInUnicast(DISCOVERY_TIMEOUT_ONE,&deviceUuid,hostAddr,CT_DEFAULT,m_TargetList,OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-06
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test discoverSingleDeviceInUnicast negatively with timeout=DISCOVERY_TIMEOUT_ZERO
 * @target          static OCStackResult discoverSingleDeviceInUnicast(unsigned short timeout,const OicUuid_t* deviceID,const std::string& hostAddress,OCConnectivityType connType,std::shared_ptr<OCSecureResource> &foundDevice)
 * @test_data       timeout=DISCOVERY_TIMEOUT_ZERO
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call discoverSingleDeviceInUnicast
 * @post_condition  None
 * @expected        discoverSingleDeviceInUnicast will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverSingleDeviceInUnicast_LOBV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.convertStrToUuid(m_UnownedDevList[0]->getDeviceID(), &deviceUuid,OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    std::string hostAddr = m_UnownedDevList[0]->getDevPtr()->endpoint.routeData;

    if(!m_PMCppHelper.discoverSingleDeviceInUnicast(DISCOVERY_TIMEOUT_ZERO,&deviceUuid,hostAddr,CT_DEFAULT,m_TargetList,OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-06
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test discoverSingleDeviceInUnicast negatively with deviceID=NULL
 * @target          static OCStackResult discoverSingleDeviceInUnicast(unsigned short timeout,const OicUuid_t* deviceID,const std::string& hostAddress,OCConnectivityType connType,std::shared_ptr<OCSecureResource> &foundDevice)
 * @test_data       deviceID=NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call discoverSingleDeviceInUnicast
 * @post_condition  None
 * @expected        discoverSingleDeviceInUnicast will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DiscoverSingleDeviceInUnicastDeviceID_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT, m_UnownedDevList, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    std::string hostAddr = m_UnownedDevList[0]->getDevPtr()->endpoint.routeData;

    if(!m_PMCppHelper.discoverSingleDeviceInUnicast(DISCOVERY_TIMEOUT,NULL,hostAddr,CT_DEFAULT,m_TargetList,OC_STACK_INVALID_PARAM))
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, GetDevInfoFromNetwork_RV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, GetDevInfoFromNetworkTime_LBV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, GetDevInfoFromNetworkTime_LOBV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.getDevInfoFromNetwork(DISCOVERY_TIMEOUT_ONE, m_OwnedDevList, m_UnownedDevList, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

#ifdef __1_2_1__
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, SetOwnerTransferCBDataJustWork_RV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, SetOwnerTransferCBDataJustWork_REV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, SetOwnerTransferCBDataJustWorkInvalidCB_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, SetOwnerTransferCBDataJustWorkInvalidParam_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.setOwnerTransferCallbackData(OIC_JUST_WORKS, NULL, NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, SetOwnerTransferCBDataJustWorkOxm_UOBV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
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

    if(!m_PMCppHelper.setOwnerTransferCallbackData((OicSecOxm_t) OTM_INVALID_UOBV, justWorksCBData, NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, SetOwnerTransferCBDataRanDomPin_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t pinBasedCBData;
    pinBasedCBData.loadSecretCB = InputPinCodeCallback;
    pinBasedCBData.createSecureSessionCB =
    CreateSecureSessionRandomPinCallback;
    pinBasedCBData.createSelectOxmPayloadCB =
    CreatePinBasedSelectOxmPayload;
    pinBasedCBData.createOwnerTransferPayloadCB =
    CreatePinBasedOwnerTransferPayload;
    OCSecure::setOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN,
            &pinBasedCBData, PMCppHelper::InputPinCB);

    OCStackResult expetedResult = OC_STACK_OK;
    OCStackResult actualResult = OCSecure::setOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN,
            &pinBasedCBData, PMCppHelper::InputPinCB);

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, SetOwnerTransferCBDataRanDomPinCB_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OTMCallbackData_t pinBasedCBData;
    pinBasedCBData.loadSecretCB = InputPinCodeCallback;
    pinBasedCBData.createSecureSessionCB =
    CreateSecureSessionRandomPinCallback;
    pinBasedCBData.createSelectOxmPayloadCB =
    CreatePinBasedSelectOxmPayload;
    pinBasedCBData.createOwnerTransferPayloadCB =
    CreatePinBasedOwnerTransferPayload;
    OCSecure::setOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN,
            &pinBasedCBData, PMCppHelper::InputPinCB);

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, SetOwnerTransferCBDataRanDomPinData_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    OCStackResult expetedResult = OC_STACK_INVALID_PARAM;
    OCStackResult actualResult = OCSecure::setOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN,
            NULL, PMCppHelper::InputPinCB);

    ASSERT_EQ(expetedResult, actualResult);
}
#endif

#endif //End of __1.2.1__

/**
 * @since           2017-03-06
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test doOwnershipTransfer positively for OIC_JUST_WORKS
 * @target          OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 * @post_condition  None
 * @expected        doOwnershipTransfer will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DoOwnershipTransferJustWork_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

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
    }
}
#endif

/**
 * @since           2017-03-06
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult registerDisplayPinCallback(DisplayPinCB displayPinCB, DisplayPinCallbackHandle* displayPinCallbackHandle)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test doOwnershipTransfer positively for OIC_RANDOM_DEVICE_PIN
 * @target          OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @test_data       regular data
 * @pre_condition   start one randompin serer simulators
 * @procedure       1. call provisionInit
 * 					2. call registerDisplayPinCallback
 *                  3. call discoverUnownedDevices
 *                  4. call doOwnershipTransfer
 *                  5. call deregisterDisplayPinCallback
 * @post_condition  None
 * @expected        doOwnershipTransfer will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DoOwnershipTransferRandomPin_RV_SRC_P)
{
    CommonUtil::launchApp(RANDOMPIN_SERVER);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerInputPinCallback(PMCppHelper::OnInputPinCB, &callbackHandle, OC_STACK_OK))
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

    if(!m_PMCppHelper.deregisterInputPinCallback(callbackHandle, OC_STACK_OK))
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
 * @objective       test doOwnershipTransfer negatively for OIC_JUST_WORKS
 * @target          OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 * @post_condition  None
 * @expected        doOwnershipTransfer will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, DoOwnershipTransferJustWorkCB_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

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

    if(!m_PMCppHelper.doOwnershipTransfer(m_UnownedDevList, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-03-09
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test getOTMethod positively
 * @target          OCStackResult getOTMethod(OicSecOxm_t* oxm)
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call getOTMethod
 * @post_condition  None
 * @expected        getOTMethod will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, getOTMethod_RV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

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

    OicSecOxm_t oxm = NULL;
    if (!m_PMCppHelper.getOTMethod(m_UnownedDevList, &oxm, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-09
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test getOTMethod negatively with oxm=NULL
 * @target          OCStackResult getOTMethod(OicSecOxm_t* oxm)
 * @test_data       oxm=NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call getOTMethod
 * @post_condition  None
 * @expected        getOTMethod will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, getOTMethod_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

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

    OicSecOxm_t oxm = NULL;
    if (!m_PMCppHelper.getOTMethod(m_UnownedDevList, NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-09
 * @see             OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test getSelectedOwnershipTransferMethod positively
 * @target          OicSecOxm_t getSelectedOwnershipTransferMethod()
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call getSelectedOwnershipTransferMethod
 * @post_condition  None
 * @expected        getSelectedOwnershipTransferMethod will return selected ownership transfer method of the device
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, getSelectedOwnershipTransferMethod_RV_P)
{
    CommonUtil::launchApp(RANDOMPIN_SERVER);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

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

    if (!m_PMCppHelper.getSelectedOwnershipTransferMethod(m_UnownedDevList,OIC_RANDOM_DEVICE_PIN))
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, GetDeviceID_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, GetDevAddr_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

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
 * @expected        getDeviceStatus will return ON
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, GetDeviceStatusOn_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, GetOwnedStatusUnowned_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

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
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @see             OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @see             static OCStackResult discoverOwnedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test getOwnedStatus positively to Check if Owned devices return Owned Status as DEVICE_OWNED
 * @target          bool getOwnedStatus()
 * @test_data       Outer Upper Boundary Value of keySize
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call discoverUnownedDevices
 *                  3. call doOwnershipTransfer
 *                  4. call discoverOwnedDevices
 *                  5. call getOwnedStatus
 *                  6. call deleteACLList for m_acl1
 * @post_condition  None
 * @expected        getOwnedStatus will return DEVICE_OWNED
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, GetOwnedStatusOwned_SRCC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,m_UnownedDevList, OC_STACK_OK))
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

    m_acl1 = (OicSecAcl_t *)OICCalloc(1,sizeof(OicSecAcl_t));
    PMCppHelper::createAcl(m_acl1, DEVICE_INDEX_ONE, FULL_PERMISSION, m_OwnedDevList);

    if(!m_PMCppHelper.provisionACL(m_OwnedDevList, m_acl1, PMCppHelper::provisionCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if (DEVICE_OWNED != m_OwnedDevList[0]->getOwnedStatus())
    {
        SET_FAILURE("getOwnedStatus Failed");
    }

    m_PMCppHelper.deleteACLList(m_acl1);
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
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, GetDevPtr_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

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
 * @since           2017-03-16
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setRandomPinPolicy negatively with pinSize = OXM_RANDOM_PIN_MIN_SIZE - 1
 * @target          static OCStackResult setRandomPinPolicy(size_t pinSize, OicSecPinType_t pinType)
 * @test_data       pinSize = OXM_RANDOM_PIN_MIN_SIZE - 1
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call setRandomPinPolicy
 * @post_condition  None
 * @expected        setRandomPinPolicy will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, setRandomPinPolicy_LOBV_N)
{
    CommonUtil::launchApp(RANDOMPIN_SERVER);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.setRandomPinPolicy(OXM_RANDOM_PIN_MIN_SIZE - 1, NUM_PIN, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-16
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setRandomPinPolicy negatively with pinSize = OXM_RANDOM_PIN_MAX_SIZE + 1
 * @target          static OCStackResult setRandomPinPolicy(size_t pinSize, OicSecPinType_t pinType)
 * @test_data       pinSize = OXM_RANDOM_PIN_MAX_SIZE + 1
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call setRandomPinPolicy
 * @post_condition  None
 * @expected        setRandomPinPolicy will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, setRandomPinPolicy_UOBV_N)
{
    CommonUtil::launchApp(RANDOMPIN_SERVER);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.setRandomPinPolicy(OXM_RANDOM_PIN_MAX_SIZE + 1, NUM_PIN, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-16
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setRandomPinPolicy negatively with pinType = INVALID_PIN_TYPE
 * @target          static OCStackResult setRandomPinPolicy(size_t pinSize, OicSecPinType_t pinType)
 * @test_data       pinType = INVALID_PIN_TYPE
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call setRandomPinPolicy
 * @post_condition  None
 * @expected        setRandomPinPolicy will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, setRandomPinPolicy_NV_N)
{
    CommonUtil::launchApp(RANDOMPIN_SERVER);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.setRandomPinPolicy(OXM_RANDOM_PIN_DEFAULT_SIZE, (OicSecPinType_t) INVALID_PIN_TYPE, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setDisplayPinCB positively
 * @target          static OCStackResult setDisplayPinCB(GeneratePinCallback displayPin)
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call setDisplayPinCB
 * @post_condition  None
 * @expected        setDisplayPinCB will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, SetDisplayPinCB_SRC_P)
{
    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.setDisplayPinCB(PMCppHelper::inputPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
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
 *                  2. call setDisplayPinCB
 * @post_condition  None
 * @expected        setDisplayPinCB will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, SetDisplayPinCB_N)
{
    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.setDisplayPinCB(NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2015-11-30
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setDisplayPinCB(GeneratePinCallback displayPin)
 * @objective       test unsetDisplayPinCB positively
 * @target          static OCStackResult unsetDisplayPinCB()
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call setDisplayPinCB
 *                  3. call unsetDisplayPinCB
 * @post_condition  None
 * @expected        unsetDisplayPinCB will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, unsetDisplayPinCB_SRC_P)
{
    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.setDisplayPinCB(PMCppHelper::inputPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.unsetDisplayPinCB(OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setInputPinCallback positively
 * @target          static OCStackResult setInputPinCallback(InputPinCallback inputPin)
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call setInputPinCallback
 * @post_condition  None
 * @expected        setInputPinCallback will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, setInputPinCallback_SRC_P)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.setInputPinCallback(PMCppHelper::inputPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test setInputPinCallback negatively with inputPin=NULL
 * @target          static OCStackResult setInputPinCallback(InputPinCallback inputPin)
 * @test_data       inputPin=NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call setInputPinCallback
 * @post_condition  None
 * @expected        setInputPinCallback will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, setInputPinCallbackInputPIn_NV_N)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.setInputPinCallback(NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult setInputPinCallback(InputPinCallback inputPin)
 * @objective       test unsetInputPinCallback positively
 * @target          static OCStackResult unsetInputPinCallback()
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call setInputPinCallback
 *                  3. call unsetInputPinCallback
 * @post_condition  None
 * @expected        unsetInputPinCallback will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, unsetInputPinCallback_SRC_P)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.setInputPinCallback(PMCppHelper::inputPinCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.unsetInputPinCallback(OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerInputPinCallback positively
 * @target          static OCStackResult registerInputPinCallback(InputPinCB inputPinCB, InputPinCallbackHandle* inputPinCallbackHandle);
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerInputPinCallback
 * @post_condition  None
 * @expected        registerInputPinCallback will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, registerInputPinCallback_SRC_P)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerInputPinCallback(PMCppHelper::OnInputPinCB, &callbackHandle, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerInputPinCallback negatively with inputPinCB = NULL
 * @target          static OCStackResult registerInputPinCallback(InputPinCB inputPinCB, InputPinCallbackHandle* inputPinCallbackHandle);
 * @test_data       inputPinCB = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerInputPinCallback
 * @post_condition  None
 * @expected        registerInputPinCallback will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, registerInputPinCallback_NV_N)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerInputPinCallback(NULL, &callbackHandle, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerInputPinCallback negatively with inputPinCallbackHandle = NULL
 * @target          static OCStackResult registerInputPinCallback(InputPinCB inputPinCB, InputPinCallbackHandle* inputPinCallbackHandle);
 * @test_data       inputPinCallbackHandle = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerInputPinCallback
 * @post_condition  None
 * @expected        registerInputPinCallback will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, registerInputPinCallbackInputPinCallbackHandle_NV_N)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerInputPinCallback(PMCppHelper::OnInputPinCB, NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult registerInputPinCallback(InputPinCB inputPinCB, InputPinCallbackHandle* inputPinCallbackHandle)
 * @objective       test deregisterInputPinCallback positively
 * @target          static OCStackResult deregisterInputPinCallback(InputPinCallbackHandle inputPinCallbackHandle)
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerInputPinCallback
 *                  3. call deregisterInputPinCallback
 * @post_condition  None
 * @expected        deregisterInputPinCallback will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, deregisterInputPinCallback_SRC_P)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerInputPinCallback(PMCppHelper::OnInputPinCB, &callbackHandle, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.deregisterInputPinCallback(callbackHandle, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-16
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerInputPinCallback positively
 * @target          static OCStackResult registerDisplayPinCallback(DisplayPinCB displayPinCB, DisplayPinCallbackHandle* displayPinCallbackHandle);
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerDisplayPinCallback
 * @post_condition  None
 * @expected        registerDisplayPinCallback will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, registerDisplayPinCallback_SRC_P)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerDisplayPinCallback(PMCppHelper::OnDisplayPinCB, &displayPinCallbackHandle, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-16
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerInputPinCallback Negatively
 * @target          static OCStackResult registerDisplayPinCallback(DisplayPinCB displayPinCB, DisplayPinCallbackHandle* displayPinCallbackHandle);
 * @test_data       displayPinCB = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerDisplayPinCallback
 * @post_condition  None
 * @expected        registerDisplayPinCallback will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, registerDisplayPinCallbackCB_NV_N)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerDisplayPinCallback(NULL, &displayPinCallbackHandle, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-16
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerInputPinCallback Negatively
 * @target          static OCStackResult registerDisplayPinCallback(DisplayPinCB displayPinCB, DisplayPinCallbackHandle* displayPinCallbackHandle);
 * @test_data       displayPinCallbackHandle = NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerDisplayPinCallback
 * @post_condition  None
 * @expected        registerDisplayPinCallback will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, registerDisplayPinCallbackDisplayPinCallbackHandle_NV_N)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerDisplayPinCallback(PMCppHelper::OnDisplayPinCB, NULL, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-16
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult registerDisplayPinCallback(DisplayPinCB displayPinCB, DisplayPinCallbackHandle* displayPinCallbackHandle)
 * @objective       test deregisterDisplayPinCallback positively
 * @target          static OCStackResult deregisterDisplayPinCallback(DisplayPinCallbackHandle displayPinCallbackHandle);
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerDisplayPinCallback
 *                  3. call deregisterDisplayPinCallback
 * @post_condition  None
 * @expected        deregisterDisplayPinCallback will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, deregisterDisplayPinCallback_SRC_P)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerDisplayPinCallback(PMCppHelper::OnDisplayPinCB, &displayPinCallbackHandle, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.deregisterDisplayPinCallback(displayPinCallbackHandle, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerDisplayNumCallback positively
 * @target          static OCStackResult registerDisplayNumCallback(DisplayNumCB displayNumCB)
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerDisplayNumCallback
 * @post_condition  None
 * @expected        registerDisplayNumCallback will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, registerDisplayNumCallback_SRC_P)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerDisplayNumCallback(PMCppHelper::displayMutualVerifNumCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerDisplayNumCallback negatively with displayNumCB=NULL
 * @target          static OCStackResult registerDisplayNumCallback(DisplayNumCB displayNumCB)
 * @test_data       displayNumCB=NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerDisplayNumCallback
 * @post_condition  None
 * @expected        registerDisplayNumCallback will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, registerDisplayNumCallback_NV_N)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerDisplayNumCallback(NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult registerDisplayNumCallback(DisplayNumCB displayNumCB)
 * @objective       test deregisterDisplayNumCallback positively
 * @target          static OCStackResult deregisterDisplayNumCallback()
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerDisplayNumCallback
 *                  3. call deregisterDisplayNumCallback
 * @post_condition  None
 * @expected        deregisterDisplayNumCallback will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, deregisterDisplayNumCallback_SRC_P)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerDisplayNumCallback(PMCppHelper::displayMutualVerifNumCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.deregisterDisplayNumCallback(OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerUserConfirmCallback positively
 * @target          static OCStackResult registerUserConfirmCallback(UserConfirmNumCB userConfirmCB)
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerUserConfirmCallback
 * @post_condition  None
 * @expected        registerUserConfirmCallback will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, registerUserConfirmCallback_SRC_P)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerUserConfirmCallback(PMCppHelper::confirmMutualVerifNumCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @objective       test registerUserConfirmCallback negatively userConfirmCB=NULL
 * @target          static OCStackResult registerUserConfirmCallback(UserConfirmNumCB userConfirmCB)
 * @test_data       userConfirmCB=NULL
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerUserConfirmCallback
 * @post_condition  None
 * @expected        registerUserConfirmCallback will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, registerUserConfirmCallback_NV_N)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerUserConfirmCallback(NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-07
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult registerUserConfirmCallback(UserConfirmNumCB userConfirmCB)
 * @objective       test deregisterUserConfirmCallback positively
 * @target          static OCStackResult deregisterUserConfirmCallback()
 * @test_data       regular data
 * @pre_condition   start two justworks simulators
 * @procedure       1. call provisionInit
 *                  2. call registerUserConfirmCallback
 *                  3. call deregisterUserConfirmCallback
 * @post_condition  None
 * @expected        deregisterUserConfirmCallback will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, deregisterUserConfirmCallback_SRC_P)
{

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerUserConfirmCallback(PMCppHelper::confirmMutualVerifNumCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.deregisterUserConfirmCallback(OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-03-06
 * @see             static OCStackResult provisionInit(const std::string& dbPath)
 * @see             static OCStackResult registerDisplayPinCallback(DisplayPinCB displayPinCB, DisplayPinCallbackHandle* displayPinCallbackHandle)
 * @see             static OCStackResult discoverUnownedDevices(unsigned short timeout, DeviceList_t &list)
 * @objective       test doOwnershipTransfer positively for OIC_RANDOM_DEVICE_PIN
 * @target          OCStackResult doOwnershipTransfer(ResultCallBack resultCallback)
 * @test_data       regular data
 * @pre_condition   start one randompin serer simulators
 * @procedure       1. call provisionInit
 *                  2. call registerDisplayPinCallback
 *                  3. call discoverUnownedDevices
 *                  4. call doOwnershipTransfer
 *                  5. call deregisterDisplayPinCallback
 * @post_condition  None
 * @expected        doOwnershipTransfer will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(PMCppOtmTest_btc, test)
{
    CommonUtil::launchApp(MV_JUSTWORKS_SERVER);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    if(!m_PMCppHelper.provisionInit())
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerInputPinCallback(PMCppHelper::OnInputPinCB, &callbackHandle, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerDisplayNumCallback(PMCppHelper::displayMutualVerifNumCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppHelper.registerUserConfirmCallback(PMCppHelper::confirmMutualVerifNumCB, OC_STACK_OK))
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

    if(!m_PMCppHelper.deregisterInputPinCallback(callbackHandle, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppHelper.getFailureMessage());
        return;
    }
}
#endif
