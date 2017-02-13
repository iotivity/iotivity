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
 * of native devices into the resource model of OCF.  The file is divided into two
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

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <fstream>
#include <map>
#include <iostream>
#include "honeywell.h"
#include "gw_error_code.h"
#include "plugin_server.h"
#include "rapidjson.h"
#include "document.h"
#include "stringbuffer.h"

#include "oic_malloc.h"
#include "oic_string.h"
#include "honeywell_lyric.h"
#include "logger.h"
#include "ConcurrentIotivityUtils.h"
#include "octypes.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "message_handler.h"

#include "honeywell_thermostat.h"
#include "honeywell_helpers.h"

#define LOG_TAG "HONEYWELL_RESOURCE"

#define MANUFACTURER_NAME "HONEYWELL"
#define DEVICE_NAME "Honeywell Lyric Translator"
#define DEVICE_TYPE "oic.d.thermostat"
#define MAX_RESOURCES 3
#define MAX_DEVICE_ID_LEN 32
#define BM 3
#define MAX_CHANGEABLEVALUES_LEN 102

using namespace OC::Bridging;
/*******************************************************************************
 * Pound defines and structure declarations go here
 ******************************************************************************/
typedef struct {
        int locationId;
        char deviceIdStr[MAX_DEVICE_ID_LEN];
        char uniqueId[MAX_UNIQUE_ID_LEN];
        double ambientTempF;
        char changeableValues[MAX_CHANGEABLEVALUES_LEN];
}plugin_specific_data_t;

/* Resources can come and go based on having a IOT device added or removed from
 * the physical infrastructure.  Also the Iotivity entity handler does not pass
 * or present a resource handle in the entity handler callback.  Therefore, in
 * the plugin specific code an associative structure needs to be kept around in
 * global space to allow the implementer to make the association between URI and
 * other attributes corresponding with the IOT device in the data model.
 *
 * NOTE: Iotivity guarantees that one can not represent two devices with exactly
 *       the same URI, so keying off of the URI is a successful strategy.
 */

/*******************************************************************************
 * global data goes here
 ******************************************************************************/

std::map<std::string, LyricThermostatSharedPtr> uriToLyricThermostatMap;
std::map<std::string, LyricThermostatSharedPtr> addedThermostats;

HoneywellLyric g_honeywell;
Honeywell::ACCESS_TOKEN m_token;
bool g_isAuthorized = false;
Honeywell::CLIENT_ID_SECRET m_clientId_secret;

/*******************************************************************************
 * prototypes go here
 ******************************************************************************/
GW_RESULT load_access_token(const char *filename, Honeywell::ACCESS_TOKEN &token);

OCEntityHandlerResult resource_entity_handler_cb(OCEntityHandlerFlag flag,
                                                 OCEntityHandlerRequest *entityHandlerRequest,
                                                 void *callbackParam);

OCEntityHandlerResult process_put_request(OCRepPayload * payload, LyricThermostatSharedPtr targetThermostat, const std::string uri);

OCRepPayload *get_payload(const char *uri, const THERMOSTAT &data);

void *accessTokenMonitorThread(void *pointer);

//bool is_authorised;

static char CRED_FILE[] = "./oic_svr_db_lyric.dat";

FILE *honeywell_fopen(const char *path, const char *mode)
{
    (void) path;
    return fopen(CRED_FILE, mode);
}

plugin_ctx_t *g_plugin_specific_ctx = NULL;

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

    // initialize global data and return value
    *plugin_specific_ctx = NULL;

    /* allocate a context structure for the plugin */
    plugin_ctx_t *ctx = (plugin_ctx_t *) OICMalloc(sizeof(plugin_ctx_t));

    /* initialize the plugin context */
    if (ctx != NULL)
    {
        memset(ctx, 0, sizeof(plugin_ctx_t));
        *plugin_specific_ctx = ctx;
    }
    else
    {
        OIC_LOG(ERROR, LOG_TAG, "Unable to allocate plugin specific context.");
        result = GW_RESULT_INTERNAL_ERROR;
        return result;
    }

    ctx->device_name = (char *) DEVICE_NAME;
    ctx->resource_type = (char *) DEVICE_TYPE;
    ctx->open = honeywell_fopen;

    FILE *fp = fopen("./lyric.cnf", "r");

    if (NULL == fp)
    {
        OIC_LOG(ERROR, LOG_TAG, "error loading lyric.cnf file.");
        return GW_RESULT_INTERNAL_ERROR;
    }
    char code[HONEYWELL_REFRESH_TOKEN_BUFSIZE];
    char str[1024];
    if (fgets(str, 1023, fp) == NULL)
    {
        OIC_LOG(ERROR, LOG_TAG, "Failed to read ./lyric.cnf");
        fclose(fp);
        return GW_RESULT_INTERNAL_ERROR;
    }

    str[strlen(str) - 1] = '\0';
    OICStrcpy(code, HONEYWELL_REFRESH_TOKEN_BUFSIZE, str);

    if (fgets(str, 1023, fp) == NULL)
    {
        OIC_LOG(ERROR, LOG_TAG, "Failed to read ./lyric.cnf");
        fclose(fp);
        return GW_RESULT_INTERNAL_ERROR;
    }

    str[strlen(str) - 1] = '\0';
    OICStrcpy(m_clientId_secret.honeywell_clientId, HONEYWELL_CLIENT_ID_BUFFSIZE, str);
    if (fgets(str, 1023, fp) == NULL)
    {
        OIC_LOG(ERROR, LOG_TAG, "Failed to read ./lyric.cnf");
        fclose(fp);
        return GW_RESULT_INTERNAL_ERROR;
    }

    str[strlen(str) - 1] = '\0';
    OICStrcpy(m_clientId_secret.honeywell_client_secret, HONEYWELL_CLIENT_AND_SECRET_64_BUFFSIZE, str);
    fclose(fp);

    g_honeywell.setClientIdAndSecret(m_clientId_secret);

    std::string acode;
    acode.assign(code);
    result = g_honeywell.getAccessToken(acode, m_token);
    if (GW_RESULT_OK != result)
    {
        OIC_LOG_V(ERROR, LOG_TAG, "getAccessToken failed with %d", result);
        // TODO - what to do in case of failure? free resources? reset auth flag?
        g_isAuthorized = false;
        return GW_RESULT_INTERNAL_ERROR;
    }
    else
    {
        OIC_LOG(DEBUG, LOG_TAG, "getAccessToken is successful");
        g_isAuthorized = true;
        g_honeywell.setAccessToken(m_token);
    }

    OIC_LOG_V(INFO, LOG_TAG, "Plugin create return value: %d.", result);

    return (result);
}

/* CALLED BY the plugin's OIC server to allow the plugin to use OIC apis
 *
 * returns:
 *     GW_RESULT_OK             - no errors
 *     GW_RESULT_INTERNAL_ERROR - stack process error
 */
GW_RESULT plugin_start(plugin_ctx_t *plugin_specific_ctx)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    int error = -1;

    if (plugin_specific_ctx != NULL)
    {
        // set global plugin context
        g_plugin_specific_ctx = plugin_specific_ctx;

        /* create house keeping thread */
        plugin_specific_ctx->stay_in_process_loop = true;
        error = pthread_create(&(plugin_specific_ctx->thread_handle), NULL, accessTokenMonitorThread, plugin_specific_ctx);
        if (error == 0)
        {
            plugin_specific_ctx->started = true;
            result = GW_RESULT_OK;
        }
        else
        {
            OIC_LOG_V(ERROR, LOG_TAG, "Can't create plugin specific thread :[%s]", strerror(error));
            plugin_stop(plugin_specific_ctx);
            result = GW_RESULT_STARTED_FAILED;
        }
    }

    OIC_LOG_V(INFO, LOG_TAG, "Plugin start return value: %d.", result);

    return (result);
}

/*  Plugin specific entry-point function to stop the plugin's threads
 *
 * returns:
 *     GW_RESULT_OK             - no errors
 *     GW_RESULT_INTERNAL_ERROR - stack process error
 */
GW_RESULT plugin_stop(plugin_ctx_t *plugin_specific_ctx)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;

    if (NULL != plugin_specific_ctx)
    {
        result = GW_RESULT_OK;

        if (plugin_specific_ctx->started == true)
        {
            plugin_specific_ctx->stay_in_process_loop = false;
            pthread_join(plugin_specific_ctx->thread_handle, NULL);
            plugin_specific_ctx->started = false;
        }

        // TODO - Need to stop presence here like in Nest?
    }

    OIC_LOG_V(INFO, LOG_TAG, "Plugin stop's return value:%d", result);

    return (result);
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

        // freeing the resource allocated in create
        OICFree(plugin_specific_ctx);
        plugin_specific_ctx = NULL;
        result = GW_RESULT_OK;
    }

    OIC_LOG_V(INFO, LOG_TAG, "Plugin destroy's return value:%d", result);

    return (result);
}

/*
 * This method checks if the request is for an actuator(oic.if.a) resource or a
 * sensor(oic.if.s) resource.
 * Sensors allow GET.
 * Actuators allow GET PUT POST.
 * Both allow GET
 * Sensors resources end with /current as in the createNewResources() in this file.
 * Actuators have uris ending with /heater or /cooler as seen in createNewResources() in this file.
*/

OCEntityHandlerResult checkIfOperationIsAllowed(std::string uri, OCMethod operation)
{
    if (operation == OC_REST_GET)
    {
        return OC_EH_OK;
    }
    if (operation == OC_REST_DELETE)
    {
        return OC_EH_FORBIDDEN;
    }

    std::string sensor_suffix = "/current";

    // uri is smaller than suffix.. which is weird.. but not a failure in this function.
    if (uri.length() < sensor_suffix.length())
    {
        return OC_EH_OK;
    }

    // Code here means operation is not a GET. Disallow operation if it is a sensor.
    if (std::equal(sensor_suffix.rbegin(), sensor_suffix.rend(), uri.rbegin()))
    {
        OIC_LOG(INFO, LOG_TAG, "PUT/POST not allowed on sensors");
        return OC_EH_FORBIDDEN;
    }

    return OC_EH_OK;
}

OCRepPayload *get_payload(const char *uri, const THERMOSTAT &data)
{
    bool result = true;
    std::string modeString;
    OCRepPayload *payload = OCRepPayloadCreate();
    if (NULL == payload)
    {
        OIC_LOG(ERROR, LOG_TAG, "Failed to allocate Payload");
        result = false;
    }

    OIC_LOG_V(INFO, LOG_TAG,
          "coolSP: %f, heatSP: %f, ambient: %f, desired: %f",
          data.coolSetpointF,
          data.heatSetpointF,
          data.ambientTempF,
          compute_target_temp(data.heatSetpointF, data.coolSetpointF));

    if (result)
    {
        result = OCRepPayloadSetUri(payload, uri);
        if (false == result)
        {
            OIC_LOG(ERROR, LOG_TAG, "OCRepPayloadSetUri failed");
        }
    }

    if (result)
    {
        result = OCRepPayloadAddResourceType(payload, HONEYWELL_THERMOSTAT_RT);
        if (false == result)
        {
            OIC_LOG(ERROR, LOG_TAG, "OCRepPayloadAddResourceType failed");
        }
    }

    if (result)
    {
        // high target temp is cool setpoint (hottest it gets before cooling kicks on)
        result = OCRepPayloadSetPropDouble(payload, REP_NAME_TARGET_TEMP_HIGH, data.coolSetpointF);
        if (false == result)
        {
            OIC_LOG(ERROR, LOG_TAG, "OCRepPayloadSetPropDouble REP_NAME_TARGET_TEMP_HIGH failed");
        }
    }
    if (result)
    {
        // low target temp is heat setpoint (coolest it gets before heating kicks on)
        result = OCRepPayloadSetPropDouble(payload, REP_NAME_TARGET_TEMP_LOW, data.heatSetpointF);
        if (false == result)
        {
            OIC_LOG(ERROR, LOG_TAG, "OCRepPayloadSetPropDouble REP_NAME_TARGET_TEMP_LOW failed");
        }
    }
    if (result)
    {
        // Return ambient/indoor temperature (if available) in optional property
        result = OCRepPayloadSetPropDouble(payload, REP_NAME_INDOOR_TEMP, data.ambientTempF);
        if (false == result)
        {
            OIC_LOG(ERROR, LOG_TAG, "OCRepPayloadSetPropDouble REP_NAME_INDOOR_TEMP failed");
        }
        // Return desired/target temperature for both gets and sets
        double temperature = compute_target_temp(data.heatSetpointF, data.coolSetpointF);
        OIC_LOG_V(INFO, LOG_TAG, "Setting temperature in payload as %f", temperature);

        result = OCRepPayloadSetPropDouble(
                payload, REP_NAME_TEMPERATURE, temperature);
        if (false == result)
        {
            OIC_LOG(ERROR, LOG_TAG, "OCRepPayloadSetPropDouble REP_NAME_TEMPERATURE failed");
        }
    }
    if (result)
    {
        if (data.hvacMode == HVAC_COOL)
        {
            modeString = REP_VALUE_COOL;
        }
        else if (data.hvacMode == HVAC_HEAT)
        {
            modeString = REP_VALUE_HEAT;
        }
        else
        {
            modeString = REP_VALUE_OFF;
        }

        result = OCRepPayloadSetPropString(payload, REP_NAME_MODE, modeString.c_str());
        if (false == result)
        {
            OIC_LOG(ERROR, LOG_TAG, "OCRepPayloadSetPropString REP_NAME_MODE failed");
        }
    }

    if (false == result)
    {
        OIC_LOG(ERROR, LOG_TAG, "Failed to set payload value(s)");

        // if we are in an error state, we need to free the payload.
        if (NULL != payload)
        {
            OCRepPayloadDestroy(payload);
            payload = NULL;
        }
    }

    return payload;
}

OCEntityHandlerResult
resourceEntityHandler(OCEntityHandlerFlag flag, OCEntityHandlerRequest *request, void *callbackParam)
{
    (void) callbackParam;
    OIC_LOG_V(INFO, LOG_TAG, "Entity handler callback - flags:0x%x", flag);
    (void) flag;

    OCEntityHandlerResult result = OC_EH_OK;

    try
    {
        std::string resourceUri;
        std::size_t found = 0;
        ConcurrentIotivityUtils::getUriFromHandle(request->resource, resourceUri);
        found = resourceUri.find_last_of("/");
        std::string uri = resourceUri.substr(0, found);  //device uri
        LyricThermostatSharedPtr targetThermostat = addedThermostats[uri];
        THERMOSTAT data;

        result = checkIfOperationIsAllowed(resourceUri, request->method);
        if (result != OC_EH_OK)
        {
           OIC_LOG_V(INFO, LOG_TAG, "Operation not allowed on %s", resourceUri.c_str());
           return result;
        }

        switch (request->method)
        {
            case OC_REST_GET:
                // Empty GET case as actual request will be processed after the switch case.
                OIC_LOG_V(INFO, LOG_TAG, "GET on %s", resourceUri.c_str());
                break;

            case OC_REST_PUT:
            case OC_REST_POST:

                OIC_LOG_V(INFO, LOG_TAG, "UPDATE on %s", resourceUri.c_str());
                result = process_put_request((OCRepPayload*) request->payload, targetThermostat, resourceUri);
                if (result != OC_EH_OK)
                {
                    OIC_LOG_V(ERROR, LOG_TAG, "process_put_request returned Error = %d", result);
                }

                break;

            default:
                OIC_LOG_V(INFO, LOG_TAG,"Unsupported method (%d) received", request->method);
                ConcurrentIotivityUtils::respondToRequestWithError(request, "Unsupported method received",
                    OC_EH_METHOD_NOT_ALLOWED);
                return OC_EH_OK;
        }

        targetThermostat->get(data);
        OCRepPayload *payload = get_payload(uri.c_str(), data);

        ConcurrentIotivityUtils::respondToRequest(request, payload, result);
        OCRepPayloadDestroy(payload);
    }

    catch (std::string errorMessage)
    {
        ConcurrentIotivityUtils::respondToRequestWithError(request, errorMessage.c_str(), OC_EH_ERROR);
        return OC_EH_OK;
    }

    return OC_EH_OK;
}


OCEntityHandlerResult process_put_request(OCRepPayload * payload, LyricThermostatSharedPtr targetThermostat, const std::string uri)
{
    OCEntityHandlerResult ehResult = OC_EH_OK;
    OIC_LOG_V(INFO, LOG_TAG, "Put request for %s", uri.c_str());
    THERMOSTAT localData;
    int result = GW_RESULT_OK;
    if (!payload)
    {
        OIC_LOG(ERROR, LOG_TAG, "Entity handler request payload is empty while processing PUT request");
        return OC_EH_ERROR;
    }

    // Get pointer to query
    if (!OCRepPayloadGetPropDouble(payload, REP_NAME_TEMPERATURE, &(localData.targetTempF)))
    {
        OIC_LOG(ERROR, LOG_TAG, "REP_NAME_TEMPERATURE not found in payload (data).");
        return OC_EH_ERROR;
    }

    OIC_LOG_V(INFO, LOG_TAG, "localData.targetTempF %f", localData.targetTempF);
    // compute cool and hot setpoints based on desired temperature
    // NOTE: low = heatSetpoint, high = coolSetpoint (they are the temperatures those
    //       modes try to achieve.)
    compute_setpoints(localData.targetTempF, localData.heatSetpointF, localData.coolSetpointF);

    OIC_LOG_V(INFO, LOG_TAG, "localData.heatSetpointF %f", localData.heatSetpointF);
    OIC_LOG_V(INFO, LOG_TAG, "localData.coolSetpointF %f", localData.coolSetpointF);

    result = g_honeywell.setTemperature(targetThermostat, localData, uri);

    OIC_LOG_V(INFO, LOG_TAG, "setTemperature returned result = %d", result);
    if (result != GW_RESULT_OK)
    {
        throw "Error setting temperature for PUT request";
    }

    return ehResult;
}

/*
 *  For initiating the scan for lights
 *  Parameters :
 *             plugin_ctx_t - Plugin context ,
 *             plugin_scan_t - Scan Message.
 */
GW_RESULT plugin_scan(plugin_ctx_t *ctx, mpm_pipe_message_t *message)
{
    OIC_LOG(INFO, LOG_TAG, "Inside plugin_scan");
    (void)ctx;
    (void)message;
    std::vector<LyricThermostatSharedPtr> thermostatsScanned;

    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;

    result = g_honeywell.getThermostats(thermostatsScanned);
    if (GW_RESULT_OK == result)
    {
        for (uint32_t i = 0; i < thermostatsScanned.size(); ++i)
        {
            LyricThermostatSharedPtr thermostat = thermostatsScanned[i];

            std::string uri = "/honeywell/" + thermostat->getDeviceUniqueId();
            if(addedThermostats.find(uri) != addedThermostats.end())
            {
                OIC_LOG_V(INFO, LOG_TAG, "Already Added %s. Ignoring", uri.c_str());
                continue;
            }

            uriToLyricThermostatMap[uri] = thermostat;

            std::string s = uri;
            unsigned long size = s.size() + 1;
            char *m = (char*) OICMalloc(size);
            memcpy(m, s.c_str(), size);
            send_response(m, size, MPM_SCAN);
            OICFree(m);
        }
    }
    else
    {
        OIC_LOG_V(ERROR, LOG_TAG, "getThermostats failed with %d", result);
    }
    OIC_LOG(INFO, LOG_TAG, "Leaving plugin specific thread handler.");
    return result;
}

bool createSecureResources()
{
    char *non_secure_env = getenv("NONSECURE");

    if (non_secure_env != NULL && std::string(non_secure_env) == "true")
    {
        OIC_LOG(INFO, LOG_TAG, "Creating NON SECURE resources");
        return false;
    }
    OIC_LOG(INFO, LOG_TAG, "Creating SECURE resources");
    return true;
}

void createPayloadForMetadata(resourceList **list , const char *uri, const char * interface)
{
    resourceList *tempPtr;
    tempPtr = (resourceList *) OICCalloc(1, sizeof(resourceList));

    if (tempPtr == NULL)
    {
        OIC_LOG(ERROR, LOG_TAG, "Memory Allocation failed");
        return;
    }
    OICStrcpy(tempPtr->rt, MAX_LENGTH_64, HONEYWELL_THERMOSTAT_RT);
    OICStrcpy(tempPtr->href, MAX_URI_LEN, uri);
    OICStrcpy(tempPtr->interfaces, MAX_LENGTH_64, interface);
    tempPtr->bitmap = BM;
    tempPtr->next = *list;
    *list  = tempPtr;
}

void updatePluginSpecificData(THERMOSTAT thermostat, plugin_specific_data_t *pluginDetails)
{
    OICStrcpy(pluginDetails->deviceIdStr, MAX_DEVICE_ID_LEN, thermostat.devInfo.deviceIdStr.c_str());
    OICStrcpy(pluginDetails->uniqueId, MAX_UNIQUE_ID_LEN, thermostat.devInfo.uniqueId.c_str());
    pluginDetails->locationId = thermostat.devInfo.locationId;
    pluginDetails->ambientTempF = thermostat.ambientTempF;
}

/*
 * Add_device - Add device in the DISCOVERABLE mode
 * Input parameters - plugin_ctx_t        - Plugin context
 *                    plugin_adddevice_t  - Add device structure
 *
 */
GW_RESULT plugin_add(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void)ctx;
    char *c_uri = NULL;

    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    if (createSecureResources()) resourceProperties |= OC_SECURE;

    if(message->payloadSize > 0 && message->payloadSize < SIZE_MAX)
    {
        c_uri = (char*) OICCalloc(1, message->payloadSize);
    }
    if(c_uri == NULL)
    {
        OIC_LOG(ERROR, LOG_TAG, "Calloc Failed");
        return GW_RESULT_INTERNAL_ERROR;
    }

    memcpy(c_uri, message->payload, message->payloadSize);
    std::string uri(c_uri);
    OICFree(c_uri);

    if(addedThermostats.find(uri) != addedThermostats.end())
    {
        OIC_LOG_V(ERROR, LOG_TAG, "%s already added", uri.c_str());
        return GW_RESULT_ALREADY_CREATED;
    }
    if(uriToLyricThermostatMap.find(uri) == uriToLyricThermostatMap.end())
    {
        OIC_LOG_V(ERROR, LOG_TAG, "%s was NOT discovered in a scan", uri.c_str());
        return GW_CB_RESULT_ERROR;
    }

    resourceList *list = NULL;

    /* Heater Resource */
    std::string honeywell_device_actuator_heater_uri = uri + "/heater";
    ConcurrentIotivityUtils::createResource(honeywell_device_actuator_heater_uri, HONEYWELL_THERMOSTAT_RT, HONEYWELL_THERMOSTAT_ACTUATOR_IF, resourceEntityHandler, NULL, resourceProperties);
    createPayloadForMetadata(&list, honeywell_device_actuator_heater_uri.c_str(), HONEYWELL_THERMOSTAT_ACTUATOR_IF);

    /* Cooler Resource */
    std::string honeywell_device_actuator_cooler_uri = uri + "/cooler";
    ConcurrentIotivityUtils::createResource(honeywell_device_actuator_cooler_uri, HONEYWELL_THERMOSTAT_RT, HONEYWELL_THERMOSTAT_ACTUATOR_IF, resourceEntityHandler, NULL, resourceProperties);
    createPayloadForMetadata(&list, honeywell_device_actuator_cooler_uri.c_str(), HONEYWELL_THERMOSTAT_ACTUATOR_IF);

    /* Sensor Resource */
    std::string honeywell_device_sensor_uri = uri + "/current";
    ConcurrentIotivityUtils::createResource(honeywell_device_sensor_uri, HONEYWELL_THERMOSTAT_RT, HONEYWELL_THERMOSTAT_SENSOR_IF, resourceEntityHandler, NULL, resourceProperties);
    createPayloadForMetadata(&list, honeywell_device_sensor_uri.c_str(), HONEYWELL_THERMOSTAT_SENSOR_IF);

    addedThermostats[uri] = uriToLyricThermostatMap[uri];

    uint8_t * buff = (uint8_t *) OICCalloc(1, MAX_METADATA_LEN);
    plugin_specific_data_t pluginDetails;
    device_specific_data_t deviceDetails;
    THERMOSTAT thermostat;
    std::string changeableValues;

    addedThermostats[uri]->get(thermostat);
    changeableValues = addedThermostats[uri]->getChangeableValues();
    memset(&pluginDetails, 0, sizeof(plugin_specific_data_t));
    memset(&deviceDetails, 0, sizeof(device_specific_data_t));

    OICStrcpy(pluginDetails.changeableValues, MAX_CHANGEABLEVALUES_LEN, changeableValues.c_str());
    updatePluginSpecificData(thermostat, &pluginDetails);
    OICStrcpy(deviceDetails.devName, MAX_LENGTH_64, DEVICE_NAME);
    OICStrcpy(deviceDetails.devType, MAX_LENGTH_64, DEVICE_TYPE);
    OICStrcpy(deviceDetails.manufacturerName, MAX_LENGTH_256, MANUFACTURER_NAME);
    formMetaData(list, &deviceDetails, buff, MAX_METADATA_LEN, (void *)&pluginDetails, sizeof(plugin_specific_data_t));

    add_response add_rsp;
    memset(&add_rsp, 0, sizeof(add_response));
    OICStrcpy(add_rsp.uri, MAX_URI_LEN, uri.c_str());
    memcpy(add_rsp.metadata, buff, MAX_METADATA_LEN);
    send_response(&add_rsp, sizeof(add_response), MPM_ADD);
    OICFree(buff);
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
    (void)ctx;
    char *c_uri = NULL;

    if(message->payloadSize > 0 && message->payloadSize < SIZE_MAX)
    {
        c_uri = (char*) OICCalloc(1, message->payloadSize);
    }
    if(c_uri == NULL)
    {
        OIC_LOG(ERROR, LOG_TAG, "Calloc Failed");
        return GW_RESULT_INTERNAL_ERROR;
    }

    memcpy(c_uri, message->payload, message->payloadSize);
    std::string uri(c_uri);
    OICFree(c_uri);

    /* Heater Resource */
    std::string honeywell_device_actuator_heater_uri = uri + "/heater";
    ConcurrentIotivityUtils::deleteResource(honeywell_device_actuator_heater_uri);

    /* Cooler Resource */
    std::string honeywell_device_actuator_cooler_uri = uri + "/cooler";
    ConcurrentIotivityUtils::deleteResource(honeywell_device_actuator_cooler_uri);

    /* Sensor Resource */
    std::string honeywell_device_sensor_uri = uri + "/current";
    ConcurrentIotivityUtils::deleteResource(honeywell_device_sensor_uri);

    addedThermostats.erase(uri);
    uriToLyricThermostatMap.erase(uri);

    unsigned long size = uri.size() + 1;
    char *m = (char *) OICCalloc(1, size);
    memcpy(m, uri.c_str(), size);
    send_response(m, size, MPM_REMOVE);
    OICFree(m);

    return GW_RESULT_OK;
}

GW_RESULT plugin_reconnect(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void)ctx;
    resourceList *list = NULL, *temp = NULL;
    char *buff = NULL;
    THERMOSTAT thermostat;
    std::vector<LyricThermostatSharedPtr> thermostatsReconnected;
    void *details = NULL;
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    std::shared_ptr<HoneywellThermostat> sharedThermostat;
    std::string thermostatMode;
    std::string uri;

    if(message->payloadSize > 0 &&message->payloadSize < SIZE_MAX)
    {
        buff = (char *) OICCalloc(1, message->payloadSize);

        if (buff ==NULL)
        {
            OIC_LOG(ERROR, LOG_TAG, "OICCalloc Failed");
            return GW_RESULT_INTERNAL_ERROR;
        }
    }
    else
    {
        OIC_LOG(ERROR, LOG_TAG, "Payload size is out of bound");
        return GW_RESULT_INTERNAL_ERROR;
    }

    memcpy(buff, message->payload, message->payloadSize);
    ParseMetaData((uint8_t*)buff, MAX_METADATA_LEN, &list, &details);

    plugin_specific_data_t *pluginDetails = (plugin_specific_data_t *)details;
    HoneywellThermostat honeywellThermostat;

    thermostat.devInfo.locationId = pluginDetails->locationId;
    thermostat.devInfo.deviceIdStr.assign(pluginDetails->deviceIdStr);
    thermostat.devInfo.uniqueId.assign(pluginDetails->uniqueId);
    thermostat.ambientTempF = pluginDetails->ambientTempF;
    honeywellThermostat.setDeviceUniqueId(thermostat.devInfo.uniqueId.c_str());

    honeywellThermostat.setChangeableValues(pluginDetails->changeableValues);

    rapidjson:: Document values;
    values.SetObject();
    if (values.Parse(pluginDetails->changeableValues).HasParseError())
    {
        OIC_LOG(ERROR, LOG_TAG,"Parse error in set changeableValues");
        result = GW_RESULT_JSON_ERROR;
        goto CLEANUP;
    }

    if(values.HasMember(JSON_MODE))
    {
        thermostatMode = values[JSON_MODE].GetString();
    }

    if(values.HasMember(HONEYWELL_HEAT_SETPOINT))
    {
       thermostat.heatSetpointF = values[HONEYWELL_HEAT_SETPOINT].GetDouble();
    }

    if(values.HasMember(HONEYWELL_COOL_SETPOINT))
    {
       thermostat.coolSetpointF = values[HONEYWELL_COOL_SETPOINT].GetDouble();
    }

    if (0 == strncmp(thermostatMode.c_str(), THERMOSTAT_MODE_COOL, sizeof(THERMOSTAT_MODE_COOL)))
    {
        thermostat.hvacMode = HVAC_COOL;
    }
    else if (0 == strncmp(thermostatMode.c_str(), THERMOSTAT_MODE_HEAT, sizeof(THERMOSTAT_MODE_HEAT)))
    {
        thermostat.hvacMode = HVAC_HEAT;
    }
    else
    {
        thermostat.hvacMode = HVAC_OFF;
    }

    thermostat.targetTempF = compute_target_temp(thermostat.heatSetpointF, thermostat.coolSetpointF);
    dump_details(thermostat, "thermostatData");
    honeywellThermostat.set(thermostat);

    sharedThermostat = std::make_shared<HoneywellThermostat>(honeywellThermostat);

    uri = "/honeywell/" + sharedThermostat->getDeviceUniqueId();
    if(uriToLyricThermostatMap.find(uri) != uriToLyricThermostatMap.end())
    {
        OIC_LOG_V(INFO, LOG_TAG, "Already found %s. Ignoring", uri.c_str());
    }
    else
    {
        OIC_LOG_V(INFO, LOG_TAG, "Adding %s to uriToLyricThermostatMap", uri.c_str());
        uriToLyricThermostatMap[uri] = sharedThermostat;
    }

    if(addedThermostats.find(uri) != addedThermostats.end())
    {
        OIC_LOG_V(ERROR, LOG_TAG, "%s already added", uri.c_str());
        result = GW_RESULT_ALREADY_CREATED;
        goto CLEANUP;
    }
    if(uriToLyricThermostatMap.find(uri) == uriToLyricThermostatMap.end())
    {
        result = GW_CB_RESULT_ERROR;
        goto CLEANUP;
    }

    if (createSecureResources()) resourceProperties |= OC_SECURE;

    while(list)
    {
        temp = list;
        std::string resourceUri(list->href);
        OIC_LOG_V(INFO, LOG_TAG, "resource uri = %s", resourceUri.c_str());
        ConcurrentIotivityUtils::createResource(resourceUri, list->rt, list->interfaces, resourceEntityHandler, NULL, resourceProperties);
        list = list->next;
        OICFree(temp);
    }

    addedThermostats[uri] = uriToLyricThermostatMap[uri];
    result = GW_RESULT_OK;

    CLEANUP:
    if (buff != NULL)
    {
        OICFree(buff);
    }
    if (pluginDetails !=NULL)
    {
        OICFree(pluginDetails);
        details = NULL;
    }
    return result;
}

/*
 * The Lyric thermostat process loop.
 *
 * - manages honeywell device resources in response to changes in authentication status
 * - performs regular queries on honeywell devices in absence of true honeywell "observe" capabilities
 * - notifies observers of any changes in honeywell device properties
 * - performs re-authentication with Honeywell servers to keep a fresh access token (Lyric access tokens
 *   only last for 10 minutes)
 */
void *accessTokenMonitorThread(void *pointer)
{
    OIC_LOG(DEBUG, LOG_TAG, "Entered accessTokenMonitorThread");
    plugin_ctx_t *ctx = (plugin_ctx_t *) pointer;
    int reauthCountdown = HW_QUERY_INTERVAL_SECONDS; // period to wait before attempting re-auth
    bool lastAuthorized = false;
    std::string emptycode;
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;

    if (ctx != NULL)
    {
        while (ctx->stay_in_process_loop)
        {
            // check if there has been a change in authorization status
            if (lastAuthorized != g_isAuthorized)
            {
                // make sure we don't get into an unproductive cycle
                lastAuthorized = g_isAuthorized;
            }

            // Only do Lyric re-auth logic if we are already authenticated.
            if (lastAuthorized && (reauthCountdown <= 0))
            {
                result = g_honeywell.getAccessToken(emptycode, m_token);
                if (GW_RESULT_OK != result)
                {
                    OIC_LOG_V(ERROR, LOG_TAG, "getAccessToken failed with %d", result);
                    // TODO - what to do in case of failure? free resources? reset auth flag?
                    g_isAuthorized = false;
                }
                else
                {
                    // reset the counter
                    reauthCountdown = (HW_AUTH_LOOP_MINUTES * 60);
                    OIC_LOG(DEBUG, LOG_TAG, "getAccessToken is successful");
                    g_isAuthorized = true;
                    g_honeywell.setAccessToken(m_token);
                }
            }
            else
            {
                // not time to refresh token yet, just decrement
                reauthCountdown--;
            }
            sleep(THREAD_PROCESS_SLEEPTIME);
        }
        OIC_LOG(INFO, LOG_TAG,"Leaving LYRIC monitor thread");
    }
    pthread_exit(NULL);
}

