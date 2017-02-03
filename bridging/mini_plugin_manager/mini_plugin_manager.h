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

/* This file contains the "C" plugin interface definition.  This header file
 * and its corresponding implementation file is intended to be shared among
 * ALL plugins.  Modification of this file is not necessary for the construction
 * of a new plugin.
 */

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include "gw_error_code.h"
#include <stdbool.h>

#ifndef __MINI_PLUGIN_MANAGER_H__
#define __MINI_PLUGIN_MANAGER_H__


/*******************************************************************************
 * defines go here
 ******************************************************************************/

#define MAX_SHARED_OBJECT_NAME_LENGTH  300
#define MAX_SHARED_OBJECTS_LOADED      20
#define MAX_FILE_NAME_LENGTH           300
/*******************************************************************************
 * type defines and structure definitions go here
 ******************************************************************************/

typedef void * MPMPluginHandle;
typedef void * MPMMessage;

typedef GW_CB_RESULT (* MPMCallback)(uint32_t msgType, MPMMessage message, size_t size, const char * plugin_name);

/* This method sends a scan request to the plugin
 * input -
 *     pluginHandle - plugin handle for the specific plugin
 *
 *     msg          - scan request to be passed from the client to
 *                    the mini_plugin_manager
 *     size         - size of the message
 * returns:
 *     GW_RESULT_OK             - no errors
 *     ERROR CODES defined in gw_error_code.h in case of error
*/
GW_RESULT MPMScan(MPMPluginHandle pluginHandle, MPMMessage message, size_t size);

/* This method sends a add device request to the plugin
 * input -
 *     pluginHandle - plugin handle for the specific plugin
 *
 *     msg          - add request to be passed from the client to
 *                    the mini_plugin_manager
 *     size         - size of the message
 * returns:
 *     GW_RESULT_OK             - no errors
 *     ERROR CODES defined in gw_error_code.h in case of error
*/
GW_RESULT MPMAddDevice(MPMPluginHandle pluginHandle, MPMMessage message, size_t size);

/* This method sends a reconnect request to the plugin
 * input -
 *     pluginHandle - plugin handle for the specific plugin
 *
 *     msg          - add request to be passed from the client to
 *                    the mini_plugin_manager
 *     size         - size of the message
 * returns:
 * TODO:
 *     GW_RESULT_OK             - no errors
 *     ERROR CODES defined in gw_error_code.h in case of error
*/
GW_RESULT MPMReconnectDevice(MPMPluginHandle pluginHandle, MPMMessage message, size_t size);

/* This method sends a remove device request to the plugin
 * input -
 *     pluginHandle - plugin handle for the specific plugin
 *
 *     msg          - remove request to be passed from the client to
 *                    the mini_plugin_manager
 *     size         - size of the message
 * returns:
 *     GW_RESULT_OK             - no errors
 *     ERROR CODES defined in gw_error_code.h in case of error
*/

GW_RESULT MPMRemoveDevice(MPMPluginHandle pluginHandle, MPMMessage message, size_t size);

/* This is MPM library API to load the Plugins. It performs
 * following functionality as:
 * 1. Allocates memory to plugin_ctx
 * 2. Opens shared library
 * 3. Resolves the function symbols
 * 4. Calls create() and start() for the plugins
 *
 * input -
 *     pluginHandle - plugin Handle to be handed back to
 *                  mpm_client application
 *     pluginName - Name of the shared library to be loaded.
 *
 * returns:
 *     GW_RESULT_OK             - no errors
 *     ERROR CODES defined in gw_error_code.h in case of error
 */
GW_RESULT MPMLoad(MPMPluginHandle *pluginHandle, const char *pluginName, MPMCallback callback, const char *filename);

/* This is MPM library API to unload the Plugins. It performs
 * following functionality as:
 * 1. Calls stop() and destroy() for the plugin
 * 2. Closes shared library
 * 3. deallocates memory allocated to plugin_ctx
 *
 * input -
 *     pluginHandle - plugin handle sent by the
 *                  mpm_client application
 *
 * returns:
 *     GW_RESULT_OK             - no errors
 *     GW_RESULT_INTERNAL_ERROR - stack process error
 */
GW_RESULT MPMUnload(MPMPluginHandle pluginHandle);

#endif /* __MINI_PLUGIN_MANAGER_H__ */
