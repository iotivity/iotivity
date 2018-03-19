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

/* This file contains plugin specific code that adapts the native resource model
 * of native devices into the resource model of OIC.  The file is divided into two
 * sections; first plugin specific entry points are implemented followed by the
 * implementation of the resource entity handler required by the IoTivity implementation
 * for each resource.
 *
 * NOTE: This file is plumbed ready for dynamic resource additions.  There is a
 * thread provided to manage the devices.  When a resource is found it is added
 * to a work queue which is serviced by the plugin process function.  The plugin
 * process function is a thread safe place for the plugin specific code to call
 * OIC APIs.
 */

#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <string.h>
#include <string>
#include <math.h>
#include <signal.h>
#include <pthread.h>
#include <iostream>
#include <map>
#include <mutex>
#include "experimental/logger.h"
#include "mpmErrorCode.h"
#include "pluginServer.h"
#include "hue_light.h"
#include "hue_auth_spec.h"
#include "hue_light.h"
#include "hue_file.h"
#include "hue_bridge.h"
#include "curlClient.h"
#include "oic_string.h"
#include "oic_malloc.h"
#include "hue_resource.h"
#include "iotivity_config.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include "messageHandler.h"
#include "ConcurrentIotivityUtils.h"
#include "IotivityWorkItem.h"
#include "cbor.h"

/*******************************************************************************
 * Pound defines and structure declarations go here
 ******************************************************************************/
#define         MAX_QUERY_STRING                200
#define         TAG                            "HUE_RESOURCE"

using namespace OC::Bridging;

/*******************************************************************************
 * global data goes here
 ******************************************************************************/
MPMPluginCtx *g_pluginCtx = NULL;
std::mutex addedLightsLock;
std::mutex authorizedBridgesLock;

std::map<std::string, HueBridge> authorizedBridges;
typedef std::map<std::string, HueBridge>::iterator bridgeItr;

std::map<std::string, HueLightSharedPtr> g_discoveredLightsMap;
std::map<std::string, HueLightSharedPtr> addedLights;
static void *hueDiscoveryThread(void *pointer);

const std::string HUE_SWITCH_RESOURCE_TYPE = "oic.r.switch.binary";
const std::string HUE_BRIGHTNESS_RESOURCE_TYPE = "oic.r.light.brightness";
const std::string HUE_CHROMA_RESOURCE_TYPE = "oic.r.colour.chroma";
const std::string SWITCH_RELATIVE_URI = "/switch";
const std::string BRIGHTNESS_RELATIVE_URI = "/brightness";
const std::string CHROMA_RELATIVE_URI = "/chroma";

const uint BINARY_SWITCH_CALLBACK = 0;
const uint BRIGHTNESS_CALLBACK = 1;
const uint CHROMA_CALLBACK = 2;

FILE *hue_fopen(const char *path, const char *mode)
{
    std::string filename = std::string("hue_") + path;
    return fopen(filename.c_str(), mode);
}

MPMResult pluginCreate(MPMPluginCtx **pluginSpecificCtx)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    if (g_pluginCtx == NULL)
    {
        *pluginSpecificCtx = NULL;

        /* allocate a context structure for the plugin */
        MPMPluginCtx *ctx = (MPMPluginCtx *) OICMalloc(sizeof(MPMPluginCtx));

        /* initialize the plugin context */
        if (ctx != NULL)
        {
            memset(ctx, 0, sizeof(MPMPluginCtx));
            *pluginSpecificCtx = ctx;
            g_pluginCtx = ctx;
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Unable to allocate plugin specific context");
            goto exit;
        }

        ctx->device_name = DEVICE_NAME;
        ctx->resource_type = DEVICE_TYPE;
        ctx->open = hue_fopen;

        result = MPM_RESULT_OK;

    }
    else
    {
        result = MPM_RESULT_ALREADY_CREATED;
    }
exit:
    OIC_LOG_V(INFO, TAG, "Plugin create return value:%d.", result);

    /*
     * NOTE: What do we do if the create for some reason failed.  To we assume that the destroy
     * will be called if the create fails??  Let let the plugin loader pick up the pieces by
     * calling destroy on an imperfectly created plugin.  Calling entry point APIs from within
     * the implementation can cause some real problems (e.g. deadlock situations).
     */

    return result;
}

MPMResult pluginStart(MPMPluginCtx *ctx)
{
    MPMResult result = MPM_RESULT_STARTED_FAILED;
    int error = 0;
    if (ctx == NULL || g_pluginCtx == NULL)
    {
        goto exit;
    }
    if (ctx->started)
    {
        result = MPM_RESULT_ALREADY_STARTED;
        goto exit;
    }

    result = hueInit(ctx, addAuthorizedBridgeCB, RemoveAuthorizedBridgeCB);
    if (MPM_RESULT_OK == result)
    {
        /*start bridge discovery*/
        if (DiscoverHueBridges() != MPM_RESULT_OK)
        {
            // DiscoverBridges if fails we try again in discovery thread, so don't return failure
            OIC_LOG(ERROR, TAG, "DiscoverBridges failed");
        }
        else
        {
            OIC_LOG(INFO, TAG, " DiscoverBridges succeeded");
        }
        /* create house keeping thread */
        ctx->stay_in_process_loop = true;

        error = pthread_create(&(ctx->thread_handle), NULL,
                               hueDiscoveryThread, ctx);
        if (error == 0)
        {
            ctx->started = true;
            result = MPM_RESULT_OK;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "Can't create plugin specific thread :[%s]",
                      strerror(errno));
            pluginStop(ctx);
            result = MPM_RESULT_STARTED_FAILED;
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "hueAuthCreate Failed. Cannot create plugin");
    }
exit:
    OIC_LOG_V(INFO, TAG, "Plugin start return value:%d.", result);
    return result;
}

// Checks if brightness has changed in the 0 - 100 range for OCF light.
bool hasBrightnessChangedInOCFScale(const HueLight::light_state_t &stateprev,
                                    const HueLight::light_state_t &statenew)
{

    uint16_t ocfBrightnessPrev = stateprev.bri / 2.54;
    uint16_t ocfBrightnessNew = statenew.bri / 2.54;
    return ocfBrightnessNew != ocfBrightnessPrev;
}

bool isSecureEnvSet()
{
    char *non_secure_env = getenv("NONSECURE");

    if (non_secure_env != NULL && (strcmp(non_secure_env, "true")) == 0)
    {
        OIC_LOG(INFO, TAG, "Creating NON SECURE resources");
        return false;
    }
    OIC_LOG(INFO, TAG, "Creating SECURE resources");
    return true;
}

MPMResult createPayloadForMetaData(MPMResourceList **list, const std::string &configURI,
                              const std::string rt, const std::string res_if)
{
    MPMResourceList *temp = (MPMResourceList *)OICCalloc(1, sizeof(MPMResourceList));
    if (temp == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Calloc failed for createPayloadForMetaData %s", strerror(errno));
        return MPM_RESULT_OUT_OF_MEMORY;
    }

    OICStrcpy(temp->rt, MPM_MAX_LENGTH_64, rt.c_str());
    OICStrcpy(temp->href, MPM_MAX_URI_LEN, configURI.c_str());
    OICStrcpy(temp->interfaces, MPM_MAX_LENGTH_64, res_if.c_str());
    temp->bitmap = BM;

    temp->next = *list;
    *list = temp;
    return MPM_RESULT_OK;
}

void addAuthorizedBridgeCB(const char *macAddress, const char *ClientId)
{
    HueBridge bridge; HueBridge::hue_bridge_data_tag bridgeData;
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    if (authorizedBridges.find(macAddress) == authorizedBridges.end())
    {
        uint32_t prefix_size = MAX_QUERY_STRING;
        char *prefix = (char *) OICMalloc(prefix_size);
        if (NULL == prefix)
        {
            OIC_LOG_V(INFO, TAG, " Failed to malloc prefix");
            return;
        }

        /*get prefix for discovering lights*/
        result = hueAuthGetHttpPrefix(prefix, &prefix_size, macAddress, ClientId);
        if (result == MPM_RESULT_INSUFFICIENT_BUFFER)
        {
            char *tmp = (char *) OICRealloc(prefix, prefix_size);
            if (NULL == tmp)
            {
                OIC_LOG_V(INFO, TAG, " Failed to realloc prefix");
                OICFree(prefix);
                return;
            }
            prefix = tmp;
            result = hueAuthGetHttpPrefix(prefix, &prefix_size, macAddress, ClientId);
        }
        if (result == MPM_RESULT_OK)
        {
            bridge.setBridgeCurlQuery(prefix);
            bridge.getBridgeConfigFromCloud();
            bridge.getBridgeConfig(bridgeData);
            OIC_LOG_V(DEBUG, TAG,
                      " \t\n\nBRIDGE AUTHORIZED\nclientID: %s\nip : %s\nmac: %s\nname : %s\nsw : %s\n", ClientId,
                      bridgeData.ip.c_str(), bridgeData.mac.c_str(), bridgeData.name.c_str(),
                      bridgeData.swVersion.c_str() );
            OIC_LOG_V(INFO, TAG,  " \n Curl prefix for bridge is %s \n", prefix);
        }
        else
        {
            OIC_LOG_V(INFO, TAG,  " Failed To Authorize the bridge bridge - %s", macAddress);
            OICFree(prefix);
            return;
        }

        authorizedBridges[macAddress] = bridge;
        OICFree(prefix);
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "Bridge is already authorized \n");
    }
}

MPMResult pluginScan(MPMPluginCtx *, MPMPipeMessage *)
{
    OIC_LOG(DEBUG, TAG,  "Inside Plugin  scan");
    std::string uri, uniqueId ;
    HueLight::light_config_t config;
    HueLight::light_state_t state;

    std::lock_guard<std::mutex> lock(authorizedBridgesLock);
    /*iterate for every bridge in the authorized bridge map*/
    for (bridgeItr it = authorizedBridges.begin(); it != authorizedBridges.end(); it++)
    {
        HueBridge *bridge = &(it->second);
        if (bridge == NULL)
        {
            continue;
        }
        // now start a new discovery and get new lights

        HueLight::lights lightsScanned;
        bridge->discoverHueLights();
        bridge->getScannedLights(lightsScanned);
        for (uint32_t i = 0; i < lightsScanned.size(); ++i)
        {
            HueLightSharedPtr light = lightsScanned[i];
            light->getConfig(config);
            light->getState(state);

            if (!state.reachable)
            {
                OIC_LOG(INFO, TAG, "Ignoring OFFLINE light");
                continue;
            }
            uniqueId = createuniqueID(config.uniqueId);
            uri = (HUE_LIGHT_URI + uniqueId) ;

            OIC_LOG_V(INFO, TAG,
                      "Found Reachable Light - light name=%s, id=%s, reachable=%d",
                      config.name.c_str(), config.uniqueId.c_str(), state.reachable);

            if (addedLights.find(uri) != addedLights.end())
            {
                OIC_LOG_V(INFO, TAG, "Already Added %s. Ignoring", uri.c_str());
                continue;
            }

            g_discoveredLightsMap[uri] = light;

            MPMSendResponse(uri.c_str(), uri.size(), MPM_SCAN);
        }
    }
    return MPM_RESULT_OK;
}

/**
 *  CreateuniqueID - Creates the unique id by removing Delimiters..
 *  @param[in] deviceId - Unique Id of the Device
 *  @return unique id without any delimiters
 */
std::string createuniqueID(std::string deviceId)
{
    std::string uniqueId(deviceId);
    std::string token = "";
    std::string delimiter1 = ":";
    std::string delimiter2 = "-";
    size_t pos = 0;

    while ( (pos = uniqueId.find(delimiter1)) != std::string::npos)
    {
        uniqueId.replace(pos, 1, token);
    }
    while ( (pos = uniqueId.find(delimiter2)) != std::string::npos)
    {
        uniqueId.replace(pos, 3, token);
    }
    return uniqueId;
}

void createOCFResources(std::string uri)
{
    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    if (isSecureEnvSet())
    {
        resourceProperties |= OC_SECURE;
    }

    ConcurrentIotivityUtils::queueCreateResource(uri + SWITCH_RELATIVE_URI,
                    HUE_SWITCH_RESOURCE_TYPE.c_str(), OC_RSRVD_INTERFACE_ACTUATOR,
                    entityHandler, (void *) BINARY_SWITCH_CALLBACK, resourceProperties);

    ConcurrentIotivityUtils::queueCreateResource(uri + BRIGHTNESS_RELATIVE_URI,
             HUE_BRIGHTNESS_RESOURCE_TYPE.c_str(), OC_RSRVD_INTERFACE_ACTUATOR,
             entityHandler, (void *) BRIGHTNESS_CALLBACK, resourceProperties);


    ConcurrentIotivityUtils::queueCreateResource(uri + CHROMA_RELATIVE_URI,
            HUE_CHROMA_RESOURCE_TYPE.c_str(), OC_RSRVD_INTERFACE_ACTUATOR,
            entityHandler, (void *) CHROMA_CALLBACK, resourceProperties);
}

MPMResult pluginAdd(MPMPluginCtx *, MPMPipeMessage *message)
{
    if (message->payloadSize <= 0 && message->payload == NULL)
    {
        OIC_LOG(ERROR, TAG, "No payload received, failed to add device");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    MPMResourceList *list  = NULL;
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    std::string uri = reinterpret_cast<const char *>(message->payload);

    if (addedLights.find(uri) != addedLights.end())
    {
        OIC_LOG_V(ERROR, TAG, "%s already added", uri.c_str());
        return MPM_RESULT_ALREADY_CREATED;
    }
    if (g_discoveredLightsMap.find(uri) == g_discoveredLightsMap.end())
    {
        OIC_LOG_V(ERROR, TAG, "%s was NOT discovered in a scan", uri.c_str());
        return result;
    }

    std::lock_guard<std::mutex> lock(addedLightsLock);
    addedLights[uri] = g_discoveredLightsMap[uri];

    uint8_t *buff = (uint8_t *)OICCalloc(1, MPM_MAX_METADATA_LEN);
    if (buff == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Calloc failed %s", strerror(errno));
        return MPM_RESULT_OUT_OF_MEMORY;
    }
    size_t size = MPM_MAX_METADATA_LEN;
    HueLightSharedPtr light; hueFile bridgeCtx;
    hueLightDetails deviceDetails;
    HueLight::light_config_t config;
    MPMDeviceSpecificData deviceConfiguration;
    memset(&deviceDetails, 0, sizeof(hueLightDetails));
    memset(&deviceConfiguration, 0, sizeof(MPMDeviceSpecificData));

    // Create Resources and form metadata for RECONNECT

    createOCFResources(uri);

    result = createPayloadForMetaData(&list, uri+SWITCH_RELATIVE_URI,
                HUE_SWITCH_RESOURCE_TYPE.c_str(), OC_RSRVD_INTERFACE_ACTUATOR);

    result= createPayloadForMetaData(&list, uri + BRIGHTNESS_RELATIVE_URI,
                HUE_BRIGHTNESS_RESOURCE_TYPE.c_str(),OC_RSRVD_INTERFACE_ACTUATOR);

    result = createPayloadForMetaData(&list, uri + CHROMA_RELATIVE_URI,
                HUE_CHROMA_RESOURCE_TYPE.c_str(), OC_RSRVD_INTERFACE_ACTUATOR);

    if(result != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, " Failed creating payload for metadata");
        return result;
    }

    light = g_discoveredLightsMap[uri];
    light->getConfig(config);

    std::string data;
    data =  light->getBridgeMac();
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
    OICStrcpy(deviceDetails.bridgeMac, MPM_MAX_UNIQUE_ID_LEN, data.c_str());
    deviceDetails.bridgeMac[MPM_MAX_UNIQUE_ID_LEN - 1] = '\0';

    OICStrcpy(deviceDetails.lightMac, MPM_MAX_LENGTH_32, config.uniqueId.c_str());
    OICStrcpy(deviceDetails.lightUri, MPM_MAX_URI_LEN, config.uri.c_str());
    OICStrcpy(deviceDetails.prefix, MPM_MAX_LENGTH_256, light->getUri().c_str());
    OICStrcpy(deviceDetails.lightNo, MPM_MAX_LENGTH_32, light->getShortId().c_str());

    findAuthorizedBridge(deviceDetails.bridgeMac, NULL, bridgeCtx);
    OICStrcpy(deviceDetails.clientId, MPM_MAX_LENGTH_64, bridgeCtx.clientID);


    OICStrcpy(deviceConfiguration.devName, MPM_MAX_LENGTH_64, DEVICE_NAME);
    OICStrcpy(deviceConfiguration.devType, MPM_MAX_LENGTH_64, DEVICE_TYPE);
    OICStrcpy(deviceConfiguration.manufacturerName, MPM_MAX_LENGTH_256, MANUFACTURER_NAME);
    MPMFormMetaData(list, &deviceConfiguration, buff, size, &deviceDetails, sizeof(deviceDetails));

    MPMAddResponse response;
    memset(&response, 0, sizeof(MPMAddResponse));
    OICStrcpy(response.uri, MPM_MAX_URI_LEN, uri.c_str());
    memcpy(response.metadata, buff, MPM_MAX_METADATA_LEN);
    size_t response_size = sizeof(MPMAddResponse);

    MPMSendResponse(&response, response_size, MPM_ADD);

    OICFree(buff);

    return MPM_RESULT_OK;
}

MPMResult pluginRemove(MPMPluginCtx *, MPMPipeMessage *message)
{
    if (message->payloadSize <= 0 && message->payload == NULL)
    {
        OIC_LOG(ERROR, TAG, "No paylaod received, failed to remove device");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    std::string uri = reinterpret_cast<const char*>(message->payload);
    OIC_LOG_V(DEBUG, TAG, "device uri to be removed - %s ", uri.c_str());

    std::lock_guard<std::mutex> lock(addedLightsLock);
    if (addedLights.find(uri) == addedLights.end())
    {
        OIC_LOG(ERROR, TAG, "Device to be removed is not added yet");
        return MPM_RESULT_NOT_PRESENT;
    }

    ConcurrentIotivityUtils::queueDeleteResource(uri + SWITCH_RELATIVE_URI);
    ConcurrentIotivityUtils::queueDeleteResource(uri + BRIGHTNESS_RELATIVE_URI);
    ConcurrentIotivityUtils::queueDeleteResource(uri + CHROMA_RELATIVE_URI);

    addedLights.erase(uri);

    MPMSendResponse(uri.c_str(), uri.size(), MPM_REMOVE);

    return MPM_RESULT_OK;
}

MPMResult pluginReconnect(MPMPluginCtx *, MPMPipeMessage *message)
{
    if (message->payloadSize <= 0 && message->payload == NULL)
    {
        OIC_LOG(ERROR, TAG, "No paylaod received, failed to reconnect");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    hueLightDetails *plugindetails = NULL;
    void *details = NULL;
    HueDiscoveredCtx discoveredCtx;
    std::size_t pos = 0;
    std::string light_Prefix, ip, light_mac, light_no, uri;
    HueBridge bridge;
    MPMResourceList *list = NULL, *temp = NULL;

    MPMParseMetaData(message->payload, MPM_MAX_METADATA_LEN, &list, &details);
    plugindetails = (hueLightDetails *) details;

    // Find Bridge ip and light id;
    light_no = plugindetails->lightNo;
    light_Prefix = plugindetails->prefix;
    pos = light_Prefix.find("/");
    ip = light_Prefix.substr(0, (pos));

    OIC_LOG_V(DEBUG, TAG,
              " \n\n Reconnect meta data \n\n Bridge Mac - %s\n Light Mac - %s\nip - %s\n Client ID -%s\n Light no - %s\n"
              "\n prefix - %s \n ", plugindetails->bridgeMac, plugindetails->lightMac, ip.c_str(),
              plugindetails->clientId,
              plugindetails->lightNo, plugindetails->prefix);

    if (('\0' != plugindetails->bridgeMac[0]) && ('\0' != plugindetails->clientId[0]))
    {
        if (authorizedBridges.find(plugindetails->bridgeMac) == authorizedBridges.end())
        {
            memset(&discoveredCtx, 0, sizeof(HueDiscoveredCtx));
            if (false == findDiscoveredBridge(plugindetails->bridgeMac, &discoveredCtx))
            {
                OICStrcpy(discoveredCtx.macAddrString, MAX_STRING - 1, plugindetails->bridgeMac);
                OICStrcpy(discoveredCtx.ipAddrString, MAX_STRING - 1, ip.c_str());
                OICStrcpy(discoveredCtx.clientIDs, MAX_STRING * MAX_CLIENTS, plugindetails->clientId);
                discoveredCtx.numClients = 1;
                addAuthorizedBridge(plugindetails->bridgeMac, plugindetails->clientId);
                result = addDiscoveredBridge(discoveredCtx);
            }
            else
            {
                updateDiscoverBridgeDetails(plugindetails->bridgeMac, plugindetails->clientId);
            }
            uint32_t prefix_size = MAX_QUERY_STRING;
            char *prefix = (char *) OICMalloc(prefix_size);
            if (NULL == prefix)
            {
                OIC_LOG_V(INFO, TAG, " Failed to malloc prefix");
                return MPM_RESULT_INTERNAL_ERROR;
            }
            result = hueAuthGetHttpPrefix(prefix, &prefix_size, plugindetails->bridgeMac,
                                          plugindetails->clientId);
            if (result == MPM_RESULT_INSUFFICIENT_BUFFER)
            {
                char *tmp = (char *) OICRealloc(prefix, prefix_size);
                if (NULL == tmp)
                {
                    OIC_LOG(DEBUG, TAG, "Failed to realloc prefix");
                    OICFree(prefix);
                    return MPM_RESULT_INTERNAL_ERROR;
                }
                prefix = tmp;
                result = hueAuthGetHttpPrefix(prefix, &prefix_size, plugindetails->bridgeMac,
                                              plugindetails->clientId);
            }
            if (result != MPM_RESULT_OK)
            {
                OIC_LOG(DEBUG, TAG, "hueAuthGetHttpPrefix failed");
                OICFree(prefix);
                return result;
            }
            bridge.setBridgeMAC(plugindetails->bridgeMac);
            bridge.setBridgeCurlQuery(prefix);
            authorizedBridges[plugindetails->bridgeMac] = bridge;
            OICFree(prefix);
        }
    }
    for (bridgeItr it = authorizedBridges.begin(); it != authorizedBridges.end(); it++)
    {
        HueBridge *authorizedbridge = &(it->second);
        std::string data;
        data =  authorizedbridge->getBridgeMAC();
        std::transform(data.begin(), data.end(), data.begin(), ::tolower);
        if (plugindetails->bridgeMac == data)
        {
            OIC_LOG(DEBUG, TAG, "Bridge Found and is authorized");
            addReconnectLightsToBridge(plugindetails, authorizedbridge, ip);
            result = MPM_RESULT_OK;
        }
    }

    while (list)
    {
        temp = list;
        list = list->next;
        OICFree(temp);
    }
    free(plugindetails);
    return MPM_RESULT_OK;
}

void addReconnectLightsToBridge(hueLightDetails *plugindetails, HueBridge *bridge,
                                std::string bridgeIp)
{
    HueLight::light_config_t config;
    std::string uuid, uri;
    OIC_LOG(INFO, TAG, " RECONNECTING ALL THE LIGHTS.......");
    std::shared_ptr<HueLight> light = std::make_shared<HueLight>(plugindetails->prefix, bridgeIp,
                                      plugindetails->bridgeMac, plugindetails->lightNo, "NULL");
    if (!light)
    {
        OIC_LOG(ERROR, TAG, " Pointer returned NULL to the light object");
        return;
    }
    config.uri = plugindetails->lightUri;
    config.uniqueId = plugindetails->lightMac;
    light->setConfig(config);
    bridge->fillLightDetails(light);

    uuid = createuniqueID(config.uniqueId);
    uri = (HUE_LIGHT_URI + uuid );
    createOCFResources(uri);

    g_discoveredLightsMap[uri] = light;
    addedLights[uri] = light;
}

/*
 * Removes bridge from the authorized bridge map.
 */
void RemoveAuthorizedBridgeCB(const char *macAddrString)
{
    OIC_LOG_V(INFO, TAG, "Remove Bridge called for bridge = %s", macAddrString);
    std::lock_guard<std::mutex> lock(authorizedBridgesLock);
    bridgeItr it = authorizedBridges.find(macAddrString);
    if (it != authorizedBridges.end())
    {
        /*remove the bridge*/
        authorizedBridges.erase(it);
    }
    return;
}


/*  Plugin specific entry-point function to stop the plugin's threads
 *
 * returns:
 *     MPM_RESULT_OK             - no errors
 *     MPM_RESULT_INTERNAL_ERROR - stack process error
 */
MPMResult pluginStop(MPMPluginCtx *ctx)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    if (NULL != ctx && g_pluginCtx != NULL)
    {
        result = MPM_RESULT_OK;
        //stop the presence before stopping the plugin
        OCStopPresence();

        if (ctx->started == true)
        {
            ctx->stay_in_process_loop = false;
            pthread_join(ctx->thread_handle, NULL);
            ctx->started = false;
        }

        //destroy the resources
        hueAuthDestroy();
        clearBridgeDetails();
    }
    OIC_LOG_V(INFO, TAG, "Plugin stop: OUT - return value:%d", result);

    return (result);
}

MPMResult pluginDestroy(MPMPluginCtx *ctx)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    if (ctx != NULL && g_pluginCtx != NULL)
    {
        result = MPM_RESULT_OK;
        if (ctx->started == true)
        {
            result = pluginStop(ctx);
        }

        /* freeing the resource allocated in create */
        OICFree(ctx);
        g_pluginCtx = NULL;
    }

    OIC_LOG_V(INFO, TAG, "Plugin destroy's return value:%d", result);

    return (result);
}

OCEntityHandlerResult handleEntityHandlerRequests(
    OCEntityHandlerFlag ,
    OCEntityHandlerRequest *entityHandlerRequest,
    std::string resourceType)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCRepPayload *responsePayload = NULL;
    OCRepPayload *payload = OCRepPayloadCreate();

    try
    {
        if (entityHandlerRequest == NULL)
        {
            throw "Entity handler received a null entity request context" ;
        }

        std::string uri = OCGetResourceUri(entityHandlerRequest->resource);
        HueLightSharedPtr hueLight = getHueLightFromOCFResourceUri(uri);
        char *interfaceQuery = NULL;
        char *resourceTypeQuery = NULL;
        char *dupQuery = OICStrdup(entityHandlerRequest->query);
        if (dupQuery)
        {
            MPMExtractFiltersFromQuery(dupQuery, &interfaceQuery, &resourceTypeQuery);
        }

        switch (entityHandlerRequest->method)
        {
            case OC_REST_GET:
                OIC_LOG_V(INFO, TAG, " GET Request for: %s", uri.c_str());
                ehResult = processGetRequest(payload, hueLight, resourceType);
                break;

            case OC_REST_PUT:
            case OC_REST_POST:

                OIC_LOG_V(INFO, TAG, "PUT / POST Request on %s", uri.c_str());
                ehResult = processPutRequest(entityHandlerRequest, hueLight, resourceType, payload);

                //  To include "if" in all payloads.
                interfaceQuery = (char *) OC_RSRVD_INTERFACE_DEFAULT;
                break;

            default:
                OIC_LOG_V(ERROR, TAG, "UnSupported Method [%d] Received ", entityHandlerRequest->method);
                ConcurrentIotivityUtils::respondToRequestWithError(entityHandlerRequest, " Unsupported Method",
                        OC_EH_METHOD_NOT_ALLOWED);
                 return OC_EH_OK;
        }
        responsePayload = getCommonPayload(uri.c_str(),interfaceQuery, resourceType, payload);
        ConcurrentIotivityUtils::respondToRequest(entityHandlerRequest, responsePayload, ehResult);
        OICFree(dupQuery);
    }
    catch (const char *errorMessage)
    {
        OIC_LOG_V(ERROR, TAG, "Error - %s ", errorMessage);
        ConcurrentIotivityUtils::respondToRequestWithError(entityHandlerRequest, errorMessage, OC_EH_ERROR);
        ehResult = OC_EH_OK;
    }

    OCRepPayloadDestroy(responsePayload);
    return ehResult;
}

// Entity handler for binary switch
OCEntityHandlerResult entityHandler(OCEntityHandlerFlag flag,
            OCEntityHandlerRequest *entityHandlerRequest, void *callback)
{
    uintptr_t callbackParamResourceType = (uintptr_t)callback;
    std::string resourceType;

    if (callbackParamResourceType == BINARY_SWITCH_CALLBACK)
    {
        resourceType = HUE_SWITCH_RESOURCE_TYPE;
    }
    else if (callbackParamResourceType == BRIGHTNESS_CALLBACK)
    {
        resourceType = HUE_BRIGHTNESS_RESOURCE_TYPE;
    }
    else
    {
        resourceType = HUE_CHROMA_RESOURCE_TYPE;
    }
    return handleEntityHandlerRequests(flag, entityHandlerRequest, resourceType);
}

OCEntityHandlerResult processGetRequest(OCRepPayload *payload, HueLightSharedPtr hueLight,
                                        std::string resType)
{
    HueLight::light_state_t light_state;
    hueLight->getState(light_state);

    if (payload == NULL)
    {
        throw "payload is null";
    }

    if (HUE_SWITCH_RESOURCE_TYPE == resType)
    {
        if (!OCRepPayloadSetPropBool(payload, "value", light_state.power))
        {
            throw "Failed to set 'value' (power) in payload";
        }
        OIC_LOG_V(INFO, TAG, "Light State: %s", light_state.power ? "true" : "false");
    }
    else if (HUE_BRIGHTNESS_RESOURCE_TYPE == resType)
    {
        uint8_t ocfBrightness = light_state.bri / 2.54;

        if (!OCRepPayloadSetPropInt(payload, "brightness", ocfBrightness))
        {
            throw "Failed to set 'brightness' in payload";
        }
        OIC_LOG_V(INFO, TAG, " Brightness State (Hue Bulb): %" PRIu64 " Brightness(OCF) : %d",
                  light_state.bri, ocfBrightness);
    }
    else if (HUE_CHROMA_RESOURCE_TYPE == resType)
    {
        if (!OCRepPayloadSetPropInt(payload, "hue", light_state.hue) ||
            !OCRepPayloadSetPropInt(payload, "saturation", light_state.sat))
        {
            throw "Failed to set 'hue' or 'saturation' in payload" ;
        }
        size_t csc_dimensions[MAX_REP_ARRAY_DEPTH] = {2, 0, 0};
        if (!OCRepPayloadSetDoubleArray(payload, "csc", light_state.csc, csc_dimensions))
        {
            throw  "Failed to set csc in payload" ;
        }
        OIC_LOG_V(INFO, TAG, "hue: %" PRIu64 ", sat: %" PRIu64 ", csc: [%f, %f] in payload.",
                  light_state.hue,
                  light_state.sat,
                  light_state.csc[0],
                  light_state.csc[1]);
    }
    else
    {
        throw "Failed due to unkwown resource type";
    }
    return OC_EH_OK;
}

OCEntityHandlerResult processPutRequest(OCEntityHandlerRequest *ehRequest,
                                        HueLightSharedPtr hueLight, std::string resourceType,
                                        OCRepPayload *payload)
{

    if (!ehRequest || !ehRequest->payload ||
                                    ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        throw "Incoming payload is NULL or not a representation";
    }

    OCRepPayload *input = reinterpret_cast<OCRepPayload *>(ehRequest->payload);
    if (!input)
    {
        throw "PUT payload is null";
    }
    HueLight::light_state_t state;
    light_resource_t light_resource;
    if (hueLight->getState(state, true) != MPM_RESULT_OK)
    {
        throw "Error Getting light. Aborting PUT" ;
    }

    if (HUE_SWITCH_RESOURCE_TYPE ==  resourceType)
    {
        if (!OCRepPayloadGetPropBool(input, "value", &light_resource.power))
        {
            throw "No value (power) in representation" ;
        }
        OIC_LOG_V(INFO, TAG, "PUT/POST value (power):%s", light_resource.power ? "true" : "false");
        state.power = light_resource.power;
        if (!OCRepPayloadSetPropBool(payload, "value", state.power))
        {
            throw "Failed to set 'value' (power) in payload";
        }
    }
    else if (HUE_BRIGHTNESS_RESOURCE_TYPE == resourceType)
    {
        if (!OCRepPayloadGetPropInt(input, "brightness", &light_resource.bri))
        {
            throw "No brightness in representation" ;
        }
        OIC_LOG_V(INFO, TAG, "PUT/POST brightness:%" PRIu64 "", light_resource.bri);

        // Sclae up from 1-100 for OCF Light to 1-254 for Hue device
        light_resource.bri *= 2.54;

        // Add 1 to make sure when we scale down later by dividing by 2.54, we try and
        // arryto the same number.
        if (light_resource.bri != 254)
        {
            light_resource.bri += 1;
        }
        // Get the current powered state of light and then set the value accordingly.
        // If the light is turned off, then PUT to bri will yield in a blink
        // and quickly to off state. In short, it is invalid.
        state.bri = light_resource.bri;
        state.power = true;

        if (!OCRepPayloadSetPropInt(payload, "brightness", state.bri))
        {
            throw "Failed to set 'brightness' in payload";
        }
    }
    else if (HUE_CHROMA_RESOURCE_TYPE == resourceType)
    {
        bool isChromaPropertyInPayload = false;

        if (!OCRepPayloadGetPropInt(input, "hue", &light_resource.hue))
        {
            OIC_LOG(INFO, TAG, "No hue in PUT payload");
        }
        else
        {
            state.hue = light_resource.hue;
            isChromaPropertyInPayload = true;
            OIC_LOG_V(INFO, TAG, "PUT/POST hue :%" PRIu64 "", state.hue);
        }

        if (!OCRepPayloadGetPropInt(input, "saturation", &light_resource.sat))
        {
            OIC_LOG(INFO, TAG, "No saturation in PUT payload");
        }
        else
        {
            state.sat = light_resource.sat;
            isChromaPropertyInPayload = true;
            OIC_LOG_V(INFO, TAG, "PUT/POST sat :%" PRIu64 "", state.sat);
        }

        if (!OCRepPayloadSetPropInt(payload, "hue", state.hue) ||
            !OCRepPayloadSetPropInt(payload, "saturation", state.sat))
        {
            throw "Failed to set 'hue' or 'saturation' in payload" ;
        }

        size_t csc_dimensions[MAX_REP_ARRAY_DEPTH] = {2, 0, 0};
        double *cscInPayload = NULL;
        if (!OCRepPayloadGetDoubleArray(input, "csc", &cscInPayload, csc_dimensions))
        {
            OIC_LOG(INFO, TAG, "No csc in PUT payload");
        }
        else
        {
            if (cscInPayload != NULL)
            {
                isChromaPropertyInPayload = true;
                state.csc[0] = cscInPayload[0];
                state.csc[1] = cscInPayload[1];
                OIC_LOG_V(INFO, TAG, "PUT/POST csc (sat) :[%f, %f]", state.csc[0], state.csc[1]);
            }
        }

        if (isChromaPropertyInPayload)
        {
            state.power = true;
            light_resource.power = true;
        }
        OICFree(cscInPayload);
    }
    else
    {
        throw "Failed due to unkwown resource type" ;
    }

    if (hueLight->setState(state) != MPM_RESULT_OK)
    {
        throw "Error setting light state" ;
    }
    return OC_EH_OK;
}

OCRepPayload *getCommonPayload(const char *uri, char *interfaceQuery,
                               std::string resType, OCRepPayload *payload)
{
    if (!OCRepPayloadSetUri(payload, uri))
    {
        throw "Unable to set URI in the payload";
    }

    if (!OCRepPayloadAddResourceType(payload, resType.c_str()))
    {
        throw "Failed to set light resource type" ;
    }
    OIC_LOG_V(INFO, TAG, "Checking against if: %s", interfaceQuery);

    // If the interface filter is explicitly oic.if.baseline, include all properties.
    if (interfaceQuery && std::string(interfaceQuery) == std::string(OC_RSRVD_INTERFACE_DEFAULT))
    {
        if (!OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_ACTUATOR))
        {
            throw "Failed to set light interface";
        }

        if (!OCRepPayloadAddInterface(payload, std::string(OC_RSRVD_INTERFACE_DEFAULT).c_str()))
        {
            throw "Failed to set baseline interface" ;
        }
    }
    return payload;
}

/**
 * Monitors the light state changes and sends notification if
 * any change. Also discovers new Bridges...!
 *
 * @param[in] pointer pluginctx
 */
static void *hueDiscoveryThread(void *pointer)
{
    MPMPluginCtx *ctx = (MPMPluginCtx *) pointer;
    if (ctx == NULL)
    {
        return NULL;
    }
    OIC_LOG(INFO, TAG, "Plugin specific thread handler entered");
    HueLight::light_config_t config;
    std::string uniqueId, uri;

    while (true == ctx->stay_in_process_loop)
    {
        addedLightsLock.lock();
        for (auto itr : addedLights)
        {
            HueLightSharedPtr light = itr.second;
            if (!light)
            {
                continue;
            }
            light->getConfig(config);

            std::string uniqueId = createuniqueID(config.uniqueId);
            uri = (HUE_LIGHT_URI + uniqueId ) ;

            HueLight::light_state_t oldState, newState ;
            light->getState(oldState);
            light->getState(newState, true);

            if (oldState.power != newState.power)
            {
                ConcurrentIotivityUtils::queueNotifyObservers(itr.first + SWITCH_RELATIVE_URI);
            }
            else if (hasBrightnessChangedInOCFScale(oldState, newState))
            {
                ConcurrentIotivityUtils::queueNotifyObservers(itr.first + BRIGHTNESS_RELATIVE_URI);
            }
            else if ((oldState.hue != newState.hue) || (oldState.sat != newState.sat))
            {
                ConcurrentIotivityUtils::queueNotifyObservers(itr.first + CHROMA_RELATIVE_URI);
            }
            else
            {
                ; //Do nothing here..
            }
        }
        addedLightsLock.unlock();
        /*start the periodic bridge discovery*/
        DiscoverHueBridges();
        sleep(MPM_THREAD_PROCESS_SLEEPTIME);
    }
    OIC_LOG(INFO, TAG, "Leaving plugin specific thread handler");
    pthread_exit(NULL);
}

HueLightSharedPtr getHueLightFromOCFResourceUri(std::string resourceUri)
{
    OIC_LOG_V(INFO, TAG, "Request for %s ", resourceUri.c_str());

    for (auto uriToHuePair : addedLights)
    {
        if (resourceUri.find(uriToHuePair.first) != std::string::npos)
        {
            return uriToHuePair.second;
        }
    }
    throw "Resource" + resourceUri + "not found";
}
