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
#include <assert.h>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include "pluginServer.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "nest.h"
#include <cstdlib>
#include <iostream>
#include <time.h>
#include "experimental/logger.h"
#include "ConcurrentIotivityUtils.h"
#include "messageHandler.h"
#include "ocpayload.h"
#include "mpmErrorCode.h"

using namespace std;

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

using namespace OC::Bridging;

typedef struct
{
    uint16_t humidity;
    uint32_t temperature;
    uint16_t targetTempF;
    uint32_t hvacMode;
    char accessToken[NEST_ACCESS_TOKEN_LEN];
    char deviceId[MPM_MAX_LENGTH_64];
} MPMPluginSpecificData;

static const std::string NEST_THERMOSTAT_IF = "oic.if.a";
static const std::string NEST_THERMOSTAT_RT = "oic.r.temperature";

FILE *nestSecurityFile(const char *path, const char *mode)
{
    std::string filename = std::string("nest_") + path;
    return fopen(filename.c_str(), mode);
}

/*******************************************************************************
 * global data goes here
 ******************************************************************************/
MPMPluginCtx *g_ctx = NULL;
Nest *g_nest = NULL;
NestThermostat::devices g_devices;
std::string nest_client_id;
std::string nest_client_secret;

std::map<std::string, NestThermostatSharedPtr> uriToNestThermostatMap;
std::map<std::string, NestThermostatSharedPtr> addedThermostats;

uint16_t getTemperatureAndUpdateMap(NestThermostatSharedPtr t);
OCEntityHandlerResult resourceEntityHandlerCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void *callbackParam);

MPMResult loadNestAuthConfig(std::string filename, std::string &pincode,
                             std::string &accessToken)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    ifstream tokenFile(filename);

    if (!tokenFile.is_open())
    {
        OIC_LOG_V(ERROR, TAG, "Could not open %s.\n", filename.c_str());
        return result;
    }
    else
    {
        if (!getline(tokenFile, pincode))
        {
            OIC_LOG(ERROR, TAG, "Could not read pincode");
            goto CLEANUP;
        }
        if (!getline(tokenFile, accessToken))
        {
            OIC_LOG(ERROR, TAG, "Could not read access token");
            goto CLEANUP;
        }
        if (!getline(tokenFile, nest_client_id))
        {
            OIC_LOG(ERROR, TAG, "Could not read client id");
            goto CLEANUP;
        }
        if (!getline(tokenFile, nest_client_secret))
        {
            OIC_LOG(ERROR, TAG, "Could not read client secret");
            goto CLEANUP;
        }

        result = MPM_RESULT_OK;
    }

CLEANUP:
    tokenFile.close();
    return result;
}

Nest::ACCESS_TOKEN populateAccessTokenFromFile(std::string accessToken)
{
    Nest::ACCESS_TOKEN aTok(accessToken);

    return aTok;
}

MPMResult checkValidityOfExistingToken(Nest::ACCESS_TOKEN aTok)
{
    MPMResult result = MPM_RESULT_OK;

    g_nest->setAccessToken(aTok);

    //verify the token by sending the token to nest cloud
    if (g_nest->isAuthorized())
    {
        OIC_LOG(INFO, TAG, "Successfully authorized");
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Could not authorize, access token is invalid/expired");
        result = MPM_RESULT_INTERNAL_ERROR;
    }

    return result;
}

void updateNestTokenFile(std::string filename, std::string pincode, std::string accessToken)
{
    ofstream tokenFile;
    tokenFile.open(filename.c_str());
    if (tokenFile.is_open())
    {
        tokenFile << pincode << std::endl;
        tokenFile << accessToken << std::endl;
        tokenFile << nest_client_id << std::endl;
        tokenFile << nest_client_secret << std::endl;
        tokenFile.close();
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Failed to open nest token file");
    }
}

MPMResult refreshAccessToken(std::string filename, std::string pincode)
{
    MPMResult result = MPM_RESULT_OK;

    Nest::ACCESS_TOKEN aToken;
    if (g_nest->getAccessToken(pincode, aToken, nest_client_id, nest_client_secret) != MPM_RESULT_OK)
    {
        OIC_LOG(ERROR, TAG, "get token failed");
        return MPM_RESULT_INTERNAL_ERROR;
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
        result = MPM_RESULT_INTERNAL_ERROR;
    }

    return result;
}

/**
 * This is a plugin's specific entry point function that allows the plugin
 * to create and initialize static structures.
 *
 * @param[out] pluginSpecificCtx            the plugin specific context populated by
 *                                          the plugin.
 *
 * @return MPM_RESULT_OK if no errors, MPM_RESULT_INTERNAL_ERROR if stack process error
 */
MPMResult pluginCreate(MPMPluginCtx **pluginSpecificCtx)
{
    MPMResult result = MPM_RESULT_OK;

    if (g_ctx)
    {
        OIC_LOG(ERROR, TAG, "Plugin is already created.");
        return MPM_RESULT_ALREADY_CREATED;
    }

    /* allocate a context structure for the plugin */
    MPMPluginCtx *ctx = (MPMPluginCtx *) OICCalloc(1, sizeof(MPMPluginCtx));

    /* initialize the plugin context */
    if (!ctx)
    {
        OIC_LOG(ERROR, TAG, "Unable to allocate plugin specific context.");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    *pluginSpecificCtx = ctx;
    g_ctx = ctx;

    ctx->device_name = DEVICE_NAME;
    ctx->resource_type = DEVICE_TYPE;
    ctx->open = nestSecurityFile;

    std::string pincode = "";
    std::string accessToken = "";

    std::string filename = "nest.cnf";

    if (loadNestAuthConfig(filename, pincode, accessToken) != MPM_RESULT_OK)
    {
        OIC_LOG(ERROR, TAG, "Unable to load nest.cnf");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    g_nest = new Nest();
    Nest::ACCESS_TOKEN aTok = populateAccessTokenFromFile(accessToken);

    result = checkValidityOfExistingToken(aTok);

    if (MPM_RESULT_OK != result)
    {
        OIC_LOG(ERROR, TAG, "Nest object could not be created, requesting new access token");
        result = refreshAccessToken(filename, pincode);
    }

    if (MPM_RESULT_OK != result)
    {
        delete (g_nest);
        g_nest = NULL;
    }

    OIC_LOG_V(INFO, TAG, "Plugin create return value:%d.", result);

    /*
     * NOTE: What do we do if the create for some reason failed.  To we assume that the destroy
     * will be called if the create fails??  Let let the plugin loader pick up the pieces by
     * calling destroy on an imperfectly created plugin.  Calling entry point APIs from within
     * the implementation can cause some real problems (e.g. deadlock situations).
     */

    return result;
}

/**
 * Invoked by the plugin's OCF server to allow the plugin to use IoTivity APIs
 *
 * @param[in] pluginSpecificCtx            The plugin specific context created during plugin
 *                                         create
 *
 * @return MPM_RESULT_OK if no errors, MPM_RESULT_INTERNAL_ERROR if stack process error
 */
MPMResult pluginStart(MPMPluginCtx *pluginSpecificCtx)
{
    MPMResult result = MPM_RESULT_OK;
    if (!pluginSpecificCtx)
    {
        result = MPM_RESULT_INTERNAL_ERROR;
    }

    return result;
}

bool isSecureEnvironmentSet()
{
    char *non_secure_env = getenv("NONSECURE");

    if (non_secure_env && !strcmp(non_secure_env, "true"))
    {
        OIC_LOG(INFO, TAG, "Creating NON SECURE resources");
        return false;
    }
    OIC_LOG(INFO, TAG, "Creating SECURE resources");
    return true;
}

/**
 * For initiating a scan for thermostats
 *
 * @return MPM_RESULT_OK if no error, specific error code defined in mpmErrorCode.h
 * upon error
 */
MPMResult pluginScan(MPMPluginCtx *, MPMPipeMessage *)
{
    MPMResult nestResult = MPM_RESULT_OK;

    std::vector<NestThermostatSharedPtr> thermostatScanned;

    nestResult = g_nest->getThermostats(thermostatScanned);
    if (MPM_RESULT_OK == nestResult)
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

                if (addedThermostats.find(uri) != addedThermostats.end())
                {
                    OIC_LOG_V(INFO, TAG, "Already added %s. Ignoring", uri.c_str());
                    continue;
                }

                uriToNestThermostatMap[uri] = thermostat;

                MPMSendResponse(uri.c_str(), uri.size(), MPM_SCAN);
            }
        }
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "getThermostats returned error %i", nestResult);
    }

    return nestResult;

}

/**
 * Creates 1 OCF resource for a NestThermostat for temperature control.
 *
 * @param[in] uri            Base uri. Resource uri is the same as base uri
 *
 * @return MPM_RESULT_OK
 */
MPMResult createOCFResource(const std::string &uri)
{
    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    if (isSecureEnvironmentSet())
    {
        resourceProperties |= OC_SECURE;
    }

    ConcurrentIotivityUtils::queueCreateResource(uri, NEST_THERMOSTAT_RT, NEST_THERMOSTAT_IF,
            resourceEntityHandlerCb,
            NULL, resourceProperties);

    return MPM_RESULT_OK;
}

MPMResult  deleteOCFResource(const std::string &uri)
{
    ConcurrentIotivityUtils::queueDeleteResource(uri);

    return MPM_RESULT_OK;
}

void createPayloadForMetadata(MPMResourceList **list, const std::string &uri)
{
    MPMResourceList *tempPtr = NULL;
    tempPtr = (MPMResourceList *) OICCalloc(1, sizeof(MPMResourceList));
    if (!tempPtr)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed");
        return;
    }

    OICStrcpy(tempPtr->rt, MPM_MAX_LENGTH_64, NEST_THERMOSTAT_RT.c_str());
    OICStrcpy(tempPtr->href, MPM_MAX_URI_LEN, uri.c_str());
    OICStrcpy(tempPtr->interfaces, MPM_MAX_LENGTH_64, NEST_THERMOSTAT_IF.c_str());
    tempPtr->bitmap = BM;
    tempPtr->next = *list;
    *list  = tempPtr;
}

void updateMPMPluginSpecificData(NestThermostat::THERMOSTAT thermostat,
                                 MPMPluginSpecificData *pluginDetails)
{
    pluginDetails->humidity = thermostat.humidity;
    pluginDetails->hvacMode = thermostat.hvacMode;
    pluginDetails->targetTempF = thermostat.targetTempF;
    pluginDetails->temperature = thermostat.temperature;
    memcpy(pluginDetails->accessToken, g_nest->getTok().c_str(), g_nest->getTok().length());
    memcpy(pluginDetails->deviceId, thermostat.devInfo.id.c_str(), thermostat.devInfo.id.length());
}

/**
 * For adding a new thermostat
 *
 * @param[in] message            The add request message coming from client. It contains
 *                               the uri of the device to be added
 *
 * @return MPM_RESULT_OK if no error, specific error code defined in mpmErrorCode.h
 * upon error
 */
MPMResult pluginAdd(MPMPluginCtx *, MPMPipeMessage *message)
{
    if (message->payloadSize <= 0)
    {
        OIC_LOG(ERROR, TAG, "add payload is null");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    MPMResourceList *list = NULL;
    uint8_t *buff = NULL;

    MPMDeviceSpecificData deviceConfiguration;
    MPMPluginSpecificData pluginSpecificDetails;
    memset(&deviceConfiguration, 0, sizeof(MPMDeviceSpecificData));
    memset(&pluginSpecificDetails, 0, sizeof(MPMPluginSpecificData));

    std::string uri = reinterpret_cast<const char *>(message->payload);

    if (addedThermostats.find(uri) != addedThermostats.end())
    {
        OIC_LOG_V(ERROR, TAG, "%s already added", uri.c_str());
        return MPM_RESULT_ALREADY_CREATED;
    }
    if (uriToNestThermostatMap.find(uri) == uriToNestThermostatMap.end())
    {
        OIC_LOG_V(ERROR, TAG, "%s was NOT discovered in a scan", uri.c_str());
        return MPM_RESULT_INTERNAL_ERROR;
    }

    createOCFResource(uri);
    addedThermostats[uri] = uriToNestThermostatMap[uri];

    createPayloadForMetadata(&list, uri);

    NestThermostat::THERMOSTAT thermostat;
    addedThermostats[uri]->get(thermostat);
    updateMPMPluginSpecificData(thermostat, &pluginSpecificDetails);

    buff = (uint8_t *)OICCalloc(1, MPM_MAX_METADATA_LEN);
    if (buff == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to allocate memory for reconnect buffer");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    OICStrcpy(deviceConfiguration.devName, MPM_MAX_LENGTH_64, DEVICE_NAME);
    OICStrcpy(deviceConfiguration.devType, MPM_MAX_LENGTH_64, DEVICE_TYPE);
    OICStrcpy(deviceConfiguration.manufacturerName, MPM_MAX_LENGTH_256, MANUFACTURER_NAME);
    MPMFormMetaData(list, &deviceConfiguration, buff, MPM_MAX_METADATA_LEN,
                    (void *)&pluginSpecificDetails, sizeof(MPMPluginSpecificData));

    MPMAddResponse response;
    memset(&response, 0, sizeof(MPMAddResponse));
    OICStrcpy(response.uri, MPM_MAX_URI_LEN, uri.c_str());
    memcpy(response.metadata, buff, MPM_MAX_METADATA_LEN);

    MPMSendResponse(&response, sizeof(MPMAddResponse), MPM_ADD);

    OICFree(buff);
    return MPM_RESULT_OK;
}

/**
 * For removing a thermostat
 *
 * @param[in] message            The remove request message coming from client. It contains
 *                               the uri of the device to be removed
 *
 * @return MPM_RESULT_OK if no error, MPM_RESULT_INTERNAL_ERROR if error
 */
MPMResult pluginRemove(MPMPluginCtx *, MPMPipeMessage *message)
{
    if (message->payloadSize > 0 && message->payload != NULL)
    {
        std::string uri = reinterpret_cast<const char *>(message->payload);
        OIC_LOG_V(DEBUG, TAG, "device uri to be removed - %s ", uri.c_str());

        if (addedThermostats.find(uri) == addedThermostats.end())
        {
            OIC_LOG(ERROR, TAG, "Device to be removed is not added yet");
            return MPM_RESULT_NOT_PRESENT;
        }

        deleteOCFResource(uri);
        addedThermostats.erase(uri);
        uriToNestThermostatMap.erase(uri);

        MPMSendResponse(uri.c_str(), uri.size(), MPM_REMOVE);
        return MPM_RESULT_OK;
    }

    return MPM_RESULT_INTERNAL_ERROR;
}

/**
 * For reconnecting a thermostat after a system reboot. This method is invoked once
 * for each device reconnect request for which is trigerred by the client.
 *
 * @param[in] message            The reconnect request message coming from client. It contains
 *                               the metadata for the device to be reconnected
 *
 * @return MPM_RESULT_OK if no error, MPM_RESULT_INTERNAL_ERROR if error
 */
MPMResult pluginReconnect(MPMPluginCtx *, MPMPipeMessage *message)
{
    MPMResourceList *list = NULL, *temp = NULL;
    void *pluginSpecificDetails = NULL;
    std::vector<NestThermostatSharedPtr> thermostatsReconnected;

    if (message->payloadSize <= 0 && message->payload == NULL)
    {
        OIC_LOG(ERROR, TAG, "No paylaod received, failed to reconnect");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    MPMParseMetaData(message->payload, MPM_MAX_METADATA_LEN, &list, &pluginSpecificDetails);

    MPMPluginSpecificData *pluginDetails = (MPMPluginSpecificData *)pluginSpecificDetails;

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

    if (uriToNestThermostatMap.find(uri) != uriToNestThermostatMap.end())
    {
        OIC_LOG_V(INFO, TAG, "Already found %s. Ignoring", uri.c_str());
    }
    else
    {
        uriToNestThermostatMap[uri] = sharedThermostat;
    }

    if (addedThermostats.find(uri) != addedThermostats.end())
    {
        OIC_LOG_V(ERROR, TAG, "%s already added", uri.c_str());
        return MPM_RESULT_ALREADY_CREATED;
    }
    if (uriToNestThermostatMap.find(uri) == uriToNestThermostatMap.end())
    {
        return MPM_RESULT_INTERNAL_ERROR;
    }

    while (list)
    {
        temp = list;
        OIC_LOG_V(INFO, TAG, "resource uri = %s", list->href);
        createOCFResource(list->href);
        list = list->next;
        OICFree(temp);
    }

    addedThermostats[uri] = uriToNestThermostatMap[uri];
    getTemperatureAndUpdateMap(addedThermostats[uri]);
    free(pluginDetails);
    return MPM_RESULT_OK;
}

/**
 * Plugin specific entry-point function to stop the plugin's threads
 *
 * @param[in] pluginSpecificCtx            The plugin specific context created during plugin
 *                                         create
 * @return MPM_RESULT_OK if no errors, MPM_RESULT_INTERNAL_ERROR if stack process error
 */
MPMResult pluginStop(MPMPluginCtx *pluginSpecificCtx)
{
    MPMPluginCtx *ctx = pluginSpecificCtx;

    if (ctx)
    {
        assert(g_ctx);
        addedThermostats.clear();
        uriToNestThermostatMap.clear();
    }

    return MPM_RESULT_OK;
}

/**
 * Plugin specific entry-point function to allow the plugin resources to be
 * freed.
 *
 * @param[in] pluginSpecificCtx            The plugin specific context created during plugin
 *                                         create
 * @return MPM_RESULT_OK if no errors, MPM_RESULT_INTERNAL_ERROR if stack process error
 */
MPMResult pluginDestroy(MPMPluginCtx *pluginSpecificCtx)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    MPMPluginCtx *ctx = pluginSpecificCtx;
    if (ctx)
    {
        assert(g_ctx);
        if(g_nest)
        {
            delete (g_nest);
            g_nest = NULL;
        }
        OICFree(ctx);
        g_ctx = NULL;
        result = MPM_RESULT_OK;
    }

    OIC_LOG_V(INFO, TAG, "Plugin destroy's return value:%d", result);

    return result;
}

NestThermostatSharedPtr getNestThermostatFromOCFResourceUri(std::string resourceUri)
{
    OIC_LOG_V(INFO, TAG, "Request for %s", resourceUri.c_str());

    for (auto uriToNestPair : addedThermostats)
    {
        if (resourceUri.find(uriToNestPair.first) != std::string::npos)
        {
            return uriToNestPair.second;
        }
    }
    throw std::runtime_error("Resource" + resourceUri + " not found");
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

    for (unsigned int i = 0; i < thermostatsGot.size(); ++i)
    {
        NestThermostat::THERMOSTAT fetchedThermostat;
        thermostatsGot[i]->get(fetchedThermostat);
        if (fetchedThermostat.devInfo.id == targetThermostat.devInfo.id)
        {
            if (targetThermostat.targetTempF != fetchedThermostat.targetTempF)
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
    std::unique_ptr<OCRepPayload, decltype(OCRepPayloadDestroy) *> payload {OCRepPayloadCreate(),
            OCRepPayloadDestroy };

    if (!payload)
    {
        throw std::runtime_error("payload cannot be NULL");
    }

    if (!OCRepPayloadSetPropDouble(payload.get(), TEMPERATURE_TAG,
                                   (double) getTemperatureAndUpdateMap(t)))
    {
        throw std::runtime_error("failed to set temperature in the payload");
    }

    return addCommonNestProperties(t, payload.release());
}

OCRepPayload *processGetRequest(NestThermostatSharedPtr t)
{
    return getTemperaturePayload(t);
}

OCEntityHandlerResult processTemperatureUpdate(OCRepPayload *payload, NestThermostatSharedPtr t)
{
    double targetTemp = 0.0;
    if (!OCRepPayloadGetPropDouble(payload, TEMPERATURE_TAG, &targetTemp))
    {
        throw std::runtime_error("Payload must contain \"temperature\"");
    }
    MPMResult result = g_nest->setAwayMode(Nest::eAWHome);
    if (result == MPM_RESULT_OK)
    {
        result = t->setTemperature(targetTemp);

        if (result != MPM_RESULT_OK)
        {
            throw std::runtime_error("Error setting temperature for PUT request");
        }
    }
    else
    {
        throw std::runtime_error("Error setting away mode to home mode for PUT request");
    }
    return OC_EH_OK;
}

OCEntityHandlerResult processPutRequest(OCRepPayload *payload, NestThermostatSharedPtr t)
{
    if (!payload)
    {
        throw std::runtime_error("PUT payload cannot be NULL");
    }

    return processTemperatureUpdate(payload, t);
}

OCEntityHandlerResult resourceEntityHandlerCb(OCEntityHandlerFlag,
        OCEntityHandlerRequest *entityHandlerRequest, void *)
{
    OCEntityHandlerResult result = OC_EH_OK;
    MPMResult res = MPM_RESULT_OK;

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

                res = (MPMResult) processPutRequest((OCRepPayload *) entityHandlerRequest->payload,
                                                    targetThermostat);
                if (res != MPM_RESULT_OK)
                    result = OC_EH_ERROR;
                break;

            default:
                OIC_LOG_V(INFO, TAG, "Unsupported method (%d) recieved", entityHandlerRequest->method);
                ConcurrentIotivityUtils::respondToRequestWithError(entityHandlerRequest,
                        "Unsupported method received",
                        OC_EH_METHOD_NOT_ALLOWED);
                return OC_EH_OK;
        }

        OCRepPayload *responsePayload = processGetRequest(targetThermostat);
        ConcurrentIotivityUtils::respondToRequest(entityHandlerRequest, responsePayload, result);
        OCRepPayloadDestroy(responsePayload);
    }
    catch (const std::exception &exp)
    {
        ConcurrentIotivityUtils::respondToRequestWithError(entityHandlerRequest, exp.what(), OC_EH_ERROR);
        result = OC_EH_OK;
    }

    return result;
}
