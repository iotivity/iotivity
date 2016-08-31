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
#include "ESMediatorHelper.h"
#include <iostream>

#define EMPTY_STRING ""
#define PROV_RESOURCE_TYPE "ocf.wk.prov"
#define NON_PROV_RESOURCE_TYPE "ocf.wk.notprov"
#define HOST "coap://192.168.1.2:5000"
#define PROV_URI "/oic/res?rt="
#define NON_PROV_URI "/NotProvisioningResURI"
#define IS_OBSERVABLE false

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
    OCConnectivityType m_connectivityType = CT_DEFAULT;

    void discoverRemoteEnrolleeCb(std::shared_ptr< OC::OCResource > resource)
    {

        if (!resource->getResourceTypes().at(0).compare(OC_RSRVD_ES_RES_TYPE_PROV))
        {
            m_enrolleeResource = resource;
        }
    }

    std::shared_ptr< OC::OCResource > CreateNotProvResource()
    {
        std::vector< std::string > types =
        { NON_PROV_RESOURCE_TYPE };
        std::vector< std::string > ifaces =
        { DEFAULT_INTERFACE };

        return OCPlatform::constructResourceObject(HOST, NON_PROV_URI, m_connectivityType,
                IS_OBSERVABLE, types, ifaces);
    }

    void discoverRemoteEnrollee()
    {
        std::string uri = std::string(PROV_URI) + OC_RSRVD_ES_RES_TYPE_PROV;
        OC::OCPlatform::findResource(EMPTY_STRING, uri, OCConnectivityType::CT_DEFAULT,
                std::bind(&ESEasySetupTest_btc::discoverRemoteEnrolleeCb, this,
                        std::placeholders::_1));

        std::unique_lock < std::mutex > lock
        { g_mutexForCondition };
        g_responseCon.wait_for(lock, g_waitForResponse);
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
// **/
//TEST_F(ESEasySetupTest_btc, CreateRemoteEnrollee_SRC_P)
//{
////    ESMediatorHelper esMediatorHelper;
////    esMediatorHelper.findEnrolleeResource();
//    discoverRemoteEnrollee();
//    g_remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(ESEasySetupTest_btc::m_enrolleeResource);
//
//    ASSERT_NE(nullptr, g_remoteEnrollee);
//}

/**
 * @since 2016-08-16
 * @objective Test 'createRemoteEnrollee' function in negative way
 * @target Void createRemoteEnrollee();
 * @test_data   1. OCResource::Ptr constructResourceObject()
 *              2. discoverRemoteEnrolleeCb callback
 * @pre_condition None
 * @procedure Perform createRemoteEnrollee
 * @post_condition None
 * @expected return null pointer
 **/
TEST_F(ESEasySetupTest_btc, CreateRemoteEnrolleeWithNonProvResource_USV_N)
{
    auto s_remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(CreateNotProvResource());
//    g_remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(CreateNotProvResource());
    cout<<"asdlfj"<<s_remoteEnrollee<<endl;

    EXPECT_EQ(nullptr, s_remoteEnrollee);
}
