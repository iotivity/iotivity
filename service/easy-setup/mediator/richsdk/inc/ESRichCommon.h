//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef ES_COMMON_RICH_H_
#define ES_COMMON_RICH_H_

#include <iostream>
#include <string>
#ifndef WITH_ARDUINO
#include <memory>
#endif

#include "ocstack.h"
#include "octypes.h"

using namespace std;

// Defines
#define OIC_STRING_MAX_VALUE    100
#define IPV4_ADDR_SIZE          16
#define IP_PORT                 55555
#define NET_WIFI_SSID_SIZE      100
#define NET_WIFI_PWD_SIZE       100
#define NET_WIFI_AUTH_SIZE      100

/**
 * @brief Mac address length for BT port
 */
#define NET_MACADDR_SIZE 18

/**
 * Attributes used to form a proper easysetup conforming JSON message.
 */
#define OC_RSRVD_ES_PROVSTATUS        "ps"
#define OC_RSRVD_ES_TRIGGER                "tr"
#define OC_RSRVD_ES_SSID                       "tnn"
#define OC_RSRVD_ES_CRED                     "cd"
#define OC_RSRVD_ES_AUTHTYPE             "wat"
#define OC_RSRVD_ES_ENCTYPE                "wet"
#define OC_RSRVD_ES_AUTHCODE            "ac"
#define OC_RSRVD_ES_AUTHPROVIDER    "apn"
#define OC_RSRVD_ES_CISERVER              "cisurl"
#define OC_RSRVD_ES_DEVNAME              "dn"
#define OC_RSRVD_ES_LANGUAGE            "lang"
#define OC_RSRVD_ES_COUNTRY              "cont"
#define OC_RSRVD_ES_TNT                        "tnt"
#define OC_RSRVD_ES_ANT                        "ant"

/**
 * Easysetup defined resoruce types and uris.
 */
#define OC_RSRVD_ES_PROV_RES_TYPE                 "oic.r.prov"
#define OC_RSRVD_ES_URI_PROV                           "/.well-known/ocf/prov"
#define OC_RSRVD_ES_RES_TYPE_WIFI                   "ocf.r.wifi"
#define OC_RSRVD_ES_URI_WIFI                             "/.well-known/ocf/prov/wifi"
#define OC_RSRVD_ES_RES_TYPE_CLOUDSERVER  "ocf.r.cloudserver"
#define OC_RSRVD_ES_URI_CLOUDSERVER            "/.well-known/ocf/prov/cloudserver"
#define OC_RSRVD_ES_RES_TYPE_DEVCONF          "ocf.r.devconf"
#define OC_RSRVD_ES_URI_DEVCONF                    "/.well-known/ocf/prov/devconf"
#define OC_RSRVD_ES_URI_NET                             "/oic/net"

/**
 * @brief Defines for Provisioning status accepted values
 */
#define ES_PS_NEED_PROVISIONING         1
#define ES_PS_PROVISIONING_COMPLETED    2
#define ES_PS_TRIGGER_INIT_VALUE        0
#define ES_PS_TRIGGER_CONNECTION        1

#ifndef WITH_ARDUINO
namespace OIC
{
    namespace Service
    {
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
         * Provosioning Status
         */
        typedef enum
        {
            DEVICE_PROVISIONED = 0,
            DEVICE_NOT_PROVISIONED,
            DEVICE_OWNED,
            DEVICE_NOT_OWNED
        } EasySetupState;

        /**
         * @brief  WIFI Authentication type of the Enroller
         */
        typedef enum
        {
            NONE_AUTH = 0,
            WEP,
            WPA_PSK,
            WPA2_PSK
        } WIFI_AUTHTYPE;

        /**
         * @brief  WIFI ecnrytion type of the Enroller
         */
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
                WIFI_AUTHTYPE authtype; /**< auth type of the Enroller**/
                WIFI_ENCTYPE enctype; /**< encryption type of the Enroller**/
                char authcode[NET_WIFI_AUTH_SIZE];
                char authserverUrl[NET_WIFI_AUTH_SIZE];
                char apiserverUrl[NET_WIFI_AUTH_SIZE];
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
         * @brief This structure represent onboarding connection instance.
        */
        typedef struct
        {
         /*Actual use of ipAddress is for unicast discovery, but also used to identify the Enrollee device as of now,
            device identification should be based on DeviceID in next release.*/
           char ipAddress[IPV4_ADDR_SIZE]; /**< IP Address of the Enrollee **/
           bool isSecured;                 /**< Secure connection**/
        }WiFiOnboadingConnection;

        typedef enum
        {
            ES_PROVISIONING_ERROR = -1,
            ES_NEED_PROVISIONING,
            ES_PROVISIONED_ALREADY,
            ES_PROVISIONING_SUCCESS
        } ESState;

        typedef enum
        {
            ES_UNKNOWN = 0,
            ES_ONBOARDED,
            ES_OWNED,
            ES_PROVISIONED
        } CurrentESState;

        typedef enum
        {
            ES_SEC_UNKNOWN = 0,
            ES_SEC_OWNED,
            ES_SEC_ACL_PROVISIONED,
            ES_SEC_CREDS_PROVISIONED
        } EnrolleeSecState;

        /**
         * Security Provisioning Status
         */
        class SecProvisioningStatus
        {
        public:
            SecProvisioningStatus(std::string deviceUUID, ESResult result) :
                m_devUUID(deviceUUID), m_result(result)
            {

            }

            std::string getDeviceUUID()
            {
                return m_devUUID;
            }

            ESResult getResult()
            {
                return m_result;
            }
        private:
            std::string m_devUUID;
            ESResult m_result;
        };

        class CapabilityData
        {
        public:
            CapabilityData()
            {

            }
        private:
        };

        class InitRemoteEnrolleeStatus
        {
        public:
            InitRemoteEnrolleeStatus(ESResult result) :
                    m_result(result)
            {

            }

            ESResult getESResult()
            {
                return m_result;
            }
        private:
            ESResult m_result;
        };

        class RequestCapabilityStatus
        {
        public:
            RequestCapabilityStatus(ESResult result, const CapabilityData& data) :
                    m_result(result), m_capabilityData(data)
            {
            }
            ESResult getESResult()
            {
                return m_result;
            }
            CapabilityData getCapabilityData()
            {
                return m_capabilityData;
            }
        private:
            ESResult m_result;
            CapabilityData m_capabilityData;
        };

        class ProvisioningStatus
        {
        public:
            ProvisioningStatus(ESResult result, ESState esState) :
                    m_result(result), m_esState(esState)
            {

            }

            ESResult getESResult()
            {
                return m_result;
            }

            ESState getESState()
            {
                return m_esState;
            }
        private:
            ESResult m_result;
            ESState m_esState;
        };

        class EasySetupStatus
        {
        public:
            EasySetupStatus(const EasySetupState& easySetupState,
                    const ProvConfig& provConfig) :
                    m_easySetupState(easySetupState), m_ProvConfig(provConfig)
            {

            }

            ProvConfig getProvConfig()
            {
                return m_ProvConfig;
            }

            EasySetupState getEasySetupState()
            {
                return m_easySetupState;
            }
        private:
            EasySetupState m_easySetupState;
            ProvConfig m_ProvConfig;
        };

        /**
         * Callback function definition for providing Enrollee security status .
         */
        typedef std::function< void(std::shared_ptr< InitRemoteEnrolleeStatus >) > InitRemoteEnrolleeStatusCb;

        /**
         * Callback function definition for providing Enrollee security status .
         */
        typedef std::function< void(std::shared_ptr< RequestCapabilityStatus >) > RequestCapabilityStatusCb;
        /**
         * Callback function definition for providing Enrollee security status .
         */
        typedef std::function< void(std::shared_ptr< ProvisioningStatus >) > DataProvStatusCb;

        /**
         * Callback function definition for providing Enrollee security status .
         */
        typedef std::function< void(std::shared_ptr<SecProvisioningStatus>) > EnrolleeSecStatusCb;

        /**
         * Callback definition to be invoked when the security stack expects a pin from application.
         */
        typedef std::function< void(std::string&) > SecurityPinCb;

        /**
         * Callback definition to be invoked when the stack expects a db path.
         */
        typedef std::function< void(std::string&) > SecProvisioningDbPathCb;

    }
}
#endif //WITH_ARDUINO

#endif //ES_COMMON_RICH_H_
