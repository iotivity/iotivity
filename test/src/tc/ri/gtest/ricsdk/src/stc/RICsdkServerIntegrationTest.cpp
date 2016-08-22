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

class RICsdkServerIntegrationTest_stc: public ::testing::Test
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

        m_result = m_pRICsdkHelper->initServer();
        ASSERT_EQ(OC_STACK_OK,m_result)<< "OCInit failed for server. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
        OCStop();
    }

    static OCEntityHandlerResult OCEntityHandlerCb (OCEntityHandlerFlag flag,
            OCEntityHandlerRequest *entityHandlerRequest, void* /*callback*/)
    {
        return OC_EH_OK;
    }
};

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCCreateResource and OCGetResourceProperties with positive way to check if set property is received
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @target OCResourceProperty OCGetResourceProperties(OCResourceHandle handle)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "core.light"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/light"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                     8. OCResourceHandle handle pointer to the created resource for getting resource properties
 * @pre_condition Call OCInit() API
 * @procedure         1. Call OCCreateResource() API
 *                     2. Call OCGetResourceProperties() API
 *                     3. Check if received property matches with the set property while creating resource
 * @post_condition Call OCStop()
 * @expected Returned resource property matches with the set property
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerIntegrationTest_stc, CreateResourceAndGetResourceProperties_GSRV_P)
{
    m_handle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_TEMPERATURE,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_TEMPERATURE);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    OCResourceProperty receivedProperty = NULL;
    receivedProperty = OCGetResourceProperties(m_handle);
    ASSERT_NE(NULL,receivedProperty) << "Server: OCGetResourceProperties failed. Received property is null";
    ASSERT_EQ(7,receivedProperty) << "Server: Set Resource Property is not Received";
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
 * @objective Test OCBindResource and OCGetResourceHandleFromCollection with positive way to check if returned handle
 *         from OCGetResourceHandleFromCollection matches with the set handle in OCBindResource
 * @target OCStackResult OCBindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
 * @target OCResourceHandle OCGetResourceHandleFromCollection(OCResourceHandle collectionHandle,
 *        uint8_t index)
 * @test_data         1. collectionHandle pointer to the collection resource
 *                     2. resourceHandle pointer to the created resource
 *                     3. collectionHandle pointer to the collection resource for getting resource handle
 *                     4. index 0
 * @pre_condition     1. Call OCInit() API
 *                     2. Call OCCreateResource() API
 * @procedure         1. Call OCBindResource() API
 *                     2. Call OCGetResourceHandleFromCollection() API
 * @post_condition Call OCStop()
 * @expected Received resource handle should match with the created resource's handle
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerIntegrationTest_stc, GetResourceHandleFromCollectionAndCheckIfAddedHandleIsFound_GSRV_P)
{
    OCResourceHandle m_lightHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_lightHandle,NULL) << "Light Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    OCResourceHandle m_fanHandle = m_pRICsdkHelper->createResource(RESOURCE_TYPE_FAN,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_FAN);
    ASSERT_NE(m_fanHandle,NULL) << "Fan Resource was not created. " + m_pRICsdkHelper->getFailureMessage();

    m_result = OCBindResource(m_fanHandle, m_lightHandle);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCBindResource failed. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);

    OCResourceHandle receivedHandle = NULL;
    uint8_t index = 0;
    receivedHandle = OCGetResourceHandleFromCollection(m_fanHandle,index);
    ASSERT_NE(NULL,receivedHandle) << "OCGetResourceHandleFromCollection failed. Received handle is null";
    ASSERT_EQ(m_lightHandle,receivedHandle) << "Received Handle does not match with the created resource";
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCCreateResource and OCGetResourceTypeName with positive way
 *         to check if created resource's resource type name matches with the get one
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @target const char *OCGetResourceTypeName(OCResourceHandle handle, uint8_t index)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "core.light"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/light"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                     8. OCResourceHandle pointer to the created resource for getting resource type name
 *                     9. index 0
 * @pre_condition     1. Call OCInit() API
 * @procedure         1. Call OCCreateResource() API
 *                     2. Call OCGetResourceTypeName() API
 *                     3. Check if received resource type name matches with the created resource's resource type name
 * @post_condition Call OCStop()
 * @expected Two resource type names should match
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerIntegrationTest_stc, CreateResourceAndGetResourceTypeName_GSRV_P)
{
    const char* addedResourceType = "core.light1";
    m_handle = m_pRICsdkHelper->createResource(addedResourceType,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT);
    ASSERT_NE(m_handle,NULL) << m_pRICsdkHelper->getFailureMessage();

    uint8_t index = 0;
    const char* receivedResourceType = NULL;
    receivedResourceType = OCGetResourceTypeName(m_handle,index);
    ASSERT_NE(NULL,receivedResourceType) << "OCGetResourceTypeName failed. Received resource type is null";
    if (strcmp(receivedResourceType, addedResourceType) != 0)
    {
        SET_FAILURE("Received resource type does match with the created resource's resource type");
    }
}
#endif

/**
 * @since 2016-07-19
 * @see OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see OCStackResult OCStop()
 * @objective Test OCCreateResource and OCGetResourceHandler with positive way to
 *         check if created resource handler matches with get resource handler
 * @target OCStackResult OCCreateResource(OCResourceHandle *handle,
 *       const char *resourceTypeName,
 *       const char *resourceInterfaceName,
 *       const char *uri,
 *       OCEntityHandler entityHandler,
 *       void* callbackParam,
 *       uint8_t resourceProperties)
 * @target OCEntityHandler OCGetResourceHandler(OCResourceHandle handle)
 * @test_data         1. OCResourceHandle pointer to the created resource
 *                     2. resourceTypeName "core.light"
 *                     3. resourceInterfaceName "oc.if.a"
 *                     4. uri "/a/light"
 *                     5. entityHandler entity handler to be called
 *                     6. callbackParam null
 *                     7. resourceProperties OC_DISCOVERABLE|OC_OBSERVABLE
 *                     8. OCResourceHandle pointer to the created resource for getting resource handler
 * @pre_condition Call OCInit() API
 * @procedure         1. Call OCCreateResource() API
 *                     2. Call OCGetResourceHandler() API
 *                     3. Check if received resource handler matches with the used resource handler in OCCreateResource
 * @post_condition Call OCStop()
 * @expected Two entity handlers should match
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICsdkServerIntegrationTest_stc, CreateResourceAndGetResourceHandler_SRC_P)
{
    m_result = OCCreateResource(&m_handle,RESOURCE_TYPE_LIGHT,RESOURCE_INTERFACE_DEFAULT,RESOURCE_URI_LIGHT,OCEntityHandlerCb,NULL,
            OC_DISCOVERABLE|OC_OBSERVABLE);
    ASSERT_EQ(OC_STACK_OK,m_result) << "OCCreateResource failed. Actual m_result : " << CommonUtil::s_OCStackResultString.at(m_result);
    ASSERT_NE(m_handle,NULL) << "Resource handle is Null";

    OCEntityHandler entityHandler = NULL;
    entityHandler = OCGetResourceHandler(m_handle);
    ASSERT_NE(NULL,entityHandler) << "OCGetResourceHandler failed. Received entity handler is null";
    ASSERT_EQ(OCEntityHandlerCb,entityHandler) << "Received entity handler does not match with the set entity handler";
}
#endif
