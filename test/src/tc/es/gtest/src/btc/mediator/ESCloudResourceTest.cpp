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
#include "ESMediatorHelper.h"
#include <iostream>

#define AUTH_CODE "authCode"
#define AUTH_PROVIDER "authProvider"
#define CI_SERVER "ciServer"
#define NULL_VALUE NULL
#define CLOUD_ID "f002ae8b-c42c-40d3-8b8d-1927c17bd1b3"
#define EMPTY_STRING ""

using namespace std;
using namespace OC;
using namespace OIC::Service;

const string HOST_ADDRESS = "coap://192.168.1.2:5000";
const string RES_ADDRESS = "/a/light";
const string RES_TYPE = "core.light";
const string INTERFACE_TYPE = "oic.if.baseline";

class ESCloudResourceTest_btc: public ::testing::Test
{
public:

    void onCloudProvResponse(const HeaderOptions&, const OCRepresentation& rep, const int eCode)
    {

    }
    CloudResource* cloudResource;

protected:

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        vector< string > types =
        { RES_TYPE };
        vector< string > ifaces =
        { INTERFACE_TYPE };

        std::shared_ptr< OC::OCResource > resource = OCPlatform::constructResourceObject(
                HOST_ADDRESS, RES_ADDRESS, CT_DEFAULT, false, types, ifaces);
        cloudResource = new CloudResource(resource);

    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();

    }

};

/**
 * @since 2016-08-16
 * @objective Test 'provisionProperties' API with positive basic way
 * @target Void provisionProperties();
 * @test_data    CloudProp
 * @pre_condition None
 * @procedure Perform provisionProperties()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESCloudResourceTest_btc, ProvisionEnrollee_SRC_P)
{
    try
    {
        CloudProp cloudProp;
        cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, CI_SERVER);
        cloudProp.setCloudID(CLOUD_ID);
        cloudResource->provisionProperties(cloudProp);
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in provisionProperties: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-16
 * @objective Test 'provisionProperties' API in negative way
 * @target Void provisionProperties();
 * @test_data    CloudProp
 * @pre_condition None
 * @procedure Perform provisionProperties()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESCloudResourceTest_btc, ProvisionEnrolleeWithoutAuthCode_NV_N)
{
    try
    {
        CloudProp cloudProp;
        cloudProp.setCloudProp(NULL_VALUE, AUTH_PROVIDER, CI_SERVER);
        cloudProp.setCloudID(CLOUD_ID);
        cloudProp.setCredID(1);
        cloudResource->provisionProperties(cloudProp);
        SET_FAILURE("No Exception occured");
    }

    catch (exception& e)
    {
        ASSERT_EQ(std::string(e.what()), "basic_string::_S_construct null not valid")<<"Exception occurred in provisionProperties but not null type";
    }
}

/**
 * @since 2016-08-16
 * @objective Test 'provisionProperties' API in negative way
 * @target Void provisionProperties();
 * @test_data    CloudProp
 * @pre_condition None
 * @procedure Perform provisionProperties()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESCloudResourceTest_btc, ProvisionEnrolleeWithoutAuthProvider_NV_N)
{
    try
    {
        CloudProp cloudProp;
        cloudProp.setCloudProp(AUTH_CODE, EMPTY_STRING, CI_SERVER);
        cloudProp.setCloudID(CLOUD_ID);
        cloudResource->provisionProperties(cloudProp);
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in provisionProperties: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-16
 * @objective Test 'provisionProperties' API in negative way
 * @target Void provisionProperties();
 * @test_data    CloudProp
 * @pre_condition None
 * @procedure Perform provisionProperties()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESCloudResourceTest_btc, ProvisionEnrolleeWithoutCiServer_NV_N)
{
    try
    {
        CloudProp cloudProp;
        cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, EMPTY_STRING);
        cloudProp.setCloudID(CLOUD_ID);
        cloudResource->provisionProperties(cloudProp);
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in provisionProperties: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-16
 * @objective Test 'provisionProperties' API in negative way
 * @target Void provisionProperties();
 * @test_data    CloudProp
 * @pre_condition None
 * @procedure Perform provisionProperties()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESCloudResourceTest_btc, ProvisionEnrolleeWithoutID_NV_N)
{
    try
    {
        CloudProp cloudProp;
        cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, CI_SERVER);
        cloudProp.setCloudID(EMPTY_STRING);
        cloudResource->provisionProperties(cloudProp);
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in provisionProperties: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-16
 * @objective Test 'provisionProperties' API in negative way
 * @target Void provisionProperties();
 * @test_data    NULL
 * @pre_condition None
 * @procedure Perform provisionProperties() without initialize cloudProp
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESCloudResourceTest_btc, ProvisionEnrolleeWithNullValue_NV_N)
{
    try
    {
        CloudProp cloudProp;
        cloudResource->provisionProperties(cloudProp);
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in provisionProperties: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-16
 * @objective Test 'registerCloudPropProvisioningStatusCallback' API in positive way
 * @target Void registerCloudPropProvisioningStatusCallback(CloudPropProvStatusCb callback);
 * @test_data    cloudProvisioningStatusCallback
 * @pre_condition None
 * @procedure Perform provisionProperties(CloudPropProvStatusCb callback)
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESCloudResourceTest_btc, registerCloudPropProvisioningStatusCallback_SRC_P)
{
    try
    {
        cloudResource->registerCloudPropProvisioningStatusCallback(
                ESMediatorHelper::cloudProvisioningStatusCallback);
    }

    catch (exception& e)
    {
        SET_FAILURE(
                "Exception occurred in registerCloudPropProvisioningStatusCallback: "
                        + std::string(e.what()));
    }
}
