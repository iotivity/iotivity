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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include "plugin_server.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "nest.h"
#include <cstdlib>
#include <iostream>
#include <time.h>
#include "logger.h"
#include "ConcurrentIotivityUtils.h"
#include "message_handler.h"
#include "ocpayload.h"
#include "gw_error_code.h"

using namespace std;
using namespace OC::Bridging;

/*******************************************************************************
 * Pound defines and structure declarations go here
 ******************************************************************************/
#define TAG "NEST_RESOURCE"

#define DEVICE_NAME "Nest Translator"
#define DEVICE_TYPE "oic.d.thermostat"
#define MANUFACTURER_NAME "Nest"

#define NEST_ID_TAG "x.com.intel.id"
#define NEST_LAST_CONNECTION_TAG "x.com.intel.lastConnection"

#define TEMPERATURE_TAG "temperature"

#define BM 3

typedef struct
{
    uint16_t humidity;
    uint32_t temperature;
    uint16_t targetTempF;
    uint32_t hvacMode;
    char accessToken[NEST_ACCESS_TOKEN_LEN];
    char deviceId[MAX_LENGTH_64];
} plugin_specific_data_t;

/* TODO: Come up with a name dynamically when more than 1 thermostats
 * are found. At that time of writing this plugin, it is assumed to
 * be only 1 Nest thermostat.
 * TODO: Rename URI with thermostat since Nest also provides smoke detector.
 */
static const std::string NEST_THERMOSTAT_IF = "oic.if.a";
static const std::string NEST_THERMOSTAT_RT = "oic.r.temperature";

static char CRED_FILE[] = "./oic_svr_db_nest.dat";

FILE *nestSecurityFile(const char *path, const char *mode)
{
    (void) path;
    return fopen(CRED_FILE, mode);
}

/*******************************************************************************
 * global data goes here
 ******************************************************************************/
plugin_ctx_t *g_ctx = NULL;
Nest *g_nest = NULL;
NestThermostat::devices g_devices;
std::string nest_client_id;
std::string nest_client_secret;

std::map<std::string, NestThermostatSharedPtr> uriToNestThermostatMap;
std::map<std::string, NestThermostatSharedPtr> addedThermostats;

uint16_t getTemperatureAndUpdateMap(NestThermostatSharedPtr t);
OCEntityHandlerResult resource_entity_handler_cb(OCEntityHandlerFlag flag,
                                                 OCEntityHandlerRequest *entityHandlerRequest, void *callbackParam);

GW_RESULT loadNestAuthConfig(std::string filename, std::string &pincode,
                     std::string &accessToken)
{
    FILE *fp = fopen(filename.c_str(), "r");
    char str[1024];
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;

    if (NULL != fp)
    {
        if (fgets(str, 1023, fp) == NULL)
        {
            OIC_LOG_V(ERROR, TAG, "fgets failed on %s", filename.c_str());
            goto CLEANUP;
        }
        //TODO the logic below needs to be fixed.
        //The intention here was to remove the last character which was new line.
        //Current logic doesn't cover edge case like empty string and etc.
        str[strlen(str) - 1] = '\0';
        pincode = std::string(str);

        if (fgets(str, 1023, fp) == NULL)
        {
            OIC_LOG_V(ERROR, TAG, "fgets failed on %s", filename.c_str());
            goto CLEANUP;
        }
        str[strlen(str) - 1] = '\0';
        accessToken = std::string(str);

        if (fgets(str, 1023, fp) == NULL)
        {
            OIC_LOG_V(ERROR, TAG, "fgets failed on %s", filename.c_str());
            goto CLEANUP;
        }
        str[strlen(str) - 1] = '\0';
        nest_client_id = std::string(str);

        if (fgets(str, 1023, fp) == NULL)
        {
            OIC_LOG_V(ERROR, TAG, "fgets failed on %s", filename.c_str());
            goto CLEANUP;
        }
        str[strlen(str) - 1] = '\0';
        nest_client_secret = std::string(str);

        result = GW_RESULT_OK;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "Could not open %s.\n", filename.c_str());
    }

    CLEANUP:
    if (fp != NULL)
    {
        fclose(fp);
    }
    return result;
}

Nest::ACCESS_TOKEN populateAccessTokenFromFile(std::string accessToken)
{
    Nest::ACCESS_TOKEN aTok(accessToken.c_str());

    return aTok;
}

GW_RESULT checkValidityOfExistingToken(Nest::ACCESS_TOKEN aTok)
{
    GW_RESULT result = GW_RESULT_OK;

    g_nest->setAccessToken(aTok);

    //verify the token by sending the token to nest cloud
    if (g_nest->isAuthorized())
    {
        OIC_LOG(INFO, TAG, "Successfully authorized");
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Could not authorize, access token is invalid/expired");
        result = GW_RESULT_INTERNAL_ERROR;
    }

    return result;
}

void updateNestTokenFile(std::string filename, std::string pincode, std::string accessToken)
{
    FILE *fp = fopen(filename.c_str(), "w");
    if(fp == NULL)
    {
        OIC_LOG(ERROR, TAG, "Failed to open nest.cnf file");
        return;
    }
    fputs(pincode.c_str(), fp);
    fputs("\n", fp);
    fputs(accessToken.c_str(), fp);
    fputs("\n", fp);
    fputs(nest_client_id.c_str(), fp);
    fputs("\n", fp);
    fputs(nest_client_secret.c_str(), fp);
    fputs("\n", fp);

    fclose(fp);
}

GW_RESULT refreshAccessToken(std::string filename, std::string pincode)
{
    GW_RESULT result = GW_RESULT_OK;

    Nest::ACCESS_TOKEN aToken;
    if (g_nest->getAccessToken(pincode, aToken, nest_client_id, nest_client_secret) != GW_RESULT_OK)
    {
        OIC_LOG(ERROR, TAG, "get token failed");
        return GW_RESULT_INTERNAL_ERROR;
    }

    g_nest->setAccessToken(aToken);

    if (g_nest->isAuthorized())
    {
        OIC_LOG(INFO, TAG, "Successfully authorized");
        std::string aTok(aToken.accessToken);
        updateNestTokenFile(filename, pincode, aTok);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Could not authorize, Please put a new pincode into the file");
        result = GW_RESULT_INTERNAL_ERROR;
    }

    return result;
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
    GW_RESULT result = GW_RESULT_OK;

    /* initialize global data and return value */
    *plugin_specific_ctx = NULL;

    /* allocate a context structure for the plugin */
    plugin_ctx_t *ctx = (plugin_ctx_t *) OICMalloc(sizeof(plugin_ctx_t));

    /* initialize the plugin context */
    if (ctx != NULL)
    {
        memset(ctx, 0, sizeof(plugin_ctx_t));
        *plugin_specific_ctx = ctx;
        g_ctx = ctx;
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Unable to allocate plugin specific context.");
        result = GW_RESULT_INTERNAL_ERROR;
        return (result);
    }

    ctx->device_name = (char *) DEVICE_NAME;
    ctx->resource_type = (char *) DEVICE_TYPE;
    ctx->open = nestSecurityFile;

    std::string pincode = "";
    std::string accessToken = "";

    std::string filename = "nest.cnf";

    if (loadNestAuthConfig(filename, pincode, accessToken) != GW_RESULT_OK)
    {
        OIC_LOG(ERROR, TAG, "Unable to load nest.cnf");
        return GW_RESULT_INTERNAL_ERROR;
    }

    g_nest = new Nest();
    Nest::ACCESS_TOKEN aTok = populateAccessTokenFromFile(accessToken);

    result = checkValidityOfExistingToken(aTok);

    if(GW_RESULT_OK != result)
    {
        OIC_LOG(ERROR, TAG, "Nest object could not be created, requesting new access token");
        result = refreshAccessToken(filename, pincode);
    }

    if(GW_RESULT_OK != result)
    {
        delete(g_nest);
    }

    OIC_LOG_V(INFO, TAG, "Plugin create return value:%d.", result);

    /*
     * NOTE: What do we do if the create for some reason failed.  To we assume that the destroy
     * will be called if the create fails??  Let let the plugin loader pick up the pieces by
     * calling destroy on an imperfectly created plugin.  Calling entry point APIs from within
     * the implementation can cause some real problems (e.g. deadlock situations).
     */

    return (result);
}

/* Invoked by the plugin's OIC server to allow the plugin to use IoTivity apis
 *
 * returns:
 *     GW_RESULT_OK             - no errors
 *     GW_RESULT_INTERNAL_ERROR - stack process error
 */
GW_RESULT plugin_start(plugin_ctx_t *plugin_specific_ctx)
{
    GW_RESULT result = GW_RESULT_OK;
    if (plugin_specific_ctx == NULL)
    {
        result = GW_RESULT_INTERNAL_ERROR;
    }

    return (result);
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

/*
 *  For initiating the scan for thermostats
 *  Parameters :
 *             plugin_ctx_t - Plugin context ,
 *             plugin_scan_t - Scan Message.
 */
GW_RESULT plugin_scan(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx; (void) message;
    int nestResult = GW_RESULT_OK;

    std::vector<NestThermostatSharedPtr> thermostatScanned;

    nestResult = g_nest->getThermostats(thermostatScanned);
    if (GW_RESULT_OK == nestResult)
    {
        if (thermostatScanned.size() <= 0)
        {
            OIC_LOG(INFO, TAG, "getThermostats succeeded but zero thermostats found");
        }
        else
        {
            for (uint32_t i = 0; i < thermostatScanned.size(); ++i)
            {
                NestThermostatSharedPtr thermostat = thermostatScanned[i];
                NestThermostat::THERMOSTAT data;
                thermostat->get(data);
                std::string uri = "/nest/" + data.devInfo.id;
                OIC_LOG_V(INFO, TAG, "uri: %s", uri.c_str());

                if(addedThermostats.find(uri) != addedThermostats.end())
                {
                    OIC_LOG_V(INFO, TAG,"Already added %s. Ignoring", uri.c_str());
                    continue;
                }

                uriToNestThermostatMap[uri] = thermostat;

                std::string s = uri;
                unsigned long size = s.size() + 1;
                char *m = (char*) OICCalloc(1, size);
                memcpy(m, s.c_str(), size);
                send_response(m, size, MPM_SCAN);
                OICFree(m);

            }
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "getThermostats returned error %i", nestResult);
    }

    return nestResult;

}

/***
 * Creates 1 OCF resource for a NestThermostat for temperature control.
 *
 * @param uri Base uri. Resource uri is the same as baseUri
 * @return
 */
GW_RESULT createOCFResource(const std::string &uri)
{
    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    if (createSecureResources()) resourceProperties |= OC_SECURE;

    ConcurrentIotivityUtils::createResource(uri, NEST_THERMOSTAT_RT, NEST_THERMOSTAT_IF, resource_entity_handler_cb,
                                            NULL, resourceProperties);

    return GW_RESULT_OK;
}

GW_RESULT  deleteOCFResource(const std::string &uri)
{
    ConcurrentIotivityUtils::deleteResource(uri);

    return GW_RESULT_OK;
}

void createPayloadForMetadata(resourceList **list, const char *uri)
{
    resourceList *tempPtr = NULL;
    tempPtr = (resourceList *) OICCalloc(1, sizeof(resourceList));
    if(tempPtr == NULL)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed");
        return;
    }

    OICStrcpy(tempPtr->rt, MAX_LENGTH_64, NEST_THERMOSTAT_RT.c_str());
    OICStrcpy(tempPtr->href, MAX_URI_LEN, uri);
    OICStrcpy(tempPtr->interfaces, MAX_LENGTH_64, NEST_THERMOSTAT_IF.c_str());
    tempPtr->bitmap = BM;
    tempPtr->next = *list;
    *list  = tempPtr;
}

void updatePluginSpecificData(NestThermostat::THERMOSTAT thermostat, plugin_specific_data_t *pluginDetails)
{
    pluginDetails->humidity = thermostat.humidity;
    pluginDetails->hvacMode = thermostat.hvacMode;
    pluginDetails->targetTempF = thermostat.targetTempF;
    pluginDetails->temperature = thermostat.temperature;
    memcpy(pluginDetails->accessToken, g_nest->getTok().c_str(), g_nest->getTok().length());
    memcpy(pluginDetails->deviceId, thermostat.devInfo.id.c_str(), thermostat.devInfo.id.length());
}

GW_RESULT plugin_add(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx;
    char *c_uri = NULL;
    uint8_t *buff = (uint8_t *)OICCalloc(1, MAX_METADATA_LEN);
    resourceList *list = NULL;

    device_specific_data_t deviceConfiguration;
    plugin_specific_data_t pluginSpecificDetails;
    memset(&deviceConfiguration, 0, sizeof(device_specific_data_t));
    memset(&pluginSpecificDetails, 0, sizeof(plugin_specific_data_t));

    if(message->payloadSize > 0 && message->payloadSize < SIZE_MAX)
    {
        c_uri = (char*) OICCalloc(1, message->payloadSize);
    }
    if(c_uri == NULL)
    {
        OIC_LOG(ERROR, TAG, "OICCalloc Failed");
        OICFree(buff);
        return GW_RESULT_INTERNAL_ERROR;
    }
    memcpy(c_uri, message->payload, message->payloadSize);
    std::string uri(c_uri);
    OICFree(c_uri);

    if(addedThermostats.find(uri) != addedThermostats.end())
    {
        OIC_LOG_V(ERROR, TAG,"%s already added", uri.c_str());
        return GW_RESULT_ALREADY_CREATED;
    }
    if(uriToNestThermostatMap.find(uri) == uriToNestThermostatMap.end())
    {
        OIC_LOG_V(ERROR, TAG,"%s was NOT discovered in a scan", uri.c_str());
        return GW_CB_RESULT_ERROR;
    }

    createOCFResource(uri);
    addedThermostats[uri] = uriToNestThermostatMap[uri];

    createPayloadForMetadata(&list,uri.c_str());

    NestThermostat::THERMOSTAT thermostat;
    addedThermostats[uri]->get(thermostat);
    updatePluginSpecificData(thermostat, &pluginSpecificDetails);

    OICStrcpy(deviceConfiguration.devName, MAX_LENGTH_64, DEVICE_NAME);
    OICStrcpy(deviceConfiguration.devType, MAX_LENGTH_64, DEVICE_TYPE);
    OICStrcpy(deviceConfiguration.manufacturerName, MAX_LENGTH_256, MANUFACTURER_NAME);
    formMetaData(list, &deviceConfiguration, buff, MAX_METADATA_LEN, (void *)&pluginSpecificDetails, sizeof(plugin_specific_data_t));

    add_response response;
    memset(&response, 0, sizeof(add_response));
    OICStrcpy(response.uri, MAX_URI_LEN, uri.c_str());
    memcpy(response.metadata, buff, MAX_METADATA_LEN);

    size_t size = sizeof(add_response);

    send_response(&response, size, MPM_ADD);

    OICFree(buff);
    return GW_RESULT_OK;
}

GW_RESULT plugin_remove(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx;
    char *c_uri = NULL;

    if(message->payloadSize > 0 && message->payloadSize < SIZE_MAX)
    {
        c_uri = (char*) OICCalloc(1, message->payloadSize);
    }
    if(c_uri == NULL)
    {
        OIC_LOG(ERROR, TAG, "OICCalloc Failed");
        return GW_RESULT_INTERNAL_ERROR;
    }
    memcpy(c_uri, message->payload, message->payloadSize);
    std::string uri(c_uri);
    OICFree(c_uri);

    deleteOCFResource(uri);

    addedThermostats.erase(uri);
    uriToNestThermostatMap.erase(uri);

    std::string s = uri;
    unsigned long size = s.size() + 1;
    char *m = (char*) OICCalloc(1, size);
    memcpy(m, s.c_str(), size);
    send_response(m, size, MPM_REMOVE);
    OICFree(m);

    return GW_RESULT_OK;
}

GW_RESULT plugin_reconnect(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void)ctx;
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    resourceList *list = NULL, *temp = NULL;
    char *buff = NULL;
    std::vector<NestThermostatSharedPtr> thermostatsReconnected;
    void *details = NULL;

    if(message->payloadSize > 0 &&message->payloadSize < SIZE_MAX)
    {
        buff = (char *)OICCalloc(1, message->payloadSize);
        if(buff == NULL)
        {
        	OIC_LOG(ERROR, TAG, "OICCalloc Failed");
        	return GW_RESULT_INTERNAL_ERROR;
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "payload size is invalid");
        return GW_RESULT_INTERNAL_ERROR;
    }

    memcpy(buff, message->payload, message->payloadSize);

    ParseMetaData((uint8_t*)buff, MAX_METADATA_LEN, &list, &details);

    plugin_specific_data_t *pluginDetails = (plugin_specific_data_t *)details;

    std::shared_ptr<NestThermostat> sharedThermostat = std::make_shared<NestThermostat>
                                                                        (pluginDetails->accessToken,
    		                                                             pluginDetails->humidity,
                                                                         pluginDetails->hvacMode,
                                                                         pluginDetails->targetTempF,
                                                                         pluginDetails->temperature,
                                                                         pluginDetails->deviceId);
    thermostatsReconnected.push_back(sharedThermostat);

    std::string uri;

    NestThermostat::THERMOSTAT data;

    sharedThermostat->get(data);
    uri = "/nest/" + data.devInfo.id;

    if(uriToNestThermostatMap.find(uri) != uriToNestThermostatMap.end())
    {
        OIC_LOG_V(INFO, TAG, "Already found %s. Ignoring", uri.c_str());
    }
    else
    {
        uriToNestThermostatMap[uri] = sharedThermostat;
     }

    if(addedThermostats.find(uri) != addedThermostats.end())
    {
        OIC_LOG_V(ERROR, TAG, "%s already added", uri.c_str());
        result = GW_RESULT_ALREADY_CREATED;
        goto CLEANUP;
    }
    if(uriToNestThermostatMap.find(uri) == uriToNestThermostatMap.end())
    {
        result = GW_CB_RESULT_ERROR;
        goto CLEANUP;
    }

    while(list)
    {
    	temp = list;
        std::string resourceUri(list->href);
        OIC_LOG_V(INFO, TAG, "resource uri = %s", resourceUri.c_str());
        createOCFResource(uri);
        list = list->next;
        OICFree(temp);
    }

    addedThermostats[uri] = uriToNestThermostatMap[uri];
    getTemperatureAndUpdateMap(addedThermostats[uri]);
    result = GW_RESULT_OK;

    CLEANUP:
    if(buff != NULL)
    {
        OICFree(buff);
        buff = NULL;
    }
    if(pluginDetails != NULL)
    {
        OICFree(pluginDetails);
        pluginDetails = NULL;
    }

    return result;
}

/*  Plugin specific entry-point function to stop the plugin's threads
 *
 * returns:
 *     GW_RESULT_OK             - no errors
 *     GW_RESULT_INTERNAL_ERROR - stack process error
 */
GW_RESULT plugin_stop(plugin_ctx_t *plugin_specific_ctx)
{
    plugin_ctx_t *ctx = plugin_specific_ctx;

    if (NULL != ctx && g_ctx != NULL)
    {
        /*for(auto itr: addedThermostats)
        {
        	deleteOCFResource(itr.first);
        }*/
    	OCStopPresence();

        addedThermostats.clear();
        uriToNestThermostatMap.clear();

        if (plugin_specific_ctx->started == true)
        {
            plugin_specific_ctx->stay_in_process_loop = false;
            plugin_specific_ctx->started = false;
        }
    }

    return (GW_RESULT_OK);
}

/* Plugin specific entry-point function to allow the plugin resources to be
 * freed.
 *
 * returns:
 *     GW_RESULT_OK             - no errors
 *     GW_RESULT_INTERNAL_ERROR - stack process error
 */
GW_RESULT plugin_destroy(plugin_ctx_t *plugin_specific_ctx)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    if (plugin_specific_ctx != NULL)
    {
        if (plugin_specific_ctx->started == true)
        {
            plugin_stop(plugin_specific_ctx);
        }

        delete(g_nest);
        OICFree(plugin_specific_ctx);
        g_ctx = NULL;
        result = GW_RESULT_OK;
    }

    OIC_LOG_V(INFO, TAG, "Plugin destroy's return value:%d", result);

    return (result);
}



NestThermostatSharedPtr getNestThermostatFromOCFResourceUri(std::string resourceUri)
{
    OIC_LOG_V(INFO, TAG, "Request for %s", resourceUri.c_str());

    for(auto uriToNestPair : addedThermostats)
    {
        if (resourceUri.find(uriToNestPair.first) != std::string::npos)
        {
            return uriToNestPair.second;
        }
    }
    throw "Resource" + resourceUri + " not found";
}

OCRepPayload *addCommonNestProperties(const NestThermostatSharedPtr &t, OCRepPayload *payload)
{
    NestThermostat::THERMOSTAT data;
    t->get(data);
    OCRepPayloadSetPropString(payload, NEST_ID_TAG, data.devInfo.id.c_str());
    OCRepPayloadSetPropString(payload, NEST_LAST_CONNECTION_TAG, data.devInfo.lastConnection.c_str());
    return payload;
}

uint16_t getTemperatureAndUpdateMap(NestThermostatSharedPtr t)
{
    NestThermostat::THERMOSTAT targetThermostat;
    t->get(targetThermostat);

    std::vector<NestThermostatSharedPtr> thermostatsGot;
    g_nest->getThermostats(thermostatsGot);

    for(unsigned int i = 0; i < thermostatsGot.size(); ++i)
    {
        NestThermostat::THERMOSTAT fetchedThermostat;
        thermostatsGot[i]->get(fetchedThermostat);
        if(fetchedThermostat.devInfo.id == targetThermostat.devInfo.id)
        {
            if(targetThermostat.targetTempF != fetchedThermostat.targetTempF)
                {
                    OIC_LOG(INFO, TAG, "temperature value has changed");
                    targetThermostat.targetTempF = fetchedThermostat.targetTempF;
                }
                break;
        }
    }

    return targetThermostat.targetTempF;
}

OCRepPayload *getTemperaturePayload(NestThermostatSharedPtr t)
{
    OCRepPayload *payload = OCRepPayloadCreate();

    OCRepPayloadSetPropDouble(payload, TEMPERATURE_TAG, (double) getTemperatureAndUpdateMap(t));

    return addCommonNestProperties(t, payload);

}

OCRepPayload* process_get_request(NestThermostatSharedPtr t)
{
    return getTemperaturePayload(t);
}

OCEntityHandlerResult processTemperatureUpdate(OCRepPayload *payload, NestThermostatSharedPtr t)
{
    double targetTemp = 0.0;
    if (!OCRepPayloadGetPropDouble(payload, TEMPERATURE_TAG, &targetTemp))
    {
        throw "Payload must contain \"temperature\"";
    }
    int result = g_nest->setAwayMode(Nest::eAWHome);
    if (result == GW_RESULT_OK)
    {
        result = t->setTemperature(targetTemp);

        if (result != GW_RESULT_OK)
        {
            throw "Error setting brightness for PUT request";
        }
    }
    else
    {
        throw "Error setting away mode to home mode for PUT request";
    }
    return OC_EH_OK;
}

OCEntityHandlerResult process_put_request(OCRepPayload *payload, NestThermostatSharedPtr t)
{
    if (payload == NULL)
    {
        throw "PUT payload cannot be NULL";
    }

    return processTemperatureUpdate(payload, t);
}

OCEntityHandlerResult resource_entity_handler_cb(OCEntityHandlerFlag flag,
                                                 OCEntityHandlerRequest *entityHandlerRequest, void *callbackParam)
{
    (void) flag;
    (void) callbackParam;
    OCEntityHandlerResult result = OC_EH_OK;
    GW_RESULT res = GW_RESULT_OK;

    try
    {
        std::string uri;
        ConcurrentIotivityUtils::getUriFromHandle(entityHandlerRequest->resource, uri);

        NestThermostatSharedPtr targetThermostat = getNestThermostatFromOCFResourceUri(uri);

        switch (entityHandlerRequest->method)
        {
            case OC_REST_GET:
                // Empty GET case as actual request will be processed after the switch case.
                break;

            case OC_REST_PUT:
            case OC_REST_POST:

                res = process_put_request((OCRepPayload*) entityHandlerRequest->payload, targetThermostat);
                if (res != GW_RESULT_OK)
                     result = OC_EH_ERROR;
                 break;

            default:
                OIC_LOG_V(INFO, TAG,"Unsupported method (%d) recieved", entityHandlerRequest->method);
                ConcurrentIotivityUtils::respondToRequestWithError(entityHandlerRequest, "Unsupported method received",
                    OC_EH_METHOD_NOT_ALLOWED);
                return OC_EH_OK;
        }

        OCRepPayload *responsePayload = process_get_request(targetThermostat);

        ConcurrentIotivityUtils::respondToRequest(entityHandlerRequest, responsePayload, result);

        OCRepPayloadDestroy(responsePayload);
    }
    catch (const char *errorMessage)
    {
        ConcurrentIotivityUtils::respondToRequestWithError(entityHandlerRequest, errorMessage, OC_EH_ERROR);
        return OC_EH_OK;
    }

    return OC_EH_OK;
}
