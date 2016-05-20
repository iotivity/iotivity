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
#include "OCPlatform.h"
#include "logger.h"
#include "OCProvisioningManager.h"


#define ES_SAMPLE_APP_TAG "ES_SAMPLE_APP_TAG"
#define DECLARE_MENU(FUNC, ...) { #FUNC, FUNC }

#define JSON_DB_PATH "./oic_svr_db_client.dat"

using namespace OC;
using namespace OIC::Service;

static EasySetup *easySetupIntance = nullptr;
static ProvConfig netInfo;
static WiFiOnboadingConnection onboardingConn;
static RemoteEnrollee::shared_ptr remoteEnrollee = nullptr;

static std::string ipaddress, ssid, pwd;
char security;

struct CloseApp
{
};

typedef void (*Runner)();

Runner g_currentRun;

int processUserInput(int min, int max)
{
    assert(min <= max);

    int input;

    std::cin >> input;

    if (!std::cin.fail())
    {
        if(input == max + 1) throw CloseApp();
        if(min <= input && input <= max) return input;
    }

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    throw std::runtime_error("Invalid Input, please try again");
}

void easySetupStatusCallback (std::shared_ptr< EasySetupStatus > easySetupStatus)
{
    OIC_LOG_V(DEBUG, ES_SAMPLE_APP_TAG, "easySetupStatusCallback status is Status = %d",
            easySetupStatus->getEasySetupState());

}

void startProvisioning()
{
    try
    {
        remoteEnrollee->startProvisioning();
    }
    catch(OCException &exception)
    {
        std::cout << "Exception : " << exception.reason();
    }
}

void initEasySetup()
{

    easySetupIntance = EasySetup::getInstance();

    cout<<"\n Enter the IP address : ";
    cin>>ipaddress;
    cout<<"\n Enter the Target Network SSID : ";
    cin>>ssid;
    cout<<"\n Enter the Target Network Password : ";
    cin>>pwd;
    cout<<"\n Enable Security: [Y/N] ";
    cin>>security;

    if ( ipaddress.size() == 0 || ssid.size() == 0  || pwd.size()==0 )
    {
         cout<<"\n Invalid information try again !!!";
    }
    else
     {
           cout <<"\n Entered details are :  \n";
           cout<<"\n IP address : "<<ipaddress;
           cout<<"\n Target Network SSID : "<<ssid;
           cout<<"\n Target Network Password : "<<pwd;

          if (security == 'Y' || security == 'y' )
          {
                  onboardingConn.isSecured = true;
                  cout<<"\n Security is Enabled\n\n\n";
           }
           else
           {
                 onboardingConn.isSecured = false;
                 cout<<"\n Security is not Enabled\n\n\n";
           }

          netInfo.connType = CT_ADAPTER_IP;

         OICStrcpy(netInfo.provData.WIFI.ssid, NET_WIFI_SSID_SIZE - 1, ssid.c_str());
         OICStrcpy(netInfo.provData.WIFI.pwd, NET_WIFI_PWD_SIZE - 1, pwd.c_str());

         OICStrcpy(onboardingConn.ipAddress, IPV4_ADDR_SIZE - 1, ipaddress.c_str());

         try
         {
             remoteEnrollee = easySetupIntance->createEnrolleeDevice(netInfo,onboardingConn);
         }
         catch (OCException &e)
         {
             std::cout << "Exception during createEnrolleeDevice call" << e.reason();
             return;
         }

         remoteEnrollee->registerEasySetupStatusHandler(&easySetupStatusCallback);
    }

}
void runEasySetupMenu()
{
    constexpr int EASY_SETUP_INIT = 1;
    constexpr int START_PROVISIONING = 2;
    constexpr int STOP_PROVISIONING = 3;
    constexpr int STOP_EASY_SETUP = 4;

    std::cout << "========================================================\n";
    std::cout << EASY_SETUP_INIT << ". Easy Setup Init                    \n";
    std::cout << START_PROVISIONING << ". Start Provisioning              \n";
    std::cout << STOP_PROVISIONING << ". Stop Provisioning                \n";
    std::cout << STOP_EASY_SETUP << ". Stop Easy Setup                    \n";
    std::cout << STOP_EASY_SETUP + 1 << ". Quit                           \n";
    std::cout << "========================================================\n";

    int selection = processUserInput(EASY_SETUP_INIT, STOP_EASY_SETUP);

    switch (selection)
    {
        case EASY_SETUP_INIT:
            initEasySetup();
            break;
        case START_PROVISIONING:
            startProvisioning();
            break;
        case STOP_PROVISIONING:
            //stopProvisioning();
            break;
        case STOP_EASY_SETUP:
            //stopEasySetup();
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

    g_currentRun = runEasySetupMenu;

    while (true)
    {
        try
        {
            g_currentRun();
        }
        catch (const std::exception& e)
        {
            std::cout << "Exception caught in main " << e.what() << std::endl;
        }
        catch (const CloseApp&)
        {
            break;
        }
    }

    std::cout << "Stopping the client" << std::endl;

    return 0;
}

