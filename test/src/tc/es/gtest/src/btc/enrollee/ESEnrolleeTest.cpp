//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "ESEnrolleeHelper.h"
#include <gtest/gtest.h>
#include "CommonUtil.h"
#include <atomic>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include "easysetup.h"
#include <escommon.h>
#include "ESEnrolleeCommon.h"
#include <iostream>
using namespace std;


class ESEnrolleeTest_btc: public ::testing::Test
{
public:
    ESProvisioningCallbacks m_callbacks;
    ESEnrolleeHelper esEnrolleeHelper;
protected:
    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        m_callbacks.WiFiProvCb = &ESEnrolleeHelper::wiFiProvCbInApp;
        m_callbacks.DevConfProvCb = &ESEnrolleeHelper::devConfProvCbInApp;
        m_callbacks.CloudDataProvCb = &ESEnrolleeHelper::cloudDataProvCbInApp;
    }
    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2016-08-09
 * @objective Test 'ESInitEnrollee' function with positive basic way
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured false
 *            		2. resourceMask ES_WIFI_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned in non-secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESInitEnrolleeResourceMaskESWIFIRESOURCENonSecured_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callbacks);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-09
 * @objective Test 'ESInitEnrollee' function with positive basic way
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured true
 *            		2. resourceMask ES_WIFI_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned in secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESInitEnrolleeResourceMaskESWIFIRESOURCESecured_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callbacks);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-09
 * @objective Test 'ESInitEnrollee' function with positive basic way
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured false
 *            		2. resourceMask ES_CLOUD_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned in non-secured mode
 * @post_condition None close the apps
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESInitEnrolleeResourceMaskESCLOUDRESOURCENonSecured_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_CLOUD_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callbacks);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-09
 * @objective Test 'ESInitEnrollee' function with positive basic way
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured true
 *            		2. resourceMask ES_CLOUD_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned in secured mode
 * @post_condition None close the apps
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESInitEnrolleeResourceMaskESCLOUDRESOURCESecured_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_CLOUD_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callbacks);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-09
 * @objective Test 'ESInitEnrollee' function with positive basic way
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured false
 *            		2. resourceMask ES_DEVCONF_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned in nonsecured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESInitEnrolleeResourceMaskESDEVCONFRESOURCENonSecured_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callbacks);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-09
 * @objective Test 'ESInitEnrollee' function with positive basic way
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured true
 *            		2. resourceMask ES_DEVCONF_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned in secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESInitEnrolleeResourceMaskESDEVCONFRESOURCESecured_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callbacks);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-09
 * @objective Test 'ESInitEnrollee' function with positive basic way
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured false
 *            		2. resourceMask ALL
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned in non-secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESInitEnrolleeResourceMaskAllNonSecured_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callbacks);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-09
 * @objective Test 'ESInitEnrollee' function with positive basic way
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured true
 *            		2. resourceMask ALL
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned in secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESInitEnrolleeResourceMaskAllSecured_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callbacks);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-11
 * @objective Test 'ESInitEnrollee' function with positive basic way
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured false
 *            		2. resourceMask ES_WIFI_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESInitEnrolleeWorkWithAllCallback_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callbacks);
    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-11
 * @objective Test 'ESInitEnrollee' throughs error with null callback
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured true
 *            		2. resourceMask ES_WIFI_RESOURCE
 *            		3. WiFiProvcb Null
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_ERROR	is returned
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESInitEnrollee_WiFiProvCb_Null_NV_N)
{
    ESResult m_result = ES_OK;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);
    m_callbacks.WiFiProvCb = NULL;

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callbacks);

    if (m_result != ES_ERROR)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-11
 * @objective Test 'ESInitEnrollee' throughs error with null callback
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured false
 *            		2. resourceMask ES_WIFI_RESOURCE
 *            		3. DevConfProvCbInApp Null
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_ERROR	is returned
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESInitEnrolleeDevConfProvCbInApp_Null_NV_N)
{
    ESResult m_result = ES_OK;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);
    m_callbacks.DevConfProvCb = NULL;

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callbacks);
    if (m_result != ES_ERROR)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-11
 * @objective Test 'ESInitEnrollee' throughs error with null callback
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured true
 *            		2. resourceMask ES_WIFI_RESOURCE
 *            		3. CloudDataProvCbInApp Null
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_ERROR	is returned
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESInitEnrolleeCloudDataProvCbInApp_Null_NV_N)
{
    ESResult m_result = ES_OK;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);
    m_callbacks.CloudDataProvCb = NULL;

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callbacks);

    if (m_result != ES_ERROR)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-11
 * @objective Test 'ESSetDeviceProperty' works properly
 * @target ESResult ESSetDeviceProperty(ESDeviceProperty *deviceProperty)
 * @test_data 		1. isSecured false
 *            		2. resourceMask
 *            		3. Callback
 * @pre_condition None
 * @procedure Perform ESSetDeviceProperty and check if ES_OK is returned in non-secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESSetDevicePropertySetDevicePropertyNonSecured_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE); // | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callbacks);
    m_result = esEnrolleeHelper.setDeviceProperty();

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-11
 * @objective Test 'ESSetDeviceProperty' works properly
 * @target ESResult ESSetDeviceProperty(ESDeviceProperty *deviceProperty)
 * @test_data 		1. isSecured true
 *            		2. resourceMask
 *            		3. Callback
 * @pre_condition None
 * @procedure Perform ESSetDeviceProperty and check if ES_OK is returned in secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESSetDevicePropertySetDevicePropertySecured_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE); // | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callbacks);
    m_result = esEnrolleeHelper.setDeviceProperty();

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-11
 * @objective Test 'ESSetState' works properly
 * @target ESResult ESResult ESSetState(ESEnrolleeState esState)
 * @test_data 		1. isSecured false
 *            		2. resourceMask
 *            		3. Callback
 * @pre_condition None
 * @procedure Perform ESSetState and check if ES_OK is returned in non-secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESSetStateSetStateNonSecured_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callbacks);
    m_result = ESSetState(ES_STATE_CONNECTED_TO_ENROLLER);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-11
 * @objective Test 'ESSetState' works properly
 * @target ESResult ESResult ESSetState(ESEnrolleeState esState)
 * @test_data 		1. isSecured true
 *            		2. resourceMask
 *            		3. Callback
 * @pre_condition None
 * @procedure Perform ESSetState and check if ES_OK is returned in secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESSetStateSetStateSecured_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callbacks);
    m_result = ESSetState(ES_STATE_CONNECTED_TO_ENROLLER);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif
/**
 * @since 2016-08-11
 * @objective Test 'ESSetErrorCode' works properly
 * @target ESResult ESSetErrorCode(ESErrorCode esErrCode)
 * @test_data 		1. isSecured false
 *            		2. resourceMask
 *            		3. Callback
 * @pre_condition None
 * @procedure Perform ESSetErrorCode and check if ES_OK is returned
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESSetErrorCodeSetErrorCodeWithSuccess_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callbacks);
    m_result = ESSetErrorCode(ES_ERRCODE_PW_WRONG);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-11
 * @objective Test 'ESSetErrorCode' works properly
 * @target ESResult ESSetErrorCode(ESErrorCode esErrCode)
 * @test_data 		1. isSecured false
 *            		2. resourceMask
 *            		3. Callback
 * @pre_condition None
 * @procedure Perform ESSetErrorCode and check if ES_OK is returned
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESSetErrorCodeSetErrorCodeUnknownWithSuccess_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callbacks);
    m_result = ESSetErrorCode(ES_ERRCODE_UNKNOWN);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-11
 * @objective Test 'ESSetErrorCode' return error with invalid input
 * @target ESResult ESSetErrorCode(ESErrorCode esErrCode)
 * @test_data 		1. isSecured false
 *            		2. resourceMask
 *            		3. Callback
 * @pre_condition None
 * @procedure Perform ESSetErrorCode and check if ES_ERROR is returned
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESSetErrorCodeSetErrorEmptyCode_USV_N)
{
    ESResult m_result = ES_OK;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callbacks);
    m_result = ESSetErrorCode(100);

    if (m_result != ES_ERROR)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-11
 * @objective Test 'ESTerminateEnrollee()' works properly
 * @target ESResult ESTerminateEnrollee()
 * @test_data
 * @pre_condition None
 * @procedure Perform ESTerminateEnrollee and check if ES_OK is returned
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_btc, ESTerminateEnrolleeSuccess_SRC_P)
{
    ESResult m_result = ES_ERROR;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);
    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    ESInitEnrollee(s_nonSecured, resourcemMask, m_callbacks);
    m_result = ESTerminateEnrollee();

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. s_result is: " + std::to_string(m_result));
    }
}
#endif

