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

#ifndef __NEST_DEFS_H__
#define __NEST_DEFS_H__

#include <string>

static const char ACCESS_TOKEN_URL[]   = "https://api.home.nest.com/oauth2/access_token";
static const char NEST_BASE_URL[]      = "https://developer-api.nest.com";

///
/// json tag definitions
///
static const char NEST_ACCESS_TOKEN_TAG[]      = "access_token";
static const char NEST_VALIDITY_TAG[]          = "expires_in";
static const char NEST_THERMOSTAT_TAG[]        = "thermostats";
static const char NEST_DEVICE_ID_TAG[]         = "device_id";
static const char NEST_LOCALE_TAG[]            = "locale";
static const char NEST_SW_VER_TAG[]            = "software_version";
static const char NEST_STRUCT_ID_TAG[]         = "structure_id";
static const char NEST_NAME_TAG[]              = "name";
static const char NEST_NAME_LONG_TAG[]         = "name_long";
static const char NEST_LAST_CONN_TAG[]         = "last_connection";
static const char NEST_ONLINE_TAG[]            = "is_online";
static const char NEST_COOL_TAG[]              = "can_cool";
static const char NEST_HEAT_TAG[]              = "can_heat";
static const char NEST_FAN_TAG[]               = "has_fan";
static const char NEST_HUMIDITY_TAG[]          = "humidity";
static const char NEST_TEMP_SCALE[]            = "temperature_scale";
static const char NEST_HVAC_MODE_TAG[]         = "hvac_mode";
static const char NEST_TARGET_TEMP_C_TAG[]     = "target_temperature_c";
static const char NEST_TARGET_TEMP_F_TAG[]     = "target_temperature_f";
static const char NEST_TARGET_TEMP_HIGH_C_TAG[] = "target_temperature_high_c";
static const char NEST_TARGET_TEMP_HIGH_F_TAG[] = "target_temperature_high_f";
static const char NEST_TARGET_TEMP_LOW_C_TAG[] = "target_temperature_low_c";
static const char NEST_TARGET_TEMP_LOW_F_TAG[] = "target_temperature_low_f";
static const char NEST_AMBIENT_TEMP_C_TAG[]    = "ambient_temperature_c";
static const char NEST_AMBIENT_TEMP_F_TAG[]    = "ambient_temperature_f";
static const char NEST_AWAY_TEMP_HIGH_C_TAG[]  = "away_temperature_high_c";
static const char NEST_AWAY_TEMP_HIGH_F_TAG[]  = "away_temperature_high_f";
static const char NEST_AWAY_TEMP_LOW_C_TAG[]   = "away_temperature_low_c";
static const char NEST_AWAY_TEMP_LOW_F_TAG[]   = "away_temperature_low_f";
static const char NEST_FAN_TIMER_ACTIVE_TAG[]  = "fan_timer_active";
static const char NEST_LEAF_TAG[]              = "has_leaf";
static const char NEST_EMERGENCY_HEAT_TAG[]    = "is_using_emergency_heat";
static const char NEST_AWAY_TAG[]              = "away";
static const char NEST_HOME_TAG[]              = "home";
static const char NEST_POSTAL_TAG[]            = "postal_code";
static const char NEST_COUNTRY_TAG[]           = "country_code";
static const char NEST_TIMEZONE_TAG[]          = "time_zone";

// Other constants
static const int NEST_ACCESS_TOKEN_LEN              = 512;
static const int NEST_ACCESS_TOKEN_EXPIRY           = 128;
static const char NEST_HVAC_HEAT[]             = "heat";
static const char NEST_HVAC_COOL[]             = "cool";
static const char NEST_HVAC_MIXED[]            = "heat-cool";
static const char NEST_HVAC_OFF[]              = "off";
static const char NEST_TEMP_SCALE_C[]          = "C";
static const char NEST_TEMP_SCALE_F[]          = "F";

// Other definitions
static const char NEST_STRUCTURE_AUTH_STR[]    = "/structures?auth=";
static const char NEST_CLIENT_ID_STR[]         = "?client_id=";
static const char NEST_CODE_STR[]              = "&code=";
static const char NEST_CLIENT_SECRET_STR[]     = "&client_secret=";
static const char NEST_AUTH_CODE_STR[]         = "&grant_type=authorization_code";
static const char NEST_DEVICES_AUTH_STR[]      = "/devices?auth=";
static const char NEST_DEVICES_THERMOSTAT_STR[] = "/devices/thermostats/";
static const char NEST_AUTH_STR[]              = "?auth=";

#endif /* __NEST_DEFS_H__ */
