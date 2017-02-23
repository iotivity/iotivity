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

#ifndef __HUE_DEFS_H__
#define __HUE_DEFS_H__

#include <string>

#define HUE_TIMESLICE               2

/**
 * Constants for the Philiphs Hue Data Model
 */
static const std::string DM_STATE_BRI           = "bri";
static const std::string DM_STATE_SAT           = "sat";
static const std::string DM_STATE_CSC           = "xy";
static const std::string DM_STATE_HUE           = "hue";
static const std::string DM_STATE_POWERED       = "on";
static const std::string DM_NAME                = "name";
static const std::string DM_TYPE                = "type";
static const std::string DM_MODEL_ID            = "modelid";
static const std::string DM_UNIQUE_ID           = "uniqueid";
static const std::string DM_VERSION             = "swversion";
static const std::string DM_STATE               = "state";
static const std::string DM_STATE_REACHABLE     = "reachable";
static const std::string DM_STATE_EFFECT        = "effect";
static const std::string DM_STATE_CT            = "ct";
static const std::string DM_STATE_ALERT         = "alert";
static const std::string DM_STATE_COLORMODE     = "colormode";
static const std::string DM_POINT_SYMBOL        = "pointsymbol";

/**
 * Bridge Constants
 */
static std::string BRIDGE_NUPNP_URI             = "https://www.meethue.com/api/nupnp";
static const std::string BRIDGE_ID              = "bridgeid";
static const std::string BRIDGE_IP              = "ipaddress";
static const std::string BRIDGE_MAC             = "mac";
static const std::string BRIDGE_NAME            = "name";
static const std::string BRIDGE_USERNAME        = "mytestapplication";
static const std::string BRIDGE_SW              = "swversion";

#endif /* __HUE_DEFS_H__ */
