//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
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

#ifndef __NEST_THERMOSTAT_H__
#define __NEST_THERMOSTAT_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include "mpmErrorCode.h"

using namespace std;

class NestThermostat
{
    public:
        typedef std::vector<NestThermostat> devices;

        NestThermostat(const std::string &token, uint16_t hum, uint32_t hvac,
                       uint16_t temp, uint32_t scale, const std::string &devId);

        NestThermostat(const std::string &token, const std::string &jsonThermostat);

        virtual ~NestThermostat()
        { }

        enum TEMPERATURE_SCALE
        {
            TEMP_UNDEFINED = 0,
            TEMP_CELSIUS = 1,
            TEMP_FAHRENHEIT = 2,
            TEMP_MAX
        };

        enum HVAC_MODE
        {
            HVAC_UNDEFINED = 0,
            HVAC_HEAT = 1,
            HVAC_COOL = 2,
            HVAC_MIXED = 3,
            HVAC_OFF = 4,
            HVAC_MAX
        };

        typedef struct _DEVICE_INFO
        {
            std::string id;                 // the unique ID for this instance
            std::string version;            // software version
            std::string structId;           // structure ID
            std::string name;               // Friendly name of the thermostat
            std::string nameLong;           // Long friendly name
            std::string lastConnection;     // Last connection time
            std::string locale;             // Locale
        } DEVICE_INFO;

        typedef struct _THERMOSTAT
        {
            DEVICE_INFO devInfo;            // Device info
            bool isOnline;                  // Is thermostat online now?
            bool canCool;                   // AC connected
            bool canHeat;                   // Heater connected
            bool usingEmergencyHeat;        // True if emergency heat is in use
            bool hasFan;                    // Is a fan installed
            bool fanTimerActive;            // Is the fan timer currently active
            bool fanTimerTimeout;           // Fan timer timeout (absolut timee)
            bool hasLeaf;                   // Is leaf (savings) displayed
            TEMPERATURE_SCALE temperature;  // Temperature scale (C or F))
            double targetTempC;             // Target temperature C
            uint16_t targetTempF;           // Target temperature F
            double targetTempHighC;         // Target high temperature C
            uint16_t targetTempHighF;       // Target high temperature F
            double targetTempLowC;          // Target low temperature C
            uint16_t targetTempLowF;        // Target low temperature F
            double awayTempHighF;           // Away high tempereature F
            double awayTempHighC;           // Away high temperature C
            uint16_t awayTempLowF;          // Away low temperature F
            uint16_t awayTempLowC;          // Away low temperature C
            HVAC_MODE hvacMode;             // Current HVAC mode
            double ambientTempF;            // Ambient temperature F
            double ambientTempC;            // Ambient temperature C
            uint16_t humidity;              // Current humidity (0 - 100)

            _THERMOSTAT()
            {
                isOnline = canCool = canHeat = usingEmergencyHeat = hasFan = fanTimerActive = fanTimerTimeout =
                                                   hasLeaf = false;
                targetTempC = targetTempHighC = targetTempLowC = awayTempHighF = awayTempHighC = ambientTempF =
                                                    ambientTempC = 0.0;
                targetTempF = targetTempHighF = targetTempLowF = awayTempLowF = awayTempLowC = humidity = 0;
                temperature = (TEMPERATURE_SCALE) - 1;
                hvacMode = (HVAC_MODE) - 1;
            }

        } THERMOSTAT;

        MPMResult get(THERMOSTAT &data)
        {
            data = m_thermostat;
            return MPM_RESULT_OK;
        }

        MPMResult setTemperature(uint16_t targetTemp);

    private:

        MPMResult buildThermostat(const std::string &json);

        HVAC_MODE getHVACmode(const std::string &hvacMode);

        TEMPERATURE_SCALE getTemperatureScale(const std::string &tempScale);

        THERMOSTAT m_thermostat;
        std::string m_token;
};

typedef std::shared_ptr<NestThermostat> NestThermostatSharedPtr;
#endif /* __NEST_THERMOSTAT_H__ */
