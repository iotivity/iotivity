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
#include <inttypes.h>
#include <string.h>
#include <string>
#include <math.h>
#include <signal.h>
#include <pthread.h>
#include <iostream>
#include <map>
#include "logger.h"
#include "gw_error_code.h"
#include "plugin_server.h"
#include "hue_light.h"
#include "hue_auth_spec.h"
#include "hue_light.h"
#include "hue_file.h"
#include "hue_bridge.h"
#include "curl_client.h"
#include "oic_string.h"
#include "oic_malloc.h"
#include "hue_resource.h"
#include "unistd.h"
#include <sys/types.h>
#include "message_handler.h"
#include "ConcurrentIotivityUtils.h"
#include "IotivityWorkItem.h"
#include "cbor.h"

/*******************************************************************************
 * Pound defines and structure declarations go here
 ******************************************************************************/

/*Discovery thread is continuously running with sleep in between*/
#define         MAX_QUERY_STRING                200
#define         TAG                            "HUE_RESOURCE"


/*******************************************************************************
 * global data goes here
 ******************************************************************************/
plugin_ctx_t *g_plugin_ctx = NULL;
pthread_mutex_t g_disc_list_lock;
pthread_mutex_t g_response_lock;

std::map<std::string, HueBridge> authorizedBridges;
typedef std::map<std::string, HueBridge>::iterator bridgeItr;

std::map<std::string, HueLightSharedPtr> g_discovered_lights_map;
std::map<std::string, HueLightSharedPtr> addedLights;

const std::string HUE_SWITCH_RESOURCE_TYPE = "oic.r.switch.binary";
const std::string HUE_BRIGHTNESS_RESOURCE_TYPE = "oic.r.light.brightness";
const std::string HUE_CHROMA_RESOURCE_TYPE = "oic.r.colour.chroma";
const std::string RES_INTERFACE_NAME = "oic.if.a";
static char CRED_FILE[] = "./oic_svr_db_hue.dat";

FILE *hue_fopen(const char *path, const char *mode)
{
    (void) path;
    return fopen(CRED_FILE, mode);
}

/* This is a plugin's specific entry point function that allows the plugin
 * to create and initialize static structures.
 *
 * returns:
 *     GW_RESULT_OK             - no errors
 *     GW_RESULT_INTERNAL_ERROR - stack process error
 */
GW_RESULT plugin_create(plugin_ctx_t **plugin_specific_ctx)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    if (g_plugin_ctx == NULL)
    {
        /* initialize global data and return value */
        memset(&g_disc_list_lock, 0, sizeof(pthread_mutex_t));
        memset(&g_response_lock, 0, sizeof(pthread_mutex_t));

        *plugin_specific_ctx = NULL;

        /* allocate a context structure for the plugin */
        plugin_ctx_t *ctx = (plugin_ctx_t *) OICMalloc(sizeof(plugin_ctx_t));

        /* initialize the plugin context */
        if (ctx != NULL)
        {
            memset(ctx, 0, sizeof(plugin_ctx_t));
            *plugin_specific_ctx = ctx;
            g_plugin_ctx = ctx;
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Unable to allocate plugin specific context");
            goto exit;
        }
        if (pthread_mutex_init(&g_disc_list_lock, NULL) != 0)
        {
            OIC_LOG(ERROR, TAG, "Unable to initialize g_disc_list_lock.");
            goto exit;
        }
        if (pthread_mutex_init(&g_response_lock, NULL) != 0)
        {
            OIC_LOG(ERROR, TAG, "Unable to initialize g_response_lock.");
            goto exit;
        }

        ctx->device_name = (char *) DEVICE_NAME;
        ctx->resource_type = (char *) DEVICE_TYPE;
        ctx->open = hue_fopen;

        /* From here we can return Success for plugin_create. If we don't find the bridge or lights
         * or if authorization fails, we try again the discovery thread
         * */
        result = OC_STACK_OK;
        authorizedBridges.clear();
    }
    else
    {
        result = GW_RESULT_ALREADY_CREATED;
    }
    exit:
    OIC_LOG_V(INFO, TAG, "Plugin create return value:%d.", result);

    /*
     * NOTE: What do we do if the create for some reason failed.  To we assume that the destroy
     * will be called if the create fails??  Let let the plugin loader pick up the pieces by
     * calling destroy on an imperfectly created plugin.  Calling entry point APIs from within
     * the implementation can cause some real problems (e.g. deadlock situations).
     */

    return (result);
}

/* CALLED BY the plugin's OIC server to allow the plugin to use OIC apis
 *
 * returns:
 *     GW_RESULT_OK             - no errors
 *     GW_RESULT_INTERNAL_ERROR - stack process error
 */
GW_RESULT plugin_start(plugin_ctx_t *ctx)
{
    GW_RESULT result = GW_RESULT_STARTED_FAILED;
    int error = 0;
    if (ctx == NULL || g_plugin_ctx == NULL)
    {
        goto exit;
    }
    if (ctx->started)
    {
        result = GW_RESULT_ALREADY_STARTED;
        goto exit;
    }

    result = hueInit(ctx, addAuthorizedBridgeCB, RemoveAuthorizedBridgeCB);
    if (GW_RESULT_OK == result)
    {
        /*start bridge discovery*/
        if (DiscoverHueBridges() != GW_RESULT_OK)
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
                               hue_discovery_thread, ctx);
        if (error == 0)
        {
            ctx->started = true;
            result = GW_RESULT_OK;
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "Can't create plugin specific thread :[%s]",
                  strerror(error));
            plugin_stop(ctx);
            result = GW_RESULT_STARTED_FAILED;
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "hueAuthCreate Failed. Cannot create plugin");
    }
    exit:
    OIC_LOG_V(INFO, TAG, "Plugin start return value:%d.", result);
    return (result);
}

// Checks if brightness has changed in the 0 - 100 range for OCF light.
bool hasBrightnessChangedInOCFScale(const HueLight::light_state_t &stateprev,
                                    const HueLight::light_state_t &statenew)
{

    uint16_t ocfBrightnessPrev = stateprev.bri / 2.54;
    uint16_t ocfBrightnessNew = statenew.bri / 2.54;
    return ocfBrightnessNew != ocfBrightnessPrev;
}

bool createSecureResources()
{
    char *non_secure_env = getenv("NONSECURE");

    if (non_secure_env != NULL && std::string(non_secure_env) == "true")
    {
        OIC_LOG(INFO, TAG,"Creating NON SECURE resources");
        return false;
    }
    OIC_LOG(INFO, TAG,"Creating SECURE resources");
    return true;
}

void createPayloadForMetaData(resourceList **list, const std::string &configURI,const std::string rt, const std::string res_if,uint8_t res_prop )
{
    resourceList *temp = (resourceList *)OICCalloc(1, sizeof(resourceList));
    if(temp == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Calloc failed for createPayloadForMetaData %s", strerror(errno));
        return;
    }

    OICStrcpy(temp->rt, MAX_LENGTH_64, rt.c_str());
    OICStrcpy(temp->href, MAX_URI_LEN, configURI.c_str());
    OICStrcpy(temp->interfaces, MAX_LENGTH_64, res_if.c_str());
    temp->bitmap = (int)res_prop;

    temp->next = *list;
    *list = temp;
}

/*
 For every <light_uri> resource found, this function will create OCF resources with uri suffixes as :
        /switch for oic.r.switch.binary
        /brightness for oic.r.light.brightness
        /chroma for oic.r.colour.chroma
 */
void add_item_ocf_profile_wrapper(const std::string &configURI, WorkItemType type)
{
    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    if (createSecureResources()) resourceProperties |= OC_SECURE;

    std::string switchURI = configURI + SWITCH;
    std::string brightnessURI = configURI + BRIGHTNESS;
    std::string chromaURI = configURI + CHROMA;

    switch (type)
    {
        case WorkItemType::CREATE:
        {
            resourceList *list  = NULL;
            uint8_t *buff = (uint8_t *)OICCalloc(1, MAX_METADATA_LEN);
            if(buff == NULL)
            {
                OIC_LOG_V(ERROR, TAG, "Calloc failed %s", strerror(errno));
                return;
            }
            size_t size = MAX_METADATA_LEN;
            HueLightSharedPtr light; hueFile bridgeCtx;
            LightDetails deviceDetails;
            HueLight::light_config_t config;
            device_specific_data_t deviceConfiguration;
            memset(&deviceDetails,0,sizeof(LightDetails));
            memset(&deviceConfiguration,0,sizeof(device_specific_data_t));

            // Create Resources and form metadata for RECONNECT
            ConcurrentIotivityUtils::createResource(switchURI, HUE_SWITCH_RESOURCE_TYPE.c_str(), RES_INTERFACE_NAME.c_str(), binary_switch_EH, NULL, resourceProperties);
            createPayloadForMetaData(&list, switchURI, HUE_SWITCH_RESOURCE_TYPE.c_str(), RES_INTERFACE_NAME.c_str(), resourceProperties);

            ConcurrentIotivityUtils::createResource(brightnessURI, HUE_BRIGHTNESS_RESOURCE_TYPE.c_str() , RES_INTERFACE_NAME.c_str(),brightness_EH, NULL, resourceProperties);
            createPayloadForMetaData(&list, brightnessURI, HUE_BRIGHTNESS_RESOURCE_TYPE.c_str(),RES_INTERFACE_NAME.c_str(), resourceProperties);


            ConcurrentIotivityUtils::createResource(chromaURI, HUE_CHROMA_RESOURCE_TYPE.c_str() , RES_INTERFACE_NAME.c_str(), chroma_EH, NULL, resourceProperties);
            createPayloadForMetaData(&list, chromaURI, HUE_CHROMA_RESOURCE_TYPE.c_str(), RES_INTERFACE_NAME.c_str(), resourceProperties);

            light = g_discovered_lights_map[configURI];
            light->getConfig(config);

            std::string data;
            data =  light->getBridgeMac();
            std::transform(data.begin(), data.end(), data.begin(), ::tolower);
            OICStrcpy(deviceDetails.bridgeMac, MAX_UNIQUE_ID_LEN, data.c_str());
            deviceDetails.bridgeMac[MAX_UNIQUE_ID_LEN-1] = '\0';

            OICStrcpy(deviceDetails.lightMac, MAX_LENGTH_32, config.uniqueId.c_str());
            OICStrcpy(deviceDetails.lightUri, MAX_URI_LEN, config.uri.c_str());
            OICStrcpy(deviceDetails.prefix, MAX_LENGTH_256, light->getUri().c_str());
            OICStrcpy(deviceDetails.lightNo, MAX_LENGTH_32, light->getShortId().c_str());

            findAuthorizedBridge(deviceDetails.bridgeMac, NULL, bridgeCtx);
            OICStrcpy(deviceDetails.clientId, MAX_LENGTH_64, bridgeCtx.clientID);


            OICStrcpy(deviceConfiguration.devName, MAX_LENGTH_64, DEVICE_NAME);
            OICStrcpy(deviceConfiguration.devType, MAX_LENGTH_64, DEVICE_TYPE);
            OICStrcpy(deviceConfiguration.manufacturerName, MAX_LENGTH_256, MANUFACTURER_NAME);
            formMetaData(list, &deviceConfiguration, buff, size, &deviceDetails,sizeof(deviceDetails));

            add_response response;
            memset(&response, 0, sizeof(add_response));
            OICStrcpy(response.uri, MAX_URI_LEN, configURI.c_str());
            memcpy(response.metadata, buff, MAX_METADATA_LEN);
            size_t response_size = sizeof(add_response);
            send_response(&response, response_size, MPM_ADD);
            OICFree(buff);
            break;
        }

        case WorkItemType::DELETE:
            ConcurrentIotivityUtils::deleteResource(switchURI);
            ConcurrentIotivityUtils::deleteResource(brightnessURI);
            ConcurrentIotivityUtils::deleteResource(chromaURI);
            break;

        default:
            OIC_LOG(ERROR, TAG, " Work item error ");
                  break;
    }
    return;
}

/*
 * Callback is called when a bridge is authorized. It adds the bridge in bridge map and inits some
 * bridge data.
 */
void addAuthorizedBridgeCB(const char *macAddress, const char *ClientId)
{
    HueBridge bridge; HueBridge::hue_bridge_data_tag bridgeData;
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;

    if (authorizedBridges.find(macAddress) == authorizedBridges.end())
    {
        uint32_t prefix_size = MAX_QUERY_STRING;
        char *prefix = (char *) OICMalloc(prefix_size);

        /*get prefix for discovering lights*/
        result = hueAuthGetHttpPrefix(prefix, &prefix_size, macAddress, ClientId);
        if (result == GW_RESULT_INSUFFICIENT_BUFFER)
        {
             prefix = (char *) realloc(prefix, prefix_size);
             result = hueAuthGetHttpPrefix(prefix, &prefix_size, macAddress, ClientId);
        }
        if (result == GW_RESULT_OK)
        {
            bridge.setBridgeCurlQuery(prefix);
            bridge.getBridgeConfigFromCloud();
            bridge.getBridgeConfig(bridgeData);
            OIC_LOG_V(DEBUG, TAG, " \t\n\nBRIDGE AUTHORIZED\nclientID: %s\nip : %s\nmac: %s\nname : %s\nsw : %s\n",ClientId,
                      bridgeData.ip.c_str(),bridgeData.mac.c_str(), bridgeData.name.c_str(), bridgeData.swVersion.c_str() );
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


/*
 *  For initiating the scan for lights
 *  Parameters :
 *             plugin_ctx_t - Plugin context ,
 *             mpm_pipe_message_t - Scan Message.
 */
GW_RESULT plugin_scan(plugin_ctx_t *ctx, mpm_pipe_message_t *message)
{
    OIC_LOG(DEBUG, TAG,  "Inside Plugin  scan");
    (void)message;
    (void)ctx;
    std::string uri,uniqueId ;
    HueLight::light_config_t config;
    HueLight::light_state_t state;

    pthread_mutex_lock(&g_disc_list_lock);
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
            uri= (HUE_LIGHT_URI + uniqueId) ;

            OIC_LOG_V(INFO, TAG,
                    "Found Reachable Light - light name=%s, id=%s, reachable=%d",
            config.name.c_str(), config.uniqueId.c_str(), state.reachable);

            if(addedLights.find(uri) != addedLights.end())
            {
                OIC_LOG_V(INFO, TAG,"Already Added %s. Ignoring", uri.c_str());
                continue;
            }

            g_discovered_lights_map[uri] = light;

            std::string s = uri;
            unsigned long size = s.size() + 1;
            char *m = (char*) OICMalloc(size);
            memcpy(m, s.c_str(), size);
            send_response(m, size, MPM_SCAN);
            OICFree(m);
        }
    }
    pthread_mutex_unlock(&g_disc_list_lock);
    return GW_RESULT_OK;
}

/*
 *  CreateuniqueID - Creates the unique id by removing Delimiters..
 *  Input Parameters : MacId - Unique Id of the Device
 */
std::string createuniqueID(std::string deviceId)
{
    std::string uniqueId(deviceId);
    std::string token = "";
    std::string delimiter1 = ":";
    std::string delimiter2 = "-";
    size_t pos=0;

    while( (pos = uniqueId.find(delimiter1)) != std::string::npos)
    {
        uniqueId.replace(pos, 1, token);
    }
    while( (pos = uniqueId.find(delimiter2)) != std::string::npos)
    {
        uniqueId.replace(pos, 3, token);
    }
    return uniqueId;
}

/*
 * Add_device - Add device in the DISCOVERABLE mode
 * Input parameters - plugin_ctx_t        - Plugin context
 *                    plugin_adddevice_t  - Add device structure
 *
 */
GW_RESULT plugin_add(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx;
    char *c_uri = NULL;
    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    if (createSecureResources()) resourceProperties |= OC_SECURE;

    if(message->payloadSize > 0 && message->payloadSize < SIZE_MAX)
    {
        c_uri = (char*) OICCalloc(1,message->payloadSize);
    }
    if(c_uri == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Calloc Failed [%s]", strerror(errno));
       return GW_RESULT_INTERNAL_ERROR;
    }
    memcpy(c_uri, message->payload, message->payloadSize);
    std::string uri(c_uri);
    OICFree(c_uri);

    if(addedLights.find(uri) != addedLights.end())
    {
        OIC_LOG_V(ERROR, TAG,"%s already added", uri.c_str());
        return GW_RESULT_ALREADY_CREATED;
    }
    if(g_discovered_lights_map.find(uri) == g_discovered_lights_map.end())
    {
        OIC_LOG_V(ERROR, TAG,"%s was NOT discovered in a scan", uri.c_str());
        return GW_CB_RESULT_ERROR;
    }

    add_item_ocf_profile_wrapper(uri, WorkItemType::CREATE);
    pthread_mutex_lock(&g_disc_list_lock);
    addedLights[uri] = g_discovered_lights_map[uri];
    pthread_mutex_unlock(&g_disc_list_lock);

    return GW_RESULT_OK;
}

/*
 * Remove_device - Remove device from the plugin and destroy resources.
 * Input parameters - plugin_ctx_t     - Plugin context
 *                     plugin_remove_t - Remove device structure
 *
 */

GW_RESULT plugin_remove(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx;
    char *c_uri = NULL;

    if(message->payloadSize > 0 && message->payloadSize < SIZE_MAX)
    {
        c_uri = (char*) OICCalloc(1,message->payloadSize);
    }
    if(c_uri == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Calloc Failed - %s ", strerror(errno));
        return GW_RESULT_INTERNAL_ERROR;
    }
    memcpy(c_uri, message->payload, message->payloadSize);
    std::string uri(c_uri);
    OICFree(c_uri);

    add_item_ocf_profile_wrapper(uri, WorkItemType::DELETE);

    addedLights.erase(uri);

    unsigned long size = uri.size() + 1;
    char *m = (char*) OICMalloc(size);
    memcpy(m, uri.c_str(), size);

    send_response(m, size, MPM_REMOVE);

    OICFree(m);
    return GW_RESULT_OK;
}

/*
 * Reconnect_device - Reconnect devices from the database.
 * Input parameters - plugin_ctx_t     - Plugin context
 *                     plugin_remove_t - Remove device structure
 *
 */
GW_RESULT plugin_reconnect(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void)ctx;
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    LightDetails *plugindetails = NULL;
    void *details = NULL;
    HueDiscoveredCtx discoveredCtx;
    std::size_t pos = 0;
    std::string light_Prefix,ip,light_mac,light_no,uri;
    uint8_t resourceProperties = 0;
    HueBridge bridge;
    resourceList *list=NULL, *temp = NULL;
    char *payload = NULL;

    if(message->payloadSize > 0 && message->payloadSize < SIZE_MAX)
    {
        payload = (char*) OICCalloc(1, message->payloadSize);
    }
    if(payload == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Calloc Failed - %s", strerror(errno));
        return GW_RESULT_INTERNAL_ERROR;
    }
    memcpy(payload, message->payload, message->payloadSize);

    ParseMetaData((uint8_t*)payload,MAX_METADATA_LEN,&list,&details);
    plugindetails = (LightDetails*) details;

    // Find Bridge ip and light id;
    light_no = plugindetails->lightNo;
    light_Prefix = plugindetails->prefix;
    pos = light_Prefix.find("/");
    ip = light_Prefix.substr(0,(pos));

    OIC_LOG_V(DEBUG, TAG," \n\n Reconnect meta data \n\n Bridge Mac - %s\n Light Mac - %s\nip - %s\n Client ID -%s\n Light no - %s\n"
             "\n prefix - %s \n ",plugindetails->bridgeMac,plugindetails->lightMac, ip.c_str(),plugindetails->clientId,
                                    plugindetails->lightNo, plugindetails->prefix);

    if ((plugindetails->bridgeMac != NULL) && ( plugindetails->clientId != NULL))
    {
        if(authorizedBridges.find(plugindetails->bridgeMac) == authorizedBridges.end())
        {
            memset(&discoveredCtx, 0, sizeof(HueDiscoveredCtx));
            OICStrcpy(discoveredCtx.macAddrString, MAX_STRING - 1,plugindetails->bridgeMac);
            OICStrcpy(discoveredCtx.ipAddrString, MAX_STRING - 1, ip.c_str());
            OICStrcpy(discoveredCtx.clientIDs, MAX_STRING * MAX_CLIENTS, plugindetails->clientId);
            discoveredCtx.numClients = 1;
            addAuthorizedBridge(plugindetails->bridgeMac,plugindetails->clientId);
            result = addDiscoveredBridge(discoveredCtx);
            uint32_t prefix_size = MAX_QUERY_STRING;
            char *prefix = (char *) OICMalloc(prefix_size);
            result = hueAuthGetHttpPrefix(prefix, &prefix_size,plugindetails->bridgeMac,plugindetails->clientId);
            if (result == GW_RESULT_INSUFFICIENT_BUFFER)
            {
                prefix = (char *) realloc(prefix, prefix_size);
                result = hueAuthGetHttpPrefix(prefix, &prefix_size, plugindetails->bridgeMac, plugindetails->clientId);
            }
            if(result != GW_RESULT_OK)
            {
                OIC_LOG(DEBUG, TAG,"hueAuthGetHttpPrefix failed");
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
        if( plugindetails->bridgeMac == authorizedbridge->getBridgeMAC())
        {
            OIC_LOG(DEBUG, TAG,"Bridge Found and is authorized");
            addReconnectLightsToBridge(plugindetails, authorizedbridge, ip);
            result = GW_RESULT_OK;
        }
    }

    while(list)
    {
        temp = list;
        resourceProperties |= list->bitmap;
        std::string uri(list->href);

        if( strcmp(list->rt,HUE_SWITCH_RESOURCE_TYPE.c_str()) == 0 )
        {
            ConcurrentIotivityUtils::createResource(uri, list->rt, list->interfaces, binary_switch_EH, NULL, resourceProperties);
        }
        else if( strcmp(list->rt,HUE_BRIGHTNESS_RESOURCE_TYPE.c_str()) == 0 )
        {
            ConcurrentIotivityUtils::createResource(uri, list->rt, list->interfaces, brightness_EH, NULL, resourceProperties);
        }
        else if( strcmp(list->rt,HUE_CHROMA_RESOURCE_TYPE.c_str()) == 0 )
        {
            ConcurrentIotivityUtils::createResource(uri, list->rt, list->interfaces, chroma_EH, NULL, resourceProperties);
        }
        else
        {
            OIC_LOG(ERROR, TAG, " Resource type not valid");
        }

        list = list->next;
        OICFree(temp);
    }
    return GW_RESULT_OK;
}

void addReconnectLightsToBridge(LightDetails *plugindetails, HueBridge *bridge, std::string bridgeIp)
{
    HueLight::light_config_t config;
    std::string uuid, uri;

    std::shared_ptr<HueLight> light = std::make_shared<HueLight>(plugindetails->prefix, bridgeIp, plugindetails->bridgeMac , plugindetails->lightNo,"NULL");
    if(!light)
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

    g_discovered_lights_map[uri] = light;
    addedLights[uri] = light;
}

/*
 * Removes bridge from the authorized bridge map.
 */
void RemoveAuthorizedBridgeCB(const char *macAddrString)
{
    OIC_LOG_V(INFO, TAG, "Remove Bridge called for bridge = %s", macAddrString);
    pthread_mutex_lock(&g_disc_list_lock);
    bridgeItr it = authorizedBridges.find(macAddrString);
    if (it != authorizedBridges.end())
    {
        /*remove the bridge*/
        authorizedBridges.erase(it);
    }
    pthread_mutex_unlock(&g_disc_list_lock);
    return;
}


/*  Plugin specific entry-point function to stop the plugin's threads
 *
 * returns:
 *     GW_RESULT_OK             - no errors
 *     GW_RESULT_INTERNAL_ERROR - stack process error
 */
GW_RESULT plugin_stop(plugin_ctx_t *ctx)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;

    if (NULL != ctx && g_plugin_ctx != NULL)
    {
        result = GW_RESULT_OK;
        OCStopPresence();
        //stop the presence before stopping the plugin
        // This lock is needed here to avoid the plugin
        // from crashing if plugin_stop is called
        // and thread_discovery* is running already or
        // called at the same time
        // Also hueAuthDestroy is moved later
        // as it results in deleting the light and Auth resources
        // and also clearing global context for Auth module, so
        // if the Discover bridge function was called at the same time,
        // just after the clearing of global context, plugin crashed
        // (for bridge discovery)!
        pthread_mutex_lock(&g_disc_list_lock);

        if (ctx->started == true)
        {
            ctx->stay_in_process_loop = false;
            pthread_join(ctx->thread_handle, NULL);
            ctx->started = false;
        }
        pthread_mutex_unlock(&g_disc_list_lock);

        //destroy the resources
        hueAuthDestroy();
        clearBridgeDetails();
    }
    OIC_LOG_V(INFO, TAG, "Plugin stop: OUT - return value:%d", result);

    return (result);
}

/* Plugin specific entry-point function to allow the plugin resources to be
 * freed.
 *
 * returns:
 *     GW_RESULT_OK             - no errors
 *     GW_RESULT_INTERNAL_ERROR - stack process error
 */
GW_RESULT plugin_destroy(plugin_ctx_t *ctx)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;

    if (ctx != NULL && g_plugin_ctx != NULL)
    {
        result = GW_RESULT_OK;
        if (ctx->started == true)
        {
            result = plugin_stop(ctx);
        }
        pthread_mutex_destroy(&g_disc_list_lock);
        pthread_mutex_destroy(&g_response_lock);
        /* freeing the resource allocated in create */
        OICFree(ctx);
        g_plugin_ctx = NULL;
    }

    OIC_LOG_V(INFO, TAG, "Plugin destroy's return value:%d", result);

    return (result);
}

OCEntityHandlerResult handle_EH_requests(
        OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest,
        std::string resourceType)
{
    (void)flag;
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    pthread_mutex_lock(&g_response_lock);
    OCRepPayload *responsePayload = NULL;
    OCRepPayload *payload = OCRepPayloadCreate();

    try
    {
        if ((entityHandlerRequest == NULL))
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
            hlpr_ExtractFiltersFromQuery(dupQuery, &interfaceQuery, &resourceTypeQuery);
        }

        switch(entityHandlerRequest->method)
        {
            case OC_REST_GET:
                OIC_LOG_V(INFO, TAG, " GET Request for: %s", uri.c_str());
                ehResult = process_get_request(payload, hueLight, resourceType);
                break;

            case OC_REST_PUT:
            case OC_REST_POST:

                OIC_LOG_V(INFO, TAG, "PUT / POST Request on %s", uri.c_str());
                ehResult = process_put_request(entityHandlerRequest, hueLight, resourceType, payload);

                //  To include "if" in all payloads.
                interfaceQuery = (char *) OC_RSRVD_INTERFACE_DEFAULT;
                break;

            default:
                OIC_LOG_V(ERROR, TAG, "UnSupported Method [%d] Received ",entityHandlerRequest->method);
                ConcurrentIotivityUtils::respondToRequestWithError(entityHandlerRequest," Unsupported Method", OC_EH_METHOD_NOT_ALLOWED);
                ehResult = OC_EH_OK;
                break;
        }
        responsePayload = get_common_payload(uri.c_str(), hueLight , interfaceQuery, resourceType, payload);
        ConcurrentIotivityUtils::respondToRequest(entityHandlerRequest, responsePayload, ehResult);
        OICFree(dupQuery);
    }
    catch (const char *errorMessage)
    {
        OIC_LOG_V(ERROR, TAG, "Error - %s ", errorMessage);
        ConcurrentIotivityUtils::respondToRequestWithError(entityHandlerRequest, errorMessage, OC_EH_ERROR);
        ehResult = OC_EH_OK;
    }

    pthread_mutex_unlock(&g_response_lock);
    OCRepPayloadDestroy(responsePayload);
    return ehResult;
}

// Entity handler for binary switch
OCEntityHandlerResult binary_switch_EH(
        OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest,
        void *callbackParam)
{
    (void) callbackParam;
    return handle_EH_requests(flag,entityHandlerRequest, HUE_SWITCH_RESOURCE_TYPE);
}

// Entity handler for brightness resource
OCEntityHandlerResult brightness_EH(
        OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest,
        void *callbackParam)
{
    (void) callbackParam;
    return handle_EH_requests(flag,entityHandlerRequest, HUE_BRIGHTNESS_RESOURCE_TYPE );
}

// Entity handler for chroma resource
OCEntityHandlerResult chroma_EH(
        OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest,
        void *callbackParam)
{
    (void) callbackParam;
    return handle_EH_requests(flag,entityHandlerRequest,HUE_CHROMA_RESOURCE_TYPE);
}

/* This function gets the State of the light [power, brightness and chroma values..] */

OCEntityHandlerResult process_get_request(OCRepPayload *payload, HueLightSharedPtr hueLight, std::string resType)
{
    HueLight::light_state_t light_state;
    hueLight->getState(light_state);

    if(payload == NULL)
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
        OIC_LOG_V(INFO, TAG, " Brightness State (Hue Bulb): %ld  Brightness(OCF) : %d", light_state.bri, ocfBrightness);
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
    OIC_LOG_V(INFO, TAG, "hue: %ld, sat: %ld, csc: [%f, %f] in payload.",
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

/* This function PUT the State on the light [power, brightness and chroma values..] */

OCEntityHandlerResult process_put_request(OCEntityHandlerRequest *ehRequest,
                                             HueLightSharedPtr hueLight, std::string resourceType,  OCRepPayload *payload)
{

    if (!ehRequest || !ehRequest->payload || ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
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
    if (hueLight->getState(state, true) != GW_RESULT_OK)
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
        OIC_LOG_V(INFO, TAG, "PUT/POST brightness:%ld", light_resource.bri);

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
            isChromaPropertyInPayload = true;
            OIC_LOG(INFO, TAG, "No hue in PUT payload");
        }
        else
        {
            state.hue = light_resource.hue;
            isChromaPropertyInPayload = true;
            OIC_LOG_V(INFO, TAG, "PUT/POST hue :%ld", state.hue);
        }

        if (!OCRepPayloadGetPropInt(input, "saturation", &light_resource.sat))
        {
            throw "No saturation in PUT payload";
        }
        else
        {
            state.sat = light_resource.sat;
            isChromaPropertyInPayload = true;
            OIC_LOG_V(INFO, TAG, "PUT/POST sat :%ld", state.sat);
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
            if(cscInPayload != NULL)
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

    if (hueLight->setState(state) != GW_RESULT_OK)
    {
        throw "Error setting light state" ;
    }
    return OC_EH_OK;
}

OCRepPayload *get_common_payload(const char *uri, HueLightSharedPtr hueLight, char *interfaceQuery, std::string resType, OCRepPayload *payload)
{
    (void)hueLight;
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
        if (!OCRepPayloadAddInterface(payload, RES_INTERFACE_NAME.c_str()))
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

/*
 * Monitors the light state changes and sends notification if
 * any change. Also discovers new Bridges...!
 */
void *hue_discovery_thread(void *pointer)
{
    plugin_ctx_t *ctx = (plugin_ctx_t *) pointer;
    if (ctx == NULL)
    {
        return NULL;
    }
    OIC_LOG(INFO, TAG, "Plugin specific thread handler entered");
    HueLight::light_config_t config;
    std::string uniqueId,uri;

    while(true == ctx->stay_in_process_loop)
    {
        pthread_mutex_lock(&g_disc_list_lock);
        for(auto itr: addedLights)
        {
            HueLightSharedPtr light = itr.second;
            if(!light)
            {
                continue;
            }
            light->getConfig(config);

            std::string uniqueId = createuniqueID(config.uniqueId);
            uri= (HUE_LIGHT_URI + uniqueId ) ;

            HueLight::light_state_t oldState, newState ;
            light->getState(oldState);
            light->getState(newState, true);

            if(oldState.power != newState.power)
            {
                ConcurrentIotivityUtils::notifyObservers(itr.first + SWITCH);
            }
            else if (hasBrightnessChangedInOCFScale(oldState, newState))
            {
                ConcurrentIotivityUtils::notifyObservers(itr.first + BRIGHTNESS);
            }
            else if((oldState.hue != newState.hue) || (oldState.sat != newState.sat))
            {
                ConcurrentIotivityUtils::notifyObservers(itr.first + CHROMA);
            }
            else
            {
                ; //Do nothing here..
            }
        }
        pthread_mutex_unlock(&g_disc_list_lock);
        /*start the periodic bridge discovery*/
        DiscoverHueBridges();
        sleep(THREAD_PROCESS_SLEEPTIME);
    }
    OIC_LOG(INFO, TAG, "Leaving plugin specific thread handler");
    pthread_exit(NULL);
}

HueLightSharedPtr getHueLightFromOCFResourceUri(std::string resourceUri)
{
    OIC_LOG_V(INFO, TAG, "Request for %s " ,resourceUri.c_str());

    for(auto uriToHuePair : addedLights)
    {
        if (resourceUri.find(uriToHuePair.first) != std::string::npos)
        {
            return uriToHuePair.second;
        }
    }
    throw "Resource" + resourceUri + "not found";
}
