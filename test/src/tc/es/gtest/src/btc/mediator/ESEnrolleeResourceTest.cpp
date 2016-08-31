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
#include <functional>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <exception>
#include "easysetup.h"
#include <escommon.h>
#include "ESEnrolleeCommon.h"
#include "EasySetup.h"
#include "EnrolleeResource.h"
#include "RemoteEnrollee.h"
#include <iostream>
#include "ESMediatorHelper.h"

#define NULL_VALUE NULL
#define SSID "Iotivity_SSID"
#define PASSWORD "Iotivity_PWD"
#define LANGUAGE "korean"
#define COUNTRY "korea"
#define LOCATION "location"
#define EMPTY_STRING ""

using namespace std;
using namespace OC;
using namespace OIC::Service;
std::shared_ptr< OC::OCResource > resource;

const string HOST_ADDRESS = "coap://192.168.1.2:5000";
const string RES_ADDRESS = "/a/light";
const string RES_TYPE = "core.light";
const string INTERFACE_TYPE = "oic.if.baseline";

class ESEnrolleeResourceTest_btc: public ::testing::Test
{
public:
    DeviceProp m_devProp;

protected:

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        vector< string > types =
        { RES_TYPE };
        vector< string > ifaces =
        { INTERFACE_TYPE };

        resource = OCPlatform::constructResourceObject(HOST_ADDRESS, RES_ADDRESS, CT_DEFAULT,
                false, types, ifaces);
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }

};

/**
 * @since 2016-08-16
 * @objective Test 'getStatus' API with positive basic way
 * @target Void getStatus();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getStatus()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESEnrolleeResourceTest_btc, EnrolleeResourceGetStatus_SRC_P)
{
    try
    {
        EnrolleeResource rc(resource);
        rc.getStatus();
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get status: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-16
 * @objective Test 'getConfiguration' API with positive basic way
 * @target Void getConfiguration();
 * @test_data 	None
 * @pre_condition None
 * @procedure Perform getConfiguration
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESEnrolleeResourceTest_btc, EnrolleeResourceGetConfiguration_SRC_P)
{
    try
    {
        EnrolleeResource rc(resource);
        rc.getConfiguration();
//        cout<<"rc.getConfiguration();" << rc.getConfiguration();
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get Configuration: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-12
 * @objective Test 'provisionProperties' API with basic positive way
 * @target Void provisionProperties(const DeviceProp& devProp)
 * @test_data 		1. DeviceProp
 * @pre_condition None
 * @procedure Perform provisionProperties()
 * @post_condition None
 * @expected successfully called api
 */
TEST_F(ESEnrolleeResourceTest_btc, EnrolleeResourceProvisionEnrollee_SRC_P)
{
    m_devProp.setWiFiProp(SSID, PASSWORD, WPA2_PSK, TKIP_AES);
    m_devProp.setDevConfProp(LANGUAGE, COUNTRY, LOCATION);

    try
    {
        EnrolleeResource rc(resource);
        rc.provisionProperties(m_devProp);
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get ProvisionEnrollee: " + std::string(e.what()));
    }

}

/**
 * @since 2016-08-12
 * @objective Test 'provisionProperties' API with negative way
 * @target Void provisionProperties(const DeviceProp& devProp)
 * @test_data 		1. DeviceProp
 * @pre_condition None
 * @procedure Perform provisionProperties()
 * @post_condition None
 * @expected throws Exception
 */
TEST_F(ESEnrolleeResourceTest_btc, EnrolleeResourceProvisionEnrolleewithoutPWD_NV_N)
{
    m_devProp.setWiFiProp(SSID, EMPTY_STRING, WPA2_PSK, TKIP_AES);
    m_devProp.setDevConfProp(LANGUAGE, COUNTRY, LOCATION);
    std::shared_ptr< OC::OCResource > resource;
    EnrolleeResource rc(resource);
    EXPECT_ANY_THROW(rc.provisionProperties(m_devProp));
}

/**
 * @since 2016-08-12
 * @objective Test 'provisionProperties' API in negative way
 * @target Void provisionProperties(const DeviceProp& devProp)
 * @test_data 		1. DeviceProp
 * @pre_condition None
 * @procedure Perform provisionProperties()
 * @post_condition None
 * @expected throws Exception
 */
TEST_F(ESEnrolleeResourceTest_btc, EnrolleeResourceProvisionEnrolleeWithoutSSID_USV_N)
{
    m_devProp.setWiFiProp(EMPTY_STRING, PASSWORD, WPA2_PSK, TKIP_AES);
    m_devProp.setDevConfProp(LANGUAGE, COUNTRY, LOCATION);
    std::shared_ptr< OC::OCResource > resource;
    EnrolleeResource rc(resource);
    EXPECT_ANY_THROW(rc.provisionProperties(m_devProp));
}
