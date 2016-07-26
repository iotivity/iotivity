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
#include <vector>
#endif

#include "ocstack.h"
#include "octypes.h"

using namespace std;

// Defines
#define IP_PORT                 55555

/**
 * Attributes used to form a proper easysetup conforming JSON message
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
 * Easysetup defined resoruce types and uris
 */
#define OC_RSRVD_ES_RES_TYPE_PROV         "ocf.wk.prov"
#define OC_RSRVD_ES_URI_PROV              "/ProvisioningResURI"
#define OC_RSRVD_ES_RES_TYPE_WIFI         "ocf.wk.wifi"
#define OC_RSRVD_ES_URI_WIFI              "/WiFiProvisioningResURI"
#define OC_RSRVD_ES_RES_TYPE_CLOUDSERVER  "ocf.wk.cloudserver"
#define OC_RSRVD_ES_URI_CLOUDSERVER       "/CloudServerProvisioningResURI"
#define OC_RSRVD_ES_RES_TYPE_DEVCONF      "ocf.wk.devconf"
#define OC_RSRVD_ES_URI_DEVCONF           "/DevConfProvisioningResURI"

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
        } ProvStatus;

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

        /**
         * @brief  Supported WIFI frequency like 2.4G and 5G
         */
        typedef enum
        {
            WIFI_24G = 0,       /**< 2.4G **/
            WIFI_5G,            /**< 5G **/
            WIFI_BOTH,          /**< 2.4G and 5G **/
            WIFI_FREQ_NONE      /**< EOF **/
        } WIFI_FREQ;

        /**
         * @brief  Supported WIFI mode like 802.11g and 802.11n
         */
        typedef enum
        {
            WIFI_11A = 0,       /**< 802.11a **/
            WIFI_11B,           /**< 802.11b **/
            WIFI_11G,           /**< 802.11g **/
            WIFI_11N,           /**< 802.11n **/
            WIFI_11AC           /**< 802.11ac **/
        } WIFI_MODE;

        /**
         * @brief Properties of provisioning resource. It includes a provisioning status and last
         *        error code.
         */
        typedef struct
        {
            ProvStatus provStatus;
            ESErrorCode lastErrCode;
        } EnrolleeStatus;

        /**
         * @brief Data structure stored for Cloud server property provisioning
         */
        typedef struct
        {
            string authCode;        /**< Auth code issued by OAuth2.0-compatible account server **/
            string authProvider;    /**< Auth provider ID **/
            string ciServer;        /**< Cloud interface server URL which an Enrollee is going to registered **/
        } CloudProp;

        /**
         * @brief Data structure stored for Device property provisioning which includes a WiFi
         *        and device configuration provisioning
         */
        typedef struct
        {
            /**
             * @brief Data structure stored for WiFi property provisioning
             */
            struct
            {
                string ssid;            /**< Ssid of the Enroller **/
                string pwd;             /**< Pwd of the Enroller **/
                WIFI_AUTHTYPE authtype; /**< Auth type of the Enroller **/
                WIFI_ENCTYPE enctype;   /**< Encryption type of the Enroller **/
            } WIFI;

            /**
             * @brief Data structure stored for device configuration property provisioning
             */
            struct
            {
                string language;        /**< IETF language tag using ISO 639X **/
                string country;         /**< ISO Country Code (ISO 3166-1 Alpha-2) **/
            } Device;
        } DeviceProp;

        /**
         * @brief Data structure for received properties of device configuration resource
         */
        typedef struct
        {
            string name;        /**< Device's human-friendly name like device model name **/
            string language;    /**< IETF language tag using ISO 639X **/
            string country;     /**< ISO Country Code (ISO 3166-1 Alpha-2) **/
        } DeviceConfig;

        /**
         * @brief Data structure for received properties of WiFi resource
         */
        typedef struct
        {
            vector<WIFI_MODE> modes;    /**< Supported WIFI mode like 802.11g and 802.11n **/
            WIFI_FREQ freq;             /**< Supported WIFI frequency like 2.4G and 5G **/
        } WiFiConfig;

        /**
         * @brief Provisioning state in cloud server property provisioning.
         */
        typedef enum
        {
            ES_CLOUD_PROVISIONING_ERROR = -1,   /**< An error in cloud provisioning happens **/
            ES_CLOUD_PROVISIONING_SUCCESS,      /**< Cloud provisioning is successfully done **/
            ES_CLOUD_ENROLLEE_FOUND,            /**< An enrollee is found in a given network **/
            ES_CLOUD_ENROLLEE_NOT_FOUND         /**< NO enrollee is found in a given network **/
        }ESCloudProvState;

        /**
         * Security Provisioning Status
         */
        class SecProvisioningStatus
        {
        public:
            SecProvisioningStatus(string deviceUUID, ESResult result) :
                m_devUUID(deviceUUID), m_result(result)
            {
            }

            const string getDeviceUUID()
            {
                return m_devUUID;
            }

            ESResult getESResult()
            {
                return m_result;
            }
        private:
            string m_devUUID;
            ESResult m_result;
        };

        /**
         * It comprises sets of WiFi and device configuration properties.
         * Additionally, it provides a variable, m_cloudable, to make app know if
         * the enrollee has a preference to register IoTivity cloud server.
         */
        class EnrolleeConf
        {
        public:
            EnrolleeConf()
            {
                m_devConfig.name = nullptr;
                m_devConfig.language = nullptr;
                m_devConfig.country = nullptr;
                m_wifiConfig.freq = WIFI_FREQ_NONE;
                m_cloudable = false;
            }

            EnrolleeConf(DeviceConfig devConfig, WiFiConfig wifiConfig, bool cloudable) :
                m_devConfig(devConfig), m_wifiConfig(wifiConfig), m_cloudable(cloudable)
            {
            }

            const DeviceConfig& getDevConf() const
            {
                return m_devConfig;
            }

            const WiFiConfig& getWiFiConf() const
            {
                return m_wifiConfig;
            }

            bool isCloudable() const
            {
                return m_cloudable;
            }

        private:
            DeviceConfig m_devConfig;
            WiFiConfig m_wifiConfig;
            bool m_cloudable;
        };

        /**
         * Status object for getStatus API. This object is given to application
         * when a response for GET request to provisioning resource at Enrollee is arrived.
         * It returns a result of the API and requested data delivered in the response which includes
         * a provisioning status and last error code stored in Enrollee.
         *
         * @see EnrolleeStatus
         */
        class GetEnrolleeStatus
        {
        public:
            GetEnrolleeStatus()
            {
            }

            GetEnrolleeStatus(ESResult result, const EnrolleeStatus& status) :
                m_result(result), m_enrolleeStatus(status)
            {
            }

            ESResult getESResult()
            {
                return m_result;
            }

            const EnrolleeStatus& getEnrolleeStatus()
            {
                return m_enrolleeStatus;
            }

        private:
            ESResult m_result;
            EnrolleeStatus m_enrolleeStatus;
        };

        /**
         * Status object for getConfiguration API. This object is given to application
         * when a response for GET request to provisioning resource at Enrollee is arrived.
         * It returns a result of the API and requested data delivered in the response which includes
         * WiFi configuration and device configuration stored in Enrollee.
         *
         * @see EnrolleeConf
         */
        class GetConfigurationStatus
        {
        public:
            GetConfigurationStatus(ESResult result, const EnrolleeConf& conf) :
                    m_result(result), m_enrolleeConf(conf)
            {
            }

            ESResult getESResult()
            {
                return m_result;
            }

            const EnrolleeConf& getEnrolleeConf()
            {
                return m_enrolleeConf;
            }

        private:
            ESResult m_result;
            EnrolleeConf m_enrolleeConf;
        };

        /**
         * Status object for provisionDeviceProperties API. This object is given to application
         * when a response for GET request to provisioning resource at Enrollee is arrived.
         * It returns a result of the request.
         */
        class DevicePropProvisioningStatus
        {
        public:
            DevicePropProvisioningStatus(ESResult result) :
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

        /**
         * Status object for provisionCloudProperties API. This object is given to application
         * when a response for GET request to provisioning resource at Enrollee is arrived.
         * It returns a result of the request and status of this provisioning. The status provides
         * an information if the enrollee is found in a given network and the provisioning is
         * successfully done.
         */
        class CloudPropProvisioningStatus
        {
        public:
            CloudPropProvisioningStatus(ESResult result, ESCloudProvState state) :
                    m_result(result), m_esCloudState(state)
            {
            }

            ESResult getESResult()
            {
                return m_result;
            }

            ESCloudProvState getESCloudState()
            {
                return m_esCloudState;
            }

        private:
            ESResult m_result;
            ESCloudProvState m_esCloudState;
        };

        /**
         * Callback function definition for providing Enrollee status
         */
        typedef function< void(shared_ptr< GetEnrolleeStatus >) > GetStatusCb;

        /**
         * Callback function definition for providing Enrollee configuration status
         */
        typedef function< void(shared_ptr< GetConfigurationStatus >) > GetConfigurationStatusCb;

        /**
         * Callback function definition for providing Enrollee device property provisioning status
         */
        typedef function< void(shared_ptr< DevicePropProvisioningStatus >) > DevicePropProvStatusCb;

        /**
         * Callback function definition for providing Enrollee cloud property provisioning status
         */
        typedef function< void(shared_ptr< CloudPropProvisioningStatus >) > CloudPropProvStatusCb;

        /**
         * Callback function definition for providing Enrollee security provisioning status
         */
        typedef function< void(shared_ptr<SecProvisioningStatus>) > SecurityProvStatusCb;

        /**
         * Callback definition to be invoked when the security stack expects a pin from application
         */
        typedef function< void(string&) > SecurityPinCb;

        /**
         * Callback definition to be invoked when the stack expects a db path
         */
        typedef function< void(string&) > SecProvisioningDbPathCb;

    }
}
#endif //WITH_ARDUINO

#endif //ES_COMMON_RICH_H_
