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

#include <iostream>
#include<stdio.h>

#include "oic_string.h"
#include "EasySetup.h"
#include "ESRichCommon.h"
#include "OCPlatform.h"
#include "logger.h"
#include "OCProvisioningManager.h"

#define ES_SAMPLE_APP_TAG "ES_SAMPLE_APP_TAG"
#define DECLARE_MENU(FUNC, ...) { #FUNC, FUNC }

#define JSON_DB_PATH "./oic_svr_db_client.dat"

using namespace OC;
using namespace OIC::Service;

static EasySetup *easySetupIntance = nullptr;
static std::shared_ptr<RemoteEnrollee> remoteEnrollee = nullptr;

static std::string ipaddress, ssid, pwd;
char security;

typedef void (*Runner)();

Runner g_currentRun;

int processUserInput(int min = std::numeric_limits<int>::min(),
        int max = std::numeric_limits<int>::max())
{
    assert(min <= max);

    int input;

    std::cin >> input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (!std::cin.fail() && min <= input && input <= max) return input;

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    throw std::runtime_error("Invalid Input, please try again");
}

void printPropertyData(PropertyData propData)
{
    cout << "===========================================" << endl;
    DeviceConfig devConfig = propData.getDevConf();
    NetworkInfo netInfo = propData.getNetInfo();

    cout << "\tDeviceConfig.id : " << devConfig.id << endl;
    cout << "\tDeviceConfig.name : " << devConfig.name << endl;
    cout << "\tDeviceConfig.language : " << devConfig.language << endl;
    cout << "\tDeviceConfig.country : " << devConfig.country << endl;

    for(auto type = netInfo.types.begin(); type != netInfo.types.end(); ++type)
    {
        cout << "\tnetInfo.types : " << static_cast<int>(*type) << endl;
    }
    cout << "\tnetInfo.freq : " << static_cast<int>(netInfo.freq) << endl;
    cout << "===========================================" << endl;
}

void RequestPropertyDataStatusCallback(std::shared_ptr< RequestPropertyDataStatus > requestPropertyDataStatus)
{
    cout << "Enter RequestPropertyDataStatusCb." << endl;

    if(requestPropertyDataStatus->getESResult() != ES_OK)
    {
      cout << "requestPropertyDataStatus is failed." << endl;
      return;
    }
    else
    {
      cout << "requestPropertyDataStatus is success." << endl;
      printPropertyData(requestPropertyDataStatus->getPropertyData());
    }
}

void dataProvisioningStatusCallback(std::shared_ptr< DataProvisioningStatus > provStatus)
{
    cout << "Enter dataProvisioningStatusCallback." << endl;

    if(provStatus->getESResult() != ES_OK)
    {
      cout << "dataProvisioningStatusCallback is failed." << endl;
      return;
    }
    else
    {
      cout << "dataProvisioningStatusCallback is success." << endl;
      cout << "ESDataProvState : " << provStatus->getESDataProvState() << endl;
    }
}

void cloudProvisioningStatusCallback(std::shared_ptr< CloudProvisioningStatus > status)
{
    cout << "Enter cloudProvisioningStatusCallback." << endl;
    cout << "CloudProvStatus : " <<  status->getESCloudState() << endl;
}

void createRemoteEnrollee()
{
    easySetupIntance = EasySetup::getInstance();
    try
    {
        remoteEnrollee = easySetupIntance->createRemoteEnrollee();
    }
    catch (OCException &e)
    {
        std::cout << "Exception during createEnrolleeDevice call" << e.reason();
        return;
    }
    cout << "createRemoteEnrollee is success." << endl;
}

void initRemoteEnrollee()
{
    try
    {
        remoteEnrollee->initRemoteEnrollee();
    }
    catch (OCException &e)
    {
        std::cout << "Exception during initRemoteEnrollee call" << e.reason();
        return;
    }
}

void requestPropertyData()
{
    try
    {
        remoteEnrollee->requestPropertyData(RequestPropertyDataStatusCallback);
    }
    catch (OCException &e)
    {
        std::cout << "Exception during requestPropertyData call" << e.reason();
        return;
    }
}

void setDataProvInfo()
{
    DataProvInfo dataProvInfo;
    dataProvInfo.WIFI.ssid = "Iotivity_2.4G";
    dataProvInfo.WIFI.pwd = "1234567890";
    dataProvInfo.WIFI.authtype = WPA2_PSK;
    dataProvInfo.WIFI.enctype = TKIP_AES;
    dataProvInfo.Device.language = "korean";
    dataProvInfo.Device.country = "korea";

    remoteEnrollee->setDataProvInfo(dataProvInfo);
}

void setCloudProvInfo()
{
    CloudProvInfo cloudProvInfo;
    cloudProvInfo.authCode = "authCode";
    cloudProvInfo.authProvider = "authProvider";
    cloudProvInfo.ciServer = "ciServer";

    remoteEnrollee->setCloudProvInfo(cloudProvInfo);
}

void startDataProvisioning()
{
    try
    {
        remoteEnrollee->startDataProvisioning(dataProvisioningStatusCallback);
    }
    catch (OCException &e)
    {
        std::cout << "Exception during startDataProvisioning call" << e.reason();
        return;
    }
}

void startCloudProvisioning()
{
    try
    {
        remoteEnrollee->startCloudProvisioning(cloudProvisioningStatusCallback);
    }
    catch (OCException &e)
    {
        std::cout << "Exception during startDataProvisioning call" << e.reason();
        return;
    }
}

void DisplayMenu()
{
    constexpr int CREATE_REMOTE_ENROLLEE = 1;
    constexpr int EASY_SETUP_INIT = 2;
    constexpr int REQUEST_PROPERTY_DATA = 3;
    constexpr int SET_DATA_PROVISIONING_INFO = 4;
    constexpr int START_DATA_PROVISIONING = 5;
    constexpr int SET_CLOUD_PROVISIONING_INFO = 6;
    constexpr int START_CLOUD_PROVISIONING = 7;

    std::cout << "========================================================\n";
    std::cout << CREATE_REMOTE_ENROLLEE << ". Create Remote Enrollee                    \n";
    std::cout << EASY_SETUP_INIT << ". Easy Setup Init                    \n";
    std::cout << REQUEST_PROPERTY_DATA << ". Request PropertyData              \n";
    std::cout << SET_DATA_PROVISIONING_INFO << ". Set Data Provisioning Info              \n";
    std::cout << START_DATA_PROVISIONING << ". Start Data Provisioning              \n";
    std::cout << SET_CLOUD_PROVISIONING_INFO << ". Set Cloud Provisioning Info              \n";
    std::cout << START_CLOUD_PROVISIONING << ". Start Cloud Provisioning              \n";
    std::cout << "========================================================\n";

    int selection = processUserInput(CREATE_REMOTE_ENROLLEE, START_CLOUD_PROVISIONING);

    switch (selection)
    {
        case CREATE_REMOTE_ENROLLEE:
            createRemoteEnrollee();
            break;
        case EASY_SETUP_INIT:
            initRemoteEnrollee();
            break;
        case REQUEST_PROPERTY_DATA:
            requestPropertyData();
            break;
        case SET_DATA_PROVISIONING_INFO:
            setDataProvInfo();
            break;
        case START_DATA_PROVISIONING:
            startDataProvisioning();
            break;
        case SET_CLOUD_PROVISIONING_INFO:
            setCloudProvInfo();
            break;
        case START_CLOUD_PROVISIONING:
            startCloudProvisioning();
            break;
        default:
            break;
    };
}

static FILE* client_open(const char *UNUSED_PARAM, const char *mode)
{
    (void)UNUSED_PARAM;
    return fopen(JSON_DB_PATH, mode);
}

int main()
{
    OCPersistentStorage ps {client_open, fread, fwrite, fclose, unlink };

    PlatformConfig config
    {
        OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos, &ps
    };

    OCPlatform::Configure(config);

#ifdef __WITH_DTLS__
    //Initializing the provisioning client stack using the db path provided by the application.
    OCStackResult result = OCSecure::provisionInit("");

    if (result != OC_STACK_OK)
    {
        return -1;
    }
#endif

    while (true)
    {
        try
        {
            DisplayMenu();
        }
        catch (const std::exception& e)
        {
            std::cout << "Exception caught in main " << e.what() << std::endl;
        }
    }

    std::cout << "Stopping the client" << std::endl;

    return 0;
}

