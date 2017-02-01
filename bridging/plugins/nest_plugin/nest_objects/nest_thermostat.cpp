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

#include <stdio.h>
#include <string>
#include "nest_thermostat.h"
#include "nest_defs.h"
#include "rapidjson.h"
#include "document.h"
#include "json_helper.h"
#include "writer.h"
#include "stringbuffer.h"
#include "curl_client.h"
#include "logger.h"

#define TAG "NEST_THERMOSTAT"

NestThermostat::NestThermostat(const std::string &token,uint16_t hum, uint32_t hvac,
                                uint16_t temp, uint32_t scale, const char * devId) : m_token(token)
{
    m_thermostat.humidity = hum;
    m_thermostat.targetTempF = temp;
    m_thermostat.temperature = (TEMPERATURE_SCALE) scale;
    m_thermostat.hvacMode = (HVAC_MODE) hvac;
    m_thermostat.devInfo.id = devId;
}
NestThermostat::NestThermostat(const std::string &token, const std::string &thermostat)
        : m_token(token)
{
    buildThermostat(thermostat);
}

NestThermostat::HVAC_MODE NestThermostat::getHVACmode(const std::string &hvacMode)
{
    NestThermostat::HVAC_MODE result = HVAC_UNDEFINED;
    if (hvacMode == NEST_HVAC_HEAT)
    {
        result = HVAC_HEAT;
    }
    else if (hvacMode == NEST_HVAC_COOL)
    {
        result = HVAC_COOL;
    }
    else if (hvacMode == NEST_HVAC_MIXED)
    {
        result = HVAC_MIXED;
    }
    else if (hvacMode == NEST_HVAC_OFF)
    {
        result = HVAC_OFF;
    }
    return result;
}

NestThermostat::TEMPERATURE_SCALE NestThermostat::getTemperatureScale(const std::string &tempScale)
{
    NestThermostat::TEMPERATURE_SCALE result = TEMP_UNDEFINED;
    if (tempScale == NEST_TEMP_SCALE_C)
    {
        result = TEMP_CELCIUS;
    }
    else if (tempScale == NEST_TEMP_SCALE_F)
    {
        result = TEMP_FAHRENHEIT;
    }
    return result;
}

int NestThermostat::buildThermostat(const std::string &thermostat)
{
    if (thermostat.empty())
    {
        return GW_RESULT_INVALID_DATA;
    }

    rapidjson::Document doc;
    doc.SetObject();

    if (doc.Parse<0>(thermostat.c_str()).HasParseError())
    {
        return GW_RESULT_JSON_ERROR;
    }

    // Read the general/common device infor structure (common for all Nest devices)
    m_thermostat.devInfo.version = doc[NEST_SW_VER_TAG.c_str()].GetString();
    m_thermostat.devInfo.locale = doc[NEST_LOCALE_TAG.c_str()].GetString();
    m_thermostat.devInfo.nameLong = doc[NEST_NAME_LONG_TAG.c_str()].GetString();
    m_thermostat.devInfo.id = doc[NEST_DEVICE_ID_TAG.c_str()].GetString();
    m_thermostat.devInfo.name = doc[NEST_NAME_TAG.c_str()].GetString();
    m_thermostat.devInfo.structId = doc[NEST_STRUCT_ID_TAG.c_str()].GetString();

    // Read the details of the thermostat
    m_thermostat.humidity = doc[NEST_HUMIDITY_TAG.c_str()].GetInt();
    m_thermostat.hasFan = doc[NEST_FAN_TAG.c_str()].GetBool();
    m_thermostat.hasLeaf = doc[NEST_LEAF_TAG.c_str()].GetBool();
    m_thermostat.canHeat = doc[NEST_HEAT_TAG.c_str()].GetBool();
    m_thermostat.canCool = doc[NEST_COOL_TAG.c_str()].GetBool();
    m_thermostat.targetTempC = doc[NEST_TARGET_TEMP_C_TAG.c_str()].GetDouble();
    m_thermostat.targetTempF = doc[NEST_TARGET_TEMP_F_TAG.c_str()].GetUint();
    m_thermostat.targetTempHighC = doc[NEST_TARGET_TEMP_HIGH_C_TAG.c_str()].GetDouble();
    m_thermostat.targetTempHighF = doc[NEST_TARGET_TEMP_HIGH_F_TAG.c_str()].GetUint();
    m_thermostat.targetTempLowC = doc[NEST_TARGET_TEMP_LOW_C_TAG.c_str()].GetDouble();
    m_thermostat.targetTempLowF = doc[NEST_TARGET_TEMP_LOW_F_TAG.c_str()].GetUint();
    m_thermostat.ambientTempC = doc[NEST_AMBIENT_TEMP_C_TAG.c_str()].GetDouble();
    m_thermostat.ambientTempF = doc[NEST_AMBIENT_TEMP_F_TAG.c_str()].GetDouble();
    m_thermostat.awayTempHighC = doc[NEST_AWAY_TEMP_HIGH_C_TAG.c_str()].GetDouble();
    m_thermostat.awayTempHighF = doc[NEST_AWAY_TEMP_HIGH_F_TAG.c_str()].GetDouble();
    m_thermostat.awayTempLowC = doc[NEST_AWAY_TEMP_LOW_C_TAG.c_str()].GetDouble();
    m_thermostat.awayTempLowF = doc[NEST_AWAY_TEMP_LOW_F_TAG.c_str()].GetDouble();
    m_thermostat.fanTimerActive = doc[NEST_FAN_TIMER_ACTIVE_TAG.c_str()].GetBool();
    m_thermostat.isOnline = doc[NEST_ONLINE_TAG.c_str()].GetBool();
    m_thermostat.hvacMode = getHVACmode(doc[NEST_HVAC_MODE_TAG.c_str()].GetString());
    m_thermostat.temperature = getTemperatureScale(doc[NEST_TEMP_SCALE.c_str()].GetString());

    return GW_RESULT_OK;
}

int NestThermostat::setTemperature(uint16_t targetTemp)
{
    std::string scale;
    std::string request = "{ }";
    rapidjson::StringBuffer sb;
    rapidjson::Document doc;

    if (m_thermostat.temperature == TEMP_FAHRENHEIT)
    {
        scale = NEST_TARGET_TEMP_F_TAG;
    }
    else
    {
        scale = NEST_TARGET_TEMP_C_TAG;
    }

    std::string uri(NEST_BASE_URL);
    uri += NEST_DEVICES_THERMOSTAT_STR + m_thermostat.devInfo.id + NEST_AUTH_STR + m_token;

    doc.SetObject();
    JsonHelper::setMember(doc, scale, targetTemp);
    request = JsonHelper::toString(doc);

    CurlClient cc = CurlClient("PUT", uri)
                 .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                 .addRequestHeader(CURL_CONTENT_TYPE_JSON)
                 .setRequestBody(request);

     int curlCode = cc.send();
     std::string response = cc.getResponseBody();

     if (curlCode != GW_RESULT_OK)
     {
         OIC_LOG_V(ERROR, TAG,"Set temperature failed. Error code %d", curlCode);
         return GW_RESULT_INTERNAL_ERROR;
     }

     return GW_RESULT_OK;
}
