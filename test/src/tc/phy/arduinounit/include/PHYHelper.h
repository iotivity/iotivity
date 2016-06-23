/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "Arduino.h"
#include <ArduinoUnit.h>

#include "Arduino.h"

#include "logger.h"
#include "ocstack.h"
#include <string.h>

#ifdef ARDUINOWIFI
// Arduino WiFi Shield
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#else
// Arduino Ethernet Shield
#include <EthernetServer.h>
#include <Ethernet.h>
#include <Dns.h>
#include <EthernetClient.h>
#include <util.h>
#include <EthernetUdp.h>
#include <Dhcp.h>
#endif

#include "easysetup.h"

#define SSID  "EasySetup123"
#define PASSWORD  "EasySetup123"

#define MAX_BUF_LEN 1024

static bool g_OnBoardingSucceeded = false;
static bool g_ProvisioningSucceeded = false;


PROGMEM const char TAG[] = "PHYHelper";

class PHYHelper
{
public:
    //char ssid[10];
   // char passwd[10];


public:
    PHYHelper();

    //int findNetworkForOnboarding();
    int oicInitialize(ESResult expectedResult);
    int oicInitialize(OCConnectivityType networkType, const char *ssid,
            const char *passwd,bool isSecured,ESResult expectedResult);
    int InitProvisioning(ESResult expectedResult);
    int oicTerminate(ESResult expectedResult);

    static void eventCallbackInApp(ESResult eventFlag, EnrolleeState enrolleeState);
    bool checkIfCallbackCalled();

    static void PHYHelper::addFilter();
    static void getData(char *readInput, size_t bufferLength, size_t *dataLength);


};
