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
#define OC_RSRVD_ES_PS                     "ps"
#define OC_RSRVD_ES_TNN                    "tnn"
#define OC_RSRVD_ES_CD                     "cd"
#define OC_RSRVD_ES_TR                     "tr"
#define OC_RSRVD_ES_TNT                    "tnt"
#define OC_RSRVD_ES_ANT                    "ant"

/**
 * Easysetup defined resoruce types and uris.
 */
#define OC_RSRVD_ES_PROV_RES_TYPE           "oic.r.prov"
#define OC_RSRVD_ES_URI_PROV               "/oic/prov"
#define OC_RSRVD_ES_URI_NET                "/oic/net"

/**
 * @brief Defines for Provisioning status accepted values
 */
#define ES_PS_NEED_PROVISIONING         1
#define ES_PS_PROVISIONING_COMPLETED    2
#define ES_PS_TRIGGER_INIT_VALUE        0
#define ES_PS_TRIGGER_CONNECTION        1


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
