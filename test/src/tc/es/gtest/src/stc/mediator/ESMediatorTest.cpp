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

#include <gtest/gtest.h>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include "escommon.h"
#include "ESRichCommon.h"
#include "EasySetup.h"
#include "RemoteEnrollee.h"
#include <iostream>
#include "ESEnrolleeCommon.h"
#include "easysetup.h"
#include "OCPlatform.h"
#include "ESMediatorHelper.h"
using namespace OC;
using namespace OIC::Service;

typedef void (*Runner)();

Runner g_currentRun;
ESMediatorHelper m_esMediatorHelper;
class ESProvisionDevicePropertiesTest_stc: public ::testing::Test
{

public:
    ESProvisionDevicePropertiesTest_stc() = default;
    ~ESProvisionDevicePropertiesTest_stc() = default;

    ESMediatorHelper esMediatorHelper;

protected:
    void SetUp()
    {

    }

    void TearDown()
    {

    }
};

/**
 * @since 2016-08-24
 * @objective test 'provisionDeviceProperties' positively
 * @target public void provisionDeviceProperties(DeviceProp deviceProp,
 *         DevicePropProvisioningCallback callback) throws ESException
 * @see public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
 *      authtype, WIFI_ENCTYPE enctype)
 * @see public void setDevConfProp(String language, String country)
 * @see public synchronized static EasySetup getInstance(Context context)
 * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
 *      enrolleeResource)
 *
 *
 * @test_data deviceProp DevicePropProvisioningCallback
 * @pre_condition 1.create remote enrollee 2.cretae deviceprop
 * @procedure 1. Call provisionDeviceProperties API.
 * @post_condition none
 * @expected provisionDeviceProperties call without exception
 *
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESProvisionDevicePropertiesTest_stc, ProvisionDeviceProperties_RSV_P)
{
    ESMediatorHelper::s_isDevicePropProvCallbackCalled = false;
    ESMediatorHelper::s_isProvisionDevicePropSuccess = false;

    m_esMediatorHelper.findEnrolleeResource();
    m_esMediatorHelper.provisionDeviceProperty();

    ASSERT_TRUE(ESMediatorHelper::s_isDevicePropProvCallbackCalled)<<"PropProvCallback Called failed";
    ASSERT_TRUE(ESMediatorHelper::s_isProvisionDevicePropSuccess)<<"Provision Device Prop Success Called failed";

}
#endif

/**
 * @since 2016-08-24
 * @see public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
 *      authtype, WIFI_ENCTYPE enctype)
 * @see public void setDevConfProp(String language, String country)
 * @see public synchronized static EasySetup getInstance()
 * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
 *      enrolleeResource)
 * @objective test getConfiguration positively
 * @target public void provisionDeviceProperties(DeviceProp deviceProp,
 *         DevicePropProvisioningCallback callback) throws ESException
 * @test_data deviceProp DevicePropProvisioningCallback
 * @pre_condition 1.create remote enrollee 2.cretae deviceprop
 * @procedure 1. Call getConfiguration API.
 * @post_condition none
 * @expected getConfiguration call without exception
 *
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESProvisionDevicePropertiesTest_stc, GetConfigDeviceProperties_RSV_P)
{
    ESMediatorHelper::s_isDeviceConfigCallbackCalled = false;
    ESMediatorHelper::s_isConfigDevicePropSuccess = false;

    m_esMediatorHelper.findEnrolleeResource();
    m_esMediatorHelper.getConfiguration();

    ASSERT_TRUE(ESMediatorHelper::s_isDeviceConfigCallbackCalled)<<"s_isDeviceConfigCallbackCalled is false";
    ASSERT_TRUE(ESMediatorHelper::s_isConfigDevicePropSuccess)<<"s_isConfigDevicePropSuccess is false";

}
#endif

/**
 * @since 2016-08-24
 * @see public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
 *      authtype, WIFI_ENCTYPE enctype)
 * @see public void setDevConfProp(String language, String country)
 * @see public synchronized static EasySetup getInstance()
 * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
 *      enrolleeResource)
 * @objective test getStatus positively
 * @target public void provisionDeviceProperties(DeviceProp deviceProp,
 *         DevicePropProvisioningCallback callback) throws ESException
 * @test_data deviceProp DevicePropProvisioningCallback
 * @pre_condition 1.create remote enrollee 2.cretae deviceprop
 * @procedure 1. Call getStatus API.
 * @post_condition none
 * @expected getStatus call without exception
 *
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESProvisionDevicePropertiesTest_stc, RemoteEnrolleeStatus_RSV_P)
{
    ESMediatorHelper::s_isGetStatusCalled = false;
    ESMediatorHelper::s_isGetStatusCallbackCalled = false;

    m_esMediatorHelper.findEnrolleeResource();
    m_esMediatorHelper.getStatus();

    ASSERT_TRUE(ESMediatorHelper::s_isGetStatusCalled)<<"s_isGetStatusCalled is false";
    ASSERT_TRUE(ESMediatorHelper::s_isGetStatusCallbackCalled)<<"s_isGetStatusCallbackCalled is false";

}
#endif

/**
 * @since 2016-08-24
 * @see setCloudProp(string authCode, string authProvider, string ciServer)
 * @see public void setDevConfProp(String language, String country)
 * @see public synchronized static EasySetup getInstance()
 * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
 *      enrolleeResource)
 * @objective test provisionCloudProperties positively
 * @target public void provisionCloudProperties(const CloudProp& cloudProp,
 CloudPropProvStatusCb callback)
 * @test_data deviceProp DevicePropProvisioningCallback
 * @pre_condition 1.create remote enrollee 2.cretae deviceprop
 * @procedure 1. Call provisionDeviceProperties API.
 * @post_condition none
 * @expected provisionCloudProperties call without exception
 *
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESProvisionDevicePropertiesTest_stc, ProvisionCloudProperties_RSV_P)
{
    ESMediatorHelper::s_isCloudPropProvCallbackCalled = false;
    ESMediatorHelper::s_isProvisionCloudPropSuccess = false;

    m_esMediatorHelper.findEnrolleeResource();
    m_esMediatorHelper.provisionCloudProperty();

    ASSERT_TRUE(ESMediatorHelper::s_isProvisionCloudPropSuccess)<<"s_isProvisionCloudPropSuccess Called failed";
    ASSERT_TRUE(ESMediatorHelper::s_isCloudPropProvCallbackCalled)<<"s_isCloudPropProvCallbackCalled Called failed";

}
#endif

/**
 * @since 2016-08-24
 * @see public synchronized static EasySetup getInstance()
 * @see public synchronized RemoteEnrollee createRemoteEnrollee(OcResource
 *      enrolleeResource)
 * @objective test provisionSecurity positively
 * @target public void provisionSecurity(const SecurityProvStatusCb callback)
 * @test_data SecurityProvStatusCb
 * @pre_condition 1.create remote enrollee
 * @procedure 1. Call provisionSecurity API.
 * @post_condition none
 * @expected provisionSecurity call without exception
 *
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(ESProvisionDevicePropertiesTest_stc, ProvisionSecurity_RSV_P)
{
    ESMediatorHelper::s_isSecurityPropProvCallbackCalled = false;
    ESMediatorHelper::s_isProvisionSecurityPropSuccess = false;

    m_esMediatorHelper.findEnrolleeResource();
    m_esMediatorHelper.provisionSecurityProperty();

    ASSERT_TRUE(ESMediatorHelper::s_isProvisionSecurityPropSuccess)<<"ProvisionSecurityPropSuccess Called failed";
    ASSERT_TRUE(ESMediatorHelper::s_isSecurityPropProvCallbackCalled)<<"SecurityPropProvCallbackCalled Called failed";

}
#endif

