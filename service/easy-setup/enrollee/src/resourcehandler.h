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

// Do not remove the include below

#include <string.h>
#include "logger.h"
#include "ocstack.h"
#include "escommon.h"
#include "octypes.h"

#ifndef ES_RESOURCE_HANDLER_H_
#define ES_RESOURCE_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ESWiFiCB) (ESResult, ESWiFiProvData *);
typedef void (*ESCloudCB) (ESResult, ESCloudProvData *);
typedef void (*ESDevConfCB) (ESResult, ESDevConfProvData *);

/* Structure to represent a Light resource */
typedef struct PROVRESOURCE
{
    OCResourceHandle handle;
    ProvStatus status; // provisiong status
    bool trigger; // Trigger network connection, 0 : Init value, 1 : Connected to the target network.
    ESLastErrCode lastErrCode;
    char errorMessage[MAX_ERRMSGLEN];
    char ocfWebLinks[MAX_WEBLINKLEN];
} ProvResource;

typedef struct
{
    OCResourceHandle handle;
    WIFI_MODE supportedMode[NUM_WIFIMODE];
    uint8_t numMode;        // the number of device's supported wifi modes
    WIFI_FREQ supportedFreq;
    char ssid[MAX_SSIDLEN]; // target network name, i.e. SSID for WLAN, MAC address for BT.
    char cred[MAX_CREDLEN]; // credential information.
    WIFI_AUTHTYPE authType;
    WIFI_ENCTYPE encType;
} WiFiResource;

typedef struct
{
    OCResourceHandle handle;
    char authCode[OIC_STRING_MAX_VALUE];
    char authProvider[OIC_STRING_MAX_VALUE];
    char ciServer[OIC_STRING_MAX_VALUE];
} CloudResource;

typedef struct
{
    OCResourceHandle handle;
    char devName[MAX_DEVICELEN];
    char language[OIC_STRING_MAX_VALUE];
    char country[OIC_STRING_MAX_VALUE];
} DevConfResource;


OCStackResult CreateEasySetupResources(bool isSecured, ESResourceMask resourceMask);
OCStackResult DeleteEasySetupResources();

OCStackResult SetDeviceProperty(ESDeviceProperty *deviceProperty);


void GetTargetNetworkInfoFromProvResource(char *, char *);
void RegisterWifiRsrcEventCallBack(ESWiFiCB);
void RegisterCloudRsrcEventCallBack(ESCloudCB);
void RegisterDevConfRsrcEventCallBack(ESDevConfCB);
void UnRegisterResourceEventCallBack(void);

#ifdef __cplusplus
}
#endif

#endif //ES_RESOURCE_HANDLER_H_
