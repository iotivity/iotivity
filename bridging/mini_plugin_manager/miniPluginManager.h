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
#include "mpmErrorCode.h"
#include <stdbool.h>

#ifndef __MINI_PLUGIN_MANAGER_H__
#define __MINI_PLUGIN_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
 * defines go here
 ******************************************************************************/

#define MAX_SHARED_OBJECT_NAME_LENGTH  300
#define MAX_SHARED_OBJECTS_LOADED      20

/*******************************************************************************
 * type defines and structure definitions go here
 ******************************************************************************/

typedef void *MPMPluginHandle;
typedef void *MPMMessage;

/** MPMCallback - for receiving the responses from the plugin */
typedef MPMCbResult (* MPMCallback)(uint32_t msgType, MPMMessage message, size_t size,
                                    const char *pluginName);

/**
 * This method sends a scan request to the plugin
 * @paran[in] pluginHandle            Plugin handle for the specific plugin
 * @param[in] message                 Scan request to be passed from the client to
 *                                    the mini_plugin_manager
 * @param[in] size                    Size of the message
 *
 * @return MPM_RESULT_OK if no errors, else ERROR CODES defined in mpmErrorCode.h in case of error
*/
MPMResult MPMScan(MPMPluginHandle pluginHandle, MPMMessage message, size_t size);

/**
 * This method sends a add device request to the plugin
 * @param[in] pluginHandle            Plugin handle for the specific plugin
 * @param[in] message                 Add request to be passed from the client to
 *                                    the mini_plugin_manager
 * @param[in] size                    Size of the message
 *
 * @return MPM_RESULT_OK if no error, else ERROR CODES defined in mpmErrorCode.h in case of error
*/
MPMResult MPMAddDevice(MPMPluginHandle pluginHandle, MPMMessage message, size_t size);

/**
 * This method sends a reconnect request to the plugin
 * @param[in] pluginHandle            Plugin handle for the specific plugin
 * @param[in] message                 Request request to be passed from the client to
 *                                    the mini_plugin_manager
 * @param[in] size                    Size of the message
 *
 * @return MPM_RESULT_OK if no error, else ERROR CODES defined in mpmErrorCode.h in case of error
*/
MPMResult MPMReconnectDevice(MPMPluginHandle pluginHandle, MPMMessage message, size_t size);

/**
 * This method sends a remove device request to the plugin
 * @param[in] pluginHandle            Plugin handle for the specific plugin
 * @param[in] message                 Remove request to be passed from the client to
 *                                    the mini_plugin_manager
 * @param[in] size                    Size of the message
 *
 * @return MPM_RESULT_OK if no error, else ERROR CODES defined in mpmErrorCode.h in case of error
*/
MPMResult MPMRemoveDevice(MPMPluginHandle pluginHandle, MPMMessage message, size_t size);

/**
 * This is MPM library API to load the Plugins. It performs
 * following functionality as:
 * 1. Allocates memory to plugin_ctx
 * 2. Opens shared library
 * 3. Resolves the function symbols
 * 4. Calls create() and start() for the plugins
 *
 * @param[out] pluginHandle            Plugin Handle to be handed back to
 *                                     mpm_client application
 * @param[in] pluginName               Name of the shared library to be loaded.
 * @param[in] callback                 Callback function to be invoked upon SARR responses from
 *                                     plugin to the client
 * @param[in] filename                 Reconnect filename each plugin would maintain, currently not used
 *
 * @return MPM_RESULT_OK if no errors, else ERROR CODES defined in mpmErrorCode.h in case of error
 */
MPMResult MPMLoad(MPMPluginHandle *pluginHandle, const char *pluginName, MPMCallback callback,
                  const char *filename);

/**
 * This is MPM library API to unload the Plugins. It performs
 * following functionality as:
 * 1. Calls stop() and destroy() for the plugin
 * 2. Closes shared library
 * 3. deallocates memory allocated to plugin_ctx
 *
 * @param[in] pluginHandle            Plugin handle sent by the
 *                                    mpm_client application
 *
 * @return MPM_RESULT_OK if no errors, MPM_RESULT_INTERNAL_ERROR if stack process error
 */
MPMResult MPMUnload(MPMPluginHandle pluginHandle);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif /* __MINI_PLUGIN_MANAGER_H__ */
