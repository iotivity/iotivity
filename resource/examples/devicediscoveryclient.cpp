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

///
///This sample demonstrates the device discovery feature
///The client queries for the device related information
///stored by the server.
///

#include <mutex>
#include <condition_variable>

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;

//Callback after device information is received
void receivedDeviceInfo(const OCRepresentation& rep)
{
    std::cout << "\nDevice Information received ---->\n";

    std::string contentType;
    std::string dateOfManufacture;
    std::string deviceName;
    std::string deviceUUID;
    std::string firmwareVersion;
    std::string hostName;
    std::string manufacturerName;
    std::string manufacturerUrl;
    std::string modelNumber;
    std::string platformVersion;
    std::string supportUrl;
    std::string version;

    if(rep.getValue("ct", contentType))
    {
        std::cout << "Content Type: " << contentType << std::endl;
    }

    if(rep.getValue("mndt", dateOfManufacture))
    {
        std::cout << "Date of manufacture: " << dateOfManufacture << std::endl;
    }

    if(rep.getValue("dn", deviceName))
    {
        std::cout << "Device Name: " << deviceName << std::endl;
    }

    if(rep.getValue("di", deviceUUID))
    {
        std::cout << "Device UUID: " << deviceUUID << std::endl;
    }

    if(rep.getValue("mnfv", firmwareVersion))
    {
        std::cout << "Firmware Version: " << firmwareVersion << std::endl;
    }

    if(rep.getValue("hn", hostName))
    {
        std::cout << "Host Name: " << hostName << std::endl;
    }

    if(rep.getValue("mnmn", manufacturerName))
    {
        std::cout << "Manufacturer Name: " << manufacturerName << std::endl;
    }

    if(rep.getValue("mnml", manufacturerUrl))
    {
        std::cout << "Manufacturer Url: " << manufacturerUrl << std::endl;
    }

    if(rep.getValue("mnmo", modelNumber))
    {
        std::cout << "Model No. : " << modelNumber << std::endl;
    }

    if(rep.getValue("mnpv", platformVersion))
    {
        std::cout << "Platform Version: " << platformVersion << std::endl;
    }

    if(rep.getValue("mnsl", supportUrl))
    {
        std::cout << "Support URL: " << supportUrl << std::endl;
    }

    if(rep.getValue("icv", version))
    {
        std::cout << "Version: " << version << std::endl;
    }
}

int main() {

    // Create PlatformConfig object
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Client,
        "0.0.0.0",
        0,
        OC::QualityOfService::LowQos
    };

    OCPlatform::Configure(cfg);
    try
    {
        OCPlatform::getDeviceInfo("", "coap://224.0.1.187/oc/core/d", &receivedDeviceInfo);
        std::cout<< "Querying for device information... " <<std::endl;

        // A condition variable will free the mutex it is given, then do a non-
        // intensive block until 'notify' is called on it.  In this case, since we
        // don't ever call cv.notify, this should be a non-processor intensive version
        // of while(true);
        std::mutex blocker;
        std::condition_variable cv;
        std::unique_lock<std::mutex> lock(blocker);
        cv.wait(lock);

    }catch(OCException& e)
    {
        //log(e.what());
    }

    return 0;
}

