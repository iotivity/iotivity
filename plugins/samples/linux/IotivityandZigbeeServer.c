//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "IotivityandZigbeeServer.h"
#include <signal.h>
#include <ocstack.h>
#include <logger.h>

#define TAG "IoTivityZigbeeServer"

int main(int argc, char* argv[])
{
    OCStackResult result;
    OC_LOG(INFO, TAG, "Initializing IoTivity...");

    // Initialize Stack
    result = OCInit(NULL, 0, OC_SERVER);
    if (result != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, "OCInit Failed %d", result);
        return -1;
    }

    // Set Platform info
    result = SetPlatformInfo();
    if (result != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, "SetPlatformInfo Failed %d", result);
        goto IotivityStop;
    }

    // Set Device Info
    result  = SetDeviceInfo();
    if (result != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, "SetPlatformInfo Failed: %d", result);
        goto IotivityStop;
    }
    // Start Presence
    result  = OCStartPresence(0);
    if (result != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, "OCStartPresence Failed: %d", result);
        goto IotivityStop;
    }

    // PIStartPlugin
    PIPluginBase* plugin = NULL;
    OC_LOG(INFO, TAG, "IoTivity Initialized properly, Starting Zigbee Plugin...");
    result = PIStartPlugin(PLUGIN_ZIGBEE, &plugin);
    if (result != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, "Zigbee Plugin Start Failed: %d", result);
        goto IotivityStop;
    }

    if (signal(SIGINT, processCancel) == SIG_ERR)
    {
        OC_LOG(ERROR, TAG, "Unable to catch SIGINT, terminating...");
    }
    else
    {
        OC_LOG(INFO, TAG, "Zigbee Plugin started correctly, press Ctrl-C to terminate application");
        // Loop until sigint
        while (!processSignal(false) && result == OC_STACK_OK)
        {
            result = OCProcess();
            if (result != OC_STACK_OK)
            {
                OC_LOG_V(ERROR, TAG, "OCProcess Failed: %d", result);
                break;
            }

            result = PIProcess(plugin);
            if (result != OC_STACK_OK)
            {
                OC_LOG_V(ERROR, TAG, "PIProcess Failed: %d", result);
            }
        }
    }

    OC_LOG(INFO, TAG, "Stopping Zigbee Plugin...");
    // PIStopPlugin
    OC_LOG(INFO, TAG, "Zigbee Plugin Stopped");
    result = PIStopPlugin(plugin);
    if (result != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, "Zigbee Plugin Stop Failed: %d", result);
    }

    // OCStop
IotivityStop:
    OC_LOG(INFO, TAG, "Stopping IoTivity...");
    result = OCStop();
    if (result != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, "OCStop Failed: %d", result);
        return 0;
    }

    OC_LOG(INFO, TAG, "Application Completed Successfully");
}

OCStackResult SetPlatformInfo()
{
    static const OCPlatformInfo platformInfo =
        {
            .platformID = "IoTivityZigbeeID",
            .manufacturerName = "IoTivity",
            .manufacturerUrl = "http://iotivity.org",
            .modelNumber = "T1000",
            .dateOfManufacture = "January 14th, 2015",
            .platformVersion = "0.9.2",
            .operatingSystemVersion = "7",
            .hardwareVersion = "0.5",
            .firmwareVersion = "0",
            .supportUrl = "http://iotivity.org",
            .systemTime = ""
        };

    return OCSetPlatformInfo(platformInfo);
}

OCStackResult SetDeviceInfo()
{
    static const OCDeviceInfo deviceInfo =
        {
            .deviceName = "IoTivity/Zigbee Server Sample"
        };

    return OCSetDeviceInfo(deviceInfo);
}

bool processSignal(bool set)
{
    static sig_atomic_t signal = 0;
    if (set)
    {
        signal = 1;
    }

    return signal == 1;
}

void processCancel(int signal)
{
    if(signal == SIGINT)
    {
        processSignal(true);
    }
}

