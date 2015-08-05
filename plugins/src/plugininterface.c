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
 * This file contains APIs for Plugin Interface module to be implemented.
 */

#include "plugininterface.h"
#include "plugininterfaceinternal.h"
#include "plugintranslatortypes.h"

// Internal Note: This API will try to start IoTivity. If it is already
//                started, nothing will occur. The IoTivity stack will not
//                allow another instance to occur within the same program space
//                and will even return a soft success when we try (ie.
//                OC_STACK_OK).
OCStackResult PIStartPlugin(PIPluginType pluginType, PIPluginBase** plugin)
{
    return OC_STACK_NOTIMPL;
}

OCStackResult PIStopPlugin(PIPluginBase * plugin)
{
    return OC_STACK_NOTIMPL;
}

OCStackResult PIProcess(PIPluginBase * plugin)
{
    OCStackResult result = OC_STACK_OK;
    if(!plugin)
    {
        return OC_STACK_INVALID_PARAM;
    }
    if(plugin->type == PLUGIN_ZIGBEE)
    {
        result = ProcessZigbee((PIPlugin_Zigbee *) plugin);
        if(result != OC_STACK_OK)
        {
            return result;
        }
    }
    else
    {
        return OC_STACK_ERROR;
    }
    return result;
}

OCStackResult ProcessZigbee(PIPlugin_Zigbee * plugin)
{
    return OC_STACK_OK;
}
