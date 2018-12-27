//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef REUTILITY_H_
#define REUTILITY_H_

#include <iomanip>
#include <iostream>

#include "ocstack.h"
#include "octypes.h"
#include "OCPlatform.h"
#include "RCSAddress.h"
#include "RCSDiscoveryManager.h"
#include "RCSException.h"
#include "RCSRemoteResourceObject.h"
#include "RCSRequest.h"
#include "RCSResourceAttributes.h"
#include "RCSResourceObject.h"
#include "RCSResponse.h"
#include "RCSRepresentation.h"

using namespace OC;
using namespace OIC;
using namespace OIC::Service;
using namespace std;

#define WAIT_TIME_MAX 10
#define WAIT_TIME_MIN 1

#define BINARY_SWITCH_URI "/retestapp/airConditioner/bin-switch"
#define TEMPERATURE_SENSOR_URI "/retestapp/airConditioner/temperature-sensor"
#define TEMPERATURE_ACTUATOR_URI "/retestapp/airConditioner/temperature-actuator"
#define BINARY_SWITCH_TYPE "oic.r.switch.binary"
#define TEMPERATURE_RES_TYPE "oic.r.temperature"
#define DEFAULT_SENSOR_INTERFACE "oic.if.s"
#define DEFAULT_ACTUATOR_INTERFACE "oic.if.a"
#define BINARY_SWITCH_KEY "value"
#define TEMPERATURE_TEMP_KEY "temperature"
#define TEMPERATURE_UNITS_KEY "units" // format "C" or "F" or "K"
#define TEMPERATURE_RANGE_KEY "range" // format "0,100"

typedef enum
{
    EXIT,

    CREATE_RESOURCE,
    SERVER_SET_ATTRIBUTES,
    SERVER_GET_ATTRIBUTES,

    DISCOVER_RESOURCE_WITHOUT_QUERY,
    DISCOVER_RESOURCE_WITH_TYPE_QUERY,
    DISCOVER_RESOURCE_WITH_TYPES_QUERY,
    DISCOVER_NON_DISCOVERABLE_RESOURCE,
    START_MONITORING,
    STOP_MONITORING,
    GET_STATE,
    SET_REMOTE_ATTRIBUTES,
    GET_REMOTE_ATTRIBUTES,
    START_CACHING,
    STOP_CACHING,
    GET_CACHED_ATTRIBUTES,
    GET_CACHED_STATE
} ApplicationMenu;

#endif /* REUTILITY_H_ */
