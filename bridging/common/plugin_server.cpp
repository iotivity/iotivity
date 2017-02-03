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
#include <unistd.h>

#include "message_handler.h"
#include "plugin_server.h"
#include "oic_string.h"
#include "plugin_if.h"
#include "ocpayload.h"
#include "logger.h"
#include "WorkQueue.h"
#include "ConcurrentIotivityUtils.h"
#include <iostream>
#include <octypes.h>

#define TAG "PLUGIN_SERVER"
#define OC_KEY_VALUE_DELIMITER             "="

extern int errno;

using namespace OC::Bridging;

/* function prototypes */

GW_RESULT init_oic_infrastructure(common_plugin_ctx_t *ctx, const char *deviceName, const char *resourceType);

GW_RESULT maintain_oic_infrastructure(GW_RESULT result, common_plugin_ctx_t *ctx);

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

pthread_t processMessageFromPipeThread;


pthread_mutex_t plugin_server_mutex;

/* plugin specific context storage point.  the plugin owns the allocation
 * and freeing of its own context */
plugin_ctx_t *g_plugin_context = NULL;


//Secure Virtual Resource database for Iotivity Server
//It contains Server's Identity and the PSK credentials
//of other devices which the server trusts
static char CRED_FILE[] = "./oic_svr_db_server.json";


extern common_plugin_ctx_t *g_com_ctx;

FILE *server_fopen(const char *path, const char *mode)
{
    (void) path;
    return fopen(CRED_FILE, mode);
}

std::unique_ptr<ConcurrentIotivityUtils> iotivityUtils = NULL;

/*
 * This is a non blocking pipe read function
 *
 * returns
 *      false - STOP request has come from the MPM
 *      true  - STOP request has not come from the MPM
 */
bool processMessagesFromMPM(int32_t fd, common_plugin_ctx_t *com_ctx, plugin_ctx_t *ctx)
{
    struct timeval tv;
    fd_set fdset;
    int32_t nfd = -1;
    int32_t nbytes = 0;
    bool shutdown = false;
    mpm_pipe_message_t pipe_message;
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
        OIC_LOG_V(ERROR, TAG,"select error: %s", strerror(errno));
    }
    else
    {
        if (FD_ISSET(fd, &(fdset)))
        {
            nbytes = MPMReadPipeMessage(fd, &pipe_message);
            if(nbytes == 0)
            {
                OIC_LOG(DEBUG, TAG,"EOF was read and file descriptor was found to be closed");
                shutdown = true;
            }
            if(nbytes > 0)
            {
                if(pipe_message.msgType == MPM_STOP)
                {
                    shutdown =  true;
                }
                else
                {
                    pthread_mutex_lock(&plugin_server_mutex);
                    request_handler(&pipe_message, ctx);
                    pthread_mutex_unlock(&plugin_server_mutex);
                }
            }
            if(pipe_message.payload != NULL)
            {
                free(pipe_message.payload);
            }
        }
    }
    return (shutdown);
}


/* this function is a OIC server.  The function does not return unless there is an
 * error or this main thread was signaled by the parent process main thread.
 */
GW_RESULT plugin_service(common_plugin_ctx_t *ctx)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;
    if (ctx == NULL)
    {
        OIC_LOG(ERROR, TAG,"Plugin context is NULL");
        goto HandleError;
    }

    // plugin_create is in the individual plugin.
    result = plugin_create(&g_plugin_context);

    if (result != GW_RESULT_OK || g_plugin_context == NULL)
    {
        OIC_LOG_V(ERROR, TAG,"Creation failed result: %d", result);
        goto HandleError;
    }

    // initialize the OIC infrastructure to be setup for a server
    result = init_oic_infrastructure(ctx, g_plugin_context->device_name, g_plugin_context->resource_type);

    if (result != GW_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG,"Error (%d) initializing OIC infrastructure", result);
        goto HandleError;
    }
    if(ctx->reconnect_file_name != NULL)
    {
        strncpy(g_plugin_context->reconnect_file_name, ctx->reconnect_file_name, strlen(ctx->reconnect_file_name));
    }
    else
    {
        memset(g_plugin_context->reconnect_file_name, 0, MAX_FILE_NAME_LENGTH);
    }
    // plugin_start is in the individual plugin.
    result = plugin_start(g_plugin_context);

    if (result != GW_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG,"Failed to start %s plugin result: %d", g_plugin_context->device_name, result);
    }

    HandleError :
    /* Sends the status of the child to Parent and
     * in the case of success it act as blocking call.
    */
    result = maintain_oic_infrastructure(result, ctx);

    return (result);
}

GW_RESULT set_platform_info_params(OCPlatformInfo &platform_info)
{
    if ((strlen(g_manufacturer_name) > MAX_MANUFACTURER_NAME_LENGTH)
        || (g_manufacturer_url != NULL && (strlen(g_manufacturer_url) > MAX_MANUFACTURER_URL_LENGTH)))
    {
        OIC_LOG(ERROR, TAG,"Manufacture name or/and url string length exceeded max length");
        return GW_RESULT_INTERNAL_ERROR;
    }

    platform_info.platformID = (char *) g_platform_id;
    platform_info.manufacturerName = (char *) g_manufacturer_name;
    platform_info.manufacturerUrl = (char *) g_manufacturer_url;
    platform_info.modelNumber = (char *) g_model_number;
    platform_info.dateOfManufacture = (char *) g_date_of_manufacture;
    platform_info.platformVersion = (char *) g_platform_version;
    platform_info.operatingSystemVersion = (char *) g_operating_system_version;
    platform_info.hardwareVersion = (char *) g_hardware_version;
    platform_info.firmwareVersion = (char *) g_firmware_version;
    platform_info.supportUrl = (char *) g_support_url;
    platform_info.systemTime = (char *) g_system_time;

    return GW_RESULT_OK;
}

GW_RESULT set_device_info_params(const char *deviceName, const char *resourceType, OCDeviceInfo &device_info)
{
    if (!deviceName || deviceName[0] == '\0')
    {
        OIC_LOG(ERROR, TAG,"Device name is NULL");
        return GW_RESULT_INVALID_PARAMETER;
    }
    device_info.deviceName = (char *) deviceName;

    OCStringLL *stringll = NULL;

    OCResourcePayloadAddStringLL(&stringll, OC_RSRVD_RESOURCE_TYPE_DEVICE);
    OCResourcePayloadAddStringLL(&stringll, resourceType);

    device_info.types = stringll;

    device_info.dataModelVersions = NULL;
    device_info.specVersion = NULL;

    return GW_RESULT_OK;
}


/*
 * This function initializes all the boiler plate items for the OIC server.
 * it relies strictly on the "C" layer of the OIC stack.  This is common
 * code for all plugins.
 *
 * Parameters:
 * ctx (in) - the context structure used by this implementation
 *
 * Returns:
 *     GW_RESULT_OK             - no errors
 *     GW_RESULT_INTERNAL_ERROR - initialization failure
 */
GW_RESULT init_oic_infrastructure(common_plugin_ctx_t *ctx, const char *deviceName, const char *resourceType)
{
    GW_RESULT result = GW_RESULT_INTERNAL_ERROR;

    uint16_t port = 0;

    if (ctx != NULL)
    {
        /* Create the unnamed pipe listener thread that will clear the
         * child's stay in the loop variable when the parent wants to
         * shut down the child process.
         */
        ctx->exit_process_loop = false;

        // Initialize Persistent Storage for SVR database
        static OCPersistentStorage ps = {g_plugin_context->open, fread, fwrite, fclose, unlink};
        OCRegisterPersistentStorageHandler(&ps);

        result = OCInit(NULL, port, OC_SERVER);
        if (result != GW_RESULT_OK)
        {
            OIC_LOG(ERROR, TAG,"OCStack init error");
            return result;
        }

        std::unique_ptr<WorkQueue<std::unique_ptr<IotivityWorkItem>>> q =
            std::unique_ptr<WorkQueue<std::unique_ptr<IotivityWorkItem>>>
            (new WorkQueue<std::unique_ptr<IotivityWorkItem>>());

        iotivityUtils =  make_unique<ConcurrentIotivityUtils>(std::move(q));
        iotivityUtils->startWorkerThreads();

        OCPlatformInfo platform_info;
        if (set_platform_info_params(platform_info) != GW_RESULT_OK)
        {
            OIC_LOG(ERROR, TAG,"Platform info setting failed locally!");
            return result;
        }

        if (OCSetPlatformInfo(platform_info) != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG,"Platform Registration failed!");
            return result;
        }

        OCDeviceInfo device_info = {NULL, NULL, NULL, NULL};
        if (set_device_info_params(deviceName, resourceType, device_info) != GW_RESULT_OK)
        {
            OIC_LOG(ERROR, TAG,"Device info setting failed locally!");
            return result;
        }

        if (OCSetDeviceInfo(device_info) != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG,"Device Registration failed!");
            return result;
        }
        // Iotivity does not take ownership of 'types' and clones itself a copy.
        OCFreeOCStringLL(device_info.types);
    }
    return GW_RESULT_OK;
}

void* processMessageFromPipeThreadProc(void *arg)
{
    common_plugin_ctx_t *ctx = (common_plugin_ctx_t *)arg;
    while(true)
    {
        bool result = processMessagesFromMPM(ctx->child_reads_fds.read_fd, ctx, g_plugin_context);
        if (result != GW_RESULT_OK)
        {
            ctx->exit_process_loop = true;
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            OIC_LOG(ERROR, TAG,"Error on process message from the pipe ");
            usleep(200000);
            break;
        }
    }
    pthread_exit(NULL);
    return NULL;
}


/*
 * this function is a blocking function that hold the iotivity processing
 * loop. this OIC server relies strictly on the "C" layer of the OIC stack.
 * This is common code for all plugins.
 *
 * Parameters:
 * ctx (in) - the context structure used by this implementation
 *
 * Returns:
 *     GW_RESULT_OK             - no errors
 *     GW_RESULT_INTERNAL_ERROR - maintenance failure
 */
GW_RESULT maintain_oic_infrastructure(GW_RESULT result, common_plugin_ctx_t *ctx)
{
    mpm_pipe_message_t pipe_message;
    void *res;

    if (ctx != NULL)
    {
        /* child tells status to parent prior to getting in the
         * processing loop
         */
        if (result == GW_RESULT_OK)
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

        if (result == GW_RESULT_OK)
        {
            pthread_mutex_init(&plugin_server_mutex,NULL);
            pthread_create(&processMessageFromPipeThread,NULL,processMessageFromPipeThreadProc,ctx);
            pthread_join(processMessageFromPipeThread, &res);
            pthread_mutex_destroy(&plugin_server_mutex);
        }
        else
        {
            return result;
        }

        OIC_LOG_V(INFO, TAG,"Stopping %s plugin", g_plugin_context->device_name);

        result = plugin_stop(g_plugin_context);

        if (result != GW_RESULT_OK)
        {
            OIC_LOG_V(ERROR, TAG,"Error(%d) stopping plugin %s", result, g_plugin_context->device_name);
        }

        result = plugin_destroy(g_plugin_context);
        if (result != GW_RESULT_OK)
        {
            OIC_LOG_V(ERROR, TAG,"Error(%d) stopping plugin %s", result, g_plugin_context->device_name);
        }

        iotivityUtils->stopWorkerThreads();

        result = OCStop();

        if (result != GW_RESULT_OK)
        {
            OIC_LOG_V(ERROR, TAG,"Error(%d) stopping Iotivity", result);
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG,"Bad context handed to maintain oic infrastructure");
    }
    return (result);
}

GW_RESULT hlpr_ExtractFiltersFromQuery(char *query, char **filterOne, char **filterTwo)
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
        return GW_RESULT_OK;
    }

    char *keyValuePair = strtok_r(query, OC_QUERY_SEPARATOR, &restOfQuery);

    while (keyValuePair)
    {
        if (numKeyValuePairsParsed >= 2)
        {
            OIC_LOG(ERROR, TAG,"More than 2 queries params in URI.");
            return GW_RESULT_INVALID_PARAMETER;
        }

        key = strtok_r(keyValuePair, OC_KEY_VALUE_DELIMITER, &value);

        if (!key || !value)
        {
            return GW_RESULT_INVALID_PARAMETER;
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
            OIC_LOG_V(ERROR, TAG,"Unsupported query key: %s", key);
            return GW_RESULT_INVALID_PARAMETER;
        }
        ++numKeyValuePairsParsed;

        keyValuePair = strtok_r(NULL, OC_QUERY_SEPARATOR, &restOfQuery);
    }
    if (*filterOne != NULL || *filterTwo != NULL)
    {
        OIC_LOG_V(ERROR, TAG,"Extracted params if: %s and rt: %s.", *filterOne, *filterTwo);
    }

    return GW_RESULT_OK;
}
