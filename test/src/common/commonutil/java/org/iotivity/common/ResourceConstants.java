/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 * <p/>
 * <p/>
 * <p/>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p/>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p/>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.common;

public class ResourceConstants {
    public static final String DEFAULT_DEVICE_NAME = "Vendor Default Smart Home Device";
    public static final String TV_DEVICE_URI = "/TV-1";
    public static final String TV_SWITCH_URI = "/BinarySwitchResURI";
    public static final String TV_AUDIO_URI = "/AudioResURI";
    public static final String TV_MEDIA_SOURCE_LIST_URI = "/mediaSourceListResURI";

    public static final String AC_DEVICE_URI = "/AirCon-1";
    public static final String AC_SWITCH_URI = "/BinarySwitchResURI";
    public static final String AC_TEMPERATURE_URI = "/TemperatureResURI";
    public static final String AC_AIR_FLOW_URI = "/AirFlowResURI";
    public static final String AC_TIMER_URI = "/Vendor/AirConditioner/TimerClock";
    public static final String AC_CHILD_LOCK_URI = "/Vendor/AirConditioner/ChildLock";

    public static final String DEVICE_TYPE_DEFAULT = "oic.wk.d";
    public static final String DEVICE_TYPE_TV = "oic.d.tv";
    public static final String DEVICE_TYPE_AC = "oic.d.airconditioner";
    public static final String SWITCH_RESOURCE_TYPE = "oic.r.switch.binary";
    public static final String AUDIO_RESOURCE_TYPE = "oic.r.audio";
    public static final String MEDIA_SOURCE_LIST_RESOURCE_TYPE = "oic.r.mediasourcelist";
    public static final String TEMPERATURE_RESOURCE_TYPE = "oic.r.temperature";
    public static final String AIR_FLOW_RESOURCE_TYPE = "oic.r.airflow";
    public static final String TIMER_RESOURCE_TYPE = "x.com.vendor.timer";
    public static final String CHILD_LOCK_RESOURCE_TYPE = "x.com.vendor.child.lock";

    public static final String TV_DEVICE_INTERFACE = "oic.if.a";
    public static final String AC_DEVICE_INTERFACE = "oic.if.a";
    public static final String SWITCH_RESOURCE_INTERFACE = "oic.if.a oic.if.baseline";
    public static final String AUDIO_RESOURCE_INTERFACE = "oic.if.a oic.if.baseline";
    public static final String MEDIA_SOURCE_LIST_RESOURCE_INTERFACE = "oic.if.a oic.if.baseline";
    public static final String TEMPERATURE_RESOURCE_INTERFACE = "oic.if.a oic.if.baseline";
    public static final String AIR_FLOW_RESOURCE_INTERFACE = "oic.if.a oic.if.baseline";
    public static final String TIMER_RESOURCE_INTERFACE = "oic.if.a oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline";
    public static final String CHILD_LOCK_RESOURCE_INTERFACE = "oic.if.a oic.if.baseline";
    public static final String READ_ONLY_INTERFACE = "oic.if.r";
    public static final String SENSOR_INTERFACE = "oic.if.s";

    public static final String ACTUATOR_INTERFACE = "oic.if.a";
    public static final String LIGHT_1_URI = "/device/light-1";
    public static final String LIGHT_2_URI = "/device/light-2";
    public static final String LIGHT_3_URI = "/device/light-3";
    public static final String LIGHT_SECURED_URI = "/device/light-1";
    public static final String FAN_1_URI = "/device/fan-1";
    public static final String FAN_2_URI = "/device/fan-2";
    public static final String FAN_SECURED_URI = "/device/fan-1";
    public static final String ROOM_1_URI = "/store/room-1";
    public static final String ROOM_2_URI = "/store/room-2";
    public static final String LIGHT_INVISIBLE_URI = "/device/light-invisible";
    public static final String FAN_INVISIBLE_URI = "/device/fan-invisible";
    public static final String RESOURCE_TYPE_LIGHT = "core.light core.brightlight";
    public static final String RESOURCE_TYPE_FAN = "core.fan core.table-fan";
    public static final String GROUP_TYPE_ROOM = "oic.wk.col";
    public static final String RESOURCE_TYPE_ROOM = "core.room";
    public static final String SERVER_IP_V4 = "0.0.0.0";
    public static final String SERVER_IP_V6 = ":::::";
    public static final int SERVER_PORT = 0;
    public static final int MAX_LIGHT_RESOURCE_COUNT = 100;
    public static final int MAX_ATTRIBUTE_VALUE_LENGTH = 2048;

    public static final int CALLBACK_WAIT_DEFAULT = 5;
    public static final int CALLBACK_WAIT_MAX = 10;
    public static final int CALLBACK_WAIT_MIN = 1;
    public static final int CALLBACK_WAIT_NONE = 0;
    public static final int SUCCESS_RESPONSE = 0;

    public static final int OIC_SUCCESS_RESPONSE = 200;
    public static final int OIC_SUCCESS_PUT_RESPONSE = 203;
    public static final int OIC_SUCCESS_POST_RESPONSE = 204;
    public static final int OIC_SUCCESS_GET_RESPONSE = 205;
    public static final int OIC_ERROR_RESPONSE = 400;
    public static final int OIC_RESOURCE_NOT_FOUND_RESPONSE = 404;

    public static final String DEFAULT_POWER_STATE = "off";
    public static final int DEFAULT_INTENSITY = 10;
    public static final String DEFAULT_MANUFACTURER = "Vendor";
    public static final String DEFAULT_REGION = "Dhaka, Bangladesh";
    public static final String DEFAULT_FACTORY_RESET_STATE = "false";
    public static final String DEFAULT_REBOOT_STATE = "false";
    public static final String DEFAULT_VERSION = "1.0";
    public static final double DEFAULT_ACCURACY = 0.85;
    public static final boolean DEFAULT_CRUDN_SUPPORT = true;

    public static final String URI_KEY = "href";
    public static final String POWER_KEY = "power";
    public static final String INTENSITY_KEY = "intensity";
    public static final String MANUFACTURER_KEY = "manufacturer";
    public static final String REGION_KEY = "region";
    public static final String FACTORY_RESET_KEY = "value";
    public static final String REBOOT_KEY = "rb";
    public static final String ACTIONSET_KEY = "ActionSet";
    public static final String CANCEL_ACTIONSET_KEY = "CancelAction";
    public static final String GET_ACTIONSET_KEY = "GetActionSet";
    public static final String EXECUTE_ACTIONSET_KEY = "DoAction";
    public static final String EXECUTE_SCHEDULED_ACTIONSET_KEY = "DoScheduledAction";
    public static final String DELETE_ACTIONSET_KEY = "DelActionSet";
    public static final String VERSION_KEY = "ver";
    public static final String ACCURACY_KEY = "accuracy";
    public static final String CRUDN_SUPPORT_KEY = "isCRUDN";
    public static final String RESOURCE_TYPE_KEY = "rt";
    public static final String INTERFACE_KEY = "if";

    public static final String BINARY_SWITCH_KEY = "value";
    public static final boolean BINARY_SWITCH_VALUE = true;

    public static final String TEMPERATURE_RANGE_KEY = "range";
    public static final String TEMPERATURE_UNITS_KEY = "units";
    public static final String TEMPERATURE_UNITS_SUPPORTED_VALUE = "C";
    public static final double TEMPERATURE_RANGE_START_VALUE = 10.01;
    public static final double TEMPERATURE_RANGE_END_VALUE = 40.01;
    public static final String TEMPERATURE_KEY = "temperature";
    public static final double TEMPERATURE_VALUE = 15.05;

    public static final String AIRFLOW_RANGE_KEY = "range";
    public static final String AIRFLOW_SPEED_KEY = "speed";
    public static final String AIRFLOW_DIRECTION_KEY = "direction";
    public static final int AIRFLOW_SPEED_VALUE = 20;
    public static final String AIRFLOW_DIRECTION_VALUE = "up";
    public static final int AIRFLOW_SPEED_RANGE_START_VALUE = 0;
    public static final int AIRFLOW_SPEED_RANGE_END_VALUE = 50;

    public static final String TIMER_HOUR_KEY = "x.com.vendor.timer.hour";
    public static final String TIMER_MINUTE_KEY = "x.com.vendor.timer.minute";
    public static final String TIMER_SECOND_KEY = "x.com.vendor.timer.second";
    public static final String TIMER_RESET_KEY = "x.com.vendor.timer.reset";
    public static final int TIMER_HOUR_VALUE = 10;
    public static final int TIMER_MINUTE_VALUE = 30;
    public static final int TIMER_SECOND_VALUE = 30;
    public static final boolean TIMER_RESET_VALUE = true;

    public static final String CHILD_LOCK_KEY = "x.com.vendor.child.lock.value";
    public static final boolean CHILD_LOCK_VALUE = false;

    public static final String NEW_RESOURCE_URI = "/new-resource/created-light-1";

    public static final String PLATFORM_ID = "436f6e66-6f72-6d61-6e63-6553696d756c";
    public static final String MANUFACTURER_NAME = DEFAULT_MANUFACTURER;
    public static final String MANUFACTURER_URL = "www.default-vendor.com";
    public static final String MODEL_NUMBER = "ABCDE00004";
    public static final String DATE_OF_MANUFACTURE = "2016-06-01";
    public static final String PLATFORM_VERSION = "0.0.1";
    public static final String OPERATING_SYSTEM_VERSION = "10";
    public static final String HARDWARE_VERSION = "1.1.0";
    public static final String FIRMWARE_VERSION = "1.1.1";
    public static final String SUPPORT_URL = "support.default-vendor.com";
    public static final String SYSTEM_TIME = "2016-06-20T10:10:10Z";
    public static final String DEVICE_NAME = "IotivitySmartRoom";

    public static final String CORE_SPEC_VERSION = "core.1.1.0";
    public static final String RESOURCE_TYPE_SPEC_VERSION = "res.1.1.0";
    public static final String SMART_HOME_SPEC_VERSION = "SH.1.1.0";

    public static final int COAP_RESPONSE_CODE_SUCCESS = 205;
    public static final int COAP_RESPONSE_CODE_CREATED = 201;
    public static final int COAP_RESPONSE_CODE_DELETED = 202;
    public static final int COAP_RESPONSE_CODE_UPDATED = 204;
    public static final int COAP_RESPONSE_CODE_RETRIEVED = 205;
    public static final int COAP_RESPONSE_CODE_ERROR = 400;
    public static final int COAP_RESPONSE_CODE_RESOURCE_UNAUTHORIZED = 401;
    public static final int COAP_RESPONSE_CODE_RESOURCE_NOT_FOUND = 404;
    public static final int COAP_RESPONSE_CODE_METHOD_NOT_ALLOWED = 405;
    public static final String DEFAULT_INTERFACE = "oic.if.baseline";
    public static final String SERVER_DATABASE_FILE_NAME = "oic_svr_db_server.dat";
    public static final String CLIENT_DATABASE_FILE_NAME = "oic_svr_db_client.dat";
}
