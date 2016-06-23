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

#include "UnsupportiveResource.h"
#include "CommonUtil.h"

#define STARTING_PORT 33353

#define DUMMY_RESOURCE_URI "/device/dummy"
#define RESOURCE_TYPE_DUMMY "core.dummy"


int main()
{

    UnsupportiveResource dummyResource;

    OCStackResult result = OC_STACK_OK;
//    string serverIp = CommonUtil::getLocalIP(IPv4);
    string serverIp = "0.0.0.0";

    result = dummyResource.constructServer(serverIp, STARTING_PORT + 0);
    result = (OCStackResult) ((int) result
            + (int) dummyResource.setResourceProperties(DUMMY_RESOURCE_URI,
                    RESOURCE_TYPE_DUMMY, DEFAULT_INTERFACE));
    result = (OCStackResult) ((int) result + (int) dummyResource.startServer());

    if (result != OC_STACK_OK)
    {
        IOTIVITYTEST_LOG(ERROR, "Unable to start unsupportive server");
        return 1;
    }

    IOTIVITYTEST_LOG(INFO, "Press any key and the enter to stop the servers :-)....");
    char n;
    std::cin >> n;

    dummyResource.stopServer();

    return 0;
}
