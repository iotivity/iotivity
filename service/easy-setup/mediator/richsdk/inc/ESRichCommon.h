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

#include "OCPlatform.h"
#include "ocstack.h"
#include "octypes.h"

#include "escommon.h"

using namespace OC;
using namespace std;

#ifndef WITH_ARDUINO
namespace OIC
{
    namespace Service
    {
        /**
         * @brief Properties of provisioning resource. It includes a provisioning status and last
         *        error code.
         */
        class EnrolleeStatus
        {
        public:

            /**
             * Constructor
             */
            EnrolleeStatus(const OCRepresentation& rep) { m_rep = rep; }

            /**
             * Get a provisioning status property of Enrollee.
             *
             * @return a provisioning status property of Enrollee
             */
            ProvStatus getProvStatus()
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_PROVSTATUS))
                    return static_cast<ProvStatus>(
                                        m_rep.getValue<int>(OC_RSRVD_ES_PROVSTATUS));
                return ES_STATE_INIT;
            }

            /**
             * Get a last error code property of Enrollee.
             *
             * @return a last error code property of Enrollee.
             */
            ESErrorCode getLastErrCode()
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_LAST_ERRORCODE))
                    return static_cast<ESErrorCode>(
                                        m_rep.getValue<int>(OC_RSRVD_ES_LAST_ERRORCODE));
                return ES_ERRCODE_NO_ERROR;
            }

            /**
             * Get OCRepresentation object
             *
             * @return OCRepresentation object
             */
            const OCRepresentation& getRepresentation()
            {
                return m_rep;
            }
        protected:
            OCRepresentation m_rep;
        };

        /**
         * @brief Data class stored for Cloud server property provisioning
         */
        class CloudProp
        {
        public:

            /**
             * Constructor
             */
            CloudProp() {};

            /**
             * Constructor with OCRepresentation object. This is used for JNI communication.
             */
            CloudProp(const OCRepresentation &rep)
            {
                m_rep = rep;
                m_cloudID = "";
            }

            /**
             * Set CloudServer resource properties to be delivered to Enrollee
             *
             * @param authCode  Auth code issued by OAuth2.0-compatible account server
             * @param authProvider Auth provider ID
             * @param ciServer Cloud interface server URL which an Enrollee is going to registered
             */
            void setCloudProp(string authCode, string authProvider, string ciServer)
            {
                m_rep.setValue(OC_RSRVD_ES_AUTHCODE, authCode);
                m_rep.setValue(OC_RSRVD_ES_AUTHPROVIDER, authProvider);
                m_rep.setValue(OC_RSRVD_ES_CISERVER, ciServer);
            }

            /**
             * Set CloudServer's UUID
             *
             * @param cloudID Cloud Interface server's UUID
             */
            void setCloudID(string cloudID)
            {
                m_cloudID = cloudID;
            }

            /**
             * Get an auth code to be delivered.
             *
             * @return an auth code to be delivered.
             */
            std::string getAuthCode()
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_AUTHCODE))
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_AUTHCODE);
                return std::string("");
            }

            /**
             * Get an auth provider which issued an auth code
             *
             * @return an auth provider which issued an auth code
             */
            std::string getAuthProvider()
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_AUTHPROVIDER))
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_AUTHPROVIDER);
                return std::string("");
            }

            /**
             * Get a CI server to be delivered
             *
             * @return a CI server to be delivered
             */
            std::string getCiServer()
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_CISERVER))
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_CISERVER);
                return std::string("");
            }

            /**
             * Get a CI server's Uuid to be delivered
             *
             * @return a CI server's Uuid to be delivered
             */
            std::string getCloudID() const
            {
                return m_cloudID;
            }

            /**
             * Get OCRepresentation object
             *
             * @return OCRepresentation object
             */
            const OCRepresentation &toOCRepresentation() const
            {
                return m_rep;
            }
        protected:
            OCRepresentation m_rep;
            std::string m_cloudID;
        };

        /**
         * @brief Data class stored for Device property provisioning which includes a WiFi
         *        and device configuration provisioning
         */
        class DeviceProp
        {
        public:

            /**
             * Constructor
             */
            DeviceProp() {}

            /**
             * Constructor with OCRepresentation object. This is used for JNI communication.
             */
            DeviceProp(const OCRepresentation &rep) { m_rep = rep; }

            /**
             * Set WiFi resource properties to be delivered to Enrollee
             *
             * @param ssid Ssid of the Enroller
             * @param pwd Pwd of the Enrolle
             * @param authtype Auth type of the Enroller
             * @param enctype Encryption type of the Enroller
             *
             * @see WIFI_AUTHTYPE
             * @see WIFI_ENCTYPE
             */
            void setWiFiProp(string ssid, string pwd, WIFI_AUTHTYPE authtype, WIFI_ENCTYPE enctype)
            {
                m_rep.setValue(OC_RSRVD_ES_SSID, ssid);
                m_rep.setValue(OC_RSRVD_ES_CRED, pwd);
                m_rep.setValue(OC_RSRVD_ES_AUTHTYPE, authtype);
                m_rep.setValue(OC_RSRVD_ES_ENCTYPE, enctype);
            }

            /**
             * Set DevConf resource properties to be delivered to Enrollee
             *
             * @param language IETF language tag using ISO 639X
             * @param country ISO Country Code (ISO 3166-1 Alpha-2)
             */
            void setDevConfProp(string language, string country)
            {
                m_rep.setValue(OC_RSRVD_ES_LANGUAGE, language);
                m_rep.setValue(OC_RSRVD_ES_COUNTRY, country);
            }

            /**
             * Get a SSID of Enroller
             *
             * @return a SSID of enroller
             */
            std::string getSsid()
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_SSID))
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_SSID);
                return std::string("");
            }

            /**
             * Get a password of Enroller
             *
             * @return a password of enroller
             */
            std::string getPassword()
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_CRED))
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_CRED);
                return std::string("");
            }

            /**
             * Get an auth type of Enroller
             *
             * @return an auth type of enroller
             *
             * @see WIFI_AUTHTYPE
             */
            WIFI_AUTHTYPE getAuthType()
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_AUTHTYPE))
                    return static_cast<WIFI_AUTHTYPE>(m_rep.getValue<int>(OC_RSRVD_ES_AUTHTYPE));
                return NONE_AUTH;
            }

            /**
             * Get an encryption type of Enroller
             *
             * @return an encryption type of enroller
             *
             * @see WIFI_ENCTYPE
             */
            WIFI_ENCTYPE getEncType()
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_ENCTYPE))
                    return static_cast<WIFI_ENCTYPE>(m_rep.getValue<int>(OC_RSRVD_ES_ENCTYPE));
                return NONE_ENC;
            }

            /**
             * Get a language to be set. A language is expressed in IETF language tag
             * using ISO 639X.
             *
             * @return a language to be set
             */
            std::string getLanguage()
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_LANGUAGE))
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_LANGUAGE);
                return std::string("");
            }

            /**
             * Get a country to be set. A country is expressed in ISO Country Code
             * (ISO 3166-1 Alpha-2)
             *
             * @return a country to be set
             */
            std::string getCountry()
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_COUNTRY))
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_COUNTRY);
                return std::string("");
            }

            /**
             * Get OCRepresentation object
             *
             * @return OCRepresentation object
             */
            const OCRepresentation &toOCRepresentation() const
            {
                return m_rep;
            }

        protected:
            OCRepresentation m_rep;
        };

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
         * @breif This provide a set of getter APIs from received response for getConfiguration().
         *        Received information includes a device name, WiFi supported mode, and frequency.
         *        Additionally, you can know if Enrollee can be access to cloud server with this
         *        object.
         */
        class EnrolleeConf
        {
        public:
            /**
             * Constructor
             * The expected OCRepresentation is one for collection resource and has several child
             * OCRepresentation object corresponding to WiFi, DevConf, and CloudServer resource's
             * representation.
             */
            EnrolleeConf(const OCRepresentation& rep)
            {
                m_ProvRep = rep;

                std::vector<OCRepresentation> children = rep.getChildren();

                for(auto child = children.begin(); child != children.end(); ++child)
                {
                    if(child->getUri().find(OC_RSRVD_ES_URI_WIFI) != std::string::npos)
                    {
                        m_WiFiRep = *child;
                    }
                    else if(child->getUri().find(OC_RSRVD_ES_URI_DEVCONF) != std::string::npos)
                    {
                        m_DevConfRep = *child;
                    }
                    else if(child->getUri().find(OC_RSRVD_ES_URI_CLOUDSERVER) != std::string::npos)
                    {
                        m_CloudRep = *child;
                    }
                }
            }

            /**
             * Get a device name of Enrollee. It is Device's human-friendly name like device model
             * name.
             *
             * @return a device name of Enrollee
             */
            std::string getDeviceName()
            {
                if(m_DevConfRep.hasAttribute(OC_RSRVD_ES_DEVNAME))
                    return m_DevConfRep.getValue<std::string>(OC_RSRVD_ES_DEVNAME);
                return std::string("");
            }

            /**
             * Get a set of WiFi supported modes of Enrollee
             *
             * @return a set of WiFi supported modes of Enrollee
             *
             * @see WIFI_MODE
             */
            vector<WIFI_MODE> getWiFiModes()
            {
                vector<WIFI_MODE> modes;
                modes.clear();

                if(m_WiFiRep.hasAttribute(OC_RSRVD_ES_SUPPORTEDWIFIMODE))
                {
                    for(auto it : m_WiFiRep.getValue
                                        <std::vector<int>>(OC_RSRVD_ES_SUPPORTEDWIFIMODE))
                    {
                        modes.push_back(static_cast<WIFI_MODE>(it));
                    }
                }
                return modes;
            }

            /**
             * Get a WiFi supported frequency of Enrollee
             *
             * @return a WiFi supported frequency of Enrollee
             *
             * @see WIFI_FREQ
             */
            WIFI_FREQ getWiFiFreq()
            {
                if(m_WiFiRep.hasAttribute(OC_RSRVD_ES_SUPPORTEDWIFIFREQ))
                    return static_cast<WIFI_FREQ>(
                                        m_WiFiRep.getValue<int>(OC_RSRVD_ES_SUPPORTEDWIFIFREQ));
                return WIFI_FREQ_NONE;
            }

            /**
             * Get an accessibility to cloud server of an Enrollee
             *
             * @return an accessibility to cloud server of an Enrollee
             */
            bool isCloudAccessible()
            {
                if(m_CloudRep.getUri().find(OC_RSRVD_ES_URI_CLOUDSERVER) != std::string::npos)
                {
                    return true;
                }
                return false;
            }

            /**
             * Get OCRepresentation object
             *
             * @return OCRepresentation object
             */
            const OCRepresentation& getProvResRep()
            {
                return m_ProvRep;
            }

        protected:
            OCRepresentation m_ProvRep, m_WiFiRep, m_DevConfRep, m_CloudRep;
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
