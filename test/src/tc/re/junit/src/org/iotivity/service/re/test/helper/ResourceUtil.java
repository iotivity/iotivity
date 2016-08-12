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

public class ResourceUtil {
  public static final String TEMP_SENSOR_URI      = "/a/TempSensor";
  public static final String TEMP_SENSOR_TYPE     = "oic.r.temperature.sensor";
  public static final String INTERFACE            = "oic.if.";
  public static final String BASELINE_INTERFACE   = "oic.if.baseline";

  public static final String LIGHT_URI            = "/device/tube-light";
  public static final String LIGHT_TYPE           = "core.light";
  public static final String FAN_URI              = "/device/table-fan";
  public static final String FAN_TYPE             = "core.fan";

  public static final String ATTR_KEY_POWER       = "power";
  public static final String ATTR_VALUE_POWER_ON  = "on";
  public static final String ATTR_VALUE_POWER_OFF = "off";
  public static final String ATTR_INTENSITY_KEY   = "intensity";
  public static final String ATTR_INTENSITY_VALUE = "10";

  public static final int    CALLBACK_WAIT_ONE    = 1;
  public static final int    CALLBACK_WAIT_MIN    = 2;
  public static final int    CALLBACK_WAIT_MAX    = 5;
}
