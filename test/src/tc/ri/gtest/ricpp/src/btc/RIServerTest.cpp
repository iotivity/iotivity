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

class RIServerTest_btc: public ::testing::Test
{
protected:
    RIHelper *m_RIHelper;

    const string m_ResourceTypeName = "core.res";
    const string m_ResourceInterface = DEFAULT_INTERFACE;
    const uint8_t m_ResourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

    OCResourceHandle m_ResourceHandle;
    string m_EmptyStr = "";
    string m_HomeUri = "a/home";
    string m_HomeType = "core.home";
    string m_KitchenUri = "a/kitchen";
    string m_KitchenType = "core.kitchen";
    string m_OfficeUri = "a/office";
    string m_OfficeType = "core.office";

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        m_RIHelper = RIHelper::getInstance();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }

public:
    OCEntityHandlerResult entityHandler(shared_ptr< OCResourceRequest > request)
    {
        return OC_EH_OK;
    }

    void onDeviceInfoReceived(const OCRepresentation& rep)
    {
    }
};

/**
 * @since 2016-06-21
 * @see none
 * @objective Test Configure(PlatformConfig& config) with InProc ServiceType
 * @target void Configure(PlatformConfig& config)
 * @test_data PlatformConfig config
 * @pre_condition configure with InProc ServiceType
 * @procedure Perform Configure() API
 * @post_condition None
 * @expected Should not throw an exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, ConfigureInProc_SRC_P)
{
    PlatformConfig config
    {   OC::ServiceType::InProc, OC::ModeType::Server, "0.0.0.0", 0, OC::QualityOfService::LowQos};

    try
    {
        OCPlatform::Configure(config);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs at ConfigureInProc_SRC_P. Exception is " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-06-21
 * @see none
 * @objective Test Configure(PlatformConfig& config) with OutOfProc ServiceType
 * @target void Configure(PlatformConfig& config)
 * @test_data PlatformConfig config
 * @pre_condition configure with OutOfProc ServiceType
 * @procedure Perform Configure() API
 * @post_condition None
 * @expected Should not throw an exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, ConfigureOutOfProc_SRC_P)
{
    PlatformConfig config
    {   OC::ServiceType::OutOfProc, OC::ModeType::Server, "0.0.0.0", 0, OC::QualityOfService::LowQos};

    try
    {
        OCPlatform::Configure(config);
    }
    catch (exception &e)
    {
        SET_FAILURE(
                "Exception occurs at ConfigureOutOfProc_SRC_P. Exception is " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-06-21
 * @see none
 * @objective Test Configure(PlatformConfig& config) default config
 * @target void Configure(const PlatformConfig& config)
 * @test_data PlatformConfig& config
 * @pre_condition config
 * @procedure Call Configure() API
 * @post_condition  None
 * @expected Should not throw an exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, ConfigureDefault_SRC_P)
{
    PlatformConfig config
    {};

    try
    {
        OCPlatform::Configure(config);
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occurs at ConfigureDefault_SRC_P. Exception is " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-06-21
 * @see none
 * @objective Test registerPlatformInfo()
 * @target OCStackResult registerPlatformInfo(OCPlatformInfo platformInfo)
 * @test_data platformInfo
 * @pre_condition initialize platformInfo
 * @procedure Perform registerPlatformInfo() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, RegisterPlatformInfo_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK, OCPlatform::registerPlatformInfo(m_RIHelper->getPlatformInfo()));
}
#endif

/**
 * @since 2016-06-21
 * @see none
 * @objective Test registerPlatformInfo() with formatted value
 * @target OCStackResult registerPlatformInfo(OCPlatformInfo platformInfo)
 * @test_data platformInfo
 * @pre_condition initialize platformInfo
 * @procedure Perform registerPlatformInfo() API
 * @post_condition None
 * @expected Should not throw an exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, RegisterPlatformInfo_FSV_P)
{
    EXPECT_NO_THROW(OCPlatform::registerPlatformInfo(m_RIHelper->getPlatformInfo()));
}
#endif

/**
 * @since 2016-06-21
 * @see none
 * @objective Test registerPlatformInfo() without proper platformInfo
 * @target OCStackResult registerPlatformInfo(OCPlatformInfo platformInfo)
 * @test_data uninitialized platformInfo
 * @pre_condition None
 * @procedure Perform registerPlatformInfo() API
 * @post_condition None
 * @expected Should throw an exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, RegisterPlatformInfo_ETC_N)
{
    OCPlatformInfo platformInfo;

    try
    {
        OCPlatform::registerPlatformInfo(platformInfo);
        SET_FAILURE("registerPlatformInfo() called successfully without proper platformInfo.");
    }
    catch (exception &e)
    {
        SUCCEED();
    }
}
#endif

/**
 * @since 2016-06-21
 * @see none
 * @objective Test registerDeviceInfo()
 * @target OCStackResult registerDeviceInfo(OCDeviceInfo deviceInfo)
 * @test_data deviceInfo
 * @pre_condition initialize deviceInfo
 * @procedure Perform registerDeviceInfo() API
 * @post_condition None
 * @expected 1. Should return OC_STACK_OK
 *           2. Should not throw any exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, RegisterDeviceInfo_SRC_P)
{
    try
    {
        OCDeviceInfo deviceInfo = m_RIHelper->getDeviceInfo();
        ASSERT_EQ(OC_STACK_OK, OCPlatform::registerDeviceInfo(deviceInfo))<< "registerDeviceInfo does not return OC_STACK_OK";
    }
    catch (exception& e)
    {
        SET_FAILURE("registerDeviceInfo() failed. Exception is " + string(e.what()));
    }
}
#endif

/**
 * @since 2016-06-27
 * @see void Configure(PlatformConfig config)
 * @objective Test registerResource() with positive way
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       string& resourceURI, const string& resourceTypeName,
 *                       const string& resourceInterface, EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @test_data 1. uri = "a/home"
 *            2. type = "core.home"
 *            3. interface = DEFAULT_INTERFACE
 * @pre_condition Platform configure for server
 * @procedure Perform registerResource() API
 * @post_condition Perform unregisterResource()
 * @expected 1. result should be OC_STACK_OK
 *           2. No exception should occur
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, registerResource_SRC_P)
{
    m_RIHelper->configClientServerPlatform();

    try
    {
        OCStackResult result = OCPlatform::registerResource(m_ResourceHandle, m_HomeUri, m_HomeType,
                m_ResourceInterface, bind(&RIServerTest_btc::entityHandler, this, PH::_1),
                m_ResourceProperty);

        OCPlatform::unregisterResource(m_ResourceHandle);

        EXPECT_EQ(OC_STACK_OK, result);
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Found exception while performing registerResource() API. Exception: "
                + string(e.what()));
    }
}
#endif

/**
 * @since 2016-06-27
 * @see void Configure(PlatformConfig config)
 * @objective Test registerResource() with empty uri
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       string& resourceURI, const string& resourceTypeName,
 *                       const string& resourceInterface, EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @test_data 1. uri = ""
 *            2. type = "core.res"
 *            3. interface = DEFAULT_INTERFACE
 * @pre_condition Platform configure for server
 * @procedure Perform registerResource() API
 * @post_condition None
 * @expected Should throw exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, registerResourceUri_ESV_P)
{
    m_RIHelper->configClientServerPlatform();

    EXPECT_ANY_THROW(
            OCPlatform::registerResource(m_ResourceHandle, m_EmptyStr, m_ResourceTypeName,
                    m_ResourceInterface, bind(&RIServerTest_btc::entityHandler, this, PH::_1),
                    m_ResourceProperty));
}
#endif

/**
 * @since 2016-06-27
 * @see void Configure(PlatformConfig config)
 * @objective Test registerResource() with empty resource type
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       string& resourceURI, const string& resourceTypeName,
 *                       const string& resourceInterface, EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @test_data 1. uri = "a/home"
 *            2. type = ""
 *            3. interface = DEFAULT_INTERFACE
 * @pre_condition Platform configure for server
 * @procedure Perform registerResource() API
 * @post_condition None
 * @expected Should throw an exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, registerResourceResType_ESV_P)
{
    m_RIHelper->configClientServerPlatform();

    EXPECT_ANY_THROW(
            OCPlatform::registerResource(m_ResourceHandle, m_HomeUri, m_EmptyStr,
                    m_ResourceInterface, bind(&RIServerTest_btc::entityHandler, this, PH::_1),
                    m_ResourceProperty));
}
#endif

/**
 * @since 2016-06-27
 * @see void Configure(PlatformConfig config)
 * @objective Test OCStackResult registerResource() with null callback
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       string& resourceURI, const string& resourceTypeName,
 *                       const string& resourceInterface, EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @test_data 1. uri = "a/home"
 *            2. type = "core.home"
 *            3. interface = DEFAULT_INTERFACE
 *            4. NULL callback
 * @pre_condition Platform configure for server
 * @procedure Perform registerResource() API
 * @post_condition Perform unregisterResource()
 * @expected Should not throw any exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, registerResourceCB_NV_P)
{
    m_RIHelper->configClientServerPlatform();

    EXPECT_NO_THROW(
            OCPlatform::registerResource(m_ResourceHandle, m_HomeUri, m_HomeType, m_ResourceInterface, NULL, m_ResourceProperty));

    OCPlatform::unregisterResource(m_ResourceHandle);
}
#endif

/**
 * @since 2016-06-27
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @objective Test unregisterResource() with positive way
 * @target OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @test_data OCResourceHandle
 * @pre_condition registerResource and get handler
 * @procedure Perform unregisterResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, UnregisterResource_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::unregisterResource(m_RIHelper->registerResource(m_HomeUri, m_HomeType)));
}
#endif

/**
 * @since 2016-06-27
 * @see None
 * @objective Test unregisterResource() with null resource handle
 * @target OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @test_data OCResourceHandle
 * @pre_condition registerResource and get handler
 * @procedure Perform unregisterResource() API
 * @post_condition None
 * @expected Should throw any exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, UnregisterResource_NV_N)
{
    EXPECT_ANY_THROW(OCPlatform::unregisterResource(NULL));
}
#endif

/**
 * @since 2016-06-27
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @objective Test bindTypeToResource() with positive way
 * @target OCStackResult bindTypeToResource(const OCResourceHandle& resourceHandle,
 *                       const std::string& resourceTypeName)
 * @test_data OCResourceHandle, resourceTypeName = "core.dimlight"
 * @pre_condition registerResource and get handler
 * @procedure Perform bindTypeToResource() API
 * @post_condition Perform unregisterResource()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, BindTypeToResource_SRC_P)
{
    string type = "core.dimlight";

    OCResourceHandle resourceHandle = m_RIHelper->registerResource(m_HomeUri, m_HomeType);

    OCStackResult result = OCPlatform::bindTypeToResource(resourceHandle, type);
    OCPlatform::unregisterResource(resourceHandle);

    EXPECT_EQ(OC_STACK_OK, result);
}
#endif

/**
 * @since 2016-06-27
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @objective Test bindTypeToResource() with empty resourceType
 * @target OCStackResult bindTypeToResource(const OCResourceHandle& resourceHandle,
 *                       const std::string& resourceTypeName)
 * @test_data OCResourceHandle, resourceTypeName = ""
 * @pre_condition registerResource and get handler
 * @procedure Perform bindTypeToResource() API
 * @post_condition Perform unregisterResource()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, BindTypeToResource_ESV_N)
{
    OCResourceHandle resourceHandle = m_RIHelper->registerResource(m_HomeUri, m_HomeType);

    OCStackResult result = OCPlatform::bindTypeToResource(resourceHandle, m_EmptyStr);
    OCPlatform::unregisterResource(resourceHandle);

    EXPECT_EQ(OC_STACK_OK, result);
}
#endif

/**
 * @since 2016-06-27
 * @see None
 * @objective Test bindTypeToResource() with empty resourceType
 * @target OCStackResult bindTypeToResource(const OCResourceHandle& resourceHandle,
 *                       const std::string& resourceTypeName)
 * @test_data Null OCResourceHandle, resourceTypeName = "core.res"
 * @pre_condition None
 * @procedure Perform bindTypeToResource() API
 * @post_condition None
 * @expected Should throw any exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, BindTypeToResource_NV_N)
{
    EXPECT_ANY_THROW(OCPlatform::bindTypeToResource(NULL, m_ResourceTypeName));
}
#endif

/**
 * @since 2016-06-28
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @objective Test bindInterfaceToResource() with positive way
 * @target OCStackResult bindInterfaceToResource(const OCResourceHandle& resourceHandle,
 *                       const std::string& resourceInterfaceName)
 * @test_data OCResourceHandle, resourceInterface = "oic.if.l"
 * @pre_condition registerResource and get handler
 * @procedure Perform bindInterfaceToResource() API
 * @post_condition Perform unregisterResource()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, BindInterfaceToResource_SRC_P)
{
    string iface = "oic.if.l";

    OCResourceHandle resourceHandle = m_RIHelper->registerResource(m_HomeUri, m_HomeType);

    OCStackResult result = OCPlatform::bindInterfaceToResource(resourceHandle, iface);

    OCPlatform::unregisterResource(resourceHandle);

    EXPECT_EQ(OC_STACK_OK, result);
}
#endif

/**
 * @since 2016-06-28
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @objective Test bindInterfaceToResource() with empty interface
 * @target OCStackResult bindInterfaceToResource(const OCResourceHandle& resourceHandle,
 *                       const std::string& resourceInterfaceName)
 * @test_data OCResourceHandle, resourceInterface = ""
 * @pre_condition registerResource and get handler
 * @procedure Perform bindInterfaceToResource() API
 * @post_condition Perform unregisterResource()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, BindInterfaceToResource_ESV_P)
{
    OCResourceHandle resourceHandle = m_RIHelper->registerResource(m_HomeUri, m_HomeType);

    OCStackResult result = OCPlatform::bindInterfaceToResource(resourceHandle, m_EmptyStr);

    OCPlatform::unregisterResource(resourceHandle);

    EXPECT_EQ(OC_STACK_OK, result);
}
#endif

/**
 * @since 2016-06-28
 * @see None
 * @objective Test bindInterfaceToResource() with null handler
 * @target OCStackResult bindInterfaceToResource(const OCResourceHandle& resourceHandle,
 *                       const std::string& resourceInterfaceName)
 * @test_data Null OCResourceHandle, resourceInterface = "oic.if.l"
 * @pre_condition registerResource and get handler
 * @procedure Perform bindInterfaceToResource() API
 * @post_condition Perform unregisterResource()
 * @expected Should throw any exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, BindInterfaceToResource_NV_N)
{
    string iface = "oic.if.l";

    EXPECT_ANY_THROW(OCPlatform::bindInterfaceToResource(NULL, iface));
}
#endif

/**
 * @since 2016-06-28
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @objective Test bindResource() with positive way
 * @target OCStackResult bindResource(const OCResourceHandle collectionHandle,
 *                       const OCResourceHandle resourceHandle)
 * @test_data OCResourceHandle
 * @pre_condition registerResource and get handler
 * @procedure Perform bindResource() API
 * @post_condition Perform unregisterResource()
 * @expected Should throw any exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, BindResource_SRC_P)
{
    OCResourceHandle resourceHome = m_RIHelper->registerResource(m_HomeUri, m_HomeType);
    OCResourceHandle resourceKitchen = m_RIHelper->registerResource(m_KitchenUri, m_KitchenType,
            LINK_INTERFACE);

    EXPECT_EQ(OC_STACK_OK, OCPlatform::bindResource(resourceHome, resourceKitchen));

    OCPlatform::unregisterResource(resourceHome);
    OCPlatform::unregisterResource(resourceKitchen);
}
#endif

/**
 * @since 2016-06-28
 * @see None
 * @objective Test bindResource()
 * @target OCStackResult bindResource(const OCResourceHandle collectionHandle,
 *                       const OCResourceHandle resourceHandle)
 * @test_data Zero OCResourceHandle
 * @pre_condition registerResource and get handler
 * @procedure Perform bindResource() API
 * @post_condition None
 * @expected Should throw any exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, BindResource_USV_N)
{
    OCResourceHandle resourceHandle1 = 0;
    OCResourceHandle resourceHandle2 = 0;

    EXPECT_ANY_THROW(OCPlatform::bindResource(resourceHandle1, resourceHandle2));
}
#endif

/**
 * @since 2016-06-28
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see OCStackResult bindResource(const OCResourceHandle collectionHandle,
 *                    const OCResourceHandle resourceHandle)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @objective Test unbindResource() with positive way
 * @target OCStackResult unbindResource(const OCResourceHandle collectionHandle,
 *                       const OCResourceHandle resourceHandle)
 * @test_data OCResourceHandle
 * @pre_condition 1. registerResource and get handler
 *                2. bindResource with handler
 * @procedure Perform unbindResource() API
 * @post_condition Perform unregisterResource()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, UnbindResource_SRC_P)
{
    OCResourceHandle resourceHome = m_RIHelper->registerResource(m_HomeUri, m_HomeType);
    OCResourceHandle resourceKitchen = m_RIHelper->registerResource(m_KitchenUri, m_KitchenType,
            LINK_INTERFACE);

    OCPlatform::bindResource(resourceHome, resourceKitchen);

    EXPECT_EQ(OC_STACK_OK, OCPlatform::unbindResource(resourceHome, resourceKitchen));

    OCPlatform::unregisterResource(resourceHome);
    OCPlatform::unregisterResource(resourceKitchen);
}
#endif

/**
 * @since 2016-06-28
 * @see None
 * @objective Test unbindResource() with negative way
 * @target OCStackResult unbindResource(const OCResourceHandle collectionHandle,
 *                       const OCResourceHandle resourceHandle)
 * @test_data OCResourceHandle
 * @pre_condition None
 * @procedure Perform unbindResource() API
 * @post_condition None
 * @expected Should throw exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, UnbindResource_USV_N)
{
    OCResourceHandle resourceHandle1 = 0;
    OCResourceHandle resourceHandle2 = 0;

    EXPECT_ANY_THROW(OCPlatform::unbindResource(resourceHandle1, resourceHandle2));
}
#endif

/**
 * @since 2016-06-28
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @objective Test bindResources() with positive way
 * @target OCStackResult bindResources(const OCResourceHandle collectionHandle,
 *                       const std::vector<OCResourceHandle>& addedResourceHandleList)
 * @test_data OCResourceHandle
 * @pre_condition registerResource and get handler
 * @procedure Perform bindResources() API
 * @post_condition Perform unregisterResource()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, BindResources_SRC_P)
{
    OCResourceHandle resourceHome = m_RIHelper->registerResource(m_HomeUri, m_HomeType);
    OCResourceHandle resourceKitchen = m_RIHelper->registerResource(m_KitchenUri, m_KitchenType,
            LINK_INTERFACE);
    OCResourceHandle resourceOffice = m_RIHelper->registerResource(m_OfficeUri, m_OfficeType,
            LINK_INTERFACE);

    vector< OCResourceHandle > resList;
    resList.push_back(resourceKitchen);
    resList.push_back(resourceOffice);

    EXPECT_EQ(OC_STACK_OK, OCPlatform::bindResources(resourceHome, resList));

    OCPlatform::unregisterResource(resourceHome);
    OCPlatform::unregisterResource(resourceKitchen);
    OCPlatform::unregisterResource(resourceOffice);
}
#endif

/**
 * @since 2016-06-29
 * @see None
 * @objective Test bindResources() with negative way
 * @target OCStackResult bindResources(const OCResourceHandle collectionHandle,
 *                       const std::vector<OCResourceHandle>& addedResourceHandleList)
 * @test_data OCResourceHandle
 * @pre_condition None
 * @procedure Perform bindResources() API
 * @post_condition None
 * @expected Should throw an exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, BindResources_USV_N)
{
    OCResourceHandle resourceHandle1 = 0;
    OCResourceHandle resourceHandle2 = 0;
    OCResourceHandle resourceHandle3 = 0;

    vector< OCResourceHandle > resList;
    resList.push_back(resourceHandle2);
    resList.push_back(resourceHandle3);

    EXPECT_ANY_THROW(OCPlatform::bindResources(resourceHandle1, resList));
}
#endif

/**
 * @since 2016-06-29
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @see OCStackResult bindResources(const OCResourceHandle collectionHandle,
 *                    const std::vector<OCResourceHandle>& addedResourceHandleList)
 * @objective Test unbindResources() with positive way
 * @target OCStackResult unbindResource(const OCResourceHandle collectionHandle,
 *                       const OCResourceHandle resourceHandle)
 * @test_data OCResourceHandle
 * @pre_condition 1. registerResource and get handler
 *                2. bindResources() with the handler
 * @procedure Perform unbindResources() API
 * @post_condition Perform unregisterResource()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, UnbindResources_SRC_P)
{
    OCResourceHandle resourceHome = m_RIHelper->registerResource(m_HomeUri, m_HomeType);
    OCResourceHandle resourceKitchen = m_RIHelper->registerResource(m_KitchenUri, m_KitchenType,
            LINK_INTERFACE);
    OCResourceHandle resourceOffice = m_RIHelper->registerResource(m_OfficeUri, m_OfficeType,
            LINK_INTERFACE);

    vector< OCResourceHandle > resList;
    resList.push_back(resourceKitchen);
    resList.push_back(resourceOffice);

    OCPlatform::bindResources(resourceHome, resList);

    EXPECT_EQ(OC_STACK_OK, OCPlatform::unbindResources(resourceHome, resList));

    OCPlatform::unregisterResource(resourceHome);
    OCPlatform::unregisterResource(resourceKitchen);
    OCPlatform::unregisterResource(resourceOffice);
}
#endif

/**
 * @since 2016-06-29
 * @see None
 * @objective Test unbindResources() with negative way
 * @target OCStackResult unbindResource(const OCResourceHandle collectionHandle,
 *                       const OCResourceHandle resourceHandle)
 * @test_data OCResourceHandle
 * @pre_condition None
 * @procedure Perform unbindResources() API
 * @post_condition None
 * @expected Should throw an exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, UnbindResources_USV_N)
{
    OCResourceHandle resourceHandle1 = 0;
    OCResourceHandle resourceHandle2 = 0;
    OCResourceHandle resourceHandle3 = 0;

    vector< OCResourceHandle > resList;
    resList.push_back(resourceHandle2);
    resList.push_back(resourceHandle3);

    EXPECT_ANY_THROW(OCPlatform::unbindResources(resourceHandle1, resList));
}
#endif

/**
 * @since 2016-06-29
 * @see none
 * @objective Test startPresence() with positive way
 * @target OCStackResult startPresence(const unsigned int ttl)
 * @test_data ttl = 30
 * @pre_condition none
 * @procedure Call startPresence() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, StartPresence_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK, OCPlatform::startPresence(DEFAULT_PRESENCE_TIME));
}
#endif

/**
 * @since 2016-06-29
 * @see none
 * @objective Test startPresence() with lower boundary value
 * @target OCStackResult startPresence(const unsigned int ttl)
 * @test_data ttl = 0
 * @pre_condition none
 * @procedure Call startPresence() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, StartPresence_LBV_P)
{
    EXPECT_EQ(OC_STACK_OK, OCPlatform::startPresence(PRESENCE_TIME_MIN));
}
#endif

/**
 * @since 2016-06-29
 * @see None
 * @objective Test startPresence() with lower boundary value
 * @target OCStackResult startPresence(const unsigned int ttl)
 * @test_data ttl = OC_MAX_PRESENCE_TTL_SECONDS
 * @pre_condition None
 * @procedure Call startPresence() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, StartPresence_UBV_P)
{
    EXPECT_EQ(OC_STACK_OK, OCPlatform::startPresence(OC_MAX_PRESENCE_TTL_SECONDS));
}
#endif

/**
 * @since 2016-06-29
 * @see OCStackResult startPresence(const unsigned int ttl)
 * @objective Test stopPresence() with positive way
 * @target OCStackResult stopPresence()
 * @test_data None
 * @pre_condition Perform startPresence() API
 * @procedure Call stopPresence() API
 * @post_condition  None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, StopPresence_SRC_P)
{
    OCPlatform::startPresence(DEFAULT_PRESENCE_TIME);
    EXPECT_EQ(OC_STACK_OK, OCPlatform::stopPresence());
}
#endif

/**
 * @since 2016-06-29
 * @see OCStackResult startPresence(const unsigned int ttl)
 * @objective Test stopPresence() without calling startPresence
 * @target OCStackResult stopPresence()
 * @test_data None
 * @pre_condition Perform startPresence() API
 * @procedure Call stopPresence() API
 * @post_condition  None
 * @expected Should throw an exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, StopPresence_USV_N)
{
    EXPECT_ANY_THROW(OCPlatform::stopPresence());
}
#endif

/**
 * @since 2016-06-29
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @objective Test notifyAllObservers() with positive way
 * @target OCStackResult notifyAllObservers(OCResourceHandle resourceHandle)
 * @test_data OCResourceHandle
 * @pre_condition registerResource and get handler
 * @procedure Perform notifyAllObservers() API
 * @post_condition Perform unregisterResource()
 * @expected Should return OC_STACK_NO_OBSERVERS
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, NotifyAllObservers_SRC_P)
{
    OCResourceHandle resourceHome = m_RIHelper->registerResource(m_HomeUri, m_HomeType);

    EXPECT_EQ(OC_STACK_NO_OBSERVERS, OCPlatform::notifyAllObservers(resourceHome));

    OCPlatform::unregisterResource(resourceHome);
}
#endif

/**
 * @since 2016-06-29
 * @see None
 * @objective Test notifyAllObservers() with negative way
 * @target OCStackResult notifyAllObservers(OCResourceHandle resourceHandle)
 * @test_data OCResourceHandle
 * @pre_condition None
 * @procedure Perform notifyAllObservers() API
 * @post_condition Perform unregisterResource()
 * @expected Should throw an exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, NotifyAllObservers_USV_N)
{
    OCResourceHandle resourceHome = 0;

    EXPECT_ANY_THROW(OCPlatform::notifyAllObservers(resourceHome));
}
#endif

/**
 * @since 2016-06-29
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @objective Test notifyAllObservers() with all Qos
 * @target OCStackResult notifyAllObservers(OCResourceHandle resourceHandle)
 * @test_data OCResourceHandle
 * @pre_condition registerResource and get handler
 * @procedure Perform notifyAllObservers() API for all Qos
 * @post_condition Perform unregisterResource()
 * @expected Should return OC_STACK_NO_OBSERVERS
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, NotifyAllObserversWithQos_SRC_P)
{
    OCResourceHandle resourceHome = m_RIHelper->registerResource(m_HomeUri, m_HomeType);

    EXPECT_EQ(OC_STACK_NO_OBSERVERS,
            OCPlatform::notifyAllObservers(resourceHome, QualityOfService::LowQos));
    EXPECT_EQ(OC_STACK_NO_OBSERVERS,
            OCPlatform::notifyAllObservers(resourceHome, QualityOfService::HighQos));
    EXPECT_EQ(OC_STACK_NO_OBSERVERS,
            OCPlatform::notifyAllObservers(resourceHome, QualityOfService::MidQos));
    EXPECT_EQ(OC_STACK_NO_OBSERVERS,
            OCPlatform::notifyAllObservers(resourceHome, QualityOfService::NaQos));

    OCPlatform::unregisterResource(resourceHome);
}
#endif

/**
 * @since 2016-06-30
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @objective Test notifyListOfObservers() negatively
 * @target OCStackResult notifyListOfObservers(OCResourceHandle resourceHandle,
 *                       ObservationIds& observationIds,
 *                       const std::shared_ptr<OCResourceResponse> responsePtr)
 * @test_data OCResourceHandle, ObservationIds and response
 * @pre_condition registerResource and get handler
 * @procedure Perform notifyListOfObservers() API
 * @post_condition Perform unregisterResource()
 * @expected Should throw exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, NotifyListOfObservers_USV_N)
{
    OCResourceHandle resourceHome = m_RIHelper->registerResource(m_HomeUri, m_HomeType);

    shared_ptr< OCResourceResponse > resourceResponse(new OCResourceResponse());
    ObservationIds interestedObservers;

    EXPECT_ANY_THROW(
            OCPlatform::notifyListOfObservers(resourceHome, interestedObservers, resourceResponse));

    OCPlatform::unregisterResource(resourceHome);
}
#endif

/**
 * @since 2016-06-30
 * @see void Configure(PlatformConfig config)
 * @see OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @see OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                    std::string& resourceURI, const std::string& resourceTypeName,
 *                    const std::string& resourceInterface, EntityHandler entityHandler,
 *                    uint8_t resourceProperty)
 * @objective Test notifyListOfObservers() negatively with Qos
 * @target OCStackResult notifyListOfObservers(OCResourceHandle resourceHandle,
 *                       ObservationIds& observationIds,
 *                       const std::shared_ptr<OCResourceResponse> responsePtr)
 * @test_data OCResourceHandle, ObservationIds and response
 * @pre_condition registerResource and get handler
 * @procedure Perform notifyListOfObservers() API
 * @post_condition Perform unregisterResource()
 * @expected Should throw exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, NotifyListOfObserversWithQos_USV_N)
{
    OCResourceHandle resourceHome = m_RIHelper->registerResource(m_HomeUri, m_HomeType);

    shared_ptr< OCResourceResponse > resourceResponse(new OCResourceResponse());

    ObservationIds interestedObservers;
    EXPECT_ANY_THROW(
            OCPlatform::notifyListOfObservers(resourceHome, interestedObservers, resourceResponse,
                    QualityOfService::HighQos));
    EXPECT_ANY_THROW(
            OCPlatform::notifyListOfObservers(resourceHome, interestedObservers, resourceResponse,
                    QualityOfService::LowQos));
    EXPECT_ANY_THROW(
            OCPlatform::notifyListOfObservers(resourceHome, interestedObservers, resourceResponse,
                    QualityOfService::MidQos));
    EXPECT_ANY_THROW(
            OCPlatform::notifyListOfObservers(resourceHome, interestedObservers, resourceResponse,
                    QualityOfService::NaQos));

    OCPlatform::unregisterResource(resourceHome);
}
#endif

/**
 * @since 2016-06-30
 * @see none
 * @objective Test setDefaultDeviceEntityHandler() positively
 * @target OCStackResult setDefaultDeviceEntityHandler(EntityHandler entityHandler)
 * @test_data entityHandler
 * @pre_condition none
 * @procedure Call setDefaultDeviceEntityHandler() API
 * @post_condition None
 * @expected Should not throw any exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, SetDefaultDeviceEntityHandler_SRC_P)
{
    EXPECT_EQ(OC_STACK_OK,
            OCPlatform::setDefaultDeviceEntityHandler(
                    bind(&RIServerTest_btc::entityHandler, this, PH::_1)));
}
#endif

/**
 * @since 2016-06-30
 * @see none
 * @objective Test setDefaultDeviceEntityHandler() with null entityHandler
 * @target OCStackResult setDefaultDeviceEntityHandler(EntityHandler entityHandler)
 * @test_data null
 * @pre_condition none
 * @procedure Call setDefaultDeviceEntityHandler() API
 * @post_condition None
 * @expected Should not throw any exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, SetDefaultDeviceEntityHandler_NV_P)
{
    EXPECT_NO_THROW(OCPlatform::setDefaultDeviceEntityHandler(NULL));
}
#endif

/**
 * @since 2016-06-30
 * @see none
 * @objective Test sendResponse() with negatively
 * @target OCStackResult sendResponse(const std::shared_ptr<OCResourceResponse> pResponse)
 * @test_data pResponse
 * @pre_condition none
 * @procedure Call sendResponse() API
 * @post_condition None
 * @expected Should throw an exception
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RIServerTest_btc, SendResponse_USV_N)
{
    shared_ptr< OCResourceResponse > resourceResponse;

    EXPECT_ANY_THROW(OCPlatform::sendResponse(resourceResponse));
}
#endif
