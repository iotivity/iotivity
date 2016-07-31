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
#define OC_RSRVD_ES_CISERVER               "cis"
#define OC_RSRVD_ES_SERVERID               "sid"
#define OC_RSRVD_ES_DEVNAME                "dn"
#define OC_RSRVD_ES_LANGUAGE               "lang"
#define OC_RSRVD_ES_COUNTRY                "ctry"

/**
 * Easysetup defined resoruce types and uris.
 */
#define OC_RSRVD_ES_RES_TYPE_PROV         "ocf.wk.prov"
#define OC_RSRVD_ES_URI_PROV              "/ProvisioningResURI"
#define OC_RSRVD_ES_RES_TYPE_WIFI         "ocf.wk.wifi"
#define OC_RSRVD_ES_URI_WIFI              "/WiFiProvisioningResURI"
#define OC_RSRVD_ES_RES_TYPE_CLOUDSERVER  "ocf.wk.cloudserver"
#define OC_RSRVD_ES_URI_CLOUDSERVER       "/CloudServerProvisioningResURI"
#define OC_RSRVD_ES_RES_TYPE_DEVCONF      "ocf.wk.devconf"
#define OC_RSRVD_ES_URI_DEVCONF           "/DevConfProvisioningResURI"

#define NUM_WIFIMODE    10
#define MAX_SSIDLEN     33
#define MAX_CREDLEN     20

#define MAX_DEVICELEN   100
#define MAX_WEBLINKLEN  100

/**
 * @brief  Supported WIFI mode like 802.11g and 802.11n
 */
typedef enum
{
    WiFi_11A = 0,       /**< 802.11a **/
    WiFi_11B,           /**< 802.11b **/
    WiFi_11G,           /**< 802.11g **/
    WiFi_11N,           /**< 802.11n **/
    WiFi_11AC,          /**< 802.11ac **/
    WiFi_EOF = 999
} WIFI_MODE;

/**
 * @brief  Supported WIFI frequency like 2.4G and 5G
 */
typedef enum
{
    WiFi_24G = 0,       /**< 2.4G **/
    WiFi_5G,            /**< 5G **/
    WiFi_BOTH,          /**< 2.4G and 5G **/
    WiFi_FREQ_NONE      /**< EOF **/
} WIFI_FREQ;

/**
 * @brief  WIFI Authentication tlype of the Enroller
 */
typedef enum
{
    NONE_AUTH = 0,      /**< NO authentication **/
    WEP,                /**< WEP **/
    WPA_PSK,            /**< WPA-PSK **/
    WPA2_PSK            /**< WPA2-PSK **/
} WIFI_AUTHTYPE;

/**
 * @brief  WIFI encryption type of the Enroller
 */
typedef enum
{
    NONE_ENC = 0,       /**< NO encryption **/
    WEP_64,             /**< WEP-64 **/
    WEP_128,            /**< WEP-128**/
    TKIP,               /**< TKIP **/
    AES,                /**< AES **/
    TKIP_AES            /**< TKIP-AES **/
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

/**
 * @brief Indicate which resource is created in Enrollee
 */
typedef enum
{
    ES_WIFI_RESOURCE = 0x01,
    ES_CLOUD_RESOURCE = 0x02,
    ES_DEVCONF_RESOURCE = 0x04
} ESResourceMask;

/**
 * @brief Data structure delivered from mediator, which provides WiFi information
 */
typedef struct
{
    char ssid[MAX_SSIDLEN];         /**< Ssid of the Enroller**/
    char pwd[MAX_CREDLEN];          /**< Pwd of the Enroller**/
    WIFI_AUTHTYPE authtype;         /**< Auth type of the Enroller**/
    WIFI_ENCTYPE enctype;           /**< Encryption type of the Enroller**/
    void *userdata;                 /**< Vender-specific data**/
} ESWiFiProvData;

/**
 * @brief Data structure delivered from mediator, which provides device configuration information
 */
typedef struct
{
    char language[OIC_STRING_MAX_VALUE];    /**< IETF language tag using ISO 639X **/
    char country[OIC_STRING_MAX_VALUE];     /**< ISO Country Code (ISO 3166-1 Alpha-2) **/
    void *userdata;                         /**< Vender-specific data**/
} ESDevConfProvData;

/**
 * @brief Data structure delivered from mediator, which provides Cloud server information
 */
typedef struct
{
    char authCode[OIC_STRING_MAX_VALUE];        /**< Auth code issued by OAuth2.0-compatible account server **/
    char authProvider[OIC_STRING_MAX_VALUE];    /**< Auth provider ID **/
    char ciServer[OIC_STRING_MAX_VALUE];        /**< Cloud interface server URL which an Enrollee is going to registered **/
    void *userdata;                             /**< Vender-specific data**/
} ESCloudProvData;

/**
 * @brief Data structure stored for Device property which includes a WiFi and device configuration.
 */
typedef struct
{
    /**
     * @brief Data structure indicating WiFi configuration of Enrollee
     */
    struct
    {
        WIFI_MODE mode[NUM_WIFIMODE];
        WIFI_FREQ freq;
    } WiFi;

    /**
     * @brief Data structure indicating device configuration of Enrollee
     */
    struct
    {
        char deviceName[MAX_DEVICELEN];
    } DevConf;
} ESDeviceProperty;

/**
 * @brief Indicate enrollee and provisioning status. Provisioning status is shown in "provisioning
 *        status" property in provisioning resource.
 */
typedef enum
{
    /**
     * Default state of the device
     */
    ES_STATE_INIT = 0,

    /**
    * Status indicating being cnnecting to target network
    */
    ES_STATE_CONNECTING_TO_ENROLLER,

    /**
    * Status indicating successful conection to target network
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

/**
 * @brief Indicate last error code to describe a reason of error during easy setup.
 */
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
