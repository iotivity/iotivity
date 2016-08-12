/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include <vector>
#include <iostream>
#include <stdlib.h>

#include "SampleResource.h"
#include "CommonUtil.h"

#define STARTING_PORT 33333

#define LIGHT_1_URI "/device/light-1"
#define LIGHT_2_URI "/device/light-2"
#define LIGHT_3_URI "/device/light-3"
#define FAN_1_URI "/device/fan-1"
#define FAN_2_URI "/device/fan-2"
#define ROOM_1_URI "/store/room-1"
#define ROOM_2_URI "/store/room-2"
#define RESOURCE_TYPE_LIGHT "core.light"
#define RESOURCE_TYPE_FAN "core.fan"

int main()
{

    SampleResource lightResource1;
    SampleResource lightResource2;
    SampleResource lightResource3;
    SampleResource fanResource1;
    SampleResource fanResource2;

    OCStackResult result = OC_STACK_OK;
    string serverIp = "0.0.0.0";

    result = SampleResource::constructServer(serverIp, 0);
    result = (OCStackResult) ((int) result
            + (int) lightResource1.setResourceProperties(LIGHT_1_URI,
            RESOURCE_TYPE_LIGHT, DEFAULT_INTERFACE));
    result = (OCStackResult) ((int) result + (int) lightResource1.startServer());

    result = (OCStackResult) ((int) result
            + (int) lightResource2.setResourceProperties(LIGHT_2_URI,
            RESOURCE_TYPE_LIGHT, DEFAULT_INTERFACE));
    result = (OCStackResult) ((int) result + (int) lightResource2.startServer());

    result = (OCStackResult) ((int) result
            + (int) lightResource3.setResourceProperties(LIGHT_3_URI,
            RESOURCE_TYPE_LIGHT, DEFAULT_INTERFACE));
    result = (OCStackResult) ((int) result + (int) lightResource3.startServer());

    result = (OCStackResult) ((int) result
            + (int) fanResource1.setResourceProperties(FAN_1_URI, RESOURCE_TYPE_FAN,
                    DEFAULT_INTERFACE));
    result = (OCStackResult) ((int) result + (int) fanResource1.startServer());

    result = (OCStackResult) ((int) result
            + (int) fanResource2.setResourceProperties(FAN_2_URI, RESOURCE_TYPE_FAN,
                    DEFAULT_INTERFACE));
    result = (OCStackResult) ((int) result + (int) fanResource2.startServer());

    if (result != OC_STACK_OK)
    {
        IOTIVITYTEST_LOG(ERROR, "Unable to start servers");
        return 1;
    }

    IOTIVITYTEST_LOG(INFO, "Press any key and the enter to stop the servers :-)....");
    char n;
    std::cin >> n;

    lightResource1.stopServer();
    lightResource2.stopServer();
    lightResource3.stopServer();
    fanResource1.stopServer();
    fanResource2.stopServer();

    return 0;
}
