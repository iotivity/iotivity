/******************************************************************

*

* Copyright 2016 Samsung Electronics All Rights Reserved.

*

*

*

* Licensed under the Apache License, Version 2.0 (the "License");

* you may not use this file except in compliance with the License.

* You may obtain a copy of the License at

*

* http://www.apache.org/licenses/LICENSE-2.0

*

* Unless required by applicable law or agreed to in writing, software

* distributed under the License is distributed on an "AS IS" BASIS,

* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

* See the License for the specific language governing permissions and

* limitations under the License.

*

******************************************************************/

package org.iotivity.service.rc.test.helper;

public final class RCCommonUtilBTC {
	public static final String TAG = "RCTest";
	public static final String EMPTY_STRING = "";
	public static final String RESOURCE_CONTAINER_EMPTY_CONFIG_XML = "/ResourceContainerEmptyConfig.xml";
	public static final String RESOURCE_CONTAINER_CONFIG_XML = "/ResourceContainerConfig.xml";
	public static final String NULL_KEY_MESSAGE = "key is null";

	public static final String BMI_BUNDLE_ID = "oic.bundle.BMISensor";
	public static final String BMI_BUNDLE_URI = "bmi/bundle/uri";
	public static final String BMI_BUNDLE_VERSION = "1.0.0";
	public static final String BMI_BUNDLE_ACTIVATOR = "bmisensor";

	public static final String DI_BUNDLE_ID = "oic.bundle.discomfortIndexSensor";
	public static final String DI_SENSOR_KEY = "discomfortIndex";
	public static final String DI_SENSOR_NAME = "discomfortIndex";
	public static final String DI_SENSOR_TYPE = "oic.r.discomfortindex";
	public static final String DI_BUNDLE_RESOURCE1_NAME = "DiscomfortIndexSensor1";
	public static final String DI_BUNDLE_RESOURCE1_NAME_SET = "DiscomfortIndexSensor1_SET";
	public static final String DI_BUNDLE_RESOURCE1_URI = "oic/DiscomfortRes";
	public static final String DI_BUNDLE_RESOURCE1_URI_SET = "oic/DiscomfortRes_SET";
	public static final String DI_RESOURCE_TYPE = "oic.r.sensor";
	public static final String DI_RESOURCE_TYPE_SET = "oic.r.sensor_set";
	public static final String DI_RESOURCE_ADDRESS = "107.109.9.85";
	public static final String DI_RESOURCE_ADDRESS_SET = "107.109.9.84";

	public static final String HUE_BUNDLE_ID = "oic.bundle.hueJavaSample";
	public static final String HUE_BUNDLE_JAR = "hue-0.1.jar";
	public static final String HUE_BUNDLE_URI = "/hueJava/light/1";
	public static final String HUE_BUNDLE_VERSION = "1.0.0";
	public static final String HUE_BUNDLE_ACTIVATOR = "org.iotivity.bundle.hue.HueBundleActivator";
	public static final String HUE_BUNDLE_RESOURCE_TYPE = "oic.r.light";
	public static final String HUE_BUNDLE_RESOURCE_ADDRESS = "http://192.168.0.2/lights/1";

	public static final String HUMIDITY_SENSOR_KEY = "humidity";
	public static final String HUMIDITY_SENSOR_NAME = "humiditySensor";
	public static final String HUMIDITY_SENSOR_TYPE = "oic.r.humidity";

	public static final String LIGHT_SENSOR_KEY = "intensity";
	public static final String LIGHT_SENSOR_NAME = "lightSensor";
	public static final String LIGHT_SENSOR_TYPE = "oic.r.lightsensor";

	public static final String TEMPERATURE_SENSOR_KEY = "temperature";
	public static final String TEMPERATURE_SENSOR_NAME = "temperatureSensor";
	public static final String TEMPERATURE_SENSOR_TYPE = "oic.r.temperature";

}
