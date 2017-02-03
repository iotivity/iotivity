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
#include <set>
#include <plugin_server.h>
#include "logger.h"

#define TAG "STUB_PLUGIN"

plugin_ctx_t *g_plugin_ctx = NULL;
static char CRED_FILE[] = "./oic_svr_db_sample.dat";

FILE *sec_file(const char *path, const char *mode)
{
    (void) path;
    return fopen(CRED_FILE, mode);
}

GW_RESULT plugin_create(plugin_ctx_t **pluginSpecificCtx)
{
    plugin_ctx_t *ctx = (plugin_ctx_t *) calloc(1, sizeof(plugin_ctx_t));
    *pluginSpecificCtx = ctx;
    g_plugin_ctx = ctx;
    ctx->device_name = (char*) "Stub plugin";
    ctx->resource_type = (char*) "oic.d.stub";
    ctx->open = sec_file;
    return GW_RESULT_OK;
}

GW_RESULT plugin_start(plugin_ctx_t *ctx)
{
    ctx->stay_in_process_loop = true;
    OIC_LOG(INFO, TAG, "Plugin start called!");
    return GW_RESULT_OK;
}

void echo_response(mpm_pipe_message_t *message, std::string type)
{
    std::string s = type + " response echo";
    unsigned long size = s.size() + 1;
    char *m = (char*) malloc(size);
    memcpy(m, s.c_str(), size);
    send_response(m, size, message->msgType);
    free(m);
}
GW_RESULT plugin_scan(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx;
    OIC_LOG(INFO, TAG, "Scan called!");
    // Send back scan response to the client.
    echo_response(message, "SCAN");
    return GW_RESULT_OK;
}

GW_RESULT plugin_add(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx;
    OIC_LOG(INFO, TAG, "Add called! Create Iotivity resources here based on what the client says");
    echo_response(message, "ADD");
    return GW_RESULT_OK;
}

GW_RESULT plugin_remove(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx;
    OIC_LOG(INFO, TAG, "Remove called! Remove iotivity resources here based on what the client says");
    echo_response(message, "REMOVE");
    return GW_RESULT_OK;
}

GW_RESULT plugin_reconnect(plugin_ctx_t *ctx, mpm_pipe_message_t * message)
{
    (void) ctx;
    OIC_LOG(INFO, TAG, "Reconnect called! Reconnect to devices, create resources from the message/cloud/db/file.");
    echo_response(message, "ADD");
    return GW_RESULT_OK;
}

GW_RESULT plugin_stop(plugin_ctx_t *pluginSpecificCtx)
{
    (void) pluginSpecificCtx;
    OIC_LOG(INFO, TAG, "Stop called !");
    return GW_RESULT_OK;
}


GW_RESULT plugin_destroy(plugin_ctx_t *pluginSpecificCtx)
{
    OIC_LOG(INFO, TAG, "Destroy called");
    if (pluginSpecificCtx != NULL && g_plugin_ctx != NULL)
    {
        if (pluginSpecificCtx->started)
        {
            plugin_stop(pluginSpecificCtx);
        }
        // freeing the resource allocated in create
        free(pluginSpecificCtx);
        g_plugin_ctx = NULL;
    }

    return (GW_RESULT_OK);
}
