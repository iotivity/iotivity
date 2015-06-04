//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

// Do not remove the include below
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

#include "common.h"

#ifndef ES_NETWORK_HANDLER_H_
#define ES_NETWORK_HANDLER_H_

#define MAXSSIDLEN 33
#define MAXNETCREDLEN 20
#define MAXNUMTYPE 5
#define MAXADDRLEN 15

typedef void (*NetworkEventCallback)(ES_RESULT);

enum NetworkType
{
    ES_WIFI = 1, ES_BT = 2, ES_BLE = 3, ES_ZIGBEE = 4, ES_ETH = 5
};

typedef struct NETWORKINFO
{
    NetworkType type;

    // for WiFI
    IPAddress ipaddr;
    char ssid[MAXSSIDLEN];

    // for BT, BLE
    byte mac[6];
} NetworkInfo;

ES_RESULT ConnectToWiFiNetworkForOnboarding(char *ssid, char *pass, NetworkEventCallback);
int getCurrentNetworkInfo(NetworkType targetType, NetworkInfo *info);

#endif
