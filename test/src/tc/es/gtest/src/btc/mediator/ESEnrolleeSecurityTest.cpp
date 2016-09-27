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

#define CBOR_DB_PATH "./oic_svr_db_client.dat"

using namespace std;
using namespace OC;
using namespace OIC::Service;


static std::shared_ptr< OC::OCResource > g_resource;

static FILE* clientOpen(const char *UNUSED_PARAM, const char *mode)
{
    (void) UNUSED_PARAM;
    return fopen(CBOR_DB_PATH, mode);
}

class ESEnrolleeSecurityTest_btc: public ::testing::Test
{
public:

protected:

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        vector< string > types =
        { RES_TYPE };
        vector< string > ifaces =
        { INTERFACE_TYPE };

        OCPersistentStorage ps
        { clientOpen, fread, fwrite, fclose, unlink };

        PlatformConfig cfg
        { OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos,
                &ps };

        OCPlatform::Configure(cfg);

        if (OCSecure::provisionInit(EMPTY_STRING) != OC_STACK_OK)
        {
            return false;
        }

        g_resource = OCPlatform::constructResourceObject(HOST_ADDRESS, RES_ADDRESS, CT_DEFAULT,
        false, types, ifaces);
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }

};

/**
 * @since 2016-08-16
 * @objective Test 'registerCallbackHandler' API with negative basic way
 * @target Void registerCallbackHandler(SecurityProvStatusCb securityProvStatusCb,
 SecurityPinCb securityPinCb, SecProvisioningDbPathCb secProvisioningDbPathCb);
 * @test_data    None
 * @pre_condition None
 * @procedure Perform registerCallbackHandler()
 * @post_condition None
 * @expected successfully called api
 **/
/*
TEST_F(ESEnrolleeSecurityTest_btc, registerCallbackHandler_ETC_N)
{
    try
    {
        std::shared_ptr < EnrolleeSecurity > m_enrolleeSecurity;
        m_enrolleeSecurity = std::make_shared < EnrolleeSecurity > (g_resource, "");
        m_enrolleeSecurity->registerCallbackHandler(NULL, NULL, NULL);
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in OwnershipTransfer: " + std::string(e.what()));
    }
}*/

/**
 * @since 2016-08-16
 * @objective Test 'getUUID' API with positive basic way
 * @target Void getUUID();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getUUID()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESEnrolleeSecurityTest_btc, getUUID_SRC_P)
{
    try
    {
        std::shared_ptr < EnrolleeSecurity > m_enrolleeSecurity;
        m_enrolleeSecurity = std::make_shared < EnrolleeSecurity > (g_resource, "");
        std::shared_ptr < SecProvisioningStatus > securityProvisioningStatus = std::make_shared
                < SecProvisioningStatus > (m_enrolleeSecurity->getUUID(), ES_ERROR);
        if (securityProvisioningStatus == NULL)
        {
            SET_FAILURE("Security Provisioning Status  is null.");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in OwnershipTransfer: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-16
 * @objective Test 'getUUID' API with positive basic way
 * @target Void getUUID();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform getUUID()
 * @post_condition None
 * @expected successfully called api
 **/
TEST_F(ESEnrolleeSecurityTest_btc, getUUID_ETC_N)
{
    try
    {
        std::shared_ptr < EnrolleeSecurity > m_enrolleeSecurity;
        m_enrolleeSecurity = std::make_shared < EnrolleeSecurity > (g_resource, "");
        if (m_enrolleeSecurity->getUUID() != "")
        {
            SET_FAILURE("Expected UUID not found");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in OwnershipTransfer: " + std::string(e.what()));
    }
}

/**
 * @since 2016-09-01
 * @objective Test 'provisionOwnership' API with positive basic way
 * @target Void provisionOwnership();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform provisionOwnership()
 * @post_condition None
 * @expected successfully called api.
 **/
TEST_F(ESEnrolleeSecurityTest_btc, provisionOwnership_SRC_P)
{
    std::shared_ptr < EnrolleeSecurity > m_enrolleeSecurity;
    m_enrolleeSecurity = std::make_shared < EnrolleeSecurity > (g_resource, "");
    if (m_enrolleeSecurity->provisionOwnership() == NULL)
        {
            SET_FAILURE("Provision Ownership is null");
        }
}

/**
 * @since 2016-08-16
 * @objective Test 'provisionOwnership' API with negative basic way
 * @target Void provisionOwnership();
 * @test_data    None
 * @pre_condition None
 * @procedure Perform provisionOwnership()
 * @post_condition None
 * @expected API should through ES_SECURE_RESOURCE_DISCOVERY_FAILURE
 **/
TEST_F(ESEnrolleeSecurityTest_btc, provisionOwnership_ETC_N)
{
    try
    {
        std::shared_ptr < EnrolleeSecurity > m_enrolleeSecurity;
        m_enrolleeSecurity = std::make_shared < EnrolleeSecurity > (g_resource, "");
        ASSERT_EQ(ES_SECURE_RESOURCE_DISCOVERY_FAILURE, m_enrolleeSecurity->provisionOwnership())<<"Expect result ES_SECURE_RESOURCE_DISCOVERY_FAILURE is not found.";
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in provisionOwnership: " + std::string(e.what()));
    }
}
