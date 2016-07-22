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
         * @brief  WIFI Authentication tlype of the Enroller
         */
        typedef enum
        {
            NONE_AUTH = 0,
            WEP,
            WPA_PSK,
            WPA2_PSK
        } WIFI_AUTHTYPE;

        /**
         * @brief  WIFI encryption type of the Enroller
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

        typedef enum
        {
            WIFI_24G = 0,
            WIFI_5G,
            WIFI_BOTH
        } WIFI_FREQ;

        typedef enum
        {
            WIFI_11A = 0,
            WIFI_11B,
            WIFI_11G,
            WIFI_11N,
            WIFI_11AC
        } WIFI_MODE;

        typedef struct
        {
            string authCode;
            string authProvider;
            string ciServer;
        } CloudProp;

        typedef struct
        {
            struct
            {
                string ssid; /**< ssid of the Enroller**/
                string pwd; /**< pwd of the Enroller**/
                WIFI_AUTHTYPE authtype; /**< auth type of the Enroller**/
                WIFI_ENCTYPE enctype; /**< encryption type of the Enroller**/
            } WIFI;

            struct
            {
                string language;
                string country;
            } Device;
        } DeviceProp;

        typedef struct
        {
            string name;
            string language;
            string country;
        } DeviceConfig;

        typedef struct
        {
            vector<WIFI_MODE> modes;
            WIFI_FREQ freq;
        } WiFiConfig;

        typedef enum
        {
            ES_PROVISIONING_ERROR = -1,
            ES_NEED_PROVISIONING,
            ES_PROVISIONED_ALREADY,
            ES_PROVISIONING_SUCCESS
        } ESDeviceProvState;

        typedef enum
        {
            ES_CLOUD_PROVISIONING_ERROR = -1,
            ES_CLOUD_PROVISIONING_SUCCESS,
            ES_CLOUD_ENROLLEE_FOUND,
            ES_CLOUD_ENROLLEE_NOT_FOUND
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

        class EnrolleeConf
        {
        public:
            EnrolleeConf()
            {
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
         * Callback function definition for providing Enrollee security status
         */
        typedef function< void(shared_ptr< GetConfigurationStatus >) > GetConfigurationStatusCb;

        /**
         * Callback function definition for providing Enrollee security status
         */
        typedef function< void(shared_ptr< DevicePropProvisioningStatus >) > DevicePropProvStatusCb;

        /**
         * Callback function definition for providing Enrollee security status
         */
        typedef function< void(shared_ptr< CloudPropProvisioningStatus >) > CloudPropProvStatusCb;

        /**
         * Callback function definition for providing Enrollee security status
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
