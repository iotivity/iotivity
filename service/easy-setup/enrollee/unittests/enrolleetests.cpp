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

/**
 * @file
 *
 * This file contains the implementation for EasySetup Enrollee testcases
 */

#include "gtest/gtest.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <stdint.h>

#include "ocstack.h"
#include "logger.h"
#include "oic_malloc.h"
#include "escommon.h"
#include "easysetup.h"
#include "onboarding.h"
#include "resourcehandler.h"
#include "softap.h"
#include "easysetupcallbacks.h"

#include "gtesthelper.h"

using namespace std;

namespace itst = iotivity::test;

#define DEFAULT_CONTEXT_VALUE 0x99

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------
static const char TAG[] = "TestHarness";

char ssid[] = "EasySetup123";
char passwd[] = "EasySetup123";

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------
extern "C"  void EventCallbackInApplication(ESResult esResult,
                                                                    ESEnrolleeState enrolleeState)
{
    OIC_LOG(INFO, TAG, "Entering EventCallbackInApplication");

    if(esResult != ES_OK)
    {
        cout<<"Easy stup is failed at Enrollee state = "<<enrolleeState<<endl;
    }
    else
    {
        if(enrolleeState == ES_ON_BOARDED_STATE)
        {
            cout<<"Device is successfully OnBoared on Adhoc network"<<endl;
        }
        else if (enrolleeState == ES_PROVISIONED_STATE)
        {
            cout<<"Device is provisioned with target network's credentials"<<endl;
        }
        else if (enrolleeState == ES_ON_BOARDED_TARGET_NETWORK_STATE)
        {
            cout<<"Device is onboarded/connected with target network"<<endl;
        }
        else
        {
            cout<<"Wrong state !! Easy setup is failed at Enrollee state = "<<enrolleeState<<endl;
        }
    }
}

//-----------------------------------------------------------------------------
//  Local functions
//-----------------------------------------------------------------------------

void InitESEnrollee()
{
    OIC_LOG(INFO, TAG, "Entering InitESEnrollee");

    EXPECT_EQ(ES_OK, ESInitEnrollee(CT_ADAPTER_IP,
                                    "EasySetup123",
                                    "EasySetup123",
                                    0,
                                    EventCallbackInApplication));
    OIC_LOG(INFO, TAG, "Leaving InitESEnrollee");
}

//-----------------------------------------------------------------------------
// Tests
//-----------------------------------------------------------------------------

TEST(ESEnrolleeInit, ESEnrolleeInitNullSSID)
{
    EXPECT_EQ(ES_ERROR, ESInitEnrollee(CT_ADAPTER_IP, 0, passwd, 0, EventCallbackInApplication));
}

TEST(ESEnrolleeInit, ESEnrolleeInitNullPassword)
{
    EXPECT_EQ(ES_ERROR, ESInitEnrollee(CT_ADAPTER_IP, ssid, 0, 0, EventCallbackInApplication));
}

TEST(ESEnrolleeInit, ESEnrolleeInitNullCb)
{
    EXPECT_EQ(ES_ERROR, ESInitEnrollee(CT_ADAPTER_IP, ssid, passwd, 0, 0));
}

TEST(ESEnrolleeInit, ESEnrolleeESInitEnrolleeSuccess)
{
    EXPECT_EQ(ES_OK, ESInitEnrollee(CT_ADAPTER_IP,
                                    ssid,
                                    passwd,
                                    0,
                                    EventCallbackInApplication));
}

TEST(ESEnrolleeInit, ESEnrolleeBleOnboarding)
{
    EXPECT_EQ(0, ESBleOnboarding());
}

TEST(ESEnrolleeInit, ESEnrolleeSoftapOnboarding)
{
    EXPECT_EQ(true, ESSoftapOnboarding());
}

TEST(ESOCInit, ESOCInitNullIPAddr)
{
    EXPECT_EQ(OC_STACK_OK, OCInit(0, 5683, OC_SERVER));
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

TEST(ESOCInit, ESOCInitNullPort)
{
    EXPECT_EQ(OC_STACK_OK, OCInit("127.0.0.1", 0, OC_SERVER));
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

TEST(ESOCInit, ESOCInitNullAddrAndPort)
{
    EXPECT_EQ(OC_STACK_OK, OCInit(0, 0, OC_SERVER));
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

TEST(ESOCInit, ESOCInitInvalidMode)
{
    EXPECT_EQ(OC_STACK_ERROR, OCInit(0, 0, (OCMode)10));
    EXPECT_EQ(OC_STACK_ERROR, OCStop());
}

TEST(ESProvisioning, ESInitProvisioningResource)
{
    EXPECT_EQ(ES_RESOURCECREATED, ESInitProvisioning());
}

TEST(ESProvisioning, ESInitProvisioningWithOCStackClientMode)
{
    EXPECT_EQ(OC_STACK_OK, OCInit(NULL, 0, OC_CLIENT));
    EXPECT_EQ(ES_ERROR, ESInitProvisioning());
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

TEST(ESProvisioningResource, ESCreateProvisioningResourceSecurityDisabled)
{
    EXPECT_EQ(OC_STACK_OK, OCInit(0, 0, OC_SERVER));
    EXPECT_EQ(OC_STACK_OK, CreateProvisioningResource(0));
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

TEST(ESProvisioningResource, ESCreateProvisioningResourceSecurityEnabled)
{
    EXPECT_EQ(OC_STACK_OK, OCInit(0, 0, OC_SERVER));
    EXPECT_EQ(OC_STACK_OK, CreateProvisioningResource(1));
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

TEST(ESProvisioningResource, ESDeleteProvisioningResource)
{
    EXPECT_EQ(OC_STACK_OK, OCInit(0, 0, OC_SERVER));
    EXPECT_EQ(OC_STACK_OK, CreateProvisioningResource(0));
    EXPECT_EQ(OC_STACK_OK, DeleteProvisioningResource());
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

TEST(ESProvisioningResource, ESDeleteProvisioningResourceWithoutCreating)
{
    EXPECT_EQ(OC_STACK_OK, OCInit(0, 0, OC_SERVER));
    EXPECT_EQ(OC_STACK_NO_RESOURCE, DeleteProvisioningResource());
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

TEST(ESOnboarding, ESOnboardingSoftAp)
{
    EXPECT_EQ(1, ESOnboard(ssid, passwd, ESOnboardingCallback));
}

TEST(ESOnboarding, ESOnboardingSoftApEnrollee)
{
#ifdef ENROLLEE_SOFTAP
    EXPECT_EQ(1, ESSoftapAtEnrollee());
    EXPECT_EQ(0, ESSoftapAtMediator());
#endif
}

TEST(ESOnboarding, ESOnboardingSoftApMediator)
{
#ifndef ENROLLEE_SOFTAP
    EXPECT_EQ(0, ESSoftapAtEnrollee());
    EXPECT_EQ(1, ESSoftapAtMediator());
#endif
}

TEST(ESOnboarding, ESOnboardingSoftApHost)
{
#ifdef ENROLLEE_SOFTAP
    EXPECT_EQ(ENROLLEE, ESSoftapHost());
#else
    EXPECT_EQ(MEDIATOR, ESSoftapHost());
#endif
}

TEST(ESStop, ESTerminateEasysetupWithoutESInit)
{
    EXPECT_EQ(ES_ERROR, ESTerminateEnrollee());
}

TEST(ESStop, ESTerminateEasysetupWithoutOCStack)
{
    EXPECT_EQ(ES_OK, ESInitEnrollee(CT_ADAPTER_IP,
                                    ssid,
                                    passwd,
                                    0,
                                    EventCallbackInApplication));
    EXPECT_EQ(ES_ERROR, ESTerminateEnrollee());
}

TEST(ESStop, ESTerminateEasysetupWithOutCreateProvisioningResource)
{
    EXPECT_EQ(ES_OK, ESInitEnrollee(CT_ADAPTER_IP,
                                    ssid,
                                    passwd,
                                    0,
                                    EventCallbackInApplication));
    EXPECT_EQ(OC_STACK_OK, OCInit(0, 0, OC_SERVER));
    EXPECT_EQ(ES_ERROR, ESTerminateEnrollee());
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

TEST(ESStop, ESTerminateEasysetupWithoutProviosioningResource)
{
    EXPECT_EQ(ES_OK, ESInitEnrollee(CT_ADAPTER_IP,
                                    ssid,
                                    passwd,
                                    0,
                                    EventCallbackInApplication));
    EXPECT_EQ(OC_STACK_OK, OCInit(NULL, 0, OC_SERVER));
    EXPECT_EQ(ES_ERROR, ESTerminateEnrollee());
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

TEST(ESStop, ESTerminateEasysetupSuccess)
{
    EXPECT_EQ(ES_OK, ESInitEnrollee(CT_ADAPTER_IP,
                                    ssid,
                                    passwd,
                                    0,
                                    EventCallbackInApplication));
    EXPECT_EQ(OC_STACK_OK, OCInit(NULL, 0, OC_SERVER));
    EXPECT_EQ(ES_RESOURCECREATED, ESInitProvisioning());
    EXPECT_EQ(ES_OK, ESTerminateEnrollee());
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

