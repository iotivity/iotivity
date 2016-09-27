//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

/**
 * @file
 *
 * This file contains the implementation for EasySetup Enrollee device
 */
#include "ESEnrolleeHelper.h"
#include <gtest/gtest.h>
#include "CommonUtil.h"
#include <atomic>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "easysetup.h"
#include <escommon.h>
#include "ESEnrolleeCommon.h"
#include <iostream>
#include <pthread.h>
using namespace std;

class ESEnrolleeTest_stc: public ::testing::Test
{
public:
    ESProvisioningCallbacks m_callBacks;
    ESEnrolleeHelper m_esEnrolleeHelper;

protected:

    virtual void SetUp()
    {
        OCStackResult r = OC_STACK_ERROR;
        CommonUtil::runCommonTCSetUpPart();
        m_callBacks.WiFiProvCb = &m_esEnrolleeHelper.wiFiProvCbInApp;
        m_callBacks.DevConfProvCb = &m_esEnrolleeHelper.devConfProvCbInApp;
        m_callBacks.CloudDataProvCb = &m_esEnrolleeHelper.cloudDataProvCbInApp;

    }
    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since 2016-08-19
 * @objective Test 'ESInitEnrollee' invoke callback for wifi resource
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured false
 *            		2. resourceMask ES_WIFI_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned and wifi prov callback is ivoked in non-secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_stc, ESInitEnrolleeResourceMaskESWIFIRESOURCENonSecuredSuccessCallBack_SRC_P)
{

    ESResult m_result;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }


    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callBacks);

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. m_result is: " + std::to_string(m_result));
        return;
    }

     m_esEnrolleeHelper.waitForResponse();

    if (!ESEnrolleeHelper::isCallbackInvoked)
    {
        SET_FAILURE("Failed to invoke callback. result is: " + std::to_string(ESEnrolleeHelper::isCallbackInvoked));
    }
}
#endif

/**
 * @since 2016-08-23
 * @objective Test 'ESInitEnrollee' invoke callback for wifi resource
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioninm_callBacks callbacks);
 * @test_data 		1. isSecured true
 *            		2. resourceMask ES_WIFI_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned and wifi prov callback is ivoked in secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_stc, ESInitEnrolleeResourceMaskESWIFIRESOURCESecuredSuccessCallBack_SRC_P)
{
    ESResult m_result;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callBacks);
    m_esEnrolleeHelper.waitForResponse();
    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. m_result is: " + std::to_string(m_result));
        return;
    }
    
    if (!ESEnrolleeHelper::isCallbackInvoked)
    {
        SET_FAILURE("Failed to invoke callback. m_result is: " + std::to_string(ESEnrolleeHelper::isCallbackInvoked));
    }
}
#endif

/**
 * @since 2016-08-19
 * @objective Test 'ESInitEnrollee' invoke callback for cloud resource
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured false
 *            		2. resourceMask ES_CLOUD_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned and CloudDataProv Cb is ivoked in non-secured mode
 * @post_condition None close the apps
 * @expected ES_OK is returned
 **/

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_stc, ESInitEnrolleeResourceMaskESCLOUDRESOURCENonSecuredSuccessCallBack_SRC_P)
{
    ESResult m_result;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_CLOUD_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callBacks);
    m_esEnrolleeHelper.waitForResponse();
    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. m_result is: " + std::to_string(m_result));
        return;
    }

    if (!ESEnrolleeHelper::isCallbackInvoked)
    {
        SET_FAILURE("Failed to invoke callback. m_result is: " + std::to_string(ESEnrolleeHelper::isCallbackInvoked));
    }
}
#endif

/**
 * @since 2016-08-23
 * @objective Test 'ESInitEnrollee' invoke callback for cloud resource
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured true
 *            		2. resourceMask ES_CLOUD_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned and CloudDataProv Cb is ivoked in secured mode
 * @post_condition None close the apps
 * @expected ES_OK is returned
 **/

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_stc, ESInitEnrolleeResourceMaskESCLOUDRESOURCESecuredSuccessCallBack_SRC_P)
{
    ESResult m_result;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_CLOUD_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callBacks);
    m_esEnrolleeHelper.waitForResponse();
    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. m_result is: " + std::to_string(m_result));
        return;
    }

    if (!ESEnrolleeHelper::isCallbackInvoked)
    {
        SET_FAILURE("Failed to invoke callback. m_result is: " + std::to_string(ESEnrolleeHelper::isCallbackInvoked));
    }
}
#endif

/**
 * @since 2016-08-19
 * @objective Test 'ESInitEnrollee' Invoke devconf collback
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured false
 *            		2. resourceMask ES_DEVCONF_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned and devconf Cb is ivoked in non-secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_stc, ESInitEnrolleeResourceMaskESDEVCONFRESOURCENonSecuredSuccessCallback_SRC_P)
{
    ESResult m_result;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callBacks);
    m_esEnrolleeHelper.waitForResponse();
    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. m_result is: " + std::to_string(m_result));
        return;
    }

    if (!ESEnrolleeHelper::isCallbackInvoked)
    {
        SET_FAILURE("Failed to invoke callback. m_result is: " + std::to_string(ESEnrolleeHelper::isCallbackInvoked));
    }
}
#endif

/**
 * @since 2016-08-23
 * @objective Test 'ESInitEnrollee' Invoke devconf collback
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured true
 *            		2. resourceMask ES_DEVCONF_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned and devconf Cb is ivoked in secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_stc, ESInitEnrolleeResourceMaskESDEVCONFRESOURCESecuredSuccessCallback_SRC_P)
{
    ESResult m_result;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callBacks);
    m_esEnrolleeHelper.waitForResponse();
    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. m_result is: " + std::to_string(m_result));
        return;
    }

    if (!ESEnrolleeHelper::isCallbackInvoked)
    {
        SET_FAILURE("Failed to invoke callback. m_result is: " + std::to_string(ESEnrolleeHelper::isCallbackInvoked));
    }
}
#endif

/**
 * @since 2016-08-19
 * @objective Test 'ESInitEnrollee' invoke all callback
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured false
 *            		2. resourceMask ALL
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned and invoke all cb in non-secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_stc, ESInitEnrolleeResourceMaskAllNonSecuredSuccesscallback_SRC_P)
{
    ESResult m_result;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callBacks);
    m_esEnrolleeHelper.waitForResponse();
    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. m_result is: " + std::to_string(m_result));
        return;
    }

    if (!ESEnrolleeHelper::isCallbackInvoked)
    {
        SET_FAILURE("Failed to invoke callback. m_result is: " + std::to_string(ESEnrolleeHelper::isCallbackInvoked));
    }
}
#endif

/**
 * @since 2016-08-19
 * @objective Test 'ESInitEnrollee' invoke all cb with secure mode
 * @target ESResult ESInitEnrollee(bool isSecured, ESResourceMask resourceMask, ESProvisioningCallbacks callbacks);
 * @test_data 		1. isSecured true
 *            		2. resourceMask ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE
 *            		3. callbacks
 * @pre_condition None
 * @procedure Perform ESInitEnrollee and check if ES_OK	is returned and invoke all cb in secured mode
 * @post_condition None
 * @expected ES_OK is returned
 **/

#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_stc, ESInitEnrolleeResourceMaskAllSecuredSuccessCallBack_SRC_P)
{
    ESResult m_result;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callBacks);
    m_esEnrolleeHelper.waitForResponse();
    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. m_result is: " + std::to_string(m_result));
        return;
    }

    if (!ESEnrolleeHelper::isCallbackInvoked)
    {
        SET_FAILURE("Failed to invoke callback. m_result is: " + std::to_string(ESEnrolleeHelper::isCallbackInvoked));
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
 * @procedure Perform ESSetDeviceProperty and check if ES_OK is returned
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_stc, ESSetDevicePropertySetDevicePropertyWithSuccessSecured_SRC_P)
{
    ESResult m_result;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_secured, resourcemMask, m_callBacks);
    m_result = m_esEnrolleeHelper.setDeviceProperty();
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
 * @test_data 		1. isSecured false
 *            		2. resourceMask
 *            		3. Callback
 * @pre_condition None
 * @procedure Perform ESSetDeviceProperty and check if ES_OK is returned
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_stc, ESSetDevicePropertySetDevicePropertyWithSuccessNonSecured_SRC_P)
{
    ESResult m_result;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callBacks);
    m_result = m_esEnrolleeHelper.setDeviceProperty();
    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. result is: " + std::to_string(m_result));
    }
}
#endif

/**
 * @since 2016-08-23
 * @objective Test 'ESTerminateEnrollee()' terminate Provisioning and Network resources successfully
 * @target ESResult ESTerminateEnrollee()
 * @test_data 		1. isSecured false
 *            		2. resourceMask
 *            		3. Callback
 * @pre_condition None
 * @procedure Perform ESTerminateEnrollee and check if ES_OK is returned, also checked all provisioning
 * and network resource terminate properly
 * @post_condition None
 * @expected ES_OK is returned
 **/
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESEnrolleeTest_stc, ESTerminateEnrolleeWorkProperly_SRC_N)
{
    ESResult m_result;
    ESResourceMask resourcemMask = (ESResourceMask)(ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE);

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        SET_FAILURE("OCStack init error!!\n");
        return;
    }
    m_result = ESInitEnrollee(s_nonSecured, resourcemMask, m_callBacks);
    m_result = ESTerminateEnrollee();
    //m_esEnrolleeHelper.waitForResponse();

    if (m_result != ES_OK)
    {
        SET_FAILURE("Failed to find resources. m_result is: " + std::to_string(m_result));
        return;
    }

    if (ESEnrolleeHelper::isCallbackInvoked)
    {
        SET_FAILURE("Failed to invoke callback. m_result is: " + std::to_string(ESEnrolleeHelper::isCallbackInvoked));
    }

}
#endif
