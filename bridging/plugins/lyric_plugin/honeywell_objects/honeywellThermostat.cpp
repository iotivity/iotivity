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

#include <stdio.h>
#include <string>
#include <sstream> // ostringstream
#include "honeywellThermostat.h"

#include "honeywellDefsLyric.h"

#include "rapidjson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "curlClient.h"
#include "experimental/logger.h"
#include "../honeywellHelpers.h"

#define LOG_TAG "HONEYWELL_THERMOSTAT"

HoneywellThermostat::HoneywellThermostat(const std::string &token, const std::string &thermostat) : m_token(token)
{
    buildThermostat(thermostat);
}

HoneywellThermostat::HoneywellThermostat(const THERMOSTAT data)
{
    m_thermostat = data;
}

HoneywellThermostat::HoneywellThermostat()
{
    m_valid = false;
}

std::string hvacModeToString(const HVAC_MODE &hvacMode)
{
    std::string result = THERMOSTAT_MODE_OFF;
    switch (hvacMode)
    {
        case HVAC_COOL :
            result = THERMOSTAT_MODE_COOL;
            break;
        case HVAC_HEAT :
            result = THERMOSTAT_MODE_HEAT;
            break;
        case HVAC_OFF :
            // do nothing, already 'off'
            break;
        default:
            OIC_LOG(ERROR, LOG_TAG, "Unexpected hvacMode; defaulting to Off");
            break;
    }
    return result;
}

void HoneywellThermostat::buildDeviceUri(const std::string &)
{
    std::string hw_tag = "/honeywell/";
    std::ostringstream uriStream;

    uriStream << hw_tag.c_str() << m_deviceUniqueId.c_str();
    m_deviceUri = uriStream.str();
    OIC_LOG_V(INFO, LOG_TAG, "m_deviceUri: %s", m_deviceUri.c_str());
}

HVAC_MODE HoneywellThermostat::getHVACmode(const std::string &hvacMode)
{
    HVAC_MODE result = HVAC_UNDEFINED;
    if (hvacMode == REP_VALUE_HEAT)
    {
        result = HVAC_HEAT;
    }
    else if (hvacMode == REP_VALUE_COOL)
    {
        result = HVAC_COOL;
    }
    else if (hvacMode == REP_VALUE_OFF)
    {
        result = HVAC_OFF;
    }
    return result;
}

TEMPERATURE_SCALE HoneywellThermostat::getTemperatureScale(const std::string &tempScale)
{
    TEMPERATURE_SCALE result = TEMP_UNDEFINED;
    if (tempScale == HONEYWELL_TEMP_SCALE_C)
    {
        result = TEMP_CELSIUS;
    }
    else if (tempScale == HONEYWELL_TEMP_SCALE_F)
    {
        result = TEMP_FAHRENHEIT;
    }
    return result;
}

int HoneywellThermostat::buildThermostat(const std::string &thermostat)
{
    if (thermostat.empty())
    {
        return MPM_RESULT_INVALID_DATA;
    }

    OIC_LOG(INFO, LOG_TAG, "This version of the function not currently used.");

    return MPM_RESULT_NOT_IMPLEMENTED;
}

MPMResult HoneywellThermostat::setTemperature(const THERMOSTAT data)
{
    MPMResult result = MPM_RESULT_OK;

    // the important values here are the coolSetpoint and heatSetpoint (honeywell doesn't
    // have a single desired temperature). we can also set the mode if the current room temp
    // is beyond the limits. (not necessary if thermostat is in auto mode, though- they
    // will automatically kick on)

    // note: the global Honeywell object now takes care of communicating with the cloud.
    //       just copy relevant values here.
    m_thermostat.targetTempF = data.targetTempF;
    m_thermostat.heatSetpointF = data.heatSetpointF;
    m_thermostat.coolSetpointF = data.coolSetpointF;
    m_thermostat.hvacMode = data.hvacMode;

    return result;
}

THERMOSTAT::THERMOSTAT(const THERMOSTAT &data)
{
    devInfo.deviceId = data.devInfo.deviceId; // tcc device id
    devInfo.id = data.devInfo.id;
    devInfo.lastConnection = data.devInfo.lastConnection;
    devInfo.locale = data.devInfo.locale;
    devInfo.name = data.devInfo.name;
    devInfo.nameLong = data.devInfo.nameLong;
    devInfo.structId = data.devInfo.structId;
    devInfo.version = data.devInfo.version;
    devInfo.deviceIdStr = data.devInfo.deviceIdStr; // lyric device id
    devInfo.locationId = data.devInfo.locationId; // lyric location id
    devInfo.uniqueId = data.devInfo.uniqueId;

    isOnline = data.isOnline;
    canCool = data.canCool;
    canHeat = data.canHeat;
    usingEmergencyHeat = data.usingEmergencyHeat;
    hasFan = data.hasFan;
    fanTimerActive = data.fanTimerActive;
    fanTimerTimeout = data.fanTimerTimeout;
    hasLeaf = data.hasLeaf;
    temperature = data.temperature;
    targetTempC = data.targetTempC;
    targetTempF = data.targetTempF;
    heatSetpointC = data.heatSetpointC;
    heatSetpointF = data.heatSetpointF;
    coolSetpointC = data.coolSetpointC;
    coolSetpointF = data.coolSetpointF;
    awayTempHighF = data.awayTempHighF;
    awayTempHighC = data.awayTempHighC;
    awayTempLowF = data.awayTempLowF;
    awayTempLowC = data.awayTempLowC;
    hvacMode = data.hvacMode;
    ambientTempF = data.ambientTempF;
    ambientTempC = data.ambientTempC;
    humidity = data.humidity;
    preferredDevice = data.preferredDevice;
    cloudIndex = data.cloudIndex;

    return;
}

THERMOSTAT::THERMOSTAT()
{
    devInfo.deviceId = 0;
    isOnline = false;
    canCool = false;
    canHeat = false;
    usingEmergencyHeat = false;
    hasFan = false;
    fanTimerActive = false;
    fanTimerTimeout = false;
    hasLeaf = false;
    temperature = TEMP_FAHRENHEIT;
    targetTempC = 0.0;
    targetTempF = 0.0;
    heatSetpointC = 0.0;
    heatSetpointF = 0;
    coolSetpointC = 0.0;
    coolSetpointF = 0.0;
    awayTempHighC = 0.0;
    awayTempHighF = 0.0;
    awayTempLowC = 0.0;
    awayTempLowF = 0.0;
    hvacMode = HVAC_OFF;
    ambientTempC = 0.0;
    ambientTempF = 0.0;
    humidity = 0.0;
    preferredDevice = false;
    cloudIndex = 0;
}

void dump_details(const THERMOSTAT &thermostat, const std::string &description)
{
    if (description.empty())
    {
        OIC_LOG_V(INFO, LOG_TAG, "deviceId: %d, heatSetpoint: %f, coolSetpoint: %f, targetTemp: %f",
              thermostat.devInfo.deviceId, thermostat.heatSetpointF, thermostat.coolSetpointF,
              thermostat.targetTempF);
    }
    else
    {
        OIC_LOG_V(INFO, LOG_TAG, "%s - deviceId: %d, heatSetpoint: %f, coolSetpoint: %f, targetTemp: %f",
              description.c_str(), thermostat.devInfo.deviceId, thermostat.heatSetpointF,
              thermostat.coolSetpointF, thermostat.targetTempF);
    }
    (void)thermostat;
    return;
}
