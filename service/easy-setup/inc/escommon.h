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
#define NET_WIFI_SSID_SIZE      100
#define NET_WIFI_PWD_SIZE       100

/**
 * @brief Mac address length for BT port
 */
#define NET_MACADDR_SIZE 18

/**
 * Attributes used to form a proper easysetup conforming JSON message.
 */
#define OC_RSRVD_ES_PROVSTATUS             "ps"
#define OC_RSRVD_ES_ERRORCODE              "ec"
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

#define OC_RSRVD_ES_URI_NET               "/oic/net"

#define NUM_WIFIMODE    10
#define MAXSSIDLEN 33
#define MAXNETCREDLEN 20

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
    WiFi_11AC
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
} ESResult;

typedef enum
{
    ES_WIFI_RESOURCE = 0x01,
    ES_CLOUD_RESOURCE = 0x02,
    ES_DEVCONF_RESOURCE = 0x04
} ESResourceMask;

typedef struct
{
    char ssid[NET_WIFI_SSID_SIZE]; /**< ssid of the Enroller**/
    char pwd[NET_WIFI_PWD_SIZE]; /**< pwd of the Enroller**/
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


// TODO : Need to be erased
typedef enum
{
    /**
     * Default state of the device
     */
    ES_INIT_STATE,

    /**
     * Device will move to this state once the on boarding begins
     */
    ES_ON_BOARDING_STATE,

    /**
     * Device will move to this state after successful on-boarding of the device
     */
    ES_ON_BOARDED_STATE,

    /**
     * Device will move to this state once the on boarding is done
     */
    ES_PROVISIONING_STATE,

    /**
     * Easy setup process is successful.
     */
    ES_PROVISIONED_STATE,

    /**
     * This state is arbitrary one, any time device can come into this state
     * Device will move to this state if the ownership transfer initiated  by the Application
     */
    ES_OWNERSHIP_TRANSFERRING_STATE,

    /**
     * This state is arbitrary one, any time device can come into this state
     * Device will move to this state if the ownership transfer is completed
     */
    ES_OWNERSHIP_TRANSFERRED_STATE,

    /**
     * This state is arbitrary one, any time device can come into this state
     * Device will move to this state once the Application factory reset the device
     */
    ES_FACTORY_RESET_STATE,

    /**
     * Enrollee moves to this state after connecting to target network
     */
    ES_ON_BOARDED_TARGET_NETWORK_STATE,
}ESEnrolleeState;

/**
 * Provisioning Device Status
 */
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
    DEVICE_PROVISIONED = 0,
    DEVICE_NOT_PROVISIONED,
    DEVICE_OWNED,
    DEVICE_NOT_OWNED
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
        char ssid[NET_WIFI_SSID_SIZE]; /**< ssid of the Enroller**/
        char pwd[NET_WIFI_PWD_SIZE]; /**< pwd of the Enroller**/
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
    char ssid[NET_WIFI_SSID_SIZE]; /**< ssid of the onboarding Adhoc Wifi network**/
    char pwd[NET_WIFI_PWD_SIZE]; /**< pwd of the onboarding Adhoc wifi network**/
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
