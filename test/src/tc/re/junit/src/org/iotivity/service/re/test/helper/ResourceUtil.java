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

package org.iotivity.service.re.test.helper;

public interface ResourceUtil {
  String TEMP_SENSOR_URI = "/a/TempSensor";
  String TEMP_SENSOR_TYPE = "oic.r.temperature.sensor";
  String DEFAULT_INTERFACE = "oic.if.";
  String BASELINE_INTERFACE = "oic.if.baseline";
  String LIGHT_URI = "/device/tube-light";
  String LIGHT_TYPE = "core.light";
  String FAN_URI = "/device/table-fan";
  String FAN_TYPE = "core.fan";
  String ATTR_KEY_POWER = "power";
  String ATTR_VALUE_POWER_ON = "on";
  String ATTR_VALUE_POWER_OFF = "off";
  String ATTR_INTENSITY_KEY = "intensity";
  String ATTR_INTENSITY_VALUE = "10";
  String DEFAULT_EMPTY_STRING_VALUE = "";

  int CALLBACK_WAIT_ONE = 1;
  int CALLBACK_WAIT_MIN = 2;
  int CALLBACK_WAIT_MAX = 5;
}
