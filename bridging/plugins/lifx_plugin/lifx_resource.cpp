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
#include "plugin_server.h"
#include "ocpayload.h"
#include "lifx.h"
#include "logger.h"
#include "ConcurrentIotivityUtils.h"

plugin_ctx_t *g_plugin_ctx = NULL;

#define DEVICE_NAME "LIFX_BULB"
#define DEVICE_TYPE "LIGHT_BULB"
#define MANUFACTURER_NAME "LIFX"

#define LIFX_SWITCH_RT "oic.r.switch.binary"
#define LIFX_IF        "oic.if.a"
#define BM 3

using namespace OC::Bridging;

typedef struct
{
    char id[MAX_LENGTH_64];
    char uuid[MAX_LENGTH_64];
    char label[MAX_LENGTH_64];
    char user[MAX_LENGTH_256];
}LightDetails;

std::map<std::string, LifxLightSharedPtr> uriToLifxLightMap;
std::map<std::string, LifxLightSharedPtr> addedLights;

// Forward declarations.
void *lightMonitorThread(void *pointer);
OCEntityHandlerResult resourceEntityHandler(OCEntityHandlerFlag flag,OCEntityHandlerRequest *request, void *callbackParam);

LifxLightSharedPtr getLifXLightFromOCFResourceUri(std::string resourceUri);

const uint BINARY_SWITCH_CALLBACK = 0;
const uint BRIGHTNESS_CALLBACK = 1;
static const std::string BINARY_SWITCH_RELATIVE_URI = "/switch";
static const std::string BRIGHTNESS_RELATIVE_URI = "/brightness";

#define MAX_ACCESS_TOKEN_LENGTH 1024
char accessToken[MAX_ACCESS_TOKEN_LENGTH];
#define TAG "LIFX_RESOURCE"
static char CRED_FILE[] = "./oic_svr_db_lifx.dat";

FILE *lifxSecurityFile(const char *path, const char *mode)
{
    (void) path;
    return fopen(CRED_FILE, mode);
}


GW_RESULT plugin_create(plugin_ctx_t **pluginSpecificCtx)
{
    if (g_plugin_ctx != NULL)
    {
        OIC_LOG(ERROR, TAG, "Plugin is already created.");
        return GW_RESULT_ALREADY_CREATED;
    }

    plugin_ctx_t *ctx = (plugin_ctx_t *) calloc(1, sizeof(plugin_ctx_t));

    if (ctx == NULL)
    {
        OIC_LOG(ERROR, TAG, "Allocation of plugin context failed");
        return GW_RESULT_INTERNAL_ERROR;
    }

    *pluginSpecificCtx = ctx;
    g_plugin_ctx = ctx;

    ctx->device_name = (char*) "Lifx Translator";
    ctx->resource_type = (char*) "oic.d.light";
    ctx->open = lifxSecurityFile;


    FILE *fp = fopen("./lifx.cnf", "r");

    if (NULL == fp)
    {
        OIC_LOG(ERROR, TAG, "error loading lifx.cnf file.");
        return GW_RESULT_INTERNAL_ERROR;
    }

    if (fgets(accessToken, MAX_ACCESS_TOKEN_LENGTH - 1, fp) == NULL)
    {
        OIC_LOG(ERROR, TAG, "Failed to read ./lifx.cnf");
        fclose(fp);
        return GW_RESULT_INTERNAL_ERROR;
    }
    accessToken[strlen(accessToken)-1] = '\0';
    fclose(fp);

    return GW_RESULT_OK;
}

GW_RESULT plugin_start(plugin_ctx_t *ctx)
{
    GW_RESULT result = GW_RESULT_INVALID_PARAMETER;
    int error = 0;

    if (ctx->started)
    {
        OIC_LOG(INFO, TAG, "Plugin is already started.");
        return GW_RESULT_ALREADY_STARTED;
    }

    ctx->stay_in_process_loop = true;

    // TODO: Maybe move this to plugin_add()? once the first light is added?
    error = pthread_create(&(ctx->thread_handle), NULL, lightMonitorThread, ctx);
    if (error == 0)
    {
        ctx->started = true;
        result = GW_RESULT_OK;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "Can't create plugin specific thread :[%s]", strerror(error));
        plugin_stop(ctx);
        result = GW_RESULT_STARTED_FAILED;
    }

    OIC_LOG_V(INFO, TAG, "Plugin start return value:%d.", result);
    return (result);
}

GW_RESULT plugin_scan(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx; (void) message;

    std::vector<LifxLightSharedPtr> lightsScanned;

    GW_RESULT result = LifxLight::getLights(accessToken, lightsScanned);

    for (uint32_t i = 0; i < lightsScanned.size(); ++i)
    {
        LifxLightSharedPtr light = lightsScanned[i];

        OIC_LOG_V(INFO, TAG,"Found %s bulb %s(%s).", light->state.connected ? "CONNECTED" : "OFFLINE",
            light->config.id.c_str(), light->config.label.c_str());

        if (!light->state.connected)
        {
            OIC_LOG(INFO, TAG, "Ignoring OFFLINE light");
            continue;
        }

        std::string uri = "/lifx/" + light->config.id;

        if(uriToLifxLightMap.find(uri) != uriToLifxLightMap.end())
        {
            OIC_LOG_V(INFO, TAG,"Already found %s. Ignoring", uri.c_str());
            continue;
        }

        uriToLifxLightMap[uri] = light;

        std::string s = uri;
        unsigned long size = s.size() + 1;
        char *m = (char*) calloc(1, size);
        memcpy(m, s.c_str(), size);
        send_response(m, size, MPM_SCAN);
        free(m);

    }
    if(result != GW_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG,"Failed to fetch lights with error (%d)", result);
        return GW_RESULT_INTERNAL_ERROR;
    }

    return GW_RESULT_OK;

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

void createPayloadForMetadata(resourceList **list, const char *uri, const char *res_type,const char *interface)
{
    resourceList *tempPtr;
    tempPtr = (resourceList *) calloc(1, sizeof(resourceList));
    strncpy(tempPtr->rt, res_type, MAX_LENGTH_64);
    strncpy(tempPtr->href, uri , MAX_URI_LEN);
    strncpy(tempPtr->interfaces, interface, MAX_LENGTH_64);
    tempPtr->bitmap = BM;
    tempPtr->next = *list;
    *list  = tempPtr;
}

/***
 * Creates 2 OCF resources for a LifxLight. One for the switch to turn it on and off
 * and one for brightness.
 *
 * @param uri Base uri. Switch and brightness uris are baseUri appended with "/switch" & "/brightness"
 * @return
 */
GW_RESULT createOCFResources(const std::string &uri)
{
    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    if (createSecureResources()) resourceProperties |= OC_SECURE;

    std::string switchUri = uri + BINARY_SWITCH_RELATIVE_URI;
    ConcurrentIotivityUtils::createResource(switchUri, "oic.r.switch.binary", "oic.if.a", resourceEntityHandler,
                                            (void*) BINARY_SWITCH_CALLBACK, resourceProperties);

    std::string brightnessUri = uri + BRIGHTNESS_RELATIVE_URI;
    ConcurrentIotivityUtils::createResource(brightnessUri, "oic.r.brightness", "oic.if.a", resourceEntityHandler,
                                            (void *) BRIGHTNESS_CALLBACK, resourceProperties);

    return GW_RESULT_OK;
}

GW_RESULT  deleteOCFResources(const std::string &uri)
{
    ConcurrentIotivityUtils::deleteResource(uri + BINARY_SWITCH_RELATIVE_URI);
    ConcurrentIotivityUtils::deleteResource(uri + BRIGHTNESS_RELATIVE_URI);
    return GW_RESULT_OK;
}


GW_RESULT plugin_add(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx;
    char *c_uri = NULL;
    uint8_t *buff = NULL;
    resourceList *list = NULL;
    device_specific_data_t deviceConfiguration;
    LightDetails pluginSpecificDetails;
    std::string user;
    memset(&pluginSpecificDetails, 0, sizeof(LightDetails));
    memset(&deviceConfiguration, 0, sizeof(device_specific_data_t));

    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    if (createSecureResources()) resourceProperties |= OC_SECURE;

    if(message->payloadSize > 0 && message->payloadSize < SIZE_MAX)
    {
        c_uri = (char*) calloc(1, message->payloadSize);
    }
    if(c_uri == NULL)
    {
        OIC_LOG(ERROR, TAG, "Calloc Failed");
        return GW_RESULT_INTERNAL_ERROR;
    }
    memcpy(c_uri, message->payload, message->payloadSize);
    std::string uri(c_uri);
    free(c_uri);

    if(addedLights.find(uri) != addedLights.end())
    {
        OIC_LOG_V(ERROR, TAG,"%s already added", uri.c_str());
        return GW_RESULT_ALREADY_CREATED;
    }
    if(uriToLifxLightMap.find(uri) == uriToLifxLightMap.end())
    {
        OIC_LOG_V(ERROR, TAG,"%s was NOT discovered in a scan", uri.c_str());
        return GW_CB_RESULT_ERROR;
    }

    createOCFResources(uri);

    buff = (uint8_t *)calloc(1, MAX_METADATA_LEN);
    if(buff == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to allocate memory for reconnect buffer");
        return GW_RESULT_INTERNAL_ERROR;
    }
    std::string switchUri = uri + BINARY_SWITCH_RELATIVE_URI;
    createPayloadForMetadata(&list, switchUri.c_str(), "oic.r.switch.binary", "oic.if.a" );

    std::string brightnessUri = uri + BRIGHTNESS_RELATIVE_URI;
    createPayloadForMetadata(&list, brightnessUri.c_str(), "oic.r.brightness", "oic.if.a");

    LifxLightSharedPtr targetLight = uriToLifxLightMap[uri];
    targetLight->getUser(user);

    // filling plugin specific details
    strncpy(pluginSpecificDetails.id, targetLight->config.id.c_str(), MAX_LENGTH_64);
    strncpy(pluginSpecificDetails.label, targetLight->config.label.c_str(), MAX_LENGTH_64);
    strncpy(pluginSpecificDetails.uuid, targetLight->config.uuid.c_str(), MAX_LENGTH_64);
    strncpy(pluginSpecificDetails.user, user.c_str(), MAX_LENGTH_256);
    // filling device specific details

    strncpy(deviceConfiguration.devName, DEVICE_NAME, MAX_LENGTH_64);
    strncpy(deviceConfiguration.devType, DEVICE_TYPE, MAX_LENGTH_64);
    strncpy(deviceConfiguration.manufacturerName, MANUFACTURER_NAME, MAX_LENGTH_256);

    formMetaData(list, &deviceConfiguration, buff, MAX_METADATA_LEN, &pluginSpecificDetails, sizeof(pluginSpecificDetails));

    addedLights[uri] = uriToLifxLightMap[uri];

    add_response response;
    strncpy(response.uri, uri.c_str(), MAX_URI_LEN);
    memcpy(response.metadata, buff, MAX_METADATA_LEN);

    size_t size = sizeof(add_response);

    send_response(&response, size, MPM_ADD);

    free(buff);
    return GW_RESULT_OK;
}

GW_RESULT plugin_remove(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx;
    char *c_uri = NULL;

    if(message->payloadSize > 0 && message->payloadSize < SIZE_MAX)
    {
        c_uri = (char*) calloc(1, message->payloadSize);
    }
    if(c_uri == NULL)
    {
        OIC_LOG(ERROR, TAG, "Calloc Failed");
        return GW_RESULT_INTERNAL_ERROR;
    }
    memcpy(c_uri, message->payload, message->payloadSize);
    std::string uri(c_uri);
    free(c_uri);

    deleteOCFResources(uri);

    addedLights.erase(uri);
    uriToLifxLightMap.erase(uri);

    std::string s = uri;
    unsigned long size = s.size() + 1;
    char *m = (char*) calloc(1, size);
    memcpy(m, s.c_str(), size);
    send_response(m, size, MPM_REMOVE);
    free(m);

    return GW_RESULT_OK;
}

GW_RESULT plugin_reconnect(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx;
    resourceList *list=NULL, *temp = NULL;
    char *c_uri = NULL;
    void * pluginSpecificDetails = NULL;
    if(message->payloadSize > 0 && message->payloadSize < SIZE_MAX)
    {
        c_uri = (char*) calloc(1, message->payloadSize);
    }
    if(c_uri == NULL)
    {
        OIC_LOG(ERROR, TAG, "Calloc Failed");
        return GW_RESULT_INTERNAL_ERROR;
    }
    memcpy(c_uri, message->payload, message->payloadSize);

    ParseMetaData((uint8_t *)c_uri, MAX_METADATA_LEN, &list, &pluginSpecificDetails);
    LightDetails *lightDetails = (LightDetails *)pluginSpecificDetails;

    uint8_t resourceProperties = (OC_OBSERVABLE | OC_DISCOVERABLE);
    if (createSecureResources()) resourceProperties |= OC_SECURE;
    OIC_LOG_V(DEBUG, TAG, "\n Reconnect Details \nid - %s\nuuid - %s\nlabel - %s\nuser - %s\n",lightDetails->id, lightDetails->uuid, lightDetails->label,lightDetails->user);
    LifxLight::lightState state;
    LifxLight::lightConfig cfg(lightDetails->id, lightDetails->uuid, lightDetails->label);
    std::string uri = "/lifx/" + cfg.id;
    std::shared_ptr<LifxLight> light = std::make_shared<LifxLight>(state, cfg, lightDetails->user);

    createOCFResources(uri);
    uriToLifxLightMap[uri] = light;
    addedLights[uri] = uriToLifxLightMap[uri];

    while(list)
    {
        temp = list;
        list = list->next;
        free(temp);
    }
    free(c_uri);
    free(lightDetails);
    return GW_RESULT_OK;
}

GW_RESULT plugin_stop(plugin_ctx_t *pluginSpecificCtx)
{
    GW_RESULT result = GW_RESULT_OK;

    plugin_ctx_t *ctx = pluginSpecificCtx;

    if (NULL != ctx && g_plugin_ctx != NULL)
    {
        for(auto itr: addedLights)
        {
            ConcurrentIotivityUtils::deleteResource(itr.first + BINARY_SWITCH_RELATIVE_URI);
            ConcurrentIotivityUtils::deleteResource(itr.first + BRIGHTNESS_RELATIVE_URI);
        }

        addedLights.clear();
        uriToLifxLightMap.clear();

        if (ctx->started)
        {
            ctx->stay_in_process_loop = false;
            result = pthread_join(ctx->thread_handle, NULL);
            ctx->started = false;
        }
    }

    OIC_LOG_V(INFO, TAG,"Plugin stop's return value:%d", result);
    return (result);

}


GW_RESULT plugin_destroy(plugin_ctx_t *pluginSpecificCtx)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    plugin_ctx_t *ctx = (plugin_ctx_t *) pluginSpecificCtx;

    if (ctx != NULL && g_plugin_ctx != NULL)
    {
        if (ctx->started)
        {
            plugin_stop(pluginSpecificCtx);
        }
        // freeing the resource allocated in create
        free(ctx);
        g_plugin_ctx = NULL;
        result = GW_RESULT_OK;
    }

    OIC_LOG_V(INFO, TAG,"Plugin destroy's return value:%d", result);

    return (result);
}



OCRepPayload *addCommonLifXProperties(const LifxLightSharedPtr &l, OCRepPayload *payload)
{
    OCRepPayloadSetPropString(payload, "x.com.intel.label", l->config.label.c_str());
    OCRepPayloadSetPropDouble(payload, "x.com.intel.secondsSinceLastSeen", l->state.secondsSinceLastSeen);
    return payload;
}

OCRepPayload* getBinarySwitchPayload(LifxLightSharedPtr l)
{
    OCRepPayload *payload = OCRepPayloadCreate();
    OCRepPayloadSetPropBool(payload, "value", l->state.power);
    return addCommonLifXProperties(l, payload);
}

OCRepPayload *getBrightnessPayload(LifxLightSharedPtr l)
{
    OCRepPayload *payload = OCRepPayloadCreate();

    // Convert LifX [0.0, 1.0] to OCF [0, 100].
    OCRepPayloadSetPropInt(payload, "brightness", (int64_t) (l->state.brightness * 100.0));
    return addCommonLifXProperties(l, payload);

}

OCRepPayload* process_get_request(LifxLightSharedPtr l, uintptr_t resourceTypeInCallback)
{
    if(resourceTypeInCallback == BINARY_SWITCH_CALLBACK)
    {
        return getBinarySwitchPayload(l);
    }
    else if(resourceTypeInCallback == BRIGHTNESS_CALLBACK)
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

    GW_RESULT result = l->setPower(power);

    if (result != GW_RESULT_OK)
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

    GW_RESULT result = l->setBrightness(lifxBrightness);

    if (result != GW_RESULT_OK)
    {
        throw std::runtime_error("Error setting brightness for PUT request");
    }
    return OC_EH_OK;
}
OCEntityHandlerResult process_put_request(OCRepPayload *payload, LifxLightSharedPtr l, uintptr_t resourceTypeInCallback)
{
    if (payload == NULL)
    {
        throw std::runtime_error("PUT payload cannot be NULL");
    }
    if(resourceTypeInCallback == BINARY_SWITCH_CALLBACK)
    {
        return processBinarySwitchUpdate(payload, l);
    }
    else if(resourceTypeInCallback == BRIGHTNESS_CALLBACK)
    {
        return processBrightnessUpdate(payload, l);
    }
    return OC_EH_OK;
}

LifxLightSharedPtr getLifXLightFromOCFResourceUri(std::string resourceUri)
{
    OIC_LOG_V(INFO, TAG, "Request for %s", resourceUri.c_str());

    for(auto uriToLifXPair : addedLights)
    {
        if (resourceUri.find(uriToLifXPair.first) != std::string::npos)
        {
            return uriToLifXPair.second;
        }
    }
    throw std::runtime_error("Resource " + resourceUri + " not found");
}

OCEntityHandlerResult resourceEntityHandler(OCEntityHandlerFlag flag, OCEntityHandlerRequest *request,
                            void *cb)
{
    (void) flag;
    uintptr_t callBackParamResourceType = (uintptr_t) cb;
    OCEntityHandlerResult result = OC_EH_OK;
    GW_RESULT res = GW_RESULT_OK;

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

                res = process_put_request((OCRepPayload*) request->payload, targetLight, callBackParamResourceType);
                if (res != GW_RESULT_OK)
                    result = OC_EH_ERROR;
                break;

            default:
                OIC_LOG_V(INFO, TAG,"Unsupported method (%d) recieved", request->method);
                ConcurrentIotivityUtils::respondToRequestWithError(request, "Unsupported method received",
                    OC_EH_METHOD_NOT_ALLOWED);
                return OC_EH_OK;
        }

        OCRepPayload *responsePayload = process_get_request(targetLight, callBackParamResourceType);

        ConcurrentIotivityUtils::respondToRequest(request, responsePayload, result);

        OCRepPayloadDestroy(responsePayload);
    }
    catch (const std::exception& exp)
    {
        ConcurrentIotivityUtils::respondToRequestWithError(request, exp.what(), OC_EH_ERROR);
        return OC_EH_OK;
    }

    return OC_EH_OK;
}

// Does not look for new lights. Only monitors existing lights.
void *lightMonitorThread(void *pointer)
{
    plugin_ctx_t *ctx = (plugin_ctx_t *) pointer;
    if (ctx != NULL)
    {
        while (ctx->stay_in_process_loop)
        {
            for(auto itr: addedLights)
            {
                LifxLightSharedPtr l = itr.second;
                if (!l)
                {
                    continue;
                }
                LifxLight::lightState oldState = l->state;

                l->refreshState();

                LifxLight::lightState newState = l->state;

                if(oldState.power != newState.power)
                {
                    ConcurrentIotivityUtils::notifyObservers(itr.first + BINARY_SWITCH_RELATIVE_URI);
                }
                if(fabs(oldState.brightness - newState.brightness) > 0.00001) // Lazy epsilon for double equals check.
                {
                    ConcurrentIotivityUtils::notifyObservers(itr.first + BRIGHTNESS_RELATIVE_URI);
                }
                if (oldState.connected != newState.connected)
                {
                    OIC_LOG_V(INFO, TAG, "%s is %s", l->config.id.c_str(),
                        l->state.connected? "ONLINE": "OFFLINE");
                }
            }
            sleep(THREAD_PROCESS_SLEEPTIME);
        }
        OIC_LOG(INFO, TAG,"Leaving LIFX monitor thread");
    }
    pthread_exit(NULL);
}
