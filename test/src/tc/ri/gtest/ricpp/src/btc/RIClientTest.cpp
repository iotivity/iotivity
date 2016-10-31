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

#include "RIHelper.h"

class RIClientTest_btc: public ::testing::Test
{
protected:
    RIHelper *m_RIHelper;

    string m_ResType = "core.home";
    ostringstream m_ResReqURI;
    ostringstream m_DeviceReqURI;
    ostringstream m_PlatformReqURI;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_RIHelper = RIHelper::getInstance();
        m_ResReqURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=" << m_ResType;
        m_DeviceReqURI << OC_MULTICAST_PREFIX << "/oic/d";
        m_PlatformReqURI << OC_MULTICAST_PREFIX << "/oic/p";
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }

public:
    void foundResource(shared_ptr< OCResource > resource)
    {
    }

    void receivedDeviceInfo(const OCRepresentation& rep)
    {
    }

    void receivedPlatformInfo(const OCRepresentation& rep)
    {
    }

    void presenceHandler(OCStackResult result, const unsigned int nonce, const string& hostAddress)
    {
    }
};

/**
 * @since 2016-07-11
 * @see none
 * @objective Test Configure(PlatformConfig& config) with InProc ServiceType
 * @target void Configure(const PlatformConfig& config)
 * @test_data PlatformConfig& config
 * @pre_condition config with InProc ServiceType
 * @procedure Call Configure() API
 * @post_condition  None
 * @expected Should not throw any exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, Configure_SRC_P)
{
    PlatformConfig config
    {   OC::ServiceType::InProc, OC::ModeType::Client, "0.0.0.0", 0, OC::QualityOfService::LowQos};

    try
    {
        OCPlatform::Configure(config);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs at Configure_SRC_P. Exception is " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-07-14
 * @see None
 * @objective Test findResource() positively
 * @target OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                       OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @test_data 1. uri = "/oic/res?rt=core.home"
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. resourceHandler
 * @pre_condition None
 * @procedure Call findResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, FindResource_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::findResource("", m_ResReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::foundResource, this, PH::_1)));
}
#endif

/**
 * @since 2016-07-14
 * @see None
 * @objective Test findResource() null uri
 * @target OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                       OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @test_data 1. uri = nullptr
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. resourceHandler
 * @pre_condition None
 * @procedure Call findResource() API
 * @post_condition None
 * @expected Should throw exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, FindResourceWithUri_NV_N)
{
    EXPECT_ANY_THROW(
            OCPlatform::findResource("", nullptr, CT_DEFAULT,
                    bind(&RIClientTest_btc::foundResource, this, PH::_1)));
}
#endif

/**
 * @since 2016-07-14
 * @see None
 * @objective Test findResource() null host
 * @target OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                       OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @test_data 1. uri = "/oic/res?rt=core.home"
 *            2. host = nullptr
 *            3. connectivityType = CT_DEFAULT
 *            4. resourceHandler
 * @pre_condition None
 * @procedure Call findResource() API
 * @post_condition None
 * @expected Should throw exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, FindResourceWithHost_NV_N)
{
    EXPECT_ANY_THROW(
            OCPlatform::findResource(nullptr, m_ResReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::foundResource, this, PH::_1)));
}
#endif

/**
 * @since 2016-07-14
 * @see None
 * @objective Test findResource() null handler
 * @target OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                       OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @test_data 1. uri = "/oic/res?rt=core.home"
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. resourceHandler = NULL
 * @pre_condition None
 * @procedure Call findResource() API
 * @post_condition None
 * @expected Should throw exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, FindResourceWithHandler_NV_N)
{
    EXPECT_ANY_THROW(OCPlatform::findResource(nullptr, m_ResReqURI.str(), CT_DEFAULT, NULL));
}
#endif

/**
 * @since 2016-07-14
 * @see None
 * @objective Test findResource() positively
 * @target OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                       OCConnectivityType connectivityType, FindCallback resourceHandler,
 *                       QualityOfService QoS)
 * @test_data 1. uri = "/oic/res?rt=core.home"
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. resourceHandler
 *            5. Qos = LowQos
 * @pre_condition None
 * @procedure Call findResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, FindResourceWithQos_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::findResource("", m_ResReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::foundResource, this, PH::_1),
                    QualityOfService::LowQos));
}
#endif

/**
 * @since 2016-07-11
 * @see None
 * @objective Test getDeviceInfo() positively
 * @target OCStackResult getDeviceInfo(const std::string& host, const std::string& deviceURI,
 *                       OCConnectivityType connectivityType, FindDeviceCallback deviceInfoHandler)
 * @test_data 1. uri = OC_MULTICAST_PREFIX
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo
 * @pre_condition None
 * @procedure Call getDeviceInfo() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetDeviceInfo_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::getDeviceInfo("", m_ResReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::receivedDeviceInfo, this, PH::_1)));
}
#endif

/**
 * @since 2016-07-11
 * @see None
 * @objective Test getDeviceInfo() with Qos positively
 * @target OCStackResult getDeviceInfo(const std::string& host, const std::string& deviceURI,
 *                       OCConnectivityType connectivityType, FindDeviceCallback deviceInfoHandler,
 *                       QualityOfService QoS)
 * @test_data 1. uri = OC_MULTICAST_PREFIX
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo
 *            5. Qos = LowQos
 * @pre_condition None
 * @procedure Call getDeviceInfo() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetDeviceInfoWithLowQos_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::getDeviceInfo("", m_DeviceReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::receivedDeviceInfo, this, PH::_1),
                    OC::QualityOfService::LowQos));
}
#endif

/**
 * @since 2016-07-11
 * @see None
 * @objective Test getDeviceInfo() with Qos positively
 * @target OCStackResult getDeviceInfo(const std::string& host, const std::string& deviceURI,
 *                       OCConnectivityType connectivityType, FindDeviceCallback deviceInfoHandler,
 *                       QualityOfService QoS)
 * @test_data 1. uri = OC_MULTICAST_PREFIX
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo
 *            5. Qos = MidQos
 * @pre_condition None
 * @procedure Call getDeviceInfo() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetDeviceInfoWithMidQos_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::getDeviceInfo("", m_DeviceReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::receivedDeviceInfo, this, PH::_1),
                    OC::QualityOfService::MidQos));
}
#endif

/**
 * @since 2016-07-11
 * @see None
 * @objective Test getDeviceInfo() with Qos positively
 * @target OCStackResult getDeviceInfo(const std::string& host, const std::string& deviceURI,
 *                       OCConnectivityType connectivityType, FindDeviceCallback deviceInfoHandler,
 *                       QualityOfService QoS)
 * @test_data 1. uri = OC_MULTICAST_PREFIX
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo
 *            5. Qos = HighQos
 * @pre_condition None
 * @procedure Call getDeviceInfo() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetDeviceInfoWithHighQos_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::getDeviceInfo("", m_DeviceReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::receivedDeviceInfo, this, PH::_1),
                    OC::QualityOfService::HighQos));
}
#endif

/**
 * @since 2016-07-11
 * @see None
 * @objective Test getDeviceInfo() with Qos positively
 * @target OCStackResult getDeviceInfo(const std::string& host, const std::string& deviceURI,
 *                       OCConnectivityType connectivityType, FindDeviceCallback deviceInfoHandler,
 *                       QualityOfService QoS)
 * @test_data 1. uri = OC_MULTICAST_PREFIX
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo
 *            5. Qos = NaQos
 * @pre_condition None
 * @procedure Call getDeviceInfo() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetDeviceInfoWithNaQos_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::getDeviceInfo("", m_DeviceReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::receivedDeviceInfo, this, PH::_1),
                    OC::QualityOfService::NaQos));
}
#endif

/**
 * @since 2016-07-11
 * @see None
 * @objective Test getDeviceInfo() with null deviceURI
 * @target OCStackResult getDeviceInfo(const std::string& host, const std::string& deviceURI,
 *                       OCConnectivityType connectivityType, FindDeviceCallback deviceInfoHandler)
 * @test_data 1. uri = nullptr
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo
 * @pre_condition None
 * @procedure Call getDeviceInfo() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetDeviceInfoWithUri_NV_N)
{
    EXPECT_ANY_THROW(
            OCPlatform::getDeviceInfo("", nullptr, CT_DEFAULT,
                    bind(&RIClientTest_btc::receivedDeviceInfo, this, PH::_1)));
}
#endif

/**
 * @since 2016-07-11
 * @see None
 * @objective Test getDeviceInfo() with null callback
 * @target OCStackResult getDeviceInfo(const std::string& host, const std::string& deviceURI,
 *                       OCConnectivityType connectivityType, FindDeviceCallback deviceInfoHandler)
 * @test_data 1. uri = nullptr
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo = NULL
 * @pre_condition None
 * @procedure Call getDeviceInfo() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetDeviceInfoWithCallback_NV_N)
{
    EXPECT_ANY_THROW(OCPlatform::getDeviceInfo("", m_DeviceReqURI.str(), CT_DEFAULT, NULL));
}
#endif

/**
 * @since 2016-07-12
 * @see None
 * @objective Test getPlatformInfo() with null deviceURI
 * @target OCStackResult getPlatformInfo(const std::string& host, const std::string& platformURI,
 *                       OCConnectivityType connectivityType, FindPlatformCallback platformInfoHandler)
 * @test_data 1. platformURI = OC_MULTICAST_PREFIX
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo
 * @pre_condition None
 * @procedure Call getPlatformInfo() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetPlatform_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::getPlatformInfo("", m_PlatformReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::receivedDeviceInfo, this, PH::_1)));
}
#endif

/**
 * @since 2016-07-12
 * @see None
 * @objective Test getPlatformInfo() positively with Qos
 * @target OCStackResult getPlatformInfo(const std::string& host, const std::string& platformURI,
 *                       OCConnectivityType connectivityType, FindPlatformCallback platformInfoHandler,
 *                       QualityOfService QoS)
 * @test_data 1. platformURI = OC_MULTICAST_PREFIX
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo
 *            5. Qos = LowQos
 * @pre_condition None
 * @procedure Call getPlatformInfo() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetPlatformInfoWithLowQos_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::getPlatformInfo("", m_PlatformReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::receivedPlatformInfo, this, PH::_1),
                    OC::QualityOfService::LowQos));
}
#endif

/**
 * @since 2016-07-12
 * @see None
 * @objective Test getPlatformInfo() positively with Qos
 * @target OCStackResult getPlatformInfo(const std::string& host, const std::string& platformURI,
 *                       OCConnectivityType connectivityType, FindPlatformCallback platformInfoHandler,
 *                       QualityOfService QoS)
 * @test_data 1. platformURI = OC_MULTICAST_PREFIX
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo
 *            5. Qos = MidQos
 * @pre_condition None
 * @procedure Call getPlatformInfo() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetPlatformInfoWithMidQos_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::getPlatformInfo("", m_PlatformReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::receivedPlatformInfo, this, PH::_1),
                    OC::QualityOfService::MidQos));
}
#endif

/**
 * @since 2016-07-12
 * @see None
 * @objective Test getPlatformInfo() positively with Qos
 * @target OCStackResult getPlatformInfo(const std::string& host, const std::string& platformURI,
 *                       OCConnectivityType connectivityType, FindPlatformCallback platformInfoHandler,
 *                       QualityOfService QoS)
 * @test_data 1. platformURI = OC_MULTICAST_PREFIX
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo
 *            5. Qos = HighQos
 * @pre_condition None
 * @procedure Call getPlatformInfo() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetPlatformInfoWithHighQos_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::getPlatformInfo("", m_PlatformReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::receivedPlatformInfo, this, PH::_1),
                    OC::QualityOfService::HighQos));
}
#endif

/**
 * @since 2016-07-12
 * @see None
 * @objective Test getPlatformInfo() positively with Qos
 * @target OCStackResult getPlatformInfo(const std::string& host, const std::string& platformURI,
 *                       OCConnectivityType connectivityType, FindPlatformCallback platformInfoHandler,
 *                       QualityOfService QoS)
 * @test_data 1. platformURI = OC_MULTICAST_PREFIX
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo
 *            5. Qos = NaQos
 * @pre_condition None
 * @procedure Call getPlatformInfo() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetPlatformInfoWithNaQos_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::getPlatformInfo("", m_PlatformReqURI.str(), CT_DEFAULT,
                    bind(&RIClientTest_btc::receivedPlatformInfo, this, PH::_1),
                    OC::QualityOfService::NaQos));
}
#endif

/**
 * @since 2016-07-12
 * @see None
 * @objective Test getPlatformInfo() with null platformURI
 * @target OCStackResult getPlatformInfo(const std::string& host, const std::string& platformURI,
 *                       OCConnectivityType connectivityType, FindPlatformCallback platformInfoHandler)
 * @test_data 1. platformURI = nullptr
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo
 * @pre_condition None
 * @procedure Call getPlatformInfo() API
 * @post_condition  None
 * @expected Should throw any exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetPlatformInfoWithUri_NV_N)
{
    EXPECT_ANY_THROW(
            OCPlatform::getPlatformInfo("", nullptr, CT_DEFAULT,
                    bind(&RIClientTest_btc::receivedPlatformInfo, this, PH::_1)));
}
#endif

/**
 * @since 2016-07-12
 * @see None
 * @objective Test getPlatformInfo() with null platformURI
 * @target OCStackResult getPlatformInfo(const std::string& host, const std::string& platformURI,
 *                       OCConnectivityType connectivityType, FindPlatformCallback platformInfoHandler)
 * @test_data 1. platformURI = OC_MULTICAST_PREFIX
 *            2. host = ""
 *            3. connectivityType = CT_DEFAULT
 *            4. receivedDeviceInfo = NULL
 * @pre_condition None
 * @procedure Call getPlatformInfo() API
 * @post_condition  None
 * @expected Should throw any exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, GetPlatformInfoWithCallback_NV_N)
{
    EXPECT_ANY_THROW(OCPlatform::getPlatformInfo("", m_PlatformReqURI.str(), CT_DEFAULT, NULL));
}
#endif

/**
 * @since 2016-07-13
 * @see None
 * @objective Test subscribePresence() positively
 * @target OCStackResult subscribePresence(OCPresenceHandle& presenceHandle, const std::string& host,
 *                       OCConnectivityType connectivityType, SubscribeCallback presenceHandler)
 * @test_data 1. host = OC_MULTICAST_IP
 *            2. connectivityType = CT_DEFAULT
 *            3. presenceHandler
 * @pre_condition None
 * @procedure Call subscribePresence() API
 * @post_condition None
 * @expected 1. Should return OC_STACK_OK
 *           2. presenceHandle should not be null
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, SubscribePresence_SRC_P)
{
    OCPlatform::OCPresenceHandle presenceHandle = nullptr;

    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::subscribePresence(presenceHandle, OC_MULTICAST_IP, CT_DEFAULT, bind(&RIClientTest_btc::presenceHandler, this, PH::_1, PH::_2, PH::_3)));

    EXPECT_NE(nullptr, presenceHandle);
}
#endif

/**
 * @since 2016-07-13
 * @see None
 * @objective Test subscribePresence() with null host address
 * @target OCStackResult subscribePresence(OCPresenceHandle& presenceHandle, const std::string& host,
 *                       OCConnectivityType connectivityType, SubscribeCallback presenceHandler)
 * @test_data 1. host = nullptr
 *            2. connectivityType = CT_DEFAULT
 *            3. presenceHandler
 * @pre_condition None
 * @procedure Call subscribePresence() API
 * @post_condition None
 * @expected 1. Should return OC_STACK_OK
 *           2. presenceHandle should not be null
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, SubscribePresenceHost_NV_N)
{
    OCPlatform::OCPresenceHandle presenceHandle = nullptr;

    EXPECT_ANY_THROW(
            OCPlatform::subscribePresence(presenceHandle, nullptr, CT_DEFAULT,
                    bind(&RIClientTest_btc::presenceHandler, this, PH::_1, PH::_2, PH::_3)));
}
#endif

/**
 * @since 2016-07-13
 * @see None
 * @objective Test subscribePresence() with null host address
 * @target OCStackResult subscribePresence(OCPresenceHandle& presenceHandle, const std::string& host,
 *                       OCConnectivityType connectivityType, SubscribeCallback presenceHandler)
 * @test_data 1. host = OC_MULTICAST_IP
 *            2. connectivityType = CT_DEFAULT
 *            3. presenceHandler = NULL
 * @pre_condition None
 * @procedure Call subscribePresence() API
 * @post_condition None
 * @expected Should throw exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, SubscribePresencePresenceHandler_NV_N)
{
    OCPlatform::OCPresenceHandle presenceHandle = nullptr;

    EXPECT_ANY_THROW(
            OCPlatform::subscribePresence(presenceHandle, OC_MULTICAST_IP, CT_DEFAULT, NULL));
}
#endif

/**
 * @since 2016-07-13
 * @see None
 * @objective Test subscribePresence() positively
 * @target OCStackResult subscribePresence(OCPresenceHandle& presenceHandle, const std::string& host,
 *                       const std::string& resourceType, OCConnectivityType connectivityType,
 *                       SubscribeCallback presenceHandler)
 * @test_data 1. host = OC_MULTICAST_IP
 *            2. resourceType = "core.light"
 *            3. connectivityType = CT_DEFAULT
 *            4. presenceHandler
 * @pre_condition None
 * @procedure Call subscribePresence() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, SubscribePresenceWithResourceType_SRC_P)
{
    OCPlatform::OCPresenceHandle presenceHandle = nullptr;
    string resourceType = "core.light";

    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::subscribePresence(presenceHandle, OC_MULTICAST_IP, resourceType, CT_DEFAULT, bind(&RIClientTest_btc::presenceHandler, this, PH::_1, PH::_2, PH::_3)));
}
#endif

/**
 * @since 2016-07-13
 * @see None
 * @objective Test subscribePresence() with null host address
 * @target OCStackResult subscribePresence(OCPresenceHandle& presenceHandle, const std::string& host,
 *                       const std::string& resourceType, OCConnectivityType connectivityType,
 *                       SubscribeCallback presenceHandler)
 * @test_data 1. host = OC_MULTICAST_IP
 *            2. resourceType = nullptr
 *            3. connectivityType = CT_DEFAULT
 *            4. presenceHandler
 * @pre_condition None
 * @procedure Call subscribePresence() API
 * @post_condition None
 * @expected Should throw exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, SubscribePresenceWithResourceType_NV_N)
{
    OCPlatform::OCPresenceHandle presenceHandle = nullptr;

    EXPECT_ANY_THROW(
            OCPlatform::subscribePresence(presenceHandle, OC_MULTICAST_IP, nullptr, CT_DEFAULT, bind(&RIClientTest_btc::presenceHandler, this, PH::_1, PH::_2, PH::_3)));
}
#endif

/**
 * @since 2016-07-13
 * @see OCStackResult subscribePresence(OCPresenceHandle& presenceHandle, const std::string& host,
 *                    const std::string& resourceType, OCConnectivityType connectivityType,
 *                    SubscribeCallback presenceHandler)
 * @objective Test unsubscribePresence() with null host address
 * @target OCStackResult unsubscribePresence(OCPresenceHandle presenceHandle)
 * @test_data presenceHandler
 * @pre_condition get presenceHandler from subscribePresence()
 * @procedure Call unsubscribePresence() API with the presenceHandler
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, UnsubscribePresence_SRC_P)
{
    OCPlatform::OCPresenceHandle presenceHandle = nullptr;

    OCPlatform::subscribePresence(presenceHandle, OC_MULTICAST_IP, CT_DEFAULT,
            bind(&RIClientTest_btc::presenceHandler, this, PH::_1, PH::_2, PH::_3));

    EXPECT_EQ(OC_STACK_OK, OCPlatform::unsubscribePresence(presenceHandle));
}
#endif

/**
 * @since 2016-07-13
 * @see OCStackResult subscribePresence(OCPresenceHandle& presenceHandle, const std::string& host,
 *                    const std::string& resourceType, OCConnectivityType connectivityType,
 *                    SubscribeCallback presenceHandler)
 * @objective Test unsubscribePresence() with null host address
 * @target OCStackResult unsubscribePresence(OCPresenceHandle presenceHandle)
 * @test_data presenceHandler
 * @pre_condition get presenceHandler from subscribePresence() (using empty string as host)
 * @procedure Call unsubscribePresence() API with the presenceHandler
 * @post_condition None
 * @expected Exception should occur
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, UnsubscribePresence_ESV_N)
{
    OCPlatform::OCPresenceHandle presenceHandle = nullptr;
    const std::string host = "";

    OCPlatform::subscribePresence(presenceHandle, host, CT_DEFAULT,
            bind(&RIClientTest_btc::presenceHandler, this, PH::_1, PH::_2, PH::_3));

    try
    {
        OCPlatform::unsubscribePresence(presenceHandle);
        SET_FAILURE("Exception should occur");
    }
    catch (exception &e)
    {
        IOTIVITYTEST_LOG(INFO, e.what());
    }
}
#endif

/**
 * @since 2016-07-13
 * @see None
 * @objective Test subscribePresence() with null host address
 * @target OCStackResult unsubscribePresence(OCPresenceHandle presenceHandle)
 * @test_data presenceHandler
 * @pre_condition get presenceHandler from subscribePresence()
 * @procedure Call unsubscribePresence() API with the presenceHandler
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIClientTest_btc, UnsubscribePresence_NV_N)
{
    OCPlatform::OCPresenceHandle presenceHandle = nullptr;

    EXPECT_ANY_THROW(OCPlatform::unsubscribePresence(presenceHandle));
}
#endif
