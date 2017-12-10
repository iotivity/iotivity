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
#include <memory>
#include <vector>

#include "OCPlatform.h"
#include "ocstack.h"
#include "octypes.h"
#ifdef __WITH_DTLS__
#include "experimental/securevirtualresourcetypes.h"
#include "OCProvisioningManager.hpp"
#include "experimental/ocrandom.h"
#endif

#include "escommon.h"

#include "experimental/logger.h"

using namespace OC;
using namespace std;

#define ES_RICH_COMMON_TAG "ES_MEDIATOR_COMMON"
#define OCF_CONTENT_FORMAT_VERSION_OPTION_ID 2053
#define OCF_CONTENT_FORMAT_VERSION_VALUE 2048

namespace OIC
{
    namespace Service
    {
        /**
         * @brief Properties of easysetup resource. It includes a provisioning status and last
         *        error code.
         */
        class EnrolleeStatus
        {
        public:

            /**
             * Constructor
             */
            EnrolleeStatus(const OCRepresentation& rep)
            {
                m_rep = rep;
            }

            EnrolleeStatus(const EnrolleeStatus& enrolleeStatus) :
                m_rep(enrolleeStatus.getRepresentation())
            {
            }

            EnrolleeStatus(const EnrolleeStatus&& enrolleeStatus) :
                m_rep(std::move(enrolleeStatus.getRepresentation()))
            {
            }

            /**
             * Get a provisioning status property of Enrollee.
             *
             * @return a provisioning status property of Enrollee
             */
            ProvStatus getProvStatus() const
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_PROVSTATUS))
                {
                    return static_cast<ProvStatus>(
                                        m_rep.getValue<int>(OC_RSRVD_ES_PROVSTATUS));
                }
                return ES_STATE_INIT;
            }

            /**
             * Get a last error code property of Enrollee.
             *
             * @return a last error code property of Enrollee.
             */
            ESErrorCode getLastErrCode() const
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_LAST_ERRORCODE))
                {
                    return static_cast<ESErrorCode>(
                                        m_rep.getValue<int>(OC_RSRVD_ES_LAST_ERRORCODE));
                }
                return ES_ERRCODE_NO_ERROR;
            }

            /**
             * Get OCRepresentation object
             *
             * @return OCRepresentation object
             */
            const OCRepresentation& getRepresentation() const
            {
                return m_rep;
            }
        protected:
            OCRepresentation m_rep;
        };

        /**
         * @brief Data class stored for provisioning of coap cloud server properties
         */
        class CloudProp
        {
        public:

            /**
             * Constructor
             */
            CloudProp()
            {
                m_cloudID = "";
                m_credID = 0;
            }

            CloudProp(const CloudProp& cloudProp) :
                                            m_rep(cloudProp.toOCRepresentation()),
                                            m_cloudID(cloudProp.getCloudID()),
                                            m_credID(cloudProp.getCredID())
            {
            }

            CloudProp(const CloudProp&& cloudProp) :
                                            m_rep(std::move(cloudProp.toOCRepresentation())),
                                            m_cloudID(cloudProp.getCloudID()),
                                            m_credID(cloudProp.getCredID())
            {
            }

            /**
             * Constructor with OCRepresentation object. This is used for JNI communication.
             * @param rep OCRepresentation object
             */
            CloudProp(const OCRepresentation &rep)
            {
                m_rep = rep;
                m_cloudID = "";
                m_credID = 0;
            }

            /**
             * Set CoapCloudConf resource properties to be delivered to Enrollee
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
             * Set CoapCloudConf resource properties with Access token to be delivered to Enrollee
             *
             * @param accessToken  Access token which is given in a return of auth code issued by
             *                     OAuth2.0-compatible account server
             * @param tokenType Access token type, i.e. "bearer"
             * @param authProvider Auth provider ID
             * @param ciServer Cloud interface server URL which an Enrollee is going to registered
             *
             * @see OAUTH_TOKENTYPE
             */
            void setCloudPropWithAccessToken(string accessToken, OAUTH_TOKENTYPE tokenType,
                                                string authProvider, string ciServer)
            {
                m_rep.setValue(OC_RSRVD_ES_ACCESSTOKEN, accessToken);
                m_rep.setValue(OC_RSRVD_ES_ACCESSTOKEN_TYPE, tokenType);
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
             * Set CloudServer's credential ID of certificate
             *
             * @param credID Cloud Interface server's credential ID of certificate
             */
            void setCredID(int credID)
            {
                m_credID = credID;
            }

            /**
             * Get an auth code to be delivered.
             *
             * @return an auth code to be delivered.
             */
            std::string getAuthCode() const
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_AUTHCODE))
                {
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_AUTHCODE);
                }
                return std::string("");
            }

            /**
             * Get an auth provider which issued an auth code
             *
             * @return an auth provider which issued an auth code
             */
            std::string getAuthProvider() const
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_AUTHPROVIDER))
                {
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_AUTHPROVIDER);
                }
                return std::string("");
            }

            /**
             * Get a CI server to be delivered
             *
             * @return a CI server to be delivered
             */
            std::string getCiServer() const
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_CISERVER))
                {
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_CISERVER);
                }
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
             * Get a CI server's credential ID of certificate
             *
             * @return a CI server's credential ID of certificated
             */
            int getCredID() const
            {
                return m_credID;
            }

            /**
             * Get an access token to be delivered.
             *
             * @return an access token to be delivered.
             */
            std::string getAccessToken() const
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_ACCESSTOKEN))
                {
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_ACCESSTOKEN);
                }
                return std::string("");
            }

            /**
             * Get an access token type to be delivered.
             *
             * @return an access token type to be delivered.
             */
            OAUTH_TOKENTYPE getAccessTokenType() const
            {

                if(m_rep.hasAttribute(OC_RSRVD_ES_ACCESSTOKEN_TYPE))
                {
                    return static_cast<OAUTH_TOKENTYPE>(
                                m_rep.getValue<int>(OC_RSRVD_ES_ACCESSTOKEN_TYPE));
                }
                return NONE_OAUTH_TOKENTYPE;
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

            /**
             * Update Cloud property representation as per OCF Specification.
             *
             * Resource Schema for CloudConf resource is different in older Enrollee following
             * OIC Spec compared to newer Enrollee following OCF Spec. This function updates
             * the representation as per spec version.
             */
            void updateOCRepresentation(int specVersion)
            {
                if(0 == specVersion) // OIC Server does not contain CONTENT FORMAT VERSION
                {
                    // Representation is already stored as per older resource schemas.
                    return;
                }

                // Representation should be updated as per OCF 1.3 Easy Setup resource schemas.
                std::string authCode = m_rep.getValue<std::string>(OC_RSRVD_ES_AUTHCODE);
                std::string authProvider = m_rep.getValue<std::string>(OC_RSRVD_ES_AUTHPROVIDER);
                std::string ciServer = m_rep.getValue<std::string>(OC_RSRVD_ES_CISERVER);

                std::string accessToken = m_rep.getValue<std::string>(OC_RSRVD_ES_ACCESSTOKEN);
                OAUTH_TOKENTYPE accessTokenType = static_cast<OAUTH_TOKENTYPE>(
                                m_rep.getValue<int>(OC_RSRVD_ES_ACCESSTOKEN_TYPE));

                OCRepresentation rootRep;
                rootRep.setUri(OC_RSRVD_ES_URI_EASYSETUP);

                OCRepresentation cloudDataRep;

                if(authCode.length() > 0)
                {
                    cloudDataRep.setValue(OC_RSRVD_ES_AUTHCODE, authCode);
                }

                if(accessToken.length() > 0)
                {
                    cloudDataRep.setValue(OC_RSRVD_ES_ACCESSTOKEN, accessToken);
                    cloudDataRep.setValue(OC_RSRVD_ES_ACCESSTOKEN_TYPE, accessTokenType);
                }

                cloudDataRep.setValue(OC_RSRVD_ES_AUTHPROVIDER, authProvider);
                cloudDataRep.setValue(OC_RSRVD_ES_CISERVER, ciServer);

                OCRepresentation cloudconfRep;
                cloudconfRep.setUri(OC_RSRVD_ES_URI_COAPCLOUDCONF);
                cloudconfRep.setValue(OC_RSRVD_HREF, std::string(OC_RSRVD_ES_URI_COAPCLOUDCONF));
                cloudconfRep.setValue(OC_RSRVD_REPRESENTATION, cloudDataRep);

                rootRep.addChild(cloudconfRep);
                m_rep = rootRep;
            }

        protected:
            OCRepresentation m_rep;
            std::string m_cloudID;
            int m_credID;
        };

        /**
         * @brief Data class stored for provisioning of Device properties which includes
         *        properties of WiFiConf resource and DevConf resource
         */
        class DeviceProp
        {
        public:

            /**
             * Constructor
             */
            DeviceProp()
            {
            }

            DeviceProp(const DeviceProp& deviceProp) :
                m_rep(deviceProp.toOCRepresentation())
            {
            }

            DeviceProp(const DeviceProp&& deviceProp) :
                m_rep(std::move(deviceProp.toOCRepresentation()))
            {
            }

            /**
             * Constructor with OCRepresentation object. This is used for JNI communication.
             * @param rep OCRepresentation object
             */
            DeviceProp(const OCRepresentation &rep)
            {
                m_rep = rep;
            }

            /**
             * Set WiFiConf resource properties to be delivered to Enrollee
             *
             * @param ssid SSID of the Enroller
             * @param pwd Password of the Enroller
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
             * Get a SSID of Enroller
             *
             * @return a SSID of enroller
             */
            std::string getSsid() const
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_SSID))
                {
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_SSID);
                }
                return std::string("");
            }

            /**
             * Get a password of Enroller
             *
             * @return a password of enroller
             */
            std::string getPassword() const
            {
                if(m_rep.hasAttribute(OC_RSRVD_ES_CRED))
                {
                    return m_rep.getValue<std::string>(OC_RSRVD_ES_CRED);
                }
                return std::string("");
            }

            /**
             * Get an auth type of Enroller
             *
             * @return an auth type of enroller
             *
             * @see WIFI_AUTHTYPE
             */
            WIFI_AUTHTYPE getAuthType() const
            {
                ///TODO: Recheck logic if this API is used.
                if(m_rep.hasAttribute(OC_RSRVD_ES_AUTHTYPE))
                {
                    return static_cast<WIFI_AUTHTYPE>(m_rep.getValue<int>(OC_RSRVD_ES_AUTHTYPE));
                }
                return NONE_AUTH;
            }

            /**
             * Get an encryption type of Enroller
             *
             * @return an encryption type of enroller
             *
             * @see WIFI_ENCTYPE
             */
            WIFI_ENCTYPE getEncType() const
            {
                ///TODO: Recheck logic if this API is used.
                if(m_rep.hasAttribute(OC_RSRVD_ES_ENCTYPE))
                {
                    return static_cast<WIFI_ENCTYPE>(m_rep.getValue<int>(OC_RSRVD_ES_ENCTYPE));
                }
                return NONE_ENC;
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

            /**
             * Update Device and Wi-Fi representation as per OCF Specification.
             *
             * Resource Schema for WiFiConf resource is different in older Enrollee following
             * OIC Spec compared to newer Enrollee following OCF Spec. This function updates
             * the representation as per spec version.
             */
            void updateOCRepresentation(int specVersion)
            {
                if(0 == specVersion) // OIC Server does not contain CONTENT FORMAT VERSION
                {
                    // Representation is already stored as per older resource schemas.
                    return;
                }

                // Representation should be updated as per OCF 1.3 Easy Setup resource schemas.
                WIFI_AUTHTYPE authtype = static_cast<WIFI_AUTHTYPE> (m_rep.getValue<int>(OC_RSRVD_ES_AUTHTYPE));
                WIFI_ENCTYPE enctype = static_cast<WIFI_ENCTYPE> (m_rep.getValue<int>(OC_RSRVD_ES_ENCTYPE));

                OCRepresentation rootRep;
                rootRep.setUri(OC_RSRVD_ES_URI_EASYSETUP);

                OCRepresentation wifiDataRep;
                wifiDataRep.setValue(OC_RSRVD_ES_SSID, m_rep.getValue<std::string>(OC_RSRVD_ES_SSID));
                wifiDataRep.setValue(OC_RSRVD_ES_CRED, m_rep.getValue<std::string>(OC_RSRVD_ES_CRED));
                wifiDataRep.setValue(OC_RSRVD_ES_AUTHTYPE, getAuthTypeAsString(authtype));
                wifiDataRep.setValue(OC_RSRVD_ES_ENCTYPE, getEncTypeAsString(enctype));

                OCRepresentation wificonfRep;
                wificonfRep.setUri(OC_RSRVD_ES_URI_WIFICONF);
                wificonfRep.setValue(OC_RSRVD_HREF, std::string(OC_RSRVD_ES_URI_WIFICONF));
                wificonfRep.setValue(OC_RSRVD_REPRESENTATION, wifiDataRep);

                rootRep.addChild(wificonfRep);
                m_rep = rootRep;
            }

        protected:
            OCRepresentation m_rep;

        private:
            std::string getAuthTypeAsString(WIFI_AUTHTYPE authType) const
            {
                switch(authType)
                {
                    case NONE_AUTH:
                        return "None";
                    case WEP:
                        return "WEP";
                    case WPA_PSK:
                        return "WPA_PSK";
                    case WPA2_PSK:
                        return "WPA2_PSK";
                    default:
                        return "None";
                }
            }

            std::string getEncTypeAsString(WIFI_ENCTYPE encType) const
            {
                switch(encType)
                {
                    case NONE_ENC:
                        return "None";
                    case WEP_64:
                        return "WEP_64";
                    case WEP_128:
                        return "WEP_128";
                    case TKIP:
                        return "TKIP";
                    case AES:
                        return "AES";
                    case TKIP_AES:
                        return "TKIP_AES";
                    default:
                        return "None";
                }
            }
        };

        /**
         * Security Provisioning Status
         */
        class SecProvisioningStatus
        {
        public:
            SecProvisioningStatus(string deviceUUID, ESResult result) :
                m_devUUID(deviceUUID), m_result(result)
            {
#ifdef __WITH_DTLS__
                m_selectedOTMethod = OIC_JUST_WORKS;
                m_isMOTEnabled = false;
                m_isOwned = false;
                m_ownerID = {};
#endif
            }
#ifdef __WITH_DTLS__
            SecProvisioningStatus(std::shared_ptr<OCSecureResource> resource, ESResult result) :
                m_result(result)
            {
                m_isMOTEnabled = false;
                if(resource.get() != nullptr)
                {
                    m_devUUID = resource->getDeviceID();
                    m_isOwned = resource->getOwnedStatus();
#ifdef MULTIPLE_OWNER
                    m_isMOTEnabled = resource->isMOTEnabled();
#endif

                    if( OC_STACK_OK != resource->getOTMethod(&m_selectedOTMethod) )
                    {
                        m_selectedOTMethod = OIC_OXM_COUNT; // Out-of-range
                    }

                    if(resource->getOwnedStatus())
                    {
                        char uuidString[UUID_STRING_SIZE] = {};
                        if(OCConvertUuidToString(resource->getDevPtr()->doxm->owner.id, uuidString))
                        {
                            m_ownerID = uuidString;
                        }
                        else
                        {
                            m_ownerID = {};
                        }
                    }
                }
            }

            OicSecOxm_t getSelectedOTMethod() const
            {
                return m_selectedOTMethod;
            }

            bool isMOTEnabled() const
            {
                return m_isMOTEnabled;
            }

            bool isOwnedDevice() const
            {
                return m_isOwned;
            }

            const std::string getOwnerID()
            {
                return m_ownerID;
            }
#endif
            const std::string getDeviceUUID()
            {
                return m_devUUID;
            }
            /**
             * Get a result for about security provisioning is success or not.
             *
             * @return ::ES_OK\n
             *         ::ES_SEC_OPERATION_IS_NOT_SUPPORTED\n
             *         ::ES_SECURE_RESOURCE_DISCOVERY_FAILURE\n
             *         ::ES_OWNERSHIP_TRANSFER_FAILURE\n
             *         ::ES_ERROR\n
             */
            ESResult getESResult()
            {
                return m_result;
            }
        private:
            string m_devUUID;
            ESResult m_result;
#ifdef __WITH_DTLS__
            OicSecOxm_t m_selectedOTMethod;
            bool m_isMOTEnabled;
            bool m_isOwned;
            std::string m_ownerID;
#endif
        };

        /**
         * @brief This provide a set of getter APIs from received response for getConfiguration().
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
             * OCRepresentation object corresponding to WiFiConf, DevConf, and CoapCloudConf
             * resources' representations.
             */

            EnrolleeConf(const OCRepresentation& rep) :
                m_EasySetupRep(rep), m_OCFServerVersion(0)
            {
            }

            EnrolleeConf(const OCRepresentation& rep, const int OCFServerVersion) :
                m_EasySetupRep(rep), m_OCFServerVersion(OCFServerVersion)
            {
            }

            EnrolleeConf(const EnrolleeConf& enrolleeConf) :
                m_EasySetupRep(enrolleeConf.getEasySetupRep()), m_OCFServerVersion(enrolleeConf.getOCFSpecVersion())
            {
            }

            EnrolleeConf(const EnrolleeConf&& enrolleeConf) :
                m_EasySetupRep(std::move(enrolleeConf.getEasySetupRep())),
                    m_OCFServerVersion(std::move(enrolleeConf.getOCFSpecVersion()))
            {
            }

            /**
             * Get a device name of Enrollee. It is Device's human-friendly name like device model
             * name.
             *
             * @return a device name of Enrollee
             */
            std::string getDeviceName() const
            {
                std::vector<OCRepresentation> children = m_EasySetupRep.getChildren();
                for(auto child = children.begin(); child != children.end(); ++child)
                {
                    if(child->getUri().find(OC_RSRVD_ES_URI_DEVCONF) != std::string::npos)
                    {
                        OCRepresentation rep;
                        if(child->hasAttribute(OC_RSRVD_REPRESENTATION))
                        {
                            rep = child->getValue<OCRepresentation>(OC_RSRVD_REPRESENTATION);
                        }
                        else
                        {
                            return std::string("");
                        }

                        if(rep.hasAttribute(OC_RSRVD_ES_DEVNAME))
                        {
                            return rep.getValue<std::string>(OC_RSRVD_ES_DEVNAME);
                        }
                    }
                }
                return std::string("");
            }

            /**
             * Get a set of WiFi supported modes of Enrollee
             *
             * @return a set of WiFi supported modes of Enrollee
             *
             * @see WIFI_MODE
             */
            vector<WIFI_MODE> getWiFiModes() const
            {
                vector<WIFI_MODE> modes;
                modes.clear();

                std::vector<OCRepresentation> children = m_EasySetupRep.getChildren();
                for(auto child = children.begin(); child != children.end(); ++child)
                {
                    if(child->getUri().find(OC_RSRVD_ES_URI_WIFICONF) != std::string::npos)
                    {
                        OCRepresentation rep;
                        if(child->hasAttribute(OC_RSRVD_REPRESENTATION))
                        {
                            rep = child->getValue<OCRepresentation>(OC_RSRVD_REPRESENTATION);
                        }
                        else
                        {
                            return modes;
                        }

                        if(rep.hasAttribute(OC_RSRVD_ES_SUPPORTEDWIFIMODE))
                        {
                            // Response from OIC Server does not contain spec version in header, so assumed as 0.
                            if(0 == m_OCFServerVersion)
                            {
                                OIC_LOG(DEBUG, ES_RICH_COMMON_TAG, "getWiFiModes() with OIC Spec");

                                // Considering Representation as per OIC Core Spec.
                                for(auto it : rep.getValue
                                        <std::vector<int>>(OC_RSRVD_ES_SUPPORTEDWIFIMODE))
                                {
                                    modes.push_back(static_cast<WIFI_MODE>(it));
                                }
                            }
                            else // m_OCFServerVersion > 0 for OCF Spec (2048 for OCF 1.0 Spec)
                            {
                                OIC_LOG(DEBUG, ES_RICH_COMMON_TAG, "getWiFiModes() with OCF Spec");

                                // Considering Representation as per OCF 1.0 / 1.x Core Spec.
                                for(auto it : rep.getValue
                                        <std::vector<std::string>>(OC_RSRVD_ES_SUPPORTEDWIFIMODE))
                                {
                                    modes.push_back(getWiFiModeAsEnum(it));
                                }
                            }
                        }
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
            WIFI_FREQ getWiFiFreq() const
            {
                std::vector<OCRepresentation> children = m_EasySetupRep.getChildren();
                for(auto child = children.begin(); child != children.end(); ++child)
                {
                    if(child->getUri().find(OC_RSRVD_ES_URI_WIFICONF) != std::string::npos)
                    {
                        OCRepresentation rep;
                        if(child->hasAttribute(OC_RSRVD_REPRESENTATION))
                        {
                            rep = child->getValue<OCRepresentation>(OC_RSRVD_REPRESENTATION);
                        }
                        else
                        {
                            return WIFI_FREQ_NONE;
                        }

                        if(rep.hasAttribute(OC_RSRVD_ES_SUPPORTEDWIFIFREQ))
                        {
                            // Response from OIC Server does not contain spec version in header, so assumed as 0.
                            if(0 == m_OCFServerVersion)
                            {
                                OIC_LOG(DEBUG, ES_RICH_COMMON_TAG, "getWiFiFreq() with OIC Spec");

                                // Considering Representation as per OIC Core Spec.
                                return static_cast<WIFI_FREQ>(
                                            rep.getValue<int>(OC_RSRVD_ES_SUPPORTEDWIFIFREQ));
                            }
                            else // m_OCFServerVersion > 0 for OCF Spec (2048 for OCF 1.0 Spec)
                            {
                                OIC_LOG(DEBUG, ES_RICH_COMMON_TAG, "getWiFiFreq() with OCF Spec");

                                // Considering Representation as per OCF 1.0 / 1.x Core Spec.
                                return getWiFiFreqAsEnum(rep.getValue
                                        <std::vector<std::string>>(OC_RSRVD_ES_SUPPORTEDWIFIFREQ));
                            }
                        }
                    }
                }
                return WIFI_FREQ_NONE;
            }

            /**
             * Get supported WiFi authentication types by the Enrollee.
             *
             * @return a set of supported WiFi authentication types of Enrollee.
             *
             * @see WIFI_FREQ
             */
            vector<std::string> getSupportedWiFiAuthTypes() const
            {
                vector<std::string> swatVector;

                std::vector<OCRepresentation> children = m_EasySetupRep.getChildren();
                for(auto child = children.begin(); child != children.end(); ++child)
                {
                    if(child->getUri().find(OC_RSRVD_ES_URI_WIFICONF) != std::string::npos)
                    {
                        OCRepresentation rep;
                        if(child->hasAttribute(OC_RSRVD_REPRESENTATION))
                        {
                            rep = child->getValue<OCRepresentation>(OC_RSRVD_REPRESENTATION);
                        }
                        else
                        {
                            break;
                        }

                        if(rep.hasAttribute(OC_RSRVD_ES_SUPPORTEDWIFIAUTHTYPE))
                        {
                            /* This property is include only from OCF 1.3 Spec Onwards */
                            swatVector = rep.getValue<std::vector<std::string>>(OC_RSRVD_ES_SUPPORTEDWIFIAUTHTYPE);
                            break;
                        }
                    }
                }
                return swatVector;
            }

            /**
             * Get supported WiFi encryption types by the Enrollee.
             *
             * @return a set of supported WiFi encryption types of Enrollee.
             *
             * @see WIFI_MODE
             */
            vector<std::string> getSupportedWiFiEncTypes() const
            {
                vector<std::string> swetVector;

                std::vector<OCRepresentation> children = m_EasySetupRep.getChildren();
                for(auto child = children.begin(); child != children.end(); ++child)
                {
                    if(child->getUri().find(OC_RSRVD_ES_URI_WIFICONF) != std::string::npos)
                    {
                        OCRepresentation rep;
                        if(child->hasAttribute(OC_RSRVD_REPRESENTATION))
                        {
                            rep = child->getValue<OCRepresentation>(OC_RSRVD_REPRESENTATION);
                        }

                        if(rep.hasAttribute(OC_RSRVD_ES_SUPPORTEDWIFIENCTYPE))
                        {
                            /* This property is include only from OCF 1.3 Spec Onwards */
                            swetVector = rep.getValue<std::vector<std::string>>(OC_RSRVD_ES_SUPPORTEDWIFIENCTYPE);
                            break;
                        }
                    }
                }
                return swetVector;
            }

            /**
             * Get a provisioning status property of Enrollee.
             *
             * @return a provisioning status property of Enrollee
             */
            ProvStatus getProvStatus() const
            {
                OCRepresentation rep;
                if(m_EasySetupRep.hasAttribute(OC_RSRVD_REPRESENTATION))
                {
                    rep = m_EasySetupRep.getValue<OCRepresentation>(OC_RSRVD_REPRESENTATION);
                }
                else
                {
                    return ES_STATE_INIT;
                }

                if(rep.hasAttribute(OC_RSRVD_ES_PROVSTATUS))
                {
                    return static_cast<ProvStatus>(
                                        rep.getValue<int>(OC_RSRVD_ES_PROVSTATUS));
                }
                return ES_STATE_INIT;
            }

            /**
             * Get a last error code property of Enrollee.
             *
             * @return a last error code property of Enrollee.
             */
            ESErrorCode getLastErrCode() const
            {
                OCRepresentation rep;
                if(m_EasySetupRep.hasAttribute(OC_RSRVD_REPRESENTATION))
                {
                    rep = m_EasySetupRep.getValue<OCRepresentation>(OC_RSRVD_REPRESENTATION);
                }
                else
                {
                    return ES_ERRCODE_NO_ERROR;
                }

                if(rep.hasAttribute(OC_RSRVD_ES_LAST_ERRORCODE))
                {
                    return static_cast<ESErrorCode>(
                                        rep.getValue<int>(OC_RSRVD_ES_LAST_ERRORCODE));
                }
                return ES_ERRCODE_NO_ERROR;
            }

            /**
             * Get an accessibility to cloud server of an Enrollee
             *
             * @return an accessibility to cloud server of an Enrollee
             */
            bool isCloudAccessible() const
            {
                std::vector<OCRepresentation> children = m_EasySetupRep.getChildren();
                for(auto child = children.begin(); child != children.end(); ++child)
                {
                    OCRepresentation rep;
                    if(child->hasAttribute(OC_RSRVD_REPRESENTATION))
                    {
                        rep = child->getValue<OCRepresentation>(OC_RSRVD_REPRESENTATION);
                    }
                    else
                    {
                        return false;
                    }

                    if(rep.hasAttribute(OC_RSRVD_RESOURCE_TYPE))
                    {
                        for (auto rt : rep.getValue<std::vector<std::string>>(OC_RSRVD_RESOURCE_TYPE))
                        {
                            if(0 == rt.compare(OC_RSRVD_ES_RES_TYPE_COAPCLOUDCONF))
                            {
                                return true;
                            }
                        }
                    }
                }
                return false;
            }

            /**
             * Get OCRepresentation object
             *
             * @return OCRepresentation object
             */
            const OCRepresentation& getEasySetupRep() const
            {
                return m_EasySetupRep;
            }

            /**
             * Get OCF Specification Version
             *
             * @return version value
             */
            int getOCFSpecVersion() const
            {
                return m_OCFServerVersion;
            }

        protected:
            OCRepresentation m_EasySetupRep;
            int m_OCFServerVersion;

        private:

            WIFI_MODE getWiFiModeAsEnum(std::string wifiMode) const
            {
                if(0 == wifiMode.compare("A"))
                {
                    return WIFI_11A;
                }
                else if(0 == wifiMode.compare("B"))
                {
                    return WIFI_11B;
                }
                else if(0 == wifiMode.compare("G"))
                {
                    return WIFI_11G;
                }
                else if(0 == wifiMode.compare("N"))
                {
                    return WIFI_11N;
                }
                else /* if(0 == wifiMode.compare("AC")) */
                {
                    return WIFI_11AC;
                }
                //return WiFi_EOF;
            }

            WIFI_FREQ getWiFiFreqAsEnum(std::vector<std::string> wifiFreqList) const
            {
                bool b_2_4G = false;
                bool b_5G = false;

                for (std::string& freq : wifiFreqList)
                {
                    if(0 == freq.compare("2.4G"))
                    {
                        b_2_4G = true;
                    }
                    else if(0 == freq.compare("5G"))
                    {
                        b_5G = true;
                    }
                }

                if(b_2_4G && b_5G)
                {
                    return WIFI_BOTH;
                }

                if(b_2_4G)
                {
                    return WIFI_24G;
                }

                if(b_5G)
                {
                    return WIFI_5G;
                }

                return WIFI_FREQ_NONE;
            }
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
            /**
             * Constructor
             */
            GetEnrolleeStatus(ESResult result, const EnrolleeStatus& status) :
                m_result(result), m_enrolleeStatus(status)
            {
            }

            /**
             * Get a result of getting provisioning status and last error code of Enrollee
             *
             * @return ::ES_OK\n
             *         ::ES_COMMUNICATION_ERROR\n
             *         ::ES_ERROR\n
             * @see ESResult
             */
            ESResult getESResult()
            {
                return m_result;
            }

            /**
             * Get Enrollee's status and last error code properties
             *
             * @return Enrollee's status and last error code properties
             *
             * @see EnrolleeStatus
             */
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
            /**
             * Constructor
             */
            GetConfigurationStatus(ESResult result, const EnrolleeConf& conf) :
                    m_result(result), m_enrolleeConf(conf)
            {
            }

            /**
             * Get a result of getting preconfiguration of Enrollee
             *
             * @return ::ES_OK\n
             *         ::ES_COMMUNICATION_ERROR\n
             *         ::ES_ERROR\n
             *
             * @see ESResult
             */
            ESResult getESResult()
            {
                return m_result;
            }

            /**
             * Get Enrollee's pre-configuration properties
             *
             * @return Enrollee's pre-configuration properties
             *
             * @see EnrolleeConf
             */
            EnrolleeConf& getEnrolleeConf()
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
            /**
             * Constructor
             */
            DevicePropProvisioningStatus(ESResult result) :
                    m_result(result)
            {
            }

            /**
             * Get a result of Device property provisioning
             *
             * @return ::ES_OK\n
             *         ::ES_COMMUNICATION_ERROR\n
             *         ::ES_ERROR\n
             *
             * @see ESResult
             */
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
            /**
             * Constructor
             */
            CloudPropProvisioningStatus(ESResult result) :
                    m_result(result)
            {
            }

            /**
             * Get a result of Cloud property provisioning
             *
             * @return ::ES_OK\n
             *         ::ES_ENROLLEE_DISCOVERY_FAILURE\n
             *         ::ES_SECURE_RESOURCE_DISCOVERY_FAILURE\n
             *         ::ES_ACL_PROVISIONING_FAILURE\n
             *         ::ES_CERT_PROVISIONING_FAILURE\n
             *         ::ES_COMMUNICATION_ERROR\n
             *         ::ES_ERROR\n
             *
             * @see ESResult
             */
            ESResult getESResult()
            {
                return m_result;
            }

        private:
            ESResult m_result;
        };

        /**
         * Status object for connect API. This object is given to application
         * when a response for 'Connect' request from Enrollee is arrived.
         */
        class ConnectRequestStatus
        {
        public:
            /**
             * Constructor
             */
            ConnectRequestStatus(ESResult result) :
                    m_result(result)
            {
            }

            /**
             * Get a result of Connect request
             *
             * @return ::ES_OK\n
             *         ::ES_COMMUNICATION_ERROR\n
             *         ::ES_ERROR\n
             *
             * @see ESResult
             */
            ESResult getESResult()
            {
                return m_result;
            }

        private:
            ESResult m_result;
        };

        class ESOwnershipTransferData
        {
        public:
#ifdef __WITH_DTLS__
            ESOwnershipTransferData() :
                m_MOTMethod(OIC_OXM_COUNT), m_preconfiguredPin("")
            {
            }

            ESOwnershipTransferData(const ESOwnershipTransferData& data) :
                m_MOTMethod(data.getMOTMethod()),
                m_preconfiguredPin(data.getPreConfiguredPin())
            {
            }

            ESResult setMOTMethod(OicSecOxm_t method)
            {
#ifdef MULTIPLE_OWNER
                if(OIC_RANDOM_DEVICE_PIN != method)
                {
                    return ES_ERROR;
                }

                m_MOTMethod = method;
                return ES_OK;
#else
                (void) method;

                return ES_ERROR;
#endif
            }

            ESResult setMOTMethod(OicSecOxm_t method, const std::string& pin)
            {
#ifdef MULTIPLE_OWNER
                if(OIC_PRECONFIG_PIN != method || pin.empty())
                {
                    return ES_ERROR;
                }

                m_preconfiguredPin = pin;
                m_MOTMethod = method;
                return ES_OK;
#else
                (void) method;
                (void) pin;

                return ES_ERROR;
#endif
            }

            OicSecOxm_t getMOTMethod() const
            {
                return m_MOTMethod;
            }

            std::string getPreConfiguredPin() const
            {
                return m_preconfiguredPin;
            }

        private:
            OicSecOxm_t m_MOTMethod;
            std::string m_preconfiguredPin;
#endif
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
         * Callback function definition for providing 'Connect' request status
         */
        typedef function< void(shared_ptr< ConnectRequestStatus >) > ConnectRequestStatusCb;


        /**
         * Callback function definition for providing Enrollee security provisioning status
         */
        typedef function< void(shared_ptr<SecProvisioningStatus>) > SecurityProvStatusCb;

        /**
         * Callback function definition for providing Enrollee security provisioning status.
         * This callback is an overloaded version of SecurityProvStatusCb, which has
         * ESOwnershipTransferData as a return value.
         */
        typedef function< ESOwnershipTransferData(shared_ptr<SecProvisioningStatus>) >
                                                                    SecurityProvStatusCbWithOption;

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

#endif //ES_COMMON_RICH_H_
