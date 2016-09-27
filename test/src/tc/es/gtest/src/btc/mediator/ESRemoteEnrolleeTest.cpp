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

#include <gtest/gtest.h>
#include "CommonUtil.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <exception>
#include "easysetup.h"
#include <escommon.h>
#include "ESEnrolleeCommon.h"
#include "ESMediatorHelper.h"
#include <iostream>

using namespace std;
using namespace OC;
using namespace OIC::Service;

class ESRemoteEnrolleeTest_btc: public ::testing::Test
{
public:
    DeviceProp m_devProp;
    CloudProp m_cloudProp;

    static void onSecurityProvStatusCb(shared_ptr< SecProvisioningStatus >)
    {
        IOTIVITYTEST_LOG(INFO, "provisionSecurity is success.");
    }
protected:

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
    }
    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }

};

/**
 * @since 2016-08-12
 * @objective Test 'getStatus' API with positive basic way
 * @target Void getStatus(GetStatusCb callback);
 * @test_data 		1. GetStatusCb callback.
 * @pre_condition None
 * @procedure Perform getStatus
 * @post_condition None
 * @expected Successfully called api
 **/
TEST_F(ESRemoteEnrolleeTest_btc, GetStatus_SRC_P)
{
    try
    {
        ESMediatorHelper esMediatorHelper;
        esMediatorHelper.findEnrolleeResource();
        ASSERT_NE(nullptr, ESMediatorHelper::s_remoteEnrollee)<<"Remote enrolle can not be null.";
        ESMediatorHelper::s_remoteEnrollee->getStatus(ESMediatorHelper::getStatusCallback);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get status: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-12
 * @objective Test 'getStatus' API in negative way
 * @target Void getStatus(GetStatusCb callback);
 * @test_data 		1. GetStatusCb callback.
 * @pre_condition None
 * @procedure Perform getStatus
 * @post_condition None
 * @expected throws Exception & should not crash
 */
TEST_F(ESRemoteEnrolleeTest_btc, GetStatus_NV_N)
{
    EXPECT_ANY_THROW(ESMediatorHelper::s_remoteEnrollee->getStatus(NULL_VALUE));
}

/**
 * @since 2016-08-12
 * @objective Test 'getConfiguration' API with positive basic way
 * @target Void getConfiguration(GetConfigurationStatusCb callback);
 * @test_data 		1. GetConfigurationStatusCb callback.
 * @pre_condition None
 * @procedure Perform getConfiguration
 * @post_condition None
 * @expected Get an Enrollee's configuration which includes WiFi supported frequency and device name
 */
TEST_F(ESRemoteEnrolleeTest_btc, GetConfiguration_SRC_P)
{
    try
    {

        ESMediatorHelper esMediatorHelper;
        esMediatorHelper.findEnrolleeResource();
        ASSERT_NE(nullptr, ESMediatorHelper::s_remoteEnrollee)<<"Remote enrolle can not be null.";
        ESMediatorHelper::s_remoteEnrollee->getConfiguration(
                &ESMediatorHelper::getConfigurationCallback);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get Configuration: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-12
 * @objective Test 'getConfiguration' API with negative way
 * @target Void getConfiguration(GetStatusCb callback);
 * @test_data 		1. GetStatusCb callback.
 * @pre_condition None
 * @procedure Perform getConfiguration
 * @post_condition None
 * @expected throws Exception but should not crash
 */
TEST_F(ESRemoteEnrolleeTest_btc, GetConfiguration_NV_N)
{
    EXPECT_ANY_THROW(ESMediatorHelper::s_remoteEnrollee->getConfiguration(NULL_VALUE));
}

/**
 * @since 2016-08-16
 * @objective Test 'provisionSecurity' in positive way
 * @target Void provisionSecurity(SecurityProvStatusCb callback);
 * @test_data 		1. SecurityProvStatusCb callback.
 * @pre_condition None
 * @procedure Perform provisionSecurity
 * @post_condition None
 * @expected Do security provisioning such as ownership transfer to Enrollee
 */
TEST_F(ESRemoteEnrolleeTest_btc, ProvisionSecurity_SRC_P)
{
    try
    {
        ESMediatorHelper esMediatorHelper;
        esMediatorHelper.findEnrolleeResource();
        ASSERT_NE(nullptr, ESMediatorHelper::s_remoteEnrollee)<<"Remote enrolle can not be null.";
        ESMediatorHelper::s_remoteEnrollee->provisionSecurity(
                &ESRemoteEnrolleeTest_btc::onSecurityProvStatusCb);
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get provisionSecurity: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-16
 * @objective Test 'provisionSecurity' in negative way
 * @target Void provisionSecurity(SecurityProvStatusCb callback);
 * @test_data 		1. SecurityProvStatusCb callback.
 * @pre_condition None
 * @procedure Perform provisionSecurity
 * @post_condition None
 * @expected No security provisioning will be done & no crash should occur
 */
TEST_F(ESRemoteEnrolleeTest_btc, ProvisionSecurity_NV_N)
{
    EXPECT_ANY_THROW(ESMediatorHelper::s_remoteEnrollee->provisionSecurity(NULL_VALUE));
}

/**
 * @since 2016-08-12
 * @objective Test 'provisionDeviceProperties' API with basic positive way
 * @target Void provisionDeviceProperties(const DeviceProp& devProp, DevicePropProvStatusCb callback)
 * @test_data 		1. DevicePropProvStatusCb callback.
 * 					2. DeviceProp
 * @pre_condition None
 * @procedure Perform provisionDeviceProperties
 * @post_condition None
 * @expected it will Provision WiFi AP information and device configuration
 */
TEST_F(ESRemoteEnrolleeTest_btc, ProvisionDeviceProperies_SRC_P)
{
    ESMediatorHelper esMediatorHelper;
    esMediatorHelper.findEnrolleeResource();
    ASSERT_NE(nullptr, ESMediatorHelper::s_remoteEnrollee)<<"Remote enrolle can not be null.";
    m_devProp.setWiFiProp(SSID, PASSWORD, WPA2_PSK, TKIP_AES);
    m_devProp.setDevConfProp(LANGUAGE, COUNTRY, LOCATION);

    try
    {
        ESMediatorHelper::s_remoteEnrollee->provisionDeviceProperties(m_devProp,
                &ESMediatorHelper::deviceProvisioningStatusCallback);
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred in get provisionDeviceProperties: " + std::string(e.what()));
    }

}

/**
 * @since 2016-08-12
 * @objective Test 'provisionDeviceProperties' API with negative way
 * @target Void provisionDeviceProperties(const DeviceProp& devProp, DevicePropProvStatusCb callback)
 * @test_data 		1. DevicePropProvStatusCb callback.
 * 					2. DeviceProp
 * @pre_condition None
 * @procedure Perform provisionDeviceProperties
 * @post_condition None
 * @expected Exception
 */
TEST_F(ESRemoteEnrolleeTest_btc, ProvisionDeviceProperieswithNullCallback_NV_N)
{
    m_devProp.setWiFiProp(SSID, PASSWORD, WPA2_PSK, TKIP_AES);
    m_devProp.setDevConfProp(LANGUAGE, COUNTRY, LOCATION);

    EXPECT_ANY_THROW(
            ESMediatorHelper::s_remoteEnrollee->provisionDeviceProperties(m_devProp, NULL_VALUE));
}

/**
 * @since 2016-08-12
 * @objective Test 'provisionDeviceProperties' API with negative way
 * @target Void provisionDeviceProperties(const DeviceProp& devProp, DevicePropProvStatusCb callback)
 * @test_data 		1. DevicePropProvStatusCb callback.
 * 					2. DeviceProp
 * @pre_condition None
 * @procedure Perform provisionDeviceProperties
 * @post_condition None
 * @expected Exception
 */
TEST_F(ESRemoteEnrolleeTest_btc, ProvisionDeviceProperiesWithoutSSID_USV_N)
{
    m_devProp.setWiFiProp(EMPTY_STRING, PASSWORD, WPA2_PSK, TKIP_AES);
    m_devProp.setDevConfProp(LANGUAGE, COUNTRY, LOCATION);
    EXPECT_ANY_THROW(ESMediatorHelper::s_remoteEnrollee->provisionDeviceProperties(m_devProp,NULL));
}

/**
 * @since 2016-08-12
 * @objective Test 'provisionCloudProperties' API with basic positive way
 * @target Void provisionCloudProperties(const CloudProp& cloudProp, CloudPropProvStatusCb callback)
 * @test_data 		1. CloudPropProvStatusCb callback.
 * 					2. CloudProp
 * @pre_condition None
 * @procedure Perform provisionCloudProperties
 * @post_condition None
 * @expected it will Provision Cloud information to Enrollee
 */
TEST_F(ESRemoteEnrolleeTest_btc, ProvisionCloudProperties_SRC_P)
{
    ESMediatorHelper esMediatorHelper;
    esMediatorHelper.findEnrolleeResource();
    ASSERT_NE(nullptr, ESMediatorHelper::s_remoteEnrollee)<<"Remote enrolle can not be null.";
    m_cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, CI_SERVER);
    m_cloudProp.setCloudID(CLOUD_ID);

    try
    {
        ESMediatorHelper::s_remoteEnrollee->provisionCloudProperties(m_cloudProp,
                ESMediatorHelper::cloudProvisioningStatusCallback);
    }
    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred in get provisionDeviceProperties: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-12
 * @objective Test 'provisionCloudProperties' API with negative way
 * @target Void provisionCloudProperties(const CloudProp& cloudProp, CloudPropProvStatusCb callback)
 * @test_data 		1. CloudPropProvStatusCb callback.
 * 					2. CloudProp
 * @pre_condition None
 * @procedure Perform provisionCloudProperties
 * @post_condition None
 * @expected it will throws Exception
 */
TEST_F(ESRemoteEnrolleeTest_btc,ProvisionCloudPropertieswithNullCallback_NV_N)
{
    m_cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, CI_SERVER);
    m_cloudProp.setCloudID(CLOUD_ID);

    EXPECT_ANY_THROW(
            ESMediatorHelper::s_remoteEnrollee->provisionCloudProperties(m_cloudProp, NULL_VALUE));
}

/**
 * @since 2016-08-12
 * @objective Test 'provisionCloudProperties' API with negative way
 * @target Void provisionCloudProperties(const CloudProp& cloudProp, CloudPropProvStatusCb callback)
 * @test_data 		1. CloudPropProvStatusCb callback.
 * 					2. CloudProp
 * @pre_condition None
 * @procedure Perform provisionCloudProperties
 * @post_condition None
 * @expected it will throws Exception
 */
TEST_F(ESRemoteEnrolleeTest_btc, ProvisionCloudPropertiesWithoutAuthCode_NV_N)
{
    m_cloudProp.setCloudProp(EMPTY_STRING, AUTH_PROVIDER, CI_SERVER);
    m_cloudProp.setCloudID(CLOUD_ID);
    EXPECT_ANY_THROW(
            ESMediatorHelper::s_remoteEnrollee->provisionCloudProperties(m_cloudProp, NULL));
}

/**
 * @since 2016-08-12
 * @objective Test 'provisionCloudProperties' API with negative way
 * @target Void provisionCloudProperties(const CloudProp& cloudProp, CloudPropProvStatusCb callback)
 * @test_data 		1. CloudPropProvStatusCb callback.
 * 					2. CloudProp
 * @pre_condition None
 * @procedure Perform provisionCloudProperties
 * @post_condition None
 * @expected it will throws Exception
 */
TEST_F(ESRemoteEnrolleeTest_btc, ProvisionCloudPropertiesWithoutAuthProvider_NV_N)
{
    m_cloudProp.setCloudProp(AUTH_CODE, EMPTY_STRING, CI_SERVER);
    m_cloudProp.setCloudID(CLOUD_ID);
    m_cloudProp.setCredID(1);
    EXPECT_ANY_THROW(
            ESMediatorHelper::s_remoteEnrollee->provisionCloudProperties(m_cloudProp,NULL));
}

/**
 * @since 2016-08-12
 * @objective Test 'provisionCloudProperties' API with negative way
 * @target Void provisionCloudProperties(const CloudProp& cloudProp, CloudPropProvStatusCb callback)
 * @test_data 		1. CloudPropProvStatusCb callback.
 * 					2. CloudProp
 * @pre_condition None
 * @procedure Perform provisionCloudProperties
 * @post_condition None
 * @expected it will throws Exception
 */
TEST_F(ESRemoteEnrolleeTest_btc, ProvisionCloudPropertiesWithoutCIServer_NV_N)
{
    m_cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, EMPTY_STRING);
    m_cloudProp.setCloudID(CLOUD_ID);
    EXPECT_ANY_THROW(
            ESMediatorHelper::s_remoteEnrollee->provisionCloudProperties(m_cloudProp,NULL));
}

