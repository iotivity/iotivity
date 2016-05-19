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
#include "networkhandler.h"
#include "octypes.h"

#ifndef ES_RESOURCE_HANDLER_H_
#define ES_RESOURCE_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ESEnrolleeResourceEventCallback)(ESResult);

/* Structure to represent a Light resource */
typedef struct PROVRESOURCE
{
    OCResourceHandle handle;
    int64_t ps; // provisiong status, 1 : need to provisioning, 2 : Connected to Enroller.
    int64_t tr; // Trigger network connection, 0 : Init value, 1 : Connected to the target network.
    int64_t tnt; // target network type, 1: WLAN, 2: BT, 3: BLE, 4: Zigbee.
    char tnn[MAXSSIDLEN]; // target network name, i.e. SSID for WLAN, MAC address for BT.
    char cd[MAXNETCREDLEN]; // credential information.
} ProvResource;

/* Structure to represent a Light resource */
typedef struct NETRESOURCE
{
    OCResourceHandle handle;
    int64_t cnt; // current network type, 1: WLAN, 2: BT, 3: BLE, 4: Zigbee.
    int64_t ant[MAXNUMTYPE]; // available network type, 1: WLAN, 2: BT, 3: BLE, 4: Zigbee.
    char ipaddr[MAXADDRLEN]; // ip address.
    char cnn[MAXSSIDLEN]; // current network name.
} NetResource;

OCStackResult CreateProvisioningResource(bool isSecured);
OCStackResult DeleteProvisioningResource();

void GetTargetNetworkInfoFromProvResource(char *, char *);
void RegisterResourceEventCallBack(ESEnrolleeResourceEventCallback);
void UnRegisterResourceEventCallBack(void);

#ifdef __cplusplus
}
#endif

#endif //ES_RESOURCE_HANDLER_H_