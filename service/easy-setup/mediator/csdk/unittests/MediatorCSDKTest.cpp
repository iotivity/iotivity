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

#include "UnitTestHelper.h"

#include "ocstack.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "oic_string.h"
#include "logger.h"

#include "provisioningapi.h"
#include "provisioning.h"
#include "escommon.h"

#include <iostream>
#include <string.h>
#include <functional>
#include <pthread.h>
#include <condition_variable>

class MediatorCSDKTest: public TestWithMock
{

    protected:
        void SetUp()
        {
            TestWithMock::SetUp();
        }

        void TearDown()
        {
            TestWithMock::TearDown();
        }

};

//callbacks
void OCProvisioningStatusCallback(EasySetupInfo  *easySetupInfo) 
{
    (void) easySetupInfo;
}

/* Test cases for prov_adapter*/

//InitProvProcess
TEST_F(MediatorCSDKTest, testInitProvProcess)
{
    OCStackResult result = InitProvProcess();
    ASSERT_EQ(OC_STACK_OK, result);
}

//ResetProvProcess
TEST_F(MediatorCSDKTest, testResetProvProcess)
{
    OCStackResult result = ResetProvProcess();
    ASSERT_EQ(OC_STACK_OK, result);
}

//RegisterCallback [positive]
TEST_F(MediatorCSDKTest, testRegisterCallback)
{
    OCStackResult result = RegisterCallback(&OCProvisioningStatusCallback);
    ASSERT_EQ(OC_STACK_OK, result);
}

//RegisterCallback [Negative]
TEST_F(MediatorCSDKTest, testRegisterCallbackNegative)
{
    OCStackResult result = RegisterCallback(NULL);
    ASSERT_EQ(OC_STACK_ERROR, result);
}

//StartProvisioning [positive]
TEST_F(MediatorCSDKTest, testStartProvisioning)
{
    ProvConfig provConfig;
    WiFiOnboadingConnection onboardConn;

    strncpy(onboardConn.ipAddress, "1.1.1.1", IPV4_ADDR_SIZE - 1);

    strncpy(provConfig.provData.WIFI.ssid, "testAP", NET_WIFI_SSID_SIZE - 1);
    strncpy(provConfig.provData.WIFI.pwd, "test@123", NET_WIFI_PWD_SIZE - 1);
    provConfig.connType = CT_ADAPTER_IP;

    //API Call
    OCStackResult result =    StartProvisioning(&provConfig, &onboardConn);

    ASSERT_EQ(OC_STACK_OK, result);
}

//StartProvisioning [Negative]
TEST_F(MediatorCSDKTest, testStartProvisioningNegative)
{
    OCStackResult result = StartProvisioning(NULL, NULL);
    ASSERT_EQ(OC_STACK_ERROR, result);
}

//StopProvisioning
TEST_F(MediatorCSDKTest, testStopProvisioning)
{
    // This API always return success
    OCStackResult result = StopProvisioning(CT_ADAPTER_IP);
    ASSERT_EQ(OC_STACK_OK, result);
}


