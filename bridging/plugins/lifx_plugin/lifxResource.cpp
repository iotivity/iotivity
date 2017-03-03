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

#include <stdio.h>
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <cmath>
#include <mutex>
#include "pluginServer.h"
#include "ocpayload.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "lifx.h"
#include "logger.h"
#include "ConcurrentIotivityUtils.h"

MPMPluginCtx *g_pluginCtx = NULL;

#define DEVICE_NAME "LIFX_BULB"
#define DEVICE_TYPE "LIGHT_BULB"
#define MANUFACTURER_NAME "LIFX"
#define BM 3
#define MAX_ACCESS_TOKEN_LENGTH 1024
#define TAG "LIFX_RESOURCE"

static const char* OIC_BINARY_SWITCH = "oic.r.switch.binary";
static const char* OIC_BRIGHTNESS_LIGHT = "oic.r.light.brightness";

const uint BINARY_SWITCH_CALLBACK = 0;
const uint BRIGHTNESS_CALLBACK = 1;

static const std::string BINARY_SWITCH_RELATIVE_URI = "/switch";
static const std::string BRIGHTNESS_RELATIVE_URI = "/brightness";

char accessToken[MAX_ACCESS_TOKEN_LENGTH];
const static char CRED_FILE[] = "./oic_svr_db_lifx.dat";

using namespace OC::Bridging;

typedef struct
{
    char id[MPM_MAX_LENGTH_64];
    char uuid[MPM_MAX_LENGTH_64];
    char label[MPM_MAX_LENGTH_64];
    char user[MPM_MAX_LENGTH_256];
} LightDetails;

std::map<std::string, LifxLightSharedPtr> uriToLifxLightMap;
std::map<std::string, LifxLightSharedPtr> addedLights;
std::mutex addedLightsLock;

// Forward declarations.
static void *lightMonitorThread(void *pointer);
OCEntityHandlerResult resourceEntityHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *request, void *callbackParam);

static LifxLightSharedPtr getLifXLightFromOCFResourceUri(std::string resourceUri);

FILE *lifxSecurityFile(const char *, const char *mode)
{
    return fopen(CRED_FILE, mode);
}

MPMResult pluginCreate(MPMPluginCtx **pluginSpecificCtx)
{
    if (g_pluginCtx != NULL)
    {
        OIC_LOG(ERROR, TAG, "Plugin is already created.");
        return MPM_RESULT_ALREADY_CREATED;
    }

    MPMPluginCtx *ctx = (MPMPluginCtx *) OICCalloc(1, sizeof(MPMPluginCtx));

    if (ctx == NULL)
    {
        OIC_LOG(ERROR, TAG, "Allocation of plugin context failed");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    *pluginSpecificCtx = ctx;
    g_pluginCtx = ctx;

    ctx->device_name = "Lifx Translator";
    ctx->resource_type = "oic.d.light";
    ctx->open = lifxSecurityFile;


    FILE *fp = fopen("./lifx.cnf", "r");

    if (NULL == fp)
    {
        OIC_LOG(ERROR, TAG, "error loading lifx.cnf file.");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    if (fgets(accessToken, MAX_ACCESS_TOKEN_LENGTH - 1, fp) == NULL)
    {
        OIC_LOG(ERROR, TAG, "Failed to read ./lifx.cnf");
        fclose(fp);
        return MPM_RESULT_INTERNAL_ERROR;
    }
    accessToken[strlen(accessToken)-1] = '\0';
    fclose(fp);

    return MPM_RESULT_OK;
}

MPMResult pluginStart(MPMPluginCtx *ctx)
{
    MPMResult result = MPM_RESULT_INVALID_PARAMETER;
    int error = 0;

    if (ctx->started)
    {
        OIC_LOG(INFO, TAG, "Plugin is already started.");
        return MPM_RESULT_ALREADY_STARTED;
    }

    ctx->stay_in_process_loop = true;

    //@todo Maybe move this to plugin_add()? once the first light is added?
    error = pthread_create(&(ctx->thread_handle), NULL, lightMonitorThread, ctx);
    if (error == 0)
    {
        ctx->started = true;
        result = MPM_RESULT_OK;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "Can't create plugin specific thread :[%s]", strerror(errno));
        pluginStop(ctx);
        result = MPM_RESULT_STARTED_FAILED;
    }

    OIC_LOG_V(INFO, TAG, "Plugin start return value:%d.", result);
    return (result);
}

MPMResult pluginScan(MPMPluginCtx *, MPMPipeMessage *)
{
    std::vector<LifxLightSharedPtr> lightsScanned;

    MPMResult result = LifxLight::getLights(accessToken, lightsScanned);

    for (uint32_t i = 0; i < lightsScanned.size(); ++i)
    {
        LifxLightSharedPtr light = lightsScanned[i];

        OIC_LOG_V(INFO, TAG, "Found %s bulb %s(%s).", light->state.connected ? "CONNECTED" : "OFFLINE",
                  light->config.id.c_str(), light->config.label.c_str());

        if (!light->state.connected)
        {
            OIC_LOG(INFO, TAG, "Ignoring OFFLINE light");
            continue;
        }

        std::string uri = "/lifx/" + light->config.id;

        if (uriToLifxLightMap.find(uri) != uriToLifxLightMap.end())
        {
            OIC_LOG_V(INFO, TAG, "Already found %s. Ignoring", uri.c_str());
            continue;
        }

        uriToLifxLightMap[uri] = light;

        MPMSendResponse(uri.c_str(), uri.size(), MPM_SCAN);
    }
    if (result != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to fetch lights with error (%d)", result);
        return MPM_RESULT_INTERNAL_ERROR;
    }

    return MPM_RESULT_OK;

}

bool isSecureEnvironmentSet()
{
    char *non_secure_env = getenv("NONSECURE");

    if (non_secure_env != NULL && (strcmp(non_secure_env, "true") == 0))
    {
        OIC_LOG(INFO, TAG, "Creating NON SECURE resources");
        return false;
    }
    OIC_LOG(INFO, TAG, "Creating SECURE resources");
    return true;
}

MPMResult createPayloadForMetadata(MPMResourceList **list, const char *uri, const char *res_type,
                              const char *interface)
{
    MPMResourceList *tempPtr;
    tempPtr = (MPMResourceList *) OICCalloc(1, sizeof(MPMResourceList));
    if (!tempPtr)
    {
        OIC_LOG_V(ERROR, TAG, "failed to allocate memory for tempPtr");
        return MPM_RESULT_OUT_OF_MEMORY;
    }
    strncpy(tempPtr->rt, res_type,  MPM_MAX_LENGTH_64);
    strncpy(tempPtr->href, uri, MPM_MAX_URI_LEN);
    strncpy(tempPtr->interfaces, interface, MPM_MAX_LENGTH_64);
    tempPtr->bitmap = BM;
    tempPtr->next = *list;
    *list  = tempPtr;
    return MPM_RESULT_OK;
}

/***
 * Creates 2 OCF resources for a LifxLight. One for the switch to turn it on and off
 * and one for brightness.
 *
 * @param[in] uri Base uri. Switch and brightness uris are baseUri appended with "/switch" & "/brightness"
 * @return MPM_RESULT_OK
 */
MPMResult createOCFResources(const std::string &uri)
{
    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    if (isSecureEnvironmentSet())
    {
        resourceProperties |= OC_SECURE;
    }

    std::string switchUri = uri + BINARY_SWITCH_RELATIVE_URI;
    ConcurrentIotivityUtils::queueCreateResource(switchUri, OIC_BINARY_SWITCH, OC_RSRVD_INTERFACE_ACTUATOR,
                                            resourceEntityHandler,
                                            (void *) BINARY_SWITCH_CALLBACK, resourceProperties);

    std::string brightnessUri = uri + BRIGHTNESS_RELATIVE_URI;
    ConcurrentIotivityUtils::queueCreateResource(brightnessUri, OIC_BRIGHTNESS_LIGHT, OC_RSRVD_INTERFACE_ACTUATOR,
                                            resourceEntityHandler,
                                            (void *) BRIGHTNESS_CALLBACK, resourceProperties);

    return MPM_RESULT_OK;
}

MPMResult deleteOCFResources(const std::string &uri)
{
    ConcurrentIotivityUtils::queueDeleteResource(uri + BINARY_SWITCH_RELATIVE_URI);
    ConcurrentIotivityUtils::queueDeleteResource(uri + BRIGHTNESS_RELATIVE_URI);
    return MPM_RESULT_OK;
}

MPMResult pluginAdd(MPMPluginCtx *, MPMPipeMessage * message)
{
    if (message->payloadSize <= 0 && message->payload == NULL)
    {
        OIC_LOG(ERROR, TAG, "No payload received, failed to add device");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    uint8_t *buff = NULL;
    MPMResourceList *list = NULL;
    MPMDeviceSpecificData deviceConfiguration;
    LightDetails pluginSpecificDetails;
    std::string user;
    memset(&pluginSpecificDetails, 0, sizeof(LightDetails));
    memset(&deviceConfiguration, 0, sizeof(MPMDeviceSpecificData));

    std::string uri = reinterpret_cast<const char*>(message->payload);

    std::lock_guard<std::mutex> lock(addedLightsLock);
    if (addedLights.find(uri) != addedLights.end())
    {
        OIC_LOG_V(ERROR, TAG, "%s already added", uri.c_str());
        return MPM_RESULT_ALREADY_CREATED;
    }
    if (uriToLifxLightMap.find(uri) == uriToLifxLightMap.end())
    {
        OIC_LOG_V(ERROR, TAG, "%s was NOT discovered in a scan", uri.c_str());
        return MPM_RESULT_INTERNAL_ERROR;
    }

    createOCFResources(uri);

    buff = (uint8_t *)OICCalloc(1, MPM_MAX_METADATA_LEN);
    if (buff == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to allocate memory for reconnect buffer");
        return MPM_RESULT_OUT_OF_MEMORY;
    }

    std::string switchUri = uri + BINARY_SWITCH_RELATIVE_URI;
    result = createPayloadForMetadata(&list, switchUri.c_str(), OIC_BINARY_SWITCH,
             OC_RSRVD_INTERFACE_ACTUATOR);

    std::string brightnessUri = uri + BRIGHTNESS_RELATIVE_URI;
    result = createPayloadForMetadata(&list, brightnessUri.c_str(), OIC_BRIGHTNESS_LIGHT,
             OC_RSRVD_INTERFACE_ACTUATOR);
    if (result == MPM_RESULT_OUT_OF_MEMORY)
    {
        OIC_LOG(ERROR, TAG, "Failed to create payload for metadata");
        return result;
    }

    LifxLightSharedPtr targetLight = uriToLifxLightMap[uri];
    targetLight->getUser(user);

    // filling plugin specific details
    strncpy(pluginSpecificDetails.id, targetLight->config.id.c_str(), MPM_MAX_LENGTH_64);
    strncpy(pluginSpecificDetails.label, targetLight->config.label.c_str(), MPM_MAX_LENGTH_64);
    strncpy(pluginSpecificDetails.uuid, targetLight->config.uuid.c_str(), MPM_MAX_LENGTH_64);
    strncpy(pluginSpecificDetails.user, user.c_str(), MPM_MAX_LENGTH_256);

    // filling device specific details
    strncpy(deviceConfiguration.devName, DEVICE_NAME, MPM_MAX_LENGTH_64);
    strncpy(deviceConfiguration.devType, DEVICE_TYPE, MPM_MAX_LENGTH_64);
    strncpy(deviceConfiguration.manufacturerName, MANUFACTURER_NAME, MPM_MAX_LENGTH_256);

    MPMFormMetaData(list, &deviceConfiguration, buff, MPM_MAX_METADATA_LEN, &pluginSpecificDetails,
                    sizeof(pluginSpecificDetails));

    addedLights[uri] = uriToLifxLightMap[uri];

    MPMAddResponse response;
    strncpy(response.uri, uri.c_str(), MPM_MAX_URI_LEN);
    memcpy(response.metadata, buff, MPM_MAX_METADATA_LEN);

    size_t size = sizeof(MPMAddResponse);

    MPMSendResponse(&response, size, MPM_ADD);

    OICFree(buff);
    return result;
}

MPMResult pluginRemove(MPMPluginCtx *, MPMPipeMessage *message)
{
    if (message->payloadSize <= 0 && message->payload == NULL)
    {
        OIC_LOG(ERROR, TAG, "No paylaod received, failed to remove device");
        return MPM_RESULT_INTERNAL_ERROR;
    }
    std::string uri = reinterpret_cast<const char *>(message->payload);
    OIC_LOG_V(DEBUG, TAG, "device uri to be removed - %s ", uri.c_str());

    std::lock_guard<std::mutex> lock(addedLightsLock);
    if (addedLights.find(uri) == addedLights.end())
    {
        OIC_LOG(ERROR, TAG, "Device to be removed is not added yet");
        return MPM_RESULT_NOT_PRESENT;
    }

    deleteOCFResources(uri);

    addedLights.erase(uri);
    uriToLifxLightMap.erase(uri);

    MPMSendResponse(uri.c_str(), uri.size(), MPM_REMOVE);
    return MPM_RESULT_OK;
}

MPMResult pluginReconnect(MPMPluginCtx *, MPMPipeMessage *message)
{
    MPMResourceList *list = NULL, *temp = NULL;
    void *pluginSpecificDetails = NULL;

    if (message->payloadSize <= 0 && message->payload == NULL)
    {
        OIC_LOG(ERROR, TAG, "No paylaod received, failed to reconnect");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    MPMParseMetaData(message->payload, MPM_MAX_METADATA_LEN, &list, &pluginSpecificDetails);

    LightDetails *lightDetails = (LightDetails *)pluginSpecificDetails;

    OIC_LOG_V(DEBUG, TAG, "\n Reconnect Details \nid - %s\nuuid - %s\nlabel - %s\nuser - %s\n",
              lightDetails->id, lightDetails->uuid, lightDetails->label, lightDetails->user);

    LifxLight::lightState state;
    LifxLight::lightConfig cfg(lightDetails->id, lightDetails->uuid, lightDetails->label);
    std::string uri = "/lifx/" + cfg.id;
    std::shared_ptr<LifxLight> light = std::make_shared<LifxLight>(state, cfg, lightDetails->user);

    createOCFResources(uri);
    uriToLifxLightMap[uri] = light;
    addedLights[uri] = uriToLifxLightMap[uri];

    while (list)
    {
        temp = list;
        list = list->next;
        OICFree(temp);
    }
    free(lightDetails);
    return MPM_RESULT_OK;
}

MPMResult pluginStop(MPMPluginCtx *pluginSpecificCtx)
{
    MPMResult result = MPM_RESULT_OK;

    MPMPluginCtx *ctx = pluginSpecificCtx;

    if (NULL != ctx && g_pluginCtx != NULL)
    {
        addedLights.clear();
        uriToLifxLightMap.clear();

        if (ctx->started)
        {
            ctx->stay_in_process_loop = false;
            pthread_join(ctx->thread_handle, NULL);
            ctx->started = false;
        }
    }

    OIC_LOG_V(INFO, TAG, "Plugin stop's return value:%d", result);
    return (result);

}

MPMResult pluginDestroy(MPMPluginCtx *pluginSpecificCtx)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    MPMPluginCtx *ctx = (MPMPluginCtx *) pluginSpecificCtx;

    if (ctx != NULL && g_pluginCtx != NULL)
    {
        if (ctx->started)
        {
            pluginStop(pluginSpecificCtx);
        }
        // freeing the resource allocated in create
        OICFree(ctx);
        g_pluginCtx = NULL;
        result = MPM_RESULT_OK;
    }

    OIC_LOG_V(INFO, TAG, "Plugin destroy's return value:%d", result);

    return (result);
}

OCRepPayload *addCommonLifXProperties(const LifxLightSharedPtr &l, OCRepPayload *payload)
{
    if (!OCRepPayloadSetPropString(payload, "x.com.intel.label", l->config.label.c_str()))
    {
        throw std::runtime_error("failed to set label");
    }

    if (!OCRepPayloadSetPropDouble(payload, "x.com.intel.secondsSinceLastSeen",
                              l->state.secondsSinceLastSeen))
    {
        throw std::runtime_error("failed to set secondsSinceLastSeen");
    }
    return payload;
}

OCRepPayload *getBinarySwitchPayload(LifxLightSharedPtr l)
{
    std::unique_ptr<OCRepPayload , decltype(OCRepPayloadDestroy) *> payload {OCRepPayloadCreate(),
         OCRepPayloadDestroy };

    if (!payload)
    {
        throw std::runtime_error("payload cannot be NULL");
    }

    if (!OCRepPayloadSetPropBool(payload.get(), "value", l->state.power))
    {
        throw std::runtime_error("failed to set binary switch value in the payload");
    }
    return addCommonLifXProperties(l, payload.release());
}

OCRepPayload *getBrightnessPayload(LifxLightSharedPtr l)
{
    std::unique_ptr<OCRepPayload , decltype(OCRepPayloadDestroy) *> payload {OCRepPayloadCreate(),
             OCRepPayloadDestroy };
    if (!payload)
    {
        throw std::runtime_error("payload cannot be NULL");
    }

    // Convert LifX [0.0, 1.0] to OCF [0, 100].
    if (!OCRepPayloadSetPropInt(payload.get(), "brightness", (int64_t) (l->state.brightness * 100.0)))
    {
         throw std::runtime_error("failed to set brightness");
    }
    return addCommonLifXProperties(l, payload.release());

}

OCRepPayload *processGetRequest(LifxLightSharedPtr l, uintptr_t resourceTypeInCallback)
{
    if (resourceTypeInCallback == BINARY_SWITCH_CALLBACK)
    {
        return getBinarySwitchPayload(l);
    }
    else if (resourceTypeInCallback == BRIGHTNESS_CALLBACK)
    {
        return getBrightnessPayload(l);
    }
    return NULL;
}

OCEntityHandlerResult processBinarySwitchUpdate(OCRepPayload *payload, LifxLightSharedPtr l)
{
    bool power = false;
    if (!OCRepPayloadGetPropBool(payload, "value", &power))
    {
        throw std::runtime_error("Payload must contain \"value\"");
    }

    MPMResult result = l->setPower(power);

    if (result != MPM_RESULT_OK)
    {
        throw std::runtime_error("Error setting power for PUT request");
    }
    return OC_EH_OK;
}

OCEntityHandlerResult processBrightnessUpdate(OCRepPayload *payload, LifxLightSharedPtr l)
{
    int64_t ocfBrightness = 0;
    if (!OCRepPayloadGetPropInt(payload, "brightness", &ocfBrightness))
    {
        throw std::runtime_error("Payload must contain \"brightness\"");
    }
    // OCF brightness is [0, 100] and Lifx is [0.0, 1.00]
    double lifxBrightness = ocfBrightness / 100.0;

    MPMResult result = l->setBrightness(lifxBrightness);

    if (result != MPM_RESULT_OK)
    {
        throw std::runtime_error("Error setting brightness for PUT request");
    }
    return OC_EH_OK;
}

OCEntityHandlerResult processPutRequest(OCRepPayload *payload, LifxLightSharedPtr l,
                                        uintptr_t resourceTypeInCallback)
{
    if (payload == NULL)
    {
        throw std::runtime_error("PUT payload cannot be NULL");
    }
    if (resourceTypeInCallback == BINARY_SWITCH_CALLBACK)
    {
        return processBinarySwitchUpdate(payload, l);
    }
    else if (resourceTypeInCallback == BRIGHTNESS_CALLBACK)
    {
        return processBrightnessUpdate(payload, l);
    }
    return OC_EH_OK;
}

static LifxLightSharedPtr getLifXLightFromOCFResourceUri(std::string resourceUri)
{
    OIC_LOG_V(INFO, TAG, "Request for %s", resourceUri.c_str());
    std::lock_guard<std::mutex> lock(addedLightsLock);
    for (auto uriToLifXPair : addedLights)
    {
        if (resourceUri.find(uriToLifXPair.first) != std::string::npos)
        {
            return uriToLifXPair.second;
        }
    }
    throw std::runtime_error("Resource " + resourceUri + " not found");
}

OCEntityHandlerResult resourceEntityHandler(OCEntityHandlerFlag ,
        OCEntityHandlerRequest *request,
        void *cb)
{
    uintptr_t callBackParamResourceType = (uintptr_t) cb;
    OCEntityHandlerResult result = OC_EH_OK;
    MPMResult res = MPM_RESULT_OK;

    try
    {
        std::string uri;
        ConcurrentIotivityUtils::getUriFromHandle(request->resource, uri);

        LifxLightSharedPtr targetLight = getLifXLightFromOCFResourceUri(uri);

        switch (request->method)
        {
            case OC_REST_GET:
                // Empty GET case as actual request will be processed after the switch case.
                break;

            case OC_REST_PUT:
            case OC_REST_POST:

                res = (MPMResult)processPutRequest((OCRepPayload *) request->payload, targetLight,
                                                   callBackParamResourceType);
                if (res != MPM_RESULT_OK)
                    result = OC_EH_ERROR;
                break;

            default:
                OIC_LOG_V(INFO, TAG, "Unsupported method (%d) recieved", request->method);
                ConcurrentIotivityUtils::respondToRequestWithError(request, "Unsupported method received",
                        OC_EH_METHOD_NOT_ALLOWED);
                return OC_EH_OK;
        }

        OCRepPayload *responsePayload = processGetRequest(targetLight, callBackParamResourceType);
        ConcurrentIotivityUtils::respondToRequest(request, responsePayload, result);
        OCRepPayloadDestroy(responsePayload);
    }
    catch (const std::exception &exp)
    {
        ConcurrentIotivityUtils::respondToRequestWithError(request, exp.what(), OC_EH_ERROR);
        return OC_EH_OK;
    }

    return OC_EH_OK;
}

/* This function does not look for new lights. Only monitors existing lights.
 * @param[in] pluginSpecificCtx        plugin specific context
 */
void *lightMonitorThread(void *pluginSpecificCtx)
{
    MPMPluginCtx *ctx = (MPMPluginCtx *) pluginSpecificCtx;
    if (ctx != NULL)
    {
        while (ctx->stay_in_process_loop)
        {
            addedLightsLock.lock();

            for (auto itr : addedLights)
            {
                LifxLightSharedPtr l = itr.second;
                if (!l)
                {
                    continue;
                }
                LifxLight::lightState oldState = l->state;

                l->refreshState();

                LifxLight::lightState newState = l->state;

                if (oldState.power != newState.power)
                {
                    ConcurrentIotivityUtils::queueNotifyObservers(itr.first + BINARY_SWITCH_RELATIVE_URI);
                }
                if (fabs(oldState.brightness - newState.brightness) >
                    0.00001) // Lazy epsilon for double equals check.
                {
                    ConcurrentIotivityUtils::queueNotifyObservers(itr.first + BRIGHTNESS_RELATIVE_URI);
                }
                if (oldState.connected != newState.connected)
                {
                    OIC_LOG_V(INFO, TAG, "%s is %s", l->config.id.c_str(),
                              l->state.connected ? "ONLINE" : "OFFLINE");
                }
            }

            addedLightsLock.unlock();
            sleep(MPM_THREAD_PROCESS_SLEEPTIME);
        }
        OIC_LOG(INFO, TAG, "Leaving LIFX monitor thread");
    }
    pthread_exit(NULL);
}
