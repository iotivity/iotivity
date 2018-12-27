/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

package org.iotivity.test.ri.tc.helper;

public interface IRIConstants {
    String                      EMPTY_STRING                             = "";
    String                      KEY_PLATFORM_ID                          = "pi";
    String                      KEY_MANUFACTURER_NAME                    = "mnmn";
    String                      KEY_MANUFACTURER_URL                     = "mnml";
    String                      KEY_MODEL_NO                             = "mnmo";
    String                      KEY_MANUFACTURE_DATE                     = "mndt";
    String                      KEY_PLATFORM_VERSION                     = "mnpv";
    String                      KEY_OS_VERSION                           = "mnos";
    String                      KEY_HW_VERSION                           = "mnhw";
    String                      KEY_FIRMWARE_VERSION                     = "mnfv";
    String                      KEY_SUPPORT_URL                          = "mnsl";
    String                      KEY_SYSTEM_TIME                          = "st";

    String                      PLATFORM_ID                              = "436f6e66-6f72-6d61-6e63-6553696d756c";
    String                      MANUFACTURER_NAME                        = "Vendor";
    String                      MANUFACTURER_URL                         = "www.default-vendor.com";
    String                      MODEL_NO                                 = "ABCDE00004";
    String                      MANUFACTURE_DATE                         = "2016-06-01";
    String                      PLATFORM_VERSION                         = "0.0.1";
    String                      OS_VERSION                               = "10";
    String                      HW_VERSION                               = "1.1.0";
    String                      FIRMWARE_VERSION                         = "1.1.1";
    String                      SUPPORT_URL                              = "support.default-vendor.com";
    String                      SYSTEM_TIME                              = "2016-06-20T10:10:10Z";

    String                      KEY_DEVICE_NAME                          = "n";
    String                      DEVICE_NAME                              = "IotivitySmartRoom";
    String                      DEVICE_TYPE_AC                           = "AirCondition";

    String                      RESOURCE_URI_TEMPERATURE                 = "/test/ri/android/temperature";
    String                      RESOURCE_TYPE_TEMPERATURE                = "oic.r.temperature";
    String                      RESOURCE_URI_LIGHT                       = "/a/light";
    String                      RESOURCE_TYPE_LIGHT                      = "core.light";
    String                      RESOURCE_URI_FAN                         = "/a/fan";
    String                      RESOURCE_TYPE_FAN                        = "core.fan";

    String                      HOST                                     = "coap://192.168.1.2:5000";

    int                         INT_ZERO                                 = 0;
    int                         INT_ONE                                  = 1;
    int                         INT_TWO                                  = 2;
    int                         INT_MINUS_ONE                            = -1;

    int                         CALLBACK_WAIT_DEFAULT                    = 5;
    int                         CALLBACK_WAIT_MAX                        = 10;
    int                         CALLBACK_WAIT_MIN                        = 1;
    int                         SUCCESS_RESPONSE                         = 0;

    int                         COAP_RESPONSE_CODE_SUCCESS               = 205;
    int                         COAP_RESPONSE_CODE_CREATED               = 201;
    int                         COAP_RESPONSE_CODE_DELETED               = 202;
    int                         COAP_RESPONSE_CODE_UPDATED               = 204;
    int                         COAP_RESPONSE_CODE_RETRIEVED             = 205;
    int                         COAP_RESPONSE_CODE_ERROR                 = 400;
    int                         COAP_RESPONSE_CODE_RESOURCE_UNAUTHORIZED = 401;
    int                         COAP_RESPONSE_CODE_RESOURCE_NOT_FOUND    = 404;
    int                         COAP_RESPONSE_CODE_METHOD_NOT_ALLOWED    = 405;

    String                      KEY_TEMPERATURE                          = "temperature";
    String                      KEY_HOUR                                 = "hour";
}