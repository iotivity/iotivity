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

/* This file contains the interface between the plugin's OIC server and the
 * plugin specific implmentation
 */

#ifndef __PLUGIN_SERVER_H__
#define __PLUGIN_SERVER_H__

#include "message_handler.h"
#include "plugin_if.h"
#include "gw_error_code.h"
#include "ocstack.h"
#include <stdbool.h>

/* This resource implementation has a work queue.  The work queue types are
 * defined here.
 */
#define WORK_ITEM_START_PRESENCE     1
#define WORK_ITEM_CREATE_RESOURCE    2
#define WORK_ITEM_DESTROY_RESOURCE   3
#define WORK_ITEM_STOP_PRESENCE      4
#define WORK_ITEM_NOTIFY_OBSERVERS   5
#define WORK_ITEM_DO_RESPONSE        6
#define WORK_ITEM_ADD_RESOURCE_TYPE  7
#define WORK_ITEM_ADD_INTERFACE      8

#define THREAD_PROCESS_SLEEPTIME     5

/* Plugin context is owned by the plugin specific content provider (you).
 */
typedef struct plugin_ctx_tag
{
    /* This plugin specific content provider has chosen to use a worker thread
     * to manage all the devices.  Keeping track if the thread is started.
     */
    bool started;

    /* The thread is staying around for the lifetime of the plugin instance.
     * This state variable is required to tell the thread leave the thread
     * function when that time comes.
     */
    bool stay_in_process_loop;

    FILE *(*open)(const char *path, const char *mode);

    /**
     * The name of the plugin being started. This name must follow OCF specification
     * for '/oic/d' device name.
     */
    char *device_name;
    char reconnect_file_name[MAX_FILE_NAME_LENGTH];
    /**
     * The name of the plugin being started. This name must follow OCF specification
     * for '/oic/d' device name.
     */
    char *resource_type;

    /* Save the thread handle so there is a possibility to wait on thread
     * handles for synchronization purposes.
     */
    pthread_t thread_handle;
} plugin_ctx_t;

/* The intention here is create a queue-able structure for a work item queue
 * The most important field is the first field which denotes the the work item
 * type.  The other field's relevancy depends on the work_id value.
 */
typedef struct work_item_tag work_item_t;
struct work_item_tag
{
    OCResourceHandle *handle;
    uint32_t work_id;
    char uri[MAX_URI_LENGTH];
    char resource_type[MAX_URI_LENGTH];
    char resource_interface_name[MAX_URI_LENGTH];
    OCEntityHandler entity_handler;
    uint8_t res_property;
    OCEntityHandlerResponse response;
};

//Initialize work queues, resource list and mutexes.
GW_RESULT server_init();

//Destroy work queues, resource list and mutexes.
GW_RESULT server_deinit();

GW_RESULT plugin_process();

/* These functions are called by the by the plugin's own Iotivity server which is
 * common code for all plugins.  There is one Iotivity server per plugin.  The APIs
 * shown below permit the plugin to initialize and to allow the plugin specific
 * code to call other Iotivity csdk APIs.  The implementation of these functions is found
 * in the plugin specific code.
 */
GW_RESULT plugin_create(plugin_ctx_t **plugin_specific_ctx);

GW_RESULT plugin_start(plugin_ctx_t *plugin_specific_ctx);

GW_RESULT plugin_stop(plugin_ctx_t *plugin_specific_ctx);

GW_RESULT plugin_destroy(plugin_ctx_t *plugin_specific_ctx);

GW_RESULT plugin_scan(plugin_ctx_t *ctx, mpm_pipe_message_t * message);

GW_RESULT plugin_add(plugin_ctx_t *ctx, mpm_pipe_message_t * message);

GW_RESULT plugin_remove(plugin_ctx_t *ctx, mpm_pipe_message_t * message);

GW_RESULT plugin_reconnect(plugin_ctx_t *ctx, mpm_pipe_message_t * message);

void plugin_specific_process(void);

void request_handler(mpm_pipe_message_t *message, plugin_ctx_t *ctx);


/*
 * Stolen from: IOTIVITY/resource/csdk/stack/src/resource.c:ExtractFiltersFromQuery()
 *
 * Function will extract 0, 1 or 2 filters from query.
 * More than 2 filters or unsupported filters will result in error.
 * If both filters are of the same supported type, the 2nd one will be picked.
 * Resource and device filters in the SAME query are NOT validated
 * and resources will likely not clear filters.
 */
GW_RESULT hlpr_ExtractFiltersFromQuery(char *query, char **filterOne, char **filterTwo);

#endif /* __PLUGIN_SERVER_H__ */
