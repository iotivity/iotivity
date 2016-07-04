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


#include "easysetup.h"

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <pthread.h>

#define TAG "TS"

using namespace std;

void *listeningFunc(void*);

/**
 * @var ssid
 * @brief Target SSID of the Soft Access point to which the device has to connect
 */
static char ssid[] = "EasySetup123";

/**
 * @var passwd
 * @brief Password of the Soft Access point to which the device has to connect
 */
static char passwd[] = "EasySetup123";

/**
 * Secure Virtual Resource database for Iotivity Server
 * It contains Server's Identity and the PSK credentials
 * of other devices which the server trusts
 */
static char CRED_FILE[] = "oic_svr_db_server.dat";

OCPersistentStorage ps ;


/**
 * @var gIsSecured
 * @brief Variable to check if secure mode is enabled or not.
 */
static bool gIsSecured = false;

void PrintMenu()
{
    cout<<"============"<<endl;
    cout<<"S: Enabled Security"<<endl;
    cout<<"I: Init easy setup"<<endl;
    cout<<"P: start provisioning resources"<<endl;
    cout<<"T: terminate"<<endl;
    cout<<"Q: quit"<<endl;
    cout<<"============"<<endl;
}

void EventCallbackInApp(ESResult esResult, ESEnrolleeState enrolleeState)
{
    cout<<"Easy setup event callback"<<endl;

    if(esResult == ES_OK)
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
    else
    {
        cout<<"Easy stup is failed at Enrollee state = "<<enrolleeState<<endl;
    }

    PrintMenu();
}

FILE* server_fopen(const char *path, const char *mode)
{
    (void) path;
    return fopen(CRED_FILE, mode);
}

void EnableSecurity()
{
    cout << "Inside EnableSecurity API.." << endl;

    gIsSecured = true;

    // Initialize Persistent Storage for SVR database
    ps = { server_fopen, fread, fwrite, fclose, unlink };
    OCRegisterPersistentStorageHandler(&ps);
}

void StartEasySetup()
{
    cout<<"StartEasySetup and onboarding started.."<<endl;

    if(ESInitEnrollee(CT_ADAPTER_IP, ssid, passwd, gIsSecured, EventCallbackInApp) == ES_ERROR)
    {
        cout<<"StartEasySetup and onboarding Fail!!"<<endl;
        return;
    }
}

void ESInitResources()
{
    cout<<"Starting Enrollee Provisioning"<<endl;

    // Initialize the OC Stack in Server mode
    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        cout<<"OCStack init error!!"<<endl;
        return;
    }

    if (ESInitProvisioning() == ES_ERROR)
    {
        cout<<"Init Provisioning Failed!!"<<endl;
        return;
    }

    pthread_t thread_handle;
    if (pthread_create(&thread_handle, NULL, listeningFunc, NULL))
    {
        cout<<"Thread creation failed"<<endl;
    }

    cout<<"ESInitProvisioning Success"<<endl;
}

void StopEasySetup()
{
    cout<<"StopEasySetup IN"<<endl;

    if (ESTerminateEnrollee() == ES_ERROR)
    {
        cout<<"ESTerminateEnrollee Failed!!"<<endl;
        return;
    }

    //stop OC Stack
    if (OCStop() != OC_STACK_OK)
    {
        cout<<"OCStack stop failed!!"<<endl;
        return;
    }

    cout<<"StopEasySetup OUT"<<endl;
}

int main()
{
    cout<<"#########################"<<endl;
    cout<<"EasySetup Enrollee SAMPLE"<<endl;
    cout<<"#########################"<<endl;
    PrintMenu();
    char option;

    while(true)
    {
        cin>>option;
        switch (option)
        {
            case 'H': // help
            case 'h':
                PrintMenu();
                break;

            case 'Q': // quit
            case 'q':
                cout<<"quit";
                break;

            case 'S': // Enable Security
            case 's':
                EnableSecurity();
                break;

            case 'I': // Init EasySetup
            case 'i':
                StartEasySetup();
                break;

            case 'P': // start provisioning
            case 'p':
                ESInitResources();
                break;

            case 'T': // stop easy setup
            case 't':
                StopEasySetup();
                break;

            default:
                cout<<"wrong option"<<endl;
                break;
        }
        if (option == 'Q' || option == 'q') break;
    }
    return 0;
}

void *listeningFunc(void*)
{
    OCStackResult result;

    while (true)
    {
        result = OCProcess();
        if (result != OC_STACK_OK)
        {
           cout<<"OCStack stop error";
        }
    }
    return NULL;
}

