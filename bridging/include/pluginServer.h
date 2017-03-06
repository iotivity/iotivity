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

/* This file contains the interface between the plugin's OCF server and the
 * plugin specific implmentation
 */

#ifndef __PLUGINSERVER_H__
#define __PLUGINSERVER_H__

#include "messageHandler.h"
#include "pluginIf.h"
#include "mpmErrorCode.h"
#include "ocstack.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MPM_THREAD_PROCESS_SLEEPTIME     5

/**
 *  Plugin context is owned by the plugin specific content provider (you).
 */
struct MPMPluginCtx
{
    /**
     * This plugin specific content provider has chosen to use a worker thread
     * to manage all the devices.  Keeping track if the thread is started.
     */
    bool started;

    /**
     * The thread is staying around for the lifetime of the plugin instance.
     * This state variable is required to tell the thread leave the thread
     * function when that time comes.
     */
    bool stay_in_process_loop;

    FILE *(*open)(const char *path, const char *mode);

    /**
     * The name of the plugin being started. This name must follow OCF specification
     * for '/oic/d' device name.
     */
    const char *device_name;
    char reconnect_file_name[MPM_MAX_FILE_NAME_LENGTH];

    /**
     * The resource type with device of the plugin being started. This name must follow OCF specification
     * for /oic/d. Some examples would be 'oic.d.light' and 'oic.d.smartlock'.
     */
    const char *resource_type;

    /**
     *  Save the thread handle so there is a possibility to wait on thread
     * handles for synchronization purposes.
     */
    pthread_t thread_handle;
};

/**
 * These functions are called by the plugin's own Iotivity server which is
 * common code for all plugins. There is one Iotivity server per plugin. The APIs
 * shown below permit the plugin to initialize and to allow the plugin specific
 * code to call other Iotivity csdk APIs. The implementation of these functions is found
 * in the plugin specific code.
 *
 * @param[in] plugin_specific_ctx            Plugin specific context
 * @param[in] message                        Message received from the client via mpm library
 *
 * @return MPM_RESULT_OK if no error, error specific value if any error occurs
 */
MPMResult pluginCreate(MPMPluginCtx **plugin_specific_ctx);

MPMResult pluginStart(MPMPluginCtx *plugin_specific_ctx);

MPMResult pluginStop(MPMPluginCtx *plugin_specific_ctx);

MPMResult pluginDestroy(MPMPluginCtx *plugin_specific_ctx);

MPMResult pluginScan(MPMPluginCtx *ctx, MPMPipeMessage *message);

MPMResult pluginAdd(MPMPluginCtx *ctx, MPMPipeMessage *message);

MPMResult pluginRemove(MPMPluginCtx *ctx, MPMPipeMessage *message);

MPMResult pluginReconnect(MPMPluginCtx *ctx, MPMPipeMessage *message);

/**
 * This function is added for Zigbee and kept as empty function
 * for all other plugins to avoid using ifdef(s)
 */
void MPMPluginSpecificProcess(void);

/**
 * This function handles the requests from mpm library to plugin
 * @param[in] message        The message to received over the pipe from the mpm library
 * @param[in] ctx            Context of the plugin to which the message has to be forwarded to
 */
void MPMRequestHandler(MPMPipeMessage *message, MPMPluginCtx *ctx);


/**
 * Stolen from: IOTIVITY/resource/csdk/stack/src/resource.c:ExtractFiltersFromQuery()
 *
 * Function will extract 0, 1 or 2 filters from query.
 * More than 2 filters or unsupported filters will result in error.
 * If both filters are of the same supported type, the 2nd one will be picked.
 * Resource and device filters in the SAME query are NOT validated
 * and resources will likely not clear filters.
 *
 * @param[in] query            The query string from which filters are to be extracted
 * @param[in,out] filterOne    The first filter string
 * @param[in,out] filterTwo    The second filter string
 *
 * @return MPM_RESULT_OK if no error, MPM_RESULT_INVALID_PARAMETER if any error
 */
MPMResult MPMExtractFiltersFromQuery(char *query, char **filterOne, char **filterTwo);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif /* __PLUGIN_SERVER_H__ */
