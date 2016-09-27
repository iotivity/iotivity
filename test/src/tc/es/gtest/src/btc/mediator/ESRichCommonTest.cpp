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
#include <iostream>
#include "ESMediatorHelper.h"
#include "OCRepresentation.h"


#define WIFI_AUTHTYPE NONE_AUTH
#define WIFI_ENCTYPE NONE_ENC

using namespace std;
using namespace OC;
using namespace OIC::Service;

class ESRichCommonTest_btc: public ::testing::Test
{
public:
    
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
 * @since 2016-08-18
 * @objective Test 'getProvStatus' API with positive basic way
 * @target Void getProvStatus();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getProvStatus()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetProvStatus_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;

        // std::shared_ptr< GetEnrolleeStatus > status;
        EnrolleeStatus enrolleeStatus(mDevConfRep);
        ASSERT_EQ(enrolleeStatus.getProvStatus(), ES_STATE_INIT);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get ProvStatus: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'getLastErrCode' API with positive basic way
 * @target Void getLastErrCode();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getLastErrCode()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetLastErrorCode_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;

        EnrolleeStatus enrolleeStatus(mDevConfRep);
        ASSERT_EQ(enrolleeStatus.getLastErrCode(), ES_ERRCODE_NO_ERROR);
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get LastErrCode: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'setCloudProp' API with positive basic way
 * @target Void setCloudProp();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform setCloudProp()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, SetCloudProp_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        CloudProp cloudProp(mDevConfRep);
        cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, CI_SERVER);
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in setCloudProp: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'setCloudProp' API in negative way with empty value
 * @target Void setCloudProp();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform setCloudProp()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, SetCloudProp_ESV_N)
{
    try
    {
        const OCRepresentation mDevConfRep;
        CloudProp cloudProp(mDevConfRep);
        cloudProp.setCloudProp(EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in setCloudProp: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'setCloudProp' API in negative way with null value
 * @target Void setCloudProp();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform setCloudProp()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, SetCloudProp_NV_N)
{
    try
    {
        const OCRepresentation mDevConfRep;
        CloudProp cloudProp(mDevConfRep);
        cloudProp.setCloudProp(NULL_VALUE, NULL_VALUE, NULL_VALUE);
    }
    catch (exception& e)
    {
        ASSERT_EQ(std::string(e.what()), "basic_string::_S_construct null not valid")<<"Exception occurred in provisionProperties but not null type";
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'setCloudID' API with positive basic way
 * @target Void setCloudID();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform setCloudID()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, setCloudID_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        CloudProp cloudProp(mDevConfRep);
        cloudProp.setCloudID(CLOUD_ID);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in set CloudId: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'setCloudID' API in negative way with empty value
 * @target Void setCloudID();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform setCloudID()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, setCloudID_ESV_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        CloudProp cloudProp(mDevConfRep);
        cloudProp.setCloudID(EMPTY_STRING);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in set CloudId: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'setCloudID' API in negative way with NULL value
 * @target Void setCloudID();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform setCloudID()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, setCloudID_NV_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        CloudProp cloudProp(mDevConfRep);
        cloudProp.setCloudID(NULL_VALUE);
    }
    catch (exception& e)
    {
        ASSERT_EQ(std::string(e.what()), "basic_string::_S_construct null not valid")<<"Exception occurred in provisionProperties but not null type";
//        SET_FAILURE("Exception occurred in set CloudId: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'getAuthCode' API with positive basic way
 * @target std::string getAuthCode() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getAuthCode()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetAuthCode_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        CloudProp cloudProp(mDevConfRep);
        cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, CI_SERVER);
        ASSERT_EQ(cloudProp.getAuthCode(), AUTH_CODE);
    }

    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get AuthCode: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'getAuthProvider' API with positive basic way
 * @target std::string getAuthProvider() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getAuthProvider()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetAuthProvider_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        CloudProp cloudProp(mDevConfRep);
        cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, CI_SERVER);
        ASSERT_EQ(cloudProp.getAuthProvider(), AUTH_PROVIDER);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get AuthProvider: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'getCiServer' API with positive basic way
 * @target std::string getCiServer() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getCiServer()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetCiServer_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        CloudProp cloudProp(mDevConfRep);
        cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, CI_SERVER);
        ASSERT_EQ(cloudProp.getCiServer(), CI_SERVER);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get CiServer: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'getCloudID' API with positive basic way
 * @target std::string getCloudID() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getCloudID()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetCloudID_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        CloudProp cloudProp(mDevConfRep);
        cloudProp.setCloudID(CLOUD_ID);
        ASSERT_EQ(cloudProp.getCloudID(), CLOUD_ID);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get CloudID: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'setWiFiProp' API with positive basic way
 * @target void setWiFiProp() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform setWiFiProp()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, SetWiFiProp_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        deviceProp.setWiFiProp(SSID, PASSWORD, WIFI_AUTHTYPE, WIFI_ENCTYPE);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in set WiFiProp: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'setWiFiProp' API in negative way with empty value
 * @target void setWiFiProp() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform setWiFiProp()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, SetWiFiProp_ESV_N)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        deviceProp.setWiFiProp(EMPTY_STRING, EMPTY_STRING, WIFI_AUTHTYPE, WIFI_ENCTYPE);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in set WiFiProp: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-18
 * @objective Test 'setWiFiProp' API in negative way with null value
 * @target void setWiFiProp() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform setWiFiProp()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, SetWiFiProp_NV_N)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        deviceProp.setWiFiProp(NULL_VALUE, NULL_VALUE, WIFI_AUTHTYPE, WIFI_ENCTYPE);
    }
    catch (exception& e)
    {
        ASSERT_EQ(std::string(e.what()), "basic_string::_S_construct null not valid")<<"Exception occurred in provisionProperties but not null type";
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'setDevConfProp' API with positive basic way
 * @target void setDevConfProp() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform setDevConfProp()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, SetDevConfProp_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        deviceProp.setDevConfProp(LANGUAGE, COUNTRY, LOCATION);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in set DevConfProp: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'setDevConfProp' API in negative way with empty value
 * @target void setDevConfProp() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform setDevConfProp()
 * @post_condition None
 * @expected successfully called api & crash should not occur
 **/
TEST_F(ESRichCommonTest_btc, SetDevConfProp_ESV_N)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        deviceProp.setDevConfProp(EMPTY_STRING, EMPTY_STRING, EMPTY_STRING);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in set DevConfProp: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'setDevConfProp' API in negative way with null value
 * @target void setDevConfProp() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform setDevConfProp()
 * @post_condition None
 * @expected successfully called api & crash should not occur
 **/
TEST_F(ESRichCommonTest_btc, SetDevConfProp_NV_N)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        deviceProp.setDevConfProp(NULL_VALUE, NULL_VALUE, NULL_VALUE);
    }
    catch (exception& e)
    {
        ASSERT_EQ(std::string(e.what()), "basic_string::_S_construct null not valid")<<"Exception occurred in provisionProperties but not null type";
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'getSsid' API with positive basic way
 * @target std::string getSsid() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getSsid()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetSsid_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        deviceProp.setWiFiProp(SSID, PASSWORD, WIFI_AUTHTYPE, WIFI_ENCTYPE);
        ASSERT_EQ(deviceProp.getSsid(), SSID);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get Ssid: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'getSsid' API in negative way(without set; trying to get SSID)
 * @target std::string getSsid() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getSsid()
 * @post_condition None
 * @expected successfully called api & should return empty string
 **/
TEST_F(ESRichCommonTest_btc, GetSsid_DSCC_N)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        ASSERT_EQ(deviceProp.getSsid(), EMPTY_STRING);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get Ssid: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'getPassword' API with positive basic way
 * @target std::string getPassword() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getPassword()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetPassword_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        deviceProp.setWiFiProp(SSID, PASSWORD, WIFI_AUTHTYPE, WIFI_ENCTYPE);
        ASSERT_EQ(deviceProp.getPassword(), PASSWORD);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get Password: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'getPassword' API in negative way(without set; trying to get Password)
 * @target std::string getPassword() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getPassword()
 * @post_condition None
 * @expected successfully called api & should return empty string
 **/
TEST_F(ESRichCommonTest_btc, GetPassword_DSCC_N)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        ASSERT_EQ(deviceProp.getPassword(), EMPTY_STRING);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get Password: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'getAuthType' API with positive basic way
 * @target WIFI_AUTHTYPE getAuthType() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getAuthType()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetAuthType_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        deviceProp.setWiFiProp(SSID, PASSWORD, WIFI_AUTHTYPE, WIFI_ENCTYPE);
        ASSERT_EQ(deviceProp.getAuthType(), WIFI_AUTHTYPE);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get AuthType: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'getEncType' API with positive basic way
 * @target WIFI_ENCTYPE getEncType() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getEncType()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, getEncType_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        ASSERT_EQ(deviceProp.getEncType(), WIFI_ENCTYPE);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get EncType: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'getLanguage' API with positive basic way
 * @target std::string getLanguage() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getLanguage()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetLanguage_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        deviceProp.setDevConfProp(LANGUAGE, COUNTRY, LOCATION);
        ASSERT_EQ(deviceProp.getLanguage(), LANGUAGE);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get Language: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'getLanguage' API in negative way(without set; trying to get language)
 * @target std::string getLanguage() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getLanguage()
 * @post_condition None
 * @expected successfully called api & should return empty string
 **/
TEST_F(ESRichCommonTest_btc, GetLanguage_DSCC_N)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        ASSERT_EQ(deviceProp.getLanguage(), EMPTY_STRING);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get Language: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'getCountry' API with positive basic way
 * @target std::string getCountry() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getCountry()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetCountry_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        deviceProp.setDevConfProp(LANGUAGE, COUNTRY, LOCATION);
        ASSERT_EQ(deviceProp.getCountry(), COUNTRY);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get Country: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'getCountry' API in negative way(without set; trying to get COUNTRY)
 * @target std::string getCountry() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getCountry()
 * @post_condition None
 * @expected successfully called api & should return empty string
 **/
TEST_F(ESRichCommonTest_btc, GetCountry_DSCC_N)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        ASSERT_EQ(deviceProp.getCountry(), EMPTY_STRING);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get Country: " + std::string(e.what()));
    }
}

/**
 * @since 2016-09-05
 * @objective Test 'get' API with positive basic way
 * @target std::string getLocation() const;
 * @test_data    set device location
 * @pre_condition None
 * @procedure Perform getLocation()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetLocation_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        deviceProp.setDevConfProp(LANGUAGE, COUNTRY, LOCATION);
        ASSERT_EQ(deviceProp.getLocation(), LOCATION);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get Country: " + std::string(e.what()));
    }
}

/**
 * @since 2016-09-05
 * @objective Test 'getLocation' API in negative way(without set; trying to get COUNTRY)
 * @target std::string getLocation() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getLocation()
 * @post_condition None
 * @expected successfully called api & should return empty string
 **/
TEST_F(ESRichCommonTest_btc, GetLocation_DSCC_N)
{
    try
    {
        const OCRepresentation mDevConfRep;
        DeviceProp deviceProp(mDevConfRep);
        ASSERT_EQ(deviceProp.getLocation(), EMPTY_STRING);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get Country: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'getDeviceName' API in negative way(without set; trying to get name)
 * @target std::string getDeviceName() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getDeviceName()
 * @post_condition None
 * @expected successfully called api & should return empty string
 **/
TEST_F(ESRichCommonTest_btc, GetDeviceName_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        EnrolleeConf enrolleeConf(mDevConfRep);
        ASSERT_EQ(enrolleeConf.getDeviceName(), EMPTY_STRING);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get DeviceName: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-19
 * @objective Test 'getWiFiFreq' API in positive way
 * @target WIFI_FREQ getWiFiFreq() const;
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getWiFiFreq()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESRichCommonTest_btc, GetWiFiFreq_SRC_P)
{
    try
    {
        const OCRepresentation mDevConfRep;
        EnrolleeConf enrolleeConf(mDevConfRep);
        ASSERT_EQ(enrolleeConf.getWiFiFreq(), WIFI_FREQ_NONE);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get WiFiFreq: " + std::string(e.what()));
    }
}
