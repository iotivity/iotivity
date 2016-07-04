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

#include "EasySetup.h"
#include "RemoteEnrollee.h"
#include "ESException.h"

#include <iostream>
#include <functional>
#include <pthread.h>
#include <condition_variable>

using namespace OIC::Service;

const std::string  ipaddress = "1.1.1.1";
const std::string  ssid = "test";
const std::string  pwd = "test@123";

class MediatorRichTest: public TestWithMock
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
void easySetupStatusCallback (std::shared_ptr< EasySetupStatus > /*easySetupStatus*/)
{


}

/* Test cases for easysetyup class*/

//getInstance
TEST_F(MediatorRichTest, testGetInstance)
{
    EasySetup *easySetupIntance1 = EasySetup::getInstance();
    EasySetup *easySetupIntance2 = EasySetup::getInstance();

    EXPECT_EQ(easySetupIntance1, easySetupIntance2);
}

//createEnrolleeDevice [positive]
TEST_F(MediatorRichTest, testCreateEnrolleeDevice)
{
    RemoteEnrollee::shared_ptr remoteEnrollee = NULL;
    ProvConfig netInfo;
    WiFiOnboadingConnection onboardingConn;

    netInfo.connType = CT_ADAPTER_IP;

    OICStrcpy(netInfo.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, ssid.c_str());
    OICStrcpy(netInfo.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, pwd.c_str());

    onboardingConn.isSecured = false;
    OICStrcpy(onboardingConn.ipAddress, IPV4_ADDR_SIZE - 1, ipaddress.c_str());

    remoteEnrollee = EasySetup::getInstance()->createEnrolleeDevice(netInfo, onboardingConn);

    EXPECT_TRUE(remoteEnrollee != NULL);
}

//createEnrolleeDevice [Negative]
TEST_F(MediatorRichTest, testCreateEnrolleeDeviceNegative)
{
    RemoteEnrollee::shared_ptr remoteEnrollee = NULL;
    ProvConfig netInfo;
    WiFiOnboadingConnection onboardingConn;
    EasySetup *easysetupInstance = EasySetup::getInstance();

    netInfo.connType = CT_ADAPTER_IP;

    OICStrcpy(netInfo.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, ssid.c_str());
    OICStrcpy(netInfo.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, pwd.c_str());

    onboardingConn.isSecured = false;
    OICStrcpy(onboardingConn.ipAddress, IPV4_ADDR_SIZE - 1, ipaddress.c_str());

    //calling the createEnrolleeDevice
    remoteEnrollee = easysetupInstance->createEnrolleeDevice(netInfo, onboardingConn);
    //calling again the createEnrolleeDevice with same config & connection info [API throws the exception]
    EXPECT_ANY_THROW(easysetupInstance->createEnrolleeDevice(netInfo, onboardingConn));
}

//createEnrolleeDevice [positive (creating the two RemoteEnrolleeDevice with different config)]
TEST_F(MediatorRichTest, testCreateEnrolleeMultipleDevice)
{
    RemoteEnrollee::shared_ptr remoteEnrollee1 = NULL;
    RemoteEnrollee::shared_ptr remoteEnrollee2 = NULL;
    WiFiOnboadingConnection onboardingConn1;
    WiFiOnboadingConnection onboardingConn2;
    ProvConfig netInfo1;
    ProvConfig netInfo2;
    std::string  ipaddress2 = "0.0.0.0";

    //calling the API for the first device
    netInfo1.connType = CT_ADAPTER_IP;
    OICStrcpy(netInfo1.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, ssid.c_str());
    OICStrcpy(netInfo1.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, pwd.c_str());

    onboardingConn1.isSecured = false;
    OICStrcpy(onboardingConn1.ipAddress, IPV4_ADDR_SIZE - 1, ipaddress.c_str());
    remoteEnrollee1 = EasySetup::getInstance()->createEnrolleeDevice(netInfo1, onboardingConn1);

    //calling the API for the second device
    netInfo1.connType = CT_ADAPTER_IP;
    OICStrcpy(netInfo1.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, ssid.c_str());
    OICStrcpy(netInfo1.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, pwd.c_str());

    onboardingConn1.isSecured = false;
    OICStrcpy(onboardingConn1.ipAddress, IPV4_ADDR_SIZE - 1, ipaddress2.c_str());
    remoteEnrollee2 = EasySetup::getInstance()->createEnrolleeDevice(netInfo2, onboardingConn2);

    EXPECT_TRUE(remoteEnrollee1 != NULL);
    EXPECT_TRUE(remoteEnrollee2 != NULL);
}

/*Test cases for RemoteEnrollee class*/

//registerEasySetupStatusHandler [positive]
TEST_F(MediatorRichTest, testRegisterEasySetupStatusHandler)
{
    //creation of the Remote Enrollee Device
    RemoteEnrollee::shared_ptr remoteEnrollee = NULL;
    ProvConfig netInfo;
    WiFiOnboadingConnection onboardingConn;

    netInfo.connType = CT_ADAPTER_IP;

    OICStrcpy(netInfo.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, ssid.c_str());
    OICStrcpy(netInfo.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, pwd.c_str());

    onboardingConn.isSecured = false;
    OICStrcpy(onboardingConn.ipAddress, IPV4_ADDR_SIZE - 1, ipaddress.c_str());

    remoteEnrollee = EasySetup::getInstance()->createEnrolleeDevice(netInfo, onboardingConn);

    //calling the API
    EXPECT_NO_THROW( remoteEnrollee->registerEasySetupStatusHandler(&easySetupStatusCallback));
}

//registerEasySetupStatusHandler [Negative]
TEST_F(MediatorRichTest, testRegisterEasySetupStatusHandlerNegative)
{
    //creation of the Remote Enrollee Device
    RemoteEnrollee::shared_ptr remoteEnrollee = NULL;
    ProvConfig netInfo;
    WiFiOnboadingConnection onboardingConn;

    netInfo.connType = CT_ADAPTER_IP;

    OICStrcpy(netInfo.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, ssid.c_str());
    OICStrcpy(netInfo.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, pwd.c_str());

    onboardingConn.isSecured = false;
    OICStrcpy(onboardingConn.ipAddress, IPV4_ADDR_SIZE - 1, ipaddress.c_str());

    remoteEnrollee = EasySetup::getInstance()->createEnrolleeDevice(netInfo, onboardingConn);

    //calling the API
    EXPECT_ANY_THROW( remoteEnrollee->registerEasySetupStatusHandler(NULL));
}

//isEnrolleeProvisioned
TEST_F(MediatorRichTest, testIsEnrolleeProvisioned)
{
    //creation of the Remote Enrollee Device
    RemoteEnrollee::shared_ptr remoteEnrollee = NULL;
    ProvConfig netInfo;
    WiFiOnboadingConnection onboardingConn;

    netInfo.connType = CT_ADAPTER_IP;

    OICStrcpy(netInfo.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, ssid.c_str());
    OICStrcpy(netInfo.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, pwd.c_str());

    onboardingConn.isSecured = false;
    OICStrcpy(onboardingConn.ipAddress, IPV4_ADDR_SIZE - 1, ipaddress.c_str());

    remoteEnrollee = EasySetup::getInstance()->createEnrolleeDevice(netInfo, onboardingConn);

    //calling the API
    bool isEnrolleeProvisioned = remoteEnrollee->isEnrolleeProvisioned();

    EXPECT_TRUE(false == isEnrolleeProvisioned);
}

//startProvisioning [positive]
TEST_F(MediatorRichTest, testStartProvisioning)
{
    //creation of the Remote Enrollee Device
    RemoteEnrollee::shared_ptr remoteEnrollee = NULL;
    ProvConfig netInfo;
    WiFiOnboadingConnection onboardingConn;

    netInfo.connType = CT_ADAPTER_IP;

    OICStrcpy(netInfo.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, ssid.c_str());
    OICStrcpy(netInfo.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, pwd.c_str());

    onboardingConn.isSecured = false;
    OICStrcpy(onboardingConn.ipAddress, IPV4_ADDR_SIZE - 1, ipaddress.c_str());

    remoteEnrollee = EasySetup::getInstance()->createEnrolleeDevice(netInfo, onboardingConn);
    remoteEnrollee->registerEasySetupStatusHandler(&easySetupStatusCallback);

#ifdef REMOTE_ARDUINO_ENROLEE
    EXPECT_NO_THROW(remoteEnrollee->startProvisioning());

#else
    /* It will throw the exception
     * as it will not able to discover the provisioning resource in the network
     */
    EXPECT_ANY_THROW(remoteEnrollee->startProvisioning());
#endif
}

//startProvisioning [Negative]
TEST_F(MediatorRichTest, testStartProvisioningNegative)
{
    //creation of the Remote Enrollee Device
    RemoteEnrollee::shared_ptr remoteEnrollee = NULL;
    ProvConfig netInfo;
    WiFiOnboadingConnection onboardingConn;

    netInfo.connType = CT_ADAPTER_IP;

    OICStrcpy(netInfo.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, ssid.c_str());
    OICStrcpy(netInfo.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, pwd.c_str());

    onboardingConn.isSecured = false;
    OICStrcpy(onboardingConn.ipAddress, IPV4_ADDR_SIZE - 1, ipaddress.c_str());

    remoteEnrollee = EasySetup::getInstance()->createEnrolleeDevice(netInfo, onboardingConn);

    /* We are not registering the EasySetupStatusHandler, so startProvisioning
     * will throw the Exception &  we are checking the same
     */
    EXPECT_ANY_THROW(remoteEnrollee->startProvisioning());
}

//stopProvisioning [positive]
TEST_F(MediatorRichTest, testStopProvisioning)
{
    //creation of the Remote Enrollee Device
    RemoteEnrollee::shared_ptr remoteEnrollee = NULL;
    ProvConfig netInfo;
    WiFiOnboadingConnection onboardingConn;

    netInfo.connType = CT_ADAPTER_IP;

    OICStrcpy(netInfo.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, ssid.c_str());
    OICStrcpy(netInfo.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, pwd.c_str());

    onboardingConn.isSecured = false;
    OICStrcpy(onboardingConn.ipAddress, IPV4_ADDR_SIZE - 1, ipaddress.c_str());

    remoteEnrollee = EasySetup::getInstance()->createEnrolleeDevice(netInfo, onboardingConn);
    remoteEnrollee->registerEasySetupStatusHandler(&easySetupStatusCallback);

#ifdef REMOTE_ARDUINO_ENROLEE
        EXPECT_NO_THROW(remoteEnrollee->startProvisioning());
        EXPECT_NO_THROW(remoteEnrollee->stopProvisioning());

#else
    /* It will throw the exception as
     * it will not able to discover the provisioning resource in the network
     */
     EXPECT_ANY_THROW(remoteEnrollee->startProvisioning());

    /* It will throw an exception
     * as internally resource will be empty [startProvisioning is not succeed]
     */
    EXPECT_ANY_THROW(remoteEnrollee->stopProvisioning());
#endif
}

//stopProvisioning [Negative]
TEST_F(MediatorRichTest, testStopProvisioningNegative)
{
    //creation of the Remote Enrollee Device
    RemoteEnrollee::shared_ptr remoteEnrollee = NULL;
    ProvConfig netInfo;
    WiFiOnboadingConnection onboardingConn;

    netInfo.connType = CT_ADAPTER_IP;

    OICStrcpy(netInfo.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, ssid.c_str());
    OICStrcpy(netInfo.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, pwd.c_str());

    onboardingConn.isSecured = false;
    OICStrcpy(onboardingConn.ipAddress, IPV4_ADDR_SIZE - 1, ipaddress.c_str());

    remoteEnrollee = EasySetup::getInstance()->createEnrolleeDevice(netInfo, onboardingConn);
    remoteEnrollee->registerEasySetupStatusHandler(&easySetupStatusCallback);

    /* we didn't call the start provisioning API and directly calling stopProvisioning API.
     * In this case API will throw the exception & we are checking the same.
     */
    EXPECT_ANY_THROW(remoteEnrollee->stopProvisioning());
}

