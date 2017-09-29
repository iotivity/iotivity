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
#include <assert.h>
#include <pluginServer.h>
#include "experimental/logger.h"

#define TAG "STUB_PLUGIN"

MPMPluginCtx *g_plugin_ctx = NULL;

FILE *sec_file(const char *, const char *mode)
{
    std::string filename = "sample_" + path;
    return fopen(filename.c_str(), mode);
}

MPMResult pluginCreate(MPMPluginCtx **pluginSpecificCtx)
{
    MPMPluginCtx *ctx = (MPMPluginCtx *) calloc(1, sizeof(MPMPluginCtx));
    if (ctx == NULL)
    {
        OIC_LOG(ERROR, TAG, "Allocation of plugin context failed");
        return MPM_RESULT_INTERNAL_ERROR;
    }
    *pluginSpecificCtx = ctx;
    g_plugin_ctx = ctx;
    ctx->device_name = "Stub plugin";
    ctx->resource_type = "oic.d.stub";
    ctx->open = sec_file;
    return MPM_RESULT_OK;
}

MPMResult pluginStart(MPMPluginCtx *ctx)
{
    ctx->stay_in_process_loop = true;
    OIC_LOG(INFO, TAG, "Plugin start called!");
    return MPM_RESULT_OK;
}

void echoResponse(MPMPipeMessage *message, std::string type)
{
    std::string s = type + " response echo";
    MPMSendResponse(s.c_str(), s.size(), message->msgType);
}

MPMResult pluginScan(MPMPluginCtx *, MPMPipeMessage *message)
{
    OIC_LOG(INFO, TAG, "Scan called!");
    // Send back scan response to the client.
    echoResponse(message, "SCAN");
    return MPM_RESULT_OK;
}

MPMResult pluginAdd(MPMPluginCtx *, MPMPipeMessage *message)
{
    OIC_LOG(INFO, TAG, "Add called! Create Iotivity resources here based on what the client says");
    echoResponse(message, "ADD");
    return MPM_RESULT_OK;
}

MPMResult pluginRemove(MPMPluginCtx *, MPMPipeMessage *message)
{
    OIC_LOG(INFO, TAG, "Remove called! Remove iotivity resources here based on what the client says");
    echoResponse(message, "REMOVE");
    return MPM_RESULT_OK;
}

MPMResult pluginReconnect(MPMPluginCtx *, MPMPipeMessage *message)
{
    OIC_LOG(INFO, TAG,
            "Reconnect called! Reconnect to devices, create resources from the message/cloud/db/file.");
    echoResponse(message, "ADD");
    return MPM_RESULT_OK;
}

MPMResult pluginStop(MPMPluginCtx *)
{
    OIC_LOG(INFO, TAG, "Stop called !");
    return MPM_RESULT_OK;
}


MPMResult pluginDestroy(MPMPluginCtx *pluginSpecificCtx)
{
    OIC_LOG(INFO, TAG, "Destroy called");
    if (!pluginSpecificCtx)
    {
        assert(g_plugin_ctx);

        if (pluginSpecificCtx->started)
        {
            pluginStop(pluginSpecificCtx);
        }
        // freeing the resource allocated in create
        free(pluginSpecificCtx);
        g_plugin_ctx = NULL;
    }

    return (MPM_RESULT_OK);
}
