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
//

#ifndef __HONEYWELLTHERMOSTAT_H__
#define __HONEYWELLTHERMOSTAT_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>
#include "mpmErrorCode.h"
#include <typeinfo>
#include <memory>

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
    int deviceId;               // honeywell device id (only used for Honeywell TCC)
    std::string id;             // the unique ID for this instance
    std::string version;        // software version
    std::string structId;       // structure ID
    std::string name;           // Friendly name of the thermostat
    std::string nameLong;       // Long friendly name
    std::string lastConnection; // Last connection time
    std::string locale;         // Locale
    std::string deviceIdStr;    // lyric device id (only for Lyric)
    std::string uniqueId;       // unique ID
    std::string uri;            // device uri
    int locationId;             // lyric location id (required for device access)
} DEVICE_INFO;

struct THERMOSTAT
{
    THERMOSTAT(); // default constructor
    THERMOSTAT(const THERMOSTAT &data); // copy constructor

    DEVICE_INFO devInfo;           // Device info
    bool isOnline;                 // Is thermostat online now?
    bool canCool;                  // AC connected
    bool canHeat;                  // Heater connected
    bool usingEmergencyHeat;       // True if emergency heat is in use
    bool hasFan;                   // Is a fan installed
    bool fanTimerActive;           // Is the fan timer currently active
    bool fanTimerTimeout;          // Fan timer timeout (absolut timee)
    bool hasLeaf;                  // Is leaf (savings) displayed
    TEMPERATURE_SCALE temperature; // Temperature scale (C or F))
    double targetTempC;            // Target temperature C
    double targetTempF;            // Target temperature F
    double awayTempHighF;          // Away high tempereature F
    double awayTempHighC;          // Away high temperature C
    double awayTempLowF;           // Away low temperature F
    double awayTempLowC;           // Away low temperature C
    HVAC_MODE hvacMode;            // Current HVAC mode
    double ambientTempF;           // Ambient temperature F
    double ambientTempC;           // Ambient temperature C
    double humidity;               // Current humidity (0 - 100)
    double heatSetpointF;           // heat threshold F
    double coolSetpointF;           // cool threshold F
    double heatSetpointC;           // heat threshold C
    double coolSetpointC;           // cool threshold C
    bool preferredDevice;           // is this device mentioned in PREFERRED_THERMOSTAT?
    int cloudIndex;                 // index of device in array returned by server
};

void dump_details(const THERMOSTAT &thermostat, const std::string &description);

std::string hvacModeToString(const HVAC_MODE &hvacMode);

class HoneywellThermostat
{
public:
    typedef std::vector<HoneywellThermostat>::iterator it;
    typedef std::vector<std::string> curlHeaders;

    bool operator==(const HoneywellThermostat &ht)
    {
        return m_changeableValues == ht.m_changeableValues;
    }

    HoneywellThermostat(const std::string &token, const std::string &jsonThermostat);

    HoneywellThermostat(const THERMOSTAT data);

    HoneywellThermostat();

    virtual ~HoneywellThermostat()
    {
    }

    int get(THERMOSTAT &data)
    {
        data = m_thermostat;
        return MPM_RESULT_OK;
    }

    void set(const THERMOSTAT &data)
    {
        m_thermostat = data;
    }

    MPMResult setTemperature(const THERMOSTAT data);

    HVAC_MODE getHVACmode(const std::string &hvacMode);

    void setChangeableValues(const std::string &changeableValues)
    {
        m_changeableValues = changeableValues;
    }

    std::string getChangeableValues()
    {
        return m_changeableValues;
    }

    void buildDeviceUri(const std::string &);

    std::string getDeviceUri()
    {
        return m_deviceUri;
    }

    void setDeviceUniqueId(const std::string &uniqueId)
    {
        m_deviceUniqueId = uniqueId;
    }

    std::string getDeviceUniqueId()
    {
        return m_deviceUniqueId;
    }

private:
    int buildThermostat(const std::string &json);

    TEMPERATURE_SCALE getTemperatureScale(const std::string &tempScale);

    bool m_valid;
    THERMOSTAT m_thermostat;
    std::string m_token;
    curlHeaders m_inHeaders;
    std::string m_changeableValues;
    std::string m_deviceUri;
    std::string m_deviceUniqueId;
};

typedef std::shared_ptr<HoneywellThermostat> LyricThermostatSharedPtr;

#endif /* __HONEYWELL_THERMOSTAT_H__ */
