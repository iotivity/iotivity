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
#include "mpmErrorCode.h"
#include "pluginServer.h"
#include "rapidjson.h"
#include "document.h"
#include "stringbuffer.h"

#include "oic_malloc.h"
#include "oic_string.h"
#include "honeywellLyric.h"
#include "experimental/logger.h"
#include "ConcurrentIotivityUtils.h"
#include "octypes.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "messageHandler.h"

#include "honeywellThermostat.h"
#include "honeywellHelpers.h"

#define LOG_TAG "HONEYWELL_RESOURCE"

#define MANUFACTURER_NAME "HONEYWELL"
#define DEVICE_NAME "Honeywell Lyric Translator"
#define DEVICE_TYPE "oic.d.thermostat"
#define MAX_RESOURCES 3
#define MAX_DEVICE_ID_LEN 32
#define BM 3
#define MAX_CHANGEABLEVALUES_LEN 103

using namespace OC::Bridging;

/*******************************************************************************
 * Pound defines and structure declarations go here
 ******************************************************************************/
typedef struct {
        int locationId;
        char deviceIdStr[MAX_DEVICE_ID_LEN];
        char uniqueId[MPM_MAX_UNIQUE_ID_LEN];
        double ambientTempF;
        char changeableValues[MAX_CHANGEABLEVALUES_LEN];
} ThermostatDetails;

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
MPMResult loadAccessToken(const std::string &filename, Honeywell::ACCESS_TOKEN &token);

OCEntityHandlerResult resourceEntityHandlerCb(OCEntityHandlerFlag,
                                                 OCEntityHandlerRequest *entityHandlerRequest,
                                                 void *);

OCEntityHandlerResult processPutRequest(OCRepPayload * payload, LyricThermostatSharedPtr targetThermostat, const std::string uri);

OCRepPayload *getPayload(const std::string uri, const THERMOSTAT &data);

void *accessTokenMonitorThread(void *pointer);

FILE *honeywellFopen(const char *path, const char *mode)
{
    std::string filename = std::string("lyric_") + path;
    return fopen(filename.c_str(), mode);
}

MPMPluginCtx *g_pluginCtx = NULL;

MPMResult pluginCreate(MPMPluginCtx **pluginSpecificCtx)
{
    MPMResult result = MPM_RESULT_OK;

    if (g_pluginCtx != NULL)
    {
        OIC_LOG(ERROR, LOG_TAG, "Plugin is already created");
        return MPM_RESULT_ALREADY_CREATED;
    }

    /* allocate a context structure for the plugin */
    MPMPluginCtx *ctx = (MPMPluginCtx *) OICCalloc(1, sizeof(MPMPluginCtx));

    /* initialize the plugin context */
    if (ctx == NULL)
    {
        OIC_LOG(ERROR, LOG_TAG, "Unable to allocate plugin specific context.");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    *pluginSpecificCtx = ctx;
    g_pluginCtx = ctx;

    ctx->device_name = DEVICE_NAME;
    ctx->resource_type = DEVICE_TYPE;
    ctx->open = honeywellFopen;

    ifstream tokenFile ("./lyric.cnf");

    if (!tokenFile.is_open())
    {
        OIC_LOG(ERROR, LOG_TAG, "error loading lyric.cnf file.");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    std::string acode;

    if (!getline (tokenFile, acode))
    {
        OIC_LOG(ERROR, LOG_TAG, "Failed to read ./lyric.cnf");
        tokenFile.close();
        return MPM_RESULT_INTERNAL_ERROR;
    }

    std::string str;
    if (!getline (tokenFile, str))
    {
        OIC_LOG(ERROR, LOG_TAG, "Failed to read ./lyric.cnf");
        tokenFile.close();
        return MPM_RESULT_INTERNAL_ERROR;
    }
    OICStrcpy(m_clientId_secret.honeywell_clientId, HONEYWELL_CLIENT_ID_BUFFSIZE, str.c_str());

    if (!getline (tokenFile, str))
    {
        OIC_LOG(ERROR, LOG_TAG, "Failed to read ./lyric.cnf");
        tokenFile.close();
        return MPM_RESULT_INTERNAL_ERROR;
    }
    OICStrcpy(m_clientId_secret.honeywell_client_secret, HONEYWELL_CLIENT_AND_SECRET_64_BUFFSIZE, str.c_str());
    tokenFile.close();

    g_honeywell.setClientIdAndSecret(m_clientId_secret);

    result = (MPMResult) g_honeywell.getAccessToken(acode, m_token);
    if (MPM_RESULT_OK != result)
    {
        OIC_LOG_V(ERROR, LOG_TAG, "getAccessToken failed with %d", result);
        // TODO - what to do in case of failure? free resources? reset auth flag?
        g_isAuthorized = false;
        return MPM_RESULT_INTERNAL_ERROR;
    }
    else
    {
        OIC_LOG(DEBUG, LOG_TAG, "getAccessToken is successful");
        g_isAuthorized = true;
        g_honeywell.setAccessToken(m_token);
    }

    OIC_LOG_V(INFO, LOG_TAG, "Plugin create return value: %d.", result);

    return result;
}

MPMResult pluginStart(MPMPluginCtx *pluginSpecificCtx)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    int error = -1;

    if (pluginSpecificCtx != NULL)
    {
        // set global plugin context
        g_pluginCtx = pluginSpecificCtx;

        /* create house keeping thread */
        error = pthread_create(&(pluginSpecificCtx->thread_handle), NULL, accessTokenMonitorThread, pluginSpecificCtx);
        if (error == 0)
        {
            pluginSpecificCtx->stay_in_process_loop = true;
            pluginSpecificCtx->started = true;
            result = MPM_RESULT_OK;
        }
        else
        {
            pluginSpecificCtx->stay_in_process_loop = false;
            pluginSpecificCtx->started = false;
            OIC_LOG_V(ERROR, LOG_TAG, "Can't create plugin specific thread :[%s]", strerror(error));
            result = MPM_RESULT_STARTED_FAILED;
        }
    }

    OIC_LOG_V(INFO, LOG_TAG, "Plugin start return value: %d.", result);

    return result;
}

MPMResult pluginStop(MPMPluginCtx *pluginSpecificCtx)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    if (NULL != pluginSpecificCtx)
    {
        result = MPM_RESULT_OK;

        if (pluginSpecificCtx->started == true)
        {
            pluginSpecificCtx->stay_in_process_loop = false;
            pthread_join(pluginSpecificCtx->thread_handle, NULL);
            pluginSpecificCtx->started = false;
        }
    }

    OIC_LOG_V(INFO, LOG_TAG, "Plugin stop's return value:%d", result);

    return result;
}

MPMResult pluginDestroy(MPMPluginCtx *pluginSpecificCtx)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    if (pluginSpecificCtx != NULL)
    {

        if (pluginSpecificCtx->started == true)
        {
            pluginStop(pluginSpecificCtx);
        }

        // freeing the resource allocated in create
        OICFree(pluginSpecificCtx);
        pluginSpecificCtx = NULL;
        result = MPM_RESULT_OK;
    }

    OIC_LOG_V(INFO, LOG_TAG, "Plugin destroy's return value:%d", result);

    return result;
}

/**
 * This method checks if the request is for an actuator(oic.if.a) resource or a
 * sensor(oic.if.s) resource. Sensors allow only GET. Actuators allow GET PUT
 * POST. Sensors resources end with /current. Actuators have uris ending with
 * /heater or /cooler.
 *
 * @param[in] uri           Resource Uri
 * @param[in] operation     Operation to be checked.
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

/**
 * This method creates payload for response to GET/PUT/POST request.
 *
 * @param[in] uri           Resource Uri
 * @param[in] data          Thermostat detials to be sent in response.
 */
OCRepPayload *getPayload(const std::string uri, const THERMOSTAT &data)
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
          computeTargetTemp(data.heatSetpointF, data.coolSetpointF));

    if (result)
    {
        result = OCRepPayloadSetUri(payload, uri.c_str());
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
        double temperature = computeTargetTemp(data.heatSetpointF, data.coolSetpointF);
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
resourceEntityHandlerCb(OCEntityHandlerFlag, OCEntityHandlerRequest *request, void *)
{
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
                result = processPutRequest((OCRepPayload*) request->payload, targetThermostat, resourceUri);
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
        OCRepPayload *payload = getPayload(uri.c_str(), data);

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


OCEntityHandlerResult processPutRequest(OCRepPayload * payload, LyricThermostatSharedPtr targetThermostat, const std::string uri)
{
    OCEntityHandlerResult ehResult = OC_EH_OK;
    OIC_LOG_V(INFO, LOG_TAG, "Put request for %s", uri.c_str());
    THERMOSTAT localData;
    int result = MPM_RESULT_OK;
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
    computeSetpoints(localData.targetTempF, localData.heatSetpointF, localData.coolSetpointF);

    OIC_LOG_V(INFO, LOG_TAG, "localData.heatSetpointF %f", localData.heatSetpointF);
    OIC_LOG_V(INFO, LOG_TAG, "localData.coolSetpointF %f", localData.coolSetpointF);

    result = g_honeywell.setTemperature(targetThermostat, localData, uri);

    OIC_LOG_V(INFO, LOG_TAG, "setTemperature returned result = %d", result);
    if (result != MPM_RESULT_OK)
    {
        throw "Error setting temperature for PUT request";
    }

    return ehResult;
}

MPMResult pluginScan(MPMPluginCtx *, MPMPipeMessage *)
{
    OIC_LOG(INFO, LOG_TAG, "Inside plugin_scan");
    std::vector<LyricThermostatSharedPtr> thermostatsScanned;

    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    result = (MPMResult) g_honeywell.getThermostats(thermostatsScanned);
    if (MPM_RESULT_OK == result)
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

            MPMSendResponse(uri.c_str(), uri.size(), MPM_SCAN);
        }
    }
    else
    {
        OIC_LOG_V(ERROR, LOG_TAG, "getThermostats failed with %d", result);
    }
    OIC_LOG(INFO, LOG_TAG, "Leaving plugin specific thread handler.");
    return result;
}

bool isSecureEnvironmentSet()
{
    char *non_secure_env = getenv("NONSECURE");

    if (non_secure_env != NULL && (strcmp(non_secure_env, "true") == 0))
    {
        OIC_LOG(INFO, LOG_TAG, "Creating NON SECURE resources");
        return false;
    }
    OIC_LOG(INFO, LOG_TAG, "Creating SECURE resources");
    return true;
}

void createPayloadForMetadata(MPMResourceList **list , const std::string &uri, const std::string &interface)
{
    MPMResourceList *tempPtr;
    tempPtr = (MPMResourceList *) OICCalloc(1, sizeof(MPMResourceList));

    if (tempPtr == NULL)
    {
        OIC_LOG(ERROR, LOG_TAG, "Memory Allocation failed");
        return;
    }
    OICStrcpy(tempPtr->rt, MPM_MAX_LENGTH_64, HONEYWELL_THERMOSTAT_RT);
    OICStrcpy(tempPtr->href, MPM_MAX_URI_LEN, uri.c_str());
    OICStrcpy(tempPtr->interfaces, MPM_MAX_LENGTH_64, interface.c_str());
    tempPtr->bitmap = BM;
    tempPtr->next = *list;
    *list  = tempPtr;
}

void updatePluginSpecificData(THERMOSTAT thermostat, ThermostatDetails *thermostatDetails)
{
    OICStrcpy(thermostatDetails->deviceIdStr, MAX_DEVICE_ID_LEN, thermostat.devInfo.deviceIdStr.c_str());
    OICStrcpy(thermostatDetails->uniqueId, MPM_MAX_UNIQUE_ID_LEN, thermostat.devInfo.uniqueId.c_str());
    thermostatDetails->locationId = thermostat.devInfo.locationId;
    thermostatDetails->ambientTempF = thermostat.ambientTempF;
}

MPMResult pluginAdd(MPMPluginCtx *, MPMPipeMessage * message)
{
    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);

    if (isSecureEnvironmentSet())
    {
        resourceProperties |= OC_SECURE;
    }

    std::string uri = reinterpret_cast<const char *>(message->payload);
    if(addedThermostats.find(uri) != addedThermostats.end())
    {
        OIC_LOG_V(ERROR, LOG_TAG, "%s already added", uri.c_str());
        return MPM_RESULT_ALREADY_CREATED;
    }
    if(uriToLyricThermostatMap.find(uri) == uriToLyricThermostatMap.end())
    {
        OIC_LOG_V(ERROR, LOG_TAG, "%s was NOT discovered in a scan", uri.c_str());
        return MPM_RESULT_INTERNAL_ERROR;
    }

    MPMResourceList *list = NULL;

    /* Heater Resource */
    std::string honeywellDeviceActuatorHeaterUri = uri + "/heater";
    ConcurrentIotivityUtils::queueCreateResource(honeywellDeviceActuatorHeaterUri, HONEYWELL_THERMOSTAT_RT, HONEYWELL_THERMOSTAT_ACTUATOR_IF, resourceEntityHandlerCb, NULL, resourceProperties);
    createPayloadForMetadata(&list, honeywellDeviceActuatorHeaterUri.c_str(), HONEYWELL_THERMOSTAT_ACTUATOR_IF);

    /* Cooler Resource */
    std::string honeywellDeviceActuatorCoolerUri = uri + "/cooler";
    ConcurrentIotivityUtils::queueCreateResource(honeywellDeviceActuatorCoolerUri, HONEYWELL_THERMOSTAT_RT, HONEYWELL_THERMOSTAT_ACTUATOR_IF, resourceEntityHandlerCb, NULL, resourceProperties);
    createPayloadForMetadata(&list, honeywellDeviceActuatorCoolerUri.c_str(), HONEYWELL_THERMOSTAT_ACTUATOR_IF);

    /* Sensor Resource */
    std::string honeywellDeviceSensorUri = uri + "/current";
    ConcurrentIotivityUtils::queueCreateResource(honeywellDeviceSensorUri, HONEYWELL_THERMOSTAT_RT, HONEYWELL_THERMOSTAT_SENSOR_IF, resourceEntityHandlerCb, NULL, resourceProperties);
    createPayloadForMetadata(&list, honeywellDeviceSensorUri.c_str(), HONEYWELL_THERMOSTAT_SENSOR_IF);

    addedThermostats[uri] = uriToLyricThermostatMap[uri];

    uint8_t * buff = (uint8_t *) OICCalloc(1, MPM_MAX_METADATA_LEN);
    ThermostatDetails thermostatDetails;
    MPMDeviceSpecificData deviceConfiguration;
    THERMOSTAT thermostat;
    std::string changeableValues;

    addedThermostats[uri]->get(thermostat);
    changeableValues = addedThermostats[uri]->getChangeableValues();
    memset(&thermostatDetails, 0, sizeof(ThermostatDetails));
    memset(&deviceConfiguration, 0, sizeof(MPMDeviceSpecificData));

    OICStrcpy(thermostatDetails.changeableValues, MAX_CHANGEABLEVALUES_LEN, changeableValues.c_str());
    updatePluginSpecificData(thermostat, &thermostatDetails);
    OICStrcpy(deviceConfiguration.devName, MPM_MAX_LENGTH_64, DEVICE_NAME);
    OICStrcpy(deviceConfiguration.devType, MPM_MAX_LENGTH_64, DEVICE_TYPE);
    OICStrcpy(deviceConfiguration.manufacturerName, MPM_MAX_LENGTH_256, MANUFACTURER_NAME);
    MPMFormMetaData(list, &deviceConfiguration, buff, MPM_MAX_METADATA_LEN, (void *)&thermostatDetails, sizeof(ThermostatDetails));

    MPMAddResponse addResponse;
    memset(&addResponse, 0, sizeof(MPMAddResponse));
    OICStrcpy(addResponse.uri, MPM_MAX_URI_LEN, uri.c_str());
    memcpy(addResponse.metadata, buff, MPM_MAX_METADATA_LEN);
    MPMSendResponse(&addResponse, sizeof(MPMAddResponse), MPM_ADD);
    OICFree(buff);
    return MPM_RESULT_OK;

}

MPMResult pluginRemove(MPMPluginCtx *, MPMPipeMessage * message)
{
    std::string uri = reinterpret_cast<const char *>(message->payload);

    /* Heater Resource */
    std::string honeywellDeviceActuatorHeaterUri = uri + "/heater";
    ConcurrentIotivityUtils::queueDeleteResource(honeywellDeviceActuatorHeaterUri);

    /* Cooler Resource */
    std::string honeywellDeviceActuatorCoolerUri = uri + "/cooler";
    ConcurrentIotivityUtils::queueDeleteResource(honeywellDeviceActuatorCoolerUri);

    /* Sensor Resource */
    std::string honeywellDeviceSensorUri = uri + "/current";
    ConcurrentIotivityUtils::queueDeleteResource(honeywellDeviceSensorUri);

    addedThermostats.erase(uri);
    uriToLyricThermostatMap.erase(uri);

    MPMSendResponse(uri.c_str(), uri.size(), MPM_REMOVE);

    return MPM_RESULT_OK;
}

MPMResult pluginReconnect(MPMPluginCtx *, MPMPipeMessage * message)
{
    MPMResourceList *list = NULL, *temp = NULL;
    THERMOSTAT thermostat;
    std::vector<LyricThermostatSharedPtr> thermostatsReconnected;
    void *details = NULL;
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    std::shared_ptr<HoneywellThermostat> sharedThermostat;
    std::string thermostatMode;
    std::string uri;

    if(message->payloadSize <= 0 && message->payload == NULL)
    {
        OIC_LOG(ERROR, LOG_TAG, "No payload received, failed to reconnect");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    MPMParseMetaData(message->payload, MPM_MAX_METADATA_LEN, &list, &details);

    ThermostatDetails *thermostatDetails = (ThermostatDetails *)details;
    HoneywellThermostat honeywellThermostat;

    thermostat.devInfo.locationId = thermostatDetails->locationId;
    thermostat.devInfo.deviceIdStr.assign(thermostatDetails->deviceIdStr);
    thermostat.devInfo.uniqueId.assign(thermostatDetails->uniqueId);
    thermostat.ambientTempF = thermostatDetails->ambientTempF;
    honeywellThermostat.setDeviceUniqueId(thermostat.devInfo.uniqueId.c_str());

    honeywellThermostat.setChangeableValues(thermostatDetails->changeableValues);

    rapidjson:: Document values;
    values.SetObject();
    if (values.Parse(thermostatDetails->changeableValues).HasParseError())
    {
        OIC_LOG(ERROR, LOG_TAG,"Parse error in set changeableValues");
        result = MPM_RESULT_JSON_ERROR;
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

    thermostat.targetTempF = computeTargetTemp(thermostat.heatSetpointF, thermostat.coolSetpointF);
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
        result = MPM_RESULT_ALREADY_CREATED;
        goto CLEANUP;
    }
    if(uriToLyricThermostatMap.find(uri) == uriToLyricThermostatMap.end())
    {
        result = MPM_RESULT_INTERNAL_ERROR;
        goto CLEANUP;
    }

    if (isSecureEnvironmentSet())
    {
        resourceProperties |= OC_SECURE;
    }

    while(list)
    {
        temp = list;
        std::string resourceUri(list->href);
        OIC_LOG_V(INFO, LOG_TAG, "resource uri = %s", resourceUri.c_str());
        ConcurrentIotivityUtils::queueCreateResource(resourceUri, list->rt, list->interfaces, resourceEntityHandlerCb, NULL, resourceProperties);
        list = list->next;
        OICFree(temp);
    }

    addedThermostats[uri] = uriToLyricThermostatMap[uri];
    result = MPM_RESULT_OK;

    CLEANUP:
    if (thermostatDetails !=NULL)
    {
        OICFree(thermostatDetails);
        details = NULL;
    }
    return result;
}

/**
 * The Lyric Access Token process loop.
 * - manages honeywell device resources in response to changes in
 *   authentication status
 * - performs re-authentication with Honeywell servers to keep a fresh
 *   access token (Lyric access tokens only last for 10 minutes)
 *
 * @param[in] pointer     plugin specific context
 */
void *accessTokenMonitorThread(void *pointer)
{
    OIC_LOG(DEBUG, LOG_TAG, "Entered accessTokenMonitorThread");
    MPMPluginCtx *ctx = (MPMPluginCtx *) pointer;
    int reauthCountdown = HW_QUERY_INTERVAL_SECONDS; // period to wait before attempting re-auth
    bool lastAuthorized = false;
    std::string emptycode;
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

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
                result = (MPMResult) g_honeywell.getAccessToken(emptycode, m_token);
                if (MPM_RESULT_OK != result)
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
            sleep(MPM_THREAD_PROCESS_SLEEPTIME);
        }
        OIC_LOG(INFO, LOG_TAG,"Leaving LYRIC monitor thread");
    }
    pthread_exit(NULL);
}

