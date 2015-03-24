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
///The server sets the device related info. which can
///be later retrieved by a client.
///

#include <mutex>
#include <condition_variable>

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;

//Set of strings for each of deviceInfo fields
std::string contentType = "myContentType";
std::string dateOfManufacture = "myDateOfManufacture";
std::string deviceName = "myDeviceName";
std::string deviceUUID = "myDeviceUUID";
std::string firmwareVersion = "myFirmwareVersion";
std::string hostName = "myHostName";
std::string manufacturerName = "myManufacturerNa";
std::string manufacturerUrl = "myManufacturerUrl";
std::string modelNumber = "myModelNumber";
std::string platformVersion = "myPlatformVersion";
std::string supportUrl = "mySupportUrl";
std::string version = "myVersion";

//OCDeviceInfo Contains all the device info to be stored
OCDeviceInfo deviceInfo;

void DeleteDeviceInfo()
{
    delete[] deviceInfo.contentType;
    delete[] deviceInfo.dateOfManufacture;
    delete[] deviceInfo.deviceName;
    delete[] deviceInfo.deviceUUID;
    delete[] deviceInfo.firmwareVersion;
    delete[] deviceInfo.hostName;
    delete[] deviceInfo.manufacturerName;
    delete[] deviceInfo.manufacturerUrl;
    delete[] deviceInfo.modelNumber;
    delete[] deviceInfo.platformVersion;
    delete[] deviceInfo.supportUrl;
    delete[] deviceInfo.version;
}

void DuplicateString(char ** targetString, std::string sourceString)
{
    *targetString = new char[sourceString.length() + 1];
    strncpy(*targetString, sourceString.c_str(), (sourceString.length() + 1));
}

OCStackResult SetDeviceInfo(std::string contentType, std::string dateOfManufacture,
                std::string deviceName, std::string deviceUUID, std::string firmwareVersion,
                std::string hostName, std::string manufacturerName, std::string manufacturerUrl,
                std::string modelNumber, std::string platformVersion, std::string supportUrl,
                std::string version)
{
    if(manufacturerName.length() > MAX_MANUFACTURER_NAME_LENGTH)
    {
        return OC_STACK_INVALID_PARAM;

    }

    if(manufacturerUrl.length() > MAX_MANUFACTURER_URL_LENGTH)
    {
        return OC_STACK_INVALID_PARAM;

    }

    try
    {
        DuplicateString(&deviceInfo.contentType, contentType);
        DuplicateString(&deviceInfo.dateOfManufacture, dateOfManufacture);
        DuplicateString(&deviceInfo.deviceName, deviceName);
        DuplicateString(&deviceInfo.deviceUUID, deviceUUID);
        DuplicateString(&deviceInfo.firmwareVersion, firmwareVersion);
        DuplicateString(&deviceInfo.hostName, hostName);
        DuplicateString(&deviceInfo.manufacturerName, manufacturerName);
        DuplicateString(&deviceInfo.manufacturerUrl, manufacturerUrl);
        DuplicateString(&deviceInfo.modelNumber, modelNumber);
        DuplicateString(&deviceInfo.platformVersion, platformVersion);
        DuplicateString(&deviceInfo.supportUrl, supportUrl);
        DuplicateString(&deviceInfo.version, version);
    }catch(exception &e)
    {
        std::cout<<"String Copy failed!!\n";
        return OC_STACK_ERROR;
    }

    return OC_STACK_OK;
}



int main()
{

    // Create PlatformConfig object
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Server,
        "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
        0,         // Uses randomly available port
        OC::QualityOfService::LowQos
    };

    OCPlatform::Configure(cfg);

    std::cout<<"Starting server & setting device info\n";

    OCStackResult result = SetDeviceInfo(contentType, dateOfManufacture, deviceName,
            deviceUUID, firmwareVersion, hostName, manufacturerName, manufacturerUrl,
            modelNumber, platformVersion, supportUrl, version);

    if(result != OC_STACK_OK)
    {
        std::cout << "Device Registration failed\n";
        return -1;
    }

    result = OCPlatform::registerDeviceInfo(deviceInfo);

    if(result != OC_STACK_OK)
    {
        std::cout << "Device Registration failed\n";
        return -1;
    }

    DeleteDeviceInfo();

    // A condition variable will free the mutex it is given, then do a non-
    // intensive block until 'notify' is called on it.  In this case, since we
    // don't ever call cv.notify, this should be a non-processor intensive version
    // of while(true);
    std::mutex blocker;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(blocker);
    cv.wait(lock);

    // No explicit call to stop the platform.
    // When OCPlatform::destructor is invoked, internally we do platform cleanup

    return 0;

}



