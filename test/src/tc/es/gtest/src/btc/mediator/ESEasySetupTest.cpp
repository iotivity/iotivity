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

using namespace std;
using namespace OC;
using namespace OIC::Service;

std::mutex g_mutexForCondition;
std::shared_ptr< RemoteEnrollee > g_remoteEnrollee;
std::condition_variable g_responseCon;
std::chrono::milliseconds g_waitForResponse(1000);

class ESEasySetupTest_btc: public ::testing::Test
{
public:
    std::shared_ptr< OC::OCResource > m_enrolleeResource;
    static std::shared_ptr< RemoteEnrollee > s_remoteEnrollee;

    std::shared_ptr<OC::OCResource> CreateNotProvResource()
    {
        OCConnectivityType connectivityType = CT_DEFAULT;
        std::vector<std::string> types = {"oic.wk.notprov"};
        std::vector<std::string> ifaces = {DEFAULT_INTERFACE};

        return OCPlatform::constructResourceObject("coap://192.168.1.2:5000",
                                                   "/NotProvisioningResURI",
                                                   connectivityType,
                                                   false,
                                                   types,
                                                   ifaces);
    }

protected:

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
    }

    virtual void TearDown()
    {
        g_remoteEnrollee = nullptr;
        CommonUtil::runCommonTCTearDownPart();
    }

};

/**
 * @since 2016-08-16
 * @objective Test 'createRemoteEnrollee' function with positive basic way
 * @target Void createRemoteEnrollee();
 * @test_data	1. OCResource::Ptr constructResourceObject()
 * 				2. discoverRemoteEnrolleeCb callback
 * @pre_condition None
 * @procedure Perform createRemoteEnrollee
 * @post_condition None
 * @expected creating a remote Enrollee instance
 **/
TEST_F(ESEasySetupTest_btc, CreateRemoteEnrollee_SRC_P)
{
    try
    {
        ESMediatorHelper esMediatorHelper;
        esMediatorHelper.findEnrolleeResource();
        
        g_remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(
                ESMediatorHelper::s_curResource);

        if(g_remoteEnrollee==NULL)
        {
            SET_FAILURE("Remote enrollee was not created.");
        }
    }
    catch (exception& e)
    {
        SET_FAILURE("Exception occurred in get ProvStatus: " + std::string(e.what()));
    }
}

/**
 * @since 2016-08-16
 * @objective Test 'createRemoteEnrollee' function in negative way
 * @target Void createRemoteEnrollee();
 * @test_data   1. OCResource::Ptr constructResourceObject()
 *              2. Null parameter
 * @pre_condition None
 * @procedure Perform createRemoteEnrollee
 * @post_condition None
 * @expected return null pointer
 **/
TEST_F(ESEasySetupTest_btc, CreateRemoteEnrolleeWithNonProvResource_USV_N)
{
    auto s_remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(CreateNotProvResource());
    EXPECT_EQ(nullptr, s_remoteEnrollee)<<"Failed for Not creating resource.";

    g_remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(NULL);
    EXPECT_EQ(nullptr, g_remoteEnrollee)<<"Failed for null parameter.";
}

