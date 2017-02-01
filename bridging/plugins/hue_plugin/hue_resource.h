//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
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
//


#include "gw_error_code.h"
#include <string.h>
#include <memory>
#include "ocstack.h"
#include "octypes.h"
#include "ocpayload.h"
#include "hue_light.h"
#include "hue_bridge.h"
#include "message_handler.h"
#ifndef __HUE_RESOURCE_H__
#define __HUE_RESOURCE_H__

#define DEVICE_NAME "Philips Hue Translator"
#define DEVICE_TYPE "oic.d.light"
#define MANUFACTURER_NAME        "Philips"
#define MAX_RESOURCES 3
#define BM 3
#define HUE_LIGHT_URI "/hue/"
#define SWITCH "/switch"
#define BRIGHTNESS "/brightness"
#define CHROMA "/chroma"

// Structure used in Put_request to get the appropriate attribute Values
typedef struct light_resource_t
{
    bool power;
    int64_t bri;
    int64_t hue;
    int64_t sat;
    double csc[2];
}light_resource;

typedef struct
{
    char prefix[MAX_LENGTH_256];
    char lightNo[MAX_LENGTH_32];
    char bridgeMac[MAX_UNIQUE_ID_LEN];
    char lightMac[MAX_LENGTH_32];
    char lightUri[MAX_URI_LEN];
    char clientId[MAX_LENGTH_64];
    light_resource resource_state;
}LightDetails;

/*******************************************************************************
 * prototypes go here
 ******************************************************************************/
OCEntityHandlerResult handle_EH_requests(OCEntityHandlerFlag flag,
                                         OCEntityHandlerRequest *entityHandlerRequest,
                                         std::string resourceType);

OCEntityHandlerResult binary_switch_EH(OCEntityHandlerFlag flag,
                                       OCEntityHandlerRequest *entityHandlerRequest,
                                       void *callbackParam);

OCEntityHandlerResult brightness_EH(OCEntityHandlerFlag flag,
                                    OCEntityHandlerRequest *entityHandlerRequest,
                                    void *callbackParam);

OCEntityHandlerResult chroma_EH(OCEntityHandlerFlag flag,
                                OCEntityHandlerRequest *entityHandlerRequest,
                                void *callbackParam);

OCEntityHandlerResult process_put_request(OCEntityHandlerRequest *ehRequest, HueLightSharedPtr hueLight, std::string resType, OCRepPayload *payload);

OCEntityHandlerResult process_get_request(OCRepPayload *payload, HueLightSharedPtr hueLight, std::string resType);

OCRepPayload *get_common_payload(const char *uri, HueLightSharedPtr hueLight, char *interfaceQuery,std::string resType, OCRepPayload *payload);

void *hue_discovery_thread(void *pointer);

/*define the authorization callbacks*/
void addAuthorizedBridgeCB(const char *macAddrString,const char *ClientId);

void RemoveAuthorizedBridgeCB(const char *macAddrString);

HueLightSharedPtr getHueLightFromOCFResourceUri(std::string resourceUri);

void addReconnectLightsToBridge(LightDetails *plugindetails, HueBridge *bridge, std::string bridgeIp);

std::string createuniqueID(std::string deviceID);

#endif /*__HUE_RESOURCE_H__*/
