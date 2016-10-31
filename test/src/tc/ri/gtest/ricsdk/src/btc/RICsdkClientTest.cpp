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

class RICsdkClientTest_btc: public ::testing::Test
{
public:
    RICsdkHelper *m_pRICsdkHelper;
    OCMode m_ClientMode = OC_CLIENT;
    OCTransportFlags m_DefaultTransportFlags = OC_DEFAULT_FLAGS;
    OCStackResult m_result;
    OCCallbackData m_cbData;
    OCDoHandle m_doHandle;
    OCResourceHandle m_resourceHandle;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        RICsdkHelper::s_isBtc = true;

        m_pRICsdkHelper = RICsdkHelper::getInstance();
        m_result = OC_STACK_ERROR;
        m_cbData.cb = DeviceDiscoveryReqCB;
        m_cbData.context = (void*) 0x99;
        m_cbData.cd = NULL;
        m_doHandle = NULL;

        m_result = m_pRICsdkHelper->initClientServer();
        ASSERT_EQ(OC_STACK_OK,m_result)<< "OCInit failed for client-server. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
        m_pRICsdkHelper->startOCProcess();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }

    static OCStackApplicationResult DeviceDiscoveryReqCB(void* ctx, OCDoHandle /*m_doHandle*/,
            OCClientResponse * clientResponse)
    {

    }
};

/**
 * @since 2016-07-19
 * @see none
 * @objective Test OCInit with positive basic way
 * @target OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @test_data    1. ipAddr null
 *                 2. port 0
 *                 3. mode client mode
 * @pre_condition none
 * @procedure Call OCInit() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCInitClient_SRC_P)
{
    m_result = m_pRICsdkHelper->initClient();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for client. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see none
 * @objective Test OCInit1 with positive basic way
 * @target OCStackResult OCInit1(OCMode mode, OCTransportFlags serverFlags,
 *         OCTransportFlags clientFlags)
 * @test_data    1. mode OCMode server mode
 *                 2. serverFlags Default server transport flags
 *                 3. clientFlags Default client transport flags
 * @pre_condition none
 * @procedure Call OCInit1() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCInit1Client_SRC_P)
{
    m_result = OCInit1(m_ClientMode, m_DefaultTransportFlags, m_DefaultTransportFlags);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit1 failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for device discovery using low quality of service
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
 *                     3. requestUri "/oic/d"
 *                     4. OCDevAddr destination address
 *                     5. OCPayload payload with the request
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_LOW_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition Call OCInit() API
 * @procedure Call OCDoResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourceDiscoverDeviceNON_SRC_P)
{
    m_result = m_pRICsdkHelper->initClient();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for client. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,DEVICE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for device discovery using high quality of service
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
 *                     3. requestUri "/oic/d"
 *                     4. OCDevAddr destination address
 *                     5. OCPayload payload with the request
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_HIGH_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition Call OCInit() API
 * @procedure Call OCDoResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourceDiscoverDeviceCON_SRC_P)
{
    m_result = m_pRICsdkHelper->initClient();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for client. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,DEVICE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for platform discovery using low quality of service
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
 *                     3. requestUri "/oic/p"
 *                     4. OCDevAddr destination address
 *                     5. OCPayload payload with the request
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_LOW_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition Call OCInit() API
 * @procedure Call OCDoResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourceDiscoverPlatformNON_SRC_P)
{
    m_result = m_pRICsdkHelper->initClient();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for client. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,PLATFORM_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for platform discovery using high quality of service
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
 *                     3. requestUri "/oic/p"
 *                     4. OCDevAddr destination address
 *                     5. OCPayload payload with the request
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_HIGH_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition Call OCInit() API
 * @procedure Call OCDoResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourceDiscoverPlatformCON_SRC_P)
{
    m_result = m_pRICsdkHelper->initClient();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for client. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,PLATFORM_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for sending get request using low quality of service
 * @target OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *m_cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @test_data         1. OCDoHandle handle to returned
 *                     2. OCMethod OC_REST_GET
 *                     3. requestUri "/oic/res"
 *                     3. OCDevAddr destination address
 *                     4. OCPayload payload with the request
 *                     5. OCConnectivityType CT_DEFAULT
 *                     6. OCQualityOfService OC_LOW_QOS
 *                     7. OCCallbackData callback data
 *                     8. OCHeaderOption 0
 *                  9. numOptions 0
 * @pre_condition     1. Call OCInit() API
 *                    2. Call OCDoResource() API for discovering resource
 *                    3. Check if resource is found
 * @procedure Call OCDoResource() API to send get request
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourceGetRequestNON_SRC_P)
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

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_GET,ALL_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for sending get request using high quality of service
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
 *                     2. OCMethod OC_REST_GET
 *                     3. requestUri "/oic/res"
 *                     4. OCDevAddr destination address
 *                     5. OCPayload payload with the request
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_HIGH_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition     1. Call OCInit() API
 *                    2. Call OCDoResource() API for discovering resource
 *                    3. Check if resource is found
 * @procedure Call OCDoResource() API to send get request
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourceGetRequestCON_SRC_P)
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

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_GET,ALL_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for sending put request using low quality of service
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
 *                     2. OCMethod OC_REST_PUT
 *                     3. requestUri "/oic/res"
 *                     3. OCDevAddr destination address
 *                     4. OCPayload payload with the request
 *                     5. OCConnectivityType CT_DEFAULT
 *                     6. OCQualityOfService OC_LOW_QOS
 *                     7. OCCallbackData callback data
 *                     8. OCHeaderOption 0
 *                  9. numOptions 0
 * @pre_condition     1. Call OCInit() API
 *                    2. Call OCDoResource() API for discovering resource
 *                    3. Check if resource is found
 * @procedure Call OCDoResource() API to send put request
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourcePutRequestNON_SRC_P)
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

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_PUT,ALL_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for sending put request using high quality of service
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
 *                     2. OCMethod OC_REST_PUT
 *                     3. requestUri "/oic/res"
 *                     4. OCDevAddr destination address
 *                     5. OCPayload payload with the request
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_HIGH_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition     1. Call OCInit() API
 *                    2. Call OCDoResource() API for discovering resource
 *                    3. Check if resource is found
 * @procedure Call OCDoResource() API
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourcePutRequestCON_SRC_P)
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

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_PUT,ALL_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for sending post request using low quality of service
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
 *                     2. OCMethod OC_REST_POST
 *                     3. requestUri "/oic/res"
 *                     3. OCDevAddr destination address
 *                     4. OCPayload payload with the request
 *                     5. OCConnectivityType CT_DEFAULT
 *                     6. OCQualityOfService OC_LOW_QOS
 *                     7. OCCallbackData callback data
 *                     8. OCHeaderOption 0
 *                  9. numOptions 0
 * @pre_condition     1. Call OCInit() API
 *                    2. Call OCDoResource() API for discovering resource
 *                    3. Check if resource is found
 * @procedure Call OCDoResource() API to send post request
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourcePostRequestNON_SRC_P)
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

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_POST,ALL_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for sending post request using high quality of service
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
 *                     2. OCMethod OC_REST_POST
 *                     3. requestUri "/oic/res"
 *                     4. OCDevAddr destination address
 *                     5. OCPayload payload with the request
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_HIGH_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition     1. Call OCInit() API
 *                    2. Call OCDoResource() API for discovering resource
 *                    3. Check if resource is found
 * @procedure Call OCDoResource() API to send post request
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourcePostRequestCON_SRC_P)
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

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_POST,ALL_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for sending delete request using low quality of service
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
 *                     2. OCMethod OC_REST_DELETE
 *                     3. requestUri "/oic/res"
 *                     3. OCDevAddr destination address
 *                     4. OCPayload payload with the request
 *                     5. OCConnectivityType CT_DEFAULT
 *                     6. OCQualityOfService OC_LOW_QOS
 *                     7. OCCallbackData callback data
 *                     8. OCHeaderOption 0
 *                  9. numOptions 0
 * @pre_condition     1. Call OCInit() API
 *                    2. Call OCDoResource() API for discovering resource
 *                    3. Check if resource is found
 * @procedure Call OCDoResource() API to send delete request
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourceDeleteRequestNON_SRC_P)
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

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DELETE,ALL_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for sending delete request using high quality of service
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
 *                     2. OCMethod OC_REST_DELETE
 *                     3. requestUri "/oic/res"
 *                     4. OCDevAddr destination address
 *                     5. OCPayload payload with the request
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_HIGH_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition     1. Call OCInit() API
 *                    2. Call OCDoResource() API for discovering resource
 *                    3. Check if resource is found
 * @procedure Call OCDoResource() API to send delete request
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourceDeleteRequestCON_SRC_P)
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

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DELETE,ALL_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for sending observe request using low quality of service
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
 *                     2. OCMethod OC_REST_OBSERVE
 *                     3. requestUri "/oic/res"
 *                     3. OCDevAddr destination address
 *                     4. OCPayload payload with the request
 *                     5. OCConnectivityType CT_DEFAULT
 *                     6. OCQualityOfService OC_LOW_QOS
 *                     7. OCCallbackData callback data
 *                     8. OCHeaderOption 0
 *                  9. numOptions 0
 * @pre_condition     1. Call OCInit() API
 *                    2. Call OCDoResource() API for discovering resource
 *                    3. Check if resource is found
 * @procedure Call OCDoResource() API to send observe request
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourceObserveRequestNON_SRC_P)
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

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_OBSERVE,ALL_RESOURCE_DISCOVERY_QUERY,OC_LOW_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for sending observe request using high quality of service
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
 *                     2. OCMethod OC_REST_OBSERVE
 *                     3. requestUri "/oic/res"
 *                     4. OCDevAddr destination address
 *                     5. OCPayload payload with the request
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_HIGH_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition     1. Call OCInit() API
 *                    2. Call OCDoResource() API for discovering resource
 *                    3. Check if resource is found
 * @procedure Call OCDoResource() API to send observe request
 * @post_condition None
 * @expected Should return OC_STACK_OK and handle should be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourceObserveRequestCON_SRC_P)
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

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_OBSERVE,ALL_RESOURCE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with negative basic way for device discovery using null payload
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
 *                     3. requestUri "/oic/d"
 *                     4. OCDevAddr destination address
 *                     5. OCPayload null
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_HIGH_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition Call OCInit() API
 * @procedure Call OCDoResource() API
 * @post_condition None
 * @expected Should return OC_STACK_INVALID_CALLBACK and handle should not be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourceDiscoverDeviceCallbackData_NV_N)
{
    m_result = m_pRICsdkHelper->initClient();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for client. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    OCDoHandle handle;
    m_result = OCDoResource(&handle, OC_REST_DISCOVER, DEVICE_DISCOVERY_QUERY, 0, NULL, CT_DEFAULT, OC_LOW_QOS, NULL, 0, 0);
    ASSERT_EQ(OC_STACK_INVALID_CALLBACK,m_result) << "OCDoResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective Test OCDoResource with positive basic way for device discovery using empty request uri
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
 *                     3. requestUri ""
 *                     4. OCDevAddr destination address
 *                     5. OCPayload null
 *                     6. OCConnectivityType CT_DEFAULT
 *                     7. OCQualityOfService OC_HIGH_QOS
 *                     8. OCCallbackData callback data
 *                     9. OCHeaderOption 0
 *                  10. numOptions 0
 * @pre_condition Call OCInit() API
 * @procedure Call OCDoResource() API
 * @post_condition None
 * @expected Should return OC_STACK_INVALID_URI and handle should not be returned
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCDoResourceDiscoverDeviceRequestUri_ESV_N)
{
    m_result = m_pRICsdkHelper->initClient();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for client. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    OCDoHandle handle;
    m_result = OCDoResource(&handle, OC_REST_DISCOVER, EMPTY_STRING, 0, 0, CT_DEFAULT, OC_LOW_QOS, &m_cbData, 0, 0);
    ASSERT_EQ(OC_STACK_INVALID_URI,m_result) << "OCDoResource failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @objective Test OCCancel with positive basic way
 * @target OCStackResult OCCancel(OCDoHandle handle,
 *                                 OCQualityOfService qos,
 *                                 OCHeaderOption * options,
 *                                 uint8_t numOptions)
 * @test_data         1.OCDoHandle Used to identify a specific OCDoResource invocation.
 *                     2.OCQualityOfService OC_HIGH_QOS
 *                     3.OCHeaderOption 0
 *                     4.numOptions 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCDoResource() API
 * @procedure Call OCCancel() API
 * @post_condition None
 * @expected Should return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCCancel_SRC_P)
{
    m_result = m_pRICsdkHelper->initClient();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for client. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,DEVICE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCCancel(m_doHandle,OC_HIGH_QOS,0,0);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCCancel failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCDoResource(OCDoHandle *handle,
 *                                    OCMethod method,
 *                                    const char *requestUri,
 *                                    const OCDevAddr *destination,
 *                                    OCPayload* payload,
 *                                    OCConnectivityType connectivityType,
 *                                    OCQualityOfService qos,
 *                                    OCCallbackData *cbData,
 *                                    OCHeaderOption *options,
 *                                    uint8_t numOptions)
 * @objective Test OCCancel with negative basic way using null doHandle
 * @target OCStackResult OCCancel(OCDoHandle handle,
 *                                 OCQualityOfService qos,
 *                                 OCHeaderOption * options,
 *                                 uint8_t numOptions)
 * @test_data         1.OCDoHandle null
 *                     2.OCQualityOfService OC_HIGH_QOS
 *                     3.OCHeaderOption 0
 *                     4.numOptions 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCDoResource() API
 * @procedure Call OCCancel() API
 * @post_condition None
 * @expected Should return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkClientTest_btc, OCCancelDoHandle_NV_N)
{
    m_result = m_pRICsdkHelper->initClient();
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCInit failed for client. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);

    m_doHandle = m_pRICsdkHelper->doResource(OC_REST_DISCOVER,DEVICE_DISCOVERY_QUERY,OC_HIGH_QOS);
    ASSERT_NE(NULL,m_doHandle) << m_pRICsdkHelper->getFailureMessage();

    m_result = OCCancel(NULL,OC_HIGH_QOS,0,0);
    ASSERT_EQ(OC_STACK_INVALID_PARAM,m_result) << "OCCancel failed. Actual result : " << CommonUtil::s_OCStackResultString.at(m_result);
}
#endif
