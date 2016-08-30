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

package org.iotivity.service.re;

public interface REUtility {
    String TAG = "RETestApp";
    int WAIT_TIME_MAX = 10;
    int WAIT_TIME_MIN = 1;
    int WAIT_TIME_DEFAULT = 3;

    String BINARY_SWITCH_URI = "/retestapp/airConditioner/bin-switch";
    String TEMPERATURE_SENSOR_URI = "/retestapp/airConditioner/temperature-sensor";
    String TEMPERATURE_ACTUATOR_URI = "/retestapp/airConditioner/temperature-actuator";
    String BINARY_SWITCH_TYPE = "oic.r.switch.binary";
    String TEMPERATURE_RES_TYPE = "oic.r.temperature";
    String DEFAULT_SENSOR_INTERFACE = "oic.if.s";
    String DEFAULT_ACTUATOR_INTERFACE = "oic.if.a";
    String BINARY_SWITCH_KEY = "value";
    String TEMPERATURE_TEMP_KEY = "temperature";
    String TEMPERATURE_UNITS_KEY = "units";
    String TEMPERATURE_RANGE_KEY = "range";
}