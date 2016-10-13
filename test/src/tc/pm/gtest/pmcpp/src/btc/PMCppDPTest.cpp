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

#include "PMCppDPHelper.h"

class PMCppDPTest_btc: public ::testing::Test
{
protected:

    PMCppDPHelper m_PMCppDPHelper;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_JUSTWORKS_SERVER_DP);
        CommonUtil::rmFile(DIRECT_PAIRING_CLIENT_DAT);
        CommonUtil::rmFile(JUSTWORKS_SERVER_DP_DAT);
        CommonUtil::copyFile(JUSTWORKS_SERVER_DP_DAT_BACKUP, JUSTWORKS_SERVER_DP_DAT);
        CommonUtil::copyFile(DIRECT_PAIRING_CLIENT_DAT_BACKUP, DIRECT_PAIRING_CLIENT_DAT);
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_JUSTWORKS_SERVER_DP);
    }
};

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @objective       test findDirectPairingDevices positively
 * @target          OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @test_data       timeout = DP_DISCOVERY_TIMEOUT
 * @pre_condition   start a direct pairing provisioned Justworks Server
 * @procedure       1. call Configure
 *                  2. call doDirectPairing
 * @post_condition  none
 * @expected        findDirectPairingDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, FindDirectPairingDevices_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @objective       test findDirectPairingDevices positively with lower boundary value of waittime
 * @target          OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @test_data       timeout = DP_DISCOVERY_TIMEOUT_ONE
 * @pre_condition   start a direct pairing provisioned Justworks Server
 * @procedure       1. call Configure
 *                  2. call doDirectPairing
 * @post_condition  none
 * @expected        findDirectPairingDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, FindDirectPairingDevices_LBV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT_ZERO,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @see             OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @objective       test doDirectPairing positively
 * @target          OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel, const std::string& pinNumber, DirectPairingCallback resultCallback)
 * @test_data       regular data for doDirectPairing API
 * @pre_condition   start a direct pairing provisioned Justworks Server
 * @procedure       1. call Configure
 *                  2. call findDirectPairingDevices
 *                  3. call doDirectPairing
 * @post_condition  none
 * @expected        doDirectPairing will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, DoDirectPairing_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.doDirectPairing(DP_DEV_INSTANCE_ONE, DP_PRE_CONFIGURED, DP_PRECONFIG_PIN, PMCppDPHelper::resultCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @see             OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @objective       test doDirectPairing negatively with NULL pinNumber
 * @target          OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel, const std::string& pinNumber, DirectPairingCallback resultCallback)
 * @test_data       pinNumber = NULL
 * @pre_condition   start a direct pairing provisioned Justworks Server
 * @procedure       1. call Configure
 *                  2. call findDirectPairingDevices
 *                  3. call doDirectPairing
 * @post_condition  none
 * @expected        doDirectPairing will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, DoDirectPairingPin_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.doDirectPairing(DP_DEV_INSTANCE_ONE, DP_NOT_ALLOWED, NULL, PMCppDPHelper::resultCallback, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @see             OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @objective       test doDirectPairing negatively with wrong pinNumber
 * @target          OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel, const std::string& pinNumber, DirectPairingCallback resultCallback)
 * @test_data       Wrong pinNumber
 * @pre_condition   start a direct pairing provisioned Justworks Server
 * @procedure       1. call Configure
 *                  2. call findDirectPairingDevices
 *                  3. call doDirectPairing
 * @post_condition  none
 * @expected        doDirectPairing's resultCallback will return OC_STACK_AUTHENTICATION_FAILURE
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, DoDirectPairingPin_EG_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.doDirectPairing(DP_DEV_INSTANCE_ONE, DP_PRE_CONFIGURED, DP_WRONG_PIN, PMCppDPHelper::resultCallback, OC_STACK_OK, OC_STACK_AUTHENTICATION_FAILURE))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @see             OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @objective       test doDirectPairing negatively with wrong pinNumber
 * @target          OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel, const std::string& pinNumber, DirectPairingCallback resultCallback)
 * @test_data       Wrong pinNumber
 * @pre_condition   start a direct pairing provisioned Justworks Server
 * @procedure       1. call Configure
 *                  2. call findDirectPairingDevices
 *                  3. call doDirectPairing
 * @post_condition  none
 * @expected        doDirectPairing's resultCallback will return OC_STACK_ERROR
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, DoDirectPairingPmSel_EG_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.doDirectPairing(DP_DEV_INSTANCE_ONE, DP_NOT_ALLOWED, DP_PRECONFIG_PIN, PMCppDPHelper::resultCallback, OC_STACK_OK, OC_STACK_ERROR))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @see             OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @objective       test doDirectPairing negatively with resultCallback as null
 * @target          OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel, const std::string& pinNumber, DirectPairingCallback resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   start a direct pairing provisioned Justworks Server
 * @procedure       1. call Configure
 *                  2. call findDirectPairingDevices
 *                  3. call doDirectPairing
 * @post_condition  none
 * @expected        doDirectPairing will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, DoDirectPairingCB_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.doDirectPairing(DP_DEV_INSTANCE_ONE, DP_PRE_CONFIGURED, DP_PRECONFIG_PIN, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @see             OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @see             OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel, const std::string& pinNumber, DirectPairingCallback resultCallback)
 * @objective       test getDirectPairedDevices positively
 * @target          OCStackResult getDirectPairedDevices(GetDirectPairedCallback callback)
 * @test_data       regular data for getDirectPairedDevices API
 * @pre_condition   start a direct pairing provisioned Justworks Server
 * @procedure       1. call Configure
 *                  2. call findDirectPairingDevices
 *                  3. call doDirectPairing
 *                  4. call getDirectPairedDevices
 * @post_condition  none
 * @expected        getDirectPairedDevices will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, GetDirectPairedDevices_RV_SRC_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.doDirectPairing(DP_DEV_INSTANCE_ONE, DP_PRE_CONFIGURED, DP_PRECONFIG_PIN, PMCppDPHelper::resultCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.getDirectPairedDevices(PMCppDPHelper::pairedDevListCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @see             OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @see             OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel, const std::string& pinNumber, DirectPairingCallback resultCallback)
 * @objective       test getDirectPairedDevices negatively with callback as NULL
 * @target          OCStackResult getDirectPairedDevices(GetDirectPairedCallback callback)
 * @test_data       callback = NULL
 * @pre_condition   start a direct pairing provisioned Justworks Server
 * @procedure       1. call Configure
 *                  2. call findDirectPairingDevices
 *                  3. call doDirectPairing
 *                  4. call getDirectPairedDevices
 * @post_condition  none
 * @expected        getDirectPairedDevices will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, GetDirectPairedDevicesCb_NV_N)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.doDirectPairing(DP_DEV_INSTANCE_ONE, DP_PRE_CONFIGURED, DP_PRECONFIG_PIN, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.getDirectPairedDevices(NULL, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @see             OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @see             OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel, const std::string& pinNumber, DirectPairingCallback resultCallback)
 * @objective       test getHost positively
 * @target          std::string getHost()
 * @test_data       none
 * @pre_condition   none
 * @procedure       1. call Configure
 *                  2. call findDirectPairingDevices
 *                  3. call getHost
 * @post_condition  none
 * @expected        getHost will throw no exception
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, GetHost_RV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.getHost(DP_DEV_INSTANCE_ONE))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @see             OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @see             OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel, const std::string& pinNumber, DirectPairingCallback resultCallback)
 * @objective       test getDeviceID positively
 * @target          std::string getDeviceID()
 * @test_data       none
 * @pre_condition   none
 * @procedure       1. call Configure
 *                  2. call findDirectPairingDevices
 *                  3. call getDeviceID
 * @post_condition  none
 * @expected        getDeviceID will throw no exception
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, GetDeviceID_RV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.getDeviceID(DP_DEV_INSTANCE_ONE))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @see             OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @see             OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel, const std::string& pinNumber, DirectPairingCallback resultCallback)
 * @objective       test getPairingMethods positively
 * @target          std::vector<OCPrm_t> getPairingMethods()
 * @test_data       none
 * @pre_condition   none
 * @procedure       1. call Configure
 *                  2. call findDirectPairingDevices
 *                  3. call getPairingMethods
 * @post_condition  none
 * @expected        getPairingMethods will throw no exception
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, GetPairingMethods_RV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.getPairingMethods(DP_DEV_INSTANCE_ONE))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @see             OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @see             OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel, const std::string& pinNumber, DirectPairingCallback resultCallback)
 * @objective       test getConnType positively
 * @target          OCConnectivityType getConnType()
 * @test_data       none
 * @pre_condition   none
 * @procedure       1. call Configure
 *                  2. call findDirectPairingDevices
 *                  3. call getConnType
 * @post_condition  none
 * @expected        getConnType will throw no exception
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, GetConnType_RV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.getConnType(DP_DEV_INSTANCE_ONE))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-08-02
 * @see             void OC::OCPlatform::Configure(const PlatformConfig& config)
 * @see             OCStackResult findDirectPairingDevices(unsigned short waittime, GetDirectPairedCallback callback)
 * @see             OCStackResult doDirectPairing(std::shared_ptr<OCDirectPairing> peer, OCPrm_t pmSel, const std::string& pinNumber, DirectPairingCallback resultCallback)
 * @objective       test getDev positively
 * @target          OCConnectivityType getDev()
 * @test_data       none
 * @pre_condition   none
 * @procedure       1. call Configure
 *                  2. call findDirectPairingDevices
 *                  3. call getConnType
 * @post_condition  none
 * @expected        getDev will throw no exception
 */
#if defined(__LINUX__)
TEST_F(PMCppDPTest_btc, GetDev_RV_P)
{
    CommonUtil::launchApp(JUSTWORKS_SERVER_DP);

    OCPersistentStorage ps
    {   clientOpen, fread, fwrite, fclose, unlink};

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, ALL_IPV4_ADDRESS, 0, OC::QualityOfService::LowQos, &ps};

    OCPlatform::Configure(cfg);

    if(!m_PMCppDPHelper.findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    PMCppDPHelper::findCallback, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
        return;
    }

    if(!m_PMCppDPHelper.getDev(DP_DEV_INSTANCE_ONE))
    {
        SET_FAILURE(m_PMCppDPHelper.getFailureMessage());
    }
}
#endif

#endif /*#if defined(__DIRECTPAIRING__)*/
