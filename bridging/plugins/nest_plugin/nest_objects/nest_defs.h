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

static const std::string AUTH_URL_BASE      = "https://home.nest.com/login/oauth2?client_id=";
static const std::string AUTH_URL_SUFFIX     = "&state=STATE";
static const std::string DEAUTHORIZATION_URL= "https://api.home.nest.com/oauth2/access_tokens/";
static const std::string ACCESS_TOKEN_URL   = "https://api.home.nest.com/oauth2/access_token";
static const std::string NEST_BASE_URL      = "https://developer-api.nest.com";

///
/// json tag definitions
///
static const std::string NEST_ACCESS_TOKEN_TAG      = "access_token";
static const std::string NEST_VALIDITY_TAG          = "expires_in";
static const std::string NEST_THERMOSTAT_TAG        = "thermostats";
static const std::string NEST_DEVICE_ID_TAG         = "device_id";
static const std::string NEST_LOCALE_TAG            = "locale";
static const std::string NEST_SW_VER_TAG            = "software_version";
static const std::string NEST_STRUCT_ID_TAG         = "structure_id";
static const std::string NEST_NAME_TAG              = "name";
static const std::string NEST_NAME_LONG_TAG         = "name_long";
static const std::string NEST_LAST_CONN_TAG         = "last_connection";
static const std::string NEST_ONLINE_TAG            = "is_online";
static const std::string NEST_COOL_TAG              = "can_cool";
static const std::string NEST_HEAT_TAG              = "can_heat";
static const std::string NEST_FAN_TAG               = "has_fan";
static const std::string NEST_HUMIDITY_TAG          = "humidity";
static const std::string NEST_TEMP_SCALE            = "temperature_scale";
static const std::string NEST_HVAC_MODE_TAG         = "hvac_mode";
static const std::string NEST_TARGET_TEMP_C_TAG     = "target_temperature_c";
static const std::string NEST_TARGET_TEMP_F_TAG     = "target_temperature_f";
static const std::string NEST_TARGET_TEMP_HIGH_C_TAG= "target_temperature_high_c";
static const std::string NEST_TARGET_TEMP_HIGH_F_TAG= "target_temperature_high_f";
static const std::string NEST_TARGET_TEMP_LOW_C_TAG = "target_temperature_low_c";
static const std::string NEST_TARGET_TEMP_LOW_F_TAG = "target_temperature_low_f";
static const std::string NEST_AMBIENT_TEMP_C_TAG    = "ambient_temperature_c";
static const std::string NEST_AMBIENT_TEMP_F_TAG    = "ambient_temperature_f";
static const std::string NEST_AWAY_TEMP_HIGH_C_TAG  = "away_temperature_high_c";
static const std::string NEST_AWAY_TEMP_HIGH_F_TAG  = "away_temperature_high_f";
static const std::string NEST_AWAY_TEMP_LOW_C_TAG   = "away_temperature_low_c";
static const std::string NEST_AWAY_TEMP_LOW_F_TAG   = "away_temperature_low_f";
static const std::string NEST_FAN_TIMER_ACTIVE_TAG  = "fan_timer_active";
static const std::string NEST_LEAF_TAG              = "has_leaf";
static const std::string NEST_EMERGENCY_HEAT_TAG    = "is_using_emergency_heat";
static const std::string NEST_AWAY_TAG              = "away";
static const std::string NEST_HOME_TAG              = "home";
static const std::string NEST_POSTAL_TAG            = "postal_code";
static const std::string NEST_COUNTRY_TAG           = "country_code";
static const std::string NEST_TIMEZONE_TAG          = "time_zone";

// Other constants
static const int NEST_ACCESS_TOKEN_LEN              = 512;
static const int NEST_ACCESS_TOKEN_EXPIRY           = 128;
static const std::string NEST_HVAC_HEAT             = "heat";
static const std::string NEST_HVAC_COOL             = "cool";
static const std::string NEST_HVAC_MIXED            = "heat-cool";
static const std::string NEST_HVAC_OFF              = "off";
static const std::string NEST_TEMP_SCALE_C          = "C";
static const std::string NEST_TEMP_SCALE_F          = "F";

// Other definitions
static const std::string NEST_STRUCTURE_AUTH_STR    = "/structures?auth=";
static const std::string NEST_CLIENT_ID_STR         = "?client_id=";
static const std::string NEST_CODE_STR              = "&code=";
static const std::string NEST_CLIENT_SECRET_STR     = "&client_secret=";
static const std::string NEST_AUTH_CODE_STR         = "&grant_type=authorization_code";
static const std::string NEST_DEVICES_AUTH_STR      = "/devices?auth=";
static const std::string NEST_DEVICES_THERMOSTAT_STR= "/devices/thermostats/";
static const std::string NEST_AUTH_STR              = "?auth=";

#endif /* __NEST_DEFS_H__ */
