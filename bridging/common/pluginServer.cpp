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

/* This file contains the plugin server implementation.  Most of this
 * implementation is reused for other plugins.  There is NO customization
 * required of functions in this file to accommodate plugin specific code.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "iotivity_config.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "messageHandler.h"
#include "pluginServer.h"
#include "oic_malloc.h"
#include "pluginIf.h"
#include "ocpayload.h"
#include "experimental/logger.h"
#include "WorkQueue.h"
#include "ConcurrentIotivityUtils.h"
#include <iostream>
#include <octypes.h>

#define TAG "PLUGIN_SERVER"
#define OC_KEY_VALUE_DELIMITER             "="

using namespace OC::Bridging;

//function prototypes

/**
 * This function initializes all the boilerplate items for the OCF server.
 * it relies strictly on the "C" layer of the OCF stack.  This is common
 * code for all plugins.
 *
 * @param[in] ctx            The context structure used by this implementation
 * @param[in] deviceName     Name of the device as defined by the plugin
 * @param[in] resourceType   Resource type as defined by the plugin
 *
 * @returns MPM_RESULT_OK if no errors, MPM_RESULT_INTERNAL_ERROR if initialization failure
 */
static MPMResult initInfrastructure(MPMCommonPluginCtx *ctx, const char *deviceName,
                                    const char *resourceType);

/**
 * this function is a blocking function that holds the iotivity processing
 * loop. this OCF server relies strictly on the "C" layer of the OCF stack.
 * This is common code for all plugins.
 *
 * @param[in,out] result     Child process status
 * @param[in]     ctx        The context structure used by this implementation
 *
 * @return MPM_RESULT_OK if no errors, MPM_RESULT_INTERNAL_ERROR if maintenance failure
 */
static MPMResult maintainInfrastructure(MPMResult result, MPMCommonPluginCtx *ctx);

const char *g_date_of_manufacture = NULL;
const char *g_firmware_version = NULL;
const char *g_manufacturer_name = "Intel";
const char *g_operating_system_version = NULL;
const char *g_hardware_version = NULL;
const char *g_platform_id = "ce530bf4-40ab-11e6-9cca-ff46602aca08";
const char *g_manufacturer_url = NULL;
const char *g_model_number = NULL;
const char *g_platform_version = NULL;
const char *g_support_url = NULL;
const char *g_version = NULL;
const char *g_system_time = NULL;

static pthread_t processMessageFromPipeThread;

/* plugin specific context storage point.  the plugin owns the allocation
 * and freeing of its own context
 */
MPMPluginCtx *g_plugin_context = NULL;


/* Secure Virtual Resource database for Iotivity Server
 * It contains Server's Identity and the PSK credentials
 * of other devices which the server trusts
 */
extern MPMCommonPluginCtx *g_com_ctx;

std::unique_ptr<ConcurrentIotivityUtils> iotivityUtils = NULL;

/**
 * This is a non blocking pipe read function
 *
 * @param[in] fd            file descriptor from where messages are to be read
 * @param[in] com_ctx       common context
 * @param[in] ctx           plugin specific context
 *
 * @return false if STOP request has come from the MPM, true if STOP request
 *         has not come from the MPM
 */
bool processMessagesFromMPM(int fd, MPMCommonPluginCtx *com_ctx, MPMPluginCtx *ctx)
{
    struct timeval tv;
    fd_set fdset;
    int nfd = -1;
    ssize_t nbytes = 0;
    bool shutdown = false;
    MPMPipeMessage pipe_message;
    g_com_ctx = com_ctx;

    tv.tv_sec = 15;
    tv.tv_usec = 0;

    pipe_message.payloadSize = 0;
    pipe_message.msgType = MPM_NOMSG;
    pipe_message.payload = NULL;

    FD_ZERO(&(fdset));
    FD_SET(fd, &(fdset));
    nfd = select(fd + 1, &(fdset), NULL, NULL, &tv);
    if (nfd == -1)
    {
        OIC_LOG_V(ERROR, TAG, "select error: %s", strerror(errno));
    }
    else
    {
        if (FD_ISSET(fd, &(fdset)))
        {
            nbytes = MPMReadPipeMessage(fd, &pipe_message);
            if (nbytes == 0)
            {
                OIC_LOG(DEBUG, TAG, "EOF was read and file descriptor was found to be closed");
                shutdown = true;
            }
            if (nbytes > 0)
            {
                if (pipe_message.msgType == MPM_STOP)
                {
                    shutdown =  true;
                }
                else
                {
                    MPMRequestHandler(&pipe_message, ctx);
                }
            }

            OICFree((void*)pipe_message.payload);

        }
    }
    return (shutdown);
}

MPMResult MPMPluginService(MPMCommonPluginCtx *ctx)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    if (ctx == NULL)
    {
        OIC_LOG(ERROR, TAG, "Plugin context is NULL");
        goto HandleError;
    }

    // plugin create is in the individual plugin.
    result = pluginCreate(&g_plugin_context);

    if (result != MPM_RESULT_OK || g_plugin_context == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Creation failed result: %d", result);
        goto HandleError;
    }

    // initialize the OCF infrastructure to be setup for a server
    result = initInfrastructure(ctx, g_plugin_context->device_name, g_plugin_context->resource_type);

    if (result != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "Error (%d) initializing OCF infrastructure", result);
        goto HandleError;
    }
    if ('\0' != ctx->reconnect_file_name[0])
    {
        strncpy(g_plugin_context->reconnect_file_name, ctx->reconnect_file_name,
                strlen(ctx->reconnect_file_name));
    }
    else
    {
        memset(g_plugin_context->reconnect_file_name, 0, MPM_MAX_FILE_NAME_LENGTH);
    }
    // plugin start is in the individual plugin.
    result = pluginStart(g_plugin_context);

    if (result != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to start %s plugin result: %d", g_plugin_context->device_name,
                  result);
    }

HandleError :
    /* Sends the status of the child to Parent and
     * in the case of success it act as blocking call.
    */
    result = maintainInfrastructure(result, ctx);

    return (result);
}

static MPMResult setPlatformInfoParams(OCPlatformInfo &platform_info)
{
    if ((strlen(g_manufacturer_name) > MAX_PLATFORM_NAME_LENGTH))
    {
        OIC_LOG(ERROR, TAG, "Manufacture name string length exceeded max length");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    if (g_manufacturer_url != NULL && (strlen(g_manufacturer_url) > MAX_PLATFORM_URL_LENGTH))
    {
        OIC_LOG(ERROR, TAG, "Url string length exceeded max length");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    platform_info.platformID = const_cast<char *> (g_platform_id);
    platform_info.manufacturerName = const_cast<char *> (g_manufacturer_name);
    platform_info.manufacturerUrl = const_cast<char *> (g_manufacturer_url);
    platform_info.modelNumber = const_cast<char *> (g_model_number);
    platform_info.dateOfManufacture = const_cast<char *> (g_date_of_manufacture);
    platform_info.platformVersion = const_cast<char *> (g_platform_version);
    platform_info.operatingSystemVersion = const_cast<char *> (g_operating_system_version);
    platform_info.hardwareVersion = const_cast<char *> (g_hardware_version);
    platform_info.firmwareVersion = const_cast<char *> (g_firmware_version);
    platform_info.supportUrl = const_cast<char *> (g_support_url);
    platform_info.systemTime = const_cast<char *> (g_system_time);

    return MPM_RESULT_OK;
}

static MPMResult setDeviceInfoParams(const char *deviceName, const char *resourceType,
                                     OCDeviceInfo &device_info)
{
    if (!deviceName || deviceName[0] == '\0')
    {
        OIC_LOG(ERROR, TAG, "Device name is NULL");
        return MPM_RESULT_INVALID_PARAMETER;
    }
    device_info.deviceName = const_cast<char *> (deviceName);

    OCStringLL *stringll = NULL;

    OCResourcePayloadAddStringLL(&stringll, OC_RSRVD_RESOURCE_TYPE_DEVICE);
    OCResourcePayloadAddStringLL(&stringll, resourceType);

    device_info.types = stringll;

    device_info.dataModelVersions = NULL;
    device_info.specVersion = NULL;

    return MPM_RESULT_OK;
}

static MPMResult initInfrastructure(MPMCommonPluginCtx *ctx, const char *deviceName,
                                    const char *resourceType)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    uint16_t port = 0;

    if (ctx != NULL)
    {
        /* Create the unnamed pipe listener thread that will clear the
         * child's stay in the loop variable when the parent wants to
         * shut down the child process.
         */
        // Initialize Persistent Storage for SVR database
        static OCPersistentStorage ps = {g_plugin_context->open, fread, fwrite, fclose, unlink};
        OCRegisterPersistentStorageHandler(&ps);

        OCStackResult ocResult = OCInit(NULL, port, OC_SERVER);
        if (ocResult != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "OCStack init error");
            return result;
        }

        std::unique_ptr<WorkQueue<std::unique_ptr<IotivityWorkItem>>> q =
            std::unique_ptr<WorkQueue<std::unique_ptr<IotivityWorkItem>>>
            (new WorkQueue<std::unique_ptr<IotivityWorkItem>>());

        iotivityUtils =  make_unique<ConcurrentIotivityUtils>(std::move(q));
        iotivityUtils->startWorkerThreads();

        OCPlatformInfo platform_info;
        if (setPlatformInfoParams(platform_info) != MPM_RESULT_OK)
        {
            OIC_LOG(ERROR, TAG, "Platform info setting failed locally!");
            return result;
        }

        if (OCSetPlatformInfo(platform_info) != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "Platform Registration failed!");
            return result;
        }

        OCDeviceInfo device_info = {NULL, NULL, NULL, NULL};
        if (setDeviceInfoParams(deviceName, resourceType, device_info) != MPM_RESULT_OK)
        {
            OIC_LOG(ERROR, TAG, "Device info setting failed locally!");
            return result;
        }

        if (OCSetDeviceInfo(device_info) != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "Device Registration failed!");
            return result;
        }
        // Iotivity does not take ownership of 'types' and clones itself a copy.
        OCFreeOCStringLL(device_info.types);
    }
    return MPM_RESULT_OK;
}

void *processMessageFromPipeThreadProc(void *arg)
{
    MPMCommonPluginCtx *ctx = (MPMCommonPluginCtx *)arg;
    while (true)
    {
        bool result = processMessagesFromMPM(ctx->child_reads_fds.read_fd, ctx, g_plugin_context);
        if (result != MPM_RESULT_OK)
        {
            OIC_LOG(INFO, TAG, "Leaving processMessageFromPipeThreadProc ");
            break;
        }
    }
    pthread_exit(NULL);
    return NULL;
}

MPMResult maintainInfrastructure(MPMResult result, MPMCommonPluginCtx *ctx)
{
    MPMPipeMessage pipe_message;
    void *res;

    if (ctx != NULL)
    {
        /* child tells status to parent prior to getting in the
         * processing loop
         */
        if (result == MPM_RESULT_OK)
        {
            pipe_message.msgType = MPM_DONE;
            pipe_message.payloadSize = 0;
            pipe_message.payload = NULL;
            result = MPMWritePipeMessage(ctx->parent_reads_fds.write_fd, &pipe_message);
        }
        else
        {
            pipe_message.msgType = MPM_ERROR;
            pipe_message.payloadSize = 0;
            pipe_message.payload = NULL;
            result = MPMWritePipeMessage(ctx->parent_reads_fds.write_fd, &pipe_message);
        }

        if (result == MPM_RESULT_OK)
        {
            pthread_create(&processMessageFromPipeThread, NULL, processMessageFromPipeThreadProc, ctx);
            pthread_join(processMessageFromPipeThread, &res);
        }
        else
        {
            return result;
        }

        OIC_LOG_V(INFO, TAG, "Stopping %s plugin", g_plugin_context->device_name);

        result = pluginStop(g_plugin_context);

        if (result != MPM_RESULT_OK)
        {
            OIC_LOG_V(ERROR, TAG, "Error(%d) stopping plugin %s", result, g_plugin_context->device_name);
        }

        result = pluginDestroy(g_plugin_context);
        if (result != MPM_RESULT_OK)
        {
            OIC_LOG_V(ERROR, TAG, "Error(%d) stopping plugin %s", result, g_plugin_context->device_name);
        }

        iotivityUtils->stopWorkerThreads();

        OCStackResult ocResult = OCStop();

        if (ocResult != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, TAG, "Error(%d) stopping Iotivity", ocResult);
            result = MPM_RESULT_INTERNAL_ERROR;
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Bad context handed to maintain ocf infrastructure");
    }
    return (result);
}

MPMResult MPMExtractFiltersFromQuery(char *query, char **filterOne, char **filterTwo)
{

    char *key = NULL;
    char *value = NULL;
    char *restOfQuery = NULL;
    int numKeyValuePairsParsed = 0;

    *filterOne = NULL;
    *filterTwo = NULL;

    if (!query)
    {
        // This is fine. This call is stemming from a non-entity handler request.
        return MPM_RESULT_OK;
    }

    char *keyValuePair = strtok_r(query, OC_QUERY_SEPARATOR, &restOfQuery);

    while (keyValuePair)
    {
        if (numKeyValuePairsParsed >= 2)
        {
            OIC_LOG(ERROR, TAG, "More than 2 queries params in URI.");
            return MPM_RESULT_INVALID_PARAMETER;
        }

        key = strtok_r(keyValuePair, OC_KEY_VALUE_DELIMITER, &value);

        if (!key || !value)
        {
            return MPM_RESULT_INVALID_PARAMETER;
        }
        else if (strncasecmp(key, OC_RSRVD_INTERFACE, sizeof(OC_RSRVD_INTERFACE) - 1) == 0)
        {
            *filterOne = value;     // if
        }
        else if (strncasecmp(key, OC_RSRVD_RESOURCE_TYPE, sizeof(OC_RSRVD_INTERFACE) - 1) == 0)
        {
            *filterTwo = value;     // rt
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "Unsupported query key: %s", key);
            return MPM_RESULT_INVALID_PARAMETER;
        }
        ++numKeyValuePairsParsed;

        keyValuePair = strtok_r(NULL, OC_QUERY_SEPARATOR, &restOfQuery);
    }
    if (*filterOne != NULL || *filterTwo != NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Extracted params if: %s and rt: %s.", *filterOne, *filterTwo);
    }

    return MPM_RESULT_OK;
}
