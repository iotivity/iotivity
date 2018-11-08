/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

#ifndef __RESOURCE_HELPER_H__
#define __RESOURCE_HELPER_H__

#ifdef __WINDOWS__
#undef _WINSOCKAPI_
#define _WINSOCKAPI_
#include <windows.h>
#endif

#ifdef __LINUX__
#include <execinfo.h>
#include "time.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <mutex>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <string>
#include <stdexcept>

#include "OCPlatform.h"
#include "rd_client.h"

using namespace OC;
using namespace std;
namespace PH = std::placeholders;

#define TV_Device_URI "/TV-1"
#define TV_SWITCH_URI "/BinarySwitchResURI"
#define TV_AUDIO_URI "/AudioResURI"
#define TV_MEDIA_SOURCE_LIST_URI "/mediaSourceListResURI"

#define COLLECTION_URI "/collection"
#define COLLECTION_SWITCH_URI "/collectionBinarySwitchResURI"

#define COLLECTION_RESOURCE_URI "/vendor/aircon/collection/extra"
#define COLLECTION_RESOURCE_URI_VENDOR "/vendor/aircon/collection"

#define AC_Device_URI  "/AirCon-1"
#define AC_SWITCH_URI "/BinarySwitchResURI"
#define AC_TEMPERATURE_URI "/TemperatureResURI"
#define AC_SENSOR_URI "/TemperatureSensorURI"
#define AC_CHROMA_URI "/ChromaResURI"
#define AC_DIMMING_URI "/DimmingResURI"
#define AC_AIR_FLOW_URI "/AirFlowResURI"
#define AC_SWITCH_URI_CHILD "/BinarySwitchResURI/Child"
#define AC_TEMPERATURE_URI_CHILD "/TemperatureResURI/Child"
#define AC_AIR_FLOW_URI_CHILD "/AirFlowResURI/Child"
#define AC_TIMER_URI "/Vendor/AirConditioner/TimerClock"
#define AC_CHILD_LOCK_URI "/Vendor/AirConditioner/ChildLock"
#define AC_SWING_URI "/Vendor/AirConditioner/Swinger"
#define AC_TIMER_URI_CHILD "/Vendor/AirConditioner/TimerClock/Children"
#define AC_SWING_URI_CHILD "/Vendor/AirConditioner/Swinger/Children"
#define AC_CON_URI "/Vendor/AirConditioner/ConfigurationResURI"

#define CON_URI "/ConfigurationResURI"

#define MN_MAINTENANCE_URI "/oic/mnt"

#define NMON_NETWORK_MONITORING_URI "/nmonResURI"

#define EXTRA_COLLECTION_URI "/bridge/root"
#define EXTRA_LIGHT_URI "/bridge/light"
#define EXTRA_SWITCH_URI "/bridge/binary-switch"
#define EXTRA_BRIGHTNESS_URI "/bridge/light-brightness"

#define COAP_CLOUD_CONF_URI "/CoAPCloudConf"

#define Device_TYPE_TV "oic.d.tv"
#define Device_TYPE_LIGHT "oic.d.light"
#define Device_TYPE_AC "oic.d.airconditioner"
#define Device_TYPE_AP "oic.d.airpurifier"
#define Device_TYPE_BPM "oic.d.bloodpressuremonitor"
#define Device_TYPE_VENDOR "x.com.vendor.device.eco.power"
#define SWITCH_RESOURCE_TYPE "oic.r.switch.binary"
#define MAINTENANCE_RESOURCE_TYPE "oic.wk.mnt"
#define NETWORK_MONITORING_RESOURCE_TYPE "oic.wk.nmon"
#define COAP_CLOUD_CONF_RESOURCE_TYPE "oic.r.coapcloudconf"
#define AUDIO_RESOURCE_TYPE "oic.r.audio"
#define MEDIA_SOURCE_LIST_RESOURCE_TYPE "oic.r.mediasourcelist"
#define TEMPERATURE_RESOURCE_TYPE "oic.r.temperature"
#define AIR_FLOW_RESOURCE_TYPE "oic.r.airflow"
#define BRIGHTNESS_RESOURCE_TYPE "oic.r.light.brightness"
#define TIMER_RESOURCE_TYPE "x.com.vendor.timer"
#define CHILD_LOCK_RESOURCE_TYPE "x.com.vendor.child.lock"
#define SWING_RESOURCE_TYPE "x.com.vendor.swing"
#define CON_RESOURCE_TYPE "oic.wk.con"
#define CHROMA_RESOURCE_TYPE "oic.r.colour.chroma"
#define DIMMING_RESOURCE_TYPE "oic.r.light.dimming"

#define TV_Device_INTERFACE "oic.if.a"
#define AC_Device_INTERFACE "oic.if.a"
#define SWITCH_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define MAINTENANCE_RESOURCE_INTERFACE "oic.if.rw oic.if.baseline"
#define NETWORK_MONITORING_RESOURCE_INTERFACE "oic.if.rw oic.if.baseline"
#define COAP_CLOUD_CONF_RESOURCE_INTERFACE "oic.if.rw oic.if.baseline"
#define AUDIO_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define MEDIA_SOURCE_LIST_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define TEMPERATURE_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define TEMPERATURE_SENSOR_INTERFACE "oic.if.s oic.if.baseline"
#define AIR_FLOW_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define BRIGHTNESS_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define LIGHT_DEVICE_INTERFACE "oic.if.r oic.if.baseline"
#define TIMER_RESOURCE_INTERFACE "oic.if.a oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline"
#define CHILD_LOCK_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define SWING_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define CON_RESOURCE_INTERFACE "oic.if.rw oic.if.baseline"
#define CHROMA_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define DIMMING_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"

#define ACTUATOR_INTERFACE "oic.if.a"
#define READ_ONLY_INTERFACE "oic.if.r"
#define LIGHT_1_URI "/device/light-1"
#define LIGHT_2_URI "/device/light-2"
#define LIGHT_3_URI "/device/light-3"
#define LIGHT_SECURED_URI "/device/light-1"
#define FAN_1_URI "/device/fan-1"
#define FAN_2_URI "/device/fan-2"
#define FAN_SECURED_URI "/device/fan-1"
#define ROOM_1_URI "/store/room-1"
#define ROOM_2_URI "/store/room-2"
#define LIGHT_INVISIBLE_URI "/device/light-invisible"
#define FAN_INVISIBLE_URI "/device/fan-invisible"
#define RESOURCE_TYPE_LIGHT "core.light core.brightlight"
#define RESOURCE_TYPE_FAN "core.fan core.table-fan"
#define GROUP_TYPE_DEFAULT "oic.wk.col"
#define GROUP_TYPE_AIRCON "x.com.vendor.aircon.collection.extra"
#define GROUP_TYPE_AIRCON_VENDOR "x.com.vendor.aircon.collection"
#ifdef GROUP_NAME
#undef GROUP_NAME
#endif
#define GROUP_NAME "AirCon Collection"
#define RESOURCE_TYPE_ROOM "core.room"
#define SERVER_IP_V4 "0.0.0.0"
#define SERVER_IP_V6 ":::::"
#define SERVER_PORT 0
#define MAX_LIGHT_RESOURCE_COUNT 100
#define MAX_ATTRIBUTE_VALUE_LENGTH 2048

const int CALLBACK_WAIT_DEFAULT = 5;
const int CALLBACK_WAIT_MAX = 10;
const int CALLBACK_WAIT_MIN = 1;
const int CALLBACK_WAIT_NONE = 0;
const int SUCCESS_RESPONSE = 0;

#define OIC_SUCCESS_RESPONSE 200
#define OIC_SUCCESS_PUT_RESPONSE 203
#define OIC_SUCCESS_POST_RESPONSE 204
#define OIC_SUCCESS_GET_RESPONSE 205
#define OIC_ERROR_RESPONSE 400
#define OIC_RESOURCE_NOT_FOUND_RESPONSE 404

#define DEFAULT_POWER_STATE "off"
#define DEFAULT_INTENSITY 10
#define DEFAULT_MANUFACTURER "Vendor"
#define DEFAULT_REGION "রাজশাহী, Bangladesh "
#define DEFAULT_FACTORY_RESET_STATE "false"
#define DEFAULT_REBOOT_STATE "false"
#define DEFAULT_VERSION 1.0
#define DEFAULT_ACCURACY 0.85
#define DEFAULT_CRUDN_SUPPORT true
#define DEFAULT_BRIGHTNESS_VALUE 10
#define LATTITUDE_VALUE 23.50
#define LONGITUDE_VALUE 90.10
#define LOCATION_NAME_VALUE "Guest Room "
#define CURRENCY_VALUE "BDT"
#define BANGLA_VALUE "bn"
#define ENGLISH_VALUE "en"
#define DEFAULT_LANGUAGE_VALUE "en"
#define BANGLA_VALUE "bn"
#define ENGLISH_VALUE "en"
#define BANGLA_NAME_VALUE "বুদ্ধিমান এয়ার কন্ডিশনার"
#define ENGLISH_NAME_VALUE "Vendor Smart Home AirCon Device"
#define AP_ENGLISH_NAME_VALUE "Vendor Smart Home Air Purifier Device"
#define LANGUAGE_VALUE "language"
#define CIS_VALUE "coaps+tcp://192.168.56.1:49160"
#define SID_VALUE "bd052d57-aa22-425f-9dc0-e18202f4b7a2"
#define AT_VALUE "0f3d9f7fe5491d54077d"
#define APN_VALUE ""

#define VOLUME_KEY "volume"
#define MUTE_KEY "mute"
#define SOURCE_KEY "source"
#define SOURCE_NAME_KEY "sourceName"
#define SOURCE_NUMBER_KEY "sourceNumber"
#define SOURCE_TYPE_KEY "sourceType"
#define STATUS_KEY "status"
#define REPRESENTATION_KEY "rep"
#define URI_KEY "href"
#define POWER_KEY "power"
#define INTENSITY_KEY "intensity"
#define MANUFACTURER_KEY "manufacturer"
#define REGION_KEY "r"
#define LOCATION_KEY "loc"
#define LOCATION_NAME_KEY "locn"
#define CURRENCY_KEY "c"
#define LANGUAGE_KEY "ln"
#define DEFAULT_LANGUAGE_KEY "dl"
#define FACTORY_RESET_KEY "value"
#define FACTORY_KEY "fr"
#define REBOOT_KEY "rb"
#define HTML_ERR_KEY "err"
#define RESET_KEY "reset"
#define COLLECTING_VALUES "col"
#define IANA_NETWORK_CONNECTION_KEY "ianaifType"
#define TRANSMITTED_KB_FROM_COL_KEY "tx"
#define MAX_TRANSMITTED_MSG_BYTES_IN_COL_KEY "mmstx"
#define AVG_TRANSMITTED_MSG_BYTES_IN_COL_KEY "amstx"
#define RECEIVED_KB_FROM_COL_KEY "rx"
#define MAX_RECEIVED_MSG_BYTES_IN_COL_KEY "mmsrx"
#define AVG_RECEIVED_MSG_BYTES_IN_COL_KEY "amsrx"
#define CIS_KEY "cis"
#define SID_KEY "sid"
#define AT_KEY "at"
#define APN_KEY "apn"
#define CLEC_KEY "clec"
#define ACTIONSET_KEY "ActionSet"
#define CANCEL_ACTIONSET_KEY "CancelAction"
#define GET_ACTIONSET_KEY "GetActionSet"
#define EXECUTE_ACTIONSET_KEY "DoAction"
#define EXECUTE_SCHEDULED_ACTIONSET_KEY "DoScheduledAction"
#define DELETE_ACTIONSET_KEY "DelActionSet"
#define VERSION_KEY "ver"
#define ACCURACY_KEY "accuracy"
#define CRUDN_SUPPORT_KEY "isCRUDN"
#define RESOURCE_TYPE_KEY "rt"
#define INTERFACE_KEY "if"
#define NAME_KEY "n"
#define TEMPERATURE_KEY "temperature"
#define SPEED_KEY "speed"
#define DIRECTION_KEY "direction"
#define ON_OFF_KEY "value"
#define BRIGHTNESS_KEY "brightness"
#define BITMASK_KEY "bm"
#define PORT_KEY "port"
#define POLICY_KEY "p"
#define SECURITY_KEY "sec"
#define DEVICE_ID_KEY "di"
#define LINKS_KEY "links"
#define ANCHOR_KEY "anchor"
#define ANCHOR_DEFAULT_VALUE "ocf://"
#define EPS_KEY "eps"
#define EP_KEY "ep"
#define EP_DEFAULT_VALUE "coaps://"
#define PRI_KEY "pri"
#define PLATFORM_ID_KEY "pi"
#define PIID_KEY "piid"
#define DMV_KEY "dmv"
#define ICV_KEY "icv"
#define MANUFACTURER_NAME_KEY "dmn"
#define UNITS_KEY "units"
#define RANGE_KEY "range"
#define TIMER_HOUR_KEY "x.com.vendor.timer.hour"
#define TIMER_MINUTE_KEY "x.com.vendor.timer.minute"
#define TIMER_SECOND_KEY "x.com.vendor.timer.second"
#define TIMER_RESET_KEY "x.com.vendor.timer.reset"
#define DEFAULT_TIME_VALUE 10

#define CHROMA_CSC_KEY "csc"
#define CHROMA_CT_KEY "ct"
#define CHROMA_HUE_KEY "hue"
#define CHROMA_SATURATION_KEY "saturation"
#define DIMMING_SETTING_KEY "dimmingSetting"
#define UNITS_VALUE "C"
#define HIGHEST_TEMPERATURE_VALUE 40.99
#define LOWEST_TEMPERATURE_VALUE 10.01
#define DEFAULT_TEMPERATURE_VALUE 24.50
#define DEFAULT_DIRECTION_VALUE "up"
#define HIGHEST_AIR_FLOW_VALUE 30
#define LOWEST_AIR_FLOW_VALUE 0
#define DEFAULT_SPEED_VALUE 10

#define CSC_X_VALUE 0.25
#define CSC_Y_VALUE 0.63
#define CT_VALUE 400
#define HUE_VALUE 20.2
#define SATURATION_VALUE 80
#define DIMMING_VALUE 80
#define PRI_DEFAULT_VALUE 1
#define HTML_ERR_DEFAULT_VALUE 501
#define IANA_NETWORK_CONNECTION_DEFAULT_VALUE 71
#define SWING_STATE_KEY "x.com.vendor.swing.on"
#define SWING_STATE_VALUE false
#define SWING_MOVEMENT_KEY "x.com.vendor.swing.blade.পরিবর্তনের.দিক"
#define SWING_MOVEMENT_VALUE "আনুভূমিক"
#define SWING_MOVEMENT_OTHER_VALUE "উল্লম্ব"
#define SWING_SUPPOTED_DIRECTION_KEY "x.com.vendor.swing.blade.পরিবর্তনের.সমর্থিত.দিক"
#define TIMER_LOCATION_KEY "x.com.vendor.timer.clock.location"
#define VERY_BIG_VALUE "dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh_dhakaRAJSHAHI,Bangladesh"

#define PLATFORM_ID "436f6e66-6f72-6d61-6e63-6553696d756c"
#define MANUFACTURER_NAME DEFAULT_MANUFACTURER
#define MANUFACTURER_URL "www.default-vendor.com"
#define MODEL_NUMBER "ABCDE00004"
#define DATE_OF_MANUFACTURE "2016-06-01"
#define PLATFORM_VERSION "0.0.1"
#define OPERATING_SYSTEM_VERSION "10"
#define HARDWARE_VERSION "1.1.0"
#define FIRMWARE_VERSION "1.1.1"
#define SUPPORT_URL "support.default-vendor.com"
#define SYSTEM_TIME "2016-06-20T10:10:10Z"
#define DEVICE_NAME "IotivitySmartRoom"
#define DEFAULT_DEVIE_TYPE "oic.wk.d"

#define TCP_ENDPOINT_KEY "tcp"
#define SECURED_ENDPOINT_KEY "coaps"

//  for OCF1.3 device
#define CORE_SPEC_VERSION "ocf.1.0.0"
#define RESOURCE_TYPE_SPEC_VERSION "ocf.res.1.3.0"
#define SMART_HOME_SPEC_VERSION "ocf.sh.1.3.0"

#define COAP_RESPONSE_CODE_SUCCESS 205
#define COAP_RESPONSE_CODE_CREATED 201
#define COAP_RESPONSE_CODE_DELETED 202
#define COAP_RESPONSE_CODE_UPDATED 204
#define COAP_RESPONSE_CODE_RETRIEVED 205
#define COAP_RESPONSE_CODE_ERROR 400
#define COAP_RESPONSE_CODE_RESOURCE_UNAUTHORIZED 401
#define COAP_RESPONSE_CODE_RESOURCE_NOT_FOUND 404
#define COAP_RESPONSE_CODE_METHOD_NOT_ALLOWED 405

class ResourceHelper
{
private:
    static ResourceHelper *s_resourceHelperInstance;
    static std::mutex s_mutex;

    HeaderOptions m_headerOption;
    OCRepresentation m_representation;
    OCStackResult m_presenceCallbackResult;
    vector< shared_ptr< OCResource > > m_resourceList;
    vector< shared_ptr< OCResource > > m_groupList;
    int m_resourceFoundCallbackCount;
    int m_groupFoundCallbackCount;
    int m_configReceivedCallbackCount;
    int m_presenceReceivedCallbackCount;
    int m_isResourceEnlisted;
    int m_callbackErrorCode;

    // for bootstrap
    std::string m_defaultRegionValue;
    std::string m_defaultTimeValue;
    std::string m_defaultCurrentTimeValue;
    std::string m_defaultNetworkValue;
    std::string m_defaultIPAddressValue;
    std::string m_defaultSecurityValue;
    std::string m_defaultModeValue;
    std::string m_defaultConfigurationValue;
    std::string m_defaultFactorySetValue;

    ResourceHelper(void);
    ~ResourceHelper(void);

    void initializeResultMap(void);

    void onFoundCandidate(vector< shared_ptr< OCResource > > resourceList);
    void onFoundGroup(shared_ptr< OCResource > groupResource);
    void onConfigurationReceived(const HeaderOptions &headerOptions, const OCRepresentation &rep,
            const int eCode);
    void onPresenceReceived(std::string presence, OCStackResult result);

public:

    map< OCStackResult, string > m_resultMap;
    int m_actionSetCallbackCount;

    /**
     * @brief   Function is for getting singleton instance of ConformanceHelper
     * @return  singleton instance of ConformanceHelper
     */
    static ResourceHelper *getInstance(void);

    /**
     * @brief   Function to Wait until resource Found
     * @param   foundCollectionResourceList - Resource Pointer List
     * @param   timeOut - Time limit for waiting
     * @return  bool - Returns true if the Resource is not found within timeOut otherwise false
     */
    bool waitForResourceFound(vector< shared_ptr< OCResource > > &foundResourceList,
            int timeOut);

    /**
     * @brief   Function to Wait until the Callback is called within TimeOut
     * @param   callbackMonitor - Reference to an int variable that will will increase
     *          when the callback is called
     * @param   timeOut - Time limit for waiting
     * @return  bool - Returns true if the Callback is not called within timeOut otherwise false
     */
    bool IsCallbackTimeoutOccured(int &callbackMonitor, int timeOut);

    /**
     * API for freezing current thread from execute next code for a given time
     *
     * @param seconds no of seconds to wait for
     *
     * @return bool
     *
     * NOTE: This API will be support other platforms
     */
    static void waitInSecond(unsigned int seconds);

    /**
     * @brief Helper function to find resource typein resource list
     * @param[in]   resourceList - list containing the resources to find
     * @param[in]   resourceTypeList   resource type list to find from
     * @param[out]   errorMessage   String to return error message if there are any
     * @return   if all resources are found , true is returned, else false is returned
     */
    bool containsResource(vector< shared_ptr< OCResource > > resourceList,
            vector< string > resourceTypeList, string &errorMessage);

    /**
     * @brief Helper function to print representaion
     * @param[in]   rep - incoming representaion
     */
    void printRepresentation(OCRepresentation rep);

    void printPayload(OCRepPayload* incomingPayload, OCRepresentation rep, int level = 0);

    /**
     * API for copying String to character array
     *
     * @param [out] targetString - character array for output
     * @param [in] sourceString - string to copy
     *
     */
    static void duplicateString(char ** targetString, string sourceString);

    /**
     * API to check whether a vector contains a specific string element
     *
     * @param [out] bool - true if the vector contains the string, false otherwise
     * @param [in] list - vec to search in
     * @param [in] targett - string to match
     *
     */
    bool containsElement(vector< string > list, string target);

    /**
     * API to get only the TCP endpoint from all available endpoints
     *
     * @param [out] string - target TCP endpoint
     * @param [in] allHosts - list of all resource endpoints
     *
     */
    string getOnlyTCPHost(vector<string> allHosts);

    /**
     * API to get only the Secured CoAP endpoint from all available endpoints
     *
     * @param [out] string - target Secured CoAP endpoint
     * @param [in] allHosts - list of all resource endpoints
     *
     */
    static string getOnlySecuredHost(vector<string> allHosts);

    /**
     * API to execute specific linux command line instruction
     *
     * @param [out] string - result of execution
     * @param [in] cmd - instruction to execute
     *
     */
    string executeCommand(string cmd);
};

#endif // __RESOURCE_HELPER_H__