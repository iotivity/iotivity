//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "iotivity_config.h"
#include <iostream>
#include <sstream>
#include <limits>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "oic_string.h"
#include "rd_client.h"

#define DEFAULT_CONTEXT_VALUE 0x99

OCResourceHandle handles[2];
std::string rdAddress;
std::mutex mutex;
std::condition_variable cond;
int in = 0;

OCStackResult registerLocalResources()
{
    std::string resourceURI_thermostat = "/a/thermostat";
    std::string resourceTypeName_thermostat = "core.thermostat";
    std::string resourceURI_light = "/a/light";
    std::string resourceTypeName_light = "core.light";
    std::string resourceInterface = OC_RSRVD_INTERFACE_DEFAULT;
    uint8_t resourceProperty = OC_DISCOVERABLE;

    OCStackResult result = OCCreateResource(&handles[0],
                                           resourceTypeName_thermostat.c_str(),
                                           resourceInterface.c_str(),
                                           resourceURI_thermostat.c_str(),
                                           NULL,
                                           NULL,
                                           resourceProperty);

    if (OC_STACK_OK != result)
    {
        return result;
    }

    result = OCCreateResource(&handles[1],
                              resourceTypeName_light.c_str(),
                              resourceInterface.c_str(),
                              resourceURI_light.c_str(),
                              NULL,
                              NULL,
                              resourceProperty);

    return result;
}

void printHelp()
{
    std::cout << std::endl;
    std::cout << "********************************************" << std::endl;
    std::cout << "*  method Type :  1 - Discover RD          *" << std::endl;
    std::cout << "*  method Type :  2 - Publish              *" << std::endl;
    std::cout << "*  method Type :  3 - Update               *" << std::endl;
    std::cout << "*  method Type :  4 - Delete               *" << std::endl;
    std::cout << "*  method Type : 99 - Exit                 *" << std::endl;
    std::cout << "********************************************" << std::endl;
    std::cout << std::endl;
}

static OCStackApplicationResult handleDiscoveryCB(__attribute__((unused))void *ctx,
                                                  __attribute__((unused)) OCDoHandle handle,
                                                  __attribute__((unused))
                                                  OCClientResponse *clientResponse)
{
    std::cout << "Successfully found RD." << std::endl;
    std::ostringstream oss;
    oss << clientResponse->devAddr.addr << ":" << clientResponse->devAddr.port;
    rdAddress = oss.str();
    std::cout << "RD Address is : " <<  rdAddress << std::endl;
    return OC_STACK_DELETE_TRANSACTION;
}

static OCStackApplicationResult handlePublishCB(__attribute__((unused))void *ctx,
                                                  __attribute__((unused)) OCDoHandle handle,
                                                  __attribute__((unused))
                                                  OCClientResponse *clientResponse)
{
    std::cout << "Successfully published resources." << std::endl;
    return OC_STACK_DELETE_TRANSACTION;
}

static OCStackApplicationResult handleUpdateCB(__attribute__((unused))void *ctx,
                                                  __attribute__((unused)) OCDoHandle handle,
                                                  __attribute__((unused))
                                                  OCClientResponse *clientResponse)
{
    std::cout << "Successfully updated resources." << std::endl;
    return OC_STACK_DELETE_TRANSACTION;
}

static OCStackApplicationResult handleDeleteCB(__attribute__((unused))void *ctx,
                                                  __attribute__((unused)) OCDoHandle handle,
                                                  __attribute__((unused))
                                                  OCClientResponse *clientResponse)
{
    std::cout << "Successfully deleted resources." << std::endl;
    return OC_STACK_DELETE_TRANSACTION;
}

static void ocThread()
{
    OCStackResult result;
    bool run = true;
    while (run)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (cond.wait_for(lock, std::chrono::milliseconds(100)) == std::cv_status::no_timeout)
        {
            switch (in)
            {
                case 1:
                {
                    OCCallbackData cbData;
                    cbData.cb = &handleDiscoveryCB;
                    cbData.cd = NULL;
                    cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
                    result = OCRDDiscover(nullptr, CT_ADAPTER_IP, &cbData, OC_LOW_QOS);
                    if (OC_STACK_OK != result)
                    {
                        std::cout << "OCRDDiscover Failed " << result << std::endl;
                    }
                    break;
                }
                case 2:
                {
                    OCCallbackData cbData;
                    cbData.cb = &handlePublishCB;
                    cbData.cd = NULL;
                    cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
                    result = OCRDPublish(nullptr, rdAddress.c_str(), CT_ADAPTER_IP, handles,
                                2, OIC_RD_PUBLISH_TTL, &cbData, OC_LOW_QOS);
                    if (OC_STACK_OK != result)
                    {
                        std::cout << "OCRDPublish Failed " << result << std::endl;
                    }
                    break;
                }
                case 3:
                {
                    /* Update the TTL */
                    OCCallbackData cbData;
                    cbData.cb = &handleUpdateCB;
                    cbData.cd = NULL;
                    cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
                    result = OCRDPublish(nullptr, rdAddress.c_str(), CT_ADAPTER_IP, handles,
                                2, OIC_RD_PUBLISH_TTL, &cbData, OC_LOW_QOS);
                    if (OC_STACK_OK != result)
                    {
                        std::cout << "OCRDPublish Failed " << result << std::endl;
                    }
                    break;
                }
                case 4:
                {
                    /* Delete all resources */
                    OCCallbackData cbData;
                    cbData.cb = &handleDeleteCB;
                    cbData.cd = NULL;
                    cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
                    result = OCRDDelete(nullptr, rdAddress.c_str(), CT_ADAPTER_IP, NULL,
                                0, &cbData, OC_LOW_QOS);
                    if (OC_STACK_OK != result)
                    {
                        std::cout << "OCRDDelete Failed " << result << std::endl;
                    }
                    break;
                }
                case 99:
                    run = false;
                    break;
                default:
                    std::cout << "Invalid input, please try again" << std::endl;
                    break;
            }
            printHelp();
        }
        result = OCProcess();
        if (OC_STACK_OK != result)
        {
            std::cout << "OCProcess Failed " << result << std::endl;
            break;
        }
    }
}

int main()
{
    std::cout << "Initializing IoTivity Platform" << std::endl;
    OCStackResult result = OCInit(NULL, 0, OC_CLIENT_SERVER);
    if (OC_STACK_OK != result)
    {
        std::cout << "OCInit Failed " << result << std::endl;
        return -1;
    }
    result = OCStopMulticastServer();
    if (OC_STACK_OK != result)
    {
        std::cout << "OCStopMulticastServer Failed " << result << std::endl;
        return -1;
    }

    std::cout << "Created Platform..." << std::endl;
    result = registerLocalResources();
    if (OC_STACK_OK != result)
    {
        std::cout << "Could not create the resource " << result << std::endl;
        return -1;
    }

    printHelp();

    std::thread t(ocThread);

    while (1)
    {
        std::cin >> in;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input type, please try again" << std::endl;
            continue;
        }

        cond.notify_one();

        if (in == 99)
        {
            break;
        }
    }

    t.join();
    return 0;
}
