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
#include "easysetup_x.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <oic_string.h>
#include "oic_malloc.h"

#define TAG "ENROLLEE_SAMPLE"

void *listeningFunc(void *);

/**
 * Secure Virtual Resource database for Iotivity Server
 * It contains Server's Identity and the PSK credentials
 * of other devices which the server trusts
 */
static char CRED_FILE[] = "oic_svr_db_server.dat";

OCPersistentStorage ps;

/**
 * @var gIsSecured
 * @brief Variable to check if secure mode is enabled or not.
 */
static bool gIsSecured = false;

UserProperties g_userProperties;

void PrintMenu()
{
    printf("============\n");
    printf("S: Enabled Security\n");
    printf("I: Init & Start EasySetup\n");
    printf("D: Set DeviceInfo\n");
    printf("T: Terminate\n");
    printf("U: set Callback for userdata\n");
    printf("Q: Quit\n");
    printf("============\n");
}

void WiFiProvCbInApp(ESWiFiProvData *eventData)
{
    printf("WiFiProvCbInApp IN\n");

    if(eventData->ssid != NULL)
    {
        printf("SSID : %s\n", eventData->ssid);
    }
    else
    {
        printf("ERROR! SSID IS NULL\n");
        return;
    }

    if(eventData->pwd != NULL)
    {
        printf("Password : %s\n", eventData->pwd);
    }
    else
    {
        printf("ERROR! Password IS NULL\n");
        return;
    }

    if(eventData->authtype == NONE_AUTH || eventData->authtype == WEP || \
        eventData->authtype == WPA_PSK || eventData->authtype == WPA2_PSK)
    {
        printf("AuthType : %d\n", eventData->authtype);
    }
    else
    {
        printf("ERROR! AuthType IS NULL\n");
        return;
    }

    if(eventData->enctype == NONE_ENC || eventData->enctype == WEP_64 || \
        eventData->enctype == WEP_128 || eventData->enctype == TKIP || \
        eventData->enctype == AES || eventData->enctype == TKIP_AES)
    {
        printf("EncType : %d\n", eventData->enctype);
    }
    else
    {
        printf("ERROR! EncType IS NULL\n");
        return;
    }

    if(eventData->userdata != NULL)
    {
        printf("userValue : %d\n", ((UserProperties *)(eventData->userdata))->userValue_int);
    }

    printf("WiFiProvCbInApp OUT\n");
}

void DevConfProvCbInApp(ESDevConfProvData *eventData)
{
    printf("DevConfProvCbInApp IN\n");

    if(eventData->language != NULL)
    {
        printf("Language : %s\n", eventData->language);
    }
    else
    {
        printf("ERROR! Language IS NULL\n");
        return;
    }

    if(eventData->country != NULL)
    {
        printf("Country : %s\n", eventData->country);
    }
    else
    {
        printf("ERROR! Country IS NULL\n");
        return;
    }

    printf("DevConfProvCbInApp OUT\n");
}

void CloudDataProvCbInApp(ESCloudProvData *eventData)
{
    printf("CloudDataProvCbInApp IN\n");

    if(eventData->authCode != NULL)
    {
        printf("AuthCode : %s\n", eventData->authCode);
    }
    else
    {
        printf("ERROR! AuthCode IS NULL\n");
        return;
    }

    if(eventData->authProvider != NULL)
    {
        printf("AuthProvider : %s\n", eventData->authProvider);
    }
    else
    {
        printf("ERROR! AuthProvider IS NULL\n");
        return;
    }

    if(eventData->ciServer != NULL)
    {
        printf("CI Server : %s\n", eventData->ciServer);
    }
    else
    {
        printf("ERROR! CI Server IS NULL\n");
        return;
    }

    printf("CloudDataProvCbInApp OUT\n");
}

ESProvisioningCallbacks gCallbacks = {
    .WiFiProvCb = &WiFiProvCbInApp,
    .DevConfProvCb = &DevConfProvCbInApp,
    .CloudDataProvCb = &CloudDataProvCbInApp
};

FILE* server_fopen(const char *path, const char *mode)
{
    (void) path;
    return fopen(CRED_FILE, mode);
}

void EnableSecurity()
{
    printf("Inside EnableSecurity API..\n");

    gIsSecured = true;

    // Initialize Persistent Storage for SVR database
    ps = (OCPersistentStorage){ server_fopen, fread, fwrite, fclose, unlink };
    OCRegisterPersistentStorageHandler(&ps);
}

void StartEasySetup()
{
    printf("StartEasySetup IN\n");

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        printf("OCStack init error!!\n");
        return;
    }

    ESResourceMask resourcemMask = ES_WIFI_RESOURCE | ES_CLOUD_RESOURCE | ES_DEVCONF_RESOURCE;
    if(ESInitEnrollee(gIsSecured, resourcemMask, gCallbacks) != ES_OK)
    {
        printf("OCStack init error!!\n");
        return;
    }
    printf("ESInitEnrollee Success\n");

    pthread_t thread_handle;
    if (pthread_create(&thread_handle, NULL, listeningFunc, NULL))
    {
        printf("Thread creation failed\n");
    }

    printf("StartEasySetup OUT\n");
}

void SetDeviceInfo()
{
    printf("SetDeviceInfo IN\n");

    ESDeviceProperty deviceProperty = {
        {{WiFi_11G, WiFi_11N, WiFi_11AC, WiFi_EOF}, WiFi_5G}, {"Test Device"}
    };

    // Set user properties if needed
    char userValue_str[] = "user_str";
    g_userProperties.userValue_int = 0;
    strcpy(g_userProperties.userValue_str, userValue_str);
    SetUserProperties(&g_userProperties);

    if(ESSetDeviceProperty(&deviceProperty) == ES_ERROR)
        printf("ESSetDeviceProperty Error\n");

    printf("SetDeviceInfo OUT\n");
}

void StopEasySetup()
{
    printf("StopEasySetup IN\n");

    if (ESTerminateEnrollee() == ES_ERROR)
    {
        printf("ESTerminateEnrollee Failed!!\n");
        return;
    }

    //stop OC Stack
    if (OCStop() != OC_STACK_OK)
    {
        printf("OCStack stop failed!!\n");
        return;
    }

    printf("StopEasySetup OUT\n");
}

void SetCallbackForUserdata()
{
    ESSetCallbackForUserdata(&ReadUserdataCb, &WriteUserdataCb);
}

int main()
{
    printf("#########################\n");
    printf("EasySetup Enrollee SAMPLE\n");
    printf("#########################\n");
    PrintMenu();
    char option;

    while(true)
    {
        if(scanf("%c", &option) != 1)
        {
            printf("Failed to read input data\n");
            continue;
        }

        if(option!= '\n')
        {
            switch (option)
            {
                case 'H': // help
                case 'h':
                    PrintMenu();
                    break;

                case 'Q': // quit
                case 'q':
                    printf("quit");
                    break;

                case 'S': // Enable Security
                case 's':
                    EnableSecurity();
                    PrintMenu();
                    break;

                case 'I': // Init EasySetup
                case 'i':
                    StartEasySetup();
                    PrintMenu();
                    break;

                case 'D': // Set Device Info
                case 'd':
                    SetDeviceInfo();
                    PrintMenu();
                    break;
                case 'T': // stop easy setup
                case 't':
                    StopEasySetup();
                    PrintMenu();
                    break;
                case 'U': // set callback
                case 'u':
                    SetCallbackForUserdata();
                    PrintMenu();
                    break;

                default:
                    printf("wrong option\n");
                    PrintMenu();
                    break;
            }
            if (option == 'Q' || option == 'q') break;
        }
    }
    return 0;
}

void *listeningFunc(void * data)
{
    (void)data;
    OCStackResult result;

    while (true)
    {
        result = OCProcess();
        if (result != OC_STACK_OK)
        {
           printf("OCStack stop error");
        }
    }
    return NULL;
}

