/*
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Copyright 2018 Intel Corporation All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

#ifndef EXAMPLE_OCF_LIGHT_PLATFORM_H_
#define EXAMPLE_OCF_LIGHT_PLATFORM_H_

#include <vector>
#include <string>
#include "OCPlatform.h"

class Platform
{
public:
    Platform(void);
    virtual ~Platform(void);

    OCStackResult start(void);
    OCStackResult registerPlatformInfo(void);
    OCPlatformInfo* getPlatformInfo(void);
    OCStackResult stop(void);

    /**
     *  SetDeviceInfo
     *  Sets the device information ("oic/d"), from the globals

     * @return OC_STACK_ERROR or OC_STACK_OK
     */
    OCStackResult setDeviceInfo(void);

private:
    // Set of strings for each of platform Info fields
    std::string m_platformId = "0A3E0D6F-DBF5-404E-8719-D6880042463A";
    std::string m_manufacturerName = "ocf";
    std::string m_manufacturerLink = "https://ocf.org/";
    std::string m_modelNumber = "ModelNumber";
    std::string m_dateOfManufacture = "2017-12-01";
    std::string m_platformVersion = "1.0";
    std::string m_operatingSystemVersion = "myOS";
    std::string m_hardwareVersion = "1.0";
    std::string m_firmwareVersion = "1.0";
    std::string m_supportLink = "https://ocf.org/";
    std::string m_systemTime = "2017-12-01T12:00:00.52Z";

    /**
     *  SetPlatformInfo
     *  Sets the platform information ("oic/p"), from the globals

     * @param platformID the platformID
     * @param manufacturerName the manufacturerName
     * @param manufacturerUrl the manufacturerUrl
     * @param modelNumber the modelNumber
     * @param platformVersion the platformVersion
     * @param operatingSystemVersion the operatingSystemVersion
     * @param hardwareVersion the hardwareVersion
     * @param firmwareVersion the firmwareVersion
     * @param supportUrl the supportUrl
     * @param systemTime the systemTime
     */
    void setPlatformInfo(std::string platformID, std::string manufacturerName,
            std::string manufacturerUrl, std::string modelNumber, std::string dateOfManufacture,
            std::string platformVersion, std::string operatingSystemVersion,
            std::string hardwareVersion, std::string firmwareVersion, std::string supportUrl,
            std::string systemTime);

    /**
     *  deletePlatformInfo
     *  Deletes the allocated platform information
     */
    void deletePlatformInfo(void);
    // OCPlatformInfo Contains all the platform info to be stored
    OCPlatformInfo platformInfo;

public:
    std::string  protocolIndependentID = "fa008167-3bbf-4c9d-8604-c9bcb96cb712";

    // Set of strings for each of device info fields
    std::string  deviceName = "Binary Switch";
    std::string  deviceType = "oic.d.light";
    std::string  specVersion = "ocf.1.0.0";
    std::vector<std::string> dataModelVersions;
};

#endif /* EXAMPLE_OCF_LIGHT_PLATFORM_H_ */
