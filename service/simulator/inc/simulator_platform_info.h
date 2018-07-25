/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
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
 ******************************************************************/
#ifndef SIMULATOR_PLATFORM_INFO_H_
#define SIMULATOR_PLATFORM_INFO_H_

#include <iostream>

/**
 * @class   PlatformInfo
 *
 * @brief   This class contains remote device platform information and provide APIs access it.
 */
class PlatformInfo
{
    public:
        /**
         * This method is for getting platfom id.
         *
         * @return Platform id.
         */
        std::string getPlatformID() const;

        /**
         * This method is for getting platform version.
         *
         * @return Platform version.
         */
        std::string getPlatformVersion() const;

        /**
         * This method is for getting manufacturer name.
         *
         * @return manufacturer name.
         */
        std::string getManufacturerName() const;

        /**
         * This method is for getting manufacturer URL.
         *
         * @return manufacturer URL.
         */
        std::string getManufacturerUrl() const;

        /**
         * This method is for getting model number.
         *
         * @return Model number.
         */
        std::string getModelNumber() const;

        /**
         * This method is for getting date of manufacture.
         *
         * @return Date of manufacture.
         */
        std::string getDateOfManfacture() const;

        /**
         * This method is for getting operating system version.
         *
         * @return Operating system version.
         */
        std::string getOSVersion() const;

        /**
         * This method is for getting hardware version.
         *
         * @return Hardware version.
         */
        std::string getHardwareVersion() const;

        /**
         * This method is for getting firmware version.
         *
         * @return Firmware version.
         */
        std::string getFirmwareVersion() const;

        /**
         * This method is for getting support link URL.
         *
         * @return URL of support link.
         */
        std::string getSupportUrl() const;

        /**
         * This method is for getting system time.
         *
         * @return System time.
         */
        std::string getSystemTime() const;

        /**
         * This method is for set the platform ID
         *
         * @param platformId    platform identity
         */
        void setPlatformID(const std::string &platformId);

        /**
         * This method is for set the platform version
         *
         * @param platformVersion    platform version information
         */
        void setPlatformVersion(const std::string &platformVersion);

        /**
         * This method is for set the manufacturer name
         *
         * @param manufacturerName       detail of manufacturer name
         */
        void setManufacturerName(const std::string &manufacturerName);

        /**
         * This method is for set the manufacturer URL details
         *
         * @param manufacturerUrl    manufacturer URL detail to set
         */
        void setManufacturerUrl(const std::string &manufacturerUrl);

        /**
         * This method is for set model number
         *
         * @param modelNumber    model number information
         */
        void setModelNumber(const std::string &modelNumber);

        /**
         * This method is for set date of manufacture
         *
         * @param dateOfManufacture    date of manufacture to set
         */
        void setDateOfManfacture(const std::string &dateOfManufacture);

        /**
         * This method is for set the OS version
         *
         * @param osVersion    OS version information to set
         */
        void setOSVersion(const std::string &osVersion);

        /**
         * This method is for set the hardware version
         *
         * @param hwVersion    hardware version information to set
         */
        void setHardwareVersion(const std::string &hwVersion);

        /**
         * This method is for set the firmware version information
         *
         * @param firmwareVersion         firmware version to set
         */
        void setFirmwareVersion(const std::string &firmwareVersion);

        /**
         * This method is for set the support URL detail
         *
         * @param supportUrl    support URL to set
         */
        void setSupportUrl(const std::string &supportUrl);

        /**
         * This method is for set the system time information
         *
         * @param systemTime       system time to set
         */
        void setSystemTime(const std::string &systemTime);

    private:
        std::string m_platformID;
        std::string m_manufacturerName;
        std::string m_manufacturerUrl;
        std::string m_modelNumber;
        std::string m_dateOfManufacture;
        std::string m_platformVersion;
        std::string m_operationSystemVersion;
        std::string m_hardwareVersion;
        std::string m_firmwareVersion;
        std::string m_supportUrl;
        std::string m_systemTime;
};

#endif
