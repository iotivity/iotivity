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


#include "mpmErrorCode.h"
#include <string.h>
#include <memory>
#include "ocstack.h"
#include "octypes.h"
#include "ocpayload.h"
#include "hue_light.h"
#include "hue_bridge.h"
#include "messageHandler.h"
#ifndef __HUE_RESOURCE_H__
#define __HUE_RESOURCE_H__

#define DEVICE_NAME          "Philips Hue Translator"
#define DEVICE_TYPE          "oic.d.light"
#define MANUFACTURER_NAME    "Philips"
#define MAX_RESOURCES        3
#define BM                   3
#define HUE_LIGHT_URI        "/hue/"

// Structure used in Put_request to get the appropriate attribute Values
typedef struct light_resource_t
{
    bool power;
    int64_t bri;
    int64_t hue;
    int64_t sat;
    double csc[2];
} light_resource;

typedef struct
{
    char prefix[MPM_MAX_LENGTH_256];
    char lightNo[MPM_MAX_LENGTH_32];
    char bridgeMac[MPM_MAX_UNIQUE_ID_LEN];
    char lightMac[MPM_MAX_LENGTH_32];
    char lightUri[MPM_MAX_URI_LEN];
    char clientId[MPM_MAX_LENGTH_64];
    light_resource resource_state;
} hueLightDetails;

/*******************************************************************************
 * prototypes go here
 ******************************************************************************/
OCEntityHandlerResult handleEntityHandlerRequests(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest,
        std::string resourceType);

OCEntityHandlerResult entityHandler(OCEntityHandlerFlag flag,
    OCEntityHandlerRequest *entityHandlerRequest, void *callback);

OCEntityHandlerResult processPutRequest(OCEntityHandlerRequest *ehRequest,
    HueLightSharedPtr hueLight, std::string resType, OCRepPayload *payload);

OCEntityHandlerResult processGetRequest(OCRepPayload *payload, HueLightSharedPtr hueLight,
                                        std::string resType);

OCRepPayload *getCommonPayload(const char *uri, char *interfaceQuery,
                               std::string resType, OCRepPayload *payload);


/**
 * Callback is called when a bridge is authorized. It adds the bridge
 * in bridge map and inits some bridge data.
 *
 * @param[in] macAddress Mac id of the bridge
 * @param[in] clientID        Clientid of the bridge
 */
void addAuthorizedBridgeCB(const char *macAddrString, const char *ClientId);

void RemoveAuthorizedBridgeCB(const char *macAddrString);

HueLightSharedPtr getHueLightFromOCFResourceUri(std::string resourceUri);

void addReconnectLightsToBridge(hueLightDetails *plugindetails, HueBridge *bridge,
                                std::string bridgeIp);

std::string createuniqueID(std::string deviceID);

#endif /*__HUE_RESOURCE_H__*/
