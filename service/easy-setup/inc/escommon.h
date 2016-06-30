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
#define OC_RSRVD_ES_ERRORMESSAGE           "em"
#define OC_RSRVD_ES_LINKS                  "links"
#define OC_RSRVD_ES_TRIGGER                "tr"
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
#define OC_RSRVD_ES_COUNTRY                "cont"

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
#define MAX_SSIDLEN 33
#define MAX_CREDLEN 20

#define MAX_DEVICELEN 100
#define MAX_ERRMSGLEN 100
#define MAX_WEBLINKLEN 100

typedef enum
{
    NO_PROVISION = 0,
    CONNECTED_ENROLLER,
    FAILED_CONNECTION
} PROV_STATUS;

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

/**
* Device Roles defined for each device type used in easy setup
*/
typedef enum
{
    ENROLLEE,
    MEDIATOR,
    ENROLLER,
} DeviceRole;

/**
* On-boarding connection to create Adhoc network.
*/
typedef enum
{
    SOFTAP,
    BLE,
} OBConnection;

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
    ES_STATE_INIT = 1,

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
} ESEnrolleeState;

typedef enum
{
    /**
    * Error Code that given WiFi's SSID is not found
    */
    ES_ERRCODE_SSID_NOTFOUND = 1,

    /**
    * Error Code that given WiFi's Password is wrong
    */
    ES_ERRCODE_PW_WRONG,

    /**
    * Error Code that IP address is not allocated
    */
    ES_ERRCODE_IP_NOTALLOCATED,

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
    ES_ERRCODE_UNKNOWN,

    /**
    * No Error Occured
    */
    ES_ERRCODE_NONE = 999
} ESLastErrCode;

typedef struct
{
    // Address of remote server
    OCDevAddr * addr;
    // Indicates adaptor type on which the response was received
    OCConnectivityType connType;
} EasySetupDeviceInfo;

/**
 * Provosioning Status
 */
typedef enum
{
    ES_NEED_PROVISION = 1,
    ES_CONNECTED_TO_ENROLLER,
    ES_CONNECTED_FAIL_TO_ENROLLER,
    ES_REGISTERED_TO_CLOUD,
    ES_REGISTERED_FAIL_TO_CLOUD
} EasySetupState, ProvStatus;

/**
 * Response from queries to remote servers.
 */
typedef struct
{
    // EasySetup Status
    EasySetupState provStatus;
    // EasySetup Device Info
    EasySetupDeviceInfo provDeviceInfo;
} EasySetupInfo, ProvisioningInfo;

/**
 * @brief  Network information of the Enroller
 */
typedef union
{
    /**
     * @brief BT Mac Information
     */
    struct
    {
        char btMacAddress[NET_MACADDR_SIZE];   /**< BT mac address **/
    } BT;

    /**
     * @brief LE MAC Information
     */
    struct
    {
        char leMacAddress[NET_MACADDR_SIZE];   /**< BLE mac address **/
    } LE;

    /**
     * @brief IP Information
     */
    struct
    {
        char ssid[MAX_SSIDLEN]; /**< ssid of the Enroller**/
        char pwd[MAX_CREDLEN]; /**< pwd of the Enroller**/
    } WIFI;
} ProvData;

/**
 * @brief Network Information
 */
typedef struct
{
    ProvData provData;    /**< Enroller Network Info**/
    OCConnectivityType connType;    /**< Connectivity Type**/
} ProvConfig;

/**
 * Client applications implement this callback to consume responses received from Servers.
 */
typedef void (*OCProvisioningStatusCB)(EasySetupInfo *easySetupInfo);

/**
 * @brief This structure represent configuration information to create wifi onboarding SoftAP or connection.
*/


// Note : Below structure is not currently used but added for future purpose.
typedef struct {
    char ssid[MAX_SSIDLEN]; /**< ssid of the onboarding Adhoc Wifi network**/
    char pwd[MAX_CREDLEN]; /**< pwd of the onboarding Adhoc wifi network**/
    bool isSecured;                 /**< Secure connection**/
}WiFiOnboardingConfig;

/**
 * @brief This structure represent onboarding connection instance.
*/
typedef struct {
 /*Actual use of ipAddress is for unicast discovery, but also used to identify the Enrollee device as of now,
    device identification should be based on DeviceID in next release.*/
   char ipAddress[IPV4_ADDR_SIZE]; /**< IP Address of the Enrollee **/
   bool isSecured;                 /**< Secure connection**/
}WiFiOnboadingConnection;


#endif //ES_COMMON_H_
