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

class RICsdkIntegrationTest_stc: public ::testing::Test
{
public:
    RICsdkHelper *m_pRICsdkHelper;
    OCMode m_ClientMode = OC_CLIENT;
    OCTransportFlags m_DefaultTransportFlags = OC_DEFAULT_FLAGS;
    OCStackResult m_result;
    OCResourceHandle m_resourceHandle;
    OCDoHandle m_doHandle;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        RICsdkHelper::s_isBtc = false;

        m_pRICsdkHelper = RICsdkHelper::getInstance();
        m_result = OC_STACK_ERROR;
        RICsdkHelper::s_isCallback == false;

        m_result = m_pRICsdkHelper->initClientServer();
        ASSERT_EQ(OC_STACK_OK,m_result)<< "OCInit failed for client-server. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);

        m_pRICsdkHelper->startOCProcess();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        RICsdkHelper::s_quitFlag = true;
        OCStop();
    }
};

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCSetPlatformInfo and OCDoResource with positive way to check if PlatformDiscoveryReqCB callback is called
 * @target OCStackResult OCSetPlatformInfo(OCPlatformInfo platformInfo)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. platformInfo Structure containing the platform information
 *                    2. OCDoHandle handle to returned
 *                     3. OCMethod OC_REST_DISCOVER
 *                     4. requestUri "/oic/p"
 *                     5. OCDevAddr destination address
 *                     6. OCPayload payload with the request
 *                     7. OCConnectivityType CT_DEFAULT
 *                     8. OCQualityOfService OC_LOW_QOS
 *                     9. OCCallbackData callback data
 *                     10. OCHeaderOption 0
 *                  11. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCSetPlatformInfo() API to set platform information
 *                     2. Call OCDoResource() using low quality of service for discovering platform information
 *                     3. Check if PlatformDiscoveryReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback should be called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, SetAndGetPlatformInfoNON_CV_P)
{
    OCStackResult m_result;

    m_result = OCSetPlatformInfo(m_pRICsdkHelper->getPlatformInfo());
    ASSERT_EQ(OC_STACK_OK,m_result) << "Server: OCSetPlatformInfo failed. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,PLATFORM_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: PlatformDiscoveryReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @see OCStackResult OCProcess()
 * @objective Test OCSetPlatformInfo and OCDoResource with positive way to check if PlatformDiscoveryReqCB callback is called
 * @target OCStackResult OCSetPlatformInfo(OCPlatformInfo platformInfo)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. platformInfo Structure containing the platform information
 *                    2. OCDoHandle handle to returned
 *                     3. OCMethod OC_REST_DISCOVER
 *                     4. requestUri "/oic/p"
 *                     5. OCDevAddr destination address
 *                     6. OCPayload payload with the request
 *                     7. OCConnectivityType CT_DEFAULT
 *                     8. OCQualityOfService OC_HIGH_QOS
 *                     9. OCCallbackData callback data
 *                     10. OCHeaderOption 0
 *                     11. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCSetPlatformInfo() API to set platform information
 *                     2. Call OCDoResource() using high quality of service for discovering platform information
 *                     3. Check if PlatformDiscoveryReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback should be called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, SetAndGetPlatformInfoCON_CV_P)
{
    OCStackResult m_result;

    m_result = OCSetPlatformInfo(m_pRICsdkHelper->getPlatformInfo());
    ASSERT_EQ(OC_STACK_OK,m_result) << "Server: OCSetPlatformInfo failed. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,PLATFORM_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: PlatformDiscoveryReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCSetPlatformInfo and OCDoResource with positive way to check if set platform information is get
 * @target OCStackResult OCSetPlatformInfo(OCPlatformInfo platformInfo)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. platformInfo Structure containing the platform information
 *                    2. OCDoHandle handle to returned
 *                     3. OCMethod OC_REST_DISCOVER
 *                     4. requestUri "/oic/p"
 *                     5. OCDevAddr destination address
 *                     6. OCPayload payload with the request
 *                     7. OCConnectivityType CT_DEFAULT
 *                     8. OCQualityOfService OC_LOW_QOS
 *                     9. OCCallbackData callback data
 *                     10. OCHeaderOption 0
 *                  11. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCSetPlatformInfo() API to set platform information
 *                     2. Call OCDoResource() using low quality of service for discovering platform information
 *                     3. Check if server sends response successfully
 *                     3. Check if set platform information is get
 * @post_condition  Call OCStop()
 * @expected Set platform information is get
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, SetAndGetPlatformInfoPayloadCheckNON_SQV_P)
{
    OCStackResult m_result;

    m_result = OCSetPlatformInfo(m_pRICsdkHelper->getPlatformInfo());
    ASSERT_EQ(OC_STACK_OK,m_result) << "Server: OCSetPlatformInfo failed. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,PLATFORM_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false || RICsdkHelper::s_isPayloadCorrect == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCSetPlatformInfo and OCDoResource with positive way to check if set platform information is get
 * @target OCStackResult OCSetPlatformInfo(OCPlatformInfo platformInfo)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. platformInfo Structure containing the platform information
 *                    2. OCDoHandle handle to returned
 *                     3. OCMethod OC_REST_DISCOVER
 *                     4. requestUri "/oic/p"
 *                     5. OCDevAddr destination address
 *                     6. OCPayload payload with the request
 *                     7. OCConnectivityType CT_DEFAULT
 *                     8. OCQualityOfService OC_HIGH_QOS
 *                     9. OCCallbackData callback data
 *                     10. OCHeaderOption 0
 *                  11. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCSetPlatformInfo() API to set platform information
 *                     2. Call OCDoResource() using high quality of service for discovering platform information
 *                     3. Check if server sends response successfully
 *                     3. Check if set platform information is get
 * @post_condition  Call OCStop()
 * @expected Set platform information is get
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, SetAndGetPlatformInfoPayloadCheckCON_SQV_P)
{
    OCStackResult m_result;

    m_result = OCSetPlatformInfo(m_pRICsdkHelper->getPlatformInfo());
    ASSERT_EQ(OC_STACK_OK,m_result) << "Server: OCSetPlatformInfo failed. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,PLATFORM_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false || RICsdkHelper::s_isPayloadCorrect == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCSetDeviceInfo and OCDoResource with positive way to check if DeviceDiscoveryReqCB callback is called
 * @target OCStackResult OCSetDeviceInfo(OCDeviceInfo deviceInfo)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCDeviceInfo Structure containing the device information
 *                    2. OCDoHandle handle to returned
 *                     3. OCMethod OC_REST_DISCOVER
 *                     4. requestUri "/oic/d"
 *                     5. OCDevAddr destination address
 *                     6. OCPayload payload with the request
 *                     7. OCConnectivityType CT_DEFAULT
 *                     8. OCQualityOfService OC_LOW_QOS
 *                     9. OCCallbackData callback data
 *                     10. OCHeaderOption 0
 *                  11. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCSetDeviceInfo() API to set device information
 *                     2. Call OCDoResource() using low quality of service for discovering device information
 *                     3. Check if DeviceDiscoveryReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback should be called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, SetAndGetDeviceInfoNON_CV_P)
{
    OCStackResult m_result;

    try
    {
        m_result = OCSetDeviceInfo(m_pRICsdkHelper->s_deviceInfo);
        ASSERT_EQ(OC_STACK_OK,m_result) << "OCSetDeviceInfo failed. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);
    }
    catch(exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,DEVICE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("PlatformDiscoveryReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCSetDeviceInfo and OCDoResource with positive way to check if DeviceDiscoveryReqCB callback is called
 * @target OCStackResult OCSetDeviceInfo(OCDeviceInfo deviceInfo)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCDeviceInfo Structure containing the device information
 *                    2. OCDoHandle handle to returned
 *                     3. OCMethod OC_REST_DISCOVER
 *                     4. requestUri "/oic/d"
 *                     5. OCDevAddr destination address
 *                     6. OCPayload payload with the request
 *                     7. OCConnectivityType CT_DEFAULT
 *                     8. OCQualityOfService OC_HIGH_QOS
 *                     9. OCCallbackData callback data
 *                     10. OCHeaderOption 0
 *                  11. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCSetDeviceInfo() API to set device information
 *                     2. Call OCDoResource() using high quality of service for discovering device information
 *                     3. Check if DeviceDiscoveryReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback should be called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, SetAndGetDeviceInfoCON_CV_P)
{
    OCStackResult m_result;

    try
    {
        m_result = OCSetDeviceInfo(m_pRICsdkHelper->s_deviceInfo);
        ASSERT_EQ(OC_STACK_OK,m_result) << "OCSetDeviceInfo failed. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);
    }
    catch(exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,DEVICE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("PlatformDiscoveryReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCSetDeviceInfo and OCDoResource with positive way to check if set device information is get
 * @target OCStackResult OCSetDeviceInfo(OCDeviceInfo deviceInfo)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCDeviceInfo Structure containing the device information
 *                    2. OCDoHandle handle to returned
 *                     3. OCMethod OC_REST_DISCOVER
 *                     4. requestUri "/oic/d"
 *                     5. OCDevAddr destination address
 *                     6. OCPayload payload with the request
 *                     7. OCConnectivityType CT_DEFAULT
 *                     8. OCQualityOfService OC_LOW_QOS
 *                     9. OCCallbackData callback data
 *                     10. OCHeaderOption 0
 *                  11. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCSetDeviceInfo() API to set device information
 *                     2. Call OCDoResource() using low quality of service for discovering device information
 *                     3. Check if server sends response successfully
 *                     4. Check if set device information is get
 * @post_condition  Call OCStop()
 * @expected Set device information is get
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, SetAndGetDeviceInfoPayloadCheckNON_SQV_P)
{
    OCStackResult m_result;

    try
    {
        m_result = OCSetDeviceInfo(m_pRICsdkHelper->s_deviceInfo);
        ASSERT_EQ(OC_STACK_OK,m_result) << "OCSetDeviceInfo failed. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);
    }
    catch(exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,DEVICE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false || RICsdkHelper::s_isPayloadCorrect == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCSetDeviceInfo and OCDoResource with positive way to check if set device information is get
 * @target OCStackResult OCSetDeviceInfo(OCDeviceInfo deviceInfo)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCDeviceInfo Structure containing the device information
 *                    2. OCDoHandle handle to returned
 *                     3. OCMethod OC_REST_DISCOVER
 *                     4. requestUri "/oic/d"
 *                     5. OCDevAddr destination address
 *                     6. OCPayload payload with the request
 *                     7. OCConnectivityType CT_DEFAULT
 *                     8. OCQualityOfService OC_HIGH_QOS
 *                     9. OCCallbackData callback data
 *                     10. OCHeaderOption 0
 *                  11. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCSetDeviceInfo() API to set device information
 *                     2. Call OCDoResource() using high quality of service for discovering device information
 *                     3. Check if server sends response successfully
 *                     4. Check if set device information is get
 * @post_condition  Call OCStop()
 * @expected Set device information is get
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, SetAndGetDeviceInfoPayloadCheckCON_SQV_P)
{
    OCStackResult m_result;

    try
    {
        m_result = OCSetDeviceInfo(m_pRICsdkHelper->s_deviceInfo);
        ASSERT_EQ(OC_STACK_OK,m_result) << "OCSetDeviceInfo failed. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);
    }
    catch(exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,DEVICE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false || RICsdkHelper::s_isPayloadCorrect == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if ResourceDiscoveryReqCB callback is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_DISCOVER
 *                     10. requestUri "/oic/res?rt=oic.r.temperature"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_LOW_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using low quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 * @post_condition  Call OCStop()
 * @expected Callback should be called and resource is found
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndDiscoverResourceNON_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if ResourceDiscoveryReqCB callback is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_DISCOVER
 *                     10. requestUri "/oic/res?rt=oic.r.temperature"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_HIGH_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 * @post_condition  Call OCStop()
 * @expected Callback should be called and resource should be found
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndDiscoverResourceCON_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if created resource's information is found
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_DISCOVER
 *                     10. requestUri "/oic/res?rt=oic.r.temperature"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_LOW_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using low quality of service for discovering resource
 *                     3. Check if server sends response successfully
 *                     4. Check if resource is found and created resources information is get
 * @post_condition  Call OCStop()
 * @expected Resource is found and created resources information is get
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndDiscoverResourcePayloadCheckNON_SQV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false || RICsdkHelper::s_isPayloadCorrect == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if created resource's information is found
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_DISCOVER
 *                     10. requestUri "/oic/res?rt=oic.r.temperature"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_HIGH_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if server sends response successfully
 *                     4. Check if resource is found and created resources information is get
 * @post_condition  Call OCStop()
 * @expected Resource is found and created resources information is get
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndDiscoverResourceCheckPayloadCheckCON_SQV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false || RICsdkHelper::s_isPayloadCorrect == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if getReqCB is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_GET
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_LOW_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using low quality of service for sending get request to the discovered resource
 *                     5. Check if getReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback is called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendGetRequestNON_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_GET,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: getReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if getReqCB is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_GET
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_HIGH_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using high quality of service for sending get request to the discovered resource
 *                     5. Check if getReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback is called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendGetRequestCON_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_GET,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: getReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if getReqCB callback's payload matches
 *         resource attribute's value which are set from server side
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @target OCStackResult OCDoResponse(OCEntityHandlerResponse *response)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_GET
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_LOW_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using low quality of service for sending get request to the discovered resource
 *                     5. Check if server sends response successfully
 *                     6. Check if callback's payload matches resource attribute's value which are set from server side
 * @post_condition  Call OCStop()
 * @expected callback's payload matches resource attribute's value which are set from server side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendGetRequestPayloadCheckNON_SQV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
        return;
    }

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_GET,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false || RICsdkHelper::s_isPayloadCorrect == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if getReqCB callback's payload matches
 *         resource attribute's value which are set from server side
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @target OCStackResult OCDoResponse(OCEntityHandlerResponse *response)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_GET
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_HIGH_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using low quality of service for sending get request to the discovered resource
 *                     5. Check if server sends response successfully
 *                     6. Check if callback's payload matches resource attribute's value which are set from server side
 * @post_condition  Call OCStop()
 * @expected callback's payload matches resource attribute's value which are set from server side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendGetRequestPayloadCheckCON_SQV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
        return;
    }

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_GET,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false || RICsdkHelper::s_isPayloadCorrect == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if putReqCB is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_PUT
 *                     10. requestUri "/TemperatureResURI"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_LOW_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using low quality of service for sending get request to the discovered resource
 *                     5. Check if putReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback is called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendPutRequestNON_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_PUT,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: putReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if putReqCB is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_PUT
 *                     10. requestUri "/TemperatureResURI"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_HIGH_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using high quality of service for sending get request to the discovered resource
 *                     5. Check if putReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback is called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendPutRequestCON_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_PUT,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: putReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if putReqCB callback's payload matches
 *         resource attribute's value which are sent from client side
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @target OCStackResult OCDoResponse(OCEntityHandlerResponse *response)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_PUT
 *                     10. requestUri "/TemperatureResURI"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_LOW_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using low quality of service for sending put request to the discovered resource
 *                     5. Check if server sends response successfully
 *                     6. Check if callback's payload matches resource attribute's value which are sent from client side
 * @post_condition  Call OCStop()
 * @expected callback's payload matches resource attribute's value which are sent from client side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendPutRequestPayloadCheckNON_SQV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
        return;
    }

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_PUT,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false || RICsdkHelper::s_isPayloadCorrect == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if putReqCB callback's payload matches
 *         resource attribute's value which are sent from client side
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @target OCStackResult OCDoResponse(OCEntityHandlerResponse *response)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_PUT
 *                     10. requestUri "/TemperatureResURI"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_HIGH_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using high quality of service for sending put request to the discovered resource
 *                     5. Check if server sends response successfully
 *                     6. Check if callback's payload matches resource attribute's value which are sent from client side
 * @post_condition  Call OCStop()
 * @expected callback's payload matches resource attribute's value which are sent from client side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendPutRequestPayloadCheckCON_SQV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
        return;
    }

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_PUT,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false || RICsdkHelper::s_isPayloadCorrect == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if postReqCB is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_POST
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_LOW_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using low quality of service for sending post request to the discovered resource
 *                     5. Check if postReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback is called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendPostRequestNON_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_POST,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: postReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if postReqCB is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_POST
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_HIGH_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using high quality of service for sending post request to the discovered resource
 *                     5. Check if postReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback is called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendPostRequestCON_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_POST,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: postReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if postReqCB callback's payload matches
 *         resource attribute's value which are sent from client side
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @target OCStackResult OCDoResponse(OCEntityHandlerResponse *response)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_POST
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_LOW_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using low quality of service for sending post request to the discovered resource
 *                     5. Check if server sends response successfully
 *                     6. Check if callback's payload matches resource attribute's value which are sent from client side
 * @post_condition  Call OCStop()
 * @expected callback's payload matches resource attribute's value which are sent from client side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendPostRequestPayloadCheckNON_SQV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
        return;
    }

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_POST,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false || RICsdkHelper::s_isPayloadCorrect == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if postReqCB callback's payload matches
 *         resource attribute's value which are sent from client side
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @target OCStackResult OCDoResponse(OCEntityHandlerResponse *response)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_POST
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_HIGH_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using high quality of service for sending post request to the discovered resource
 *                     5. Check if server sends response successfully
 *                     6. Check if callback's payload matches resource attribute's value which are sent from client side
 * @post_condition  Call OCStop()
 * @expected callback's payload matches resource attribute's value which are sent from client side
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendPostRequestPayloadCheckCON_SQV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
        return;
    }

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_POST,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isServerResponse == false || RICsdkHelper::s_isPayloadCorrect == false)
    {
        SET_FAILURE(RICsdkHelper::s_failureMsg);
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if deleteReqCB is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_POST
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_LOW_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using low quality of service for sending delete request to the discovered resource
 *                     5. Check if deleteReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback is called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendDeleteRequestNON_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DELETE,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: deleteReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if deleteReqCB is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_POST
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_HIGH_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using high quality of service for sending delete request to the discovered resource
 *                     5. Check if deleteReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback is called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendDeleteRequestCON_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DELETE,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: deleteReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if deleteReqCB is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_POST
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_LOW_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using low quality of service for sending delete request to the discovered resource
 *                     5. Check if deleteReqCB callback is called
 *                     6. Call Call OCDoResource() again for discovering resource
 *                     7. Check if resource is not found
 * @post_condition  Call OCStop()
 * @expected Resource is not found after delete operation
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendDeleteRequestAndCheckIfResourceDeleted_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DELETE,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: deleteReqCB callback is not called");
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == true)
    {
        SET_FAILURE("Client: Resource should not be found");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if obsReqCB is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_POST
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_LOW_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using low quality of service for sending observe request to the discovered resource
 *                     5. Check if obsReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback is called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendObserveRequestNON_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_OBSERVE,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: observeReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCCreateResource and OCDoResource with positive way to check if obsReqCB is called
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "oic.r.temperature"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/fan"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                    8. OCDoHandle handle to returned
 *                     9. OCMethod OC_REST_POST
 *                     10. requestUri "/TemperatureResURI?if=oic.if.baseline"
 *                     11. OCDevAddr destination address
 *                     12. OCPayload payload with the request
 *                     13. OCConnectivityType CT_DEFAULT
 *                     14. OCQualityOfService OC_HIGH_QOS
 *                     15. OCCallbackData callback data
 *                     16. OCHeaderOption 0
 *                  17. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCDoResource() using high quality of service for discovering resource
 *                     3. Check if ResourceDiscoveryReqCB callback is called and resource is found
 *                     4. Call OCDoResource() using high quality of service for sending observe request to the discovered resource
 *                     5. Check if obsReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback is called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, CreateResourceAndSendObserveRequestCON_CV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called");
        return;
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
        return;
    }

    RICsdkHelper::s_isCallback = false;

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_OBSERVE,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: observeReqCB callback is not called");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCStopMulticastServer and OCDoResource with positive way to check if resource is discovered after stopping multicast
 * @target OCStackResult OCStopMulticastServer()
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCDoHandle handle to returned
 *                     2. OCMethod OC_REST_DISCOVER
 *                     3. requestUri "/oic/res?rt=oic.r.temperature"
 *                     4. OCDevAddr destination address
 *                     5. OCPayload payload with the request
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_LOW_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCStopMulticastServer() API
 *                     3. Call OCDoResource() using low quality of service for discovering resource
 *                     4. Check if ResourceDiscoveryReqCB callback is called
 * @post_condition  Call OCStop()
 * @expected Callback should not be called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, StopMulticastAndDiscoverResource_SQV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCStopMulticastServer();
    ASSERT_EQ(OC_STACK_OK,m_result) << "Server: OCStopMulticastServer failed. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,TEMPERATURE_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == true)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback should not be called as multicast is stopped");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCProcess()
 * @objective Test OCBindResourceTypeToResource and OCDoResource with positive way to check if resource is found using the newly bound
 *         resource type
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *                                     const char *resourceTypeName,
 *                                     const char *resourceInterfaceName,
 *                                     const char *uri,
 *                                     OCEntityHandler entityHandler,
 *                                     void* callbackParam,
 *                                     uint8_t resourceProperties)
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @target OCStackResult OCBindResourceTypeToResource(OCResourceHandle handle,
 *                                                     const char *resourceTypeName)
 * @test_data         1. OCResourceHandle handle of the created resource
 *                     2. resourceTypeName "core.light"
 *                     1. OCDoHandle handle to returned
 *                     2. OCMethod OC_REST_DISCOVER
 *                     3. requestUri "/oic/res?rt=oic.r.temperature"
 *                     4. OCDevAddr destination address
 *                     5. OCPayload payload with the request
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_LOW_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition   1. Call OCInit() API
 *                     2. Call OCProcess() API
 * @procedure         1. Call OCCreateResource() API to to create resource
 *                     2. Call OCBindResourceTypeToResource to bind new resource type with resource
 *                     3. Call OCDoResource() using low quality of service for discovering resource
 *                     4. Check if ResourceDiscoveryReqCB callback is called and resource is found
 * @post_condition  Call OCStop()
 * @expected Callback should be called and resource should be found
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkIntegrationTest_stc, BindResourceTypeToResourceAndCheckIfResourceFoundUsingType_GSRV_P)
{
    m_resourceHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_resourceHandle,NULL) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResourceTypeToResource(m_resourceHandle, RESOURCE_TYPE_LIGHT);
    ASSERT_EQ(OC_STACK_OK,m_result) << "Server: OCBindResourceTypeToResource failed. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,LIGHT_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if(RICsdkHelper::s_isCallback == false)
    {
        SET_FAILURE("Client: ResourceDiscoveryReqCB callback is not called while finding using added resource type");
    }

    if(RICsdkHelper::s_isResourceFound == false)
    {
        SET_FAILURE("Client: Resource is not found");
    }
}
#endif
