//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

/**
 * @file
 *
 * This file contains APIs for PIPlugin module to be implemented.
 */

#ifndef PLUGININTERFACE_H_
#define PLUGININTERFACE_H_

#include "plugintypes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Todo: The following APIs can be modified as needed. This just a loose
// declaration to illustrate how we can manage our plugins. The types are
// defined in plugintypes.h.

/**
 *
 * Makes any required calls to instantiate IoTivity and/or plugin's radio.
 *
 * @param[in]  pluginType The type of plugin to start.
 * @param[out] plugin The plugin handle that will be started.
 *   Note: Please note that the plugin will need to be managed in the
 *         application space.
 */
OCStackResult PIStartPlugin(PIPluginType pluginType, PIPluginBase ** plugin);

/**
 *
 * Makes any required calls to stop plugin's radio.
 *
 * @param[in] PIStopPlugin The plugin to be stopped.
 *           Note: If NULL Makes any required calls to stop IoTivity and ALL
 *                 plugin radios.
 */
OCStackResult PIStopPlugin(PIPluginBase * plugin);

/**
 *
 * Called in main loop of application. Gives cycles for Plugin Interface'
 * internal operation.
 *
 */
OCStackResult PIProcess(PIPluginBase * plugin);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* PLUGININTERFACE_H_ */
