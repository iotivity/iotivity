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

#include "RICsdkHelper.h"

class RICsdkServerTest_btc: public ::testing::Test
{
public:
    RICsdkHelper *m_pRICsdkHelper;
    OCMode m_ServerMode = OC_SERVER;
    OCMode m_GatewayMode = OC_GATEWAY;
    OCMode m_ClientServerMode = OC_CLIENT_SERVER;
    OCTransportFlags m_DefaultTransportFlags = OC_DEFAULT_FLAGS;
    OCStackResult m_result;
    OCResourceHandle m_handle;
    OCResourceHandle m_lightHandle;
    OCResourceHandle m_fanHandle;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        m_pRICsdkHelper = RICsdkHelper::getInstance();
        m_result = OC_STACK_ERROR;
        m_handle = NULL;
        m_lightHandle = NULL;
        m_fanHandle = NULL;
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        OCStop();
    }

    static OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag,
            OCEntityHandlerRequest *entityHandlerRequest, void* /*callback*/)
    {
        return OC_EH_OK;
    }

    static FILE* server_fopen(const char *path, const char *mode)
    {
        (void) path;
        return fopen(DATABASE_FILE, mode);
    }
};

/**
 * @since 2016-07-19
 * @see none
 * @see OCStackResult OCStop()
 * @objective Test OCInit with positive basic way
 * @target OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @test_data    1. ipAddr null
 *                 2. port 0
 *                 3. mode server mode
 * @pre_condition none
 * @procedure Call OCInit() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCInitServer_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see none
 * @see OCStackResult OCStop()
 * @objective Test OCInit with positive basic way using mode as client_server
 * @target OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @test_data    1. ipAddr null
 *                 2. port 0
 *                 3. mode client_server mode
 * @pre_condition none
 * @procedure Call OCInit() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCInitClientServer_SRC_P)
{
    uint16_t portZero = 0;
    const char* ipAddress = nullptr;

    m_result = OCInit(ipAddress, portZero, m_ClientServerMode);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for client-server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see none
 * @see OCStackResult OCStop()
 * @objective Test OCInit with negative basic way using mode as gateway
 * @target OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @test_data    1. ipAddr null
 *                 2. port 0
 *                 3. mode gateway mode
 * @pre_condition none
 * @procedure Call OCInit() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCInit_USV_N)
{
    uint16_t portZero = 0;
    const char* ipAddress = nullptr;

    m_result = OCInit(ipAddress, portZero, m_GatewayMode);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see none
 * @see OCStackResult OCStop()
 * @objective Test OCInit1 with positive basic way
 * @target OCStackResult OCInit1(OCMode mode, OCTransportFlags serverFlags,
 *         OCTransportFlags clientFlags)
 * @test_data    1. mode OCMode server mode
 *                 2. serverFlags Default server transport flags
 *                 3. clientFlags Default client transport flags
 * @pre_condition none
 * @procedure Call OCInit1() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCInit1Server_SRC_P)
{
    m_result = OCInit1(m_ServerMode, m_DefaultTransportFlags, m_DefaultTransportFlags);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit1 failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see none
 * @see OCStackResult OCStop()
 * @objective Test OCInit1 with positive basic way using client_server mode
 * @target OCStackResult OCInit1(OCMode mode, OCTransportFlags serverFlags,
 *         OCTransportFlags clientFlags)
 * @test_data    1. mode OCMode server mode
 *                 2. serverFlags Default server transport flags
 *                 3. clientFlags Default client transport flags
 * @pre_condition none
 * @procedure Call OCInit1() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCInit1ClientServer_SRC_P)
{
    m_result = OCInit1(m_ClientServerMode, m_DefaultTransportFlags, m_DefaultTransportFlags);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit1 failed for client-server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see none
 * @see OCStackResult OCStop()
 * @objective Test OCInit1 with negative basic way with mode as gateway
 * @target OCStackResult OCInit1(OCMode mode, OCTransportFlags serverFlags,
 *         OCTransportFlags clientFlags)
 * @test_data    1. mode gateway mode
 *                 2. serverFlags Default server transport flags
 *                 3. clientFlags Default client transport flags
 * @pre_condition none
 * @procedure Call OCInit1() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCInit1Server_USV_N)
{
    m_result = OCInit1(m_GatewayMode, m_DefaultTransportFlags, m_DefaultTransportFlags);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCInit1 failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCStop with positive basic way
 * @target OCStackResult OCStop()
 * @test_data none
 * @pre_condition Call OCInit() API
 * @procedure Call OCStop()() API
 * @post_condition none
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCStop_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCStop();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCStop failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCSetPlatformInfo with positive basic way
 * @target OCStackResult OCSetPlatformInfo(OCPlatformInfo platformInfo)
 * @test_data platformInfo Structure containing the platform information
 * @pre_condition Call OCInit() API
 * @procedure Call OCSetPlatformInfo() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCSetPlatformInfo_SRC_FSV_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCSetPlatformInfo(m_pRICsdkHelper->getPlatformInfo());
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCSetPlatformInfo failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCSetPlatformInfo with negative basic way using platformInfo without setting platform information
 * @target OCStackResult OCSetPlatformInfo(OCPlatformInfo platformInfo)
 * @test_data platformInfo Structure without the platform information
 * @pre_condition Call OCInit() API
 * @procedure Call OCSetPlatformInfo() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCSetPlatformInfo_USV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    OCPlatformInfo platformInfo;
    m_result = OCSetPlatformInfo(platformInfo);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCSetPlatformInfo failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCSetDeviceInfo with positive basic way
 * @target OCStackResult OCSetDeviceInfo(OCDeviceInfo deviceInfo)
 * @test_data deviceInfo Structure containing the device information
 * @pre_condition Call OCInit() API
 * @procedure Call OCSetDeviceInfo() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCSetDeviceInfo_SRC_FSV_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    try
    {
        m_result = OCSetDeviceInfo(m_pRICsdkHelper->getDeviceInfo());
        ASSERT_EQ(OC_STACK_OK,m_result) << "OCSetDeviceInfo failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
    }
    catch(exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCCreateResource with positive basic way
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                 2. resourceTypeName "core.light"
 *                 3. resourceInterfaceName "oc.if.a"
 *                 4. uri "/a/light"
 *                 5. entityHandler entity handler to be called
 *                 6. callbackParam null
 *                 7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 * @pre_condition Call OCInit() API
 * @procedure Call OCCreateResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCCreateResource_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCCreateResource(&m_handle,RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT,OCEntityHandlerCb,NULL,OC_DISCOVERABLE|OC_OBSERVABLE);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCCreateResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCCreateResource with negative basic way using empty resourceTypeName
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                 2. resourceTypeName ""
 *                 3. resourceInterfaceName "oc.if.a"
 *                 4. uri "/a/light"
 *                 5. entityHandler entity handler to be called
 *                 6. callbackParam null
 *                 7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 * @pre_condition Call OCInit() API
 * @procedure Call OCCreateResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCCreateResourceTypeName_ESV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCCreateResource(&m_handle,EMPTY_STRING,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT,OCEntityHandlerCb,NULL,OC_DISCOVERABLE|OC_OBSERVABLE);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCCreateResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCCreateResource with negative basic way using empty uri
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                 2. resourceTypeName "core.light"
 *                 3. resourceInterfaceName "oc.mi.def"
 *                 4. uri ""
 *                 5. entityHandler entity handler to be called
 *                 6. callbackParam null
 *                 7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 * @pre_condition Call OCInit() API
 * @procedure Call OCCreateResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_URI
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCCreateResourceURI_ESV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCCreateResource(&m_handle,RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,EMPTY_STRING,OCEntityHandlerCb,NULL,OC_DISCOVERABLE|OC_OBSERVABLE);
    ASSERT_EQ(OC_STACK_INVALID_URI,m_result) << "OCCreateResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @objective Test OCDeleteResource with positive basic way
 * @target OCDeleteResource(OCResourceHandle handle)
 * @test_data OCResourceHandle pointer to the created resource
 * @pre_condition Call OCInit() API
 * @procedure Call OCCreateResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCDeleteResource_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCDeleteResource(m_handle);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCDeleteResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @objective Test OCDeleteResource with negative basic way using null value as handle
 * @target OCDeleteResource(OCResourceHandle handle)
 * @test_data OCResourceHandle null
 * @pre_condition Call OCInit() API
 * @procedure Call OCCreateResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCDeleteResourceResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCDeleteResource(NULL);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCDeleteResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCBindResourceTypeToResource with positive basic way
 * @target OCStackResult OCBindResourceTypeToResource(OCResourceHandle handle,
 *       const char *resourceTypeName)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "core.fan"
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCBindResourceTypeToResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCBindResourceTypeToResource_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResourceTypeToResource(m_handle, RESOURCE_TYPE_FAN);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCBindResourceTypeToResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCBindResourceTypeToResource with negative basic way using null handle
 * @target OCStackResult OCBindResourceTypeToResource(OCResourceHandle handle,
 *       const char *resourceTypeName)
 * @test_data         1. OCResourceHandle null
 *                     2. resourceTypeName "core.fan
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCBindResourceTypeToResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCBindResourceTypeToResourceResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResourceTypeToResource(NULL, RESOURCE_TYPE_LIGHT);
    ASSERT_EQ(OC_STACK_ERROR,m_result) << "OCBindResourceTypeToResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCBindResourceTypeToResource with negative basic way using null resource type
 * @target OCStackResult OCBindResourceTypeToResource(OCResourceHandle handle,
 *       const char *resourceTypeName)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName null
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCBindResourceTypeToResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCBindResourceTypeToResourceResourceType_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResourceTypeToResource(m_handle, NULL);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCBindResourceTypeToResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCBindResourceInterfaceToResource with positive basic way
 * @target OCStackResult OCBindResourceInterfaceToResource(OCResourceHandle handle,
 *       const char *resourceInterfaceName)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceInterfaceName "core.rw"
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCBindResourceInterfaceToResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCBindResourceInterfaceToResource_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResourceInterfaceToResource(m_handle, RESOURCE_INTERFACE_RW);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCBindResourceInterfaceToResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCBindResourceInterfaceToResource with negative basic way using null handle
 * @target OCStackResult OCBindResourceInterfaceToResource(OCResourceHandle handle,
 *        const char *resourceInterfaceName)
 * @test_data         1. OCResourceHandle null
 *                     2. resourceInterfaceName "core.rw"
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCBindResourceInterfaceToResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCBindResourceInterfaceToResourceResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResourceInterfaceToResource(NULL, RESOURCE_INTERFACE_RW);
    ASSERT_EQ(OC_STACK_ERROR,m_result) << "OCBindResourceInterfaceToResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCBindResourceInterfaceToResource with negative basic way using null interface name
 * @target OCStackResult OCBindResourceInterfaceToResource(OCResourceHandle handle,
 *        const char *resourceInterfaceName)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceInterfaceName null
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCBindResourceInterfaceToResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCBindResourceInterfaceToResourceResourceType_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResourceInterfaceToResource(m_handle, NULL);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCBindResourceInterfaceToResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCBindResource with positive basic way
 * @target OCStackResult OCBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @test_data         1. collectionHandle pointer to the collection resource
 *                     2. resourceHandle pointer to the child resource
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCBindResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCBindResource_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_lightHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_lightHandle,NULL) << "Light Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_fanHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_FAN,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_FAN);
    ASSERT_NE(m_fanHandle,NULL) << "Fan Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResource(m_fanHandle, m_lightHandle);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCBindResource with negative basic way using null collection handle
 * @target OCStackResult OCBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @test_data         1. collectionHandle null
 *                     2. resourceHandle pointer to the child resource
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCBindResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCBindResourceCollectionHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_lightHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_lightHandle,NULL) << "Light Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_fanHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_FAN,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_FAN);
    ASSERT_NE(m_fanHandle,NULL) << "Fan Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResource(NULL, m_lightHandle);
    ASSERT_EQ(OC_STACK_ERROR,m_result) << "OCBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCBindResource with negative basic way using null resource handle
 * @target OCStackResult OCBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @test_data         1. collectionHandle pointer to the collection resource
 *                     2. resourceHandle null
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCBindResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCBindResourceResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_lightHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_lightHandle,NULL) << "Light Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_fanHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_FAN,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_FAN);
    ASSERT_NE(m_fanHandle,NULL) << "Fan Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResource(m_fanHandle, NULL);
    ASSERT_EQ(OC_STACK_ERROR,m_result) << "OCBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCBindResource with negative basic way using same handle as collection and resource
 * @target OCStackResult OCBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @test_data         1. collectionHandle pointer to the resource
 *                     2. resourceHandle pointer to the same resource
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCBindResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCBindResourceSameHandle_ECRC_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_lightHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_lightHandle,NULL) << "Light Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_fanHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_FAN,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_FAN);
    ASSERT_NE(m_fanHandle,NULL) << "Fan Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResource(m_lightHandle, m_lightHandle);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @see OCStackResult OCBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @objective Test OCBindResourceInterfaceToResource with positive basic way
 * @target OCStackResult OCUnBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @test_data         1. collectionHandle pointer to the collection resource
 *                     2. resourceHandle pointer to the child resource
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCUnBindResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCUnBindResource_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_lightHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_lightHandle,NULL) << "Light Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_fanHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_FAN,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_FAN);
    ASSERT_NE(m_fanHandle,NULL) << "Fan Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResource(m_fanHandle, m_lightHandle);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCUnBindResource(m_fanHandle, m_lightHandle);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCUnBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @see OCStackResult OCBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @objective Test OCBindResourceInterfaceToResource with negative basic way using null collection handle
 * @target OCStackResult OCUnBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @test_data         1. collectionHandle null
 *                     2. resourceHandle pointer to the child resource
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCUnBindResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCUnBindResourceCollectionHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_lightHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_lightHandle,NULL) << "Light Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_fanHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_FAN,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_FAN);
    ASSERT_NE(m_fanHandle,NULL) << "Fan Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResource(m_fanHandle, m_lightHandle);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCUnBindResource(NULL, m_lightHandle);
    ASSERT_EQ(OC_STACK_ERROR,m_result) << "OCUnBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @see OCStackResult OCBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @objective Test OCBindResourceInterfaceToResource with negative basic way using null resource handle
 * @target OCStackResult OCUnBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @test_data         1. collectionHandle pointer to the collection resource
 *                     2. resourceHandle null
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCUnBindResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCUnBindResourceResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_lightHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_lightHandle,NULL) << "Light Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_fanHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_FAN,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_FAN);
    ASSERT_NE(m_fanHandle,NULL) << "Fan Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResource(m_fanHandle, m_lightHandle);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCUnBindResource(m_fanHandle, NULL);
    ASSERT_EQ(OC_STACK_ERROR,m_result) << "OCUnBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @see OCStackResult OCBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @objective Test OCBindResourceInterfaceToResource with negative basic way using same handle as collection and resource
 * @target OCStackResult OCUnBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @test_data         1. collectionHandle pointer to the collection resource
 *                     2. resourceHandle null
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCUnBindResource() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCUnBindResourceSameHandle_ECRC_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_lightHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_lightHandle,NULL) << "Light Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_fanHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_FAN,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_FAN);
    ASSERT_NE(m_fanHandle,NULL) << "Fan Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResource(m_fanHandle, m_lightHandle);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCUnBindResource(m_lightHandle, m_lightHandle);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCUnBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCStartPresence with positive basic way
 * @target OCStackResult OCStartPresence(const uint32_t ttl)
 * @test_data ttl 0 and 50
 * @pre_condition Call OCInit() API
 * @procedure Call OCStartPresence() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCStartPresence_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    const uint32_t ttl = 0;
    m_result = OCStartPresence(ttl);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCStartPresence failed for time 0. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    const uint32_t ttl2 = 50;
    m_result = OCStartPresence(ttl2);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCStartPresence failed for time 10. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCStopPresence with positive basic way
 * @target OCStackResult OCStopPresence()
 * @test_data none
 * @pre_condition Call OCInit() API
 * @procedure Call OCStop()Presence() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCStopPresence_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    const uint32_t ttl = 0;
    m_result = OCStartPresence(ttl);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCStartPresence failed for time 10. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCStopPresence();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCStopPresence failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCNotifyAllObservers with positive basic way using low quality of service
 * @target OCStackResult OCNotifyAllObservers(OCResourceHandle handle, OCQualityOfService qos)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. OCQualityOfService OC_LOW_QOS
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCNotifyAllObservers() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_NO_OBSERVERS
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCNotifyAllObserversNON_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    OCQualityOfService qos = OC_LOW_QOS;
    m_result = OCNotifyAllObservers(m_handle,qos);
    ASSERT_EQ(OC_STACK_NO_OBSERVERS,m_result) << "OCNotifyAllObservers failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCNotifyAllObservers with positive basic way using high quality of service
 * @target OCStackResult OCNotifyAllObservers(OCResourceHandle handle, OCQualityOfService qos)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. OCQualityOfService OC_HIGH_QOS
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCNotifyAllObservers() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_NO_OBSERVERS
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCNotifyAllObserversCON_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    OCQualityOfService qos = OC_HIGH_QOS;
    m_result = OCNotifyAllObservers(m_handle,qos);
    ASSERT_EQ(OC_STACK_NO_OBSERVERS,m_result) << "OCNotifyAllObservers failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCNotifyAllObservers with negative basic way using null resource handle
 * @target OCStackResult OCNotifyAllObservers(OCResourceHandle handle, OCQualityOfService qos)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. OCQualityOfService OC_HIGH_QOS
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCNotifyAllObservers() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCNotifyAllObserversResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    OCQualityOfService qos = OC_HIGH_QOS;
    m_result = OCNotifyAllObservers(NULL,qos);
    ASSERT_EQ(OC_STACK_ERROR,m_result) << "OCNotifyAllObservers failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCNotifyListOfObservers with positive basic way using low quality of service
 * @target OCNotifyListOfObservers (OCResourceHandle handle,
 *                            OCObservationId  *obsIdList,
 *                            uint8_t          numberOfIds,
 *                            const OCRepPayload *payload,
 *                            OCQualityOfService qos)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. OCObservationId List of observation IDs that need to be notified.
 *                     3. numberOfIds 0
 *                     4. OCRepPayload Object representing the notification
 *                     5. OCQualityOfService OC_LOW_QOS
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCNotifyListOfObservers() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCNotifyListOfObserversNON_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    OCObservationId obsIdList = 1;
    uint8_t numberOfIds = 0;
    OCRepPayload payload;
    payload.uri = "uri";
    const OCRepPayload payload2 = payload;
    OCQualityOfService qos = OC_LOW_QOS;

    m_result = OCNotifyListOfObservers(m_handle,&obsIdList,numberOfIds,&payload2,qos);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCNotifyListOfObservers failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCNotifyListOfObservers with positive basic way using high quality of service
 * @target OCNotifyListOfObservers (OCResourceHandle handle,
 *                            OCObservationId  *obsIdList,
 *                            uint8_t          numberOfIds,
 *                            const OCRepPayload *payload,
 *                            OCQualityOfService qos)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. OCObservationId List of observation IDs that need to be notified.
 *                     3. numberOfIds 0
 *                     4. OCRepPayload Object representing the notification
 *                     5. OCQualityOfService OC_HIGH_QOS
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCNotifyListOfObservers() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCNotifyListOfObserversCON_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    OCObservationId obsIdList = 1;
    uint8_t numberOfIds = 0;
    OCRepPayload payload;
    payload.uri = "uri";
    const OCRepPayload payload2 = payload;
    OCQualityOfService qos = OC_HIGH_QOS;

    m_result = OCNotifyListOfObservers(m_handle,&obsIdList,numberOfIds,&payload2,qos);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCNotifyListOfObservers failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCNotifyListOfObservers with negative basic way using null resource handle
 * @target OCNotifyListOfObservers (OCResourceHandle handle,
 *                            OCObservationId  *obsIdList,
 *                            uint8_t          numberOfIds,
 *                            const OCRepPayload *payload,
 *                            OCQualityOfService qos)
 * @test_data         1. OCResourceHandle null
 *                     2. OCObservationId List of observation IDs that need to be notified.
 *                     3. numberOfIds 0
 *                     4. OCRepPayload Object representing the notification
 *                     5. OCQualityOfService OC_HIGH_QOS
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCNotifyListOfObservers() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCNotifyListOfObserversResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    OCObservationId *obsIdList;
    uint8_t numberOfIds = 0;
    const OCRepPayload *payload;
    OCQualityOfService qos = OC_HIGH_QOS;

    m_result = OCNotifyListOfObservers(NULL,obsIdList,numberOfIds,payload,qos);
    ASSERT_EQ(OC_STACK_ERROR,m_result) << "OCNotifyListOfObservers failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCNotifyListOfObservers with negative basic way using null observationid list
 * @target OCNotifyListOfObservers (OCResourceHandle handle,
 *                            OCObservationId  *obsIdList,
 *                            uint8_t          numberOfIds,
 *                            const OCRepPayload *payload,
 *                            OCQualityOfService qos)
 * @test_data         1. OCResourceHandle pointer to created resource
 *                     2. OCObservationId null
 *                     3. numberOfIds 0
 *                     4. OCRepPayload Object representing the notification
 *                     5. OCQualityOfService OC_HIGH_QOS
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCNotifyListOfObservers() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCNotifyListOfObserversOCObservationId_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    uint8_t numberOfIds = 0;
    const OCRepPayload *payload;
    OCQualityOfService qos = OC_HIGH_QOS;

    m_result = OCNotifyListOfObservers(m_handle,NULL,numberOfIds,payload,qos);
    ASSERT_EQ(OC_STACK_ERROR,m_result) << "OCNotifyListOfObservers failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCNotifyListOfObservers with negative basic way using null value as payload
 * @target OCNotifyListOfObservers (OCResourceHandle handle,
 *                            OCObservationId  *obsIdList,
 *                            uint8_t          numberOfIds,
 *                            const OCRepPayload *payload,
 *                            OCQualityOfService qos)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. OCObservationId List of observation IDs that need to be notified
 *                     3. numberOfIds 0
 *                     4. OCRepPayload null
 *                     5. OCQualityOfService OC_HIGH_QOS
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCNotifyListOfObservers() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_ERROR
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCNotifyListOfObserversPayload_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    OCObservationId *obsIdList;
    uint8_t numberOfIds = 0;
    const OCRepPayload *payload;
    OCQualityOfService qos = OC_HIGH_QOS;

    m_result = OCNotifyListOfObservers(m_handle,obsIdList,numberOfIds,NULL,qos);
    ASSERT_EQ(OC_STACK_ERROR,m_result) << "OCNotifyListOfObservers failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCSetDefaultDeviceEntityHandler with positive basic way
 * @target OCStackResult OCSetDefaultDeviceEntityHandler(OCDeviceEntityHandler entityHandler, void* callbackParameter)
 * @test_data         1. OCDeviceEntityHandler handler to be set
 *                     2. callbackParameter Parameter passed back when entityHandler is called
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCSetDefaultDeviceEntityHandler() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCSetDefaultDeviceEntityHandler_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    void* callbackParameter;
    m_result = OCSetDefaultDeviceEntityHandler(OCEntityHandlerCb,callbackParameter);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCNotifyListOfObservers failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCDoResponse with negative basic way using null OCEntityHandlerResponse
 * @target OCStackResult OCDoResponse(OCEntityHandlerResponse *response)
 * @test_data OCEntityHandlerResponse null
 * @pre_condition Call OCInit() API
 * @procedure Call OCDoResponse() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCDoResponse_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCDoResponse(NULL);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCDoResponse failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCProcess with positive basic way
 * @target OCStackResult OCProcess()
 * @test_data none
 * @pre_condition Call OCInit() API
 * @procedure Call OCProcess() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCProcess_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCProcess();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCProcess failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCRegisterPersistentStorageHandler with positive basic way
 * @target OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @test_data persistentStorageHandler Pointers to open, read, write, close & unlink handlers
 * @pre_condition None
 * @procedure Call OCRegisterPersistentStorageHandler() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCRegisterPersistentStorageHandler_SRC_P)
{
    OCPersistentStorage ps =
    {   server_fopen, fread, fwrite, fclose, unlink};
    m_result = OCRegisterPersistentStorageHandler(&ps);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCRegisterPersistentStorageHandler failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCRegisterPersistentStorageHandler with negative basic way using null persistentStorageHandler
 * @target OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @test_data persistentStorageHandler null
 * @pre_condition None
 * @procedure Call OCRegisterPersistentStorageHandler() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCRegisterPersistentStorageHandler_USV_N)
{
    OCPersistentStorage ps2 =
    {   NULL, NULL, NULL, NULL, NULL};
    m_result = OCRegisterPersistentStorageHandler(NULL);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCRegisterPersistentStorageHandler failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCBindResourceHandler with positive basic way
 * @target OCStackResult OCBindResourceHandler(OCResourceHandle handle,
 *                                              OCEntityHandler entityHandler,
 *                                              void *callbackParameter)
 * @test_data         1. handle pointer to the created resource
 *                     2. entityHandler entityhandler to be called
 *                     3. callbackParameter Context parameter that will be passed to entityHandler
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCBindResourceHandler() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCBindResourceHandler_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCCreateResource(&m_handle,RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT,NULL,NULL,OC_DISCOVERABLE|OC_OBSERVABLE);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCCreateResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    void* callbackParam;
    m_result = OCBindResourceHandler(m_handle,OCEntityHandlerCb,callbackParam);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCBindResourceHandler failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCBindResourceHandler with negative basic way using null resource handle
 * @target OCStackResult OCBindResourceHandler(OCResourceHandle handle,
 *                                              OCEntityHandler entityHandler,
 *                                              void *callbackParameter)
 * @test_data         1. handle null
 *                     2. entityHandler entityHandler to be called
 *                     3. callbackParameter Context parameter that will be passed to entityHandler
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCBindResourceHandler() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCBindResourceHandlerResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCCreateResource(&m_handle,RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT,NULL,NULL,OC_DISCOVERABLE|OC_OBSERVABLE);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCCreateResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    void* callbackParam;
    m_result = OCBindResourceHandler(NULL,OCEntityHandlerCb,callbackParam);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCBindResourceHandler failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetNumberOfResources with positive basic way
 * @target OCStackResult OCGetNumberOfResources(uint8_t *numResources)
 * @test_data numResources 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetNumberOfResources() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetNumberOfResources_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    uint8_t numResources=0;
    m_result = OCGetNumberOfResources(&numResources);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCGetNumberOfResources failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetNumberOfResources with negative basic way using null numResources
 * @target OCStackResult OCGetNumberOfResources(uint8_t *numResources)
 * @test_data numResources 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetNumberOfResources() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetNumberOfResources_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCGetNumberOfResources(NULL);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCGetNumberOfResources failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetResourceHandle with positive basic way
 * @target OCResourceHandle OCGetResourceHandle(uint8_t index)
 * @test_data index 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetResourceHandle() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetResourceHandle_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    uint8_t index = 0;
    OCResourceHandle receivedHandle=NULL;
    receivedHandle = OCGetResourceHandle(index);
    ASSERT_NE(receivedHandle,NULL) << "OCGetResourceHandle failed. Received handle is null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCGetServerInstanceIDString with positive basic way
 * @target const char* OCGetServerInstanceIDString(void)
 * @test_data none
 * @pre_condition Call OCInit() API
 * @procedure Call OCGetServerInstanceIDString() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetServerInstanceIDString_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    const char* receivedIDString;
    receivedIDString = OCGetServerInstanceIDString();
    ASSERT_NE(NULL,receivedIDString) << "OCGetServerInstanceIDString failed. Received id is null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetResourceUri with positive basic way
 * @target const char *OCGetResourceUri(OCResourceHandle handle)
 * @test_data handle pointer to the created resource
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetResourceUri() API
 * @post_condition Call OCStop()
 * @expected Should return resource uri
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetResourceUri_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    const char* receivedUri=NULL;
    receivedUri = OCGetResourceUri(m_handle);
    ASSERT_NE(NULL,receivedUri) << "OCGetResourceUri failed. Received uri is null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetResourceUri with negative basic way using null resource handle
 * @target const char *OCGetResourceUri(OCResourceHandle handle)
 * @test_data handle null
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetResourceUri() API
 * @post_condition Call OCStop()
 * @expected Returned resource uri is null
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetResourceUri_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    const char* receivedUri=NULL;
    receivedUri = OCGetResourceUri(NULL);
    ASSERT_EQ(NULL,receivedUri) << "OCGetResourceUri failed. Received uri should be null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetResourceProperties with positive basic way
 * @target OCResourceProperty OCGetResourceProperties(OCResourceHandle handle)
 * @test_data handle pointer to the created resource
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetResourceProperties() API
 * @post_condition Call OCStop()
 * @expected Should return resource property
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetResourceProperties_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    OCResourceProperty receivedProperty = NULL;
    receivedProperty = OCGetResourceProperties(m_handle);
    ASSERT_NE(NULL,receivedProperty) << "OCGetResourceProperties failed. Received property is null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetNumberOfResourceTypes with positive basic way
 * @target OCStackResult OCGetNumberOfResourceTypes(OCResourceHandle handle, uint8_t *numResourceTypes)
 * @test_data         1. handle pointer to the created resource
 *                     2. numResourceTypes 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetNumberOfResourceTypes() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetNumberOfResourceTypes_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    uint8_t numResourceTypes=0;
    m_result = OCGetNumberOfResourceTypes(m_handle,&numResourceTypes);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCGetNumberOfResourceTypes failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetNumberOfResourceTypes with negative basic way using null value as hanle
 * @target OCStackResult OCGetNumberOfResourceTypes(OCResourceHandle handle, uint8_t *numResourceTypes)
 * @test_data         1. handle null
 *                     2. numResourceTypes 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetNumberOfResourceTypes() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetNumberOfResourceTypesResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    uint8_t numResourceTypes=0;
    m_result = OCGetNumberOfResourceTypes(NULL,&numResourceTypes);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCGetNumberOfResourceTypes failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetNumberOfResourceTypes with negative basic way using null value in place of number of resource types
 * @target OCStackResult OCGetNumberOfResourceTypes(OCResourceHandle handle, uint8_t *numResourceTypes)
 * @test_data         1. handle pointer to the created resource
 *                     2. numResourceTypes 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetNumberOfResourceTypes() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetNumberOfResourceTypesResourceTypes_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCGetNumberOfResourceTypes(m_handle,NULL);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCGetNumberOfResourceTypes failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetResourceTypeName with positive basic way
 * @target const char *OCGetResourceTypeName(OCResourceHandle handle, uint8_t index)
 * @test_data         1. handle pointer to the created resource
 *                     2. index 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetResourceTypeName() API
 * @post_condition Call OCStop()
 * @expected Should return resourceType Name
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetResourceTypeName_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    uint8_t index = 0;
    const char* receivedResourceType = NULL;
    receivedResourceType = OCGetResourceTypeName(m_handle,index);
    ASSERT_NE(NULL,receivedResourceType) << "OCGetResourceTypeName failed. Received resource type is null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetResourceTypeName with negative basic way using null value as resource handle
 * @target const char *OCGetResourceTypeName(OCResourceHandle handle, uint8_t index)
 * @test_data         1. handle null
 *                     2. index 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetResourceTypeName() API
 * @post_condition Call OCStop()
 * @expected Returned resourceType Name is null
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetResourceTypeNameResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    uint8_t index = 0;
    const char* receivedResourceType = NULL;
    receivedResourceType = OCGetResourceTypeName(NULL,index);
    ASSERT_EQ(NULL,receivedResourceType) << "OCGetResourceTypeName failed. Received resource type should be null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetNumberOfResourceInterfaces with positive basic way
 * @target  OCStackResult OCGetNumberOfResourceInterfaces(OCResourceHandle handle,
 *        uint8_t *numResourceInterfaces)
 * @test_data         1. handle pointer to the created resource
 *                     2. numResourceInterfaces 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetNumberOfResourceInterfaces() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetNumberOfResourceInterfaces_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    uint8_t numResourceInterfaces = 0;
    m_result = OCGetNumberOfResourceInterfaces(m_handle,&numResourceInterfaces);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCGetNumberOfResourceInterfaces failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetNumberOfResourceInterfaces with negative basic way using null value as resource handle
 * @target  OCStackResult OCGetNumberOfResourceInterfaces(OCResourceHandle handle,
 *        uint8_t *numResourceInterfaces)
 * @test_data         1. handle null
 *                     2. numResourceInterfaces 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetNumberOfResourceInterfaces() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetNumberOfResourceInterfacesResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    uint8_t numResourceInterfaces = 0;
    m_result = OCGetNumberOfResourceInterfaces(NULL,&numResourceInterfaces);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCGetNumberOfResourceInterfaces failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetNumberOfResourceInterfaces with negative basic way using null value as numResourceInterfaces
 * @target  OCStackResult OCGetNumberOfResourceInterfaces(OCResourceHandle handle,
 *        uint8_t *numResourceInterfaces)
 * @test_data         1. handle pointer to the created resource
 *                     2. numResourceInterfaces null
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetNumberOfResourceInterfaces() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetNumberOfResourceInterfacesNumResourceInterfaces_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCGetNumberOfResourceInterfaces(m_handle,NULL);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCGetNumberOfResourceInterfaces failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetResourceInterfaceName with positive basic way
 * @target const char *OCGetResourceInterfaceName(OCResourceHandle handle, uint8_t index)
 * @test_data         1. handle pointer to the created resource
 *                     2. index 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetResourceInterfaceName() API
 * @post_condition Call OCStop()
 * @expected Should return resource interface name
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetResourceInterfaceName_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    uint8_t index = 0;
    const char* receivedResourceInterfaceName = NULL;
    receivedResourceInterfaceName = OCGetResourceInterfaceName(m_handle,index);
    ASSERT_NE(NULL,receivedResourceInterfaceName) << "OCGetResourceInterfaceName failed. Received resource interface is null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetResourceInterfaceName with negative basic way using null resource handle
 * @target const char *OCGetResourceInterfaceName(OCResourceHandle handle, uint8_t index)
 * @test_data         1. handle null
 *                     2. index 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetResourceInterfaceName() API
 * @post_condition Call OCStop()
 * @expected Returned resource interface name is null
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetResourceInterfaceNameResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    uint8_t index = 0;
    const char* receivedResourceInterfaceName = NULL;
    receivedResourceInterfaceName = OCGetResourceInterfaceName(NULL,index);
    ASSERT_EQ(NULL,receivedResourceInterfaceName) << "OCGetResourceInterfaceName failed. Received resource interface should be null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @see OCStackResult OCStop()
 * @objective Test OCGetResourceHandleFromCollection with positive basic way
 * @target OCResourceHandle OCGetResourceHandleFromCollection(OCResourceHandle collectionHandle,
 *        uint8_t index)
 * @test_data         1. collectionHandle pointer to the collection resource
 *                     2. index 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 *                     3. Call OCBindResource() API
 * @procedure Call OCGetResourceHandleFromCollection() API
 * @post_condition Call OCStop()
 * @expected Should return resource handle
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetResourceHandleFromCollection_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_lightHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_lightHandle,NULL) << "Light Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_fanHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_FAN,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_FAN);
    ASSERT_NE(m_fanHandle,NULL) << "Fan Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResource(m_fanHandle, m_lightHandle);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    OCResourceHandle receivedHandle = NULL;
    uint8_t index = 0;
    receivedHandle = OCGetResourceHandleFromCollection(m_fanHandle,index);
    ASSERT_NE(NULL,receivedHandle) << "OCGetResourceHandleFromCollection failed. Received handle is null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetResourceHandleFromCollection with negative basic way using null value as resource handle
 * @target OCResourceHandle OCGetResourceHandleFromCollection(OCResourceHandle collectionHandle,
 *        uint8_t index)
 * @test_data         1. collectionHandle null
 *                     2. index 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetResourceHandleFromCollection() API
 * @post_condition Call OCStop()
 * @expected Returned resource handle is null
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetResourceHandleFromCollectionResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_lightHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_lightHandle,NULL) << "Light Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_fanHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_FAN,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_FAN);
    ASSERT_NE(m_fanHandle,NULL) << "Fan Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResource(m_fanHandle, m_lightHandle);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCBindResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    OCResourceHandle receivedHandle = NULL;
    uint8_t index = 0;
    receivedHandle = OCGetResourceHandleFromCollection(NULL,index);
    ASSERT_EQ(NULL,receivedHandle) << "OCGetResourceHandleFromCollection failed. Received handle should be null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetResourceHandler with positive basic way
 * @target OCEntityHandler OCGetResourceHandler(OCResourceHandle handle)
 * @test_data handle pointer to the created resource
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetResourceHandler() API
 * @post_condition Call OCStop()
 * @expected Should return entity handler
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetResourceHandler_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    OCEntityHandler entityHandler = NULL;
    entityHandler = OCGetResourceHandler(m_handle);
    ASSERT_NE(NULL,entityHandler) << "OCGetResourceHandler failed. Received entity handler is null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @see OCStackResult OCStop()
 * @objective Test OCGetResourceHandler with negative basic way using null value as resource handle
 * @target OCEntityHandler OCGetResourceHandler(OCResourceHandle handle)
 * @test_data handle null
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure Call OCGetResourceHandler() API
 * @post_condition Call OCStop()
 * @expected Returned entity handler should be null
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCGetResourceHandlerResourceHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    OCEntityHandler entityHandler = NULL;
    entityHandler = OCGetResourceHandler(NULL);
    ASSERT_EQ(NULL,entityHandler) << "OCGetResourceHandler failed. Received entity handler should be null";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCStartMulticastServer with positive basic way
 * @target OCStackResult OCStartMulticastServer()
 * @test_data none
 * @pre_condition Call OCInit() API
 * @procedure Call OCStartMulticastServer() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCStartMulticastServer_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCStartMulticastServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCStartMulticastServer failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCStopMulticastServer with positive basic way
 * @target OCStackResult OCStopMulticastServer()
 * @test_data none
 * @pre_condition Call OCInit() API
 * @procedure Call OCStopMulticastServer() API
 * @post_condition Call OCStop()
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerTest_btc, OCStopMulticastServer_SRC_P)
{
    m_result = m_pRICsdkHelper->initServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_result = OCStopMulticastServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCStopMulticastServer failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif
