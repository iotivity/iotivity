//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef ES_COMMON_H_
#define ES_COMMON_H_

#include "ocstack.h"
#include "octypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Defines
#define OIC_STRING_MAX_VALUE    100
#define IPV4_ADDR_SIZE          16
#define IP_PORT                 55555

/**
 * @brief Mac address length for BT port
 */
#define NET_MACADDR_SIZE 18

/**
 * Attributes used to form a proper easysetup conforming JSON message.
 */
#define OC_RSRVD_ES_PROVSTATUS             "ps"
#define OC_RSRVD_ES_LAST_ERRORCODE         "lec"
#define OC_RSRVD_ES_LINKS                  "links"
#define OC_RSRVD_ES_SUPPORTEDWIFIMODE      "swmt"
#define OC_RSRVD_ES_SUPPORTEDWIFIFREQ      "swf"
#define OC_RSRVD_ES_SSID                   "tnn"
#define OC_RSRVD_ES_CRED                   "cd"
#define OC_RSRVD_ES_AUTHTYPE               "wat"
#define OC_RSRVD_ES_ENCTYPE                "wet"
#define OC_RSRVD_ES_AUTHCODE               "ac"
#define OC_RSRVD_ES_AUTHPROVIDER           "apn"
#define OC_RSRVD_ES_CISERVER               "cisurl"
#define OC_RSRVD_ES_DEVNAME                "dn"
#define OC_RSRVD_ES_LANGUAGE               "lang"
#define OC_RSRVD_ES_COUNTRY                "ctry"

/**
 * Easysetup defined resoruce types and uris.
 */
#define OC_RSRVD_ES_RES_TYPE_PROV         "ocf.r.prov"
#define OC_RSRVD_ES_URI_PROV              "/.well-known/ocf/prov"
#define OC_RSRVD_ES_RES_TYPE_WIFI         "ocf.r.wifi"
#define OC_RSRVD_ES_URI_WIFI              "/.well-known/ocf/prov/wifi"
#define OC_RSRVD_ES_RES_TYPE_CLOUDSERVER  "ocf.r.cloudserver"
#define OC_RSRVD_ES_URI_CLOUDSERVER       "/.well-known/ocf/prov/cloudserver"
#define OC_RSRVD_ES_RES_TYPE_DEVCONF      "ocf.r.devconf"
#define OC_RSRVD_ES_URI_DEVCONF           "/.well-known/ocf/prov/devconf"

#define NUM_WIFIMODE    10
#define MAX_SSIDLEN     33
#define MAX_CREDLEN     20

#define MAX_DEVICELEN   100
#define MAX_ERRMSGLEN   100
#define MAX_WEBLINKLEN  100

typedef enum
{
    WiFi_11A = 0,
    WiFi_11B,
    WiFi_11G,
    WiFi_11N,
    WiFi_11AC,
    WiFi_EOF = 999,
} WIFI_MODE;

typedef enum
{
    WiFi_24G = 0,
    WiFi_5G,
    WiFi_BOTH
} WIFI_FREQ;

typedef enum
{
    NONE_AUTH = 0,
    WEP,
    WPA_PSK,
    WPA2_PSK
} WIFI_AUTHTYPE;

typedef enum
{
    NONE_ENC = 0,
    WEP_64,
    WEP_128,
    TKIP,
    AES,
    TKIP_AES
} WIFI_ENCTYPE;

typedef enum
{
    ES_ERROR = -1,
    ES_OK = 0,
    ES_NETWORKFOUND = 1,
    ES_NETWORKCONNECTED,
    ES_NETWORKNOTCONNECTED,
    ES_RESOURCECREATED = 11,
    ES_RECVREQOFPROVRES = 21,
    ES_RECVREQOFNETRES,
    ES_RECVUPDATEOFPROVRES,
    ES_RECVTRIGGEROFPROVRES,
    ES_UNAUTHORIZED = 31
} ESResult;

typedef enum
{
    ES_WIFI_RESOURCE = 0x01,
    ES_CLOUD_RESOURCE = 0x02,
    ES_DEVCONF_RESOURCE = 0x04
} ESResourceMask;

typedef struct
{
    char ssid[MAX_SSIDLEN]; /**< ssid of the Enroller**/
    char pwd[MAX_CREDLEN]; /**< pwd of the Enroller**/
    WIFI_AUTHTYPE authtype; /**< auth type of the Enroller**/
    WIFI_ENCTYPE enctype; /**< encryption type of the Enroller**/
} ESWiFiProvData;

typedef struct
{
    char language[OIC_STRING_MAX_VALUE];
    char country[OIC_STRING_MAX_VALUE];
} ESDevConfProvData;

typedef struct
{
    char authCode[OIC_STRING_MAX_VALUE];
    char authProvider[OIC_STRING_MAX_VALUE];
    char ciServer[OIC_STRING_MAX_VALUE];
} ESCloudProvData;

typedef struct
{
    struct
    {
        WIFI_MODE mode[NUM_WIFIMODE];
        WIFI_FREQ freq;
    } WiFi;
    struct
    {
        char deviceName[MAX_DEVICELEN];
    } DevConf;
} ESDeviceProperty;

typedef enum
{
    /**
     * Default state of the device
     */
    ES_STATE_INIT = 0,

    /**
    * Status indicating successful cnnection to target network
    */
    ES_STATE_CONNECTED_TO_ENROLLER,

    /**
    * Status indicating failure connection to target network
    */
    ES_STATE_CONNECTED_FAIL_TO_ENROLLER,

    /**
    * Status indicating successful registration to cloud
    */
    ES_STATE_REGISTERED_TO_CLOUD,

    /**
    * Status indicating failure registeration to cloud
    */
    ES_STATE_REGISTRRED_FAIL_TO_CLOUD
} ESEnrolleeState, ProvStatus;

typedef enum
{
    /**
     * Init Error Code
     */
    ES_ERRCODE_NO_ERROR = 0,

    /**
    * Error Code that given WiFi's SSID is not found
    */
    ES_ERRCODE_SSID_NOT_FOUND,

    /**
    * Error Code that given WiFi's Password is wrong
    */
    ES_ERRCODE_PW_WRONG,

    /**
    * Error Code that IP address is not allocated
    */
    ES_ERRCODE_IP_NOT_ALLOCATED,

    /**
    * Error Code that there is no Internet connection
    */
    ES_ERRCODE_NO_INTERNETCONNECTION,

    /**
    * Error Code that Timeout occured
    */
    ES_ERRCODE_TIMEOUT,

    /**
    * Error Code that Unknown error occured
    */
    ES_ERRCODE_UNKNOWN
} ESErrorCode;

#ifdef __cplusplus
}
#endif

#endif //ES_COMMON_H_
