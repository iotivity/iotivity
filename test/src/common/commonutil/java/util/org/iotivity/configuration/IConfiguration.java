/******************************************************************
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
 ******************************************************************/
package org.iotivity.configuration;

public interface IConfiguration {

    String  COMMON_TAG                   = "IoTivity Test";

    int     SERVERPORT                   = 7200;
    String  SERVERIP                     = "192.168.43.227";
    String  KEY_GIT_USER_ID              = "github_userid";
    String  KEY_GIT_USER_PASSWORD        = "github_passowrd";
    String  KEY_DEFAULT_CI_SERVER_IP     = "ci_server_wifi_ip";

    String  DEFAULT_CI_SERVER_PORT       = "5683";
    String  DEFAULT_AUTH_PROVIDER        = "github";

    String  TEMP_DIR                     = "/data/local/tmp/";
    String  CONFIG_FILE_NAME             = "config.properties";

    String  COAPS_PREFIX                 = "coaps+tcp://";
    String  COAP_PREFIX                  = "coap+tcp://";

    String  START_JUSTWORK_SERVER_01     = "./iotivity_pm_server oic_svr_db_server_justworks.dat 1";
    String  START_JUSTWORK_SERVER_02     = "./iotivity_pm_server oic_svr_db_server.dat 1";
    String  START_PRECONFIGPIN_SERVER    = "./iotivity_pm_server preconfig_server_1.dat 3";
    String  KILL_ALL_SERVER              = "kill all server";
    String  INTENT_START_SERVER_JUSTWORK = "service";
    String  INTENT_KILL_SERVER           = "kill_server";
    String  ALL_SERVER_STARTED           = "serverStarted";

    String  INIT_GET_LOGIN_URL           = "https://github.com/login";
    String  LOGIN_URL                    = "https://github.com/session";
    String  IOTIVITY_CLOUD_URL           = "https://github.com/login?return_to=%2Flogin%2Foauth%2Fauthorize%3Fclient_id%3Dea9c18f540323b0213d0%26redirect_uri%3Dhttp%253A%252F%252Fwww.example.com%252Foauth_callback%252F";

    String  PAR_POST_VAL                 = "commit=Sign+in&utf8=%E2%9C%93";                                                                                                                                                 // "commit=Sign+in&utf8=✓";//"commit=Sign+in&utf8=%E2%9C%93";
    String  AUTHENTICITY_TOKEN           = "&authenticity_token=";
    String  COOKIES_HEADER               = "Set-Cookie";

    int     DELAY_SHORT                  = 1000;                                                                                                                                                                            // millisecond
    int     DISCOVERY_TIMEOUT_05         = 5;
    int     DISCOVERY_TIMEOUT_ZERO       = 0;
    int     DISCOVERY_TIMEOUT_ONE        = 1;
    int     DISCOVERY_TIMEOUT_TWO        = 2;
    int     DISCOVERY_TIMEOUT_NEGATIVE   = -1;

    // Callback Related Resources
    int     SUCCES_CODE                  = 0;
    int     ERROR_CODE                   = 1;
    int     CALLBACK_TIMEOUT             = 60;                                                                                                                                                                              // 60

    int     CALLBACK_INVOKED_BUT_FAILED  = 0;
    String  CallBackNotInvoked           = "CallBack Not Invoked";

    int     SERVER_TIMEOUT_DELAY         = 40000;

    boolean CALLBACK_INVOKED             = true;
    boolean CALLBACK_NOT_INVOKED         = false;

    enum TLS
    {
        ENABLED, DISABLED
    };

}
