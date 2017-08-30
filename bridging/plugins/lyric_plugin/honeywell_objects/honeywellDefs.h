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

#ifndef __HONEYWELLDEFS_H__
#define __HONEYWELLDEFS_H__

#include <string>

// *** THIS CONTAINS CONSTANTS COMMON TO ALL HONEYWELL PLUGINS ***

// various constants
#define MESSAGE_STRING_SIZE         1024
#define MAX_LOG_STRING              768
#define HONEYWELL_HEAT_SETPOINT     "heatSetpoint"
#define HONEYWELL_COOL_SETPOINT     "coolSetpoint"
#define HONEYWELL_VALUE             "value"
#define HONEYWELL_SETPOINT_BUFFER   2
#define HONEYWELL_THERMOSTAT_ACTUATOR_IF     "oic.if.a"
#define HONEYWELL_THERMOSTAT_SENSOR_IF       "oic.if.s"
#define HONEYWELL_THERMOSTAT_BASELINE_IF     "oic.if.baseline"
#define HONEYWELL_THERMOSTAT_RT     "oic.r.temperature"
#define INVALID_DEVICE_INDEX        (-1)
#define DEVICE_INDEX_START          1
#define FIRST_ARRAY_ELEMENT         0
#define HW_MANUFACTURER_NAME        "Honeywell"
#define HW_AUTH_LOOP_MINUTES        9
#define HW_QUERY_INTERVAL_SECONDS   60
#define HONEYWELL_TEMP_SCALE_C      "C"
#define HONEYWELL_TEMP_SCALE_F      "F"

// representation value names
#define REP_NAME_TEMPERATURE        "temperature"
#define REP_NAME_MODE               "x.intel.com.mode"
#define REP_NAME_TARGET_TEMP_HIGH   "x.intel.com.targetTempHigh"
#define REP_NAME_TARGET_TEMP_LOW    "x.intel.com.targetTempLow"
#define REP_NAME_INDOOR_TEMP        "x.intel.com.indoorTemp"

// representation values
#define REP_VALUE_COOL              "cool"
#define REP_VALUE_HEAT              "heat"
#define REP_VALUE_OFF               "off"

// HONEYWELL THERMOSTAT MODES
// See ThermostatMode enumeration documentation at TCC Web API Help site (more modes there)
#define THERMOSTAT_MODE_FORMAT         "\"%s\""
#define THERMOSTAT_MODE_OFF            "Off"
#define THERMOSTAT_MODE_COOL           "Cool"
#define THERMOSTAT_MODE_HEAT           "Heat"

///
/// json tag definitions
///
#define JSON_REFRESH_TOKEN              "refresh_token"
#define JSON_ACCESS_TOKEN               "access_token"
#define JSON_ERROR                      "error"
#define JSON_USER_ID                    "userID"
#define JSON_DEVICE_ID                  "deviceID"
#define JSON_UNIQUE_ID                  "macID"
#define JSON_ACCESS_CONFIRMED           "accessConfirmed"
#define JSON_MODE                       "mode"
#define JSON_EXPIRES_IN                 "expires_in"
#define JSON_DEVICE_NAME                "deviceName"
#define JSON_RESPONSE_CODE              "code"
#define JSON_RESPONSE_MESSAGE           "message"
#define JSON_RESPONSE_UNAUTHORIZED      "unauthorized"
#define JSON_LOCATION_ID                "locationID"
#define JSON_USERS_ARRAY                "users"
#define JSON_DEVICES_ARRAY              "devices"
#define JSON_THERMOSTAT                 "thermostat"
#define JSON_CHANGEABLE_VALUES          "changeableValues"

#endif /* __HONEYWELLDEFS_H__ */
